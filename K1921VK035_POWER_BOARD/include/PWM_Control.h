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
#include "K1921VK035.h"
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
// Set TZ input filter width in nanoseconds for all PWM modules
void PWM_TZ_SetFilter_ns(uint32_t ns);

// ADC with Digital Comparator for overcurrent protection
// Configures PB0, PB1, PB2 as ADC channels 0, 1, 2 with digital comparators
// Triggers on N consecutive samples >= highThreshold (code 4095 = full scale)
// Returns 1 on success, 0 on invalid parameters
uint8_t ADC_DC_Overcurrent_Init(uint8_t alertEvents, uint16_t highThreshold, uint16_t lowThreshold);
void ADC_DC_IRQHandler(void);

// ADC FOC initialization
// ACLK = 32MHz (SYSCLK=96MHz, DIVN=2)
// Channels 0,1,2: phase currents ABC, Channel 3: DC bus voltage
// Triggered by PWM_SOCA (CTR=CMPB down-count)
// DMA transfers results to RAM
// Captures offset at startup (~1.65V = 2048 codes for 0A)
typedef struct {
    uint16_t  *dma_buffer;      // Buffer for 4 channels * samples
    uint32_t   buffer_size;     // Number of samples per channel
    uint16_t   current_offset;  // Captured offset for 0A (~2048)
    float      current_gain;    // 0.03 V/A conversion factor
    float      voltage_gain;    // 0.0625 V/V conversion factor
} adc_foc_config_t;

void ADC_FOC_Init(const adc_foc_config_t *cfg);
void ADC_FOC_CaptureOffset(void);
void ADC_FOC_Start(void);
void ADC_FOC_Stop(void);

typedef enum {
    STATE_NORMAL,
    STATE_OVERCURRENT_FAULT
} fsm_state_t;

extern volatile fsm_state_t g_foc_state;

#endif /* INCLUDE_PWM_CONTROL_H_ */
