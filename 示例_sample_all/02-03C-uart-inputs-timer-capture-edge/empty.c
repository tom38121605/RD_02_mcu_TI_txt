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

#include "ti_msp_dl_config.h"

#include "stdio.h"
#include "string.h"


void SendString(char *str);
char txBuf[100];

volatile uint8_t gEchoData = 0;

void delay_ms(uint32_t ms)
{
    while(ms--)
    {
        delay_cycles(CPUCLK_FREQ/1000);
    }
}



volatile uint32_t gCaptureCnt;
volatile bool gSynced;
volatile bool gCheckCaptures;
uint32_t gLoadValue;


   

int main(void)
{
    volatile static uint32_t pwmPeriod;

    SYSCFG_DL_init();

    gLoadValue = DL_TimerG_getLoadValue(CAPTURE_0_INST);

    gSynced        = false;
    gCheckCaptures = false;


    DL_TimerG_setCoreHaltBehavior( CAPTURE_0_INST, DL_TIMER_CORE_HALT_IMMEDIATE);
    DL_TimerG_setCoreHaltBehavior(PWM_0_INST, DL_TIMER_CORE_HALT_IMMEDIATE);

    NVIC_EnableIRQ(CAPTURE_0_INST_INT_IRQN);
    DL_TimerG_startCounter(CAPTURE_0_INST);
    DL_TimerG_startCounter(PWM_0_INST);

    while (1) 
    {
    
        delay_ms(1000);
        while (false == gCheckCaptures) ;

        gCheckCaptures = false;

        pwmPeriod = gLoadValue - gCaptureCnt;
        
        sprintf(txBuf, "Period %d\r\n", pwmPeriod);
        SendString(txBuf);           
        
    }
}

void CAPTURE_0_INST_IRQHandler(void)
{
    switch (DL_TimerG_getPendingInterrupt(CAPTURE_0_INST)) 
    {
        case DL_TIMERG_IIDX_CC0_DN:
        
            if (gSynced == true)   //第3个是pwm的下降沿，   
            {
                gCaptureCnt = DL_TimerG_getCaptureCompareValue(CAPTURE_0_INST, DL_TIMER_CC_0_INDEX);
                gCheckCaptures = true;
            } 
            
            else     //第1个是pwm的下降沿做同步信号     （第2个是pwm的上升沿，不会触发中断，但会拍照计数值到CC0）     
             
                gSynced = true;
            
            
            DL_TimerG_setTimerCount(CAPTURE_0_INST, gLoadValue);   // 收到同步信号后（或者完成一个pwm周期后），重新装载。 
            
            break;
            
            
        case DL_TIMERG_IIDX_ZERO:

            gSynced = false;    //没有检测到pwm信号
            break;
            
        default:
            break;
    }
}

void UART_0_INST_IRQHandler(void)
{
    switch (DL_UART_Main_getPendingInterrupt(UART_0_INST)) 
    {
        case DL_UART_MAIN_IIDX_RX:

            gEchoData = DL_UART_Main_receiveData(UART_0_INST);
            DL_UART_Main_transmitData(UART_0_INST, gEchoData);
            break;
        default:
            break;
    }
}


void SendString(char *str)
{
    while(*str != '\0')
    {
        DL_UART_Main_transmitDataBlocking(UART_0_INST, *str++);
    }
}
