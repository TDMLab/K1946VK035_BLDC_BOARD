/******************************************************************************
 * @file        main.c
 * @brief       BLDC PWM control
 * @version     v1.1
 * @date        19.04.25
 *
 * @note TDM LAB
 *
 * 3-phase PWM sine demo with TZ overcurrent protection
 ******************************************************************************/

#include "main.h"
#include "PWM_Control.h"

#define PIN_PORT GPIOA
#define LED_PIN 14
#define LED2_PIN 15

#define BLINK_FAST 200000
#define BLINK_SLOW 1000000
#define FAULT_BLINK_PERIOD 500000

volatile fsm_state_t g_foc_state = STATE_NORMAL;
volatile uint32_t fault_led_timer = 0;

uint8_t Sin_Table256[256] = {
128,131,134,137,140,143,146,149,152,156,159,162,165,168,171,174,176,179,182,185,188,191,193,196,199,201,204,206,209,211,213,216,
218,220,222,224,226,228,230,232,234,235,237,239,240,242,243,244,246,247,248,249,250,251,251,252,253,253,254,254,254,255,255,255,
255,255,255,255,254,254,253,253,252,252,251,250,249,248,247,246,245,244,242,241,239,238,236,235,233,231,229,227,225,223,221,219,
217,215,212,210,207,205,202,200,197,195,192,189,186,184,181,178,175,172,169,166,163,160,157,154,151,148,145,142,138,135,132,129,
126,123,120,117,113,110,107,104,101,98,95,92,89,86,83,80,77,74,71,69,66,63,60,58,55,53,50,48,45,43,40,38,
36,34,32,30,28,26,24,22,20,19,17,16,14,13,11,10,9,8,7,6,5,4,3,3,2,2,1,1,0,0,0,0,
0,0,0,1,1,1,2,2,3,4,4,5,6,7,8,9,11,12,13,15,16,18,20,21,23,25,27,29,31,33,35,37,
39,42,44,46,49,51,54,56,59,62,64,67,70,73,76,79,81,84,87,90,93,96,99,103,106,109,112,115,118,121,124,128
};

uint8_t i = 0;
uint32_t acc = 0;

uint8_t Count_PHA = 0;
uint8_t Count_PHB = 0;
uint8_t Count_PHC = 0;

int TimerPeriod = BLINK_FAST;

int ledState = 0;

void LED_blink(void) {
    static int timer;
    if (timer > TimerPeriod) {
        timer = 0;
        ledState ^= 0x1;

        if (ledState == 0x01)
            PIN_PORT->DATA &= ~(1 << LED_PIN);
        else
            PIN_PORT->DATA |= (1 << LED_PIN);
    }
    timer++;
}

void LED2_fault_blink(void) {
    fault_led_timer++;
    if (fault_led_timer > FAULT_BLINK_PERIOD) {
        fault_led_timer = 0;
        GPIOA->DATA ^= (1 << LED2_PIN);
    }
}

void PWM0_TZ_IRQHandler(void) {
    g_foc_state = STATE_OVERCURRENT_FAULT;
    PWM0->TZINTCLR_bit.INT = 1;
    NVIC_ClearPendingIRQ(PWM0_TZ_IRQn);
}

void PWM1_TZ_IRQHandler(void) {
    g_foc_state = STATE_OVERCURRENT_FAULT;
    PWM1->TZINTCLR_bit.INT = 1;
    NVIC_ClearPendingIRQ(PWM1_TZ_IRQn);
}

void PWM2_TZ_IRQHandler(void) {
    g_foc_state = STATE_OVERCURRENT_FAULT;
    PWM2->TZINTCLR_bit.INT = 1;
    NVIC_ClearPendingIRQ(PWM2_TZ_IRQn);
}

int16 main(void) {
    SystemInit();

    SIU->CLKOUTCTL = (1 << 0);
    RCU->CLKOUTCFG = 0x0101;

    GPIOA->DENSET = (1 << LED_PIN | 1 << LED2_PIN);
    GPIOA->OUTENSET = (1 << LED_PIN | 1 << LED2_PIN);
    GPIOA->ALTFUNCCLR = (1 << LED_PIN | 1 << LED2_PIN);

    PWM_Module_Init();

    tz_config_t tz_cfg;
    tz_cfg.mode = TZ_MODE_ONE_SHOT;
    tz_cfg.filter_width = 15;
    tz_cfg.tza_action = TZ_FORCE_LO;
    tz_cfg.tzb_action = TZ_FORCE_LO;
    PWM_TZ_Protection_Init(&tz_cfg);

    EINT;

    while (1) {
        if (g_foc_state == STATE_OVERCURRENT_FAULT) {
            LED2_fault_blink();
            continue;
        }

        Count_PHA = i + 85;
        Count_PHB = i;
        Count_PHC = i + 171;

        PWM0->CMPA_bit.CMPA = Sin_Table256[Count_PHA] * 3061UL / 256 + 1;
        PWM1->CMPA_bit.CMPA = Sin_Table256[Count_PHB] * 3061UL / 256 + 1;
        PWM2->CMPA_bit.CMPA = Sin_Table256[Count_PHC] * 3061UL / 256 + 1;

        i = acc >> 14;
        acc++;

        LED_blink();
    }
}

/*@}*/
