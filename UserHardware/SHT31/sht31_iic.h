#ifndef _SHT31_H
#define _SHT31_H
#include "stm32l1xx_hal.h"

//////////////////////////////////////////////////////////////////////////////////	 
//本程序只供学习使用，未经作者许可，不得用于其它任何用途
//ALIENTEK STM32F103开发板
//IIC驱动代码	   
//正点原子@ALIENTEK
//技术论坛:www.openedv.com
//创建日期:2017/6/13
//版本：V1.0
//版权所有，盗版必究。
//Copyright(C) 广州市星翼电子科技有限公司 2014-2024
//All rights reserved									  
////////////////////////////////////////////////////////////////////////////////// 	
//IO方向设置

//#define SHT31_SDA_IN()  {GPIOB->MODER&=~(3<<(11*2));GPIOB->MODER|=(0<<(11*2));}	//PB7输入模式
//#define SHT31_SDA_OUT() {GPIOB->MODER&=~(3<<(11*2));GPIOB->MODER|=(1<<(11*2));}	//PB7输入模式


#define SHT31_SDA_IN()  do{GPIOA->MODER&=~(3<<(12*2));GPIOA->MODER|=(0<<(12*2));}while(0)	//PB7输入模式
#define SHT31_SDA_OUT() do{GPIOA->MODER&=~(3<<(12*2));GPIOA->MODER|=(1<<(12*2));}while(0)	//PB7输入模式

//#define SHT31_SCL_Pin GPIO_PIN_10
//#define SHT31_SCL_GPIO_Port GPIOB
//#define SHT31_SDA_Pin GPIO_PIN_11
//#define SHT31_SDA_GPIO_Port GPIOB

#define SHT31_SCL_Pin GPIO_PIN_11
#define SHT31_SCL_GPIO_Port GPIOA
#define SHT31_SDA_Pin GPIO_PIN_12
#define SHT31_SDA_GPIO_Port GPIOA

typedef union 
{
    float fdata;
    unsigned char cdata[4];
}float_data;//定义联合体存储float数据，float类型的存储符合IEEE标准，可用于传输数据
typedef struct
{
	float temperature;
	float humidity;
}SHT31_DATA;

#define SHT31_READ 0X01
#define SHT31_WRITE 0X00 

#define SHT31_DEFAULT_ADDR    0x44
#define SHT31_MEAS_HIGHREP_STRETCH 0x2C06
#define SHT31_MEAS_MEDREP_STRETCH  0x2C0D
#define SHT31_MEAS_LOWREP_STRETCH  0x2C10
#define SHT31_MEAS_HIGHREP         0x2400
#define SHT31_MEAS_MEDREP          0x240B
#define SHT31_MEAS_LOWREP          0x2416
#define SHT31_READSTATUS           0xF32D
#define SHT31_CLEARSTATUS          0x3041
#define SHT31_SOFTRESET            0x30A2
#define SHT31_HEATEREN             0x306D
#define SHT31_HEATERDIS            0x3066


//IIC所有操作函数
void SHT31_IIC_Init(void);                //初始化IIC的IO口				 
static void SHT31_IIC_Start(void);				//发送IIC开始信号
static void SHT31_IIC_Stop(void);	  			//发送IIC停止信号
static void SHT31_IIC_Send_Byte(uint8_t txd);			//IIC发送一个字节
static uint8_t SHT31_IIC_Read_Byte(unsigned char ack);//IIC读取一个字节
static uint8_t I7020_IIC_Wait_Ack(void); 				//IIC等待ACK信号
static void SHT31_IIC_Ack(void);					//IIC发送ACK信号
static void SHT31_IIC_NAck(void);				//IIC不发送ACK信号

void SHT31_SingleShot(uint8_t *buffer);
uint8_t SHT31_ReadStatus(void);
void SHT31_ClearStatuse(void);
void SHT31_GetStatus(uint8_t *buffer);
void SHT31_SoftReset(void);
void SHT31_SendByte(uint16_t cmd,uint8_t stop);
void SHT31_ReadTH(SHT31_DATA *TH);
#endif

