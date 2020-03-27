
#if defined __cplusplus
extern "C"
{
#endif

#include <stdio.h>
#include "nc.h"
#include "xm_ptc.h"
#include "nc_xm.h"
#include "tool_log.h"
#include "tool_type.h"
#include "tool_sysf.h"
#include "tool_stream2.h"
#include "tool_stream3.h"
#include "tool_sock.h"
#include "tool_ptc.h"
#include "tool_md5.h"
#include "tool_http.h"
#include "tool_json.h"

#define	NC_XM_HEARTBEAT_CYCLE		5
#define	NC_XM_RECONNECT_CYCLE		5

#define NC_XM_RTS_POOL_SIZE			(4*1024*1024)
#define NC_XM_PBS_POOL_SIZE			(10*1024*1024)

#define NC_XM_DEVICE_SIZE			10
#define NC_XM_CHANNEL_SIZE			32
#define NC_XM_CHANNEL_PBS			32
#define NC_XM_CHANNEL_TALK			33
#define NC_XM_STREAM_SIZE			2

	
typedef struct
{
	TOOL_INT32 rts_id;
	TOOL_INT32 channel_no;
	TOOL_INT32 stream_type;
	TOOL_INT32 state;			//0=offline,1=online,2=recv
	TOOL_THREAD play_pid;
	TOOL_THREAD recv_pid;	
	TOOL_INT32 online;
	TOOL_VOID* father_dev;
	
	tool_sock_item_t rts_sock;
	tool_stream3_t stream;
	tool_stream3_pos_t read_pos;
	TOOL_UINT8* recv_buf;
	TOOL_UINT32 recv_len;
	ptc_frame_video_t* cb_frame;
	TOOL_UINT8* left_buf;
	TOOL_UINT32 left_len;
	
//	TOOL_MUTEX mutex;		// for msg_sock
	TOOL_INT32 key_flag;	
	TOOL_INT32 error_flag;

//	xm_msg_startrts_d2c_t startrts_d2c;
	TOOL_INT32 tick;

	NC_CB cb;
	TOOL_VOID* param;	
}nc_xm_rts_t;

typedef struct
{
	TOOL_INT32 pbs_no;
	ptc_cfg_record_t ptc_record;
	TOOL_INT32 state;			//0=offline,1=online,2=recv
	TOOL_THREAD play_pid;
	TOOL_THREAD recv_pid;	
	TOOL_INT32 online;
	TOOL_VOID* father_dev;
	
	tool_sock_item_t pbs_sock;
	tool_stream3_t stream;
	tool_stream3_pos_t read_pos;
	TOOL_UINT8* recv_buf;
	TOOL_UINT32 recv_len;
	ptc_frame_video_t* cb_frame;
	TOOL_UINT8* left_buf;
	TOOL_UINT32 left_len;
//	TOOL_UINT32 full_flag;
//	TOOL_MUTEX mutex;		// for msg_sock
	TOOL_INT32 key_flag;
	TOOL_INT32 error_flag;
	TOOL_UINT32 history_pos;
	TOOL_INT32 end_flag;

//	xm_msg_startpbs_d2c_t startpbs_d2c;
	TOOL_INT32 tick_sec;
	TOOL_INT32 reconnect_tick_sec;
//	TOOL_UINT32 u32PlayTime;

	NC_CB cb;
	TOOL_VOID* param;	
}nc_xm_pbs_t;

typedef struct
{
	TOOL_INT32 state;
	TOOL_THREAD recv_pid;
	TOOL_THREAD send_pid;
	TOOL_INT32 talk_no;
	TOOL_VOID* father_dev;
	TOOL_INT32 tick;
	TOOL_INT32 online;
	
	tool_stream2_t* send_stream;
	TOOL_UINT32 send_pos;
	ptc_frame_audio_t send_frame;
	ptc_frame_audio_t recv_frame;
	TOOL_UINT32 send_len;
	NC_CB cb;
	TOOL_VOID* param;
	TOOL_MUTEX mutex;		// for cb & param
	TOOL_INT32 error_flag;
	TOOL_INT32 recv_flag;

//	xm_msg_starttalk_d2c_t starttalk_d2c;
	tool_sock_item_t talk_sock;
}nc_xm_talk_t;


typedef struct 
{
	TOOL_INT32 dev_id;		//*
	TOOL_VOID* father_nc;	//*
	TOOL_MUTEX mutex;		// for msg_sock

	TOOL_INT32 state;		//*
	TOOL_INT32 online;
	nc_dev	st_nc_dev;
//	NC_PTC_E ptc;
//	TOOL_INT8 scan_id[PTC_STR_SIZE];
//	TOOL_INT8 ip_addr[PTC_IP_SIZE];		//*
//	TOOL_INT32 msg_port;					//*
//	TOOL_INT32 stream_port;
//	TOOL_INT8 user_name[PTC_ID_SIZE];	//*
//	TOOL_INT8 user_pswd[PTC_ID_SIZE];	//*
	TOOL_INT32 state_tick_sec;	
	TOOL_INT32 state_tick_usec;

	tool_sock_item_t msg_sock;			//*
	xm_msg_t xm_msg;					//*
	xm_search_t* pst_xm_search;			//*
	TOOL_INT32 session_id;				//»á»°ID
	TOOL_INT32 channel_num;
	TOOL_INT32 video_in_num;
	TOOL_INT32 audio_in_num;
	TOOL_INT32 alarm_in_num;
	TOOL_INT32 alarm_out_num;	
//	xm_search_t* xm_search;

	//login
	TOOL_THREAD login_pid;	
	TOOL_INT32 hb_tick_sec;	
	TOOL_INT32 hb_tick_usec;	
	TOOL_INT32 is_sleep;

	//rts
	nc_xm_rts_t rts[NC_XM_CHANNEL_SIZE][NC_XM_STREAM_SIZE];
	TOOL_INT32 cur_rts_enable[NC_XM_CHANNEL_SIZE][NC_XM_STREAM_SIZE];

	//pbs
	nc_xm_pbs_t pbs;
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
	nc_xm_talk_t talk;
	TOOL_INT32 cur_talk_enable;

	//time
	TOOL_INT32 last_time_req;
	TOOL_INT32 cur_time_req;
	TOOL_UINT32 utc_sec;
	TOOL_INT32 zone;
	TOOL_INT32 dst_hour;

	//cfg-records
	TOOL_INT32 last_records_req;
	TOOL_INT32 cur_records_req;
	ptc_cfg_records_t cfg_records;
	TOOL_INT32 result_records;
	TOOL_COND cond_records;
	//cfg-logs
	TOOL_INT32 last_logs_v2_req;
	TOOL_INT32 cur_logs_v2_req;
	ptc_logs_v2_t cfg_logs_v2;
	TOOL_INT32 result_logs_v2;
	TOOL_COND cond_logs_v2;
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
}nc_xm_dev_t;


typedef struct
{
	nc_xm_dev_t dev[NC_XM_DEVICE_SIZE];
	TOOL_MUTEX mutex;

	TOOL_INT32 state;
	TOOL_THREAD pid;

	TOOL_INT32 buffer_usec;
}nc_xm_t;

nc_xm_t* g_nc_xm = NULL;

TOOL_INT32 nc_xm_dev_recvMsg(nc_xm_dev_t* dev, TOOL_INT32 cmd)
{
	TOOL_INT32 ret = 0;
	while (1)
	{
		ret = tool_ptc_xm_recvMsg(&dev->msg_sock, &dev->xm_msg);
		if (ret < 0)
			break;
		/*
		if (dev->xm_msg.head.cmd == HY_CMD_ALARM)
		{
			log_state("HY_CMD_ALARM");
			nc_xm_dev_doAlarm(dev);
		}
		*/
		
		if (dev->xm_msg.head.command_id == cmd || cmd == XM_CMD_ALL)
			break;		
		log_state("invalid command_id(0x%04x) cmd(0x%04x)", dev->xm_msg.head.command_id, cmd);
	}
	return ret;
}

TOOL_INT32 nc_xm_dev_recvSearch(nc_xm_dev_t* dev, TOOL_INT32 cmd)
{
	TOOL_INT32 ret = 0;
	while (1)
	{
		ret = tool_ptc_xm_recvSearch(&dev->msg_sock, dev->pst_xm_search);
		if (ret < 0)
			break;
		/*
		if (dev->xm_msg.head.cmd == HY_CMD_ALARM)
		{
			log_state("HY_CMD_ALARM");
			nc_xm_dev_doAlarm(dev);
		}
		*/
		
		if (dev->pst_xm_search->head.command_id == cmd || cmd == XM_CMD_ALL)
			break;		
		log_state("invalid command_id(0x%04x) cmd(0x%04x)", dev->pst_xm_search->head.command_id, cmd);
	}
	return ret;
}


#define __NC_XM_RTS__


TOOL_VOID nc_xm_rts_dostate(nc_xm_rts_t* rts, TOOL_INT32 type)
{
	ptc_frame_head_t head;
	tool_mem_memset(&head, sizeof(ptc_frame_head_t));
	head.frame_type = type;
//	log_debug("type(0x%08x)", type);
	tool_stream3_set_noDrop(&rts->stream, &head, sizeof(ptc_frame_head_t), NULL);	
}

TOOL_VOID* nc_xm_rts_runRecv(TOOL_VOID* param)
{
	nc_xm_rts_t* rts = (nc_xm_rts_t*)param;
	nc_xm_dev_t* dev = (nc_xm_dev_t*)rts->father_dev;
	nc_xm_t* nc = (nc_xm_t*)dev->father_nc;
	log_state("dev(%d) nc_xm_rts_runRecv(%d)(%d) start", dev->dev_id, rts->channel_no, rts->stream_type);
	TOOL_INT32 ret = 0;
	rts->key_flag = 0;
	rts->recv_len = 0;
	rts->left_len = 0;
	TOOL_INT32 rts_online = 0;
	ptc_frame_head_t* pst_frame_head = NULL;
	TOOL_UINT32 frame_len = 0;

//	TOOL_INT32 tick = 0;

	while (rts->state)
	{
		if (rts_online != rts->online)
		{
			rts_online = rts->online;
			if (rts->online)
				nc_xm_rts_dostate(rts, rts_online);
		}

/*
		tool_sock_setTick(&tick);
		if (tick == 10  && rts->channel_no == 1)
		{
			log_debug("tool_sock_setTick 10---------------------------");
			rts->error_flag = 1;
			break;
		}
*/		
		ret = tool_sock_item_pollRead(&rts->rts_sock, 0);
		if (ret < 0)
		{
			log_error("tool_sock_item_pollRead error");
			rts->key_flag = 0;
			rts->recv_len = 0;
			rts->left_len = 0;
			rts->error_flag = 1;
			break;
		}
		else if (ret == 0)
		{
			tool_sysf_usleep(20*1000);
			continue;
		}

		ret = tool_ptc_xm_recvRts(&rts->rts_sock, rts->recv_buf, &rts->recv_len, PTC_VIDEO_FRAME_SIZE, &pst_frame_head, &frame_len, &rts->left_buf, &rts->left_len);
		if (ret < 0)
		{
			log_error("tool_ptc_xm_recvRts error");
			rts->key_flag = 0;
			rts->recv_len = 0;
			rts->left_len = 0;
			rts->error_flag = 1;
			break;
		}
		else if (ret == 0)
			continue;

		if (rts->key_flag == 0)
		{
			if (pst_frame_head->frame_type != PTC_FRAME_I)
			{
				continue;
			}
			else
				rts->key_flag = 1;
		}		
//		log_debug("pst_frame_head->len(%d)", pst_frame_head->len);
		tool_stream3_set_withDrop(&rts->stream, pst_frame_head, frame_len, nc->buffer_usec);
	}

	log_state("dev(%d) nc_xm_rts_runRecv(%d)(%d) stop", dev->dev_id, rts->channel_no, rts->stream_type);
    return NULL;	
}

TOOL_VOID* nc_xm_rts_runPlay(TOOL_VOID* param)
{
	nc_xm_rts_t* rts = (nc_xm_rts_t*)param;
	nc_xm_dev_t* dev = (nc_xm_dev_t*)rts->father_dev;
	log_state("dev(%d) nc_xm_rts_runPlay(%d)(%d) start", dev->dev_id, rts->channel_no, rts->stream_type);
	nc_xm_t* nc = (nc_xm_t*)dev->father_nc;
	TOOL_UINT32 len = 0;

	while (rts->state)
	{					
		len = PTC_VIDEO_FRAME_SIZE;
		if (tool_stream3_get_withRts(&rts->stream, &rts->read_pos, rts->cb_frame, &len, nc->buffer_usec) == 0)
		{
			tool_sysf_usleep(20*1000);
			continue;
		}
/*
		log_debug("(%010d.%06d) no(%08d) type(%d) rate(%02d) len(%06d) res(%04dx%04d)", 
			rts->cb_frame->frame_head.sec, rts->cb_frame->frame_head.usec, rts->cb_frame->frame_head.no, 
			rts->cb_frame->frame_head.frame_type, rts->cb_frame->frame_head.frame_sub_type, 
			rts->cb_frame->frame_head.frame_rate, rts->cb_frame->frame_head.len,
			rts->cb_frame->frame_head.width, rts->cb_frame->frame_head.height);
*/		
		if (rts->cb)
		{
			rts->cb(rts->rts_id, rts->cb_frame, len, rts->param);
		}
	}

	log_state("dev(%d) nc_xm_rts_runPlay(%d)(%d) stop", dev->dev_id, rts->channel_no, rts->stream_type);
	return NULL;
}

TOOL_INT32 nc_xm_rts_start(nc_xm_rts_t* rts)
{
//	log_state("nc_xm_rts_start");
	nc_xm_dev_t* dev = (nc_xm_dev_t*)rts->father_dev;
	TOOL_INT8 stream_type_str[NC_XM_STREAM_SIZE][16] = {"Main", "Extra1"}; 
	TOOL_INT32 ret = 0;
	TOOL_INT32 xm_ret = 0;	
	xm_msg_t xm_msg;
	do
	{
		rts->state = 1;
		if (tool_sock_item_open_v2(&rts->rts_sock, TOOL_SOCK_TYPE1_TCP, TOOL_SOCK_TYPE2_CLIENT, dev->st_nc_dev.ip, dev->st_nc_dev.msg_port, NULL, 0) < 0)
		{
			log_error("tool_sock_item_open RTS");
			ret = -1;
			break;
		}
		tool_mem_memset(&xm_msg.head, sizeof(xm_msg_head_t));
		xm_msg.head.session_id = dev->session_id;
		xm_msg.head.command_id = XM_CMD_LOGIN_RTS_REQ;
		xm_msg.head.data_len = snprintf((TOOL_INT8*)xm_msg.data, sizeof(xm_msg.data),
			"{ "
				"\"Name\" : \"OPMonitor\", "
				"\"OPMonitor\" : "
				"{ "
					"\"Action\" : \"Claim\", "
					"\"Parameter\" : "
					"{ "
						"\"Channel\" : %d, "
						"\"CombinMode\" : \"NONE\", "
						"\"StreamType\" : \"%s\", "
						"\"TransMode\" : \"TCP\" "
					"} "
				"}, "
				"\"SessionID\" : \"0x%x\" "
			"}\n",
			rts->channel_no, stream_type_str[rts->stream_type],	dev->session_id);
		if (tool_ptc_xm_sendMsg(&rts->rts_sock, &xm_msg) < 0)
		{
			log_error("tool_ptc_xm_sendMsg");
			ret = -1;
			break;
		}
		if (tool_ptc_xm_recvMsg(&rts->rts_sock, &xm_msg) < 0 || xm_msg.head.command_id != XM_CMD_LOGIN_RTS_RSP)
		{
			log_error("nc_xm_recv xm_msg.head.command_id(%d)", xm_msg.head.command_id);
			ret = -1;
			break;
		}
		if (tool_json2_getNumber(xm_msg.data, "Ret", &xm_ret) < 0 || xm_ret != XM_RET_OK)
		{
			log_error("nc_xm_dev_recvMsg(%s) ret(%d)", xm_msg.data, xm_ret);
			ret = -1;
			break;
		}

		tool_mem_memset(&dev->xm_msg, sizeof(xm_msg_head_t));
		dev->xm_msg.head.session_id = dev->session_id;
		dev->xm_msg.head.command_id = XM_CMD_CTRL_RTS_REQ;
		dev->xm_msg.head.data_len = snprintf((TOOL_INT8*)dev->xm_msg.data, sizeof(dev->xm_msg.data),
			"{ "
				"\"Name\" : \"OPMonitor\", "
				"\"OPMonitor\" : "
				"{ "
					"\"Action\" : \"Start\", "
					"\"Parameter\" : "
					"{ "
						"\"Channel\" : %d, "
						"\"CombinMode\" : \"NONE\", "
						"\"StreamType\" : \"%s\", "
						"\"TransMode\" : \"TCP\" "
					"} "
				"}, "
				"\"SessionID\" : \"0x%x\" "
			"}\n",
			rts->channel_no, stream_type_str[rts->stream_type],	dev->session_id);
		if (tool_ptc_xm_sendMsg(&dev->msg_sock, &dev->xm_msg) < 0)
		{
			log_error("tool_ptc_xm_sendMsg");
			ret = -1;
			break;
		}
		if (nc_xm_dev_recvMsg(dev, XM_CMD_CTRL_RTS_RSP) < 0)
		{
			log_error("nc_xm_recv");
			ret = -1;
			break;
		}
		if (tool_json2_getNumber(dev->xm_msg.data, "Ret", &xm_ret) < 0)
		{
			log_error("nc_xm_dev_recvMsg(%s)", dev->xm_msg.data);
			ret = -1;
			break;
		}
		if (xm_ret != XM_RET_OK)
		{
			log_error("ret(%d)", xm_ret);
			if (xm_ret == XM_RET_ERROR_NO_RIGHT)
			{
				rts->online = PTC_FRAME_STREAM_RIGHT;
			}
		}

		
		tool_stream3_init(&rts->stream, NC_XM_RTS_POOL_SIZE);
		rts->recv_buf = (TOOL_UINT8*)tool_mem_malloc(PTC_VIDEO_FRAME_SIZE, 0);
		rts->cb_frame = (ptc_frame_video_t*)tool_mem_malloc(sizeof(ptc_frame_video_t), 0);
		tool_stream3_initReadPos(&rts->stream, &rts->read_pos);
		if (tool_thread_create(&rts->play_pid, NULL, nc_xm_rts_runPlay, (TOOL_VOID*)rts) < 0)
			log_fatalNo("tool_thread_create");
		if (tool_thread_create(&rts->recv_pid, NULL, nc_xm_rts_runRecv, (TOOL_VOID*)rts) < 0)
			log_fatalNo("tool_thread_create");
	}while (0);
	if (ret < 0)
	{		
		rts->state = 0;
		if (tool_sock_item_isLive(&rts->rts_sock))
			tool_sock_item_close(&rts->rts_sock);
	}
	return ret;
}

TOOL_INT32 nc_xm_rts_stop(nc_xm_rts_t* rts)
{
	if (rts->state)
	{
		rts->state = 0;
		tool_thread_join(rts->play_pid);
		tool_thread_join(rts->recv_pid);
		tool_mem_free(rts->recv_buf);
		rts->recv_buf = NULL;
		tool_mem_free(rts->cb_frame);
		rts->cb_frame = NULL;
		tool_stream3_done(&rts->stream);
	}
	rts->error_flag = 0;
	
	nc_xm_dev_t* dev = (nc_xm_dev_t*)rts->father_dev;
	TOOL_INT8 stream_type_str[NC_XM_STREAM_SIZE][16] = {"Main", "Extra1"}; 
	TOOL_INT32 xm_ret = 0;	
	tool_mem_memset(&dev->xm_msg.head, sizeof(xm_msg_head_t));
	dev->xm_msg.head.session_id = dev->session_id;
	dev->xm_msg.head.command_id = XM_CMD_CTRL_RTS_REQ;
	dev->xm_msg.head.data_len = snprintf((TOOL_INT8*)dev->xm_msg.data, sizeof(dev->xm_msg.data),
		"{ "
			"\"Name\" : \"OPMonitor\", "
			"\"OPMonitor\" : "
			"{ "
				"\"Action\" : \"Stop\", "
				"\"Parameter\" : "
				"{ "
					"\"Channel\" : %d, "
					"\"CombinMode\" : \"NONE\", "
					"\"StreamType\" : \"%s\", "
					"\"TransMode\" : \"TCP\" "
				"} "
			"}, "
			"\"SessionID\" : \"0x%x\" "
		"}\n",
		rts->channel_no, stream_type_str[rts->stream_type], dev->session_id);
	if (tool_ptc_xm_sendMsg(&dev->msg_sock, &dev->xm_msg) < 0)
	{
		log_error("tool_ptc_xm_sendMsg");
		return -1;
	}
	if (nc_xm_dev_recvMsg(dev, XM_CMD_CTRL_RTS_RSP) < 0)
	{
		log_error("nc_xm_recv");
		return -1;
	}
	if (tool_json2_getNumber(dev->xm_msg.data, "Ret", &xm_ret) < 0 || xm_ret != XM_RET_OK)
	{
		log_error("nc_xm_dev_recvMsg(%s) ret(%d)", dev->xm_msg.data, xm_ret);
//		return -1;
	}

	if (tool_sock_item_isLive(&rts->rts_sock))
		tool_sock_item_close(&rts->rts_sock);
	return 0;
}


#define __NC_XM_PBS__

TOOL_VOID nc_xm_pbs_doState(nc_xm_pbs_t* pbs, TOOL_INT32 type)
{
	ptc_frame_head_t head;
	tool_mem_memset(&head, sizeof(ptc_frame_head_t));
	head.frame_type = type;
//	log_debug("type(0x%08x)", type);
	tool_stream3_set_noDrop(&pbs->stream, &head, sizeof(ptc_frame_head_t), NULL);
}

TOOL_VOID* nc_xm_pbs_runPlay(TOOL_VOID* param)
{
	nc_xm_pbs_t* pbs = (nc_xm_pbs_t*)param;
	nc_xm_dev_t* dev = (nc_xm_dev_t*)pbs->father_dev;
	TOOL_INT8 begin_time_str[32] = {0};
	TOOL_INT8 end_time_str[32] = {0};
	tool_sysf_tt2str(dev->pbs.ptc_record.start_time, 0, begin_time_str, sizeof(begin_time_str));
	tool_sysf_tt2str(dev->pbs.ptc_record.end_time, 0, end_time_str, sizeof(end_time_str));	
	
	log_state("dev(%d) nc_xm_pbs_runPlay(%s)(%s) file(%s) start", dev->dev_id, begin_time_str, end_time_str, pbs->ptc_record.file_info);
	TOOL_UINT32 len = 0;
	pbs->history_pos = 0;

	tool_stream3_initReadPos(&dev->pbs.stream, &dev->pbs.read_pos);
	while (pbs->state)
	{		
		len = PTC_VIDEO_FRAME_SIZE;
		if (tool_stream3_get_withPbs(&pbs->stream, &pbs->read_pos, pbs->cb_frame, &len) == 0)
		{
			tool_sysf_usleep(20*1000);
			continue;
		}
		if (pbs->cb)
		{
			pbs->cb(dev->st_nc_dev.ptc | (dev->dev_id<<8) | pbs->pbs_no, pbs->cb_frame, len, pbs->param);
			if (pbs->cb_frame->frame_head.frame_type == PTC_FRAME_I || pbs->cb_frame->frame_head.frame_type == PTC_FRAME_P)
				pbs->history_pos = pbs->cb_frame->frame_head.sec;
		}
	}
	pbs->history_pos = 0;
	log_state("dev(%d) nc_xm_pbs_runPlay(%s)(%s) file(%s) stop", dev->dev_id, begin_time_str, end_time_str, pbs->ptc_record.file_info);
	return NULL;
}

TOOL_VOID* nc_xm_pbs_runRecv(TOOL_VOID* param)
{
	nc_xm_pbs_t* pbs = (nc_xm_pbs_t*)param;
	nc_xm_dev_t* dev = (nc_xm_dev_t*)pbs->father_dev;
	TOOL_INT32 ret = 0;
	pbs->key_flag = 0;
	pbs->recv_len = 0;
	pbs->left_len = 0;
	pbs->end_flag = 0;
	TOOL_INT32 pbs_online = 0;
	ptc_frame_head_t* pst_frame_head = NULL;
	TOOL_UINT32 frame_len = 0;

//	TOOL_INT32 tick = 0;

	while (pbs->state)
	{	
		if (pbs_online != pbs->online)
		{
			pbs_online = pbs->online;
			if (pbs->online)
				nc_xm_pbs_doState(pbs, pbs_online);
		}

/*
		tool_sock_setTick(&tick);
		if (tick == 20)
		{
			log_debug("tool_sock_setTick 10---------------------------");
			pbs->error_flag = 1;
			break;
		}
*/
		ret = tool_sock_item_pollRead(&dev->pbs.pbs_sock, 0);
		if (ret < 0)
		{
			pbs->error_flag = 1;
			pbs->key_flag = 0;
			pbs->recv_len = 0;
			pbs->left_len = 0;
			break;
		}
		else if (ret == 0)
		{
			if (tool_sock_item_isTimeout(&pbs->pbs_sock, 4) && pbs->end_flag == 0)
			{
				nc_xm_pbs_doState(pbs, PTC_FRAME_PBS_END);
				pbs->end_flag = 1;
			}
			tool_sysf_usleep(20*1000);
			continue;
		}
		ret = tool_ptc_xm_recvRts(&pbs->pbs_sock, pbs->recv_buf, &pbs->recv_len, PTC_VIDEO_FRAME_SIZE, &pst_frame_head, &frame_len, &pbs->left_buf, &pbs->left_len);
		if (ret < 0)
		{
			log_error("tool_ptc_rtp_recvTcp_video error");
			pbs->error_flag = 1;
			pbs->key_flag = 0;
			pbs->recv_len = 0;
			pbs->left_len = 0;
			break;
		}
		else if (ret == 0)
			continue;
		
//		pbs->recv_frame->frame_head.sec = pbs->u32PlayTime;
//		log_debug("frame_time(%s) len(%d)", tool_sysf_dateLocal_1(pbs->recv_frame->frame_head.sec), pbs->recv_frame->frame_head.len);
//		continue;
/*
		if (pst_frame_head->sec < dev->pbs.ptc_record.start_time)
		{
			log_debug("frame_time(%s) start_time(%s)", pst_frame_head->sec, dev->pbs.ptc_record.start_time);
			continue;
		}
		if (pst_frame_head->sec > dev->pbs.ptc_record.end_time)
		{
			log_debug("frame_time(%s) end_time(%s)", pst_frame_head->sec, dev->pbs.ptc_record.end_time);
		//	continue;
		}
*/
//		log_debug("pbs->recv_frame->frame_head.frame_type(%d)", pbs->recv_frame->frame_head.frame_type);
		if (dev->pbs.key_flag == 0)
		{
			if (pst_frame_head->frame_type != PTC_FRAME_I)
			{
				continue;
			}
			else
				dev->pbs.key_flag = 1;
		}
		tool_stream3_set_noDrop(&dev->pbs.stream, pst_frame_head, frame_len, &dev->cur_pbs_enable);
	}

	log_state("dev(%d) RecvPbs stop", dev->dev_id);
    return NULL;	   
}

TOOL_INT32 nc_xm_pbs_start(nc_xm_pbs_t* pbs, ptc_cfg_record_t* pst_cfg_record)
{
	nc_xm_dev_t* dev = (nc_xm_dev_t*)pbs->father_dev;	
	TOOL_INT32 ret = 0;
	xm_msg_t xm_msg;
	TOOL_INT32 xm_ret = 0;
	do
	{
		//start pbs
		pbs->state = 1;		
		if (tool_sock_item_open_v2(&pbs->pbs_sock, TOOL_SOCK_TYPE1_TCP, TOOL_SOCK_TYPE2_CLIENT, dev->st_nc_dev.ip, dev->st_nc_dev.msg_port, NULL, 0) < 0)
		{
			log_error("tool_sock_item_open PBS");
			ret = -1;
			break;
		}
		TOOL_INT8 begin_time_str[32] = {0};
		TOOL_INT8 end_time_str[32] = {0};
		tool_sysf_tt2str(pst_cfg_record->start_time, 0, begin_time_str, sizeof(begin_time_str));
		tool_sysf_tt2str(pst_cfg_record->end_time, 0, end_time_str, sizeof(end_time_str));
		tool_mem_memset(&xm_msg.head, sizeof(xm_msg_head_t));
		xm_msg.head.session_id = dev->session_id;
		xm_msg.head.command_id = XM_CMD_LOGIN_PBS_REQ;
		xm_msg.head.data_len = snprintf((TOOL_INT8*)xm_msg.data, sizeof(xm_msg.data),			
			"{ "
				"\"Name\" : \"OPPlayBack\", "
				"\"OPPlayBack\" : "
				"{ "
					"\"Action\" : \"Claim\", "
					"\"EndTime\" : \"%s\", "
					"\"Parameter\" : "
					"{ "
						"\"FileName\" : \"%s\", "
						"\"PlayMode\" : \"ByName\", "
						"\"Stream_Type\" : 1, "
						"\"TransMode\" : \"TCP\", "
						"\"Value\" : 0 "
					"}, "
					"\"StartTime\" : \"%s\" "
				"}, "
				"\"SessionID\" : \"0x%x\" "
			"}",
			end_time_str, pst_cfg_record->file_info, begin_time_str, dev->session_id);
		if (tool_ptc_xm_sendMsg(&pbs->pbs_sock, &xm_msg) < 0)
		{
			log_error("tool_ptc_xm_sendMsg");
			ret = -1;
			break;
		}
		if (tool_ptc_xm_recvMsg(&pbs->pbs_sock, &xm_msg) < 0 || xm_msg.head.command_id != XM_CMD_LOGIN_PBS_RSP)
		{
			log_error("nc_xm_recv xm_msg.head.command_id(%d)", xm_msg.head.command_id);
			ret = -1;
			break;
		}
		if (tool_json2_getNumber(xm_msg.data, "Ret", &xm_ret) < 0 || xm_ret != XM_RET_OK)
		{
			log_error("nc_xm_dev_recvMsg(%s) ret(%d)", xm_msg.data, xm_ret);
			ret = -1;
			break;
		}

		tool_mem_memset(&dev->xm_msg, sizeof(xm_msg_head_t));
		dev->xm_msg.head.session_id = dev->session_id;
		dev->xm_msg.head.command_id = XM_CMD_CTRL_PBS_REQ;
		dev->xm_msg.head.data_len = snprintf((TOOL_INT8*)dev->xm_msg.data, sizeof(dev->xm_msg.data),
			"{ "
				"\"Name\" : \"OPPlayBack\", "
				"\"OPPlayBack\" : "
				"{ "
					"\"Action\" : \"Start\", "
					"\"EndTime\" : \"%s\", "
					"\"Parameter\" : "
					"{ "
						"\"FileName\" : \"%s\", "
						"\"PlayMode\" : \"ByName\", "
						"\"Stream_Type\" : 0, "
						"\"TransMode\" : \"TCP\", "
						"\"Value\" : 0 "
					"}, "
					"\"StartTime\" : \"%s\" "
				"}, "
				"\"SessionID\" : \"0x%x\" "
			"}",
			end_time_str, pst_cfg_record->file_info, begin_time_str, dev->session_id);
		if (tool_ptc_xm_sendMsg(&dev->msg_sock, &dev->xm_msg) < 0)
		{
			log_error("tool_ptc_xm_sendMsg");
			ret = -1;
			break;
		}
		if (nc_xm_dev_recvMsg(dev, XM_CMD_CTRL_PBS_RSP) < 0)
		{
			log_error("nc_xm_recv");
			ret = -1;
			break;
		}
		if (tool_json2_getNumber(dev->xm_msg.data, "Ret", &xm_ret) < 0)
		{
			log_error("nc_xm_dev_recvMsg(%s)", dev->xm_msg.data);
			ret = -1;
			break;
		}
		if (xm_ret != XM_RET_OK)
		{
			log_error("ret(%d)", xm_ret);
			if (xm_ret == XM_RET_ERROR_NO_RIGHT)
			{
				pbs->online = PTC_FRAME_STREAM_RIGHT;
			}
		}
		
		tool_stream3_init(&pbs->stream, NC_XM_PBS_POOL_SIZE);
		pbs->recv_buf = (TOOL_UINT8*)tool_mem_malloc(PTC_VIDEO_FRAME_SIZE, 0);
		pbs->cb_frame = (ptc_frame_video_t*)tool_mem_malloc(PTC_VIDEO_FRAME_SIZE, 0);
		if (tool_thread_create(&pbs->play_pid, NULL, nc_xm_pbs_runPlay, (TOOL_VOID*)pbs) < 0)
			log_fatalNo("tool_thread_create");
		if (tool_thread_create(&pbs->recv_pid, NULL, nc_xm_pbs_runRecv, (TOOL_VOID*)pbs) < 0)
			log_fatalNo("tool_thread_create");	
	}while (0);
	if (ret < 0)
	{		
		pbs->state = 0;
		if (tool_sock_item_isLive(&pbs->pbs_sock))
			tool_sock_item_close(&pbs->pbs_sock);
	}
	return ret;
}

/*
TOOL_INT32 nc_xm_pbs_get(nc_xm_pbs_t* pbs)
{
	if (tool_sock_isTickTimeout(pbs->tick_sec, 1) == 0)
		return 0;
	tool_sock_setTick(&pbs->tick_sec);
	
	nc_xm_dev_t* dev = (nc_xm_dev_t*)pbs->father_dev;	
	tool_mem_memset(&dev->xm_msg.head, sizeof(xm_msg_head_t));
	dev->xm_msg.head.u16Command = XM_MSG_ID_VOD_GET_PLAXMTATUS;
	dev->xm_msg.head.u32ContentLength = sizeof(xm_msg_getpbs_c2d_t);
	xm_msg_getpbs_c2d_t* pst_getpbs_c2d = (xm_msg_getpbs_c2d_t*)dev->xm_msg.data;
	tool_mem_memset(pst_getpbs_c2d, sizeof(xm_msg_getpbs_c2d_t));
	tool_ptc_xm_fillIeHead_getpbs_c2d(pst_getpbs_c2d);
	tool_mem_memcpy(&pst_getpbs_c2d->user_log_info, &dev->login_d2c.user_log_info, sizeof(xm_ie_user_log_info_t));
	tool_mem_memcpy(&pst_getpbs_c2d->task_no, &pbs->startpbs_d2c.task_no, sizeof(xm_ie_task_no_t));
	if (tool_ptc_xm_sendMsg(&dev->msg_sock, &dev->xm_msg) < 0)
	{
		log_error("tool_ptc_xm_sendMsg XM_MSG_ID_VOD_GET_PLAXMTATUS");
		return -1;
	}
	if (nc_xm_dev_recvMsg(dev, XM_MSG_ID_VOD_GET_PLAXMTATUS) < 0)
	{
		log_error("nc_xm_dev_recvMsg XM_MSG_ID_VOD_GET_PLAXMTATUS");
		return -1;
	}
	xm_msg_getpbs_d2c_t* pst_getpbs_d2c = (xm_msg_getpbs_d2c_t*)dev->xm_msg.data;
	log_state("u32Task_No(0x%08x) u32PlayTime(%d) u32PlayStatus(%d)", pst_getpbs_d2c->task_no.u32Task_No, 
		pst_getpbs_d2c->play_status.u32PlayTime, pst_getpbs_d2c->play_status.u32PlayStatus);
	pbs->u32PlayTime = pst_getpbs_d2c->play_status.u32PlayTime;
	return 0;	
}

TOOL_INT32 nc_xm_pbs_pause(nc_xm_pbs_t* pbs)
{
	nc_xm_dev_t* dev = (nc_xm_dev_t*)pbs->father_dev;	
	tool_mem_memset(&dev->xm_msg.head, sizeof(xm_msg_head_t));
	dev->xm_msg.head.u16Command = XM_MSG_ID_VOD_PLAY;
	dev->xm_msg.head.u32ContentLength = sizeof(xm_msg_setpbs_c2d_t);
	xm_msg_setpbs_c2d_t* pst_setpbs_c2d = (xm_msg_setpbs_c2d_t*)dev->xm_msg.data;
	tool_mem_memset(pst_setpbs_c2d, sizeof(xm_msg_setpbs_c2d_t));
	tool_ptc_xm_fillIeHead_setpbs_c2d(pst_setpbs_c2d);
	tool_mem_memcpy(&pst_setpbs_c2d->user_log_info, &dev->login_d2c.user_log_info, sizeof(xm_ie_user_log_info_t));
	tool_mem_memcpy(&pst_setpbs_c2d->task_no, &pbs->startpbs_d2c.task_no, sizeof(xm_ie_task_no_t));
	pst_setpbs_c2d->play_status.u32PlayTime = pbs->u32PlayTime;
	pst_setpbs_c2d->play_status.u32PlayStatus = XM_PBS_STATUS_PAUSE;
	if (tool_ptc_xm_sendMsg(&dev->msg_sock, &dev->xm_msg) < 0)
	{
		log_error("tool_ptc_xm_sendMsg XM_MSG_ID_VOD_PLAY XM_PBS_STATUS_PAUSE");
		return -1;
	}
	if (nc_xm_dev_recvMsg(dev, XM_MSG_ID_VOD_PLAY) < 0)
	{
		log_error("nc_xm_dev_recvMsg XM_MSG_ID_VOD_PLAY XM_PBS_STATUS_PAUSE");
		return -1;
	}
	log_state("nc_xm_pbs_pause (%d)", pbs->u32PlayTime);
	return 0;	
}

TOOL_INT32 nc_xm_pbs_resume(nc_xm_pbs_t* pbs)
{
	nc_xm_dev_t* dev = (nc_xm_dev_t*)pbs->father_dev;	
	tool_mem_memset(&dev->xm_msg.head, sizeof(xm_msg_head_t));
	dev->xm_msg.head.u16Command = XM_MSG_ID_VOD_PLAY;
	dev->xm_msg.head.u32ContentLength = sizeof(xm_msg_setpbs_c2d_t);
	xm_msg_setpbs_c2d_t* pst_setpbs_c2d = (xm_msg_setpbs_c2d_t*)dev->xm_msg.data;
	tool_mem_memset(pst_setpbs_c2d, sizeof(xm_msg_setpbs_c2d_t));
	tool_ptc_xm_fillIeHead_setpbs_c2d(pst_setpbs_c2d);
	tool_mem_memcpy(&pst_setpbs_c2d->user_log_info, &dev->login_d2c.user_log_info, sizeof(xm_ie_user_log_info_t));
	tool_mem_memcpy(&pst_setpbs_c2d->task_no, &pbs->startpbs_d2c.task_no, sizeof(xm_ie_task_no_t));
	pst_setpbs_c2d->play_status.u32PlayTime = pbs->u32PlayTime;
	pst_setpbs_c2d->play_status.u32PlayStatus = XM_PBS_STATUS_RESUME;
	if (tool_ptc_xm_sendMsg(&dev->msg_sock, &dev->xm_msg) < 0)
	{
		log_error("tool_ptc_xm_sendMsg XM_MSG_ID_VOD_PLAY XM_PBS_STATUS_RESUME");
		return -1;
	}
	if (nc_xm_dev_recvMsg(dev, XM_MSG_ID_VOD_PLAY) < 0)
	{
		log_error("nc_xm_dev_recvMsg XM_MSG_ID_VOD_PLAY XM_PBS_STATUS_RESUME");
		return -1;
	}
	log_state("nc_xm_pbs_resume (%d)", pbs->u32PlayTime);
	return 0;	
}
*/

TOOL_INT32 nc_xm_pbs_stop(nc_xm_pbs_t* pbs, ptc_cfg_record_t* pst_cfg_record)
{
	if (pbs->state)
	{
		pbs->state = 0;
		tool_thread_join(pbs->play_pid);
		tool_thread_join(pbs->recv_pid);
		tool_mem_free(pbs->recv_buf);
		pbs->recv_buf = NULL;
		tool_mem_free(pbs->cb_frame);
		pbs->cb_frame = NULL;
		tool_stream3_done(&pbs->stream);
	}
	pbs->error_flag = 0;
	
	nc_xm_dev_t* dev = (nc_xm_dev_t*)pbs->father_dev;
	TOOL_INT32 xm_ret = 0;	
	TOOL_INT8 begin_time_str[32] = {0};
	TOOL_INT8 end_time_str[32] = {0};
	tool_sysf_tt2str(pst_cfg_record->start_time, 0, begin_time_str, sizeof(begin_time_str));
	tool_sysf_tt2str(pst_cfg_record->end_time, 0, end_time_str, sizeof(end_time_str));	
	tool_mem_memset(&dev->xm_msg.head, sizeof(xm_msg_head_t));
	dev->xm_msg.head.session_id = dev->session_id;
	dev->xm_msg.head.command_id = XM_CMD_CTRL_PBS_REQ;
	dev->xm_msg.head.data_len = snprintf((TOOL_INT8*)dev->xm_msg.data, sizeof(dev->xm_msg.data),
		"{ "
			"\"Name\" : \"OPPlayBack\", "
			"\"OPPlayBack\" : "
			"{ "
				"\"Action\" : \"Stop\", "
				"\"EndTime\" : \"%s\", "
				"\"Parameter\" : "
				"{ "
					"\"FileName\" : \"%s\", "
					"\"PlayMode\" : \"ByName\", "
					"\"Stream_Type\" : 0, "
					"\"TransMode\" : \"TCP\", "
					"\"Value\" : 0 "
				"}, "
				"\"StartTime\" : \"%s\" "
			"}, "
			"\"SessionID\" : \"0x%x\" "
		"}",
		end_time_str, pst_cfg_record->file_info, begin_time_str, dev->session_id);
	if (tool_ptc_xm_sendMsg(&dev->msg_sock, &dev->xm_msg) < 0)
	{
		log_error("tool_ptc_xm_sendMsg");
		return -1;
	}
	if (nc_xm_dev_recvMsg(dev, XM_CMD_CTRL_PBS_RSP) < 0)
	{
		log_error("nc_xm_recv");
		return -1;
	}
	if (tool_json2_getNumber(dev->xm_msg.data, "Ret", &xm_ret) < 0 || xm_ret != XM_RET_OK)
	{
		log_error("nc_xm_dev_recvMsg(%s) ret(%d)", dev->xm_msg.data, xm_ret);
//		return -1;
	}
	
	if (tool_sock_item_isLive(&pbs->pbs_sock))
		tool_sock_item_close(&pbs->pbs_sock);
	return 0;
}

#define __NC_XM_TALK__

TOOL_VOID* nc_xm_talk_runRecv(TOOL_VOID* param)
{
	nc_xm_talk_t* talk = (nc_xm_talk_t*)param;
	nc_xm_dev_t* dev = (nc_xm_dev_t*)talk->father_dev;
	TOOL_INT32 ret = 0;
	log_state("dev(%d) Recv talk start", dev->dev_id);
	talk->recv_flag = 0;
	TOOL_INT32 talk_online = 0;
	ptc_frame_head_t head;
	tool_mem_memset(&head, sizeof(ptc_frame_head_t));

//	TOOL_INT32 tick = 0;
	
	while (talk->state)
	{	
		if (talk_online != talk->online)
		{
			talk_online = talk->online;
			if (talk->online)
			{
				head.frame_type = talk_online;
				if (talk->cb)
				{
					talk->cb(talk->talk_no, &head, sizeof(ptc_frame_head_t), talk->param);
				}
			}
		}
		
/*
		tool_sock_setTick(&tick);
		if (tick == 20)
		{
			log_debug("tool_sock_setTick 10---------------------------");
			talk->error_flag = 1;
			break;
		}
*/
 		ret = tool_sock_item_pollRead(&talk->talk_sock, 0);
		if (ret < 0)
		{
			log_error("tool_sock_item_pollRead");
			talk->error_flag = 1;
			break;
		}
		else if (ret == 0)
		{
			tool_sysf_usleep(20*1000);
			continue;
		}
		if (tool_ptc_xm_recvTalk(&talk->talk_sock, &talk->recv_frame) < 0)
		{
			log_error("tool_ptc_rtp_recvTcp_video error");
			talk->error_flag = 1;
			break;
		}
		if (talk->recv_flag == 0)
			talk->recv_flag = 1;

		tool_thread_lockMutex(&talk->mutex);
		if (talk->cb)
		{
			talk->cb(talk->talk_no, &talk->recv_frame, sizeof(ptc_frame_head_t)+talk->recv_frame.frame_head.len, talk->param);
		}
		tool_thread_unlockMutex(&talk->mutex);
	}

	log_state("dev(%d) Recv talk stop", dev->dev_id);
    return NULL;	   
}

TOOL_VOID* nc_xm_talk_runSend(TOOL_VOID* param)
{
	nc_xm_talk_t* talk = (nc_xm_talk_t*)param;
	nc_xm_dev_t* dev = (nc_xm_dev_t*)talk->father_dev;

	log_state("dev(%d) send talk start", dev->dev_id);
	while (talk->state)
	{	
		if (talk->recv_flag == 0)
		{
			tool_sysf_usleep(20*1000);
			continue;
		}

		dev->talk.send_len = sizeof(ptc_frame_audio_t);
		if (tool_stream2_get(talk->send_stream, &talk->send_pos, &talk->send_frame, &talk->send_len) == 0)
		{
			tool_sysf_usleep(20*1000);
			continue;
		}

		if (tool_ptc_xm_sendTalk(&talk->talk_sock, &talk->send_frame, dev->session_id) < 0)
		{
			log_error("tool_ptc_rtp_sendTcp_talk");
			talk->error_flag = 1;
			break;
		}
	}

	log_state("dev(%d) send talk stop", dev->dev_id);
    return NULL;	   
}

TOOL_INT32 nc_xm_talk_start(nc_xm_talk_t* talk)
{
	nc_xm_dev_t* dev = (nc_xm_dev_t*)talk->father_dev;	
	TOOL_INT32 ret = 0;
	xm_msg_t xm_msg;	
	TOOL_INT32 xm_ret = 0;
	do
	{
		//start talk
		talk->state = 1;		
		if (tool_sock_item_open_v2(&talk->talk_sock, TOOL_SOCK_TYPE1_TCP, TOOL_SOCK_TYPE2_CLIENT, dev->st_nc_dev.ip, dev->st_nc_dev.msg_port, NULL, 0) < 0)
		{
			log_error("tool_sock_item_open TALK");
			ret = -1;
			break;
		}
		tool_mem_memset(&xm_msg.head, sizeof(xm_msg_head_t));
		xm_msg.head.session_id = dev->session_id;
		xm_msg.head.command_id = XM_CMD_LOGIN_TALK_REQ;
		xm_msg.head.data_len = snprintf((TOOL_INT8*)xm_msg.data, sizeof(xm_msg.data),
			"{ "
				"\"Name\" : \"OPTalk\", "
				"\"OPTalk\" : "
				"{ "
					"\"Action\" : \"Claim\", "
					"\"AudioFormat\" : "
					"{ "
						"\"BitRate\" : 32, "
						"\"EncodeType\" : \"G711_ALAW\", "
						"\"SampleBit\" : 8, "
						"\"SampleRate\" : 8 "
					"} "
				"}, "
				"\"SessionID\" : \"0x%x\" "
			"}",
			dev->session_id);
		if (tool_ptc_xm_sendMsg(&talk->talk_sock, &xm_msg) < 0)
		{
			log_error("tool_ptc_xm_sendMsg");
			ret = -1;
			break;
		}
		if (tool_ptc_xm_recvMsg(&talk->talk_sock, &xm_msg) < 0 || xm_msg.head.command_id != XM_CMD_LOGIN_TALK_RSP)
		{
			log_error("nc_xm_recv xm_msg.head.command_id(%d)", xm_msg.head.command_id);
			ret = -1;
			break;
		}
		if (tool_json2_getNumber(xm_msg.data, "Ret", &xm_ret) < 0 || xm_ret != XM_RET_OK)
		{
			log_error("nc_xm_dev_recvMsg(%s) ret(%d)", xm_msg.data, xm_ret);
			ret = -1;
			if (xm_ret == XM_RET_ERROR_TALK_BUSY)
				ret *= xm_ret;
			break;
		}

		tool_mem_memset(&dev->xm_msg, sizeof(xm_msg_head_t));
		dev->xm_msg.head.session_id = dev->session_id;
		dev->xm_msg.head.command_id = XM_CMD_CTRL_TALK_REQ;
		dev->xm_msg.head.data_len = snprintf((TOOL_INT8*)dev->xm_msg.data, sizeof(dev->xm_msg.data),
			"{ "
				"\"Name\" : \"OPTalk\", "
				"\"OPTalk\" : "
				"{ "
					"\"Action\" : \"Start\", "
					"\"AudioFormat\" : "
					"{ "
						"\"BitRate\" : 128, "
						"\"EncodeType\" : \"G711_ALAW\", "
						"\"SampleBit\" : 8, "
						"\"SampleRate\" : 8000 "
					"} "
				"}, "
				"\"SessionID\" : \"0x%x\" "
			"}",
			dev->session_id);
		if (tool_ptc_xm_sendMsg(&dev->msg_sock, &dev->xm_msg) < 0)
		{
			log_error("tool_ptc_xm_sendMsg");
			ret = -1;
			break;
		}
		if (nc_xm_dev_recvMsg(dev, XM_CMD_CTRL_TALK_RSP) < 0)
		{
			log_error("nc_xm_recv");
			ret = -1;
			break;
		}
		if (tool_json2_getNumber(dev->xm_msg.data, "Ret", &xm_ret) < 0)
		{
			log_error("nc_xm_dev_recvMsg(%s)", dev->xm_msg.data);
			ret = -1;
			break;
		}
		if (xm_ret != XM_RET_OK)
		{
			log_error("ret(%d)", xm_ret);
			if (xm_ret == XM_RET_ERROR_NO_RIGHT)
			{
				talk->online = PTC_FRAME_STREAM_RIGHT;
			}
		}

		talk->send_stream = (tool_stream2_t*)tool_mem_malloc(sizeof(tool_stream2_t), 0);
		tool_stream2_init(talk->send_stream);
		tool_stream2_initReadPos(talk->send_stream, &talk->send_pos);
		if (tool_thread_create(&talk->recv_pid, NULL, nc_xm_talk_runRecv, (TOOL_VOID*)talk) < 0)
			log_fatalNo("tool_thread_create");
		if (tool_thread_create(&talk->send_pid, NULL, nc_xm_talk_runSend, (TOOL_VOID*)talk) < 0)
			log_fatalNo("tool_thread_create");
	}while (0);
	if (ret < 0)
	{
		talk->state = 0;	
		if (tool_sock_item_isLive(&talk->talk_sock))
			tool_sock_item_close(&talk->talk_sock);
	}
	return ret;	
}

TOOL_INT32 nc_xm_talk_stop(nc_xm_talk_t* talk)
{
	if (talk->state)
	{
		talk->state = 0;
		tool_thread_join(talk->recv_pid);
		tool_thread_join(talk->send_pid);
		tool_stream2_done(talk->send_stream);
		tool_mem_free(talk->send_stream);
		talk->send_stream = NULL;
	}
	talk->error_flag = 0;
	
	nc_xm_dev_t* dev = (nc_xm_dev_t*)talk->father_dev;
	TOOL_INT32 xm_ret = 0;	
	tool_mem_memset(&dev->xm_msg.head, sizeof(xm_msg_head_t));
	dev->xm_msg.head.session_id = dev->session_id;
	dev->xm_msg.head.command_id = XM_CMD_CTRL_TALK_REQ;
	dev->xm_msg.head.data_len = snprintf((TOOL_INT8*)dev->xm_msg.data, sizeof(dev->xm_msg.data),
		"{ "
			"\"Name\" : \"OPTalk\", "
			"\"OPTalk\" : "
			"{ "
				"\"Action\" : \"Stop\", "
				"\"AudioFormat\" : "
				"{ "
					"\"BitRate\" : 128, "
					"\"EncodeType\" : \"G711_ALAW\", "
					"\"SampleBit\" : 8, "
					"\"SampleRate\" : 8000 "
				"} "
			"}, "
			"\"SessionID\" : \"0x%x\" "
		"}",
		dev->session_id);
	if (tool_ptc_xm_sendMsg(&dev->msg_sock, &dev->xm_msg) < 0)
	{
		log_error("tool_ptc_xm_sendMsg");
		return -1;
	}
	if (nc_xm_dev_recvMsg(dev, XM_CMD_CTRL_TALK_RSP) < 0)
	{
		log_error("nc_xm_recv");
		return -1;
	}
	if (tool_json2_getNumber(dev->xm_msg.data, "Ret", &xm_ret) < 0 || xm_ret != XM_RET_OK)
	{
		log_error("nc_xm_dev_recvMsg(%s) ret(%d)", dev->xm_msg.data, xm_ret);
//		return -1;
	}
	if (tool_sock_item_isLive(&talk->talk_sock))
		tool_sock_item_close(&talk->talk_sock);
	return 0;
}

#define __NC_XM_DEV__

TOOL_VOID nc_xm_dev_setOnline(nc_xm_dev_t* dev, TOOL_INT32 online)
{
	dev->online = online;
	TOOL_INT32 i = 0;
	TOOL_INT32 j = 0;
	for (i = 0; i < NC_XM_CHANNEL_SIZE; i++)
	{
		for (j = 0; j < NC_XM_STREAM_SIZE; j++)
		{
			dev->rts[i][j].online = online;
		}
	}
	dev->pbs.online = online;
	dev->talk.online = online;
}

TOOL_INT32 nc_xm_dev_login(nc_xm_dev_t* dev)
{
	if (tool_sock_item_isLive(&dev->msg_sock))
		return 0;
	if (tool_sock_item_isTimeout(&dev->msg_sock, NC_XM_RECONNECT_CYCLE) == 0)
	{
		tool_sysf_usleep(20*1000);
		return -1;
	}

	TOOL_INT32 ret = 0;
	TOOL_INT8 md5_pswd[16] = {0};
	tool_md5_calc_xm((TOOL_UINT8*)dev->st_nc_dev.pswd, tool_str_strlen(dev->st_nc_dev.pswd), md5_pswd);
	
	do
	{
		nc_xm_dev_setOnline(dev, PTC_FRAME_STREAM_CONNECT);
		if (tool_sock_item_open_v2(&dev->msg_sock, TOOL_SOCK_TYPE1_TCP, TOOL_SOCK_TYPE2_CLIENT, dev->st_nc_dev.ip, dev->st_nc_dev.msg_port, NULL, 0) < 0)
		{
			log_error("nc_xm_dev_login tool_sock_item_open");
			ret = NC_ERROR_CONNECT;
			nc_xm_dev_setOnline(dev, PTC_FRAME_STREAM_CONNECT_FAIL);
			break;
		}
		tool_mem_memset(&dev->xm_msg.head, sizeof(xm_msg_head_t));
		dev->xm_msg.head.command_id = XM_CMD_LOGIN_REQ;
		dev->xm_msg.head.data_len = snprintf((TOOL_INT8*)dev->xm_msg.data, sizeof(dev->xm_msg.data),
			"{ \"EncryptType\" : \"MD5\", \"LoginType\" : \"DVRIP-Web\", \"PassWord\" : \"%s\", \"UserName\" : \"%s\" }",
			md5_pswd, dev->st_nc_dev.user);
		nc_xm_dev_setOnline(dev, PTC_FRAME_STREAM_LOGIN);
		if (tool_ptc_xm_sendMsg(&dev->msg_sock, &dev->xm_msg) < 0)
		{
			ret = NC_ERROR_SEND;
			nc_xm_dev_setOnline(dev, PTC_FRAME_STREAM_LOGIN_FAIL);
			break;
		}
		if (nc_xm_dev_recvMsg(dev, XM_CMD_LOGIN_RSP) < 0)
		{

			ret = NC_ERROR_RECV;
			nc_xm_dev_setOnline(dev, PTC_FRAME_STREAM_LOGIN_FAIL);
			break;
		}

		TOOL_INT32 xm_ret = 0;
		if (tool_json2_getNumber(dev->xm_msg.data, "Ret", &xm_ret) < 0 || xm_ret != XM_RET_OK)
		{
			if (xm_ret == XM_RET_ERROR_USER || xm_ret == XM_RET_ERROR_PSWD)
			{
				ret = NC_ERROR_AUTH;
				nc_xm_dev_setOnline(dev, PTC_FRAME_STREAM_AUTH_FAIL);
			}
			else
			{
				ret = NC_ERROR_PROTOCOL;
				nc_xm_dev_setOnline(dev, PTC_FRAME_STREAM_LOGIN_FAIL);
			}
			break;
		}
		
		if (tool_json2_getNumber(dev->xm_msg.data, "ChannelNum", &dev->channel_num) < 0 ||
			tool_json2_getNumber(dev->xm_msg.data, "SessionID", &dev->session_id) < 0)
		{
			ret = NC_ERROR_PROTOCOL;
			nc_xm_dev_setOnline(dev, PTC_FRAME_STREAM_LOGIN_FAIL);
			break;
		}
		if (dev->channel_num > NC_XM_CHANNEL_SIZE)
		{
			log_error("dev->channel_num(%d)", dev->channel_num);
			nc_xm_dev_setOnline(dev, PTC_FRAME_STREAM_LOGIN_FAIL);
			ret = NC_ERROR_INTERNAL;
			break;
		}
		log_state("login success channel_num(%d) session_id(0x%08x)", dev->channel_num, dev->session_id);

		tool_mem_memset(&dev->xm_msg.head, sizeof(xm_msg_head_t));
		dev->xm_msg.head.session_id = dev->session_id;
		dev->xm_msg.head.command_id = XM_CMD_SYSINFO_REQ;
		dev->xm_msg.head.data_len = snprintf((TOOL_INT8*)dev->xm_msg.data, sizeof(dev->xm_msg.data),
			"{ \"Name\" : \"SystemInfo\", \"SessionID\" : \"0x%08x\" }", dev->session_id);
		if (tool_ptc_xm_sendMsg(&dev->msg_sock, &dev->xm_msg) < 0)
		{
			ret = NC_ERROR_SEND;
			nc_xm_dev_setOnline(dev, PTC_FRAME_STREAM_LOGIN_FAIL);
			break;
		}
		if (nc_xm_dev_recvMsg(dev, XM_CMD_SYSINFO_RSP) < 0)
		{
			ret = NC_ERROR_RECV;
			nc_xm_dev_setOnline(dev, PTC_FRAME_STREAM_LOGIN_FAIL);
			break;
		}
		
		xm_ret = 0;
		if (tool_json2_getNumber(dev->xm_msg.data, "Ret", &xm_ret) < 0 || xm_ret != XM_RET_OK)
		{
			ret = NC_ERROR_PROTOCOL;
			nc_xm_dev_setOnline(dev, PTC_FRAME_STREAM_LOGIN_FAIL);
			break;
		}

		if (tool_json2_getNumber(dev->xm_msg.data, "VideoInChannel", &dev->video_in_num) < 0 ||
			tool_json2_getNumber(dev->xm_msg.data, "AudioInChannel", &dev->audio_in_num) < 0 ||
			tool_json2_getNumber(dev->xm_msg.data, "AlarmInChannel", &dev->alarm_in_num) < 0 ||
			tool_json2_getNumber(dev->xm_msg.data, "AlarmOutChannel", &dev->alarm_out_num) < 0)
		{
			ret = NC_ERROR_PROTOCOL;
			nc_xm_dev_setOnline(dev, PTC_FRAME_STREAM_LOGIN_FAIL);
			break;
		}
		
		log_state("video(%d) audio(%d) alarm(%d.%d)", dev->video_in_num, dev->audio_in_num, dev->alarm_in_num, dev->alarm_out_num);
	} while (0);
	if (ret < 0)
	{
        if (tool_sock_item_isLive(&dev->msg_sock))
			tool_sock_item_close(&dev->msg_sock);
    }
	else
	{
		nc_xm_dev_setOnline(dev, PTC_FRAME_STREAM_RUNNING);
	}
	return ret;
}

TOOL_INT32 nc_xm_dev_logout(nc_xm_dev_t* dev)
{	
	TOOL_INT32 i = 0, j = 0;
	if (tool_sock_item_isLive(&dev->msg_sock))
		tool_sock_item_close(&dev->msg_sock);
	for (i = 0; i < NC_XM_CHANNEL_SIZE; i++)
	{
		for (j = 0; j < NC_XM_STREAM_SIZE; j++)
		{
			dev->rts[i][j].state = 0;
			if (tool_sock_item_isLive(&dev->rts[i][j].rts_sock))
				tool_sock_item_close(&dev->rts[i][j].rts_sock);
			dev->rts[i][j].error_flag = 0;
		}
	}

	dev->pbs.state = 0;
	dev->pbs.error_flag = 0;
	if (tool_sock_item_isLive(&dev->pbs.pbs_sock))
		tool_sock_item_close(&dev->pbs.pbs_sock);
	dev->pbs.left_len = 0;
	dev->pbs_pause = 0;
	dev->talk.state = 0;
	dev->last_ptz_cmd = 0;
	nc_xm_dev_setOnline(dev, PTC_FRAME_STREAM_WAIT);
	return 0;
}


TOOL_INT32 nc_xm_dev_heartbeat(nc_xm_dev_t* dev)
{
	if (tool_sysf_isTickTimeout(dev->hb_tick_sec, dev->hb_tick_usec, NC_XM_HEARTBEAT_CYCLE*1000*1000) == 0)
		return 0;

	tool_mem_memset(&dev->xm_msg.head, sizeof(xm_msg_head_t));
	dev->xm_msg.head.session_id = dev->session_id;
	dev->xm_msg.head.command_id = XM_CMD_HEARTBEAT_REQ;
	dev->xm_msg.head.data_len = snprintf((TOOL_INT8*)dev->xm_msg.data, sizeof(dev->xm_msg.data),
		"{ \"Name\" : \"KeepAlive\", \"SessionID\" : \"0x%08x\" }", dev->session_id);
	if (tool_ptc_xm_sendMsg(&dev->msg_sock, &dev->xm_msg) < 0)
	{
		log_error("tool_ptc_xm_sendMsg");
		return -1;
	}
	if (nc_xm_dev_recvMsg(dev, XM_CMD_HEARTBEAT_RSP) < 0)
	{
		log_error("nc_xm_dev_recvMsg");
		return -1;
	}

	TOOL_INT32 xm_ret = 0;
	if (tool_json2_getNumber(dev->xm_msg.data, "Ret", &xm_ret) < 0 || xm_ret != XM_RET_OK)
	{
		log_error("nc_xm_dev_recvMsg(%s) ret(%d)", dev->xm_msg.data, xm_ret);
		return -1;
	}
	
	tool_sysf_setTick(&dev->hb_tick_sec, &dev->hb_tick_usec);
	return 0;
}

TOOL_INT32 nc_xm_dev_doRts(nc_xm_dev_t* dev)
{	
	TOOL_INT32 i = 0, j = 0;
	for (i = 0; i < NC_XM_CHANNEL_SIZE; i++)
	{
		for (j = 0; j < NC_XM_STREAM_SIZE; j++)
		{
			if (dev->rts[i][j].error_flag)
			{
				if (nc_xm_rts_stop(&dev->rts[i][j]) < 0)
				{
					log_error("nc_xm_rts_stop[%d][%d] error", i, j);
					return -1;
				}
				tool_sock_setTick(&dev->rts[i][j].tick);
				log_state("nc_xm_rts_stop[%d][%d]", i, j);
//				log_error("dev->rts[%d].error_flag", i);
//				return -1;
			}
		}
	}

	TOOL_INT32 rts_enable[NC_XM_CHANNEL_SIZE][NC_XM_STREAM_SIZE] = {{0}};
	tool_thread_lockMutex(&dev->mutex);
	for (i = 0; i < NC_XM_CHANNEL_SIZE; i++)
	{
		for (j = 0; j < NC_XM_STREAM_SIZE; j++)
		{
			rts_enable[i][j] = dev->cur_rts_enable[i][j];
		}
	}
	tool_thread_unlockMutex(&dev->mutex);	

	for (i = 0; i < NC_XM_CHANNEL_SIZE; i++)
	{
		for (j = 0; j < NC_XM_STREAM_SIZE; j++)
		{
			if (dev->rts[i][j].state == rts_enable[i][j])
				continue;
			if (rts_enable[i][j])
			{
				if (tool_sock_isTickTimeout(dev->rts[i][j].tick, 5) == 0)
				{
					dev->rts[i][j].online = 0;
					continue;				
				}
				
				if (nc_xm_rts_start(&dev->rts[i][j]) < 0)
				{
/*
					if (dev->xm_msg.head.u16Status == XM_ERR_LIVE_INPUT_NOT_READY)
					{
						tool_sock_setTick(&dev->rts[i][j].tick);
						dev->rts[i][j].online = PTC_FRAME_STREAM_NOVIDEO;
						continue;
					}
*/;
					log_error("nc_xm_rts_start[%d][%d] error", i, j);
					return -1;
				}
			}
			else
			{
				if (nc_xm_rts_stop(&dev->rts[i][j]) < 0)
				{
					log_error("nc_xm_rts_stop[%d][%d] error", i, j);
					return -1;
				}
			}
		}		
	}
	return 0;
}

TOOL_INT32 nc_xm_dev_doPtz(nc_xm_dev_t* dev)
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

	if (dev->last_ptz_channel_no != ptz_channel_no ||
		dev->last_ptz_cmd != ptz_cmd ||
		dev->last_ptz_param != ptz_param ||
		dev->last_ptz_step != ptz_step)
	{
		TOOL_INT8 xm_Command[32] = {0};
		TOOL_INT32 xm_Preset = 0;
		TOOL_INT32 xm_Step = 0;
		TOOL_INT32 xm_ret = 0;
		if (ptz_cmd == PTC_PTZ_CMD_LEFT)
		{
			tool_str_strncpy(xm_Command, "DirectionLeft", sizeof(xm_Command)-1);
			xm_Preset = 65535;
			xm_Step = ptz_param;
		}
		else if (ptz_cmd == PTC_PTZ_CMD_RIGHT)
		{
			tool_str_strncpy(xm_Command, "DirectionRight", sizeof(xm_Command)-1);
			xm_Preset = 65535;
			xm_Step = ptz_param;
		}
		else if (ptz_cmd == PTC_PTZ_CMD_UP)
		{
			tool_str_strncpy(xm_Command, "DirectionUp", sizeof(xm_Command)-1);
			xm_Preset = 65535;
			xm_Step = ptz_param;
		}
		else if (ptz_cmd == PTC_PTZ_CMD_DOWN)
		{
			tool_str_strncpy(xm_Command, "DirectionDown", sizeof(xm_Command)-1);
			xm_Preset = 65535;
			xm_Step = ptz_param;
		}
		else if (ptz_cmd == PTC_PTZ_CMD_LEFT_UP)
		{
			tool_str_strncpy(xm_Command, "DirectionLeftUp", sizeof(xm_Command)-1);
			xm_Preset = 65535;
			xm_Step = ptz_param;
		}
		else if (ptz_cmd == PTC_PTZ_CMD_LEFT_DOWN)
		{
			tool_str_strncpy(xm_Command, "DirectionLeftDown", sizeof(xm_Command)-1);
			xm_Preset = 65535;
			xm_Step = ptz_param;
		}
		else if (ptz_cmd == PTC_PTZ_CMD_RIGHT_UP)
		{
			tool_str_strncpy(xm_Command, "DirectionRightUp", sizeof(xm_Command)-1);
			xm_Preset = 65535;
			xm_Step = ptz_param;
		}
		else if (ptz_cmd == PTC_PTZ_CMD_RIGHT_DOWN)
		{
			tool_str_strncpy(xm_Command, "DirectionRightDown", sizeof(xm_Command)-1);
			xm_Preset = 65535;
			xm_Step = ptz_param;
		}
		else if (ptz_cmd == PTC_PTZ_CMD_NEAR)
		{
			tool_str_strncpy(xm_Command, "FocusNear", sizeof(xm_Command)-1);
			xm_Preset = 65535;
			xm_Step = ptz_param;
		}
		else if (ptz_cmd == PTC_PTZ_CMD_FAR)
		{
			tool_str_strncpy(xm_Command, "FocusFar", sizeof(xm_Command)-1);
			xm_Preset = 65535;
			xm_Step = ptz_param;
		}
		else if (ptz_cmd == PTC_PTZ_CMD_ZOOM_OUT)
		{
			tool_str_strncpy(xm_Command, "ZoomTile", sizeof(xm_Command)-1);
			xm_Preset = 65535;
			xm_Step = ptz_param;
		}
		else if (ptz_cmd == PTC_PTZ_CMD_ZOOM_IN)
		{
			tool_str_strncpy(xm_Command, "ZoomWide", sizeof(xm_Command)-1);
			xm_Preset = 65535;
			xm_Step = ptz_param;
		}
		else if (ptz_cmd == PTC_PTZ_CMD_IRIS_OPEN)
		{
			tool_str_strncpy(xm_Command, "IrisLarge", sizeof(xm_Command)-1);
			xm_Preset = 65535;
			xm_Step = ptz_param;
		}
		else if (ptz_cmd == PTC_PTZ_CMD_IRIS_CLOSE)
		{
			tool_str_strncpy(xm_Command, "IrisSmall", sizeof(xm_Command)-1);
			xm_Preset = 65535;
			xm_Step = ptz_param;
		}
		else if (ptz_cmd == PTC_PTZ_CMD_PRESET_GO)
		{
			tool_str_strncpy(xm_Command, "GotoPreset", sizeof(xm_Command)-1);
			xm_Preset = ptz_param;
			xm_Step = 0;
		}
		else if (ptz_cmd == PTC_PTZ_CMD_PRESET_SET)
		{
			tool_str_strncpy(xm_Command, "SetPreset", sizeof(xm_Command)-1);
			xm_Preset = ptz_param;
			xm_Step = 0;
		}
		else if (ptz_cmd == PTC_PTZ_CMD_PRESET_DEL)
		{
			tool_str_strncpy(xm_Command, "ClearPreset", sizeof(xm_Command)-1);
			xm_Preset = ptz_param;
			xm_Step = 0;
		}
		else if (ptz_cmd == PTC_PTZ_CMD_AUTO_SCAN_START)
		{
			tool_str_strncpy(xm_Command, "AutoPanOn", sizeof(xm_Command)-1);
			xm_Preset = 0;
			xm_Step = ptz_param;
		}
		else if (ptz_cmd == PTC_PTZ_CMD_AUTO_SCAN_STOP)
		{
			tool_str_strncpy(xm_Command, "AutoPanOff", sizeof(xm_Command)-1);
			xm_Preset = 0;
			xm_Step = ptz_param;
		}
		else 
		{
			tool_str_strncpy(xm_Command, "DirectionLeft", sizeof(xm_Command)-1);
			xm_Preset = -1;
			xm_Step = ptz_param;
		}

		tool_mem_memset(&dev->xm_msg, sizeof(xm_msg_head_t));
		dev->xm_msg.head.session_id = dev->session_id;
		dev->xm_msg.head.command_id = XM_CMD_PTZ_REQ;
		dev->xm_msg.head.data_len = snprintf((TOOL_INT8*)dev->xm_msg.data, sizeof(dev->xm_msg.data),
			"{ "
				"\"Name\" : \"OPPTZControl\", "
				"\"OPPTZControl\" : "
				"{ "
					"\"Command\" : \"DirectionLeft\", "
					"\"Parameter\" : "
					"{ "
						"\"AUX\" : "
						"{ "
							"\"Number\" : 0, "
							"\"Status\" : \"On\" "
						"}, "
						"\"Channel\" : %d, "
						"\"MenuOpts\" : \"Enter\", "
						"\"POINT\" : "
						"{ "
							"\"bottom\" : 0, "
							"\"left\" : 0, "
							"\"right\" : 0, "
							"\"top\" : 0 "
						"}, "
						"\"Pattern\" : \"SetBegin\", "
						"\"Preset\" : -1, "
						"\"Step\" : 0, "
						"\"Tour\" : 0 "
					"} "
				"}, "
				"\"SessionID\" : \"0x%x\" "
			"}",
			ptz_channel_no, dev->session_id);
		
		if (tool_ptc_xm_sendMsg(&dev->msg_sock, &dev->xm_msg) < 0)
		{
			log_error("tool_ptc_xm_sendMsg");
			return -1;
		}
		if (nc_xm_dev_recvMsg(dev, XM_CMD_PTZ_RSP) < 0)
		{
			log_error("nc_xm_recv");
			return -1;
		}
		if (tool_json2_getNumber(dev->xm_msg.data, "Ret", &xm_ret) < 0 || xm_ret != XM_RET_OK)
		{
			log_error("nc_xm_dev_recvMsg(%s) ret(%d)", dev->xm_msg.data, xm_ret);
			return -1;
		}

		tool_mem_memset(&dev->xm_msg, sizeof(xm_msg_head_t));
		dev->xm_msg.head.session_id = dev->session_id;
		dev->xm_msg.head.command_id = XM_CMD_PTZ_REQ;
		dev->xm_msg.head.data_len = snprintf((TOOL_INT8*)dev->xm_msg.data, sizeof(dev->xm_msg.data),
			"{ "
				"\"Name\" : \"OPPTZControl\", "
				"\"OPPTZControl\" : "
				"{ "
					"\"Command\" : \"%s\", "
					"\"Parameter\" : "
					"{ "
						"\"AUX\" : "
						"{ "
							"\"Number\" : 0, "
							"\"Status\" : \"On\" "
						"}, "
						"\"Channel\" : %d, "
						"\"MenuOpts\" : \"Enter\", "
						"\"POINT\" : "
						"{ "
							"\"bottom\" : 0, "
							"\"left\" : 0, "
							"\"right\" : 0, "
							"\"top\" : 0 "
						"}, "
						"\"Pattern\" : \"SetBegin\", "
						"\"Preset\" : %d, "
						"\"Step\" : %d, "
						"\"Tour\" : 0 "
					"} "
				"}, "
				"\"SessionID\" : \"0x%x\" "
			"}",
			xm_Command, ptz_channel_no, xm_Preset, xm_Step, dev->session_id);
		
		if (tool_ptc_xm_sendMsg(&dev->msg_sock, &dev->xm_msg) < 0)
		{
			log_error("tool_ptc_xm_sendMsg");
			return -1;
		}
		if (nc_xm_dev_recvMsg(dev, XM_CMD_PTZ_RSP) < 0)
		{
			log_error("nc_xm_recv");
			return -1;
		}
		if (tool_json2_getNumber(dev->xm_msg.data, "Ret", &xm_ret) < 0 || xm_ret != XM_RET_OK)
		{
			log_error("nc_xm_dev_recvMsg(%s) ret(%d)", dev->xm_msg.data, xm_ret);
			return -1;
		}

		tool_sysf_setTick(&dev->ptz_tick_sec, &dev->ptz_tick_usec);
		dev->last_ptz_channel_no = ptz_channel_no;
		dev->last_ptz_cmd = ptz_cmd;
		dev->last_ptz_param = ptz_param;
		dev->last_ptz_step = ptz_step;
		log_state("cmd(%d.%s) param(%d)", ptz_cmd, xm_Command, ptz_param);
	}

	return 0;
}

