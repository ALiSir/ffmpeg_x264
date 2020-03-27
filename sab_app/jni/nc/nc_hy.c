
#if defined __cplusplus
extern "C"
{
#endif

#include "nc.h"
#include "qh_ptc.h"
#include "nc_hy.h"
#include "hy_ptc.h"
#include "tool_log.h"
#include "tool_type.h"
#include "tool_sysf.h"
#include "tool_stream2.h"
#include "tool_stream3.h"
#include "tool_sock.h"
#include "tool_ptc.h"

#define NC_HY_USER_SIZE				10
#define	NC_HY_HEARTBEAT_CYCLE		5
#define	NC_HY_RECONNECT_CYCLE		5
#define NC_HY_CHANNEL_SIZE			16
#define NC_HY_STREAM_SIZE			2

#define NC_HY_RTS_POOL_SIZE			(4*1024*1024)
#define NC_HY_PBS_POOL_SIZE			(10*1024*1024)


typedef struct
{
	TOOL_INT32 channel_no;
	TOOL_INT32 stream_type;
	TOOL_INT32 state;			//0=offline,1=online,2=recv
	TOOL_INT32 active;
	TOOL_THREAD recv_pid;
	TOOL_THREAD play_pid;	
	TOOL_VOID* father_dev;
	
	tool_sock_item_t rts_sock;
	tool_stream3_t stream;
	tool_stream3_pos_t read_pos;
	TOOL_UINT8* recv_frame;
	TOOL_UINT8* cb_frame;
//	TOOL_MUTEX mutex;		// for msg_sock
	TOOL_INT32 error_flag;

	NC_CB cb;
	TOOL_VOID* param;	
}nc_hy_rts_t;

typedef struct
{
	TOOL_INT32 pbs_no;
	ptc_cfg_record_t ptc_record;
	TOOL_INT32 state;			//0=offline,1=online,2=recv
	TOOL_INT32 active;
	TOOL_THREAD play_pid;
	TOOL_THREAD recv_pid;
	TOOL_VOID* father_dev;
	
	tool_sock_item_t pbs_sock;
	tool_stream3_t stream;
	tool_stream3_pos_t read_pos;
	TOOL_UINT8* recv_frame;
	TOOL_UINT32 len;
	TOOL_UINT8* cb_frame;
	TOOL_UINT8* left_buf;
	TOOL_UINT32 left_len;
	TOOL_UINT32 full_flag;
//	TOOL_MUTEX mutex;		// for msg_sock
	TOOL_INT32 key_flag;
	TOOL_INT32 error_flag;
	TOOL_INT32 history_pos;
	TOOL_INT32 start_flag;

	NC_CB cb;
	TOOL_VOID* param;	
}nc_hy_pbs_t;

typedef struct
{
	TOOL_INT32 alarm_no;
	TOOL_INT32 state;			//0=offline,1=online
	TOOL_VOID* father_dev;
}nc_hy_alarm_t;

typedef struct 
{
	TOOL_INT32 dev_id;		//*
	TOOL_VOID* father_nc;	//*
	nc_hy_rts_t rts[NC_HY_CHANNEL_SIZE];
	nc_hy_pbs_t pbs;
	nc_hy_alarm_t alarm;
	TOOL_MUTEX mutex;		// for msg_sock

	TOOL_INT32 state;		//*
	TOOL_INT32 online;		
	TOOL_THREAD login_pid;
	TOOL_INT8 ip_addr[PTC_IP_SIZE];		//*
	TOOL_INT32 port;					//*
	TOOL_INT8 user_name[PTC_ID_SIZE];	//*
	TOOL_INT8 user_pswd[PTC_ID_SIZE];	//*
	TOOL_INT32 state_tick_sec;	
	TOOL_INT32 state_tick_usec;

	tool_sock_item_t msg_sock;			//*
	hy_msg_t hy_msg;					//*

	hy_msg_login0_d2c_t login0_d2c;
	TOOL_INT32 hb_tick_sec;	
	TOOL_INT32 hb_tick_usec;	
	TOOL_INT32 is_sleep;

	//rts
	TOOL_INT32 cur_rts_enable[NC_HY_CHANNEL_SIZE];
	TOOL_INT32 cur_rts_stream_type[NC_HY_CHANNEL_SIZE];

	//pbs
	TOOL_INT32 cur_pbs_enable;
	TOOL_INT32 pbs_pause;

	//ptz
	TOOL_UINT8 last_ptz_channel_no;
	TOOL_UINT8 last_ptz_cmd;
	TOOL_UINT8 last_ptz_param;
	TOOL_UINT8 last_ptz_step;
	TOOL_UINT8 cur_ptz_channel_no;
	TOOL_UINT8 cur_ptz_cmd;
	TOOL_UINT8 cur_ptz_param;
	TOOL_UINT8 cur_ptz_step;	
	TOOL_INT32 ptz_tick_sec;	
	TOOL_INT32 ptz_tick_usec;

	//talk
	TOOL_INT32 cur_talk_enable;
	TOOL_INT32 last_talk_enable;
	TOOL_INT32 talk_no;
	tool_stream2_t* talk_send_stream;
	TOOL_UINT32 talk_send_pos;
	TOOL_UINT8 talk_send_frame[PTC_AUDIO_FRAME_SIZE];
	TOOL_UINT32 talk_send_len;
	NC_CB talk_cb;
	TOOL_VOID* talk_param;

	//alarm
	TOOL_INT32 cur_alarm_enable;
	NC_CB alarm_cb;
	TOOL_VOID* alarm_param;

	//time
	TOOL_UINT32 last_utc_sec;
	TOOL_UINT32 cur_utc_sec;

	//cfg-records
	TOOL_INT32 last_records_req;
	TOOL_INT32 cur_records_req;
	ptc_cfg_records_t cfg_records;
	TOOL_INT32 result_records;
	TOOL_COND cond_records;
	//cfg-logs
	TOOL_INT32 last_logs_req;
	TOOL_INT32 cur_logs_req;
	ptc_logs_t cfg_logs;
	TOOL_INT32 result_logs;
	TOOL_COND cond_logs;
	//cfg-status set
	TOOL_INT32 last_set_status_req;
	TOOL_INT32 cur_set_status_req;
	ptc_cfg_status_t cfg_set_status;
	//cfg-status get
	TOOL_INT32 last_get_status_req;
	TOOL_INT32 cur_get_status_req;
	ptc_cfg_status_t cfg_get_status;
	TOOL_INT32 result_status;
	TOOL_COND cond_status;
}nc_hy_dev_t;

typedef struct
{
	nc_hy_dev_t dev[NC_HY_USER_SIZE];
	TOOL_MUTEX mutex;

	TOOL_INT32 state;
	TOOL_THREAD pid;

	TOOL_INT32 buffer_usec;
}nc_hy_t;

nc_hy_t* g_nc_hy = NULL;



#define __NC_HY_RTS__


	
#define __NC_HY_DEV__

TOOL_VOID nc_hy_dev_doAlarm(nc_hy_dev_t* dev)
{
/*	
	TOOL_INT32 alarm_enable;
	NC_CB alarm_cb;
	TOOL_VOID* alarm_param;
	
	tool_thread_lockMutex(&dev->mutex);
	alarm_enable = dev->cur_alarm_enable;
	alarm_cb = dev->alarm_cb;
	alarm_param = dev->alarm_param;
	tool_thread_unlockMutex(&dev->mutex);
	if (alarm_enable == 0)
		return ;

	TOOL_INT32 id = (NC_PTC_HuaYi | (dev->dev_id<<8) | dev->alarm.alarm_no);
	ptc_log_t log;
	tool_mem_memset(&log, sizeof(ptc_log_t));
	log.time = ;
	log.type = ;
	if (alarm_cb)
		alarm_cb(id, &log, );
*/
}

TOOL_INT32 nc_hy_dev_recv(nc_hy_dev_t* dev, TOOL_INT32 cmd)
{
	TOOL_INT32 ret = 0;
	while (1)
	{
		ret = tool_ptc_hy_recvMsg(&dev->msg_sock, &dev->hy_msg);
		if (ret < 0)
			break;
		if (dev->hy_msg.head.cmd == HY_CMD_ALARM)
		{
			log_state("HY_CMD_ALARM");
			nc_hy_dev_doAlarm(dev);
		}
		
		if (dev->hy_msg.head.cmd == cmd || cmd == HY_CMD_ALL)
			break;		
		log_state("invalid cmd(0x%02x) cmd(0x%02x)", dev->hy_msg.head.cmd, cmd);
	}
	return ret;
}


TOOL_VOID nc_hy_dev_init(nc_hy_dev_t* dev, TOOL_INT32 dev_id, TOOL_VOID* father_nc)
{
	dev->dev_id = dev_id;
	dev->father_nc = father_nc;
	tool_thread_initMutex(&dev->mutex);
	tool_sock_item_setFlag(&dev->msg_sock, &dev->state);
	
	//rts
	TOOL_INT32 i = 0;
	for (i = 0; i < NC_HY_CHANNEL_SIZE; i++)
	{
		dev->rts[i].channel_no = i;
		dev->rts[i].father_dev = dev;
		tool_sock_item_setFlag(&dev->rts[i].rts_sock, &dev->rts[i].state);		
	}
	//pbs
	dev->pbs.father_dev = dev;
	dev->pbs.pbs_no = NC_HY_CHANNEL_SIZE;
	tool_sock_item_setFlag(&dev->pbs.pbs_sock, &dev->pbs.state);			
	//talk
	dev->talk_no = NC_HY_CHANNEL_SIZE*2;
	//alarm
	dev->alarm.father_dev = dev;
	dev->alarm.alarm_no = NC_HY_CHANNEL_SIZE*3;
	//cfg
	tool_thread_initCond(&dev->cond_records);
	tool_thread_initCond(&dev->cond_logs);
	tool_thread_initCond(&dev->cond_status);
}

TOOL_VOID nc_hy_dev_done(nc_hy_dev_t* dev)
{
	tool_thread_doneMutex(&dev->mutex);
	TOOL_INT32 i = 0;
	for (i = 0; i < NC_HY_CHANNEL_SIZE; i++)
	{
	}
	//pbs

	//cfg
	tool_thread_doneCond(&dev->cond_records);
	tool_thread_doneCond(&dev->cond_logs);
	tool_thread_doneCond(&dev->cond_status);
}

TOOL_VOID nc_hy_rts_dostate(nc_hy_rts_t* rts, TOOL_INT32 type)
{
	ptc_frame_head_t head;
	tool_mem_memset(&head, sizeof(ptc_frame_head_t));
	head.frame_type = type;
//	log_debug("type(0x%08x)", type);
	tool_stream3_set_noDrop(&rts->stream, &head, sizeof(ptc_frame_head_t), NULL);	
}

TOOL_VOID nc_hy_pbs_doState(nc_hy_pbs_t* pbs, TOOL_INT32 type)
{
	ptc_frame_head_t head;
	tool_mem_memset(&head, sizeof(ptc_frame_head_t));
	head.frame_type = type;
//	log_debug("type(0x%08x)", type);
	tool_stream3_set_noDrop(&pbs->stream, &head, sizeof(ptc_frame_head_t), NULL);
}

TOOL_VOID* nc_hy_dev_runRecvRts(TOOL_VOID* param)
{
	nc_hy_rts_t* rts = (nc_hy_rts_t*)param;
	nc_hy_dev_t* dev = (nc_hy_dev_t*)rts->father_dev;
	nc_hy_t* nc = (nc_hy_t*)dev->father_nc;
	log_state("dev(%d) RecvRts start", dev->dev_id);
	TOOL_INT32 ret = 0;
	TOOL_UINT32 len = 0;
	ptc_frame_head_t* ptc_frame = NULL;
	TOOL_INT32 ch = rts->channel_no;
	TOOL_INT32 dev_online = 0;

	while (rts->state)
	{
		if (dev_online != dev->online)
		{
			dev_online = dev->online;
			if (dev->online)
				nc_hy_rts_dostate(rts, dev_online);
		}
		
		if (rts->active == 0)
		{
			tool_sysf_usleep(20*1000);
			continue;
		}
		
		ret = tool_sock_item_pollRead(&dev->rts[ch].rts_sock, 0);
		if (ret < 0)
		{
			log_error("tool_sock_item_pollRead error");
			rts->error_flag = 1;
			break;
		}
		else if (ret == 0)
		{
			tool_sysf_usleep(20*1000);
			continue;
		}

		len = PTC_VIDEO_FRAME_SIZE-sizeof(ptc_frame_head_t);
		ret = tool_ptc_hy_recvRts(&dev->rts[ch].rts_sock, dev->rts[ch].recv_frame+sizeof(ptc_frame_head_t), &len, &ptc_frame);
		if (ret < 0)
		{
			log_error("tool_ptc_hy_recvRts error");
			rts->error_flag = 1;
			break;
		}
		else if (ret == 0)
			continue;
		
		tool_stream3_set_withDrop(&dev->rts[ch].stream, ptc_frame, len, nc->buffer_usec);
	}

	log_state("dev(%d) RecvRts stop", dev->dev_id);
    return NULL;	
}

TOOL_VOID* nc_hy_dev_runPlayRts(TOOL_VOID* param)
{
log_debug("");
	nc_hy_rts_t* rts = (nc_hy_rts_t*)param;log_debug("");
	nc_hy_dev_t* dev = (nc_hy_dev_t*)rts->father_dev;log_debug("");
	log_state("dev(%d) CbRts(%d)(%d) start", dev->dev_id, rts->channel_no, rts->stream_type);
	nc_hy_t* nc = (nc_hy_t*)dev->father_nc;
	TOOL_UINT32 len = 0;
//	TOOL_INT32 ch = rts->channel_no;

	tool_stream3_initReadPos(&rts->stream, &rts->read_pos);

	while (rts->state)
	{
//		log_debug("i(%d) state(%d) enable(%d)", i, dev->rts[i].state, dev->cur_rts_enable[i]);
		/*
		if (rts->state == 0)
		{
			tool_sysf_usleep(20*1000);
			continue;
		}
		*/
			
		len = PTC_VIDEO_FRAME_SIZE;
		if (tool_stream3_get_withRts(&rts->stream, &rts->read_pos, rts->cb_frame, &len, nc->buffer_usec) == 0)
		{
			tool_sysf_usleep(20*1000);
			continue;
		}
		if (rts->cb)
		{
			rts->cb(NC_PTC_HuaYi | (dev->dev_id<<8) | rts->channel_no, rts->cb_frame, len, rts->param);
		}
	}

	log_state("dev(%d) CbRts(%d)(%d) stop", dev->dev_id, rts->channel_no, rts->stream_type);
	return NULL;
}

TOOL_VOID* nc_hy_dev_runRecvPbs(TOOL_VOID* param)
{
	nc_hy_pbs_t* pbs = (nc_hy_pbs_t*)param;
	nc_hy_dev_t* dev = (nc_hy_dev_t*)pbs->father_dev;
	TOOL_INT32 ret = 0;
	TOOL_UINT32 frame_len = 0;
	ptc_frame_head_t* frame_head = NULL;
	pbs->len = 0;
	pbs->key_flag = 0;
	pbs->left_len = 0;
	pbs->left_buf = NULL;
	pbs->start_flag = 0;
	TOOL_INT32 dev_online = 0;
	
	while (pbs->state)
	{
		if (dev_online != dev->online)
		{
			dev_online = dev->online;
			if (dev->online)
				nc_hy_pbs_doState(pbs, dev_online);
		}
/*
		TOOL_INT32 tick = 0;
		tool_sock_setTick(&tick);
		if (tick == 10)
			pbs->error_flag = 1;
*/		
		ret = tool_sock_item_pollRead(&dev->pbs.pbs_sock, 0);
		if (ret < 0)
		{
			log_error("tool_sock_item_pollRead");
			pbs->key_flag = 0;
			pbs->left_len = 0;
			pbs->error_flag = 1;
			break;
		}
		else if (ret == 0)
		{
			tool_sysf_usleep(20*1000);
			continue;
		}
		ret = tool_ptc_hy_recvPbs(&pbs->pbs_sock, pbs->recv_frame+sizeof(ptc_frame_head_t), &pbs->len, PTC_VIDEO_FRAME_SIZE-sizeof(ptc_frame_head_t),
			&frame_head, &frame_len, &pbs->left_buf, &pbs->left_len);
		if (ret < 0)
		{
			pbs->key_flag = 0;
			pbs->left_len = 0;
			pbs->error_flag = 1;
			break;
		}
		else if (ret == 0)
			continue;

		if (dev->pbs.active == 0 || dev->cur_pbs_enable == 0)
		{
			pbs->start_flag = 0;
			pbs->key_flag = 0;
			continue;
		}

		if (pbs->start_flag == 0)
		{
			if (frame_head->sec != dev->pbs.ptc_record.start_time)
			{
				log_debug("---------frame_head->sec(%d) dev->pbs.ptc_record.start_time(%d)", frame_head->sec, dev->pbs.ptc_record.start_time);
				continue;
			}
			pbs->start_flag = 1;
		}
		
		if (frame_head->sec >= dev->pbs.ptc_record.end_time)
		{
			nc_hy_pbs_doState(pbs, PTC_FRAME_PBS_END);
		//	log_debug("----------frame_head->sec(%d) dev->pbs.ptc_record.end_time(%d)", frame_head->sec, dev->pbs.ptc_record.end_time);
		//	continue;
		}
//		log_debug("frame_head->sec(%d) dev->pbs.ptc_record.start_time(%d)", frame_head->sec, dev->pbs.ptc_record.start_time);

		if (pbs->key_flag == 0)
		{
			if (frame_head->frame_type != PTC_FRAME_I)
			{
		//		log_debug("pbs->key_flag(%d) frame_head->frame_type(%d)", pbs->key_flag, frame_head->frame_type);
				continue;
			}
			else
				pbs->key_flag = 1;
		}
//		log_debug(" ptc_frame_head_t %d, %d, %d, %d", frame_head->no, frame_head->width, frame_head->height, frame_head->len);
		tool_stream3_set_noDrop(&dev->pbs.stream, frame_head, frame_len, &dev->cur_pbs_enable);
	}

	log_state("dev(%d) RecvPbs stop", dev->dev_id);
    return NULL;	   
}

TOOL_VOID* nc_hy_dev_runPlayPbs(TOOL_VOID* param)
{
	nc_hy_pbs_t* pbs = (nc_hy_pbs_t*)param;
	nc_hy_dev_t* dev = (nc_hy_dev_t*)pbs->father_dev;
	log_state("dev(%d) nc_hy_dev_runPlayPbs start", dev->dev_id);
	TOOL_UINT32 len = 0;

	tool_stream3_initReadPos(&pbs->stream, &pbs->read_pos);
	ptc_frame_head_t* frame_head = NULL;
	pbs->history_pos = 0;

	while (pbs->state)
	{
/*		
		if (pbs->state == 0)
		{
			tool_sysf_usleep(20*1000);
			continue;
		}
*/		
		len = PTC_VIDEO_FRAME_SIZE;
		if (tool_stream3_get_withPbs(&pbs->stream, &pbs->read_pos, pbs->cb_frame, &len) == 0)
		{
			tool_sysf_usleep(20*1000);
			continue;
		}
		
		if (dev->pbs.active == 0 || dev->cur_pbs_enable == 0)
		{
			tool_stream3_initReadPos(&pbs->stream, &pbs->read_pos);
			continue;
		}

//		ptc_frame_head_t* frame_head = (ptc_frame_head_t*)pbs->cb_frame;
//		log_debug(" ptc_frame_head_t %d, %d, %d, %d", frame_head->no, frame_head->width, frame_head->height, frame_head->len);
		if (pbs->cb)
		{
			pbs->cb(NC_PTC_HuaYi | (dev->dev_id<<8) | pbs->pbs_no, pbs->cb_frame, len, pbs->param);
			frame_head = (ptc_frame_head_t*)pbs->cb_frame;
			if (frame_head->frame_type == PTC_FRAME_I || frame_head->frame_type == PTC_FRAME_P)
				pbs->history_pos = frame_head->sec;
		}
	}
	pbs->history_pos = 0;

	log_state("dev(%d) nc_hy_dev_runPlayPbs stop", dev->dev_id);
	return NULL;
}

TOOL_INT32 nc_hy_dev_logout(nc_hy_dev_t* dev)
{
	TOOL_INT32 i = 0;
	if (tool_sock_item_isLive(&dev->msg_sock))
		tool_sock_item_close(&dev->msg_sock);
	for (i = 0; i < NC_HY_CHANNEL_SIZE; i++)
	{
		if (dev->rts[i].state)
		{
			dev->rts[i].state = 0;
			dev->rts[i].error_flag = 0;
			tool_thread_join(dev->rts[i].play_pid);
			tool_thread_join(dev->rts[i].recv_pid);
			tool_mem_free(dev->rts[i].recv_frame);
			dev->rts[i].recv_frame = NULL;
			tool_mem_free(dev->rts[i].cb_frame);
			dev->rts[i].cb_frame = NULL;
			tool_stream3_done(&dev->rts[i].stream);
		}
		dev->rts[i].error_flag = 0;
		dev->rts[i].active = 0;
		if (tool_sock_item_isLive(&dev->rts[i].rts_sock))
			tool_sock_item_close(&dev->rts[i].rts_sock);		
	}

	if (dev->pbs.state)
	{
		dev->pbs.state = 0;
		dev->pbs.active = 0;
		tool_thread_join(dev->pbs.play_pid);
		tool_thread_join(dev->pbs.recv_pid);
		tool_mem_free(dev->pbs.recv_frame);
		dev->pbs.recv_frame = NULL;
		tool_mem_free(dev->pbs.cb_frame);
		dev->pbs.cb_frame = NULL;
		tool_stream3_done(&dev->pbs.stream);
	}
	dev->pbs.error_flag = 0;
	if (tool_sock_item_isLive(&dev->pbs.pbs_sock))
		tool_sock_item_close(&dev->pbs.pbs_sock);
	dev->pbs.left_len = 0;
	dev->pbs_pause = 0;

	if (dev->last_talk_enable)
	{
		dev->last_talk_enable = 0;
	}
	if (dev->talk_send_stream)
	{
		tool_stream2_done(dev->talk_send_stream);
		tool_mem_free(dev->talk_send_stream);
		dev->talk_send_stream = NULL;
	}
	
	dev->last_ptz_cmd = 0;
	return 0;
}

TOOL_INT32 nc_hy_dev_login_simple(nc_hy_dev_t* dev)
{
	TOOL_INT32 ret = 0;
	do
	{
		if (tool_sock_item_open_v2(&dev->msg_sock, TOOL_SOCK_TYPE1_TCP, TOOL_SOCK_TYPE2_CLIENT, dev->ip_addr, dev->port, NULL, 0) < 0)
		{
			log_error("nc_hy_dev_login tool_sock_item_open");
			ret = NC_ERROR_CONNECT;
			break;
		}
		hy_msg_login0_c2d_t* login0_c2d = (hy_msg_login0_c2d_t*)&dev->hy_msg;
		tool_mem_memset(login0_c2d, sizeof(hy_msg_login0_c2d_t));
		login0_c2d->cmd = HY_CMD_LOGIN0_C2D;		// command
		login0_c2d->reserve1[2] = 0x60;
		tool_str_strncpy(login0_c2d->user, dev->user_name, 8);		//username
		tool_str_strncpy(login0_c2d->pswd, dev->user_pswd, 8);		//password
		login0_c2d->ver = 0x00000104;		// client verion
		login0_c2d->flag = 0xAAA10000;	
		if (tool_ptc_hy_sendMsg(&dev->msg_sock, &dev->hy_msg) < 0)
		{
			ret = NC_ERROR_SEND;
			break;
		}
		if (tool_ptc_hy_recvMsg(&dev->msg_sock, &dev->hy_msg) < 0)
		{
			ret = NC_ERROR_RECV;
			break;
		}
		
		hy_msg_login0_d2c_t* login0_d2c = (hy_msg_login0_d2c_t*)&dev->hy_msg;
		if (login0_d2c->cmd != HY_CMD_LOGIN0_D2C)
		{
			ret = NC_ERROR_PROTOCOL;
			break;
		}
		if (login0_d2c->fail)
		{
			if (login0_d2c->fail_code == 0 || login0_d2c->fail_code == 1)
			{
				ret = NC_ERROR_AUTH;
			}
			else if (login0_d2c->fail_code == 6)
			{
				ret = NC_ERROR_MAXCONNECT;
			}
			break;
		}
		if (login0_d2c->ch_num > NC_HY_CHANNEL_SIZE)
		{
			ret = NC_ERROR_INTERNAL;
			break;
		}
		dev->login0_d2c.ch_num = login0_d2c->ch_num;
		tool_mem_memcpy(&dev->login0_d2c.id, &login0_d2c->id, 4);
		log_state("ch_num(%d) id(0x%08x)", dev->login0_d2c.ch_num, login0_d2c->id);
	} while (0);
	if (ret < 0)
	{
        nc_hy_dev_logout(dev);
    }

	return ret;
}

TOOL_INT32 nc_hy_dev_login(nc_hy_dev_t* dev)
{
	if (tool_sock_item_isLive(&dev->msg_sock))
		return 0;
	if (tool_sock_item_isTimeout(&dev->msg_sock, NC_HY_RECONNECT_CYCLE) == 0)
	{
		tool_sysf_usleep(20*1000);
		return -1;
	}
	
	TOOL_INT32 ret = 0;
	TOOL_INT32 i = 0;
	do
	{
		dev->online = PTC_FRAME_STREAM_CONNECT;
		if (tool_sock_item_open_v2(&dev->msg_sock, TOOL_SOCK_TYPE1_TCP, TOOL_SOCK_TYPE2_CLIENT, dev->ip_addr, dev->port, NULL, 0) < 0)
		{
			log_error("nc_hy_dev_login tool_sock_item_open");
			ret = NC_ERROR_CONNECT;
			dev->online = PTC_FRAME_STREAM_CONNECT_FAIL;
			break;
		}
		hy_msg_login0_c2d_t* login0_c2d = (hy_msg_login0_c2d_t*)&dev->hy_msg;
		tool_mem_memset(login0_c2d, sizeof(hy_msg_login0_c2d_t));
		login0_c2d->cmd = HY_CMD_LOGIN0_C2D;		// command
		login0_c2d->reserve1[2] = 0x60;
		tool_str_strncpy(login0_c2d->user, dev->user_name, 8);		//username
		tool_str_strncpy(login0_c2d->pswd, dev->user_pswd, 8);		//password
		login0_c2d->ver = 0x00000104;		// client verion
		login0_c2d->flag = 0xAAA10000;	
		dev->online = PTC_FRAME_STREAM_LOGIN;
		if (tool_ptc_hy_sendMsg(&dev->msg_sock, &dev->hy_msg) < 0)
		{
			ret = NC_ERROR_SEND;
			dev->online = PTC_FRAME_STREAM_LOGIN_FAIL;
			break;
		}
		if (tool_ptc_hy_recvMsg(&dev->msg_sock, &dev->hy_msg) < 0)
		{
			ret = NC_ERROR_RECV;
			dev->online = PTC_FRAME_STREAM_LOGIN_FAIL;
			break;
		}
		
		hy_msg_login0_d2c_t* login0_d2c = (hy_msg_login0_d2c_t*)&dev->hy_msg;
		if (login0_d2c->cmd != HY_CMD_LOGIN0_D2C)
		{
			ret = NC_ERROR_PROTOCOL;
			dev->online = PTC_FRAME_STREAM_LOGIN_FAIL;
			break;
		}
		if (login0_d2c->fail)
		{
			if (login0_d2c->fail_code == 0 || login0_d2c->fail_code == 1)
			{
				dev->online = PTC_FRAME_STREAM_AUTH_FAIL;
				ret = NC_ERROR_AUTH;
			}
			else if (login0_d2c->fail_code == 6)
			{
				dev->online = PTC_FRAME_STREAM_MAX_CONNECT;
				ret = NC_ERROR_MAXCONNECT;
			}
			break;
		}
		if (login0_d2c->ch_num > NC_HY_CHANNEL_SIZE)
		{
			dev->online = PTC_FRAME_STREAM_LOGIN_FAIL;
			ret = NC_ERROR_INTERNAL;
			break;
		}
		dev->login0_d2c.ch_num = login0_d2c->ch_num;
		tool_mem_memcpy(&dev->login0_d2c.id, &login0_d2c->id, 4);
		log_state("ch_num(%d) id(0x%08x)", dev->login0_d2c.ch_num, login0_d2c->id);

		hy_msg_login1_c2d_t* login1_c2d = (hy_msg_login1_c2d_t*)&dev->hy_msg;
		tool_mem_memset(login1_c2d, sizeof(hy_msg_login1_c2d_t));
		login1_c2d->cmd = HY_CMD_LOGIN1_C2D;
		tool_mem_memcpy(&login1_c2d->id, &dev->login0_d2c.id, 4);
		login1_c2d->type = 1;
		dev->online = PTC_FRAME_STREAM_START;
		for (i = 0; i < dev->login0_d2c.ch_num; i++)
		{
			login1_c2d->ch_no = i+1;			
			dev->rts[i].state = 1;			
			if (tool_sock_item_open_v2(&dev->rts[i].rts_sock, TOOL_SOCK_TYPE1_TCP, TOOL_SOCK_TYPE2_CLIENT, dev->ip_addr, dev->port, NULL, 0) < 0)
			{
				dev->rts[i].state = 0;	
				ret = NC_ERROR_CONNECT;
				dev->online = PTC_FRAME_STREAM_START_FAIL;
				break;
			}
			if (tool_ptc_hy_sendMsg(&dev->rts[i].rts_sock, &dev->hy_msg) < 0)
			{
				dev->rts[i].state = 0;	
				dev->online = PTC_FRAME_STREAM_START_FAIL;
				ret = NC_ERROR_SEND;
				break;
			}
			tool_stream3_init(&dev->rts[i].stream, NC_HY_RTS_POOL_SIZE);
			dev->rts[i].recv_frame	= (TOOL_UINT8*)tool_mem_malloc(PTC_VIDEO_FRAME_SIZE, 0);
			dev->rts[i].cb_frame	= (TOOL_UINT8*)tool_mem_malloc(PTC_VIDEO_FRAME_SIZE, 0);
			if (tool_thread_create(&dev->rts[i].play_pid, NULL, nc_hy_dev_runPlayRts, (TOOL_VOID*)&dev->rts[i]) < 0)
				log_fatalNo("tool_thread_create");
			if (tool_thread_create(&dev->rts[i].recv_pid, NULL, nc_hy_dev_runRecvRts, (TOOL_VOID*)&dev->rts[i]) < 0)
				log_fatalNo("tool_thread_create");
		}
		if (i != dev->login0_d2c.ch_num)
			break;

		login1_c2d->type = 2;
		login1_c2d->ch_no = dev->login0_d2c.ch_num+1;
		dev->pbs.state = 1;
		if (tool_sock_item_open_v2(&dev->pbs.pbs_sock, TOOL_SOCK_TYPE1_TCP, TOOL_SOCK_TYPE2_CLIENT, dev->ip_addr, dev->port, NULL, 0) < 0)
		{
			dev->pbs.state = 0;	
			ret = NC_ERROR_CONNECT;
			dev->online = PTC_FRAME_STREAM_START_FAIL;
			break;
		}
		if (tool_ptc_hy_sendMsg(&dev->pbs.pbs_sock, &dev->hy_msg) < 0)
		{
			dev->pbs.state = 0;	
			dev->online = PTC_FRAME_STREAM_START_FAIL;
			ret = NC_ERROR_SEND;
			break;
		}
		tool_stream3_init(&dev->pbs.stream, NC_HY_PBS_POOL_SIZE);
		dev->pbs.recv_frame = (TOOL_UINT8*)tool_mem_malloc(PTC_VIDEO_FRAME_SIZE, 0);
		dev->pbs.cb_frame   = (TOOL_UINT8*)tool_mem_malloc(PTC_VIDEO_FRAME_SIZE, 0);
		if (tool_thread_create(&dev->pbs.play_pid, NULL, nc_hy_dev_runPlayPbs, (TOOL_VOID*)&dev->pbs) < 0)
			log_fatalNo("tool_thread_create");
		if (tool_thread_create(&dev->pbs.recv_pid, NULL, nc_hy_dev_runRecvPbs, (TOOL_VOID*)&dev->pbs) < 0)
			log_fatalNo("tool_thread_create");	

		dev->talk_send_stream = (tool_stream2_t*)tool_mem_malloc(sizeof(tool_stream2_t), 0);
		tool_stream2_init(dev->talk_send_stream);
		tool_stream2_initReadPos(dev->talk_send_stream, &dev->talk_send_pos);
	} while (0);
	if (ret < 0)
	{
        nc_hy_dev_logout(dev);
    }
	else
	{
		dev->online = PTC_FRAME_STREAM_RUNNING;
	}
	return ret;
}

TOOL_INT32 nc_hy_dev_heartbeat(nc_hy_dev_t* dev)
{
	if (tool_sysf_isTickTimeout(dev->hb_tick_sec, dev->hb_tick_usec, NC_HY_HEARTBEAT_CYCLE*1000*1000) == 0)
		return 0;
	
	tool_mem_memset(&dev->hy_msg, sizeof(hy_msg_head_t));
	dev->hy_msg.head.cmd = HY_CMD_HB_C2D;
	if (tool_ptc_hy_sendMsg(&dev->msg_sock, &dev->hy_msg) < 0)
	{
		log_error("tool_ptc_hy_sendMsg");
		return -1;
	}
	if (nc_hy_dev_recv(dev, HY_CMD_HB_D2C) < 0)
	{
		log_error("nc_hy_dev_recv");
		return -1;
	}
	log_debug("dev(%d) heartbeat", dev->dev_id);
	tool_sysf_setTick(&dev->hb_tick_sec, &dev->hb_tick_usec);
	return 0;
}

TOOL_INT32 nc_hy_dev_doRts(nc_hy_dev_t* dev)
{	
	TOOL_INT32 i = 0;
	for (i = 0; i < NC_HY_CHANNEL_SIZE; i++)
	{
		if (dev->rts[i].error_flag)
		{
			log_error("dev->rts[%d].error_flag", i);
			return -1;
		}
	}

	TOOL_INT32 rts_enable[NC_HY_CHANNEL_SIZE] = {0};
	TOOL_INT32 rts_stream_type[NC_HY_CHANNEL_SIZE] = {0};
	tool_thread_lockMutex(&dev->mutex);
	for (i = 0; i < NC_HY_CHANNEL_SIZE; i++)
	{
		rts_enable[i] = dev->cur_rts_enable[i];
		rts_stream_type[i] = dev->cur_rts_stream_type[i];
	}
	tool_thread_unlockMutex(&dev->mutex);	

	TOOL_INT32 flag = 0;
	hy_msg_rts_t* rts = (hy_msg_rts_t*)&dev->hy_msg;
	tool_mem_memset(rts, sizeof(hy_msg_rts_t));
	rts->cmd = HY_CMD_RTS;
	rts->len = 16;
	for (i = 0; i < NC_HY_CHANNEL_SIZE; i++)
	{
		if (dev->rts[i].active && dev->rts[i].stream_type != rts_stream_type[i])
		{
			log_debug("i(%d) stream_type(%d=>%d)", i, dev->rts[i].stream_type, rts_stream_type[i]);
			flag = 1;
			rts->enable[i] = 0;
			rts->stream[i] = dev->rts[i].stream_type;

			dev->rts[i].active = 0;
		}
	}
	if (flag)
	{
		if (tool_ptc_hy_sendMsg(&dev->msg_sock, &dev->hy_msg) < 0)
		{
			log_error("nc_hy_dev_doRts stop");
			return -1;
		}
		log_state("HY_CMD_RTS stop");
	}

	flag = 0;
	tool_mem_memset(rts, sizeof(hy_msg_rts_t));
	rts->cmd = HY_CMD_RTS;
	rts->len = 16;
	for (i = 0; i < NC_HY_CHANNEL_SIZE; i++)
	{
		if (dev->rts[i].active != rts_enable[i])
		{
			log_debug("i(%d) state(%d=>%d)", i, dev->rts[i].active, rts_enable[i]);
			flag = 1;
			rts->enable[i] = rts_enable[i];
			rts->stream[i] = rts_stream_type[i];

			dev->rts[i].active = rts_enable[i];
			dev->rts[i].stream_type = rts_stream_type[i];
		}
	}
	if (flag)
	{
		if (tool_ptc_hy_sendMsg(&dev->msg_sock, &dev->hy_msg) < 0)
		{
			log_error("nc_hy_dev_doRts start");
			return -1;
		}
		log_state("HY_CMD_RTS start");
	}
	return 0;
}

TOOL_INT32 nc_hy_dev_doPbs(nc_hy_dev_t* dev)
{
	if (dev->pbs.error_flag)
	{
		log_error("dev->pbs.error_flag");
		return -1;
	}
	TOOL_INT32 pbs_enable;
	ptc_cfg_record_t ptc_record;
	tool_thread_lockMutex(&dev->mutex);	
	pbs_enable = dev->cur_pbs_enable;
	tool_mem_memcpy(&ptc_record, &dev->pbs.ptc_record, sizeof(ptc_cfg_record_t));
	tool_thread_unlockMutex(&dev->mutex);

	tool_date_t date;
	hy_msg_pbs_start_t* pbs_start = (hy_msg_pbs_start_t*)&dev->hy_msg;
	hy_msg_pbs_stop_t* pbs_stop = (hy_msg_pbs_stop_t*)&dev->hy_msg;	
	hy_msg_record_t* record = (hy_msg_record_t*)ptc_record.file_info;
	if (dev->pbs.active != pbs_enable)
	{
		if (pbs_enable == 0)
		{
			tool_mem_memset(pbs_stop, sizeof(hy_msg_pbs_stop_t));
			pbs_stop->cmd = HY_CMD_PBS_STOP;
			pbs_stop->channel_no = ptc_record.channel+1;
			if (tool_ptc_hy_sendMsg(&dev->msg_sock, &dev->hy_msg) < 0)
			{
				log_error("HY_CMD_PBS_STOP");
				return -1;
			}
			dev->pbs.active = pbs_enable;
			log_state("HY_CMD_PBS_STOP success");
		}
		else
		{
			tool_mem_memset(pbs_stop, sizeof(hy_msg_pbs_stop_t));
			pbs_stop->cmd = HY_CMD_PBS_STOP;
			pbs_stop->channel_no = ptc_record.channel+1;
			if (tool_ptc_hy_sendMsg(&dev->msg_sock, &dev->hy_msg) < 0)
			{
				log_error("HY_CMD_PBS_STOP");
				return -1;
			}
			tool_mem_memset(pbs_start, sizeof(hy_msg_pbs_start_t));
			pbs_start->cmd = HY_CMD_PBS_START;
//			if (dev->pbs.history_pos > ptc_record.start_time && dev->pbs.history_pos < ptc_record.end_time)
//				tool_sysf_dateLocal(&date, dev->pbs.history_pos);
//			else
				tool_sysf_dateLocal(&date, ptc_record.start_time);
			pbs_start->channel_no = ptc_record.channel+1;
			pbs_start->year = date.year;
			pbs_start->month = date.month;
			pbs_start->day = date.day;
			pbs_start->hour = date.hour;
			pbs_start->minute = date.minute;
			pbs_start->second = date.second;
			log_debug("------(%d) %d:%d:%d",  ptc_record.start_time, pbs_start->hour, pbs_start->minute, pbs_start->second);
			tool_mem_memcpy(pbs_start->start_pos, record->start_pos, 4);	
			if (tool_ptc_hy_sendMsg(&dev->msg_sock, &dev->hy_msg) < 0)
			{
				log_error("HY_CMD_PBS_START");
				return -1;
			}
			dev->pbs.active = pbs_enable;	
			log_state("HY_CMD_PBS_START success");
		}
	}

	if (dev->pbs.active)
	{
		if (dev->pbs_pause)
		{
			if (tool_stream3_state(&dev->pbs.stream) < 0)
			{
				tool_mem_memset(pbs_stop, sizeof(hy_msg_pbs_stop_t));
				pbs_stop->cmd = HY_CMD_PBS_PAUSE;
				pbs_stop->reserve1[0] = 0x1;
				pbs_stop->channel_no = ptc_record.channel+1;
				if (tool_ptc_hy_sendMsg(&dev->msg_sock, &dev->hy_msg) < 0)
				{
					log_error("HY_CMD_PBS_RESTORE");
					return -1;
				}
				dev->pbs_pause = 0;
				log_state("HY_CMD_PBS_RESTORE success");
			}
		}
		else
		{
			if (tool_stream3_state(&dev->pbs.stream) > 0)
			{
				tool_mem_memset(pbs_stop, sizeof(hy_msg_pbs_stop_t));
				pbs_stop->cmd = HY_CMD_PBS_PAUSE;
				pbs_stop->channel_no = ptc_record.channel+1;
				if (tool_ptc_hy_sendMsg(&dev->msg_sock, &dev->hy_msg) < 0)
				{
					log_error("HY_CMD_PBS_PAUSE");
					return -1;
				}
				dev->pbs_pause = 1;
				log_state("HY_CMD_PBS_PAUSE success");
			}
		}
	}

	return 0;
}

TOOL_INT32 nc_hy_dev_doPtz(nc_hy_dev_t* dev)
{
	TOOL_UINT8 ptz_channel_no;
	TOOL_UINT8 ptz_cmd;
	TOOL_UINT8 ptz_param;
	TOOL_UINT8 ptz_step;
	tool_thread_lockMutex(&dev->mutex);
	ptz_channel_no = dev->cur_ptz_channel_no;
	ptz_cmd = dev->cur_ptz_cmd;
	ptz_param = dev->cur_ptz_param;
	ptz_step = dev->cur_ptz_step;
	tool_thread_unlockMutex(&dev->mutex);

	hy_msg_ptz_t* ptz = (hy_msg_ptz_t*)&dev->hy_msg;
	if (dev->last_ptz_channel_no != ptz_channel_no ||
		dev->last_ptz_cmd != ptz_cmd ||
		dev->last_ptz_param != ptz_param ||
		dev->last_ptz_step != ptz_step)
	{
		tool_mem_memset(ptz, sizeof(hy_msg_pbs_start_t));
		if (dev->last_ptz_cmd >= PTC_PTZ_CMD_LEFT && dev->last_ptz_cmd <= PTC_PTZ_CMD_IRIS_CLOSE)
		{
			ptz->cmd = HY_CMD_PTZ;
			ptz->protocol = 1;
			ptz->channel_no = dev->last_ptz_channel_no;
			if (dev->last_ptz_cmd == PTC_PTZ_CMD_LEFT)
				ptz->type = 0x02;
			else if (dev->last_ptz_cmd == PTC_PTZ_CMD_RIGHT)
				ptz->type = 0x03;
			else if (dev->last_ptz_cmd == PTC_PTZ_CMD_UP)
				ptz->type = 0x00;
			else if (dev->last_ptz_cmd == PTC_PTZ_CMD_DOWN)
				ptz->type = 0x01;
			else if (dev->last_ptz_cmd == PTC_PTZ_CMD_LEFT_UP)
				ptz->type = 0x20;
			else if (dev->last_ptz_cmd == PTC_PTZ_CMD_LEFT_DOWN)
				ptz->type = 0x22;
			else if (dev->last_ptz_cmd == PTC_PTZ_CMD_RIGHT_UP)
				ptz->type = 0x21;
			else if (dev->last_ptz_cmd == PTC_PTZ_CMD_RIGHT_DOWN)
				ptz->type = 0x23;
			else if (dev->last_ptz_cmd == PTC_PTZ_CMD_NEAR)
				ptz->type = 0x07;
			else if (dev->last_ptz_cmd == PTC_PTZ_CMD_FAR)
				ptz->type = 0x08;
			else if (dev->last_ptz_cmd == PTC_PTZ_CMD_ZOOM_OUT)
				ptz->type = 0x04;
			else if (dev->last_ptz_cmd == PTC_PTZ_CMD_ZOOM_IN)
				ptz->type = 0x05;
			else if (dev->last_ptz_cmd == PTC_PTZ_CMD_IRIS_OPEN)
				ptz->type = 0x09;
			else if (dev->last_ptz_cmd == PTC_PTZ_CMD_IRIS_CLOSE)
				ptz->type = 0x0A;
			ptz->speed = 0;
			if (tool_ptc_hy_sendMsg(&dev->msg_sock, &dev->hy_msg) < 0)
			{
				log_error("stop");
				return -1;
			}
			tool_sysf_usleep(1000*1000);
		}
		
		ptz->cmd = HY_CMD_PTZ;
		ptz->protocol = 0;
		ptz->channel_no = ptz_channel_no;
		if (ptz_cmd == PTC_PTZ_CMD_LEFT)
			ptz->type = 0x02;
		else if (ptz_cmd == PTC_PTZ_CMD_RIGHT)
			ptz->type = 0x03;
		else if (ptz_cmd == PTC_PTZ_CMD_UP)
			ptz->type = 0x00;
		else if (ptz_cmd == PTC_PTZ_CMD_DOWN)
			ptz->type = 0x01;
		else if (ptz_cmd == PTC_PTZ_CMD_LEFT_UP)
		{
			ptz->type = 0x20;
			ptz->step = 5;
		}
		else if (ptz_cmd == PTC_PTZ_CMD_LEFT_DOWN)
		{
			ptz->type = 0x22;
			ptz->step = 5;
		}
		else if (ptz_cmd == PTC_PTZ_CMD_RIGHT_UP)
		{
			ptz->type = 0x21;
			ptz->step = 5;
		}
		else if (ptz_cmd == PTC_PTZ_CMD_RIGHT_DOWN)
		{
			ptz->type = 0x23;
			ptz->step = 5;
		}
		else if (ptz_cmd == PTC_PTZ_CMD_NEAR)
			ptz->type = 0x07;
		else if (ptz_cmd == PTC_PTZ_CMD_FAR)
			ptz->type = 0x08;
		else if (ptz_cmd == PTC_PTZ_CMD_ZOOM_OUT)
			ptz->type = 0x04;
		else if (ptz_cmd == PTC_PTZ_CMD_ZOOM_IN)
			ptz->type = 0x05;
		else if (ptz_cmd == PTC_PTZ_CMD_IRIS_OPEN)
			ptz->type = 0x09;
		else if (ptz_cmd == PTC_PTZ_CMD_IRIS_CLOSE)
			ptz->type = 0x0A;
		else if (ptz_cmd == PTC_PTZ_CMD_PRESET_GO)
			ptz->type = 0x10;
		else if (ptz_cmd == PTC_PTZ_CMD_PRESET_SET)
			ptz->type = 0x11;
		else if (ptz_cmd == PTC_PTZ_CMD_PRESET_DEL)
			ptz->type = 0x12;
		else if (ptz_cmd == PTC_PTZ_CMD_AUTO_SCAN_START)
			ptz->type = 0x2B;
		else if (ptz_cmd == PTC_PTZ_CMD_AUTO_SCAN_STOP)
			ptz->type = 0x2C;
		if (ptz_cmd == PTC_PTZ_CMD_STOP)
			ptz->speed = 0;
		else
			ptz->speed = ptz_param;
		if (tool_ptc_hy_sendMsg(&dev->msg_sock, &dev->hy_msg) < 0)
		{
			log_error("HY_CMD_PTZ");
			return -1;
		}
		tool_sysf_setTick(&dev->ptz_tick_sec, &dev->ptz_tick_usec);
		
		dev->last_ptz_channel_no = ptz_channel_no;
		dev->last_ptz_cmd = ptz_cmd;
		dev->last_ptz_param = ptz_param;
		dev->last_ptz_step = ptz_step;
	}
	
	if (tool_sysf_isTickTimeout(dev->ptz_tick_sec, dev->ptz_tick_usec, 100*1000) &&
		(dev->last_ptz_cmd >= PTC_PTZ_CMD_LEFT && dev->last_ptz_cmd <= PTC_PTZ_CMD_IRIS_CLOSE))
	{
		log_debug("dev->last_ptz_cmd(%d)", dev->last_ptz_cmd);
		tool_mem_memset(ptz, sizeof(hy_msg_pbs_start_t));
		ptz->cmd = HY_CMD_PTZ;
		ptz->protocol = 0;
		ptz->channel_no = dev->last_ptz_channel_no;
		if (dev->last_ptz_cmd == PTC_PTZ_CMD_LEFT)
			ptz->type = 0x02;
		else if (dev->last_ptz_cmd == PTC_PTZ_CMD_RIGHT)
			ptz->type = 0x03;
		else if (dev->last_ptz_cmd == PTC_PTZ_CMD_UP)
			ptz->type = 0x00;
		else if (dev->last_ptz_cmd == PTC_PTZ_CMD_DOWN)
			ptz->type = 0x01;
		else if (dev->last_ptz_cmd == PTC_PTZ_CMD_LEFT_UP)
		{
			ptz->type = 0x20;
			ptz->step = 5;
		}
		else if (dev->last_ptz_cmd == PTC_PTZ_CMD_LEFT_DOWN)
		{
			ptz->type = 0x22;
			ptz->step = 5;
		}
		else if (dev->last_ptz_cmd == PTC_PTZ_CMD_RIGHT_UP)
		{
			ptz->type = 0x21;
			ptz->step = 5;
		}
		else if (dev->last_ptz_cmd == PTC_PTZ_CMD_RIGHT_DOWN)
		{
			ptz->type = 0x23;
			ptz->step = 5;
		}
		else if (dev->last_ptz_cmd == PTC_PTZ_CMD_NEAR)
			ptz->type = 0x07;
		else if (dev->last_ptz_cmd == PTC_PTZ_CMD_FAR)
			ptz->type = 0x08;
		else if (dev->last_ptz_cmd == PTC_PTZ_CMD_ZOOM_OUT)
			ptz->type = 0x04;
		else if (dev->last_ptz_cmd == PTC_PTZ_CMD_ZOOM_IN)
			ptz->type = 0x05;
		else if (dev->last_ptz_cmd == PTC_PTZ_CMD_IRIS_OPEN)
			ptz->type = 0x09;
		else if (dev->last_ptz_cmd == PTC_PTZ_CMD_IRIS_CLOSE)
			ptz->type = 0x0A;
		ptz->speed = dev->last_ptz_param;
		if (tool_ptc_hy_sendMsg(&dev->msg_sock, &dev->hy_msg) < 0)
		{
			log_error("HY_CMD_PTZ");
			return -1;
		}
		tool_sysf_setTick(&dev->ptz_tick_sec, &dev->ptz_tick_usec);
	}
	return 0;
}

TOOL_INT32 nc_hy_dev_doTalk(nc_hy_dev_t* dev)
{
	TOOL_INT32 talk_enable;
	tool_thread_lockMutex(&dev->mutex);
	talk_enable = dev->cur_talk_enable;
	tool_thread_unlockMutex(&dev->mutex);
	hy_msg_talk_t* talk = NULL;
	if (talk_enable)
	{
		if (dev->last_talk_enable == 0)
		{
			talk = (hy_msg_talk_t*)&dev->hy_msg;
			tool_mem_memset(talk, sizeof(hy_msg_talk_t));
			talk->cmd = HY_CMD_TALK;
			talk->reserve2[4] = 0x01;
			if (tool_ptc_hy_sendTalk_msg(&dev->msg_sock, &dev->hy_msg) < 0)
			{
				log_error("HY_CMD_TALK");
				return -1;
			}
			dev->last_talk_enable = talk_enable;
		}
		
		dev->talk_send_len = PTC_AUDIO_FRAME_SIZE;
		if (tool_stream2_get(dev->talk_send_stream, &dev->talk_send_pos, dev->talk_send_frame, &dev->talk_send_len))
		{
			if (tool_ptc_hy_sendTalk_frame(&dev->msg_sock, dev->talk_send_frame, dev->talk_send_len) < 0)
			{
				log_error("tool_ptc_hy_sendTalk_frame");
				return -1;
			}
			dev->is_sleep = 0;
		}
	}
	else 
	{
		if (dev->last_talk_enable)
		{
			talk = (hy_msg_talk_t*)&dev->hy_msg;
			tool_mem_memset(talk, sizeof(hy_msg_talk_t));
			talk->cmd = HY_CMD_TALK;
			if (tool_ptc_hy_sendMsg(&dev->msg_sock, &dev->hy_msg) < 0)
			{
				log_error("HY_CMD_TALK");
				return -1;
			}
			dev->last_talk_enable = talk_enable;
		}
	}
	return 0;
}

TOOL_INT32 nc_hy_dev_doSyncTime(nc_hy_dev_t* dev)
{
	TOOL_UINT32 utc_sec;
	tool_thread_lockMutex(&dev->mutex);
	utc_sec = dev->cur_utc_sec;
	tool_thread_unlockMutex(&dev->mutex);
	if (dev->last_utc_sec == utc_sec)
		return 0;
	hy_msg_param_time_t* param_time = NULL;
	param_time = (hy_msg_param_time_t*)&dev->hy_msg;
	tool_mem_memset(param_time, sizeof(hy_msg_param_time_t));
	tool_date_t date;
	tool_sysf_dateLocal(&date, utc_sec);
	param_time->cmd = HY_CMD_PARAM_TIME;
	param_time->reserve2[0] = 0x01;
	param_time->reserve2[8] = date.year-2000;
	param_time->reserve2[9] = date.month;
	param_time->reserve2[10] = date.day;
	param_time->reserve2[11] = date.hour;
	param_time->reserve2[12] = date.minute;
	param_time->reserve2[13] = date.second;
	if (tool_ptc_hy_sendMsg(&dev->msg_sock, &dev->hy_msg) < 0)
	{
		log_error("HY_CMD_PARAM_TIME send");
		return -1;
	}
	if (nc_hy_dev_recv(dev, HY_CMD_PARAM_TIME) < 0)
	{
		log_error("HY_CMD_PARAM_TIME recv");
		return -1;
	}
	dev->last_utc_sec = utc_sec;
	log_debug("dev(%d) sync time(%04d-%02d-%02d %02d:%02d:%02d)", dev->dev_id, date.year, date.month, date.day, date.hour, date.minute, date.second);
	return 0;
}

TOOL_INT32 nc_hy_dev_doGetStatus(nc_hy_dev_t* dev)
{
	ptc_cfg_status_t cfg_status;
	tool_thread_lockMutex(&dev->mutex);
	if (dev->last_get_status_req == dev->cur_get_status_req)
	{
	//	log_debug("dev->last_get_status_req(%d, %d)", dev->last_get_status_req, dev->cur_get_status_req);
		tool_thread_unlockMutex(&dev->mutex);
		return 0;
	}
	tool_mem_memcpy(&cfg_status, &dev->cfg_get_status, sizeof(ptc_cfg_status_t));
	tool_thread_unlockMutex(&dev->mutex);	

	TOOL_INT32 ret = 0;
	TOOL_INT32 result = 0;
	do
	{
		hy_msg_ao_t* ao = (hy_msg_ao_t*)&dev->hy_msg;
		tool_mem_memset(ao, sizeof(hy_msg_ao_t));
		ao->cmd = HY_CMD_GET_AO_C2D;
		ao->reserve2[0] = 1;
		if (tool_ptc_hy_sendMsg(&dev->msg_sock, &dev->hy_msg) < 0)
		{
			ret = NC_ERROR_SEND;
			result = NC_ERROR_SEND;
			break;
		}
		if (nc_hy_dev_recv(dev, HY_CMD_GET_AO_D2C) < 0)
		{
			ret = NC_ERROR_RECV;
			result = NC_ERROR_RECV;
			break;
		}

		cfg_status.io_out = 0;
		TOOL_INT32 i = 0;
		for (i = 0; i < 16; i++)
		{
			if (ao->out[i] == 1)
				cfg_status.io_out |= (1<<i);
		}
		log_state("nc_hy_dev_doGetStatus io_out(%016llx)", cfg_status.io_out);
	}while (0);

	tool_thread_lockMutex(&dev->mutex);
	tool_mem_memcpy(&dev->cfg_get_status, &cfg_status, sizeof(ptc_cfg_status_t));
	dev->result_status = result;
	dev->last_get_status_req = dev->cur_get_status_req;
	tool_thread_broadcastCond(&dev->cond_status);
	tool_thread_unlockMutex(&dev->mutex);
	return ret; 
}

TOOL_INT32 nc_hy_dev_doSetStatus(nc_hy_dev_t* dev)
{
	TOOL_INT32 set_status_req;
	ptc_cfg_status_t cfg_status;
	tool_thread_lockMutex(&dev->mutex);
	set_status_req = dev->cur_set_status_req;
	tool_mem_memcpy(&cfg_status, &dev->cfg_set_status, sizeof(ptc_cfg_status_t));
	tool_thread_unlockMutex(&dev->mutex);
	if (dev->last_set_status_req == set_status_req)
		return 0;

	log_state("nc_hy_dev_doSetStatus io_out(%016llx)", cfg_status.io_out);
	hy_msg_ao_t* ao = (hy_msg_ao_t*)&dev->hy_msg;
	tool_mem_memset(ao, sizeof(hy_msg_ao_t));
	ao->cmd = HY_CMD_SET_AO;
	TOOL_INT32 i = 0;
	for (i = 0; i < 20; i++)
	{
		if (cfg_status.io_out & (1<<i))
			ao->out[i] = 1;
		else
			ao->out[i] = 0;
	}

	if (tool_ptc_hy_sendMsg(&dev->msg_sock, &dev->hy_msg) < 0)
	{
		log_error("HY_CMD_SET_AO send");
		return -1;
	}
	if (nc_hy_dev_recv(dev, HY_CMD_SET_AO) < 0)
	{
		log_error("HY_CMD_SET_AO recv");
		return -1;
	}
	dev->last_set_status_req = set_status_req;
	return 0;
}

TOOL_INT32 nc_hy_dev_doRecords(nc_hy_dev_t* dev)
{
	ptc_cfg_records_t records;
	tool_thread_lockMutex(&dev->mutex);
	if (dev->last_records_req == dev->cur_records_req)
	{
		tool_thread_unlockMutex(&dev->mutex);
		return 0;
	}
	tool_mem_memcpy(&records, &dev->cfg_records, sizeof(ptc_cfg_records_t));
	tool_thread_unlockMutex(&dev->mutex);	

	TOOL_INT32 ret = 0;
	TOOL_INT32 result = 0;
	do
	{
		records.num = 0;
		TOOL_UINT8 type = 0;
		if (records.type == PTC_RECORD_TYPE_TIME || records.type == PTC_RECORD_TYPE_ALL)
			type = 0;
		else if (records.type == PTC_RECORD_TYPE_ALARM && records.sub_type == PTC_RECORD_SUBTYPE_SENSOR)
			type = 1;
		else if (records.type == PTC_RECORD_TYPE_ALARM && records.sub_type == PTC_RECORD_SUBTYPE_MOTION)
			type = 2;
		else
		{
			log_error("type(%02x) sub_type(%02x)", records.type, records.sub_type);
			break;
		}
		TOOL_INT32 i = 0;
		TOOL_UINT8 channel_no = 0;
		for (i = 0; i < NC_HY_CHANNEL_SIZE; i++)
		{
			if (records.channel & (TOOL_UINT64)(1<<i))
			{
				channel_no = i;
				break;
			}
		}
		if (i == NC_HY_CHANNEL_SIZE)
		{
			log_error("channel(%lld)", records.channel);
			result = NC_ERROR_INPUT;
			break;
		}
		
		tool_date_t date;
		tool_sysf_dateLocal(&date, records.start_time); 
		hy_msg_record_c2d_t record_c2d;
		hy_msg_record_d2c_t record_d2c;
		tool_mem_memset(&record_c2d, sizeof(hy_msg_record_c2d_t));
		record_c2d.cmd = HY_CMD_GET_RECODR_C2D;
		record_c2d.len = 0;
		record_c2d.channel_no = channel_no+1;
		record_c2d.year = date.year;
		record_c2d.month = date.month;
		record_c2d.day = date.day;
		record_c2d.hour = date.hour;
		record_c2d.minute = date.minute;
		record_c2d.second = date.second;
		record_c2d.type = type;
		log_debug("channel(%d) start(%04d-%02d-%02d %02d:%02d:%02d) type(%d)", 
			record_c2d.channel_no-1, record_c2d.year, record_c2d.month, record_c2d.day, 
			record_c2d.hour, record_c2d.minute, record_c2d.second, record_c2d.type);
		while (1)
		{
			tool_mem_memcpy(&dev->hy_msg.head, &record_c2d, sizeof(hy_msg_head_t));
			if (tool_ptc_hy_sendMsg(&dev->msg_sock, &dev->hy_msg) < 0)
			{
				ret = NC_ERROR_SEND;
				result = NC_ERROR_SEND;
				break;
			}
			if (nc_hy_dev_recv(dev, HY_CMD_GET_RECORD_D2C) < 0)
			{
				ret = NC_ERROR_RECV;
				result = NC_ERROR_RECV;
				break;
			}
			
			tool_mem_memcpy(&record_d2c, &dev->hy_msg, sizeof(hy_msg_record_d2c_t));
			for (i = 0; i < HY_MSG_RECORD_NUM; i++)
			{
				if (records.num >= PTC_RECORDS_SIZE)
					break;
				if (i * sizeof(hy_msg_record_t) >= record_d2c.len)
					break;
				tool_time_Date2Time(record_d2c.record[i].start_time.year+2000-1900, record_d2c.record[i].start_time.month-1, 
					record_d2c.record[i].start_time.day, record_d2c.record[i].start_time.hour, 
					record_d2c.record[i].start_time.minute, record_d2c.record[i].start_time.second, 
					&records.record[records.num].start_time);
				tool_time_Date2Time(record_d2c.record[i].end_time.year+2000-1900, record_d2c.record[i].end_time.month-1, 
					record_d2c.record[i].end_time.day, record_d2c.record[i].end_time.hour, 
					record_d2c.record[i].end_time.minute, record_d2c.record[i].end_time.second, 
					&records.record[records.num].end_time);
				records.record[records.num].size = record_d2c.record[i].size*1024;
				records.record[records.num].channel = record_d2c.record[i].channel_no;
				records.record[records.num].type = records.type;
				records.record[records.num].sub_type = records.sub_type;
				tool_mem_memcpy(records.record[records.num].file_info, &record_d2c.record[i], sizeof(hy_msg_record_t));
				if (records.record[records.num].end_time   <= records.start_time || 
					records.record[records.num].start_time >= records.end_time)
				{
//					log_debug("(%04d-%02d-%02d %02d:%02d:%02d)~(%04d-%02d-%02d %02d:%02d:%02d) (%d)~(%d)", 
//						record_d2c.record[i].start_time.year+2000, record_d2c.record[i].start_time.month,
//						record_d2c.record[i].start_time.day, record_d2c.record[i].start_time.hour,
//						record_d2c.record[i].start_time.minute, record_d2c.record[i].start_time.second,
//						record_d2c.record[i].end_time.year+2000, record_d2c.record[i].end_time.month,
//						record_d2c.record[i].end_time.day, record_d2c.record[i].end_time.hour,
//						record_d2c.record[i].end_time.minute, record_d2c.record[i].end_time.second,
//						records.start_time, records.end_time);
					continue;
				}
				
				records.num ++;
			}
			if (i != HY_MSG_RECORD_NUM)
				break;
			record_c2d.year = record_d2c.record[i-1].end_time.year+2000;
			record_c2d.month = record_d2c.record[i-1].end_time.month;
			record_c2d.day = record_d2c.record[i-1].end_time.day;
			record_c2d.hour = record_d2c.record[i-1].end_time.hour;
			record_c2d.minute = record_d2c.record[i-1].end_time.minute;
			record_c2d.second = record_d2c.record[i-1].end_time.second;
		}
	}while (0);

	tool_thread_lockMutex(&dev->mutex);
	tool_mem_memcpy(&dev->cfg_records, &records, sizeof(ptc_cfg_records_t));
	dev->result_records = result;
	dev->last_records_req = dev->cur_records_req;
	tool_thread_broadcastCond(&dev->cond_records);
	tool_thread_unlockMutex(&dev->mutex);
	return ret; 
}

TOOL_INT32 nc_hy_dev_doLogs(nc_hy_dev_t* dev)
{
	ptc_logs_t logs;
	tool_thread_lockMutex(&dev->mutex);
	if (dev->last_logs_req == dev->cur_logs_req)
	{
		tool_thread_unlockMutex(&dev->mutex);
		return 0;
	}
	tool_mem_memcpy(&logs, &dev->cfg_logs, sizeof(ptc_logs_t));
	tool_thread_unlockMutex(&dev->mutex);	

	TOOL_INT32 ret = 0;
	TOOL_INT32 result = 0;
	do
	{
		TOOL_INT32 i = 0;
		hy_msg_log_c2d_t log_c2d;
		hy_msg_log_d2c_t log_d2c;
		tool_mem_memset(&log_c2d, sizeof(hy_msg_log_c2d_t));
		log_c2d.cmd = HY_CMD_LOG_C2D;
		log_c2d.len = 0;
		log_c2d.reserve2[1] = 1;
		tool_mem_memcpy(&dev->hy_msg.head, &log_c2d, sizeof(hy_msg_head_t));
		if (tool_ptc_hy_sendMsg(&dev->msg_sock, &dev->hy_msg) < 0)
		{
			ret = NC_ERROR_SEND;
			result = NC_ERROR_SEND;
			break;
		}
		while (1)
		{
			if (nc_hy_dev_recv(dev, HY_CMD_LOG_D2C) < 0)
			{
				ret = NC_ERROR_SEND;
				result = NC_ERROR_SEND;
				break;
			}
			tool_mem_memcpy(&log_d2c, &dev->hy_msg, sizeof(hy_msg_log_d2c_t));
			for (i = 0; i < HY_MSG_LOG_NUM; i++)
			{
				if (logs.num >= PTC_LOGS_SIZE)
					break;
				if (i * sizeof(hy_msg_log_t) >= log_d2c.len)
					break;
				tool_time_Date2Time(log_d2c.log[i].time.year+2000-1900, log_d2c.log[i].time.month-1, 
					log_d2c.log[i].time.day, log_d2c.log[i].time.hour, 
					log_d2c.log[i].time.minute, log_d2c.log[i].time.second, 
					&logs.log[logs.num].time);
				if (log_d2c.log[i].type == HY_LOG_TYPE_REBOOT)
				{
					logs.log[logs.num].type = PTC_LOG_TYPE_SYSTEM;
					logs.log[logs.num].sub_type = PTC_LOG_REBOOT;
				}
				else if (log_d2c.log[i].type == HY_LOG_TYPE_SHUT)
				{
					logs.log[logs.num].type = PTC_LOG_TYPE_SYSTEM;
					logs.log[logs.num].sub_type = PTC_LOG_SHUTDOWN;
				}
				else if (log_d2c.log[i].type == HY_LOG_TYPE_UPGRADE)
				{
					logs.log[logs.num].type = PTC_LOG_TYPE_SYSTEM;
					logs.log[logs.num].sub_type = PTC_LOG_UPGRADE_SUCC;
				}
				else if (log_d2c.log[i].type == HY_LOG_TYPE_CONFSAVE)
				{
					logs.log[logs.num].type = PTC_LOG_TYPE_OPERATE;
					if (log_d2c.log[i].data == HY_CFG_INDEX_COMM)
					{
						logs.log[logs.num].sub_type = PTC_LOG_MODIFY_SERIAL_CONFIG;	
					}
					else if (log_d2c.log[i].data == HY_CFG_INDEX_NET)
					{
						logs.log[logs.num].sub_type = PTC_LOG_CHGE_IP;	
					}
					else if (log_d2c.log[i].data == HY_CFG_INDEX_RECORD)
					{
						logs.log[logs.num].sub_type = PTC_LOG_CHGE_SCH_SCHEDULE;	
					}
					else if (log_d2c.log[i].data == HY_CFG_INDEX_CAPTURE)
					{
						logs.log[logs.num].sub_type = PTC_LOG_CHGE_SNAP_SETTING;	
					}
					else if (log_d2c.log[i].data == HY_CFG_INDEX_PTZ)
					{
						logs.log[logs.num].sub_type = PTC_LOG_MODIFY_PTZ; 
					}
					else if (log_d2c.log[i].data == HY_CFG_INDEX_DETECT)
					{
						logs.log[logs.num].sub_type = PTC_LOG_MODIFY_MOTION;	
					}
					else if (log_d2c.log[i].data == HY_CFG_INDEX_ALARM)
					{
						logs.log[logs.num].sub_type = PTC_LOG_CHGE_SENSOR_SCH;	
					}
					else if (log_d2c.log[i].data == HY_CFG_INDEX_TITLE)
					{
						logs.log[logs.num].sub_type = PTC_LOG_CHGE_CAM_NAME;	
					}
					else if (log_d2c.log[i].data == HY_CFG_INDEX_MAIL)
					{
						logs.log[logs.num].sub_type = PTC_LOG_MODIFY_MAIL;	
					}
					else if (log_d2c.log[i].data == HY_CFG_INDEX_MAIL)
					{
						logs.log[logs.num].sub_type = PTC_LOG_MODIFY_MAIL;	
					}
					else if (log_d2c.log[i].data == HY_CFG_INDEX_PPPOE)
					{
						logs.log[logs.num].sub_type = PTC_LOG_MODIFY_PPPOE;	
					}
					else if (log_d2c.log[i].data == HY_CFG_INDEX_DDNS)
					{
						logs.log[logs.num].sub_type = PTC_LOG_MODIFY_DDNS;	
					}
					else if (log_d2c.log[i].data == HY_CFG_INDEX_DSPINFO)
					{
						logs.log[logs.num].sub_type = PTC_LOG_MODIFY_VIDEO_ENCODE;	
					}
					else if (log_d2c.log[i].data == HY_CFG_INDEX_COLOR)
					{
						logs.log[logs.num].sub_type = PTC_LOG_MODIFY_COLOR;	
					}
					else
					{
						logs.log[logs.num].sub_type = PTC_LOG_MODIFY_SYSCONFIG;	
						log_debug("type(HY_LOG_TYPE_CONFSAVE) subtype(%d)", log_d2c.log[i].data);
					}
				}
				else if (log_d2c.log[i].type == HY_LOG_TYPE_FSERROR ||
						 log_d2c.log[i].type == HY_LOG_TYPE_HDD_WERR ||
						 log_d2c.log[i].type == HY_LOG_TYPE_HDD_RERR ||
						 log_d2c.log[i].type == HY_LOG_TYPE_HDD_NOSPACE ||
						 log_d2c.log[i].type == HY_LOG_TYPE_HDD_TYPE_RW ||
						 log_d2c.log[i].type == HY_LOG_TYPE_HDD_TYPE_RO ||
						 log_d2c.log[i].type == HY_LOG_TYPE_HDD_TYPE_RE ||
						 log_d2c.log[i].type == HY_LOG_TYPE_HDD_TYPE_SS ||
						 log_d2c.log[i].type == HY_LOG_TYPE_HDD_NONE ||
						 log_d2c.log[i].type == HY_LOG_TYPE_HDD_NOWORKHDD ||
						 log_d2c.log[i].type == HY_LOG_TYPE_HDD_TYPE_BK ||
						 log_d2c.log[i].type == HY_LOG_TYPE_HDD_TYPE_REVERSE)
				{
					logs.log[logs.num].type = PTC_LOG_TYPE_ALARM;
					logs.log[logs.num].sub_type = PTC_LOG_DISK_IO_ERR;
				}
				else if (log_d2c.log[i].type == HY_LOG_TYPE_HDD_TYPE ||
						 log_d2c.log[i].type == HY_LOG_TYPE_HDD_FORMAT)
				{
					logs.log[logs.num].type = PTC_LOG_TYPE_ALARM;
					logs.log[logs.num].sub_type = PTC_LOG_FORMAT_FAIL;
				}
				else if (log_d2c.log[i].type == HY_LOG_TYPE_ALM_IN ||
						 log_d2c.log[i].type == HY_LOG_TYPE_NETALM_IN ||
						 log_d2c.log[i].type == HY_LOG_TYPE_ALM_BOSHI)
				{
					logs.log[logs.num].type = PTC_LOG_TYPE_ALARM;
					logs.log[logs.num].sub_type = PTC_LOG_PROBER_START;
					logs.log[logs.num].channel = log_d2c.log[i].data-1;
				}
				else if (log_d2c.log[i].type == HY_LOG_TYPE_ALM_END)
				{
					logs.log[logs.num].type = PTC_LOG_TYPE_ALARM;
					logs.log[logs.num].sub_type = PTC_LOG_PROBER_STOP;
					logs.log[logs.num].channel = log_d2c.log[i].data-1;
				}
				else if (log_d2c.log[i].type == HY_LOG_TYPE_LOSS_IN)
				{
					logs.log[logs.num].type = PTC_LOG_TYPE_ALARM;
					logs.log[logs.num].sub_type = PTC_LOG_VIDEOLOSS_START;
					logs.log[logs.num].channel = log_d2c.log[i].data-1;
				}
				else if (log_d2c.log[i].type == HY_LOG_TYPE_LOSS_END)
				{
					logs.log[logs.num].type = PTC_LOG_TYPE_ALARM;
					logs.log[logs.num].sub_type = PTC_LOG_VIDEOLOSS_STOP;
					logs.log[logs.num].channel = log_d2c.log[i].data-1;
				}
				else if (log_d2c.log[i].type == HY_LOG_TYPE_MOTION_IN)
				{
					logs.log[logs.num].type = PTC_LOG_TYPE_ALARM;
					logs.log[logs.num].sub_type = PTC_LOG_MOTION_START;
					logs.log[logs.num].channel = log_d2c.log[i].data-1;
				}
				else if (log_d2c.log[i].type == HY_LOG_TYPE_MOTION_END)
				{
					logs.log[logs.num].type = PTC_LOG_TYPE_ALARM;
					logs.log[logs.num].sub_type = PTC_LOG_MOTION_STOP;
					logs.log[logs.num].channel = log_d2c.log[i].data-1;
				}
				else if (log_d2c.log[i].type == HY_LOG_TYPE_NET_ABORT)
				{
					logs.log[logs.num].type = PTC_LOG_TYPE_ALARM;
					logs.log[logs.num].sub_type = PTC_LOG_NETWORK_ERR;
				}
				else if (log_d2c.log[i].type == HY_LOG_TYPE_BLIND_IN)
				{
					logs.log[logs.num].type = PTC_LOG_TYPE_ALARM;
					logs.log[logs.num].sub_type = PTC_LOG_VIDEOCOVER_START;
					logs.log[logs.num].channel = log_d2c.log[i].data-1;
				}
				else if (log_d2c.log[i].type == HY_LOG_TYPE_BLIND_END)
				{
					logs.log[logs.num].type = PTC_LOG_TYPE_ALARM;
					logs.log[logs.num].sub_type = PTC_LOG_VIDEOCOVER_STOP;
					logs.log[logs.num].channel = log_d2c.log[i].data-1;
				}
				else if (log_d2c.log[i].type == HY_LOG_TYPE_IPCONFLICT)
				{
					logs.log[logs.num].type = PTC_LOG_TYPE_ALARM;
					logs.log[logs.num].sub_type = PTC_LOG_IP_CONFLICT;
				}

				else if (log_d2c.log[i].type == HY_LOG_TYPE_AUTOMATIC_RECORD)
				{
					logs.log[logs.num].type = PTC_LOG_TYPE_OPERATE;
					logs.log[logs.num].sub_type = PTC_LOG_RECYCLE_REC_ON;
				}
				else if (log_d2c.log[i].type == HY_LOG_TYPE_MANUAL_RECORD)
				{
					logs.log[logs.num].type = PTC_LOG_TYPE_OPERATE;
					logs.log[logs.num].sub_type = PTC_LOG_MANUAL_START;
				}
				else if (log_d2c.log[i].type == HY_LOG_TYPE_CLOSED_RECORD)
				{
					logs.log[logs.num].type = PTC_LOG_TYPE_OPERATE;
					logs.log[logs.num].sub_type = PTC_LOG_MANUAL_STOP;
				}
				else if (log_d2c.log[i].type == HY_LOG_TYPE_LOGIN)
				{
					logs.log[logs.num].type = PTC_LOG_TYPE_OPERATE;
					logs.log[logs.num].sub_type = PTC_LOG_LOG_IN;
				}
				else if (log_d2c.log[i].type == HY_LOG_TYPE_LOGOUT)
				{
					logs.log[logs.num].type = PTC_LOG_TYPE_OPERATE;
					logs.log[logs.num].sub_type = PTC_LOG_LOG_OFF;
				}
				else if (log_d2c.log[i].type == HY_LOG_TYPE_ADD_USER ||
						 log_d2c.log[i].type == HY_LOG_TYPE_NET_LOGIN)
				{
					logs.log[logs.num].type = PTC_LOG_TYPE_OPERATE;
					logs.log[logs.num].sub_type = PTC_LOG_USER_ADD;
				}
				else if (log_d2c.log[i].type == HY_LOG_TYPE_DELETE_USER)
				{
					logs.log[logs.num].type = PTC_LOG_TYPE_OPERATE;
					logs.log[logs.num].sub_type = PTC_LOG_USER_DELETE;
				}
				else if (log_d2c.log[i].type == HY_LOG_TYPE_MODIFY_USER)
				{
					logs.log[logs.num].type = PTC_LOG_TYPE_OPERATE;
					logs.log[logs.num].sub_type = PTC_LOG_USER_MODIFY;
				}
				else if (log_d2c.log[i].type == HY_LOG_TYPE_CLEAR)
				{
					logs.log[logs.num].type = PTC_LOG_TYPE_OPERATE;
					logs.log[logs.num].sub_type = PTC_LOG_DELETE_FILE;
				}
				else if (log_d2c.log[i].type == HY_LOG_TYPE_SEARCHLOG)
				{
					logs.log[logs.num].type = PTC_LOG_TYPE_OPERATE;
					logs.log[logs.num].sub_type = PTC_LOG_VIEW_LOG;
				}
				else if (log_d2c.log[i].type == HY_LOG_TYPE_SEARCH)
				{
					logs.log[logs.num].type = PTC_LOG_TYPE_OPERATE;
					logs.log[logs.num].sub_type = PTC_LOG_SEARCH_TIME;
				}
				else if (log_d2c.log[i].type == HY_LOG_TYPE_DOWNLOAD ||
						 log_d2c.log[i].type == HY_LOG_TYPE_PLAYBACK)
				{
					logs.log[logs.num].type = PTC_LOG_TYPE_OPERATE;
					logs.log[logs.num].sub_type = PTC_LOG_PLAYBACK_PLAY;
					logs.log[logs.num].channel = log_d2c.log[i].context[4]-1;
				}
				else if (log_d2c.log[i].type == HY_LOG_TYPE_BACKUP)
				{
					logs.log[logs.num].type = PTC_LOG_TYPE_OPERATE;
					logs.log[logs.num].sub_type = PTC_LOG_BACKUP_COMPLETE;
				}
				else if (log_d2c.log[i].type == HY_LOG_TYPE_BACKUPERROR)
				{
					logs.log[logs.num].type = PTC_LOG_TYPE_OPERATE;
					logs.log[logs.num].sub_type = PTC_LOG_BACKUP_FAIL;
				}
				else
				{
					log_error("log_d2c.log[%d].type(0x%04x)", i, log_d2c.log[i].type);
					continue;
				}

				if (logs.log[logs.num].time <= logs.start_time || logs.log[logs.num].time >= logs.end_time)
				{
					log_debug("(%04d-%02d-%02d %02d:%02d:%02d) (%d)~(%d)", 
						log_d2c.log[i].time.year+2000, log_d2c.log[i].time.month,
						log_d2c.log[i].time.day, log_d2c.log[i].time.hour,
						log_d2c.log[i].time.minute, log_d2c.log[i].time.second,
						logs.start_time, logs.end_time);
					continue;
				}
				logs.log[logs.num].sub_type2 = logs.log[logs.num].sub_type;

	//			log_debug("logs.num(%d) time(%d) type(%d.%d), channel(%d)", 
	//				logs.num+1, logs.log[logs.num].time, logs.log[logs.num].type, logs.log[logs.num].sub_type, logs.log[logs.num].channel);
				logs.num ++;
			}
			if (i != HY_MSG_LOG_NUM)
				break;

			if (dev->hy_msg.head.len < HY_MSG_LOG_NUM*sizeof(hy_msg_log_t))
				break;
		}		
	}while (0);

	tool_thread_lockMutex(&dev->mutex);
	tool_mem_memcpy(&dev->cfg_logs, &logs, sizeof(ptc_logs_t));
	dev->result_logs = result;
	dev->last_logs_req = dev->cur_logs_req;
	tool_thread_broadcastCond(&dev->cond_logs);
	tool_thread_unlockMutex(&dev->mutex);
	return ret;
}

TOOL_INT32 nc_hy_dev_doRecv(nc_hy_dev_t* dev)
{
	TOOL_INT32 ret = 0;
	ret = tool_sock_item_pollRead(&dev->msg_sock, 0);
	if (ret < 0)
	{
		log_error("tool_sock_item_pollRead dev(%d)", dev->dev_id);
		return -1;
	}
	else if (ret > 0)
	{
		if (nc_hy_dev_recv(dev, HY_CMD_ALL) < 0)
		{
			log_error("nc_hy_dev_recv dev(%d)", dev->dev_id);
			return -1;
		}
		log_state("cmd(0x%02x)", dev->hy_msg.head.cmd);
		dev->is_sleep = 0;
	}
	return 0;
}

TOOL_VOID* nc_hy_dev_runLogin(TOOL_VOID* param)
{
	nc_hy_dev_t* dev = (nc_hy_dev_t*)param;
	dev->online = 0;
	log_state("dev(%d) Login start", dev->dev_id);
	while (dev->state)
	{
		if (nc_hy_dev_login(dev) < 0)
			continue;
		dev->is_sleep = 1;
		if (nc_hy_dev_heartbeat(dev) < 0 ||
			nc_hy_dev_doRts(dev) < 0 ||
			nc_hy_dev_doPbs(dev) < 0 ||
			nc_hy_dev_doPtz(dev) < 0 ||
			nc_hy_dev_doTalk(dev) < 0 ||
			nc_hy_dev_doGetStatus(dev) < 0 ||
			nc_hy_dev_doSetStatus(dev) < 0 ||
			nc_hy_dev_doSyncTime(dev) < 0 ||
			nc_hy_dev_doRecords(dev) < 0 ||
			nc_hy_dev_doLogs(dev) < 0 ||
			nc_hy_dev_doRecv(dev) < 0)
		{
			nc_hy_dev_logout(dev);
			dev->online = PTC_FRAME_STREAM_WAIT;
			continue;
		}
		
		if (dev->is_sleep)
			tool_sysf_usleep(20*1000);
	}

	log_state("dev(%d) Login stop", dev->dev_id);
	nc_hy_dev_logout(dev);
    return NULL;
}

TOOL_INT32 nc_hy_dev_start(nc_hy_dev_t* dev, const TOOL_INT8 *ip_addr, TOOL_INT32 port, const TOOL_INT8 *user_name, const TOOL_INT8 *user_pswd)
{
	tool_str_strncpy(dev->ip_addr, ip_addr, sizeof(dev->ip_addr)-1);
	dev->port = port;
	tool_str_strncpy(dev->user_name, user_name, sizeof(dev->user_name)-1);
	tool_str_strncpy(dev->user_pswd, user_pswd, sizeof(dev->user_pswd)-1);
	tool_sock_item_init(&dev->msg_sock);
	tool_sysf_setTick(&dev->state_tick_sec, &dev->state_tick_usec);	
//	tool_stream3_init(&dev->pbs.stream, NC_HY_PBS_POOL_SIZE);
//	dev->pbs.recv_frame = (TOOL_UINT8*)tool_mem_malloc(PTC_VIDEO_FRAME_SIZE, 0);
	dev->state = 1;
	if (tool_thread_create(&dev->login_pid, NULL, nc_hy_dev_runLogin, (TOOL_VOID*)dev) < 0)
		log_fatalNo("tool_thread_create");
//	if (tool_thread_create(&dev->recv_pbs_pid, NULL, nc_hy_dev_runRecvPbs, (TOOL_VOID*)&dev->pbs) < 0)
//		log_fatalNo("tool_thread_create");		
	return 0;	
}

TOOL_VOID nc_hy_dev_stop(nc_hy_dev_t* dev)
{
	if (dev->state == 0)
		return;
	
	dev->state = 0;
	tool_thread_join(dev->login_pid);
//	tool_thread_join(dev->recv_pbs_pid);
//	tool_mem_free(dev->pbs.recv_frame);
//	dev->pbs.recv_frame = NULL;
//	tool_stream3_done(&dev->pbs.stream);	
}

TOOL_INT32 nc_hy_dev_startAlarm(nc_hy_dev_t* dev, NC_CB cb, TOOL_VOID* param)
{
	TOOL_INT32 ret = 0;
	tool_thread_lockMutex(&dev->mutex);
	if (dev->cur_alarm_enable)
	{
		log_error("nc_hy_dev_startAlarm repeat");
		tool_thread_unlockMutex(&dev->mutex);
		return NC_ERROR_REPEAT;
	}
	dev->alarm_cb = cb;
	dev->alarm_param = param;
	dev->cur_alarm_enable = 1;
	ret = (NC_PTC_HuaYi | (dev->dev_id<<8) | dev->alarm.alarm_no);
	tool_sysf_setTick(&dev->state_tick_sec, &dev->state_tick_usec);
	tool_thread_unlockMutex(&dev->mutex);
	log_state("nc_hy_dev_startAlarm");
	
	return ret;
}

TOOL_INT32 nc_hy_dev_stopAlarm(nc_hy_dev_t* dev)
{
	TOOL_INT32 ret = 0;
	tool_thread_lockMutex(&dev->mutex);
	if (dev->cur_alarm_enable)
	{
		dev->cur_alarm_enable = 0;
		dev->alarm_cb = NULL;
		dev->alarm_param = NULL;
	}
	tool_sysf_setTick(&dev->state_tick_sec, &dev->state_tick_usec);
	tool_thread_unlockMutex(&dev->mutex);
	log_state("nc_hy_dev_stopAlarm");
	return ret;	
}

TOOL_INT32 nc_hy_dev_startRts(nc_hy_dev_t* dev, TOOL_INT32 channel_no, TOOL_INT32 stream_type, NC_CB cb, TOOL_VOID* param)
{
	TOOL_INT32 ret = 0;
	tool_thread_lockMutex(&dev->mutex);
	if (dev->cur_rts_enable[channel_no])
	{
		log_error("nc_hy_dev_startRts repeat[%d]", channel_no);
		tool_thread_unlockMutex(&dev->mutex);
		return NC_ERROR_REPEAT;
	}
//	tool_stream3_init(&dev->rts[channel_no].stream, NC_HY_RTS_POOL_SIZE);
//	dev->rts[channel_no].recv_frame = (TOOL_UINT8*)tool_mem_malloc(PTC_VIDEO_FRAME_SIZE, 0);
//	dev->rts[channel_no].cb_frame   = (TOOL_UINT8*)tool_mem_malloc(PTC_VIDEO_FRAME_SIZE, 0);
	dev->rts[channel_no].cb = cb;
	dev->rts[channel_no].param = param;
	dev->cur_rts_enable[channel_no] = 1;
	dev->cur_rts_stream_type[channel_no] = stream_type;
//	if (tool_thread_create(&dev->rts_pid[channel_no], NULL, nc_hy_dev_runPlayRts, (TOOL_VOID*)&dev->rts[channel_no]) < 0)
//		log_fatalNo("tool_thread_create");
//	if (tool_thread_create(&dev->recv_rts_pid[channel_no], NULL, nc_hy_dev_runRecvRts, (TOOL_VOID*)&dev->rts[channel_no]) < 0)
//		log_fatalNo("tool_thread_create");
	ret = (NC_PTC_HuaYi | (dev->dev_id<<8) | channel_no);
	tool_sysf_setTick(&dev->state_tick_sec, &dev->state_tick_usec);
	tool_thread_unlockMutex(&dev->mutex);
	log_state("nc_hy_dev_startRts[%d][%d]", channel_no, stream_type);

	return ret;
}

TOOL_INT32 nc_hy_dev_stopRts(nc_hy_dev_t* dev, TOOL_INT32 channel_no)
{
	TOOL_INT32 ret = 0;
	tool_thread_lockMutex(&dev->mutex);
	if (dev->cur_rts_enable[channel_no])
	{
		dev->cur_rts_enable[channel_no] = 0;
//		tool_thread_join(dev->rts_pid[channel_no]);
//		tool_thread_join(dev->recv_rts_pid[channel_no]);
		dev->rts[channel_no].cb = NULL;
		dev->rts[channel_no].param = NULL;
//		tool_mem_free(dev->rts[channel_no].recv_frame);
//		dev->rts[channel_no].recv_frame = NULL;
//		tool_mem_free(dev->rts[channel_no].cb_frame);
//		dev->rts[channel_no].cb_frame = NULL;
//		tool_stream3_done(&dev->rts[channel_no].stream);
	}
/*	
	TOOL_INT32 i = 0;
	for (i = 0; i < NC_HY_CHANNEL_SIZE; i++)
	{
		if (dev->cur_rts_enable[i])
			ret ++;
	}
	if (dev->cur_pbs_enable)
		ret ++;
	if (dev->cur_talk_enable)
		ret ++;
*/		
	tool_sysf_setTick(&dev->state_tick_sec, &dev->state_tick_usec);
	tool_thread_unlockMutex(&dev->mutex);
	log_state("nc_hy_dev_stopRts[%d][%d]", channel_no, dev->cur_rts_stream_type[channel_no]);
	return ret;
}

TOOL_INT32 nc_hy_dev_startPbs(nc_hy_dev_t* dev, ptc_cfg_record_t* record, NC_CB cb, TOOL_VOID* param)
{
	TOOL_INT32 tick = 0;
	tool_sock_setTick(&tick);
	while (1)
	{
		if (tool_sock_isTickTimeout(tick, 5))
		{
			log_error("tool_sock_isTickTimeout");
			return NC_ERROR_TIMEOUT;
		}
		
		if (dev->pbs.active == 0)
		{
			break;
		}
		tool_sysf_usleep(10*1000);
	}

	TOOL_INT32 ret = 0;
	tool_thread_lockMutex(&dev->mutex);
	if (dev->cur_pbs_enable)
	{
		log_error("nc_hy_dev_startPbs repeat");
		tool_thread_unlockMutex(&dev->mutex);
		return NC_ERROR_REPEAT;
	}
	tool_mem_memcpy(&dev->pbs.ptc_record, record, sizeof(ptc_cfg_record_t));
//	tool_stream3_init(&dev->pbs.stream, NC_HY_PBS_POOL_SIZE);
//	dev->pbs.recv_frame = (TOOL_UINT8*)tool_mem_malloc(PTC_VIDEO_FRAME_SIZE, 0);
//	dev->pbs.cb_frame   = (TOOL_UINT8*)tool_mem_malloc(PTC_VIDEO_FRAME_SIZE, 0);
	dev->pbs.cb = cb;
	dev->pbs.param = param;
	dev->cur_pbs_enable = 1;
//	if (tool_thread_create(&dev->pbs_pid, NULL, nc_hy_dev_runPlayPbs, (TOOL_VOID*)&dev->pbs) < 0)
//		log_fatalNo("tool_thread_create");
//	if (tool_thread_create(&dev->recv_pbs_pid, NULL, nc_hy_dev_runRecvPbs, (TOOL_VOID*)&dev->pbs) < 0)
//		log_fatalNo("tool_thread_create");	
	ret = (NC_PTC_HuaYi | (dev->dev_id<<8) | dev->pbs.pbs_no);
	tool_sysf_setTick(&dev->state_tick_sec, &dev->state_tick_usec);
	tool_thread_unlockMutex(&dev->mutex);
	log_state("nc_hy_dev_startPbs");
	return ret;
}

TOOL_INT32 nc_hy_dev_stopPbs(nc_hy_dev_t* dev)
{
	TOOL_INT32 ret = 0;
	tool_thread_lockMutex(&dev->mutex);
	if (dev->cur_pbs_enable)
	{
		dev->cur_pbs_enable = 0;
//		tool_thread_join(dev->pbs_pid);
//		tool_thread_join(dev->recv_pbs_pid);
		dev->pbs.cb = NULL;
		dev->pbs.param = NULL;
//		tool_mem_free(dev->pbs.recv_frame);
//		dev->pbs.recv_frame = NULL;
//		tool_mem_free(dev->pbs.cb_frame);
//		dev->pbs.cb_frame = NULL;
//		tool_stream3_done(&dev->pbs.stream);
	}
/*	
	TOOL_INT32 i = 0;
	for (i = 0; i < NC_HY_CHANNEL_SIZE; i++)
	{
		if (dev->cur_rts_enable[i])
			ret ++;
	}
	if (dev->cur_pbs_enable)
		ret ++;
	if (dev->cur_talk_enable)
		ret ++;
*/		
	tool_sysf_setTick(&dev->state_tick_sec, &dev->state_tick_usec);
	tool_thread_unlockMutex(&dev->mutex);
	log_state("nc_hy_dev_stopPbs");
	return ret;
}

TOOL_INT32 nc_hy_dev_ptz(nc_hy_dev_t* dev, TOOL_INT32 channel_no, PTC_PTZ_CMD_E cmd, TOOL_INT32 param)
{
	tool_thread_lockMutex(&dev->mutex);
	dev->cur_ptz_channel_no = channel_no;
	dev->cur_ptz_cmd = cmd;
	dev->cur_ptz_param = param;
	dev->cur_ptz_step = 0;
	tool_sysf_setTick(&dev->state_tick_sec, &dev->state_tick_usec);
	tool_thread_unlockMutex(&dev->mutex);
	return 0;
}

TOOL_INT32 nc_hy_dev_startTalk(nc_hy_dev_t* dev, NC_CB cb, TOOL_VOID* param)
{
	TOOL_INT32 ret = 0;
	tool_thread_lockMutex(&dev->mutex);
	if (dev->cur_talk_enable)
	{
		log_error("nc_hy_dev_startTalk repeat");
		tool_thread_unlockMutex(&dev->mutex);
		return NC_ERROR_REPEAT;
	}

//	dev->talk_send_stream = (tool_stream2_t*)tool_mem_malloc(sizeof(tool_stream2_t), 0);
//	tool_stream2_init(dev->talk_send_stream);
//	tool_stream2_initReadPos(dev->talk_send_stream, &dev->talk_send_pos);
	dev->talk_cb = cb;
	dev->talk_param = param;
	dev->cur_talk_enable = 1;
	ret = (NC_PTC_HuaYi | (dev->dev_id<<8) | dev->talk_no);
	tool_sysf_setTick(&dev->state_tick_sec, &dev->state_tick_usec);
	tool_thread_unlockMutex(&dev->mutex);
	log_state("nc_hy_dev_startTalk");
	return ret;
}

TOOL_INT32 nc_hy_dev_stopTalk(nc_hy_dev_t* dev)
{
	TOOL_INT32 ret = 0;
	tool_thread_lockMutex(&dev->mutex);
	if (dev->cur_talk_enable)
	{
		dev->cur_talk_enable = 0;
		dev->talk_cb = NULL;
		dev->talk_param = NULL;
//		tool_stream2_done(dev->talk_send_stream);
//		tool_mem_free(dev->talk_send_stream);
//		dev->talk_send_stream = NULL;
	}
/*	
	TOOL_INT32 i = 0;
	for (i = 0; i < NC_HY_CHANNEL_SIZE; i++)
	{
		if (dev->cur_rts_enable[i])
			ret ++;
	}
	if (dev->cur_pbs_enable)
		ret ++;
	if (dev->cur_talk_enable)
		ret ++;
*/		
	tool_sysf_setTick(&dev->state_tick_sec, &dev->state_tick_usec);
	tool_thread_unlockMutex(&dev->mutex);
	return ret;
}

TOOL_INT32 nc_hy_dev_syncTime(nc_hy_dev_t* dev, TOOL_UINT32 utc_sec)
{
	tool_thread_lockMutex(&dev->mutex);
	dev->cur_utc_sec = utc_sec;
	tool_sysf_setTick(&dev->state_tick_sec, &dev->state_tick_usec);
	tool_thread_unlockMutex(&dev->mutex);
	return 0;
}

TOOL_INT32 nc_hy_dev_searchRecords(nc_hy_dev_t* dev, ptc_cfg_records_t* records)
{
	TOOL_INT32 ret = 0;
	tool_thread_lockMutex(&dev->mutex);
	tool_sysf_setTick(&dev->state_tick_sec, &dev->state_tick_usec);
	dev->cur_records_req ++;
	tool_mem_memcpy(&dev->cfg_records, records, sizeof(ptc_cfg_records_t));
	if (tool_thread_timedwaitCond(&dev->cond_records, &dev->mutex, 5) < 0)
	{
		ret = NC_ERROR_TIMEOUT;
	}
	else
	{
		tool_mem_memcpy(records, &dev->cfg_records, sizeof(ptc_cfg_records_t));
		ret = dev->result_records;
	}
	tool_sysf_setTick(&dev->state_tick_sec, &dev->state_tick_usec);
	tool_thread_unlockMutex(&dev->mutex);
	return ret; 
}

TOOL_INT32 nc_hy_dev_searchLogs(nc_hy_dev_t* dev, ptc_logs_t* logs)
{
	TOOL_INT32 ret = 0;
	tool_thread_lockMutex(&dev->mutex);
	tool_sysf_setTick(&dev->state_tick_sec, &dev->state_tick_usec);
	dev->cur_logs_req ++;
	tool_mem_memcpy(&dev->cfg_logs, logs, sizeof(ptc_logs_t));
	if (tool_thread_timedwaitCond(&dev->cond_logs, &dev->mutex, 5) < 0)
	{
		ret = NC_ERROR_TIMEOUT;
	}
	else
	{
		tool_mem_memcpy(logs, &dev->cfg_logs, sizeof(ptc_logs_t));
		ret = dev->result_logs;
	}
	tool_sysf_setTick(&dev->state_tick_sec, &dev->state_tick_usec);
	tool_thread_unlockMutex(&dev->mutex);
	return ret;
}

TOOL_INT32 nc_hy_dev_getStatus(nc_hy_dev_t* dev, ptc_cfg_status_t* cfg_status)
{
	TOOL_INT32 ret = 0;
	tool_thread_lockMutex(&dev->mutex);
	tool_sysf_setTick(&dev->state_tick_sec, &dev->state_tick_usec);
	dev->cur_get_status_req ++;
//	log_debug("dev->last_get_status_req(%d, %d)", dev->last_get_status_req, dev->cur_get_status_req);
	tool_mem_memcpy(&dev->cfg_get_status, cfg_status, sizeof(ptc_cfg_status_t));
	if (tool_thread_timedwaitCond(&dev->cond_status, &dev->mutex, 5) < 0)
	{
		ret = NC_ERROR_TIMEOUT;
	}
	else
	{
		tool_mem_memcpy(cfg_status, &dev->cfg_get_status, sizeof(ptc_cfg_status_t));
		ret = dev->result_status;
	}
	tool_sysf_setTick(&dev->state_tick_sec, &dev->state_tick_usec);
	tool_thread_unlockMutex(&dev->mutex);
	return ret;
}

TOOL_INT32 nc_hy_dev_setStatus(nc_hy_dev_t* dev, ptc_cfg_status_t* cfg_status)
{
	tool_thread_lockMutex(&dev->mutex);
	dev->cur_set_status_req ++;
	tool_mem_memcpy(&dev->cfg_set_status, cfg_status, sizeof(ptc_cfg_status_t));
	tool_thread_unlockMutex(&dev->mutex);
	return 0;
}

#define __NC_HY_INFO__

TOOL_VOID* nc_hy_run(TOOL_VOID* param)
{
	TOOL_INT32 i = 0;
	TOOL_INT32 j = 0;
	TOOL_INT32 ret = 0;
	TOOL_INT32 count = 0;
	nc_hy_t* nc = (nc_hy_t*)param;
	nc_hy_dev_t* dev = NULL;
	while (nc->state)
	{
		count = 0;
		tool_thread_lockMutex(&nc->mutex);
		for (i = 0; i < NC_HY_USER_SIZE; i++)
		{
			dev = &nc->dev[i];
			if (dev->state == 0)
				continue;
			
			ret = 0;
			for (j = 0; j < NC_HY_CHANNEL_SIZE; j++)
			{
				if (dev->cur_rts_enable[j])
					ret ++;
			}
			if (dev->cur_pbs_enable)
				ret ++;
			if (dev->cur_talk_enable)
				ret ++;
			if (ret == 0 && tool_sysf_isTickTimeout(dev->state_tick_sec, dev->state_tick_usec, 60*1000*1000))
			{
				nc_hy_dev_stop(dev);
			}
			else
			{
				count ++;
			}
		}
		tool_thread_unlockMutex(&nc->mutex);
	//	log_debug("nc_hy_run count(%d)", count);
		tool_sysf_sleep(1);
	}
	return NULL;
}

TOOL_VOID nc_hy_init()
{
	if (g_nc_hy)
		return ;
	log_state("init hy sdk(v%04x.%04x)", PTC_VERSION, PTC_DATA);
	g_nc_hy = (nc_hy_t*)tool_mem_malloc(sizeof(nc_hy_t), 1);
    tool_thread_initMutex(&g_nc_hy->mutex);
	g_nc_hy->buffer_usec = 300*1000;
	TOOL_INT32 i = 0;
	for (i = 0; i < NC_HY_USER_SIZE; i++)
		nc_hy_dev_init(&g_nc_hy->dev[i], i, g_nc_hy);
	g_nc_hy->state = 1;
	if (tool_thread_create(&g_nc_hy->pid, NULL, nc_hy_run, (TOOL_VOID*)g_nc_hy) < 0)
		log_fatal("tool_thread_create");
}

TOOL_INT32 nc_hy_setBuffer(TOOL_INT32 buffer_usec)
{
	if (g_nc_hy == NULL)
	{
		log_error("nc");
		return NC_ERROR_UNINIT;
	}
	if (buffer_usec < 0 || buffer_usec > 3000*1000)
	{
		log_error("buffer_usec(%d)", buffer_usec);
		return NC_ERROR_INPUT;
	}
	g_nc_hy->buffer_usec = buffer_usec;
	return 0;
}

TOOL_VOID nc_hy_done()
{
    if (g_nc_hy == NULL)
        return;
	g_nc_hy->state = 0;
	tool_thread_join(g_nc_hy->pid);
    log_state("done hy sdk(v%04x.%04x)", PTC_VERSION, PTC_DATA);
	TOOL_INT32 i = 0;
	for (i = 0; i < NC_HY_USER_SIZE; i++)
		nc_hy_dev_done(&g_nc_hy->dev[i]);
	tool_thread_doneMutex(&g_nc_hy->mutex);
	tool_mem_free(g_nc_hy);
	g_nc_hy = NULL;
	tool_sock_done();
}

TOOL_INT32 nc_hy_searchDev(TOOL_VOID* param)
{
	if (g_nc_hy == NULL)
	{
		log_error("nc");
		return NC_ERROR_UNINIT;
	}
	ptc_mcast_searchs_t* searchs = (ptc_mcast_searchs_t*)param;
	if (searchs == NULL)
	{
		log_error("nc_hy_searchDev arg");
		return NC_ERROR_INPUT;
	}		
	searchs->num = 0;

	TOOL_INT32 tick_sec = 0;
	TOOL_INT32 tick_usec = 0;
	TOOL_INT32 ret = 0;
	TOOL_INT32 result = -1;
	hy_msg_search_c2d_t search_c2d;
	hy_msg_search_d2c_t search_d2c;
	tool_sock_item_t sock_s2c;
	do
	{
		if (tool_sock_item_open_v2(&sock_s2c, TOOL_SOCK_TYPE1_BCAST, TOOL_SOCK_TYPE2_NONE, HY_BCAST_IP, HY_BCAST_PORT, NULL, HY_BCAST_PORT) < 0)
		{
			log_error("tool_sock_createMcast sock_s2c");
			result = NC_ERROR_INTERNAL;
			break;
		}

		tool_mem_memset(&search_c2d, sizeof(hy_msg_search_c2d_t));
		search_c2d.cmd = HY_CMD_GET_C2D;
		search_c2d.reserve1[0] = 0x01;
		search_c2d.reserve2[8] = 0x02;
		if (tool_sock_item_sendto(&sock_s2c, &search_c2d, sizeof(hy_msg_search_c2d_t)) < 0)
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

			tool_mem_memset(&search_d2c, sizeof(hy_msg_search_d2c_t));
			if (tool_sock_item_recvfrom(&sock_s2c, &search_d2c, sizeof(hy_msg_search_d2c_t)) < 0)
			{
				log_error("tool_sock_item_sendto");
				break;
			}

 	//		log_debug("cmd(0x%02x)", search_d2c.cmd);
			if (search_d2c.cmd == HY_CMD_GET_D2C)
			{
				tool_mem_memset(&searchs->search[searchs->num], sizeof(ptc_mcast_search_t));
				searchs->search[searchs->num].net.enable1 = 1;
				tool_mem_memcpy(searchs->search[searchs->num].net.ip1, search_d2c.ip, 4);
				tool_mem_memcpy(searchs->search[searchs->num].net.mask1, search_d2c.mask, 4);
				tool_mem_memcpy(searchs->search[searchs->num].net.gate1, search_d2c.gate, 4);
				searchs->search[searchs->num].net.web_port = search_d2c.web_port;
				searchs->search[searchs->num].net.dev_port = search_d2c.dev_port;
				searchs->search[searchs->num].nc_ptc_type = NC_PTC_HuaYi;
				searchs->num ++;
			}
		}
		result = 0;		
	}while (0);
	tool_sock_item_close(&sock_s2c);

	return result;
}

