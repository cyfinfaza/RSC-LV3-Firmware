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
