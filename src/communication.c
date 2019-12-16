#include "communication.h"



CanTxMsg TxMessage;
CanRxMsg RxMessage;

rings finalData[5];
data_sensors ring1,ring2,ring3,ring4,ring5;
uint64_t tic = 0;
uint64_t toc = 0;
char group;

void state_MachineMain(void)
{
	static states actualRing = RING_1_0;
	static uint8_t enteringState = TRUE;
	static maxAttempts numMaxAttempts = 0;
	static uint8_t nothingReceived = FALSE;
	static uint8_t send = TRUE;

	switch(actualRing)
	{

		case RING_1_0:
		{
			if (TRUE == enteringState)
			{
				toc = 0;
				displayString(1);
				enteringState     = FALSE;
			}
			{
				if (TRUE == send )
				{
					send = FALSE;
					TxMessage.StdId   = RING1_ID;
					TxMessage.Data[0] = (uint8_t)0x00;
					TxMessage.RTR     = CAN_RTR_DATA;
					TxMessage.DLC     = 8;

					CAN_Transmit(CAN1, &TxMessage);//transmit
				}


				//tic = clock();

				/**
				 *	TODO: WITH TOC IS NOT WORKING GOOD. CREATE TIMER AT 1MS to put this part good.
				 */
				CAN_Receive(CAN1,0,&RxMessage);
				if (RxMessage.StdId == RING1_ID)
				{
					//CAN_Receive(CAN1, 0 ,&RxMessage);
					actualRing = RING_1_1;
					numMaxAttempts = 0;
					ring1.sensor1 =  RxMessage.Data[1] + (RxMessage.Data[0] << 8 );
					ring1.sensor2 =  RxMessage.Data[3] + (RxMessage.Data[2] << 8 );
					ring1.sensor3 =  RxMessage.Data[5] + (RxMessage.Data[4] << 8 );
					ring1.sensor4 =  RxMessage.Data[7] + (RxMessage.Data[6] << 8 );
					displei();

				}
				else
				{
					toc++;
										//displei();
					if( (unsigned long)(toc - tic) >= HOLD_TIME)
					{
						//displei();
						numMaxAttempts ++;
						nothingReceived = TRUE;
						actualRing = RING_1_0;
						send = TRUE;
						break;
					}
				}

				if (TRUE == nothingReceived && numMaxAttempts == MAX_CAN_TRANSMIT_ATTEMPTS)
				{
					actualRing = RING_2_0;
					numMaxAttempts = 0;
					nothingReceived = FALSE;
					ring1.sensor1 =  NULL_SENSOR;
					ring1.sensor2 =  NULL_SENSOR;
					ring1.sensor3 =  NULL_SENSOR;
					ring1.sensor4 =  NULL_SENSOR;
					enteringState = TRUE;
				}
			}
		}
		break;
		case RING_1_1:
		{
			if (TRUE == enteringState)
			{
				toc = 0;
				displayString2(1);
				enteringState     = FALSE;
			}
			{
				if (TRUE == send )
				{
					TxMessage.StdId   = RING1_ID;
					TxMessage.Data[0] = (uint8_t)0x01;
					TxMessage.RTR     = CAN_RTR_DATA;
					TxMessage.DLC     = 8;

					CAN_Transmit(CAN1, &TxMessage);//transmit
					send = FALSE;
				}

				//tic = clock();

				/**
				 *	TODO: WITH TOC IS NOT WORKING GOOD. CREATE TIMER AT 1MS to put this part good.
				 */

				while(CAN_MessagePending(CAN1,CAN_FIFO0) != FALSE || numMaxAttempts <= MAX_CAN_TRANSMIT_ATTEMPTS)
				{   //wait for msg
					toc++;
					if( (unsigned long)(toc - tic) >= HOLD_TIME)
					{
						numMaxAttempts ++;
						nothingReceived = TRUE;
						actualRing = RING_1_1;
						send = TRUE;
						break;
					}
				}
				if (FALSE == nothingReceived)
				{
					CAN_Receive(CAN1, 0 ,&RxMessage);
					numMaxAttempts = 0;
					actualRing = RING_2_0;
					ring1.sensor5 =  RxMessage.Data[1] + (RxMessage.Data[0] << 8 );
					ring1.sensor6 =  RxMessage.Data[3] + (RxMessage.Data[2] << 8 );
					ring1.sensor7 =  RxMessage.Data[5] + (RxMessage.Data[4] << 8 );
					ring1.sensor8 =  RxMessage.Data[7] + (RxMessage.Data[6] << 8 );
					displei();
					enteringState = TRUE;
				}
				else if (TRUE == nothingReceived && numMaxAttempts == MAX_CAN_TRANSMIT_ATTEMPTS)
				{
					actualRing = RING_2_0;

					numMaxAttempts = 0;
					nothingReceived = FALSE;
					enteringState = TRUE;
					ring1.sensor5 =  NULL_SENSOR;
					ring1.sensor6 =  NULL_SENSOR;
					ring1.sensor7 =  NULL_SENSOR;
					ring1.sensor8 =  NULL_SENSOR;
				}
			}
		}
		break;
		case RING_2_0:
		{
			if (TRUE == enteringState)
			{
				toc = 0;
				displayString(2);
				enteringState     = FALSE;
			}
			{
				if (TRUE == send)
				{
					TxMessage.StdId   = RING2_ID;
					TxMessage.Data[0] = (uint8_t)0x00;
					TxMessage.RTR     = CAN_RTR_DATA;
					TxMessage.DLC     = 8;
					send = FALSE;

					CAN_Transmit(CAN1, &TxMessage);//transmit
				}


				//tic = clock();

				/**
				 *	TODO: WITH TOC IS NOT WORKING GOOD. CREATE TIMER AT 1MS to put this part good.
				 */

				while(CAN_MessagePending(CAN1,CAN_FIFO0) != FALSE || numMaxAttempts <= MAX_CAN_TRANSMIT_ATTEMPTS)
				{   //wait for msg
					toc++;
					if( (double)(toc - tic) >= HOLD_TIME)
					{
						numMaxAttempts ++;
						nothingReceived = TRUE;
						actualRing = RING_2_0;
						send = TRUE;
						break;
					}
				}
				if (FALSE == nothingReceived)
				{
					CAN_Receive(CAN1, 0 ,&RxMessage);
					actualRing = RING_2_1;
					numMaxAttempts = 0;
					ring2.sensor1 =  RxMessage.Data[1] + (RxMessage.Data[0] << 8 );
					ring2.sensor2 =  RxMessage.Data[3] + (RxMessage.Data[2] << 8 );
					ring2.sensor3 =  RxMessage.Data[5] + (RxMessage.Data[4] << 8 );
					ring2.sensor4 =  RxMessage.Data[7] + (RxMessage.Data[6] << 8 );
					displei();
				}
				else if (TRUE == nothingReceived && numMaxAttempts == MAX_CAN_TRANSMIT_ATTEMPTS)
				{
					actualRing = RING_3_0;
					numMaxAttempts = 0;
					nothingReceived = FALSE;
					enteringState = TRUE;
					ring2.sensor1 =  NULL_SENSOR;
					ring2.sensor2 =  NULL_SENSOR;
					ring2.sensor3 =  NULL_SENSOR;
					ring2.sensor4 =  NULL_SENSOR;
				}
			}
		}
		break;
		case RING_2_1:
		{
			if (TRUE == enteringState)
			{
				toc = 0;
				displayString2(2);
				enteringState     = FALSE;
			}
			{
				if (TRUE == send)
				{
					TxMessage.StdId   = RING2_ID;
					TxMessage.Data[0] = (uint8_t)0x01;
					TxMessage.RTR     = CAN_RTR_DATA;
					TxMessage.DLC     = 8;

					CAN_Transmit(CAN1, &TxMessage);//transmit

					send = FALSE;
				}

				//tic = clock();

				/**
				 *	TODO: WITH TOC IS NOT WORKING GOOD. CREATE TIMER AT 1MS to put this part good.
				 */

				while(CAN_MessagePending(CAN1,CAN_FIFO0) != FALSE || numMaxAttempts <= MAX_CAN_TRANSMIT_ATTEMPTS)
				{   //wait for msg
					toc++;
					if( (double)(toc - tic) >= HOLD_TIME)
					{
						numMaxAttempts ++;
						nothingReceived = TRUE;
						actualRing = RING_2_1;
						send = TRUE;
						break;
					}
				}
				if (FALSE == nothingReceived)
				{
					CAN_Receive(CAN1, 0 ,&RxMessage);
					actualRing = RING_3_0;
					numMaxAttempts = 0;
					enteringState = TRUE;
					ring2.sensor5 =  RxMessage.Data[1] + (RxMessage.Data[0] << 8 );
					ring2.sensor6 =  RxMessage.Data[3] + (RxMessage.Data[2] << 8 );
					ring2.sensor7 =  RxMessage.Data[5] + (RxMessage.Data[4] << 8 );
					ring2.sensor8 =  RxMessage.Data[7] + (RxMessage.Data[6] << 8 );
					displei();
				}
				else if (TRUE == nothingReceived && numMaxAttempts == MAX_CAN_TRANSMIT_ATTEMPTS)
				{
					actualRing = RING_3_0;
					numMaxAttempts = 0;
					nothingReceived = FALSE;
					enteringState = TRUE;
					ring2.sensor5 =  NULL_SENSOR;
					ring2.sensor6 =  NULL_SENSOR;
					ring2.sensor7 =  NULL_SENSOR;
					ring2.sensor8 =  NULL_SENSOR;
				}
			}
		}
		break;
		case RING_3_0:
		{
			if (TRUE == enteringState)
			{
				toc = 0;
				displayString(3);
				enteringState     = FALSE;
			}
			{
				if (TRUE == send)
				{
					TxMessage.StdId   = RING3_ID;
					TxMessage.Data[0] = (uint8_t)0x00;
					TxMessage.RTR     = CAN_RTR_DATA;
					TxMessage.DLC     = 8;

					CAN_Transmit(CAN1, &TxMessage);//transmit

					send = FALSE;
				}

				//tic = clock();

				/**
				 *	TODO: WITH TOC IS NOT WORKING GOOD. CREATE TIMER AT 1MS to put this part good.
				 */

				while(CAN_MessagePending(CAN1,CAN_FIFO0) != FALSE || numMaxAttempts <= MAX_CAN_TRANSMIT_ATTEMPTS)
				{   //wait for msg
					toc++;
					if( (double)(toc - tic) >= HOLD_TIME)
					{
						numMaxAttempts ++;
						nothingReceived = TRUE;
						actualRing = RING_3_0;
						send = TRUE;
						break;
					}
				}
				if (FALSE == nothingReceived)
				{
					CAN_Receive(CAN1, 0 ,&RxMessage);
					actualRing = RING_3_1;
					numMaxAttempts = 0;
					ring3.sensor1 =  RxMessage.Data[1] + (RxMessage.Data[0] << 8 );
					ring3.sensor2 =  RxMessage.Data[3] + (RxMessage.Data[2] << 8 );
					ring3.sensor3 =  RxMessage.Data[5] + (RxMessage.Data[4] << 8 );
					ring3.sensor4 =  RxMessage.Data[7] + (RxMessage.Data[6] << 8 );
					displei();
					enteringState = TRUE;
				}
				else if (TRUE == nothingReceived && numMaxAttempts == MAX_CAN_TRANSMIT_ATTEMPTS)
				{
					actualRing = RING_4_0;
					numMaxAttempts = 0;
					nothingReceived = FALSE;
					ring3.sensor1 =  NULL_SENSOR;
					ring3.sensor2 =  NULL_SENSOR;
					ring3.sensor3 =  NULL_SENSOR;
					ring3.sensor4 =  NULL_SENSOR;
					enteringState = TRUE;
				}
			}
		}
		break;
		case RING_3_1:
		{
			if (TRUE == enteringState)
			{
				toc = 0;
				enteringState     = FALSE;
				displayString2(3);
			}
			{
				if (TRUE == send)
				{
					TxMessage.StdId   = RING3_ID;
					TxMessage.Data[0] = (uint8_t)0x01;
					TxMessage.RTR     = CAN_RTR_DATA;
					TxMessage.DLC     = 8;

					CAN_Transmit(CAN1, &TxMessage);//transmit

					send = FALSE;
				}

				//tic = clock();

				/**
				 *	TODO: WITH TOC IS NOT WORKING GOOD. CREATE TIMER AT 1MS to put this part good.
				 */

				while(CAN_MessagePending(CAN1,CAN_FIFO0) != FALSE || numMaxAttempts <= MAX_CAN_TRANSMIT_ATTEMPTS)
				{   //wait for msg
					toc++;
					if( (double)(toc - tic) >= HOLD_TIME)
					{
						numMaxAttempts ++;
						nothingReceived = TRUE;
						actualRing = RING_3_1;
						send = TRUE;
						break;
					}
				}
				if (FALSE == nothingReceived)
				{
					CAN_Receive(CAN1, 0 ,&RxMessage);
					actualRing = RING_4_0;
					numMaxAttempts = 0;
					ring3.sensor5 =  RxMessage.Data[1] + (RxMessage.Data[0] << 8 );
					ring3.sensor6 =  RxMessage.Data[3] + (RxMessage.Data[2] << 8 );
					ring3.sensor7 =  RxMessage.Data[5] + (RxMessage.Data[4] << 8 );
					ring3.sensor8 =  RxMessage.Data[7] + (RxMessage.Data[6] << 8 );
					displei();
					enteringState = TRUE;
				}
				else if (TRUE == nothingReceived && numMaxAttempts == MAX_CAN_TRANSMIT_ATTEMPTS)
				{
					actualRing = RING_4_0;
					numMaxAttempts = 0;
					nothingReceived = FALSE;
					ring3.sensor5 =  NULL_SENSOR;
					ring3.sensor6 =  NULL_SENSOR;
					ring3.sensor7 =  NULL_SENSOR;
					ring3.sensor8 =  NULL_SENSOR;
					enteringState = TRUE;
				}
			}
		}
		break;
		case RING_4_0:
		{
			if (TRUE == enteringState)
			{
				toc = 0;
				displayString(4);
				enteringState     = FALSE;
			}
			{
				if (TRUE == send)
				{
					TxMessage.StdId   = RING4_ID;
					TxMessage.Data[0] = (uint8_t)0x00;
					TxMessage.RTR     = CAN_RTR_DATA;
					TxMessage.DLC     = 8;

					CAN_Transmit(CAN1, &TxMessage);//transmit

					send = FALSE;
				}

				//tic = clock();

				/**
				 *	TODO: WITH TOC IS NOT WORKING GOOD. CREATE TIMER AT 1MS to put this part good.
				 */

				while(CAN_MessagePending(CAN1,CAN_FIFO0) != FALSE || numMaxAttempts <= MAX_CAN_TRANSMIT_ATTEMPTS)
				{   //wait for msg
					toc++;
					if( (double)(toc - tic) >= HOLD_TIME)
					{
						numMaxAttempts ++;
						nothingReceived = TRUE;
						actualRing = RING_4_0;
						send = TRUE;

						break;
					}
				}
				if (FALSE == nothingReceived)
				{
					CAN_Receive(CAN1, 0 ,&RxMessage);
					actualRing = RING_4_1;
					numMaxAttempts = 0;
					ring4.sensor1 =  RxMessage.Data[1] + (RxMessage.Data[0] << 8 );
					ring4.sensor2 =  RxMessage.Data[3] + (RxMessage.Data[2] << 8 );
					ring4.sensor3 =  RxMessage.Data[5] + (RxMessage.Data[4] << 8 );
					ring4.sensor4 =  RxMessage.Data[7] + (RxMessage.Data[6] << 8 );
					displei();
					enteringState = TRUE;
				}
				else if (TRUE == nothingReceived && numMaxAttempts == MAX_CAN_TRANSMIT_ATTEMPTS)
				{
					actualRing = RING_5_0;
					numMaxAttempts = 0;
					nothingReceived = FALSE;
					ring4.sensor1 =  NULL_SENSOR;
					ring4.sensor2 =  NULL_SENSOR;
					ring4.sensor3 =  NULL_SENSOR;
					ring4.sensor4 =  NULL_SENSOR;
					enteringState = TRUE;
				}
			}
		}
		break;
		case RING_4_1:
		{
			if (TRUE == enteringState)
			{
				toc = 0;
				enteringState     = FALSE;
				displayString2(4);
			}
			{

				if (TRUE == send)
				{
					TxMessage.StdId   = RING4_ID;

					TxMessage.Data[0] = (uint8_t)0x01;
					TxMessage.RTR     = CAN_RTR_DATA;
					TxMessage.DLC     = 8;

					CAN_Transmit(CAN1, &TxMessage);//transmit

					send = FALSE;
				}
				//tic = clock();

				/**
				 *	TODO: WITH TOC IS NOT WORKING GOOD. CREATE TIMER AT 1MS to put this part good.
				 */

				while(CAN_MessagePending(CAN1,CAN_FIFO0) != FALSE || numMaxAttempts <= MAX_CAN_TRANSMIT_ATTEMPTS)
				{   //wait for msg
					toc++;
					if( (double)(toc - tic) >= HOLD_TIME)
					{
						numMaxAttempts ++;
						nothingReceived = TRUE;
						actualRing = RING_4_1;
						send = TRUE;
						break;
					}
				}
				if (FALSE == nothingReceived)
				{
					CAN_Receive(CAN1, 0 ,&RxMessage);
					actualRing = RING_5_0;
					numMaxAttempts = 0;
					ring4.sensor5 =  RxMessage.Data[1] + (RxMessage.Data[0] << 8 );
					ring4.sensor6 =  RxMessage.Data[3] + (RxMessage.Data[2] << 8 );
					ring4.sensor7 =  RxMessage.Data[5] + (RxMessage.Data[4] << 8 );
					ring4.sensor8 =  RxMessage.Data[7] + (RxMessage.Data[6] << 8 );
					enteringState = TRUE;
				}
				else if (TRUE == nothingReceived && numMaxAttempts == MAX_CAN_TRANSMIT_ATTEMPTS)
				{
					actualRing = RING_5_0;
					numMaxAttempts = 0;
					nothingReceived = FALSE;
					ring4.sensor5 =  NULL_SENSOR;
					ring4.sensor6 =  NULL_SENSOR;
					ring4.sensor7 =  NULL_SENSOR;
					ring4.sensor8 =  NULL_SENSOR;
					enteringState = TRUE;
				}
			}
		}
		break;
		case RING_5_0:
			{
				if (TRUE == enteringState)
				{
					toc = 0;
					displayString(5);
					enteringState     = FALSE;
				}
				{
					if (TRUE == send)
					{
						TxMessage.StdId   = RING5_ID;
						TxMessage.Data[0] = (uint8_t)0x00;
						TxMessage.RTR     = CAN_RTR_DATA;
						TxMessage.DLC     = 8;

						CAN_Transmit(CAN1, &TxMessage);//transmit

						send = FALSE;
					}

					//tic = clock();

					/**
					 *	TODO: WITH TOC IS NOT WORKING GOOD. CREATE TIMER AT 1MS to put this part good.
					 */

					while(CAN_MessagePending(CAN1,CAN_FIFO0) != FALSE || numMaxAttempts <= MAX_CAN_TRANSMIT_ATTEMPTS)
					{   //wait for msg
						toc++;
						if( (double)(toc - tic) >= HOLD_TIME)
						{
							numMaxAttempts ++;
							nothingReceived = TRUE;
							actualRing = RING_5_0;
							send = TRUE;
							break;
						}
					}
					if (FALSE == nothingReceived)
					{
						CAN_Receive(CAN1, 0 ,&RxMessage);
						actualRing = RING_5_1;
						numMaxAttempts = 0;
						ring5.sensor1 =  RxMessage.Data[1] + (RxMessage.Data[0] << 8 );
						ring5.sensor2 =  RxMessage.Data[3] + (RxMessage.Data[2] << 8 );
						ring5.sensor3 =  RxMessage.Data[5] + (RxMessage.Data[4] << 8 );
						ring5.sensor4 =  RxMessage.Data[7] + (RxMessage.Data[6] << 8 );
						enteringState = TRUE;
					}
					else if (TRUE == nothingReceived && numMaxAttempts == MAX_CAN_TRANSMIT_ATTEMPTS)
					{
						actualRing = ROLL_CALC;
						numMaxAttempts = 0;
						nothingReceived = FALSE;
						ring5.sensor1 =  NULL_SENSOR;
						ring5.sensor2 =  NULL_SENSOR;
						ring5.sensor3 =  NULL_SENSOR;
						ring5.sensor4 =  NULL_SENSOR;
						enteringState = TRUE;
					}
				}
			}
			break;
			case RING_5_1:
			{
				if (TRUE == enteringState)
				{
					toc = 0;
					enteringState  = FALSE;
					displayString2(5);
				}
				{
					if (TRUE == send)
					{
						TxMessage.StdId   = RING5_ID;
						TxMessage.Data[0] = (uint8_t)0x01;
						TxMessage.RTR     = CAN_RTR_DATA;
						TxMessage.DLC     = 8;

						CAN_Transmit(CAN1, &TxMessage);//transmit

						send = FALSE;
					}

					//tic = clock();

					/**
					 *	TODO: WITH TOC IS NOT WORKING GOOD. CREATE TIMER AT 1MS to put this part good.
					 */

					while(CAN_MessagePending(CAN1,CAN_FIFO0) != FALSE || numMaxAttempts <= MAX_CAN_TRANSMIT_ATTEMPTS)
					{   //wait for msg
						toc++;
						if( (double)(toc - tic) >= HOLD_TIME)
						{
							numMaxAttempts ++;
							nothingReceived = TRUE;
							actualRing = RING_5_1;
							break;
						}
					}
					if (FALSE == nothingReceived)
					{
						CAN_Receive(CAN1, 0 ,&RxMessage);
						//actualRing = ROLL_CALC;
						actualRing = RING_1_0;
						numMaxAttempts = 0;
						ring5.sensor5 =  RxMessage.Data[1] + (RxMessage.Data[0] << 8 );
						ring5.sensor6 =  RxMessage.Data[3] + (RxMessage.Data[2] << 8 );
						ring5.sensor7 =  RxMessage.Data[5] + (RxMessage.Data[4] << 8 );
						ring5.sensor8 =  RxMessage.Data[7] + (RxMessage.Data[6] << 8 );
						enteringState = TRUE;
					}
					else if (TRUE == nothingReceived && numMaxAttempts == MAX_CAN_TRANSMIT_ATTEMPTS)
					{
						//actualRing = ROLL_CALC;
						actualRing = RING_1_0;
						numMaxAttempts = 0;
						nothingReceived = FALSE;
						ring5.sensor5 =  NULL_SENSOR;
						ring5.sensor6 =  NULL_SENSOR;
						ring5.sensor7 =  NULL_SENSOR;
						ring5.sensor8 =  NULL_SENSOR;
						enteringState = TRUE;
					}

				}
			}
			break;
			case (ROLL_CALC):
			{
				toc = 0;
				actualRing = RING_1_0;
			}
			break;
			default:
				break;
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