TOOL_INT32 nc_hy_login_v2(nc_dev* info, NC_CB cb, TOOL_VOID* param)
{
	if (g_nc_hy == NULL)
	{
		log_error("nc");
		return NC_ERROR_UNINIT;
	}
	if (info == NULL)
	{
		log_error("info(0x%08x)", info);
		return NC_ERROR_INPUT;
	}
	
	TOOL_INT32 i = 0;
	TOOL_INT32 ret = 0;
	tool_thread_lockMutex(&g_nc_hy->mutex);
	for (i = 0; i < NC_HY_USER_SIZE; i++)
	{
		if (tool_str_strncmp(g_nc_hy->dev[i].ip_addr, info->ip, PTC_IP_SIZE-1) == 0 &&
			tool_str_strncmp(g_nc_hy->dev[i].user_name, info->user, PTC_ID_SIZE-1) == 0 &&
			tool_str_strncmp(g_nc_hy->dev[i].user_pswd, info->pswd, PTC_ID_SIZE-1) == 0 &&
			g_nc_hy->dev[i].port == info->msg_port &&
			g_nc_hy->dev[i].state)
		{
			ret = nc_hy_dev_startAlarm(&g_nc_hy->dev[i], cb, param);
			tool_thread_unlockMutex(&g_nc_hy->mutex);
			return ret;
		}
	}
	for (i = 0; i < NC_HY_USER_SIZE; i++)
	{
		if (g_nc_hy->dev[i].state == 0)
			break;
	}
	if (i == NC_HY_USER_SIZE)
	{
		log_error("nc full");
		tool_thread_unlockMutex(&g_nc_hy->mutex);
		return NC_ERROR_FULL;
	}
	
	ret = nc_hy_dev_start(&g_nc_hy->dev[i], info->ip, info->msg_port, info->user, info->pswd);
	if (ret < 0)
	{
		log_error("nc_hy_dev_start(%s:%d)", info->ip, info->msg_port);
		tool_thread_unlockMutex(&g_nc_hy->mutex);
		return ret;
	}
	ret = nc_hy_dev_startAlarm(&g_nc_hy->dev[i], cb, param);
	if (ret < 0)
	{
		log_error("nc_hy_dev_startAlarm");
//		nc_hy_dev_stop(&g_nc_hy->dev[i]);
	}
	tool_thread_unlockMutex(&g_nc_hy->mutex);
	return ret;	
}

