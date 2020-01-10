#include "communication.h"
#include "display.h"



CanTxMsg TxMessage;
CanRxMsg RxMessage;

rings finalData[5];
//data_sensors ring1,ring2,ring3,ring4,ring5;
rings ring1,ring2,ring3,ring4,ring5;
rings _rings[5];
uint64_t tic = 0;
uint64_t toc = 0;

float rollGeral;
float pitchGeral;

char group;

static void delay(void);


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

void CAN_CleanRxBuffer(void)
{
	RxMessage.StdId = 0xFF;

	RxMessage.Data[0] = 0;
	RxMessage.Data[1] = 0;
	RxMessage.Data[2] = 0;
	RxMessage.Data[3] = 0;
	RxMessage.Data[4] = 0;
	RxMessage.Data[5] = 0;
	RxMessage.Data[6] = 0;
	RxMessage.Data[7] = 0;
}



void stateMachineReloaded(void)
{
	static states state = 0;
	static int enteringState = TRUE;

	switch (state)
	{

	case RING_1_0:
	{
		if (TRUE == enteringState )
		{
			displayString(1,TRUE);
			enteringState = FALSE;
			TxMessage.StdId   = RING1_ID;
			TxMessage.Data[0] = 0x00;
			TxMessage.RTR     = CAN_RTR_DATA;
			TxMessage.DLC     = 8;

			CAN_Transmit(CAN1, &TxMessage);//transmit
			strcpy(TxMessage.Data,"        ");
			b50ms_Counter = FALSE;
			i50ms_Counter = 0;
		}

		while ((CAN_MessagePending(CAN1,CAN_FIFO0) == 0) && (FALSE == b50ms_Counter));

		if (TRUE == b50ms_Counter )
		{
			state = RING_2_0;
			b50ms_Counter = FALSE;
			enteringState = TRUE;

			_rings[0].sensor_lux[0] =  NULL_SENSOR;
			_rings[0].sensor_lux[1] =  NULL_SENSOR;
			_rings[0].sensor_lux[2] =  NULL_SENSOR;
			_rings[0].sensor_lux[3] =  NULL_SENSOR;
			_rings[0].successfullTransmision = FALSE;

		}
		else
		{
			CAN_Receive(CAN1,CAN_FIFO0,&RxMessage);
			if (RING1_ID == RxMessage.StdId )
			{

				_rings[0].sensor_lux[0] =  RxMessage.Data[1] + (RxMessage.Data[0] << 8 );
				_rings[0].sensor_lux[1] =  RxMessage.Data[3] + (RxMessage.Data[2] << 8 );
				_rings[0].sensor_lux[2] =  RxMessage.Data[5] + (RxMessage.Data[4] << 8 );
				_rings[0].sensor_lux[3] =  RxMessage.Data[7] + (RxMessage.Data[6] << 8 );
				_rings[0].successfullTransmision = TRUE;

				state = RING_1_1;

				delay();
				enteringState = TRUE;
			}
			else
			{
				displayErrorID(RxMessage.StdId);
			}
			//displei();
		}


	}
	break;
	case RING_1_1:
	{
		if (TRUE == enteringState )
		{
			i50ms_Counter = 0;
			displayString(1,FALSE);
			enteringState = FALSE;
			TxMessage.StdId   = RING1_ID;
			TxMessage.Data[0] = (uint8_t)0x01;
			TxMessage.RTR     = CAN_RTR_DATA;
			TxMessage.DLC     = 8;

			CAN_Transmit(CAN1, &TxMessage);//transmit
			strcpy(TxMessage.Data,"        ");

			b50ms_Counter = FALSE;
			i50ms_Counter = 0;
		}

		while ((CAN_MessagePending(CAN1,CAN_FIFO0) == 0) && (FALSE == b50ms_Counter));

		if (TRUE == b50ms_Counter )
		{
			state = RING_2_0;
			b50ms_Counter = FALSE;
			enteringState = TRUE;

			_rings[0].sensor_lux[4] =  NULL_SENSOR;
			_rings[0].sensor_lux[5] =  NULL_SENSOR;
			_rings[0].sensor_lux[6] =  NULL_SENSOR;
			_rings[0].sensor_lux[7] =  NULL_SENSOR;
			_rings[0].successfullTransmision = FALSE;
		}
		else
		{
			CAN_Receive(CAN1,CAN_FIFO0,&RxMessage);
			if (RING1_ID == RxMessage.StdId )
			{

				state = RING_2_0;

				delay();
				enteringState = TRUE;

				_rings[0].sensor_lux[4] =  RxMessage.Data[1] + (RxMessage.Data[0] << 8 );
				_rings[0].sensor_lux[5] =  RxMessage.Data[3] + (RxMessage.Data[2] << 8 );
				_rings[0].sensor_lux[6] =  RxMessage.Data[5] + (RxMessage.Data[4] << 8 );
				_rings[0].sensor_lux[7] =  RxMessage.Data[7] + (RxMessage.Data[6] << 8 );
				_rings[0].successfullTransmision = TRUE;
				displei(1);
			}
			else
			{
				displayErrorID(RxMessage.StdId);
			}
		}
		//displei();
		//		mostra_tudo();

	}
	break;
	case RING_2_0:
	{
		if (TRUE == enteringState )
		{
			displayString(2,TRUE);
			enteringState = FALSE;
			TxMessage.StdId   = RING2_ID;
			TxMessage.Data[0] = 0x00;
			TxMessage.RTR     = CAN_RTR_DATA;
			TxMessage.DLC     = 8;

			CAN_Transmit(CAN1, &TxMessage);//transmit
			strcpy(TxMessage.Data,"        ");
			b50ms_Counter = FALSE;
			i50ms_Counter = 0;
		}

		while ((CAN_MessagePending(CAN1,CAN_FIFO0) == 0) && (FALSE == b50ms_Counter));

		if (TRUE == b50ms_Counter )
		{
			state = RING_3_0;
			b50ms_Counter = FALSE;
			enteringState = TRUE;

			_rings[1].sensor_lux[0] =  NULL_SENSOR;
			_rings[1].sensor_lux[1] =  NULL_SENSOR;
			_rings[1].sensor_lux[2] =  NULL_SENSOR;
			_rings[1].sensor_lux[3] =  NULL_SENSOR;
			_rings[1].successfullTransmision =FALSE;
		}
		else
		{
			CAN_Receive(CAN1,CAN_FIFO0,&RxMessage);
			if (RING2_ID == RxMessage.StdId )
			{


				state = RING_2_1;

				delay();
				enteringState = TRUE;

				_rings[1].sensor_lux[0] =  RxMessage.Data[1] + (RxMessage.Data[0] << 8 );
				_rings[1].sensor_lux[1] =  RxMessage.Data[3] + (RxMessage.Data[2] << 8 );
				_rings[1].sensor_lux[2] =  RxMessage.Data[5] + (RxMessage.Data[4] << 8 );
				_rings[1].sensor_lux[3] =  RxMessage.Data[7] + (RxMessage.Data[6] << 8 );
				//displei();
			}
			else
			{
				displayErrorID(RxMessage.StdId);
			}
		}


	}
	break;
	case RING_2_1:
	{
		if (TRUE == enteringState )
		{
			i50ms_Counter = 0;
			displayString(2,FALSE);
			enteringState = FALSE;
			TxMessage.StdId   = RING2_ID;
			TxMessage.Data[0] = (uint8_t)0x01;
			TxMessage.RTR     = CAN_RTR_DATA;
			TxMessage.DLC     = 8;

			CAN_Transmit(CAN1, &TxMessage);//transmit
			strcpy(TxMessage.Data,"        ");

			b50ms_Counter = FALSE;
			i50ms_Counter = 0;
		}

		while ((CAN_MessagePending(CAN1,CAN_FIFO0) == 0) && (FALSE == b50ms_Counter));

		if (TRUE == b50ms_Counter )
		{
			state = RING_3_0;
			b50ms_Counter = FALSE;
			enteringState = TRUE;

			_rings[1].sensor_lux[4] =  NULL_SENSOR;
			_rings[1].sensor_lux[5] =  NULL_SENSOR;
			_rings[1].sensor_lux[6] =  NULL_SENSOR;
			_rings[1].sensor_lux[7] =  NULL_SENSOR;
			_rings[1].successfullTransmision = FALSE;
		}
		else
		{
			CAN_Receive(CAN1,CAN_FIFO0,&RxMessage);
			if (RING2_ID == RxMessage.StdId )
			{


				state = RING_3_0;

				delay();
				enteringState = TRUE;

				_rings[1].sensor_lux[4] =  RxMessage.Data[1] + (RxMessage.Data[0] << 8 );
				_rings[1].sensor_lux[5] =  RxMessage.Data[3] + (RxMessage.Data[2] << 8 );
				_rings[1].sensor_lux[6] =  RxMessage.Data[5] + (RxMessage.Data[4] << 8 );
				_rings[1].sensor_lux[7] =  RxMessage.Data[7] + (RxMessage.Data[6] << 8 );
				_rings[1].successfullTransmision = TRUE;
				displei(2);
			}
			else
			{
				displayErrorID(RxMessage.StdId);
			}
		}
		//displei();

	}
	break;
	case RING_3_0:
	{
		if (TRUE == enteringState )
		{
			displayString(3,TRUE);
			enteringState = FALSE;
			TxMessage.StdId   = RING3_ID;
			TxMessage.Data[0] = 0x00;
			TxMessage.RTR     = CAN_RTR_DATA;
			TxMessage.DLC     = 8;

			CAN_Transmit(CAN1, &TxMessage);//transmit
			strcpy(TxMessage.Data,"        ");
			b50ms_Counter = FALSE;
			i50ms_Counter = 0;
		}

		while ((CAN_MessagePending(CAN1,CAN_FIFO0) == 0) && (FALSE == b50ms_Counter));

		if (TRUE == b50ms_Counter )
		{
			state = RING_4_0;
			b50ms_Counter = FALSE;
			enteringState = TRUE;

			_rings[2].sensor_lux[0] =  NULL_SENSOR;
			_rings[2].sensor_lux[1] =  NULL_SENSOR;
			_rings[2].sensor_lux[2] =  NULL_SENSOR;
			_rings[2].sensor_lux[3] =  NULL_SENSOR;
			_rings[2].successfullTransmision = FALSE;
		}
		else
		{
			CAN_Receive(CAN1,CAN_FIFO0,&RxMessage);
			if (RING3_ID == RxMessage.StdId )
			{


				state = RING_3_1;

				delay();
				enteringState = TRUE;

				_rings[2].sensor_lux[0] =  RxMessage.Data[1] + (RxMessage.Data[0] << 8 );
				_rings[2].sensor_lux[1] =  RxMessage.Data[3] + (RxMessage.Data[2] << 8 );
				_rings[2].sensor_lux[2] =  RxMessage.Data[5] + (RxMessage.Data[4] << 8 );
				_rings[2].sensor_lux[3] =  RxMessage.Data[7] + (RxMessage.Data[6] << 8 );
				_rings[2].successfullTransmision = TRUE;

				mostra_tudo();
			}
			else
			{
				displayErrorID(RxMessage.StdId);
			}
		}

		//displei();
	}
	break;
	case RING_3_1:
	{
		if (TRUE == enteringState )
		{
			i50ms_Counter = 0;
			displayString(3,FALSE);
			enteringState = FALSE;
			TxMessage.StdId   = RING3_ID;
			TxMessage.Data[0] = (uint8_t)0x01;
			TxMessage.RTR     = CAN_RTR_DATA;
			TxMessage.DLC     = 8;

			CAN_Transmit(CAN1, &TxMessage);//transmit
			strcpy(TxMessage.Data,"        ");

			b50ms_Counter = FALSE;
			i50ms_Counter = 0;
		}

		while ((CAN_MessagePending(CAN1,CAN_FIFO0) == 0) && (FALSE == b50ms_Counter));

		if (TRUE == b50ms_Counter )
		{
			state = RING_4_0;
			b50ms_Counter = FALSE;
			enteringState = TRUE;

			_rings[2].sensor_lux[4] =  NULL_SENSOR;
			_rings[2].sensor_lux[5] =  NULL_SENSOR;
			_rings[2].sensor_lux[6] =  NULL_SENSOR;
			_rings[2].sensor_lux[7] =  NULL_SENSOR;
			_rings[2].successfullTransmision = FALSE;
		}
		else
		{
			CAN_Receive(CAN1,CAN_FIFO0,&RxMessage);
			if (RING3_ID == RxMessage.StdId )
			{


				state = RING_4_0;

				delay();
				enteringState = TRUE;

				_rings[2].sensor_lux[4] =  RxMessage.Data[1] + (RxMessage.Data[0] << 8 );
				_rings[2].sensor_lux[5] =  RxMessage.Data[3] + (RxMessage.Data[2] << 8 );
				_rings[2].sensor_lux[6] =  RxMessage.Data[5] + (RxMessage.Data[4] << 8 );
				_rings[2].sensor_lux[7] =  RxMessage.Data[7] + (RxMessage.Data[6] << 8 );
				_rings[2].successfullTransmision = TRUE;

				//displei(3);
				mostra_tudo();
			}
			else
			{
				displayErrorID(RxMessage.StdId);
			}
		}
		//displei();

	}
	break;
	case RING_4_0:
	{
		if (TRUE == enteringState )
		{
			displayString(4,TRUE);
			enteringState = FALSE;
			TxMessage.StdId   = RING4_ID;
			TxMessage.Data[0] = 0x00;
			TxMessage.RTR     = CAN_RTR_DATA;
			TxMessage.DLC     = 8;

			CAN_Transmit(CAN1, &TxMessage);//transmit
			strcpy(TxMessage.Data,"        ");
			b50ms_Counter = FALSE;
			i50ms_Counter = 0;
		}

		while ((CAN_MessagePending(CAN1,CAN_FIFO0) == 0) && (FALSE == b50ms_Counter));

		if (TRUE == b50ms_Counter )
		{
			state = RING_5_0;
			b50ms_Counter = FALSE;
			enteringState = TRUE;

			_rings[3].sensor_lux[0] =  NULL_SENSOR;
			_rings[3].sensor_lux[1] =  NULL_SENSOR;
			_rings[3].sensor_lux[2] =  NULL_SENSOR;
			_rings[3].sensor_lux[3] =  NULL_SENSOR;
			_rings[3].successfullTransmision = FALSE;
		}
		else
		{
			CAN_Receive(CAN1,CAN_FIFO0,&RxMessage);
			if (RING4_ID == RxMessage.StdId )
			{


				state = RING_4_1;

				delay();
				enteringState = TRUE;

				_rings[3].sensor_lux[0] =  RxMessage.Data[1] + (RxMessage.Data[0] << 8 );
				//				sprintf(buff,"----------%d------------\r\n",k,i, _rings[3].sensor_lux[0]);
				//				for(int a=0; buff[a]!='\0';a++)
				//				{
				//					while(USART_GetFlagStatus(USART2, USART_FLAG_TXE) == RESET);
				//					USART_SendData(USART2, buff[a]);
				//					USART_ClearFlag(USART2,USART_FLAG_TXE);
				//				}
				_rings[3].sensor_lux[1] =  RxMessage.Data[3] + (RxMessage.Data[2] << 8 );
				_rings[3].sensor_lux[2] =  RxMessage.Data[5] + (RxMessage.Data[4] << 8 );
				_rings[3].sensor_lux[3] =  RxMessage.Data[7] + (RxMessage.Data[6] << 8 );
				_rings[3].successfullTransmision = TRUE;
				valid(4,0);
				mostra_tudo();
			}
			else
			{
				displayErrorID(RxMessage.StdId);
			}
		}

		//displei();
	}
	break;
	case RING_4_1:
	{
		if (TRUE == enteringState )
		{
			i50ms_Counter = 0;
			displayString(4,FALSE);
			enteringState = FALSE;
			TxMessage.StdId   = RING4_ID;
			TxMessage.Data[0] = (uint8_t)0x01;
			TxMessage.RTR     = CAN_RTR_DATA;
			TxMessage.DLC     = 8;

			CAN_Transmit(CAN1, &TxMessage);//transmit
			strcpy(TxMessage.Data,"        ");

			b50ms_Counter = FALSE;
			i50ms_Counter = 0;
		}

		while ((CAN_MessagePending(CAN1,CAN_FIFO0) == 0) && (FALSE == b50ms_Counter));

		if (TRUE == b50ms_Counter )
		{
			state = RING_5_0;
			b50ms_Counter = FALSE;
			enteringState = TRUE;

			_rings[3].sensor_lux[4] =  NULL_SENSOR;
			_rings[3].sensor_lux[5] =  NULL_SENSOR;
			_rings[3].sensor_lux[6] =  NULL_SENSOR;
			_rings[3].sensor_lux[7] =  NULL_SENSOR;
			_rings[3].successfullTransmision = FALSE;
		}
		else
		{
			CAN_Receive(CAN1,CAN_FIFO0,&RxMessage);
			if (RING4_ID == RxMessage.StdId )
			{


				state = RING_5_0;

				delay();
				enteringState = TRUE;

				_rings[3].sensor_lux[4] =  RxMessage.Data[1] + (RxMessage.Data[0] << 8 );
				_rings[3].sensor_lux[5] =  RxMessage.Data[3] + (RxMessage.Data[2] << 8 );
				_rings[3].sensor_lux[6] =  RxMessage.Data[5] + (RxMessage.Data[4] << 8 );
				_rings[3].sensor_lux[7] =  RxMessage.Data[7] + (RxMessage.Data[6] << 8 );
				_rings[3].successfullTransmision = TRUE;
				valid(4,1);
				mostra_tudo();
				displei(4);
			}
			else
			{
				displayErrorID(RxMessage.StdId);
			}
		}
		//displei();


	}
	break;
	case RING_5_0:
	{
		if (TRUE == enteringState )
		{
			displayString(5,TRUE);
			enteringState = FALSE;
			TxMessage.StdId   = RING5_ID;
			TxMessage.Data[0] = 0x00;
			TxMessage.RTR     = CAN_RTR_DATA;
			TxMessage.DLC     = 8;

			CAN_Transmit(CAN1, &TxMessage);//transmit
			strcpy(TxMessage.Data,"        ");
			b50ms_Counter = FALSE;
			i50ms_Counter = 0;
		}

		while ((CAN_MessagePending(CAN1,CAN_FIFO0) == 0) && (FALSE == b50ms_Counter));

		if (TRUE == b50ms_Counter )
		{
			state = ROLL_CALC;

			b50ms_Counter = FALSE;
			enteringState = TRUE;

			_rings[4].sensor_lux[0] =  NULL_SENSOR;
			_rings[4].sensor_lux[1] =  NULL_SENSOR;
			_rings[4].sensor_lux[2] =  NULL_SENSOR;
			_rings[4].sensor_lux[3] =  NULL_SENSOR;
			_rings[4].successfullTransmision = FALSE;

		}
		else
		{
			CAN_Receive(CAN1,CAN_FIFO0,&RxMessage);
			if (RING5_ID == RxMessage.StdId )
			{


				delay();
				enteringState = TRUE;
				state = RING_5_1;

				_rings[4].sensor_lux[0] =  RxMessage.Data[1] + (RxMessage.Data[0] << 8 );
				_rings[4].sensor_lux[1] =  RxMessage.Data[3] + (RxMessage.Data[2] << 8 );
				_rings[4].sensor_lux[2] =  RxMessage.Data[5] + (RxMessage.Data[4] << 8 );
				_rings[4].sensor_lux[3] =  RxMessage.Data[7] + (RxMessage.Data[6] << 8 );
				_rings[4].successfullTransmision = TRUE;
				valid(5, 0);
			}
			else
			{
				displayErrorID(RxMessage.StdId);
			}

		}


	}
	break;
	case RING_5_1:
	{
		if (TRUE == enteringState )
		{
			i50ms_Counter = 0;
			displayString(5,FALSE);
			enteringState = FALSE;
			TxMessage.StdId   = RING5_ID;
			TxMessage.Data[0] = (uint8_t)0x01;
			TxMessage.RTR     = CAN_RTR_DATA;
			TxMessage.DLC     = 8;

			CAN_Transmit(CAN1, &TxMessage);//transmit
			strcpy(TxMessage.Data,"        ");

			b50ms_Counter = FALSE;
			i50ms_Counter = 0;
		}

		while ((CAN_MessagePending(CAN1,CAN_FIFO0) == 0) && (FALSE == b50ms_Counter));

		if (TRUE == b50ms_Counter )
		{
			state = ROLL_CALC;
			b50ms_Counter = FALSE;
			enteringState = TRUE;

			_rings[4].sensor_lux[4] =  NULL_SENSOR;
			_rings[4].sensor_lux[5] =  NULL_SENSOR;
			_rings[4].sensor_lux[6] =  NULL_SENSOR;
			_rings[4].sensor_lux[7] =  NULL_SENSOR;
			_rings[4].successfullTransmision = FALSE;
		}
		else
		{
			CAN_Receive(CAN1,CAN_FIFO0,&RxMessage);
			if (RING5_ID == RxMessage.StdId )
			{


				state = ROLL_CALC;

				delay();
				enteringState = TRUE;

				_rings[4].sensor_lux[4] =  RxMessage.Data[1] + (RxMessage.Data[0] << 8 );
				_rings[4].sensor_lux[5] =  RxMessage.Data[3] + (RxMessage.Data[2] << 8 );
				_rings[4].sensor_lux[6] =  RxMessage.Data[5] + (RxMessage.Data[4] << 8 );
				_rings[4].sensor_lux[7] =  RxMessage.Data[7] + (RxMessage.Data[6] << 8 );
				_rings[4].successfullTransmision = TRUE;
				valid(5, 1);
				displei(5);
			}
			else
			{
				displayErrorID(RxMessage.StdId);
			}
		}
		//displei();

	}
	break;
	case ROLL_CALC:
	{
		rollGeral = 0;
		int validRoll = 0;

		for (int i=0;i<5;i++)
		{
			_rings[i].roll = compute_roll(&_rings[i],&validRoll);
			rollGeral += _rings[i].roll;
			pitchGeral = compute_pitch(&_rings[i]);
		}
		rollGeral /= validRoll;

		state = RING_1_0;
	}

	//mostra_tudo();
	break;
	default:

		break;
	}
}


