/**
	************************************************************
	************************************************************
	************************************************************
	*	�ļ����� 	usart.c
	*
	*	���ߣ� 		�ż���
	*
	*	���ڣ� 		2017-05-11
	*
	*	�汾�� 		V1.1
	*
	*	˵���� 		��Ƭ�����������ʼ������ʽ����ӡ
	*
	*	�޸ļ�¼��	V1.1������DMA���ܷ���
	************************************************************
	************************************************************
	************************************************************
**/

//Ӳ������
#include "usart.h"

//C��
#include <stdarg.h>
#include <string.h>
#include <stdio.h>


ALTER_INFO alterInfo;


unsigned char UsartPrintfBuf[296];


/*
************************************************************
*	�������ƣ�	Usart1_Init
*
*	�������ܣ�	����1��ʼ��
*
*	��ڲ�����	baud���趨�Ĳ�����
*
*	���ز�����	��
*
*	˵����		TX-PA9		RX-PA10
*				���ͣ�DMA1_Channel4
*				���գ�DMA1_Channel5
*				δʹ��DMA_TC�жϣ�������RTOS�汾���п��ܻ�ر����ж϶�������ѭ��
************************************************************
*/
void Usart1_Init(unsigned int baud)
{

	GPIO_InitTypeDef gpioInitStruct;
	USART_InitTypeDef usartInitStruct;
	NVIC_InitTypeDef nvicInitStruct;
#if(USART_DMA_TX_EN == 1)
	DMA_InitTypeDef dmaInitStruct;
	
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);								//ʹ��DMAʱ��
#endif
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);
	
	//PA9	TXD
	gpioInitStruct.GPIO_Mode = GPIO_Mode_AF_PP;
	gpioInitStruct.GPIO_Pin = GPIO_Pin_9;
	gpioInitStruct.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &gpioInitStruct);
	
	//PA10	RXD
	gpioInitStruct.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	gpioInitStruct.GPIO_Pin = GPIO_Pin_10;
	gpioInitStruct.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &gpioInitStruct);
	
	usartInitStruct.USART_BaudRate = baud;
	usartInitStruct.USART_HardwareFlowControl = USART_HardwareFlowControl_None;		//��Ӳ������
	usartInitStruct.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;						//���պͷ���
	usartInitStruct.USART_Parity = USART_Parity_No;									//��У��
	usartInitStruct.USART_StopBits = USART_StopBits_1;								//1λֹͣλ
	usartInitStruct.USART_WordLength = USART_WordLength_8b;							//8λ����λ
	USART_Init(USART1, &usartInitStruct);
	
	USART_Cmd(USART1, ENABLE);														//ʹ�ܴ���
	
	USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);									//ʹ�ܽ����ж�
	
	nvicInitStruct.NVIC_IRQChannel = USART1_IRQn;
	nvicInitStruct.NVIC_IRQChannelCmd = ENABLE;
	nvicInitStruct.NVIC_IRQChannelPreemptionPriority = 0;
	nvicInitStruct.NVIC_IRQChannelSubPriority = 2;
	NVIC_Init(&nvicInitStruct);
	