TOOL_INT32 nc_hy_logout(TOOL_INT32 id)
{
	if (g_nc_hy == NULL)
	{
		log_error("nc");
		return NC_ERROR_UNINIT;
	}
	TOOL_INT32 dev_id = (id >> 8) & 0xFF;
	TOOL_INT32 alarm_no = (id) & 0xFF;
	if (dev_id < 0 || dev_id >= NC_HY_USER_SIZE || alarm_no != 3*NC_HY_CHANNEL_SIZE)
	{
		log_error("id(0x%08x)", id);
		return NC_ERROR_INPUT;
	}
	tool_thread_lockMutex(&g_nc_hy->mutex);
	nc_hy_dev_t* dev = (nc_hy_dev_t*)&g_nc_hy->dev[dev_id];
	nc_hy_dev_stopAlarm(dev);
	tool_thread_unlockMutex(&g_nc_hy->mutex);
	return 0;
}

TOOL_INT32 nc_hy_getChannelNum_v2(nc_dev* info)
{
	if (g_nc_hy == NULL)
	{
		log_error("nc");
		return NC_ERROR_UNINIT;
	}
	if (info == NULL)
	{
		log_error("info(0x%08x)", info);
		return NC_ERROR_INPUT;
	}

	nc_hy_dev_t dev;
	tool_mem_memset(&dev, sizeof(nc_hy_dev_t));
	tool_str_strncpy(dev.ip_addr, info->ip, sizeof(dev.ip_addr)-1);
	dev.port = info->msg_port;
	tool_str_strncpy(dev.user_name, info->user, sizeof(dev.user_name)-1);
	tool_str_strncpy(dev.user_pswd, info->pswd, sizeof(dev.user_pswd)-1);
	dev.state = 1;
	TOOL_INT32 ret = 0;
	tool_sock_item_init(&dev.msg_sock);
	ret = nc_hy_dev_login_simple(&dev);
	if (ret < 0)
	{
		dev.state = 0;
		return ret;
	}

	do
	{
		hy_msg_param_ability_c2d_t param_ability_c2d;
		tool_mem_memset(&param_ability_c2d, sizeof(hy_msg_param_ability_c2d_t));
		param_ability_c2d.cmd = HY_CMD_GET_SYSTEM_C2D; 	// command
		param_ability_c2d.type = 1;
		tool_mem_memcpy(&dev.hy_msg, &param_ability_c2d, sizeof(hy_msg_param_ability_c2d_t));
		if (tool_ptc_hy_sendMsg(&dev.msg_sock, &dev.hy_msg) < 0)
		{
			ret = NC_ERROR_SEND;
			break;
		}
		if (nc_hy_dev_recv(&dev, HY_CMD_GET_SYSTEM_D2C) < 0)
		{
			ret = NC_ERROR_RECV;
			break;
		}
		hy_msg_param_ability1_d2c_t param_ability1_d2c;
		tool_mem_memcpy(&param_ability1_d2c, &dev.hy_msg, sizeof(hy_msg_param_ability1_d2c_t));
		log_debug("video(%d) alarm(%d%d)", param_ability1_d2c.video_in_num, param_ability1_d2c.alarm_in_num, param_ability1_d2c.alarm_out_num);
		ret = param_ability1_d2c.video_in_num;
	} while(0);
	nc_hy_dev_logout(&dev);
	dev.state = 0;
	return ret;
}

