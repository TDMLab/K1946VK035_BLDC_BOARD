/*
 * PWM_Control.c
 *
 *  Created on: 23 May 2026
 *      Author: TDA
 */
#include "PWM_Control.h"

#define PWM_TZ_PIN      7
#define CONCAT2(a, b)   a ## b
#define PIN_(n)         CONCAT2(PIN, n)

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

     SIU->PWMSYNC_bit.PRESCRST = 0;                      // Reset PWM prescaler

     PWM0->TBPHS_bit.TBPHS = 0x0000;                     // Phase is 0
     PWM0->TBCTR = 0x0000;                               // Clear counter

     PWM0->TBCTL_bit.PRDLD = TB_SHADOW;                  // Load TBPRD at TBCTR = 0
     PWM0->TBCTL_bit.CTRMODE = TB_COUNT_UPDOWN;          // Up-down count mode
     PWM0->TBCTL_bit.PHSEN = TB_DISABLE;                 // Disable phase shift
     PWM0->TBCTL_bit.HSPCLKDIV = PWM_TBCTL_CLKDIV_Div1;  // High-speed clock divider = 1
     PWM0->TBCTL_bit.CLKDIV = PWM_TBCTL_HSPCLKDIV_Div1;  // Clock divider = 1
     PWM0->TBCTL_bit.SYNCOSEL = TB_CTR_ZERO;             // Sync at TBCTR = 0
     PWM0->TBCTL_bit.FREESOFT = 2;                       // Freeze on emulation suspend
     PWM0->TBCTL_bit.SHDWGLOB = 1;                       // Enable global shadow register load

     PWM0->CMPCTL_bit.SHDWAMODE = CC_SHADOW;             // Shadow mode for CMPA
     PWM0->CMPCTL_bit.LOADAMODE = CC_CTR_ZERO;           // Load CMPA at TBCTR = 0
     PWM0->CMPCTL_bit.SHDWBMODE = CC_SHADOW;             // Shadow mode for CMPB
     PWM0->CMPCTL_bit.LOADBMODE = CC_CTR_ZERO;           // Load CMPB at TBCTR = 0

     PWM0->AQCTLA_bit.ZRO = AQ_CLEAR;                    // Clear output at TBCTR = 0
     PWM0->AQCTLA_bit.CAU = AQ_SET;                      // Set output at CMPA up
     PWM0->AQCTLA_bit.CAD = AQ_CLEAR;                    // Clear output at CMPA down
     // PWM0->AQCTLA_bit.PRD = PWM_AQCTLA_PRD_Set;
     // PWM0->AQCTLA_bit.CAU = PWM_AQCTLA_CAU_Clear;
     // PWM0->AQCTLB_bit.PRD = PWM_AQCTLB_PRD_Clear;
     // PWM0->AQCTLB_bit.CBU = PWM_AQCTLB_CBU_Set;

     PWM0->AQSFRC_bit.RLDCSF = 0;                        // No immediate reload

     RCU->PCLKCFG_bit.PWM0EN = 1;
     RCU->PRSTCFG_bit.PWM0EN = 1;
     PWM0->CMPA_bit.CMPA = 0;
     PWM0->CMPB_bit.CMPB = 0;
     PWM0->TBPRD = 3125; // 16 ���

     // Dead-time configuration
     // DBRED = DBFED = T�_��� * 100
      // => Dead-time = DBRED / 100
     PWM0->DBRED = 50;
     PWM0->DBFED = PWM0->DBRED;
     // ��������� ������ ������� �������:
      // INMODE: S5=0 S4=0 - Input from PWMxA, filtered
      //         Input source: PWMxA
      // POLSEL: S3 = 1 S2 = 1 - Active high for PWMxA, active low for PWMxB
      //         Complementary outputs
      // OUTMODE: PWMxA controls output, PWMxB is inverted
      //         PWMxB is inverted PWMxA
     PWM0->DBCTL_bit.INMODE = DBA_ALL;
     PWM0->DBCTL_bit.POLSEL = DB_ACTV_HIC;
     PWM0->DBCTL_bit.OUTMODE = DB_FULL_ENABLE;

      // Trip-Zone configuration
     #define TZ_STATE 0x0
      PWM0->TZCTL_bit.TZA = TZ_STATE;         // Disable One-Shot Trip
      PWM0->TZCTL_bit.TZB = TZ_STATE;         // Disable Cycle-by-Cycle Trip

 // ------------------------------------------------------------------------
      // Configure ePWM1
 // ------------------------------------------------------------------------
     // Setup TBCLK

     PWM1->TBPRD = PWM0->TBPRD;                              // ������ ����� �� ��� � PWM0
     PWM1->TBPHS_bit.TBPHS = 0x0001;                         // ���� ����� 1 ��-�� �������� � ���� ���� �� �������������
     PWM1->TBCTR = 0x0000;                                   // Clear counter
      // Configure time-base
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
      // TZ configuration same as PWM0
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
      // Configure ePWM2
 // ------------------------------------------------------------------------
     // Setup TBCLK
     PWM2->TBPRD = PWM0->TBPRD;
     PWM2->TBPHS_bit.TBPHS = 0x0001;                         // ���� ����� 1 ��-�� �������� � ���� ���� �� �������������
     PWM2->TBCTR = 0x0000;                                   // Clear counter

      // Configure time-base same as PWM1
     PWM2->TBCTL = PWM1->TBCTL;

     // ������� ��������, ��������� �������� ��������, ������ �����
      // TZ configuration same as PWM0 and PWM1
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

      // Configure compare registers
     //  PWM0->TZFRC_bit.OST = 1;
     //  PWM1->TZFRC_bit.OST = 1;
     //  PWM2->TZFRC_bit.OST = 1;

      // Enable PWM clocks
     SIU->PWMSYNC_bit.PRESCRST = 0b111;
}

