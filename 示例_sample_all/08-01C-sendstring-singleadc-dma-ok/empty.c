/*
 * Copyright (c) 2021, Texas Instruments Incorporated
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * *  Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *
 * *  Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * *  Neither the name of Texas Instruments Incorporated nor the names of
 *    its contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */




#include "ti/driverlib/dl_adc12.h"
#include "ti_msp_dl_config.h"


#define ADC_BUF_LEN   10
uint16_t gADCSamples[ADC_BUF_LEN];

#include "stdio.h"
#include "string.h"

void SendString(char *str);
char txBuf[100];

volatile uint8_t flg_adcin = 0;


void delay_ms(uint32_t ms)
{
    while(ms--)
    {
        delay_cycles(CPUCLK_FREQ/1000);
    }
}


int main(void)
{
    SYSCFG_DL_init();

////    DL_DMA_setSrcAddr(DMA, DMA_CH0_CHAN_ID, DL_ADC12_getMemResultAddress(ADC12_0_INST,DL_ADC12_MEM_IDX_0 ) );////    
////    DL_DMA_setDestAddr(DMA, DMA_CH0_CHAN_ID, (uint32_t)&gADCSamples[0]);
////    DL_DMA_enableChannel(DMA, DMA_CH0_CHAN_ID);
////    NVIC_EnableIRQ(ADC12_0_INST_INT_IRQN);
////    DL_ADC12_startConversion(ADC12_0_INST);
    
    //DMA源：MEM0存储器地址
    DL_DMA_setSrcAddr(DMA, DMA_CH0_CHAN_ID, DL_ADC12_getMemResultAddress(ADC12_0_INST, DL_ADC12_MEM_IDX_0));
    DL_DMA_setDestAddr(DMA, DMA_CH0_CHAN_ID, (uint32_t)&gADCSamples[0]);
    DL_DMA_setTransferSize(DMA, DMA_CH0_CHAN_ID, ADC_BUF_LEN);
    
    DL_DMA_setSrcIncrement(DMA, DMA_CH0_CHAN_ID, DL_DMA_ADDR_UNCHANGED);
    DL_DMA_setDestIncrement(DMA, DMA_CH0_CHAN_ID, DL_DMA_ADDR_INCREMENT);
    
    DL_DMA_setSrcWidth(DMA, DMA_CH0_CHAN_ID, DL_DMA_WIDTH_HALF_WORD);
    DL_DMA_setDestWidth(DMA, DMA_CH0_CHAN_ID, DL_DMA_WIDTH_HALF_WORD);

    DL_DMA_enableChannel(DMA, DMA_CH0_CHAN_ID);

    DL_ADC12_enableDMA(ADC12_0_INST);
    DL_ADC12_enableDMATrigger(ADC12_0_INST, DL_ADC12_DMA_MEM0_RESULT_LOADED);

    NVIC_EnableIRQ(ADC12_0_INST_INT_IRQN);
    DL_ADC12_startConversion(ADC12_0_INST);
    
    
    delay_ms(10);

    while (1)
    {
       //delay_ms(1);  //ok
    
       if(flg_adcin==1)
       {
           flg_adcin=0;
              
              
           sprintf(txBuf, "adc %d\r\n", gADCSamples[0]);
           SendString(txBuf);              
          
           DL_ADC12_enableConversions(ADC12_0_INST);
           DL_ADC12_startConversion(ADC12_0_INST);          
       
       }

    }
}

void ADC12_0_INST_IRQHandler(void)
{
    switch (DL_ADC12_getPendingInterrupt(ADC12_0_INST))
    {
        case DL_ADC12_IIDX_DMA_DONE:
            DL_ADC12_disableConversions(ADC12_0_INST);
            
            //Process
            //__BKPT();
            
            flg_adcin = 1;
            
            //DL_ADC12_enableConversions(ADC12_0_INST);
            //DL_ADC12_startConversion(ADC12_0_INST);
            break;
        default:
            break;
    }
}




//----------------------------------err------------------------------------------------------

//#include "ti/driverlib/dl_adc12.h"
//#include "ti_msp_dl_config.h"

//#define ADC_BUF_LEN 1000
//uint16_t gADCSamples[ADC_BUF_LEN];
//volatile bool gDmaCompleteFlag = false;

