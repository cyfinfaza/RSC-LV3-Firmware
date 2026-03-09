/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2026 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32h7xx_hal.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */

/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/
/* USER CODE BEGIN EC */

/* USER CODE END EC */

/* Exported macro ------------------------------------------------------------*/
/* USER CODE BEGIN EM */

/* USER CODE END EM */

void HAL_TIM_MspPostInit(TIM_HandleTypeDef *htim);

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* USER CODE BEGIN EFP */

/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define LED_USR_Pin GPIO_PIN_2
#define LED_USR_GPIO_Port GPIOE
#define SD_EN_Pin GPIO_PIN_3
#define SD_EN_GPIO_Port GPIOE
#define DPAD_A0_Pin GPIO_PIN_2
#define DPAD_A0_GPIO_Port GPIOC
#define DPAD_A1_Pin GPIO_PIN_3
#define DPAD_A1_GPIO_Port GPIOC
#define V_SENSE_5_Pin GPIO_PIN_4
#define V_SENSE_5_GPIO_Port GPIOC
#define V_SENSE_12_Pin GPIO_PIN_1
#define V_SENSE_12_GPIO_Port GPIOB
#define BKLT_CTRL_Pin GPIO_PIN_9
#define BKLT_CTRL_GPIO_Port GPIOE
#define LCD_EN_Pin GPIO_PIN_10
#define LCD_EN_GPIO_Port GPIOE
#define SD_CS_Pin GPIO_PIN_15
#define SD_CS_GPIO_Port GPIOB
#define SD_WP_Pin GPIO_PIN_8
#define SD_WP_GPIO_Port GPIOD
#define SD_CD_Pin GPIO_PIN_9
#define SD_CD_GPIO_Port GPIOD
#define CTP_SCL_Pin GPIO_PIN_12
#define CTP_SCL_GPIO_Port GPIOD
#define CTP_SDA_Pin GPIO_PIN_13
#define CTP_SDA_GPIO_Port GPIOD
#define CTP_INT_Pin GPIO_PIN_14
#define CTP_INT_GPIO_Port GPIOD
#define CTP_RST_Pin GPIO_PIN_15
#define CTP_RST_GPIO_Port GPIOD
#define DPAD_RX_Pin GPIO_PIN_0
#define DPAD_RX_GPIO_Port GPIOD
#define DPAD_TX_Pin GPIO_PIN_1
#define DPAD_TX_GPIO_Port GPIOD
#define BOOT0_SENSE_Pin GPIO_PIN_7
#define BOOT0_SENSE_GPIO_Port GPIOB

/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