TOOL_INT32 nc_xm_dev_doRecords(nc_xm_dev_t* dev)
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
	TOOL_INT32 array_x = 1024;
	TOOL_INT32 array_y = 256;
	TOOL_INT8* pch_array_value = (TOOL_INT8*)tool_mem_malloc(array_x*array_y, 0);
	do
	{
		records.num = 0;

		TOOL_INT8 BeginTime_str[32] = {0};
		TOOL_INT8 EndTime_str[32] = {0};
		tool_sysf_tt2str(records.start_time, 0, BeginTime_str, sizeof(BeginTime_str));
		tool_sysf_tt2str(records.end_time, 0, EndTime_str, sizeof(EndTime_str));
		
		TOOL_INT8 xm_Event = '*';
		if (records.type == PTC_RECORD_TYPE_TIME)
		{
			xm_Event = 'R';
		}
		else if (records.type == PTC_RECORD_TYPE_ALARM)
		{
			if (records.sub_type == PTC_RECORD_SUBTYPE_MOTION || records.sub_type == PTC_RECORD_SUBTYPE_COVER)
				xm_Event = 'M';
			else if (records.sub_type == PTC_RECORD_SUBTYPE_SENSOR)
				xm_Event = 'A';
			else
				break;
		}
		else if (records.type == PTC_RECORD_TYPE_MANUAL)
		{
			xm_Event = 'H';
		}
		else if (records.type == PTC_RECORD_TYPE_ALL)
		{
			xm_Event = '*';
		}
		else
		{
			break;
		}

		TOOL_INT32 i = 0;
		TOOL_UINT8 channel_no = 0;
		for (i = 0; i < NC_XM_CHANNEL_SIZE; i++)
		{
			if (records.channel & (TOOL_UINT64)(1<<i))
			{
				channel_no = i;
				break;
			}
		}
		if (i == NC_XM_CHANNEL_SIZE)
		{
			log_error("channel(%lld)", records.channel);
			result = NC_ERROR_INPUT;
			break;
		}

		tool_mem_memset(&dev->xm_msg, sizeof(xm_msg_head_t));
		dev->xm_msg.head.session_id = dev->session_id;
		dev->xm_msg.head.command_id = XM_CMD_SEARCH_REC_REQ;
		dev->xm_msg.head.data_len = snprintf((TOOL_INT8*)dev->xm_msg.data, sizeof(dev->xm_msg.data),
			"{ "
				"\"Name\" : \"OPFileQuery\", "
				"\"OPFileQuery\" : "
				"{ "
					"\"BeginTime\" : \"%s\", "
					"\"Channel\" : %d, "
					"\"DriverTypeMask\" : \"0x0000FFFF\", "
					"\"EndTime\" : \"%s\", "
					"\"Event\" : \"%c\", "
					"\"StreamType\" : \"0x00000000\", "
					"\"Type\" : \"h264\" "
				"}, "
				"\"SessionID\" : \"0x%x\" "
			"}",
			BeginTime_str, channel_no, EndTime_str, xm_Event, dev->session_id);		
		if (tool_ptc_xm_sendMsg(&dev->msg_sock, &dev->xm_msg) < 0)
		{
			log_error("tool_ptc_xm_sendMsg");
			result = NC_ERROR_SEND;
			break;
		}
		if (nc_xm_dev_recvSearch(dev, XM_CMD_SEARCH_REC_RSP) < 0)
		{
			log_error("nc_xm_recv");
			result = NC_ERROR_RECV;
			break;
		}
		TOOL_INT32 xm_ret = 0;
		if (tool_json2_getNumber(dev->pst_xm_search->data, "Ret", &xm_ret) < 0 || xm_ret != XM_RET_OK)
		{
			if (xm_ret == XM_RET_ERROR_NO_RECORD)
			{
				log_state("no record");
				break;
			}
			log_error("nc_xm_dev_recvMsg(%s) ret(%d)", dev->pst_xm_search->data, xm_ret);
			result = NC_ERROR_PROTOCOL;
			break;
		}

		TOOL_INT32 num = tool_json2_getArray(dev->pst_xm_search->data, "OPFileQuery", pch_array_value, array_x, array_y);
		if (num < 0)
		{
			log_error("tool_json2_getArray(%s)", dev->pst_xm_search->data);
			result = NC_ERROR_PROTOCOL;
			break;
		}
		TOOL_INT8 start_time_str[32] = {0};
		TOOL_INT8 end_time_str[32] = {0};
		for (i = 0; i < num ; i++)
		{
			if (records.num >= PTC_RECORDS_SIZE)
				break;
			
			records.record[records.num].channel = channel_no;
			records.record[records.num].type = records.type;
			records.record[records.num].sub_type = records.sub_type;
			if (tool_json2_getValue(&pch_array_value[i*array_y], "BeginTime", start_time_str, sizeof(start_time_str)) < 0 ||
				tool_sysf_str2tt(start_time_str, 0, &records.record[records.num].start_time) < 0 ||
				tool_json2_getValue(&pch_array_value[i*array_y], "EndTime", end_time_str, sizeof(end_time_str)) < 0 ||
				tool_sysf_str2tt(end_time_str, 0, &records.record[records.num].end_time) < 0 ||
				tool_json2_getNumber(&pch_array_value[i*array_y], "FileLength", (TOOL_INT32*)&records.record[records.num].size) < 0 ||
				tool_json2_getValue(&pch_array_value[i*array_y], "FileName", (TOOL_INT8*)records.record[records.num].file_info, 128) < 0)
			{
				log_error("tool_json2_getArray(%s)", dev->pst_xm_search->data);
				result = NC_ERROR_PROTOCOL;
				break;
			}
			records.num ++;
		}
	}while (0);

	tool_mem_free(pch_array_value);
	pch_array_value = NULL;

	tool_thread_lockMutex(&dev->mutex);
	tool_mem_memcpy(&dev->cfg_records, &records, sizeof(ptc_cfg_records_t));
	dev->result_records = result;
	dev->last_records_req = dev->cur_records_req;
	tool_thread_broadcastCond(&dev->cond_records);
	tool_thread_unlockMutex(&dev->mutex);
	return ret; 
}

