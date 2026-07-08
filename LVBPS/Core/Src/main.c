/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
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
/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "LV3_CAN.h"
#include "adc_sense.h"
#include <stdint.h>
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

// Battery relay will not turn on if any of these faults are set
typedef union {
  uint8_t raw;
  struct {
    uint8_t startup_delay         : 1; // System has not yet been on for SETPOINT_STARTUP_DELAY_MS
    uint8_t startup_undervoltage  : 1; // Battery voltage is below SETPOINT_STARTUP_THRESHOLD_V
    uint8_t undervoltage  : 1; // Battery voltage has gone below SETPOINT_UNDERVOLTAGE_V
    uint8_t overvoltage   : 1; // Battery voltage has gone above SETPOINT_OVERVOLTAGE_V
    uint8_t overcurrent  : 1; // Battery current has gone above SETPOINT_MAX_CURRENT_A
    uint8_t undercurrent : 1; // Battery current has gone below SETPOINT_MIN_CURRENT_A
    uint8_t precharge_timeout : 1; // Precharge has lasted longer than PRECHARGE_TIMEOUT_MS
    uint8_t relay_fault       : 1; // Voltage difference exceeded precharge threshold while relay was on
  };
} Bat_Relay_Faults;

// DCDC relay will not turn on if any of these faults are set
typedef union {
  uint8_t raw;
  struct {
    uint8_t dcdc_startup_undervoltage  : 1; // DCDC voltage is below SETPOINT_STARTUP_THRESHOLD_V
    uint8_t dcdc_startup_overvoltage  : 1; // DCDC voltage is above SETPOINT_MAX_CHARGE_V
    uint8_t dcdc_undervoltage  : 1; // DCDC voltage is below SETPOINT_UNDERVOLTAGE_V
    uint8_t dcdc_overvoltage   : 1; // DCDC voltage is above SETPOINT_MAX_CHARGE_V
    uint8_t dcdc_overcurrent  : 1; // DCDC is causing battery current to go below SETPOINT_MIN_CURRENT_A
    uint8_t dcdc_sink : 1; // DCDC is sinking current
  };
} DCDC_Relay_Faults;
/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

// Battery relay will not turn on until HAL_GetTick() exceeds this value, sets startup delay fault
#define SETPOINT_STARTUP_DELAY_MS        500

// Battery will be cut off no matter what if voltage gets this low, latches undervoltage fault
#define SETPOINT_UNDERVOLTAGE_V    11.0f
#define SETPOINT_UNDERVOLTAGE_TIME_MS     500

// Battery will be cut off no matter what if voltage gets this high, latches overvoltage fault
#define SETPOINT_OVERVOLTAGE_V        14.8f
#define SETPOINT_OVERVOLTAGE_TIME_MS       500

// Bus will not transition from off to on if battery voltage is below this threshold, sets startup undervoltage fault
// DCDC relay will not turn on if battery voltage is below this threshold, sets DCDC startup undervoltage fault
// If DCDC relay is on and battery voltage goes below this threshold, DCDC relay will turn off, latches DCDC undervoltage fault
#define SETPOINT_STARTUP_THRESHOLD_V 11.8f

// DCDC relay will turn on if DCDC voltage and battery voltage is below this threshold, sets DCDC startup undervoltage fault
// If DCDC relay is on and battery voltage exceeds this threshold, DCDC relay will turn off, latches DCDC overvoltage fault
#define SETPOINT_MAX_CHARGE_V       13.8f

// Battery will be cut off no matter what if battery current exceeds this threshold for SETPOINT_OVERCURRENT_TIME_MS, latches overcurrent fault
#define SETPOINT_MAX_CURRENT_A       7.0f
#define SETPOINT_OVERCURRENT_TIME_MS      500

// If DCDC relay is on and difference between battery current and load current exceeds this threshold, DCDC relay will turn off, latches DCDC overcurrent fault
// If load current remains goes below this threshold for SETPOINT_UNDERCURRENT_TIME_MS, battery will be cut off, latches undercurrent fault
#define SETPOINT_MIN_CURRENT_A      -2.0f
#define SETPOINT_UNDERCURRENT_TIME_MS     100
#define DCDC_OVERCURRENT_TIMEOUT_MS       500

