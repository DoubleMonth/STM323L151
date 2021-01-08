#include "stdio.h" 
#include "stm32l1xx_hal.h"
#include "pcf8563/pcf8563.h"
#include "delay/delay.h"
//////////////////////////////////////////////////////////////////////////////////	 



//uint8_t time_buf1[8]={1,2,3,4,5,6,7,8};
unsigned char time_buf[8];
unsigned char time_buf2[8];
unsigned char *time_buf1_sp;
uint8_t time_buffer[8];
//IIC初始化
void PCF8563_IIC_Init(void)
{
    GPIO_InitTypeDef GPIO_Initure;
    
    __HAL_RCC_GPIOB_CLK_ENABLE();   //使能GPIOC时钟
    
    //PC11,12初始化设置
    GPIO_Initure.Pin|=PCF8563_SCL_Pin|PCF8563_SDA_Pin;
    GPIO_Initure.Mode=GPIO_MODE_OUTPUT_PP;  //推挽输出
    GPIO_Initure.Pull=GPIO_PULLUP;          //上拉
    GPIO_Initure.Speed=GPIO_SPEED_FREQ_HIGH;     //高速
    HAL_GPIO_Init(PCF8563_SCL_GPIO_Port,&GPIO_Initure);
    
	HAL_GPIO_WritePin(PCF8563_SCL_GPIO_Port,PCF8563_SCL_Pin,GPIO_PIN_SET);
	HAL_GPIO_WritePin(PCF8563_SDA_GPIO_Port,PCF8563_SDA_Pin,GPIO_PIN_SET);
}

