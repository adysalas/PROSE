/*
 * system.c
 *
 *  Created on: Dec 2, 2019
 *      Author: Saldory
 */
#include "system.h"

void StateMachine_Sensors()
{
	static states actualState  = INIT;
	static states nextState    = INIT;
	static bool   enterState   = TRUE;
	static bool   exitState    = FALSE;

	switch (actualState)
	{
		case (INIT):
		{
			if (TRUE == enterState)
			{

			}

			if (TRUE == exitState)
			{
				actualState = nextState;
			}
		}
		break;

		default:
			break;

	}

}
