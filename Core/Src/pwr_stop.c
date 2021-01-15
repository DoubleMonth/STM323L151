#include "pwr_stop.h"
#include "gpio.h"
#include "spi.h"
#include "usart.h"
#include "si7020/si7020_iic.h"
#include "Delay/delay.h"
#include "pcf8563/pcf8563.h"
#include "ePaper_2in13/Example/EPD_Test.h"
#include "display.h"
#include "key/key.h"
#include "pwr_stop.h"
extern void SystemClock_Config(void);
void SetUnusedPin(void)
{
	 GPIO_InitTypeDef GPIO_Initure;
    
	__HAL_RCC_GPIOA_CLK_ENABLE();   //使能GPIOC时钟
	__HAL_RCC_GPIOB_CLK_ENABLE();   //使能GPIOC时钟
	__HAL_RCC_GPIOC_CLK_ENABLE();   //使能GPIOC时钟
    
	GPIO_Initure.Pin|=GPIO_PIN_0|GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3|GPIO_PIN_4|GPIO_PIN_5|GPIO_PIN_6|GPIO_PIN_7 ;
    GPIO_Initure.Mode=GPIO_MODE_ANALOG;  //模拟输入
    GPIO_Initure.Speed=GPIO_SPEED_FREQ_LOW;     //低速
    HAL_GPIO_Init(GPIOA,&GPIO_Initure);
	
	GPIO_Initure.Pin=GPIO_PIN_All;
    GPIO_Initure.Mode=GPIO_MODE_ANALOG;  //模拟输入
    GPIO_Initure.Speed=GPIO_SPEED_FREQ_LOW;     //低速
    HAL_GPIO_Init(GPIOB,&GPIO_Initure);
	
	GPIO_Initure.Pin=GPIO_PIN_All;
    GPIO_Initure.Mode=GPIO_MODE_ANALOG;  //模拟输入
    GPIO_Initure.Speed=GPIO_SPEED_FREQ_LOW;     //低速
    HAL_GPIO_Init(GPIOC,&GPIO_Initure);
	
//	GPIO_Initure.Pin |= GPIO_PIN_8|GPIO_PIN_15;  //外部唤醒引脚设置为中断输入
//	GPIO_Initure.Mode = GPIO_MODE_IT_FALLING;
//	GPIO_Initure.Pull = GPIO_PULLUP;
//	GPIO_Initure.Speed = GPIO_SPEED_FREQ_HIGH;
//	HAL_GPIO_Init(GPIOA, &GPIO_Initure);
	
	__HAL_RCC_GPIOB_CLK_DISABLE();   //使能GPIOC时钟
	__HAL_RCC_GPIOC_CLK_DISABLE();   //使能GPIOC时钟
}
void enterStopMode(void)
{
//	SetUnusedPin();
	__HAL_RCC_PWR_CLK_ENABLE();
	HAL_PWREx_EnableUltraLowPower();
	HAL_PWREx_EnableFastWakeUp();
	__HAL_PWR_CLEAR_FLAG(PWR_FLAG_WU);
	HAL_PWR_EnterSTOPMode(PWR_LOWPOWERREGULATOR_ON,PWR_STOPENTRY_WFI);  //进入STOP模式
}
void afterWakeUpConfigAgain(void)
{
	__HAL_PWR_CLEAR_FLAG(PWR_FLAG_WU); 
	SystemClock_Config();
	delay_init(24);
	MX_GPIO_Init();
	MX_SPI1_Init();
	SI7020_IIC_Init();   //SI7020初始化
	keyInit(); 			//按键和1秒中断初始化
	MX_USART1_UART_Init();
}
void EPDPowerOn(void)
{
	GPIO_InitTypeDef GPIO_InitStruct = {0};
	GPIO_InitStruct.Pin = EPAPER_CTL_Pin;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull = GPIO_PULLUP;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
	HAL_GPIO_Init(EPAPER_CTL_GPIO_Port, &GPIO_InitStruct);
	HAL_GPIO_WritePin(EPAPER_CTL_GPIO_Port, EPAPER_CTL_Pin, GPIO_PIN_SET);
}
void EPDPowerOff(void)
{
	HAL_GPIO_WritePin(EPAPER_CTL_GPIO_Port, EPAPER_CTL_Pin, GPIO_PIN_RESET);
}
