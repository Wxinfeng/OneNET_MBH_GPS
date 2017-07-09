/**
	************************************************************
	************************************************************
	************************************************************
	*	文件名： 	onenetAPP.c
	*
	*	作者： 		张继瑞
	*
	*	日期： 		2017-02-27
	*
	*	版本： 		V1.0
	*
	*	说明： 		onenet平台下发数据的处理
	*
	*	修改记录：	
	************************************************************
	************************************************************
	************************************************************
**/

//协议层
#include "onenet.h"
#include "fault.h"

//应用
#include "onenetAPP.h"

//网络设备驱动
#include "net_device.h"

//硬件驱动
#include "usart.h"

//C库
#include <string.h>


//==========================================================
//	函数名称：	OneNet_Event
//
//	函数功能：	平台返回数据检测
//
//	入口参数：	dataPtr：平台返回的数据
//
//	返回参数：	无
//
//	说明：		
//==========================================================
void OneNet_App(unsigned char *dataPtr)
{

	if(strstr((char *)dataPtr, "CLOSED"))
	{
		UsartPrintf(USART_DEBUG, "TCP CLOSED\r\n");
		
		faultTypeReport = faultType = FAULT_PRO;						//标记为协议错误
		
		oneNetInfo.errCount++;
	}
	else
	{
		//这里用来检测是否发送成功
		if(strstr((char *)dataPtr, "succ"))
		{
			UsartPrintf(USART_DEBUG, "Tips:		Send OK\r\n");
			oneNetInfo.errCount = 0;
		}
		else
		{
			UsartPrintf(USART_DEBUG, "Tips:		Send Err\r\n");
			oneNetInfo.errCount++;
		}
	}
	
	NET_DEVICE_ClrData();

}
