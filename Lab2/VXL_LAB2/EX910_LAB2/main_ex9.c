/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2025 STMicroelectronics.
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

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
TIM_HandleTypeDef htim2;

/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_TIM2_Init(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
#define MAX_LED 4U
#define SCAN_TICKS 25U
#define DOT_TICKS 100U

#define TIMER_CYCLE 10U

#define MAX_LED_MATRIX 8U
#define MATRIX_SCAN_TIME_MS 1U

#define MATRIX_EN_ALL_PINS (ENM0_Pin | ENM1_Pin | \
                            ENM2_Pin | ENM3_Pin | \
                            ENM4_Pin | ENM5_Pin | \
                            ENM6_Pin | ENM7_Pin)

#define MATRIX_ROW_ALL_PINS (ROW0_Pin | ROW1_Pin | \
                             ROW2_Pin | ROW3_Pin | \
                             ROW4_Pin | ROW5_Pin | \
                             ROW6_Pin | ROW7_Pin)

static const uint16_t matrixEnablePins[MAX_LED_MATRIX] =
{
    ENM0_Pin,
    ENM1_Pin,
    ENM2_Pin,
    ENM3_Pin,
    ENM4_Pin,
    ENM5_Pin,
    ENM6_Pin,
    ENM7_Pin
};

static const uint16_t matrixRowPins[MAX_LED_MATRIX] =
{
    ROW0_Pin,
    ROW1_Pin,
    ROW2_Pin,
    ROW3_Pin,
    ROW4_Pin,
    ROW5_Pin,
    ROW6_Pin,
    ROW7_Pin
};

/* Bitmap chu A theo cot, giong ban demo main_ex9_ldt.c. */
static const uint8_t matrixLetterA[MAX_LED_MATRIX] =
{
    0x00U,
    0xFCU,
    0x0AU,
    0x09U,
    0x09U,
    0x0AU,
    0xFCU,
    0x00U
};

static volatile uint8_t index_led_matrix = 0U;
static uint32_t matrixLastTick = 0U;

static void updateLEDMatrix(uint8_t index)
{
    uint16_t activeRowPins = 0U;

    if (index >= MAX_LED_MATRIX)
    {
        return;
    }

    /* ENM active-low: SET tat tat ca cot truoc khi thay doi ROW. */
    HAL_GPIO_WritePin(
        GPIOA,
        MATRIX_EN_ALL_PINS,
        GPIO_PIN_SET
    );

    /* ROW active-low: SET tat tat ca hang. */
    HAL_GPIO_WritePin(
        GPIOB,
        MATRIX_ROW_ALL_PINS,
        GPIO_PIN_SET
    );

    /* Moi byte la du lieu 8 hang cua mot cot; bit 1 nghia la LED sang. */
    for (uint8_t row = 0U; row < MAX_LED_MATRIX; row++)
    {
        if ((matrixLetterA[index] & (1U << row)) != 0U)
        {
            activeRowPins |= matrixRowPins[row];
        }
    }

    HAL_GPIO_WritePin(GPIOB, activeRowPins, GPIO_PIN_RESET);

    /* Chi chon mot cot bang muc RESET, dung nhu ban demo. */
    HAL_GPIO_WritePin(
        GPIOA,
        matrixEnablePins[index],
        GPIO_PIN_RESET
    );
}

static volatile uint16_t timer0_counter = 0U;
static volatile uint8_t timer0_flag = 0U;

static volatile uint16_t timer1_counter = 0U;
static volatile uint8_t timer1_flag = 0U;

static volatile uint8_t index_led = 0U;
static volatile uint16_t scan_counter = 0U;
static volatile uint16_t dot_counter = 0U;

static uint8_t hour = 15U;
static uint8_t minute = 8U;
static uint8_t second = 50U;


#define SEG_ALL_PINS (GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_2 | \
                      GPIO_PIN_3 | GPIO_PIN_4 | GPIO_PIN_5 | \
                      GPIO_PIN_6)

