//
//  tool_stream2.c
//  SocketThread
//
//  Created by lanbh on 14-3-6.
//  Copyright (c) 2014年 com.vision. All rights reserved.
//
#if defined __cplusplus
extern "C"
{
#endif

#include <stdio.h>
#include "tool_stream2.h"
#include "tool_sysf.h"
#include "tool_type.h"
#include "tool_log.h"

TOOL_VOID tool_stream2_init(tool_stream2_t* stream)
{
    if (stream == NULL)
        log_fatal("arg NULL");
	tool_mem_memset(stream, sizeof(tool_stream2_t));
	tool_thread_initMutex(&stream->mutex);
    stream->write_pos = 0;
}

TOOL_VOID tool_stream2_initReadPos(tool_stream2_t* stream, TOOL_UINT32* read_pos)
{
    if (stream == NULL || read_pos == NULL)
        log_fatal("arg NULL");
	*read_pos = stream->write_pos;
}

TOOL_VOID tool_stream2_set(tool_stream2_t* stream, TOOL_VOID* data, TOOL_UINT32 len)
{
    if (stream == NULL || data == NULL)
        log_fatal("arg NULL");
    if (len > TOOL_STREAM2_DATA_SIZE)
	{
		log_error("len(%d) too long!", len);
		return ;
	}
    
    tool_thread_lockMutex(&stream->mutex);
	tool_mem_memcpy(stream->pool[stream->write_pos].data, data, len);
	stream->pool[stream->write_pos].len = len;
	stream->write_pos++;
	stream->write_pos %= TOOL_STREAM2_POOL_SIZE;
	tool_thread_unlockMutex(&stream->mutex);
}

TOOL_INT32 tool_stream2_get(tool_stream2_t* stream, TOOL_UINT32* read_pos, TOOL_VOID* data, TOOL_UINT32* len)
{
	if (stream == NULL || read_pos == NULL || data == NULL || len == NULL)
        log_fatal("arg NULL");
	if (*read_pos >= TOOL_STREAM2_POOL_SIZE)
        log_fatal("readpos(%d)", *read_pos);
 
    tool_thread_lockMutex(&stream->mutex);
	if (*read_pos == stream->write_pos)
	{
		tool_thread_unlockMutex(&stream->mutex);
		return 0;
	}
	if (*len < stream->pool[*read_pos].len)
	{
		log_error("len(%d) < len(%d)", *len, stream->pool[*read_pos].len);
		(*read_pos)++;
		(*read_pos) %= TOOL_STREAM2_POOL_SIZE;
		tool_thread_unlockMutex(&stream->mutex);
		return 0;
	}
	tool_mem_memcpy(data, stream->pool[*read_pos].data, stream->pool[*read_pos].len);
	*len = stream->pool[*read_pos].len;
	(*read_pos)++;
	(*read_pos) %= TOOL_STREAM2_POOL_SIZE;
	tool_thread_unlockMutex(&stream->mutex);
	return 1;
}

TOOL_VOID tool_stream2_done(tool_stream2_t* stream)
{
	if (stream == NULL)
        log_fatal("arg NULL");
    
	tool_thread_doneMutex(&stream->mutex);
}



#if defined __cplusplus
}
#endif
