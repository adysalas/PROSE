#include "communication.h"



CanTxMsg TxMessage;
CanRxMsg RxMessage;

rings finalData[5];
//data_sensors ring1,ring2,ring3,ring4,ring5;
rings ring1,ring2,ring3,ring4,ring5;
uint64_t tic = 0;
uint64_t toc = 0;
char group;

static void delay(void);

void state_MachineMain(void)
{
	static states actualRing = RING_1_0;
	static uint8_t enteringState = TRUE;
	static maxAttempts numMaxAttempts = 0;
	static uint8_t nothingReceived = FALSE;
	static uint8_t send = TRUE;


	//CAN_Receive(CAN1,0,&RxMessage);




	switch(actualRing)
	{

		case RING_1_0:
		{
			if (TRUE == enteringState)
			{
				toc = 0;
				displayString(1,TRUE);
				enteringState     = FALSE;
				CAN_CleanRxBuffer();
			}
			{
				if (TRUE == send )
				{
					send = FALSE;
					TxMessage.StdId   = RING1_ID;
					TxMessage.Data[0] = (uint8_t)0x00;
					TxMessage.RTR     = CAN_RTR_DATA;
					TxMessage.DLC     = 8;
					send = TRUE;
					CAN_Transmit(CAN1, &TxMessage);//transmit
				}


				while ( CAN_MessagePending(CAN1,CAN_FIFO0) != 0)
				{
					CAN_Receive(CAN1,CAN_FIFO0,&RxMessage);
				}

				if (RxMessage.StdId == RING1_ID)
				{
					//CAN_Receive(CAN1, 0 ,&RxMessage);
					actualRing = RING_1_1;
					numMaxAttempts = 0;
				}
			}
		}
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

				ring1.sensor_lux[0] =  NULL_SENSOR;
				ring1.sensor_lux[1] =  NULL_SENSOR;
				ring1.sensor_lux[2] =  NULL_SENSOR;
				ring1.sensor_lux[3] =  NULL_SENSOR;

			}
			else
			{
				CAN_Receive(CAN1,CAN_FIFO0,&RxMessage);
				displei();

				ring1.sensor_lux[0] =  RxMessage.Data[1] + (RxMessage.Data[0] << 8 );
				ring1.sensor_lux[1] =  RxMessage.Data[3] + (RxMessage.Data[2] << 8 );
				ring1.sensor_lux[2] =  RxMessage.Data[5] + (RxMessage.Data[4] << 8 );
				ring1.sensor_lux[3] =  RxMessage.Data[7] + (RxMessage.Data[6] << 8 );

				state = RING_1_1;

				delay();
				enteringState = TRUE;
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

				ring1.sensor_lux[4] =  NULL_SENSOR;
				ring1.sensor_lux[5] =  NULL_SENSOR;
				ring1.sensor_lux[6] =  NULL_SENSOR;
				ring1.sensor_lux[7] =  NULL_SENSOR;
			}
			else
			{
				CAN_Receive(CAN1,CAN_FIFO0,&RxMessage);
				displei();

				state = RING_2_0;

				delay();
				enteringState = TRUE;

				ring1.sensor_lux[4] =  RxMessage.Data[1] + (RxMessage.Data[0] << 8 );
				ring1.sensor_lux[5] =  RxMessage.Data[3] + (RxMessage.Data[2] << 8 );
				ring1.sensor_lux[6] =  RxMessage.Data[5] + (RxMessage.Data[4] << 8 );
				ring1.sensor_lux[7] =  RxMessage.Data[7] + (RxMessage.Data[6] << 8 );
			}

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

				ring2.sensor_lux[0] =  NULL_SENSOR;
				ring2.sensor_lux[1] =  NULL_SENSOR;
				ring2.sensor_lux[2] =  NULL_SENSOR;
				ring2.sensor_lux[3] =  NULL_SENSOR;
			}
			else
			{
				CAN_Receive(CAN1,CAN_FIFO0,&RxMessage);
				displei();

				state = RING_2_1;

				delay();
				enteringState = TRUE;

				ring2.sensor_lux[0] =  RxMessage.Data[1] + (RxMessage.Data[0] << 8 );
				ring2.sensor_lux[1] =  RxMessage.Data[3] + (RxMessage.Data[2] << 8 );
				ring2.sensor_lux[2] =  RxMessage.Data[5] + (RxMessage.Data[4] << 8 );
				ring2.sensor_lux[3] =  RxMessage.Data[7] + (RxMessage.Data[6] << 8 );
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

				ring2.sensor_lux[4] =  NULL_SENSOR;
				ring2.sensor_lux[5] =  NULL_SENSOR;
				ring2.sensor_lux[6] =  NULL_SENSOR;
				ring2.sensor_lux[7] =  NULL_SENSOR;
			}
			else
			{
				CAN_Receive(CAN1,CAN_FIFO0,&RxMessage);
				displei();

				state = RING_3_0;

				delay();
				enteringState = TRUE;

				ring2.sensor_lux[4] =  RxMessage.Data[1] + (RxMessage.Data[0] << 8 );
				ring2.sensor_lux[5] =  RxMessage.Data[3] + (RxMessage.Data[2] << 8 );
				ring2.sensor_lux[6] =  RxMessage.Data[5] + (RxMessage.Data[4] << 8 );
				ring2.sensor_lux[7] =  RxMessage.Data[7] + (RxMessage.Data[6] << 8 );
			}

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

				ring3.sensor_lux[0] =  NULL_SENSOR;
				ring3.sensor_lux[1] =  NULL_SENSOR;
				ring3.sensor_lux[2] =  NULL_SENSOR;
				ring3.sensor_lux[3] =  NULL_SENSOR;
			}
			else
			{
				CAN_Receive(CAN1,CAN_FIFO0,&RxMessage);
				displei();

				state = RING_3_1;

				delay();
				enteringState = TRUE;

				ring3.sensor_lux[0] =  RxMessage.Data[1] + (RxMessage.Data[0] << 8 );
				ring3.sensor_lux[1] =  RxMessage.Data[3] + (RxMessage.Data[2] << 8 );
				ring3.sensor_lux[2] =  RxMessage.Data[5] + (RxMessage.Data[4] << 8 );
				ring3.sensor_lux[3] =  RxMessage.Data[7] + (RxMessage.Data[6] << 8 );
			}


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

				ring3.sensor_lux[4] =  NULL_SENSOR;
				ring3.sensor_lux[5] =  NULL_SENSOR;
				ring3.sensor_lux[6] =  NULL_SENSOR;
				ring3.sensor_lux[7] =  NULL_SENSOR;
			}
			else
			{
				CAN_Receive(CAN1,CAN_FIFO0,&RxMessage);
				displei();

				state = RING_4_0;

				delay();
				enteringState = TRUE;

				ring3.sensor_lux[4] =  RxMessage.Data[1] + (RxMessage.Data[0] << 8 );
				ring3.sensor_lux[5] =  RxMessage.Data[3] + (RxMessage.Data[2] << 8 );
				ring3.sensor_lux[6] =  RxMessage.Data[5] + (RxMessage.Data[4] << 8 );
				ring3.sensor_lux[7] =  RxMessage.Data[7] + (RxMessage.Data[6] << 8 );
			}

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

				ring4.sensor_lux[0] =  NULL_SENSOR;
				ring4.sensor_lux[1] =  NULL_SENSOR;
				ring4.sensor_lux[2] =  NULL_SENSOR;
				ring4.sensor_lux[3] =  NULL_SENSOR;
			}
			else
			{
				CAN_Receive(CAN1,CAN_FIFO0,&RxMessage);
				displei();

				state = RING_4_1;

				delay();
				enteringState = TRUE;

				ring4.sensor_lux[0] =  RxMessage.Data[1] + (RxMessage.Data[0] << 8 );
				ring4.sensor_lux[1] =  RxMessage.Data[3] + (RxMessage.Data[2] << 8 );
				ring4.sensor_lux[2] =  RxMessage.Data[5] + (RxMessage.Data[4] << 8 );
				ring4.sensor_lux[3] =  RxMessage.Data[7] + (RxMessage.Data[6] << 8 );
			}


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

				ring4.sensor_lux[4] =  NULL_SENSOR;
				ring4.sensor_lux[5] =  NULL_SENSOR;
				ring4.sensor_lux[6] =  NULL_SENSOR;
				ring4.sensor_lux[7] =  NULL_SENSOR;
			}
			else
			{
				CAN_Receive(CAN1,CAN_FIFO0,&RxMessage);
				displei();

				state = RING_5_0;

				delay();
				enteringState = TRUE;

				ring4.sensor_lux[4] =  RxMessage.Data[1] + (RxMessage.Data[0] << 8 );
				ring4.sensor_lux[5] =  RxMessage.Data[3] + (RxMessage.Data[2] << 8 );
				ring4.sensor_lux[6] =  RxMessage.Data[5] + (RxMessage.Data[4] << 8 );
				ring4.sensor_lux[7] =  RxMessage.Data[7] + (RxMessage.Data[6] << 8 );
			}

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

				ring5.sensor_lux[0] =  NULL_SENSOR;
				ring5.sensor_lux[1] =  NULL_SENSOR;
				ring5.sensor_lux[2] =  NULL_SENSOR;
				ring5.sensor_lux[3] =  NULL_SENSOR;
			}
			else
			{
				CAN_Receive(CAN1,CAN_FIFO0,&RxMessage);
				displei();

				state = RING_5_1;

				delay();
				enteringState = TRUE;

				ring5.sensor_lux[0] =  RxMessage.Data[1] + (RxMessage.Data[0] << 8 );
				ring5.sensor_lux[1] =  RxMessage.Data[3] + (RxMessage.Data[2] << 8 );
				ring5.sensor_lux[2] =  RxMessage.Data[5] + (RxMessage.Data[4] << 8 );
				ring5.sensor_lux[3] =  RxMessage.Data[7] + (RxMessage.Data[6] << 8 );
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

				ring5.sensor_lux[4] =  NULL_SENSOR;
				ring5.sensor_lux[5] =  NULL_SENSOR;
				ring5.sensor_lux[6] =  NULL_SENSOR;
				ring5.sensor_lux[7] =  NULL_SENSOR;
			}
			else
			{
				CAN_Receive(CAN1,CAN_FIFO0,&RxMessage);
				displei();

				state = ROLL_CALC;

				delay();
				enteringState = TRUE;

				ring5.sensor_lux[4] =  RxMessage.Data[1] + (RxMessage.Data[0] << 8 );
				ring5.sensor_lux[5] =  RxMessage.Data[3] + (RxMessage.Data[2] << 8 );
				ring5.sensor_lux[6] =  RxMessage.Data[5] + (RxMessage.Data[4] << 8 );
				ring5.sensor_lux[7] =  RxMessage.Data[7] + (RxMessage.Data[6] << 8 );
			}

		}
		break;
		case ROLL_CALC:
		{
			state = RING_1_0;
		}
		default:

			break;
	}
}