#define DIGIT_ALL_PINS (GPIO_PIN_6 | GPIO_PIN_7 | \
                        GPIO_PIN_8 | GPIO_PIN_9)

static const uint8_t segPattern[10] =
{
    0x3FU, // 0
    0x06U, // 1
    0x5BU, // 2
    0x4FU, // 3
    0x66U, // 4
    0x6DU, // 5
    0x7DU, // 6
    0x07U, // 7
    0x7FU, // 8
    0x6FU  // 9
};

static const uint16_t digitEnablePins[MAX_LED] =
{
    GPIO_PIN_6, // index 0 → EN0 → PA6
    GPIO_PIN_7, // index 1 → EN1 → PA7
    GPIO_PIN_8, // index 2 → EN2 → PA8
    GPIO_PIN_9  // index 3 → EN3 → PA9
};

static uint8_t ledBuffer[MAX_LED] =
{
    1U, // Digit 0
    5U, // Digit 1
    0U, // Digit 2
    8U  // Digit 3
};

static void setTimer0(uint16_t duration)
{
    timer0_counter = duration / TIMER_CYCLE;
    timer0_flag = 0U;
}

static void setTimer1(uint16_t duration)
{
    timer1_counter = duration / TIMER_CYCLE;
    timer1_flag = 0U;
}

static void timer_run(void)
{
    if (timer0_counter > 0U)
    {
        timer0_counter--;

        if (timer0_counter == 0U)
        {
            timer0_flag = 1U;
        }
    }

    if (timer1_counter > 0U)
    {
        timer1_counter--;

        if (timer1_counter == 0U)
        {
            timer1_flag = 1U;
        }
    }

}

static void disableAllDigits(void)
{
    HAL_GPIO_WritePin(
        GPIOA,
        DIGIT_ALL_PINS,
        GPIO_PIN_SET
    );
}

static void display7SEG(uint8_t number)
{
    if (number > 9U)
    {
        return;
    }

    HAL_GPIO_WritePin(
        GPIOB,
        SEG_ALL_PINS,
        GPIO_PIN_SET
    );

    HAL_GPIO_WritePin(
        GPIOB,
        (uint16_t)segPattern[number],
        GPIO_PIN_RESET
    );
}

static void update7SEG(uint8_t index)
{
    if (index >= MAX_LED)
    {
        return;
    }

    disableAllDigits();
    display7SEG(ledBuffer[index]);
    HAL_GPIO_WritePin(
        GPIOA,
        digitEnablePins[index],
        GPIO_PIN_RESET
    );
}

static void updateClockBuffer(void)
{
    ledBuffer[0] = hour / 10U;
    ledBuffer[1] = hour % 10U;

    ledBuffer[2] = minute / 10U;
    ledBuffer[3] = minute % 10U;
}

