/**
	************************************************************
	************************************************************
	************************************************************
	*	�ļ����� 	onenet.c
	*
	*	���ߣ� 		�ż���
	*
	*	���ڣ� 		2017-02-27
	*
	*	�汾�� 		V1.1
	*
	*	˵���� 		��onenetƽ̨�����ݽ����ӿڲ�
	*
	*	�޸ļ�¼��	V1.0��Э���װ�������ж϶���ͬһ���ļ������Ҳ�ͬЭ��ӿڲ�ͬ��
	*				V1.1���ṩͳһ�ӿڹ�Ӧ�ò�ʹ�ã����ݲ�ͬЭ���ļ�����װЭ����ص����ݡ�
	************************************************************
	************************************************************
	************************************************************
**/

//��Ƭ��ͷ�ļ�
#include "stm32f10x.h"

//�����豸
#include "net_device.h"

//Э���ļ�
#include "onenet.h"
#include "fault.h"

//Ӳ������
#include "usart.h"

#include "gps_config.h"

//ͼƬ�����ļ�
#include "image_2k.h"

//C��
#include <string.h>
#include <stdio.h>



ONETNET_INFO oneNetInfo = {"8707723", "4JJ0ldDiZeZQIjMV8wu7uNy92us=",
							"183.230.40.33", "80",
							NULL, 0,
							0, 0, 0, 0};





//==========================================================
//	�������ƣ�	OneNet_SendData
//
//	�������ܣ�	�ϴ����ݵ�ƽ̨
//
//	��ڲ�����	type���������ݵĸ�ʽ
//				devid���豸ID
//				apikey���豸apikey
//				streamArray��������
//				streamArrayNum������������
//
//	���ز�����	0-�ɹ�		1-ʧ��
//
//	˵����		
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
	
	faultTypeReport = FAULT_NONE; //����֮��������
	
	NET_DEVICE_Set_DataMode(DEVICE_DATA_MODE);
	
	return status;
	
}

//==========================================================
//	�������ƣ�	OneNet_Status
//
//	�������ܣ�	����״̬���
//
//	��ڲ�����	��
//
//	���ز�����	��
//
//	˵����		
//==========================================================
void OneNet_Status(void)
{
	
	unsigned char errType = 0;
	
	if(!oneNetInfo.netWork || NET_DEVICE_Get_DataMode() != DEVICE_DATA_MODE)	//�������Ϊ���� �� ��Ϊ�����շ�ģʽ
		return;
	
	NET_DEVICE_Set_DataMode(DEVICE_CMD_MODE);									//����Ϊ�����շ�ģʽ
	
	errType = NET_DEVICE_Check();
	
	if(errType == CHECK_CLOSED || errType == CHECK_GOT_IP)
	{
		faultTypeReport = faultType = FAULT_PRO;								//���ΪЭ�����
		
		oneNetInfo.errCount++;
	}
	else if(errType == CHECK_NO_DEVICE)
	{
		faultTypeReport = faultType = FAULT_NODEVICE;							//���Ϊ�豸����
		
		oneNetInfo.errCount++;
	}
	else
	{
		faultTypeReport = faultType = FAULT_NONE;								//�޴���
	}
	
	NET_DEVICE_ClrData();														//�������
	
	NET_DEVICE_Set_DataMode(DEVICE_DATA_MODE);									//����Ϊ�����շ�ģʽ
	
}

//==========================================================
//	�������ƣ�	OneNet_FillBuf
//
//	�������ܣ�	GPS��Ϣ���
//
//	��ڲ�����	buf :���������ݻ����
//
//	���ز�����	��
//
//	˵����		
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
//	�������ƣ�	OneNet_SendGPS
//
//	�������ܣ�	�ϴ����ݵ�ƽ̨
//
//	��ڲ�����	type���������ݵĸ�ʽ
//
//	���ز�����	0-�ɹ�		1-ʧ��
//
//	˵����		
//==========================================================
_Bool OneNet_SendGPS(FORMAT_TYPE type, char *devid, char *apikey, DATA_STREAM *streamArray, unsigned short streamArrayCnt)
{
	char send_buf[512];				
	_Bool status = 0;
	
	if(!oneNetInfo.netWork || NET_DEVICE_Get_DataMode() != DEVICE_DATA_MODE)
		return 1;
	
	NET_DEVICE_Set_DataMode(DEVICE_CMD_MODE);
	
	OneNet_FillBuf(send_buf, devid, apikey);								//��װ������
	
	NET_DEVICE_SendData((unsigned char *)send_buf, strlen(send_buf));	//�ϴ�����
	
	UsartPrintf(USART_DEBUG, "Tips:	OneNet_SendData-HTTP_TYPE%d\r\n", type);
	
	faultTypeReport = FAULT_NONE; //����֮��������
	
	NET_DEVICE_Set_DataMode(DEVICE_DATA_MODE);
	
	memset(send_buf, 0, sizeof(send_buf));
	
	return status;
}