void PWM_TZ_Protection_Init()
{
    PWM0->TZSEL_bit.OST = 1;
    PWM1->TZSEL_bit.OST = 1;
    PWM2->TZSEL_bit.OST = 1;
    PWM0->TZCTL_bit.TZB = 1;
    PWM1->TZCTL_bit.TZB = 1;
    PWM2->TZCTL_bit.TZB = 1;
    PWM0->TZCTL_bit.TZA = 1;
    PWM1->TZCTL_bit.TZA = 1;
    PWM2->TZCTL_bit.TZA = 1;
    PWM0->TZEINT_bit.OST = 1;
    PWM1->TZEINT_bit.OST = 1;
    PWM2->TZEINT_bit.OST = 1;
    NVIC_EnableIRQ(PWM0_TZ_IRQn);
    NVIC_EnableIRQ(PWM1_TZ_IRQn);
    NVIC_EnableIRQ(PWM2_TZ_IRQn);
    GPIOA->DENSET_bit.PIN_(PWM_TZ_PIN) = 1;
    GPIOA->ALTFUNCSET_bit.PIN_(PWM_TZ_PIN) = 1;
    GPIOA->SYNCCLR_bit.PIN_(PWM_TZ_PIN) = 1;
    GPIOA->QUALSET_bit.PIN_(PWM_TZ_PIN) = 1;
    GPIOA->QUALMODECLR_bit.PIN_(PWM_TZ_PIN) = 1;
    GPIOA->QUALSAMPLE_bit.VAL = 9;
}

void PWM_HD_Protection_Init()
{
    PWM0->HDEINT_bit.OST = 1;
    PWM1->HDEINT_bit.OST = 1;
    PWM2->HDEINT_bit.OST = 1;
    PWM0->HDCTL_bit.HDB = 3;
    PWM0->HDCTL_bit.HDA = 3;
    PWM1->HDCTL_bit.HDB = 3;
    PWM1->HDCTL_bit.HDA = 3;
    PWM2->HDCTL_bit.HDB = 3;
    PWM2->HDCTL_bit.HDA = 3;
    PWM0->HDSEL_bit.OST = 1;
    PWM1->HDSEL_bit.OST = 1;
    PWM2->HDSEL_bit.OST = 1;
    NVIC_EnableIRQ(PWM2_HD_IRQn);
    NVIC_EnableIRQ(PWM1_HD_IRQn);
    NVIC_EnableIRQ(PWM0_HD_IRQn);

}

