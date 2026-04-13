#pragma once

// Voltage rail sensing via ADC1 with DMA.
//
// ADC1 is configured in scan mode with 2 channels:
//   Rank 1: V_SENSE_5  (PC4, ADC1_INP4) — 5V rail sense
//   Rank 2: V_SENSE_12 (PB1, ADC1_INP5) — 12V rail sense
//
// DMA runs in circular mode so adc_dma_buf is continuously updated without
// CPU intervention. Oversampling is configured in CubeMX as 12-bit -> 16-bit
// (4x oversample, right-shift by 2).
//
// The EEZ getter functions apply a scaling formula to produce voltages in volts.

#include <stdint.h>

// Start ADC calibration and kick off the DMA circular transfer.
// Call once after MX_ADC1_Init.
void ADC_Sensors_Init(void);

// Raw 16-bit oversampled ADC counts for each channel, as written by DMA.
// Index 0 = V_SENSE_5, index 1 = V_SENSE_12.
extern volatile uint32_t adc_dma_buf[2];

// EEZ Studio variable bindings — return the converted voltage in volts.
// Setters are no-ops; these are read-only sensor values.
float get_var_v_sense_5(void);
void set_var_v_sense_5(float value);
float get_var_v_sense_12(void);
void set_var_v_sense_12(float value);
