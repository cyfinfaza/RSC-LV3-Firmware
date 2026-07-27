#pragma once

// Screen helper for the Driver Dashboard screen: turn signal / hazard control.
//
// The left/right/hazards buttons select which signal(s) should blink;
// DriverDashboard_Update owns the actual 500ms blink timing and writes the
// signal_left / signal_right CAN parameters (which the rest of the vehicle
// reads to drive the physical signal lights).

// Wires up the button click handlers. Must be called after ui_init().
void DriverDashboard_Init(void);

// Advances the blink timer, updates signal_left/signal_right, and syncs the
// button backgrounds (transparent/amber) to the current light state. Call
// every main loop tick while the driver dashboard screen is active.
void DriverDashboard_Update(void);
