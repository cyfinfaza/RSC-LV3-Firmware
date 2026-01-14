#include "FT5206.h"

extern I2C_HandleTypeDef hi2c4;
volatile uint8_t touch_irq_flag = 0;

/**
 * @brief  Initialize the FT5206 communication
 * @retval None
 */
void FT5206_Init(void)
{
    // Drive Reset Pin Low (Reset Active)
    HAL_GPIO_WritePin(CTP_RST_GPIO_Port, CTP_RST_Pin, GPIO_PIN_RESET);
    HAL_Delay(20);
    
    // Drive Reset Pin High (Release Reset)
    HAL_GPIO_WritePin(CTP_RST_GPIO_Port, CTP_RST_Pin, GPIO_PIN_SET);
    HAL_Delay(200); // Wait for device to boot

    // Check if the device is ready
    volatile HAL_StatusTypeDef status = HAL_I2C_IsDeviceReady(&hi2c4, FT5206_I2C_ADDR, 5, 100);

    if (status != HAL_OK)
    {
       // Device still not responding after reset
    }
}

/**
 * @brief  Interrupt Handler (called from EXTI ISR)
 * @retval None
 */
void FT5206_IRQ_Handler(void)
{
    touch_irq_flag = 1;
}

/**
 * @brief  Get the touch screen state
 * @param  indev: pointer to the input device
 * @param  data: pointer to the input device data
 * @retval None
 */
void FT5206_Read(lv_indev_t * indev, lv_indev_data_t * data)
{
    static int16_t last_x = 0;
    static int16_t last_y = 0;
    uint8_t rx_buf[5];

    // Check if interrupt occurred or if pin is still low (touch held)
    if (touch_irq_flag || HAL_GPIO_ReadPin(CTP_INT_GPIO_Port, CTP_INT_Pin) == GPIO_PIN_RESET)
    {
        touch_irq_flag = 0; // Clear flag

        /* 
        Register Map to read:
        0x02: TD_STATUS
        0x03: P1_XH (Event Flag[7:6] + X[11:8])
        0x04: P1_XL (X[7:0])
        0x05: P1_YH (Touch ID[7:4] + Y[11:8])
        0x06: P1_YL (Y[7:0])
        */

        if (HAL_I2C_Mem_Read(&hi2c4, FT5206_I2C_ADDR, 0x02, I2C_MEMADD_SIZE_8BIT, rx_buf, 5, 20) == HAL_OK)
        {
            uint8_t touch_count = rx_buf[0] & 0x0F;
            
            if (touch_count > 0 && touch_count <= 5)
            {
                // Extract coordinates
                uint16_t x = ((rx_buf[1] & 0x0F) << 8) | rx_buf[2];
                uint16_t y = ((rx_buf[3] & 0x0F) << 8) | rx_buf[4];
                
                last_x = x;
                last_y = y;
                data->state = LV_INDEV_STATE_PRESSED;
            }
            else
            {
                data->state = LV_INDEV_STATE_RELEASED;
            }
        }
        else
        {
            // I2C transfer error, assume released
            data->state = LV_INDEV_STATE_RELEASED;
        }
    }
    else
    {
        // No interrupt and pin high -> Released
        data->state = LV_INDEV_STATE_RELEASED;
    }

    data->point.x = last_x;
    data->point.y = last_y;
}
