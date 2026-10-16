/*
 * test.c
 *
 *  Created on: 17/09/26
 *      Author: TienAn
 */

#include "test.h"
#include "global.h"
#include "main.h"
#include "gpio.h"
#include "software_timer.h"
#include "button.h"
#include "led7seg.h"

static uint8_t led_blinking = 0;

void test_toggle_led_debug() {
	HAL_GPIO_TogglePin(LED_DEBUG_GPIO_Port, LED_DEBUG_Pin);
	HAL_Delay(500);
}

void test_button() {
	if (button_is_pressed(0)) {
		HAL_GPIO_TogglePin(LED_DEBUG_GPIO_Port, LED_DEBUG_Pin);
	}

	if (button_is_pressed(1)) {
		HAL_GPIO_TogglePin(LED_DEBUG_GPIO_Port, LED_DEBUG_Pin);
	}

	if (button_is_pressed(2)) {
		HAL_GPIO_TogglePin(LED_DEBUG_GPIO_Port, LED_DEBUG_Pin);
	}

	if (button_is_held(0, 250)) {
		led_blinking = 1;
	}
	else {
		led_blinking = 0;
	}

	if (timer_is_expired(TEST_BUTTON)) {
		if (led_blinking) {
			HAL_GPIO_TogglePin(LED_DEBUG_GPIO_Port, LED_DEBUG_Pin);
		}
	}
}

void test_led7seg() {
	static int counter = 0;
	if (timer_is_expired(TEST_LED7SEG)) {
		led7seg_set(0, 1);
		led7seg_set(1, 2);
		led7seg_set(2, 3);
		led7seg_set(3, counter++);
		if (counter == 10) {
			counter = 0;
		}
	}
}
