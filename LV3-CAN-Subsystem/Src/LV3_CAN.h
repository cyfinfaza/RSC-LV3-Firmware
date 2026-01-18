#pragma once

#include <stdint.h>

#define LV3_CAN_HEARTBEAT_INTERVAL 1000
#define LV3_CAN_HEARTBEAT_PULSE 250

#define LV3_CAN_LOWEST_PRIORITY 0xF

/**
 * @brief Result codes for all LV3_CAN and LV3_CAN_Driver functions
 */
typedef enum { LV3_CAN_OK, LV3_CAN_FAIL } LV3_CAN_Result;

/**
 * @brief Message type codes for LV3 CAN messages
 */
typedef enum {
	/** Parameter Message: Indicates an update to a globally-synchronized LV3 CAN system parameter originating from a
	   single module */
	LV3_CAN_MessageType_Parameter = 0,

	/** Trigger: Indicates the occurrence of an event, optionally with data */
	LV3_CAN_MessageType_Trigger = 1,

	/** Diagnostics Message: Indicates an update to diagnostic (system health) parameters of a particular module */
	LV3_CAN_MessageType_Diagnostics = 2,

	/** Console Message: Chunk of ASCII text stream to be displayed on a debug console */
	LV3_CAN_MessageType_Console = 3,

	/** Heartbeat Message: Special message used for heartbeat blink pattern */
	LV3_CAN_MessageType_Heartbeat = 4
} LV3_CAN_MessageType;

/**
 * @brief Bitfield decomposition of LV3 29-bit CAN ID
 */
typedef union {
	/** The raw 29-bit CAN ID */
	uint32_t raw;

	struct {
		/** The module ID of the sender */
		uint32_t sender : 8;

		/** The ID of the parameter or event within the scope of the sender */
		uint32_t parameter : 13;

		/** The type of message */
		uint32_t message_type : 4;

		/** The priority of the message (0 = highest, 15 = lowest) */
		uint32_t priority : 4;

		/** Reserved bits, must be zero */
		uint32_t _reserved : 3;
	} elements;
} LV3_CAN_CompositeID;

/**
 * @brief Read-only metadata about a system parameter
 */
typedef struct {
	/** 29-bit CAN ID of the parameter, encoding additional metadata */
	LV3_CAN_CompositeID can_id;

	/** Human-readable name of the parameter */
	char* name_str;

	/** Refresh Interval: Minimum time between transmission of updates (ms) */
	unsigned int refresh_interval;  // ms

	/** Time To Live: Maximum time before a parameter is considered stale (ms) */
	unsigned int ttl;  // ms

	/** Whether the parameter has a safe state */
	unsigned int has_safe_state;  // boolean

	/** Safe state value to be used for the parameter, if has_safe_state is 1 */
	uint32_t safe_state;
} LV3_CAN_ParamMetadataStruct;

/**
 * @brief State variables managed by LV3 CAN for a system parameter
 */
typedef struct {
	/** Last value sent for this parameter */
	uint32_t last_sent_value;

	/** Local time of last successful transmission of this parameter (Write Mode only) */
	unsigned int last_sent_time;  // ms

	/** Local time of last reception of this parameter (Read Mode only) */
	unsigned int last_received_time;  // ms

	/** Whether the last received time is greater than ttl ago (Read Mode only) */
	char is_stale;

	/** Whether the parameter is flagged for sending (Write Mode only) */
	char flag_for_send;
} LV3_CAN_ParamStateStruct;

/**
 * @brief Unique IDs for all parameters (message type 0) known to the LV3 CAN system
 */
#define X(can_id, name, refresh_interval, ttl, has_safe_state, safe_state) name,
typedef enum {
#include "../Define/Parameters.def"
	LV3_CAN_ParamCount
} LV3_CAN_Param;
#undef X

/**
 * @brief System health data provided to LV3 CAN by the driver
 */
