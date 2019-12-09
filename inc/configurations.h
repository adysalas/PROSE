/*
 * configurations.h
 *
 *  Created on: Dec 9, 2019
 *      Author: Saldory
 */
#include "stm32f10x.h"


#ifndef CONFIGURATIONS_H_
#define CONFIGURATIONS_H_


void configurationTimer(void);
void configurationTimeToggle(void);
void configurationInterruptTim3(void);
void RCC_Config_HSI_PLL_64MHz(void);
void Init(void);
void show(int info);



#endif /* CONFIGURATIONS_H_ */
