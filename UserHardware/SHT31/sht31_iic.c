#include "stdio.h" 
#include "main.h"
#include "stm32l1xx_hal.h"
#include "usart.h"

#include "SHT31/sht31_iic.h"
#include "Delay/delay.h"
//////////////////////////////////////////////////////////////////////////////////	 								  
////////////////////////////////////////////////////////////////////////////////// 	
uint8_t status[3];//״̬�Ĵ���
uint8_t status_crc;//CRCУ�����
uint8_t crc_check;//crc���
//IIC��ʼ��
void SHT31_IIC_Init(void)
{
    GPIO_InitTypeDef GPIO_Initure;
    
	__HAL_RCC_GPIOA_CLK_ENABLE();   //ʹ��GPIOCʱ��
    
    //PC11,12��ʼ������
    GPIO_Initure.Pin|=SHT31_SDA_Pin;
    GPIO_Initure.Mode=GPIO_MODE_OUTPUT_PP;  //�������
    GPIO_Initure.Pull=GPIO_PULLUP;          //����
    GPIO_Initure.Speed=GPIO_SPEED_FREQ_HIGH;     //����
    HAL_GPIO_Init(SHT31_SDA_GPIO_Port,&GPIO_Initure);
	
	 GPIO_Initure.Pin|=SHT31_SCL_Pin;
    GPIO_Initure.Mode=GPIO_MODE_OUTPUT_PP;  //�������
    GPIO_Initure.Pull=GPIO_PULLUP;          //����
    GPIO_Initure.Speed=GPIO_SPEED_FREQ_HIGH;     //����
    HAL_GPIO_Init(SHT31_SCL_GPIO_Port,&GPIO_Initure);
    
	HAL_GPIO_WritePin(SHT31_SCL_GPIO_Port,SHT31_SCL_Pin,GPIO_PIN_SET);
	HAL_GPIO_WritePin(SHT31_SDA_GPIO_Port,SHT31_SDA_Pin,GPIO_PIN_SET);
}

//����IIC��ʼ�ź�
static void SHT31_IIC_Start(void)
{
	SHT31_SDA_OUT(); 	         //sda�����
	HAL_GPIO_WritePin(SHT31_SCL_GPIO_Port,SHT31_SCL_Pin,GPIO_PIN_SET);
	HAL_GPIO_WritePin(SHT31_SDA_GPIO_Port,SHT31_SDA_Pin,GPIO_PIN_SET);
	delay_us(20);
 	HAL_GPIO_WritePin(SHT31_SDA_GPIO_Port,SHT31_SDA_Pin,GPIO_PIN_RESET);//START:when CLK is high,DATA change form high to low 
	delay_us(20);
	HAL_GPIO_WritePin(SHT31_SCL_GPIO_Port,SHT31_SCL_Pin,GPIO_PIN_RESET);//ǯסI2C���ߣ�׼�����ͻ�������� 
}	  
//����IICֹͣ�ź�
static void SHT31_IIC_Stop(void)
{
	SHT31_SDA_OUT();//sda�����
	HAL_GPIO_WritePin(SHT31_SCL_GPIO_Port,SHT31_SCL_Pin,GPIO_PIN_RESET);
	HAL_GPIO_WritePin(SHT31_SDA_GPIO_Port,SHT31_SDA_Pin,GPIO_PIN_RESET);//STOP:when CLK is high DATA change form low to high
 	delay_us(20);
	HAL_GPIO_WritePin(SHT31_SCL_GPIO_Port,SHT31_SCL_Pin,GPIO_PIN_SET);
	HAL_GPIO_WritePin(SHT31_SDA_GPIO_Port,SHT31_SDA_Pin,GPIO_PIN_SET);//����I2C���߽����ź�
	delay_us(20);							   	
}
//�ȴ�Ӧ���źŵ���
//����ֵ��1������Ӧ��ʧ��
//        0������Ӧ��ɹ�
static uint8_t SHT31_IIC_Wait_Ack(void)
{
	uint8_t ucErrTime=0;
	SHT31_SDA_IN();      //SDA����Ϊ����  
	HAL_GPIO_WritePin(SHT31_SDA_GPIO_Port,SHT31_SDA_Pin,GPIO_PIN_SET);
	delay_us(20);	   
	HAL_GPIO_WritePin(SHT31_SCL_GPIO_Port,SHT31_SCL_Pin,GPIO_PIN_SET);
	delay_us(20);	 
	while(HAL_GPIO_ReadPin(SHT31_SDA_GPIO_Port,SHT31_SDA_Pin))
	{
		ucErrTime++;
		if(ucErrTime>250)
		{
			SHT31_IIC_Stop();
			return 1;
		}
	}
	HAL_GPIO_WritePin(SHT31_SCL_GPIO_Port,SHT31_SCL_Pin,GPIO_PIN_RESET);//ʱ�����0 	   
	return 0;  
} 
//����ACKӦ��
static void SHT31_IIC_Ack(void)
{
	HAL_GPIO_WritePin(SHT31_SCL_GPIO_Port,SHT31_SCL_Pin,GPIO_PIN_RESET);
	SHT31_SDA_OUT();
	HAL_GPIO_WritePin(SHT31_SDA_GPIO_Port,SHT31_SDA_Pin,GPIO_PIN_RESET);
	delay_us(20);
	HAL_GPIO_WritePin(SHT31_SCL_GPIO_Port,SHT31_SCL_Pin,GPIO_PIN_SET);
	delay_us(20);
	HAL_GPIO_WritePin(SHT31_SCL_GPIO_Port,SHT31_SCL_Pin,GPIO_PIN_RESET);
}
//������ACKӦ��		    
static void SHT31_IIC_NAck(void)
{
	HAL_GPIO_WritePin(SHT31_SCL_GPIO_Port,SHT31_SCL_Pin,GPIO_PIN_RESET);
	SHT31_SDA_OUT();
	HAL_GPIO_WritePin(SHT31_SDA_GPIO_Port,SHT31_SDA_Pin,GPIO_PIN_SET);
	delay_us(20);
	HAL_GPIO_WritePin(SHT31_SCL_GPIO_Port,SHT31_SCL_Pin,GPIO_PIN_SET);
	delay_us(20);
	HAL_GPIO_WritePin(SHT31_SCL_GPIO_Port,SHT31_SCL_Pin,GPIO_PIN_RESET);
}	

