/**
	************************************************************
	************************************************************
	************************************************************
	*	�ļ����� 	net_IO.c
	*
	*	���ߣ� 		�ż���
	*
	*	���ڣ� 		2016-11-23
	*
	*	�汾�� 		V1.0
	*
	*	˵���� 		�����豸����IO��
	*
	*	�޸ļ�¼��	
	************************************************************
	************************************************************
	************************************************************
**/

//��Ƭ��ͷ�ļ�
#include "stm32f10x.h"

//�����豸����IO
#include "net_io.h"

//Ӳ������
#include "delay.h"
#include "usart.h"

//C��
#include <string.h>




NET_IO_INFO netIOInfo;




/*
************************************************************
*	�������ƣ�	NET_IO_Init
*
*	�������ܣ�	��ʼ�������豸IO������
*
*	��ڲ�����	��
*
*	���ز�����	��
*
*	˵����		�ײ�������շ�����
************************************************************
*/
void NET_IO_Init(void)
{

	Usart2_Init(115200);
	
	NET_IO_ClearRecive();

}

/*
************************************************************
*	�������ƣ�	NET_IO_Send
*
*	�������ܣ�	��������
*
*	��ڲ�����	str����Ҫ���͵�����
*				len�����ݳ���
*
*	���ز�����	��
*
*	˵����		�ײ�����ݷ�������
*
************************************************************
*/
void NET_IO_Send(unsigned char *str, unsigned short len)
{

#if(USART_DMA_TX_EN == 0)
	unsigned short count = 0;
	
	for(; count < len; count++)
	{
		USART_SendData(NET_IO, *str++);									//��������
		while(USART_GetFlagStatus(NET_IO, USART_FLAG_TC) == RESET);		//�ȴ��������
	}
#else
	unsigned int mAddr = (unsigned int)str;
	
	while(DMA_GetFlagStatus(DMA1_FLAG_TC7) == RESET);					//�ȴ�ͨ��7�������
	DMA_ClearFlag(DMA1_FLAG_TC7);										//���ͨ��7������ɱ�־
	
	USARTx_ResetMemoryBaseAddr(NET_IO, mAddr, len);
#endif

}

/*
************************************************************
*	�������ƣ�	NET_IO_WaitRecive
*
*	�������ܣ�	�ȴ��������
*
*	��ڲ�����	��
*
*	���ز�����	REV_OK-�������		REV_WAIT-���ճ�ʱδ���
*
*	˵����		ѭ�����ü���Ƿ�������
************************************************************
*/
_Bool NET_IO_WaitRecive(void)
{

	if(netIOInfo.dataLen == 0) 						//������ռ���Ϊ0 ��˵��û�д��ڽ��������У�����ֱ����������������
		return REV_WAIT;
		
	if(netIOInfo.dataLen == netIOInfo.dataLenPre)	//�����һ�ε�ֵ�������ͬ����˵���������
	{
		netIOInfo.dataLen = 0;						//��0���ռ���
			
		return REV_OK;								//���ؽ�����ɱ�־
	}
		
	netIOInfo.dataLenPre = netIOInfo.dataLen;		//��Ϊ��ͬ
	
	return REV_WAIT;								//���ؽ���δ��ɱ�־

}

/*
************************************************************
*	�������ƣ�	NET_IO_ClearRecive
*
*	�������ܣ�	��ջ���
*
*	��ڲ�����	��
*
*	���ز�����	��
*
*	˵����		
************************************************************
*/
void NET_IO_ClearRecive(void)
{

	netIOInfo.dataLen = 0;
	
	memset(netIOInfo.buf, 0, sizeof(netIOInfo.buf));

}

/*
************************************************************
*	�������ƣ�	USART2_IRQHandler
*
*	�������ܣ�	�����ж�
*
*	��ڲ�����	��
*
*	���ز�����	��
*
*	˵����		
************************************************************
*/
void USART2_IRQHandler(void)
{

	if(USART_GetITStatus(USART2, USART_IT_RXNE) != RESET) //�����ж�
	{
		if(netIOInfo.dataLen >= sizeof(netIOInfo.buf))	netIOInfo.dataLen = 0; //��ֹ���ڱ�ˢ��
		netIOInfo.buf[netIOInfo.dataLen++] = USART2->DR;
		
		USART_ClearFlag(USART2, USART_FLAG_RXNE);
	}

}
