#ifndef __PWR_STOP_H__
#define __PWR_STOP_H__

#include "stm32l1xx_hal.h"

void enterStopMode(void);
void SetUnusedPin(void);
void afterWakeUpConfigAgain(void);
void EPDPowerOn(void);
void EPDPowerOff(void);
#endif

