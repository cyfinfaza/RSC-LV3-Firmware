#include "adc_sensors.h"
#include "main.h"

// ADC handle defined by CubeMX in main.c
extern ADC_HandleTypeDef hadc1;

// DMA destination buffer placed in D2 SRAM (0x30000000 region), which DMA1/DMA2
// can reach. D2 SRAM IS cached (D-cache is on and no MPU region marks it
// otherwise), so readers must invalidate before reading what DMA wrote.
//
// Cache maintenance works on whole 32-byte lines, so this buffer must not share
// a line with anything else — invalidating it would discard the CPU's pending
// writes to whatever else lives in that line. LVGL's heap is also in .ram_d2,
// so the buffer is aligned to, and padded out to, a full cache line to keep it
// isolated. (See LV_ATTRIBUTE_LARGE_RAM_ARRAY in lv_conf.h.)
//
// [0] = V_SENSE_5 (ADC1 rank 1), [1] = V_SENSE_12 (ADC1 rank 2)
volatile uint32_t adc_dma_buf[ADC_DMA_BUF_LEN]
    __attribute__((section(".ram_d2"), aligned(ADC_CACHE_LINE_SIZE)));

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
