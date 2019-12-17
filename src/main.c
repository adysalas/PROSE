#include "stm32f10x.h"
#include "stm32f10x_can.h"
#include "lcd.h"
#include "time.h"
#include "configurations.h"
#include "communication.h"
#include "stdio.h"

char buff[30];
uint16_t ADC1_Addr[8];
int a = 1;

float roll;		/* Body roll */
float pitch;	/* Body pitch */



/* State machine states defines */
#define STATE_FIRST_RUN 0
#define STATE_REQUEST_SENSOR_DATA 1

/* Ring IDs defines */


static void Config_Tim(void);
static void initTimers(void);

/* Other defines */

void displei(void)
{

	for(int i=0; i<8;i++)
	{
		sprintf(buff,"Sensor %d --> %d \r\n",i, RxMessage.Data[i]);
		for(int a=0; buff[a]!='\0';a++)
		{
			while(USART_GetFlagStatus(USART2, USART_FLAG_TXE) == RESET);
			USART_SendData(USART2, buff[a]);
			USART_ClearFlag(USART2,USART_FLAG_TXE);
		}
	}

	sprintf(buff,"********************** \r\n");
	for(int a=0; buff[a]!='\0';a++)
	{
		while(USART_GetFlagStatus(USART2, USART_FLAG_TXE) == RESET);
		USART_SendData(USART2, buff[a]);
		USART_ClearFlag(USART2,USART_FLAG_TXE);
	}
}

void display(float *info)
{
	for(int i=0; i<8;i++)
	{
		sprintf(buff,"Sensor %d: %.3f \r\n\n",i, *info);
		for(int a=0; buff[a]!='\0';a++)
		{
			while(USART_GetFlagStatus(USART2, USART_FLAG_TXE) == RESET);
			USART_SendData(USART2, buff[a]);
			USART_ClearFlag(USART2,USART_FLAG_TXE);
		}
		info++;
	}
}

void displayString(int value,int cluster)
{
	if (TRUE == cluster)
		sprintf(buff,"Sensor %i Cluster 1 pedido \r\n\n", value);
	else
		sprintf(buff,"Sensor %i Cluster 2 pedido \r\n\n", value);
	for(int a=0; buff[a]!='\0';a++)
	{
		while(USART_GetFlagStatus(USART2, USART_FLAG_TXE) == RESET);
		USART_SendData(USART2, buff[a]);
		USART_ClearFlag(USART2,USART_FLAG_TXE);
	}
}

void displayString2(int value)
{
	sprintf(buff,"Sensor %i_1 pedido \r\n\n", value);
	for(int a=0; buff[a]!='\0';a++)
	{
		while(USART_GetFlagStatus(USART2, USART_FLAG_TXE) == RESET);
		USART_SendData(USART2, buff[a]);
		USART_ClearFlag(USART2,USART_FLAG_TXE);
	}
}



void show(int info)
{

	sprintf(buff,"%d \r\n\n",info);
	for(int a=0; buff[a]!='\0';a++)
	{
		while(USART_GetFlagStatus(USART2, USART_FLAG_TXE) == RESET);
		USART_SendData(USART2, buff[a]);
		USART_ClearFlag(USART2,USART_FLAG_TXE);
	}

}


int main(void)
{
	RCC_Config_HSI_PLL_64MHz();
	Init();
	lcd_init();
	rcc_lcd_info();
	Config_Tim();
	initTimers();
	while(1) {

		///state_machine();
		stateMachineReloaded();
		//displei();
		//state_MachineMain();
		//display();

	} /* While loop end */


	return 0;
} /* Main end */


static void Config_Tim(void)
{
	//Timer3 - 1KHz
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);

	TIM_TimeBaseInitTypeDef TIM_T3x;
		TIM_T3x.TIM_Period = 100;//1Khz Escolhido 100 para facilitar a %
		TIM_T3x.TIM_ClockDivision = TIM_CKD_DIV1;
		TIM_T3x.TIM_Prescaler = 633; //prescaler de 0 ate 65535
		TIM_T3x.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseInit(TIM3, &TIM_T3x);
	TIM_Cmd(TIM3, ENABLE);

	//Timer3 - Interrupcao
    NVIC_InitTypeDef NVIC_T3x;
		NVIC_PriorityGroupConfig(NVIC_PriorityGroup_0);
		NVIC_T3x.NVIC_IRQChannel = TIM3_IRQn;
		NVIC_T3x.NVIC_IRQChannelPreemptionPriority = 0;
		NVIC_T3x.NVIC_IRQChannelSubPriority = 1;
		NVIC_T3x.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_T3x);
    TIM_ITConfig(TIM3, TIM_IT_Update, ENABLE);
}


void displayNADA(void)
{
	char buffe[30];
	sprintf(buffe,"NADA \r\n");
	for(int a=0; buffe[a]!='\0';a++)
	{
		while(USART_GetFlagStatus(USART2, USART_FLAG_TXE) == RESET);
		USART_SendData(USART2, buffe[a]);
		USART_ClearFlag(USART2,USART_FLAG_TXE);
	}
}

static void initTimers(void)
{
	i50ms_Counter = 0;
	b50ms_Counter = FALSE;
}


void displayError(void)
{
	sprintf(buff,"Error, nothing received. \r\n");
	for(int a=0; buff[a]!='\0';a++)
	{
		while(USART_GetFlagStatus(USART2, USART_FLAG_TXE) == RESET);
		USART_SendData(USART2, buff[a]);
		USART_ClearFlag(USART2,USART_FLAG_TXE);
	}
}
