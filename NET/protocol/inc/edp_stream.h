#ifndef _EDP_STREAM_H_
#define _EDP_STREAM_H_


#include "dStream.h"




short EDP_LoadDataStream(unsigned char type, char *devid, char *apikey, DATA_STREAM *streamArray, unsigned short streamArrayCnt, char *sendBuf, short maxLen);

short EDP_LoadDataStream_Measure(unsigned char type, char *devid, char *apikey, DATA_STREAM *streamArray, unsigned short streamArrayCnt);

#endif
