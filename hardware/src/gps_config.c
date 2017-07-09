/**
  ******************************************************************************
  * @file    gps_config.c
  * @author  fire
  * @version V1.0
  * @date    2014-08-xx
  * @brief   gpsģ��ӿ���������
  ******************************************************************************
  * @attention
  *
  * ʵ��ƽ̨:Ұ�� ISO-STM32 ������
  * ��̳    :http://www.chuxue123.com
  * �Ա�    :http://firestm32.taobao.com
  *
  ******************************************************************************
	*/

#include "gps_config.h"
#include "usart.h"
#include "nmea/nmea.h"


/* DMA���ջ���  */
uint8_t gps_rbuff[GPS_RBUFF_SIZE];

/* DMA���������־ */
__IO uint8_t GPS_TransferEnd = 0, GPS_HalfTransferEnd = 0;

		double  lat;        /**< Latitude in NDEG - +/-[degree][min].[sec/60] */
    double  lon;        /**< Longitude in NDEG - +/-[degree][min].[sec/60] */


		nmeaINFO GPS_info;          //GPS�����õ�����Ϣ 
		nmeaPARSER parser;      //����ʱʹ�õ����ݽṹ  
    uint8_t new_parse=0;    //�Ƿ����µĽ������ݱ�־
  
    nmeaTIME beiJingTime;    //����ʱ�� 



/**
  * @brief  GPS_Interrupt_Config ����GPSʹ�õ�DMA�ж� 
  * @param  None.
  * @retval None.
  */
static void GPS_Interrupt_Config(void)
{
	NVIC_InitTypeDef NVIC_InitStructure;

//  NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);

	// DMA2 Channel Interrupt ENABLE
	NVIC_InitStructure.NVIC_IRQChannel = GPS_DMA_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);

}


/**
  * @brief  GPS_ProcessDMAIRQ GPS DMA�жϷ�����
  * @param  None.
  * @retval None.
  */
void GPS_ProcessDMAIRQ(void)
{
  
  if(DMA_GetITStatus(GPS_DMA_IT_HT) )         /* DMA �봫����� */
  {
    GPS_HalfTransferEnd = 1;                //���ð봫����ɱ�־λ
    DMA_ClearFlag(GPS_DMA_FLAG_HT);
  }
  else if(DMA_GetITStatus(GPS_DMA_IT_TC))     /* DMA ������� */
  {
    GPS_TransferEnd = 1;                    //���ô�����ɱ�־λ
    DMA_ClearFlag(GPS_DMA_FLAG_TC);

   }
}


/**
  * @brief  GPS_DMA_Config gps dma��������
  * @param  ��
  * @retval ��
  */
static void GPS_DMA_Config(void)
{
		DMA_InitTypeDef DMA_InitStructure;
	
		/*����DMAʱ��*/
		RCC_AHBPeriphClockCmd(GPS_DMA_CLK, ENABLE);

		/*����DMAԴ���������ݼĴ�����ַ*/
		DMA_InitStructure.DMA_PeripheralBaseAddr = GPS_DATA_ADDR;	   

		/*�ڴ��ַ(Ҫ����ı�����ָ��)*/
		DMA_InitStructure.DMA_MemoryBaseAddr = (u32)gps_rbuff;

		/*���򣺴��ڴ浽����*/		
		DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;	

		/*�����СDMA_BufferSize=SENDBUFF_SIZE*/	
		DMA_InitStructure.DMA_BufferSize = GPS_RBUFF_SIZE;

		/*�����ַ����*/	    
		DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable; 

		/*�ڴ��ַ����*/
		DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;	

		/*�������ݵ�λ*/	
		DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;

		/*�ڴ����ݵ�λ 8bit*/
		DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;	 

		/*DMAģʽ������ѭ��*/
		DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;	 

		/*���ȼ�����*/	
		DMA_InitStructure.DMA_Priority = DMA_Priority_Medium;  

		/*��ֹ�ڴ浽�ڴ�Ĵ���	*/
		DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;

		/*����DMA��ͨ��*/		   
		DMA_Init(GPS_DMA_CHANNEL, &DMA_InitStructure); 	   
    
    GPS_Interrupt_Config();
		
    DMA_ITConfig(GPS_DMA_CHANNEL,DMA_IT_HT|DMA_IT_TC,ENABLE);  //����DMA������ɺ�����ж�

		/*ʹ��DMA*/
		DMA_Cmd (GPS_DMA_CHANNEL,ENABLE);		
    
    /* ���ô��� �� DMA����TX���� */
		USART_DMACmd(GPS_USART, USART_DMAReq_Rx, ENABLE);

    
}

/**
  * @brief  GPS_Config gps ��ʼ��
  * @param  ��
  * @retval ��
  */
