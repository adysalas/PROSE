/*
 * main.h
 *
 *  Created on: Dec 2, 2019
 *      Author: Saldory
 */

#ifndef MAIN_H_
#define MAIN_H_

void Config_USART2(void);
void USART_SendString2(char *message);
void Config_Pin_General(void);
void RCC_Config_HSI_PLL_64MHz(void);

#endif /* MAIN_H_ */
