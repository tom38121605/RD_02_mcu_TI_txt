


#include "ti_msp_dl_config.h"

#define DMA_TRANSFER_SIZE_WORDS (16)

const uint32_t gSrcData[DMA_TRANSFER_SIZE_WORDS] = {0x00000000, 0x10101010,
      0x20202020, 0x30303030, 0x40404040, 0x50505050, 0x60606060, 0x70707070,
      0x80808080, 0x90909090, 0xA0A0A0A0, 0xB0B0B0B0, 0xC0C0C0C0, 0xD0D0D0D0,
      0xE0E0E0E0, 0xF0F0F0F0};
uint32_t gDstData[DMA_TRANSFER_SIZE_WORDS];

volatile bool flg_dmairq = false;
volatile bool gVerifyResult           = false;


int main(void)
{

      SYSCFG_DL_init();
   
      //DL_GPIO_setPins( LED_PORT, LED_PIN_0_PIN);


      NVIC_EnableIRQ(DMA_INT_IRQn);


      DL_DMA_setSrcAddr(DMA, DMA_CH0_CHAN_ID, (uint32_t) &gSrcData[0]);
      DL_DMA_setDestAddr(DMA, DMA_CH0_CHAN_ID, (uint32_t) &gDstData[0]);
      
      DL_DMA_setTransferSize( DMA, DMA_CH0_CHAN_ID, sizeof(gSrcData) / sizeof(uint32_t) );
      DL_DMA_enableChannel(DMA, DMA_CH0_CHAN_ID);


      flg_dmairq = false;
      DL_DMA_startTransfer(DMA, DMA_CH0_CHAN_ID);

      while (flg_dmairq == false) 
      {
            ;//__WFE();
      }

      gVerifyResult = true;
      for (int i = 0; i < DMA_TRANSFER_SIZE_WORDS; i++) 
      {
            gVerifyResult &= gSrcData[i] == gDstData[i];
      }

      //if(gVerifyResult)
      //    DL_GPIO_clearPins( LED_PORT, LED_PIN_0_PIN);


      __BKPT(0);

      while (1) 
      {
          ;//__WFI();
      }
      
}

void DMA_IRQHandler(void)
{
      /* Example interrupt code -- just used to break the WFI in this example */
      switch (DL_DMA_getPendingInterrupt(DMA)) {
            case DL_DMA_EVENT_IIDX_DMACH0:
                  flg_dmairq = true;
                  break;
            default:
                  break;
      }
}

