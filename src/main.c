/*
 * Team Members:
 * 		Ady Salas (1180092)
*		Ricardo Fernandes (1180099)
 *
 */

#include "stm32f10x.h"
#include "candriver.h"
#include <stdint.h>
#include "lcd.h"
#include "main.h"
#include "stm32f10x_can.h"
#include "system.h"


int main(void)
{

	RCC_Config_HSI_PLL_64MHz();
	Config_USART2();
	Config_Pin_General();
	Config_CAN();
	lcd_init();
	rcc_lcd_info();

    /* Infinite loop */
    for(;;)
    {
    	StateMachine_Sensors();
    }
}



void Config_USART2()
{
	USART_InitTypeDef USART_InitStructure;
	GPIO_InitTypeDef GPIO_InitStructure;

	/* USART2 is configured as follow:
		- BaudRate = 115200 baud
		- Word Length = 8 Bits
		- 1 Stop Bit
		- No parity
		- Hardware flow control disabled (RTS and CTS signals)
		- Receive and transmit enabled */

	/* Enable GPIOA clock */
	RCC_APB2PeriphClockCmd( RCC_APB2Periph_GPIOA , ENABLE );

	/* USART Periph clock enable */
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);

	GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_2;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_3;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);


	USART_InitStructure.USART_BaudRate = 115200;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	USART_InitStructure.USART_StopBits = USART_StopBits_1;
	USART_InitStructure.USART_Parity = USART_Parity_No;
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode = USART_Mode_Tx | USART_Mode_Rx;

	/* Configure the USART2 */
	USART_Init(USART2, &USART_InitStructure);
	/* Enable the USART2 */
	USART_Cmd(USART2, ENABLE);

	//Interrupcao_USART
	NVIC_InitTypeDef NVIC_UART2;

	NVIC_UART2.NVIC_IRQChannel = USART2_IRQn;
	NVIC_UART2.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_UART2.NVIC_IRQChannelSubPriority = 1;
	NVIC_UART2.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_UART2);

	USART_ITConfig(USART2,USART_IT_RXNE,ENABLE);
}

void USART_SendString2(char *message)
{
uint16_t cont_aux=0;

    while(cont_aux != strlen(message))
    {
        USART_SendData(USART2, (uint8_t) message[cont_aux]);
        while(USART_GetFlagStatus(USART2, USART_FLAG_TXE) == RESET)
        {
        }
        cont_aux++;
    }
}

void Config_Pin_General()
{
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);

	//LED_B0, LED_B1 e LED_B2
	//Inicializar variavel
	GPIO_InitTypeDef GPIO_B;
	//Configurar variavel
	GPIO_B.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_2;
	GPIO_B.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_B.GPIO_Mode = GPIO_Mode_Out_PP;
	//Atualizar Porto
	GPIO_Init(GPIOB, &GPIO_B);
	GPIO_WriteBit(GPIOB, GPIO_Pin_0, DISABLE);
	GPIO_WriteBit(GPIOB, GPIO_Pin_1, DISABLE);
	GPIO_WriteBit(GPIOB, GPIO_Pin_2, DISABLE);

}

void RCC_Config_HSI_PLL_64MHz() //HSI com PLL a frequencia maxima
{
	//Reset �s configurac�es
	RCC_DeInit(); //Faz reset ao sistema e coloca a configuraccao por omiss�o

	//Ativar Fonte
	RCC_HSICmd(ENABLE); //Ativar a HSI

	//Esperar arranque do clock
	while(RCC_GetFlagStatus(RCC_FLAG_HSIRDY)==RESET);

	//Configurar Wait States
	FLASH_SetLatency(FLASH_Latency_2);

	//Configurar os prescalers AHB, APB1 e APB2, respetivamente
	RCC_HCLKConfig(RCC_SYSCLK_Div1);//divisor unitario que � o divisor mais baixo possivel
	RCC_PCLK1Config(RCC_HCLK_Div2); //PCLK1 no valor maximo possivel, ou seja, colocar o divisor minimo possivel
	RCC_PCLK2Config(RCC_HCLK_Div1); //PCLK2 no valor maximo possivel, ou seja, colocar o divisor minimo possivel

	//Configuracao da PLL
	RCC_PLLConfig(RCC_PLLSource_HSI_Div2, RCC_PLLMul_16); //Obrigatorio dividir por 2 e multipicar pelo maior valor (x16)

	//Ativar PLL
	RCC_PLLCmd(ENABLE); //Ativar a PLL

	//Esperar pela ativacao da PLL
	while(RCC_GetFlagStatus(RCC_FLAG_PLLRDY)==RESET);

	//Configurar SYSCLK
	RCC_SYSCLKConfig(RCC_SYSCLKSource_PLLCLK); //Selecionar a fonte PLL

	//Aguardar que a fonte estabilize
	while(RCC_GetSYSCLKSource() != 0x08); //Esperar que a fonte PLL(0x08) estabilize

}