#if(USART_DMA_TX_EN == 1)
	DMA_DeInit(DMA1_Channel4);														//��DMA��ͨ��1�Ĵ�������Ϊȱʡֵ
	dmaInitStruct.DMA_PeripheralBaseAddr = (unsigned int)&USART1->DR;				//DMA�������ַ
	dmaInitStruct.DMA_MemoryBaseAddr = NULL;										//DMA�ڴ����ַ
	dmaInitStruct.DMA_DIR = DMA_DIR_PeripheralDST;									//���ݴ��䷽�򣬴��ڴ��ȡ���͵�����
	dmaInitStruct.DMA_BufferSize = 0;												//DMAͨ����DMA����Ĵ�С
	dmaInitStruct.DMA_PeripheralInc = DMA_PeripheralInc_Disable;					//�����ַ�Ĵ�������
	dmaInitStruct.DMA_MemoryInc = DMA_MemoryInc_Enable;								//�ڴ��ַ�Ĵ�������
	dmaInitStruct.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;				//�������ݿ��Ϊ8λ
	dmaInitStruct.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;						//�ڴ����ݿ��Ϊ8λ
	dmaInitStruct.DMA_Mode = DMA_Mode_Normal;										//��������������ģʽ
	dmaInitStruct.DMA_Priority = DMA_Priority_Medium;								//DMAͨ��4ӵ�������ȼ�
	dmaInitStruct.DMA_M2M = DMA_M2M_Disable;										//DMAͨ��4û������Ϊ�ڴ浽�ڴ洫��
	DMA_Init(DMA1_Channel4, &dmaInitStruct);										//����dmaInitStruct��ָ���Ĳ�����ʼ��DMA��ͨ��USART1_Tx_DMA_Channel����ʶ�ļĴ���
	
//	nvicInitStruct.NVIC_IRQChannel = DMA1_Channel4_IRQn;							//����USART1��DMA�ж�
//	nvicInitStruct.NVIC_IRQChannelCmd = ENABLE;
//	nvicInitStruct.NVIC_IRQChannelPreemptionPriority = 1;
//	nvicInitStruct.NVIC_IRQChannelSubPriority = 2;
//	NVIC_Init(&nvicInitStruct);
//	DMA_ITConfig(DMA1_Channel4, DMA_IT_TC, ENABLE);									//������������ж�
	
	USART_DMACmd(USART1, USART_DMAReq_Tx, ENABLE);									//ʹ��USART1��DMA���͹���
	
	USARTx_ResetMemoryBaseAddr(USART1, (unsigned int)UsartPrintfBuf, 1);			//����һ������
#endif

}

/*
************************************************************
*	�������ƣ�	Usart2_Init
*
*	�������ܣ�	����2��ʼ��
*
*	��ڲ�����	baud���趨�Ĳ�����
*
*	���ز�����	��
*
*	˵����		TX-PA2		RX-PA3
*				���ͣ�DMA1_Channel7
*				���գ�DMA1_Channel6
*				δʹ��DMA_TC�жϣ�������RTOS�汾���п��ܻ�ر����ж϶�������ѭ��
************************************************************
*/
void Usart2_Init(unsigned int baud)
{

	GPIO_InitTypeDef gpioInitStruct;
	USART_InitTypeDef usartInitStruct;
	NVIC_InitTypeDef nvicInitStruct;
#if(USART_DMA_TX_EN == 1)
	DMA_InitTypeDef dmaInitStruct;
	
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);								//ʹ��DMAʱ��
#endif
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);
	
	//PA2	TXD
	gpioInitStruct.GPIO_Mode = GPIO_Mode_AF_PP;
	gpioInitStruct.GPIO_Pin = GPIO_Pin_2;
	gpioInitStruct.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &gpioInitStruct);
	
	//PA3	RXD
	gpioInitStruct.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	gpioInitStruct.GPIO_Pin = GPIO_Pin_3;
	gpioInitStruct.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &gpioInitStruct);
	
	usartInitStruct.USART_BaudRate = baud;
	usartInitStruct.USART_HardwareFlowControl = USART_HardwareFlowControl_None;		//��Ӳ������
	usartInitStruct.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;						//���պͷ���
	usartInitStruct.USART_Parity = USART_Parity_No;									//��У��
	usartInitStruct.USART_StopBits = USART_StopBits_1;								//1λֹͣλ
	usartInitStruct.USART_WordLength = USART_WordLength_8b;							//8λ����λ
	USART_Init(USART2, &usartInitStruct);
	
	USART_Cmd(USART2, ENABLE);														//ʹ�ܴ���
	
	USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);									//ʹ�ܽ����ж�
	
	nvicInitStruct.NVIC_IRQChannel = USART2_IRQn;
	nvicInitStruct.NVIC_IRQChannelCmd = ENABLE;
	nvicInitStruct.NVIC_IRQChannelPreemptionPriority = 0;
	nvicInitStruct.NVIC_IRQChannelSubPriority = 0;
	NVIC_Init(&nvicInitStruct);
	
