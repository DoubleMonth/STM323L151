#ifndef _SHT31_H
#define _SHT31_H
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
//IO��������

//#define SHT31_SDA_IN()  {GPIOB->MODER&=~(3<<(11*2));GPIOB->MODER|=(0<<(11*2));}	//PB7����ģʽ
//#define SHT31_SDA_OUT() {GPIOB->MODER&=~(3<<(11*2));GPIOB->MODER|=(1<<(11*2));}	//PB7����ģʽ


#define SHT31_SDA_IN()  do{GPIOA->MODER&=~(3<<(12*2));GPIOA->MODER|=(0<<(12*2));}while(0)	//PB7����ģʽ
#define SHT31_SDA_OUT() do{GPIOA->MODER&=~(3<<(12*2));GPIOA->MODER|=(1<<(12*2));}while(0)	//PB7����ģʽ

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
}float_data;//����������洢float���ݣ�float���͵Ĵ洢����IEEE��׼�������ڴ�������
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


//IIC���в�������
void SHT31_IIC_Init(void);                //��ʼ��IIC��IO��				 
static void SHT31_IIC_Start(void);				//����IIC��ʼ�ź�
static void SHT31_IIC_Stop(void);	  			//����IICֹͣ�ź�
static void SHT31_IIC_Send_Byte(uint8_t txd);			//IIC����һ���ֽ�
static uint8_t SHT31_IIC_Read_Byte(unsigned char ack);//IIC��ȡһ���ֽ�
static uint8_t I7020_IIC_Wait_Ack(void); 				//IIC�ȴ�ACK�ź�
static void SHT31_IIC_Ack(void);					//IIC����ACK�ź�
static void SHT31_IIC_NAck(void);				//IIC������ACK�ź�

void SHT31_SingleShot(uint8_t *buffer);
uint8_t SHT31_ReadStatus(void);
void SHT31_ClearStatuse(void);
void SHT31_GetStatus(uint8_t *buffer);
void SHT31_SoftReset(void);
void SHT31_SendByte(uint16_t cmd,uint8_t stop);
void SHT31_ReadTH(SHT31_DATA *TH);
#endif

