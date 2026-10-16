/*
 * button.h
 * Created on: 17/09/26
 * Author: TienAn
 */
#ifndef INC_BUTTON_H_
#define INC_BUTTON_H_
#include "main.h"
#define NUMBER_OF_BUTTONS 3U
#define BUTTON_SCAN_MS 10U
#define BUTTON_DEBOUNCE_SAMPLES 3U
void button_init(void);
void button_scan(void);
uint8_t button_is_pressed(uint8_t index);
uint8_t button_is_held(uint8_t index, int duration);
#endif