// If difference between battery current and load current exceeds this threshold, DCDC relay will turn off, latches DCDC sink fault
#define DCDC_MAX_SINK_CURRENT_A          0.5f
#define DCDC_UNDERVOLTAGE_TIMEOUT_MS      500
#define DCDC_OVERVOLTAGE_TIMEOUT_MS       500
#define DCDC_SINK_TIMEOUT_MS              500

// If precharging lasts longer than this time, latches precharge timeout fault
#define PRECHARGE_TIMEOUT_MS                500

// Maximum voltage difference between battery and load for precharge
#define PRECHARGE_THRESHOLD_V  9.0f


/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
ADC_HandleTypeDef hadc1;
DMA_HandleTypeDef hdma_adc1;

DAC_HandleTypeDef hdac1;

FDCAN_HandleTypeDef hfdcan1;

TIM_HandleTypeDef htim1;
TIM_HandleTypeDef htim2;

PCD_HandleTypeDef hpcd_USB_DRD_FS;

/* USER CODE BEGIN PV */
// Input state
GPIO_PinState BTN_USR;
GPIO_PinState BTN_USR_PREV = GPIO_PIN_RESET;

// Output state (0 = off, 1 = on)
uint8_t RELAY_CONTROL_BAT = 0;
uint8_t RELAY_CONTROL_DCDC = 0;
uint8_t PRECHARGE_CONTROL_BAT = 0;

uint8_t lv_bus_enabled = 1;
uint8_t bat_charge_enable = 1;
Bat_Relay_Faults bat_faults = {0};
uint8_t bat_faults_latched = 0;
DCDC_Relay_Faults dcdc_faults = {0};
uint32_t undervoltage_start_tick = 0;
uint32_t overvoltage_start_tick = 0;
uint32_t overcurrent_start_tick = 0;
uint32_t undercurrent_start_tick = 0;
uint32_t precharge_start_tick = 0;
uint32_t relay_fault_start_tick = 0;
uint32_t dcdc_undervoltage_start_tick = 0;
uint32_t dcdc_overvoltage_start_tick = 0;
uint32_t dcdc_overcurrent_start_tick = 0;
uint32_t dcdc_sink_start_tick = 0;
uint8_t precharge_complete = 0;

// LV3 CAN bound variables (integer mV / mA for CAN transmission)
uint32_t lv_bat_voltage_mv = 0;
uint32_t lv_dcdc_voltage_mv = 0;
uint32_t lv_load_voltage_mv = 0;
uint32_t lv_bat_current_ma = 0;
uint32_t lv_sys_current_ma = 0;
uint32_t lv_bat_temp_mv = 0;    // thermistor pin voltage in mV (raw; needs NTC curve for °C)
uint32_t lv_bat_faults_val = 0;
uint32_t lv_dcdc_faults_val = 0;
uint32_t lv_bat_relay_val = 0;
uint32_t lv_dcdc_relay_val = 0;

