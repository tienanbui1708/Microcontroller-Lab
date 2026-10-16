/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
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

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32f1xx_hal.h"

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

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* USER CODE BEGIN EFP */

/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define LED_DEBUG_Pin GPIO_PIN_0
#define LED_DEBUG_GPIO_Port GPIOA
#define LED7SEG_a_Pin GPIO_PIN_1
#define LED7SEG_a_GPIO_Port GPIOA
#define LED7SEG_b_Pin GPIO_PIN_2
#define LED7SEG_b_GPIO_Port GPIOA
#define LED7SEG_c_Pin GPIO_PIN_3
#define LED7SEG_c_GPIO_Port GPIOA
#define LED7SEG_d_Pin GPIO_PIN_4
#define LED7SEG_d_GPIO_Port GPIOA
#define LED7SEG_e_Pin GPIO_PIN_5
#define LED7SEG_e_GPIO_Port GPIOA
#define LED7SEG_f_Pin GPIO_PIN_6
#define LED7SEG_f_GPIO_Port GPIOA
#define LED7SEG_g_Pin GPIO_PIN_7
#define LED7SEG_g_GPIO_Port GPIOA
#define BUTTON_0_Pin GPIO_PIN_0
#define BUTTON_0_GPIO_Port GPIOB
#define BUTTON_1_Pin GPIO_PIN_1
#define BUTTON_1_GPIO_Port GPIOB
#define BUTTON_2_Pin GPIO_PIN_2
#define BUTTON_2_GPIO_Port GPIOB
#define EN_0_Pin GPIO_PIN_8
#define EN_0_GPIO_Port GPIOA
#define EN_1_Pin GPIO_PIN_9
#define EN_1_GPIO_Port GPIOA
#define EN_2_Pin GPIO_PIN_10
#define EN_2_GPIO_Port GPIOA
#define EN_3_Pin GPIO_PIN_11
#define EN_3_GPIO_Port GPIOA
#define GREEN1_Pin GPIO_PIN_3
#define GREEN1_GPIO_Port GPIOB
#define AMBER1_Pin GPIO_PIN_4
#define AMBER1_GPIO_Port GPIOB
#define RED1_Pin GPIO_PIN_5
#define RED1_GPIO_Port GPIOB
#define GREEN2_Pin GPIO_PIN_6
#define GREEN2_GPIO_Port GPIOB
#define AMBER2_Pin GPIO_PIN_7
#define AMBER2_GPIO_Port GPIOB
#define RED2_Pin GPIO_PIN_8
#define RED2_GPIO_Port GPIOB

/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
