/**
	************************************************************
	************************************************************
	************************************************************
	*	文件名： 	http_stream.c
	*
	*	作者： 		张继瑞
	*
	*	日期： 		2017-02-28
	*
	*	版本： 		V1.0
	*
	*	说明： 		HTTP协议封装层
	*
	*	修改记录：	
	************************************************************
	************************************************************
	************************************************************
**/

//协议封装文件
#include "http_stream.h"

//数据封装层
#include "dStream.h"

//硬件驱动
#include "usart.h"

//C库
#include <string.h>
#include <stdio.h>






//==========================================================
//	函数名称：	HTTP_LoadDataStream
//
//	函数功能：	数据流封装
//
//	入口参数：	type：发送数据的格式
//				send_buf：发送缓存指针
//				devid：设备ID
//				apikey：apikey
//				streamArray：数据流
//				streamArrayNum：数据流个数
//
//	返回参数：	数据装载的长度，0-失败
//
//	说明：		sendBuf传入之前需要清空
//==========================================================
short HTTP_LoadDataStream(unsigned char type, char *devid, char *apikey, DATA_STREAM *streamArray, unsigned short streamArrayCnt, char *sendBuf, short maxLen)
{
	
	char *ptr = sendBuf;
	unsigned short strLen = 0, numBytes = 0;
	
	snprintf(sendBuf, maxLen, "POST /devices/%s/datapoints?type=%d HTTP/1.1\r\napi-key:%s\r\nHost:api.heclouds.com\r\n"
					"Content-Length:0000\r\n\r\n",									//预留0000，之后填装正确的长度
	
					devid, type, apikey);
	strLen = strlen(sendBuf);														//计算Head长度
	
	UsartPrintf(USART_DEBUG, "Tips:	OneNet_SendData-HTTP_TYPE%d\r\n", type);
	numBytes = DSTREAM_GetDataStream_Body(type, streamArray, streamArrayCnt, sendBuf + strLen, maxLen - strLen);	//封装Body、获取Body长度
	if(!numBytes)
		return 0;
	
	numBytes += strLen;																//Body长+Head长为总长
	strLen = strlen(sendBuf) - strLen;												//重计算Body长度
	
	ptr = strstr(sendBuf, "-Length:");												//搜索，准备替换Body长度数据
	if(ptr != NULL)
	{
		ptr += 8;																	//指向数据长度开头

		*ptr++ = strLen / 1000 % 10 + '0';
		*ptr++ = strLen / 100 % 10 + '0';
		*ptr++ = strLen / 10 % 10 + '0';
		*ptr++ = strLen / 1 % 10 + '0';
	}
	
	//UsartPrintf(USART_DEBUG, "HTTP Len: %d\r\n", numBytes);
	return numBytes;

}

//==========================================================
//	函数名称：	HTTP_LoadDataStream_HardWareHTTP
//
//	函数功能：	使用模块硬件HTTP时的封装函数
//
//	入口参数：	type：发送数据的格式
//				HTTP_Url：URL缓存指针
//				HTTP_UserData：UserData缓存指针
//				HTTP_Body：Body缓存指针
//				devid：设备ID
//				apikey：apikey
//
//	返回参数：	HTTP_Body长度
//
//	说明：		HTTP_Url、HTTP_UserData、HTTP_Body在传入之前需要清空
//				注意三个buffer的长度
//==========================================================
unsigned short HTTP_LoadDataStream_HardWareHTTP(unsigned char type, char *HTTP_Url, char *HTTP_UserData, char *HTTP_Body, char *devid, char *apikey)
{

	//api.heclouds.com/devices/4093253/datapoints?type=3
	sprintf(HTTP_Url, "api.heclouds.com/devices/%s/datapoints?type=%d", devid, type);
	
	//api-key:f=LzrTF6B77mlSm=vo=kmNp43oI=
	sprintf(HTTP_UserData, "api-key:%s", apikey);
	
	//例如{"rssi":68}
	//DSTREAM_GetDataStream_Body(type, HTTP_Body, 1024);
	
	return strlen(HTTP_Body);

}

//==========================================================
//	函数名称：	HTTP_LoadDataStream_Measure
//
//	函数功能：	测量数据流格式总长度
//
//	入口参数：	type：格式类型
//
//	返回参数：	数据流所需要的最大字节长度
//
//	说明：		
//==========================================================
short HTTP_LoadDataStream_Measure(unsigned char type, char *devid, char *apikey, DATA_STREAM *streamArray, unsigned short streamArrayCnt)
{
	
	short bBytes = 0, bBytesPre = 0;
	char i = 0;
	char sendBuf[160];
	
	memset(sendBuf, 0, sizeof(sendBuf));

	for(; i < 10; i++)															//测量10次，取最大值
	{
		bBytes = DSTREAM_GetDataStream_Body_Measure(type, streamArray, streamArrayCnt);//测量Body长度
		
		if(bBytes < bBytesPre)
			bBytes = bBytesPre;
		else
			bBytesPre = bBytes;
	}
	
	snprintf(sendBuf, sizeof(sendBuf), "POST /devices/%s/datapoints?type=%d HTTP/1.1\r\napi-key:%s\r\nHost:api.heclouds.com\r\n"
					"Content-Length:0000\r\n\r\n",
	
					devid, type, apikey);
	bBytes += strlen(sendBuf);													//加上Head长度
	
	bBytes += (8 - (bBytes % 8)) + 8;											//加一个数 使之变为8的倍数(多加1个8是为了多留一点空间)
	
	UsartPrintf(USART_DEBUG, "PRO_Load_DataStreamMeasure Max Bytes: %d\r\n", bBytes);
	
	return bBytes;

}