TOOL_INT32 nc_hy_startRts_v2(nc_dev* info, TOOL_INT32 channel, TOOL_INT32 stream_type, NC_CB cb, void* param)
{
	if (g_nc_hy == NULL)
	{
		log_error("nc");
		return NC_ERROR_UNINIT;
	}
	if (info == NULL || channel < 0 || channel >= NC_HY_CHANNEL_SIZE || stream_type < 0 || stream_type >= NC_HY_STREAM_SIZE)
	{
		log_error("info(0x%08x), channel(%d), stream_type(%d)", info, channel, stream_type);
		return NC_ERROR_INPUT;
	}
	
	TOOL_INT32 i = 0;
	TOOL_INT32 ret = 0;
	tool_thread_lockMutex(&g_nc_hy->mutex);
	for (i = 0; i < NC_HY_USER_SIZE; i++)
	{
		if (tool_str_strncmp(g_nc_hy->dev[i].ip_addr, info->ip, PTC_IP_SIZE-1) == 0 &&
			tool_str_strncmp(g_nc_hy->dev[i].user_name, info->user, PTC_ID_SIZE-1) == 0 &&
			tool_str_strncmp(g_nc_hy->dev[i].user_pswd, info->pswd, PTC_ID_SIZE-1) == 0 &&			
			g_nc_hy->dev[i].port == info->msg_port &&
			g_nc_hy->dev[i].state)
		{
			ret = nc_hy_dev_startRts(&g_nc_hy->dev[i], channel, stream_type, cb, param);
			tool_thread_unlockMutex(&g_nc_hy->mutex);
			return ret;
		}
	}
	for (i = 0; i < NC_HY_USER_SIZE; i++)
	{
		if (g_nc_hy->dev[i].state == 0)
			break;
	}
	if (i == NC_HY_USER_SIZE)
	{
		log_error("nc full");
		tool_thread_unlockMutex(&g_nc_hy->mutex);
		return NC_ERROR_FULL;
	}
	
	ret = nc_hy_dev_start(&g_nc_hy->dev[i], info->ip, info->msg_port, info->user, info->pswd);
	if (ret < 0)
	{
		log_error("nc_hy_dev_start(%s:%d)", info->ip, info->msg_port);
		tool_thread_unlockMutex(&g_nc_hy->mutex);
		return ret;
	}
	ret = nc_hy_dev_startRts(&g_nc_hy->dev[i], channel, stream_type, cb, param);
	if (ret < 0)
	{
		log_error("nc_hy_dev_startRts[%d][%d]", channel, stream_type);
//		nc_hy_dev_stop(&g_nc_hy->dev[i]);
	}
	tool_thread_unlockMutex(&g_nc_hy->mutex);
	return ret;
}

