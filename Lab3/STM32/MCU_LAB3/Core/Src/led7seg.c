/*
 * led7seg.c
 * Created on: 17/09/26
 * Author: TienAn
 */
#include "led7seg.h"
#include "main.h"

#define NUMBER_OF_LED7SEG 	4

static uint8_t led7seg_map_of_number[] = {0x40, 0x79, 0x24, 0x30, 0x19, 0x12, 0x02, 0x78, 0x00, 0x10};
static uint8_t led7seg_map_of_index[] = {0x01, 0x02, 0x04, 0x08};
static uint8_t led7seg_index = 0;
static uint8_t led_7seg[NUMBER_OF_LED7SEG] = {0, 0, 0, 0};

// Hardware function
void _led7seg_display(int num) {
	HAL_GPIO_WritePin(LED7SEG_a_GPIO_Port, LED7SEG_a_Pin, !((~led7seg_map_of_number[num]) & (1 << 0)));
	HAL_GPIO_WritePin(LED7SEG_b_GPIO_Port, LED7SEG_b_Pin, !((~led7seg_map_of_number[num]) & (1 << 1)));
	HAL_GPIO_WritePin(LED7SEG_c_GPIO_Port, LED7SEG_c_Pin, !((~led7seg_map_of_number[num]) & (1 << 2)));
	HAL_GPIO_WritePin(LED7SEG_d_GPIO_Port, LED7SEG_d_Pin, !((~led7seg_map_of_number[num]) & (1 << 3)));
	HAL_GPIO_WritePin(LED7SEG_e_GPIO_Port, LED7SEG_e_Pin, !((~led7seg_map_of_number[num]) & (1 << 4)));
	HAL_GPIO_WritePin(LED7SEG_f_GPIO_Port, LED7SEG_f_Pin, !((~led7seg_map_of_number[num]) & (1 << 5)));
	HAL_GPIO_WritePin(LED7SEG_g_GPIO_Port, LED7SEG_g_Pin, !((~led7seg_map_of_number[num]) & (1 << 6)));
}

void _led7seg_en(int num) {
	HAL_GPIO_WritePin(EN_3_GPIO_Port, EN_3_Pin, !((led7seg_map_of_index[num]) & (1 << 3)));
	HAL_GPIO_WritePin(EN_2_GPIO_Port, EN_2_Pin, !((led7seg_map_of_index[num]) & (1 << 2)));
	HAL_GPIO_WritePin(EN_1_GPIO_Port, EN_1_Pin, !((led7seg_map_of_index[num]) & (1 << 1)));
	HAL_GPIO_WritePin(EN_0_GPIO_Port, EN_0_Pin, !((led7seg_map_of_index[num]) & (1 << 0)));
}

// APIs
void led7seg_scan() {
	HAL_GPIO_WritePin(EN_3_GPIO_Port, EN_3_Pin, 1);
	HAL_GPIO_WritePin(EN_2_GPIO_Port, EN_2_Pin, 1);
	HAL_GPIO_WritePin(EN_1_GPIO_Port, EN_1_Pin, 1);
	HAL_GPIO_WritePin(EN_0_GPIO_Port, EN_0_Pin, 1);

	_led7seg_display(led_7seg[led7seg_index]);
	_led7seg_en(led7seg_index);
	++led7seg_index;
	if (led7seg_index >= NUMBER_OF_LED7SEG) {
		led7seg_index = 0;
	}
}

void led7seg_set(uint8_t index, uint8_t number) {
	if (index >= NUMBER_OF_LED7SEG || number > 9) return;
	led_7seg[index] = number;
}
