#ifndef __GPS_CONFIG_H
#define	__GPS_CONFIG_H

#include "stm32f10x.h"
#include "nmea/nmea.h"


/* 配置 */

//#define __GPS_DEBUG     //定义这个宏，在nmea解码时会输出调试信息

//#define __GPS_LOG_FILE    //定义这个宏，对SD卡上的gpslog.txt文件进行解码；不定义的话使用串口2接收GPS信息解码


/* GPS接口配置 使用不同的串口时，要修改对应的接口 */

#define USART3_DR_Base            (0x40004800+0x04)		  // 串口3的数据寄存器地址


#define GPS_DATA_ADDR             USART3_DR_Base        //GPS使用的串口的数据寄存器地址
#define GPS_RBUFF_SIZE            512                   //串口接收缓冲区大小
#define HALF_GPS_RBUFF_SIZE       (GPS_RBUFF_SIZE/2)    //串口接收缓冲区一半  

/* 外设 */
#define GPS_USART                    USART3
#define GPS_DMA                      DMA1
#define GPS_DMA_CLK                  RCC_AHBPeriph_DMA1
#define GPS_DMA_CHANNEL              DMA1_Channel3
#define GPS_DMA_IRQn                 DMA1_Channel3_IRQn         //GPS中断源

/* 外设标志 */
#define GPS_DMA_FLAG_TC              DMA1_FLAG_TC3
#define GPS_DMA_FLAG_TE              DMA1_FLAG_TE3
#define GPS_DMA_FLAG_HT              DMA1_FLAG_HT3
#define GPS_DMA_FLAG_GL              DMA1_FLAG_GL3
#define GPS_DMA_IT_HT                DMA1_IT_HT3
#define GPS_DMA_IT_TC                DMA1_IT_TC3


/* 函数 */
#define GPS_USART_INIT               Usart3_Init             //GPS使用的串口初始化函数，波特率9600 
#define GPS_DMA_IRQHANDLER           DMA1_Channel3_IRQHandler   //GPS使用的DMA中断服务函数


extern uint8_t gps_rbuff[GPS_RBUFF_SIZE];
extern __IO uint8_t GPS_TransferEnd ;
extern __IO uint8_t GPS_HalfTransferEnd;

extern double  lat;        /**< Latitude in NDEG - +/-[degree][min].[sec/60] */
extern double  lon;        /**< Longitude in NDEG - +/-[degree][min].[sec/60] */
extern nmeaINFO GPS_info;


void GPS_ProcessDMAIRQ(void);
void GPS_Config(unsigned int baud);

void trace(const char *str, int str_size);
void error(const char *str, int str_size);
void GMTconvert(nmeaTIME *SourceTime, nmeaTIME *ConvertTime, uint8_t GMT,uint8_t AREA) ;

void nmea_decode_test(void);




#endif 
