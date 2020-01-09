#include "stm32f10x.h"
#include "stm32f10x_can.h"
#include "lcd.h"
#include "time.h"
#include "configurations.h"
#include "communication.h"
#include "stdio.h"
#include "display.h"

uint16_t ADC1_Addr[8];
int a = 1;

/* State machine states defines */
#define STATE_FIRST_RUN 0
#define STATE_REQUEST_SENSOR_DATA 1

/* Ring IDs defines */


static void Config_Tim(void);
static void initTimers(void);

/* Other defines */

int main(void)
{
	RCC_Config_HSI_PLL_64MHz();
	Init();
	lcd_init();
	rcc_lcd_info();
	Config_Tim();
	initTimers();

	while(1)
	{
		stateMachineReloaded();

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


static void initTimers(void)
{
	i50ms_Counter = 0;
	b50ms_Counter = FALSE;
}

