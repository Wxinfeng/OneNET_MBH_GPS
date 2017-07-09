#ifndef _HTTP_STREAM_H_
#define _HTTP_STREAM_H_


#include "dStream.h"




short HTTP_LoadDataStream(unsigned char type, char *devid, char *apikey, DATA_STREAM *streamArray, unsigned short streamArrayCnt, char *sendBuf, short maxLen);

unsigned short HTTP_LoadDataStream_HardWareHTTP(unsigned char type, char *HTTP_Url, char *HTTP_UserData, char *HTTP_Body, char *devid, char *apikey);

short HTTP_LoadDataStream_Measure(unsigned char type, char *devid, char *apikey, DATA_STREAM *streamArray, unsigned short streamArrayCnt);

#endif
