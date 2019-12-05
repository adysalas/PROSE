#include "stm32f10x.h"
#include "stm32f10x_can.h"
#include "lcd.h"
#include "time.h"

char buff[30];
uint16_t ADC1_Addr[8];

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

void Init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;					//tx
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;					//rx
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_Init(GPIOA, &GPIO_InitStructure);


    //USART CONFIG
	NVIC_InitTypeDef NVIC_InitStructure;
	NVIC_Init(&NVIC_InitStructure);
	USART_InitTypeDef USART_InitStructure;

	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);

	USART_InitStructure.USART_BaudRate = 9600;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	USART_InitStructure.USART_StopBits = USART_StopBits_1;
	USART_InitStructure.USART_Parity = USART_Parity_No;
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;

	USART_Init(USART2, &USART_InitStructure);

	USART_Cmd(USART2, ENABLE);

	CAN_init();

}

void CAN_init(void)
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

void read_adc(void)
{
	//read_values_from_ADC_pins
	ADC_SoftwareStartConvCmd ( ADC1 , ENABLE );
	while(!DMA_GetFlagStatus(DMA1_FLAG_TC1));
	DMA_ClearFlag(DMA1_FLAG_TC1);
	DMA_ClearFlag(DMA1_FLAG_TC1);
}

void display(void)
{

	for(int i=0; i<8;i++)
	{
		sprintf(buff,"%d e %d \r\n",i,ADC1_Addr[i]);
		for(int a=0; buff[a]!='\0';a++)
		{
			while(USART_GetFlagStatus(USART2, USART_FLAG_TXE) == RESET);
			USART_SendData(USART2, buff[a]);
			USART_ClearFlag(USART2,USART_FLAG_TXE);
		}
	}
}

void can_send()
{
	/* Send a message with the CAN */
	CanTxMsg TxMessage;
	TxMessage.StdId = 0x01;			//identifier
	TxMessage.ExtId = 0x00;			//extended identifier
	TxMessage.IDE = CAN_ID_STD;		//type of identifier
	TxMessage.RTR = CAN_RTR_DATA;	//type of frame for message
	TxMessage.DLC = 8;				//length of frame
	TxMessage.Data[0] = 0x01;		//information to be transmitted, size of vector->8
	TxMessage.Data[1] = 0x01;
	TxMessage.Data[2] = 0x01;
	TxMessage.Data[3] = 0x01;
	TxMessage.Data[4] = 0x01;
	TxMessage.Data[5] = 0x01;
	TxMessage.Data[6] = 0x01;
	TxMessage.Data[7] = 0x01;
	CAN_Transmit(CAN1, &TxMessage);		//send information

}

void can_recieve()
{
	CanRxMsg RxMessage;
	CAN_Receive(CAN1, 0 ,&RxMessage);
}

int main(void)
{
	RCC_Config_HSI_PLL_64MHz();
	Init();
	lcd_init();
	rcc_lcd_info();
	while(1)
	{
		//read_adc();
		//can_recieve();
		//can_send();
		//can_send();
		display();
	}
}
