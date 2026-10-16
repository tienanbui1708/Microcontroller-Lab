/*
 * app.c
 * Created on: 17/09/26
 * Author: TienAn
 * Four traffic phases and mode/value displays.
 * Reworked: one-shot buttons, 2 Hz blink, explicit drafts and safe timer restart.
 */
#include "app.h"
#include "main.h"
#include "button.h"
#include "led7seg.h"
#include "software_timer.h"

enum { TIMER_ONE_SECOND, TIMER_LED_BLINKING };
enum { BUTTON_SELECT_MODE, BUTTON_INCREASE, BUTTON_SET };
typedef enum { RED_GREEN, RED_AMBER, GREEN_RED, AMBER_RED } TrafficPhase;
typedef enum { GREEN, AMBER, RED } Color;
typedef struct { uint8_t red, amber, green; } Durations;
static Durations active = {5, 2, 3}, saved = {5, 2, 3};
static AppMode mode = NORMAL;
static TrafficPhase phase = RED_GREEN;
static uint8_t edit_value, countdown1, countdown2, blink_on, config_error;
volatile AppDebug app_debug;

static void display_pair(uint8_t road, uint8_t value) {
    led7seg_set(road * 2U, value / 10U);
    led7seg_set(road * 2U + 1U, value % 10U);
}
static void lights_off(void) {
    HAL_GPIO_WritePin(GPIOB, GREEN1_Pin | AMBER1_Pin | RED1_Pin |
                     GREEN2_Pin | AMBER2_Pin | RED2_Pin, GPIO_PIN_SET);
}
static void light_set(uint8_t road, Color color) {
    static const uint16_t pins[2][3] = {
        {GREEN1_Pin, AMBER1_Pin, RED1_Pin},
        {GREEN2_Pin, AMBER2_Pin, RED2_Pin}
    };
    HAL_GPIO_WritePin(GPIOB, pins[road][color], GPIO_PIN_RESET);
}
static void traffic_outputs(void) {
    lights_off();
    switch (phase) {
    case RED_GREEN: light_set(0, RED); light_set(1, GREEN); break;
    case RED_AMBER: light_set(0, RED); light_set(1, AMBER); break;
    case GREEN_RED: light_set(0, GREEN); light_set(1, RED); break;
    case AMBER_RED: light_set(0, AMBER); light_set(1, RED); break;
    }
    display_pair(0, countdown1);
    display_pair(1, countdown2);
}
static void start_normal(void) {
    mode = NORMAL;
    phase = RED_GREEN;
    countdown1 = active.red;
    countdown2 = active.green;
    timer_set(TIMER_ONE_SECOND, 1000);
    traffic_outputs();
}
static uint8_t saved_value(void) {
    if (mode == SET_RED) return saved.red;
    if (mode == SET_AMBER) return saved.amber;
    return saved.green;
}
static void publish_debug(void) {
    app_debug.mode = mode;
    app_debug.edit_value = edit_value;
    app_debug.active_red = active.red;
    app_debug.active_amber = active.amber;
    app_debug.active_green = active.green;
    app_debug.saved_red = saved.red;
    app_debug.saved_amber = saved.amber;
    app_debug.saved_green = saved.green;
    app_debug.config_error = config_error;
    /* Active-low debug LED: ON means rejected inconsistent timing set. */
    HAL_GPIO_WritePin(LED_DEBUG_GPIO_Port, LED_DEBUG_Pin,
                     config_error ? GPIO_PIN_RESET : GPIO_PIN_SET);
}

// REPORT_EX6_BEGIN
static void edit_outputs(void) {
    lights_off();
    if (blink_on) {
        Color color = (mode == SET_RED) ? RED : ((mode == SET_AMBER) ? AMBER : GREEN);
        light_set(0, color);
        light_set(1, color);
    }
    display_pair(0, edit_value);
    display_pair(1, (uint8_t)mode); /* 02, 03 or 04 */
}
static void enter_edit(AppMode next) {
    mode = next;
    edit_value = saved_value();
    blink_on = 1;
    timer_set(TIMER_LED_BLINKING, 250); /* toggle every 250 ms = 2 Hz */
    edit_outputs();
}
// REPORT_EX6_END

