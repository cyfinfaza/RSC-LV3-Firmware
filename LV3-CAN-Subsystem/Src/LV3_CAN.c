#include "LV3_CAN.h"

#include "LV3_CAN_Driver.h"

static LV3_CAN_Binding *bindings;
static unsigned int num_bindings = 0;
static uint8_t this_module_id = 0;

static LV3_CAN_ParamMetadataStruct param_metadata[LV3_CAN_ParamCount] = {
#define X(_can_id, _name, _refresh_interval, _ttl, _has_safe_state,            \
          _safe_state)                                                         \
  {.can_id = {.raw = _can_id},                                                 \
   .name_str = #_name,                                                         \
   .refresh_interval = _refresh_interval,                                      \
   .ttl = _ttl,                                                                \
   .has_safe_state = _has_safe_state,                                          \
   .safe_state = _safe_state},
#include "../Define/Parameters.def"
#undef X
};

static LV3_CAN_ParamStateStruct param_state[LV3_CAN_ParamCount] = {0};

static char stale_data = 0;
static char tx_error = 0;
static char bus_conflict = 0;
static LV3_CAN_CommStatus system_status = LV3_CAN_Status_Good;

enum LED_Color {
  LED_Color_Off = 0,
  LED_Color_Red = 0x000000FF,
  LED_Color_Yellow = 0x0000FFFF,
  LED_Color_Green = 0x0000FF00,
  LED_Color_Blue = 0x00FF0000
};

static unsigned int heartbeat_timestamp = 0;
static unsigned int heartbeat_counter = 0;
static char heartbeat_pulse_in_progress = 0;
static char heartbeat_reset_occurred = 0;

LV3_CAN_Result LV3_CAN_Init(uint8_t _this_module_id, LV3_CAN_BusMode _bus_mode,
                            const LV3_CAN_Binding *_bindings,
                            unsigned int _num_bindings) {
  LV3_CAN_Driver_Init();
  LV3_CAN_Driver_SetLED(LED_Color_Red);

  bindings = (LV3_CAN_Binding *)_bindings;
  num_bindings = _num_bindings;
  this_module_id = _this_module_id;

  const uint8_t data[1] = {1};

  LV3_CAN_Driver_SendMessage(0x123, data, 1);

  return LV3_CAN_OK;
}

