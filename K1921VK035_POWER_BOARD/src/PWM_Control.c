/*
 * PWM_Control.c
 *
 *  Created on: 23 ���. 2026 �.
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
 // ����������� ������ ePWM0
 // ------------------------------------------------------------------------

     SIU->PWMSYNC_bit.PRESCRST = 0;                      //������������� �������� - ����� ���������� ���������

     PWM0->TBPHS_bit.TBPHS = 0x0000;                     // Phase is 0
     PWM0->TBCTR = 0x0000;                               // Clear counter

     PWM0->TBCTL_bit.PRDLD = TB_SHADOW;                  // �������� TBPRD ��� TBCTR = 0
     PWM0->TBCTL_bit.CTRMODE = TB_COUNT_UPDOWN;          // ���� �����-����
     PWM0->TBCTL_bit.PHSEN = TB_DISABLE;                 // �� ���������������� ���� ������ �� � ���
     PWM0->TBCTL_bit.HSPCLKDIV = PWM_TBCTL_CLKDIV_Div1;  // ������ �������� ������� (��� �������)
     PWM0->TBCTL_bit.CLKDIV = PWM_TBCTL_HSPCLKDIV_Div1;  // ������ �������� ������� (��� �������)
     PWM0->TBCTL_bit.SYNCOSEL = TB_CTR_ZERO;             // ����� ������-������ ��� TBCTR = 0
     PWM0->TBCTL_bit.FREESOFT = 2;                       // ���������� ���� ��� �������� �������� ����������
     PWM0->TBCTL_bit.SHDWGLOB = 1;                       // ��������� ���������� ������ ������� ���������

     PWM0->CMPCTL_bit.SHDWAMODE = CC_SHADOW;             // ������������ ������� ������� ��� CMPA
     PWM0->CMPCTL_bit.LOADAMODE = CC_CTR_ZERO;           // ��������� � �������� �� �������� �� ���� ��������
     PWM0->CMPCTL_bit.SHDWBMODE = CC_SHADOW;             // ������������ ������� ������� ��� CMPB
     PWM0->CMPCTL_bit.LOADBMODE = CC_CTR_ZERO;           // ��������� � �������� �� �������� �� ���� ��������

     PWM0->AQCTLA_bit.ZRO = AQ_CLEAR;                    // �������� ��� ���� ��������
     PWM0->AQCTLA_bit.CAU = AQ_SET;                      // �������� ��� ��������� � ����������������
     PWM0->AQCTLA_bit.CAD = AQ_CLEAR;                    // �������� ��� ��������� � ����������������
     // PWM0->AQCTLA_bit.PRD = PWM_AQCTLA_PRD_Set;
     // PWM0->AQCTLA_bit.CAU = PWM_AQCTLA_CAU_Clear;
     // PWM0->AQCTLB_bit.PRD = PWM_AQCTLB_PRD_Clear;
     // PWM0->AQCTLB_bit.CBU = PWM_AQCTLB_CBU_Set;

     PWM0->AQSFRC_bit.RLDCSF = 0;                        // �� �� ���� ������ 0

     RCU->PCLKCFG_bit.PWM0EN = 1;
     RCU->PRSTCFG_bit.PWM0EN = 1;
     PWM0->CMPA_bit.CMPA = 0;
     PWM0->CMPB_bit.CMPB = 0;
     PWM0->TBPRD = 3125; // 16 ���

     // ��������� ����� ������� �������
     // DBRED = DBFED = T�_��� * 100
     // => T�_��� = DBRED / 100
     PWM0->DBRED = 50;
     PWM0->DBFED = PWM0->DBRED;
     // ��������� ������ ������� �������:
     // INMODE: S5=0 S4=0 - ������ � ����������� �������� ������� � ������ � ����������� ������ �������
     //         ����������� �� ������� PWMA
     // POLSEL: S3 = 1 S2 = 1 - ������ � ����������� �������� ������� ��� ��� ����, � ������
     //         � ����������� ������ ������� �������������
     // OUTMODE:S0 = 1 S1 = 1 �� ����� PWMA ������� �������� ������ PWMA, �� � ����������� �������� �������
     //         �� ����� PWMB ������� ������ PWMA � ����������� ������ ������� �� ��� � � ���������
     PWM0->DBCTL_bit.INMODE = DBA_ALL;
     PWM0->DBCTL_bit.POLSEL = DB_ACTV_HIC;
     PWM0->DBCTL_bit.OUTMODE = DB_FULL_ENABLE;

     // ��������� Trip-Zone
     #define TZ_STATE 0x0
     PWM0->TZCTL_bit.TZA = TZ_STATE;         // �� ������� "One-Shot Trip" ���������
     PWM0->TZCTL_bit.TZB = TZ_STATE;         // ��� ������ � ������ ��������� (������)

 // ------------------------------------------------------------------------
 // ����������� ������ ePWM1
 // ------------------------------------------------------------------------
     // Setup TBCLK

     PWM1->TBPRD = PWM0->TBPRD;                              // ������ ����� �� ��� � PWM0
     PWM1->TBPHS_bit.TBPHS = 0x0001;                         // ���� ����� 1 ��-�� �������� � ���� ���� �� �������������
     PWM1->TBCTR = 0x0000;                                   // Clear counter
     // ��������� ��������
     PWM1->TBCTL_bit.PRDLD = TB_SHADOW;                      // �������� TBPRD ��� TBCTR = 0
     PWM1->TBCTL_bit.CTRMODE = TB_COUNT_UPDOWN;              // ���� �����-����
     PWM1->TBCTL_bit.PHSEN = TB_ENABLE;                      // ��������� �������������
     PWM1->TBCTL_bit.PHSDIR = TB_UP;                         // ������� ����� ����� �������� ����
     PWM1->TBCTL_bit.HSPCLKDIV = PWM_TBCTL_CLKDIV_Div1;      // ������ �������� ������� (��� �������)
     PWM1->TBCTL_bit.CLKDIV = PWM_TBCTL_HSPCLKDIV_Div1;      // ������ �������� ������� (��� �������)
     PWM1->TBCTL_bit.SYNCOSEL = TB_SYNC_IN;                  // ���������� ������-������ "��������"
     PWM1->TBCTL_bit.FREESOFT = 2;                           // ���������� ���� ��� �������� �������� ����������
     PWM1->TBCTL_bit.SHDWGLOB = 1;                           // ��������� ���������� ������ ������� ���������

     // ������� ��������, ��������� �������� ��������, ������ �����
     // TZ � ���������� ������������� ��� ��, ��� � PWM0
     PWM1->CMPCTL = PWM0->CMPCTL;
     PWM1->AQCTLA = PWM0->AQCTLA;
     PWM1->DBRED = PWM0->DBRED;
     PWM1->DBFED = PWM0->DBFED;
     PWM1->DBCTL = PWM0->DBCTL;
     PWM1->TZCTL = PWM0->TZCTL;
     PWM1->ETSEL = PWM0->ETSEL;

     PWM1->AQSFRC_bit.RLDCSF = 0;                            // �� �� ���� ������ 0

     // ��������� �������
     PWM1->CMPA_bit.CMPA = PWM0->CMPA_bit.CMPA;
     PWM1->CMPB_bit.CMPB = PWM0->CMPB_bit.CMPB;

 // ------------------------------------------------------------------------
 // ����������� ������ ePWM2
 // ------------------------------------------------------------------------
     // Setup TBCLK
     PWM2->TBPRD = PWM0->TBPRD;
     PWM2->TBPHS_bit.TBPHS = 0x0001;                         // ���� ����� 1 ��-�� �������� � ���� ���� �� �������������
     PWM2->TBCTR = 0x0000;                                   // Clear counter

     // ��������� �������� ����� ��, ��� � PWM1
     PWM2->TBCTL = PWM1->TBCTL;

     // ������� ��������, ��������� �������� ��������, ������ �����
     // TZ � ���������� ������������� ��� ��, ��� � PWM0 � PWM1
     PWM2->CMPCTL = PWM0->CMPCTL;
     PWM2->AQCTLA = PWM0->AQCTLA;
     PWM2->DBRED = PWM0->DBRED;
     PWM2->DBFED = PWM0->DBFED;
     PWM2->DBCTL = PWM0->DBCTL;
     PWM2->TZCTL = PWM0->TZCTL;
     PWM2->ETSEL = PWM0->ETSEL;

     // ��������� �������
     PWM2->CMPA_bit.CMPA = PWM0->CMPA_bit.CMPA;
     PWM2->CMPB_bit.CMPB = PWM0->CMPB_bit.CMPB;

     PWM2->AQSFRC_bit.RLDCSF = 0;                            // �� �� ���� ������ 0

     // ��������� �����
     //  PWM0->TZFRC_bit.OST = 1;
     //  PWM1->TZFRC_bit.OST = 1;
     //  PWM2->TZFRC_bit.OST = 1;

     //���������� ������ ���
     SIU->PWMSYNC_bit.PRESCRST = 0b111;
}

void PWM_TZ_Protection_Init(const tz_config_t *cfg)
{
    uint32_t tza = cfg->tza_action;
    uint32_t tzb = cfg->tzb_action;
    uint8_t  fwidth = cfg->filter_width;

    PWM_TypeDef *pwm_modules[3] = { PWM0, PWM1, PWM2 };

    for (int i = 0; i < 3; i++) {
        PWM_TypeDef *pwm = pwm_modules[i];

        pwm->TZSEL = 0;
        if (cfg->mode == TZ_MODE_ONE_SHOT) {
            pwm->TZSEL_bit.OST = TZ_ENABLE;
        } else {
            pwm->TZSEL_bit.CBC = TZ_ENABLE;
        }

        pwm->TZCTL_bit.TZA = tza;
        pwm->TZCTL_bit.TZB = tzb;

        pwm->FWDTH_bit.VAL = fwidth;

        pwm->TZEINT = 0;
        if (cfg->mode == TZ_MODE_ONE_SHOT) {
            pwm->TZEINT_bit.OST = 1;
        } else {
            pwm->TZEINT_bit.CBC = 1;
        }

        pwm->TZCLR_bit.INT = 1;
        pwm->TZCLR_bit.OST = 1;
        pwm->TZCLR_bit.CBC = 1;

        pwm->TZINTCLR_bit.INT = 1;
    }

    NVIC_EnableIRQ(PWM0_TZ_IRQn);
    NVIC_EnableIRQ(PWM1_TZ_IRQn);
    NVIC_EnableIRQ(PWM2_TZ_IRQn);
}
