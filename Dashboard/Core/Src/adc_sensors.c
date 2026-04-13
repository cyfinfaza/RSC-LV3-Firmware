#include "adc_sensors.h"
#include "main.h"

// ADC handle defined by CubeMX in main.c
extern ADC_HandleTypeDef hadc1;

// DMA destination buffer placed in D2 SRAM (0x30000000 region).
// D2 SRAM is accessible by DMA1/DMA2 and is non-cacheable by default on
// STM32H7, so the CPU always reads the values DMA last wrote with no cache
// flush/invalidate needed.
// [0] = V_SENSE_5 (ADC1 rank 1), [1] = V_SENSE_12 (ADC1 rank 2)
volatile uint32_t adc_dma_buf[2] __attribute__((section(".ram_d2")));

void ADC_Sensors_Init(void) {
    // Offset calibration should run before starting conversions
    HAL_ADCEx_Calibration_Start(&hadc1, ADC_CALIB_OFFSET, ADC_SINGLE_ENDED);

    // Start DMA in circular mode — hadc1 must be configured with
    // ContinuousConvMode=ENABLE, ScanConvMode=ENABLE, NbrOfConversion=2,
    // ConversionDataManagement=ADC_CONVERSIONDATA_DMA_CIRCULAR,
    // and OversamplingMode=ENABLE (12-bit -> 16-bit) in CubeMX.
    HAL_ADC_Start_DMA(&hadc1, (uint32_t *)adc_dma_buf, 2);
}

// EEZ Studio variable bindings for v_sense_5
float get_var_v_sense_5(void) {
    SCB_InvalidateDCache_by_Addr((uint32_t*)adc_dma_buf, sizeof(adc_dma_buf));
    float voltage = (float)adc_dma_buf[0] * (3.3f / 65535.0f) * 2.0f;
    return voltage;
}

void set_var_v_sense_5(float value) { (void)value; }

// EEZ Studio variable bindings for v_sense_12
float get_var_v_sense_12(void) {
    SCB_InvalidateDCache_by_Addr((uint32_t*)adc_dma_buf, sizeof(adc_dma_buf));
    float voltage = (float)adc_dma_buf[1] * (3.3f / 65535.0f) * ((100.0f + 5.1f) / 5.1f);
    return voltage;
}

void set_var_v_sense_12(float value) { (void)value; }
