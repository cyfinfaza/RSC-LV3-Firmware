#include "adc_sense.h"

// 12-bit ADC, 64x oversampling, right-shift 2: effective full scale = 4095 * 64 / 4
#define ADC_FULL_SCALE  (4095.0f * 64.0f / 4.0f)
#define ADC_VREF        3.3f
#define DIV_12V         ((100.0f + 5.1f) / 5.1f)  // 100k / 5.1k voltage divider
#define I_SENSE_SCALE   (0.002f * 50.0f)           // 2mΩ shunt, 50x INA181

uint16_t adc_buf[9];
ADC_Sense_t adc_sense = {0};

void ADC_Sense_Update(void) {
    float v;

    v = (adc_buf[0] / ADC_FULL_SCALE) * ADC_VREF;
    adc_sense.v_sense_5 = v * 2.0f;

    v = (adc_buf[1] / ADC_FULL_SCALE) * ADC_VREF;
    adc_sense.v_sense_12_bat = v * DIV_12V;

    v = (adc_buf[2] / ADC_FULL_SCALE) * ADC_VREF;
    adc_sense.v_sense_12_dcdc = v * DIV_12V;

    v = (adc_buf[3] / ADC_FULL_SCALE) * ADC_VREF;
    adc_sense.v_sense_12_load = v * DIV_12V;

    v = (adc_buf[4] / ADC_FULL_SCALE) * ADC_VREF;
    adc_sense.i_sense_bat = (v - ADC_VREF / 2.0f) / I_SENSE_SCALE - I_SENSE_BAT_OFFSET;

    v = (adc_buf[5] / ADC_FULL_SCALE) * ADC_VREF;
    adc_sense.i_sense_load = (v - ADC_VREF / 2.0f) / I_SENSE_SCALE - I_SENSE_LOAD_OFFSET;

    v = (adc_buf[6] / ADC_FULL_SCALE) * ADC_VREF;
    adc_sense.thermistor_mv = v * 1000.0f;
}
