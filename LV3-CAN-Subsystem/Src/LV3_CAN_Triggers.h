#pragma once

/**
 * @brief Unique IDs for all triggers (message type 1) known to the LV3 CAN system.
 * These are placed in the 'parameter' field of the CAN composite ID.
 */
typedef enum {
    LV3_CAN_Trigger_toggle_hv_bps_local_enable = 0x001,
    LV3_CAN_Trigger_send_orion_obd2_clear = 0x002,
} LV3_CAN_Trigger;
