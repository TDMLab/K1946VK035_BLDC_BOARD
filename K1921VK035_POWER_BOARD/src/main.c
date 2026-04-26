/******************************************************************************
 * @file        main.c
 * @brief       Главный файл проекта
 * @version     v1.0
 * @date        19.04.25
 *
 * @note TDM LAB
 *
 * Пример работы 3-х фазной ШИМ
 ******************************************************************************/

#include "main.h"
#include "PWM_Control.h"

#define PIN_PORT GPIOA
#define LED_PIN 14
#define BUTTON_PIN 15

#define BLINK_FAST 200000
#define BLINK_SLOW 1000000

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

//Глобальная переменная периода счета таймера (может быть изменена при остановке расчета в окне просмотра переменных)
int TimerPeriod = BLINK_FAST;

// Глобальная переменная для хранения состояния светодиода
int ledState = 0;
int ButtonState = 0;
int ButtonClickFlag = 0;

//Функция мигания светодиодом
void LED_blink (void) {
    static int timer;   //переменная таймера
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

int16 main (void){
//    DINT;
    //Инициализация микроконтроллера: настройка таймеров, инициализаци периферийных устройств
    SystemInit();
/*
 *
    CLKOUTCTL Регистр настройки выдачи тактового сигнала, ножка SERVEN
    CLKOUTCFG Регистр настройки выходного тактового сигнала
    0x0101 выход PLL без деления PLLCLK
    0x0201 выход частоты кварца OSECLK
*/
    SIU->CLKOUTCTL = (1 << 0);
    RCU->CLKOUTCFG = 0x0101;

    //Инициализация периферии для управления светодиодами
    GPIOA->DENSET = (1 << LED_PIN | 1 << BUTTON_PIN);   // Регистр разрешения цифровой функции порта
    GPIOA->OUTENSET = (1 << LED_PIN);                   // Ножка настраивается на выход
    GPIOA->ALTFUNCCLR = (1 << LED_PIN);
//   GPIOA->PULLMODE |= (1 << 28);                       // Подтягиваем PA14 Pull-up

    GPIOA->PULLMODE |= (1 << 31);                       // Подтягиваем PA15 Pull-down

    GPIOA->QUALSET |= (1 << BUTTON_PIN);                // Регистр включения фильтров портов
    GPIOA->QUALMODESET |= (1 << BUTTON_PIN);            // Режим измерения уровня входа по 6 отсчетам
    GPIOA->QUALSAMPLE = 255;                            // Регистр настройки фильтра порта, значение в тактах FCLK

//    pwm0_init();
    PWM_Module_Init();

//    PWM0->FWDTH = 255;
//    PWM1->FWDTH = 255;
//    PWM2->FWDTH = 255;

//    EINT;//разрешение прерываний


    while(1)
    {
//        PWM0->CMPA_bit.CMPA = 781;
//        PWM1->CMPA_bit.CMPA = 1562;
//        PWM2->CMPA_bit.CMPA = 2343;

        Count_PHA = i + 85;
        Count_PHB = i;
        Count_PHC = i + 171;

        PWM0->CMPA_bit.CMPA = Sin_Table256[Count_PHA] * 3061UL / 256 + 1;
        PWM1->CMPA_bit.CMPA = Sin_Table256[Count_PHB] * 3061UL / 256 + 1;
        PWM2->CMPA_bit.CMPA = Sin_Table256[Count_PHC] * 3061UL / 256 + 1;

        i = acc >> 14;
        acc++;

        if((GPIOA->DATA & (1 << BUTTON_PIN)) != 0 && ButtonClickFlag == 0) {
            ButtonClickFlag = 1;
            ButtonState++;
            if (ButtonState > 1){
                ButtonState = 0;
            }

            if(ButtonState == 1){
                TimerPeriod = BLINK_FAST;
                RCU->CLKOUTCFG = 0x0201;
            }
            else {
                TimerPeriod = BLINK_SLOW;
                RCU->CLKOUTCFG = 0x0101;
            }
        }
        else if ((GPIOA->DATA & (1 << BUTTON_PIN)) == 0) {
            ButtonClickFlag = 0;
        }
        LED_blink();    // Мигание светодиодом
    }
}

/*@}*/
