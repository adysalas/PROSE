/*
 * candriver.c
 *
 *  Created on: Dec 2, 2019
 *      Author: Saldory
 */
#include "stm32f10x.h"
#include "stm32f10x_can.h"

/**
 * Declaration of static functions
 */
static void Config_Can_PinOut(void);

/**
 * Delcaration of Static variables
 */
CAN_InitTypeDef CAN_InitStructure;

/**
 * Declaration Of Global Variables
 */


void Config_CAN()
{
	Config_Can_PinOut();

	//Deinitialization
	CAN_DeInit(CAN1);

	//Initialization

	CAN_StructInit(&CAN_InitStructure);

	CAN_InitStructure.CAN_TTCM = DISABLE; //Mensagens periodicas - http://www.cs.put.poznan.pl/wswitala/download/pdf/CiA2000Paper_1.pdf
	CAN_InitStructure.CAN_ABOM = DISABLE; // http://www.chibios.com/forum/viewtopic.php?t=3138&start=10
	CAN_InitStructure.CAN_AWUM = DISABLE; // Clean auto of SLEEP - http://www.chibios.com/forum/viewtopic.php?t=712
	CAN_InitStructure.CAN_NART = DISABLE; // automatic wake-up mode - Page 618
	CAN_InitStructure.CAN_RFLM = DISABLE; // FIFO is full the next incoming message will be discarded
	CAN_InitStructure.CAN_TXFP = DISABLE; // Priority driven by the request order (chronologically)
	CAN_InitStructure.CAN_Mode = CAN_Mode_Normal;

	/**
	 * Can is set at:
	 *  CAN RATE = 1Mbits/s
	 *  Clock Rate 32Mhz ( PCLKK1 )
	 *  Sample-Point 87,5%
	 *
	 *  http://www.oertel-halle.de/files/cia99paper.pdf
	 */
	CAN_InitStructure.CAN_SJW = CAN_SJW_1tq;  // Resynchronization Jump Width=1 time quantum (usual)
	CAN_InitStructure.CAN_BS1 = CAN_BS1_13tq; // Open this link to calculate this 3 values: http://www.bittiming.can-wiki.info/#bxCAN
	CAN_InitStructure.CAN_BS2 = CAN_BS2_2tq;  // BPS = (ClockRate/[(1+bs1+bs2)*pres]) = (32*10^6/[(1+13+2)*2]) = 1000kbps
	CAN_InitStructure.CAN_Prescaler = 2; 	  // Length of a time quantum 1-1024

	/**
	 * Filters
	 */
	CAN_FilterInitTypeDef CAN_FilterInitStructure;
	CAN_FilterInitStructure.CAN_FilterNumber = 0;
	CAN_FilterInitStructure.CAN_FilterMode = CAN_FilterMode_IdMask;
	CAN_FilterInitStructure.CAN_FilterScale = CAN_FilterScale_16bit;
	CAN_FilterInitStructure.CAN_FilterIdHigh = 0x0000;
	CAN_FilterInitStructure.CAN_FilterIdLow = 0x0000;
	CAN_FilterInitStructure.CAN_FilterMaskIdHigh = 0x0000;
	CAN_FilterInitStructure.CAN_FilterMaskIdLow = 0x0000;
	CAN_FilterInitStructure.CAN_FilterFIFOAssignment = CAN_FilterFIFO0;
	CAN_FilterInitStructure.CAN_FilterActivation = ENABLE;

	//Put Initialization and Filters
	CAN_FilterInit(&CAN_FilterInitStructure);
	CAN_Init(CAN1,&CAN_InitStructure);//The filter scale and mode configuration must be configured before entering Normal Mode.

	//Interrupt_CAN1
	NVIC_InitTypeDef NVIC_CAN1;

	NVIC_CAN1.NVIC_IRQChannel = USB_HP_CAN1_TX_IRQn;
	NVIC_CAN1.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_CAN1.NVIC_IRQChannelSubPriority = 5;
	NVIC_CAN1.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_CAN1);

	NVIC_CAN1.NVIC_IRQChannel = USB_LP_CAN1_RX0_IRQn;
	NVIC_CAN1.NVIC_IRQChannelSubPriority = 5;
	NVIC_Init(&NVIC_CAN1);

	CAN_ITConfig(CAN1,CAN_IT_FMP0,ENABLE); // Something received in CANRXD
	CAN_ITConfig(CAN1,CAN_IT_RQCP0,ENABLE); //Something was sended to CANTX - Request completed mailbox 0

}


static void Config_Can_PinOut()
{
	/**
	 * Enable PORTA
	 */
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO , ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);

	/**
	 * Enable for CAN1 Clock
	 */
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_CAN1, ENABLE);

	/**
	 * Enable GPIO's
	 * First RXD and then TXD
	 */
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_Init(GPIOA, &GPIO_InitStructure);

}
