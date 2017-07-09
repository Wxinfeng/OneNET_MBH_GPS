/**
	************************************************************
	************************************************************
	************************************************************
	*	文件名： 	main.c
	*
	*	作者： 		张继瑞
	*
	*	日期： 		2017-01-011
	*
	*	版本： 		V1.0
	*
	*	说明： 		接入onenet，上传数据和命令控制
	*
	*	修改记录：	
	************************************************************
	************************************************************
	************************************************************
**/

//单片机头文件
#include "stm32f10x.h"

//网络协议层
#include "onenet.h"
#include "fault.h"

//应用
#include "onenetAPP.h"

//网络设备
#include "net_device.h"

//硬件驱动
#include "led.h"
#include "delay.h"
#include "key.h"
#include "lcd1602.h"
#include "usart.h"
#include "hwtimer.h"
#include "i2c.h"
//#include "gy30.h"
//#include "adxl345.h"
//#include "sht20.h"
//#include "iwdg.h"
#include "at24c02.h"
#include "selfcheck.h"
#include "beep.h"
//#include "oled.h"
#include "info.h"
//#include "tcrt5000.h"
//#include "spilcd.h"

#include "gps_config.h"

//中文数据流
#include "dataStreamName.h"

//字库
#include "font.h"

//C库
#include <string.h>
#include <stdlib.h>





//数据流
DATA_STREAM dataStream[] = {
//								{ZW_REDLED, &ledStatus.Led4Sta, TYPE_BOOL, 1},
//								{ZW_GREENLED, &ledStatus.Led5Sta, TYPE_BOOL, 1},
//								{ZW_YELLOWLED, &ledStatus.Led6Sta, TYPE_BOOL, 1},
//								{ZW_BLUELED, &ledStatus.Led7Sta, TYPE_BOOL, 1},
//								{ZW_BEEP, &beepInfo.Beep_Status, TYPE_BOOL, 1},
//								{ZW_TEMPERATURE, &sht20Info.tempreture, TYPE_FLOAT, 1},
//								{ZW_HUMIDITY, &sht20Info.humidity, TYPE_FLOAT, 1},
//								{ZW_X, &adxlInfo.incidence_Xf, TYPE_FLOAT, 1},
//								{ZW_Y, &adxlInfo.incidence_Yf, TYPE_FLOAT, 1},
//								{ZW_Z, &adxlInfo.incidence_Zf, TYPE_FLOAT, 1},
//								{ZW_GPS_LAT, &GPS_info.lat, TYPE_DOUBLE, 1},
//								{ZW_GPS_LON, &GPS_info.lon, TYPE_DOUBLE, 1},
								{ZW_ERRTYPE, &faultTypeReport, TYPE_UCHAR, 1},
							};
unsigned char dataStreamCnt = sizeof(dataStream) / sizeof(dataStream[0]);

_Bool sendFlag = 0;

/*
************************************************************
*	函数名称：	Hardware_Init
*
*	函数功能：	硬件初始化
*
*	入口参数：	无
*
*	返回参数：	无
*
*	说明：		初始化单片机功能以及外接设备
************************************************************
*/
void Hardware_Init(void)
{
	
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);								//中断控制器分组设置

	Delay_Init();																//systick初始化
	
	Led_Init();																	//LED初始化
	
	Key_Init();																	//按键初始化
	
	Beep_Init();																//蜂鸣器初始化
	
	IIC_Init();																	//软件IIC总线初始化
	
	Lcd1602_Init();																//LCD1602初始化
	
//	OLED_Init();
	
	Usart1_Init(115200); 														//初始化串口   115200bps
	
//	GPS_Config(9600);                              //GPS初始化
	
	Lcd1602_DisString(0x80, "Check Power On");									//提示进行开机检测
	Check_PowerOn(); 															//上电自检
	Lcd1602_Clear(0x80);														//清第一行显示
	