typedef struct {
	/** System uptime in milliseconds */
	uint32_t uptime;  // ms

	/** 12V rail voltage in mV */
	int32_t v_sense_12;  // mV

	/** 5V rail voltage in mV */
	int32_t v_sense_5;  // mV

	/** MCU temperature in degrees Celsius * 1000 */
	int32_t mcu_temp;  // deg C *1000

	/** Main loop frequency in Hz. Will be updated automatically by the library if 0 is provided. */
	uint32_t main_loop_freq;  // Hz
} LV3_CAN_SystemHealthData;

/**
 * @brief Binding mode for a parameter binding
 */
typedef enum {
	/**
	 * Read Mode: The module will listen for updates to this parameter from the CAN bus and update the locally bound
	 * variable. The parameter will be monitored for staleness.
	 */
	LV3_CAN_BindMode_Read,

	/**
	 * Write Mode: The module will watch for changes to the locally bound variable and send updates the CAN bus. A CAN
	 * message will be queued if:
	 * - The parameter was last sent more than TTL/2 ago
	 * - The locally bound variable has changed since the last send, and the last send was more than the refresh
	 * interval ago
	 */
	LV3_CAN_BindMode_Write
} LV3_CAN_BindMode;

/**
 * @brief LV3 CAN initialization mode for the module
 */
typedef enum {
	/** Normal Mode: Module will read and write to the bus, participate in heartbeat, and send health data. */
	LV3_CAN_BusMode_Normal,

	/** Spectator Mode: Module will only read from the bus, heartbeat and health data will be disabled. */
	LV3_CAN_BusMode_Spectator
} LV3_CAN_BusMode;

/**
 * @brief LV3 CAN local variable binding configuration, to be passed to Init function
 */
typedef struct {
	/** Pointer to the local variable that shall be watched for changes (Write Mode) or updated to synchronize with the
	 * origin (Read Mode) */
	uint32_t* user_var;

	/** The ID from LV3_CAN_Param enum of the LV3 parameter to bind this variable to */
	uint32_t param_id;

	/** The binding mode for this parameter */
	LV3_CAN_BindMode bind_mode;
} LV3_CAN_Binding;

/**
 * @brief LV3 CAN system communication status
 */
typedef enum {
	/** Status Good: No CAN system errors, all Read Mode parameters are fresh. */
	LV3_CAN_Status_Good,

	/** Status MissingData: No CAN system errors, one or more Read Mode parameters are stale. */
	LV3_CAN_Status_MissingData,

	/** Status Error: CAN system errors are present. */
	LV3_CAN_Status_Error
} LV3_CAN_CommStatus;

/**
 * @brief Initialize the LV3 CAN system for this module.
 *
 * @pre The CAN driver should be initialized before calling this function.
 *
 * @param this_module_id The unique module ID for this module on the CAN bus
 * @param bus_mode The initialization mode for the module
 * @param bindings An array of LV3_CAN_Binding structures defining local variable bindings
 * @param num_bindings The number of bindings in the bindings array
 */
LV3_CAN_Result LV3_CAN_Init(uint8_t this_module_id,
                            LV3_CAN_BusMode bus_mode,
                            const LV3_CAN_Binding* bindings,
                            unsigned int num_bindings);

/**
 * @brief Main LV3 CAN system loop function, to be called regularly by the main program loop
 */
LV3_CAN_Result LV3_CAN_Loop();

/**
 * @brief Push a newly received CAN message into the LV3 CAN system, to be called by the driver.
 *
 * @param id The raw 29-bit CAN ID of the received message
 * @param data Pointer to the data payload of the received message
 * @param length Length of the data payload in bytes
 */
LV3_CAN_Result LV3_CAN_PushNewMessage(uint32_t id, uint8_t* data, uint8_t length);

/**
 * @brief (NOT IMPLEMENTED) Force sending of a parameter message on the CAN bus
 *
 * @param param_id The LV3_CAN_Param ID of the parameter to send
 */
LV3_CAN_Result LV3_CAN_ForceSend(LV3_CAN_Param param_id);