const LV3_CAN_Binding lv3_can_bindings[] = {
    {&lv_bat_voltage_mv,  lv_bat_voltage,  LV3_CAN_BindMode_Write},
    {&lv_dcdc_voltage_mv, lv_dcdc_voltage, LV3_CAN_BindMode_Write},
    {&lv_load_voltage_mv, lv_load_voltage, LV3_CAN_BindMode_Write},
    {&lv_bat_current_ma,  lv_bat_current,  LV3_CAN_BindMode_Write},
    {&lv_sys_current_ma,  lv_sys_current,  LV3_CAN_BindMode_Write},
    {&lv_bat_temp_mv,     lv_bat_temp,     LV3_CAN_BindMode_Write},
    {&lv_bat_faults_val,  lv_bat_faults,   LV3_CAN_BindMode_Write},
    {&lv_dcdc_faults_val, lv_dcdc_faults,  LV3_CAN_BindMode_Write},
    {&lv_bat_relay_val,   lv_bat_relay,    LV3_CAN_BindMode_Write},
    {&lv_dcdc_relay_val,  lv_dcdc_relay,   LV3_CAN_BindMode_Write},
};
const unsigned int lv3_can_bindings_count = sizeof(lv3_can_bindings) / sizeof(LV3_CAN_Binding);
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_DMA_Init(void);
static void MX_FDCAN1_Init(void);
static void MX_USB_DRD_FS_PCD_Init(void);
static void MX_TIM1_Init(void);
static void MX_TIM2_Init(void);
static void MX_ADC1_Init(void);
static void MX_DAC1_Init(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

static uint8_t timed_fault(uint8_t condition, uint32_t *start_tick, uint32_t timeout_ms) {
  if (condition) {
    if (*start_tick == 0) *start_tick = HAL_GetTick();
    if (HAL_GetTick() - *start_tick >= timeout_ms) return 1;
  } else {
    *start_tick = 0;
  }
  return 0;
}

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{

  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_DMA_Init();
  MX_FDCAN1_Init();
  MX_USB_DRD_FS_PCD_Init();
  MX_TIM1_Init();
  MX_TIM2_Init();
  MX_ADC1_Init();
  MX_DAC1_Init();
  /* USER CODE BEGIN 2 */

  HAL_ADCEx_Calibration_Start(&hadc1);

  HAL_ADC_Start_DMA(&hadc1, (uint32_t *)adc_buf, 9);

  HAL_DAC_SetValue(&hdac1, DAC_CHANNEL_1, DAC_ALIGN_12B_R, 4096 / 2);
  HAL_DAC_Start(&hdac1, DAC_CHANNEL_1);
  HAL_DAC_SetValue(&hdac1, DAC_CHANNEL_2, DAC_ALIGN_12B_R, 4096 / 2);
  HAL_DAC_Start(&hdac1, DAC_CHANNEL_2);

  HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_1);
  HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_2);
  HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_3);

  __HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_1, 0);
  __HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_2, 0);
  __HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_3, 0);

  LV3_CAN_Init(15, LV3_CAN_BusMode_Normal, lv3_can_bindings, lv3_can_bindings_count);

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */

    // --- Read inputs ---
    ADC_Sense_Update();
    BTN_USR = HAL_GPIO_ReadPin(BTN_USR_GPIO_Port, BTN_USR_Pin);

    // --- Process state ---
    lv_bat_voltage_mv  = (uint32_t)(adc_sense.v_sense_12_bat  * 1000.0f);
    lv_dcdc_voltage_mv = (uint32_t)(adc_sense.v_sense_12_dcdc * 1000.0f);
    lv_load_voltage_mv = (uint32_t)(adc_sense.v_sense_12_load * 1000.0f);
    lv_bat_current_ma  = (uint32_t)(adc_sense.i_sense_bat     * 1000.0f);
    lv_sys_current_ma  = (uint32_t)(adc_sense.i_sense_load    * 1000.0f);
    lv_bat_temp_mv     = (uint32_t)(adc_sense.thermistor_mv);

    // USR button toggles lv_bus_enabled on rising edge
    if (BTN_USR == GPIO_PIN_SET && BTN_USR_PREV == GPIO_PIN_RESET) {
      lv_bus_enabled ^= 1;
    }
    BTN_USR_PREV = BTN_USR;

    // DCDC current: battery current minus load current (positive = DCDC sourcing into bat, negative = sinking from bat)
    float dcdc_current = adc_sense.i_sense_bat - adc_sense.i_sense_load;

    // Clear all bat faults each loop unless latched, so faults reflect live conditions when relay is off
    if (!bat_faults_latched) bat_faults.raw = 0;

    // Non-latching startup faults: only relevant when relay is off; gate turn-on, not turn-off
    bat_faults.startup_delay        = !RELAY_CONTROL_BAT && (HAL_GetTick() < SETPOINT_STARTUP_DELAY_MS);
    bat_faults.startup_undervoltage = !RELAY_CONTROL_BAT && (adc_sense.v_sense_12_bat < SETPOINT_STARTUP_THRESHOLD_V);

    // Latching bat faults
    if (timed_fault(adc_sense.v_sense_12_bat < SETPOINT_UNDERVOLTAGE_V,                                       &undervoltage_start_tick, SETPOINT_UNDERVOLTAGE_TIME_MS)) bat_faults.undervoltage = 1;
    if (timed_fault(adc_sense.v_sense_12_bat > SETPOINT_OVERVOLTAGE_V,                                        &overvoltage_start_tick,  SETPOINT_OVERVOLTAGE_TIME_MS))  bat_faults.overvoltage  = 1;
    if (timed_fault(adc_sense.i_sense_bat > SETPOINT_MAX_CURRENT_A,                                           &overcurrent_start_tick,  SETPOINT_OVERCURRENT_TIME_MS))  bat_faults.overcurrent  = 1;
    if (timed_fault(adc_sense.i_sense_load < SETPOINT_MIN_CURRENT_A,                                          &undercurrent_start_tick, SETPOINT_UNDERCURRENT_TIME_MS)) bat_faults.undercurrent = 1;
    if (timed_fault(RELAY_CONTROL_BAT && adc_sense.v_sense_12_bat - adc_sense.v_sense_12_load >= PRECHARGE_THRESHOLD_V, &relay_fault_start_tick,  PRECHARGE_TIMEOUT_MS))          bat_faults.relay_fault  = 1;

    // Precharge state machine
    uint8_t relay_was_on = RELAY_CONTROL_BAT;
    uint8_t want_bus = lv_bus_enabled && !bat_faults.raw;
    if (want_bus && !PRECHARGE_CONTROL_BAT) {  // precharge just requested (once)
      precharge_start_tick = HAL_GetTick();
    }
    PRECHARGE_CONTROL_BAT = want_bus;  // precharge always matches bus intent
    if (want_bus) {
      if (adc_sense.v_sense_12_bat - adc_sense.v_sense_12_load < PRECHARGE_THRESHOLD_V) {  // within threshold (repeatedly)
        precharge_complete = 1;
      }
      if (precharge_complete) {  // precharge was successful (repeatedly)
        RELAY_CONTROL_BAT = 1;
      } else if (HAL_GetTick() - precharge_start_tick >= PRECHARGE_TIMEOUT_MS) {  // precharge timed out (once)
        bat_faults.precharge_timeout = 1;
      }
    } else {
      RELAY_CONTROL_BAT = 0;
      precharge_complete = 0;
      precharge_start_tick = 0;
    }

    // Latch if relay was on this loop and a latching fault fired (relay may now be off due to the fault)
    if ((relay_was_on && (bat_faults.undervoltage || bat_faults.overvoltage
                        || bat_faults.overcurrent  || bat_faults.undercurrent
                        || bat_faults.relay_fault))
        || bat_faults.precharge_timeout) {
      bat_faults_latched = 1;
    }

    // Non-latching DCDC startup faults: only relevant when relay is off; gate turn-on, not turn-off
    dcdc_faults.dcdc_startup_undervoltage = !RELAY_CONTROL_DCDC && (adc_sense.v_sense_12_bat < SETPOINT_STARTUP_THRESHOLD_V
                                                                  || adc_sense.v_sense_12_dcdc < SETPOINT_STARTUP_THRESHOLD_V);
    dcdc_faults.dcdc_startup_overvoltage  = !RELAY_CONTROL_DCDC && (adc_sense.v_sense_12_bat >= SETPOINT_MAX_CHARGE_V
                                                                  || adc_sense.v_sense_12_dcdc >= SETPOINT_MAX_CHARGE_V);

    // Latching DCDC faults
    if (timed_fault(RELAY_CONTROL_DCDC && adc_sense.v_sense_12_bat < SETPOINT_STARTUP_THRESHOLD_V, &dcdc_undervoltage_start_tick, DCDC_UNDERVOLTAGE_TIMEOUT_MS)) dcdc_faults.dcdc_undervoltage = 1;
    if (timed_fault(RELAY_CONTROL_DCDC && adc_sense.v_sense_12_bat > SETPOINT_MAX_CHARGE_V,        &dcdc_overvoltage_start_tick,  DCDC_OVERVOLTAGE_TIMEOUT_MS))  dcdc_faults.dcdc_overvoltage  = 1;
    if (timed_fault(RELAY_CONTROL_DCDC && dcdc_current < SETPOINT_MIN_CURRENT_A,         &dcdc_overcurrent_start_tick,  DCDC_OVERCURRENT_TIMEOUT_MS))  dcdc_faults.dcdc_overcurrent  = 1;
    if (timed_fault(RELAY_CONTROL_DCDC && dcdc_current > DCDC_MAX_SINK_CURRENT_A,        &dcdc_sink_start_tick,         DCDC_SINK_TIMEOUT_MS))         dcdc_faults.dcdc_sink         = 1;

    // DCDC relay: on when BAT relay is on, bat_charge_enable set, and no dcdc faults (startup faults clear automatically)
    RELAY_CONTROL_DCDC = RELAY_CONTROL_BAT && bat_charge_enable && !dcdc_faults.raw;

    // --- Apply outputs ---
    HAL_GPIO_WritePin(RELAY_CONTROL_BAT_GPIO_Port, RELAY_CONTROL_BAT_Pin, RELAY_CONTROL_BAT);
    HAL_GPIO_WritePin(RELAY_CONTROL_DCDC_GPIO_Port, RELAY_CONTROL_DCDC_Pin, RELAY_CONTROL_DCDC);
    HAL_GPIO_WritePin(PRECHARGE_CONTROL_BAT_GPIO_Port, PRECHARGE_CONTROL_BAT_Pin, PRECHARGE_CONTROL_BAT);

    // STAT0 LED: battery voltage mapped as hue red→yellow→green→cyan→blue across undervoltage→overvoltage
    float v_norm = (adc_sense.v_sense_12_bat - SETPOINT_UNDERVOLTAGE_V) / (SETPOINT_OVERVOLTAGE_V - SETPOINT_UNDERVOLTAGE_V);
    if (v_norm < 0.0f) v_norm = 0.0f;
    if (v_norm > 1.0f) v_norm = 1.0f;
    float hue = v_norm * 240.0f;  // red=0, yellow=60, green=120, cyan=180, blue=240
    float hf = hue / 60.0f;
    int   hs = (int)hf;
    float f  = hf - hs;
    float r, g, b;
    switch (hs) {
      case 0:  r=1.0f;   g=f;      b=0.0f;  break;  // red → yellow
      case 1:  r=1.0f-f; g=1.0f;   b=0.0f;  break;  // yellow → green
      case 2:  r=0.0f;   g=1.0f;   b=f;     break;  // green → cyan
      case 3:  r=0.0f;   g=1.0f-f; b=1.0f;  break;  // cyan → blue
      default: r=0.0f;   g=0.0f;   b=1.0f;  break;
    }

    uint8_t led_on;
    if      (bat_faults_latched)  
    led_on = (HAL_GetTick() % 200) < 100;   // fast blink: latching fault
    else if (bat_faults.raw)      led_on = (HAL_GetTick() % 1000) < 500;  // slow blink: non-latching fault
    else                          led_on = 1;

    __HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_1, led_on ? (uint32_t)(g * 100) : 0);  // G
    __HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_2, led_on ? (uint32_t)(r * 100) : 0);  // R
    __HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_3, led_on ? (uint32_t)(b * 100) : 0);  // B

    lv_bat_faults_val  = bat_faults.raw;
    lv_dcdc_faults_val = dcdc_faults.raw;
    lv_bat_relay_val   = RELAY_CONTROL_BAT;
    lv_dcdc_relay_val  = RELAY_CONTROL_DCDC;

    LV3_CAN_Loop();

  }
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Configure the main internal regulator output voltage
  */
  HAL_PWREx_ControlVoltageScaling(PWR_REGULATOR_VOLTAGE_SCALE1);

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE|RCC_OSCILLATORTYPE_HSI48;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.HSI48State = RCC_HSI48_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLM = RCC_PLLM_DIV2;
  RCC_OscInitStruct.PLL.PLLN = 32;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = RCC_PLLQ_DIV2;
  RCC_OscInitStruct.PLL.PLLR = RCC_PLLR_DIV4;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_1) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief ADC1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_ADC1_Init(void)
{

  /* USER CODE BEGIN ADC1_Init 0 */

  /* USER CODE END ADC1_Init 0 */

  ADC_ChannelConfTypeDef sConfig = {0};

  /* USER CODE BEGIN ADC1_Init 1 */

  /* USER CODE END ADC1_Init 1 */

  /** Configure the global features of the ADC (Clock, Resolution, Data Alignment and number of conversion)
  */
  hadc1.Instance = ADC1;
  hadc1.Init.ClockPrescaler = ADC_CLOCK_SYNC_PCLK_DIV2;
  hadc1.Init.Resolution = ADC_RESOLUTION_12B;
  hadc1.Init.DataAlign = ADC_DATAALIGN_RIGHT;
  hadc1.Init.ScanConvMode = ADC_SCAN_SEQ_FIXED;
  hadc1.Init.EOCSelection = ADC_EOC_SINGLE_CONV;
  hadc1.Init.LowPowerAutoWait = DISABLE;
  hadc1.Init.LowPowerAutoPowerOff = DISABLE;
  hadc1.Init.ContinuousConvMode = ENABLE;
  hadc1.Init.NbrOfConversion = 1;
  hadc1.Init.ExternalTrigConv = ADC_SOFTWARE_START;
  hadc1.Init.ExternalTrigConvEdge = ADC_EXTERNALTRIGCONVEDGE_NONE;
  hadc1.Init.DMAContinuousRequests = ENABLE;
  hadc1.Init.Overrun = ADC_OVR_DATA_PRESERVED;
  hadc1.Init.SamplingTimeCommon1 = ADC_SAMPLETIME_7CYCLES_5;
  hadc1.Init.OversamplingMode = ENABLE;
  hadc1.Init.Oversampling.Ratio = ADC_OVERSAMPLING_RATIO_64;
  hadc1.Init.Oversampling.RightBitShift = ADC_RIGHTBITSHIFT_2;
  hadc1.Init.Oversampling.TriggeredMode = ADC_TRIGGEREDMODE_SINGLE_TRIGGER;
  hadc1.Init.TriggerFrequencyMode = ADC_TRIGGER_FREQ_HIGH;
  if (HAL_ADC_Init(&hadc1) != HAL_OK)
  {
    Error_Handler();
  }

  /** Configure Regular Channel
  */
  sConfig.Channel = ADC_CHANNEL_0;
  sConfig.Rank = ADC_RANK_CHANNEL_NUMBER;
  if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK)
  {
    Error_Handler();
  }

  /** Configure Regular Channel
  */
  sConfig.Channel = ADC_CHANNEL_1;
  if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK)
  {
    Error_Handler();
  }

  /** Configure Regular Channel
  */
  sConfig.Channel = ADC_CHANNEL_2;
  if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK)
  {
    Error_Handler();
  }

  /** Configure Regular Channel
  */
  sConfig.Channel = ADC_CHANNEL_3;
  if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK)
  {
    Error_Handler();
  }

  /** Configure Regular Channel
  */
  sConfig.Channel = ADC_CHANNEL_6;
  if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK)
  {
    Error_Handler();
  }

  /** Configure Regular Channel
  */
  sConfig.Channel = ADC_CHANNEL_7;
  if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK)
  {
    Error_Handler();
  }

  /** Configure Regular Channel
  */
  sConfig.Channel = ADC_CHANNEL_TEMPSENSOR;
  if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK)
  {
    Error_Handler();
  }

  /** Configure Regular Channel
  */
  sConfig.Channel = ADC_CHANNEL_VREFINT;
  if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK)
  {
    Error_Handler();
  }

  /** Configure Regular Channel
  */
  sConfig.Channel = ADC_CHANNEL_8;
  if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN ADC1_Init 2 */

  /* USER CODE END ADC1_Init 2 */

}