TOOL_INT32 nc_hy_stopRts(TOOL_INT32 id)
{
	if (g_nc_hy == NULL)
	{
		log_error("nc");
		return NC_ERROR_UNINIT;
	}
	TOOL_INT32 dev_id = (id >> 8) & 0xFF;
	TOOL_INT32 channel_no = (id) & 0xFF;
	if (dev_id < 0 || dev_id >= NC_HY_USER_SIZE || channel_no < 0 || channel_no >= NC_HY_CHANNEL_SIZE)
	{
		log_error("id(0x%08x)", id);
		return NC_ERROR_INPUT;
	}
	tool_thread_lockMutex(&g_nc_hy->mutex);
	nc_hy_dev_t* dev = (nc_hy_dev_t*)&g_nc_hy->dev[dev_id];
	nc_hy_dev_stopRts(dev, channel_no);
//	TOOL_INT32 ret = nc_hy_dev_stopRts(dev, channel_no);
//	if (ret == 0)
//		nc_hy_dev_stop(dev);
	tool_thread_unlockMutex(&g_nc_hy->mutex);
	return 0;
}

TOOL_INT32 nc_hy_controlPTZ_v2(nc_dev* info, TOOL_INT32 channel, PTC_PTZ_CMD_E cmd, TOOL_INT32 param)
{
	if (g_nc_hy == NULL)
	{
		log_error("nc");
		return NC_ERROR_UNINIT;
	}
	if (info == NULL)
	{
		log_error("info");
		return NC_ERROR_INPUT;
	}

	TOOL_INT32 i = 0;
	TOOL_INT32 ret = 0;
	tool_thread_lockMutex(&g_nc_hy->mutex);
	for (i = 0; i < NC_HY_USER_SIZE; i++)
	{
		if (tool_str_strncmp(g_nc_hy->dev[i].ip_addr, info->ip, PTC_IP_SIZE-1) == 0 &&
			tool_str_strncmp(g_nc_hy->dev[i].user_name, info->user, PTC_ID_SIZE-1) == 0 &&
			tool_str_strncmp(g_nc_hy->dev[i].user_pswd, info->pswd, PTC_ID_SIZE-1) == 0 &&			
			g_nc_hy->dev[i].port == info->msg_port &&
			g_nc_hy->dev[i].state)
		{
			break;
		}
	}
	if (i == NC_HY_USER_SIZE)
	{
		tool_thread_unlockMutex(&g_nc_hy->mutex);
		return NC_ERROR_PTZNORTS;
	}
	ret = nc_hy_dev_ptz(&g_nc_hy->dev[i], channel, cmd, param);
	tool_thread_unlockMutex(&g_nc_hy->mutex);
	return ret;
}

