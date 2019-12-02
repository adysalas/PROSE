/*
 * system.h
 *
 *  Created on: Dec 2, 2019
 *      Author: Saldory
 */

#ifndef SYSTEM_H_
#define SYSTEM_H_


typedef enum
{
    INIT = 1,
} states;

#define bool int
#define TRUE 	(1)
#define FALSE 	(0)

void StateMachine_Sensors(void);

#endif /* SYSTEM_H_ */
