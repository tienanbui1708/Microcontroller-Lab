/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    gpio.c
  * @brief   This file provides code for the configuration
  *          of all used GPIO pins.
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
#include "gpio.h"

/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/*----------------------------------------------------------------------------*/
/* Configure GPIO                                                             */
/*----------------------------------------------------------------------------*/
/* USER CODE BEGIN 1 */

/* USER CODE END 1 */

/** Configure pins
*/
void MX_GPIO_Init(void)
{

  GPIO_InitTypeDef GPIO_InitStruct = {0};

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOA, LED_DEBUG_Pin|LED7SEG_a_Pin|LED7SEG_b_Pin|LED7SEG_c_Pin
                          |LED7SEG_d_Pin|LED7SEG_e_Pin|LED7SEG_f_Pin|LED7SEG_g_Pin
                          |EN_0_Pin|EN_3_Pin|EN_1_Pin|EN_2_Pin, GPIO_PIN_SET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOB, LED_T2_Pin|LED_T3_Pin|LED_T4_Pin|LED_T5_Pin
                          |GREEN1_Pin|AMBER1_Pin|RED1_Pin|GREEN2_Pin
                          |AMBER2_Pin|RED2_Pin|LED_T1_Pin, GPIO_PIN_SET);

  /*Configure GPIO pins : LED_DEBUG_Pin LED7SEG_a_Pin LED7SEG_b_Pin LED7SEG_c_Pin
                           LED7SEG_d_Pin LED7SEG_e_Pin LED7SEG_f_Pin LED7SEG_g_Pin
                           EN_0_Pin EN_3_Pin EN_1_Pin EN_2_Pin */
  GPIO_InitStruct.Pin = LED_DEBUG_Pin|LED7SEG_a_Pin|LED7SEG_b_Pin|LED7SEG_c_Pin
                          |LED7SEG_d_Pin|LED7SEG_e_Pin|LED7SEG_f_Pin|LED7SEG_g_Pin
                          |EN_0_Pin|EN_3_Pin|EN_1_Pin|EN_2_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /*Configure GPIO pins : BUTTON_0_Pin BUTTON_1_Pin BUTTON_2_Pin */
  GPIO_InitStruct.Pin = BUTTON_0_Pin|BUTTON_1_Pin|BUTTON_2_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /*Configure GPIO pins : LED_T2_Pin LED_T3_Pin LED_T4_Pin LED_T5_Pin
                           GREEN1_Pin AMBER1_Pin RED1_Pin GREEN2_Pin
                           AMBER2_Pin RED2_Pin LED_T1_Pin */
  GPIO_InitStruct.Pin = LED_T2_Pin|LED_T3_Pin|LED_T4_Pin|LED_T5_Pin
                          |GREEN1_Pin|AMBER1_Pin|RED1_Pin|GREEN2_Pin
                          |AMBER2_Pin|RED2_Pin|LED_T1_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

}

/* USER CODE BEGIN 2 */

/* USER CODE END 2 */
