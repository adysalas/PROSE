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


/* Other defines */



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

	while(1) {

		///state_machine();
		state_MachineMain();
		//display();

	} /* While loop end */


	return 0;
} /* Main end */
