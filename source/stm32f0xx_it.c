/**
  ******************************************************************************
  * @file    ADC/ADC_DMA/stm32f0xx_it.c 
  * @author  MCD Application Team
  * @version V1.4.0
  * @date    24-July-2014
  * @brief   Main Interrupt Service Routines.
  *          This file provides template for all exceptions handler and 
  *          peripherals interrupt service routine.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; COPYRIGHT 2014 STMicroelectronics</center></h2>
  *
  * Licensed under MCD-ST Liberty SW License Agreement V2, (the "License");
  * You may not use this file except in compliance with the License.
  * You may obtain a copy of the License at:
  *
  *        http://www.st.com/software_license_agreement_liberty_v2
  *
  * Unless required by applicable law or agreed to in writing, software 
  * distributed under the License is distributed on an "AS IS" BASIS, 
  * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  * See the License for the specific language governing permissions and
  * limitations under the License.
  *
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "stm32f0xx_it.h"
#include "mb.h"

/** @addtogroup STM32F0xx_StdPeriph_Examples
  * @{
  */

/** @addtogroup ADC_DMA
  * @{
  */

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/

/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/******************************************************************************/
/*            Cortex-M0 Processor Exceptions Handlers                         */
/******************************************************************************/

/**
  * @brief  This function handles NMI exception.
  * @param  None
  * @retval None
  */
	/*
void NMI_Handler(void)
{
}
*/
/**
  * @brief  This function handles Hard Fault exception.
  * @param  None
  * @retval None
  */
/*
void HardFault_Handler(void)
{
  //Go to infinite loop when Hard Fault exception occurs 
  while (1)
  {
  }
}
*/
/**
  * @brief  This function handles SVCall exception.
  * @param  None
  * @retval None
  */
/*void SVC_Handler(void)
{
}*/

/**
  * @brief  This function handles PendSVC exception.
  * @param  None
  * @retval None
  */
/*
void PendSV_Handler(void)
{
}
*/
/**
  * @brief  This function handles SysTick Handler.
  * @param  None
  * @retval None
  */
	/*
void SysTick_Handler(void)
{
}
*/
/******************************************************************************/
/*                 STM32F0xx Peripherals Interrupt Handlers                   */
/*  Add here the Interrupt Handler for the used peripheral(s) (PPP), for the  */
/*  available peripheral interrupt handler's name please refer to the startup */
/*  file (startup_stm32f0xx.s).                                               */
/******************************************************************************/


void TIM14_IRQHandler(void)
{
	if(TIM_GetITStatus(TIM14,TIM_IT_Update)== SET)
    {
      TIM_ClearITPendingBit(TIM14,TIM_IT_Update);//clear interupt mark
      TIM_Cmd(TIM14,DISABLE);//stop timer
			
			mbRcvComplete();
    }
		
}

void USART1_IRQHandler(void)
{
	uint8_t res;
  uint8_t err;
	//mb_CtrMap(MB_RX_BUFF,&MB_RX_CNT,&MB_FrameFlag,&MB_TX_EN);
  if(USART_GetITStatus(USART1,USART_IT_RXNE)== SET)
  {
      //if(USART_GetFlagStatus(USART1,USART_FLAG_NE|USART_FLAG_FE|USART_FLAG_PE)) err=1;//检测到噪音、帧错误或校验错误
      //else err=0;
			USART_ClearITPendingBit(USART1, USART_IT_RXNE);
      
			mbRcvProcess();
      TIM_ClearITPendingBit(TIM14,TIM_IT_Update);//清除定时器溢出中断
      TIM_SetCounter(TIM14,0);//当接收到一个新的字节，将定时器7复位为0，重新计时（相当于喂狗）
      TIM_Cmd(TIM14,ENABLE);//开始计时
  }
}
/*****
void ADC1_IRQHandler(void)
{
	
}

void DMA1_Channel1_IRQHandler(void)
{
	if(DMA_GetFlagStatus(DMA1_FLAG_TC1) != RESET)
	{
		DMA_ClearITPendingBit(DMA1_IT_TC1);
		setDmaCompleteFlag();
	}
}
*/
/**
  * @brief  This function handles PPP interrupt request.
  * @param  None
  * @retval None
  */
/*void PPP_IRQHandler(void)
{
}*/

/**
  * @}
  */

/**
  * @}
  */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