//	if(checkInfo.ADXL345_OK == DEV_OK) 											//如果检测到ADXL345则初始化
//		;
////		ADXL345_Init();
//	
	if(checkInfo.OLED_OK == DEV_OK)												//如果检测到OLED则初始化
	{
//		OLED_Init();
//		OLED_ClearScreen();														//清屏
	}

	if(RCC_GetFlagStatus(RCC_FLAG_IWDGRST) == SET) 								//如果是看门狗复位则提示
	{
		UsartPrintf(USART_DEBUG, "WARN:	IWDG Reboot\r\n");
		
		RCC_ClearFlag();														//清除看门狗复位标志位
		
		faultTypeReport = faultType = FAULT_REBOOT; 							//标记为重启错误
		
		netDeviceInfo.reboot = 1;
	}
	else
	{
		UsartPrintf(USART_DEBUG, "2.DEVID: %s,     APIKEY: %s\r\n"
								, oneNetInfo.devID, oneNetInfo.apiKey);
		
		netDeviceInfo.reboot = 0;
	}
	
	//Iwdg_Init(4, 1250); 														//64分频，每秒625次，重载1250次，2s
	
	Lcd1602_Clear(0xff);														//清屏
	
	Timer6_7_Init(TIM6, 49, 35999);												//72MHz，36000分频-500us，50重载值。则中断周期为500us * 50 = 25ms
	Timer6_7_Init(TIM7, 1999, 35999);											//72MHz，36000分频-500us，2000重载值。则中断周期为500us * 2000 = 1s
																				//定时检查网络状态标志位
	
	UsartPrintf(USART_DEBUG, "3.Hardware init OK\r\n");							//提示初始化完成

}