//IIC����һ���ֽ�
//���شӻ�����Ӧ��
//1����Ӧ��
//0����Ӧ��			  
static void SHT31_IIC_Send_Byte(uint8_t txd)
{                        
    uint8_t t;   
	SHT31_SDA_OUT(); 	    
    HAL_GPIO_WritePin(SHT31_SCL_GPIO_Port,SHT31_SCL_Pin,GPIO_PIN_RESET);//����ʱ�ӿ�ʼ���ݴ���
    for(t=0;t<8;t++)
    {              
		if((txd&0x80)>>7)
			HAL_GPIO_WritePin(SHT31_SDA_GPIO_Port,SHT31_SDA_Pin,GPIO_PIN_SET);
		else
			HAL_GPIO_WritePin(SHT31_SDA_GPIO_Port,SHT31_SDA_Pin,GPIO_PIN_RESET);
        txd<<=1; 	  
		delay_us(20);   //��TEA5767��������ʱ���Ǳ����
		HAL_GPIO_WritePin(SHT31_SCL_GPIO_Port,SHT31_SCL_Pin,GPIO_PIN_SET);
		delay_us(20); 
		HAL_GPIO_WritePin(SHT31_SCL_GPIO_Port,SHT31_SCL_Pin,GPIO_PIN_RESET);
		delay_us(20);
    }	 
} 	    
//��1���ֽڣ�ack=1ʱ������ACK��ack=0������nACK   
static uint8_t SHT31_IIC_Read_Byte(unsigned char ack)
{
	unsigned char i,receive=0;
	SHT31_SDA_IN();//SDA����Ϊ����
    for(i=0;i<8;i++ )
	{
        HAL_GPIO_WritePin(SHT31_SCL_GPIO_Port,SHT31_SCL_Pin,GPIO_PIN_RESET);
        delay_us(20);
		HAL_GPIO_WritePin(SHT31_SCL_GPIO_Port,SHT31_SCL_Pin,GPIO_PIN_SET);
        receive<<=1;
        if(HAL_GPIO_ReadPin(SHT31_SDA_GPIO_Port,SHT31_SDA_Pin))receive++;   
		delay_us(20); 
    }					 
    if (!ack)
        SHT31_IIC_NAck();//����nACK
    else
        SHT31_IIC_Ack(); //����ACK   
    return receive;
}
//CRCУ��
static uint8_t CRC_Check(uint8_t *check_data, uint8_t num, uint8_t check_crc)
{
    uint8_t bit;        // bit mask
    uint8_t crc = 0xFF; // calculated checksum
    uint8_t byteCtr;    // byte counter
    
 // calculates 8-Bit checksum with given polynomial x8+x5+x4+1
    for(byteCtr = 0; byteCtr < num; byteCtr++)
    {
        crc ^= (*(check_data+byteCtr));
        //crcУ�飬���λ��1��^0x31
        for(bit = 8; bit > 0; --bit)
        {
            if(crc & 0x80)
                crc = (crc << 1) ^ 0x31;
            else
                crc = (crc << 1);
        }
    }
    if(crc==check_crc)
        return 1;
    else 
        return 0;
}
//SHT31 ��ȡһ���ֽ�  cmd�ǵ�ַ��stop=0��������ֹͣλ��stop=1,����ֹͣλ
void SHT31_SendByte(uint16_t cmd,uint8_t stop)
{
	SHT31_IIC_Start();
	SHT31_IIC_Send_Byte(SHT31_DEFAULT_ADDR<<1|SHT31_WRITE);
	SHT31_IIC_Wait_Ack();
	SHT31_IIC_Send_Byte(cmd>>8);
	SHT31_IIC_Wait_Ack();
	SHT31_IIC_Send_Byte(cmd&0XFF);
	SHT31_IIC_Wait_Ack();
	if (stop)
	{
		SHT31_IIC_Stop();
	}
}
//SHT31 ��λ
void SHT31_SoftReset(void)
{
	SHT31_SendByte(SHT31_SOFTRESET,1);
}
//��ȡ״̬�Ĵ���ֵ
void SHT31_GetStatus(uint8_t *buffer)
{
	SHT31_SendByte(SHT31_READSTATUS,0);
	HAL_Delay(3);
	SHT31_IIC_Start();
	SHT31_IIC_Send_Byte(SHT31_DEFAULT_ADDR<<1 | SHT31_READ);
	if(SHT31_IIC_Wait_Ack==0)
	{
		buffer[0]=SHT31_IIC_Read_Byte(1);
		buffer[1]=SHT31_IIC_Read_Byte(1);
		buffer[2]=SHT31_IIC_Read_Byte(1);
		SHT31_IIC_Stop();
	}
}
//���״̬�Ĵ���
void SHT31_ClearStatuse(void)
{
	SHT31_SendByte(SHT31_CLEARSTATUS,1);
}
// ����0 ����״̬�Ĵ�����ȡ�ɹ���1�����ȡ����
uint8_t SHT31_ReadStatus(void)
{
	SHT31_GetStatus(status);
	if(CRC_Check(status,2,*(status+2)))
		return 0;
	else
		return 1;
}

