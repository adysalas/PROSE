#include "communication.h"



CanTxMsg TxMessage;
CanRxMsg RxMessage;

rings ring1, ring2, ring3, ring4, ring5;

char group;

void state_MachineMain(void)
{
	static states actualRing = RING_1;
	static uint8_t enteringState = TRUE;


	switch(actualRing)
	{

		case RING_1:
		{
			if (TRUE == enteringState)
			{
				group = 1;
				enteringState = FALSE;
			}

			//Grupo 1
			show(group);
			TxMessage.StdId = RING1_ID;	//identifier
			TxMessage.Data[0] = (uint8_t)0x01;
			TxMessage.Data[1] = (uint8_t)0x02;
			TxMessage.RTR     = CAN_RTR_DATA;
			TxMessage.DLC     = 8;
			CAN_Transmit(CAN1, &TxMessage);				  //transmit

			//tic = clock();

			while(CAN_MessagePending(CAN1,CAN_FIFO0)!= FALSE) {   //wait for msg
				//toc = clock();
				//if( (double)(toc - tic) >= 50000) { //espera 50 ms
					group = 1;
					break;
				}

			CAN_Receive(CAN1, 0 ,&RxMessage);			  //msg

			for (int i=0 ; i<8 ; i++) {
				ring1.sensor_lux[i] = RxMessage.Data[i];
				//ring1.sensor_lux[i] = i;
			}
			display(ring1.sensor_lux);
		}
		break;
		/*case 4:
		{

			//Grupo2
			//show(group);
			TxMessage.StdId = RING2_ID;						  //identifier
			CAN_Transmit(CAN1, &TxMessage);				  //transmit

			//tic = clock();
			while(CAN_MessagePending(CAN1,CAN_FIFO0) != 0) {   //wait for msg
				//toc = clock();
				//if( (double)(toc - tic) >= 50000) { //espera 50 ms
					group = 4;
					break;

			}

			CAN_Receive(CAN1, 0 ,&RxMessage);			  //msg

			for (int i=0 ; i<8 ; i++) {
				ring2.sensor_lux[i] = RxMessage.Data[i];
			}

			display(ring2.sensor_lux);
		}
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

void CAN_Configuration(void)
{
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO , ENABLE);
	GPIO_InitTypeDef GPIO_InitStructure;
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_CAN1, ENABLE);

	GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_12; //tx with can1
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_11; //rx with can1
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	CAN_DeInit(CAN1); //reset can
	//////////////////////////////////////////////////////////////////////////init/////////////////////////////////////////////////////////////////////////
	CAN_InitTypeDef CAN_InitStructure;

	CAN_InitStructure.CAN_TTCM = DISABLE;
	CAN_InitStructure.CAN_ABOM = DISABLE;
	CAN_InitStructure.CAN_AWUM = DISABLE;
	CAN_InitStructure.CAN_NART = DISABLE;
	CAN_InitStructure.CAN_RFLM = DISABLE;
	CAN_InitStructure.CAN_TXFP = DISABLE;
	CAN_InitStructure.CAN_Mode = CAN_Mode_Normal;
	CAN_InitStructure.CAN_SJW = CAN_SJW_1tq;
	CAN_InitStructure.CAN_BS1 = CAN_BS1_13tq;			//configuration to 1Mb/s
	CAN_InitStructure.CAN_BS2 = CAN_BS2_2tq;
	CAN_InitStructure.CAN_Prescaler = 2;


	///////////////////////////////////////////////////////////////////////filter/////////////////////////////////////////////////////////////////////////
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

	CAN_FilterInit(&CAN_FilterInitStructure);
	CAN_Init(CAN1, &CAN_InitStructure);
}


void can_send()
{
	/* Send a message with the CAN */
	CanTxMsg Tx;

	Tx.StdId = 0x01;			//identifier
	Tx.ExtId = 0x00;			//extended identifier
	Tx.IDE = CAN_ID_STD;		//type of identifier
	Tx.RTR = CAN_RTR_DATA;	//type of frame for message
	Tx.DLC = 8;				//length of frame
	Tx.Data[0] = 0x01;		//information to be transmitted, size of vector->8
	Tx.Data[1] = 0x01;
	Tx.Data[2] = 0x01;
	Tx.Data[3] = 0x01;
	Tx.Data[4] = 0x01;
	Tx.Data[5] = 0x01;
	Tx.Data[6] = 0x01;
	Tx.Data[7] = 0x01;

	CAN_Transmit(CAN1, &Tx);		//send information

}

void can_recieve()
{
	CanRxMsg RxMessage;
	CAN_Receive(CAN1, 0 ,&RxMessage);
}
