/*
 * PWM_Control.c
 *
 *  Created on: 23 апр. 2026 г.
 *      Author: TDA
 */
#include "PWM_Control.h"

//void PWM_Module_Init(TPWM_Module *p) {
void PWM_Module_Init(void)
{
     GPIOA->DENSET = GPIO_DENSET_PIN8_Msk | GPIO_DENSET_PIN9_Msk |
     GPIO_DENSET_PIN10_Msk | GPIO_DENSET_PIN11_Msk | GPIO_DENSET_PIN12_Msk | GPIO_DENSET_PIN13_Msk;
     GPIOA->ALTFUNCSET = GPIO_ALTFUNCSET_PIN8_Msk | GPIO_ALTFUNCSET_PIN9_Msk |
     GPIO_ALTFUNCSET_PIN10_Msk | GPIO_ALTFUNCSET_PIN11_Msk | GPIO_ALTFUNCSET_PIN12_Msk | GPIO_ALTFUNCSET_PIN13_Msk;

 // ------------------------------------------------------------------------
 // Настраиваем модуль ePWM0
 // ------------------------------------------------------------------------

     SIU->PWMSYNC_bit.PRESCRST = 0;                      //синхронизация таймеров - сброс внутренних счетчиков

     PWM0->TBPHS_bit.TBPHS = 0x0000;                     // Phase is 0
     PWM0->TBCTR = 0x0000;                               // Clear counter

     PWM0->TBCTL_bit.PRDLD = TB_SHADOW;                  // Загрузка TBPRD при TBCTR = 0
     PWM0->TBCTL_bit.CTRMODE = TB_COUNT_UPDOWN;          // Счёт вверх-вниз
     PWM0->TBCTL_bit.PHSEN = TB_DISABLE;                 // Не синхронизировать этот таймер ни с чем
     PWM0->TBCTL_bit.HSPCLKDIV = PWM_TBCTL_CLKDIV_Div1;  // Первый делитель частоты (нет деления)
     PWM0->TBCTL_bit.CLKDIV = PWM_TBCTL_HSPCLKDIV_Div1;  // Второй делитель частоты (нет деления)
     PWM0->TBCTL_bit.SYNCOSEL = TB_CTR_ZERO;             // Выдаём синхро-сигнал при TBCTR = 0
     PWM0->TBCTL_bit.FREESOFT = 2;                       // Продолжать счёт при дебажном останове процессора
     PWM0->TBCTL_bit.SHDWGLOB = 1;                       // Разрешить нормальную работу теневых регистров

     PWM0->CMPCTL_bit.SHDWAMODE = CC_SHADOW;             // Использовать теневой регистр для CMPA
     PWM0->CMPCTL_bit.LOADAMODE = CC_CTR_ZERO;           // Загружать в активный из теневого по нулю счётчика
     PWM0->CMPCTL_bit.SHDWBMODE = CC_SHADOW;             // Использовать теневой регистр для CMPB
     PWM0->CMPCTL_bit.LOADBMODE = CC_CTR_ZERO;           // Загружать в активный из теневого по нулю счётчика

     PWM0->AQCTLA_bit.ZRO = AQ_CLEAR;                    // Обнуляем при нуле счетчика
     PWM0->AQCTLA_bit.CAU = AQ_SET;                      // Включаем при сравнении и инкрементиовании
     PWM0->AQCTLA_bit.CAD = AQ_CLEAR;                    // Обнуляем при сравнении и декрементровании
     // PWM0->AQCTLA_bit.PRD = PWM_AQCTLA_PRD_Set;
     // PWM0->AQCTLA_bit.CAU = PWM_AQCTLA_CAU_Clear;
     // PWM0->AQCTLB_bit.PRD = PWM_AQCTLB_PRD_Clear;
     // PWM0->AQCTLB_bit.CBU = PWM_AQCTLB_CBU_Set;

     PWM0->AQSFRC_bit.RLDCSF = 0;                        // По ТО надо писать 0

     RCU->PCLKCFG_bit.PWM0EN = 1;
     RCU->PRSTCFG_bit.PWM0EN = 1;
     PWM0->CMPA_bit.CMPA = 0;
     PWM0->CMPB_bit.CMPB = 0;
     PWM0->TBPRD = 3125; // 16 кГц

     // Настройка блока мёртвого времени
     // DBRED = DBFED = Tм_мкс * 100
     // => Tм_мкс = DBRED / 100
     PWM0->DBRED = 50;
     PWM0->DBFED = PWM0->DBRED;
     // Настройка модуля мёртвого времени:
     // INMODE: S5=0 S4=0 - сигнал с задержанным передним фронтом и сигнал с задержанным задним фронтом
     //         формируются из сигнала PWMA
     // POLSEL: S3 = 1 S2 = 1 - сигнал с задержанным передним фронтом идёт как есть, а сигнал
     //         с задержанным задним фронтом инвертируется
     // OUTMODE:S0 = 1 S1 = 1 на канал PWMA выходит исходный сигнал PWMA, но с задержанным передним фронтом
     //         На канал PWMB выходит сигнал PWMA с задержанным задним фронтом да ещё и с инверсией
     PWM0->DBCTL_bit.INMODE = DBA_ALL;
     PWM0->DBCTL_bit.POLSEL = DB_ACTV_HIC;
     PWM0->DBCTL_bit.OUTMODE = DB_FULL_ENABLE;

     // Настройка Trip-Zone
     #define TZ_STATE 0x0
     PWM0->TZCTL_bit.TZA = TZ_STATE;         // по событию "One-Shot Trip" переводим
     PWM0->TZCTL_bit.TZB = TZ_STATE;         // ШИМ выходы в нужное состояние (третье)

 // ------------------------------------------------------------------------
 // Настраиваем модуль ePWM1
 // ------------------------------------------------------------------------
     // Setup TBCLK

     PWM1->TBPRD = PWM0->TBPRD;                              // Период такой же как у PWM0
     PWM1->TBPHS_bit.TBPHS = 0x0001;                         // Фаза равна 1 из-за задержки в один такт на синхронизацию
     PWM1->TBCTR = 0x0000;                                   // Clear counter
     // Настройка счётчика
     PWM1->TBCTL_bit.PRDLD = TB_SHADOW;                      // Загрузка TBPRD при TBCTR = 0
     PWM1->TBCTL_bit.CTRMODE = TB_COUNT_UPDOWN;              // Счёт вверх-вниз
     PWM1->TBCTL_bit.PHSEN = TB_ENABLE;                      // Разрешить синхронизацию
     PWM1->TBCTL_bit.PHSDIR = TB_UP;                         // Считать вверх после загрузки фазы
     PWM1->TBCTL_bit.HSPCLKDIV = PWM_TBCTL_CLKDIV_Div1;      // Первый делитель частоты (нет деления)
     PWM1->TBCTL_bit.CLKDIV = PWM_TBCTL_HSPCLKDIV_Div1;      // Второй делитель частоты (нет деления)
     PWM1->TBCTL_bit.SYNCOSEL = TB_SYNC_IN;                  // Пропускаем синхро-сигнал "насквозь"
     PWM1->TBCTL_bit.FREESOFT = 2;                           // Продолжать счёт при дебажном останове процессора
     PWM1->TBCTL_bit.SHDWGLOB = 1;                           // Разрешить нормальную работу теневых регистров

     // Теневые регистры, изменения выходных сигналов, мёртвое время
     // TZ и прерывания настраиваются так же, как у PWM0
     PWM1->CMPCTL = PWM0->CMPCTL;
     PWM1->AQCTLA = PWM0->AQCTLA;
     PWM1->DBRED = PWM0->DBRED;
     PWM1->DBFED = PWM0->DBFED;
     PWM1->DBCTL = PWM0->DBCTL;
     PWM1->TZCTL = PWM0->TZCTL;
     PWM1->ETSEL = PWM0->ETSEL;

     PWM1->AQSFRC_bit.RLDCSF = 0;                            // По ТО надо писать 0

     // Обнуление уставок
     PWM1->CMPA_bit.CMPA = PWM0->CMPA_bit.CMPA;
     PWM1->CMPB_bit.CMPB = PWM0->CMPB_bit.CMPB;

 // ------------------------------------------------------------------------
 // Настраиваем модуль ePWM2
 // ------------------------------------------------------------------------
     // Setup TBCLK
     PWM2->TBPRD = PWM0->TBPRD;
     PWM2->TBPHS_bit.TBPHS = 0x0001;                         // Фаза равна 1 из-за задержки в один такт на синхронизацию
     PWM2->TBCTR = 0x0000;                                   // Clear counter

     // Настройка счётчика такая же, как у PWM1
     PWM2->TBCTL = PWM1->TBCTL;

     // Теневые регистры, изменения выходных сигналов, мёртвое время
     // TZ и прерывания настраиваются так же, как у PWM0 и PWM1
     PWM2->CMPCTL = PWM0->CMPCTL;
     PWM2->AQCTLA = PWM0->AQCTLA;
     PWM2->DBRED = PWM0->DBRED;
     PWM2->DBFED = PWM0->DBFED;
     PWM2->DBCTL = PWM0->DBCTL;
     PWM2->TZCTL = PWM0->TZCTL;
     PWM2->ETSEL = PWM0->ETSEL;

     // Обнуление уставок
     PWM2->CMPA_bit.CMPA = PWM0->CMPA_bit.CMPA;
     PWM2->CMPB_bit.CMPB = PWM0->CMPB_bit.CMPB;

     PWM2->AQSFRC_bit.RLDCSF = 0;                            // По ТО надо писать 0

     // Отключаем ключи
     //  PWM0->TZFRC_bit.OST = 1;
     //  PWM1->TZFRC_bit.OST = 1;
     //  PWM2->TZFRC_bit.OST = 1;

     //Синхронный запуск ШИМ
     SIU->PWMSYNC_bit.PRESCRST = 0b111;
}