// ���ζ�ȡ��ʪ��
void SHT31_SingleShot(uint8_t *buffer)
{
	uint8_t tryTime=100;
	SHT31_SendByte(SHT31_MEAS_HIGHREP_STRETCH,1);
	HAL_Delay(20);
	SHT31_IIC_Start();
	SHT31_IIC_Send_Byte(SHT31_DEFAULT_ADDR<<1 | SHT31_READ);
	while(SHT31_IIC_Wait_Ack())
	{
		tryTime--;
		delay_us(50);
		if(tryTime==0)
			return;
	}
	buffer[0]=SHT31_IIC_Read_Byte(1);
	buffer[1]=SHT31_IIC_Read_Byte(1);
	buffer[2]=SHT31_IIC_Read_Byte(1);
	buffer[3]=SHT31_IIC_Read_Byte(1);
	buffer[4]=SHT31_IIC_Read_Byte(1);
	buffer[5]=SHT31_IIC_Read_Byte(1);
	SHT31_IIC_Stop();
}
//SHT31 ��ȡ��ʪ�� SHT31_DATA�ṹ������¶Ⱥ�ʪ��
void SHT31_ReadTH(SHT31_DATA *TH)
{
	uint8_t buff[6];
	uint16_t tem,hum;
	uint8_t crcT,crcH;
	float Temperature=0;
	float Humidity=0;
	SHT31_SingleShot(buff);
	tem=((buff[0]<<8)|buff[1]);
	hum=((buff[3]<<8)|buff[4]);
	crcT=CRC_Check(buff,2,buff[2]);
	crcH=CRC_Check(buff+3,2,buff[5]);
	
	if(crcT &&crcH)
	{
		//�����ֲ���㹫ʽ����
        Temperature= (175.0*(float)tem/65535.0-45.0) ;  // T = -45 + 175 * tem / (2^16-1)
        Humidity= (100.0*(float)hum/65535.0);           // RH = hum*100 / (2^16-1)
        if((Temperature>=-20)&&(Temperature<=125)&&(Humidity>=0)&&(Humidity<=100))//���˳���������Χ�Ĵ�������
        {
            TH->temperature=Temperature;
            TH->humidity=Humidity;
        }
	}
}


