/*
 * scheduler.h
 *
 * Cooperative scheduler for Lab 4.
 * Tasks are kept in a delta queue sorted by due time: each entry stores
 * the number of ticks after the entry before it. SCH_Update() therefore
 * only touches the head of the queue (O(1)) inside the timer ISR.
 */

#ifndef INC_SCHEDULER_H_
#define INC_SCHEDULER_H_

#include <stdint.h>

#define SCH_TICK_MS     10u   /* TIM2 interrupt period */
#define SCH_MAX_TASKS   16u
#define SCH_NO_TASK_ID  0xFFFFFFFFu

void     SCH_Init(void);
void     SCH_Update(void);           /* call from the TIM2 interrupt */
void     SCH_Dispatch_Tasks(void);   /* call from the main loop */

/* DELAY and PERIOD are in ms, rounded down to whole ticks.
 * PERIOD below SCH_TICK_MS (e.g. 0) runs the task once.
 * Returns SCH_NO_TASK_ID when the queue is full. */
uint32_t SCH_Add_Task(void (*pFunction)(void), uint32_t DELAY, uint32_t PERIOD);

/* Returns 1 when the task was removed, 0 when the ID is unknown. */
uint8_t  SCH_Delete_Task(uint32_t taskID);

/* Time since SCH_Init in ms, accurate to one tick. */
uint32_t SCH_Get_Time(void);

#endif /* INC_SCHEDULER_H_ */