uint8_t ADC_DC_Overcurrent_Init()
{
    RCU->ADCCFG_bit.CLKEN = 1;
    RCU->ADCCFG_bit.RSTDIS = 1;
    RCU->ADCCFG_bit.CLKSEL = 1;
    RCU->ADCCFG_bit.DIVEN = 1;
    RCU->ADCCFG_bit.DIVN = 7;

    GPIOB->DENSET = GPIO_DENSET_PIN0_Msk | GPIO_DENSET_PIN1_Msk | GPIO_DENSET_PIN2_Msk;
    GPIOB->ALTFUNCSET = GPIO_ALTFUNCSET_PIN0_Msk | GPIO_ALTFUNCSET_PIN1_Msk | GPIO_ALTFUNCSET_PIN2_Msk;

    ADC->SEQEN_bit.SEQEN0 = 0;
    ADC->SEQ[0].SRQSEL_bit.RQ0 = 0;
    ADC->SEQ[0].SRQSEL_bit.RQ1 = 1;
    ADC->SEQ[0].SRQSEL_bit.RQ2 = 2;
    ADC->SEQ[0].SRQCTL_bit.RQMAX = 2;
    ADC->SEQ[0].SCCTL_bit.ICNT = 1;
    ADC->SEQ[0].SCCTL_bit.RAVGEN = 0;
    ADC->EMUX_bit.EM0 = 0;

    for (int i = 0; i < 3; i++) {
        ADC->DC[i].DCTL_bit.SRC = 1;
        ADC->DC[i].DCTL_bit.CHNL = i;
        ADC->DC[i].DCTL_bit.CIM = 0;
        ADC->DC[i].DCTL_bit.CIC = 0;
        ADC->DC[i].DCTL_bit.CIE = 1;
        ADC->DC[i].DCTL_bit.CTM = 0;
        ADC->DC[i].DCTL_bit.CTC = 2;
        ADC->DC[i].DCTL_bit.CTE = 1;
        ADC->DC[i].DCMP_bit.CMPL = 0;
        ADC->DC[i].DCMP_bit.CMPH = 4095;
    }

    //for measure >= cmph - int, for measure <=cmpl - trig

    ADC->SEQ[0].SDC_bit.DC0 = 1;
    ADC->SEQ[0].SDC_bit.DC1 = 1;
    ADC->SEQ[0].SDC_bit.DC2 = 1;

    ADC->SEQEN_bit.SEQEN0 = 1;
    ADC->SEQSYNC_bit.SYNC0 = 1;

    NVIC_EnableIRQ(ADC_DC_IRQn);
    return 1;
}


void ADC_DC_IRQHandler(void)
{
    if (ADC->DCTRIG_bit.DCEV0) {
        ADC->IC_bit.DCIC0 = 1;
        //g_foc_state = STATE_OVERCURRENT_FAULT;
    }
    if (ADC->DCTRIG_bit.DCEV1) {
        ADC->IC_bit.DCIC1 = 1;
        //g_foc_state = STATE_OVERCURRENT_FAULT;
    }
    if (ADC->DCTRIG_bit.DCEV2) {
        ADC->IC_bit.DCIC2 = 1;
        //g_foc_state = STATE_OVERCURRENT_FAULT;
    }
    if (ADC->DCTRIG_bit.DCEV3) {
        ADC->IC_bit.DCIC3 = 1;
        //g_foc_state = STATE_OVERCURRENT_FAULT;
    }
    NVIC_ClearPendingIRQ(ADC_DC_IRQn);
}

static adc_foc_config_t *g_adc_foc_cfg = NULL;

