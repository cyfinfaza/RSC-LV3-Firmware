#pragma once

#include <stdint.h>

#define LV3_CAN_HEARTBEAT_INTERVAL 1000
#define LV3_CAN_HEARTBEAT_PULSE 250

#define LV3_CAN_LOWEST_PRIORITY 0xF

typedef enum { LV3_CAN_OK, LV3_CAN_FAIL } LV3_CAN_Result;

typedef enum {
  LV3_CAN_MessageType_Parameter = 0,
  LV3_CAN_MessageType_Trigger = 1,
  LV3_CAN_MessageType_Diagnostics = 2,
  LV3_CAN_MessageType_Console = 3,
  LV3_CAN_MessageType_Heartbeat = 4
} LV3_CAN_MessageType;

typedef union {
  uint32_t raw;
  struct {
    uint32_t sender : 8;
    uint32_t parameter : 13;
    uint32_t message_type : 4;
    uint32_t priority : 4;
    uint32_t _reserved : 4;
  } elements;
} LV3_CAN_CompositeID;

typedef struct {
  LV3_CAN_CompositeID can_id;
  char *name_str;
  unsigned int refresh_interval; // ms
  unsigned int ttl;              // ms
  unsigned int has_safe_state;   // boolean
  uint32_t safe_state;
} LV3_CAN_ParamMetadataStruct;

typedef struct {
  uint32_t last_sent_value;
  unsigned int last_sent_time;     // ms
  unsigned int last_received_time; // ms
  char is_stale;
  char flag_for_send;
} LV3_CAN_ParamStateStruct;

#define X(can_id, name, refresh_interval, ttl, has_safe_state, safe_state) name,
typedef enum {
#include "../Define/Parameters.def"
  LV3_CAN_ParamCount
} LV3_CAN_Param;
#undef X

typedef struct {
  uint32_t uptime;         // ms
  int32_t v_sense_12;      // mV
  int32_t v_sense_5;       // mV
  int32_t mcu_temp;        // deg C *1000
  uint32_t main_loop_freq; // Hz
} LV3_CAN_SystemHealthData;

typedef enum { LV3_CAN_BindMode_Read, LV3_CAN_BindMode_Write } LV3_CAN_BindMode;

typedef enum {
  LV3_CAN_BusMode_Normal,
  LV3_CAN_BusMode_Spectator
} LV3_CAN_BusMode;

typedef struct {
  uint32_t *user_var;
  uint32_t param_id;
  LV3_CAN_BindMode bind_mode;
} LV3_CAN_Binding;

typedef enum {
  LV3_CAN_Status_Good,
  LV3_CAN_Status_MissingData,
  LV3_CAN_Status_Error
} LV3_CAN_CommStatus;

LV3_CAN_Result LV3_CAN_Init(uint8_t this_module_id, LV3_CAN_BusMode bus_mode,
                            const LV3_CAN_Binding *bindings,
                            unsigned int num_bindings);

LV3_CAN_Result LV3_CAN_Loop();

LV3_CAN_Result LV3_CAN_PushNewMessage(uint32_t id, uint8_t *data,
                                      uint8_t length);

LV3_CAN_Result LV3_CAN_ForceSend(LV3_CAN_Param param_id);