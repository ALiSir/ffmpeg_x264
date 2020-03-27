//
//  tool_stream1.h
//  SocketThread
//
//  Created by lanbh on 14-3-6.
//  Copyright (c) 2014年 com.vision. All rights reserved.
//

#if defined __cplusplus
extern "C"
{
#endif

#ifndef __TOOL_STREAM1_H__
#define __TOOL_STREAM1_H__

#include "tool_type.h"

#define TOOL_STREAM1_POOL_SIZE       100

typedef struct
{
	TOOL_VOID* data;
	TOOL_INT32 len;
}tool_stream1_data_t;

typedef struct
{
	tool_stream1_data_t pool[TOOL_STREAM1_POOL_SIZE];
	TOOL_INT32 write_pos;
	TOOL_MUTEX mutex;
    
    TOOL_INT32 pool_size;
    TOOL_INT32 data_size;
}tool_stream1_t;

TOOL_VOID tool_stream1_init(tool_stream1_t* stream, TOOL_INT32 pool_size, TOOL_INT32 data_size);
TOOL_VOID tool_stream1_initReadPos(tool_stream1_t* stream, TOOL_INT32* read_pos);
TOOL_VOID tool_stream1_set(tool_stream1_t* stream, TOOL_VOID* data, TOOL_INT32 len);
TOOL_INT32 tool_stream1_get(tool_stream1_t* stream, TOOL_INT32* read_pos, TOOL_VOID* data, TOOL_INT32* len);
TOOL_VOID tool_stream1_done(tool_stream1_t* stream);

TOOL_VOID tool_stream1_push(tool_stream1_t* stream, TOOL_VOID* data, TOOL_INT32 len);
TOOL_INT32 tool_stream1_pop(tool_stream1_t* stream, TOOL_INT32* read_pos, TOOL_VOID** data, TOOL_INT32* len);

#endif


#if defined __cplusplus
}
#endif

