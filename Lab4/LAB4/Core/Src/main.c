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
#include "tim.h"
#include "usart.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include <stdio.h>
#include "scheduler.h"
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
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
/* Print "[    1500 ms] T3 1.5s" so each callback shows when it really ran. */
static void log_task(const char *name) {
	char buf[72];
	int len = snprintf(buf, sizeof(buf), "[%8lu ms] %s\r\n",
			(unsigned long)SCH_Get_Time(), name);
	HAL_UART_Transmit(&huart1, (uint8_t *)buf, len, 10);
}

/* Each task toggles its own LED, so LED_Tn blinks with half its task rate. */
static void task_0_5s(void) {
	HAL_GPIO_TogglePin(LED_T1_GPIO_Port, LED_T1_Pin);
	log_task("T1 0.5s");
}

static void task_1s(void) {
	HAL_GPIO_TogglePin(LED_T2_GPIO_Port, LED_T2_Pin);
	log_task("T2 1s");
}

static void task_1_5s(void) {
	HAL_GPIO_TogglePin(LED_T3_GPIO_Port, LED_T3_Pin);
	log_task("T3 1.5s");
}

static void task_2s(void) {
	HAL_GPIO_TogglePin(LED_T4_GPIO_Port, LED_T4_Pin);
	log_task("T4 2s");
}

static void task_2_5s(void) {
	HAL_GPIO_TogglePin(LED_T5_GPIO_Port, LED_T5_Pin);
	log_task("T5 2.5s");
}

/* Demo: a 10 ms timeout printing get_time, removed by ID after 100 ms. */
static uint32_t tick_10ms_id = SCH_NO_TASK_ID;

static void task_10ms(void) {
	log_task("tick 10ms");
}

static void stop_10ms(void) {
	char msg[56];
	uint8_t first = SCH_Delete_Task(tick_10ms_id);
	uint8_t again = SCH_Delete_Task(tick_10ms_id);   /* stale ID -> 0 */
	snprintf(msg, sizeof(msg), "delete ID 0x%04lX -> %u, again -> %u",
			(unsigned long)tick_10ms_id, first, again);
	log_task(msg);
}

/* Demo: one-shot timeouts set up before the main loop. */
static void once_250ms(void) {
	log_task("one-shot 250 ms");
}

static void once_1250ms(void) {
	log_task("one-shot 1250 ms");
}

static void once_3330ms(void) {
	log_task("one-shot 3330 ms");
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
  MX_USART1_UART_Init();
  /* USER CODE BEGIN 2 */
  SCH_Init();
  HAL_TIM_Base_Start_IT(&htim2);
  /* DELAY = PERIOD: first run after one period, then every period (ms). */
  SCH_Add_Task(task_0_5s, 500, 500);
  SCH_Add_Task(task_1s, 1000, 1000);
  SCH_Add_Task(task_1_5s, 1500, 1500);
  SCH_Add_Task(task_2s, 2000, 2000);
  SCH_Add_Task(task_2_5s, 2500, 2500);

  /* 10 ms timeout: runs at 10, 20, ..., 100 ms. stop_10ms is due at 100 ms
   * too but registered later, so the last tick prints before the delete. */
  tick_10ms_id = SCH_Add_Task(task_10ms, 10, 10);
  SCH_Add_Task(stop_10ms, 100, 0);

  /* One-shots (PERIOD = 0); all fire long after the main loop starts. */
  SCH_Add_Task(once_250ms, 250, 0);
  SCH_Add_Task(once_1250ms, 1250, 0);
  SCH_Add_Task(once_3330ms, 3330, 0);

  log_task("Lab 4 scheduler demo start");
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
    SCH_Dispatch_Tasks();
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

/* USER CODE BEGIN 4 */
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim) {
	if (htim->Instance == TIM2) {
		SCH_Update();
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
