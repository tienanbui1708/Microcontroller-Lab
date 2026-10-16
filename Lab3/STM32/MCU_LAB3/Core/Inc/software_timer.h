/*
 * software_timer.h
 * Created on: 17/09/26
 * Author: TienAn
 */
#ifndef INC_SOFTWARE_TIMER_H_
#define INC_SOFTWARE_TIMER_H_
#include <stdint.h>
#ifndef TIMER_PERIOD
#define TIMER_PERIOD 10U /* TIM2 milliseconds: single source of configuration. */
#endif
#if TIMER_PERIOD < 1 || TIMER_PERIOD > 100
#error TIMER_PERIOD must be between 1 and 100 ms
#endif
#define NUMBER_OF_TIMERS 5U
extern volatile uint32_t timer_interrupts;
void timer_init(void);
void timer_set(int index, int duration_ms);
void timer_run(void);
int timer_is_expired(int index);
#endif
