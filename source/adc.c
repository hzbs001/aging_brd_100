#include "stm32f10x.h"
#include "adc.h"
//#define ADC1_DR_Address    0x40012440
#define TC_CAL1            ((uint16_t *)0x1FFFF7B8)
#define VREFINT_CAL        ((uint16_t *)0x1FFFF7BA)

union pfloat{
	float a;
	uint8_t b[4];
};

static const uint8_t buf_len = 2;
static uint16_t dma4adc[buf_len];
static uint8_t dma_complete_flag;
static uint8_t iconv = 0;

static ADC_InitTypeDef ADC_InitStructure;
static DMA_InitTypeDef DMA_InitStructure;
static GPIO_InitTypeDef GPIO_InitStructure;

static uint16_t adcValue,tempValue,vrefValue;

union pfloat prcv[16];

static uint8_t i = 0;
static uint16_t rs0 = 510;
static uint16_t rl = 510;
	
/*********DMA Complete flag set ********/
void setDmaCompleteFlag(void)
{
	dma_complete_flag=1;
}

/**********adc data processing*************/
/*****channel1 with 16bits
******temperature with 12bits
******reference voltage1.2V with 16bits
--by lwang @home 2018/03/12 
******************************************/
float adcDataProcess(uint16_t *pADC)
{
	uint32_t tmp1=0,tmp2=0,tmp3=0;
	uint16_t i,tDREF,vDREF;
	float vREF,rs;
	
	union pfloat temp,adc1,iref;

	if(dma_complete_flag == 1)
		{
			dma_complete_flag = 0;
			//tDREF = *TC_CAL1;
			//vDREF = *VREFINT_CAL;
			
			*pADC++ = dma4adc[0]&0xFFF;
			*pADC = dma4adc[1]&0xFFF;
			
			return tmp1;
		}
}


/*void adcData4Brc(uint16_t *pADC,uint8_t count)
{
	union pfloat adc1;
	//using usRegHoldingBuf[3]
	adc1.a = adcValue*1.2/vrefValue; 
	*pADC++ = (adc1.b[3]<<8) | adc1.b[2];
	*pADC = (adc1.b[1]<<8) | adc1.b[0];;

}*/

void adc_GPIO_cfg(void)
{

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0|GPIO_Pin_1|\
								GPIO_Pin_2|GPIO_Pin_3|\
								GPIO_Pin_4|GPIO_Pin_5|\
								GPIO_Pin_6|GPIO_Pin_7; //MUX CTRL
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA,&GPIO_InitStructure);

	/*********** Settings for ADC *****************/
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1; 
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB,&GPIO_InitStructure);
	
	(iconv&0x01)?GPIO_ResetBits(GPIOA,GPIO_Pin_0):GPIO_SetBits(GPIOA,GPIO_Pin_0);
	(iconv&0x02)?GPIO_ResetBits(GPIOA,GPIO_Pin_1):GPIO_SetBits(GPIOA,GPIO_Pin_1);
	(iconv&0x04)?GPIO_ResetBits(GPIOA,GPIO_Pin_2):GPIO_SetBits(GPIOA,GPIO_Pin_2);
	(iconv&0x08)?GPIO_ResetBits(GPIOA,GPIO_Pin_3):GPIO_SetBits(GPIOA,GPIO_Pin_3);
			
	(iconv&0x01)?GPIO_ResetBits(GPIOA,GPIO_Pin_4):GPIO_SetBits(GPIOA,GPIO_Pin_4);
	(iconv&0x02)?GPIO_ResetBits(GPIOA,GPIO_Pin_5):GPIO_SetBits(GPIOA,GPIO_Pin_5);
	(iconv&0x04)?GPIO_ResetBits(GPIOA,GPIO_Pin_6):GPIO_SetBits(GPIOA,GPIO_Pin_6);
	(iconv&0x08)?GPIO_ResetBits(GPIOA,GPIO_Pin_7):GPIO_SetBits(GPIOA,GPIO_Pin_7);
}

/*************ADC conversion config********************/
/********Turn on CH1,CH16,CH17*************************/
/********Measure external,temp,vref********************/
void ADC_cfg()
{
	ADC_InitStructure.ADC_Mode = ADC_Mode_Independent;
	ADC_InitStructure.ADC_ScanConvMode = ENABLE;
	ADC_InitStructure.ADC_ContinuousConvMode = ENABLE;
	ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;
	ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;
	ADC_InitStructure.ADC_NbrOfChannel = 2;
	ADC_Init(ADC1, &ADC_InitStructure);
	
	
	ADC_RegularChannelConfig(ADC1,ADC_Channel_8,1,ADC_SampleTime_239Cycles5);
	ADC_RegularChannelConfig(ADC1,ADC_Channel_9,2,ADC_SampleTime_239Cycles5);
	
	//ADC_ITConfig(ADC1,ADC_IT_EOC,ENABLE);
	ADC_DMACmd(ADC1,ENABLE);
	ADC_Cmd(ADC1,ENABLE);
	//Calibrate
	ADC_ResetCalibration(ADC1);
	while (ADC_GetResetCalibrationStatus(ADC1));
	ADC_StartCalibration(ADC1);
	while (ADC_GetCalibrationStatus(ADC1));
	
	//while(!ADC_GetFlagStatus(ADC1, ADC_FLAG_EOC)); 
	
}
/***************DMA control,256 data to average**********/
void DMA_cfg()
{
	DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)(&(ADC1->DR));
	DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)dma4adc;
	DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;
  DMA_InitStructure.DMA_BufferSize = buf_len;
  DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
  DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
  DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;
  DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;
  DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;
  DMA_InitStructure.DMA_Priority = DMA_Priority_High;
	DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;
	DMA_Init(DMA1_Channel1, &DMA_InitStructure);
	
	DMA_ITConfig(DMA1_Channel1,DMA_IT_TC,ENABLE);
	DMA_Cmd(DMA1_Channel1, ENABLE);
}

/********ADC interupt processing*****************/
void ADC1_2_IRQHandler(void)
{
	
}
/********DMA interupt processing*****************/
void DMA1_Channel1_IRQHandler(void)
{

}