#if(USART_DMA_TX_EN == 1)
	DMA_DeInit(DMA1_Channel7);														//��DMA��ͨ��1�Ĵ�������Ϊȱʡֵ
	dmaInitStruct.DMA_PeripheralBaseAddr = (unsigned int)&USART2->DR;				//DMA�������ַ
	dmaInitStruct.DMA_MemoryBaseAddr = NULL;										//DMA�ڴ����ַ
	dmaInitStruct.DMA_DIR = DMA_DIR_PeripheralDST;									//���ݴ��䷽�򣬴��ڴ��ȡ���͵�����
	dmaInitStruct.DMA_BufferSize = 0;												//DMAͨ����DMA����Ĵ�С
	dmaInitStruct.DMA_PeripheralInc = DMA_PeripheralInc_Disable;					//�����ַ�Ĵ�������
	dmaInitStruct.DMA_MemoryInc = DMA_MemoryInc_Enable;								//�ڴ��ַ�Ĵ�������
	dmaInitStruct.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;				//�������ݿ��Ϊ8λ
	dmaInitStruct.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;						//�ڴ����ݿ��Ϊ8λ
	dmaInitStruct.DMA_Mode = DMA_Mode_Normal;										//��������������ģʽ
	dmaInitStruct.DMA_Priority = DMA_Priority_Medium;								//DMAͨ��4ӵ�������ȼ�
	dmaInitStruct.DMA_M2M = DMA_M2M_Disable;										//DMAͨ��4û������Ϊ�ڴ浽�ڴ洫��
	DMA_Init(DMA1_Channel7, &dmaInitStruct);										//����dmaInitStruct��ָ���Ĳ�����ʼ��DMA��ͨ��USART2_Tx_DMA_Channel����ʶ�ļĴ���
	
//	nvicInitStruct.NVIC_IRQChannel = DMA1_Channel7_IRQn;							//����USART2��DMA�ж�
//	nvicInitStruct.NVIC_IRQChannelCmd = ENABLE;
//	nvicInitStruct.NVIC_IRQChannelPreemptionPriority = 1;
//	nvicInitStruct.NVIC_IRQChannelSubPriority = 2;
//	NVIC_Init(&nvicInitStruct);
//	DMA_ITConfig(DMA1_Channel7, DMA_IT_TC, ENABLE);									//������������ж�
	
	USART_DMACmd(USART2, USART_DMAReq_Tx, ENABLE);									//ʹ��USART2��DMA���͹���
	
	USARTx_ResetMemoryBaseAddr(USART2, (unsigned int)UsartPrintfBuf, 1);			//����һ������
#endif

}

/*
************************************************************
*	�������ƣ�	Usart3_Init
*
*	�������ܣ�	����3��ʼ��
*
*	��ڲ�����	baud���趨�Ĳ�����
*
*	���ز�����	��
*
*	˵����		TX-PB1		RX-PB11
************************************************************
*/
void Usart3_Init(unsigned int baud)
{

	GPIO_InitTypeDef gpioInitStruct;
	USART_InitTypeDef usartInitStruct;
	NVIC_InitTypeDef nvicInitStruct;
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3, ENABLE);
	
	//PB10	TXD
	gpioInitStruct.GPIO_Mode = GPIO_Mode_AF_PP;
	gpioInitStruct.GPIO_Pin = GPIO_Pin_10;
	gpioInitStruct.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &gpioInitStruct);
	
	//PB11	RXD
	gpioInitStruct.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	gpioInitStruct.GPIO_Pin = GPIO_Pin_11;
	gpioInitStruct.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &gpioInitStruct);
	
	usartInitStruct.USART_BaudRate = baud;
	usartInitStruct.USART_HardwareFlowControl = USART_HardwareFlowControl_None;		//��Ӳ������
	usartInitStruct.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;						//���պͷ���
	usartInitStruct.USART_Parity = USART_Parity_No;									//��У��
	usartInitStruct.USART_StopBits = USART_StopBits_1;								//1λֹͣλ
	usartInitStruct.USART_WordLength = USART_WordLength_8b;							//8λ����λ
	USART_Init(USART3, &usartInitStruct);
	
	USART_Cmd(USART3, ENABLE);														//ʹ�ܴ���
	
	USART_ITConfig(USART3, USART_IT_RXNE, ENABLE);									//ʹ�ܽ����ж�
	