//int main(void)
//{
//    SYSCFG_DL_init();

//    //DMA源：MEM0存储器地址
//    DL_DMA_setSrcAddr(DMA, DMA_CH0_CHAN_ID, DL_ADC12_getMemResultAddress(ADC12_0_INST, DL_ADC12_MEM_IDX_0));
//    DL_DMA_setDestAddr(DMA, DMA_CH0_CHAN_ID, (uint32_t)&gADCSamples[0]);

//    DL_DMA_setTransferSize(DMA, DMA_CH0_CHAN_ID, ADC_BUF_LEN);
//    DL_DMA_setSrcIncrement(DMA, DMA_CH0_CHAN_ID, DL_DMA_ADDR_UNCHANGED);
//    DL_DMA_setDestIncrement(DMA, DMA_CH0_CHAN_ID, DL_DMA_ADDR_INCREMENT);
//    DL_DMA_setSrcWidth(DMA, DMA_CH0_CHAN_ID, DL_DMA_WIDTH_HALF_WORD);
//    DL_DMA_setDestWidth(DMA, DMA_CH0_CHAN_ID, DL_DMA_WIDTH_HALF_WORD);

//    DL_DMA_enableChannel(DMA, DMA_CH0_CHAN_ID);

//    DL_ADC12_enableDMA(ADC12_0_INST);
//    //补全第二个入参：MEM0加载完成触发DMA
//    DL_ADC12_enableDMATrigger(ADC12_0_INST, DL_ADC12_DMA_MEM0_RESULT_LOADED);

//    NVIC_EnableIRQ(ADC12_0_INST_INT_IRQN);
//    DL_ADC12_startConversion(ADC12_0_INST);

//    while (1)
//    {
//        if(gDmaCompleteFlag)
//        {
//            gDmaCompleteFlag = false;

//            //=====处理 gADCSamples 1000点数据=====


//            DL_ADC12_startConversion(ADC12_0_INST);
//        }
//    }
//}

//void ADC12_0_INST_IRQHandler(void)
//{
//    switch (DL_ADC12_getPendingInterrupt(ADC12_0_INST))
//    {
//        case DL_ADC12_IIDX_DMA_DONE:
//            DL_ADC12_stopConversion(ADC12_0_INST);
//            gDmaCompleteFlag = true;
//            break;
//        default:
//            break;
//    }
//}



//------------------------------------ok----------------------------------------------------

//#include "ti/driverlib/dl_adc12.h"
//#include "ti_msp_dl_config.h"

//uint16_t gADCSamples[1000];

//int main(void)
//{
//    SYSCFG_DL_init();

//    //DL_DMA_setSrcAddr(DMA, DMA_CH0_CHAN_ID, (uint32_t)0x40556280);
//    DL_DMA_setSrcAddr(DMA, DMA_CH0_CHAN_ID, DL_ADC12_getMemResultAddress(ADC12_0_INST,DL_ADC12_MEM_IDX_0 ) );
//    
//    
//    DL_DMA_setDestAddr(DMA, DMA_CH0_CHAN_ID, (uint32_t)&gADCSamples[0]);
//    DL_DMA_enableChannel(DMA, DMA_CH0_CHAN_ID);

//    NVIC_EnableIRQ(ADC12_0_INST_INT_IRQN);
//    DL_ADC12_startConversion(ADC12_0_INST);

//    while (1)
//    {

//    }
//}

//void ADC12_0_INST_IRQHandler(void)
//{
//    switch (DL_ADC12_getPendingInterrupt(ADC12_0_INST))
//    {
//        case DL_ADC12_IIDX_DMA_DONE:
//            DL_ADC12_disableConversions(ADC12_0_INST);
//            //Process
//            __BKPT();
//            DL_ADC12_enableConversions(ADC12_0_INST);
//            DL_ADC12_startConversion(ADC12_0_INST);
//            break;
//        default:
//            break;
//    }
//}




//----------------------------------------------------------------------------------------


void SendString(char *str)
{
    while(*str != '\0')
    {
        DL_UART_Main_transmitDataBlocking(UART_0_INST, *str++);
    }
}



