#include "stm32f10x.h"
#include "metronome.h"

static GPIO_InitTypeDef GPIO_InitStructure;
static TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
//static uint8_t i4led = 0;
static uint8_t l2tell = 0;

/*void led_tog(void)
{
	i4led = ~i4led;
	if (i4led == 0){
		GPIO_ResetBits(GPIOA,GPIO_Pin_6);
	}
	else{
		GPIO_SetBits(GPIOA,GPIO_Pin_6);
	}
}*/

uint8_t time2tell(void)
{
	if(l2tell==1){
		l2tell = 0;
		return 1;
	}
	else{
		return 0;
	}
}

/*uint8_t getTimerStatus(void)
{
	if(l2tell==1)
		TIM_Cmd(TIM3,DISABLE);
	return l2tell;
}

void clrTimerStatus(void)
{
	l2tell = 0;
	TIM_Cmd(TIM3,ENABLE);
}*/

/****** Settings for LED1, LED2 ************/
void led_GPIO_cfg(void)
{
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0|GPIO_Pin_1|\
								GPIO_Pin_2|GPIO_Pin_3; //LED1 LED2
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA,&GPIO_InitStructure);
}

void mtn_TIM_cfg(void)
{
	/*****stm030f4 timers******************
	***APB1: TIM3,TIM14 ***********
	***APB2: TIM1,TIM16,TIM3  ***********/
	/*****stm030C8 timers******************
	***APB1: TIM3,TIM6,TIM14  ***********
	***APB2: TIM1,TIM16,TIM3 ***********/
	TIM_TimeBaseStructure.TIM_Prescaler = 3999; //1199; //4MHz/4000= 1KHz
	TIM_TimeBaseStructure.TIM_Period = 499; //1 second
	TIM_TimeBaseStructure.TIM_ClockDivision = 0;
  TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseInit(TIM3,&TIM_TimeBaseStructure);
	
	TIM_ARRPreloadConfig(TIM3,ENABLE);
	TIM_ITConfig(TIM3,TIM_IT_Update,ENABLE);
	TIM_Cmd(TIM3,ENABLE);
}

void TIM3_IRQHandler(void)
{
	if(TIM_GetITStatus(TIM3,TIM_IT_Update)== SET){
		TIM_ClearITPendingBit(TIM3,TIM_IT_Update);//clear interupt mark
		l2tell = 1;
	}

}