TOOL_INT32 nc_xm_dev_doPbs(nc_xm_dev_t* dev)
{
	TOOL_INT32 pbs_enable;
	ptc_cfg_record_t ptc_record;
	tool_thread_lockMutex(&dev->mutex);	
	pbs_enable = dev->cur_pbs_enable;
	tool_mem_memcpy(&ptc_record, &dev->pbs.ptc_record, sizeof(ptc_cfg_record_t));
	tool_thread_unlockMutex(&dev->mutex);

	if (dev->pbs.error_flag)
	{
		if (nc_xm_pbs_stop(&dev->pbs, &ptc_record) < 0)
		{
			log_error("nc_xm_pbs_stop error");
			return -1;
		}
		log_state("XM_CMD_PBS_STOP success");
		tool_sock_setTick(&dev->pbs.reconnect_tick_sec);
//		log_error("dev->pbs.error_flag");
//		return -1;
	}

	if (dev->pbs.state != pbs_enable)
	{
		if (pbs_enable == 0)
		{
			if (nc_xm_pbs_stop(&dev->pbs, &ptc_record) < 0)
			{
				log_error("nc_xm_pbs_stop error");
				return -1;
			}
			log_state("XM_CMD_PBS_STOP success");
		}
		else
		{
			if (tool_sock_isTickTimeout(dev->pbs.reconnect_tick_sec, 5) == 0)
			{
				dev->pbs.online = 0;
				return 0;		
			}
			
			if (nc_xm_pbs_start(&dev->pbs, &ptc_record) < 0)
			{
				log_error("nc_xm_pbs_start error");
				return -1;
			}
			dev->pbs.tick_sec = 0;
			log_state("XM_CMD_PBS_START success");
		}
		dev->pbs.state = pbs_enable;		
	}

/*
	if (dev->pbs.state)
	{
		if (nc_xm_pbs_get(&dev->pbs) < 0)
		{
			log_error("nc_xm_pbs_get error");
			return -1;
		}
		if (dev->pbs_pause)
		{
			if (tool_stream3_state(&dev->pbs.stream) < 0)
			{
				if (nc_xm_pbs_resume(&dev->pbs) < 0)
				{
					log_error("nc_xm_pbs_resume");
					return -1;
				}
				dev->pbs_pause = 0;
			}
		}
		else
		{
			if (tool_stream3_state(&dev->pbs.stream) > 0)
			{
				if (nc_xm_pbs_pause(&dev->pbs) < 0)
				{
					log_error("nc_xm_pbs_pause");
					return -1;
				}
				dev->pbs_pause = 1;
			}
		}
	}
*/	

	return 0;
}

