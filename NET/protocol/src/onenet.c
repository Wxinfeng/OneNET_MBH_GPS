/**
	************************************************************
	************************************************************
	************************************************************
	*	文件名： 	onenet.c
	*
	*	作者： 		张继瑞
	*
	*	日期： 		2017-02-27
	*
	*	版本： 		V1.1
	*
	*	说明： 		与onenet平台的数据交互接口层
	*
	*	修改记录：	V1.0：协议封装、返回判断都在同一个文件，并且不同协议接口不同。
	*				V1.1：提供统一接口供应用层使用，根据不同协议文件来封装协议相关的内容。
	************************************************************
	************************************************************
	************************************************************
**/

//单片机头文件
#include "stm32f10x.h"

//网络设备
#include "net_device.h"

//协议文件
#include "onenet.h"
#include "fault.h"

//硬件驱动
#include "usart.h"

#include "gps_config.h"

//图片数据文件
#include "image_2k.h"

//C库
#include <string.h>
#include <stdio.h>



ONETNET_INFO oneNetInfo = {"8707723", "4JJ0ldDiZeZQIjMV8wu7uNy92us=",
							"183.230.40.33", "80",
							NULL, 0,
							0, 0, 0, 0};





//==========================================================
//	函数名称：	OneNet_SendData
//
//	函数功能：	上传数据到平台
//
//	入口参数：	type：发送数据的格式
//				devid：设备ID
//				apikey：设备apikey
//				streamArray：数据流
//				streamArrayNum：数据流个数
//
//	返回参数：	0-成功		1-失败
//
//	说明：		
//==========================================================
_Bool OneNet_SendData(FORMAT_TYPE type, char *devid, char *apikey, DATA_STREAM *streamArray, unsigned short streamArrayCnt)
{
	
	_Bool status = 0;
	
	if(!oneNetInfo.netWork || NET_DEVICE_Get_DataMode() != DEVICE_DATA_MODE)
		return 1;
	
	NET_DEVICE_Set_DataMode(DEVICE_CMD_MODE);
	
	memset(oneNetInfo.buffer, 0, sizeof(oneNetInfo.bufferSize));
	
	if(HTTP_LoadDataStream(type, devid, apikey, streamArray, streamArrayCnt, oneNetInfo.buffer, oneNetInfo.bufferSize))
		NET_DEVICE_SendData((unsigned char *)oneNetInfo.buffer, strlen(oneNetInfo.buffer));
	else
		status = 1;
	
	faultTypeReport = FAULT_NONE; //发送之后清除标记
	
	NET_DEVICE_Set_DataMode(DEVICE_DATA_MODE);
	
	return status;
	
}

//==========================================================
//	函数名称：	OneNet_Status
//
//	函数功能：	连接状态检查
//
//	入口参数：	无
//
//	返回参数：	无
//
//	说明：		
//==========================================================
void OneNet_Status(void)
{
	
	unsigned char errType = 0;
	
	if(!oneNetInfo.netWork || NET_DEVICE_Get_DataMode() != DEVICE_DATA_MODE)	//如果网络为连接 或 不为数据收发模式
		return;
	
	NET_DEVICE_Set_DataMode(DEVICE_CMD_MODE);									//设置为命令收发模式
	
	errType = NET_DEVICE_Check();
	
	if(errType == CHECK_CLOSED || errType == CHECK_GOT_IP)
	{
		faultTypeReport = faultType = FAULT_PRO;								//标记为协议错误
		
		oneNetInfo.errCount++;
	}
	else if(errType == CHECK_NO_DEVICE)
	{
		faultTypeReport = faultType = FAULT_NODEVICE;							//标记为设备错误
		
		oneNetInfo.errCount++;
	}
	else
	{
		faultTypeReport = faultType = FAULT_NONE;								//无错误
	}
	
	NET_DEVICE_ClrData();														//情况缓存
	
	NET_DEVICE_Set_DataMode(DEVICE_DATA_MODE);									//设置为数据收发模式
	
}

//==========================================================
//	函数名称：	OneNet_FillBuf
//
//	函数功能：	GPS信息封包
//
//	入口参数：	buf :待发送数据缓存包
//
//	返回参数：	无
//
//	说明：		
//==========================================================
void OneNet_FillBuf(char *buf, char *devid, char *apikey)
{
	
	char text[128];
	char buf1[256];
	
	memset(text, 0, sizeof(text));
	memset(buf1, 0, sizeof(buf1));
	
		strcpy(buf1, "{");
	
	memset(text, 0, sizeof(text));
	sprintf(text, "\"GPS\":{\"lon\":%f,\"lat\":%f}", lon, lat);
	strcat(buf1, text);
	
	strcat(buf1, "}");
	
	sprintf(buf, "POST /devices/%s/datapoints?type=3 HTTP/1.1\r\napi-key:%s\r\nHost:api.heclouds.com\r\n"
					"Content-Length:%d\r\n\r\n",
	
					devid, apikey, strlen(buf1));
					
	strcat(buf, buf1);
}

//==========================================================
//	函数名称：	OneNet_SendGPS
//
//	函数功能：	上传数据到平台
//
//	入口参数：	type：发送数据的格式
//
//	返回参数：	0-成功		1-失败
//
//	说明：		
//==========================================================
_Bool OneNet_SendGPS(FORMAT_TYPE type, char *devid, char *apikey, DATA_STREAM *streamArray, unsigned short streamArrayCnt)
{
	char send_buf[512];				
	_Bool status = 0;
	
	if(!oneNetInfo.netWork || NET_DEVICE_Get_DataMode() != DEVICE_DATA_MODE)
		return 1;
	
	NET_DEVICE_Set_DataMode(DEVICE_CMD_MODE);
	
	OneNet_FillBuf(send_buf, devid, apikey);								//封装数据流
	
	NET_DEVICE_SendData((unsigned char *)send_buf, strlen(send_buf));	//上传数据
	
	UsartPrintf(USART_DEBUG, "Tips:	OneNet_SendData-HTTP_TYPE%d\r\n", type);
	
	faultTypeReport = FAULT_NONE; //发送之后清除标记
	
	NET_DEVICE_Set_DataMode(DEVICE_DATA_MODE);
	
	memset(send_buf, 0, sizeof(send_buf));
	
	return status;
}
