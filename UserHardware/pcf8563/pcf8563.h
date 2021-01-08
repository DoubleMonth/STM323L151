#ifndef _PCF8563_H
#define _PCF8563_H
#include "stm32l1xx_hal.h"
//////////////////////////////////////////////////////////////////////////////////	 
//������ֻ��ѧϰʹ�ã�δ��������ɣ��������������κ���;
//ALIENTEK STM32F103������
//IIC��������	   
//����ԭ��@ALIENTEK
//������̳:www.openedv.com
//��������:2017/6/13
//�汾��V1.0
//��Ȩ���У�����ؾ���
//Copyright(C) ������������ӿƼ����޹�˾ 2014-2024
//All rights reserved									  
////////////////////////////////////////////////////////////////////////////////// 	
//IO��������  MODER

#define SDA_IN()  do{GPIOB->MODER&=~(3<<(1*2));GPIOB->MODER|=(0<<(1*2));}while(0)	//PB7����ģʽ
#define SDA_OUT() do{GPIOB->MODER&=~(3<<(1*2));GPIOB->MODER|=(1<<(1*2));}while(0)	//PB7����ģʽ


#define PCF8563_SCL_Pin GPIO_PIN_0   //GPIO_PIN_0
#define PCF8563_SCL_GPIO_Port GPIOB
#define PCF8563_SDA_Pin GPIO_PIN_1   //GPIO_PIN_1
#define PCF8563_SDA_GPIO_Port GPIOB

#define PCF8563_ADDR    0xA2
#define PCF8563_READ 	0X01	//������

//���ƺ�״̬�Ĵ���
#define CONTROL_STATUS_1 (unsigned char)0X00    
#define CONTROL_STATUS_2 (unsigned char)0X01
	
//ʱ������ڼĴ���
#define PCF8563_SECOND_ADDRESS (unsigned char)0X02				//���ַ
#define PCF8563_MINUTE_ADDRESS (unsigned char)0X03				//�ֵ�ַ
#define PCF8563_HOUR_ADDRESS (unsigned char)0X04				//ʱ��ַ
#define PCF8563_DAY_ADDRESS (unsigned char)0X05				//�յ�ַ
#define PCF8563_WEEK_ADDRESS (unsigned char)0X06					//���ڵ�ַ
#define PCF8563_MONTH_ADDRESS (unsigned char)0X07				//�µ�ַ
#define PCF8563_YEAR_ADDRESS (unsigned char)0X08				//���ַ

#define PCF8563_MINUTE_ALARM_ADDRESS 0X09				//���ӷֵ�ַ
#define PCF8563_HOUR_ALARM_ADDRESS 0X0A				//����ʱ��ַ
#define PCF8563_DAY_ALARM_ADDRESS 0X0B				//�����յ�ַ
#define PCF8563_WEEK_ALARM_ADDRESS 0X0C					//�������ڵ�ַ

#define PCF8563_CLKOUT_CONTROL 0X0D				//ʱ��������Ƶ�ַ

//IIC���в�������
void PCF8563_IIC_Init(void);                //��ʼ��IIC��IO��				 
static void PCF8563_IIC_Start(void);				//����IIC��ʼ�ź�
static void PCF8563_IIC_Stop(void);	  			//����IICֹͣ�ź�
static void PCF8563_IIC_Send_Byte(uint8_t txd);			//IIC����һ���ֽ�
static uint8_t PCF8563_IIC_Read_Byte(unsigned char ack);//IIC��ȡһ���ֽ�
static uint8_t PCF8563_IIC_Wait_Ack(void); 				//IIC�ȴ�ACK�ź�
static void PCF8563_IIC_Ack(void);					//IIC����ACK�ź�
static void PCF8563_IIC_NAck(void);				//IIC������ACK�ź�
	
static uint8_t PCF8563_ReadOneByte(uint16_t ReadAddr);
void PCF8563_ReadTime(uint8_t *p_time_buf);
void PCF8563_WriteTime(uint8_t *_time_buf);
void PCF8563_SetINT(void);
void PCF8563_ClearINT(void);
uint8_t PCF85636_ReadINT(void);
#endif

