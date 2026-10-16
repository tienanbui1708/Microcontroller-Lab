/*
 * software_timer.c
 * Created on: 17/09/26
 * Author: TienAn
 * Millisecond-based timers with phase carry.
 */
#include "software_timer.h"
#include "tim.h"
typedef struct {
    int32_t remaining_ms;
    uint32_t period_ms, pending;
} SoftwareTimer;
static volatile SoftwareTimer timers[NUMBER_OF_TIMERS];
volatile uint32_t timer_interrupts;
void timer_init(void) {
    __HAL_TIM_CLEAR_FLAG(&htim2, TIM_FLAG_UPDATE);
    if (HAL_TIM_Base_Start_IT(&htim2) != HAL_OK) Error_Handler();
}
void timer_set(int index, int duration_ms) {
    if (index < 0 || index >= (int)NUMBER_OF_TIMERS || duration_ms <= 0) return;
    uint32_t saved = __get_PRIMASK();
    __disable_irq();
    timers[index].period_ms = (uint32_t)duration_ms;
    timers[index].remaining_ms = duration_ms;
    timers[index].pending = 0;
    __set_PRIMASK(saved);
}
/* Called only by TIM2: no truncation of duration_ms / TIMER_PERIOD. */
void timer_run(void) {
    for (unsigned i = 0; i < NUMBER_OF_TIMERS; ++i) {
        if (timers[i].period_ms == 0) continue;
        timers[i].remaining_ms -= (int32_t)TIMER_PERIOD;
        while (timers[i].remaining_ms <= 0) {
            timers[i].remaining_ms += (int32_t)timers[i].period_ms;
            if (timers[i].pending != UINT32_MAX) ++timers[i].pending;
        }
    }
}
int timer_is_expired(int index) {
    if (index < 0 || index >= (int)NUMBER_OF_TIMERS) return 0;
    uint32_t saved = __get_PRIMASK();
    __disable_irq();
    int expired = timers[index].pending != 0;
    if (expired) --timers[index].pending;
    __set_PRIMASK(saved);
    return expired;
}
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim) {
    if (htim->Instance == TIM2) {
        ++timer_interrupts;
        timer_run();
    }
}
