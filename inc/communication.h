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
#define RING3_ID 					(0x03)
#define RING4_ID 					(0x04)
#define RING5_ID 					(0x05)
#define MAX_CAN_TRANSMIT_ATTEMPTS 	(3u)
#define HOLD_TIME					(125000ul)

#define TRUE (1u)
#define FALSE (0u)

#define NULL_SENSOR (0u)

typedef struct ring{
	float sensor_lux[8];
	float roll;
	float pitch;
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


typedef float data_sensors[8];


/**
 * Declaration of Extern Variables.
 */
extern CanTxMsg TxMessage;
extern CanRxMsg RxMessage;
extern int i50ms_Counter;
extern int b50ms_Counter;
extern char buff[100];


void CAN_Configuration(void);
void can_send(void);
void can_recieve(void);

void CAN_CleanRxBuffer(void);

void stateMachineReloaded(void);

float compute_pitch(rings *sensor);
float compute_roll(rings *sensor);


#endif /* COMMUNICATION_H_ */