TOOL_INT32 nc_xm_dev_doLogs(nc_xm_dev_t* dev)
{
	ptc_logs_v2_t logs_v2;
	tool_thread_lockMutex(&dev->mutex);
	if (dev->last_logs_v2_req == dev->cur_logs_v2_req)
	{
		tool_thread_unlockMutex(&dev->mutex);
		return 0;
	}
	tool_mem_memcpy(&logs_v2, &dev->cfg_logs_v2, sizeof(ptc_logs_v2_t));
	tool_thread_unlockMutex(&dev->mutex);	

	logs_v2.num = 0;
	TOOL_INT32 ret = 0;
	TOOL_INT32 result = 0;
	TOOL_INT32 array_x = 1024;
	TOOL_INT32 array_y = 256;
	TOOL_INT8* pch_array_value = (TOOL_INT8*)tool_mem_malloc(array_x*array_y, 0);
	TOOL_INT32 xm_LogPosition = 0;
	TOOL_INT32 xm_ret = 0;	

	logs_v2.num = 0;
	TOOL_INT8 BeginTime_str[32] = {0};
	TOOL_INT8 EndTime_str[32] = {0};
	tool_sysf_tt2str(logs_v2.start_time, 0, BeginTime_str, sizeof(BeginTime_str));
	tool_sysf_tt2str(logs_v2.end_time, 0, EndTime_str, sizeof(EndTime_str));
	TOOL_INT32 i = 0;
	TOOL_INT8 xm_Time[32] = {0};
	TOOL_INT8 xm_Type[32] = {0};	
	TOOL_INT32 num = 0;
	while (1)
	{
		if (logs_v2.num >= PTC_LOGS_SIZE)
			break;

		tool_mem_memset(&dev->xm_msg, sizeof(xm_msg_head_t));
		dev->xm_msg.head.session_id = dev->session_id;
		dev->xm_msg.head.command_id = XM_CMD_SEARCH_LOG_REQ;
		dev->xm_msg.head.data_len = snprintf((TOOL_INT8*)dev->xm_msg.data, sizeof(dev->xm_msg.data),
			"{ "
				"\"Name\" : \"OPLogQuery\", "
				"\"OPLogQuery\" : "
				"{ "
					"\"BeginTime\" : \"%s\", "
					"\"EndTime\" : \"%s\", "
					"\"LogPosition\" : %d, "
					"\"Type\" : \"LogAll\" "
				"}, "
				"\"SessionID\" : \"0x%x\" "
			"}",
			BeginTime_str, EndTime_str, xm_LogPosition, dev->session_id);		
		if (tool_ptc_xm_sendMsg(&dev->msg_sock, &dev->xm_msg) < 0)
		{
			log_error("tool_ptc_xm_sendMsg");
			result = NC_ERROR_SEND;
			break;
		}
		if (nc_xm_dev_recvSearch(dev, XM_CMD_SEARCH_LOG_RSP) < 0)
		{
			log_error("nc_xm_recv");
			result = NC_ERROR_RECV;
			break;
		}
		if (tool_json2_getNumber(dev->pst_xm_search->data, "Ret", &xm_ret) < 0 || xm_ret != XM_RET_OK)
		{
			log_error("nc_xm_dev_recvMsg(%s) ret(%d)", dev->pst_xm_search->data, xm_ret);
			result = NC_ERROR_PROTOCOL;
			break;
		}

		num = tool_json2_getArray(dev->pst_xm_search->data, "OPLogQuery", pch_array_value, array_x, array_y);
		if (num < 0)
		{
			log_error("tool_json2_getArray(%s)", dev->pst_xm_search->data);
//			log_error("tool_json2_getArray()");
			result = NC_ERROR_PROTOCOL;
			break;
		}

		for (i = 0; i < num ; i++)
		{
			if (logs_v2.num >= PTC_LOGS_SIZE)
				break;

			tool_mem_memset(&logs_v2.log_v2[logs_v2.num], sizeof(ptc_log_v2_t));
			if (tool_json2_getValue(&pch_array_value[i*array_y], "Time", xm_Time, sizeof(xm_Time)) < 0 ||
				tool_sysf_str2tt(xm_Time, 0, &logs_v2.log_v2[logs_v2.num].time) < 0 ||
				tool_json2_getNumber(&pch_array_value[i*array_y], "Position", &xm_LogPosition) < 0 ||
				tool_json2_getValue(&pch_array_value[i*array_y], "Type", xm_Type, sizeof(xm_Type)) < 0 ||
				tool_json2_getValue(&pch_array_value[i*array_y], "Data", logs_v2.log_v2[logs_v2.num].data, 32) < 0)
			{
				log_error("tool_json2_getArray(%s)", dev->pst_xm_search->data);
//				log_error("tool_json2_getArray()");
				result = NC_ERROR_PROTOCOL;
				break;
			}

			if (tool_str_strncmp(xm_Type, "Reboot", sizeof(xm_Type)-1) == 0)
			{
				logs_v2.log_v2[logs_v2.num].type = PTC_LOG_TYPE_SYSTEM;
				logs_v2.log_v2[logs_v2.num].sub_type2= PTC_LOG_XM_Reboot;
	
			}
			else if (tool_str_strncmp(xm_Type, "ShutDown", sizeof(xm_Type)-1) == 0)
			{
				logs_v2.log_v2[logs_v2.num].type = PTC_LOG_TYPE_SYSTEM;
				logs_v2.log_v2[logs_v2.num].sub_type2= PTC_LOG_XM_ShutDown;
			}
			else if (tool_str_strncmp(xm_Type, "SaveConfig", sizeof(xm_Type)-1) == 0)
			{
				logs_v2.log_v2[logs_v2.num].type = PTC_LOG_TYPE_OPERATE;
				logs_v2.log_v2[logs_v2.num].sub_type2 = PTC_LOG_XM_SaveConfig;
			}
			else if (tool_str_strncmp(xm_Type, "FileAccessError", sizeof(xm_Type)-1) == 0)
			{
				logs_v2.log_v2[logs_v2.num].type = PTC_LOG_TYPE_OPERATE;
				logs_v2.log_v2[logs_v2.num].sub_type2 = PTC_LOG_XM_FileAccessError;
			}
			else if (tool_str_strncmp(xm_Type, "FileAccessError", sizeof(xm_Type)-1) == 0)
			{
				logs_v2.log_v2[logs_v2.num].type = PTC_LOG_TYPE_OPERATE;
				logs_v2.log_v2[logs_v2.num].sub_type2 = PTC_LOG_XM_SetDriverType;
			}
			else if (tool_str_strncmp(xm_Type, "ClearDriver", sizeof(xm_Type)-1) == 0)
			{
				logs_v2.log_v2[logs_v2.num].type = PTC_LOG_TYPE_OPERATE;
				logs_v2.log_v2[logs_v2.num].sub_type2 = PTC_LOG_XM_ClearDriver;
			}
			else if (tool_str_strncmp(xm_Type, "StorageDeviceError", sizeof(xm_Type)-1) == 0)
			{
				logs_v2.log_v2[logs_v2.num].type = PTC_LOG_TYPE_OPERATE;
				logs_v2.log_v2[logs_v2.num].sub_type2 = PTC_LOG_XM_StorageDeviceError;
			}
			else if (tool_str_strncmp(xm_Type, "EventStart", sizeof(xm_Type)-1) == 0)
			{
				logs_v2.log_v2[logs_v2.num].type = PTC_LOG_TYPE_ALARM;
				logs_v2.log_v2[logs_v2.num].sub_type2 = PTC_LOG_XM_EventStart;
			}
			else if (tool_str_strncmp(xm_Type, "EventStop", sizeof(xm_Type)-1) == 0)
			{
				logs_v2.log_v2[logs_v2.num].type = PTC_LOG_TYPE_ALARM;
				logs_v2.log_v2[logs_v2.num].sub_type2 = PTC_LOG_XM_EventStop;
			}
			else if (tool_str_strncmp(xm_Type, "LogIn", sizeof(xm_Type)-1) == 0)
			{
				logs_v2.log_v2[logs_v2.num].type = PTC_LOG_TYPE_OPERATE;
				logs_v2.log_v2[logs_v2.num].sub_type2 = PTC_LOG_XM_LogIn;
			}
			else if (tool_str_strncmp(xm_Type, "LogOut", sizeof(xm_Type)-1) == 0)
			{
				logs_v2.log_v2[logs_v2.num].type = PTC_LOG_TYPE_OPERATE;
				logs_v2.log_v2[logs_v2.num].sub_type2 = PTC_LOG_XM_LogOut;
			}
			else if (tool_str_strncmp(xm_Type, "AddUser", sizeof(xm_Type)-1) == 0)
			{
				logs_v2.log_v2[logs_v2.num].type = PTC_LOG_TYPE_OPERATE;
				logs_v2.log_v2[logs_v2.num].sub_type2 = PTC_LOG_XM_AddUser;
			}
			else if (tool_str_strncmp(xm_Type, "DeleteUser", sizeof(xm_Type)-1) == 0)
			{
				logs_v2.log_v2[logs_v2.num].type = PTC_LOG_TYPE_OPERATE;
				logs_v2.log_v2[logs_v2.num].sub_type2 = PTC_LOG_XM_DeleteUser;
			}
			else if (tool_str_strncmp(xm_Type, "ModifyUser", sizeof(xm_Type)-1) == 0)
			{
				logs_v2.log_v2[logs_v2.num].type = PTC_LOG_TYPE_OPERATE;
				logs_v2.log_v2[logs_v2.num].sub_type2 = PTC_LOG_XM_ModifyUser;
			}
			else if (tool_str_strncmp(xm_Type, "ModifyPassword", sizeof(xm_Type)-1) == 0)
			{
				logs_v2.log_v2[logs_v2.num].type = PTC_LOG_TYPE_OPERATE;
				logs_v2.log_v2[logs_v2.num].sub_type2 = PTC_LOG_XM_ModifyPassword;
			}
			else if (tool_str_strncmp(xm_Type, "AddGroup", sizeof(xm_Type)-1) == 0)
			{
				logs_v2.log_v2[logs_v2.num].type = PTC_LOG_TYPE_OPERATE;
				logs_v2.log_v2[logs_v2.num].sub_type2 = PTC_LOG_XM_AddGroup;
			}
			else if (tool_str_strncmp(xm_Type, "DeleteGroup", sizeof(xm_Type)-1) == 0)
			{
				logs_v2.log_v2[logs_v2.num].type = PTC_LOG_TYPE_OPERATE;
				logs_v2.log_v2[logs_v2.num].sub_type2 = PTC_LOG_XM_DeleteGroup;
			}
			else if (tool_str_strncmp(xm_Type, "ModifyGroup", sizeof(xm_Type)-1) == 0)
			{
				logs_v2.log_v2[logs_v2.num].type = PTC_LOG_TYPE_OPERATE;
				logs_v2.log_v2[logs_v2.num].sub_type2 = PTC_LOG_XM_ModifyGroup;
			}
			else if (tool_str_strncmp(xm_Type, "ClearLog", sizeof(xm_Type)-1) == 0)
			{
				logs_v2.log_v2[logs_v2.num].type = PTC_LOG_TYPE_SYSTEM;
				logs_v2.log_v2[logs_v2.num].sub_type2 = PTC_LOG_XM_ClearLog;
			}
			else if (tool_str_strncmp(xm_Type, "FileSearch", sizeof(xm_Type)-1) == 0)
			{
				logs_v2.log_v2[logs_v2.num].type = PTC_LOG_TYPE_OPERATE;
				logs_v2.log_v2[logs_v2.num].sub_type2 = PTC_LOG_XM_FileSearch;
			}
			else if (tool_str_strncmp(xm_Type, "FileAccess", sizeof(xm_Type)-1) == 0)
			{
				logs_v2.log_v2[logs_v2.num].type = PTC_LOG_TYPE_OPERATE;
				logs_v2.log_v2[logs_v2.num].sub_type2 = PTC_LOG_XM_FileAccess;
			}
			else if (tool_str_strncmp(xm_Type, "Record", sizeof(xm_Type)-1) == 0)
			{
				logs_v2.log_v2[logs_v2.num].type = PTC_LOG_TYPE_OPERATE;
				logs_v2.log_v2[logs_v2.num].sub_type2 = PTC_LOG_XM_Record;
			}
			else if (tool_str_strncmp(xm_Type, "ModifyTime", sizeof(xm_Type)-1) == 0 ||
				tool_str_strncmp(xm_Type, "SetTime", sizeof(xm_Type)-1) == 0 ||
				tool_str_strncmp(xm_Type, "TimeChanged", sizeof(xm_Type)-1) == 0)
			{
				logs_v2.log_v2[logs_v2.num].type = PTC_LOG_TYPE_SYSTEM;
				logs_v2.log_v2[logs_v2.num].sub_type2 = PTC_LOG_XM_ModifyTime;
			}
			else if (tool_str_strncmp(xm_Type, "ZeroBitrate", sizeof(xm_Type)-1) == 0)
			{
				logs_v2.log_v2[logs_v2.num].type = PTC_LOG_TYPE_SYSTEM;
				logs_v2.log_v2[logs_v2.num].sub_type2 = PTC_LOG_XM_ZeroBitrate;
			}
			else if (tool_str_strncmp(xm_Type, "AccountRestore", sizeof(xm_Type)-1) == 0)
			{
				logs_v2.log_v2[logs_v2.num].type = PTC_LOG_TYPE_OPERATE;
				logs_v2.log_v2[logs_v2.num].sub_type2 = PTC_LOG_XM_AccountRestore;
			}
			else if (tool_str_strncmp(xm_Type, "Upgrade", sizeof(xm_Type)-1) == 0)
			{
				logs_v2.log_v2[logs_v2.num].type = PTC_LOG_TYPE_SYSTEM;
				logs_v2.log_v2[logs_v2.num].sub_type2 = PTC_LOG_XM_Upgrade;
			}
			else if (tool_str_strncmp(xm_Type, "DiskChanged", sizeof(xm_Type)-1) == 0)
			{
				logs_v2.log_v2[logs_v2.num].type = PTC_LOG_TYPE_OPERATE;
				logs_v2.log_v2[logs_v2.num].sub_type2 = PTC_LOG_XM_DiskChanged;
			}
			else if (tool_str_strncmp(xm_Type, "Exception", sizeof(xm_Type)-1) == 0)
			{
				logs_v2.log_v2[logs_v2.num].type = PTC_LOG_TYPE_SYSTEM;
				logs_v2.log_v2[logs_v2.num].sub_type2 = PTC_LOG_XM_Exception;
			}
			else if (tool_str_strncmp(xm_Type, "SaveSystemState", sizeof(xm_Type)-1) == 0)
			{
				logs_v2.log_v2[logs_v2.num].type = PTC_LOG_TYPE_SYSTEM;
				logs_v2.log_v2[logs_v2.num].sub_type2 = PTC_LOG_XM_SaveSystemState;
			}
			else
			{
				log_error("xm_Type(%s) (%d) (%s)", xm_Type, i, &pch_array_value[i*array_y]);
				continue;
			}
			logs_v2.num ++;
		}
		xm_LogPosition ++;
		if (num < 128)
			break;
	}
	tool_mem_free(pch_array_value);
	pch_array_value = NULL;

	log_debug("logs_v2.num(%d)", logs_v2.num);

	tool_thread_lockMutex(&dev->mutex);
	tool_mem_memcpy(&dev->cfg_logs_v2, &logs_v2, sizeof(ptc_logs_v2_t));
	dev->result_logs_v2 = result;
	dev->last_logs_v2_req = dev->cur_logs_v2_req;
	tool_thread_broadcastCond(&dev->cond_logs_v2);
	tool_thread_unlockMutex(&dev->mutex);
	return ret;
}

