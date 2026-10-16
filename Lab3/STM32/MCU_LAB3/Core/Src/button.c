/*
 * button.c
 * Created on: 17/09/26
 * Author: TienAn
 * Independent debounced edge events.
 */
#include "button.h"
typedef struct {
    GPIO_TypeDef *port;
    uint16_t pin;
    uint8_t candidate, stable, matches, pressed_event;
    uint32_t held_ms;
} Button;
static Button buttons[NUMBER_OF_BUTTONS] = {
    {BUTTON_0_GPIO_Port, BUTTON_0_Pin, 1, 1, 0, 0, 0},
    {BUTTON_1_GPIO_Port, BUTTON_1_Pin, 1, 1, 0, 0, 0},
    {BUTTON_2_GPIO_Port, BUTTON_2_Pin, 1, 1, 0, 0, 0}
};
void button_init(void) {
    for (unsigned i = 0; i < NUMBER_OF_BUTTONS; ++i) {
        buttons[i].candidate = buttons[i].stable = 1;
        buttons[i].matches = buttons[i].pressed_event = 0;
        buttons[i].held_ms = 0;
    }
}
/* Call once per BUTTON_SCAN_MS of real time, never replay missed samples. */
void button_scan(void) {
    for (unsigned i = 0; i < NUMBER_OF_BUTTONS; ++i) {
        Button *b = &buttons[i];
        uint8_t raw = (uint8_t)HAL_GPIO_ReadPin(b->port, b->pin);
        if (raw != b->candidate) {
            b->candidate = raw;
            b->matches = 1;
        } else if (b->matches < BUTTON_DEBOUNCE_SAMPLES) {
            ++b->matches;
        }
        if (b->matches == BUTTON_DEBOUNCE_SAMPLES && b->stable != raw) {
            b->stable = raw;
            b->held_ms = 0;
            if (raw == 0) b->pressed_event = 1;
        } else if (b->stable == 0 && raw == 0 && b->held_ms < UINT32_MAX - BUTTON_SCAN_MS) {
            b->held_ms += BUTTON_SCAN_MS;
        }
        /* No early return: sample every button on every scan. */
    }
}
uint8_t button_is_pressed(uint8_t index) {
    if (index >= NUMBER_OF_BUTTONS) return 0;
    uint8_t event = buttons[index].pressed_event;
    buttons[index].pressed_event = 0;
    return event;
}
uint8_t button_is_held(uint8_t index, int duration) {
    if (index >= NUMBER_OF_BUTTONS || duration < 0) return 0;
    return buttons[index].stable == 0 && buttons[index].held_ms >= (uint32_t)duration;
}
