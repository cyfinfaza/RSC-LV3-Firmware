#pragma once

// MPPT communication via Aux CAN.

#include <stdint.h>

typedef struct {
  uint32_t arrayVoltage;    // * 10 mV  (* 0.01  = V)
  uint32_t arrayCurrent;    // *  1 mA  (* 0.001 = A)
  uint32_t batteryVoltage;  // * 10 mV  (* 0.01  = V)
  uint32_t batteryTemp;     // * 10 mC  (* 0.01  = C)
} mppt_data_t;

extern mppt_data_t mppt_data[4];

// // Aux CAN frames received from the MPPT, in messages/second. Sampled over a
// // 250 ms window in MPPT_Loop; 0 means the BMS link is dead.
// extern uint32_t mppt_message_rate;

void MPPT_Init(void);