TOOL_INT32 nc_xm_dev_doTalk(nc_xm_dev_t* dev)
{
	if (dev->talk.error_flag)
	{
		if (nc_xm_talk_stop(&dev->talk) < 0)
		{
			log_error("nc_xm_talk_stop error");
			return -1;
		}
		log_state("nc_xm_talk_stop success");
		tool_sock_setTick(&dev->talk.tick);
//		log_error("dev->talk.error_flag");
//		return -1;
	}
	
	TOOL_INT32 talk_enable;
	tool_thread_lockMutex(&dev->mutex);
	talk_enable = dev->cur_talk_enable;
	tool_thread_unlockMutex(&dev->mutex);

	TOOL_INT32 ret = 0;
	if (dev->talk.state != talk_enable)
	{
		if (talk_enable == 0)
		{
			if (nc_xm_talk_stop(&dev->talk) < 0)
			{
				log_error("nc_xm_talk_stop error");
				return -1;
			}
			log_state("nc_xm_talk_stop success");
		}
		else
		{
			if (tool_sock_isTickTimeout(dev->talk.tick, 5) == 0)
			{
				dev->talk.online = 0;
				return 0;		
			}

			ret = nc_xm_talk_start(&dev->talk);
			if (ret < 0)
			{	
				if (ret == XM_RET_ERROR_TALK_BUSY*(-1))
				{
					tool_sock_setTick(&dev->talk.tick);
					dev->talk.online = PTC_FRAME_STREAM_TALK_BUSY;
					return 0;
				}
				log_error("nc_xm_talk_start error");
				return -1;
			}
			log_state("nc_xm_talk_start success");
		}
 	}
	return 0;
}

