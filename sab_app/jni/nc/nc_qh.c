
#if defined __cplusplus
extern "C"
{
#endif

#include "nc.h"
#include "nc_qh.h"
#include "qh_ptc.h"
#include "tool_log.h"
#include "tool_type.h"
#include "tool_sysf.h"
#include "tool_stream2.h"
#include "tool_stream3.h"
#include "tool_sock.h"
#include "tool_ptc.h"
#include "tool_json.h"
#include <stdio.h>

#define NC_QH_USER_SIZE				128
#define	NC_QH_HEARTBEAT_CYCLE		5
#define	NC_QH_RECONNECT_CYCLE		3
#define NC_QH_SEARCH_DEV_CYCLE		30
#define NC_QH_STREAM_TIMEOUT 		10
#define NC_QH_USLEEP_CYCLE			(100*1000)


#define NC_QH_RTS_POOL_SIZE			(4*1024*1024)
#define NC_QH_PBS_POOL_SIZE			(4*1024*1024)

#define __NC_QH_TOOL__

typedef struct __nc_qh_user_t
{
	TOOL_INT32 user_id;
	TOOL_VOID* father_nc;
	TOOL_INT32 is_used;
	TOOL_MUTEX mutex;

	TOOL_UINT32 type;
	TOOL_INT32 state;
	TOOL_THREAD pid;
	TOOL_THREAD pid2;
	TOOL_INT8 ip_addr[PTC_IP_SIZE];
	TOOL_INT32 port;
	TOOL_INT8 user_name[PTC_ID_SIZE];
	TOOL_INT8 user_pswd[PTC_ID_SIZE];
	NC_CB cb;
	TOOL_VOID* param;

	tool_stream3_t stream3_recv;
	tool_stream3_pos_t stream3_read_pos;
	ptc_frame_video_t* stream3_frame_data;

	ptc_rts_start_t rts_start;
	TOOL_INT32 rts_key_flag;

	TOOL_INT32 pbs_end_flag;
	ptc_cfg_record_t pbs_start;
	TOOL_UINT32 pbs_pos;
	TOOL_INT32 pbs_pause;
	TOOL_INT32 pbs_restart;
	TOOL_INT32 pbs_speed;
	
	tool_sock_item_t sock_item;
	ptc_frame_t* frame;
	tool_stream2_t* audio_send;
	TOOL_UINT32 audio_pos;
	ptc_msg_t msg;
	TOOL_INT32 stream_tick;
	TOOL_INT32 last_type;
	
	TOOL_INT32 cfg_tick_sec;
	TOOL_INT32 cfg_tick_usec;
}nc_qh_user_t;

typedef struct
{
	nc_qh_user_t user[NC_QH_USER_SIZE];
	TOOL_MUTEX mutex;

	TOOL_INT32 state;
	TOOL_THREAD pid;
	TOOL_INT32 tick_sec;
	TOOL_INT32 tick_usec;

	TOOL_INT32 buffer_usec;

	TOOL_INT32 search_tick;
	TOOL_MUTEX search_mutex;
	ptc_mcast_searchs_t searchs;
}nc_qh_t;



nc_qh_t* g_nc_qh = NULL;


#define __NC_QH_QH_USER__
// ---------------------- user ------------------------------

/*
TOOL_INT32 nc_qh_user_checkResolution(nc_qh_user_t* user)
{
	if (user->stream3_frame_data == NULL)
		return -1;

	TOOL_INT32 i = 0;
	TOOL_INT32 j = 0;
	TOOL_INT32 start = 0;
	TOOL_INT32 end = 0;	
	for (i = 0; i < 100; i++)
	{
		if (user->stream3_frame_data->frame_body[i] == 0x00 &&
			user->stream3_frame_data->frame_body[i+1] == 0x00 && 
			user->stream3_frame_data->frame_body[i+2] == 0x00 &&
			user->stream3_frame_data->frame_body[i+3] == 0x01 &&
			user->stream3_frame_data->frame_body[i+4] == 0x67)
		{
			start = i+5;
			for (j = start; j < 100; j++)
			{
				if (user->stream3_frame_data->frame_body[i] == 0x00 &&
					user->stream3_frame_data->frame_body[i+1] == 0x00 && 
					user->stream3_frame_data->frame_body[i+2] == 0x00 &&
					user->stream3_frame_data->frame_body[i+3] == 0x01)
				{
					end = j;
					break;
				}
			}
			break;
		}
	}

	if (start == 0 || end == 0)
		return -1;

	lan_H264Context h;
	lan_init_lan_get_bits(&h.gb, sps, len*8);
	lan_ff_h264_decode_seq_parameter_set(&h, width, height);
	if (*width == 1920 && *height == 1088)
		*height = 1080;
	return 0;		
}
*/

TOOL_VOID nc_qh_user_doState(nc_qh_user_t* user, TOOL_INT32 type)
{
	if (type == user->last_type)
		return;

	ptc_frame_head_t head;
	tool_mem_memset(&head, sizeof(ptc_frame_head_t));
	head.frame_type = type;
	if (user->type == PTC_CMD_RTS_BASE || user->type == PTC_CMD_PBS_BASE)
	{
		tool_stream3_set_noDrop(&user->stream3_recv, &head, sizeof(ptc_frame_head_t), NULL);
	}
	else if (user->type == PTC_CMD_TALK_BASE)
	{
		if (user->cb)
			user->cb(user->user_id, &head, sizeof(ptc_frame_head_t), user->param);
	}
	
	user->last_type = type;
	tool_sysf_usleep(100*1000);
//	else if (user->type == PTC_CMD_PBS_BASE)
//	{
//		if (user->cb)
//			user->cb(user->user_id, &head, sizeof(ptc_frame_head_t), user->param);
//	}
}

TOOL_INT32 nc_qh_user_logout(nc_qh_user_t* user)
{
    if (tool_sock_item_isLive(&user->sock_item) == 0)
        return -1;
	user->msg.head.ack = 0;
    user->msg.head.len = 0;
	user->msg.head.cmd = PTC_CMD_USER_LOGOUT;
	tool_ptc_qh_sendMsg(&user->sock_item, &user->msg);
    tool_sock_item_close(&user->sock_item);
	return 0;
}

TOOL_INT32 nc_qh_user_login(nc_qh_user_t* user)
{
	nc_qh_user_doState(user, PTC_FRAME_STREAM_CONNECT);
	if (tool_sock_item_open_v2(&user->sock_item, TOOL_SOCK_TYPE1_TCP, TOOL_SOCK_TYPE2_CLIENT, user->ip_addr, user->port+PTC_PORT_OFFSET, NULL, 0) < 0)
	{
		log_error("nc_qh_user_login tool_sock_item_open");
		nc_qh_user_doState(user, PTC_FRAME_STREAM_CONNECT_FAIL);
		return NC_ERROR_CONNECT;
	}
	user->msg.head.len = sizeof(ptc_user_login_t);
	user->msg.head.cmd = PTC_CMD_USER_LOGIN;
	user->msg.head.ack = 0;
	ptc_user_login_t* user_login = (ptc_user_login_t*)user->msg.data;
    tool_mem_memset(user_login, sizeof(ptc_user_login_t));
    tool_str_strncpy(user_login->user, user->user_name, PTC_ID_SIZE-1);
    tool_str_strncpy(user_login->pswd, user->user_pswd, PTC_ID_SIZE-1);
	user_login->type = user->type;
	TOOL_INT32 ret = 0;
	do
	{
		nc_qh_user_doState(user, PTC_FRAME_STREAM_LOGIN);
		if (tool_ptc_qh_sendMsg(&user->sock_item, &user->msg) < 0)
		{
			nc_qh_user_doState(user, PTC_FRAME_STREAM_LOGIN_FAIL);
			ret = NC_ERROR_SEND;
			break;
		}
		if (tool_ptc_qh_recvMsg(&user->sock_item, &user->msg) < 0)
		{
			nc_qh_user_doState(user, PTC_FRAME_STREAM_LOGIN_FAIL);
			ret = NC_ERROR_RECV;
			break;
		}
		if (user->msg.head.cmd != PTC_CMD_USER_LOGIN)
		{
			nc_qh_user_doState(user, PTC_FRAME_STREAM_LOGIN_FAIL);
			ret = NC_ERROR_PROTOCOL;
			break;
		}
		if (user->msg.head.ack == PTC_ACK_ERROR_AUTH)
		{
			nc_qh_user_doState(user, PTC_FRAME_STREAM_AUTH_FAIL);
			ret = NC_ERROR_AUTH;
			break;
		}
		if (user->msg.head.ack == PTC_ACK_ERROR_BUSY && user->type == PTC_CMD_TALK_BASE)
		{
			nc_qh_user_doState(user, PTC_FRAME_STREAM_TALK_BUSY);
			ret = NC_ERROR_MAXCONNECT;
			break;
		}
		if (user->msg.head.ack == PTC_ACK_ERROR_BUSY && (user->type == PTC_CMD_RTS_BASE || user->type == PTC_CMD_PBS_BASE))
		{
			nc_qh_user_doState(user, PTC_FRAME_STREAM_VIDEO_BUSY);
			ret = NC_ERROR_MAXCONNECT;
			break;
		}
		
	} while (0);
	if (ret < 0)
	{
        log_error("nc_qh_user_login NC_QH_MSG_LOGIN ret(%d)", ret);
		tool_sock_item_close(&user->sock_item);
    }	
	user->rts_key_flag = 1;
	return ret;
}

TOOL_INT32 nc_qh_user_doRts(nc_qh_user_t* user)
{
	if (tool_sock_isTickTimeout(user->stream_tick, NC_QH_STREAM_TIMEOUT))
	{
		tool_sock_setTick(&user->stream_tick);
		nc_qh_user_doState(user, PTC_FRAME_STREAM_TIMEOUT);
	}

	nc_qh_t* nc = (nc_qh_t*)user->father_nc;
	if (tool_sock_item_isLive(&user->sock_item) == 0)
	{
		if (tool_sock_item_isTimeout(&user->sock_item, NC_QH_RECONNECT_CYCLE) == 0)
		{
			nc_qh_user_doState(user, PTC_FRAME_STREAM_WAIT);
			tool_sysf_usleep(NC_QH_USLEEP_CYCLE);
			return -1;
		}
		log_state("addr(%s:%d)", user->ip_addr, user->port);
		if (nc_qh_user_login(user) < 0)
		{
			return -1;
		}

		nc_qh_user_doState(user, PTC_FRAME_STREAM_START);
		user->msg.head.len = sizeof(ptc_rts_start_t);
		user->msg.head.cmd = PTC_CMD_RTS_START;
		user->msg.head.ack = 0;
		tool_mem_memcpy(user->msg.data, &user->rts_start, sizeof(ptc_rts_start_t));
		if (tool_ptc_qh_sendMsg(&user->sock_item, &user->msg) < 0 ||
			tool_ptc_qh_recvMsg(&user->sock_item, &user->msg) < 0 ||
			user->msg.head.cmd != PTC_CMD_RTS_START)
		{
			tool_sock_item_close(&user->sock_item);
			nc_qh_user_doState(user, PTC_FRAME_STREAM_START_FAIL);
			return -1;
		}
		nc_qh_user_doState(user, PTC_FRAME_STREAM_RUNNING);
	}

	tool_thread_lockMutex(&user->mutex);
	if (user->rts_key_flag)
	{
		user->msg.head.len = 0;
		user->msg.head.cmd = PTC_CMD_RTS_FORCE_KEY_FRAME;
		user->msg.head.ack = 0;
		if (tool_ptc_qh_sendMsg(&user->sock_item, &user->msg) < 0)
		{
			tool_thread_unlockMutex(&user->mutex);
			tool_sock_item_close(&user->sock_item);
			nc_qh_user_doState(user, PTC_FRAME_STREAM_BROKEN);
			return -1;
		}
		user->rts_key_flag = 0;
	}
	tool_thread_unlockMutex(&user->mutex);

	if (tool_sock_item_isTimeout(&user->sock_item, NC_QH_HEARTBEAT_CYCLE))
	{
		user->msg.head.len = 0;
		user->msg.head.cmd = PTC_CMD_USRE_HEARTBEAT;
		user->msg.head.ack = 0;
		if (tool_ptc_qh_sendMsg(&user->sock_item, &user->msg) < 0)
		{
			tool_sock_item_close(&user->sock_item);
			nc_qh_user_doState(user, PTC_FRAME_STREAM_BROKEN);
			return -1;
		}
		log_debug("rts(%d) heartbeat", user->user_id);
	}

	TOOL_INT32 ret = tool_sock_item_pollRead(&user->sock_item, 5000);
    if (ret <= 0)
    {
        tool_sock_item_close(&user->sock_item);
		nc_qh_user_doState(user, PTC_FRAME_STREAM_BROKEN);
		return -1;
    }
//	else if (ret == 0)
//	{
//		tool_sysf_usleep(20*1000);
//		return 0;
//	}

	if (tool_ptc_qh_recvFrame(&user->sock_item, user->frame) < 0)
	{
		tool_sock_item_close(&user->sock_item);
		nc_qh_user_doState(user, PTC_FRAME_STREAM_BROKEN);
		return -1;
	}

	if (user->frame->head.cmd == PTC_CMD_RTS_FRAME)
	{
		tool_stream3_set_withDrop(&user->stream3_recv, user->frame->data, user->frame->head.len, nc->buffer_usec);
	//	if (user->cb)
	//		user->cb(user->user_id, user->frame->data, user->frame->head.len, user->param);
	}
	else
	{
		log_state("user->frame->head.cmd(0x%08x)", user->frame->head.cmd);
	}
	tool_sock_setTick(&user->stream_tick);
	return 0;
}

TOOL_INT32 nc_qh_user_doTalk(nc_qh_user_t* user)
{
	TOOL_INT32 flag = 0;
	if (tool_sock_item_isLive(&user->sock_item) == 0)
	{
		if (tool_sock_item_isTimeout(&user->sock_item, NC_QH_RECONNECT_CYCLE) == 0)
		{
			tool_sysf_usleep(20*1000);
			return -1;
		}
		if (nc_qh_user_login(user) < 0)
		{
			return -1;
		}

		user->msg.head.len = 0;
		user->msg.head.cmd = PTC_CMD_TALK_START;
		user->msg.head.ack = 0;
		if (tool_ptc_qh_sendMsg(&user->sock_item, &user->msg) < 0 ||
			tool_ptc_qh_recvMsg(&user->sock_item, &user->msg) < 0 ||
			user->msg.head.cmd != PTC_CMD_TALK_START)
		{
			tool_sock_item_close(&user->sock_item);
			return -1;
		}
	}

	user->msg.head.len = PTC_MSG_SIZE;
	if (tool_stream2_get(user->audio_send, &user->audio_pos, user->msg.data, &user->msg.head.len))
	{
		user->msg.head.cmd = PTC_CMD_TALK_C2S;
		user->msg.head.ack = 0;
		if (tool_ptc_qh_sendMsg(&user->sock_item, &user->msg) < 0)
		{
			tool_sock_item_close(&user->sock_item);
			return -1;
		}
	}
	else
	{
		flag ++;
	}


	TOOL_INT32 ret = tool_sock_item_pollRead(&user->sock_item, 0);
    if (ret < 0)
    {
        tool_sock_item_close(&user->sock_item);
		return -1;
    }
	else if (ret > 0)
	{
		if (tool_ptc_qh_recvMsg(&user->sock_item, &user->msg) < 0 ||
			user->msg.head.cmd != PTC_CMD_TALK_S2C)
		{
			log_error("user->msg.head.cmd(0x%08x)", user->msg.head.cmd);
			tool_sock_item_close(&user->sock_item);
			return -1;
		}
		if (user->cb)
			user->cb(user->user_id, user->msg.data, user->msg.head.len, user->param);
	}
	else
	{
		flag ++;
	}

	if (flag == 2)
	{
		tool_sysf_usleep(20*1000);
	}
	return 0;
}


TOOL_INT32 nc_qh_user_doAlarm(nc_qh_user_t* user)
{
	if (tool_sock_item_isLive(&user->sock_item) == 0)
	{
		if (tool_sock_item_isTimeout(&user->sock_item, NC_QH_RECONNECT_CYCLE) == 0)
		{
			tool_sysf_usleep(20*1000);
			return -1;
		}
		if (nc_qh_user_login(user) < 0)
		{
			return -1;
		}
	}

	if (tool_sock_item_isTimeout(&user->sock_item, NC_QH_HEARTBEAT_CYCLE))
    {
		user->msg.head.len = 0;
		user->msg.head.cmd = PTC_CMD_USRE_HEARTBEAT;
		user->msg.head.ack = 0;
		while (1)
		{
			if (tool_ptc_qh_sendMsg(&user->sock_item, &user->msg) < 0
				|| tool_ptc_qh_recvMsg(&user->sock_item, &user->msg) < 0)
			{
				log_error("alarm(%d)", user->user_id);
				tool_sock_item_close(&user->sock_item);
				return -1;
			}
		
			if (user->msg.head.cmd == PTC_CMD_USRE_HEARTBEAT)
			{
				log_debug("alarm(%d) heartbeat", user->user_id);
				break;
			}
			else
			{
				if (user->cb)
					user->cb(user->user_id, user->msg.data, user->msg.head.len, user->param);
			}
		}
	}
	
	TOOL_INT32 ret = tool_sock_item_pollRead(&user->sock_item, 0);
    if (ret < 0)
    {
        tool_sock_item_close(&user->sock_item);
		return -1;
    }
	else if (ret > 0)
	{
		if (tool_ptc_qh_recvMsg(&user->sock_item, &user->msg) < 0)
		{
			tool_sock_item_close(&user->sock_item);
			return -1;
		}
		if (user->cb)
			user->cb(user->user_id, user->msg.data, user->msg.head.len, user->param);
		return 0;
	}

	tool_sysf_usleep(20*1000);
    return 0;
}

TOOL_INT32 nc_qh_user_doPbs(nc_qh_user_t* user)
{
	if (tool_sock_isTickTimeout(user->stream_tick, NC_QH_STREAM_TIMEOUT))
	{
		tool_sock_setTick(&user->stream_tick);
		nc_qh_user_doState(user, PTC_FRAME_STREAM_TIMEOUT);
	}

	if (user->pbs_end_flag == 1)
	{
		tool_sysf_usleep(NC_QH_USLEEP_CYCLE);
		return 0;
	}
	if (tool_sock_item_isLive(&user->sock_item) == 0)
	{
		if (tool_sock_item_isTimeout(&user->sock_item, NC_QH_RECONNECT_CYCLE) == 0)
		{
			nc_qh_user_doState(user, PTC_FRAME_STREAM_WAIT);
			tool_sysf_usleep(NC_QH_USLEEP_CYCLE);
			return -1;
		}
		if (nc_qh_user_login(user) < 0)
		{
			return -1;
		}

		nc_qh_user_doState(user, PTC_FRAME_STREAM_START);
		user->msg.head.len = sizeof(ptc_cfg_record_t);
		user->msg.head.cmd = PTC_CMD_PBS_START;
		user->msg.head.ack = 0;
		tool_mem_memcpy(user->msg.data, &user->pbs_start, sizeof(ptc_cfg_record_t));
		if (tool_ptc_qh_sendMsg(&user->sock_item, &user->msg) < 0 ||
			tool_ptc_qh_recvMsg(&user->sock_item, &user->msg) < 0 ||
			user->msg.head.cmd != PTC_CMD_PBS_START)
		{
			tool_sock_item_close(&user->sock_item);
			nc_qh_user_doState(user, PTC_FRAME_STREAM_START_FAIL);
			return -1;
		}
		nc_qh_user_doState(user, PTC_FRAME_STREAM_RUNNING);
	}

	if (tool_sock_item_isTimeout(&user->sock_item, NC_QH_HEARTBEAT_CYCLE))
	{
		user->msg.head.len = 0;
		user->msg.head.cmd = PTC_CMD_USRE_HEARTBEAT;
		user->msg.head.ack = 0;
		if (tool_ptc_qh_sendMsg(&user->sock_item, &user->msg) < 0)
		{
			tool_sock_item_close(&user->sock_item);
			nc_qh_user_doState(user, PTC_FRAME_STREAM_WAIT);
			return -1;
		}
		log_state("pbs(%d) heartbeat", user->user_id);
	}

	tool_thread_lockMutex(&user->mutex);
	if (user->pbs_pos)
	{
		user->msg.head.len = sizeof(TOOL_UINT32);
		user->msg.head.cmd = PTC_CMD_PBS_POS;
		user->msg.head.ack = 0;
		tool_mem_memcpy(user->msg.data, &user->pbs_pos, sizeof(TOOL_UINT32));
		if (tool_ptc_qh_sendMsg(&user->sock_item, &user->msg) < 0)
		{
			tool_thread_unlockMutex(&user->mutex);
			tool_sock_item_close(&user->sock_item);
			nc_qh_user_doState(user, PTC_FRAME_STREAM_WAIT);
			return -1;
		}
		user->pbs_pos = 0;
	}
	if (user->pbs_pause)
	{
		user->msg.head.len = 0;
		user->msg.head.cmd = PTC_CMD_PBS_PAUSE;
		user->msg.head.ack = 0;
		if (tool_ptc_qh_sendMsg(&user->sock_item, &user->msg) < 0)
		{
			tool_thread_unlockMutex(&user->mutex);
			tool_sock_item_close(&user->sock_item);
			nc_qh_user_doState(user, PTC_FRAME_STREAM_WAIT);
			return -1;
		}
		user->pbs_pause = 0;
	}
	if (user->pbs_restart)
	{
		user->msg.head.len = 0;
		user->msg.head.cmd = PTC_CMD_PBS_RESTART;
		user->msg.head.ack = 0;
		if (tool_ptc_qh_sendMsg(&user->sock_item, &user->msg) < 0)
		{
			tool_thread_unlockMutex(&user->mutex);
			tool_sock_item_close(&user->sock_item);
			nc_qh_user_doState(user, PTC_FRAME_STREAM_WAIT);
			return -1;
		}
		user->pbs_restart = 0;
	}
	if (user->pbs_speed)
	{
		user->msg.head.len = sizeof(TOOL_UINT32);
		user->msg.head.cmd = PTC_CMD_PBS_SPEED;
		user->msg.head.ack = 0;
		tool_mem_memcpy(user->msg.data, &user->pbs_speed, sizeof(TOOL_UINT32));
		if (tool_ptc_qh_sendMsg(&user->sock_item, &user->msg) < 0)
		{
			tool_thread_unlockMutex(&user->mutex);
			tool_sock_item_close(&user->sock_item);
			nc_qh_user_doState(user, PTC_FRAME_STREAM_WAIT);
			return -1;
		}
		user->pbs_speed = 0;
	}
	tool_thread_unlockMutex(&user->mutex);

	TOOL_INT32 ret = tool_sock_item_pollRead(&user->sock_item, 5000);
    if (ret <= 0)
    {
        tool_sock_item_close(&user->sock_item);
		nc_qh_user_doState(user, PTC_FRAME_STREAM_WAIT);
		return -1;
    }
//	else if (ret == 0)
//	{
//		tool_sysf_usleep(20*1000);
//		return 0;
//	}

	if (tool_ptc_qh_recvFrame(&user->sock_item, user->frame) < 0)
	{
		tool_sock_item_close(&user->sock_item);
		nc_qh_user_doState(user, PTC_FRAME_STREAM_WAIT);
		return -1;
	}
	if (user->frame->head.cmd == PTC_CMD_PBS_FRAME)
	{
		ptc_frame_head_t* frame_head = (ptc_frame_head_t*)user->frame->data;
		if (frame_head->frame_type == PTC_FRAME_PBS_END)
		{
			user->pbs_end_flag = 1;
			tool_sock_item_close(&user->sock_item);
		}
		else
		{
			user->pbs_start.start_time = frame_head->sec;
		}
		tool_stream3_set_noDrop(&user->stream3_recv, user->frame->data, user->frame->head.len, &user->state);
	}
	else
	{
		log_state("user->frame->head.cmd(0x%08x)", user->frame->head.cmd);
	}
	tool_sock_setTick(&user->stream_tick);
	return 1;
}

TOOL_VOID nc_qh_user_init(nc_qh_user_t* user, TOOL_INT32 user_id, TOOL_VOID* father_nc)
{
	user->user_id = user_id;
	user->father_nc = father_nc;
	tool_thread_initMutex(&user->mutex);
	tool_sock_item_setFlag(&user->sock_item, &user->state);	
}

TOOL_VOID* nc_qh_user_run(TOOL_VOID* param)
{
	nc_qh_user_t* user = (nc_qh_user_t*)param;
	tool_sock_setTick(&user->stream_tick);
	log_state("user(%d) type(0x%08x) start", user->user_id, user->type);
	while (user->state)
	{
		switch (user->type)
		{
			case PTC_CMD_RTS_BASE:
				nc_qh_user_doRts(user);
				break;
			case PTC_CMD_TALK_BASE:
				nc_qh_user_doTalk(user);
				break;
			case PTC_CMD_ALARM_BASE:
				nc_qh_user_doAlarm(user);
				break;
			case PTC_CMD_PBS_BASE:
				nc_qh_user_doPbs(user);
				break;
			default:
				tool_sysf_usleep(100*1000);
				break;
		}
	}
	log_state("user(%d) type(0x%08x) stop", user->user_id, user->type);
	nc_qh_user_logout(user);
	return 0;
}

TOOL_VOID* nc_qh_user_run2(TOOL_VOID* param)
{
	TOOL_UINT32 len = 0;
	nc_qh_user_t* user = (nc_qh_user_t*)param;
	nc_qh_t* nc = (nc_qh_t*)user->father_nc;
	tool_stream3_initReadPos(&user->stream3_recv, &user->stream3_read_pos);
	log_state("user_ex(%d) type(0x%08x) start", user->user_id, user->type);
	while (user->state)
	{
		len = sizeof(ptc_frame_t);
		if (tool_stream3_get_withRts(&user->stream3_recv, &user->stream3_read_pos, user->stream3_frame_data, &len, nc->buffer_usec) == 0)
		{
			tool_sysf_usleep(10*1000);
			continue;
		}
	
		if (user->cb)
			user->cb(user->user_id, user->stream3_frame_data, len, user->param);
	}
	log_state("user_ex(%d) type(0x%08x) stop", user->user_id, user->type);
	return 0;	
}

TOOL_INT32 nc_qh_user_start(nc_qh_t* nc, const TOOL_INT8 *ip_addr, TOOL_INT32 port, const TOOL_INT8 *user_name, const TOOL_INT8 *user_pswd, 
	TOOL_UINT32 type, NC_CB cb, TOOL_VOID* param, ptc_rts_start_t* rts_start, ptc_cfg_record_t* pbs_start)
{
	nc_qh_user_t* user = NULL;
	TOOL_INT32 i = 0;
	tool_thread_lockMutex(&nc->mutex);
	for (i = 0; i < NC_QH_USER_SIZE; i++)
	{
		if (nc->user[i].is_used == 0)
		{
			nc->user[i].is_used = 1;
			break;
		}
	}
	tool_thread_unlockMutex(&nc->mutex);
	if (i == NC_QH_USER_SIZE)
	{
		log_error("nc full");
		return NC_ERROR_FULL;
	}
	user = &nc->user[i];
	tool_str_strncpy(user->ip_addr, ip_addr, sizeof(user->ip_addr)-1);
	user->port = port;
	tool_str_strncpy(user->user_name, user_name, sizeof(user->user_name)-1);
	tool_str_strncpy(user->user_pswd, user_pswd, sizeof(user->user_pswd)-1);
	user->type = type;
	user->cb = cb;
	user->param = param;
	if (user->type == PTC_CMD_RTS_BASE)
	{
		tool_mem_memcpy(&user->rts_start, rts_start, sizeof(ptc_rts_start_t));
		user->frame = (ptc_frame_t*)tool_mem_malloc(sizeof(ptc_frame_t), 0);
		tool_stream3_init(&user->stream3_recv, NC_QH_RTS_POOL_SIZE);
		user->stream3_frame_data = (ptc_frame_video_t*)tool_mem_malloc(sizeof(ptc_frame_video_t), 0);
	}
	else if (user->type == PTC_CMD_TALK_BASE)
	{
		user->audio_send = (tool_stream2_t*)tool_mem_malloc(sizeof(tool_stream2_t), 0);
		tool_stream2_init(user->audio_send);
		tool_stream2_initReadPos(user->audio_send, &user->audio_pos);
	}
	else if (user->type == PTC_CMD_PBS_BASE)
	{
		tool_mem_memcpy(&user->pbs_start, pbs_start, sizeof(ptc_cfg_record_t));
		user->frame = (ptc_frame_t*)tool_mem_malloc(sizeof(ptc_frame_t), 0);
		tool_stream3_init(&user->stream3_recv, NC_QH_PBS_POOL_SIZE);
		user->stream3_frame_data = (ptc_frame_video_t*)tool_mem_malloc(sizeof(ptc_frame_video_t), 0);
		user->pbs_end_flag = 0;
	}
	else if (user->type == PTC_CMD_CFG_BASE)
	{
	}
	user->state = 1;
	if (tool_thread_create(&user->pid, NULL, nc_qh_user_run, (TOOL_VOID*)user) < 0)
		log_fatalNo("tool_thread_create");
	if (user->type == PTC_CMD_RTS_BASE || user->type == PTC_CMD_PBS_BASE)
	{
		if (tool_thread_create(&user->pid2, NULL, nc_qh_user_run2, (TOOL_VOID*)user) < 0)
			log_fatalNo("tool_thread_create");
	}
	
	return user->user_id;
}

void nc_qh_user_stop(nc_qh_user_t* user)
{
	if (user->state)
	{
		user->state = 0;
		tool_thread_join(user->pid);
	}
	else
	{
		return;
	}
	
	if (user->type == PTC_CMD_RTS_BASE)
	{
		tool_thread_join(user->pid2);
		if (user->frame)
		{
			tool_mem_free(user->frame);
			user->frame = NULL;
		}
		tool_stream3_done(&user->stream3_recv);
		tool_mem_free(user->stream3_frame_data);
		user->stream3_frame_data = NULL;
	}
	else if (user->type == PTC_CMD_TALK_BASE)
	{
		tool_stream2_done(user->audio_send);
		tool_mem_free(user->audio_send);
		user->audio_send = NULL;
	}
	else if (user->type == PTC_CMD_PBS_BASE)
	{
		tool_thread_join(user->pid2);
		if (user->frame)
		{
			tool_mem_free(user->frame);
			user->frame = NULL;
		}
		tool_stream3_done(&user->stream3_recv);
		tool_mem_free(user->stream3_frame_data);
		user->stream3_frame_data = NULL;
	}
	else if (user->type == PTC_CMD_CFG_BASE)
	{
	}
	user->cb = NULL;
	user->param = NULL;
	user->is_used = 0;
}

TOOL_VOID nc_qh_user_done(nc_qh_user_t* user)
{
	nc_qh_user_stop(user);
	tool_thread_doneMutex(&user->mutex);
}

TOOL_INT32 nc_qh_user_doCfg(const TOOL_INT8 *ip_addr, TOOL_INT32 port, const TOOL_INT8 *user_name, const TOOL_INT8 *user_pswd, 
	TOOL_UINT32 cmd, TOOL_VOID* in_data, TOOL_UINT32 in_len, TOOL_VOID* out_data, TOOL_UINT32* out_len)
{
	if ((cmd&0xFFFF0000) != PTC_CMD_CFG_BASE)
	{
		log_error("cmd(0x%08x)", cmd);
		return NC_ERROR_INTERNAL;
	}
	TOOL_INT32 ret = 0;
	nc_qh_user_t user;
	tool_mem_memset(&user, sizeof(nc_qh_user_t));
	tool_str_strncpy(user.ip_addr, ip_addr, sizeof(user.ip_addr)-1);
	user.port = port;
	tool_str_strncpy(user.user_name, user_name, sizeof(user.user_name)-1);
	tool_str_strncpy(user.user_pswd, user_pswd, sizeof(user.user_pswd)-1);
	user.type = PTC_CMD_CFG_BASE;
	user.state = 1;
	ret = nc_qh_user_login(&user);
	if (ret < 0)
	{
		user.state = 0;
		return ret;
	}
	ret = 0;
	do
	{
		user.msg.head.cmd = cmd;
		user.msg.head.len = in_len;
		user.msg.head.ack = 0;
		if (in_data && in_len)
			tool_mem_memcpy(user.msg.data, in_data, in_len);
		if (tool_ptc_qh_sendMsg(&user.sock_item, &user.msg) < 0)
		{
			ret = NC_ERROR_SEND;
			break;
		}
		if (tool_ptc_qh_recvMsg(&user.sock_item, &user.msg) < 0)
		{
			ret = NC_ERROR_RECV;
			break;
		}
		if (user.msg.head.cmd != cmd)
		{
			ret = NC_ERROR_INTERNAL;
			break;
		}

		if (out_data && out_len)
		{
			if (*out_len < user.msg.head.len)
			{
				log_error("out_len(%d), user.msg.head.len(%d)", *out_len, user.msg.head.len);
				ret = NC_ERROR_NOSPACE;
				break;
			}
			tool_mem_memcpy(out_data, user.msg.data, user.msg.head.len);
			*out_len = user.msg.head.len;
		}
	}while (0);
	nc_qh_user_logout(&user);
	user.state = 0;
	return ret;
}

TOOL_INT32 nc_qh_user_doCfg_v2(nc_dev* dev, ptc_msg_t* pst_msg)
{
	TOOL_INT32 cmd = pst_msg->head.cmd;
	TOOL_INT32 ret = 0;
	nc_qh_user_t user;
	tool_mem_memset(&user, sizeof(nc_qh_user_t));
	tool_str_strncpy(user.ip_addr, dev->ip, sizeof(user.ip_addr)-1);
	user.port = dev->msg_port;
	tool_str_strncpy(user.user_name, dev->user, sizeof(user.user_name)-1);
	tool_str_strncpy(user.user_pswd, dev->pswd, sizeof(user.user_pswd)-1);
	user.type = PTC_CMD_CFG2_BASE;
	user.state = 1;
	ret = nc_qh_user_login(&user);
	if (ret < 0)
	{
		user.state = 0;
		return ret;
	}
	ret = 0;
	do
	{
		log_debug("(0x%08x)", pst_msg->head.cmd);
		if (tool_ptc_qh_sendMsg(&user.sock_item, pst_msg) < 0)
		{
			ret = NC_ERROR_SEND;
			break;
		}
		if (tool_ptc_qh_recvMsg(&user.sock_item, pst_msg) < 0)
		{
			ret = NC_ERROR_RECV;
			break;
		}
		if (pst_msg->head.cmd != cmd)
		{
			log_debug("(0x%08x)(0x%08x) len(%d)", pst_msg->head.cmd, cmd, pst_msg->head.len);
			ret = NC_ERROR_INTERNAL;
			break;
		}

	}while (0);
	nc_qh_user_logout(&user);
	user.state = 0;
	return ret;
}

TOOL_INT32 nc_qh_user_doSearch(const TOOL_INT8 *ip_addr, TOOL_INT32 port, const TOOL_INT8 *user_name, const TOOL_INT8 *user_pswd, 
	TOOL_UINT32 cmd, TOOL_VOID* in_data, TOOL_UINT32 in_len, TOOL_VOID* out_data, TOOL_UINT32* out_len)
{
	if ((cmd&0xFFFF0000) != PTC_CMD_SEARCH_BASE)
	{
		log_error("cmd(0x%08x)", cmd);
		return NC_ERROR_INTERNAL;
	}
	TOOL_INT32 ret = 0;
	nc_qh_user_t user;
	tool_mem_memset(&user, sizeof(nc_qh_user_t));
	tool_str_strncpy(user.ip_addr, ip_addr, sizeof(user.ip_addr)-1);
	user.port = port;
	tool_str_strncpy(user.user_name, user_name, sizeof(user.user_name)-1);
	tool_str_strncpy(user.user_pswd, user_pswd, sizeof(user.user_pswd)-1);
	user.type = PTC_CMD_SEARCH_BASE;
	user.state = 1;
	ret = nc_qh_user_login(&user);
	if (ret < 0)
	{
		user.state = 0;
		return ret;
	}
	ret = 0;
	user.frame = (ptc_frame_t*)tool_mem_malloc(sizeof(ptc_frame_t), 0);
	do
	{
		user.frame->head.cmd = cmd;
		user.frame->head.len = in_len;
		user.frame->head.ack = 0;
		if (in_data && in_len)
			tool_mem_memcpy(user.frame->data, in_data, in_len);
		if (tool_ptc_qh_sendFrame(&user.sock_item, user.frame) < 0)
		{
			ret = NC_ERROR_SEND;
			break;
		}
		if (tool_ptc_qh_recvFrame(&user.sock_item, user.frame) < 0)
		{
			ret = NC_ERROR_RECV;
			break;
		}
		if (user.frame->head.cmd != cmd)
		{
			ret = NC_ERROR_INTERNAL;
			break;
		}

		if (out_data && out_len)
		{
			if (*out_len < user.frame->head.len)
			{
				log_error("out_len(%d), user.frame->head.len(%d)", *out_len, user.frame->head.len);
				ret = NC_ERROR_NOSPACE;
				break;
			}
			tool_mem_memcpy(out_data, user.frame->data, user.frame->head.len);
			*out_len = user.frame->head.len;
		}
	}while (0);
	tool_mem_free(user.frame);
	user.frame = NULL;
	nc_qh_user_logout(&user);
	user.state = 0;	
	return ret;
}

TOOL_INT32 nc_qh_user_doSearch2(nc_dev* dev, ptc_frame_t* pst_frame)
{
	TOOL_INT32 cmd = pst_frame->head.cmd;
	TOOL_INT32 ret = 0;
	nc_qh_user_t user;
	tool_mem_memset(&user, sizeof(nc_qh_user_t));
	tool_str_strncpy(user.ip_addr, dev->ip, sizeof(user.ip_addr)-1);
	user.port = dev->msg_port;
	tool_str_strncpy(user.user_name, dev->user, sizeof(user.user_name)-1);
	tool_str_strncpy(user.user_pswd, dev->pswd, sizeof(user.user_pswd)-1);
	user.type = PTC_CMD_SEARCH2_BASE;
	user.state = 1;
	ret = nc_qh_user_login(&user);
	if (ret < 0)
	{
		user.state = 0;
		return ret;
	}
	ret = 0;
	do
	{
		log_debug("(0x%08x)", pst_frame->head.cmd);
		if (tool_ptc_qh_sendFrame(&user.sock_item, pst_frame) < 0)
		{
			ret = NC_ERROR_SEND;
			break;
		}
		if (tool_ptc_qh_recvFrame(&user.sock_item, pst_frame) < 0)
		{
			ret = NC_ERROR_RECV;
			break;
		}
		if (pst_frame->head.cmd != cmd)
		{
			log_debug("(0x%08x)(0x%08x) len(%d)", pst_frame->head.cmd, cmd, pst_frame->head.len);
			ret = NC_ERROR_INTERNAL;
			break;
		}
//		log_debug("pst_frame->head.data(%s)", pst_frame->data);
	}while (0);
	nc_qh_user_logout(&user);
	user.state = 0;
	return ret;
}


//////////////////////////////////////////////////////////////////////////
#define __NC_QH_INFO__

TOOL_VOID* nc_qh_run(TOOL_VOID* param)
{
	TOOL_INT32 i = 0;
	TOOL_INT32 j = 0;
	nc_qh_t* nc = (nc_qh_t*)param;
	nc_qh_user_t* user = NULL;
//	tool_sysf_setTick(&nc->tv_sec, &nc->tv_usec);
	ptc_mcast_searchs_t searchs;
	while (nc->state)
	{	
		tool_thread_lockMutex(&nc->mutex);
		for (i = 0; i < NC_QH_USER_SIZE; i++)
		{
			user = &nc->user[i];
			if (user->is_used && user->type == PTC_CMD_CFG_BASE && 
				tool_sysf_isTickTimeout(user->cfg_tick_sec, user->cfg_tick_usec, 10*1000*1000))
			{
				nc_qh_user_stop(user);
			}
		}
		tool_thread_unlockMutex(&nc->mutex);

		if (tool_sock_isTickTimeout(nc->search_tick, NC_QH_SEARCH_DEV_CYCLE))
		{
			tool_sock_setTick(&nc->search_tick);
			nc_qh_searchDev(&searchs);
		
			tool_thread_lockMutex(&nc->search_mutex);
			for (j = 0; j < searchs.num; j++)
			{
				searchs.search[j].base.flag = 2;
			}

			for (i = 0; i < nc->searchs.num; i++)
			{
				nc->searchs.search[i].base.flag --;
			}


			for (i = 0; i < nc->searchs.num; i++)
			{
				if (nc->searchs.search[i].base.flag <= 0 || nc->searchs.search[i].base.flag > 2)
					continue;
				
				if (searchs.num == PTC_MCAST_SEARCH_SIZE)
					break;
				
				for (j = 0; j < searchs.num; j++)
				{
					if (tool_str_strncmp(searchs.search[j].base.dev_id, nc->searchs.search[i].base.dev_id, 64) == 0)
						break;
				}
				if (j >= searchs.num)
				{
					tool_mem_memcpy(&searchs.search[j], &nc->searchs.search[i], sizeof(ptc_mcast_search_t));
					searchs.num ++;
				}
			}

			tool_mem_memcpy(&nc->searchs, &searchs, sizeof(ptc_mcast_searchs_t));
			tool_thread_unlockMutex(&nc->search_mutex);
/*
			for (i = 0; i < searchs.num; i++)
			{
				log_debug("i(%d) addr(%d.%d.%d.%d:%d) dev_id(%s) flag(%d)", 
					i, searchs.search[i].net.ip1[0], searchs.search[i].net.ip1[1], searchs.search[i].net.ip1[2], searchs.search[i].net.ip1[3],
					searchs.search[i].net.dev_port, searchs.search[i].base.dev_id, searchs.search[i].base.flag);
			}
*/
		}
		
		tool_sysf_usleep(100*1000);
	}
	return NULL;
}

TOOL_VOID nc_qh_init()
{
//	log_debug("sizeof(ptc_mcast_search_t)=%d", sizeof(ptc_mcast_search_t));
//	log_debug("sizeof(ptc_cfg_net_t)=%d", sizeof(ptc_cfg_net_t));
//	log_debug("sizeof(ptc_cfg_ability_t)=%d", sizeof(ptc_cfg_ability_t));
//	tool_sysf_sleep(1000);

	if (g_nc_qh)
		return ;
	log_state("init qh sdk(v%04x.%04x)", PTC_VERSION, PTC_DATA);
	g_nc_qh = (nc_qh_t*)tool_mem_malloc(sizeof(nc_qh_t), 1);
    tool_thread_initMutex(&g_nc_qh->mutex);
	g_nc_qh->buffer_usec = 300*1000;
	tool_thread_initMutex(&g_nc_qh->search_mutex);
	g_nc_qh->search_tick = -12345678;

	TOOL_INT32 i = 0;
	for (i = 0; i < NC_QH_USER_SIZE; i++)
		nc_qh_user_init(&g_nc_qh->user[i], i, g_nc_qh);

	g_nc_qh->state = 1;
	if (tool_thread_create(&g_nc_qh->pid, NULL, nc_qh_run, (TOOL_VOID*)g_nc_qh) < 0)
		log_fatal("tool_thread_create");
}

TOOL_INT32 nc_qh_setBuffer(TOOL_INT32 buffer_usec)
{
	if (g_nc_qh == NULL)
	{
		log_error("nc");
		return NC_ERROR_UNINIT;
	}
	if (buffer_usec < 0 || buffer_usec > 3000*1000)
	{
		log_error("buffer_usec(%d)", buffer_usec);
		return NC_ERROR_INPUT;
	}
	g_nc_qh->buffer_usec = buffer_usec;
	return 0;
}

TOOL_VOID nc_qh_done()
{
    if (g_nc_qh == NULL)
        return;
    log_state("done qh sdk(v%04x.%04x)", PTC_VERSION, PTC_DATA);
	g_nc_qh->state = 0;
	tool_thread_join(g_nc_qh->pid);
	TOOL_INT32 i = 0;
	for (i = 0; i < NC_QH_USER_SIZE; i++)
		nc_qh_user_done(&g_nc_qh->user[i]);
	tool_thread_doneMutex(&g_nc_qh->mutex);
	tool_thread_doneMutex(&g_nc_qh->search_mutex);
	tool_mem_free(g_nc_qh);
	g_nc_qh = NULL;
	tool_sock_done();
}

TOOL_INT32 nc_qh_getSearchById(nc_dev* pst_nc_dev)
{
	if (g_nc_qh == NULL)
	{
		log_error("nc");
		return NC_ERROR_UNINIT;
	}
	if (pst_nc_dev == NULL)
	{
		log_error("pst_nc_dev(0x%08x)", pst_nc_dev);
		return NC_ERROR_INPUT;
	}

	TOOL_INT32 i = 0;
	tool_thread_lockMutex(&g_nc_qh->search_mutex);
	for (i = 0; i < g_nc_qh->searchs.num; i++)
	{
//		log_debug("i(%d), dev_id(%s)(%s)", i, pst_nc_dev->dev_id, g_nc_qh->searchs.search[i].base.dev_id); 
		if (tool_str_strncmp(pst_nc_dev->dev_id, g_nc_qh->searchs.search[i].base.dev_id, 64) == 0)
		{
			pst_nc_dev->ptc = NC_PTC_Qihan;
			tool_sysf_inet_ntop(g_nc_qh->searchs.search[i].net.ip1, pst_nc_dev->ip, PTC_IP_SIZE);
			pst_nc_dev->msg_port = g_nc_qh->searchs.search[i].net.dev_port;
			pst_nc_dev->stream_port = 0;
			tool_mem_memset(pst_nc_dev->user, PTC_ID_SIZE);
			tool_str_strncpy(pst_nc_dev->user, "qihan", 5);
			tool_mem_memset(pst_nc_dev->pswd, PTC_ID_SIZE);
			tool_str_strncpy(pst_nc_dev->pswd, "nahiq", 5);
			log_debug("dev_id(%s) addr(%s:%d) user(%s.%s)", pst_nc_dev->dev_id, pst_nc_dev->ip, pst_nc_dev->msg_port, pst_nc_dev->user, pst_nc_dev->pswd);
			tool_thread_unlockMutex(&g_nc_qh->search_mutex);
			return 1;
		}
	}
	tool_thread_unlockMutex(&g_nc_qh->search_mutex);
	log_debug("dev_id(%s)", pst_nc_dev->dev_id);
	return 0;
}

TOOL_INT32 nc_qh_getSearch(ptc_mcast_searchs_t* pst_searchs)
{
	if (g_nc_qh == NULL)
	{
		log_error("nc");
		return NC_ERROR_UNINIT;
	}
	if (pst_searchs == NULL)
	{
		log_error("nc_qh_getSearch arg");
		return NC_ERROR_INPUT;
	}		

	tool_thread_lockMutex(&g_nc_qh->search_mutex);
	tool_mem_memcpy(pst_searchs, &g_nc_qh->searchs, sizeof(ptc_mcast_searchs_t));
	g_nc_qh->search_tick = -12345678;
	tool_thread_unlockMutex(&g_nc_qh->search_mutex);
	return 0;
}


TOOL_INT32 nc_qh_searchDev(ptc_mcast_searchs_t* searchs)
{	
	searchs->num = 0;

	TOOL_INT32 tick_sec = 0;
	TOOL_INT32 tick_usec = 0;
	TOOL_INT32 ret = 0;
	TOOL_INT32 result = -1;
	ptc_msg_t msg;
	tool_sock_item_t sock_c2s;
	tool_sock_item_t sock_s2c;
	TOOL_INT32 i = 0;
	do
	{
		if (tool_sock_item_open_v2(&sock_c2s, TOOL_SOCK_TYPE1_BCAST, TOOL_SOCK_TYPE2_NONE, PTC_BCAST_IP, PTC_BCAST_C2S_PORT, NULL, PTC_BCAST_C2S_PORT) < 0)
		{
			log_error("tool_sock_createMcast sock_c2s");
			result = NC_ERROR_INTERNAL;
			break;
		}
		if (tool_sock_item_open_v2(&sock_s2c, TOOL_SOCK_TYPE1_BCAST, TOOL_SOCK_TYPE2_NONE, PTC_BCAST_IP, PTC_BCAST_S2C_PORT, NULL, PTC_BCAST_S2C_PORT) < 0)
		{
			log_error("tool_sock_createMcast sock_s2c");
			result = NC_ERROR_INTERNAL;
			break;
		}

		tool_mem_memset(&msg, sizeof(ptc_head_t));
		msg.head.cmd = PTC_CMD_MCAST_SEARCH_REQ;
		msg.head.len = 0;
		msg.head.ack = 0;
		if (tool_ptc_qh_sendtoMsg(&sock_c2s, &msg) < 0)
		{
			log_error("tool_sock_item_sendto");
			result = NC_ERROR_SEND;
			break;
		}

		tool_sysf_setTick(&tick_sec, &tick_usec);
		while (1)
		{
			if (searchs->num >= PTC_MCAST_SEARCH_SIZE)
				break;
			
			if (tool_sysf_isTickTimeout(tick_sec, tick_usec, 5000*1000))
				break;

			ret = tool_sock_item_pollRead(&sock_s2c, 100);
			if (ret < 0)
			{
				log_state("tool_sock_item_pollRead");
				break;
			}
			else if (ret == 0)
				continue;

			tool_mem_memset(&msg, sizeof(ptc_head_t));
			if (tool_ptc_qh_recvfromMsg(&sock_s2c, &msg) < 0)
			{
				log_error("tool_ptc_qh_recvfromMsg");
				break;
			}
			if (msg.head.len != sizeof(ptc_mcast_search_t))
			{
				log_error("msg.head.len(%d)", msg.head.len);
 				continue;
			}

			if (msg.head.cmd == PTC_CMD_MCAST_SEARCH_RESP)
			{
				for (i = 0; i < searchs->num; i++)
				{
					if (tool_mem_memcmp(&searchs->search[searchs->num], msg.data, sizeof(ptc_mcast_search_t)) == 0)
						break;
				}
				if (i >= searchs->num)
				{
					tool_mem_memcpy(&searchs->search[searchs->num], msg.data, sizeof(ptc_mcast_search_t));
					searchs->search[searchs->num].nc_ptc_type = NC_PTC_Qihan;
					searchs->num ++;	
				}
			}
		}
		result = 0;		
	}while (0);
	tool_sock_item_close(&sock_c2s);
	tool_sock_item_close(&sock_s2c);

	return result;
}

TOOL_INT32 nc_qh_auth(const TOOL_INT8* ip, TOOL_INT32 port, const TOOL_INT8 *user, const TOOL_INT8 *pswd)
{
	if (g_nc_qh == NULL)
	{
		log_error("nc");
		return NC_ERROR_UNINIT;
	}
	if (ip == NULL || user == NULL || pswd == NULL)
	{
		log_error("nc_qh_Login arg");
		return NC_ERROR_INPUT;
	}
	
	return nc_qh_user_doCfg(ip, port, user, pswd, PTC_CMD_CFG_AUTH, NULL, 0, NULL, NULL);
}

TOOL_INT32 nc_qh_auth_v2(nc_dev* dev)
{
	if (g_nc_qh == NULL)
	{
		log_error("nc");
		return NC_ERROR_UNINIT;
	}
	if (dev == NULL)
	{
		log_error("nc_qh_Login arg");
		return NC_ERROR_INPUT;
	}
	
	return nc_qh_user_doCfg(dev->ip, dev->msg_port, dev->user, dev->pswd, PTC_CMD_CFG_AUTH, NULL, 0, NULL, NULL);
}

TOOL_INT32 nc_qh_login(const TOOL_INT8* ip, TOOL_INT32 port, const TOOL_INT8 *user, const TOOL_INT8 *pswd, NC_CB cb, TOOL_VOID* param)
{
	if (g_nc_qh == NULL)
	{
		log_error("nc");
		return NC_ERROR_UNINIT;
	}
	if (ip == NULL || user == NULL || pswd == NULL)
	{
		log_error("nc_qh_Login arg");
		return NC_ERROR_INPUT;
	}
	return nc_qh_user_start(g_nc_qh, ip, port, user, pswd, PTC_CMD_ALARM_BASE, cb, param, NULL, NULL);
}

TOOL_INT32 nc_qh_login_v2(nc_dev* dev, NC_CB cb, TOOL_VOID* param)
{
	if (g_nc_qh == NULL)
	{
		log_error("nc");
		return NC_ERROR_UNINIT;
	}
	if (dev == NULL)
	{
		log_error("nc_qh_Login arg");
		return NC_ERROR_INPUT;
	}
	return nc_qh_user_start(g_nc_qh, dev->ip, dev->msg_port, dev->user, dev->pswd, PTC_CMD_ALARM_BASE, cb, param, NULL, NULL);
}

TOOL_INT32 nc_qh_logout(TOOL_INT32 user_id)
{
	if (g_nc_qh == NULL)
	{
		log_error("nc");
		return NC_ERROR_UNINIT;
	}
	if (user_id < 0 || user_id >= NC_QH_USER_SIZE)
	{
		log_error("user_id(%d)", user_id);
		return NC_ERROR_INPUT;
	}
    nc_qh_user_stop(&g_nc_qh->user[user_id]);
	return 0;
}

TOOL_INT32 nc_qh_getOnlineStatus(TOOL_INT32 user_id)
{
	if (g_nc_qh == NULL)
	{
		log_error("nc");
		return NC_ERROR_UNINIT;
	}
	if (user_id < 0 || user_id >= NC_QH_USER_SIZE)
	{
		log_error("user_id(%d)", user_id);
		return NC_ERROR_INPUT;
	}
    return tool_sock_item_isLive(&g_nc_qh->user[user_id].sock_item);
}

TOOL_INT32 nc_qh_getChannelNum(TOOL_INT32 user_id)
{
    if (g_nc_qh == NULL)
	{
		log_error("nc");
		return NC_ERROR_UNINIT;
	}
	if (user_id < 0 || user_id >= NC_QH_USER_SIZE)
	{
		log_error("user_id(%d)", user_id);
		return NC_ERROR_INPUT;
	}
	nc_qh_user_t* user = &g_nc_qh->user[user_id];
	ptc_cfg_ability_t ability;
	TOOL_UINT32 len = sizeof(ptc_cfg_ability_t);
	TOOL_INT32 ret = nc_qh_user_doCfg(user->ip_addr, user->port, user->user_name, user->user_pswd, PTC_CMD_CFG_GET_ABILITY, NULL, 0, &ability, &len);
	if (ret < 0)
		return ret;
	return ability.video_input_num;
}

TOOL_INT32 nc_qh_getChannelNum_v2(nc_dev* dev)
{
	if (g_nc_qh == NULL)
	{
		log_error("nc");
		return NC_ERROR_UNINIT;
	}
	ptc_cfg_ability_t ability;
	TOOL_UINT32 len = sizeof(ptc_cfg_ability_t);
	TOOL_INT32 ret = nc_qh_user_doCfg(dev->ip, dev->msg_port, dev->user, dev->pswd, PTC_CMD_CFG_GET_ABILITY, NULL, 0, &ability, &len);
	if (ret < 0)
		return ret;
	return ability.video_input_num;
}

TOOL_INT32 nc_qh_startRts(TOOL_INT32 user_id, TOOL_INT32 channel, TOOL_INT32 stream_type, NC_CB cb, TOOL_VOID* param)
{
	if (g_nc_qh == NULL)
	{
		log_error("nc");
		return NC_ERROR_UNINIT;
	}
	if (user_id < 0 || user_id >= NC_QH_USER_SIZE)
	{
		log_error("user_id(%d)", user_id);
		return NC_ERROR_INPUT;
	}
	ptc_rts_start_t rts_start;
	rts_start.channel_no = channel;
	rts_start.channel_type = stream_type;
	nc_qh_user_t* user = &g_nc_qh->user[user_id];
	return nc_qh_user_start(g_nc_qh, user->ip_addr, user->port, user->user_name, user->user_pswd, PTC_CMD_RTS_BASE, cb, param, &rts_start, NULL);
}

TOOL_INT32 nc_qh_startRts_v2(nc_dev* dev, TOOL_INT32 channel, TOOL_INT32 stream_type, NC_CB cb, void* param)
{
	if (g_nc_qh == NULL)
	{
		log_error("nc");
		return NC_ERROR_UNINIT;
	}
	if (dev == NULL)
	{
		log_error("dev");
		return NC_ERROR_INPUT;
	}
	ptc_rts_start_t rts_start;
	rts_start.channel_no = channel;
	rts_start.channel_type = stream_type;
	return nc_qh_user_start(g_nc_qh, dev->ip, dev->msg_port, dev->user, dev->pswd, PTC_CMD_RTS_BASE, cb, param, &rts_start, NULL);
}

int nc_qh_forceRtsKeyFrame(int user_id)
{
	if (g_nc_qh == NULL)
	{
		log_error("nc");
		return NC_ERROR_UNINIT;
	}
	if (user_id < 0 || user_id >= NC_QH_USER_SIZE)
	{
		log_error("user_id(%d)", user_id);
		return NC_ERROR_INPUT;
	}
	nc_qh_user_t* user = (nc_qh_user_t*)&g_nc_qh->user[user_id];
	tool_thread_lockMutex(&user->mutex);
	user->rts_key_flag = 1;
	tool_thread_unlockMutex(&user->mutex);
	return 0;	
}

TOOL_INT32 nc_qh_stopRts(TOOL_INT32 user_id)
{
	if (g_nc_qh == NULL)
	{
		log_error("nc");
		return NC_ERROR_UNINIT;
	}
	if (user_id < 0 || user_id >= NC_QH_USER_SIZE)
	{
		log_error("user_id(%d)", user_id);
		return NC_ERROR_INPUT;
	}
    nc_qh_user_stop(&g_nc_qh->user[user_id]);
	return 0;
}

TOOL_INT32 nc_qh_getRtsStatus(TOOL_INT32 user_id)
{
	if (g_nc_qh == NULL)
	{
		log_error("nc");
		return NC_ERROR_UNINIT;
	}
	if (user_id < 0 || user_id >= NC_QH_USER_SIZE)
	{
		log_error("user_id(%d)", user_id);
		return NC_ERROR_INPUT;
	}
        
	return tool_sock_item_isLive(&g_nc_qh->user[user_id].sock_item);
}


TOOL_INT32 nc_qh_controlPTZ_byRts(TOOL_INT32 user_id, PTC_PTZ_CMD_E cmd, TOOL_INT32 param)
{
	if (g_nc_qh == NULL)
	{
		log_error("nc");
		return NC_ERROR_UNINIT;
	}
	if (user_id < 0 || user_id >= NC_QH_USER_SIZE)
	{
		log_error("user_id(%d)", user_id);
		return NC_ERROR_INPUT;
	}
	nc_qh_user_t* user = &g_nc_qh->user[user_id];
	ptc_control_ptz_t control_ptz;
	control_ptz.channel_no = user->rts_start.channel_no;
	control_ptz.cmd = cmd;
	control_ptz.param = param;
	TOOL_UINT32 len = sizeof(ptc_control_ptz_t);
	return nc_qh_user_doCfg(user->ip_addr, user->port, user->user_name, user->user_pswd, PTC_CMD_CFG_CONTROL_PTZ, &control_ptz, len, NULL, NULL);
}

TOOL_INT32 nc_qh_controlPTZ_byUserAndChannel(TOOL_INT32 user_id, TOOL_INT32 channel, PTC_PTZ_CMD_E cmd, TOOL_INT32 param)
{
	if (g_nc_qh == NULL)
	{
		log_error("nc");
		return NC_ERROR_UNINIT;
	}
	if (user_id < 0 || user_id >= NC_QH_USER_SIZE)
	{
		log_error("user_id(%d)", user_id);
		return NC_ERROR_INPUT;
	}
	nc_qh_user_t* user = &g_nc_qh->user[user_id];
	ptc_control_ptz_t control_ptz;
	control_ptz.channel_no = channel;
	control_ptz.cmd = cmd;
	control_ptz.param = param;
	TOOL_UINT32 len = sizeof(ptc_control_ptz_t);
	return nc_qh_user_doCfg(user->ip_addr, user->port, user->user_name, user->user_pswd, PTC_CMD_CFG_CONTROL_PTZ, &control_ptz, len, NULL, NULL);
}

TOOL_INT32 nc_qh_controlPTZ_v2(nc_dev* dev, TOOL_INT32 channel, PTC_PTZ_CMD_E cmd, TOOL_INT32 param)
{
	if (g_nc_qh == NULL)
	{
		log_error("nc");
		return NC_ERROR_UNINIT;
	}
	if (dev == NULL)
	{
		log_error("dev");
		return NC_ERROR_INPUT;
	}

	ptc_control_ptz_t control_ptz;
	control_ptz.channel_no = channel;
	control_ptz.cmd = cmd;
	control_ptz.param = param;
	TOOL_UINT32 len = sizeof(ptc_control_ptz_t);
	return nc_qh_user_doCfg(dev->ip, dev->msg_port, dev->user, dev->pswd, PTC_CMD_CFG_CONTROL_PTZ, &control_ptz, len, NULL, NULL);

}

TOOL_INT32 nc_qh_getAudioAbility(TOOL_INT32 user_id, ptc_cfg_audio_ability_t* audio_ability)
{
    if (g_nc_qh == NULL)
	{
		log_error("nc");
		return NC_ERROR_UNINIT;
	}
	if (user_id < 0 || user_id >= NC_QH_USER_SIZE)
	{
		log_error("user_id(%d)", user_id);
		return NC_ERROR_INPUT;
	}
	nc_qh_user_t* user = &g_nc_qh->user[user_id];
	ptc_cfg_ability_t ability;
	TOOL_UINT32 len = sizeof(ptc_cfg_ability_t);
	TOOL_INT32 ret = nc_qh_user_doCfg(user->ip_addr, user->port, user->user_name, user->user_pswd, PTC_CMD_CFG_GET_ABILITY, NULL, 0, &ability, &len);
	if (ret < 0)
		return ret;
	tool_mem_memcpy(audio_ability, &ability.audio_ability, sizeof(ptc_cfg_audio_ability_t));
	return 0;	
}

TOOL_INT32 nc_qh_startTalk(TOOL_INT32 user_id, NC_CB cb, TOOL_VOID* param)
{
	if (g_nc_qh == NULL)
	{
		log_error("nc");
		return NC_ERROR_UNINIT;
	}
	if (user_id < 0 || user_id >= NC_QH_USER_SIZE)
	{
		log_error("user_id(%d)", user_id);
		return NC_ERROR_INPUT;
	}
	nc_qh_user_t* user = &g_nc_qh->user[user_id];
	return nc_qh_user_start(g_nc_qh, user->ip_addr, user->port, user->user_name, user->user_pswd, PTC_CMD_TALK_BASE, cb, param, NULL, NULL);
}

TOOL_INT32 nc_qh_startTalk_v2(nc_dev* dev, NC_CB cb, void* param)
{
	if (g_nc_qh == NULL)
	{
		log_error("nc");
		return NC_ERROR_UNINIT;
	}
	if (dev == NULL)
	{
		log_error("dev");
		return NC_ERROR_INPUT;
	}
	return nc_qh_user_start(g_nc_qh, dev->ip, dev->msg_port, dev->user, dev->pswd, PTC_CMD_TALK_BASE, cb, param, NULL, NULL);
}

TOOL_INT32 nc_qh_stopTalk(TOOL_INT32 user_id)
{
	if (g_nc_qh == NULL)
	{
		log_error("nc");
		return NC_ERROR_UNINIT;
	}
	if (user_id < 0 || user_id >= NC_QH_USER_SIZE)
	{
		log_error("user_id(%d)", user_id);
		return NC_ERROR_INPUT;
	}
    nc_qh_user_stop(&g_nc_qh->user[user_id]);
	return 0;
}

TOOL_INT32 nc_qh_sendTalk(TOOL_INT32 user_id, TOOL_UINT8* buf, TOOL_INT32 len)
{
	if (g_nc_qh == NULL)
	{
		log_error("nc");
		return NC_ERROR_UNINIT;
	}
	if (user_id < 0 || user_id >= NC_QH_USER_SIZE || buf == NULL)
	{
		log_error("user_id(%d) buf(0x%x)", user_id, buf);
		return NC_ERROR_INPUT;
	}
        
	tool_stream2_set(g_nc_qh->user[user_id].audio_send, buf, len);
	return 0;
}

TOOL_INT32 nc_qh_getAbility(TOOL_INT32 user_id, ptc_cfg_ability_t* ability)
{
    if (g_nc_qh == NULL)
	{
		log_error("nc");
		return NC_ERROR_UNINIT;
	}
	if (user_id < 0 || user_id >= NC_QH_USER_SIZE || ability == NULL)
	{
		log_error("user_id(%d) ability(0x%x)", user_id, ability);
		return NC_ERROR_INPUT;
	}
	nc_qh_user_t* user = &g_nc_qh->user[user_id];
	TOOL_UINT32 len = sizeof(ptc_cfg_ability_t);
	return nc_qh_user_doCfg(user->ip_addr, user->port, user->user_name, user->user_pswd, PTC_CMD_CFG_GET_ABILITY, NULL, 0, ability, &len);
}

TOOL_INT32 nc_qh_getAbility_v2(nc_dev* dev, ptc_cfg_ability_t* ability)
{
    if (g_nc_qh == NULL)
	{
		log_error("nc");
		return NC_ERROR_UNINIT;
	}
	if (dev == NULL)
	{
		log_error("dev");
		return NC_ERROR_INPUT;
	}
	TOOL_UINT32 len = sizeof(ptc_cfg_ability_t);
	return nc_qh_user_doCfg(dev->ip, dev->msg_port, dev->user, dev->pswd, PTC_CMD_CFG_GET_ABILITY, NULL, 0, ability, &len);
}

TOOL_INT32 nc_qh_getDeviceStatus(TOOL_INT32 user_id, ptc_cfg_status_t* status)
{
	if (g_nc_qh == NULL)
	{
		log_error("nc");
		return NC_ERROR_UNINIT;
	}
	if (user_id < 0 || user_id >= NC_QH_USER_SIZE || status == NULL)
	{
		log_error("user_id(%d) status(0x%x)", user_id, status);
		return NC_ERROR_INPUT;
	}
	nc_qh_user_t* user = &g_nc_qh->user[user_id];
	TOOL_UINT32 len = sizeof(ptc_cfg_status_t);
	return nc_qh_user_doCfg(user->ip_addr, user->port, user->user_name, user->user_pswd, PTC_CMD_CFG_GET_STATUS, NULL, 0, status, &len);
}

TOOL_INT32 nc_qh_getDeviceStatus_v2(nc_dev* dev, ptc_cfg_status_t* status)
{
	if (g_nc_qh == NULL)
	{
		log_error("nc");
		return NC_ERROR_UNINIT;
	}
	if (dev == NULL)
	{
		log_error("dev");
		return NC_ERROR_INPUT;
	}
	TOOL_UINT32 len = sizeof(ptc_cfg_status_t);
	return nc_qh_user_doCfg(dev->ip, dev->msg_port, dev->user, dev->pswd, PTC_CMD_CFG_GET_STATUS, NULL, 0, status, &len);
}

TOOL_INT32 nc_qh_setDeviceStatus(TOOL_INT32 user_id, ptc_cfg_status_t* status)
{
	if (g_nc_qh == NULL)
	{
		log_error("nc");
		return NC_ERROR_UNINIT;
	}
	if (user_id < 0 || user_id >= NC_QH_USER_SIZE || status == NULL)
	{
		log_error("user_id(%d) status(0x%x)", user_id, status);
		return NC_ERROR_INPUT;
	}
	nc_qh_user_t* user = &g_nc_qh->user[user_id];
	TOOL_UINT32 len = sizeof(ptc_cfg_status_t);
	return nc_qh_user_doCfg(user->ip_addr, user->port, user->user_name, user->user_pswd, PTC_CMD_CFG_SET_STATUS, status, len, NULL, NULL);
}

TOOL_INT32 nc_qh_setDeviceStatus_v2(nc_dev* dev, ptc_cfg_status_t* status)
{
	if (g_nc_qh == NULL)
	{
		log_error("nc");
		return NC_ERROR_UNINIT;
	}
	if (dev == NULL)
	{
		log_error("dev");
		return NC_ERROR_INPUT;
	}
	TOOL_UINT32 len = sizeof(ptc_cfg_status_t);
	return nc_qh_user_doCfg(dev->ip, dev->msg_port, dev->user, dev->pswd, PTC_CMD_CFG_SET_STATUS, status, len, NULL, NULL);
}

TOOL_INT32 nc_qh_getCfg(nc_dev* dev, ptc_cfg_t* cfg)
{
	if (g_nc_qh == NULL)
	{
		log_error("nc");
		return NC_ERROR_UNINIT;
	}
	if (cfg == NULL)
	{
		log_error("cfg");
		return NC_ERROR_INPUT;
	}
	TOOL_UINT32 len = 16+cfg->len;
	return nc_qh_user_doSearch(dev->ip, dev->msg_port, dev->user, dev->pswd, PTC_CMD_SEARCH_GET, cfg, len, cfg, &len);
}

TOOL_INT32 nc_qh_getDefaultCfg(nc_dev* dev, ptc_cfg_t* cfg)
{
	if (g_nc_qh == NULL)
	{
		log_error("nc");
		return NC_ERROR_UNINIT;
	}
	if (cfg == NULL)
	{
		log_error("cfg");
		return NC_ERROR_INPUT;
	}
	TOOL_UINT32 len = 16+cfg->len;
	return nc_qh_user_doSearch(dev->ip, dev->msg_port, dev->user, dev->pswd, PTC_CMD_SEARCH_GET_DEFAULT, cfg, len, cfg, &len);
}

TOOL_INT32 nc_qh_setCfg(nc_dev* dev, ptc_cfg_t* cfg)
{
	if (g_nc_qh == NULL)
	{
		log_error("nc");
		return NC_ERROR_UNINIT;
	}
	if (dev == NULL || cfg == NULL)
	{
		log_error("dev(0x%08x) cfg(0x%08x)", dev, cfg);
		return NC_ERROR_INPUT;
	}
	TOOL_UINT32 len = 16+cfg->len;
	return nc_qh_user_doSearch(dev->ip, dev->msg_port, dev->user, dev->pswd, PTC_CMD_SEARCH_SET, cfg, len, NULL, NULL);
}

TOOL_INT32 nc_qh_getCfg_time(nc_dev* dev, ptc_cfg_time_t* cfg_time)
{
	if (g_nc_qh == NULL)
	{
		log_error("nc");
		return NC_ERROR_UNINIT;
	}
	if (dev == NULL || cfg_time == NULL)
	{
		log_error("dev(0x%08x) cfg_time(0x%08x)", dev, cfg_time);
		return NC_ERROR_INPUT;
	}
	TOOL_UINT32 len = sizeof(ptc_cfg_time_t);
	return nc_qh_user_doCfg(dev->ip, dev->msg_port, dev->user, dev->pswd, PTC_CMD_CFG_GET_TIME, NULL, 0, cfg_time, &len);
}

TOOL_INT32 nc_qh_setCfg_time(nc_dev* dev, ptc_cfg_time_t* cfg_time)
{
	if (g_nc_qh == NULL)
	{
		log_error("nc");
		return NC_ERROR_UNINIT;
	}
	if (dev == NULL || cfg_time == NULL)
	{
		log_error("dev(0x%08x) cfg_time(0x%08x)", dev, cfg_time);
		return NC_ERROR_INPUT;
	}
	TOOL_UINT32 len = sizeof(ptc_cfg_time_t);
	return nc_qh_user_doCfg(dev->ip, dev->msg_port, dev->user, dev->pswd, PTC_CMD_CFG_SET_TIME, cfg_time, len, NULL, NULL);	
}

TOOL_INT32 nc_qh_getCfg_alarm(nc_dev* dev, PTC_ALARM_TYPE_E alarm_type, ptc_cfg_alarm_t* cfg_alarm)
{
	if (g_nc_qh == NULL)
	{
		log_error("nc");
		return NC_ERROR_UNINIT;
	}
	if (dev == NULL || cfg_alarm == NULL)
	{
		log_error("dev(0x%08x) cfg_alarm(0x%08x)", dev, cfg_alarm);
		return NC_ERROR_INPUT;
	}

	ptc_msg_t st_msg;
	tool_mem_memset(&st_msg, sizeof(ptc_msg_t));
	st_msg.head.cmd = PTC_CMD_CFG2_GET_ALARM;
	tool_ptc_qh_c2d_encode_getAlarm(&st_msg, alarm_type);
	TOOL_INT32 ret = nc_qh_user_doCfg_v2(dev, &st_msg);
	if (ret < 0)
		return ret;
	ret = tool_ptc_qh_d2c_decode_getAlarm(&st_msg, cfg_alarm);
	if (ret < 0);
		return ret;
	return 0;
}

TOOL_INT32 nc_qh_setCfg_alarm(nc_dev* dev, PTC_ALARM_TYPE_E alarm_type, ptc_cfg_alarm_t* cfg_alarm)
{
	if (g_nc_qh == NULL)
	{
		log_error("nc");
		return NC_ERROR_UNINIT;
	}
	if (dev == NULL || cfg_alarm == NULL)
	{
		log_error("dev(0x%08x) cfg_alarm(0x%08x)", dev, cfg_alarm);
		return NC_ERROR_INPUT;
	}

	ptc_msg_t st_msg;
	tool_mem_memset(&st_msg, sizeof(ptc_msg_t));
	st_msg.head.cmd = PTC_CMD_CFG2_SET_ALARM;
	tool_ptc_qh_c2d_encode_setAlarm(&st_msg, alarm_type, cfg_alarm);
	TOOL_INT32 ret = nc_qh_user_doCfg_v2(dev, &st_msg);
	if (ret < 0)
		return ret;
	ret = tool_ptc_qh_d2c_decode_setAlarm(&st_msg);
	if (ret < 0)
		return ret;
	return 0;	
}

TOOL_INT32 nc_qh_getCfg_workMode(nc_dev* dev, ptc_cfg_workMode_t* cfg_workMode)
{
	if (g_nc_qh == NULL)
	{
		log_error("nc");
		return NC_ERROR_UNINIT;
	}
	if (dev == NULL || cfg_workMode == NULL)
	{
		log_error("dev(0x%08x) cfg_workMode(0x%08x)", dev, cfg_workMode);
		return NC_ERROR_INPUT;
	}

	ptc_msg_t st_msg;
	tool_mem_memset(&st_msg, sizeof(ptc_msg_t));
	st_msg.head.cmd = PTC_CMD_CFG2_GET_WORKMODE;
	tool_ptc_qh_c2d_encode_getWorkMode(&st_msg);
	TOOL_INT32 ret = nc_qh_user_doCfg_v2(dev, &st_msg);
	if (ret < 0)
		return ret;
	ret = tool_ptc_qh_d2c_decode_getWorkMode(&st_msg, cfg_workMode);
	if (ret < 0)
		return ret;
	return 0;
}

TOOL_INT32 nc_qh_setCfg_workMode(nc_dev* dev, ptc_cfg_workMode_t* cfg_workMode)
{
	if (g_nc_qh == NULL)
	{
		log_error("nc");
		return NC_ERROR_UNINIT;
	}
	if (dev == NULL || cfg_workMode == NULL)
	{
		log_error("dev(0x%08x) cfg_workMode(0x%08x)", dev, cfg_workMode);
		return NC_ERROR_INPUT;
	}

	ptc_msg_t st_msg;
	tool_mem_memset(&st_msg, sizeof(ptc_msg_t));
	st_msg.head.cmd = PTC_CMD_CFG2_SET_WORKMODE;
	tool_ptc_qh_c2d_encode_setWorkMode(&st_msg, cfg_workMode);
	TOOL_INT32 ret = nc_qh_user_doCfg_v2(dev, &st_msg);
	if (ret < 0)
		return ret;
	ret = tool_ptc_qh_d2c_decode_setWorkMode(&st_msg);
	if (ret < 0)
		return ret;
	return 0;	
}

TOOL_INT32 nc_qh_getCfg_mobile(nc_dev* dev, ptc_cfg_mobile_t* cfg_mobile)
{
	if (g_nc_qh == NULL)
	{
		log_error("nc");
		return NC_ERROR_UNINIT;
	}
	if (dev == NULL || cfg_mobile == NULL)
	{
		log_error("dev(0x%08x) cfg_mobile(0x%08x)", dev, cfg_mobile);
		return NC_ERROR_INPUT;
	}

	ptc_msg_t st_msg;
	tool_mem_memset(&st_msg, sizeof(ptc_msg_t));
	st_msg.head.cmd = PTC_CMD_CFG2_GET_MOBILE;
	tool_ptc_qh_c2d_encode_getMobile(&st_msg);
	TOOL_INT32 ret = nc_qh_user_doCfg_v2(dev, &st_msg);
	if (ret < 0)
		return ret;
	ret = tool_ptc_qh_d2c_decode_getMobile(&st_msg, cfg_mobile);
	if (ret < 0)
		return ret;
	return 0;
}

TOOL_INT32 nc_qh_setCfg_mobile(nc_dev* dev, ptc_cfg_mobile_t* cfg_mobile)
{
	if (g_nc_qh == NULL)
	{
		log_error("nc");
		return NC_ERROR_UNINIT;
	}
	if (dev == NULL || cfg_mobile == NULL)
	{
		log_error("dev(0x%08x) cfg_mobile(0x%08x)", dev, cfg_mobile);
		return NC_ERROR_INPUT;
	}

	ptc_msg_t st_msg;
	tool_mem_memset(&st_msg, sizeof(ptc_msg_t));
	st_msg.head.cmd = PTC_CMD_CFG2_SET_MOBILE;
	tool_ptc_qh_c2d_encode_setMobile(&st_msg, cfg_mobile);
	TOOL_INT32 ret = nc_qh_user_doCfg_v2(dev, &st_msg);
	if (ret < 0)
		return ret;
	ret = tool_ptc_qh_d2c_decode_setMobile(&st_msg);
	if (ret < 0)
		return ret;
	return 0;	
}

TOOL_INT32 nc_qh_getCfg_wifi(nc_dev* dev, ptc_cfg_wifi_t* cfg_wifi)
{
	if (g_nc_qh == NULL)
	{
		log_error("nc");
		return NC_ERROR_UNINIT;
	}
	if (dev == NULL || cfg_wifi == NULL)
	{
		log_error("dev(0x%08x) cfg_wifi(0x%08x)", dev, cfg_wifi);
		return NC_ERROR_INPUT;
	}

	ptc_msg_t st_msg;
	tool_mem_memset(&st_msg, sizeof(ptc_msg_t));
	st_msg.head.cmd = PTC_CMD_CFG2_GET_WIFI;
	tool_ptc_qh_c2d_encode_getWifi(&st_msg);
	TOOL_INT32 ret = nc_qh_user_doCfg_v2(dev, &st_msg);
	if (ret < 0)
		return ret;
	ret = tool_ptc_qh_d2c_decode_getWifi(&st_msg, cfg_wifi);
	if (ret < 0)
		return ret;
	return 0;
}

TOOL_INT32 nc_qh_setCfg_wifi(nc_dev* dev, ptc_cfg_wifi_t* cfg_wifi)
{
	if (g_nc_qh == NULL)
	{
		log_error("nc");
		return NC_ERROR_UNINIT;
	}
	if (dev == NULL || cfg_wifi == NULL)
	{
		log_error("dev(0x%08x) cfg_wifi(0x%08x)", dev, cfg_wifi);
		return NC_ERROR_INPUT;
	}
	ptc_msg_t st_msg;
	tool_mem_memset(&st_msg, sizeof(ptc_msg_t));
	st_msg.head.cmd = PTC_CMD_CFG2_SET_WIFI;
	tool_ptc_qh_c2d_encode_setWifi(&st_msg, cfg_wifi);
	TOOL_INT32 ret = nc_qh_user_doCfg_v2(dev, &st_msg);
	if (ret < 0)
		return ret;
	ret = tool_ptc_qh_d2c_decode_setWifi(&st_msg);
	if (ret < 0)
		return ret;
	return 0;	
}

TOOL_INT32 nc_qh_getCfg_devRecord(nc_dev* dev, ptc_cfg_devRecord_t* cfg_devRecord)
{
	if (g_nc_qh == NULL)
	{
		log_error("nc");
		return NC_ERROR_UNINIT;
	}
	if (dev == NULL || cfg_devRecord == NULL)
	{
		log_error("dev(0x%08x) cfg_devRecord(0x%08x)", dev, cfg_devRecord);
		return NC_ERROR_INPUT;
	}

	ptc_msg_t st_msg;
	tool_mem_memset(&st_msg, sizeof(ptc_msg_t));
	st_msg.head.cmd = PTC_CMD_CFG2_GET_DEVRECORD;
	tool_ptc_qh_c2d_encode_getDevRecord(&st_msg);
	TOOL_INT32 ret = nc_qh_user_doCfg_v2(dev, &st_msg);
	if (ret < 0)
		return ret;
	ret = tool_ptc_qh_d2c_decode_getDevRecord(&st_msg, cfg_devRecord);
	if (ret < 0)
		return ret;
	return 0;
}

TOOL_INT32 nc_qh_setCfg_devRecord(nc_dev* dev, ptc_cfg_devRecord_t* cfg_devRecord)
{
	if (g_nc_qh == NULL)
	{
		log_error("nc");
		return NC_ERROR_UNINIT;
	}
	if (dev == NULL || cfg_devRecord == NULL)
	{
		log_error("dev(0x%08x) cfg_devRecord(0x%08x)", dev, cfg_devRecord);
		return NC_ERROR_INPUT;
	}

	ptc_msg_t st_msg;
	tool_mem_memset(&st_msg, sizeof(ptc_msg_t));
	st_msg.head.cmd = PTC_CMD_CFG2_SET_DEVRECORD;
	tool_ptc_qh_c2d_encode_setDevRecord(&st_msg, cfg_devRecord);
	TOOL_INT32 ret = nc_qh_user_doCfg_v2(dev,&st_msg);
	if (ret < 0)
		return ret;
	ret = tool_ptc_qh_d2c_decode_setDevRecord(&st_msg);
	if (ret < 0)
		return ret;
	return 0;	
}

TOOL_INT32 nc_qh_getCfg_all(nc_dev* dev, TOOL_INT32 id, TOOL_VOID* cfg_all)
{
	if (g_nc_qh == NULL)
	{
		log_error("nc");
		return NC_ERROR_UNINIT;
	}
	if (dev == NULL || id < 0 || id >= NC_QH_CFG_NUM || cfg_all == NULL)
	{
		log_error("dev(0x%08x) id(%d) cfg_all(0x%08x)", dev, id, cfg_all);
		return NC_ERROR_INPUT;
	}


	ptc_msg_t st_msg;
	tool_mem_memset(&st_msg, sizeof(ptc_msg_t));
	//////////////////////
	if (id == NC_QH_CFG_AV)
	{
		st_msg.head.cmd = PTC_CMD_CFG2_GET_AV;
		tool_ptc_qh_c2d_encode_getAv(&st_msg);
	}
	else if (id == NC_QH_CFG_WIFI)
	{
		st_msg.head.cmd = PTC_CMD_CFG2_GET_WIFI;
		tool_ptc_qh_c2d_encode_getWifi(&st_msg);
	}
	else if (id == NC_QH_CFG_DEVRECORD)
	{
		st_msg.head.cmd = PTC_CMD_CFG2_GET_DEVRECORD;
		tool_ptc_qh_c2d_encode_getDevRecord(&st_msg);
	}
	else if (id == NC_QH_CFG_WORKMODE)
	{
		st_msg.head.cmd = PTC_CMD_CFG2_GET_WORKMODE;
		tool_ptc_qh_c2d_encode_getWorkMode(&st_msg);
	}
	else
	{
		log_error("id(%d)", id);
		return NC_ERROR_INTERNAL;
	}
	
	TOOL_INT32 ret = nc_qh_user_doCfg_v2(dev, &st_msg);
	if (ret < 0)
		return ret;

	//////////////////////
	if (id == NC_QH_CFG_AV)
	{
		ret = tool_ptc_qh_d2c_decode_getAv(&st_msg, (ptc_cfg_av_t *)cfg_all);
	}
	else if (id == NC_QH_CFG_WIFI)
	{
		ret = tool_ptc_qh_d2c_decode_getWifi(&st_msg, (ptc_cfg_wifi_t *)cfg_all);
	}
	else if (id == NC_QH_CFG_DEVRECORD)
	{
		ret = tool_ptc_qh_d2c_decode_getDevRecord(&st_msg, (ptc_cfg_devRecord_t *)cfg_all);
	}
	else if (id == NC_QH_CFG_WORKMODE)
	{
		ret = tool_ptc_qh_d2c_decode_getWorkMode(&st_msg, (ptc_cfg_workMode_t *)cfg_all);
	}
	else
	{
		log_error("id(%d)", id);
		return NC_ERROR_INTERNAL;
	}
	
	if (ret < 0)
		return ret;
	return 0;
}

TOOL_INT32 nc_qh_setCfg_all(nc_dev* dev, TOOL_INT32 id, TOOL_VOID* cfg_all)
{
	if (g_nc_qh == NULL)
	{
		log_error("nc");
		return NC_ERROR_UNINIT;
	}
	if (dev == NULL || id < 0 || id >= NC_QH_CFG_NUM || cfg_all == NULL)
	{
		log_error("dev(0x%08x) id(%d) cfg_all(0x%08x)", dev, id, cfg_all);
		return NC_ERROR_INPUT;
	}

	ptc_msg_t st_msg;
	tool_mem_memset(&st_msg, sizeof(ptc_msg_t));
	//////////////////////
	if (id == NC_QH_CFG_AV)
	{
		st_msg.head.cmd = PTC_CMD_CFG2_SET_AV;
		tool_ptc_qh_c2d_encode_setAv(&st_msg, (ptc_cfg_av_t *)cfg_all);
	}
	else if (id == NC_QH_CFG_WIFI)
	{
		st_msg.head.cmd = PTC_CMD_CFG2_SET_WIFI;
		tool_ptc_qh_c2d_encode_setWifi(&st_msg, (ptc_cfg_wifi_t *)cfg_all);
	}
	else if (id == NC_QH_CFG_DEVRECORD)
	{
		st_msg.head.cmd = PTC_CMD_CFG2_SET_DEVRECORD;
		tool_ptc_qh_c2d_encode_setDevRecord(&st_msg, (ptc_cfg_devRecord_t *)cfg_all);
	}
	else if (id == NC_QH_CFG_WORKMODE)
	{
		st_msg.head.cmd = PTC_CMD_CFG2_SET_WORKMODE;
		tool_ptc_qh_c2d_encode_setWorkMode(&st_msg, (ptc_cfg_workMode_t *)cfg_all);
	}
	else
	{
		log_error("id(%d)", id);
		return NC_ERROR_INTERNAL;
	}
	
	TOOL_INT32 ret = nc_qh_user_doCfg_v2(dev,&st_msg);
	if (ret < 0)
		return ret;

	//////////////////////
	if (id == NC_QH_CFG_AV)
	{
		ret = tool_ptc_qh_d2c_decode_setAv(&st_msg);
	}
	else if (id == NC_QH_CFG_WIFI)
	{
		ret = tool_ptc_qh_d2c_decode_setWifi(&st_msg);
	}
	else if (id == NC_QH_CFG_DEVRECORD)
	{
		ret = tool_ptc_qh_d2c_decode_setDevRecord(&st_msg);
	}
	else if (id == NC_QH_CFG_WORKMODE)
	{
		ret = tool_ptc_qh_d2c_decode_setWorkMode(&st_msg);
	}
	else
	{
		log_error("id(%d)", id);
		return NC_ERROR_INTERNAL;
	}
	
	if (ret < 0)
		return ret;
	return 0;	
}



/*
同步IPC时间: 
	将UTC+TimeZone+DST时间设置为设备UTC时间，设备使用假的UTC时间；
	记录TimeZone和DST，在使用时间时做相应的偏移；
同步DVR时间: 
	将UTC+TimeZone+DST时间设置进入设备，设备自动识别TimeZone和DST，将正确的UTC时间设置设备时间
	将TimeZone设置进入设备，设备自行生效；
	将DST设置进入设备，设备自行生效；
 */
TOOL_INT32 nc_qh_syncTime(nc_dev* dev, TOOL_UINT32 utc_sec, TOOL_INT32 zone, TOOL_INT32 dst_hour)
{
	if (g_nc_qh == NULL)
	{
		log_error("nc");
		return NC_ERROR_UNINIT;
	}
	if (dev == NULL || zone < PTC_TIME_ZONE_W1200 || zone > PTC_TIME_ZONE_E1300 || dst_hour < 0 || dst_hour > 3)
	{
		log_error("dev(0x%08x) zone(%d) dst_hour(%d)", dev, zone, dst_hour);
		return NC_ERROR_INPUT;
	}
	ptc_cfg_time_t cfg_time;
	TOOL_INT32 ret = nc_qh_getCfg_time(dev, &cfg_time);
	if (ret < 0)
		return ret;
/*
	log_debug("(%d,%d,%s,%d,%d,%d) (%d,%d,%d,%d,%d,%d,%d,%d-%d,%d,%d,%d,%d,%d,%d,%d) (%d,%d,%d,%d,%d,%d)", 
		cfg_time.utc_sec, cfg_time.zone, cfg_time.ntp_ip, cfg_time.ntp_port, cfg_time.ntp_enable, cfg_time.dst_enable, 
		cfg_time.start_year, cfg_time.start_month, cfg_time.start_day, cfg_time.start_weekinMonth, cfg_time.start_dayofWeek, cfg_time.start_hour, cfg_time.start_min, cfg_time.start_sec, 
		cfg_time.end_year, cfg_time.end_month, cfg_time.end_day, cfg_time.end_weekinMonth, cfg_time.end_dayofWeek, cfg_time.end_hour, cfg_time.end_min, cfg_time.end_sec, 
		cfg_time.shift_hour, cfg_time.status, cfg_time.dst_hour, cfg_time.dst_mode, cfg_time.date_mode, cfg_time.time_mode);
*/	
	cfg_time.utc_sec = utc_sec;
	cfg_time.zone = zone;
	if (dst_hour == 0)
	{
		cfg_time.dst_enable = PTC_ENABLE_FLASE;
		cfg_time.dst_hour = 0;
	}
	else
	{
		cfg_time.dst_enable = PTC_ENABLE_TRUE;
		cfg_time.dst_mode = PTC_TIME_DSTMODE_DATE;
		cfg_time.start_month = 1;
		cfg_time.start_day = 1;
		cfg_time.start_weekinMonth = 1;
		cfg_time.start_dayofWeek = 0;
		cfg_time.start_hour = 0;
		cfg_time.start_min = 0;
		cfg_time.start_sec = 0;
		cfg_time.end_month = 12;
		cfg_time.end_day = 31;
		cfg_time.end_weekinMonth = 5;
		cfg_time.end_dayofWeek = 6;
		cfg_time.end_hour = 23;
		cfg_time.end_min = 59;
		cfg_time.end_sec = 59;
		cfg_time.shift_hour = dst_hour;
		cfg_time.dst_hour = dst_hour;
	}

	return nc_qh_setCfg_time(dev, &cfg_time);
}

TOOL_INT32 nc_qh_searchDates(nc_dev* dev, ptc_cfg_dates_t* dates)
{
	if (g_nc_qh == NULL)
	{
		log_error("nc");
		return NC_ERROR_UNINIT;
	}
	if (dev == NULL || dates == NULL)
	{
		log_error("dev(0x%x) dates(0x%x)", dev, dates);
		return NC_ERROR_INPUT;
	}

	ptc_frame_t* pst_frame = (ptc_frame_t*)tool_mem_malloc(sizeof(ptc_frame_t), 0);
	tool_mem_memset(pst_frame, sizeof(ptc_head_t));
	pst_frame->head.cmd = PTC_CMD_SEARCH2_DATES;
	tool_ptc_qh_c2d_encode_searchDates(pst_frame, dates);
	TOOL_INT32 ret = nc_qh_user_doSearch2(dev, pst_frame);
	if (ret < 0)
		return ret;
	ret = tool_ptc_qh_d2c_decode_searchDates(pst_frame, dates);
	if (ret < 0);
		return ret;
	return 0;
}

TOOL_INT32 nc_qh_searchRecords(nc_dev* dev, ptc_cfg_records_t* records)
{
	if (g_nc_qh == NULL)
	{
		log_error("nc");
		return NC_ERROR_UNINIT;
	}
	if (dev == NULL || records == NULL)
	{
		log_error("dev(0x%x) records(0x%x)", dev, records);
		return NC_ERROR_INPUT;
	}

	TOOL_UINT32 in_len = 32;
	TOOL_UINT32 out_len = sizeof(ptc_cfg_records_t);
	return nc_qh_user_doSearch(dev->ip, dev->msg_port, dev->user, dev->pswd, PTC_CMD_SEARCH_RECORDS, records, in_len, records, &out_len);
}

TOOL_INT32 nc_qh_startPbs(nc_dev* dev, ptc_cfg_record_t* record, NC_CB cb, void* param)
{
	if (g_nc_qh == NULL)
	{
		log_error("nc");
		return NC_ERROR_UNINIT;
	}
	if (dev == NULL || record == NULL)
	{
		log_error("dev(0x%x) record(0x%x)", dev, record);
		return NC_ERROR_INPUT;
	}
	
	return nc_qh_user_start(g_nc_qh, dev->ip, dev->msg_port, dev->user, dev->pswd, PTC_CMD_PBS_BASE, cb, param, NULL, record);
}

TOOL_INT32 nc_qh_setPbsPos(TOOL_INT32 user_id, TOOL_UINT32 pos)
{
	if (g_nc_qh == NULL)
	{
		log_error("nc");
		return NC_ERROR_UNINIT;
	}
	if (user_id < 0 || user_id >= NC_QH_USER_SIZE)
	{
		log_error("user_id(%d)", user_id);
		return NC_ERROR_INPUT;
	}
	nc_qh_user_t* user = (nc_qh_user_t*)&g_nc_qh->user[user_id];
	tool_thread_lockMutex(&user->mutex);
	user->pbs_pos = pos;
	tool_thread_unlockMutex(&user->mutex);
	return 0;	
}

TOOL_INT32 nc_qh_pausePbs(TOOL_INT32 user_id)
{
	if (g_nc_qh == NULL)
	{
		log_error("nc");
		return NC_ERROR_UNINIT;
	}
	if (user_id < 0 || user_id >= NC_QH_USER_SIZE)
	{
		log_error("user_id(%d)", user_id);
		return NC_ERROR_INPUT;
	}
	nc_qh_user_t* user = (nc_qh_user_t*)&g_nc_qh->user[user_id];
	tool_thread_lockMutex(&user->mutex);
	user->pbs_pause = 1;
	tool_thread_unlockMutex(&user->mutex);
	return 0;
}

TOOL_INT32 nc_qh_restartPbs(TOOL_INT32 user_id)
{
	if (g_nc_qh == NULL)
	{
		log_error("nc");
		return NC_ERROR_UNINIT;
	}
	if (user_id < 0 || user_id >= NC_QH_USER_SIZE)
	{
		log_error("user_id(%d)", user_id);
		return NC_ERROR_INPUT;
	}
	nc_qh_user_t* user = (nc_qh_user_t*)&g_nc_qh->user[user_id];
	tool_thread_lockMutex(&user->mutex);
	user->pbs_restart = 1;
	tool_thread_unlockMutex(&user->mutex);
	return 0;
}

TOOL_INT32 nc_qh_setPbsSpeed(TOOL_INT32 user_id, TOOL_INT32 speed)
{
	if (g_nc_qh == NULL)
	{
		log_error("nc");
		return NC_ERROR_UNINIT;
	}
	if (user_id < 0 || user_id >= NC_QH_USER_SIZE)
	{
		log_error("user_id(%d)", user_id);
		return NC_ERROR_INPUT;
	}
	if (speed < -3 || speed > 3)
	{
		log_error("speed(%d)", speed);
		return NC_ERROR_INPUT;
	}
	nc_qh_user_t* user = (nc_qh_user_t*)&g_nc_qh->user[user_id];
	tool_thread_lockMutex(&user->mutex);
	user->pbs_speed = speed;
	tool_thread_unlockMutex(&user->mutex);
	return 0;

}

TOOL_INT32 nc_qh_stopPbs(TOOL_INT32 user_id)
{
	if (g_nc_qh == NULL)
	{
		log_error("nc");
		return NC_ERROR_UNINIT;
	}
	if (user_id < 0 || user_id >= NC_QH_USER_SIZE)
	{
		log_error("user_id(%d)", user_id);
		return NC_ERROR_INPUT;
	}
    nc_qh_user_stop(&g_nc_qh->user[user_id]);
	return 0;
}

TOOL_INT32 nc_qh_searchLogs(nc_dev* dev, ptc_logs_t* logs)
{
	if (g_nc_qh == NULL)
	{
		log_error("nc");
		return NC_ERROR_UNINIT;
	}
	if (dev == NULL || logs == NULL)
	{
		log_error("dev(0x%x) logs(0x%x)", dev, logs);
		return NC_ERROR_INPUT;
	}
	TOOL_UINT32 in_len = 16;
	TOOL_UINT32 out_len = sizeof(ptc_logs_t);
	TOOL_INT32 ret = nc_qh_user_doSearch(dev->ip, dev->msg_port, dev->user, dev->pswd, PTC_CMD_SEARCH_LOGS, logs, in_len, logs, &out_len);
	if (ret < 0)
		return ret;
	TOOL_INT32 i = 0;
	for (i = 0; i < logs->num; i++)
		logs->log[i].sub_type2 = logs->log[i].sub_type;
	return ret;
}

TOOL_INT32 nc_qh_sendData(nc_dev* dev, TOOL_VOID* in_data, TOOL_UINT32 in_len, TOOL_VOID* out_data, TOOL_UINT32* out_len)
{
	if (g_nc_qh == NULL)
	{
		log_error("nc");
		return NC_ERROR_UNINIT;
	}
	if (dev == NULL || in_len > PTC_VIDEO_FRAME_SIZE)
	{
		log_error("dev(0x%x) in_len(%d)", dev, PTC_VIDEO_FRAME_SIZE);
		return NC_ERROR_INPUT;
	}

	return nc_qh_user_doSearch(dev->ip, dev->msg_port, dev->user, dev->pswd, PTC_CMD_SEARCH_DATA, in_data, in_len, out_data, out_len);	
}

#if defined __cplusplus
}
#endif

