#include "orion_bms_interface.h"

#include "stm32g4xx_hal.h"

// Aux CAN is the isolated bus wired directly to the Orion BMS2.
// It runs at a different baud rate than the main LV3 vehicle CAN (FDCAN2)
// and carries proprietary Orion BMS2 broadcast messages, not LV3 protocol frames.
extern FDCAN_HandleTypeDef hfdcan3;

// Pack-level status
uint8_t pack_soc = 0;
uint16_t bms_relay_state = 0;
uint16_t pack_ccl = 0;
uint16_t pack_dcl = 0;
int16_t pack_current = 0;
uint16_t pack_dod = 0;
uint16_t bms_supply_rail = 0;

// Per-cell data from 0x36 broadcasts. Full struct kept alongside cell_voltages
// so IR and open-voltage are available for future diagnostics without re-parsing.
// Packed to match the Orion BMS2 wire layout for direct cast in the RX callback.
typedef struct __attribute__((packed)) {
    uint16_t voltage;             // * 0.1 mV
    uint16_t internal_resistance; // * 0.01 mOhm
    uint16_t open_voltage;        // * 0.1 mV
} cell_voltage_data_t;

static cell_voltage_data_t cell_voltage_data[24];

// cell_voltages mirrors cell_voltage_data[n].voltage but as uint32_t so it
// can be bound directly to LV3 CAN parameters (which are all uint32_t).
uint32_t cell_voltages[24] = {0};

// The Orion BMS2 doesn't broadcast a pack total directly; we compute it as the
// running sum of all 24 cell voltages on every 0x36 update.
uint32_t reported_pack_voltage = 0;

int32_t reported_pack_current = 0; // conversion of pack_current to int32_t for LV3 CAN binding

// Orion BMS2 reports SOC as 0-200 (0.5% steps); halve it for a 0-100% value.
uint32_t reported_pack_soc = 0;

int8_t thermistor_temp[8] = {0};
int32_t average_thermistor_temp = 0;
int32_t max_thermistor_temp = 0;

// Called by the LV3 CAN driver for every Aux CAN FDCAN RX FIFO0 event.
// Overrides the __weak default in LV3_CAN_Driver.c.
void LV3_CAN_AUX_RxFifo0Callback(FDCAN_HandleTypeDef *hfdcan,
                                   uint32_t RxFifo0ITs) {
    if (hfdcan->Instance != FDCAN3) return;
    if (!(RxFifo0ITs & FDCAN_IT_RX_FIFO0_NEW_MESSAGE)) return;

    FDCAN_RxHeaderTypeDef rx_header;
    uint8_t rx_data[8];
    HAL_FDCAN_GetRxMessage(hfdcan, FDCAN_RX_FIFO0, &rx_header, rx_data);

    switch (rx_header.Identifier) {
        // 0x100: pack summary — SOC and relay state (Orion BMS2 broadcast)
        case 0x100:
            pack_soc = rx_data[6];
            reported_pack_soc = pack_soc / 2;
            bms_relay_state = (rx_data[0] << 8) | rx_data[1];
            break;

        // 0x101: current limits and live current/DOD. Cast the 8-byte payload
        // directly onto a packed struct — field order matches the Orion BMS2 spec.
        case 0x101: {
            struct __attribute__((packed)) {
                uint16_t ccl;
                uint16_t dcl;
                int16_t current;
                uint16_t dod;
            } *s = (void *)rx_data;
            pack_ccl = s->ccl;
            pack_dcl = s->dcl;
            pack_current = s->current;
            pack_dod = s->dod;

            reported_pack_current = (int32_t)pack_current; // sign-extend for LV3 CAN binding

            break;
        }

        // 0x111: BMS supply rail voltage sits at bytes 2-3
        case 0x111:
            bms_supply_rail = (rx_data[2] << 8) | rx_data[3];
            break;

        // 0x36: per-cell voltage broadcast — one cell per frame, index in byte 0.
        // Recompute pack voltage on every update so it stays consistent with
        // whatever cells have been heard most recently.
        case 0x36: {
            uint8_t idx = rx_data[0];
            if (idx < 24) {
                cell_voltage_data[idx].voltage = (rx_data[1] << 8) | rx_data[2];
                cell_voltage_data[idx].internal_resistance = (rx_data[3] << 8) | rx_data[4];
                cell_voltage_data[idx].open_voltage = (rx_data[5] << 8) | rx_data[6];
                cell_voltages[idx] = cell_voltage_data[idx].voltage;
            }
            uint32_t sum = 0;
            for (int i = 0; i < 24; i++) sum += cell_voltages[i];
            reported_pack_voltage = sum;
            break;
        }

        // 0x76: thermistor broadcast — one sensor per frame, index in byte 0
        case 0x76: {
            uint8_t idx = rx_data[0];
            if (idx < 8) thermistor_temp[idx] = (int8_t)rx_data[1];
            break;
        }
    }
}

void OrionBMS_Init(void) {
    HAL_FDCAN_Start(&hfdcan3);
    HAL_FDCAN_ActivateNotification(&hfdcan3, FDCAN_IT_RX_FIFO0_NEW_MESSAGE, 0);

    // Pass-all filter: the Orion BMS2 broadcasts across several IDs in the
    // 0x36-0x7E3 range, so filtering by ID would need a list. Accepting all is
    // simpler and safe because Aux CAN is a point-to-point link to the BMS only.
    FDCAN_FilterTypeDef filter = {
        .IdType = FDCAN_STANDARD_ID,
        .FilterIndex = 0,
        .FilterType = FDCAN_FILTER_MASK,
        .FilterConfig = FDCAN_FILTER_TO_RXFIFO0,
        .FilterID1 = 0x000,
        .FilterID2 = 0x000,
    };
    HAL_FDCAN_ConfigFilter(&hfdcan3, &filter);
}

void OrionBMS_Loop(void) {
    int32_t sum = 0;
    int32_t max = -128;
    for (int i = 0; i < 8; i++) {
        sum += thermistor_temp[i];
        if (thermistor_temp[i] > max) max = thermistor_temp[i];
    }
    average_thermistor_temp = sum / 8;
    max_thermistor_temp = max;
}

// OBD2 service 0x04 (clear emission-related DTCs) is the command the Orion BMS2
// uses to reset its fault flags. ID 0x7E3 is the Orion BMS2 diagnostic request ID.
void OrionBMS_SendClearFaults(void) {
    FDCAN_TxHeaderTypeDef tx_header = {
        .Identifier = 0x7E3,
        .IdType = FDCAN_STANDARD_ID,
        .TxFrameType = FDCAN_DATA_FRAME,
        .DataLength = FDCAN_DLC_BYTES_8,
        .ErrorStateIndicator = FDCAN_ESI_ACTIVE,
        .BitRateSwitch = FDCAN_BRS_OFF,
        .FDFormat = FDCAN_CLASSIC_CAN,
        .TxEventFifoControl = FDCAN_NO_TX_EVENTS,
        .MessageMarker = 0,
    };
    uint8_t tx_data[8] = {0x01, 0x04, 0, 0, 0, 0, 0, 0};
    HAL_FDCAN_AddMessageToTxFifoQ(&hfdcan3, &tx_header, tx_data);
}