static void increaseClockOneSecond(void)
{
    second++;

    if (second >= 60U)
    {
        second = 0U;
        minute++;
    }

    if (minute >= 60U)
    {
        minute = 0U;
        hour++;
    }

    if (hour >= 24U)
    {
        hour = 0U;
    }
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
  MX_TIM2_Init();
  /* USER CODE BEGIN 2 */
  disableAllDigits();

  HAL_GPIO_WritePin(
      GPIOA,
      GPIO_PIN_4,
      GPIO_PIN_SET
  );

  updateClockBuffer();

  index_led = 0U;
  update7SEG(index_led);

  HAL_GPIO_WritePin(
      GPIOA,
      MATRIX_EN_ALL_PINS,
      GPIO_PIN_SET
  );

  HAL_GPIO_WritePin(
      GPIOB,
      MATRIX_ROW_ALL_PINS,
      GPIO_PIN_SET
  );

  index_led_matrix = 0U;


  setTimer0(1000U);

  setTimer1(250U);


  HAL_TIM_Base_Start_IT(&htim2);
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
	    /*
	     * Clock và DOT.
	     */
	    if (timer0_flag == 1U)
	    {
	        increaseClockOneSecond();
	        updateClockBuffer();

	        HAL_GPIO_TogglePin(
	            GPIOA,
	            GPIO_PIN_4
	        );

	        setTimer0(1000U);
	    }

	    /*
	     * Quét bốn LED 7 đoạn.
	     */
	    if (timer1_flag == 1U)
	    {
	        index_led++;

	        if (index_led >= MAX_LED)
	        {
	            index_led = 0U;
	        }

	        update7SEG(index_led);
	        setTimer1(250U);
	    }

	    uint32_t currentTick = HAL_GetTick();

	    if ((currentTick - matrixLastTick) >= MATRIX_SCAN_TIME_MS)
	    {
	        matrixLastTick = currentTick;

	        updateLEDMatrix(index_led_matrix);

	        index_led_matrix++;

	        if (index_led_matrix >= MAX_LED_MATRIX)
	        {
	            index_led_matrix = 0U;
	        }
	    }

    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
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

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_NONE;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_HSI;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_0) != HAL_OK)
  {
    Error_Handler();
  }
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

  TIM_ClockConfigTypeDef sClockSourceConfig = {0};
  TIM_MasterConfigTypeDef sMasterConfig = {0};

  /* USER CODE BEGIN TIM2_Init 1 */

  /* USER CODE END TIM2_Init 1 */
  htim2.Instance = TIM2;
  htim2.Init.Prescaler = 7999;
  htim2.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim2.Init.Period = 9;
  htim2.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim2.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_Base_Init(&htim2) != HAL_OK)
  {
    Error_Handler();
  }
  sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
  if (HAL_TIM_ConfigClockSource(&htim2, &sClockSourceConfig) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim2, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM2_Init 2 */

  /* USER CODE END TIM2_Init 2 */

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
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOA, ENM0_Pin|ENM1_Pin|DOT_Pin|LED_Pin
                          |EN0_Pin|EN1_Pin|EN2_Pin|EN3_Pin
                          |ENM2_Pin|ENM3_Pin|ENM4_Pin|ENM5_Pin
                          |ENM6_Pin|ENM7_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOB, A_Pin|B_Pin|C_Pin|ROW2_Pin
                          |ROW3_Pin|ROW4_Pin|ROW5_Pin|ROW6_Pin
                          |ROW7_Pin|D_Pin|E_Pin|F_Pin
                          |G_Pin|ROW0_Pin|ROW1_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pins : ENM0_Pin ENM1_Pin DOT_Pin LED_Pin
                           EN0_Pin EN1_Pin EN2_Pin EN3_Pin
                           ENM2_Pin ENM3_Pin ENM4_Pin ENM5_Pin
                           ENM6_Pin ENM7_Pin */
  GPIO_InitStruct.Pin = ENM0_Pin|ENM1_Pin|DOT_Pin|LED_Pin
                          |EN0_Pin|EN1_Pin|EN2_Pin|EN3_Pin
                          |ENM2_Pin|ENM3_Pin|ENM4_Pin|ENM5_Pin
                          |ENM6_Pin|ENM7_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /*Configure GPIO pins : A_Pin B_Pin C_Pin ROW2_Pin
                           ROW3_Pin ROW4_Pin ROW5_Pin ROW6_Pin
                           ROW7_Pin D_Pin E_Pin F_Pin
                           G_Pin ROW0_Pin ROW1_Pin */
  GPIO_InitStruct.Pin = A_Pin|B_Pin|C_Pin|ROW2_Pin
                          |ROW3_Pin|ROW4_Pin|ROW5_Pin|ROW6_Pin
                          |ROW7_Pin|D_Pin|E_Pin|F_Pin
                          |G_Pin|ROW0_Pin|ROW1_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /* USER CODE BEGIN MX_GPIO_Init_2 */

  /* USER CODE END MX_GPIO_Init_2 */
}

/* USER CODE BEGIN 4 */
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
    if (htim->Instance == TIM2)
    {
        timer_run();
    }
}
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
