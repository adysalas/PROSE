/*
 * communication.h
 *
 *  Created on: Dec 9, 2019
 *      Author: Saldory
 */

#ifndef COMMUNICATION_H_
#define COMMUNICATION_H_

#include "stm32f10x.h"
#include "stm32f10x_can.h"
#include "time.h"

#define RING1_ID 					(0x01)
#define RING2_ID 					(0x02)
#define RING3_ID 					(0x02)
#define RING4_ID 					(0x04)
#define RING5_ID 					(0x05)
#define MAX_CAN_TRANSMIT_ATTEMPTS 	(3u)
#define HOLD_TIME					(250000ul)

#define TRUE (1u)
#define FALSE (0u)

#define NULL_SENSOR (0u)

typedef struct ring{
	float sensor_lux[8];
	float roll;
}rings;


#define maxAttempts uint8_t


#define STATIC_UINT8(variable_name)  (static uint8 (variable_name))

typedef enum{
		RING_1_0 = 0,
		RING_1_1,
		RING_2_0,
		RING_2_1,
		RING_3_0,
		RING_3_1,
		RING_4_0,
		RING_4_1,
		RING_5_0,
		RING_5_1,
		ROLL_CALC
}states ;

typedef float sensor;


typedef union _data_sensors{
	sensor sensor1;
	sensor sensor2;
	sensor sensor3;
	sensor sensor4;
	sensor sensor5;
	sensor sensor6;
	sensor sensor7;
	sensor sensor8;

	sensor sensors[8];
}data_sensors;


/**
 * Declaration of Extern Variables.
 */
extern CanTxMsg TxMessage;
extern CanRxMsg RxMessage;


void CAN_Configuration(void);
void can_send(void);
void can_recieve(void);
void displayString(int value);
void displayString2(int value);
void displei(void);


#endif /* COMMUNICATION_H_ */
