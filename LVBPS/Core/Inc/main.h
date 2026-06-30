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
#define V_SENSE_12_BAT_Pin GPIO_PIN_1
#define V_SENSE_12_BAT_GPIO_Port GPIOA
#define V_SENSE_12_DCDC_Pin GPIO_PIN_2
#define V_SENSE_12_DCDC_GPIO_Port GPIOA
#define V_SENSE_12_LOAD_Pin GPIO_PIN_3
#define V_SENSE_12_LOAD_GPIO_Port GPIOA
#define INA_REF_BAT_Pin GPIO_PIN_4
#define INA_REF_BAT_GPIO_Port GPIOA
#define INA_REF_LOAD_Pin GPIO_PIN_5
#define INA_REF_LOAD_GPIO_Port GPIOA
#define I_SENSE_BAT_Pin GPIO_PIN_6
#define I_SENSE_BAT_GPIO_Port GPIOA
#define I_SENSE_LOAD_Pin GPIO_PIN_7
#define I_SENSE_LOAD_GPIO_Port GPIOA
#define THERMISTOR_SENSE_Pin GPIO_PIN_0
#define THERMISTOR_SENSE_GPIO_Port GPIOB
#define POWER_SWITCH_LED_Pin GPIO_PIN_12
#define POWER_SWITCH_LED_GPIO_Port GPIOB
#define RELAY_CONTROL_BAT_Pin GPIO_PIN_13
#define RELAY_CONTROL_BAT_GPIO_Port GPIOB
#define RELAY_CONTROL_DCDC_Pin GPIO_PIN_14
#define RELAY_CONTROL_DCDC_GPIO_Port GPIOB
#define PRECHARGE_CONTROL_BAT_Pin GPIO_PIN_15
#define PRECHARGE_CONTROL_BAT_GPIO_Port GPIOB
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

/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
