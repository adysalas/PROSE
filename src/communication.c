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
int b5seg=0;

float rollGeral;
float pitchGeral;

char group;

int calibUp = FALSE;
int calibDown = FALSE;

static void delay(void);

float valueADCpotenciometroPC01;
float valueADCpotenciometroPC02;
float valueADCpotenciometroPC03;
float gx,gxx;
float gy,gyy;
float gz,gzz;
long int ADC_MAX_X=0,ADC_MAX_Y=0,ADC_MAX_Z=0;
long int ADC_MIN_X=4095,ADC_MIN_Y=4095,ADC_MIN_Z=4095;
uint16_t ADC1ConvertedValue[3];


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


void stateMachineV2(void)
{
	static states_v2 state = INIT_ACCELEROMETER;
	static int enteringState = TRUE;
	static int exitState = FALSE;
	states_v2 nextState;

	switch (state)
	{

	case INIT_ACCELEROMETER:
	{
		if (TRUE == enteringState )
		{
			enteringState = FALSE;
		}

		if (TRUE == bStartUp)
		{
			
			sprintf(buff,"Calibration of ADC Accelerometer \n\r");
			sendString(buff);
			if (FALSE == b5seg )
			{
				calibration();
			}
			else
			{
				exitState = TRUE;
				nextState = INIT;
			}
			
			
		}

		if (TRUE == exitState)
		{
			bStartUp = FALSE;
			exitState = FALSE;
			state = nextState;
			enteringState = TRUE;
		}	
	}
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
			exitState = FALSE;
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

			_rings[0].roll =  NULL_ROLL;
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
			exitState = FALSE;
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

			_rings[1].roll =  NULL_ROLL;
			_rings[1].higherLux =  NULL_SENSOR;
			_rings[1].successfullTransmision = FALSE;

			sprintf(buff,"***** RING2 NOT RESPONSE IN 50ms ***** \n\r");
			sendString(buff);

		}
		else
		{
			CAN_Receive(CAN1,CAN_FIFO0,&RxMessage);
			if (RING2_ID == RxMessage.StdId )
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
			exitState = FALSE;
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

			_rings[2].roll =  NULL_ROLL;
			_rings[2].higherLux =  NULL_SENSOR;
			_rings[2].successfullTransmision = FALSE;

			sprintf(buff,"***** RING3 NOT RESPONSE IN 50ms ***** \n\r");
			sendString(buff);

		}
		else
		{
			CAN_Receive(CAN1,CAN_FIFO0,&RxMessage);
			if (RING3_ID == RxMessage.StdId )
			{

				_rings[2].roll			=  RxMessage.Data[1] + (RxMessage.Data[0] << 8 );
				_rings[2].higherLux     =  RxMessage.Data[3] + (RxMessage.Data[2] << 8 );
				_rings[2].successfullTransmision = TRUE;

				nextState = RING_4;
				exitState = TRUE;
				sprintf(buff,"RING3: Roll-->%u   HigherLux -->%u\n\r",_rings[2].roll,_rings[2].higherLux);
				sendString(buff);
			}
			else
			{
				displayErrorID(RxMessage.StdId);
			}
		}
		if (TRUE == exitState)
		{
			exitState = FALSE;
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

			_rings[3].roll =  NULL_ROLL;
			_rings[3].higherLux =  NULL_SENSOR;
			_rings[3].successfullTransmision = FALSE;

			sprintf(buff,"***** RING4 NOT RESPONSE IN 50ms ***** \n\r");
			sendString(buff);

		}
		else
		{
			CAN_Receive(CAN1,CAN_FIFO0,&RxMessage);
			if (RING4_ID == RxMessage.StdId )
			{

				_rings[3].roll			=  RxMessage.Data[1] + (RxMessage.Data[0] << 8 );
				_rings[3].higherLux     =  RxMessage.Data[3] + (RxMessage.Data[2] << 8 );
				_rings[3].successfullTransmision = TRUE;

				nextState = RING_5;
				exitState = TRUE;
				sprintf(buff,"RING4: Roll-->%u   HigherLux -->%u\n\r",_rings[3].roll,_rings[3].higherLux);
				sendString(buff);
			}
			else
			{
				displayErrorID(RxMessage.StdId);
			}
		}
		if (TRUE == exitState)
		{
			exitState = FALSE;
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

			_rings[4].roll =  NULL_ROLL;
			_rings[4].higherLux =  NULL_SENSOR;
			_rings[4].successfullTransmision = FALSE;

			sprintf(buff,"***** RING5 NOT RESPONSE IN 50ms ***** \n\r");
			sendString(buff);

		}
		else
		{
			CAN_Receive(CAN1,CAN_FIFO0,&RxMessage);
			if (RING5_ID == RxMessage.StdId )
			{

				_rings[4].roll			=  RxMessage.Data[1] + (RxMessage.Data[0] << 8 );
				_rings[4].higherLux     =  RxMessage.Data[3] + (RxMessage.Data[2] << 8 );
				_rings[4].successfullTransmision = TRUE;

				nextState = ROLL;
				exitState = TRUE;
				sprintf(buff,"RING5: Roll-->%u   HigherLux -->%u\n\r",_rings[4].roll,_rings[4].higherLux);
				sendString(buff);
			}
			else
			{
				displayErrorID(RxMessage.StdId);
			}
		}
		if (TRUE == exitState)
		{
			exitState = FALSE;
			state = nextState;
			enteringState = TRUE;
		}
	}
	break;
	case ROLL:
	{
		int validRolls = 0;
		if (TRUE == enteringState )
		{
			enteringState = FALSE;
			sprintf(buff,"//// ROLL CALCULATION \\\\\\\\ \n\r");
			sendString(buff);
		}

		for (int i=0;i<5;i++)
		{
			rollGeral += rollValue(&_rings[i],i,&validRolls);
		}

		rollGeral /= validRolls;
		exitState = TRUE;
		nextState = PITCH;

		if (TRUE == exitState)
		{
			exitState = FALSE;
			state = nextState;
			enteringState = TRUE;
		}
	}
	break;
	case PITCH:
	{
		int validRolls = 0;
		if (TRUE == enteringState )
		{
			enteringState = FALSE;
			sprintf(buff,"//// PITCH CALCULATION \\\\\\\\ \n\r");
			sendString(buff);
		}
		/**
		 * 
		 * */
		readDMA();
		/*
		 * TODO: CALCULATE PITCH with the rest of values and with the actual "PITCH"
		 */
		nextState = RING_1;
		exitState = TRUE;
		if (TRUE == exitState)
		{
			exitState = FALSE;
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


float rollValue(rings *sensor,int numberOfSensor,int *validRolls)
{
	float roll = 0;

	if (0 == numberOfSensor || 1 == numberOfSensor || 3 == numberOfSensor || 4 == numberOfSensor)
	{
		if (NULL_ROLL == sensor->roll)
		{
			return 0;
		}
		else
		{
			(*validRolls)++;
			return sensor->roll;
		}

	}
}

void calibration(int cimaBaixo)
{
	// Fazer pedido aos aneis das pontas
	// Anotar os higher lux
	if(cimaBaixo)
	{

	}
}


float compute_pitch(rings *sensor)
{
	float pitch;

	return pitch;
}


float cmpt_roll_avrg(void)
{
	float xTotal, yTotal;
	float valR = PI / 180;
	float valD = 180 / PI;

	//	Convert from polar to cartesian and add them all
	for (int i=0; i<5; i++) {
		xTotal += cos(valR *_rings[i].roll);
		yTotal += sin(valR *_rings[i].roll);
	}

	return (valD * atan2( (yTotal / NUMBER_OF_RINGS) , (xTotal / NUMBER_OF_RINGS)));

}


void readDMA(void)
{
	while(!DMA_GetFlagStatus(DMA1_FLAG_TC1));
	DMA_ClearFlag(DMA1_FLAG_TC1);
	/* Convert each value readed on ADC1 by the DMA into Voltages Value usint the relationship Vread=ADCreaded*(3.3V/4095)
		* Where 3.3v is the Voltage Reference used by the uC.
		* 4095 is the max value that can give us the ADC with a resolution of 2^12
		*/
	valueADCpotenciometroPC01 = ADC1ConvertedValue[0] * (3.3 / 4095);
	valueADCpotenciometroPC02 = ADC1ConvertedValue[1] * (3.3 / 4095);
	valueADCpotenciometroPC03 = ADC1ConvertedValue[2] * (3.3 / 4095);

	/*
		* Convertion of Voltage values into G forces.
		*/
	gx = valorG1(ADC1ConvertedValue[2],ADC_MIN_X,ADC_MAX_X,SCALE_NEGATIVE,SCALE_POSITIVE);
	gy = valorG1(ADC1ConvertedValue[1],ADC_MIN_Y,ADC_MAX_Y,SCALE_NEGATIVE,SCALE_POSITIVE);
	gz = valorG1(ADC1ConvertedValue[0],ADC_MIN_Z,ADC_MAX_Z,SCALE_NEGATIVE,SCALE_POSITIVE);
}

void calibrationADC(void)
{
	while(!DMA_GetFlagStatus(DMA1_FLAG_TC1));
	DMA_ClearFlag(DMA1_FLAG_TC1);

	if (ADC1ConvertedValue[0] > ADC_MAX_Z)
	{
		ADC_MAX_Z = ADC1ConvertedValue[0];
	}

	if (ADC1ConvertedValue[0] < ADC_MIN_Z)
	{
		ADC_MIN_Z = ADC1ConvertedValue[0];
	}
	if (ADC1ConvertedValue[1] > ADC_MAX_Y)
	{
		ADC_MAX_Y = ADC1ConvertedValue[1];
	}

	if (ADC1ConvertedValue[1] < ADC_MIN_Y)
	{
		ADC_MIN_Y = ADC1ConvertedValue[1];
	}
	if (ADC1ConvertedValue[2] > ADC_MAX_X)
	{
		ADC_MAX_X = ADC1ConvertedValue[2];
	}

	if (ADC1ConvertedValue[2] < ADC_MIN_X)
	{
		ADC_MIN_X = ADC1ConvertedValue[2];
	}
}