static void delay(void)
{
	for (int i=0;i<1500000;i++);
}


float compute_roll(rings *sensor) {

	double roll;
	//Ver quais são os dois mais intensos

	double int_primeiro = 0;
	double int_segundo = 0;
	int angulo_primeiro, angulo_segundo, primeiro_sensor, segundo_sensor;

	for(int i=0; i<=7; i++) {

		if(sensor->sensor_lux[i] >= int_segundo) {
			int_segundo = sensor->sensor_lux[i]; angulo_segundo = (i*45); segundo_sensor = i;
		}

		if(sensor->sensor_lux[i] >= int_primeiro) {
			int_primeiro = sensor->sensor_lux[i]; angulo_primeiro = (i*45); primeiro_sensor = i;
		}

	}

	//calcular a percentagem da diferença relativamente ao mais intenso

	float DiferencaEntreSensores;
	DiferencaEntreSensores = int_segundo/int_primeiro;

	//casos particulares do circulo trigonométrico
	if(primeiro_sensor == 0 && segundo_sensor == 7) {
		angulo_primeiro += 360;
		primeiro_sensor = 8;
	}

	else if(primeiro_sensor == 7 && segundo_sensor == 0) {
		angulo_segundo += 360;
	}


	if(primeiro_sensor > segundo_sensor ) { //está para a direita
		roll = angulo_primeiro - DiferencaEntreSensores * 45;
	}

	else if(segundo_sensor > primeiro_sensor) { //está para a esquerda
		roll = angulo_primeiro + DiferencaEntreSensores * 45;
	}

	return roll;
}