void GPS_Config(unsigned int baud)
{
  GPS_USART_INIT(baud);
  GPS_DMA_Config();  

    /* �����������������Ϣ�ĺ��� */
    nmea_property()->trace_func = &trace;
    nmea_property()->error_func = &error;

    /* ��ʼ��GPS���ݽṹ */
    nmea_zero_INFO(&GPS_info);
    nmea_parser_init(&parser);
  
}

 

/**
  * @brief  trace �ڽ���ʱ��������GPS���
  * @param  str: Ҫ������ַ�����str_size:���ݳ���
  * @retval ��
  */
void trace(const char *str, int str_size)
{
  #ifdef __GPS_DEBUG    //��gps_config.h�ļ���������꣬�Ƿ����������Ϣ
    uint16_t i;
    printf("\r\nTrace: ");
    for(i=0;i<str_size;i++)
      printf("%c",*(str+i));
  
    printf("\n");
  #endif
}

/**
  * @brief  error �ڽ������ʱ�����ʾ��Ϣ
  * @param  str: Ҫ������ַ�����str_size:���ݳ���
  * @retval ��
  */
void error(const char *str, int str_size)
{
    #ifdef __GPS_DEBUG   //��gps_config.h�ļ���������꣬�Ƿ����������Ϣ

    uint16_t i;
    printf("\r\nError: ");
    for(i=0;i<str_size;i++)
      printf("%c",*(str+i));
    printf("\n");
    #endif
}



/******************************************************************************************************** 
**     ��������:            bit        IsLeapYear(uint8_t    iYear) 
**    ��������:            �ж�����(�������2000�Ժ�����) 
**    ��ڲ�����            iYear    ��λ���� 
**    ���ڲ���:            uint8_t        1:Ϊ����    0:Ϊƽ�� 
********************************************************************************************************/ 
static uint8_t IsLeapYear(uint8_t iYear) 
{ 
    uint16_t    Year; 
    Year    =    2000+iYear; 
    if((Year&3)==0) 
    { 
        return ((Year%400==0) || (Year%100!=0)); 
    } 
     return 0; 
} 

/******************************************************************************************************** 
**     ��������:            void    GMTconvert(uint8_t *DT,uint8_t GMT,uint8_t AREA) 
**    ��������:            ��������ʱ�任�������ʱ��ʱ�� 
**    ��ڲ�����            *DT:    ��ʾ����ʱ������� ��ʽ YY,MM,DD,HH,MM,SS 
**                        GMT:    ʱ���� 
**                        AREA:    1(+)���� W0(-)���� 
********************************************************************************************************/ 
void    GMTconvert(nmeaTIME *SourceTime, nmeaTIME *ConvertTime, uint8_t GMT,uint8_t AREA) 
{ 
    uint32_t    YY,MM,DD,hh,mm,ss;        //������ʱ�����ݴ���� 
     
    if(GMT==0)    return;                //�������0ʱ��ֱ�ӷ��� 
    if(GMT>12)    return;                //ʱ�����Ϊ12 �����򷵻�         

    YY    =    SourceTime->year;                //��ȡ�� 
    MM    =    SourceTime->mon;                 //��ȡ�� 
    DD    =    SourceTime->day;                 //��ȡ�� 
    hh    =    SourceTime->hour;                //��ȡʱ 
    mm    =    SourceTime->min;                 //��ȡ�� 
    ss    =    SourceTime->sec;                 //��ȡ�� 

    if(AREA)                        //��(+)ʱ������ 
    { 
        if(hh+GMT<24)    hh    +=    GMT;//������������ʱ�䴦��ͬһ�������Сʱ���� 
        else                        //����Ѿ����ڸ�������ʱ��1����������ڴ��� 
        { 
            hh    =    hh+GMT-24;        //�ȵó�ʱ�� 
            if(MM==1 || MM==3 || MM==5 || MM==7 || MM==8 || MM==10)    //���·�(12�µ�������) 
            { 
                if(DD<31)    DD++; 
                else 
                { 
                    DD    =    1; 
                    MM    ++; 
                } 
            } 
            else if(MM==4 || MM==6 || MM==9 || MM==11)                //С�·�2�µ�������) 
            { 
                if(DD<30)    DD++; 
                else 
                { 
                    DD    =    1; 
                    MM    ++; 
                } 
            } 
            else if(MM==2)    //����2�·� 
            { 
                if((DD==29) || (DD==28 && IsLeapYear(YY)==0))        //��������������2��29�� ���߲�����������2��28�� 
                { 
                    DD    =    1; 
                    MM    ++; 
                } 
                else    DD++; 
            } 
            else if(MM==12)    //����12�·� 
            { 
                if(DD<31)    DD++; 
                else        //�������һ�� 
                {               
                    DD    =    1; 
                    MM    =    1; 
                    YY    ++; 
                } 
            } 
        } 
    } 
    else 
    {     
        if(hh>=GMT)    hh    -=    GMT;    //������������ʱ�䴦��ͬһ�������Сʱ���� 
        else                        //����Ѿ����ڸ�������ʱ��1����������ڴ��� 
        { 
            hh    =    hh+24-GMT;        //�ȵó�ʱ�� 
            if(MM==2 || MM==4 || MM==6 || MM==8 || MM==9 || MM==11)    //�����Ǵ��·�(1�µ�������) 
            { 
                if(DD>1)    DD--; 
                else 
                { 
                    DD    =    31; 
                    MM    --; 
                } 
            } 
            else if(MM==5 || MM==7 || MM==10 || MM==12)                //������С�·�2�µ�������) 
            { 
                if(DD>1)    DD--; 
                else 
                { 
                    DD    =    30; 
                    MM    --; 
                } 
            } 
            else if(MM==3)    //�����ϸ�����2�·� 
            { 
                if((DD==1) && IsLeapYear(YY)==0)                    //�������� 
                { 
                    DD    =    28; 
                    MM    --; 
                } 
                else    DD--; 
            } 
            else if(MM==1)    //����1�·� 
            { 
                if(DD>1)    DD--; 
                else        //�����һ�� 
                {               
                    DD    =    31; 
                    MM    =    12; 
                    YY    --; 
                } 
            } 
        } 
    }         

    ConvertTime->year   =    YY;                //������ 
    ConvertTime->mon    =    MM;                //������ 
    ConvertTime->day    =    DD;                //������ 
    ConvertTime->hour   =    hh;                //����ʱ 
    ConvertTime->min    =    mm;                //���·� 
    ConvertTime->sec    =    ss;                //������ 
}  