/*
************************************************************
*	函数名称：	main
*
*	函数功能：	
*
*	入口参数：	无
*
*	返回参数：	0
*
*	说明：		
************************************************************
*/
int main(void)
{
	
	unsigned char *dataPtr;
	unsigned int runTime = 0;

	Hardware_Init();									//硬件初始化
	
//	SPILCD_Clear(BGC);
	
	//标题显示
//	OLED_DisChar16x16(0, 0, san);						//显示“三”
//	OLED_DisChar16x16(0, 16, zhou);						//显示“轴”
//	OLED_DisString6x8(1, 32, ":");						//显示“：”
//	
//	OLED_DisChar16x16(2, 0, wen);						//显示“温”
//	OLED_DisChar16x16(2, 16, shi);						//显示“湿”
//	OLED_DisChar16x16(2, 32, du);						//显示“度”
//	OLED_DisString6x8(3, 48, ":");						//显示“：”
//	
//	OLED_DisChar16x16(6, 0, zhuang);					//显示“状”
//	OLED_DisChar16x16(6, 16, tai);						//显示“态”
//	OLED_DisString6x8(7, 32, ":");						//显示“：”
	
	//标题显示
//	SPILCD_DisZW(0, 0, RED, san);								//显示“三”
//	SPILCD_DisZW(16, 0, RED, zhou);								//显示“轴”
//	
//	SPILCD_DisZW(0, 32, RED, wen);								//显示“温”
//	SPILCD_DisZW(16, 32, RED, shi);								//显示“湿”
//	SPILCD_DisZW(32, 32, RED, du);								//显示“度”
//	
//	SPILCD_DisZW(0, 64, RED, zhuang);							//显示“状”
//	SPILCD_DisZW(16, 64, RED, tai);								//显示“态”
	
	oneNetInfo.bufferSize = HTTP_LoadDataStream_Measure(FORMAT_TYPE3, oneNetInfo.devID, oneNetInfo.apiKey, dataStream, dataStreamCnt);
	oneNetInfo.buffer = (char *)malloc(oneNetInfo.bufferSize);
	
	NET_DEVICE_IO_Init();								//网络设备IO初始化
	NET_DEVICE_Reset();									//网络设备复位
	NET_DEVICE_Set_DataMode(DEVICE_CMD_MODE);			//设置为命令收发模式(例如ESP8266要区分AT的返回还是平台下发数据的返回)
	
	GPS_Config(9600);
	
	
	while(1)
	{
		
		if(oneNetInfo.netWork == 1)
		{
/******************************************************************************
			按键扫描
******************************************************************************/
			switch(Keyboard())
			{
				case KEY0DOWN:
					
					if(ledStatus.Led4Sta == LED_OFF)
						Led4_Set(LED_ON);
					else
						Led4_Set(LED_OFF);
					
					oneNetInfo.sendData = 1;										//标记数据发送
					
				break;
				
				case KEY2DOWN:
					
					if(ledStatus.Led5Sta == LED_OFF)
						Led5_Set(LED_ON);
					else
						Led5_Set(LED_OFF);
					
					oneNetInfo.sendData = 1;
					
				break;
				
				case KEY3DOWN:
					
					if(ledStatus.Led6Sta == LED_OFF)
						Led6_Set(LED_ON);
					else
						Led6_Set(LED_OFF);
					
					oneNetInfo.sendData = 1;
					
				break;
				
				case KEY1DOWN:
					
					if(ledStatus.Led7Sta == LED_OFF)
						Led7_Set(LED_ON);
					else
						Led7_Set(LED_OFF);
					
					oneNetInfo.sendData = 1;
					
				break;
				
				default:
				break;
			}
			
/******************************************************************************
			数据与心跳
******************************************************************************/
			if(timInfo.timer6Out - runTime >= 300)									//25s一次(25ms中断)
			{
				runTime = timInfo.timer6Out;
				
				if(sendFlag)
				{
					TIM_Cmd(OS_TIMER, DISABLE);
					OneNet_Status();												//心跳连接
					TIM_Cmd(OS_TIMER, ENABLE);
				}
				else
				{
					TIM_Cmd(OS_TIMER, DISABLE);
//					OneNet_SendData(FORMAT_TYPE3, oneNetInfo.devID, oneNetInfo.apiKey, dataStream, dataStreamCnt);		//数据发送
					UsartPrintf(USART_DEBUG,"\r\n正在使用卫星数%d,可见卫星数%d",GPS_info.satinfo.inuse,GPS_info.satinfo.inview);
					OneNet_SendGPS(FORMAT_TYPE3, oneNetInfo.devID, oneNetInfo.apiKey, dataStream, dataStreamCnt);
					TIM_Cmd(OS_TIMER, ENABLE);
				}
		
				UsartPrintf(USART_DEBUG, "sendFlag :%d\r\n",sendFlag);
				sendFlag = !sendFlag;
				
				
			}
			
/******************************************************************************
			平台下发命令解析
******************************************************************************/
			if(oneNetInfo.netWork && NET_DEVICE_Get_DataMode() == DEVICE_DATA_MODE)	//当有网络 且 在命令接收模式时
			{
				dataPtr = NET_DEVICE_GetIPD(0);										//不等待，获取平台下发的数据
				if(dataPtr != NULL)													//如果数据指针不为空，则代表收到了数据
				{
					OneNet_App(dataPtr);											//集中处理
				}
			}
			
/******************************************************************************
			传感器
******************************************************************************/
			if(checkInfo.ADXL345_OK == DEV_OK) 										//只有设备存在时，才会读取值和显示
			{
//				ADXL345_GetValue();													//采集传感器数据
//				Lcd1602_DisString(0x80, "X%0.1f,Y%0.1f,Z%0.1f", adxlInfo.incidence_Xf, adxlInfo.incidence_Yf, adxlInfo.incidence_Zf);
//				OLED_DisString6x8(1, 40, "X%0.1f,Y%0.1f,Z%0.1f", adxlInfo.incidence_Xf, adxlInfo.incidence_Yf, adxlInfo.incidence_Zf);
//				SPILCD_DisString(0, 16, 16, BLUE, 1, "X%0.1f,Y%0.1f,Z%0.1f", adxlInfo.incidence_Xf, adxlInfo.incidence_Yf, adxlInfo.incidence_Zf);
			}
			if(checkInfo.SHT20_OK == DEV_OK) 										//只有设备存在时，才会读取值和显示
			{
//				SHT20_GetValue();													//采集传感器数据
//				Lcd1602_DisString(0xC0, "%0.1fC,%0.1f%%", sht20Info.tempreture, sht20Info.humidity);
//				OLED_DisString6x8(3, 56, "%0.1fC,%0.1f%%", sht20Info.tempreture, sht20Info.humidity);
//				SPILCD_DisString(0, 48, 16, BLUE, 1, "%0.1fC,%0.1f%%", sht20Info.tempreture, sht20Info.humidity);
			}
			
//			if(t5000Info.status == TCRT5000_ON)
//			{
////				TCRT5000_GetValue(5);
//				if(t5000Info.voltag < 3500)
//					Beep_Set(BEEP_ON);
//				else
//					Beep_Set(BEEP_OFF);
//			}

			nmea_decode_test();
			Lcd1602_DisString(0x80, "LAT: %f", lat);
			Lcd1602_DisString(0xC0, "LON: %f", lon);
			
/******************************************************************************
			错误处理
******************************************************************************/
			if(faultType != FAULT_NONE)												//如果错误标志被设置
			{
				if(faultType == FAULT_PRO || faultType == FAULT_NODEVICE)
				{
//					OLED_DisChar16x16(6, 64, duan);OLED_DisChar16x16(6, 80, kai);
//					SPILCD_DisZW(16, 80, BLUE, duan);SPILCD_DisZW(32, 80, BLUE, kai);
				}
				
				UsartPrintf(USART_DEBUG, "WARN:	Fault Process\r\n");
				Fault_Process();													//进入错误处理函数
			}
			
/******************************************************************************
			数据反馈
******************************************************************************/
			if(oneNetInfo.sendData)
			{
//				oneNetInfo.sendData = OneNet_SendData(FORMAT_TYPE3, oneNetInfo.devID, oneNetInfo.apiKey, dataStream, dataStreamCnt);
				oneNetInfo.sendData = OneNet_SendGPS(FORMAT_TYPE3, oneNetInfo.devID, oneNetInfo.apiKey, dataStream, dataStreamCnt);
			}
		}
		else
		{
/******************************************************************************
			初始化网络设备、接入平台
******************************************************************************/
			if(!oneNetInfo.netWork && (checkInfo.NET_DEVICE_OK == DEV_OK))			//当没有网络 且 网络模块检测到时
			{
//				OLED_DisChar16x16(6, 48, lian);OLED_DisChar16x16(6, 64, jie);OLED_DisChar16x16(6, 80, zhong);
//			
//				SPILCD_DisZW(0, 80, BLUE, lian);SPILCD_DisZW(16, 80, BLUE, jie);SPILCD_DisZW(32, 80, BLUE, zhong);
				
				NET_DEVICE_Set_DataMode(DEVICE_CMD_MODE);							//设置为命令收发模式
				
				NET_DEVICE_Set_DataMode(DEVICE_CMD_MODE);							//ÉèÖÃÎªÃüÁîÊÕ·¢Ä£Ê½
				
				if(!NET_DEVICE_Init(oneNetInfo.ip, oneNetInfo.port))				//³õÊ¼»¯ÍøÂçÉè±¸£¬ÄÜÁ¬ÈëÍøÂç
				{
					UsartPrintf(USART_DEBUG, "Tips:	NetWork OK\r\n");
		
					oneNetInfo.netWork = 1;
					
					NET_DEVICE_Set_DataMode(DEVICE_DATA_MODE);						//ÍøÂçÉè±¸Ö¸ÁîÄ£Ê½
					
					Beep_Set(BEEP_ON);
					DelayXms(200);
					Beep_Set(BEEP_OFF);
						
//					OLED_DisChar16x16(6, 48, yi);OLED_DisChar16x16(6, 64, lian);OLED_DisChar16x16(6, 80, jie);
//					
//					SPILCD_DisZW(0, 80, BLUE, yi);SPILCD_DisZW(16, 80, BLUE, lian);SPILCD_DisZW(32, 80, BLUE, jie);
					
					runTime = timInfo.timer6Out;
				}
			}
			
/******************************************************************************
			网络设备检测
******************************************************************************/
			if(checkInfo.NET_DEVICE_OK == DEV_ERR) 									//当网络设备未做检测
			{
				NET_DEVICE_Set_DataMode(DEVICE_CMD_MODE);							//设置为命令收发模式
				
				if(timerCount >= NET_TIME) 											//如果网络连接超时
				{
					UsartPrintf(USART_DEBUG, "Tips:		Timer Check Err\r\n");
					
					NET_DEVICE_Reset();												//复位网络设备
					timerCount = 0;													//清零连接超时计数
					faultType = FAULT_NONE;											//清除错误标志
				}
				
				if(!NET_DEVICE_Exist())												//网络设备检测
				{
					UsartPrintf(USART_DEBUG, "NET Device :Ok\r\n");
					checkInfo.NET_DEVICE_OK = DEV_OK;								//检测到网络设备，标记
					NET_DEVICE_Set_DataMode(DEVICE_DATA_MODE);						//设置为数据收发模式
				}
				else
					UsartPrintf(USART_DEBUG, "NET Device :Error\r\n");
			}
		}
	}
}
