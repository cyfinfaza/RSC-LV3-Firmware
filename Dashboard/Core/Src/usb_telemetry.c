#include "usb_telemetry.h"
#include "can_interface.h"
#include "usbd_cdc_if.h"
#include "usb_device.h"
#include <stdio.h>

extern USBD_HandleTypeDef hUsbDeviceFS;

// Parameter name strings and pointers to the mirror variables, in X macro order.
static const char *const param_names[] = {
#define X(can_id, name, refresh_interval, ttl, has_safe_state, safe_state) #name,
  LV3_CAN_Parameters_XMacro
#undef X
};

static uint32_t *const param_values[] = {
#define X(can_id, name, refresh_interval, ttl, has_safe_state, safe_state) &lv3c_param_##name,
  LV3_CAN_Parameters_XMacro
#undef X
};

// Widest line is a ~30 char name, a space, 10 digits, '\n' and NUL.
#define LINE_BUF_LEN 64

static int next_param = 0;

void USB_Telemetry_Init(void) {
    next_param = 0;
}

void USB_Telemetry_Loop(void) {
    // Only transmit once the host has configured the device; otherwise
    // CDC_Transmit_FS would be writing into an unopened endpoint.
    if (hUsbDeviceFS.dev_state != USBD_STATE_CONFIGURED) {
        return;
    }

    char buf[LINE_BUF_LEN];
    int len = snprintf(buf, sizeof(buf), "%s %lu\n", param_names[next_param],
                       (unsigned long)*param_values[next_param]);

    // A busy endpoint means the previous packet hasn't drained yet — leave
    // next_param alone and retry the same parameter on the next iteration.
    if (CDC_Transmit_FS((uint8_t *)buf, (uint16_t)len) != USBD_OK) {
        return;
    }

    next_param++;
    if (next_param >= LV3_CAN_ParamCount) {
        next_param = 0;
    }
}
