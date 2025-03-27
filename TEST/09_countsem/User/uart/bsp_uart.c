#include "./uart/bsp_uart.h"


void USART1_Config(void){

	//����tx��rx����
	GPIO_InitTypeDef GPIO_InitStructure;
    USART_InitTypeDef USART_InitStructure;
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);
	

	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
	
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	
	GPIO_PinAFConfig(GPIOA, GPIO_PinSource9, GPIO_AF_USART1);//���ӵ�PA9����
	GPIO_PinAFConfig(GPIOA, GPIO_PinSource10, GPIO_AF_USART1);//���ӵ�PA10����
	
	USART_InitStructure.USART_BaudRate = 115200;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	USART_InitStructure.USART_StopBits = USART_StopBits_1;
	USART_InitStructure.USART_Parity = USART_Parity_No;
	USART_InitStructure.USART_Mode = USART_Mode_Tx | USART_Mode_Rx;
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_Init(USART1, &USART_InitStructure);
	
//	USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);//�� USART ���յ����ݲ�����������ݼĴ�����RDR��ʱ
	USART_Cmd(USART1, ENABLE);                    //RXNE ��־λ�ᱻ�� 1����ʾ�������ݼĴ����������ݿɶ�
}


/* �ض���printf���������� */
int fputc(int ch, FILE *f){

	USART_SendData(USART1, (uint16_t) ch);
	while(USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET);//�ȴ����ͼĴ���Ϊ��
	return (ch);
}
/* �ض���scanf���������� */
int fgetc(FILE *f)
{
		while (USART_GetFlagStatus(USART1, USART_FLAG_RXNE) == RESET);//�����յ�����

		return (int)USART_ReceiveData(USART1);
}