TOOL_INT32 nc_hy_searchRecords(nc_dev* info, ptc_cfg_records_t* records)
{
	if (g_nc_hy == NULL)
	{
		log_error("nc");
		return NC_ERROR_UNINIT;
	}
	if (info == NULL || records == NULL)
	{
		log_error("info(0x%08x), records(0x%08x)", info, records);
		return NC_ERROR_INPUT;
	}

	TOOL_INT32 i = 0;
	TOOL_INT32 ret = 0;
	tool_thread_lockMutex(&g_nc_hy->mutex);
	for (i = 0; i < NC_HY_USER_SIZE; i++)
	{
		if (tool_str_strncmp(g_nc_hy->dev[i].ip_addr, info->ip, PTC_IP_SIZE-1) == 0 &&
			tool_str_strncmp(g_nc_hy->dev[i].user_name, info->user, PTC_ID_SIZE-1) == 0 &&
			tool_str_strncmp(g_nc_hy->dev[i].user_pswd, info->pswd, PTC_ID_SIZE-1) == 0 &&			
			g_nc_hy->dev[i].port == info->msg_port &&
			g_nc_hy->dev[i].state)
		{
			ret = nc_hy_dev_searchRecords(&g_nc_hy->dev[i], records);
			tool_thread_unlockMutex(&g_nc_hy->mutex);
			return ret;
		}
	}
	for (i = 0; i < NC_HY_USER_SIZE; i++)
	{
		if (g_nc_hy->dev[i].state == 0)
			break;
	}
	if (i == NC_HY_USER_SIZE)
	{
		log_error("nc full");
		tool_thread_unlockMutex(&g_nc_hy->mutex);
		return NC_ERROR_FULL;
	}
	
	ret = nc_hy_dev_start(&g_nc_hy->dev[i], info->ip, info->msg_port, info->user, info->pswd);
	if (ret < 0)
	{
		log_error("nc_hy_dev_start(%s:%d)", info->ip, info->msg_port);
		tool_thread_unlockMutex(&g_nc_hy->mutex);
		return ret;
	}
	ret = nc_hy_dev_searchRecords(&g_nc_hy->dev[i], records);
	if (ret < 0)
	{
		log_error("nc_hy_dev_searchRecords");
//		nc_hy_dev_stop(&g_nc_hy->dev[i]);
	}
	tool_thread_unlockMutex(&g_nc_hy->mutex);
	return ret;	
}

