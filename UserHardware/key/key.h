#ifndef _KEY_H
#define _KEY_H
#include "stm32l1xx_hal.h"

#define KEY1_Pin GPIO_PIN_11
#define KEY1_GPIO_Port GPIOA
#define KEY2_Pin GPIO_PIN_12
#define KEY2_GPIO_Port GPIOA
#define KEY3_Pin GPIO_PIN_15
#define KEY3_GPIO_Port GPIOA
#define PCF8563_INT_Pin GPIO_PIN_8
#define PCF8563_INT_GPIO_Port GPIOA



void keyInit(void);
#endif