//	nvicInitStruct.NVIC_IRQChannel = USART3_IRQn;
//	nvicInitStruct.NVIC_IRQChannelCmd = ENABLE;
//	nvicInitStruct.NVIC_IRQChannelPreemptionPriority = 0;
//	nvicInitStruct.NVIC_IRQChannelSubPriority = 0;
//	NVIC_Init(&nvicInitStruct);

}

/*
************************************************************
*	�������ƣ�	USARTx_ResetMemoryBaseAddr
*
*	�������ܣ�	����DMA�ڴ��ַ��ʹ�ܷ���
*
*	��ڲ�����	USARTx��������
*				mAddr���ڴ��ֵַ
*				num�����η��͵����ݳ���(�ֽ�)
*
*	���ز�����	��
*
*	˵����		
************************************************************
*/
void USARTx_ResetMemoryBaseAddr(USART_TypeDef *USARTx, unsigned int mAddr, unsigned short num)
{

#if(USART_DMA_TX_EN == 1)
	if(USARTx == USART1)
	{
		DMA_Cmd(DMA1_Channel4, DISABLE );				//�ر�USART1 TX DMA1 ��ָʾ��ͨ��
		
		DMA1_Channel4->CMAR = mAddr;					//DMAͨ�����ڴ��ַ
		DMA_SetCurrDataCounter(DMA1_Channel4, num);		//DMAͨ����DMA����Ĵ�С
		
		DMA_Cmd(DMA1_Channel4, ENABLE);
	}
	else if(USARTx == USART2)
	{
		DMA_Cmd(DMA1_Channel7, DISABLE );				//�ر�USART2 TX DMA1 ��ָʾ��ͨ��
		
		DMA1_Channel7->CMAR = mAddr;					//DMAͨ�����ڴ��ַ
		DMA_SetCurrDataCounter(DMA1_Channel7, num);		//DMAͨ����DMA����Ĵ�С
		
		DMA_Cmd(DMA1_Channel7, ENABLE);
	}
#endif

}

/*
************************************************************
*	�������ƣ�	Usart_SendString
*
*	�������ܣ�	�������ݷ���
*
*	��ڲ�����	USARTx��������
*				str��Ҫ���͵�����
*				len�����ݳ���
*
*	���ز�����	��
*
*	˵����		
************************************************************
*/
void Usart_SendString(USART_TypeDef *USARTx, unsigned char *str, unsigned short len)
{

#if(USART_DMA_TX_EN == 0)
	unsigned short count = 0;
	
	for(; count < len; count++)
	{
		USART_SendData(USARTx, *str++);									//��������
		while(USART_GetFlagStatus(USARTx, USART_FLAG_TC) == RESET);		//�ȴ��������
	}
#else
	unsigned int mAddr = (unsigned int)str;
	
	if(USARTx == USART1)
	{
		while(DMA_GetFlagStatus(DMA1_FLAG_TC4) == RESET);					//�ȴ�ͨ��4�������
		DMA_ClearFlag(DMA1_FLAG_TC4);										//���ͨ��4������ɱ�־
	}
	else if(USARTx == USART2)
	{
		while(DMA_GetFlagStatus(DMA1_FLAG_TC7) == RESET);					//�ȴ�ͨ��7�������
		DMA_ClearFlag(DMA1_FLAG_TC7);										//���ͨ��7������ɱ�־
	}
	
	USARTx_ResetMemoryBaseAddr(USARTx, mAddr, len);
#endif

}