void stateMachineV2(void)
{
	static states_v2 state = INIT;
	static int enteringState = TRUE;
	static int exitState = FALSE;
	states_v2 nextState;

	switch (state)
	{
		case INIT:
		{
			if (TRUE == enteringState )
			{
				enteringState = FALSE;
				TxMessage.StdId   = CALIBRATION_ID;
				TxMessage.Data[0] = CALIBRATION_DATA;
				TxMessage.RTR     = CAN_RTR_DATA;
				TxMessage.DLC     = 1;
			}

			if (TRUE == bStartUp)
			{
				CAN_Transmit(CAN1, &TxMessage);//transmit
				nextState = RING_1;
				sprintf(buff,"Calibration Message Sent Successfully \n\r");
				sendString(buff);
				exitState = TRUE;
			}

			if (TRUE == exitState)
			{
				state = nextState;
				enteringState = TRUE;
			}
			//char
		}
		break;
		case RING_1:
		{
			if (TRUE == enteringState )
			{
				displayString(1,TRUE);
				enteringState = FALSE;
				TxMessage.StdId   = RING1_ID;
				TxMessage.Data[0] = REQUEST_DATA;
				TxMessage.RTR     = CAN_RTR_DATA;
				TxMessage.DLC     = 1;

				CAN_Transmit(CAN1, &TxMessage);//transmit
				//strcpy(TxMessage.Data,"        ");
				b50ms_Counter = FALSE;
				i50ms_Counter = 0;
			}

			while ((CAN_MessagePending(CAN1,CAN_FIFO0) == 0) && (FALSE == b50ms_Counter));

			if (TRUE == b50ms_Counter )
			{
				nextState = RING_2;
				exitState = TRUE;
				b50ms_Counter = FALSE;
				enteringState = TRUE;

				_rings[0].roll =  NULL_SENSOR;
				_rings[0].higherLux =  NULL_SENSOR;
				_rings[0].successfullTransmision = FALSE;

				sprintf(buff,"***** RING1 NOT RESPONSE IN 50ms ***** \n\r");
				sendString(buff);

			}
			else
			{
				CAN_Receive(CAN1,CAN_FIFO0,&RxMessage);
				if (RING1_ID == RxMessage.StdId )
				{

					_rings[0].roll			=  RxMessage.Data[1] + (RxMessage.Data[0] << 8 );
					_rings[0].higherLux     =  RxMessage.Data[3] + (RxMessage.Data[2] << 8 );
					_rings[0].successfullTransmision = TRUE;

					nextState = RING_2;
					exitState = TRUE;
					sprintf(buff,"RING1: Roll-->%u   HigherLux -->%u\n\r",_rings[0].roll,_rings[0].higherLux);
					sendString(buff);
				}
				else
				{
					displayErrorID(RxMessage.StdId);
				}
			}
			if (TRUE == exitState)
			{
				state = nextState;
				enteringState = TRUE;
			}
		}
		break;
		case RING_2:
		{
			if (TRUE == enteringState )
			{
				displayString(2,TRUE);
				enteringState = FALSE;
				TxMessage.StdId   = RING2_ID;
				TxMessage.Data[0] = REQUEST_DATA;
				TxMessage.RTR     = CAN_RTR_DATA;
				TxMessage.DLC     = 1;

				CAN_Transmit(CAN1, &TxMessage);//transmit
				//strcpy(TxMessage.Data,"        ");
				b50ms_Counter = FALSE;
				i50ms_Counter = 0;
			}

			while ((CAN_MessagePending(CAN1,CAN_FIFO0) == 0) && (FALSE == b50ms_Counter));

			if (TRUE == b50ms_Counter )
			{
				nextState = RING_3;
				exitState = TRUE;
				b50ms_Counter = FALSE;
				enteringState = TRUE;

				_rings[0].roll =  NULL_SENSOR;
				_rings[0].higherLux =  NULL_SENSOR;
				_rings[0].successfullTransmision = FALSE;

				sprintf(buff,"***** RING2 NOT RESPONSE IN 50ms ***** \n\r");
				sendString(buff);

			}
			else
			{
				CAN_Receive(CAN1,CAN_FIFO0,&RxMessage);
				if (RING1_ID == RxMessage.StdId )
				{

					_rings[1].roll			=  RxMessage.Data[1] + (RxMessage.Data[0] << 8 );
					_rings[1].higherLux     =  RxMessage.Data[3] + (RxMessage.Data[2] << 8 );
					_rings[1].successfullTransmision = TRUE;

					nextState = RING_3;
					exitState = TRUE;
					sprintf(buff,"RING2: Roll-->%u   HigherLux -->%u\n\r",_rings[1].roll,_rings[1].higherLux);
					sendString(buff);
				}
				else
				{
					displayErrorID(RxMessage.StdId);
				}
			}
			if (TRUE == exitState)
			{
				state = nextState;
				enteringState = TRUE;
			}
		}
		break;
		case RING_3:
		{
			if (TRUE == enteringState )
			{
				displayString(3,TRUE);
				enteringState = FALSE;
				TxMessage.StdId   = RING3_ID;
				TxMessage.Data[0] = REQUEST_DATA;
				TxMessage.RTR     = CAN_RTR_DATA;
				TxMessage.DLC     = 1;

				CAN_Transmit(CAN1, &TxMessage);//transmit
				//strcpy(TxMessage.Data,"        ");
				b50ms_Counter = FALSE;
				i50ms_Counter = 0;
			}

			while ((CAN_MessagePending(CAN1,CAN_FIFO0) == 0) && (FALSE == b50ms_Counter));

			if (TRUE == b50ms_Counter )
			{
				nextState = RING_4;
				exitState = TRUE;
				b50ms_Counter = FALSE;
				enteringState = TRUE;

				_rings[0].roll =  NULL_SENSOR;
				_rings[0].higherLux =  NULL_SENSOR;
				_rings[0].successfullTransmision = FALSE;

				sprintf(buff,"***** RING3 NOT RESPONSE IN 50ms ***** \n\r");
				sendString(buff);

			}
			else
			{
				CAN_Receive(CAN1,CAN_FIFO0,&RxMessage);
				if (RING1_ID == RxMessage.StdId )
				{

					_rings[1].roll			=  RxMessage.Data[1] + (RxMessage.Data[0] << 8 );
					_rings[1].higherLux     =  RxMessage.Data[3] + (RxMessage.Data[2] << 8 );
					_rings[1].successfullTransmision = TRUE;

					nextState = RING_4;
					exitState = TRUE;
					sprintf(buff,"RING3: Roll-->%u   HigherLux -->%u\n\r",_rings[1].roll,_rings[1].higherLux);
					sendString(buff);
				}
				else
				{
					displayErrorID(RxMessage.StdId);
				}
			}
			if (TRUE == exitState)
			{
				state = nextState;
				enteringState = TRUE;
			}
		}
		break;
		case RING_4:
		{
			if (TRUE == enteringState )
			{
				displayString(4,TRUE);
				enteringState = FALSE;
				TxMessage.StdId   = RING4_ID;
				TxMessage.Data[0] = REQUEST_DATA;
				TxMessage.RTR     = CAN_RTR_DATA;
				TxMessage.DLC     = 1;

				CAN_Transmit(CAN1, &TxMessage);//transmit
				//strcpy(TxMessage.Data,"        ");
				b50ms_Counter = FALSE;
				i50ms_Counter = 0;
			}

			while ((CAN_MessagePending(CAN1,CAN_FIFO0) == 0) && (FALSE == b50ms_Counter));

			if (TRUE == b50ms_Counter )
			{
				nextState = RING_5;
				exitState = TRUE;
				b50ms_Counter = FALSE;
				enteringState = TRUE;

				_rings[0].roll =  NULL_SENSOR;
				_rings[0].higherLux =  NULL_SENSOR;
				_rings[0].successfullTransmision = FALSE;

				sprintf(buff,"***** RING4 NOT RESPONSE IN 50ms ***** \n\r");
				sendString(buff);

			}
			else
			{
				CAN_Receive(CAN1,CAN_FIFO0,&RxMessage);
				if (RING1_ID == RxMessage.StdId )
				{

					_rings[1].roll			=  RxMessage.Data[1] + (RxMessage.Data[0] << 8 );
					_rings[1].higherLux     =  RxMessage.Data[3] + (RxMessage.Data[2] << 8 );
					_rings[1].successfullTransmision = TRUE;

					nextState = RING_5;
					exitState = TRUE;
					sprintf(buff,"RING4: Roll-->%u   HigherLux -->%u\n\r",_rings[1].roll,_rings[1].higherLux);
					sendString(buff);
				}
				else
				{
					displayErrorID(RxMessage.StdId);
				}
			}
			if (TRUE == exitState)
			{
				state = nextState;
				enteringState = TRUE;
			}
		}
		break;
		case RING_5:
		{
			if (TRUE == enteringState )
			{
				displayString(5,TRUE);
				enteringState = FALSE;
				TxMessage.StdId   = RING5_ID;
				TxMessage.Data[0] = REQUEST_DATA;
				TxMessage.RTR     = CAN_RTR_DATA;
				TxMessage.DLC     = 1;

				CAN_Transmit(CAN1, &TxMessage);//transmit
				//strcpy(TxMessage.Data,"        ");
				b50ms_Counter = FALSE;
				i50ms_Counter = 0;
			}

			while ((CAN_MessagePending(CAN1,CAN_FIFO0) == 0) && (FALSE == b50ms_Counter));

			if (TRUE == b50ms_Counter )
			{
				nextState = ROLL;
				exitState = TRUE;
				b50ms_Counter = FALSE;
				enteringState = TRUE;

				_rings[0].roll =  NULL_SENSOR;
				_rings[0].higherLux =  NULL_SENSOR;
				_rings[0].successfullTransmision = FALSE;

				sprintf(buff,"***** RING5 NOT RESPONSE IN 50ms ***** \n\r");
				sendString(buff);

			}
			else
			{
				CAN_Receive(CAN1,CAN_FIFO0,&RxMessage);
				if (RING1_ID == RxMessage.StdId )
				{

					_rings[1].roll			=  RxMessage.Data[1] + (RxMessage.Data[0] << 8 );
					_rings[1].higherLux     =  RxMessage.Data[3] + (RxMessage.Data[2] << 8 );
					_rings[1].successfullTransmision = TRUE;

					nextState = ROLL;
					exitState = TRUE;
					sprintf(buff,"RING5: Roll-->%u   HigherLux -->%u\n\r",_rings[1].roll,_rings[1].higherLux);
					sendString(buff);
				}
				else
				{
					displayErrorID(RxMessage.StdId);
				}
			}
			if (TRUE == exitState)
			{
				state = nextState;
				enteringState = TRUE;
			}
		}
		break;

	default:
		break;
	}
}

