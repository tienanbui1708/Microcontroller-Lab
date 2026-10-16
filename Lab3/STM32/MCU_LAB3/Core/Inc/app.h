/*
 * app.h
 * Created on: 17/09/26
 * Author: TienAn
 * Four modes, confirmed draft timings and a safe, validated apply step.
 */
#ifndef INC_APP_H_
#define INC_APP_H_
#include <stdint.h>
typedef enum { NORMAL = 1, SET_RED = 2, SET_AMBER = 3, SET_GREEN = 4 } AppMode;
typedef struct {
    uint32_t mode, edit_value, active_red, active_amber, active_green;
    uint32_t saved_red, saved_amber, saved_green, config_error;
} AppDebug;
extern volatile AppDebug app_debug;
void app_Set_timer(void);
void app_Select_mode(void);
#endif
