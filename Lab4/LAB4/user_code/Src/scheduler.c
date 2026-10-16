/*
 * scheduler.c
 *
 * Delta-queue cooperative scheduler.
 *
 * - The queue is a singly linked list over a static pool, sorted by due
 *   time. delay of the head = ticks until it is due; delay of every other
 *   node = ticks after the node before it.
 * - SCH_Update (ISR) only decrements the head: O(1). If the head is already
 *   due and the main loop has not run it yet, the tick is counted in
 *   late_ticks so no time is lost for the tasks behind it.
 * - SCH_Dispatch_Tasks (main loop) removes due tasks one at a time, puts
 *   periodic tasks back with delay = period, then runs the task with
 *   interrupts enabled. Tasks due at the same tick run in registration
 *   order (SCH_Add_Task call order), including periodic tasks put back.
 * - A task ID is (generation << 8) | slot, so a stale ID cannot delete a
 *   task that later reused the same slot.
 */

#include "scheduler.h"
#include "main.h"

#define SCH_NONE 0xFFu

typedef struct {
	void     (*pTask)(void);
	uint32_t delay;    /* ticks after the previous node (head: ticks from now) */
	uint32_t period;   /* ticks, 0 = one-shot */
	uint32_t id;
	uint32_t seq;      /* registration order, breaks ties between equal due times */
	uint8_t  next;
	uint8_t  used;
} sTask;

static sTask    tasks[SCH_MAX_TASKS];
static uint8_t  head = SCH_NONE;
static uint8_t  generation[SCH_MAX_TASKS];
static uint32_t next_seq;
static volatile uint32_t late_ticks;
static volatile uint32_t time_ticks;

static uint32_t lock(void) {
	uint32_t primask = __get_PRIMASK();
	__disable_irq();
	return primask;
}

static void unlock(uint32_t primask) {
	__set_PRIMASK(primask);
}

/* Link a used slot into the queue. Caller holds the lock. */
static void insert(uint8_t slot, uint32_t delay) {
	uint8_t prev = SCH_NONE;
	uint8_t cur = head;

	/* Tasks due at the same tick stay in registration order, also after a
	 * periodic task is put back, so T1 always runs before T2 at 1000 ms. */
	while (cur != SCH_NONE && (tasks[cur].delay < delay
			|| (tasks[cur].delay == delay && tasks[cur].seq < tasks[slot].seq))) {
		delay -= tasks[cur].delay;
		prev = cur;
		cur = tasks[cur].next;
	}

	tasks[slot].delay = delay;
	tasks[slot].next = cur;
	if (cur != SCH_NONE) {
		tasks[cur].delay -= delay;
	}
	if (prev == SCH_NONE) {
		head = slot;
	} else {
		tasks[prev].next = slot;
	}
}

/* Hand ticks that arrived while the head was due to the tasks behind it. */
static void apply_late_ticks(void) {
	while (late_ticks > 0 && head != SCH_NONE && tasks[head].delay > 0) {
		uint32_t step = late_ticks < tasks[head].delay ? late_ticks : tasks[head].delay;
		tasks[head].delay -= step;
		late_ticks -= step;
	}
	if (head == SCH_NONE) {
		late_ticks = 0;
	}
}

void SCH_Init(void) {
	uint32_t primask = lock();
	for (uint8_t i = 0; i < SCH_MAX_TASKS; ++i) {
		tasks[i].used = 0;
		tasks[i].pTask = 0;
	}
	head = SCH_NONE;
	next_seq = 0;
	late_ticks = 0;
	time_ticks = 0;
	unlock(primask);
}

void SCH_Update(void) {
	++time_ticks;
	if (head == SCH_NONE) {
		return;
	}
	if (tasks[head].delay > 0) {
		--tasks[head].delay;
	} else {
		++late_ticks;
	}
}

uint32_t SCH_Add_Task(void (*pFunction)(void), uint32_t DELAY, uint32_t PERIOD) {
	if (pFunction == 0) {
		return SCH_NO_TASK_ID;
	}

	uint32_t primask = lock();
	uint8_t slot = 0;
	while (slot < SCH_MAX_TASKS && tasks[slot].used) {
		++slot;
	}
	if (slot == SCH_MAX_TASKS) {
		unlock(primask);
		return SCH_NO_TASK_ID;
	}

	++generation[slot];
	tasks[slot].used = 1;
	tasks[slot].pTask = pFunction;
	tasks[slot].period = PERIOD / SCH_TICK_MS;
	tasks[slot].id = ((uint32_t)generation[slot] << 8) | slot;
	tasks[slot].seq = next_seq++;
	insert(slot, DELAY / SCH_TICK_MS);

	uint32_t id = tasks[slot].id;
	unlock(primask);
	return id;
}

uint8_t SCH_Delete_Task(uint32_t taskID) {
	uint8_t slot = (uint8_t)(taskID & 0xFFu);
	if (slot >= SCH_MAX_TASKS) {
		return 0;
	}

	uint32_t primask = lock();
	if (!tasks[slot].used || tasks[slot].id != taskID) {
		unlock(primask);
		return 0;
	}

	uint8_t prev = SCH_NONE;
	uint8_t cur = head;
	while (cur != slot) {
		prev = cur;
		cur = tasks[cur].next;
	}

	uint8_t next = tasks[slot].next;
	if (next != SCH_NONE) {
		tasks[next].delay += tasks[slot].delay;
	}
	if (prev == SCH_NONE) {
		head = next;
	} else {
		tasks[prev].next = next;
	}
	tasks[slot].used = 0;
	tasks[slot].pTask = 0;
	if (prev == SCH_NONE) {
		apply_late_ticks();
	}
	unlock(primask);
	return 1;
}

void SCH_Dispatch_Tasks(void) {
	for (;;) {
		uint32_t primask = lock();
		if (head == SCH_NONE || tasks[head].delay > 0) {
			unlock(primask);
			/* Sleep until the next interrupt. WFI runs with interrupts
			 * enabled: the Proteus CM3 model does not wake from WFI while
			 * PRIMASK is set. A tick landing just before WFI is picked up
			 * at the next HAL SysTick (1 ms), so tasks run < 1 ms late. */
			__WFI();
			return;
		}

		uint8_t slot = head;
		void (*pTask)(void) = tasks[slot].pTask;
		head = tasks[slot].next;

		if (tasks[slot].period > 0) {
			insert(slot, tasks[slot].period);
		} else {
			tasks[slot].used = 0;
			tasks[slot].pTask = 0;
		}
		apply_late_ticks();
		unlock(primask);

		pTask();
	}
}

uint32_t SCH_Get_Time(void) {
	return time_ticks * SCH_TICK_MS;
}