/**
  * @brief DAC1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_DAC1_Init(void)
{

  /* USER CODE BEGIN DAC1_Init 0 */

  /* USER CODE END DAC1_Init 0 */

  DAC_ChannelConfTypeDef sConfig = {0};

  /* USER CODE BEGIN DAC1_Init 1 */

  /* USER CODE END DAC1_Init 1 */

  /** DAC Initialization
  */
  hdac1.Instance = DAC1;
  if (HAL_DAC_Init(&hdac1) != HAL_OK)
  {
    Error_Handler();
  }

  /** DAC channel OUT1 config
  */
  sConfig.DAC_SampleAndHold = DAC_SAMPLEANDHOLD_DISABLE;
  sConfig.DAC_Trigger = DAC_TRIGGER_NONE;
  sConfig.DAC_OutputBuffer = DAC_OUTPUTBUFFER_ENABLE;
  sConfig.DAC_ConnectOnChipPeripheral = DAC_CHIPCONNECT_DISABLE;
  sConfig.DAC_UserTrimming = DAC_TRIMMING_FACTORY;
  if (HAL_DAC_ConfigChannel(&hdac1, &sConfig, DAC_CHANNEL_1) != HAL_OK)
  {
    Error_Handler();
  }

  /** DAC channel OUT2 config
  */
  if (HAL_DAC_ConfigChannel(&hdac1, &sConfig, DAC_CHANNEL_2) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN DAC1_Init 2 */

  /* USER CODE END DAC1_Init 2 */

}