//产生IIC起始信号
static void PCF8563_IIC_Start(void)
{
	SDA_OUT(); 	         //sda线输出
	HAL_GPIO_WritePin(PCF8563_SCL_GPIO_Port,PCF8563_SCL_Pin,GPIO_PIN_SET);
	HAL_GPIO_WritePin(PCF8563_SDA_GPIO_Port,PCF8563_SDA_Pin,GPIO_PIN_SET);
	delay_us(20);
 	HAL_GPIO_WritePin(PCF8563_SDA_GPIO_Port,PCF8563_SDA_Pin,GPIO_PIN_RESET);//START:when CLK is high,DATA change form high to low 
	delay_us(20);
	HAL_GPIO_WritePin(PCF8563_SCL_GPIO_Port,PCF8563_SCL_Pin,GPIO_PIN_RESET);//钳住I2C总线，准备发送或接收数据 
}	  
//产生IIC停止信号
static void PCF8563_IIC_Stop(void)
{
	SDA_OUT();//sda线输出
	HAL_GPIO_WritePin(PCF8563_SCL_GPIO_Port,PCF8563_SCL_Pin,GPIO_PIN_RESET);
	HAL_GPIO_WritePin(PCF8563_SDA_GPIO_Port,PCF8563_SDA_Pin,GPIO_PIN_RESET);//STOP:when CLK is high DATA change form low to high
 	delay_us(20);
	HAL_GPIO_WritePin(PCF8563_SCL_GPIO_Port,PCF8563_SCL_Pin,GPIO_PIN_SET);
	HAL_GPIO_WritePin(PCF8563_SDA_GPIO_Port,PCF8563_SDA_Pin,GPIO_PIN_SET);//发送I2C总线结束信号
	delay_us(20);						   	
}
//等待应答信号到来
//返回值：1，接收应答失败
//        0，接收应答成功
static uint8_t PCF8563_IIC_Wait_Ack(void)
{
	uint8_t ucErrTime=0;
	SDA_IN();      //SDA设置为输入  
	HAL_GPIO_WritePin(PCF8563_SDA_GPIO_Port,PCF8563_SDA_Pin,GPIO_PIN_SET);
	delay_us(20);	   
	HAL_GPIO_WritePin(PCF8563_SCL_GPIO_Port,PCF8563_SCL_Pin,GPIO_PIN_SET);
	delay_us(20); 
	while(HAL_GPIO_ReadPin(PCF8563_SDA_GPIO_Port,PCF8563_SDA_Pin))
	{
		ucErrTime++;
		if(ucErrTime>250)
		{
			PCF8563_IIC_Stop();
			return 1;
		}
	}
	HAL_GPIO_WritePin(PCF8563_SCL_GPIO_Port,PCF8563_SCL_Pin,GPIO_PIN_RESET);//时钟输出0 	   
	return 0;  
} 
//产生ACK应答
static void PCF8563_IIC_Ack(void)
{
	HAL_GPIO_WritePin(PCF8563_SCL_GPIO_Port,PCF8563_SCL_Pin,GPIO_PIN_RESET);
	SDA_OUT();
	HAL_GPIO_WritePin(PCF8563_SDA_GPIO_Port,PCF8563_SDA_Pin,GPIO_PIN_RESET);
	delay_us(20);
	HAL_GPIO_WritePin(PCF8563_SCL_GPIO_Port,PCF8563_SCL_Pin,GPIO_PIN_SET);
	delay_us(20);
	HAL_GPIO_WritePin(PCF8563_SCL_GPIO_Port,PCF8563_SCL_Pin,GPIO_PIN_RESET);
}
//不产生ACK应答		    
static void PCF8563_IIC_NAck(void)
{
	HAL_GPIO_WritePin(PCF8563_SCL_GPIO_Port,PCF8563_SCL_Pin,GPIO_PIN_RESET);
	SDA_OUT();
	HAL_GPIO_WritePin(PCF8563_SDA_GPIO_Port,PCF8563_SDA_Pin,GPIO_PIN_SET);
	delay_us(20);
	HAL_GPIO_WritePin(PCF8563_SCL_GPIO_Port,PCF8563_SCL_Pin,GPIO_PIN_SET);
	delay_us(20);
	HAL_GPIO_WritePin(PCF8563_SCL_GPIO_Port,PCF8563_SCL_Pin,GPIO_PIN_RESET);
}					 				     
//IIC发送一个字节
//返回从机有无应答
//1，有应答
//0，无应答			  
static void PCF8563_IIC_Send_Byte(uint8_t txd)
{                        
    uint8_t t;   
	SDA_OUT(); 	    
    HAL_GPIO_WritePin(PCF8563_SCL_GPIO_Port,PCF8563_SCL_Pin,GPIO_PIN_RESET);//拉低时钟开始数据传输
    for(t=0;t<8;t++)
    {              
		if((txd&0x80)>>7)
			HAL_GPIO_WritePin(PCF8563_SDA_GPIO_Port,PCF8563_SDA_Pin,GPIO_PIN_SET);
		else
			HAL_GPIO_WritePin(PCF8563_SDA_GPIO_Port,PCF8563_SDA_Pin,GPIO_PIN_RESET);
        txd<<=1; 	  
		delay_us(20);   //对TEA5767这三个延时都是必须的
		HAL_GPIO_WritePin(PCF8563_SCL_GPIO_Port,PCF8563_SCL_Pin,GPIO_PIN_SET);
		delay_us(20);
		HAL_GPIO_WritePin(PCF8563_SCL_GPIO_Port,PCF8563_SCL_Pin,GPIO_PIN_RESET);
		delay_us(20);
    }	 
} 	    
//读1个字节，ack=1时，发送ACK，ack=0，发送nACK   
static uint8_t PCF8563_IIC_Read_Byte(unsigned char ack)
{
	unsigned char i,receive=0;
	SDA_IN();//SDA设置为输入
    for(i=0;i<8;i++ )
	{
        HAL_GPIO_WritePin(PCF8563_SCL_GPIO_Port,PCF8563_SCL_Pin,GPIO_PIN_RESET);
        delay_us(20);
		HAL_GPIO_WritePin(PCF8563_SCL_GPIO_Port,PCF8563_SCL_Pin,GPIO_PIN_SET);
        receive<<=1;
        if(HAL_GPIO_ReadPin(PCF8563_SDA_GPIO_Port,PCF8563_SDA_Pin))receive++;   
		delay_us(20);
    }					 
    if (!ack)
        PCF8563_IIC_NAck();//发送nACK
    else
        PCF8563_IIC_Ack(); //发送ACK   
    return receive;
}


