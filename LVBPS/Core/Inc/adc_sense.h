#pragma once

#include <stdint.h>

// Offset calibration: power the board off USB and see what each sensor reads
#define I_SENSE_BAT_OFFSET   0.15f
#define I_SENSE_LOAD_OFFSET  (-0.1f)

typedef struct {
    float v_sense_5;
    float v_sense_12_bat;
    float v_sense_12_dcdc;
    float v_sense_12_load;
    float i_sense_bat;
    float i_sense_load;
    float thermistor_mv;  // raw pin voltage in mV (needs NTC curve for °C)
} ADC_Sense_t;

extern uint16_t adc_buf[9];
extern ADC_Sense_t adc_sense;

void ADC_Sense_Update(void);
