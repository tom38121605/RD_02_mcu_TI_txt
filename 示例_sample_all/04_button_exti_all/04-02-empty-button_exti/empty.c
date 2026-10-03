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

volatile uint8_t flg_key1 =0;


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
    
    NVIC_EnableIRQ(BUTTON_0_INT_IRQN);
    

    while (1) 
    {
        uint32_t AAA;
        
        if(flg_key1==1)
        {
            flg_key1=0;
            
            AAA = DL_GPIO_readPins(BUTTON_0_PORT, BUTTON_0_PIN_1_PIN);
            if(!AAA)
            {
              delay_ms(10);
              
              AAA = DL_GPIO_readPins(BUTTON_0_PORT, BUTTON_0_PIN_1_PIN);
              if(!AAA)
                  DL_GPIO_togglePins(LED_PORT, LED_PIN_0_PIN);
             }
        }
    }
    
}


void GROUP1_IRQHandler(void)
{

    if(DL_Interrupt_getStatusGroup(DL_INTERRUPT_GROUP_1, DL_INTERRUPT_GROUP1_GPIOB))
    {
        DL_Interrupt_clearGroup(DL_INTERRUPT_GROUP_1, DL_INTERRUPT_GROUP1_GPIOB); //清除中断标志
        
        if(!DL_GPIO_readPins(BUTTON_0_PORT, BUTTON_0_PIN_1_PIN))    //PB21
           flg_key1 = 1;
    }
}