/**
  * @brief  nmea_decode_test ����GPSģ����Ϣ
  * @param  ��
  * @retval ��
  */
void nmea_decode_test(void)
{

////    nmeaINFO GPS_info;          //GPS�����õ�����Ϣ 
////    nmeaPARSER parser;      //����ʱʹ�õ����ݽṹ  
////    uint8_t new_parse=0;    //�Ƿ����µĽ������ݱ�־
////  
////    nmeaTIME beiJingTime;    //����ʱ�� 

////    /* �����������������Ϣ�ĺ��� */
////    nmea_property()->trace_func = &trace;
////    nmea_property()->error_func = &error;

////    /* ��ʼ��GPS���ݽṹ */
////    nmea_zero_INFO(&GPS_info);
////    nmea_parser_init(&parser);

      if(GPS_HalfTransferEnd)     /* ���յ�GPS_RBUFF_SIZEһ������� */
      {
        /* ����nmea��ʽ���� */
        nmea_parse(&parser, (const char*)&gps_rbuff[0], HALF_GPS_RBUFF_SIZE, &GPS_info);
        
        GPS_HalfTransferEnd = 0;   //��ձ�־λ
        new_parse = 1;             //���ý�����Ϣ��־ 
      }
      else if(GPS_TransferEnd)    /* ���յ���һ������ */
      {

        nmea_parse(&parser, (const char*)&gps_rbuff[HALF_GPS_RBUFF_SIZE], HALF_GPS_RBUFF_SIZE, &GPS_info);
       
        GPS_TransferEnd = 0;
        new_parse =1;
      }
      
      if(new_parse )                //���µĽ�����Ϣ   
      {    
        /* �Խ�����ʱ�����ת����ת���ɱ���ʱ�� */
        GMTconvert(&GPS_info.utc,&beiJingTime,8,1);
        
//        /* �������õ�����Ϣ */
//				UsartPrintf(USART_DEBUG,"/************************�������õ�����Ϣ************************/");
//        UsartPrintf(USART_DEBUG,"\r\nʱ��%d,%d,%d,%d,%d,%d", beiJingTime.year+1900, beiJingTime.mon+1,beiJingTime.day,beiJingTime.hour,beiJingTime.min,beiJingTime.sec);
//        UsartPrintf(USART_DEBUG,"\r\nγ�ȣ�%f,����%f",GPS_info.lat,GPS_info.lon);
//        UsartPrintf(USART_DEBUG,"\r\n����ʹ�õ����ǣ�%d,�ɼ����ǣ�%d",GPS_info.satinfo.inuse,GPS_info.satinfo.inview);
//        UsartPrintf(USART_DEBUG,"\r\n���θ߶ȣ�%f �� ", GPS_info.elv);
//        UsartPrintf(USART_DEBUG,"\r\n�ٶȣ�%f km/h ", GPS_info.speed);
//        UsartPrintf(USART_DEBUG,"\r\n����%f ��\r\n", GPS_info.direction);
				
				//GPS����ת��
				lat = (int)GPS_info.lat/100;
				lon = (int)GPS_info.lon/100;
        lat = lat + (GPS_info.lat - lat*100)/60;
				lon = lon + (GPS_info.lon - lon*100)/60;
        new_parse = 0;
      }
	

    /* �ͷ�GPS���ݽṹ */
//     nmea_parser_destroy(&parser);

    
    //  return 0;
}





/*********************************************************end of file**************************************************/