TOOL_INT32 nc_xm_dev_doSyncTime(nc_xm_dev_t* dev)
{
	TOOL_UINT32 utc_sec;
	TOOL_INT32 zone;
	TOOL_INT32 dst_hour;
	TOOL_INT32 xm_ret = 0;	
	tool_thread_lockMutex(&dev->mutex);
	if (dev->last_time_req == dev->cur_time_req)
	{
		tool_thread_unlockMutex(&dev->mutex);
		return 0;
	}
	utc_sec = dev->utc_sec;
	zone = dev->zone;
	dst_hour = dev->dst_hour;
	tool_thread_unlockMutex(&dev->mutex);

	tool_mem_memset(&dev->xm_msg, sizeof(xm_msg_head_t));
	dev->xm_msg.head.session_id = dev->session_id;
	dev->xm_msg.head.command_id = XM_CMD_SET_CFG_REQ;
	dev->xm_msg.head.data_len = snprintf((TOOL_INT8*)dev->xm_msg.data, sizeof(dev->xm_msg.data),
		"{ "
			"\"Name\" : \"System.TimeZone\", "
			"\"SessionID\" : \"0x%x\", "
			"\"System.TimeZone\" : "
			"{ "
				"\"FirstUserTimeZone\" : 0, "
				"\"timeMin\" : %d "
			"} "
		"}",
		dev->session_id, zone*(-15));
	if (tool_ptc_xm_sendMsg(&dev->msg_sock, &dev->xm_msg) < 0)
	{
		log_error("tool_ptc_xm_sendMsg");
		return -1;
	}
	if (nc_xm_dev_recvMsg(dev, XM_CMD_SET_CFG_RSP) < 0)
	{
		log_error("nc_xm_dev_recvMsg");
		return -1;
	}
	if (tool_json2_getNumber(dev->xm_msg.data, "Ret", &xm_ret) < 0 || xm_ret != XM_RET_OK)
	{
		log_error("nc_xm_dev_recvMsg(%s) ret(%d)", dev->xm_msg.data, xm_ret);
		return -1;
	}	

	TOOL_INT8 date_time[32] = {0};
	tool_mem_memset(&dev->xm_msg, sizeof(xm_msg_head_t));
	dev->xm_msg.head.session_id = dev->session_id;
	dev->xm_msg.head.command_id = XM_CMD_MANAGE_SYS_REQ;
	dev->xm_msg.head.data_len = snprintf((TOOL_INT8*)dev->xm_msg.data, sizeof(dev->xm_msg.data),
		"{ "
			"\"Name\" : \"OPTimeSetting\", "
			"\"OPTimeSetting\" : \"%s\", "
			"\"SessionID\" : \"0x%x\" "
		"}",
		tool_sysf_tt2str(utc_sec, 0, date_time, sizeof(date_time)), dev->session_id);
	if (tool_ptc_xm_sendMsg(&dev->msg_sock, &dev->xm_msg) < 0)
	{
		log_error("tool_ptc_xm_sendMsg");
		return -1;
	}
	if (nc_xm_dev_recvMsg(dev, XM_CMD_MANAGE_SYS_RSP) < 0)
	{
		log_error("nc_xm_dev_recvMsg");
		return -1;
	}
	if (tool_json2_getNumber(dev->xm_msg.data, "Ret", &xm_ret) < 0 || xm_ret != XM_RET_OK)
	{
		log_error("nc_xm_dev_recvMsg(%s) ret(%d)", dev->xm_msg.data, xm_ret);
		return -1;
	}
	dev->last_time_req = dev->cur_time_req;
	return 0;
}

TOOL_INT32 nc_xm_dev_doGetStatus(nc_xm_dev_t* dev)
{
	ptc_cfg_status_t cfg_status;
	tool_thread_lockMutex(&dev->mutex);
	if (dev->last_get_status_req == dev->cur_get_status_req)
	{
		tool_thread_unlockMutex(&dev->mutex);
		return 0;
	}
	tool_mem_memcpy(&cfg_status, &dev->cfg_get_status, sizeof(ptc_cfg_status_t));
	tool_thread_unlockMutex(&dev->mutex);	

	TOOL_INT32 result = 0;
	TOOL_INT32 ret = 0;
	do
	{
	
		TOOL_INT32 xm_ret = 0;
		tool_mem_memset(&dev->xm_msg, sizeof(xm_msg_head_t));
		dev->xm_msg.head.session_id = dev->session_id;
		dev->xm_msg.head.command_id = XM_CMD_GET_CFG_REQ;
		dev->xm_msg.head.data_len = snprintf((TOOL_INT8*)dev->xm_msg.data, sizeof(dev->xm_msg.data),
			"{ "
				"\"Name\" : \"Alarm.AlarmOut\", "
				"\"SessionID\" : \"0x%x\" "
			"}",
			dev->session_id);
		if (tool_ptc_xm_sendMsg(&dev->msg_sock, &dev->xm_msg) < 0)
		{
			log_error("tool_ptc_xm_sendMsg");
			result = NC_ERROR_SEND;
			ret = NC_ERROR_SEND;
			break;
		}
		if (nc_xm_dev_recvMsg(dev, XM_CMD_GET_CFG_RSP) < 0)
		{
			log_error("nc_xm_dev_recvMsg");
			result = NC_ERROR_RECV;
			ret = NC_ERROR_RECV;
			break;
		}
		if (tool_json2_getNumber(dev->xm_msg.data, "Ret", &xm_ret) < 0 || xm_ret != XM_RET_OK)
		{
			log_error("nc_xm_dev_recvMsg(%s) ret(%d)", dev->xm_msg.data, xm_ret);
			result = NC_ERROR_PROTOCOL;
			ret = NC_ERROR_PROTOCOL;
			break;
		}

		TOOL_INT32 array_x = 32;
		TOOL_INT32 array_y = 64;
		TOOL_INT8 array_value[32*64] = {0};
		TOOL_INT32 num = 0;
		TOOL_INT32 i = 0;
		TOOL_INT8 AlarmOutStatus[32] = {0};
		num = tool_json2_getArray(dev->xm_msg.data, "Alarm.AlarmOut", array_value, array_x, array_y);
		if (num < 0)
		{
			log_error("tool_json2_getArray(%s)", dev->xm_msg.data);
			result = NC_ERROR_PROTOCOL;
			ret = NC_ERROR_PROTOCOL;
			break;
		}
		cfg_status.io_out = 0;
		for (i = 0; i < num && i < dev->alarm_out_num; i++)
		{	
			if (tool_json2_getValue(&array_value[i*array_y], "AlarmOutStatus", AlarmOutStatus, sizeof(AlarmOutStatus)) < 0)
			{
				log_error("tool_json2_getArray(%s)", dev->xm_msg.data);
				result = NC_ERROR_PROTOCOL;
				ret = NC_ERROR_PROTOCOL;
				break;
			}
			if (tool_str_strncmp(AlarmOutStatus, "CLOSE", sizeof(AlarmOutStatus)-1) == 0)
				cfg_status.io_out |= (1<<i);
		}
		log_state("nc_xm_dev_doGetStatus io_out(%016llx) num(%d)", cfg_status.io_out, num);
	}	while (0);

	tool_thread_lockMutex(&dev->mutex);
	tool_mem_memcpy(&dev->cfg_get_status, &cfg_status, sizeof(ptc_cfg_status_t));
	dev->result_status = result;
	dev->last_get_status_req = dev->cur_get_status_req;
	tool_thread_broadcastCond(&dev->cond_status);
	tool_thread_unlockMutex(&dev->mutex);
	return ret; 
}

TOOL_INT32 nc_xm_dev_doSetStatus(nc_xm_dev_t* dev)
{
	TOOL_INT32 set_status_req;
	ptc_cfg_status_t cfg_status;
	tool_thread_lockMutex(&dev->mutex);
	set_status_req = dev->cur_set_status_req;
	tool_mem_memcpy(&cfg_status, &dev->cfg_set_status, sizeof(ptc_cfg_status_t));
	tool_thread_unlockMutex(&dev->mutex);
	if (dev->last_set_status_req == set_status_req)
		return 0;

	log_state("nc_xm_dev_doSetStatus io_out(%016llx)", cfg_status.io_out);
	TOOL_INT8 AlarmOutStatus[32][32] = {{0}};
	TOOL_INT32 i = 0;
	for (i = 0; i < 32; i++)
	{
		if (i < dev->alarm_out_num)
		{
			if ((cfg_status.io_out>>i)&0x1)
				tool_str_strncpy(AlarmOutStatus[i], "MANUAL", 6);
			else
				tool_str_strncpy(AlarmOutStatus[i], "CLOSE", 5);
		}
		else
		{
			tool_str_strncpy(AlarmOutStatus[i], "AUTO", 4);
		}
	}
	TOOL_INT32 xm_ret = 0;
	tool_mem_memset(&dev->xm_msg, sizeof(xm_msg_head_t));
	dev->xm_msg.head.session_id = dev->session_id;
	dev->xm_msg.head.command_id = XM_CMD_SET_CFG_REQ;
	dev->xm_msg.head.data_len = snprintf((TOOL_INT8*)dev->xm_msg.data, sizeof(dev->xm_msg.data),
		"{ "
			"\"Alarm.AlarmOut\" : "
			"[ "
				"{ \"AlarmOutStatus\" : \"OPEN\", \"AlarmOutType\" : \"%s\" }," 
				"{ \"AlarmOutStatus\" : \"OPEN\", \"AlarmOutType\" : \"%s\" }," 
				"{ \"AlarmOutStatus\" : \"OPEN\", \"AlarmOutType\" : \"%s\" }," 
				"{ \"AlarmOutStatus\" : \"OPEN\", \"AlarmOutType\" : \"%s\" }," 
				"{ \"AlarmOutStatus\" : \"OPEN\", \"AlarmOutType\" : \"%s\" }," 
				"{ \"AlarmOutStatus\" : \"OPEN\", \"AlarmOutType\" : \"%s\" }," 
				"{ \"AlarmOutStatus\" : \"OPEN\", \"AlarmOutType\" : \"%s\" }," 
				"{ \"AlarmOutStatus\" : \"OPEN\", \"AlarmOutType\" : \"%s\" }," 
				"{ \"AlarmOutStatus\" : \"OPEN\", \"AlarmOutType\" : \"%s\" }," 
				"{ \"AlarmOutStatus\" : \"OPEN\", \"AlarmOutType\" : \"%s\" }," 
				"{ \"AlarmOutStatus\" : \"OPEN\", \"AlarmOutType\" : \"%s\" }," 
				"{ \"AlarmOutStatus\" : \"OPEN\", \"AlarmOutType\" : \"%s\" }," 
				"{ \"AlarmOutStatus\" : \"OPEN\", \"AlarmOutType\" : \"%s\" }," 
				"{ \"AlarmOutStatus\" : \"OPEN\", \"AlarmOutType\" : \"%s\" }," 
				"{ \"AlarmOutStatus\" : \"OPEN\", \"AlarmOutType\" : \"%s\" }," 
				"{ \"AlarmOutStatus\" : \"OPEN\", \"AlarmOutType\" : \"%s\" }," 
				"{ \"AlarmOutStatus\" : \"OPEN\", \"AlarmOutType\" : \"%s\" }," 
				"{ \"AlarmOutStatus\" : \"OPEN\", \"AlarmOutType\" : \"%s\" }," 
				"{ \"AlarmOutStatus\" : \"OPEN\", \"AlarmOutType\" : \"%s\" }," 
				"{ \"AlarmOutStatus\" : \"OPEN\", \"AlarmOutType\" : \"%s\" }," 
				"{ \"AlarmOutStatus\" : \"OPEN\", \"AlarmOutType\" : \"%s\" }," 
				"{ \"AlarmOutStatus\" : \"OPEN\", \"AlarmOutType\" : \"%s\" }," 
				"{ \"AlarmOutStatus\" : \"OPEN\", \"AlarmOutType\" : \"%s\" }," 
				"{ \"AlarmOutStatus\" : \"OPEN\", \"AlarmOutType\" : \"%s\" }," 
				"{ \"AlarmOutStatus\" : \"OPEN\", \"AlarmOutType\" : \"%s\" }," 
				"{ \"AlarmOutStatus\" : \"OPEN\", \"AlarmOutType\" : \"%s\" }," 
				"{ \"AlarmOutStatus\" : \"OPEN\", \"AlarmOutType\" : \"%s\" }," 
				"{ \"AlarmOutStatus\" : \"OPEN\", \"AlarmOutType\" : \"%s\" }," 
				"{ \"AlarmOutStatus\" : \"OPEN\", \"AlarmOutType\" : \"%s\" }," 
				"{ \"AlarmOutStatus\" : \"OPEN\", \"AlarmOutType\" : \"%s\" }," 
				"{ \"AlarmOutStatus\" : \"OPEN\", \"AlarmOutType\" : \"%s\" }," 
				"{ \"AlarmOutStatus\" : \"OPEN\", \"AlarmOutType\" : \"%s\" } "
			"], "
			"\"Name\" : \"Alarm.AlarmOut\", "
			"\"SessionID\" : \"0x%x\" "
		"}",
		AlarmOutStatus[0],
		AlarmOutStatus[1],
		AlarmOutStatus[2],
		AlarmOutStatus[3],
		AlarmOutStatus[4],
		AlarmOutStatus[5],
		AlarmOutStatus[6],
		AlarmOutStatus[7],
		AlarmOutStatus[8],
		AlarmOutStatus[9],
		AlarmOutStatus[10],
		AlarmOutStatus[11],
		AlarmOutStatus[12],
		AlarmOutStatus[13],
		AlarmOutStatus[14],
		AlarmOutStatus[15],
		AlarmOutStatus[16],
		AlarmOutStatus[17],
		AlarmOutStatus[18],
		AlarmOutStatus[19],
		AlarmOutStatus[20],
		AlarmOutStatus[21],
		AlarmOutStatus[22],
		AlarmOutStatus[23],
		AlarmOutStatus[24],
		AlarmOutStatus[25],
		AlarmOutStatus[26],
		AlarmOutStatus[27],
		AlarmOutStatus[28],
		AlarmOutStatus[29],
		AlarmOutStatus[30],
		AlarmOutStatus[31],
		dev->session_id);
	if (tool_ptc_xm_sendMsg(&dev->msg_sock, &dev->xm_msg) < 0)
	{
		log_error("tool_ptc_xm_sendMsg");
		return -1;
	}
	if (nc_xm_dev_recvMsg(dev, XM_CMD_SET_CFG_RSP) < 0)
	{
		log_error("nc_xm_dev_recvMsg");
		return -1;
	}
	if (tool_json2_getNumber(dev->xm_msg.data, "Ret", &xm_ret) < 0 || xm_ret != XM_RET_OK)
	{
		log_error("nc_xm_dev_recvMsg(%s) ret(%d)", dev->xm_msg.data, xm_ret);
		return -1;
	}	
	dev->last_set_status_req = set_status_req;
	return 0;
}

TOOL_INT32 nc_xm_dev_doRecv(nc_xm_dev_t* dev)
{
	TOOL_INT32 ret = 0;
	ret = tool_sock_item_pollRead(&dev->msg_sock, 0);
	if (ret < 0)
	{
		log_error("tool_sock_item_pollRead dev(%d)", dev->dev_id);
		return -1;
	}
	else if (ret == 0)
		return 0;
	if (tool_ptc_xm_recvMsg(&dev->msg_sock, &dev->xm_msg) < 0)
	{
		log_error("tool_ptc_xm_recvMsg dev(%d)", dev->dev_id);
		return -1;
	}
	log_state("u16Command(0d%d)", dev->xm_msg.head.command_id);
	dev->is_sleep = 0;
	return 0;
}

TOOL_VOID* nc_xm_dev_runLogin(TOOL_VOID* param)
{
	nc_xm_dev_t* dev = (nc_xm_dev_t*)param;
	log_state("dev(%d) Login start", dev->dev_id);
	nc_xm_dev_setOnline(dev, 0);

	while (dev->state)
	{
		if (nc_xm_dev_login(dev) < 0)
			continue;
		dev->is_sleep = 1;
		if (nc_xm_dev_heartbeat(dev) < 0 ||
			nc_xm_dev_doRts(dev) < 0 ||
			nc_xm_dev_doPtz(dev) < 0 ||
			nc_xm_dev_doRecords(dev) < 0 ||
			nc_xm_dev_doPbs(dev) < 0 ||
			nc_xm_dev_doLogs(dev) < 0 ||
			nc_xm_dev_doTalk(dev) < 0 ||
			nc_xm_dev_doSyncTime(dev) < 0 ||
			nc_xm_dev_doGetStatus(dev) < 0 ||
			nc_xm_dev_doSetStatus(dev) < 0 ||
			nc_xm_dev_doRecv(dev) < 0)
		{
			nc_xm_dev_logout(dev);
			continue;
		}

		if (dev->is_sleep)
			tool_sysf_usleep(20*1000);
	}

	log_state("dev(%d) Login stop", dev->dev_id);
	nc_xm_dev_logout(dev);
    return NULL;
}

TOOL_INT32 nc_xm_dev_start(nc_xm_dev_t* dev, nc_dev* pst_nc_dev)
{
	tool_mem_memcpy(&dev->st_nc_dev, pst_nc_dev, sizeof(nc_dev));
	tool_sock_item_init(&dev->msg_sock);
	tool_sysf_setTick(&dev->state_tick_sec, &dev->state_tick_usec);	
	dev->pst_xm_search = (xm_search_t*)tool_mem_malloc(sizeof(xm_search_t), 0);
	dev->state = 1;
	if (tool_thread_create(&dev->login_pid, NULL, nc_xm_dev_runLogin, (TOOL_VOID*)dev) < 0)
		log_fatalNo("tool_thread_create");
	return 0;	
}

TOOL_VOID nc_xm_dev_stop(nc_xm_dev_t* dev)
{
	if (dev->state == 0)
		return;
	
	dev->state = 0;
	tool_thread_join(dev->login_pid);
	tool_mem_free(dev->pst_xm_search);
	dev->pst_xm_search = NULL;
}

TOOL_INT32 nc_xm_dev_startRts(nc_xm_dev_t* dev, TOOL_INT32 channel_no, TOOL_INT32 stream_type, NC_CB cb, TOOL_VOID* param)
{
	TOOL_INT32 ret = 0;
	tool_thread_lockMutex(&dev->mutex);
	if (dev->cur_rts_enable[channel_no][stream_type])
	{
		log_error("nc_xm_dev_startRts repeat[%d]", channel_no);
		tool_thread_unlockMutex(&dev->mutex);
		return NC_ERROR_REPEAT;
	}
//	tool_stream3_init(&dev->rts[channel_no][stream_type].stream, NC_XM_RTS_POOL_SIZE);
//	dev->rts[channel_no][stream_type].recv_buf = (TOOL_UINT8*)tool_mem_malloc(PTC_VIDEO_FRAME_SIZE, 0);
//	dev->rts[channel_no][stream_type].cb_frame = (ptc_frame_video_t*)tool_mem_malloc(sizeof(ptc_frame_video_t), 0);
	dev->rts[channel_no][stream_type].cb = cb;
	dev->rts[channel_no][stream_type].param = param;
	dev->cur_rts_enable[channel_no][stream_type] = 1;
//	tool_stream3_initReadPos(&dev->rts[channel_no][stream_type].stream, &dev->rts[channel_no][stream_type].read_pos);
//	if (tool_thread_create(&dev->rts_pid[channel_no][stream_type], NULL, nc_xm_rts_runPlay, (TOOL_VOID*)&dev->rts[channel_no][stream_type]) < 0)
//		log_fatalNo("tool_thread_create");
//	if (tool_thread_create(&dev->recv_rts_pid[channel_no][stream_type], NULL, nc_xm_rts_runRecv, (TOOL_VOID*)&dev->rts[channel_no][stream_type]) < 0)
//		log_fatalNo("tool_thread_create");
	ret = dev->rts[channel_no][stream_type].rts_id;
	tool_sysf_setTick(&dev->state_tick_sec, &dev->state_tick_usec);
	tool_thread_unlockMutex(&dev->mutex);
	log_state("nc_xm_dev_startRts[%d][%d]", channel_no, stream_type);

	return ret;
}


TOOL_INT32 nc_xm_dev_stopRts(nc_xm_dev_t* dev, TOOL_INT32 channel_no, TOOL_INT32 stream_type)
{
	TOOL_INT32 ret = 0;
	tool_thread_lockMutex(&dev->mutex);
	if (dev->cur_rts_enable[channel_no][stream_type])
	{
		dev->cur_rts_enable[channel_no][stream_type] = 0;
//		tool_thread_join(dev->rts_pid[channel_no][stream_type]);
//		tool_thread_join(dev->recv_rts_pid[channel_no][stream_type]);
		dev->rts[channel_no][stream_type].cb = NULL;
		dev->rts[channel_no][stream_type].param = NULL;
//		tool_mem_free(dev->rts[channel_no][stream_type].recv_buf);
//		dev->rts[channel_no][stream_type].recv_buf = NULL;
//		tool_mem_free(dev->rts[channel_no][stream_type].cb_frame);
//		dev->rts[channel_no][stream_type].cb_frame = NULL;
//		tool_stream3_done(&dev->rts[channel_no][stream_type].stream);
	}
	
	tool_sysf_setTick(&dev->state_tick_sec, &dev->state_tick_usec);
	tool_thread_unlockMutex(&dev->mutex);
	return ret;
}

