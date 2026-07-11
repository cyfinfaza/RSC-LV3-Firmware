#ifndef FT5206_H
#define FT5206_H

#ifdef __cplusplus
extern "C" {
#endif

#include "main.h"
#include "lvgl.h"

/* FT5206 I2C Address (7-bit address is 0x38) */
#define FT5206_I2C_ADDR (0x38 << 1)

/* Function Prototypes */
void FT5206_Init(void);
void FT5206_Read(lv_indev_t * indev, lv_indev_data_t * data);
void FT5206_IRQ_Handler(void);

#ifdef __cplusplus
}
#endif

#endif /* FT5206_H */
