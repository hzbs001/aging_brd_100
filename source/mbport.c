//#include "stm32f0xx_it.h"
#include "stm32f10x.h"
#include "mb.h"
#include "mbport.h"

static USART_InitTypeDef USART_InitStructure;
static TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
static GPIO_InitTypeDef GPIO_InitStructure;

const static uint32_t MB_Baudrate=115200;//Baudrate
const static uint8_t MB_Parity=0;
static uint16_t MB_TX_EN=0;
static uint8_t MB_TX_EXPIRED = 0;
void delay_hfms(uint16_t time)
{
	uint16_t i = 4999;
	while(time--){
		while(i--);
	}
}

//////////////////////////////////////////////////////////////////////////////
//发送n个字节数据
//buff:发送区首地址
//len：发送的字节数
void mb_SendData(uint8_t *buff,uint8_t len)
{ 
    MB_TX_EN=1;//Sending mode

    while(len--){
      while(USART_GetFlagStatus(USART1,USART_FLAG_TXE)==RESET);//等待发送区为空

      USART_SendData(USART1,*(buff++));
    }
    while(USART_GetFlagStatus(USART1,USART_FLAG_TC)==RESET);//等待发送完成
	
		MB_TX_EN=0;//Receiving mode
}

void mb_receiveData(uint8_t *buff)
{
	*buff = USART_ReceiveData(USART1);
}
void mb_TIM_cfg(void)
{
	TIM_TimeBaseStructure.TIM_Prescaler = 399; //1199; //4MHz/400= 10KHz
	TIM_TimeBaseStructure.TIM_Period = 45; //4.5m second
	TIM_TimeBaseStructure.TIM_ClockDivision = 0;
  TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseInit(TIM2,&TIM_TimeBaseStructure);
	
	TIM_ARRPreloadConfig(TIM2,ENABLE);
	TIM_ITConfig(TIM2,TIM_IT_Update,ENABLE);
}
void mb_USART_cfg(void)
{
	//GPIO_PinRemapConfig(GPIO_Remap_USART1,ENABLE);	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9; 
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	//GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA,&GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10; 
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	//GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA,&GPIO_InitStructure);
	
	USART_InitStructure.USART_BaudRate = MB_Baudrate;
  USART_InitStructure.USART_WordLength = USART_WordLength_8b;
  USART_InitStructure.USART_StopBits = USART_StopBits_1;
  USART_InitStructure.USART_Parity = MB_Parity;
  USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
  USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
	USART_Init(USART1,&USART_InitStructure);
	
	USART_ITConfig(USART1,USART_IT_RXNE,ENABLE);
	//ART_ITConfig(USART1,USART_IT_TXE,ENABLE);
	USART_ClearFlag(USART1,USART_FLAG_TC);
	USART_Cmd(USART1,ENABLE);
}

/*********Interupt processing************/
void TIM2_IRQHandler(void)
{
	if(TIM_GetITStatus(TIM2,TIM_IT_Update)== SET) {
      TIM_ClearITPendingBit(TIM2,TIM_IT_Update);//clear interupt mark
      TIM_Cmd(TIM2,DISABLE);//stop timer
	
			mbRcvComplete();//We can shutdown receive mode at this place.
    }
}

/*********Interupt processing************/
void USART1_IRQHandler(void)
{
  if(USART_GetITStatus(USART1,USART_IT_RXNE)== SET) {
      //if(USART_GetFlagStatus(USART1,USART_FLAG_NE|USART_FLAG_FE|USART_FLAG_PE)) err=1;//检测到噪音、帧错误或校验错误
      //else err=0;
			USART_ClearITPendingBit(USART1, USART_IT_RXNE);
      //USART_SendData(USART1,0x58);
			mbRcvProcess();
      TIM_ClearITPendingBit(TIM2,TIM_IT_Update);//清除定时器溢出中断
      TIM_SetCounter(TIM2,0);//当接收到一个新的字节，将定时器17复位为0，重新计时（相当于喂狗）
      TIM_Cmd(TIM2,ENABLE);//开始计时
  }
}