/**
  * @brief FDCAN1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_FDCAN1_Init(void)
{

  /* USER CODE BEGIN FDCAN1_Init 0 */

  /* USER CODE END FDCAN1_Init 0 */

  /* USER CODE BEGIN FDCAN1_Init 1 */

  /* USER CODE END FDCAN1_Init 1 */
  hfdcan1.Instance = FDCAN1;
  hfdcan1.Init.ClockDivider = FDCAN_CLOCK_DIV1;
  hfdcan1.Init.FrameFormat = FDCAN_FRAME_CLASSIC;
  hfdcan1.Init.Mode = FDCAN_MODE_NORMAL;
  hfdcan1.Init.AutoRetransmission = DISABLE;
  hfdcan1.Init.TransmitPause = DISABLE;
  hfdcan1.Init.ProtocolException = DISABLE;
  hfdcan1.Init.NominalPrescaler = 12;
  hfdcan1.Init.NominalSyncJumpWidth = 1;
  hfdcan1.Init.NominalTimeSeg1 = 13;
  hfdcan1.Init.NominalTimeSeg2 = 2;
  hfdcan1.Init.DataPrescaler = 1;
  hfdcan1.Init.DataSyncJumpWidth = 1;
  hfdcan1.Init.DataTimeSeg1 = 1;
  hfdcan1.Init.DataTimeSeg2 = 1;
  hfdcan1.Init.StdFiltersNbr = 0;
  hfdcan1.Init.ExtFiltersNbr = 0;
  hfdcan1.Init.TxFifoQueueMode = FDCAN_TX_FIFO_OPERATION;
  if (HAL_FDCAN_Init(&hfdcan1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN FDCAN1_Init 2 */

  /* USER CODE END FDCAN1_Init 2 */

}

/**
  * @brief TIM1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM1_Init(void)
{

  /* USER CODE BEGIN TIM1_Init 0 */

  /* USER CODE END TIM1_Init 0 */

  TIM_MasterConfigTypeDef sMasterConfig = {0};
  TIM_OC_InitTypeDef sConfigOC = {0};
  TIM_BreakDeadTimeConfigTypeDef sBreakDeadTimeConfig = {0};

  /* USER CODE BEGIN TIM1_Init 1 */

  /* USER CODE END TIM1_Init 1 */
  htim1.Instance = TIM1;
  htim1.Init.Prescaler = 10;
  htim1.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim1.Init.Period = 100;
  htim1.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim1.Init.RepetitionCounter = 0;
  htim1.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_PWM_Init(&htim1) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterOutputTrigger2 = TIM_TRGO2_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim1, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  sConfigOC.OCMode = TIM_OCMODE_PWM1;
  sConfigOC.Pulse = 0;
  sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;
  sConfigOC.OCNPolarity = TIM_OCNPOLARITY_HIGH;
  sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;
  sConfigOC.OCIdleState = TIM_OCIDLESTATE_RESET;
  sConfigOC.OCNIdleState = TIM_OCNIDLESTATE_RESET;
  if (HAL_TIM_PWM_ConfigChannel(&htim1, &sConfigOC, TIM_CHANNEL_1) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_TIM_PWM_ConfigChannel(&htim1, &sConfigOC, TIM_CHANNEL_2) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_TIM_PWM_ConfigChannel(&htim1, &sConfigOC, TIM_CHANNEL_3) != HAL_OK)
  {
    Error_Handler();
  }
  sBreakDeadTimeConfig.OffStateRunMode = TIM_OSSR_DISABLE;
  sBreakDeadTimeConfig.OffStateIDLEMode = TIM_OSSI_DISABLE;
  sBreakDeadTimeConfig.LockLevel = TIM_LOCKLEVEL_OFF;
  sBreakDeadTimeConfig.DeadTime = 0;
  sBreakDeadTimeConfig.BreakState = TIM_BREAK_DISABLE;
  sBreakDeadTimeConfig.BreakPolarity = TIM_BREAKPOLARITY_HIGH;
  sBreakDeadTimeConfig.BreakFilter = 0;
  sBreakDeadTimeConfig.BreakAFMode = TIM_BREAK_AFMODE_INPUT;
  sBreakDeadTimeConfig.Break2State = TIM_BREAK2_DISABLE;
  sBreakDeadTimeConfig.Break2Polarity = TIM_BREAK2POLARITY_HIGH;
  sBreakDeadTimeConfig.Break2Filter = 0;
  sBreakDeadTimeConfig.Break2AFMode = TIM_BREAK_AFMODE_INPUT;
  sBreakDeadTimeConfig.AutomaticOutput = TIM_AUTOMATICOUTPUT_DISABLE;
  if (HAL_TIMEx_ConfigBreakDeadTime(&htim1, &sBreakDeadTimeConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM1_Init 2 */

  /* USER CODE END TIM1_Init 2 */
  HAL_TIM_MspPostInit(&htim1);

}

/**
  * @brief TIM2 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM2_Init(void)
{

  /* USER CODE BEGIN TIM2_Init 0 */

  /* USER CODE END TIM2_Init 0 */

  TIM_MasterConfigTypeDef sMasterConfig = {0};
  TIM_OC_InitTypeDef sConfigOC = {0};

  /* USER CODE BEGIN TIM2_Init 1 */

  /* USER CODE END TIM2_Init 1 */
  htim2.Instance = TIM2;
  htim2.Init.Prescaler = 10;
  htim2.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim2.Init.Period = 100;
  htim2.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim2.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_PWM_Init(&htim2) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim2, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  sConfigOC.OCMode = TIM_OCMODE_PWM1;
  sConfigOC.Pulse = 0;
  sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;
  sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;
  if (HAL_TIM_PWM_ConfigChannel(&htim2, &sConfigOC, TIM_CHANNEL_1) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_TIM_PWM_ConfigChannel(&htim2, &sConfigOC, TIM_CHANNEL_2) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_TIM_PWM_ConfigChannel(&htim2, &sConfigOC, TIM_CHANNEL_3) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM2_Init 2 */

  /* USER CODE END TIM2_Init 2 */
  HAL_TIM_MspPostInit(&htim2);

}

/**
  * @brief USB_DRD_FS Initialization Function
  * @param None
  * @retval None
  */
static void MX_USB_DRD_FS_PCD_Init(void)
{

  /* USER CODE BEGIN USB_DRD_FS_Init 0 */

  /* USER CODE END USB_DRD_FS_Init 0 */

  /* USER CODE BEGIN USB_DRD_FS_Init 1 */

  /* USER CODE END USB_DRD_FS_Init 1 */
  hpcd_USB_DRD_FS.Instance = USB_DRD_FS;
  hpcd_USB_DRD_FS.Init.dev_endpoints = 8;
  hpcd_USB_DRD_FS.Init.Host_channels = 8;
  hpcd_USB_DRD_FS.Init.speed = PCD_SPEED_FULL;
  hpcd_USB_DRD_FS.Init.phy_itface = PCD_PHY_EMBEDDED;
  hpcd_USB_DRD_FS.Init.Sof_enable = DISABLE;
  hpcd_USB_DRD_FS.Init.low_power_enable = DISABLE;
  hpcd_USB_DRD_FS.Init.lpm_enable = DISABLE;
  hpcd_USB_DRD_FS.Init.battery_charging_enable = DISABLE;
  hpcd_USB_DRD_FS.Init.vbus_sensing_enable = DISABLE;
  hpcd_USB_DRD_FS.Init.bulk_doublebuffer_enable = DISABLE;
  hpcd_USB_DRD_FS.Init.iso_singlebuffer_enable = DISABLE;
  if (HAL_PCD_Init(&hpcd_USB_DRD_FS) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USB_DRD_FS_Init 2 */

  /* USER CODE END USB_DRD_FS_Init 2 */

}

/**
  * Enable DMA controller clock
  */
static void MX_DMA_Init(void)
{

  /* DMA controller clock enable */
  __HAL_RCC_DMA1_CLK_ENABLE();

  /* DMA interrupt init */
  /* DMA1_Channel1_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(DMA1_Channel1_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(DMA1_Channel1_IRQn);

}

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};
  /* USER CODE BEGIN MX_GPIO_Init_1 */

  /* USER CODE END MX_GPIO_Init_1 */

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOF_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();
  __HAL_RCC_GPIOD_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOB, POWER_SWITCH_LED_Pin|RELAY_CONTROL_BAT_Pin|RELAY_CONTROL_DCDC_Pin|PRECHARGE_CONTROL_BAT_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin : BTN_USR_Pin */
  GPIO_InitStruct.Pin = BTN_USR_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(BTN_USR_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pins : POWER_SWITCH_LED_Pin RELAY_CONTROL_BAT_Pin RELAY_CONTROL_DCDC_Pin PRECHARGE_CONTROL_BAT_Pin */
  GPIO_InitStruct.Pin = POWER_SWITCH_LED_Pin|RELAY_CONTROL_BAT_Pin|RELAY_CONTROL_DCDC_Pin|PRECHARGE_CONTROL_BAT_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /* USER CODE BEGIN MX_GPIO_Init_2 */

  /* USER CODE END MX_GPIO_Init_2 */
}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}
#ifdef USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
