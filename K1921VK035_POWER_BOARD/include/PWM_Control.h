/*
 * PWM_Control.h
 *
 *  Created on: 23 ���. 2026 �.
 *      Author: TDA
 */

#ifndef INCLUDE_PWM_CONTROL_H_
#define INCLUDE_PWM_CONTROL_H_

#include "main.h"
#include "EPwm_defines.h"
#include "math.h"

typedef enum {
    TZ_MODE_ONE_SHOT,
    TZ_MODE_CYCLE_BY_CYCLE
} tz_mode_t;

typedef struct {
    tz_mode_t mode;
    uint8_t   filter_width;
    uint32_t  tza_action;
    uint32_t  tzb_action;
} tz_config_t;

void PWM_Module_Init(void);
void PWM_TZ_Protection_Init(const tz_config_t *cfg);

typedef enum {
    STATE_NORMAL,
    STATE_OVERCURRENT_FAULT
} fsm_state_t;

extern volatile fsm_state_t g_foc_state;

#endif /* INCLUDE_PWM_CONTROL_H_ */
