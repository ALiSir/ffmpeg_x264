
#if defined __cplusplus
extern "C"
{
#endif

#ifndef __TOOL_STREAM3_H__
#define __TOOL_STREAM3_H__

#include "tool_type.h"

#define TOOL_STREAM3_FLAG			"TOOL_STREAM"

#define TOOL_STREAM3_DATA_SIZE		(1024*1024)
#define TOOL_STREAM3_POOL_NUM		1024

typedef struct
{
	TOOL_UINT32 offset;
	TOOL_UINT32 ring;

	TOOL_INT32 tick_sec;
	TOOL_INT32 tick_usec;
}tool_stream3_pos_t;

typedef struct
{
	TOOL_UINT8 flag[12];
	TOOL_UINT32 len;
}tool_stream3_head_t;

typedef struct
{
	TOOL_UINT32 offset;
	TOOL_UINT8 type;
	TOOL_UINT8 reserve[3];
}tool_stream3_index_t;

typedef struct
{
	TOOL_UINT8* pool;
	TOOL_UINT32 size;
	TOOL_UINT32 len;	
	TOOL_INT32 count;
	tool_stream3_pos_t write_pos;
	TOOL_MUTEX mutex;

	TOOL_INT32 drop_flag;
}tool_stream3_t;

TOOL_VOID tool_stream3_init(tool_stream3_t* stream, TOOL_UINT32 size);
TOOL_VOID tool_stream3_initReadPos(tool_stream3_t* stream, tool_stream3_pos_t* read_pos);
TOOL_INT32 tool_stream3_set_noDrop(tool_stream3_t* stream, TOOL_VOID* data, TOOL_UINT32 len, TOOL_INT32* flag);
TOOL_VOID tool_stream3_set_withDrop(tool_stream3_t* stream, TOOL_VOID* data, TOOL_UINT32 len, TOOL_INT32 buffer_usec);
TOOL_INT32 tool_stream3_state(tool_stream3_t* stream);
TOOL_INT32 tool_stream3_get_simple(tool_stream3_t* stream, tool_stream3_pos_t* read_pos, TOOL_VOID* data, TOOL_UINT32* len);
TOOL_INT32 tool_stream3_get_withRts(tool_stream3_t* stream, tool_stream3_pos_t* read_pos, TOOL_VOID* data, TOOL_UINT32* len, TOOL_INT32 buffer_usec);
TOOL_INT32 tool_stream3_get_withPbs(tool_stream3_t* stream, tool_stream3_pos_t* read_pos, TOOL_VOID* data, TOOL_UINT32* len);
TOOL_VOID tool_stream3_done(tool_stream3_t* stream);



#endif

#if defined __cplusplus
}
#endif

