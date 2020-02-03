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
#include <math.h>

#define RING1_ID 					(0x01)
#define RING2_ID 					(0x02)
#define RING3_ID 					(0x03)
#define RING4_ID 					(0x04)
#define RING5_ID 					(0x05)
#define CALIBRATION_ID  			(0x10)
#define MAX_CAN_TRANSMIT_ATTEMPTS 	(3u)
#define HOLD_TIME					(125000ul)
#define CALIBRATION_DATA    		('I')
#define REQUEST_DATA    			('R')

#define TRUE (1u)
#define FALSE (0u)

#define NULL_SENSOR 				(0u)
#define NULL_ROLL   				(65535u)

#define PI							3.14159265
#define NUMBER_OF_RINGS				5

#define ADC1_DR_Address    ((uint32_t)0x4001244C)
#define ZERO_X  1.5 //
#define ZERO_Y  1.5 //
#define ZERO_Z  1.5 //
#define SENSIBILIDAD 0.3 // sensibilidad del sensor.
#define ADC1_DR_Address    ((uint32_t)0x4001244C)
#define MAX_ADC_READ 4095
#define MIN_ADC_READ 0
#define SCALE_NEGATIVE -3
#define SCALE_POSITIVE 3
#define DIST_MAX (10.0)

typedef struct ring{
	uint16_t sensor_lux[8];
	uint16_t higherLux;
	uint16_t roll;
	uint16_t lux_0m;
	uint16_t lux_1m;
	uint16_t lux_ref;
	int   successfullTransmision;
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

typedef enum{
	INIT = 0,
	INIT_ACCELEROMETER,
	/*CALIB_UP,
	CALIB_DOWN,*/
	RING_1,
	RING_2,
	RING_3,
	RING_4,
	RING_5,
	ROLL,
	PITCH
}states_v2;

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
extern int bStartUp;
extern int calibUp;
extern int calibDown;
extern int b5seg;


void CAN_Configuration(void);
void can_send(void);
void can_recieve(void);

void CAN_CleanRxBuffer(void);

void stateMachineReloaded(void);

float compute_pitch();
float cmpt_roll_avrg(void);
float rollValue(rings *sensor,int numberOfSensor,int *validRolls);
void calibration(int cimaBaixo);
void calibrationADC(void);
float valorG1(float ADCread, float minADC, float maxADC, float gMin, float gMax);
void readDMA(void);
void roli(void);

void accel_Setup();

#endif /* COMMUNICATION_H_ */

