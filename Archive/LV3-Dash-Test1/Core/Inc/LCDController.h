/*
 * LCDController.h
 *
 *  Created on: Sep 28, 2023
 *      Author: controllerstech
 */

#ifndef INC_LCDCONTROLLER_H_
#define INC_LCDCONTROLLER_H_

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/

#include "lvgl.h"
#include "stm32h7xx_hal.h"

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 * GLOBAL PROTOTYPES
 **********************/
/* Initialize low level display driver */
void lv_port_disp_init(LTDC_HandleTypeDef *provided_hltcd);

/* Enable updating the screen (the flushing process) when disp_flush() is called
 * by LVGL
 */
void disp_enable_update(void);

/* Disable updating the screen (the flushing process) when disp_flush() is
 * called by LVGL
 */
void disp_disable_update(void);

/* Manual display refresh trigger - called from HAL_LTDC_ReloadEventCallback()
 * This is used for decoupled refresh timer mode, synchronized with VSYNC/TE
 * signal
 */
extern void _lv_disp_refr_timer(struct _lv_timer_t *timer);

/**********************
 *      MACROS
 **********************/

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /* INC_LCDCONTROLLER_H_ */