/*
************************************************************
*	�������ƣ�	UsartPrintf
*
*	�������ܣ�	��ʽ����ӡ
*
*	��ڲ�����	USARTx��������
*				fmt����������
*
*	���ز�����	��
*
*	˵����		
************************************************************
*/
void UsartPrintf(USART_TypeDef *USARTx, char *fmt,...)
{

	va_list ap;
	unsigned char *pStr = UsartPrintfBuf;
	
#if(USART_DMA_TX_EN == 1)
	if(USARTx == USART1)
	{
		while(DMA_GetFlagStatus(DMA1_FLAG_TC4) == RESET);					//�ȴ�ͨ��4�������
		DMA_ClearFlag(DMA1_FLAG_TC4);										//���ͨ��4������ɱ�־
	}
	else if(USARTx == USART2)
	{
		while(DMA_GetFlagStatus(DMA1_FLAG_TC7) == RESET);					//�ȴ�ͨ��7�������
		DMA_ClearFlag(DMA1_FLAG_TC7);										//���ͨ��7������ɱ�־
	}
	
	memset(UsartPrintfBuf, 0, sizeof(UsartPrintfBuf));					//���buffer
#endif
	
	va_start(ap, fmt);
	vsprintf((char *)UsartPrintfBuf, fmt, ap);							//��ʽ��
	va_end(ap);
	
#if(USART_DMA_TX_EN == 1)
	USARTx_ResetMemoryBaseAddr(USARTx, (unsigned int)UsartPrintfBuf,
							strlen((const char *)pStr));
#else
	while(*pStr != 0)
	{
		USART_SendData(USARTx, *pStr++);
		while(USART_GetFlagStatus(USARTx, USART_FLAG_TC) == RESET);
	}
#endif

}


/*
************************************************************
*	�������ƣ�	DMA1_Channel4_IRQHandler
*
*	�������ܣ�	DMA1_ͨ��4_�жϺ���
*
*	��ڲ�����	��
*
*	���ز�����	��
*
*	˵����		
************************************************************
*/
void DMA1_Channel4_IRQHandler(void)
{

	if(DMA_GetITStatus(DMA1_IT_TC4) == SET)
	{
		DMA_ClearFlag(DMA1_IT_TC4);
	}

}

/*
************************************************************
*	�������ƣ�	DMA1_Channel7_IRQHandler
*
*	�������ܣ�	DMA1_ͨ��4_�жϺ���
*
*	��ڲ�����	��
*
*	���ز�����	��
*
*	˵����		
************************************************************
*/
void DMA1_Channel7_IRQHandler(void)
{

	if(DMA_GetITStatus(DMA1_IT_TC7) == SET)
	{
		DMA_ClearFlag(DMA1_IT_TC7);
	}

}

/*
************************************************************
*	�������ƣ�	USART1_IRQHandler
*
*	�������ܣ�	����1�շ��ж�
*
*	��ڲ�����	��
*
*	���ز�����	��
*
*	˵����		
************************************************************
*/
void USART1_IRQHandler(void)
{
	
	if(USART_GetITStatus(USART1, USART_IT_RXNE) != RESET) //�����ж�
	{
        if(alterInfo.alterCount > strlen(alterInfo.alterBuf))
            alterInfo.alterCount = 0;
        alterInfo.alterBuf[alterInfo.alterCount++] = USART1->DR;
		USART_ClearFlag(USART1, USART_FLAG_RXNE);
	}

}
