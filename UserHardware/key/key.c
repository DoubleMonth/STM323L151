#include "key/key.h"
#include "main.h"
#include "usart.h"
#include "pwr_stop.h"
//extern  void SystemClock_Config(void);
uint8_t Interrupt_1min=0;  //1�����жϱ�־
uint8_t Interrupt_10min=0;  //10�����жϱ�־
uint8_t set_key;
uint8_t add_key;
uint8_t dec_key;
void keyInit(void)
{
	GPIO_InitTypeDef GPIO_InitStruct = {0};

	/* GPIO Ports Clock Enable */
	__HAL_RCC_GPIOA_CLK_ENABLE();

	/*Configure GPIO pin Output Level */
	HAL_GPIO_WritePin(KEY1_GPIO_Port, KEY1_Pin, GPIO_PIN_RESET);

	/*Configure GPIO pins : PAPin PAPin */
	GPIO_InitStruct.Pin |= KEY1_Pin|KEY2_Pin|KEY3_Pin|PCF8563_INT_Pin;
	GPIO_InitStruct.Mode = GPIO_MODE_IT_FALLING;
	GPIO_InitStruct.Pull = GPIO_PULLUP;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
	HAL_GPIO_Init(KEY1_GPIO_Port, &GPIO_InitStruct);
	  
	//�ж�
	HAL_NVIC_SetPriority(EXTI9_5_IRQn,0,0); //��ռ���ȼ�Ϊ0�������ȼ�Ϊ0
	HAL_NVIC_EnableIRQ(EXTI9_5_IRQn);   //ʹ���ж���3
	HAL_NVIC_SetPriority(EXTI15_10_IRQn,0,0); //��ռ���ȼ�Ϊ0�������ȼ�Ϊ0
	HAL_NVIC_EnableIRQ(EXTI15_10_IRQn);   //ʹ���ж���3
	
	set_key=0;
	add_key=0;
	dec_key=0;
	
}  
void EXTI9_5_IRQHandler(void)
{
    HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_8);		//�����жϴ����ú���  pcf8563�ж����
}
void EXTI15_10_IRQHandler(void)
{
    
	HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_11);		//�����жϴ����ú���	�����ж�
	HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_12);		//�����жϴ����ú���	�����ж�
	HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_15);		//�����жϴ����ú���	�����ж�
}
extern void SystemClock_Config(void);
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
	afterWakeUpConfigAgain(); 			//���Ѻ���������ʱ�Ӻ�����
	switch (GPIO_Pin)
	{
		case GPIO_PIN_8:
			Interrupt_1min++;   //PCF8563 1�����ж����
			Interrupt_10min++;		//PCF8563 10�����жϼ���
			printf("PCF8563 interrupt wakeup\r\n!");
		break;
		case GPIO_PIN_11:				//�Ӱ��� �ж�
			add_key++;	
			printf("add key wakeup %d\r\n",add_key);
		break;
		case GPIO_PIN_12:				//������ �ж�
			dec_key++;
			printf("dec key wakeup %d\r\n",dec_key);
		break ;  
		case GPIO_PIN_15:   			//SET���� 1s�ж�
			set_key++;
			printf("set key wakeup%d\r\n",set_key);
		break;
		default:
			break;
	}
}
