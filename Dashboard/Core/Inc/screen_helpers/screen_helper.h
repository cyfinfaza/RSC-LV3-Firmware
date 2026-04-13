#pragma once

// Central screen helper dispatcher.
//
// ScreenHelper_Init sets up any initial widget state for all screens and must
// be called once after ui_init().
//
// ScreenHelper_Loop should be called every main loop tick. It checks which
// screen is currently active and calls only that screen's updater, so work
// is proportional to what is actually visible.

void ScreenHelper_Init(void);
void ScreenHelper_Loop(void);
