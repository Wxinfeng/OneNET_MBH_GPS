/**
	************************************************************
	************************************************************
	************************************************************
	*	文件名： 	edp_stream.c
	*
	*	作者： 		张继瑞
	*
	*	日期： 		2017-03-02
	*
	*	版本： 		V1.0
	*
	*	说明： 		EDP协议封装层
	*
	*	修改记录：	
	************************************************************
	************************************************************
	************************************************************
**/

//协议封装文件
#include "edp_stream.h"

//数据封装层
#include "dStream.h"

//硬件驱动
#include "usart.h"

//C库
#include <string.h>
#include <stdio.h>






//==========================================================
//	函数名称：	EDP_LoadDataStream
//
//	函数功能：	数据流封装
//
//	入口参数：	type：发送数据的格式
//				send_buf：发送缓存指针
//				devid：设备ID
//				apikey：apikey
//
//	返回参数：	数据装载的长度，0-失败
//
//	说明：		sendBuf传入之前需要清空
//==========================================================
short EDP_LoadDataStream(unsigned char type, char *devid, char *apikey, DATA_STREAM *streamArray, unsigned short streamArrayCnt, char *sendBuf, short maxLen)
{
	
	unsigned short numBytes = 0;
	
	UsartPrintf(USART_DEBUG, "Tips:	OneNet_SendData-EDP_TYPE%d\r\n", type);
	numBytes = DSTREAM_GetDataStream_Body(type, streamArray, streamArrayCnt, sendBuf, maxLen);				//封装Body、获取Body长度
	if(!numBytes)
		return 0;
	
	//UsartPrintf(USART_DEBUG, "EDP Len: %d\r\n", numBytes);
	return numBytes;

}

//==========================================================
//	函数名称：	EDP_LoadDataStream_Measure
//
//	函数功能：	测量数据流格式总长度
//
//	入口参数：	type：格式类型
//
//	返回参数：	数据流所需要的最大字节长度
//
//	说明：		
//==========================================================
short EDP_LoadDataStream_Measure(unsigned char type, char *devid, char *apikey, DATA_STREAM *streamArray, unsigned short streamArrayCnt)
{
	
	short bBytes = 0, bBytesPre = 0;
	char i = 0;

	for(; i < 10; i++)															//测量10次，取最大值
	{
		bBytes = DSTREAM_GetDataStream_Body_Measure(type, streamArray, streamArrayCnt);//测量Body长度
		
		if(bBytes < bBytesPre)
			bBytes = bBytesPre;
		else
			bBytesPre = bBytes;
	}
	
	bBytes += (8 - (bBytes % 8)) + 8;											//加一个数 使之变为8的倍数(多加1个8是为了多留一点空间)
	
	UsartPrintf(USART_DEBUG, "PRO_Load_DataStreamMeasure Max Bytes: %d\r\n", bBytes);
	
	return bBytes;

}
