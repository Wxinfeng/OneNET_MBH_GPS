#ifndef _ONENET_H_
#define _ONENET_H_


#include "http_stream.h"




typedef struct
{

    char devID[15];
    char apiKey[35];
	
	char ip[16];
	char port[8];
	
	char *buffer;
	unsigned short bufferSize;	//
	
	unsigned char netWork : 1; 	//1-OneNET接入成功		0-OneNET接入失败
	unsigned char sendData : 1;
	unsigned char errCount : 3; //错误计数
	unsigned char reverse : 3;

} ONETNET_INFO;

extern ONETNET_INFO oneNetInfo;





#define CHECK_CONNECTED			0	//已连接
#define CHECK_CLOSED			1	//已断开
#define CHECK_GOT_IP			2	//已获取到IP
#define CHECK_NO_DEVICE			3	//无设备
#define CHECK_INITIAL			4	//初始化状态
#define CHECK_NO_CARD			5	//没有sim卡
#define CHECK_NO_ERR			255 //

#define DEVICE_CMD_MODE			0 //AT命令模式
#define DEVICE_DATA_MODE		1 //平台命令下发模式
#define DEVICE_HEART_MODE		2 //心跳连接模式




_Bool OneNet_SendData(FORMAT_TYPE type, char *devid, char *apikey, DATA_STREAM *streamArray, unsigned short streamArrayCnt);

void OneNet_Status(void);

_Bool OneNet_SendGPS(FORMAT_TYPE type, char *devid, char *apikey, DATA_STREAM *streamArray, unsigned short streamArrayCnt);


#endif