static void delay(void)
{
	for (int i=0;i<1500000;i++);
}


float compute_roll(rings *sensor,int *validsRoll)
{

	double roll = 0;
	//Ver quais sÃ£o os dois mais intensos

	float fFirstHigherLux = sensor->sensor_lux[0];
	float fSecondHigherLux = 0;
	int iFirstBrightSensor = 0, iSecondBrightSensor = 0;

	if (sensor->successfullTransmision == TRUE)
	{
		(*validsRoll)++;
		for(int i=0; i<=7; i++)
		{
			if (sensor->sensor_lux[i] > fFirstHigherLux)
			{
				fFirstHigherLux = sensor->sensor_lux[i];
				iFirstBrightSensor = i;
			}
		}

		//		sprintf(buff,"HigherLux: %f  Sensor: %i.\n\r", fFirstHigherLux,iFirstBrightSensor);
		//		sendString(&buff);
		//calcular a percentagem da diferenÃ§a relativamente ao mais intenso

		if(iFirstBrightSensor >= 1 &&  iFirstBrightSensor <= 6)
		{
			/**
			 * Normal Case Highest Sensor is not 0.
			 */
			if (sensor->sensor_lux[iFirstBrightSensor + 1] > sensor->sensor_lux[iFirstBrightSensor - 1])
			{
				iSecondBrightSensor =  iFirstBrightSensor + 1;
				fSecondHigherLux    = sensor->sensor_lux[iFirstBrightSensor + 1];
			}
			else
			{
				iSecondBrightSensor =  iFirstBrightSensor - 1;
				fSecondHigherLux    = sensor->sensor_lux[iFirstBrightSensor - 1];
			}
		}
		else if ( iFirstBrightSensor == 0)
		{
			/**
			 *  Case when most bright sensor is the 0.
			 */
			if (sensor->sensor_lux[iFirstBrightSensor + 1] > sensor->sensor_lux[7])
			{
				iSecondBrightSensor =  iFirstBrightSensor + 1;
				fSecondHigherLux    = sensor->sensor_lux[iFirstBrightSensor + 1];
			}
			else
			{
				iSecondBrightSensor =  7;
				fSecondHigherLux    = sensor->sensor_lux[7];
			}
		}
		else
		{
			/**
			 *  Case when most bright sensor is the 7.
			 */
			if (sensor->sensor_lux[0] > sensor->sensor_lux[iFirstBrightSensor - 1])
			{
				iSecondBrightSensor =  0;
				fSecondHigherLux    = sensor->sensor_lux[0];
			}
			else
			{
				iSecondBrightSensor = iFirstBrightSensor - 1;
				fSecondHigherLux    = sensor->sensor_lux[iFirstBrightSensor - 1];
			}
		}

		//		sprintf(buff,"Second HigherLux: %f  Sensor: %i. \n", fSecondHigherLux,iSecondBrightSensor);
		//		sendString(&buff);

		float DiferencaEntreSensores;
		DiferencaEntreSensores = (fSecondHigherLux/fFirstHigherLux)*45;
		float fFirstHigherLuxAngle = iFirstBrightSensor * 45;
		float fSecondHigherLuxAngle = iSecondBrightSensor * 45;

		if(fFirstHigherLuxAngle > fSecondHigherLuxAngle ) { //estÃ¡ para a direita
			roll = fFirstHigherLuxAngle - DiferencaEntreSensores;
		}

		else if(fSecondHigherLuxAngle > fFirstHigherLuxAngle) { //estÃ¡ para a esquerda
			roll = fSecondHigherLuxAngle + DiferencaEntreSensores;
		}

	}
	else
	{
		sensor->successfullTransmision = TRUE;
	}


	sprintf(buff,"Angulo: %f\n\r", roll);
	sendString(&buff);

	return roll;
}

float compute_pitch(rings *sensor)
{
	float pitch;

	return pitch;

}