TOOL_INT32 nc_hy_startPbs(nc_dev* info, ptc_cfg_record_t* ptc_record, NC_CB cb, void* param)
{
	if (g_nc_hy == NULL)
	{
		log_error("nc");
		return NC_ERROR_UNINIT;
	}
	if (info == NULL || ptc_record == NULL)
	{
		log_error("info(0x%08x), record(0x%08x)", info, ptc_record);
		return NC_ERROR_INPUT;
	}
	
	TOOL_INT32 i = 0;
	TOOL_INT32 ret = 0;
	tool_thread_lockMutex(&g_nc_hy->mutex);
	for (i = 0; i < NC_HY_USER_SIZE; i++)
	{
		if (tool_str_strncmp(g_nc_hy->dev[i].ip_addr, info->ip, PTC_IP_SIZE-1) == 0 &&
			tool_str_strncmp(g_nc_hy->dev[i].user_name, info->user, PTC_ID_SIZE-1) == 0 &&
			tool_str_strncmp(g_nc_hy->dev[i].user_pswd, info->pswd, PTC_ID_SIZE-1) == 0 &&			
			g_nc_hy->dev[i].port == info->msg_port &&
			g_nc_hy->dev[i].state)
		{
			ret = nc_hy_dev_startPbs(&g_nc_hy->dev[i], ptc_record, cb, param);
			tool_thread_unlockMutex(&g_nc_hy->mutex);
			return ret;
		}
	}
	for (i = 0; i < NC_HY_USER_SIZE; i++)
	{
		if (g_nc_hy->dev[i].state == 0)
			break;
	}
	if (i == NC_HY_USER_SIZE)
	{
		log_error("nc full");
		tool_thread_unlockMutex(&g_nc_hy->mutex);
		return NC_ERROR_FULL;
	}
	
	ret = nc_hy_dev_start(&g_nc_hy->dev[i], info->ip, info->msg_port, info->user, info->pswd);
	if (ret < 0)
	{
		log_error("nc_hy_dev_start(%s:%d)", info->ip, info->msg_port);
		tool_thread_unlockMutex(&g_nc_hy->mutex);
		return ret;
	}
	ret = nc_hy_dev_startPbs(&g_nc_hy->dev[i], ptc_record, cb, param);
	if (ret < 0)
	{
		log_error("nc_hy_dev_startPbs");
//		nc_hy_dev_stop(&g_nc_hy->dev[i]);
	}
	tool_thread_unlockMutex(&g_nc_hy->mutex);
	return ret;
}

TOOL_INT32 nc_hy_stopPbs(TOOL_INT32 id)
{
	if (g_nc_hy == NULL)
	{
		log_error("nc");
		return NC_ERROR_UNINIT;
	}
	TOOL_INT32 dev_id = (id >> 8) & 0xFF;
	TOOL_INT32 pbs_no = (id) & 0xFF;
	if (dev_id < 0 || dev_id >= NC_HY_USER_SIZE || pbs_no != NC_HY_CHANNEL_SIZE)
	{
		log_error("id(0x%08x)", id);
		return NC_ERROR_INPUT;
	}
	tool_thread_lockMutex(&g_nc_hy->mutex);
	nc_hy_dev_t* dev = (nc_hy_dev_t*)&g_nc_hy->dev[dev_id];
	nc_hy_dev_stopPbs(dev);
//	TOOL_INT32 ret = nc_hy_dev_stopPbs(dev);
//	if (ret == 0)
//		nc_hy_dev_stop(dev);
	tool_thread_unlockMutex(&g_nc_hy->mutex);
	return 0;	
}

