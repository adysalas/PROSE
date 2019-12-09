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
char group = 1;



float roll;		/* Body roll */
float pitch;	/* Body pitch */

static int state = 0;	/* State of state machine */

clock_t tic, toc;

/* State machine states defines */
#define STATE_FIRST_RUN 0
#define STATE_REQUEST_SENSOR_DATA 1

/* Ring IDs defines */


/* Other defines */
#define MAX_CAN_TRANSMIT_ATTEMPTS 3;



rings ring1, ring2, ring3, ring4, ring5;







void display(int info)
{

	for(int i=0; i<8;i++)
	{
		sprintf(buff,"%d e %d \r\n\n",i, info);
		for(int a=0; buff[a]!='\0';a++)
		{
			while(USART_GetFlagStatus(USART2, USART_FLAG_TXE) == RESET);
			USART_SendData(USART2, buff[a]);
			USART_ClearFlag(USART2,USART_FLAG_TXE);
		}
	}
}

void show(int info) {

	sprintf(buff,"%d \r\n\n",info);
	for(int a=0; buff[a]!='\0';a++)
	{
		while(USART_GetFlagStatus(USART2, USART_FLAG_TXE) == RESET);
		USART_SendData(USART2, buff[a]);
		USART_ClearFlag(USART2,USART_FLAG_TXE);
	}

}




void prose(void)
{

	switch(group)
	{

		case 1:
			//Grupo 1
			show(group);
			TxMessage.StdId = RING1_ID;						  //identifier
			CAN_Transmit(CAN1, &TxMessage);				  //transmit

			tic = clock();

			while(CAN_MessagePending(CAN1,CAN_FIFO0)==0) {   //wait for msg
				toc = clock();
				if( (double)(toc - tic) >= 50000) { //espera 50 ms
					group = 2;
					break;
				}
			}

			CAN_Receive(CAN1, 0 ,&RxMessage);			  //msg

			for (int i=0 ; i<8 ; i++) {
				ring1.sensor_lux[i] = RxMessage.Data[i];
			}
			//display(ring1.sensor_lux);
			break;


		case 2:
			//Grupo2
			show(group);
			TxMessage.StdId = RING2_ID;						  //identifier
			CAN_Transmit(CAN1, &TxMessage);				  //transmit

			tic = clock();
			while(CAN_MessagePending(CAN1,CAN_FIFO0)==0) {   //wait for msg
				toc = clock();
				if( (double)(toc - tic) >= 50000) { //espera 50 ms
					group = 1;
					break;
				}
			}

			CAN_Receive(CAN1, 0 ,&RxMessage);			  //msg

			for (int i=0 ; i<8 ; i++) {
				ring2.sensor_lux[i] = RxMessage.Data[i];
			}
			//display(ring2.sensor_lux);
			break;
/*
		case 3:
			//Grupo3
			show(group);
			TxMessage.StdId = RING3_ID;						  //identifier
			CAN_Transmit(CAN1, &TxMessage);				  //transmit

			tic = clock();
			while(CAN_MessagePending(CAN1,CAN_FIFO0)==0) {   //wait for msg
				toc = clock();
				if(toc - tic >= 50000) { //espera 50 ms
					group = 4;
					break;
				}
			}

			CAN_Receive(CAN1, 0 ,&RxMessage);			  //msg

			for (int i=0 ; i<8 ; i++) {
				ring3.sensor_lux[i] = RxMessage.Data[i];
			}
			//display(ring3.sensor_lux);
			break;

		case 4:
			//Grupo4
			show(group);
			TxMessage.StdId = RING4_ID;						  //identifier
			CAN_Transmit(CAN1, &TxMessage);				  //transmit

			tic = clock();
			while(CAN_MessagePending(CAN1,CAN_FIFO0)==0) {   //wait for msg
				toc = clock();
				if(toc - tic >= 50000) { //espera 50 ms
					group = 5;
					break;
				}
			}

			CAN_Receive(CAN1, 0 ,&RxMessage);			  //msg

			for (int i=0 ; i<8 ; i++) {
				ring4.sensor_lux[i] = RxMessage.Data[i];
			}
			//display(ring4.sensor_lux);
			break;

		case 5:
			//Grupo5
			show(group);
			TxMessage.StdId = RING5_ID;						  //identifier
			CAN_Transmit(CAN1, &TxMessage);				  //transmit

			tic = clock();
			while(CAN_MessagePending(CAN1,CAN_FIFO0)==0) {   //wait for msg
				toc = clock();
				if((toc - tic)) >= 50000) { //espera 50 ms
					group = 1;
					break;
				}
			}

			CAN_Receive(CAN1, 0 ,&RxMessage);			  //msg

			for (int i=0 ; i<8 ; i++) {
				ring5.sensor_lux[i] = RxMessage.Data[i];
			}
			//display(ring5.sensor_lux);
			break;*/

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
		//prose();
		//display();

	} /* While loop end */


	return 0;
} /* Main end */
