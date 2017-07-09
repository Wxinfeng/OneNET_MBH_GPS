/**
	************************************************************
	************************************************************
	************************************************************
	*	�ļ����� 	edp_stream.c
	*
	*	���ߣ� 		�ż���
	*
	*	���ڣ� 		2017-03-02
	*
	*	�汾�� 		V1.0
	*
	*	˵���� 		EDPЭ���װ��
	*
	*	�޸ļ�¼��	
	************************************************************
	************************************************************
	************************************************************
**/

//Э���װ�ļ�
#include "edp_stream.h"

//���ݷ�װ��
#include "dStream.h"

//Ӳ������
#include "usart.h"

//C��
#include <string.h>
#include <stdio.h>






//==========================================================
//	�������ƣ�	EDP_LoadDataStream
//
//	�������ܣ�	��������װ
//
//	��ڲ�����	type���������ݵĸ�ʽ
//				send_buf�����ͻ���ָ��
//				devid���豸ID
//				apikey��apikey
//
//	���ز�����	����װ�صĳ��ȣ�0-ʧ��
//
//	˵����		sendBuf����֮ǰ��Ҫ���
//==========================================================
short EDP_LoadDataStream(unsigned char type, char *devid, char *apikey, DATA_STREAM *streamArray, unsigned short streamArrayCnt, char *sendBuf, short maxLen)
{
	
	unsigned short numBytes = 0;
	
	UsartPrintf(USART_DEBUG, "Tips:	OneNet_SendData-EDP_TYPE%d\r\n", type);
	numBytes = DSTREAM_GetDataStream_Body(type, streamArray, streamArrayCnt, sendBuf, maxLen);				//��װBody����ȡBody����
	if(!numBytes)
		return 0;
	
	//UsartPrintf(USART_DEBUG, "EDP Len: %d\r\n", numBytes);
	return numBytes;

}

//==========================================================
//	�������ƣ�	EDP_LoadDataStream_Measure
//
//	�������ܣ�	������������ʽ�ܳ���
//
//	��ڲ�����	type����ʽ����
//
//	���ز�����	����������Ҫ������ֽڳ���
//
//	˵����		
//==========================================================
short EDP_LoadDataStream_Measure(unsigned char type, char *devid, char *apikey, DATA_STREAM *streamArray, unsigned short streamArrayCnt)
{
	
	short bBytes = 0, bBytesPre = 0;
	char i = 0;

	for(; i < 10; i++)															//����10�Σ�ȡ���ֵ
	{
		bBytes = DSTREAM_GetDataStream_Body_Measure(type, streamArray, streamArrayCnt);//����Body����
		
		if(bBytes < bBytesPre)
			bBytes = bBytesPre;
		else
			bBytesPre = bBytes;
	}
	
	bBytes += (8 - (bBytes % 8)) + 8;											//��һ���� ʹ֮��Ϊ8�ı���(���1��8��Ϊ�˶���һ��ռ�)
	
	UsartPrintf(USART_DEBUG, "PRO_Load_DataStreamMeasure Max Bytes: %d\r\n", bBytes);
	
	return bBytes;

}
