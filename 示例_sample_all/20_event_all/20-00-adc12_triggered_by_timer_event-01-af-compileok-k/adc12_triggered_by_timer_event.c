

#include "ti_msp_dl_config.h"

volatile bool gCheckADC;
volatile uint16_t gADCResult[4];

int main(void)
{
    SYSCFG_DL_init();

    NVIC_EnableIRQ(ADC12_0_INST_INT_IRQN);

    gCheckADC = false;

    DL_TimerG_startCounter(TIMER_0_INST);

    while (false == gCheckADC) {
        __WFE();
    }

    gADCResult[0] = DL_ADC12_getMemResult(ADC12_0_INST, DL_ADC12_MEM_IDX_0);
    gADCResult[1] = DL_ADC12_getMemResult(ADC12_0_INST, DL_ADC12_MEM_IDX_1);
    gADCResult[2] = DL_ADC12_getMemResult(ADC12_0_INST, DL_ADC12_MEM_IDX_2);
    gADCResult[3] = DL_ADC12_getMemResult(ADC12_0_INST, DL_ADC12_MEM_IDX_3);

    __BKPT(0);

    while (1) {
        __WFI();
    }
}

void ADC12_0_INST_IRQHandler(void)
{
    switch (DL_ADC12_getPendingInterrupt(ADC12_0_INST)) {
        case DL_ADC12_IIDX_MEM3_RESULT_LOADED:
            gCheckADC = true;
            break;
        default:
            break;
    }
}
