/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2020 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under BSD 3-Clause license,
  * the "License"; You may not use this file except in compliance with the
  * License. You may obtain a copy of the License at:
  *                        opensource.org/licenses/BSD-3-Clause
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "adc.h"
#include "spi.h"
#include "usart.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

#include "si7020/si7020_iic.h"
#include "Delay/delay.h"
#include "pcf8563/pcf8563.h"
#include "ePaper_2in13/Example/EPD_Test.h"
#include "display.h"
#include "key/key.h"
#include "pwr_stop.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */

SI7020_DATA read_si7020_data;
uint8_t clockBuffer_test[8]={0};
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
extern uint8_t Interrupt_1min;  //1分钟中断标志
extern uint8_t Interrupt_10min; //10分钟中断标志
//extern uint16_t USART_RX_STA;       //接收状态标记
//extern UART_HandleTypeDef huart1;
//extern uint8_t USART_RX_BUF[USART_REC_LEN];     //接收缓冲,最大USART_REC_LEN个字节.
unsigned char  default_time_buf[8]={20,20,12,15,23,59,40,3};
//uint8_t setTimeBuff[8]={0};

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  delay_init(24);
  MX_GPIO_Init();
  MX_SPI1_Init();
  MX_USART1_UART_Init();
  
  /* USER CODE BEGIN 2 */
	SI7020_IIC_Init();   //SHT31初始化
	PCF8563_IIC_Init();   //PCF8563初始化
	PCF8563_WriteTime(default_time_buf);   //PCF8563写时间
	PCF8563_SetINT();
	keyInit(); 					//按键和1秒中断初始化
	printf("InitFinsh\r\n");   
# if DISPLAY_EPAPER
//	updateDisplayFull();   //全局刷新显示	
#endif
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
	while (1)
	{
		/* USER CODE END WHILE */
		delay_us(1000*1000);
		#if DISPLAY_EPAPER
		if(Interrupt_10min>=10)			//每十分钟全局刷新显示一次
		{
			
//			updateDisplayFull();   //全局刷新显示 
			Interrupt_10min=0;
			Interrupt_1min=0;  //设置1分钟标志为0，防止再次进行局部刷新
			printf("PCF8563 interrupt 10min %d ",Interrupt_10min);
		}
		if(Interrupt_1min)   
		{
//			updateDisplayPart();   //局部刷新显示
			Interrupt_1min=0;
			printf("PCF8563 interrupt 1min %d ",Interrupt_1min);
		}
		#endif
		#if UART_UPDATE_TIME
		setTime();
		#endif
		PCF8563_ReadTime(clockBuffer_test);	
		printf("%d%d-%d%d-%d%d\r\n",clockBuffer_test[1]/10,clockBuffer_test[1]%10,clockBuffer_test[2]/10,clockBuffer_test[2]%10,clockBuffer_test[3]/10,clockBuffer_test[3]%10);
		printf("%d%d:%d%d:%d%d,%d\r\n",clockBuffer_test[4]/10,clockBuffer_test[4]%10,clockBuffer_test[5]/10,clockBuffer_test[5]%10,clockBuffer_test[6]/10,clockBuffer_test[6]%10,clockBuffer_test[7]%10);
		enterStopMode();
//		si7020Measure(&read_si7020_data);
//		printf("tempERATURE=%d,humi=%d\r\n",(uint32_t)(read_si7020_data.temperature *10),(uint32_t)(read_si7020_data.humidity*10));
		
		/* USER CODE BEGIN 3 */
	}
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Configure the main internal regulator output voltage
  */
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);
  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
  RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL3;
  RCC_OscInitStruct.PLL.PLLDIV = RCC_PLL_DIV2;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }
  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_1) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */

  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     tex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
