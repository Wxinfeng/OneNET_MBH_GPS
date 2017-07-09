/**
	************************************************************
	************************************************************
	************************************************************
	*	文件名： 	net_IO.c
	*
	*	作者： 		张继瑞
	*
	*	日期： 		2016-11-23
	*
	*	版本： 		V1.0
	*
	*	说明： 		网络设备数据IO层
	*
	*	修改记录：	
	************************************************************
	************************************************************
	************************************************************
**/

//单片机头文件
#include "stm32f10x.h"

//网络设备数据IO
#include "net_io.h"

//硬件驱动
#include "delay.h"
#include "usart.h"

//C库
#include <string.h>




NET_IO_INFO netIOInfo;




/*
************************************************************
*	函数名称：	NET_IO_Init
*
*	函数功能：	初始化网络设备IO驱动层
*
*	入口参数：	无
*
*	返回参数：	无
*
*	说明：		底层的数据收发驱动
************************************************************
*/
void NET_IO_Init(void)
{

	Usart2_Init(115200);
	
	NET_IO_ClearRecive();

}

/*
************************************************************
*	函数名称：	NET_IO_Send
*
*	函数功能：	发送数据
*
*	入口参数：	str：需要发送的数据
*				len：数据长度
*
*	返回参数：	无
*
*	说明：		底层的数据发送驱动
*
************************************************************
*/
void NET_IO_Send(unsigned char *str, unsigned short len)
{

#if(USART_DMA_TX_EN == 0)
	unsigned short count = 0;
	
	for(; count < len; count++)
	{
		USART_SendData(NET_IO, *str++);									//发送数据
		while(USART_GetFlagStatus(NET_IO, USART_FLAG_TC) == RESET);		//等待发送完成
	}
#else
	unsigned int mAddr = (unsigned int)str;
	
	while(DMA_GetFlagStatus(DMA1_FLAG_TC7) == RESET);					//等待通道7传输完成
	DMA_ClearFlag(DMA1_FLAG_TC7);										//清除通道7传输完成标志
	
	USARTx_ResetMemoryBaseAddr(NET_IO, mAddr, len);
#endif

}

/*
************************************************************
*	函数名称：	NET_IO_WaitRecive
*
*	函数功能：	等待接收完成
*
*	入口参数：	无
*
*	返回参数：	REV_OK-接收完成		REV_WAIT-接收超时未完成
*
*	说明：		循环调用检测是否接收完成
************************************************************
*/
_Bool NET_IO_WaitRecive(void)
{

	if(netIOInfo.dataLen == 0) 						//如果接收计数为0 则说明没有处于接收数据中，所以直接跳出，结束函数
		return REV_WAIT;
		
	if(netIOInfo.dataLen == netIOInfo.dataLenPre)	//如果上一次的值和这次相同，则说明接收完毕
	{
		netIOInfo.dataLen = 0;						//清0接收计数
			
		return REV_OK;								//返回接收完成标志
	}
		
	netIOInfo.dataLenPre = netIOInfo.dataLen;		//置为相同
	
	return REV_WAIT;								//返回接收未完成标志

}

/*
************************************************************
*	函数名称：	NET_IO_ClearRecive
*
*	函数功能：	清空缓存
*
*	入口参数：	无
*
*	返回参数：	无
*
*	说明：		
************************************************************
*/
void NET_IO_ClearRecive(void)
{

	netIOInfo.dataLen = 0;
	
	memset(netIOInfo.buf, 0, sizeof(netIOInfo.buf));

}

/*
************************************************************
*	函数名称：	USART2_IRQHandler
*
*	函数功能：	接收中断
*
*	入口参数：	无
*
*	返回参数：	无
*
*	说明：		
************************************************************
*/
void USART2_IRQHandler(void)
{

	if(USART_GetITStatus(USART2, USART_IT_RXNE) != RESET) //接收中断
	{
		if(netIOInfo.dataLen >= sizeof(netIOInfo.buf))	netIOInfo.dataLen = 0; //防止串口被刷爆
		netIOInfo.buf[netIOInfo.dataLen++] = USART2->DR;
		
		USART_ClearFlag(USART2, USART_FLAG_RXNE);
	}

}
