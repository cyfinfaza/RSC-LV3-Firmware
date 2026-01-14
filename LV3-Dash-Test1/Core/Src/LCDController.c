/*
 * LCDController.c
 *
 *  Created on: Sep 28, 2023
 *      Author: controllerstech
 */


/*********************
 *      INCLUDES
 *********************/
#include "LCDController.h"
#include <stdbool.h>
#include <string.h>
#include "main.h"

/*********************
 *      DEFINES
 *********************/
#define MY_DISP_HOR_RES    480
#define MY_DISP_VER_RES    272

#define DOUBLE_BUFFERED 1

#define VSYNC_ENABLED 0

#define FRAME_SIZE MY_DISP_HOR_RES * MY_DISP_VER_RES * sizeof(lv_color_t)


/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/
static void disp_init(void);

static void disp_flush(lv_disp_drv_t * disp_drv, const lv_area_t * area, lv_color_t * color_p);
//static void gpu_fill(lv_disp_drv_t * disp_drv, lv_color_t * dest_buf, lv_coord_t dest_width,
//        const lv_area_t * fill_area, lv_color_t color);

/**********************
 *  STATIC VARIABLES
 **********************/

static lv_disp_drv_t disp_drv;                         /*Descriptor of a display driver*/

static LTDC_HandleTypeDef * hltdc;


// Double buffer frame buffers
// Buffer 1 goes to AXI SRAM (D1)
// This fits comfortably in the 512KB RAM block
uint8_t fb_axiram[FRAME_SIZE] __attribute__((section(".ram_d1"))) __attribute__((aligned(32)));

// Buffer 2 goes to AXI SRAM (D1)
// Moving to D1 to resolve access issues with LTDC
uint8_t fb_sram[FRAME_SIZE] __attribute__((section(".ram_d1"))) __attribute__((aligned(32)));

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

void lv_port_disp_init(LTDC_HandleTypeDef * provided_hltcd)
{
    /*-------------------------
     * Initialize your display
     * -----------------------*/
    disp_init();
    hltdc = provided_hltcd;

    /*-----------------------------
     * Create display buffers
     *----------------------------*/
    static lv_disp_draw_buf_t draw_buf_dsc;
    /* Use fb_sram for LVGL draw buffer. fb_axiram will be the LTDC frame buffer */
    lv_disp_draw_buf_init(&draw_buf_dsc, (lv_color_t *)fb_sram, NULL, MY_DISP_HOR_RES * MY_DISP_VER_RES);

    /* Set initial frame buffer address to fb_axiram */
    HAL_LTDC_SetAddress(hltdc, (uint32_t)fb_axiram, 0);

    /*-----------------------------------
     * Register the display driver
     *----------------------------------*/
    lv_disp_drv_init(&disp_drv);

    /* Set display resolution */
    disp_drv.hor_res = MY_DISP_HOR_RES;
    disp_drv.ver_res = MY_DISP_VER_RES;

    /* Set flush callback */
    disp_drv.flush_cb = disp_flush;

    /* Set display buffer */
    disp_drv.draw_buf = &draw_buf_dsc;

    disp_drv.sw_rotate = 1; 
    disp_drv.rotated = LV_DISP_ROT_180;

    /* Register the driver */
    lv_disp_drv_register(&disp_drv);
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

/*Initialize your display and the required peripherals.*/
static void disp_init(void)
{
    // display already initilaized
}

volatile bool disp_flush_enabled = true;

/* Enable updating the screen (the flushing process) when disp_flush() is called by LVGL
 */
void disp_enable_update(void)
{
    disp_flush_enabled = true;
}

/* Disable updating the screen (the flushing process) when disp_flush() is called by LVGL
 */
void disp_disable_update(void)
{
    disp_flush_enabled = false;
}

/* Flush the content of the internal buffer to the display
 * For full refresh / double buffering, we swapped the LTDC frame buffer.
 * For partial refresh, we copy the dirty area to the active frame buffer. */
static void disp_flush(lv_disp_drv_t * disp_drv, const lv_area_t * area, lv_color_t * color_p)
{
    lv_coord_t width = lv_area_get_width(area);
    lv_coord_t height = lv_area_get_height(area);

    /* The LTDC frame buffer is fb_axiram */
    lv_color_t * dest_buf = (lv_color_t *)fb_axiram;
    
    /* Copy data line by line */
    for(int y = area->y1; y <= area->y2; y++) {
        /* Destination offset: row index * row width + start x */
        /* Source is packed, so we just increment pointer */
        memcpy(&dest_buf[y * MY_DISP_HOR_RES + area->x1], color_p, width * sizeof(lv_color_t));
        
        /* Clean D-Cache for the updated line to ensure LTDC sees it */
        SCB_CleanDCache_by_Addr((uint32_t*)&dest_buf[y * MY_DISP_HOR_RES + area->x1], width * sizeof(lv_color_t));
        
        /* Advance source pointer */
        color_p += width;
    }

    /* Notify LVGL that flushing is complete */
    lv_disp_flush_ready(disp_drv);
}
