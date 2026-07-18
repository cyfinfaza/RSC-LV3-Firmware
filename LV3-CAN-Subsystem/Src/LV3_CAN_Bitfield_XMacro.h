#pragma once

// Single source of truth for LV3 CAN parameters whose value is a bitfield.
//
// Each X() entry carries the field's C identifier and its human-readable label.
// The order of entries IS the bit order: the first entry is bit 0 of the
// parameter, the second is bit 1, and so on.
//
// Expand it wherever it's needed. The producing module builds a packed union
// from it (see hvbps_faults_t in HVBPS/Core/Inc/lv3_can_bindings.h) and sets
// bits by name; a consuming module can expand the same list to walk the bits in
// order and label them. Because both sides come from this one list, no bit
// numbers ever appear in the code.
//
// IMPORTANT: this list is APPEND-ONLY. Reordering or removing an entry silently
// changes what every existing bit means, and modules flashed at different times
// would disagree about the wire format. To retire a flag, keep its slot and mark
// it reserved.

// X(name, label)
#define LV3_CAN_Bitfield_hv_fault_XMacro \
\
X(communication_fault,      "CAN Comms Lost") \
X(orion_de_fault,           "Orion Discharge Disabled") \
X(e_stop_fault,             "E-Stop Pressed") \
X(cell_overvoltage,         "Cell Overvoltage") \
X(cell_undervoltage,        "Cell Undervoltage") \
X(pack_overcurrent,         "Pack Overcurrent") \
X(pack_undercurrent,        "Pack Undercurrent") \
X(pack_overtemp_discharge,  "Pack Overtemp (Discharge)") \
X(pack_undertemp_discharge, "Pack Undertemp (Discharge)") \
X(pack_overtemp_charge,     "Pack Overtemp (Charge)") \
X(pack_undertemp_charge,    "Pack Undertemp (Charge)")

// Battery relay will not turn on if any of these faults are set.
#define LV3_CAN_Bitfield_lv_bat_faults_XMacro \
\
X(startup_delay,         "Startup Delay")        /* System has not yet been on for SETPOINT_STARTUP_DELAY_MS */ \
X(startup_undervoltage,  "Startup Undervoltage") /* Battery voltage is below SETPOINT_STARTUP_THRESHOLD_V */ \
X(undervoltage,          "Undervoltage")         /* Battery voltage has gone below SETPOINT_UNDERVOLTAGE_V */ \
X(overvoltage,           "Overvoltage")          /* Battery voltage has gone above SETPOINT_OVERVOLTAGE_V */ \
X(overcurrent,           "Overcurrent")          /* Battery current has gone above SETPOINT_MAX_CURRENT_A */ \
X(undercurrent,          "Undercurrent")         /* Battery current has gone below SETPOINT_MIN_CURRENT_A */ \
X(precharge_timeout,     "Precharge Timeout")    /* Precharge has lasted longer than PRECHARGE_TIMEOUT_MS */ \
X(relay_fault,           "Relay Fault")          /* Voltage difference exceeded precharge threshold while relay was on */ \
X(overtemperature,       "Overtemperature")      /* NTC temperature has exceeded SETPOINT_MAX_TEMP_C */

// DCDC relay will not turn on if any of these faults are set.
#define LV3_CAN_Bitfield_lv_dcdc_faults_XMacro \
\
X(dcdc_startup_undervoltage, "Startup Undervoltage") /* DCDC voltage is below SETPOINT_STARTUP_THRESHOLD_V */ \
X(dcdc_startup_overvoltage,  "Startup Overvoltage")  /* DCDC voltage is above SETPOINT_MAX_CHARGE_V */ \
X(dcdc_undervoltage,         "Undervoltage")         /* DCDC voltage is below SETPOINT_UNDERVOLTAGE_V */ \
X(dcdc_overvoltage,          "Overvoltage")          /* DCDC voltage is above SETPOINT_MAX_CHARGE_V */ \
X(dcdc_overcurrent,          "Overcurrent")          /* DCDC is causing battery current to go below SETPOINT_MIN_CURRENT_A */ \
X(dcdc_sink,                 "Sink Current")         /* DCDC is sinking current */
