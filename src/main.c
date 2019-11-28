/*
 * Team Members:
 * 		Ady Salas (1180092)
*		Ricardo Fernandes (1180099)
 *
 */

#include "stm32f10x.h"



void ledBlinkUsingST(uint8_t ValueInputPin);



uint8_t flagON = 1;
uint32_t number;
uint32_t tickResult = 0;
uint8_t value=0;




int main(void)
{
    /* Enable GPIOA clock */
   // RCC->APB2ENR = 0x00000004;

    /* GPIOA5 CNF: General Purpose Output push-pull  */
    //GPIOA->CRL &= ~0x00C00000;

    /* GPIOA5 MODE: Output mode, max speed 50 MHz */
   // GPIOA->CRL |= 0x00300000;

    /* Set GPIOA5 */
   //GPIOA->BSRR = 0x00000020;

    /* Reset GPIOA5 */
    //GPIOA->BSRR = 0x00200000;  //or GPIOA->BRR = 0x00000020;

	/*
	 * Using ST Library
	 */

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,1);
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA,&GPIO_InitStructure);


	/*
	 * Defining GPIOC as Input.
	 */
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC,1);
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13 ;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOC,&GPIO_InitStructure);


    /* Infinite loop */
    for(;;)
    {
    	//ledBlinkWithoutST();
    	ledBlinkUsingST(GPIO_ReadInputDataBit(GPIOC,GPIO_Pin_13));
    }
}

/*
 * Function to make blink the leds using a for function and a "counter"
 * Without using ST Peripherical Library
 */
void ledBlinkWithoutST()
{
	if (number > 0)
	{
		number--;
	}
	else
	{
		if(flagON)
		{
			number=0x0007FFFF;
			GPIOA->BSRR = 0x00200000;
			flagON=0;
		}
		else
		{
			number=0x0007FFFF;
			GPIOA->BSRR = 0x00000020;
			flagON=1;
		}
	}
}

/*
 * Function to make blink the leds using a for function and a "counter"
 * Using ST Peripherical Library and a input PIN to modify the frequency.
 */
void ledBlinkUsingST(uint8_t ValueInputPin)
{
	static uint8_t lastStatus = 1;

	if (number > 0)
	{
		if(lastStatus == ValueInputPin )
		{
			number--;
		}
		else
		{
			if (!lastStatus && ValueInputPin)
				number=number*2;
			else if(lastStatus && !ValueInputPin)
				number=number/2;
		}
	}
	else
	{
		if(flagON)
		{
			number=0x00008FFF;
			GPIO_WriteBit(GPIOA,GPIO_Pin_5,Bit_SET);
			flagON=0;
		}
		else
		{
			number=0x00008FFF;
			GPIO_WriteBit(GPIOA,GPIO_Pin_5,Bit_RESET);
			flagON=1;
		}
	}
	lastStatus = ValueInputPin;
}
