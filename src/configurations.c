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

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;					//tx CAN
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;					//rx CAN
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	//Configurate GPIOA1 as INPUT with PULL-UP intern. SW5 DSO BOARD.
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA,&GPIO_InitStructure);

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

void configurationInterruptExtPortA(void)
{

	EXTI_InitTypeDef NVIC_InitStructure;
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_1);

	NVIC_InitStructure.EXTI_Line = EXTI1_IRQn;
	NVIC_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
	NVIC_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;
	NVIC_InitStructure.EXTI_LineCmd = ENABLE;

	EXTI_Init(&NVIC_InitStructure);

	NVIC_InitTypeDef NVIC_InitStructure1;
//	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_1);
	NVIC_InitStructure1.NVIC_IRQChannel = EXTI1_IRQn;
	NVIC_InitStructure1.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_InitStructure1.NVIC_IRQChannelSubPriority = 1;
	NVIC_InitStructure1.NVIC_IRQChannelCmd = ENABLE;

	NVIC_Init(&NVIC_InitStructure1);

	GPIO_EXTILineConfig(GPIO_PortSourceGPIOA,GPIO_PinSource1);
}


void configurationADC(void)
{

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC,ENABLE); // Enable RCC to GPIOC
	/*	Clock ADC */
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1,ENABLE);

	/* AHB */
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_2;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOC,&GPIO_InitStructure);

	RCC_ADCCLKConfig(RCC_PCLK2_Div6);
	ADC_InitTypeDef ADC_InitStructure;

	ADC_InitStructure.ADC_Mode = ADC_Mode_Independent;//ADC_Mode_Independent
	ADC_InitStructure.ADC_ScanConvMode = ENABLE;// DISABLE PARA UN SOLO CANAL, ENABLE PARA VARIOS.
	ADC_InitStructure.ADC_ContinuousConvMode = ENABLE;
	ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;
	ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;
	ADC_InitStructure.ADC_NbrOfChannel = 3;

	ADC_Init(ADC1,&ADC_InitStructure);

	ADC_RegularChannelConfig(ADC1, ADC_Channel_10, 1, ADC_SampleTime_7Cycles5); // Z
	ADC_RegularChannelConfig(ADC1, ADC_Channel_11, 2, ADC_SampleTime_7Cycles5); // Y
	ADC_RegularChannelConfig(ADC1, ADC_Channel_12, 3, ADC_SampleTime_7Cycles5); // X

	ADC_DMACmd(ADC1, ENABLE);

	ADC_Cmd(ADC1, ENABLE);

	ADC_ResetCalibration(ADC1);
	while(ADC_GetResetCalibrationStatus(ADC1));
	ADC_StartCalibration(ADC1);
	while(ADC_GetCalibrationStatus(ADC1));

}