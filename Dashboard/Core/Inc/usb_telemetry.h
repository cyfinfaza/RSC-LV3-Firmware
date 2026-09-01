#pragma once

// Streams the CAN parameter mirror values out over USB CDC as plain text.
//
// One parameter per loop iteration, formatted as "<name> <value>\n", cycling
// through the whole table and starting over. Nothing is sent unless the host
// has actually enumerated and configured the CDC interface.

void USB_Telemetry_Init(void);
void USB_Telemetry_Loop(void);
