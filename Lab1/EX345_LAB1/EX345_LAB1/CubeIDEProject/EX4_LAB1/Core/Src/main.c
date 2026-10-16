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

/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
//bit0=a, bit1=b, ... bit6=g
static const uint8_t segCode[10] = {
		  /*0*/ 0b1000000,  // abcdef ON, g OFF
		  /*1*/ 0b1111001,  // b c
		  /*2*/ 0b0100100,  // a b d e g
		  /*3*/ 0b0110000,  // a b c d g
		  /*4*/ 0b0011001,  // b c f g
		  /*5*/ 0b0010010,  // a c d f g
		  /*6*/ 0b0000010,  // a c d e f g
		  /*7*/ 0b1111000,  // a b c
		  /*8*/ 0b0000000,  // all
		  /*9*/ 0b0010000   // a b c d f g
};
// Mapping GPIO Pins a..g to PBx pins in Proteus.
// a->PB0, b->PB1, ..., g->PB6 in CubeIDE GPIO Pins Configure
static const uint16_t segPins[7] = {
  GPIO_PIN_0, // a -> PB0
  GPIO_PIN_1, // b -> PB1
  GPIO_PIN_2, // c -> PB2
  GPIO_PIN_3, // d -> PB3
  GPIO_PIN_4, // e -> PB4
  GPIO_PIN_5, // f -> PB5
  GPIO_PIN_6  // g -> PB6
};

// 0-9 numbers on 7-seg COM-ANODE (0=ON, 1=OFF)
void display7SEG(uint8_t num)
{
  if (num > 9) return;
  uint8_t code = segCode[num];

  for (int i = 0; i < 7; ++i) {
    /* bit = 1 (OFF) -> SET (HIGH); bit = 0 (ON) -> RESET (LOW) */
    GPIO_PinState st = (code & (1U << i)) ? GPIO_PIN_SET : GPIO_PIN_RESET;
    HAL_GPIO_WritePin(GPIOB, segPins[i], st);
  }
}

static void test7SEG(void)
{
  /* Exercise 4: show every digit once before the traffic cycle starts. */
  for (uint8_t digit = 0; digit < 10U; ++digit) {
    display7SEG(digit);
    HAL_Delay(1000);
  }
}

static void setTrafficLights(uint16_t horizontalPin, uint16_t verticalPin)
{
  const uint16_t horizontalPins = Hz_Green_Pin | Hz_Yellow_Pin | Hz_Red_Pin;
  const uint16_t verticalPins = Vt_Green_Pin | Vt_Yellow_Pin | Vt_Red_Pin;

  /* Turn everything off first, then enable exactly one light per direction. */
  HAL_GPIO_WritePin(GPIOA, horizontalPins | verticalPins, GPIO_PIN_RESET);
  HAL_GPIO_WritePin(GPIOA, horizontalPin | verticalPin, GPIO_PIN_SET);
}

static void countdown(uint8_t seconds)
{
  for (uint8_t value = seconds; value > 0U; --value) {
    display7SEG(value);
    HAL_Delay(1000);
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
  /* USER CODE BEGIN 2 */
  test7SEG();
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
	  /* Exercises 3 and 5: safe two-direction traffic-light cycle. */
	  setTrafficLights(Hz_Green_Pin, Vt_Red_Pin);
	  countdown(3);

	  setTrafficLights(Hz_Yellow_Pin, Vt_Red_Pin);
	  countdown(2);

	  setTrafficLights(Hz_Red_Pin, Vt_Green_Pin);
	  countdown(3);

	  setTrafficLights(Hz_Red_Pin, Vt_Yellow_Pin);
	  countdown(2);


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
  HAL_GPIO_WritePin(GPIOA, Hz_Green_Pin|Hz_Yellow_Pin|Hz_Red_Pin|Vt_Green_Pin
                          |Vt_Yellow_Pin|Vt_Red_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOB, A_Pin|B_Pin|C_Pin|D_Pin
                          |E_Pin|F_Pin|G_Pin, GPIO_PIN_SET); //RESET -> COM_ANODE : HIGH = OFF

  /*Configure GPIO pins : Hz_Green_Pin Hz_Yellow_Pin Hz_Red_Pin Vt_Green_Pin
                           Vt_Yellow_Pin Vt_Red_Pin */
  GPIO_InitStruct.Pin = Hz_Green_Pin|Hz_Yellow_Pin|Hz_Red_Pin|Vt_Green_Pin
                          |Vt_Yellow_Pin|Vt_Red_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /*Configure GPIO pins : A_Pin B_Pin C_Pin D_Pin
                           E_Pin F_Pin G_Pin */
  GPIO_InitStruct.Pin = A_Pin|B_Pin|C_Pin|D_Pin
                          |E_Pin|F_Pin|G_Pin;
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