LV3_CAN_Result LV3_CAN_Loop() {
  // iterate through all bindings
  stale_data = 0;
  tx_error = 0;
  for (unsigned int i = 0; i < num_bindings; i++) {
    LV3_CAN_Binding *binding = &bindings[i];
    LV3_CAN_ParamMetadataStruct *metadata = &param_metadata[binding->param_id];
    LV3_CAN_ParamStateStruct *state = &param_state[binding->param_id];
    if (binding->bind_mode == LV3_CAN_BindMode_Write) {
      if ((LV3_CAN_Driver_GetTick() - state->last_sent_time) * 2 >
              metadata->ttl ||
          (state->last_sent_value != *binding->user_var &&
           state->last_sent_time + metadata->refresh_interval <
               LV3_CAN_Driver_GetTick())) {
        state->flag_for_send = 1;
        // for now, just send no matter what
        LV3_CAN_Result res = LV3_CAN_Driver_SendMessage(
            metadata->can_id.raw, (uint8_t *)binding->user_var,
            sizeof(uint32_t));
        if (res == LV3_CAN_OK) {
          state->last_sent_value = *binding->user_var;
          state->last_sent_time = LV3_CAN_Driver_GetTick();
          state->flag_for_send = 0;
        } else {
          tx_error = 1;
        }
      }
      continue;
    }
    if (binding->bind_mode == LV3_CAN_BindMode_Read) {
      if (state->last_received_time + metadata->ttl <
          LV3_CAN_Driver_GetTick()) {
        stale_data = 1;
        state->is_stale = 1;
        if (metadata->has_safe_state) {
          *(uint32_t *)binding->user_var = metadata->safe_state;
        }
      } else {
        state->is_stale = 0;
      }
      continue;
    }
  }
  if (heartbeat_counter >= 2 && !heartbeat_pulse_in_progress) {
    heartbeat_timestamp = LV3_CAN_Driver_GetTick();
    heartbeat_reset_occurred = 0;
    heartbeat_pulse_in_progress = 1;
  }
  if (!heartbeat_reset_occurred &&
      (LV3_CAN_Driver_GetTick() - heartbeat_timestamp >
       (LV3_CAN_HEARTBEAT_PULSE + this_module_id))) {
    heartbeat_counter = 0;
    heartbeat_pulse_in_progress = 0;
    heartbeat_reset_occurred = 1;
  }
  if (LV3_CAN_Driver_GetTick() - heartbeat_timestamp >
      (LV3_CAN_HEARTBEAT_INTERVAL + this_module_id)) {
    // send heartbeat
    LV3_CAN_CompositeID heartbeat_id = {
        .elements = {.sender = this_module_id,
                     .parameter = 0,
                     .message_type = LV3_CAN_MessageType_Heartbeat,
                     .priority = LV3_CAN_LOWEST_PRIORITY,
                     ._reserved = 0}};
    const uint8_t *dummy;
    LV3_CAN_Result res = LV3_CAN_Driver_SendMessage(heartbeat_id.raw, dummy, 0);
    if (res != LV3_CAN_OK) {
      tx_error = 1;
    } else {
      heartbeat_timestamp = LV3_CAN_Driver_GetTick();
      heartbeat_counter++;
      heartbeat_reset_occurred = 0;
    }
  }
  // update system status, set LED color
  if (heartbeat_pulse_in_progress) {
    LV3_CAN_Driver_SetLED(LED_Color_Off);
    return LV3_CAN_OK;
  } else {
    if (tx_error || bus_conflict) {
      system_status = LV3_CAN_Status_Error;
      LV3_CAN_Driver_SetLED(LED_Color_Red);
    } else if (stale_data) {
      system_status = LV3_CAN_Status_MissingData;
      LV3_CAN_Driver_SetLED(LED_Color_Yellow);
    } else {
      system_status = LV3_CAN_Status_Good;
      LV3_CAN_Driver_SetLED(LED_Color_Green);
    }
  }
  return LV3_CAN_OK;
}

LV3_CAN_Result LV3_CAN_PushNewMessage(uint32_t id, uint8_t *data,
                                      uint8_t length) {
  LV3_CAN_CompositeID can_id = {.raw = id};
  if (can_id.elements.sender == this_module_id) {
    // bus conflict
    bus_conflict = 1;
    return LV3_CAN_OK;
  }
  if (can_id.elements.message_type == LV3_CAN_MessageType_Parameter) {
    for (unsigned int i = 0; i < num_bindings; i++) {
      LV3_CAN_Binding *binding = &bindings[i];
      if (binding->bind_mode != LV3_CAN_BindMode_Read) {
        continue;
      }
      LV3_CAN_ParamMetadataStruct *metadata =
          &param_metadata[binding->param_id];
      LV3_CAN_ParamStateStruct *state = &param_state[binding->param_id];
      if (metadata->can_id.raw == id) {
        if (length != sizeof(uint32_t)) {
          // invalid length
          return LV3_CAN_FAIL;
        }
        // copy data to user variable
        *(uint32_t *)binding->user_var = *(uint32_t *)data;
        state->last_received_time = LV3_CAN_Driver_GetTick();
        state->is_stale = 0;
      }
    }
  }
  if (can_id.elements.message_type == LV3_CAN_MessageType_Heartbeat) {
    heartbeat_counter++;
  }
  return LV3_CAN_OK;
}

LV3_CAN_Result LV3_CAN_ForceSend(LV3_CAN_Param param_id) {
  (void)param_id;
  return LV3_CAN_OK;
}