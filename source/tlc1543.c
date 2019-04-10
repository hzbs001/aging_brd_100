#include "stm32f10x.h"
#include "tlc1543.h"
#define SPI1_SCL_CLR	GPIO_ResetBits(GPIOA,GPIO_Pin_5)
#define SPI1_SCL_SET	GPIO_SetBits(GPIOA,GPIO_Pin_5)

#define SPI1_OUT_CLR	GPIO_ResetBits(GPIOA,GPIO_Pin_7)
#define SPI1_OUT_SET	GPIO_SetBits(GPIOA,GPIO_Pin_7)

#define SPI1_READ_BIT GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_6)

static GPIO_InitTypeDef GPIO_InitStructure;

static void delay_AD()
{
	__NOP();__NOP();__NOP();__NOP();
	__NOP();__NOP();__NOP();__NOP();
	__NOP();__NOP();__NOP();__NOP();
	__NOP();__NOP();__NOP();__NOP();
}

static void delay_tconv_AD()
{
	static uint8_t i;
	for(i=0;i<22;i++){
		__NOP();__NOP();__NOP();__NOP();
		__NOP();__NOP();__NOP();__NOP();
	}
}
// Write address code and read last datas(one channel)
static uint8_t tlc1543_cfg_rd(uint8_t addr,uint16_t *pADC)
{
	static uint8_t i,mask;
	static uint16_t received=0;
	static uint8_t tempaddr;
	tempaddr = addr;
	tempaddr <<= 4;
	for(i=0;i<10;i++){
		SPI1_SCL_CLR;
		delay_AD();
		
		(tempaddr&0x80)?SPI1_OUT_SET:SPI1_OUT_CLR;
		received |= SPI1_READ_BIT;
		delay_AD();
		
		SPI1_SCL_SET;
		delay_AD();
		
		received <<= 1;
		tempaddr <<= 1;	
	}
	
	received >>= 1;
	*pADC = received;
	received = 0;
	
	return 1;
}
//select tlc1543
static uint8_t tlc1543_cs(uint8_t ichip)
{
	static uint16_t tchip;
	tchip = 0x0001 ;
	tchip <<= ichip;
	
	(tchip&0x0001)?GPIO_ResetBits(GPIOE,GPIO_Pin_8):GPIO_SetBits(GPIOE,GPIO_Pin_8);
	(tchip&0x0002)?GPIO_ResetBits(GPIOE,GPIO_Pin_9):GPIO_SetBits(GPIOE,GPIO_Pin_9);
	(tchip&0x0004)?GPIO_ResetBits(GPIOE,GPIO_Pin_10):GPIO_SetBits(GPIOE,GPIO_Pin_10);
	(tchip&0x0008)?GPIO_ResetBits(GPIOE,GPIO_Pin_11):GPIO_SetBits(GPIOE,GPIO_Pin_11);
			
	(tchip&0x0010)?GPIO_ResetBits(GPIOE,GPIO_Pin_12):GPIO_SetBits(GPIOE,GPIO_Pin_12);
	(tchip&0x0020)?GPIO_ResetBits(GPIOE,GPIO_Pin_13):GPIO_SetBits(GPIOE,GPIO_Pin_13);
	(tchip&0x0040)?GPIO_ResetBits(GPIOE,GPIO_Pin_14):GPIO_SetBits(GPIOE,GPIO_Pin_14);
	(tchip&0x0080)?GPIO_ResetBits(GPIOE,GPIO_Pin_15):GPIO_SetBits(GPIOE,GPIO_Pin_15);
	
	(tchip&0x0100)?GPIO_ResetBits(GPIOB,GPIO_Pin_10):GPIO_SetBits(GPIOB,GPIO_Pin_10);
	(tchip&0x0200)?GPIO_ResetBits(GPIOB,GPIO_Pin_11):GPIO_SetBits(GPIOB,GPIO_Pin_11);
	delay_AD();
	delay_AD();
	return 1;
}
//convert 10 channel of tlc1543,change tlc1543 every time
uint8_t tlc1543(uint16_t *pADC)
{
	static uint8_t iaddr = 0;
	static uint8_t ichip = 0;
	static uint8_t nchip = 12;//No chip selected
	
	static uint16_t temp;
	
	tlc1543_cs(nchip);
	tlc1543_cs(ichip);
	
	for(iaddr=0;iaddr<11;iaddr++){
		tlc1543_cs(nchip);
		tlc1543_cs(ichip);
		if(iaddr==0){
			tlc1543_cfg_rd(iaddr,&temp);
			delay_tconv_AD();
		}
		else{
			tlc1543_cfg_rd(iaddr,&pADC[ichip*10+iaddr-1]);
			delay_tconv_AD();delay_tconv_AD();
		}

	}
	
	tlc1543_cs(nchip);
	
	if(ichip<10)
		ichip++;
	else
		ichip = 0;
	return 1;
}

void tlc1543_GPIO_cfg(void)
{
	//SPI1
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5|GPIO_Pin_7;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA,&GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA,&GPIO_InitStructure);
	//CS PE8~PE15
	GPIO_InitStructure.GPIO_Pin = 0xFF00;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOE,&GPIO_InitStructure);
	
	//CS PB10~PB11
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10|GPIO_Pin_11;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB,&GPIO_InitStructure);
	//Initilize CS = logic high
	GPIO_SetBits(GPIOE,GPIO_Pin_8);
	GPIO_SetBits(GPIOE,GPIO_Pin_9);
	GPIO_SetBits(GPIOE,GPIO_Pin_10);
	GPIO_SetBits(GPIOE,GPIO_Pin_11);
	GPIO_SetBits(GPIOE,GPIO_Pin_12);
	GPIO_SetBits(GPIOE,GPIO_Pin_13);
	GPIO_SetBits(GPIOE,GPIO_Pin_14);
	GPIO_SetBits(GPIOE,GPIO_Pin_15);
	
	GPIO_SetBits(GPIOB,GPIO_Pin_10);
	GPIO_SetBits(GPIOB,GPIO_Pin_11);
	
}

