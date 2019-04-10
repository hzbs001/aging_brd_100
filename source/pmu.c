#include "stm32f10x.h"
#include "pmu.h"

static GPIO_InitTypeDef GPIO_InitStructure;
static TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
static uint8_t pwr_ch = 0;
static uint8_t swFlag = 0;

uint8_t plugDetect()
{
	uint8_t plugState = RESET;
	if ((GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_8)==0)&& \
		(GPIO_ReadInputDataBit(GPIOC,GPIO_Pin_4)==0))
	{
		plugState = SET;
	}
	else
	{
		plugState = RESET;
	}
	//plugState = GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_8);
	//plugState <<= 1;
	//plugState |= GPIO_ReadInputDataBit(GPIOC,GPIO_Pin_4);
	return plugState;
}

void ctrPWR(uint8_t ctrState)
{
	/********if connected, power on/off every minute*****
	 ctrState is determined by connecting state
	 swFlag is determined by timer
	******/
	//Control 5V powr
	if(ctrState==ENABLE)
	{
		GPIO_SetBits(GPIOA,GPIO_Pin_4);
	}
	else
	{
		GPIO_ResetBits(GPIOA,GPIO_Pin_4);
	}
	//Control heating power
	if((ctrState==ENABLE)&&swFlag)
	{
		switch (pwr_ch)
		{
			case 0:GPIO_SetBits(GPIOD,GPIO_Pin_1);break;
			case 1:GPIO_SetBits(GPIOD,GPIO_Pin_0);break;
			case 2:GPIO_SetBits(GPIOC,GPIO_Pin_12);break;
			case 3:GPIO_SetBits(GPIOC,GPIO_Pin_11);break;
			case 4:GPIO_SetBits(GPIOC,GPIO_Pin_10);break;
			default:break;
		}
		if(pwr_ch<5)
			pwr_ch++;
	}
	else
	{
		pwr_ch = 0;
		GPIO_ResetBits(GPIOD,GPIO_Pin_1);
		GPIO_ResetBits(GPIOD,GPIO_Pin_0);
		GPIO_ResetBits(GPIOC,GPIO_Pin_12);
		GPIO_ResetBits(GPIOC,GPIO_Pin_11);
		GPIO_ResetBits(GPIOC,GPIO_Pin_10);
	}
}

void plug_GPIO_cfg(void)
{
	//PA8
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8;
	//GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA,&GPIO_InitStructure);
	//PC4
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOC,&GPIO_InitStructure);
	//PD0,PD1
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0|GPIO_Pin_1;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOD,&GPIO_InitStructure);
	//PC10,PC11,PC12
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10|GPIO_Pin_11|GPIO_Pin_12;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOC,&GPIO_InitStructure);
	//PA4
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA,&GPIO_InitStructure);
}

void sw_TIM_cfg(void)
{
	TIM_TimeBaseStructure.TIM_Prescaler = 39999; //4MHz/40000= 100Hz
	TIM_TimeBaseStructure.TIM_Period = 11999; //60 second
	TIM_TimeBaseStructure.TIM_ClockDivision = 0;
  TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseInit(TIM4,&TIM_TimeBaseStructure);
	
	TIM_ARRPreloadConfig(TIM4,ENABLE);
	TIM_ITConfig(TIM4,TIM_IT_Update,ENABLE);
	TIM_Cmd(TIM4,ENABLE);
}

void TIM4_IRQHandler(void)
{
	if(TIM_GetITStatus(TIM4,TIM_IT_Update)== SET){
		TIM_ClearITPendingBit(TIM4,TIM_IT_Update);//clear interupt mark
		//l2tell = 1;
		swFlag = !swFlag;
	}

}
