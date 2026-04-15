#include "lv3_can_bindings.h"
#include "orion_bms_interface.h"

// sw_hv_main: the dash HV enable switch, received from the primary CAN bus.
// Prefixed lv3c_ to avoid conflict with the sw_hv_main enum constant in LV3_CAN_Param.
uint32_t lv3c_sw_hv_main = 0;

// main_contactor_enabled: computed by main, written to both the contactor GPIO
// and broadcast on CAN so other nodes can observe contactor state
uint32_t main_contactor_enabled = 0;

const LV3_CAN_Binding lv3_can_bindings[] = {
    {&lv3c_sw_hv_main,              sw_hv_main,          LV3_CAN_BindMode_Read},
    {&main_contactor_enabled,       hv_main_active,      LV3_CAN_BindMode_Write},
    {(uint32_t*)&average_thermistor_temp, hv_pack_temp,       LV3_CAN_BindMode_Write},
    {(uint32_t*)&max_thermistor_temp,     hv_pack_max_temp,   LV3_CAN_BindMode_Write},
    {&reported_pack_soc,            hv_pack_soc,         LV3_CAN_BindMode_Write},
    {&reported_pack_voltage,        hv_pack_voltage,     LV3_CAN_BindMode_Write},
    {&cell_voltages[0],             hv_cell_voltage_1,   LV3_CAN_BindMode_Write},
    {&cell_voltages[1],             hv_cell_voltage_2,   LV3_CAN_BindMode_Write},
    {&cell_voltages[2],             hv_cell_voltage_3,   LV3_CAN_BindMode_Write},
    {&cell_voltages[3],             hv_cell_voltage_4,   LV3_CAN_BindMode_Write},
    {&cell_voltages[4],             hv_cell_voltage_5,   LV3_CAN_BindMode_Write},
    {&cell_voltages[5],             hv_cell_voltage_6,   LV3_CAN_BindMode_Write},
    {&cell_voltages[6],             hv_cell_voltage_7,   LV3_CAN_BindMode_Write},
    {&cell_voltages[7],             hv_cell_voltage_8,   LV3_CAN_BindMode_Write},
    {&cell_voltages[8],             hv_cell_voltage_9,   LV3_CAN_BindMode_Write},
    {&cell_voltages[9],             hv_cell_voltage_10,  LV3_CAN_BindMode_Write},
    {&cell_voltages[10],            hv_cell_voltage_11,  LV3_CAN_BindMode_Write},
    {&cell_voltages[11],            hv_cell_voltage_12,  LV3_CAN_BindMode_Write},
    {&cell_voltages[12],            hv_cell_voltage_13,  LV3_CAN_BindMode_Write},
    {&cell_voltages[13],            hv_cell_voltage_14,  LV3_CAN_BindMode_Write},
    {&cell_voltages[14],            hv_cell_voltage_15,  LV3_CAN_BindMode_Write},
    {&cell_voltages[15],            hv_cell_voltage_16,  LV3_CAN_BindMode_Write},
    {&cell_voltages[16],            hv_cell_voltage_17,  LV3_CAN_BindMode_Write},
    {&cell_voltages[17],            hv_cell_voltage_18,  LV3_CAN_BindMode_Write},
    {&cell_voltages[18],            hv_cell_voltage_19,  LV3_CAN_BindMode_Write},
    {&cell_voltages[19],            hv_cell_voltage_20,  LV3_CAN_BindMode_Write},
    {&cell_voltages[20],            hv_cell_voltage_21,  LV3_CAN_BindMode_Write},
    {&cell_voltages[21],            hv_cell_voltage_22,  LV3_CAN_BindMode_Write},
    {&cell_voltages[22],            hv_cell_voltage_23,  LV3_CAN_BindMode_Write},
    {&cell_voltages[23],            hv_cell_voltage_24,  LV3_CAN_BindMode_Write},
};

const unsigned int lv3_can_bindings_count = sizeof(lv3_can_bindings) / sizeof(LV3_CAN_Binding);
