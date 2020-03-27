//
//  tool_stream1.c
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
#include "tool_stream1.h"
#include "tool_sysf.h"
#include "tool_type.h"
#include "tool_log.h"

TOOL_VOID tool_stream1_init(tool_stream1_t* stream, TOOL_INT32 pool_size, TOOL_INT32 data_size)
{
    if (stream == NULL)
        log_fatal("arg NULL");
    if (pool_size > TOOL_STREAM1_POOL_SIZE)
        log_fatal("pool_size(%d)", pool_size);

	tool_mem_memset(stream, sizeof(tool_stream1_t));
	tool_thread_initMutex(&stream->mutex);
    stream->pool_size = pool_size;
    stream->data_size = data_size;
    stream->write_pos = 0;
    TOOL_INT32 i = 0;
    for (i = 0; i < stream->pool_size; i++)
    {
        if (data_size > 0)
            stream->pool[i].data = tool_mem_malloc(stream->data_size, 0);
        else
            stream->pool[i].data = NULL;
        stream->pool[i].len = 0;
    }
}

TOOL_VOID tool_stream1_initReadPos(tool_stream1_t* stream, TOOL_INT32* read_pos)
{
    if (stream == NULL || read_pos == NULL)
        log_fatal("arg NULL");
    
	*read_pos = stream->write_pos;
}

TOOL_VOID tool_stream1_set(tool_stream1_t* stream, TOOL_VOID* data, TOOL_INT32 len)
{
    if (stream == NULL || data == NULL)
        log_fatal("arg NULL");
    if (len > stream->data_size)
	{
		log_error("len(%d) too long!", len);
		return ;
	}
    
    tool_thread_lockMutex(&stream->mutex);
    if (stream->pool[stream->write_pos].data == NULL)
        log_fatal("tool_stream1_set");
	tool_mem_memcpy(stream->pool[stream->write_pos].data, data, len);
	stream->pool[stream->write_pos].len = len;
	stream->write_pos++;
	stream->write_pos %= stream->pool_size;
	tool_thread_unlockMutex(&stream->mutex);
}

TOOL_INT32 tool_stream1_get(tool_stream1_t* stream, TOOL_INT32* read_pos, TOOL_VOID* data, TOOL_INT32* len)
{
	if (stream == NULL || read_pos == NULL || data == NULL || len == NULL)
        log_fatal("arg NULL");
	if (*read_pos < 0 && *read_pos >= stream->pool_size)
        log_fatal("readpos(%d)", *read_pos);
 
    tool_thread_lockMutex(&stream->mutex);
	if (*read_pos == stream->write_pos)
	{
		tool_thread_unlockMutex(&stream->mutex);
		return 0;
	}
    if (stream->pool[stream->write_pos].data == NULL)
        log_fatal("tool_stream1_get");
	if (*len < stream->pool[*read_pos].len)
	{
		log_error("len(%d) < len(%d)", *len, stream->pool[*read_pos].len);
		(*read_pos)++;
		(*read_pos) %= stream->pool_size;
		tool_thread_unlockMutex(&stream->mutex);
		return 0;
	}
	tool_mem_memcpy(data, stream->pool[*read_pos].data, stream->pool[*read_pos].len);
	*len = stream->pool[*read_pos].len;
	(*read_pos)++;
	(*read_pos) %= stream->pool_size;
	tool_thread_unlockMutex(&stream->mutex);
    
	return 1;
}

TOOL_VOID tool_stream1_done(tool_stream1_t* stream)
{
	if (stream == NULL)
        log_fatal("arg NULL");
    
	tool_thread_doneMutex(&stream->mutex);
    TOOL_INT32 i = 0;
    for (i = 0; i < stream->pool_size; i++)
    {
        if (stream->pool[i].data)
        {
            tool_mem_free(stream->pool[i].data);
            stream->pool[i].data = NULL;
            stream->pool[i].len = 0;
        }
    }
}

TOOL_VOID tool_stream1_push(tool_stream1_t* stream, TOOL_VOID* data, TOOL_INT32 len)
{
    if (stream == NULL || data == NULL)
        log_fatal("arg NULL");
    
    tool_thread_lockMutex(&stream->mutex);
    if (stream->pool[stream->write_pos].data)
    {
        tool_mem_free(stream->pool[stream->write_pos].data);
        stream->pool[stream->write_pos].data = NULL;
        stream->pool[stream->write_pos].len = 0;
    }
    stream->pool[stream->write_pos].data = data;
	stream->pool[stream->write_pos].len = len;
	stream->write_pos++;
	stream->write_pos %= stream->pool_size;
	tool_thread_unlockMutex(&stream->mutex);
}

TOOL_INT32 tool_stream1_pop(tool_stream1_t* stream, TOOL_INT32* read_pos, TOOL_VOID** data, TOOL_INT32* len)
{
    if (stream == NULL || read_pos == NULL || data == NULL || len == NULL)
        log_fatal("arg NULL");
    if (*read_pos < 0 && *read_pos >= stream->pool_size)
        log_fatal("readpos(%d)", *read_pos);
    
    tool_thread_lockMutex(&stream->mutex);
	if (*read_pos == stream->write_pos)
	{
		tool_thread_unlockMutex(&stream->mutex);
		return 0;
	}
    if (stream->pool[stream->write_pos].data == NULL)
        log_fatal("tool_stream1_get");
    *data = stream->pool[*read_pos].data;
	*len = stream->pool[*read_pos].len;
    stream->pool[*read_pos].data = NULL;
    stream->pool[*read_pos].len = 0;
	(*read_pos)++;
	(*read_pos) %= stream->pool_size;
	tool_thread_unlockMutex(&stream->mutex);
    return 1;
}


#if defined __cplusplus
}
#endif
