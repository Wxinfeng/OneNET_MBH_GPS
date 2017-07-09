/**
  ******************************************************************************
  * @file    nmea_decode_test.c
  * @author  fire
  * @version V1.0
  * @date    2013-xx-xx
  * @brief   ����NEMA�����
  ******************************************************************************
  * @attention
  *
  * ʵ��ƽ̨:Ұ�� iSO STM32 ������ 
  * ��̳    :http://www.chuxue123.com
  * �Ա�    :http://firestm32.taobao.com
  *
  ******************************************************************************
  */ 
  
#include "stm32f10x.h"
//#include "bsp_sdio_sdcard.h"
#include "usart.h"	
//#include "bsp_usart2.h"
//#include "ff.h"
#include "nmea/nmea.h"
#include "gps_config.h"

nmeaINFO info;          //GPS�����õ�����Ϣ 

#ifdef __GPS_LOG_FILE             //��SD���ϵ�gpslog.txt�ļ����н��룻����Ҫ��sd���ϴ��gpslog.txt�ļ���




#else       //��GPSģ�鴫�ص���Ϣ���н���

/**
  * @brief  nmea_decode_test ����GPSģ����Ϣ
  * @param  ��
  * @retval ��
  */
void nmea_decode_test(void)
{

    
    nmeaPARSER parser;      //����ʱʹ�õ����ݽṹ  
    uint8_t new_parse=0;    //�Ƿ����µĽ������ݱ�־
  
    nmeaTIME beiJingTime;    //����ʱ�� 

    /* �����������������Ϣ�ĺ��� */
    nmea_property()->trace_func = &trace;
    nmea_property()->error_func = &error;

    /* ��ʼ��GPS���ݽṹ */
    nmea_zero_INFO(&info);
    nmea_parser_init(&parser);

      if(GPS_HalfTransferEnd)     /* ���յ�GPS_RBUFF_SIZEһ������� */
      {
        /* ����nmea��ʽ���� */
        nmea_parse(&parser, (const char*)&gps_rbuff[0], HALF_GPS_RBUFF_SIZE, &info);
        
        GPS_HalfTransferEnd = 0;   //��ձ�־λ
        new_parse = 1;             //���ý�����Ϣ��־ 
      }
      else if(GPS_TransferEnd)    /* ���յ���һ������ */
      {

        nmea_parse(&parser, (const char*)&gps_rbuff[HALF_GPS_RBUFF_SIZE], HALF_GPS_RBUFF_SIZE, &info);
       
        GPS_TransferEnd = 0;
        new_parse =1;
      }
      
      if(new_parse )                //���µĽ�����Ϣ   
      {    
        /* �Խ�����ʱ�����ת����ת���ɱ���ʱ�� */
        GMTconvert(&info.utc,&beiJingTime,8,1);
        
        /* �������õ�����Ϣ */
				UsartPrintf(USART_DEBUG,"/************************�������õ�����Ϣ************************/");
        UsartPrintf(USART_DEBUG,"\r\nʱ��%d,%d,%d,%d,%d,%d", beiJingTime.year+1900, beiJingTime.mon+1,beiJingTime.day,beiJingTime.hour,beiJingTime.min,beiJingTime.sec);
        UsartPrintf(USART_DEBUG,"\r\nγ�ȣ�%f,����%f",info.lat,info.lon);
        UsartPrintf(USART_DEBUG,"\r\n����ʹ�õ����ǣ�%d,�ɼ����ǣ�%d",info.satinfo.inuse,info.satinfo.inview);
        UsartPrintf(USART_DEBUG,"\r\n���θ߶ȣ�%f �� ", info.elv);
        UsartPrintf(USART_DEBUG,"\r\n�ٶȣ�%f km/h ", info.speed);
        UsartPrintf(USART_DEBUG,"\r\n����%f ��\r\n", info.direction);
        
        new_parse = 0;
      }
	

    /* �ͷ�GPS���ݽṹ */
     nmea_parser_destroy(&parser);

    
    //  return 0;
}

#endif






/**************************************************end of file****************************************/