// REPORT_SHARED_EDIT_BEGIN
static void increase_edit(void) {
    edit_value = (edit_value == 99U) ? 1U : edit_value + 1U;
}
/* BTN2 confirms the edited colour. Unsaved changes are discarded on mode change. */
static void save_edit(void) {
    switch (mode) {
    // REPORT_EX7_BEGIN
    case SET_RED:
        saved.red = edit_value;
        break;
    // REPORT_EX7_END
    // REPORT_EX8_BEGIN
    case SET_AMBER:
        saved.amber = edit_value;
        break;
    // REPORT_EX8_END
    // REPORT_EX9_BEGIN
    case SET_GREEN:
        saved.green = edit_value;
        break;
    // REPORT_EX9_END
    default: break;
    }
}
// REPORT_SHARED_EDIT_END

// REPORT_VALIDATION_BEGIN
static uint8_t valid_durations(Durations value) {
    return value.red >= 1U && value.red <= 99U &&
           value.amber >= 1U && value.amber <= 99U &&
           value.green >= 1U && value.green <= 99U &&
           value.red == (uint16_t)value.amber + value.green;
}
static void next_mode(void) {
    switch (mode) {
    case NORMAL: enter_edit(SET_RED); break;
    case SET_RED: enter_edit(SET_AMBER); break;
    case SET_AMBER: enter_edit(SET_GREEN); break;
    case SET_GREEN:
        config_error = !valid_durations(saved);
        if (!config_error) active = saved;
        /* Invalid drafts stay available for correction. Old active timings remain. */
        start_normal();
        break;
    }
}
// REPORT_VALIDATION_END

// REPORT_TRAFFIC_BEGIN
static void traffic_second(void) {
    if (countdown1 > 0) --countdown1;
    if (countdown2 > 0) --countdown2;
    switch (phase) {
    case RED_GREEN:
        if (countdown2 == 0) {
            phase = RED_AMBER;
            countdown2 = active.amber;
        }
        break;
    case RED_AMBER:
        if (countdown2 == 0) {
            phase = GREEN_RED;
            countdown1 = active.green;
            countdown2 = active.red;
        }
        break;
    case GREEN_RED:
        if (countdown1 == 0) {
            phase = AMBER_RED;
            countdown1 = active.amber;
        }
        break;
    case AMBER_RED:
        if (countdown1 == 0) {
            phase = RED_GREEN;
            countdown1 = active.red;
            countdown2 = active.green;
        }
        break;
    }
    traffic_outputs();
}
// REPORT_TRAFFIC_END

void app_Set_timer(void) {
    active = (Durations){5, 2, 3};
    saved = active;
    edit_value = config_error = 0;
    start_normal();
    publish_debug();
}

// REPORT_EVENTS_BEGIN
void app_Select_mode(void) {
    /* Consume each event once, including buttons ignored in the current mode. */
    uint8_t select = button_is_pressed(BUTTON_SELECT_MODE);
    uint8_t increase = button_is_pressed(BUTTON_INCREASE);
    uint8_t confirm = button_is_pressed(BUTTON_SET);
    if (select) {
        next_mode(); /* Mode selection wins if buttons are pressed simultaneously. */
    } else if (mode == NORMAL) {
        while (timer_is_expired(TIMER_ONE_SECOND)) traffic_second();
    } else {
        if (increase) increase_edit(); /* No 250 ms auto-repeat: one press = +1. */
        if (confirm) save_edit();
        while (timer_is_expired(TIMER_LED_BLINKING)) blink_on ^= 1U;
        edit_outputs();
    }
    publish_debug();
}
// REPORT_EVENTS_END
