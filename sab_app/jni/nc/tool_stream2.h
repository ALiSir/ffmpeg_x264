//
//  tool_stream2.h
//  SocketThread
//
//  Created by lanbh on 14-3-6.
//  Copyright (c) 2014年 com.vision. All rights reserved.
//

#if defined __cplusplus
extern "C"
{
#endif

#ifndef __TOOL_STREAM2_H__
#define __TOOL_STREAM2_H__

#include "tool_type.h"

#define TOOL_STREAM2_POOL_SIZE       100
#define TOOL_STREAM2_DATA_SIZE       3000

typedef struct
{
	TOOL_UINT8 data[TOOL_STREAM2_DATA_SIZE];
	TOOL_UINT32 len;
}tool_stream2_data_t;

typedef struct
{
	tool_stream2_data_t pool[TOOL_STREAM2_POOL_SIZE];
	TOOL_UINT32 write_pos;
	TOOL_MUTEX mutex;
}tool_stream2_t;

TOOL_VOID tool_stream2_init(tool_stream2_t* stream);
TOOL_VOID tool_stream2_initReadPos(tool_stream2_t* stream, TOOL_UINT32* read_pos);
TOOL_VOID tool_stream2_set(tool_stream2_t* stream, TOOL_VOID* data, TOOL_UINT32 len);
TOOL_INT32 tool_stream2_get(tool_stream2_t* stream, TOOL_UINT32* read_pos, TOOL_VOID* data, TOOL_UINT32* len);
TOOL_VOID tool_stream2_done(tool_stream2_t* stream);


#endif


#if defined __cplusplus
}
#endif

