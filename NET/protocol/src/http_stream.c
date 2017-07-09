/**
	************************************************************
	************************************************************
	************************************************************
	*	�ļ����� 	http_stream.c
	*
	*	���ߣ� 		�ż���
	*
	*	���ڣ� 		2017-02-28
	*
	*	�汾�� 		V1.0
	*
	*	˵���� 		HTTPЭ���װ��
	*
	*	�޸ļ�¼��	
	************************************************************
	************************************************************
	************************************************************
**/

//Э���װ�ļ�
#include "http_stream.h"

//���ݷ�װ��
#include "dStream.h"

//Ӳ������
#include "usart.h"

//C��
#include <string.h>
#include <stdio.h>






//==========================================================
//	�������ƣ�	HTTP_LoadDataStream
//
//	�������ܣ�	��������װ
//
//	��ڲ�����	type���������ݵĸ�ʽ
//				send_buf�����ͻ���ָ��
//				devid���豸ID
//				apikey��apikey
//				streamArray��������
//				streamArrayNum������������
//
//	���ز�����	����װ�صĳ��ȣ�0-ʧ��
//
//	˵����		sendBuf����֮ǰ��Ҫ���
//==========================================================
short HTTP_LoadDataStream(unsigned char type, char *devid, char *apikey, DATA_STREAM *streamArray, unsigned short streamArrayCnt, char *sendBuf, short maxLen)
{
	
	char *ptr = sendBuf;
	unsigned short strLen = 0, numBytes = 0;
	
	snprintf(sendBuf, maxLen, "POST /devices/%s/datapoints?type=%d HTTP/1.1\r\napi-key:%s\r\nHost:api.heclouds.com\r\n"
					"Content-Length:0000\r\n\r\n",									//Ԥ��0000��֮����װ��ȷ�ĳ���
	
					devid, type, apikey);
	strLen = strlen(sendBuf);														//����Head����
	
	UsartPrintf(USART_DEBUG, "Tips:	OneNet_SendData-HTTP_TYPE%d\r\n", type);
	numBytes = DSTREAM_GetDataStream_Body(type, streamArray, streamArrayCnt, sendBuf + strLen, maxLen - strLen);	//��װBody����ȡBody����
	if(!numBytes)
		return 0;
	
	numBytes += strLen;																//Body��+Head��Ϊ�ܳ�
	strLen = strlen(sendBuf) - strLen;												//�ؼ���Body����
	
	ptr = strstr(sendBuf, "-Length:");												//������׼���滻Body��������
	if(ptr != NULL)
	{
		ptr += 8;																	//ָ�����ݳ��ȿ�ͷ

		*ptr++ = strLen / 1000 % 10 + '0';
		*ptr++ = strLen / 100 % 10 + '0';
		*ptr++ = strLen / 10 % 10 + '0';
		*ptr++ = strLen / 1 % 10 + '0';
	}
	
	//UsartPrintf(USART_DEBUG, "HTTP Len: %d\r\n", numBytes);
	return numBytes;

}

//==========================================================
//	�������ƣ�	HTTP_LoadDataStream_HardWareHTTP
//
//	�������ܣ�	ʹ��ģ��Ӳ��HTTPʱ�ķ�װ����
//
//	��ڲ�����	type���������ݵĸ�ʽ
//				HTTP_Url��URL����ָ��
//				HTTP_UserData��UserData����ָ��
//				HTTP_Body��Body����ָ��
//				devid���豸ID
//				apikey��apikey
//
//	���ز�����	HTTP_Body����
//
//	˵����		HTTP_Url��HTTP_UserData��HTTP_Body�ڴ���֮ǰ��Ҫ���
//				ע������buffer�ĳ���
//==========================================================
unsigned short HTTP_LoadDataStream_HardWareHTTP(unsigned char type, char *HTTP_Url, char *HTTP_UserData, char *HTTP_Body, char *devid, char *apikey)
{

	//api.heclouds.com/devices/4093253/datapoints?type=3
	sprintf(HTTP_Url, "api.heclouds.com/devices/%s/datapoints?type=%d", devid, type);
	
	//api-key:f=LzrTF6B77mlSm=vo=kmNp43oI=
	sprintf(HTTP_UserData, "api-key:%s", apikey);
	
	//����{"rssi":68}
	//DSTREAM_GetDataStream_Body(type, HTTP_Body, 1024);
	
	return strlen(HTTP_Body);

}

//==========================================================
//	�������ƣ�	HTTP_LoadDataStream_Measure
//
//	�������ܣ�	������������ʽ�ܳ���
//
//	��ڲ�����	type����ʽ����
//
//	���ز�����	����������Ҫ������ֽڳ���
//
//	˵����		
//==========================================================
short HTTP_LoadDataStream_Measure(unsigned char type, char *devid, char *apikey, DATA_STREAM *streamArray, unsigned short streamArrayCnt)
{
	
	short bBytes = 0, bBytesPre = 0;
	char i = 0;
	char sendBuf[160];
	
	memset(sendBuf, 0, sizeof(sendBuf));

	for(; i < 10; i++)															//����10�Σ�ȡ���ֵ
	{
		bBytes = DSTREAM_GetDataStream_Body_Measure(type, streamArray, streamArrayCnt);//����Body����
		
		if(bBytes < bBytesPre)
			bBytes = bBytesPre;
		else
			bBytesPre = bBytes;
	}
	
	snprintf(sendBuf, sizeof(sendBuf), "POST /devices/%s/datapoints?type=%d HTTP/1.1\r\napi-key:%s\r\nHost:api.heclouds.com\r\n"
					"Content-Length:0000\r\n\r\n",
	
					devid, type, apikey);
	bBytes += strlen(sendBuf);													//����Head����
	
	bBytes += (8 - (bBytes % 8)) + 8;											//��һ���� ʹ֮��Ϊ8�ı���(���1��8��Ϊ�˶���һ��ռ�)
	
	UsartPrintf(USART_DEBUG, "PRO_Load_DataStreamMeasure Max Bytes: %d\r\n", bBytes);
	
	return bBytes;

}
