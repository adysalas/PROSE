/*
 * display.c
 *
 *  Created on: Dec 17, 2019
 *      Author: Saldory
 */
#include "stm32f10x.h"
#include "stdio.h"
#include "communication.h"

extern rings _rings[5];
char buff[100];

void valid(int a, int c)
{
	sprintf(buff,"Recebido Anel %d  ---  Cluster %d, \r\n",a,c);
	for(int a=0; buff[a]!='\0';a++)
	{
		while(USART_GetFlagStatus(USART2, USART_FLAG_TXE) == RESET);
		USART_SendData(USART2, buff[a]);
		USART_ClearFlag(USART2,USART_FLAG_TXE);
	}
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

void displayErrorID(int error)
{
	sprintf(buff,"Wrong ID Speaking on WRONG TIME: %x \r\n",error);
	for(int a=0; buff[a]!='\0';a++)
	{
		while(USART_GetFlagStatus(USART2, USART_FLAG_TXE) == RESET);
		USART_SendData(USART2, buff[a]);
		USART_ClearFlag(USART2,USART_FLAG_TXE);
	}
}



void sendString(char* string)
{
	for(int a=0; *string!='\0';a++)
	{
		while(USART_GetFlagStatus(USART2, USART_FLAG_TXE) == RESET);
		USART_SendData(USART2, *string);
		USART_ClearFlag(USART2,USART_FLAG_TXE);

		string++;
	}
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

void mostra_tudo(void)
{
//	for(int ringue = 0; ringue<=5; ringue++) { //Para cada anel
//
//		sprintf(buff,"Anel %d.\r\n",ringue);
//		for(int a=0; buff[a]!='\0';a++)
//		{
//			while(USART_GetFlagStatus(USART2, USART_FLAG_TXE) == RESET);
//			USART_SendData(USART2, buff[a]);
//			USART_ClearFlag(USART2,USART_FLAG_TXE);
//		}

		for(int i=0; i<8;i++) {	//Para cada sensor

			sprintf(buff,"Byte %d --> %d \r\n",i, RxMessage.Data[i]);
			for(int a=0; buff[a]!='\0';a++)
			{
				while(USART_GetFlagStatus(USART2, USART_FLAG_TXE) == RESET);
				USART_SendData(USART2, buff[a]);
				USART_ClearFlag(USART2,USART_FLAG_TXE);
			}
		}

		sprintf(buff,"------------------------------------ \r\n");
		for(int a=0; buff[a]!='\0';a++)
		{
			while(USART_GetFlagStatus(USART2, USART_FLAG_TXE) == RESET);
			USART_SendData(USART2, buff[a]);
			USART_ClearFlag(USART2,USART_FLAG_TXE);
		}
//	}
}

void displei(int k)
{


	for(int i=0; i<8;i++)
	{
		//int tete = atoi(_rings[k].sensor_lux);
		sprintf(buff,"Anel --> %d		 Sensor %d --> %d \r\n",k,i, _rings[k].sensor_lux[i]);
		for(int a=0; buff[a]!='\0';a++)
		{
			while(USART_GetFlagStatus(USART2, USART_FLAG_TXE) == RESET);
			USART_SendData(USART2, buff[a]);
			USART_ClearFlag(USART2,USART_FLAG_TXE);
		}
	}

	sprintf(buff,"************************************************** \r\n");
	for(int a=0; buff[a]!='\0';a++)
	{
		while(USART_GetFlagStatus(USART2, USART_FLAG_TXE) == RESET);
		USART_SendData(USART2, buff[a]);
		USART_ClearFlag(USART2,USART_FLAG_TXE);
	}

//	sprintf(buff,"----------------------- \r\n");
//	for(int a=0; buff[a]!='\0';a++)
//	{
//		while(USART_GetFlagStatus(USART2, USART_FLAG_TXE) == RESET);
//		USART_SendData(USART2, buff[a]);
//		USART_ClearFlag(USART2,USART_FLAG_TXE);
//	}
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
