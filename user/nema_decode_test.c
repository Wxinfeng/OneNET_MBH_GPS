/**
  ******************************************************************************
  * @file    nmea_decode_test.c
  * @author  fire
  * @version V1.0
  * @date    2013-xx-xx
  * @brief   测试NEMA解码库
  ******************************************************************************
  * @attention
  *
  * 实验平台:野火 iSO STM32 开发板 
  * 论坛    :http://www.chuxue123.com
  * 淘宝    :http://firestm32.taobao.com
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

nmeaINFO info;          //GPS解码后得到的信息 

#ifdef __GPS_LOG_FILE             //对SD卡上的gpslog.txt文件进行解码；（需要在sd卡上存放gpslog.txt文件）




#else       //对GPS模块传回的信息进行解码

/**
  * @brief  nmea_decode_test 解码GPS模块信息
  * @param  无
  * @retval 无
  */
void nmea_decode_test(void)
{

    
    nmeaPARSER parser;      //解码时使用的数据结构  
    uint8_t new_parse=0;    //是否有新的解码数据标志
  
    nmeaTIME beiJingTime;    //北京时间 

    /* 设置用于输出调试信息的函数 */
    nmea_property()->trace_func = &trace;
    nmea_property()->error_func = &error;

    /* 初始化GPS数据结构 */
    nmea_zero_INFO(&info);
    nmea_parser_init(&parser);

      if(GPS_HalfTransferEnd)     /* 接收到GPS_RBUFF_SIZE一半的数据 */
      {
        /* 进行nmea格式解码 */
        nmea_parse(&parser, (const char*)&gps_rbuff[0], HALF_GPS_RBUFF_SIZE, &info);
        
        GPS_HalfTransferEnd = 0;   //清空标志位
        new_parse = 1;             //设置解码消息标志 
      }
      else if(GPS_TransferEnd)    /* 接收到另一半数据 */
      {

        nmea_parse(&parser, (const char*)&gps_rbuff[HALF_GPS_RBUFF_SIZE], HALF_GPS_RBUFF_SIZE, &info);
       
        GPS_TransferEnd = 0;
        new_parse =1;
      }
      
      if(new_parse )                //有新的解码消息   
      {    
        /* 对解码后的时间进行转换，转换成北京时间 */
        GMTconvert(&info.utc,&beiJingTime,8,1);
        
        /* 输出解码得到的信息 */
				UsartPrintf(USART_DEBUG,"/************************输出解码得到的信息************************/");
        UsartPrintf(USART_DEBUG,"\r\n时间%d,%d,%d,%d,%d,%d", beiJingTime.year+1900, beiJingTime.mon+1,beiJingTime.day,beiJingTime.hour,beiJingTime.min,beiJingTime.sec);
        UsartPrintf(USART_DEBUG,"\r\n纬度：%f,经度%f",info.lat,info.lon);
        UsartPrintf(USART_DEBUG,"\r\n正在使用的卫星：%d,可见卫星：%d",info.satinfo.inuse,info.satinfo.inview);
        UsartPrintf(USART_DEBUG,"\r\n海拔高度：%f 米 ", info.elv);
        UsartPrintf(USART_DEBUG,"\r\n速度：%f km/h ", info.speed);
        UsartPrintf(USART_DEBUG,"\r\n航向：%f 度\r\n", info.direction);
        
        new_parse = 0;
      }
	

    /* 释放GPS数据结构 */
     nmea_parser_destroy(&parser);

    
    //  return 0;
}

#endif






/**************************************************end of file****************************************/

