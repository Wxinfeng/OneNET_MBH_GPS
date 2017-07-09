/**
	************************************************************
	************************************************************
	************************************************************
	*	�ļ����� 	onenetAPP.c
	*
	*	���ߣ� 		�ż���
	*
	*	���ڣ� 		2017-02-27
	*
	*	�汾�� 		V1.0
	*
	*	˵���� 		onenetƽ̨�·����ݵĴ���
	*
	*	�޸ļ�¼��	
	************************************************************
	************************************************************
	************************************************************
**/

//Э���
#include "onenet.h"
#include "fault.h"

//Ӧ��
#include "onenetAPP.h"

//�����豸����
#include "net_device.h"

//Ӳ������
#include "usart.h"

//C��
#include <string.h>


//==========================================================
//	�������ƣ�	OneNet_Event
//
//	�������ܣ�	ƽ̨�������ݼ��
//
//	��ڲ�����	dataPtr��ƽ̨���ص�����
//
//	���ز�����	��
//
//	˵����		
//==========================================================
void OneNet_App(unsigned char *dataPtr)
{

	if(strstr((char *)dataPtr, "CLOSED"))
	{
		UsartPrintf(USART_DEBUG, "TCP CLOSED\r\n");
		
		faultTypeReport = faultType = FAULT_PRO;						//���ΪЭ�����
		
		oneNetInfo.errCount++;
	}
	else
	{
		//������������Ƿ��ͳɹ�
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
