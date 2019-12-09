/*
 * configurations.c
 *
 *  Created on: Dec 9, 2019
 *      Author: Saldory
 */


#include "configurations.h"
#include "communication.h"


void Init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3,ENABLE);

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;					//tx
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;					//rx
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_Init(GPIOA, &GPIO_InitStructure);


    //USART CONFIG
	NVIC_InitTypeDef NVIC_InitStructure;
	NVIC_Init(&NVIC_InitStructure);
	USART_InitTypeDef USART_InitStructure;

	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);

	USART_InitStructure.USART_BaudRate = 9600;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	USART_InitStructure.USART_StopBits = USART_StopBits_1;
	USART_InitStructure.USART_Parity = USART_Parity_No;
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;

	USART_Init(USART2, &USART_InitStructure);

	USART_Cmd(USART2, ENABLE);

	CAN_Configuration();

}

void RCC_Config_HSI_PLL_64MHz() //HSI com PLL a frequencia maxima
{
	//Reset às configuracões
	RCC_DeInit(); //Faz reset ao sistema e coloca a configuraccao por omissão

	//Ativar Fonte
	RCC_HSICmd(ENABLE); //Ativar a HSI

	//Esperar arranque do clock
	while(RCC_GetFlagStatus(RCC_FLAG_HSIRDY)==RESET);

	//Configurar Wait States
	FLASH_SetLatency(FLASH_Latency_2);

	//Configurar os prescalers AHB, APB1 e APB2, respetivamente
	RCC_HCLKConfig(RCC_SYSCLK_Div1);//divisor unitario que é o divisor mais baixo possivel
	RCC_PCLK1Config(RCC_HCLK_Div2); //PCLK1 no valor maximo possivel, ou seja, colocar o divisor minimo possivel
	RCC_PCLK2Config(RCC_HCLK_Div1); //PCLK2 no valor maximo possivel, ou seja, colocar o divisor minimo possivel

	//Configuracao da PLL
	RCC_PLLConfig(RCC_PLLSource_HSI_Div2, RCC_PLLMul_16); //Obrigatorio dividir por 2 e multipicar pelo maior valor (x16)

	//Ativar PLL
	RCC_PLLCmd(ENABLE); //Ativar a PLL

	//Esperar pela ativacao da PLL
	while(RCC_GetFlagStatus(RCC_FLAG_PLLRDY)==RESET);

	//Configurar SYSCLK
	RCC_SYSCLKConfig(RCC_SYSCLKSource_PLLCLK); //Selecionar a fonte PLL

	//Aguardar que a fonte estabilize
	while(RCC_GetSYSCLKSource() != 0x08); //Esperar que a fonte PLL(0x08) estabilize

}


void configurationTimer()
{
	// para 2HZ .. ARR=20.10^3 PSC= 179
	TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
	TIM_TimeBaseStructure.TIM_Period = 20000; //auto-reload 0 at ́e 65535
	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;
	TIM_TimeBaseStructure.TIM_Prescaler = 1790; //prescaler de 0 at ́e 65535 TIM
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;

	TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure);
	TIM_Cmd(TIM3, ENABLE);

}

void configurationTimeToggle()
{
	TIM_OCInitTypeDef TIM_OCInitStructure;
	TIM_OCStructInit(&TIM_OCInitStructure);

	TIM_OCInitTypeDef TIM_OCInitStructure1;
	TIM_OCStructInit(&TIM_OCInitStructure1);

	TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
	TIM_TimeBaseStructure.TIM_Period = 554; //auto-reload 0 at ́e 65535
	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;
	TIM_TimeBaseStructure.TIM_Prescaler = 65000; //prescaler de 0 at ́e 65535 TIM
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure);
	TIM_Cmd(TIM3, ENABLE);

}

void configurationInterruptTim3()
{
	NVIC_InitTypeDef NVIC_InitStructure;
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_1);

	NVIC_InitStructure.NVIC_IRQChannel = TIM3_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;

	NVIC_Init(&NVIC_InitStructure);

	TIM_ITConfig(TIM3, TIM_IT_Update,ENABLE);
}
