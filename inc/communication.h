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

#define RING1_ID (0x01)
#define RING2_ID (0x02)
#define RING3_ID (0x02)
#define RING4_ID (0x04)
#define RING5_ID (0x05)

#define TRUE (1u)
#define FALSE (0u)

typedef struct ring{
	float sensor_lux[8];
	float roll;
}rings;


typedef enum{
		RING_1 = 0,
		RING_2,
		RING_3,
		RING_4,
		RING_5
		}states ;

/**
 * Declaration of Extern Variables.
 */
extern CanTxMsg TxMessage;
extern CanRxMsg RxMessage;


void CAN_Configuration(void);
void can_send(void);
void can_recieve(void);


#endif /* COMMUNICATION_H_ */