void ADC_FOC_Init(const adc_foc_config_t *cfg)
{
    g_adc_foc_cfg = (adc_foc_config_t *)cfg;

    RCU->ADCCFG_bit.CLKEN = 1;
    RCU->ADCCFG_bit.RSTDIS = 1;
    RCU->ADCCFG_bit.CLKSEL = 1;
    RCU->ADCCFG_bit.DIVEN = 1;
    RCU->ADCCFG_bit.DIVN = 2;

    GPIOB->DENSET = GPIO_DENSET_PIN0_Msk | GPIO_DENSET_PIN1_Msk | GPIO_DENSET_PIN2_Msk | GPIO_DENSET_PIN3_Msk;
    GPIOB->ALTFUNCSET = GPIO_ALTFUNCSET_PIN0_Msk | GPIO_ALTFUNCSET_PIN1_Msk | GPIO_ALTFUNCSET_PIN2_Msk | GPIO_ALTFUNCSET_PIN3_Msk;

    ADC->SEQEN_bit.SEQEN0 = 0;

    ADC->SEQ[0].SRQSEL_bit.RQ0 = 0;
    ADC->SEQ[0].SRQSEL_bit.RQ1 = 1;
    ADC->SEQ[0].SRQSEL_bit.RQ2 = 2;
    ADC->SEQ[0].SRQSEL_bit.RQ3 = 3;
    ADC->SEQ[0].SRQCTL_bit.RQMAX = 3;
    ADC->SEQ[0].SRQCTL_bit.QAVGEN = 0;
    ADC->SEQ[0].SCCTL_bit.ICNT = 0;
    ADC->SEQ[0].SCCTL_bit.RAVGEN = 0;
    ADC->EMUX_bit.EM0 = 7;

    ADC->SEQ[0].SDC_bit.DC0 = 0;
    ADC->SEQ[0].SDC_bit.DC1 = 0;
    ADC->SEQ[0].SDC_bit.DC2 = 0;
    ADC->SEQ[0].SDC_bit.DC3 = 0;

    ADC->CHCTL[0].CHCTL_bit.OFFTRIM = 0;
    ADC->CHCTL[1].CHCTL_bit.OFFTRIM = 0;
    ADC->CHCTL[2].CHCTL_bit.OFFTRIM = 0;
    ADC->CHCTL[3].CHCTL_bit.OFFTRIM = 0;

    ADC->SEQEN_bit.SEQEN0 = 1;
    ADC->SEQSYNC_bit.SYNC0 = 1;

    while (!ADC->ACTL_bit.ADCRDY);

    if (cfg && cfg->dma_buffer && cfg->buffer_size > 0) {
        DMA->CH[DMA_CH_ADCSEQ0].CTRL_bit.EN = 0;
        DMA->CH[DMA_CH_ADCSEQ0].SRC = (uint32_t)&ADC->SEQ[0].SFIFO;
        DMA->CH[DMA_CH_ADCSEQ0].DST = (uint32_t)cfg->dma_buffer;
        DMA->CH[DMA_CH_ADCSEQ0].CTRL_bit.SIZE = cfg->buffer_size * 4;
        DMA->CH[DMA_CH_ADCSEQ0].CTRL_bit.SRC_INC = 0;
        DMA->CH[DMA_CH_ADCSEQ0].CTRL_bit.DST_INC = 1;
        DMA->CH[DMA_CH_ADCSEQ0].CTRL_bit.SRC_WIDTH = 2;
        DMA->CH[DMA_CH_ADCSEQ0].CTRL_bit.DST_WIDTH = 2;
        DMA->CH[DMA_CH_ADCSEQ0].CTRL_bit.EN = 1;
        ADC->SEQ[0].SDMACTL_bit.DMAEN = 1;
        ADC->SEQ[0].SDMACTL_bit.WMARK = 0;
    }

    PWM0->ETSEL_bit.SOCASEL = 7;
    PWM0->ETSEL_bit.SOCAEN = 1;
    PWM0->ETPS_bit.SOCAPRD = 1;
}

void ADC_FOC_CaptureOffset(void)
{
    uint32_t sum[4] = {0};
    const uint32_t samples = 256;

    PWM0->ETSEL_bit.SOCAEN = 0;

    ADC->EMUX_bit.EM0 = 0;
    ADC->SEQSYNC_bit.SYNC0 = 1;

    for (uint32_t i = 0; i < samples; i++) {
        ADC->SEQSYNC_bit.SYNC0 = 1;
        while (ADC->SEQ[0].SFLOAD_bit.VAL < 4);
        for (int ch = 0; ch < 4; ch++) {
            sum[ch] += ADC->SEQ[0].SFIFO_bit.DATA;
        }
    }

    for (int ch = 0; ch < 3; ch++) {
        ADC->CHCTL[ch].CHCTL_bit.OFFTRIM = (int16_t)(2048 - (sum[ch] / samples));
    }
    ADC->CHCTL[3].CHCTL_bit.OFFTRIM = (int16_t)(2048 - (sum[3] / samples));

    if (g_adc_foc_cfg) {
        g_adc_foc_cfg->current_offset = sum[0] / samples;
    }

    ADC->EMUX_bit.EM0 = 7;
    PWM0->ETSEL_bit.SOCAEN = 1;
}

void ADC_FOC_Start(void)
{
    PWM0->ETSEL_bit.SOCAEN = 1;
}

void ADC_FOC_Stop(void)
{
    PWM0->ETSEL_bit.SOCAEN = 0;
}

