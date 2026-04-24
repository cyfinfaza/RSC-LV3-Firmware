#pragma once

// Orion BMS2 communication via Aux CAN.
//
// Receives battery pack data from the Orion BMS2 over the isolated Aux CAN
// bus and exposes it for consumption by the LV3 CAN bindings and contactor
// control logic.

#include <stdint.h>

// Pack-level status
extern uint8_t pack_soc;         // raw BMS SOC, 0-200 (divide by 2 for %)
extern uint16_t bms_relay_state;
extern uint16_t pack_ccl;        // charge current limit (A)
extern uint16_t pack_dcl;        // discharge current limit (A)
extern int16_t pack_current;     // pack current (A)
extern uint16_t pack_dod;        // depth of discharge
extern uint16_t bms_supply_rail; // BMS supply voltage (raw)

// Per-cell voltages (* 0.1 mV) and pack voltage aggregate
extern uint32_t cell_voltages[24];
extern uint32_t reported_pack_voltage; // sum of all 24 cell voltages (* 0.1 mV)
extern int32_t reported_pack_current; // conversion of pack_current to int32_t for LV3 CAN binding
extern uint32_t reported_pack_soc;     // pack_soc / 2 (percentage)

// Thermistor temperatures (degrees C) and aggregates
extern int8_t thermistor_temp[8];
extern int32_t average_thermistor_temp;
extern int32_t max_thermistor_temp;

// Initialize Aux CAN for BMS communication (start, filter, RX interrupt)
void OrionBMS_Init(void);

// Update thermistor aggregate values; call once per main loop iteration
void OrionBMS_Loop(void);

// Send OBD2 clear-faults command to the BMS
void OrionBMS_SendClearFaults(void);
