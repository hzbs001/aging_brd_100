//#include "stdint.h"
#include "stm32f10x.h"

#include "user_init.h"

#include "mbport.h"
#include "mb.h"

#include "adc.h"

#include "metronome.h"
#include "pmu.h"
#include "tlc1543.h"

#define TC_CAL1            ((uint16_t *)0x1FFFF7B8)
#define VREFINT_CAL            ((uint16_t *)0x1FFFF7BA)

/* ----------------------- Defines ------------------------------------------*/
//const uint8_t REG_INPUT_START = 0;
//const uint8_t REG_INPUT_NREGS = 8;
const uint8_t REG_HOLDING_START = 0;
//const uint8_t REG_HOLDING_NREGS = 16;
/* ----------------------- Static variables ---------------------------------*/
//static uint16_t   usRegInputStart = REG_INPUT_START;
//static uint16_t   usRegInputBuf[REG_INPUT_NREGS];
static uint16_t   usRegHoldingStart = REG_HOLDING_START;
static uint16_t   usRegHoldingBuf[REG_HOLDING_NREGS];
/* ----------------------- Start implementation -----------------------------*/
int main( void )
{
	//clock settings and interrupt management
	RCC_cfg();
	NVIC_cfg();
	
	//For ADC
	//adc_GPIO_cfg();
	//ADC_cfg();
	//DMA_cfg();
	
	//metronome
	mtn_TIM_cfg();
	sw_TIM_cfg();
	
	//ADC_SoftwareStartConvCmd(ADC1,ENABLE);
	
	//For Modbus
	mb_TIM_cfg();
	mb_USART_cfg();
	//led_GPIO_cfg();
	
	plug_GPIO_cfg();
	tlc1543_GPIO_cfg();
	
	setMBAddr(88);
	
	usRegHoldingBuf[8] = 0x8888;
	
	while(1) {
		mb_Service();		//modbus service
		//adcDataProcess(usRegHoldingBuf);
		if(time2tell()) {
			if(plugDetect()==SET) {
				ctrPWR(ENABLE);
				tlc1543(usRegHoldingBuf);
			}
			else {
				ctrPWR(DISABLE);
			}
		}
  }
}

void mbRegHolding(uint8_t *p,uint16_t regAddr,uint16_t regNum,RD_WR mode)
{
	uint16_t i;
	uint16_t iRegIndex;
	if((regAddr >= REG_HOLDING_START) && ((regAddr+regNum) <= (REG_HOLDING_START+REG_HOLDING_NREGS))){
		iRegIndex = (uint16_t)(regAddr-usRegHoldingStart);
		
		switch(mode)
		{
			case READ:
			{
				for(i=0;i<regNum;i++) {
					*p++ = (uint8_t)(usRegHoldingBuf[iRegIndex+i] >> 8 );  //High byte first
					*p++ = (uint8_t)(usRegHoldingBuf[iRegIndex+i] & 0xFF); //low byte first
				}
			}
			break;
			case WRITE:
			{
				for(i=0;i<regNum;i++) {
					*(usRegHoldingBuf+iRegIndex+i)= ((*p++))<<8; 	//High byte
					*(usRegHoldingBuf+iRegIndex+i)|= *(p++); 		//Low byte
				}

			}	
		}
	}
}

void updateADCValue(uint16_t value,uint8_t ich)
{
	*(usRegHoldingBuf+ich) = value;
}