TOOL_INT32 nc_xm_dev_isSame(nc_dev* pst_nc_dev, nc_xm_dev_t* pst_xm_dev)
{
	if (pst_nc_dev->ptc == NC_PTC_XiongMai && pst_xm_dev->st_nc_dev.ptc == NC_PTC_XiongMai &&
		tool_str_strncmp(pst_xm_dev->st_nc_dev.ip, pst_nc_dev->ip, PTC_IP_SIZE-1) == 0 &&
		tool_str_strncmp(pst_xm_dev->st_nc_dev.user, pst_nc_dev->user, PTC_ID_SIZE-1) == 0 &&
		tool_str_strncmp(pst_xm_dev->st_nc_dev.pswd, pst_nc_dev->pswd, PTC_ID_SIZE-1) == 0 &&		
		pst_xm_dev->st_nc_dev.msg_port== pst_nc_dev->msg_port &&
		pst_xm_dev->state)
		return 1;
	return 0;
}

TOOL_INT32 nc_xm_dev_ptz(nc_xm_dev_t* dev, TOOL_INT32 channel_no, PTC_PTZ_CMD_E cmd, TOOL_INT32 param)
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

TOOL_INT32 nc_xm_dev_searchRecords(nc_xm_dev_t* dev, ptc_cfg_records_t* records)
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

TOOL_INT32 nc_xm_dev_startPbs(nc_xm_dev_t* dev, ptc_cfg_record_t* record, NC_CB cb, TOOL_VOID* param)
{
	TOOL_INT32 ret = 0;
	tool_thread_lockMutex(&dev->mutex);
	if (dev->cur_pbs_enable)
	{
		log_error("nc_xm_dev_startPbs repeat");
		tool_thread_unlockMutex(&dev->mutex);
		return NC_ERROR_REPEAT;
	}
	tool_mem_memcpy(&dev->pbs.ptc_record, record, sizeof(ptc_cfg_record_t));
//	tool_stream3_init(&dev->pbs.stream, NC_XM_PBS_POOL_SIZE);
//	dev->pbs.recv_buf = (TOOL_UINT8*)tool_mem_malloc(PTC_VIDEO_FRAME_SIZE, 0);
//	dev->pbs.cb_frame = (ptc_frame_video_t*)tool_mem_malloc(PTC_VIDEO_FRAME_SIZE, 0);
	dev->pbs.cb = cb;
	dev->pbs.param = param;
	dev->cur_pbs_enable = 1;
//	if (tool_thread_create(&dev->pbs_pid, NULL, nc_xm_pbs_runPlay, (TOOL_VOID*)&dev->pbs) < 0)
//		log_fatalNo("tool_thread_create");
//	if (tool_thread_create(&dev->recv_pbs_pid, NULL, nc_xm_pbs_runRecv, (TOOL_VOID*)&dev->pbs) < 0)
//		log_fatalNo("tool_thread_create");	
	ret = dev->pbs.pbs_no;
	tool_sysf_setTick(&dev->state_tick_sec, &dev->state_tick_usec);
	tool_thread_unlockMutex(&dev->mutex);
	log_state("nc_xm_dev_startPbs");
	return ret;
}

TOOL_INT32 nc_xm_dev_stopPbs(nc_xm_dev_t* dev)
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
//		tool_mem_free(dev->pbs.recv_buf);
//		dev->pbs.recv_buf = NULL;
//		tool_mem_free(dev->pbs.cb_frame);
//		dev->pbs.cb_frame = NULL;
//		tool_stream3_done(&dev->pbs.stream);
	}
	
	tool_sysf_setTick(&dev->state_tick_sec, &dev->state_tick_usec);
	tool_thread_unlockMutex(&dev->mutex);
	return ret;
}

TOOL_INT32 nc_xm_dev_searchLogs(nc_xm_dev_t* dev, ptc_logs_v2_t* logs_v2)
{
	TOOL_INT32 ret = 0;
	tool_thread_lockMutex(&dev->mutex);
	tool_sysf_setTick(&dev->state_tick_sec, &dev->state_tick_usec);
	dev->cur_logs_v2_req ++;
	tool_mem_memcpy(&dev->cfg_logs_v2, logs_v2, sizeof(ptc_logs_v2_t));
	if (tool_thread_timedwaitCond(&dev->cond_logs_v2, &dev->mutex, 5) < 0)
	{
		ret = NC_ERROR_TIMEOUT;
	}
	else
	{
		tool_mem_memcpy(logs_v2, &dev->cfg_logs_v2, sizeof(ptc_logs_v2_t));
		ret = dev->result_logs_v2;
	}
	tool_sysf_setTick(&dev->state_tick_sec, &dev->state_tick_usec);
	tool_thread_unlockMutex(&dev->mutex);
	return ret;
}

TOOL_INT32 nc_xm_dev_startTalk(nc_xm_dev_t* dev, NC_CB cb, TOOL_VOID* param)
{

	TOOL_INT32 ret = 0;
	tool_thread_lockMutex(&dev->mutex);
	if (dev->cur_talk_enable)
	{
		log_error("nc_xm_dev_startTalk repeat");
		tool_thread_unlockMutex(&dev->mutex);
		return NC_ERROR_REPEAT;
	}

	tool_thread_lockMutex(&dev->talk.mutex);
	dev->talk.cb = cb;
	dev->talk.param = param;
	tool_thread_unlockMutex(&dev->talk.mutex);
	
	dev->cur_talk_enable = 1;
	ret = (dev->st_nc_dev.ptc | (dev->dev_id<<8) | dev->talk.talk_no);
	tool_sysf_setTick(&dev->state_tick_sec, &dev->state_tick_usec);
	tool_thread_unlockMutex(&dev->mutex);
	log_state("nc_xm_dev_startTalk");
	return ret;

/*	
	TOOL_INT32 ret = 0;
	tool_thread_lockMutex(&dev->mutex);
	if (dev->cur_talk_enable)
	{
		log_error("nc_xm_dev_startTalk repeat");
		tool_thread_unlockMutex(&dev->mutex);
		return NC_ERROR_REPEAT;
	}

	dev->talk.send_stream = (tool_stream2_t*)tool_mem_malloc(sizeof(tool_stream2_t), 0);
	tool_stream2_init(dev->talk.send_stream);
	tool_stream2_initReadPos(dev->talk.send_stream, &dev->talk.send_pos);
	dev->talk.cb = cb;
	dev->talk.param = param;
	dev->cur_talk_enable = 1;
	if (tool_thread_create(&dev->talk_recv_pid, NULL, nc_xm_talk_runRecv, (TOOL_VOID*)&dev->talk) < 0)
			log_fatalNo("tool_thread_create");
	if (tool_thread_create(&dev->talk_send_pid, NULL, nc_xm_talk_runSend, (TOOL_VOID*)&dev->talk) < 0)
			log_fatalNo("tool_thread_create");
	ret = (dev->st_nc_dev.ptc | (dev->dev_id<<8) | dev->talk.talk_no);
	tool_sysf_setTick(&dev->state_tick_sec, &dev->state_tick_usec);
	tool_thread_unlockMutex(&dev->mutex);
	log_state("nc_xm_dev_startTalk");
	return ret;
*/
}

TOOL_INT32 nc_xm_dev_stopTalk(nc_xm_dev_t* dev)
{
	TOOL_INT32 ret = 0;
	tool_thread_lockMutex(&dev->mutex);
	dev->cur_talk_enable = 0;
	
	tool_thread_lockMutex(&dev->talk.mutex);
	dev->talk.cb = NULL;
	dev->talk.param = NULL;
	tool_thread_unlockMutex(&dev->talk.mutex);
 	
	tool_sysf_setTick(&dev->state_tick_sec, &dev->state_tick_usec);
	tool_thread_unlockMutex(&dev->mutex);
	return ret;

/*	
	TOOL_INT32 ret = 0;
	tool_thread_lockMutex(&dev->mutex);
	if (dev->cur_talk_enable)
	{
		dev->cur_talk_enable = 0;
		tool_thread_join(dev->talk_recv_pid);
		tool_thread_join(dev->talk_send_pid);
		dev->talk.cb = NULL;
		dev->talk.param = NULL;
		tool_stream2_done(dev->talk.send_stream);
		tool_mem_free(dev->talk.send_stream);
		dev->talk.send_stream = NULL;
	}
	
	tool_sysf_setTick(&dev->state_tick_sec, &dev->state_tick_usec);
	tool_thread_unlockMutex(&dev->mutex);
	return ret;
*/	
}


TOOL_INT32 nc_xm_dev_syncTime(nc_xm_dev_t* dev, TOOL_UINT32 utc_sec, TOOL_INT32 zone, TOOL_INT32 dst_hour)
{
	tool_thread_lockMutex(&dev->mutex);
	dev->cur_time_req ++;
	dev->utc_sec = utc_sec;
	dev->zone = zone;
	dev->dst_hour = dst_hour;
	tool_sysf_setTick(&dev->state_tick_sec, &dev->state_tick_usec);
	tool_thread_unlockMutex(&dev->mutex);
	return 0;
}


TOOL_INT32 nc_xm_dev_getStatus(nc_xm_dev_t* dev, ptc_cfg_status_t* cfg_status)
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

TOOL_INT32 nc_xm_dev_setStatus(nc_xm_dev_t* dev, ptc_cfg_status_t* cfg_status)
{
	tool_thread_lockMutex(&dev->mutex);
	dev->cur_set_status_req ++;
	tool_mem_memcpy(&dev->cfg_set_status, cfg_status, sizeof(ptc_cfg_status_t));
	tool_thread_unlockMutex(&dev->mutex);
	return 0;
}

TOOL_VOID nc_xm_dev_init(nc_xm_dev_t* dev, TOOL_INT32 dev_id, TOOL_VOID* father_nc)
{
	dev->dev_id = dev_id;
	dev->father_nc = father_nc;
	tool_thread_initMutex(&dev->mutex);
	tool_sock_item_setFlag(&dev->msg_sock, &dev->state);
	
	//rts
	TOOL_INT32 i = 0, j = 0;
	for (i = 0; i < NC_XM_CHANNEL_SIZE; i++)
	{
		for (j = 0; j < NC_XM_STREAM_SIZE; j++)
		{
			dev->rts[i][j].rts_id = NC_PTC_XiongMai | ((dev->dev_id&0xFF) << 8) | ((i&0x3F) << 2) | (j&0x3);
			dev->rts[i][j].channel_no = i;
			dev->rts[i][j].stream_type = j;
			dev->rts[i][j].father_dev = dev;
			dev->rts[i][j].tick = -12345678;
			tool_sock_item_setFlag(&dev->rts[i][j].rts_sock, &dev->rts[i][j].state);
		}
	}
	//pbs
	dev->pbs.father_dev = dev;
	dev->pbs.pbs_no = (NC_PTC_XiongMai | ((dev->dev_id&0xFF)<<8) | ((NC_XM_CHANNEL_PBS&0x3F) << 2));
	dev->pbs.reconnect_tick_sec = -12345678;
	tool_sock_item_setFlag(&dev->pbs.pbs_sock, &dev->pbs.state);
	//talk
	dev->talk.father_dev = dev;
	dev->talk.talk_no = (NC_PTC_XiongMai | ((dev->dev_id&0xFF)<<8) | ((NC_XM_CHANNEL_TALK&0x3F) << 2));
	tool_thread_initMutex(&dev->talk.mutex);
	dev->talk.tick = -12345678;
	tool_sock_item_setFlag(&dev->talk.talk_sock, &dev->talk.state);
	//cfg
	tool_thread_initCond(&dev->cond_records);
	tool_thread_initCond(&dev->cond_logs_v2);
	tool_thread_initCond(&dev->cond_status);	
}

TOOL_VOID nc_xm_dev_done(nc_xm_dev_t* dev)
{
	tool_thread_doneMutex(&dev->mutex);
	TOOL_INT32 i = 0, j = 0;
	for (i = 0; i < NC_XM_CHANNEL_SIZE; i++)
	{
		for (j = 0; j < NC_XM_STREAM_SIZE; j++)
		{
		}
	}

	//pbs

	//talk
	tool_thread_doneMutex(&dev->talk.mutex);

	//cfg
	tool_thread_doneCond(&dev->cond_records);
	tool_thread_doneCond(&dev->cond_logs_v2);
	tool_thread_doneCond(&dev->cond_status);
}


#define __NC_XM_INFO__

TOOL_VOID* nc_xm_run(TOOL_VOID* param)
{
	TOOL_INT32 i = 0;
	TOOL_INT32 j = 0;
	TOOL_INT32 k = 0;
	TOOL_INT32 ret = 0;
	TOOL_INT32 count = 0;
	nc_xm_t* nc = (nc_xm_t*)param;
	nc_xm_dev_t* dev = NULL;
	while (nc->state)
	{
		count = 0;
		tool_thread_lockMutex(&nc->mutex);
		for (i = 0; i < NC_XM_DEVICE_SIZE; i++)
		{
			dev = &nc->dev[i];
			if (dev->state == 0)
				continue;
			
			ret = 0;
			for (j = 0; j < NC_XM_CHANNEL_SIZE; j++)
			{
				for (k = 0; k < NC_XM_STREAM_SIZE; k++)
				{
					if (dev->cur_rts_enable[j][k])
						ret ++;
				}
			}

			if (dev->cur_pbs_enable)
				ret ++;
			if (dev->cur_talk_enable)
				ret ++;
			if (ret == 0 && tool_sysf_isTickTimeout(dev->state_tick_sec, dev->state_tick_usec, 60*1000*1000))
			{
				nc_xm_dev_stop(dev);
			}
			else
			{
				count ++;
			}
		}
		tool_thread_unlockMutex(&nc->mutex);
	//	log_debug("nc_xm_run count(%d)", count);
		tool_sysf_sleep(1);
	}
	return NULL;
}

TOOL_VOID nc_xm_init()
{
	if (g_nc_xm)
		return ;
	log_state("init xm sdk(v%04x.%04x)", PTC_VERSION, PTC_DATA);
	g_nc_xm = (nc_xm_t*)tool_mem_malloc(sizeof(nc_xm_t), 1);
    tool_thread_initMutex(&g_nc_xm->mutex);
	g_nc_xm->buffer_usec = 300*1000;
	TOOL_INT32 i = 0;
	for (i = 0; i < NC_XM_DEVICE_SIZE; i++)
		nc_xm_dev_init(&g_nc_xm->dev[i], i, g_nc_xm);
	g_nc_xm->state = 1;
	if (tool_thread_create(&g_nc_xm->pid, NULL, nc_xm_run, (TOOL_VOID*)g_nc_xm) < 0)
		log_fatal("tool_thread_create");
}

TOOL_INT32 nc_xm_setBuffer(TOOL_INT32 buffer_usec)
{
	if (g_nc_xm == NULL)
	{
		log_error("nc");
		return NC_ERROR_UNINIT;
	}
	if (buffer_usec < 0 || buffer_usec > 3000*1000)
	{
		log_error("buffer_usec(%d)", buffer_usec);
		return NC_ERROR_INPUT;
	}
	g_nc_xm->buffer_usec = buffer_usec;
	return 0;
}

TOOL_VOID nc_xm_done()
{
    if (g_nc_xm == NULL)
        return;
	g_nc_xm->state = 0;
	tool_thread_join(g_nc_xm->pid);
    log_state("done xm sdk(v%04x.%04x)", PTC_VERSION, PTC_DATA);
	TOOL_INT32 i = 0;
	for (i = 0; i < NC_XM_DEVICE_SIZE; i++)
		nc_xm_dev_done(&g_nc_xm->dev[i]);
	tool_thread_doneMutex(&g_nc_xm->mutex);
	tool_mem_free(g_nc_xm);
	g_nc_xm = NULL;
	tool_sock_done();
}

TOOL_INT32 nc_xm_searchDev(TOOL_VOID* param)
{
	if (g_nc_xm == NULL)
	{
		log_error("nc");
		return NC_ERROR_UNINIT;
	}
	ptc_mcast_searchs_t* searchs = (ptc_mcast_searchs_t*)param;
	if (searchs == NULL)
	{
		log_error("nc_xm_searchDev arg");
		return NC_ERROR_INPUT;
	}		
	searchs->num = 0;

	TOOL_INT32 tick_sec = 0;
	TOOL_INT32 tick_usec = 0;
	TOOL_INT32 ret = 0;
	TOOL_INT32 ip_addr = 0;
	TOOL_INT32 port = 0;
	TOOL_INT32 result = -1;
	xm_msg_t st_msg;
	tool_sock_item_t sock_s2c;
	do
	{
		if (tool_sock_item_open_v2(&sock_s2c, TOOL_SOCK_TYPE1_BCAST, TOOL_SOCK_TYPE2_NONE, XM_BCAST_IP, XM_BCAST_PORT, NULL, XM_BCAST_PORT) < 0)
		{
			log_error("tool_sock_createMcast sock_s2c");
			result = NC_ERROR_INTERNAL;
			break;
		}

		tool_mem_memset(&st_msg, sizeof(xm_msg_head_t));
		st_msg.head.head_flag = XM_HEAD_FLAG;
		st_msg.head.command_id = XM_CMD_SEARCH_DEV_REQ;
		if (tool_sock_item_sendto(&sock_s2c, &st_msg, sizeof(xm_msg_head_t)) < 0)
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

			tool_mem_memset(&st_msg, sizeof(xm_msg_head_t));
			if (tool_sock_item_recvfrom(&sock_s2c, &st_msg, sizeof(xm_msg_t)) < 0)
			{
				log_error("tool_sock_item_sendto");
				break;
			}

 	//		log_debug("cmd(0x%04x) flag(%02x)", st_msg.head.command_id, st_msg.head.head_flag);
			if (st_msg.head.command_id == XM_CMD_SEARCH_DEV_RSP && st_msg.head.head_flag == XM_HEAD_FLAG)
			{
				if (tool_json2_getNumber((TOOL_INT8*)st_msg.data, "Ret", &ret) < 0 || ret != XM_RET_OK)
				{
					log_debug("st_msg.data(%s) ret(%d)", st_msg.data, ret);
					continue;			
				}
				
				tool_mem_memset(&searchs->search[searchs->num], sizeof(ptc_mcast_search_t));
				searchs->search[searchs->num].net.enable1 = 1;
				if (tool_json2_getNumber((TOOL_INT8*)st_msg.data, "HostIP", &ip_addr) < 0)
				{
					log_debug("st_msg.data(%s)", st_msg.data);
					continue;			
				}
				tool_mem_memcpy(searchs->search[searchs->num].net.ip1, &ip_addr, 4);
				
				if (tool_json2_getNumber((TOOL_INT8*)st_msg.data, "GateWay", &ip_addr) < 0)
				{
					log_debug("st_msg.data(%s)", st_msg.data);
					continue;
				}
				tool_mem_memcpy(searchs->search[searchs->num].net.gate1, &ip_addr, 4);

				if (tool_json2_getNumber((TOOL_INT8*)st_msg.data, "HttpPort", &port) < 0)
				{
					log_debug("st_msg.data(%s)", st_msg.data);
					continue;	
				}
				searchs->search[searchs->num].net.web_port = port;

				if (tool_json2_getNumber((TOOL_INT8*)st_msg.data, "TCPPort", &port) < 0)
				{
					log_debug("st_msg.data(%s)", st_msg.data);
					continue;	
				}
				searchs->search[searchs->num].net.dev_port = port;
				searchs->search[searchs->num].nc_ptc_type = NC_PTC_XiongMai;
				searchs->num ++;
			}
		}
		result = 0;		
	}while (0);
	tool_sock_item_close(&sock_s2c);
	return result;
}




TOOL_INT32 nc_xm_getChannelNum_v2(nc_dev* pst_nc_dev)
{
	if (g_nc_xm == NULL)
	{
		log_error("nc");
		return NC_ERROR_UNINIT;
	}
	if (pst_nc_dev == NULL)
	{
		log_error("pst_nc_dev(0x%08x)", pst_nc_dev);
		return NC_ERROR_INPUT;
	}
	TOOL_INT32 ret = 0;
	TOOL_INT32 ch_num = 0;
	nc_xm_dev_t dev;
	tool_mem_memset(&dev, sizeof(nc_xm_dev_t));
	tool_mem_memcpy(&dev.st_nc_dev, pst_nc_dev, sizeof(nc_dev));
	dev.state = 1;
	tool_sock_item_init(&dev.msg_sock);
	ret = nc_xm_dev_login(&dev);
	if (ret < 0)
	{
		dev.state = 0;
		return ret;
	}
	ch_num = dev.channel_num;
	nc_xm_dev_logout(&dev);
	dev.state = 0;
	return ch_num;
}