//uint8_t si70xx_buff[1];
//uint8_t si70xx_data[2];
static uint8_t PCF8563_ReadOneByte(uint16_t ReadAddr)
{
	uint8_t temp = 0;
	PCF8563_IIC_Start();
	PCF8563_IIC_Send_Byte(PCF8563_ADDR);
	PCF8563_IIC_Wait_Ack();
	PCF8563_IIC_Send_Byte(ReadAddr);
	PCF8563_IIC_Wait_Ack();
	PCF8563_IIC_Start();
	PCF8563_IIC_Send_Byte(PCF8563_ADDR|PCF8563_READ);
	PCF8563_IIC_Wait_Ack();
	temp = PCF8563_IIC_Read_Byte(0);
	
	PCF8563_IIC_Stop();
	return temp;
}
static uint8_t PCF8563_WriteOneByte(uint8_t WriteAddr,uint8_t DataToWrite)
{
	PCF8563_IIC_Start();
	PCF8563_IIC_Send_Byte(PCF8563_ADDR);
	PCF8563_IIC_Wait_Ack();
	PCF8563_IIC_Send_Byte(WriteAddr);
	PCF8563_IIC_Wait_Ack();
	PCF8563_IIC_Send_Byte(DataToWrite);
	PCF8563_IIC_Wait_Ack();
	PCF8563_IIC_Stop();
	delay_us(20);
	return 0;
}
void PCF8563_WriteTime(uint8_t *_time_buf)
{
	uint8_t i,temp;
	for(i=0;i<8;i++)
	{
		temp = _time_buf[i]/10;
		time_buf[i] = _time_buf[i]%10;
		time_buf[i] = time_buf[i]+temp*16;
	}
	PCF8563_WriteOneByte(CONTROL_STATUS_1,0X20);
	PCF8563_WriteOneByte(PCF8563_YEAR_ADDRESS,time_buf[1]);
	PCF8563_WriteOneByte(PCF8563_MONTH_ADDRESS,time_buf[2]);
	PCF8563_WriteOneByte(PCF8563_DAY_ADDRESS,time_buf[3]);
	PCF8563_WriteOneByte(PCF8563_HOUR_ADDRESS,time_buf[4]);
	PCF8563_WriteOneByte(PCF8563_MINUTE_ADDRESS,time_buf[5]);
	PCF8563_WriteOneByte(PCF8563_SECOND_ADDRESS,time_buf[6]);
	PCF8563_WriteOneByte(PCF8563_WEEK_ADDRESS,time_buf[7]);
	PCF8563_WriteOneByte(CONTROL_STATUS_1,0X00);
}
extern uint8_t time_buffer[8];
void PCF8563_ReadTime(uint8_t *p_time_buf)
{
	uint8_t i,temp;
	time_buf[1] = PCF8563_ReadOneByte(PCF8563_YEAR_ADDRESS)&0XFF;
	time_buf[2] = PCF8563_ReadOneByte(PCF8563_MONTH_ADDRESS)&0X1F;
	time_buf[3] = PCF8563_ReadOneByte(PCF8563_DAY_ADDRESS)&0X3F;
	time_buf[4] = PCF8563_ReadOneByte(PCF8563_HOUR_ADDRESS)&0X3F;
	time_buf[5] = PCF8563_ReadOneByte(PCF8563_MINUTE_ADDRESS)&0X7F;
	time_buf[6] = PCF8563_ReadOneByte(PCF8563_SECOND_ADDRESS)&0X7F;
	time_buf[7] = PCF8563_ReadOneByte(PCF8563_WEEK_ADDRESS)&0X07;
	for(i=0;i<7;i++)
	{
		temp=time_buf[i]/16;
		p_time_buf[i]=time_buf[i]%16;
		p_time_buf[i]=p_time_buf[i]+temp*10;
	}
	p_time_buf[7]=time_buf[7]&0x07;
}
void PCF8563_SetINT(void)
{
	PCF8563_WriteOneByte(0x01,0x11); //控制状态寄存器2设置，TI/TP=0;TIE=1
	PCF8563_WriteOneByte(0x0E,0x82);	//设置定时器控制器寄存器 TE=1,TD1=0,TD0=1(1Hz)
	PCF8563_WriteOneByte(0x0F,0x3C);	//设置定时器到计数器数值为60，即定时1min
}
void PCF8563_ClearINT(void)  //清除中断TF标志位
{
	PCF8563_WriteOneByte(0x01,0x11);
}
uint8_t PCF85636_ReadINT(void)
{
	uint8_t temp;
	temp=PCF8563_ReadOneByte(0x01);
	if(0x04==(0x04&temp))
		return 1;
	else
		return 0;
}
