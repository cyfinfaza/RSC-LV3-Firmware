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
#include "stm32g0xx_hal.h"

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
#define BTN_USR_Pin GPIO_PIN_13
#define BTN_USR_GPIO_Port GPIOC
#define V_SENSE_5_Pin GPIO_PIN_0
#define V_SENSE_5_GPIO_Port GPIOA
#define V_SENSE_12_Pin GPIO_PIN_1
#define V_SENSE_12_GPIO_Port GPIOA
#define E_STOP_SENSE_Pin GPIO_PIN_2
#define E_STOP_SENSE_GPIO_Port GPIOA
#define E_STOP_SWITCH_LED_Pin GPIO_PIN_3
#define E_STOP_SWITCH_LED_GPIO_Port GPIOA
#define MAIN_CONTACTOR_ENABLE_Pin GPIO_PIN_4
#define MAIN_CONTACTOR_ENABLE_GPIO_Port GPIOA
#define BMS_DE_SENSE_Pin GPIO_PIN_5
#define BMS_DE_SENSE_GPIO_Port GPIOA
#define MODULE_LED_Pin GPIO_PIN_6
#define MODULE_LED_GPIO_Port GPIOA
#define STAT0_R_Pin GPIO_PIN_8
#define STAT0_R_GPIO_Port GPIOA
#define STAT0_G_Pin GPIO_PIN_9
#define STAT0_G_GPIO_Port GPIOA
#define STAT1_B_Pin GPIO_PIN_6
#define STAT1_B_GPIO_Port GPIOC
#define STAT0_B_Pin GPIO_PIN_10
#define STAT0_B_GPIO_Port GPIOA
#define STAT1_R_Pin GPIO_PIN_15
#define STAT1_R_GPIO_Port GPIOA
#define STAT1_G_Pin GPIO_PIN_3
#define STAT1_G_GPIO_Port GPIOB
#define FAN_1_TACH_Pin GPIO_PIN_6
#define FAN_1_TACH_GPIO_Port GPIOB
#define FAN_1_PWM_Pin GPIO_PIN_7
#define FAN_1_PWM_GPIO_Port GPIOB
#define FAN_2_TACH_Pin GPIO_PIN_8
#define FAN_2_TACH_GPIO_Port GPIOB
#define FAN_2_PWM_Pin GPIO_PIN_9
#define FAN_2_PWM_GPIO_Port GPIOB

/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