TOOL_INT32 nc_xm_startRts_v2(nc_dev* info, TOOL_INT32 channel, TOOL_INT32 stream_type, NC_CB cb, void* param)
{
	if (g_nc_xm == NULL)
	{
		log_error("nc");
		return NC_ERROR_UNINIT;
	}
	if (info == NULL || channel < 0 || channel >= NC_XM_CHANNEL_SIZE || stream_type < 0 || stream_type >= NC_XM_STREAM_SIZE)
	{
		log_error("info(0x%08x), channel(%d), stream_type(%d)", info, channel, stream_type);
		return NC_ERROR_INPUT;
	}

	TOOL_INT32 i = 0;
	TOOL_INT32 ret = 0;
	tool_thread_lockMutex(&g_nc_xm->mutex);
	for (i = 0; i < NC_XM_DEVICE_SIZE; i++)
	{
		if (nc_xm_dev_isSame(info, &g_nc_xm->dev[i]))
		{
			ret = nc_xm_dev_startRts(&g_nc_xm->dev[i], channel, stream_type, cb, param);
			tool_thread_unlockMutex(&g_nc_xm->mutex);
			return ret;
		}
	}
	for (i = 0; i < NC_XM_DEVICE_SIZE; i++)
	{
		if (g_nc_xm->dev[i].state == 0)
			break;
	}
	if (i == NC_XM_DEVICE_SIZE)
	{
		log_error("nc full");
		tool_thread_unlockMutex(&g_nc_xm->mutex);
		return NC_ERROR_FULL;
	}
	
	ret = nc_xm_dev_start(&g_nc_xm->dev[i], info);
	if (ret < 0)
	{
		log_error("nc_xm_dev_start id(%s), addr(%s:%d)", info->dev_id, info->ip, info->msg_port);
		tool_thread_unlockMutex(&g_nc_xm->mutex);
		return ret;
	}
	ret = nc_xm_dev_startRts(&g_nc_xm->dev[i], channel, stream_type, cb, param);
	if (ret < 0)
	{
		log_error("nc_xm_dev_startRts[%d][%d]", channel, stream_type);
		nc_xm_dev_stop(&g_nc_xm->dev[i]);
	}
	tool_thread_unlockMutex(&g_nc_xm->mutex);
	return ret;
}

TOOL_INT32 nc_xm_stopRts(TOOL_INT32 id)
{
	if (g_nc_xm == NULL)
	{
		log_error("nc");
		return NC_ERROR_UNINIT;
	}

	TOOL_INT32 d = 0; 
	TOOL_INT32 c = 0;
	TOOL_INT32 s = 0;
	tool_thread_lockMutex(&g_nc_xm->mutex);
	for (d = 0; d < NC_XM_DEVICE_SIZE; d ++)
	{
		for (c = 0; c < NC_XM_CHANNEL_SIZE; c++)
		{
			for (s = 0; s < NC_XM_STREAM_SIZE; s++)
			{
				if (g_nc_xm->dev[d].rts[c][s].rts_id == id)
				{
					nc_xm_dev_stopRts(&g_nc_xm->dev[d], c, s);
					tool_thread_unlockMutex(&g_nc_xm->mutex);
					return 0;
				}
			}
		}
	}
	tool_thread_unlockMutex(&g_nc_xm->mutex);
	return 0;
}

TOOL_INT32 nc_xm_controlPTZ_v2(nc_dev* info, TOOL_INT32 channel, PTC_PTZ_CMD_E cmd, TOOL_INT32 param)
{
	if (g_nc_xm == NULL)
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
	tool_thread_lockMutex(&g_nc_xm->mutex);
	for (i = 0; i < NC_XM_DEVICE_SIZE; i++)
	{
		if (nc_xm_dev_isSame(info, &g_nc_xm->dev[i]))
		{
			break;
		}
	}
	if (i == NC_XM_DEVICE_SIZE)
	{
		tool_thread_unlockMutex(&g_nc_xm->mutex);
		return NC_ERROR_PTZNORTS;
	}
	ret = nc_xm_dev_ptz(&g_nc_xm->dev[i], channel, cmd, param);
	tool_thread_unlockMutex(&g_nc_xm->mutex);
	return ret;
}

TOOL_INT32 nc_xm_searchRecords(nc_dev* info, ptc_cfg_records_t* records)
{
	if (g_nc_xm == NULL)
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
	tool_thread_lockMutex(&g_nc_xm->mutex);
	for (i = 0; i < NC_XM_DEVICE_SIZE; i++)
	{
		if (nc_xm_dev_isSame(info, &g_nc_xm->dev[i]))
		{
			ret = nc_xm_dev_searchRecords(&g_nc_xm->dev[i], records);
			tool_thread_unlockMutex(&g_nc_xm->mutex);
			return ret;
		}
	}
	for (i = 0; i < NC_XM_DEVICE_SIZE; i++)
	{
		if (g_nc_xm->dev[i].state == 0)
			break;
	}
	if (i == NC_XM_DEVICE_SIZE)
	{
		log_error("nc full");
		tool_thread_unlockMutex(&g_nc_xm->mutex);
		return NC_ERROR_FULL;
	}
	
	ret = nc_xm_dev_start(&g_nc_xm->dev[i], info);
	if (ret < 0)
	{
		log_error("nc_xm_dev_start id(%s), addr(%s:%d)", info->dev_id, info->ip, info->msg_port);
		tool_thread_unlockMutex(&g_nc_xm->mutex);
		return ret;
	}
	ret = nc_xm_dev_searchRecords(&g_nc_xm->dev[i], records);
	if (ret < 0)
	{
		log_error("nc_xm_dev_searchRecords");
		nc_xm_dev_stop(&g_nc_xm->dev[i]);
	}
	tool_thread_unlockMutex(&g_nc_xm->mutex);
	return ret;	
}

TOOL_INT32 nc_xm_startPbs(nc_dev* info, ptc_cfg_record_t* ptc_record, NC_CB cb, void* param)
{
	if (g_nc_xm == NULL)
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
	tool_thread_lockMutex(&g_nc_xm->mutex);
	for (i = 0; i < NC_XM_DEVICE_SIZE; i++)
	{
		if (nc_xm_dev_isSame(info, &g_nc_xm->dev[i]))
		{
			ret = nc_xm_dev_startPbs(&g_nc_xm->dev[i], ptc_record, cb, param);
			tool_thread_unlockMutex(&g_nc_xm->mutex);
			return ret;
		}
	}
	for (i = 0; i < NC_XM_DEVICE_SIZE; i++)
	{
		if (g_nc_xm->dev[i].state == 0)
			break;
	}
	if (i == NC_XM_DEVICE_SIZE)
	{
		log_error("nc full");
		tool_thread_unlockMutex(&g_nc_xm->mutex);
		return NC_ERROR_FULL;
	}
	
	ret = nc_xm_dev_start(&g_nc_xm->dev[i], info);
	if (ret < 0)
	{
		log_error("nc_xm_dev_start id(%s), addr(%s:%d)", info->dev_id, info->ip, info->msg_port);
		tool_thread_unlockMutex(&g_nc_xm->mutex);
		return ret;
	}
	ret = nc_xm_dev_startPbs(&g_nc_xm->dev[i], ptc_record, cb, param);
	if (ret < 0)
	{
		log_error("nc_xm_dev_startPbs");
		nc_xm_dev_stop(&g_nc_xm->dev[i]);
	}
	tool_thread_unlockMutex(&g_nc_xm->mutex);
	return ret;
}

TOOL_INT32 nc_xm_stopPbs(TOOL_INT32 id)
{
	if (g_nc_xm == NULL)
	{
		log_error("nc");
		return NC_ERROR_UNINIT;
	}

	TOOL_INT32 d = 0;
	tool_thread_lockMutex(&g_nc_xm->mutex);
	for (d = 0; d < NC_XM_DEVICE_SIZE; d ++)
	{
		if (g_nc_xm->dev[d].pbs.pbs_no == id)
		{
			nc_xm_dev_stopPbs(&g_nc_xm->dev[d]);
			tool_thread_unlockMutex(&g_nc_xm->mutex);
			return 0;
		}
	}
	tool_thread_unlockMutex(&g_nc_xm->mutex);
	return 0;	
}

TOOL_INT32 nc_xm_searchLogs(nc_dev* info, ptc_logs_v2_t* logs_v2)
{
	if (g_nc_xm == NULL)
	{
		log_error("nc");
		return NC_ERROR_UNINIT;
	}
	if (info == NULL || logs_v2 == NULL)
	{
		log_error("info(0x%08x), logs_v2(0x%08x)", info, logs_v2);
		return NC_ERROR_INPUT;
	}

	TOOL_INT32 i = 0;
	TOOL_INT32 ret = 0;
	tool_thread_lockMutex(&g_nc_xm->mutex);
	for (i = 0; i < NC_XM_DEVICE_SIZE; i++)
	{
		if (nc_xm_dev_isSame(info, &g_nc_xm->dev[i]))
		{
			ret = nc_xm_dev_searchLogs(&g_nc_xm->dev[i], logs_v2);
			tool_thread_unlockMutex(&g_nc_xm->mutex);
			return ret;
		}
	}
	for (i = 0; i < NC_XM_DEVICE_SIZE; i++)
	{
		if (g_nc_xm->dev[i].state == 0)
			break;
	}
	if (i == NC_XM_DEVICE_SIZE)
	{
		log_error("nc full");
		tool_thread_unlockMutex(&g_nc_xm->mutex);
		return NC_ERROR_FULL;
	}
	
	ret = nc_xm_dev_start(&g_nc_xm->dev[i], info);
	if (ret < 0)
	{
		log_error("nc_xm_dev_start id(%s), addr(%s:%d)", info->dev_id, info->ip, info->msg_port);
		tool_thread_unlockMutex(&g_nc_xm->mutex);
		return ret;
	}
	ret = nc_xm_dev_searchLogs(&g_nc_xm->dev[i], logs_v2);
	if (ret < 0)
	{
		log_error("nc_xm_dev_searchLogs");
		nc_xm_dev_stop(&g_nc_xm->dev[i]);
	}
	tool_thread_unlockMutex(&g_nc_xm->mutex);
	return ret;	
}

TOOL_INT32 nc_xm_startTalk_v2(nc_dev* info, NC_CB cb, void* param)
{
	if (g_nc_xm == NULL)
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
	tool_thread_lockMutex(&g_nc_xm->mutex);
	for (i = 0; i < NC_XM_DEVICE_SIZE; i++)
	{
		if (nc_xm_dev_isSame(info, &g_nc_xm->dev[i]))
		{
			ret = nc_xm_dev_startTalk(&g_nc_xm->dev[i], cb, param);
			tool_thread_unlockMutex(&g_nc_xm->mutex);
			return ret;
		}
	}
	for (i = 0; i < NC_XM_DEVICE_SIZE; i++)
	{
		if (g_nc_xm->dev[i].state == 0)
			break;
	}
	if (i == NC_XM_DEVICE_SIZE)
	{
		log_error("nc full");
		tool_thread_unlockMutex(&g_nc_xm->mutex);
		return NC_ERROR_FULL;
	}
	
	ret = nc_xm_dev_start(&g_nc_xm->dev[i], info);
	if (ret < 0)
	{
		log_error("nc_xm_dev_start id(%s), addr(%s:%d)", info->dev_id, info->ip, info->msg_port);
		tool_thread_unlockMutex(&g_nc_xm->mutex);
		return ret;
	}
	ret = nc_xm_dev_startTalk(&g_nc_xm->dev[i], cb, param);
	if (ret < 0)
	{
		log_error("nc_xm_dev_startTalk");
		nc_xm_dev_stop(&g_nc_xm->dev[i]);
	}
	tool_thread_unlockMutex(&g_nc_xm->mutex);
	return ret;

}

TOOL_INT32 nc_xm_stopTalk(TOOL_INT32 id)
{
	if (g_nc_xm == NULL)
	{
		log_error("nc");
		return NC_ERROR_UNINIT;
	}

	TOOL_INT32 d = 0;
	tool_thread_lockMutex(&g_nc_xm->mutex);
	for (d = 0; d < NC_XM_DEVICE_SIZE; d ++)
	{
		if (g_nc_xm->dev[d].talk.talk_no== id)
		{
			nc_xm_dev_stopTalk(&g_nc_xm->dev[d]);
			tool_thread_unlockMutex(&g_nc_xm->mutex);
			return 0;
		}
	}
	tool_thread_unlockMutex(&g_nc_xm->mutex);
	return 0;
}

TOOL_INT32 nc_xm_sendTalk(TOOL_INT32 id, TOOL_UINT8* buf, TOOL_INT32 len)
{
	if (g_nc_xm == NULL)
	{
		log_error("nc");
		return NC_ERROR_UNINIT;
	}

	TOOL_INT32 dev_id  = (id >> 8) & 0xFF;
	TOOL_INT32 talk_no = (id >> 2) & 0x3F;
	if (dev_id < 0 || dev_id >= NC_XM_DEVICE_SIZE || talk_no != NC_XM_CHANNEL_TALK || buf == NULL || 
		(len != 192 && len != 352))
	{
		log_error("id(0x%08x) buf(0x%x) len(%d)", id, buf, len);
		return NC_ERROR_INPUT;
	}
	if (g_nc_xm->dev[dev_id].talk.state && g_nc_xm->dev[dev_id].talk.send_stream)
		tool_stream2_set(g_nc_xm->dev[dev_id].talk.send_stream, buf, len);
	return 0;
}


TOOL_INT32 nc_xm_getAbility_v2(nc_dev* pst_nc_dev, ptc_cfg_ability_t* ability)
{
	if (g_nc_xm == NULL)
	{
		log_error("nc");
		return NC_ERROR_UNINIT;
	}
	if (pst_nc_dev == NULL || ability == NULL)
	{
		log_error("pst_nc_dev(0x%08x) ability(0x%08x)", pst_nc_dev, ability);
		return NC_ERROR_INPUT;
	}
	TOOL_INT32 ret = 0;
	nc_xm_dev_t dev;
	tool_mem_memset(&dev, sizeof(nc_xm_dev_t));
	tool_mem_memcpy(&dev.st_nc_dev, pst_nc_dev, sizeof(nc_dev));
	dev.state = 1;
	tool_sock_item_init(&dev.msg_sock);
	ret = nc_xm_dev_login(&dev);
	if (ret < 0)
	{
		dev.state = 0;
		return ret;
	}
	tool_mem_memset(ability, sizeof(ptc_cfg_ability_t));
	ability->video_input_num = dev.channel_num;
	ability->audio_input_num = dev.audio_in_num;
	ability->alarm_input_num = dev.alarm_in_num;
	ability->alarm_output_num = dev.alarm_out_num;
	ability->base.video_input_num = dev.channel_num;
	ability->base.audio_input_num = dev.audio_in_num;
	ability->base.alarm_input_num = dev.alarm_in_num;
	ability->base.alarm_output_num = dev.alarm_out_num;
	ability->base.device_type = PTC_DEVTYPE_YUSHI_BASE;
	ability->audio_ability.enable = 1;
	ability->audio_ability.sample_rate = PTC_AUDIO_RATE_8K;
	ability->audio_ability.bit_width = PTC_AUDIO_WIDTH_16BIT;
	ability->audio_ability.encode_type = PTC_AUDIO_ENCODE_G711A;
	ability->audio_ability.frame_size = 160;
	nc_xm_dev_logout(&dev);
	dev.state = 0;
	return 0;	
}

TOOL_INT32 nc_xm_syncTime(nc_dev* info, TOOL_UINT32 utc_sec, TOOL_INT32 zone, TOOL_INT32 dst_hour)
{
	if (g_nc_xm == NULL)
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
	tool_thread_lockMutex(&g_nc_xm->mutex);
	for (i = 0; i < NC_XM_DEVICE_SIZE; i++)
	{
		if (nc_xm_dev_isSame(info, &g_nc_xm->dev[i]))
		{
			ret = nc_xm_dev_syncTime(&g_nc_xm->dev[i], utc_sec, zone, dst_hour);
			tool_thread_unlockMutex(&g_nc_xm->mutex);
			return ret;
		}
	}
	for (i = 0; i < NC_XM_DEVICE_SIZE; i++)
	{
		if (g_nc_xm->dev[i].state == 0)
			break;
	}
	if (i == NC_XM_DEVICE_SIZE)
	{
		log_error("nc full");
		tool_thread_unlockMutex(&g_nc_xm->mutex);
		return NC_ERROR_FULL;
	}
	
	ret = nc_xm_dev_start(&g_nc_xm->dev[i], info);
	if (ret < 0)
	{
		log_error("nc_xm_dev_start id(%s), addr(%s:%d)", info->dev_id, info->ip, info->msg_port);
		tool_thread_unlockMutex(&g_nc_xm->mutex);
		return ret;
	}
	ret = nc_xm_dev_syncTime(&g_nc_xm->dev[i], utc_sec, zone, dst_hour);
	if (ret < 0)
	{
		log_error("nc_xm_dev_syncTime");
		nc_xm_dev_stop(&g_nc_xm->dev[i]);
	}
	tool_thread_unlockMutex(&g_nc_xm->mutex);
	return ret;

}

TOOL_INT32 nc_xm_getDeviceStatus_v2(nc_dev* info, ptc_cfg_status_t* status)
{
	if (g_nc_xm == NULL)
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
	tool_thread_lockMutex(&g_nc_xm->mutex);
	for (i = 0; i < NC_XM_DEVICE_SIZE; i++)
	{
		if (nc_xm_dev_isSame(info, &g_nc_xm->dev[i]))
		{
			ret = nc_xm_dev_getStatus(&g_nc_xm->dev[i], status);
			tool_thread_unlockMutex(&g_nc_xm->mutex);
			return ret;
		}
	}
	for (i = 0; i < NC_XM_DEVICE_SIZE; i++)
	{
		if (g_nc_xm->dev[i].state == 0)
			break;
	}
	if (i == NC_XM_DEVICE_SIZE)
	{
		log_error("nc full");
		tool_thread_unlockMutex(&g_nc_xm->mutex);
		return NC_ERROR_FULL;
	}
	
	ret = nc_xm_dev_start(&g_nc_xm->dev[i], info);
	if (ret < 0)
	{
		log_error("nc_xm_dev_start id(%s), addr(%s:%d)", info->dev_id, info->ip, info->msg_port);
		tool_thread_unlockMutex(&g_nc_xm->mutex);
		return ret;
	}
	ret = nc_xm_dev_getStatus(&g_nc_xm->dev[i], status);
	if (ret < 0)
	{
		log_error("nc_xm_dev_getStatus ret(%d)", ret);
		nc_xm_dev_stop(&g_nc_xm->dev[i]);
	}
	tool_thread_unlockMutex(&g_nc_xm->mutex);
	return ret;		
}

TOOL_INT32 nc_xm_setDeviceStatus_v2(nc_dev* info, ptc_cfg_status_t* status)
{
	if (g_nc_xm == NULL)
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
	tool_thread_lockMutex(&g_nc_xm->mutex);
	for (i = 0; i < NC_XM_DEVICE_SIZE; i++)
	{
		if (nc_xm_dev_isSame(info, &g_nc_xm->dev[i]))
		{
			ret = nc_xm_dev_setStatus(&g_nc_xm->dev[i], status);
			tool_thread_unlockMutex(&g_nc_xm->mutex);
			return ret;
		}
	}
	for (i = 0; i < NC_XM_DEVICE_SIZE; i++)
	{
		if (g_nc_xm->dev[i].state == 0)
			break;
	}
	if (i == NC_XM_DEVICE_SIZE)
	{
		log_error("nc full");
		tool_thread_unlockMutex(&g_nc_xm->mutex);
		return NC_ERROR_FULL;
	}
	
	ret = nc_xm_dev_start(&g_nc_xm->dev[i], info);
	if (ret < 0)
	{
		log_error("nc_xm_dev_start id(%s), addr(%s:%d)", info->dev_id, info->ip, info->msg_port);
		tool_thread_unlockMutex(&g_nc_xm->mutex);
		return ret;
	}
	ret = nc_xm_dev_setStatus(&g_nc_xm->dev[i], status);
	if (ret < 0)
	{
		log_error("nc_xm_dev_setStatus ret(%d)", ret);
		nc_xm_dev_stop(&g_nc_xm->dev[i]);
	}
	tool_thread_unlockMutex(&g_nc_xm->mutex);
	return ret;			
}


#if defined __cplusplus
}
#endif


