
#if defined __cplusplus
extern "C"
{
#endif

#include "tool_log.h"
#include "tool_sysf.h"
#include "tool_type.h"
#include "tool_stream3.h"
#include "qh_ptc.h"

TOOL_VOID tool_stream3_init(tool_stream3_t* stream, TOOL_UINT32 size)
{
    if (stream == NULL)
        log_fatal("arg NULL");
	if (size < 4*1024*1024)
		size = 4*1024*1024;
	if (size > 8*1024*1024)
		size = 8*1024*1024;
	stream->size = size;
	stream->pool = (TOOL_UINT8*)tool_mem_malloc(stream->size, 0);
	stream->len = 0;
	stream->count = 0;
    stream->write_pos.offset = 0;
	stream->write_pos.ring = 0;
	stream->drop_flag = 0;
	tool_thread_initMutex(&stream->mutex);
}

TOOL_VOID tool_stream3_initReadPos(tool_stream3_t* stream, tool_stream3_pos_t* read_pos)
{
    if (stream == NULL || read_pos == NULL)
        log_fatal("arg NULL");
	tool_thread_lockMutex(&stream->mutex);
	stream->len = 0;
	stream->count = 0;
	read_pos->offset = stream->write_pos.offset;
	read_pos->ring = stream->write_pos.ring;
	tool_sysf_setTick(&read_pos->tick_sec, &read_pos->tick_usec);
	tool_thread_unlockMutex(&stream->mutex);
}

TOOL_INT32 tool_stream3_set_noDrop(tool_stream3_t* stream, TOOL_VOID* data, TOOL_UINT32 len, TOOL_INT32* flag)
{
    if (stream == NULL || data == NULL)
        log_fatal("arg NULL");
    if (len > TOOL_STREAM3_DATA_SIZE)
	{
		log_error("len(%d)", len);
		return 0;
	}

	ptc_frame_video_t* frame_data = (ptc_frame_video_t*)data;
	tool_stream3_head_t head;
	tool_mem_memcpy(head.flag, TOOL_STREAM3_FLAG, 12);
	TOOL_INT32 ret = -1;
	while (1)
	{
		tool_thread_lockMutex(&stream->mutex);
		if (stream->len > stream->size-2*TOOL_STREAM3_DATA_SIZE)
		{
			tool_thread_unlockMutex(&stream->mutex);
			tool_sysf_usleep(20*1000);
			if (flag && *flag)
				continue;
			else
				break;
		}
		if (stream->write_pos.offset + sizeof(tool_stream3_head_t) > stream->size)
			log_fatal("stream->write_pos.offset(%d)", stream->write_pos.offset);
		if (stream->write_pos.offset + 2*sizeof(tool_stream3_head_t) + TOOL_STREAM3_DATA_SIZE > stream->size)
		{
			head.len = 0xFFFFFFFF;
			tool_mem_memcpy(stream->pool+stream->write_pos.offset, &head, sizeof(tool_stream3_head_t));
			stream->write_pos.offset = 0;
			stream->write_pos.ring ++;
		}
		head.len = len;
		tool_mem_memcpy(stream->pool+stream->write_pos.offset, &head, sizeof(tool_stream3_head_t));
		stream->write_pos.offset += sizeof(tool_stream3_head_t);
		tool_mem_memcpy(stream->pool+stream->write_pos.offset, data, len);
		stream->write_pos.offset += len;
		if (frame_data->frame_head.frame_type == PTC_FRAME_I || frame_data->frame_head.frame_type == PTC_FRAME_P)
			stream->count ++;
		stream->len += (len+sizeof(tool_stream3_head_t));
	//	log_debug("tool_stream3_set_withDrop flag(%s) len(%d) offset(%d.%d) count(%d)", head.flag, head.len, stream->write_pos.offset, stream->size, stream->count);
		tool_thread_unlockMutex(&stream->mutex);
		ret = 0;
		break;
	}
	return ret;
}

TOOL_VOID tool_stream3_set_withDrop(tool_stream3_t* stream, TOOL_VOID* data, TOOL_UINT32 len, TOOL_INT32 nc_buffer_usec)
{
    if (stream == NULL || data == NULL)
        log_fatal("arg NULL");
    if (len > TOOL_STREAM3_DATA_SIZE)
	{
		log_error("len(%d)", len);
		return ;
	}

	ptc_frame_video_t* frame_data = (ptc_frame_video_t*)data;
	tool_stream3_head_t head;
	tool_mem_memcpy(head.flag, TOOL_STREAM3_FLAG, 12);
	
	TOOL_INT32 inteval_usec = 0;
	TOOL_INT32 buffer_usec = 0;	
	if (frame_data->frame_head.frame_rate <= 0 || frame_data->frame_head.frame_rate > 30)
		inteval_usec = 40*1000;
	else
		inteval_usec = 1000*1000/frame_data->frame_head.frame_rate;

    tool_thread_lockMutex(&stream->mutex);

	if (stream->drop_flag)
	{
		if (frame_data->frame_head.frame_type == PTC_FRAME_P)
		{
	//		log_debug("(%08x)xxxxx drop no(%d) type(%d)", stream, frame_data->frame_head.no, frame_data->frame_head.frame_type);
			tool_thread_unlockMutex(&stream->mutex);
			return ;
		}
		else if (frame_data->frame_head.frame_type == PTC_FRAME_I)
		{
	///		log_debug("(%08x)stop  drop no(%d) type(%d)", stream, frame_data->frame_head.no, frame_data->frame_head.frame_type);
			stream->drop_flag = 0;
		}
	}

	if (frame_data->frame_head.frame_type == PTC_FRAME_I || frame_data->frame_head.frame_type == PTC_FRAME_P)
	{		
		buffer_usec = inteval_usec * stream->count;
//		log_debug("buffer_usec(%d) nc_buffer_usec(%d) stream->count(%d)", buffer_usec, nc_buffer_usec, stream->count);
		if ((stream->len > stream->size-2*TOOL_STREAM3_DATA_SIZE || buffer_usec > 2*nc_buffer_usec) && (stream->count > 5))
		{
			stream->drop_flag = 1;
//			log_debug("(%08x)begin drop no(%d) type(%d)", stream, frame_data->frame_head.no, frame_data->frame_head.frame_type);
			tool_thread_unlockMutex(&stream->mutex);
			return ;
		}
	}
	
	if (stream->write_pos.offset + sizeof(tool_stream3_head_t) > stream->size)
		log_fatal("stream->write_pos.offset(%d)", stream->write_pos.offset);
	if (stream->write_pos.offset + 2*sizeof(tool_stream3_head_t) + TOOL_STREAM3_DATA_SIZE > stream->size)
	{
		head.len = 0xFFFFFFFF;
		tool_mem_memcpy(stream->pool+stream->write_pos.offset, &head, sizeof(tool_stream3_head_t));
		stream->write_pos.offset = 0;
		stream->write_pos.ring ++;
	}
	head.len = len;
	tool_mem_memcpy(stream->pool+stream->write_pos.offset, &head, sizeof(tool_stream3_head_t));
	stream->write_pos.offset += sizeof(tool_stream3_head_t);
	tool_mem_memcpy(stream->pool+stream->write_pos.offset, data, len);
	stream->write_pos.offset += len;
	if (frame_data->frame_head.frame_type == PTC_FRAME_I || frame_data->frame_head.frame_type == PTC_FRAME_P)
		stream->count ++;
	stream->len += (len+sizeof(tool_stream3_head_t));
//	log_debug("tool_stream3_set_withDrop flag(%s) len(%d) offset(%d) count(%d)", head.flag, head.len, stream->write_pos.offset, stream->count);
	tool_thread_unlockMutex(&stream->mutex);
}

#include "tool_sock.h"

TOOL_INT32 tool_stream3_state(tool_stream3_t* stream)
{
	if (stream == NULL)
        log_fatal("stream(0x%08x)", stream);

	static TOOL_INT32 tick = 0;
	if (tool_sock_isTickTimeout(tick, 1))
	{
//		log_debug("len(%d) size(%d)", stream->len, stream->size);
		tool_sock_setTick(&tick);
	}

	if (stream->len < TOOL_STREAM3_DATA_SIZE)
		return -1;
	else if (stream->len > stream->size-2*TOOL_STREAM3_DATA_SIZE)
		return 1;
	else
		return 0;
}

TOOL_INT32 tool_stream3_get_simple(tool_stream3_t* stream, tool_stream3_pos_t* read_pos, TOOL_VOID* data, TOOL_UINT32* len)
{
	if (stream == NULL || read_pos == NULL || data == NULL || len == NULL)
        log_fatal("stream(0x%08x) read_pos(0x%08x) data(0x%08x) len(0x%08x)", stream, read_pos, data, len);
	if (read_pos->offset >= stream->size)
        log_fatal("read_pos->offset(%d)", read_pos->offset);

	ptc_frame_video_t* frame_data = (ptc_frame_video_t*)data;
	tool_stream3_head_t head;
//	log_debug("stream(%08x)", stream);
    tool_thread_lockMutex(&stream->mutex);
//	log_debug("stream(%08x)", stream);
	if (read_pos->offset == stream->write_pos.offset && read_pos->ring == stream->write_pos.ring)
	{
//		log_debug("");
		tool_thread_unlockMutex(&stream->mutex);
		return 0;
	}
	if ((stream->write_pos.ring-read_pos->ring)*stream->size + (stream->write_pos.offset-read_pos->offset) > stream->size)
	{
		log_state("read(%d,%d) write(%d,%d)", read_pos->offset, read_pos->ring, stream->write_pos.offset, stream->write_pos.ring);
		stream->count = 0;
		stream->len = 0;
		read_pos->offset = stream->write_pos.offset;
		read_pos->ring = stream->write_pos.ring;
		tool_thread_unlockMutex(&stream->mutex);
		return 0;
	}

	tool_mem_memcpy(&head, stream->pool+read_pos->offset, sizeof(tool_stream3_head_t));
	read_pos->offset += sizeof(tool_stream3_head_t);
	if (head.len == 0xFFFFFFFF)
	{
		read_pos->offset = 0;
		read_pos->ring ++;
		tool_mem_memcpy(&head, stream->pool+read_pos->offset, sizeof(tool_stream3_head_t));
		read_pos->offset += sizeof(tool_stream3_head_t);
	}
	if (*len < head.len)
	{
		log_error("len(%d) < len(%d)", *len, head.len);
		stream->len = 0;
		stream->count = 0;
		read_pos->offset = stream->write_pos.offset;
		read_pos->ring = stream->write_pos.ring;
		tool_thread_unlockMutex(&stream->mutex);
		return 0;
	}
	tool_mem_memcpy(data, stream->pool+read_pos->offset, head.len);
	read_pos->offset += head.len;
	if (frame_data->frame_head.frame_type == PTC_FRAME_I || frame_data->frame_head.frame_type == PTC_FRAME_P)
		stream->count --;
	*len = head.len;
	stream->len -= (*len+sizeof(tool_stream3_head_t));
//	log_debug("tool_stream3_get_simple flag(%s) len(%d) offset(%d) count(%d)", head.flag, head.len, read_pos->offset, stream->count);
	tool_thread_unlockMutex(&stream->mutex);
	return 1;
}

TOOL_INT32 tool_stream3_get_withRts(tool_stream3_t* stream, tool_stream3_pos_t* read_pos, TOOL_VOID* data, TOOL_UINT32* len, TOOL_INT32 nc_buffer_usec)
{
	TOOL_INT32 inteval_usec = 0;
	TOOL_INT32 buffer_usec = 0;	
	ptc_frame_video_t* frame_data = (ptc_frame_video_t*)data;

	if (tool_stream3_get_simple(stream, read_pos, data, len) == 0)
		return 0;

	if (frame_data->frame_head.frame_type != PTC_FRAME_I && frame_data->frame_head.frame_type != PTC_FRAME_P)
		return 1;

	if (frame_data->frame_head.frame_rate <= 0 || frame_data->frame_head.frame_rate > 30)
		inteval_usec = 40*1000;
	else
		inteval_usec = 1000*1000/frame_data->frame_head.frame_rate;
	buffer_usec = inteval_usec * stream->count;
//	log_debug("inteval_usec(%d,%d) buffer_usec(%d,%d)", inteval_usec, stream->count, buffer_usec, nc_buffer_usec);
	if (buffer_usec < nc_buffer_usec/4)
	{
		tool_sysf_waitTickTimeout(read_pos->tick_sec, read_pos->tick_usec, 4*inteval_usec);
	}
	else if (buffer_usec < nc_buffer_usec/2)
	{
		tool_sysf_waitTickTimeout(read_pos->tick_sec, read_pos->tick_usec, 2*inteval_usec);
	}
	else if (buffer_usec < nc_buffer_usec*3/4)
	{
		tool_sysf_waitTickTimeout(read_pos->tick_sec, read_pos->tick_usec, 4*inteval_usec/3);
	}
	else if (buffer_usec < nc_buffer_usec)
	{
		tool_sysf_waitTickTimeout(read_pos->tick_sec, read_pos->tick_usec, inteval_usec);
	}
	else if (buffer_usec < nc_buffer_usec*5/4)
	{
		tool_sysf_waitTickTimeout(read_pos->tick_sec, read_pos->tick_usec, 3*inteval_usec/4);
	}
	else if (buffer_usec < nc_buffer_usec*6/4)
	{
		tool_sysf_waitTickTimeout(read_pos->tick_sec, read_pos->tick_usec, inteval_usec/2);
	}
	else
	{
		tool_sysf_waitTickTimeout(read_pos->tick_sec, read_pos->tick_usec, inteval_usec/4);
	}

	tool_sysf_setTick(&read_pos->tick_sec, &read_pos->tick_usec);
//	log_debug("frame(%d) type(%d,%d)", frame_data->frame_head.no, frame_data->frame_head.frame_type, frame_data->frame_head.frame_sub_type);	
	return 1;
}

TOOL_INT32 tool_stream3_get_withPbs(tool_stream3_t* stream, tool_stream3_pos_t* read_pos, TOOL_VOID* data, TOOL_UINT32* len)
{
	TOOL_INT32 inteval_usec = 0;
	ptc_frame_video_t* frame_data = (ptc_frame_video_t*)data;
	if (tool_stream3_get_simple(stream, read_pos, data, len) == 0)
		return 0;
	if (frame_data->frame_head.frame_type == PTC_FRAME_I || frame_data->frame_head.frame_type == PTC_FRAME_P)
	{
		if (frame_data->frame_head.frame_rate <= 0 || frame_data->frame_head.frame_rate > 30)
			inteval_usec = 40*1000;
		else
			inteval_usec = 1000*1000/frame_data->frame_head.frame_rate;
		tool_sysf_waitTickTimeout(read_pos->tick_sec, read_pos->tick_usec, inteval_usec);
		tool_sysf_setTick(&read_pos->tick_sec, &read_pos->tick_usec);
	}
	return 1;
}


TOOL_VOID tool_stream3_done(tool_stream3_t* stream)
{
	if (stream == NULL)
        log_fatal("arg NULL");

	tool_mem_free(stream->pool);
	stream->pool = NULL;
	tool_thread_doneMutex(&stream->mutex);
}

#if defined __cplusplus
}
#endif