TOOL_INT32 nc_hy_searchLogs(nc_dev* info, ptc_logs_t* logs)
{
	if (g_nc_hy == NULL)
	{
		log_error("nc");
		return NC_ERROR_UNINIT;
	}
	if (info == NULL || logs == NULL)
	{
		log_error("info(0x%08x), logs(0x%08x)", info, logs);
		return NC_ERROR_INPUT;
	}

	TOOL_INT32 i = 0;
	TOOL_INT32 ret = 0;
	tool_thread_lockMutex(&g_nc_hy->mutex);
	for (i = 0; i < NC_HY_USER_SIZE; i++)
	{
		if (tool_str_strncmp(g_nc_hy->dev[i].ip_addr, info->ip, PTC_IP_SIZE-1) == 0 &&
			tool_str_strncmp(g_nc_hy->dev[i].user_name, info->user, PTC_ID_SIZE-1) == 0 &&
			tool_str_strncmp(g_nc_hy->dev[i].user_pswd, info->pswd, PTC_ID_SIZE-1) == 0 &&			
			g_nc_hy->dev[i].port == info->msg_port &&
			g_nc_hy->dev[i].state)
		{
			ret = nc_hy_dev_searchLogs(&g_nc_hy->dev[i], logs);
			tool_thread_unlockMutex(&g_nc_hy->mutex);
			return ret;
		}
	}
	for (i = 0; i < NC_HY_USER_SIZE; i++)
	{
		if (g_nc_hy->dev[i].state == 0)
			break;
	}
	if (i == NC_HY_USER_SIZE)
	{
		log_error("nc full");
		tool_thread_unlockMutex(&g_nc_hy->mutex);
		return NC_ERROR_FULL;
	}
	
	ret = nc_hy_dev_start(&g_nc_hy->dev[i], info->ip, info->msg_port, info->user, info->pswd);
	if (ret < 0)
	{
		log_error("nc_hy_dev_start(%s:%d)", info->ip, info->msg_port);
		tool_thread_unlockMutex(&g_nc_hy->mutex);
		return ret;
	}
	ret = nc_hy_dev_searchLogs(&g_nc_hy->dev[i], logs);
	if (ret < 0)
	{
		log_error("nc_hy_dev_searchLogs ret(%d)", ret);
//		nc_hy_dev_stop(&g_nc_hy->dev[i]);
	}
	tool_thread_unlockMutex(&g_nc_hy->mutex);
	return ret;	
}

TOOL_INT32 nc_hy_startTalk_v2(nc_dev* info, NC_CB cb, void* param)
{
	if (g_nc_hy == NULL)
	{
		log_error("nc");
		return NC_ERROR_UNINIT;
	}
	if (info == NULL)
	{
		log_error("info(0x%08x)", info);
		return NC_ERROR_INPUT;
	}
	
	TOOL_INT32 i = 0;
	TOOL_INT32 ret = 0;
	tool_thread_lockMutex(&g_nc_hy->mutex);
	for (i = 0; i < NC_HY_USER_SIZE; i++)
	{
		if (tool_str_strncmp(g_nc_hy->dev[i].ip_addr, info->ip, PTC_IP_SIZE-1) == 0 &&
			tool_str_strncmp(g_nc_hy->dev[i].user_name, info->user, PTC_ID_SIZE-1) == 0 &&
			tool_str_strncmp(g_nc_hy->dev[i].user_pswd, info->pswd, PTC_ID_SIZE-1) == 0 &&			
			g_nc_hy->dev[i].port == info->msg_port &&
			g_nc_hy->dev[i].state)
		{
			ret = nc_hy_dev_startTalk(&g_nc_hy->dev[i], cb, param);
			tool_thread_unlockMutex(&g_nc_hy->mutex);
			return ret;
		}
	}
	for (i = 0; i < NC_HY_USER_SIZE; i++)
	{
		if (g_nc_hy->dev[i].state == 0)
			break;
	}
	if (i == NC_HY_USER_SIZE)
	{
		log_error("nc full");
		tool_thread_unlockMutex(&g_nc_hy->mutex);
		return NC_ERROR_FULL;
	}
	
	ret = nc_hy_dev_start(&g_nc_hy->dev[i], info->ip, info->msg_port, info->user, info->pswd);
	if (ret < 0)
	{
		log_error("nc_hy_dev_start(%s:%d)", info->ip, info->msg_port);
		tool_thread_unlockMutex(&g_nc_hy->mutex);
		return ret;
	}
	ret = nc_hy_dev_startTalk(&g_nc_hy->dev[i], cb, param);
	if (ret < 0)
	{
		log_error("nc_hy_dev_startTalk");
		nc_hy_dev_stop(&g_nc_hy->dev[i]);
	}
	tool_thread_unlockMutex(&g_nc_hy->mutex);
	return ret;

}

TOOL_INT32 nc_hy_stopTalk(TOOL_INT32 id)
{
	if (g_nc_hy == NULL)
	{
		log_error("nc");
		return NC_ERROR_UNINIT;
	}
	TOOL_INT32 dev_id = (id >> 8) & 0xFF;
	TOOL_INT32 talk_no = (id) & 0xFF;
	if (dev_id < 0 || dev_id >= NC_HY_USER_SIZE || talk_no != 2*NC_HY_CHANNEL_SIZE)
	{
		log_error("id(0x%08x)", id);
		return NC_ERROR_INPUT;
	}
	tool_thread_lockMutex(&g_nc_hy->mutex);
	nc_hy_dev_t* dev = (nc_hy_dev_t*)&g_nc_hy->dev[dev_id];
	nc_hy_dev_stopTalk(dev);
//	TOOL_INT32 ret = nc_hy_dev_stopTalk(dev);
//	if (ret == 0)
//		nc_hy_dev_stop(dev);
	tool_thread_unlockMutex(&g_nc_hy->mutex);
	return 0;
}

TOOL_INT32 nc_hy_sendTalk(TOOL_INT32 id, TOOL_UINT8* buf, TOOL_INT32 len)
{
	if (g_nc_hy == NULL)
	{
		log_error("nc");
		return NC_ERROR_UNINIT;
	}
	TOOL_INT32 dev_id = (id >> 8) & 0xFF;
	TOOL_INT32 talk_no = (id) & 0xFF;
	if (dev_id < 0 || dev_id >= NC_HY_USER_SIZE || talk_no != 2*NC_HY_CHANNEL_SIZE || buf == NULL || len != HY_TALK_FRAME_SIZE+sizeof(ptc_frame_head_t))
	{
		log_error("id(0x%08x) buf(0x%x) len(%d)", id, buf, len);
		return NC_ERROR_INPUT;
	}
	if (g_nc_hy->dev[dev_id].last_talk_enable && g_nc_hy->dev[dev_id].talk_send_stream)
		tool_stream2_set(g_nc_hy->dev[dev_id].talk_send_stream, buf, len);
	return 0;
}

TOOL_INT32 nc_hy_getAbility_v2(nc_dev* info, ptc_cfg_ability_t* ability)
{
	if (g_nc_hy == NULL)
	{
		log_error("nc");
		return NC_ERROR_UNINIT;
	}
	if (info == NULL)
	{
		log_error("info(0x%08x)", info);
		return NC_ERROR_INPUT;
	}

	nc_hy_dev_t dev;
	tool_mem_memset(&dev, sizeof(nc_hy_dev_t));
	tool_str_strncpy(dev.ip_addr, info->ip, sizeof(dev.ip_addr)-1);
	dev.port = info->msg_port;
	tool_str_strncpy(dev.user_name, info->user, sizeof(dev.user_name)-1);
	tool_str_strncpy(dev.user_pswd, info->pswd, sizeof(dev.user_pswd)-1);
	dev.state = 1;
	TOOL_INT32 ret = 0;
	tool_sock_item_init(&dev.msg_sock);
	ret = nc_hy_dev_login_simple(&dev);
	if (ret < 0)
	{
		dev.state = 0;
		return ret;
	}
	do
	{
		tool_mem_memset(ability, sizeof(ptc_cfg_ability_t));
		ability->audio_ability.enable = 1;
		ability->audio_ability.sample_rate = PTC_AUDIO_RATE_8K;
		ability->audio_ability.bit_width = PTC_AUDIO_WIDTH_16BIT;
		ability->audio_ability.encode_type = PTC_AUDIO_ENCODE_PCM;
		ability->audio_ability.frame_size = 2048;
		
		hy_msg_param_ability_c2d_t param_ability_c2d;
		tool_mem_memset(&param_ability_c2d, sizeof(hy_msg_param_ability_c2d_t));
		param_ability_c2d.cmd = HY_CMD_GET_SYSTEM_C2D; 	// command
		param_ability_c2d.type = 1;	
		tool_mem_memcpy(&dev.hy_msg, &param_ability_c2d, sizeof(hy_msg_param_ability_c2d_t));
		if (tool_ptc_hy_sendMsg(&dev.msg_sock, &dev.hy_msg) < 0)
		{
			ret = NC_ERROR_SEND;
			break;
		}
		if (nc_hy_dev_recv(&dev, HY_CMD_GET_SYSTEM_D2C) < 0)
		{
			ret = NC_ERROR_RECV;
			break;
		}
		hy_msg_param_ability1_d2c_t param_ability1_d2c;
		tool_mem_memcpy(&param_ability1_d2c, &dev.hy_msg, sizeof(hy_msg_param_ability1_d2c_t));
		log_debug("video(%d) alarm(%d%d)", param_ability1_d2c.video_in_num, param_ability1_d2c.alarm_in_num, param_ability1_d2c.alarm_out_num);
		ability->video_input_num = param_ability1_d2c.video_in_num;
		ability->alarm_input_num = param_ability1_d2c.alarm_in_num;
		ability->alarm_output_num = param_ability1_d2c.alarm_out_num;
		ability->base.video_input_num = param_ability1_d2c.video_in_num;
		ability->base.alarm_input_num= param_ability1_d2c.alarm_in_num;
		ability->base.alarm_output_num = param_ability1_d2c.alarm_out_num;

		tool_mem_memset(&param_ability_c2d, sizeof(hy_msg_param_ability_c2d_t));
		param_ability_c2d.cmd = HY_CMD_GET_SYSTEM_C2D; 	// command
		param_ability_c2d.type = 8;
		tool_mem_memcpy(&dev.hy_msg, &param_ability_c2d, sizeof(hy_msg_param_ability_c2d_t));
		if (tool_ptc_hy_sendMsg(&dev.msg_sock, &dev.hy_msg) < 0)
		{
			ret = NC_ERROR_SEND;
			break;
		}
		if (nc_hy_dev_recv(&dev, HY_CMD_GET_SYSTEM_D2C) < 0)
		{
			ret = NC_ERROR_RECV;
			break;
		}
		hy_msg_param_ability8_d2c_t* param_ability8_d2c = (hy_msg_param_ability8_d2c_t*)&dev.hy_msg;
		log_debug("device_type(%s)", param_ability8_d2c->data);
		if (tool_str_strstr(param_ability8_d2c->data, "DVR-G004"))
			ability->base.device_type = PTC_DEVTYPE_HUYYI_D5004;
		else if (tool_str_strstr(param_ability8_d2c->data, "D6004"))
			ability->base.device_type = PTC_DEVTYPE_HUYYI_D6004;
		else if (tool_str_strstr(param_ability8_d2c->data, "D6008"))
			ability->base.device_type = PTC_DEVTYPE_HUYYI_D6008;
		else if (tool_str_strstr(param_ability8_d2c->data, "D7108"))
			ability->base.device_type = PTC_DEVTYPE_HUYYI_D7108;
		else if (tool_str_strstr(param_ability8_d2c->data, "D7116"))
			ability->base.device_type = PTC_DEVTYPE_HUYYI_D7116;
		else
			ability->base.device_type = PTC_DEVTYPE_HUAYI_BASE;
	}while(0);
	nc_hy_dev_logout(&dev);
	dev.state = 0;
	return ret;	
}

TOOL_INT32 nc_hy_getDeviceStatus_v2(nc_dev* info, ptc_cfg_status_t* status)
{
	if (g_nc_hy == NULL)
	{
		log_error("nc");
		return NC_ERROR_UNINIT;
	}
	if (info == NULL || status == NULL)
	{
		log_error("info(0x%08x), status(0x%08x)", info, status);
		return NC_ERROR_INPUT;
	}

	TOOL_INT32 i = 0;
	TOOL_INT32 ret = 0;
	tool_thread_lockMutex(&g_nc_hy->mutex);
	for (i = 0; i < NC_HY_USER_SIZE; i++)
	{
		if (tool_str_strncmp(g_nc_hy->dev[i].ip_addr, info->ip, PTC_IP_SIZE-1) == 0 &&
			tool_str_strncmp(g_nc_hy->dev[i].user_name, info->user, PTC_ID_SIZE-1) == 0 &&
			tool_str_strncmp(g_nc_hy->dev[i].user_pswd, info->pswd, PTC_ID_SIZE-1) == 0 &&			
			g_nc_hy->dev[i].port == info->msg_port &&
			g_nc_hy->dev[i].state)
		{
			ret = nc_hy_dev_getStatus(&g_nc_hy->dev[i], status);
			tool_thread_unlockMutex(&g_nc_hy->mutex);
			return ret;
		}
	}
	for (i = 0; i < NC_HY_USER_SIZE; i++)
	{
		if (g_nc_hy->dev[i].state == 0)
			break;
	}
	if (i == NC_HY_USER_SIZE)
	{
		log_error("nc full");
		tool_thread_unlockMutex(&g_nc_hy->mutex);
		return NC_ERROR_FULL;
	}
	
	ret = nc_hy_dev_start(&g_nc_hy->dev[i], info->ip, info->msg_port, info->user, info->pswd);
	if (ret < 0)
	{
		log_error("nc_hy_dev_start(%s:%d)", info->ip, info->msg_port);
		tool_thread_unlockMutex(&g_nc_hy->mutex);
		return ret;
	}
	ret = nc_hy_dev_getStatus(&g_nc_hy->dev[i], status);
	if (ret < 0)
	{
		log_error("nc_hy_dev_getStatus ret(%d)", ret);
//		nc_hy_dev_stop(&g_nc_hy->dev[i]);
	}
	tool_thread_unlockMutex(&g_nc_hy->mutex);
	return ret;		
}

TOOL_INT32 nc_hy_setDeviceStatus_v2(nc_dev* info, ptc_cfg_status_t* status)
{
	if (g_nc_hy == NULL)
	{
		log_error("nc");
		return NC_ERROR_UNINIT;
	}
	if (info == NULL || status == NULL)
	{
		log_error("info(0x%08x), status(0x%08x)", info, status);
		return NC_ERROR_INPUT;
	}

	TOOL_INT32 i = 0;
	TOOL_INT32 ret = 0;
	tool_thread_lockMutex(&g_nc_hy->mutex);
	for (i = 0; i < NC_HY_USER_SIZE; i++)
	{
		if (tool_str_strncmp(g_nc_hy->dev[i].ip_addr, info->ip, PTC_IP_SIZE-1) == 0 &&
			tool_str_strncmp(g_nc_hy->dev[i].user_name, info->user, PTC_ID_SIZE-1) == 0 &&
			tool_str_strncmp(g_nc_hy->dev[i].user_pswd, info->pswd, PTC_ID_SIZE-1) == 0 &&			
			g_nc_hy->dev[i].port == info->msg_port &&
			g_nc_hy->dev[i].state)
		{
			ret = nc_hy_dev_setStatus(&g_nc_hy->dev[i], status);
			tool_thread_unlockMutex(&g_nc_hy->mutex);
			return ret;
		}
	}
	for (i = 0; i < NC_HY_USER_SIZE; i++)
	{
		if (g_nc_hy->dev[i].state == 0)
			break;
	}
	if (i == NC_HY_USER_SIZE)
	{
		log_error("nc full");
		tool_thread_unlockMutex(&g_nc_hy->mutex);
		return NC_ERROR_FULL;
	}
	
	ret = nc_hy_dev_start(&g_nc_hy->dev[i], info->ip, info->msg_port, info->user, info->pswd);
	if (ret < 0)
	{
		log_error("nc_hy_dev_start(%s:%d)", info->ip, info->msg_port);
		tool_thread_unlockMutex(&g_nc_hy->mutex);
		return ret;
	}
	ret = nc_hy_dev_setStatus(&g_nc_hy->dev[i], status);
	if (ret < 0)
	{
		log_error("nc_hy_dev_setStatus ret(%d)", ret);
//		nc_hy_dev_stop(&g_nc_hy->dev[i]);
	}
	tool_thread_unlockMutex(&g_nc_hy->mutex);
	return ret;			
}

TOOL_INT32 nc_hy_syncTime(nc_dev* info, TOOL_UINT32 utc_sec, TOOL_INT32 zone, TOOL_INT32 dst_hour)
{
	if (g_nc_hy == NULL)
	{
		log_error("nc");
		return NC_ERROR_UNINIT;
	}
	if (info == NULL)
	{
		log_error("info");
		return NC_ERROR_INPUT;
	}

	TOOL_INT32 i = 0;
	TOOL_INT32 ret = 0;
	tool_thread_lockMutex(&g_nc_hy->mutex);
	for (i = 0; i < NC_HY_USER_SIZE; i++)
	{
		if (tool_str_strncmp(g_nc_hy->dev[i].ip_addr, info->ip, PTC_IP_SIZE-1) == 0 &&
			tool_str_strncmp(g_nc_hy->dev[i].user_name, info->user, PTC_ID_SIZE-1) == 0 &&
			tool_str_strncmp(g_nc_hy->dev[i].user_pswd, info->pswd, PTC_ID_SIZE-1) == 0 &&			
			g_nc_hy->dev[i].port == info->msg_port &&
			g_nc_hy->dev[i].state)
		{
			ret = nc_hy_dev_syncTime(&g_nc_hy->dev[i], utc_sec);
			tool_thread_unlockMutex(&g_nc_hy->mutex);
			return ret;
		}
	}
	for (i = 0; i < NC_HY_USER_SIZE; i++)
	{
		if (g_nc_hy->dev[i].state == 0)
			break;
	}
	if (i == NC_HY_USER_SIZE)
	{
		log_error("nc full");
		tool_thread_unlockMutex(&g_nc_hy->mutex);
		return NC_ERROR_FULL;
	}

	ret = nc_hy_dev_start(&g_nc_hy->dev[i], info->ip, info->msg_port, info->user, info->pswd);
	if (ret < 0)
	{
		log_error("nc_hy_dev_start(%s:%d)", info->ip, info->msg_port);
		tool_thread_unlockMutex(&g_nc_hy->mutex);
		return ret;
	}
	ret = nc_hy_dev_syncTime(&g_nc_hy->dev[i], utc_sec);
	tool_thread_unlockMutex(&g_nc_hy->mutex);
	return ret;	
}

#if defined __cplusplus
}
#endif

