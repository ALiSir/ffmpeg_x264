
#if defined __cplusplus
extern "C"
{
#endif

#include "nc.h"
#include "qh_ptc.h"
#include "nc_ys.h"
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

#define NC_YS_USER_SIZE				10
#define	NC_YS_HEARTBEAT_CYCLE		5
#define	NC_YS_RECONNECT_CYCLE		5
#define NC_YS_CHANNEL_SIZE			64
#define NC_YS_STREAM_SIZE			2

#define NC_YS_RTS_POOL_SIZE			(4*1024*1024)
#define NC_YS_PBS_POOL_SIZE			(10*1024*1024)

	
typedef struct
{
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
	ptc_frame_video_t* recv_frame;
	ptc_frame_video_t* cb_frame;
//	TOOL_MUTEX mutex;		// for msg_sock
	TOOL_INT32 key_flag;	
	TOOL_INT32 error_flag;

	ys_msg_startrts_d2c_t startrts_d2c;
	TOOL_INT32 tick;
	TOOL_INT32 no_video;

	NC_CB cb;
	TOOL_VOID* param;	
}nc_ys_rts_t;

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
	ptc_frame_video_t* recv_frame;
	ptc_frame_video_t* cb_frame;
	TOOL_UINT8 left_buf[32*1024];
	TOOL_UINT32 left_len;
	TOOL_UINT32 full_flag;
//	TOOL_MUTEX mutex;		// for msg_sock
	TOOL_INT32 key_flag;
	TOOL_INT32 error_flag;
	TOOL_UINT32 history_pos;

	ys_msg_startpbs_d2c_t startpbs_d2c;
	TOOL_INT32 tick_sec;
	TOOL_INT32 reconnect_tick_sec;
	TOOL_UINT32 u32PlayTime;

	NC_CB cb;
	TOOL_VOID* param;	
}nc_ys_pbs_t;

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

	ys_msg_starttalk_d2c_t starttalk_d2c;
	tool_sock_item_t talk_sock;
}nc_ys_talk_t;

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
	ys_msg_t ys_msg;					//*
	ys_search_t* ys_search;

	//login
	TOOL_THREAD login_pid;
	ys_msg_login_c2d_t login_c2d;
	ys_msg_login_d2c_t login_d2c;
	TOOL_INT32 hb_tick_sec;	
	TOOL_INT32 hb_tick_usec;	
	TOOL_INT32 is_sleep;

	//rts
	nc_ys_rts_t rts[NC_YS_CHANNEL_SIZE][NC_YS_STREAM_SIZE];
	TOOL_INT32 cur_rts_enable[NC_YS_CHANNEL_SIZE][NC_YS_STREAM_SIZE];

	//pbs
	nc_ys_pbs_t pbs;
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
	nc_ys_talk_t talk;
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
}nc_ys_dev_t;

typedef struct
{
	nc_ys_dev_t dev[NC_YS_USER_SIZE];
	TOOL_MUTEX mutex;

	TOOL_INT32 state;
	TOOL_THREAD pid;

	TOOL_INT32 buffer_usec;
}nc_ys_t;

nc_ys_t* g_nc_ys = NULL;


TOOL_INT32 nc_ys_dev_recvMsg(nc_ys_dev_t* dev, TOOL_INT32 cmd)
{
	TOOL_INT32 ret = 0;
	while (1)
	{
		ret = tool_ptc_ys_recvMsg(&dev->msg_sock, &dev->ys_msg);
		if (ret < 0)
			break;
		/*
		if (dev->ys_msg.head.cmd == HY_CMD_ALARM)
		{
			log_state("HY_CMD_ALARM");
			nc_ys_dev_doAlarm(dev);
		}
		*/
		
		if (dev->ys_msg.head.u16Command == cmd || cmd == YS_IE_ID_BUTT)
			break;		
		log_state("invalid u16Command(0x%02x) cmd(0x%02x)", dev->ys_msg.head.u16Command, cmd);
	}
	return ret;
}

TOOL_INT32 nc_ys_dev_recvSearch(nc_ys_dev_t* dev, TOOL_INT32 cmd)
{
	TOOL_INT32 ret = 0;
	while (1)
	{
		ret = tool_ptc_ys_recvSearch(&dev->msg_sock, dev->ys_search);
		if (ret < 0)
			break;
		/*
		if (dev->ys_msg.head.cmd == HY_CMD_ALARM)
		{
			log_state("HY_CMD_ALARM");
			nc_ys_dev_doAlarm(dev);
		}
		*/
		
		if (dev->ys_search->head.u16Command == cmd || cmd == YS_IE_ID_BUTT)
			break;		
		log_state("invalid u16Command(0x%02x) cmd(0x%02x)", dev->ys_search->head.u16Command, cmd);
	}
	return ret;
}


#define __NC_YS_RTS__

TOOL_VOID nc_ys_rts_dostate(nc_ys_rts_t* rts, TOOL_INT32 type)
{
	ptc_frame_head_t head;
	tool_mem_memset(&head, sizeof(ptc_frame_head_t));
	head.frame_type = type;
//	log_debug("type(0x%08x)", type);
	tool_stream3_set_noDrop(&rts->stream, &head, sizeof(ptc_frame_head_t), NULL);	
}

TOOL_VOID* nc_ys_rts_runRecv(TOOL_VOID* param)
{
	nc_ys_rts_t* rts = (nc_ys_rts_t*)param;
	nc_ys_dev_t* dev = (nc_ys_dev_t*)rts->father_dev;
	nc_ys_t* nc = (nc_ys_t*)dev->father_nc;
	log_state("dev(%d) nc_ys_rts_runRecv(%d)(%d) start", dev->dev_id, rts->channel_no, rts->stream_type);
	TOOL_INT32 ret = 0;
	rts->key_flag = 0;
	TOOL_INT32 rts_online = 0;

//	TOOL_INT32 tick;

	while (rts->state)
	{
		if (rts_online != rts->online)
		{
			rts_online = rts->online;
			if (rts->online)
				nc_ys_rts_dostate(rts, rts_online);
		}

/*
	//	log_debug("tool_sock_setTick 10---------------------------");
		tool_sock_setTick(&tick);
		if (tick == 15 && rts->channel_no == 1)
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
			rts->error_flag = 1;
			break;
		}
		else if (ret == 0)
		{
			tool_sysf_usleep(20*1000);
			continue;
		}

		if (tool_ptc_rtp_recvTcp_video(&rts->rts_sock, rts->recv_frame) < 0)
		{
			log_error("tool_ptc_ys_recvRts error");
			rts->error_flag = 1;
			break;
		}

		if (rts->key_flag == 0)
		{
			if (rts->recv_frame->frame_head.frame_type != PTC_FRAME_I)
			{
				continue;
			}
			else
				rts->key_flag = 1;
		}		
//		log_debug("rts->recv_frame->head.len(%d)", rts->recv_frame->frame_head.len);
		tool_stream3_set_withDrop(&rts->stream, rts->recv_frame, sizeof(ptc_frame_head_t)+rts->recv_frame->frame_head.len, nc->buffer_usec);
	}

	log_state("dev(%d) nc_ys_rts_runRecv(%d)(%d) stop", dev->dev_id, rts->channel_no, rts->stream_type);
    return NULL;	
}

TOOL_VOID* nc_ys_rts_runPlay(TOOL_VOID* param)
{
	nc_ys_rts_t* rts = (nc_ys_rts_t*)param;
	nc_ys_dev_t* dev = (nc_ys_dev_t*)rts->father_dev;
	log_state("dev(%d) nc_ys_rts_runPlay(%d)(%d) start", dev->dev_id, rts->channel_no, rts->stream_type);
	nc_ys_t* nc = (nc_ys_t*)dev->father_nc;
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
			rts->cb(dev->st_nc_dev.ptc | (dev->dev_id<<8) | (rts->channel_no<<2) | rts->stream_type, rts->cb_frame, len, rts->param);
		}
	}

	log_state("dev(%d) nc_ys_rts_runPlay(%d)(%d) stop", dev->dev_id, rts->channel_no, rts->stream_type);
	return NULL;
}

TOOL_INT32 nc_ys_rts_start(nc_ys_rts_t* rts)
{
	log_debug("nc_ys_rts_start");
	nc_ys_dev_t* dev = (nc_ys_dev_t*)rts->father_dev;
	TOOL_INT32 ret = 0;
	TOOL_INT16 u16_port = tool_sock_getYsPort();
	do
	{	
		tool_mem_memset(&dev->ys_msg.head, sizeof(ys_msg_head_t));
		dev->ys_msg.head.u16Command = YS_MSG_ID_START_LIVE;
		dev->ys_msg.head.u32ContentLength = sizeof(ys_msg_startrts_c2d_t);
		ys_msg_startrts_c2d_t* startrts_c2d = (ys_msg_startrts_c2d_t*)dev->ys_msg.data;
		tool_mem_memset(startrts_c2d, sizeof(ys_msg_startrts_c2d_t));
		tool_ptc_ys_fillIeHead_startrts_c2d(startrts_c2d);
		tool_mem_memcpy(&startrts_c2d->user_log_info, &dev->login_d2c.user_log_info, sizeof(ys_ie_user_log_info_t));
		startrts_c2d->resource_code.stResourceCode.u8DevType = YS_DEV_TYPE_DIGITAL_ENCODER;
		startrts_c2d->resource_code.stResourceCode.u8ResType = YS_RES_TYPE_VIDEO_IN_CHL;
		startrts_c2d->resource_code.stResourceCode.u8MappingChlIndex = rts->channel_no+1;
		startrts_c2d->resource_code.stResourceCode.u8ChlID = rts->channel_no+1;
		startrts_c2d->media_ip.u8RecvSendFlag = 5;
		startrts_c2d->media_ip.u16Port = u16_port;
		startrts_c2d->media_ip.stIPAddress.u16DomainType = YS_AF_INET;
		startrts_c2d->media_transport.u32TransportProtocal = 1; 		//tcp
		startrts_c2d->video_index.u32StreamIndex = rts->stream_type+1;		
		if (tool_ptc_ys_sendMsg(&dev->msg_sock, &dev->ys_msg) < 0)
		{
			log_error("tool_ptc_ys_sendMsg");
			ret = -1;
			break;
		}
		if (nc_ys_dev_recvMsg(dev, YS_MSG_ID_START_LIVE) < 0)
		{
			log_error("nc_ys_recv[%d][%d]", rts->channel_no, rts->stream_type);
			ret = -1;
			break;
		}
		ys_msg_startrts_d2c_t* startrts_d2c = (ys_msg_startrts_d2c_t*)dev->ys_msg.data;
		log_state("u32Task_No(0x%08x) szSessionID(%s)", startrts_d2c->task_no.u32Task_No, startrts_d2c->session_id.szSessionID);
		tool_mem_memcpy(&rts->startrts_d2c, startrts_d2c, sizeof(ys_msg_startrts_d2c_t));

		
		rts->state = 1;
		if (tool_sock_item_open_v2(&rts->rts_sock, TOOL_SOCK_TYPE1_TCP, TOOL_SOCK_TYPE2_CLIENT, dev->st_nc_dev.ip, dev->st_nc_dev.stream_port, NULL, u16_port) < 0)
		{
			log_error("tool_sock_item_open RTS");
			ret = -1;
			break;
		}
		ys_msg_play_session_tcp_t play_session_tcp;
		tool_mem_memset(&play_session_tcp, sizeof(ys_msg_play_session_tcp_t));
		play_session_tcp.stHeader.ucTcpFlag = 0x24;
		play_session_tcp.stHeader.ucTcpchnl = 0x00; 
		play_session_tcp.stHeader.usBodySize = tool_sysf_htons(24); 
		play_session_tcp.stPlaySession.usMsgId = tool_sysf_htons(0xFFFD); 
		play_session_tcp.stPlaySession.usLen= tool_sysf_htons(0); 
		play_session_tcp.stPlaySession.usCmd = tool_sysf_htons(0x0001); 
		tool_mem_memcpy(play_session_tcp.stPlaySession.aucSession, rts->startrts_d2c.session_id.szSessionID, 16);
		if (tool_sock_item_send(&rts->rts_sock, &play_session_tcp, sizeof(ys_msg_play_session_tcp_t)) != sizeof(ys_msg_play_session_tcp_t))
		{
			log_error("tool_sock_item_send play_session_tcp");
			ret = -1;
			break;
		}
		tool_stream3_init(&rts->stream, NC_YS_RTS_POOL_SIZE);
		rts->recv_frame = (ptc_frame_video_t*)tool_mem_malloc(sizeof(ptc_frame_video_t), 0);
		rts->cb_frame	= (ptc_frame_video_t*)tool_mem_malloc(sizeof(ptc_frame_video_t), 0);
		tool_stream3_initReadPos(&rts->stream, &rts->read_pos);
		if (tool_thread_create(&rts->play_pid, NULL, nc_ys_rts_runPlay, (TOOL_VOID*)rts) < 0)
			log_fatalNo("tool_thread_create");
		if (tool_thread_create(&rts->recv_pid, NULL, nc_ys_rts_runRecv, (TOOL_VOID*)rts) < 0)
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

TOOL_INT32 nc_ys_rts_stop(nc_ys_rts_t* rts)
{
	if (rts->state)
	{
		rts->state = 0;
		tool_thread_join(rts->play_pid);
		tool_thread_join(rts->recv_pid);
		tool_mem_free(rts->recv_frame);
		rts->recv_frame = NULL;
		tool_mem_free(rts->cb_frame);
		rts->cb_frame = NULL;
		tool_stream3_done(&rts->stream);
	}
	rts->error_flag = 0;

	nc_ys_dev_t* dev = (nc_ys_dev_t*)rts->father_dev;
	tool_mem_memset(&dev->ys_msg.head, sizeof(ys_msg_head_t));
	dev->ys_msg.head.u16Command = YS_MSG_ID_STOP_LIVE;
	dev->ys_msg.head.u32ContentLength = sizeof(ys_msg_stoprts_c2d_t);
	ys_msg_stoprts_c2d_t* stoprts_c2d = (ys_msg_stoprts_c2d_t*)dev->ys_msg.data;
	tool_mem_memset(stoprts_c2d, sizeof(ys_msg_stoprts_c2d_t));
	tool_ptc_ys_fillIeHead_stoprts_c2d(stoprts_c2d);
	tool_mem_memcpy(&stoprts_c2d->user_log_info, &dev->login_d2c.user_log_info, sizeof(ys_ie_user_log_info_t));
	tool_mem_memcpy(&stoprts_c2d->task_no, &rts->startrts_d2c.task_no, sizeof(ys_ie_task_no_t));
	if (tool_ptc_ys_sendMsg(&dev->msg_sock, &dev->ys_msg) < 0)
	{
		log_error("tool_ptc_ys_sendMsg");
		return -1;
	}
	if (nc_ys_dev_recvMsg(dev, YS_MSG_ID_STOP_LIVE) < 0)
	{
		log_error("nc_ys_dev_recvMsg");
		return -1;
	}
	if (tool_sock_item_isLive(&rts->rts_sock))
		tool_sock_item_close(&rts->rts_sock);
	return 0;
}


#define __NC_YS_PBS__

TOOL_VOID nc_ys_pbs_doState(nc_ys_pbs_t* pbs, TOOL_INT32 type)
{
	ptc_frame_head_t head;
	tool_mem_memset(&head, sizeof(ptc_frame_head_t));
	head.frame_type = type;
//	log_debug("type(0x%08x)", type);
	tool_stream3_set_noDrop(&pbs->stream, &head, sizeof(ptc_frame_head_t), NULL);
}

TOOL_VOID* nc_ys_pbs_runPlay(TOOL_VOID* param)
{
	nc_ys_pbs_t* pbs = (nc_ys_pbs_t*)param;
	nc_ys_dev_t* dev = (nc_ys_dev_t*)pbs->father_dev;
	ys_ie_vod_file_t* pst_vod_file = (ys_ie_vod_file_t*)pbs->ptc_record.file_info;
//	log_state("dev(%d) file(%s) start_time(%s) end_time(%s) type(%d) start", 
//		dev->dev_id, pst_vod_file->szRecordFileName, 
//		tool_sysf_dateLocal_1(pst_vod_file->u32Begin), tool_sysf_dateLocal_2(pst_vod_file->u32End), 
//		pst_vod_file->u32RecordType);
	TOOL_UINT32 len = 0;
	pbs->history_pos = 0;

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
	log_state("dev(%d) file(%s) time(%d-%d) type(%d) stop", dev->dev_id,
		pst_vod_file->szRecordFileName, pst_vod_file->u32Begin, pst_vod_file->u32End, pst_vod_file->u32RecordType);
	return NULL;
}

TOOL_VOID* nc_ys_pbs_runRecv(TOOL_VOID* param)
{
	nc_ys_pbs_t* pbs = (nc_ys_pbs_t*)param;
	nc_ys_dev_t* dev = (nc_ys_dev_t*)pbs->father_dev;
	TOOL_INT32 ret = 0;
	pbs->key_flag = 0;
	TOOL_INT32 pbs_online = 0;

	log_state("dev(%d) nc_ys_pbs_runRecv start", dev->dev_id);

//	TOOL_INT32 tick;

	while (pbs->state)
	{	
		if (pbs_online != pbs->online)
		{
			pbs_online = pbs->online;
			if (pbs->online)
				nc_ys_pbs_doState(pbs, pbs_online);
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
			log_error("tool_sock_item_pollRead");
			pbs->error_flag = 1;
			pbs->key_flag = 0;
			break;
		}
		else if (ret == 0)
		{
			tool_sysf_usleep(20*1000);
			continue;
		}
		if (tool_ptc_rtp_recvTcp_video(&pbs->pbs_sock, pbs->recv_frame) < 0)
		{
			log_error("tool_ptc_rtp_recvTcp_video error");
			pbs->error_flag = 1;
			pbs->key_flag = 0;
			break;
		}
		pbs->recv_frame->frame_head.sec = pbs->u32PlayTime;
//		log_debug("frame_time(%s) len(%d)", tool_sysf_dateLocal_1(pbs->recv_frame->frame_head.sec), pbs->recv_frame->frame_head.len);
//		continue;

		if (pbs->recv_frame->frame_head.sec < dev->pbs.ptc_record.start_time)
		{
//			log_debug("frame_time(%s) start_time(%s)", tool_sysf_dateLocal_1(pbs->recv_frame->frame_head.sec), tool_sysf_dateLocal_2(pbs->recv_frame->frame_head.sec));
			continue;
		}
		if (pbs->recv_frame->frame_head.sec > dev->pbs.ptc_record.end_time)
		{
//			log_debug("frame_time(%s) end_time(%s)", tool_sysf_dateLocal_1(pbs->recv_frame->frame_head.sec), tool_sysf_dateLocal_2(pbs->recv_frame->frame_head.sec));
		//	continue;
		}

//		log_debug("pbs->recv_frame->frame_head.frame_type(%d)", pbs->recv_frame->frame_head.frame_type);
		if (dev->pbs.key_flag == 0)
		{
			if (pbs->recv_frame->frame_head.frame_type != PTC_FRAME_I)
			{
				continue;
			}
			else
				dev->pbs.key_flag = 1;
		}
		tool_stream3_set_noDrop(&dev->pbs.stream, pbs->recv_frame, sizeof(ptc_frame_head_t)+pbs->recv_frame->frame_head.len, &dev->cur_pbs_enable);
	}

	log_state("dev(%d) nc_ys_pbs_runRecv stop", dev->dev_id);
    return NULL;	   
}

TOOL_INT32 nc_ys_pbs_start(nc_ys_pbs_t* pbs, ptc_cfg_record_t* pst_cfg_record)
{
	nc_ys_dev_t* dev = (nc_ys_dev_t*)pbs->father_dev;	
	TOOL_INT32 ret = 0;
	TOOL_INT16 u16_port = tool_sock_getYsPort();
	do
	{
		//start pbs
		tool_mem_memset(&dev->ys_msg.head, sizeof(ys_msg_head_t));
		dev->ys_msg.head.u16Command = YS_MSG_ID_VOD_OPEN;
		dev->ys_msg.head.u32ContentLength = sizeof(ys_msg_startpbs_c2d_t);
		ys_msg_startpbs_c2d_t* pst_startpbs_c2d = (ys_msg_startpbs_c2d_t*)dev->ys_msg.data;
		tool_mem_memset(pst_startpbs_c2d, sizeof(ys_msg_startpbs_c2d_t));
		tool_ptc_ys_fillIeHead_startpbs_c2d(pst_startpbs_c2d);
		tool_mem_memcpy(&pst_startpbs_c2d->user_log_info, &dev->login_d2c.user_log_info, sizeof(ys_ie_user_log_info_t));
		pst_startpbs_c2d->resource_code.stResourceCode.u8DevType = YS_DEV_TYPE_DIGITAL_ENCODER;
		pst_startpbs_c2d->resource_code.stResourceCode.u8ResType = YS_RES_TYPE_VIDEO_IN_CHL;
		pst_startpbs_c2d->resource_code.stResourceCode.u8MappingChlIndex = pst_cfg_record->channel+1;
		pst_startpbs_c2d->resource_code.stResourceCode.u8ChlID = pst_cfg_record->channel+1;
		pst_startpbs_c2d->media_ip.u8RecvSendFlag = 5;
		pst_startpbs_c2d->media_ip.u16Port = u16_port;
		pst_startpbs_c2d->media_ip.stIPAddress.u16DomainType = YS_AF_INET;
		tool_mem_memcpy(&pst_startpbs_c2d->vod_file, pst_cfg_record->file_info, sizeof(ys_ie_vod_file_t));
		if (tool_ptc_ys_sendMsg(&dev->msg_sock, &dev->ys_msg) < 0)
		{
			log_error("tool_ptc_ys_sendMsg YS_MSG_ID_VOD_OPEN");
			ret = -1;
			break;
		}
		if (nc_ys_dev_recvMsg(dev, YS_MSG_ID_VOD_OPEN) < 0)
		{
			log_error("nc_ys_dev_recvMsg YS_MSG_ID_VOD_OPEN");
			ret = -1;
			break;
		}
		ys_msg_startpbs_d2c_t* pst_startpbs_d2c = (ys_msg_startpbs_d2c_t*)dev->ys_msg.data;
		log_state("u32Task_No(0x%08x) szSessionID(%s)", pst_startpbs_d2c->task_no.u32Task_No, pst_startpbs_d2c->session_id.szSessionID);
		tool_mem_memcpy(&pbs->startpbs_d2c, pst_startpbs_d2c, sizeof(ys_msg_startpbs_d2c_t));

		//connect
		pbs->state = 1;		
		if (tool_sock_item_open_v2(&pbs->pbs_sock, TOOL_SOCK_TYPE1_TCP, TOOL_SOCK_TYPE2_CLIENT, dev->st_nc_dev.ip, dev->st_nc_dev.stream_port, NULL, u16_port) < 0)
		{
			log_error("tool_sock_item_open PBS");
			ret = -1;
			break;
		}
		ys_msg_play_session_tcp_t play_session_tcp;
		tool_mem_memset(&play_session_tcp, sizeof(ys_msg_play_session_tcp_t));
		play_session_tcp.stHeader.ucTcpFlag = 0x24;
		play_session_tcp.stHeader.ucTcpchnl = 0x00; 
		play_session_tcp.stHeader.usBodySize = tool_sysf_htons(24); 
		play_session_tcp.stPlaySession.usMsgId = tool_sysf_htons(0xFFFD); 
		play_session_tcp.stPlaySession.usLen= tool_sysf_htons(0); 
		play_session_tcp.stPlaySession.usCmd = tool_sysf_htons(0x0001); 
		tool_mem_memcpy(play_session_tcp.stPlaySession.aucSession, pbs->startpbs_d2c.session_id.szSessionID, 16);
		if (tool_sock_item_send(&pbs->pbs_sock, &play_session_tcp, sizeof(ys_msg_play_session_tcp_t)) != sizeof(ys_msg_play_session_tcp_t))
		{
			log_error("tool_sock_item_send play_session_tcp");
			ret = -1;
			break;
		}

		//set pbs
		tool_mem_memset(&dev->ys_msg.head, sizeof(ys_msg_head_t));
		dev->ys_msg.head.u16Command = YS_MSG_ID_VOD_PLAY;
		dev->ys_msg.head.u32ContentLength = sizeof(ys_msg_setpbs_c2d_t);
		ys_msg_setpbs_c2d_t* pst_setpbs_c2d = (ys_msg_setpbs_c2d_t*)dev->ys_msg.data;
		tool_mem_memset(pst_setpbs_c2d, sizeof(ys_msg_setpbs_c2d_t));
		tool_ptc_ys_fillIeHead_setpbs_c2d(pst_setpbs_c2d);
		tool_mem_memcpy(&pst_setpbs_c2d->user_log_info, &dev->login_d2c.user_log_info, sizeof(ys_ie_user_log_info_t));
		tool_mem_memcpy(&pst_setpbs_c2d->task_no, &pbs->startpbs_d2c.task_no, sizeof(ys_ie_task_no_t));
		if (dev->pbs.history_pos > pst_cfg_record->start_time && dev->pbs.history_pos < pst_cfg_record->end_time)
			pst_setpbs_c2d->play_status.u32PlayTime = pbs->history_pos;
		else
			pst_setpbs_c2d->play_status.u32PlayTime = pst_cfg_record->start_time;
		pst_setpbs_c2d->play_status.u32PlayStatus = YS_PBS_STATUS_1_FORWARD;
		if (tool_ptc_ys_sendMsg(&dev->msg_sock, &dev->ys_msg) < 0)
		{
			log_error("tool_ptc_ys_sendMsg YS_MSG_ID_VOD_PLAY");
			ret = -1;
			break;
		}
		if (nc_ys_dev_recvMsg(dev, YS_MSG_ID_VOD_PLAY) < 0)
		{
			log_error("nc_ys_dev_recvMsg YS_MSG_ID_VOD_PLAY");
			ret = -1;
			break;
		}
		tool_stream3_init(&pbs->stream, NC_YS_PBS_POOL_SIZE);
		pbs->recv_frame = (ptc_frame_video_t*)tool_mem_malloc(PTC_VIDEO_FRAME_SIZE, 0);
		pbs->cb_frame	= (ptc_frame_video_t*)tool_mem_malloc(PTC_VIDEO_FRAME_SIZE, 0);
		tool_stream3_initReadPos(&pbs->stream, &pbs->read_pos);
		if (tool_thread_create(&pbs->play_pid, NULL, nc_ys_pbs_runPlay, (TOOL_VOID*)pbs) < 0)
			log_fatalNo("tool_thread_create");
		if (tool_thread_create(&pbs->recv_pid, NULL, nc_ys_pbs_runRecv, (TOOL_VOID*)pbs) < 0)
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

TOOL_INT32 nc_ys_pbs_get(nc_ys_pbs_t* pbs)
{
	if (tool_sock_isTickTimeout(pbs->tick_sec, 1) == 0)
		return 0;
	tool_sock_setTick(&pbs->tick_sec);
	
	nc_ys_dev_t* dev = (nc_ys_dev_t*)pbs->father_dev;	
	tool_mem_memset(&dev->ys_msg.head, sizeof(ys_msg_head_t));
	dev->ys_msg.head.u16Command = YS_MSG_ID_VOD_GET_PLAYSTATUS;
	dev->ys_msg.head.u32ContentLength = sizeof(ys_msg_getpbs_c2d_t);
	ys_msg_getpbs_c2d_t* pst_getpbs_c2d = (ys_msg_getpbs_c2d_t*)dev->ys_msg.data;
	tool_mem_memset(pst_getpbs_c2d, sizeof(ys_msg_getpbs_c2d_t));
	tool_ptc_ys_fillIeHead_getpbs_c2d(pst_getpbs_c2d);
	tool_mem_memcpy(&pst_getpbs_c2d->user_log_info, &dev->login_d2c.user_log_info, sizeof(ys_ie_user_log_info_t));
	tool_mem_memcpy(&pst_getpbs_c2d->task_no, &pbs->startpbs_d2c.task_no, sizeof(ys_ie_task_no_t));
	if (tool_ptc_ys_sendMsg(&dev->msg_sock, &dev->ys_msg) < 0)
	{
		log_error("tool_ptc_ys_sendMsg YS_MSG_ID_VOD_GET_PLAYSTATUS");
		return -1;
	}
	if (nc_ys_dev_recvMsg(dev, YS_MSG_ID_VOD_GET_PLAYSTATUS) < 0)
	{
		log_error("nc_ys_dev_recvMsg YS_MSG_ID_VOD_GET_PLAYSTATUS");
		return -1;
	}
	ys_msg_getpbs_d2c_t* pst_getpbs_d2c = (ys_msg_getpbs_d2c_t*)dev->ys_msg.data;
	log_state("u32Task_No(0x%08x) u32PlayTime(%d) u32PlayStatus(%d)", pst_getpbs_d2c->task_no.u32Task_No, 
		pst_getpbs_d2c->play_status.u32PlayTime, pst_getpbs_d2c->play_status.u32PlayStatus);
	pbs->u32PlayTime = pst_getpbs_d2c->play_status.u32PlayTime;
	return 0;	
}

TOOL_INT32 nc_ys_pbs_pause(nc_ys_pbs_t* pbs)
{
	nc_ys_dev_t* dev = (nc_ys_dev_t*)pbs->father_dev;	
	tool_mem_memset(&dev->ys_msg.head, sizeof(ys_msg_head_t));
	dev->ys_msg.head.u16Command = YS_MSG_ID_VOD_PLAY;
	dev->ys_msg.head.u32ContentLength = sizeof(ys_msg_setpbs_c2d_t);
	ys_msg_setpbs_c2d_t* pst_setpbs_c2d = (ys_msg_setpbs_c2d_t*)dev->ys_msg.data;
	tool_mem_memset(pst_setpbs_c2d, sizeof(ys_msg_setpbs_c2d_t));
	tool_ptc_ys_fillIeHead_setpbs_c2d(pst_setpbs_c2d);
	tool_mem_memcpy(&pst_setpbs_c2d->user_log_info, &dev->login_d2c.user_log_info, sizeof(ys_ie_user_log_info_t));
	tool_mem_memcpy(&pst_setpbs_c2d->task_no, &pbs->startpbs_d2c.task_no, sizeof(ys_ie_task_no_t));
	pst_setpbs_c2d->play_status.u32PlayTime = pbs->u32PlayTime;
	pst_setpbs_c2d->play_status.u32PlayStatus = YS_PBS_STATUS_PAUSE;
	if (tool_ptc_ys_sendMsg(&dev->msg_sock, &dev->ys_msg) < 0)
	{
		log_error("tool_ptc_ys_sendMsg YS_MSG_ID_VOD_PLAY YS_PBS_STATUS_PAUSE");
		return -1;
	}
	if (nc_ys_dev_recvMsg(dev, YS_MSG_ID_VOD_PLAY) < 0)
	{
		log_error("nc_ys_dev_recvMsg YS_MSG_ID_VOD_PLAY YS_PBS_STATUS_PAUSE");
		return -1;
	}
	log_state("nc_ys_pbs_pause (%d)", pbs->u32PlayTime);
	return 0;	
}

TOOL_INT32 nc_ys_pbs_resume(nc_ys_pbs_t* pbs)
{
	nc_ys_dev_t* dev = (nc_ys_dev_t*)pbs->father_dev;	
	tool_mem_memset(&dev->ys_msg.head, sizeof(ys_msg_head_t));
	dev->ys_msg.head.u16Command = YS_MSG_ID_VOD_PLAY;
	dev->ys_msg.head.u32ContentLength = sizeof(ys_msg_setpbs_c2d_t);
	ys_msg_setpbs_c2d_t* pst_setpbs_c2d = (ys_msg_setpbs_c2d_t*)dev->ys_msg.data;
	tool_mem_memset(pst_setpbs_c2d, sizeof(ys_msg_setpbs_c2d_t));
	tool_ptc_ys_fillIeHead_setpbs_c2d(pst_setpbs_c2d);
	tool_mem_memcpy(&pst_setpbs_c2d->user_log_info, &dev->login_d2c.user_log_info, sizeof(ys_ie_user_log_info_t));
	tool_mem_memcpy(&pst_setpbs_c2d->task_no, &pbs->startpbs_d2c.task_no, sizeof(ys_ie_task_no_t));
	pst_setpbs_c2d->play_status.u32PlayTime = pbs->u32PlayTime;
	pst_setpbs_c2d->play_status.u32PlayStatus = YS_PBS_STATUS_RESUME;
	if (tool_ptc_ys_sendMsg(&dev->msg_sock, &dev->ys_msg) < 0)
	{
		log_error("tool_ptc_ys_sendMsg YS_MSG_ID_VOD_PLAY YS_PBS_STATUS_RESUME");
		return -1;
	}
	if (nc_ys_dev_recvMsg(dev, YS_MSG_ID_VOD_PLAY) < 0)
	{
		log_error("nc_ys_dev_recvMsg YS_MSG_ID_VOD_PLAY YS_PBS_STATUS_RESUME");
		return -1;
	}
	log_state("nc_ys_pbs_resume (%d)", pbs->u32PlayTime);
	return 0;	
}


TOOL_INT32 nc_ys_pbs_stop(nc_ys_pbs_t* pbs)
{
	if (pbs->state)
	{
		pbs->state = 0; 
		tool_thread_join(pbs->play_pid);
		tool_thread_join(pbs->recv_pid);
		tool_mem_free(pbs->recv_frame);
		pbs->recv_frame = NULL;
		tool_mem_free(pbs->cb_frame);
		pbs->cb_frame = NULL;
		tool_stream3_done(&pbs->stream);
	}
	pbs->error_flag = 0;
	
	nc_ys_dev_t* dev = (nc_ys_dev_t*)pbs->father_dev;
	tool_mem_memset(&dev->ys_msg.head, sizeof(ys_msg_head_t));
	dev->ys_msg.head.u16Command = YS_MSG_ID_VOD_CLOSE;
	dev->ys_msg.head.u32ContentLength = sizeof(ys_msg_stoppbs_c2d_t);
	ys_msg_stoppbs_c2d_t* stoppbs_c2d = (ys_msg_stoppbs_c2d_t*)dev->ys_msg.data;
	tool_mem_memset(stoppbs_c2d, sizeof(ys_msg_stoppbs_c2d_t));
	tool_ptc_ys_fillIeHead_stoppbs_c2d(stoppbs_c2d);
	tool_mem_memcpy(&stoppbs_c2d->user_log_info, &dev->login_d2c.user_log_info, sizeof(ys_ie_user_log_info_t));
	tool_mem_memcpy(&stoppbs_c2d->task_no, &pbs->startpbs_d2c.task_no, sizeof(ys_ie_task_no_t));
	if (tool_ptc_ys_sendMsg(&dev->msg_sock, &dev->ys_msg) < 0)
	{
		log_error("tool_ptc_ys_sendMsg");
		return -1;
	}
	if (nc_ys_dev_recvMsg(dev, YS_MSG_ID_VOD_CLOSE) < 0)
	{
		log_error("nc_ys_dev_recvMsg");
		return -1;
	}
	if (tool_sock_item_isLive(&pbs->pbs_sock))
		tool_sock_item_close(&pbs->pbs_sock);
	return 0;
}

#define __NC_YS_TALK__

TOOL_VOID* nc_ys_talk_runRecv(TOOL_VOID* param)
{
	nc_ys_talk_t* talk = (nc_ys_talk_t*)param;
	nc_ys_dev_t* dev = (nc_ys_dev_t*)talk->father_dev;
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
					talk->cb(dev->st_nc_dev.ptc | (dev->dev_id<<8) | talk->talk_no, &head, sizeof(ptc_frame_head_t), talk->param);
				}
			}
		}

/*
		tool_sock_setTick(&tick);
		if (tick == 20)
		{
			log_debug("tool_sock_setTick 10---------------------------");
			talk->error_flag = 1;
			continue;
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
		if (tool_ptc_rtp_recvTcp_talk(&talk->talk_sock, &talk->recv_frame) < 0)
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
			talk->cb(dev->st_nc_dev.ptc | (dev->dev_id<<8) | talk->talk_no, &talk->recv_frame, sizeof(ptc_frame_head_t)+talk->recv_frame.frame_head.len, talk->param);
		}
		tool_thread_unlockMutex(&talk->mutex);
	}

	log_state("dev(%d) Recv talk stop", dev->dev_id);
    return NULL;	   
}

TOOL_VOID* nc_ys_talk_runSend(TOOL_VOID* param)
{
	nc_ys_talk_t* talk = (nc_ys_talk_t*)param;
	nc_ys_dev_t* dev = (nc_ys_dev_t*)talk->father_dev;

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

		if (tool_ptc_rtp_sendTcp_talk(&talk->talk_sock, &talk->send_frame) < 0)
		{
			log_error("tool_ptc_rtp_sendTcp_talk");
			talk->error_flag = 1;
			break;
		}
	}

	log_state("dev(%d) send talk stop", dev->dev_id);
    return NULL;	   
}

TOOL_INT32 nc_ys_talk_start(nc_ys_talk_t* talk)
{
	nc_ys_dev_t* dev = (nc_ys_dev_t*)talk->father_dev;	
	TOOL_INT32 ret = 0;
	TOOL_INT16 u16_port = tool_sock_getYsPort();
	do
	{
		//start talk
		tool_mem_memset(&dev->ys_msg.head, sizeof(ys_msg_head_t));
		dev->ys_msg.head.u16Command = YS_MSG_ID_START_TALK;
		dev->ys_msg.head.u32ContentLength = sizeof(ys_msg_starttalk_c2d_t);
		ys_msg_starttalk_c2d_t* pst_starttalk_c2d = (ys_msg_starttalk_c2d_t*)dev->ys_msg.data;
		tool_mem_memset(pst_starttalk_c2d, sizeof(ys_msg_starttalk_c2d_t));
		tool_ptc_ys_fillIeHead_starttalk_c2d(pst_starttalk_c2d);
		tool_mem_memcpy(&pst_starttalk_c2d->user_log_info, &dev->login_d2c.user_log_info, sizeof(ys_ie_user_log_info_t));
		pst_starttalk_c2d->resource_code_in.stResourceCode.u8DevType = YS_DEV_TYPE_ANALOG_ENCODER;
		pst_starttalk_c2d->resource_code_in.stResourceCode.u8ResType = YS_RES_TYPE_INDEPENDENT_AUDIO_INPUT_CHL;
		pst_starttalk_c2d->resource_code_in.stResourceCode.u8ChlID = 1;
		pst_starttalk_c2d->resource_code_out.stResourceCode.u8DevType = YS_DEV_TYPE_ANALOG_ENCODER;
		pst_starttalk_c2d->resource_code_out.stResourceCode.u8ResType = YS_RES_TYPE_INDEPENDENT_AUDIO_OUTPUT_CHL;
		pst_starttalk_c2d->resource_code_out.stResourceCode.u8ChlID = 1;
		pst_starttalk_c2d->media_ip_send_remote.u8RecvSendFlag = 5;
		pst_starttalk_c2d->media_ip_send_remote.u16Port = u16_port;
		pst_starttalk_c2d->media_ip_send_remote.stIPAddress.u16DomainType = YS_AF_INET;
		pst_starttalk_c2d->media_ip_recv_remote.u8RecvSendFlag = 5;
		pst_starttalk_c2d->media_ip_recv_remote.u16Port = u16_port;
		pst_starttalk_c2d->media_ip_recv_remote.stIPAddress.u16DomainType = YS_AF_INET;
		pst_starttalk_c2d->media_transport.u32TransportProtocal = 1;
		if (tool_ptc_ys_sendMsg(&dev->msg_sock, &dev->ys_msg) < 0)
		{
			log_error("tool_ptc_ys_sendMsg YS_MSG_ID_START_TALK");
			ret = -1;
			break;
		}
		if (nc_ys_dev_recvMsg(dev, YS_MSG_ID_START_TALK) < 0)
		{
			log_error("nc_ys_dev_recvMsg YS_MSG_ID_START_TALK");
			ret = -1;
			break;
		}
		ys_msg_starttalk_d2c_t* pst_starttalk_d2c = (ys_msg_starttalk_d2c_t*)dev->ys_msg.data;
		log_state("u32Task_No(0x%08x) szSessionID(%s)", pst_starttalk_d2c->task_no.u32Task_No, pst_starttalk_d2c->session_id.szSessionID);
		tool_mem_memcpy(&talk->starttalk_d2c, pst_starttalk_d2c, sizeof(ys_msg_starttalk_d2c_t));

		//connect
		talk->state = 1;		
		if (tool_sock_item_open_v2(&talk->talk_sock, TOOL_SOCK_TYPE1_TCP, TOOL_SOCK_TYPE2_CLIENT, dev->st_nc_dev.ip, dev->st_nc_dev.stream_port, NULL, u16_port) < 0)
		{
			log_error("tool_sock_item_open talk");
			ret = -1;
			break;
		}
		ys_msg_play_session_tcp_t play_session_tcp;
		tool_mem_memset(&play_session_tcp, sizeof(ys_msg_play_session_tcp_t));
		play_session_tcp.stHeader.ucTcpFlag = 0x24;
		play_session_tcp.stHeader.ucTcpchnl = 0x00; 
		play_session_tcp.stHeader.usBodySize = tool_sysf_htons(24); 
		play_session_tcp.stPlaySession.usMsgId = tool_sysf_htons(0xFFFD); 
		play_session_tcp.stPlaySession.usLen= tool_sysf_htons(0); 
		play_session_tcp.stPlaySession.usCmd = tool_sysf_htons(0x0001); 
		tool_mem_memcpy(play_session_tcp.stPlaySession.aucSession, talk->starttalk_d2c.session_id.szSessionID, 16);
		if (tool_sock_item_send(&talk->talk_sock, &play_session_tcp, sizeof(ys_msg_play_session_tcp_t)) != sizeof(ys_msg_play_session_tcp_t))
		{
			log_error("tool_sock_item_send play_session_tcp");
			ret = -1;
			break;
		}
		talk->send_stream = (tool_stream2_t*)tool_mem_malloc(sizeof(tool_stream2_t), 0);
		tool_stream2_init(talk->send_stream);
		tool_stream2_initReadPos(talk->send_stream, &talk->send_pos);
		if (tool_thread_create(&talk->recv_pid, NULL, nc_ys_talk_runRecv, (TOOL_VOID*)talk) < 0)
			log_fatalNo("tool_thread_create");
		if (tool_thread_create(&talk->send_pid, NULL, nc_ys_talk_runSend, (TOOL_VOID*)talk) < 0)
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

TOOL_INT32 nc_ys_talk_stop(nc_ys_talk_t* talk)
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
	
	nc_ys_dev_t* dev = (nc_ys_dev_t*)talk->father_dev;
	tool_mem_memset(&dev->ys_msg.head, sizeof(ys_msg_head_t));
	dev->ys_msg.head.u16Command = YS_MSG_ID_STOP_TALK;
	dev->ys_msg.head.u32ContentLength = sizeof(ys_msg_stoptalk_c2d_t);
	ys_msg_stoptalk_c2d_t* stoptalk_c2d = (ys_msg_stoptalk_c2d_t*)dev->ys_msg.data;
	tool_mem_memset(stoptalk_c2d, sizeof(ys_msg_stoptalk_c2d_t));
	tool_ptc_ys_fillIeHead_stoptalk_c2d(stoptalk_c2d);
	tool_mem_memcpy(&stoptalk_c2d->user_log_info, &dev->login_d2c.user_log_info, sizeof(ys_ie_user_log_info_t));
	tool_mem_memcpy(&stoptalk_c2d->task_no, &talk->starttalk_d2c.task_no, sizeof(ys_ie_task_no_t));
	if (tool_ptc_ys_sendMsg(&dev->msg_sock, &dev->ys_msg) < 0)
	{
		log_error("tool_ptc_ys_sendMsg");
		return -1;
	}
	if (nc_ys_dev_recvMsg(dev, YS_MSG_ID_STOP_TALK) < 0)
	{
		log_error("nc_ys_dev_recvMsg");
		return -1;
	}
	if (tool_sock_item_isLive(&talk->talk_sock))
		tool_sock_item_close(&talk->talk_sock);
	return 0;
}


#define __NC_YS_DEV__

TOOL_VOID nc_ys_dev_init(nc_ys_dev_t* dev, TOOL_INT32 dev_id, TOOL_VOID* father_nc)
{
	dev->dev_id = dev_id;
	dev->father_nc = father_nc;
	tool_thread_initMutex(&dev->mutex);
	tool_sock_item_setFlag(&dev->msg_sock, &dev->state);
	//rts
	TOOL_INT32 i = 0, j = 0;
	for (i = 0; i < NC_YS_CHANNEL_SIZE; i++)
	{
		for (j = 0; j < NC_YS_STREAM_SIZE; j++)
		{
			dev->rts[i][j].channel_no = i;
			dev->rts[i][j].stream_type = j;
			dev->rts[i][j].father_dev = dev;
			dev->rts[i][j].tick = -12345678;
			tool_sock_item_setFlag(&dev->rts[i][j].rts_sock, &dev->rts[i][j].state);
		}
	}
	//pbs
	dev->pbs.father_dev = dev;
	dev->pbs.pbs_no = NC_YS_CHANNEL_SIZE;
	dev->pbs.reconnect_tick_sec = -12345678;
	tool_sock_item_setFlag(&dev->pbs.pbs_sock, &dev->pbs.state);
	//talk
	dev->talk.father_dev = dev;
	dev->talk.talk_no = NC_YS_CHANNEL_SIZE*2;
	tool_thread_initMutex(&dev->talk.mutex);
	dev->talk.tick = -12345678;
	tool_sock_item_setFlag(&dev->talk.talk_sock, &dev->talk.state);
	//cfg
	tool_thread_initCond(&dev->cond_records);
	tool_thread_initCond(&dev->cond_logs);
	tool_thread_initCond(&dev->cond_status);	
}

TOOL_VOID nc_ys_dev_done(nc_ys_dev_t* dev)
{
	tool_thread_doneMutex(&dev->mutex);
	TOOL_INT32 i = 0, j = 0;
	for (i = 0; i < NC_YS_CHANNEL_SIZE; i++)
	{
		for (j = 0; j < NC_YS_STREAM_SIZE; j++)
		{
		}
	}

	//pbs

	//talk
	tool_thread_doneMutex(&dev->talk.mutex);

	//cfg
	tool_thread_doneCond(&dev->cond_records);
	tool_thread_doneCond(&dev->cond_logs);
	tool_thread_doneCond(&dev->cond_status);
}

TOOL_VOID nc_ys_dev_setOnline(nc_ys_dev_t* dev, TOOL_INT32 online)
{
	dev->online = online;
	TOOL_INT32 i = 0;
	TOOL_INT32 j = 0;
	for (i = 0; i < NC_YS_CHANNEL_SIZE; i++)
	{
		for (j = 0; j < NC_YS_STREAM_SIZE; j++)
		{
			dev->rts[i][j].online = online;
		}
	}
	dev->pbs.online = online;
	dev->talk.online = online;
}
	

TOOL_INT32 nc_ys_dev_login(nc_ys_dev_t* dev)
{
	if (tool_sock_item_isLive(&dev->msg_sock))
		return 0;
	if (tool_sock_item_isTimeout(&dev->msg_sock, NC_YS_RECONNECT_CYCLE) == 0)
	{
		tool_sysf_usleep(20*1000);
		return -1;
	}

	TOOL_INT32 ret = 0;
	ret = nc_ys_getDevInfo(&dev->st_nc_dev);
	if (ret < 0)
		return ret;
	
	do
	{
		nc_ys_dev_setOnline(dev, PTC_FRAME_STREAM_CONNECT);
		if (tool_sock_item_open_v2(&dev->msg_sock, TOOL_SOCK_TYPE1_TCP, TOOL_SOCK_TYPE2_CLIENT, dev->st_nc_dev.ip, dev->st_nc_dev.msg_port, NULL, 0) < 0)
		{
			log_error("nc_ys_dev_login tool_sock_item_open");
			ret = NC_ERROR_CONNECT;
			nc_ys_dev_setOnline(dev, PTC_FRAME_STREAM_CONNECT_FAIL);
			break;
		}
		tool_mem_memset(&dev->ys_msg.head, sizeof(ys_msg_head_t));
		dev->ys_msg.head.u16Command = YS_MSG_ID_USER_LOGIN;
		dev->ys_msg.head.u32ContentLength = sizeof(ys_msg_login_c2d_t);
		ys_msg_login_c2d_t* login_c2d = (ys_msg_login_c2d_t*)dev->ys_msg.data;
		tool_mem_memset(login_c2d, sizeof(ys_msg_login_c2d_t));
		tool_ptc_ys_fillIeHead_login_c2d(login_c2d);
		tool_str_strncpy(login_c2d->user_simple.szUserName, dev->st_nc_dev.user, YS_USER_NAME_LEN-1);
		TOOL_UINT8 result[16] = {0};
		tool_md5_calc((TOOL_UINT8*)dev->st_nc_dev.pswd, tool_str_strlen(dev->st_nc_dev.pswd), result);
		tool_str_byte2str(result, 16, login_c2d->user_simple.szUserPasswd);
		login_c2d->ip_address.stIPAddress.u16DomainType = YS_AF_INET;
		tool_sock_item_getSourceAddr(&dev->msg_sock, &login_c2d->ip_address.stIPAddress.be32Ipv4Addr, NULL);
		login_c2d->ip_address.u16Port = 0;
		login_c2d->user_log_type.u8LoginType = YS_USER_LOGIN_TYPE_THIRD;
		nc_ys_dev_setOnline(dev, PTC_FRAME_STREAM_LOGIN);
		if (tool_ptc_ys_sendMsg(&dev->msg_sock, &dev->ys_msg) < 0)
		{
			ret = NC_ERROR_SEND;
			nc_ys_dev_setOnline(dev, PTC_FRAME_STREAM_LOGIN_FAIL);
			break;
		}
		tool_mem_memcpy(&dev->login_c2d, login_c2d, sizeof(ys_msg_login_c2d_t));
		if (nc_ys_dev_recvMsg(dev, YS_MSG_ID_USER_LOGIN) < 0)
		{
			
			if (dev->ys_msg.head.u16Status == YS_ERR_USER_WRONG_PASSWD)
			{
				ret = NC_ERROR_AUTH;
				nc_ys_dev_setOnline(dev, PTC_FRAME_STREAM_AUTH_FAIL);
			}
			else
			{
				ret = NC_ERROR_RECV;
				nc_ys_dev_setOnline(dev, PTC_FRAME_STREAM_LOGIN_FAIL);
			}
			break;
		}
		ys_msg_login_d2c_t* login_d2c = (ys_msg_login_d2c_t*)dev->ys_msg.data;
		if (login_d2c->dev_ability.stDevInfo.u8IPChanNum > NC_YS_CHANNEL_SIZE)
		{
			log_debug("login_d2c->dev_ability.stDevInfo.u8IPChanNum(%d)", login_d2c->dev_ability.stDevInfo.u8IPChanNum);
			nc_ys_dev_setOnline(dev, PTC_FRAME_STREAM_LOGIN_FAIL);
			ret = NC_ERROR_INTERNAL;
			break;
		}
		
		tool_mem_memcpy(&dev->login_d2c, login_d2c, sizeof(ys_msg_login_d2c_t));
		log_state("ch_num(%d,%d) alarm_num(%d,%d) audio(%d,%d)", 
			dev->login_d2c.dev_ability.stDevInfo.u8IPChanNum,      dev->login_d2c.dev_ability.stDevInfo.u8ZeroChanNum, 
			dev->login_d2c.dev_ability.stDevInfo.u8AlarmInPortNum, dev->login_d2c.dev_ability.stDevInfo.u8AlarmOutPortNum, 
			dev->login_d2c.dev_ability.stDevInfo.u8AudioInNum,     dev->login_d2c.dev_ability.stDevInfo.u8AudioOutNum);
	
	} while (0);
	if (ret < 0)
	{
        if (tool_sock_item_isLive(&dev->msg_sock))
			tool_sock_item_close(&dev->msg_sock);
    }
	else
	{
		nc_ys_dev_setOnline(dev, PTC_FRAME_STREAM_RUNNING);
	}
	return ret;
}

TOOL_INT32 nc_ys_dev_logout(nc_ys_dev_t* dev)
{
	tool_mem_memset(&dev->ys_msg.head, sizeof(ys_msg_head_t));
	dev->ys_msg.head.u16Command = YS_MSG_ID_USER_LOGOUT;
	dev->ys_msg.head.u32ContentLength = sizeof(ys_msg_logout_c2d_t);
	ys_msg_logout_c2d_t* logout_c2d = (ys_msg_logout_c2d_t*)dev->ys_msg.data;
	tool_mem_memset(logout_c2d, sizeof(ys_msg_logout_c2d_t));
	tool_ptc_ys_fillIeHead_logout_c2d(logout_c2d);
	tool_mem_memcpy(&logout_c2d->user_log_info, &dev->login_d2c.user_log_info, sizeof(ys_ie_user_log_info_t));
	tool_mem_memcpy(&logout_c2d->user_log_type, &dev->login_c2d.user_log_type, sizeof(ys_ie_user_log_type_t));
	if (tool_ptc_ys_sendMsg(&dev->msg_sock, &dev->ys_msg) < 0 ||
		nc_ys_dev_recvMsg(dev, YS_MSG_ID_USER_LOGOUT) < 0)
	{
		log_error("nc_ys_dev_logout");
	}
	
	TOOL_INT32 i = 0, j = 0;
	if (tool_sock_item_isLive(&dev->msg_sock))
		tool_sock_item_close(&dev->msg_sock);
	for (i = 0; i < NC_YS_CHANNEL_SIZE; i++)
	{
		for (j = 0; j < NC_YS_STREAM_SIZE; j++)
		{
			dev->rts[i][j].state = 0;
			if (tool_sock_item_isLive(&dev->rts[i][j].rts_sock))
				tool_sock_item_close(&dev->rts[i][j].rts_sock);
			dev->rts[i][j].error_flag = 0;
			dev->rts[i][j].no_video = 0;
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
	nc_ys_dev_setOnline(dev, PTC_FRAME_STREAM_WAIT);
	return 0;
}

TOOL_INT32 nc_ys_dev_heartbeat(nc_ys_dev_t* dev)
{
	if (tool_sysf_isTickTimeout(dev->hb_tick_sec, dev->hb_tick_usec, NC_YS_HEARTBEAT_CYCLE*1000*1000) == 0)
		return 0;
	
	tool_mem_memset(&dev->ys_msg.head, sizeof(ys_msg_head_t));
	dev->ys_msg.head.u16Command = YS_MSG_ID_USER_KEEPLIVE;
	dev->ys_msg.head.u32ContentLength = sizeof(ys_ie_user_log_info_t);
	ys_msg_heartbeat_c2d_t* headbeat_c2d = (ys_msg_heartbeat_c2d_t*)dev->ys_msg.data;
	tool_mem_memset(headbeat_c2d, sizeof(ys_msg_heartbeat_c2d_t));
	tool_ptc_ys_fillIeHead_heartbeat_c2d(headbeat_c2d);
	tool_mem_memcpy(&headbeat_c2d->user_log_info, &dev->login_d2c.user_log_info, sizeof(ys_ie_user_log_info_t));
	if (tool_ptc_ys_sendMsg(&dev->msg_sock, &dev->ys_msg) < 0)
	{
		log_error("tool_ptc_ys_sendMsg");
		return -1;
	}
	if (nc_ys_dev_recvMsg(dev, YS_MSG_ID_USER_KEEPLIVE) < 0)
	{
		log_error("nc_ys_dev_recvMsg");
		return -1;
	}

/*	
	static TOOL_INT32 count = 0;
	count ++;
	log_debug("nc_ys_dev_heartbeat count(%d)", count);
	if (count >= 6)
	{
		count = 0;
		return -1;
	}
*/
	
	tool_sysf_setTick(&dev->hb_tick_sec, &dev->hb_tick_usec);
	return 0;
}

TOOL_INT32 nc_ys_dev_doRts(nc_ys_dev_t* dev)
{	
	TOOL_INT32 i = 0, j = 0;
	for (i = 0; i < NC_YS_CHANNEL_SIZE; i++)
	{
		for (j = 0; j < NC_YS_STREAM_SIZE; j++)
		{
			if (dev->rts[i][j].error_flag)
			{
				if (nc_ys_rts_stop(&dev->rts[i][j]) < 0)
				{
					log_error("nc_ys_rts_stop[%d][%d] error", i, j);
					return -1;
				}
				tool_sock_setTick(&dev->rts[i][j].tick);
				log_state("nc_ys_rts_stop[%d][%d]", i, j);
	//			log_error("dev->rts[%d].error_flag", i);
	//			return -1;
			}
		}
	}

	TOOL_INT32 rts_enable[NC_YS_CHANNEL_SIZE][NC_YS_STREAM_SIZE] = {{0}};
	tool_thread_lockMutex(&dev->mutex);
	for (i = 0; i < NC_YS_CHANNEL_SIZE; i++)
	{
		for (j = 0; j < NC_YS_STREAM_SIZE; j++)
		{
			rts_enable[i][j] = dev->cur_rts_enable[i][j];
		}
	}
	tool_thread_unlockMutex(&dev->mutex);	

	for (i = 0; i < NC_YS_CHANNEL_SIZE; i++)
	{
		for (j = 0; j < NC_YS_STREAM_SIZE; j++)
		{
			if (dev->rts[i][j].state == rts_enable[i][j])
				continue;
			if (rts_enable[i][j])
			{
				if (dev->rts[i][j].no_video)
					continue;
				
				if (tool_sock_isTickTimeout(dev->rts[i][j].tick, 5) == 0)
				{
					dev->rts[i][j].online = 0;
					continue;				
				}
				if (nc_ys_rts_start(&dev->rts[i][j]) < 0)
				{
					if (dev->ys_msg.head.u16Status == YS_ERR_LIVE_INPUT_NOT_READY)
					{
						dev->rts[i][j].no_video = 1;
						tool_sock_setTick(&dev->rts[i][j].tick);
						dev->rts[i][j].online = PTC_FRAME_STREAM_NOVIDEO;
						continue;
					}
					log_error("nc_ys_rts_start[%d][%d] error", i, j);
					return -1;
				}
				log_state("nc_ys_rts_start[%d][%d]", i, j);
			}
			else
			{
				if (nc_ys_rts_stop(&dev->rts[i][j]) < 0)
				{
					log_error("nc_ys_rts_stop[%d][%d] error", i, j);
					return -1;
				}
				log_state("nc_ys_rts_stop[%d][%d]", i, j);
			}
		}		
	}
	return 0;
}

TOOL_INT32 nc_ys_dev_doPtz(nc_ys_dev_t* dev)
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

	ys_msg_ptz_c2d_t* ptz_c2d = (ys_msg_ptz_c2d_t*)dev->ys_msg.data;
	if (dev->last_ptz_channel_no != ptz_channel_no ||
		dev->last_ptz_cmd != ptz_cmd ||
		dev->last_ptz_param != ptz_param ||
		dev->last_ptz_step != ptz_step)
	{
		dev->ys_msg.head.u16Command = YS_MSG_ID_PTZ_COMMAND;
		dev->ys_msg.head.u32ContentLength = sizeof(ys_msg_ptz_c2d_t);
		tool_mem_memset(ptz_c2d, sizeof(ys_msg_ptz_c2d_t));
		tool_ptc_ys_fillIeHead_ptz_c2d(ptz_c2d);
		tool_mem_memcpy(&ptz_c2d->user_log_info, &dev->login_d2c.user_log_info, sizeof(ys_ie_user_log_info_t));
		ptz_c2d->resource_code.stResourceCode.u8DevType = YS_DEV_TYPE_DIGITAL_ENCODER;
		ptz_c2d->resource_code.stResourceCode.u8ResType = YS_RES_TYPE_VIDEO_IN_CHL;
		ptz_c2d->resource_code.stResourceCode.u8MappingChlIndex = ptz_channel_no+1;
		ptz_c2d->resource_code.stResourceCode.u8ChlID = ptz_channel_no+1;
		if (ptz_cmd == PTC_PTZ_CMD_LEFT)
			ptz_c2d->ptz_command.u16Command = YS_PTZ_CMD_PANLEFT;
		else if (ptz_cmd == PTC_PTZ_CMD_RIGHT)
			ptz_c2d->ptz_command.u16Command = YS_PTZ_CMD_PANRIGHT;
		else if (ptz_cmd == PTC_PTZ_CMD_UP)
			ptz_c2d->ptz_command.u16Command = YS_PTZ_CMD_TILTUP;
		else if (ptz_cmd == PTC_PTZ_CMD_DOWN)
			ptz_c2d->ptz_command.u16Command = YS_PTZ_CMD_TILTDOWN;
		else if (ptz_cmd == PTC_PTZ_CMD_LEFT_UP)
			ptz_c2d->ptz_command.u16Command = YS_PTZ_CMD_LEFTUP;
		else if (ptz_cmd == PTC_PTZ_CMD_LEFT_DOWN)
			ptz_c2d->ptz_command.u16Command = YS_PTZ_CMD_LEFTDOWN;
		else if (ptz_cmd == PTC_PTZ_CMD_RIGHT_UP)
			ptz_c2d->ptz_command.u16Command = YS_PTZ_CMD_RIGHTUP;
		else if (ptz_cmd == PTC_PTZ_CMD_RIGHT_DOWN)
			ptz_c2d->ptz_command.u16Command = YS_PTZ_CMD_RIGHTDOWN;
		else if (ptz_cmd == PTC_PTZ_CMD_NEAR)
			ptz_c2d->ptz_command.u16Command = YS_PTZ_CMD_FOCUSNEAR;
		else if (ptz_cmd == PTC_PTZ_CMD_FAR)
			ptz_c2d->ptz_command.u16Command = YS_PTZ_CMD_FOCUSFAR;
		else if (ptz_cmd == PTC_PTZ_CMD_ZOOM_OUT)
			ptz_c2d->ptz_command.u16Command = YS_PTZ_CMD_ZOOMTELE;
		else if (ptz_cmd == PTC_PTZ_CMD_ZOOM_IN)
			ptz_c2d->ptz_command.u16Command = YS_PTZ_CMD_ZOOMWIDE;
		else if (ptz_cmd == PTC_PTZ_CMD_IRIS_OPEN)
			ptz_c2d->ptz_command.u16Command = YS_PTZ_CMD_IRISOPEN;
		else if (ptz_cmd == PTC_PTZ_CMD_IRIS_CLOSE)
			ptz_c2d->ptz_command.u16Command = YS_PTZ_CMD_IRISCLOSE;
		else if (ptz_cmd == PTC_PTZ_CMD_PRESET_GO)
			ptz_c2d->ptz_command.u16Command = YS_PTZ_CMD_PRECALL;
		else if (ptz_cmd == PTC_PTZ_CMD_PRESET_SET)
			ptz_c2d->ptz_command.u16Command = YS_PTZ_CMD_PRESAVE;
		else if (ptz_cmd == PTC_PTZ_CMD_PRESET_DEL)
			ptz_c2d->ptz_command.u16Command = YS_PTZ_CMD_PREDEL;
		else if (ptz_cmd == PTC_PTZ_CMD_AUTO_SCAN_START)
			ptz_c2d->ptz_command.u16Command = YS_PTZ_CMD_SCANCRUISE;
		else if (ptz_cmd == PTC_PTZ_CMD_AUTO_SCAN_STOP)
			ptz_c2d->ptz_command.u16Command = YS_PTZ_CMD_SCANCRUISESTOP;
		else 
			ptz_c2d->ptz_command.u16Command = YS_PTZ_CMD_ALLSTOP;
		ptz_c2d->ptz_command.u8Param1 = ptz_param;
		ptz_c2d->ptz_command.u8Param2 = ptz_param;
		if (tool_ptc_ys_sendMsg(&dev->msg_sock, &dev->ys_msg) < 0 ||
			nc_ys_dev_recvMsg(dev, YS_MSG_ID_PTZ_COMMAND) < 0)
		{
			log_error("YS_MSG_ID_PTZ_COMMAND");
			return -1;
		}
		
		tool_sysf_setTick(&dev->ptz_tick_sec, &dev->ptz_tick_usec);
		dev->last_ptz_channel_no = ptz_channel_no;
		dev->last_ptz_cmd = ptz_cmd;
		dev->last_ptz_param = ptz_param;
		dev->last_ptz_step = ptz_step;
	}
	return 0;
}

TOOL_INT32 nc_ys_dev_doRecords(nc_ys_dev_t* dev)
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
	dev->ys_search = (ys_search_t*)tool_mem_malloc(sizeof(ys_search_t), 0); 


	TOOL_INT32 ret = 0;
	TOOL_INT32 result = 0;
	do
	{
		records.num = 0;
		TOOL_INT32 i = 0;
		TOOL_UINT8 channel_no = 0;
		for (i = 0; i < NC_YS_CHANNEL_SIZE; i++)
		{
			if (records.channel & (TOOL_UINT64)(1<<i))
			{
				channel_no = i;
				break;
			}
		}
		if (i == NC_YS_CHANNEL_SIZE)
		{
			log_error("channel(%lld)", records.channel);
			result = NC_ERROR_INPUT;
			break;
		}

		ys_msg_searchrecords_c2d_t* pst_searchrecords_c2d = (ys_msg_searchrecords_c2d_t*)dev->ys_msg.data;
		dev->ys_msg.head.u16Command = YS_MSG_ID_VOD_QRY;
		dev->ys_msg.head.u32ContentLength = sizeof(ys_msg_searchrecords_c2d_t);
		tool_mem_memset(pst_searchrecords_c2d, sizeof(ys_msg_searchrecords_c2d_t));
		tool_ptc_ys_fillIeHead_searchrecords_c2d(pst_searchrecords_c2d);
		tool_mem_memcpy(&pst_searchrecords_c2d->user_log_info, &dev->login_d2c.user_log_info, sizeof(ys_ie_user_log_info_t));
		pst_searchrecords_c2d->resource_code.stResourceCode.u8DevType = YS_DEV_TYPE_DIGITAL_ENCODER;
		pst_searchrecords_c2d->resource_code.stResourceCode.u8ResType = YS_RES_TYPE_VIDEO_IN_CHL;
		pst_searchrecords_c2d->resource_code.stResourceCode.u8MappingChlIndex = channel_no+1;
		pst_searchrecords_c2d->resource_code.stResourceCode.u8ChlID = channel_no+1;
		pst_searchrecords_c2d->time_range.u32Begin = records.start_time;
		pst_searchrecords_c2d->time_range.u32End   = records.end_time;
/*
		if (records.type == PTC_RECORD_TYPE_ALL)
			dev->ys_msg.head.u32ContentLength = sizeof(ys_msg_searchrecords_c2d_t) - sizeof(ys_ie_storage_type_t);
		else if (records.type == PTC_RECORD_TYPE_TIME)
			pst_searchrecords_c2d->storage_type.u32RecordType = YS_RECORD_TYPE_NORMAL;
		else if (records.type == PTC_RECORD_TYPE_ALARM)
		{
			if (records.sub_type == PTC_RECORD_SUBTYPE_MOTION)
				pst_searchrecords_c2d->storage_type.u32RecordType = YS_RECORD_TYPE_MOVEDETECT;
			else 
				pst_searchrecords_c2d->storage_type.u32RecordType = YS_RECORD_TYPE_ALARM;
		}
		else if (records.type == PTC_RECORD_TYPE_MANUAL)
			pst_searchrecords_c2d->storage_type.u32RecordType = YS_RECORD_TYPE_UW_MANUAL;
*/
		log_debug("channel(%d) time(%d-%d) type(%d.%d)", channel_no, records.start_time, records.end_time, records.type, records.sub_type);

		if (tool_ptc_ys_sendMsg(&dev->msg_sock, &dev->ys_msg) < 0)
		{
			ret = NC_ERROR_SEND;
			result = NC_ERROR_SEND;
			break;
		}
		if (nc_ys_dev_recvSearch(dev, YS_MSG_ID_VOD_QRY) < 0)
		{
			ret = NC_ERROR_RECV;
			result = NC_ERROR_RECV;
			break;
		}

		ys_msg_searchrecords_d2c_t* pst_searchrecords_d2c = (ys_msg_searchrecords_d2c_t*)dev->ys_search->data;
		
		for (i = 0; i < YS_VOID_FILE_NUM; i++)
		{
			if (records.num >= PTC_RECORDS_SIZE)
				break;
			if (i * sizeof(ys_ie_vod_file_t) >= dev->ys_search->head.u32ContentLength)
				break;
			log_debug("record(%d) time(%d-%d), type(%d)", 
				records.num, pst_searchrecords_d2c->stAr_vod_file[i].u32Begin, pst_searchrecords_d2c->stAr_vod_file[i].u32End,
				pst_searchrecords_d2c->stAr_vod_file[i].u32RecordType);


			if (pst_searchrecords_d2c->stAr_vod_file[i].u32RecordType == YS_RECORD_TYPE_NORMAL)
			{
				if (records.type == PTC_RECORD_TYPE_ALL || records.type == PTC_RECORD_TYPE_TIME)
				{
					records.record[records.num].type = PTC_RECORD_TYPE_TIME;
					records.record[records.num].sub_type = PTC_RECORD_SUBTYPE_NONE;			
				}
				else
					continue;
			}
			else if (pst_searchrecords_d2c->stAr_vod_file[i].u32RecordType == YS_RECORD_TYPE_MOVEDETECT)
			{
				if (records.type == PTC_RECORD_TYPE_ALL)
				{
					records.record[records.num].type = PTC_RECORD_TYPE_ALARM;
					records.record[records.num].sub_type = PTC_RECORD_SUBTYPE_MOTION;			
				}
				else if (records.type == PTC_RECORD_TYPE_ALARM)
				{
					if (records.sub_type == PTC_RECORD_SUBTYPE_MOTION)
					{
						records.record[records.num].type = PTC_RECORD_TYPE_ALARM;
						records.record[records.num].sub_type = PTC_RECORD_SUBTYPE_MOTION;		
					}
					else 
						continue;
				}
				else
					continue;
			}
			else if (pst_searchrecords_d2c->stAr_vod_file[i].u32RecordType == YS_RECORD_TYPE_ALARM)
			{
				if (records.type == PTC_RECORD_TYPE_ALL)
				{
					records.record[records.num].type = PTC_RECORD_TYPE_ALARM;
					records.record[records.num].sub_type = PTC_RECORD_SUBTYPE_SENSOR;			
				}
				else if (records.type == PTC_RECORD_TYPE_ALARM)
				{
					if (records.sub_type == PTC_RECORD_SUBTYPE_SENSOR)
					{
						records.record[records.num].type = PTC_RECORD_TYPE_ALARM;
						records.record[records.num].sub_type = PTC_RECORD_SUBTYPE_SENSOR;		
					}
					else 
						continue;
				}
				else
					continue;
			}
			else if (pst_searchrecords_d2c->stAr_vod_file[i].u32RecordType == YS_RECORD_TYPE_MA ||
					pst_searchrecords_d2c->stAr_vod_file[i].u32RecordType == YS_RECORD_TYPE_M_A)
			{
				if (records.type == PTC_RECORD_TYPE_ALL)
				{
					records.record[records.num].type = PTC_RECORD_TYPE_ALARM;
					records.record[records.num].sub_type = PTC_RECORD_SUBTYPE_MOTION | PTC_RECORD_SUBTYPE_SENSOR;			
				}
				else if (records.type == PTC_RECORD_TYPE_ALARM)
				{
					if (records.sub_type == PTC_RECORD_SUBTYPE_MOTION || records.sub_type == PTC_RECORD_SUBTYPE_SENSOR)
					{
						records.record[records.num].type = PTC_RECORD_TYPE_ALARM;
						records.record[records.num].sub_type = PTC_RECORD_SUBTYPE_MOTION | PTC_RECORD_SUBTYPE_SENSOR;		
					}
					else 
						continue;
				}
				else
					continue;
			}
			else if (pst_searchrecords_d2c->stAr_vod_file[i].u32RecordType == YS_RECORD_TYPE_UW_MANUAL)
			{
				if (records.type == PTC_RECORD_TYPE_ALL || records.type == PTC_RECORD_TYPE_MANUAL)
				{
					records.record[records.num].type = PTC_RECORD_TYPE_MANUAL;
					records.record[records.num].sub_type = PTC_RECORD_SUBTYPE_NONE; 		
				}
				else
					continue;
			}
			else
				continue;
		
			records.record[records.num].start_time = pst_searchrecords_d2c->stAr_vod_file[i].u32Begin;
			records.record[records.num].end_time = pst_searchrecords_d2c->stAr_vod_file[i].u32End;
			records.record[records.num].size = 0;
			records.record[records.num].channel = channel_no;
			tool_mem_memcpy(records.record[records.num].file_info, &pst_searchrecords_d2c->stAr_vod_file[i], sizeof(ys_ie_vod_file_t));
			records.num ++;
		}
	}while (0);
	tool_mem_free(dev->ys_search);
	dev->ys_search = NULL;

	tool_thread_lockMutex(&dev->mutex);
	tool_mem_memcpy(&dev->cfg_records, &records, sizeof(ptc_cfg_records_t));
	dev->result_records = result;
	dev->last_records_req = dev->cur_records_req;
	tool_thread_broadcastCond(&dev->cond_records);
	tool_thread_unlockMutex(&dev->mutex);
	return ret; 
}

TOOL_INT32 nc_ys_dev_doLogs(nc_ys_dev_t* dev)
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

	logs.num = 0;
	TOOL_INT32 ret = 0;
	TOOL_INT32 result = 0;
	dev->ys_search = (ys_search_t*)tool_mem_malloc(sizeof(ys_search_t), 0); 	
	
	do
	{
		ys_msg_searchlogs_c2d_t* pst_searchlogs_c2d = (ys_msg_searchlogs_c2d_t*)dev->ys_msg.data;
		dev->ys_msg.head.u16Command = YS_MSG_ID_QUERY_LOG;
		dev->ys_msg.head.u32ContentLength = sizeof(ys_msg_searchlogs_c2d_t);
		tool_mem_memset(pst_searchlogs_c2d, sizeof(ys_msg_searchlogs_c2d_t));
		tool_ptc_ys_fillIeHead_searchlogs_c2d(pst_searchlogs_c2d);
		tool_mem_memcpy(&pst_searchlogs_c2d->user_log_info, &dev->login_d2c.user_log_info, sizeof(ys_ie_user_log_info_t));
		pst_searchlogs_c2d->log_type.u16MainType = YS_LOG_MAINTYPE_ALL;
		pst_searchlogs_c2d->log_type.u16SubType = YS_LOG_ALL_SUB_TYPES;
		pst_searchlogs_c2d->time_range.u32Begin = logs.start_time;
		pst_searchlogs_c2d->time_range.u32End = logs.end_time;
		if (tool_ptc_ys_sendMsg(&dev->msg_sock, &dev->ys_msg) < 0)
		{
			ret = NC_ERROR_SEND;
			result = NC_ERROR_SEND;
			break;
		}
		if (nc_ys_dev_recvSearch(dev, YS_MSG_ID_QUERY_LOG) < 0)
		{
			ret = NC_ERROR_SEND;
			result = NC_ERROR_SEND;
			break;
		}

		ys_msg_searchlogs_d2c_t* pst_searchlogs_d2c = (ys_msg_searchlogs_d2c_t*)dev->ys_search->data;
		TOOL_INT32 i = 0;
		for (i = 0; i < YS_LOG_MAX_NUM; i++)
		{
			if (logs.num >= PTC_LOGS_SIZE)
					break;
			if (i * sizeof(ys_ie_log_info_t) >= dev->ys_search->head.u32ContentLength)
				break;
/*			
			log_debug("u32Time(%d) u16MainType(%d) u16SubType(%d) acChannCode(%s) szUserName(%s)", 
				pst_searchlogs_d2c->log_info[i].u32Time, 
				pst_searchlogs_d2c->log_info[i].u16MainType,
				pst_searchlogs_d2c->log_info[i].u16SubType, 
				pst_searchlogs_d2c->log_info[i].acChannCode,
				pst_searchlogs_d2c->log_info[i].szUserName);
*/				
			logs.log[logs.num].time = pst_searchlogs_d2c->log_info[i].u32Time;
			if (pst_searchlogs_d2c->log_info[i].u16MainType == YS_LOG_MAINTYPE_ALARM)
				logs.log[logs.num].type = PTC_LOG_TYPE_ALARM;
			else if (pst_searchlogs_d2c->log_info[i].u16MainType == YS_LOG_MAINTYPE_EXCEPTION 
				|| pst_searchlogs_d2c->log_info[i].u16MainType == YS_LOG_MAINTYPE_MESSAGE)
				logs.log[logs.num].type = PTC_LOG_TYPE_SYSTEM;
			else if (pst_searchlogs_d2c->log_info[i].u16MainType == YS_LOG_MAINTYPE_OPERATION)
				logs.log[logs.num].type = PTC_LOG_TYPE_OPERATE;
			else
			{
				log_error("pst_searchlogs_d2c->log_info[i].u16MainType(d%d)", pst_searchlogs_d2c->log_info[i].u16MainType);
				continue;
			}
			logs.log[logs.num].sub_type = PTC_LOG_DIAMOND_NVR;
			logs.log[logs.num].sub_type2 = pst_searchlogs_d2c->log_info[i].u16SubType;
			if (pst_searchlogs_d2c->log_info[i].acChannCode[0] == 'D')
			{
				logs.log[logs.num].ch_type = PTC_CH_TYPE_DIGIT;
				logs.log[logs.num].channel = tool_sysf_atoi((TOOL_INT8*)&pst_searchlogs_d2c->log_info[i].acChannCode[1])-1;
			}
			else if (tool_mem_memcmp(pst_searchlogs_d2c->log_info[i].acChannCode, "A->", 3) == 0)
			{
				logs.log[logs.num].ch_type = PTC_CH_TYPE_LOCAL_ALARM_OUT;
				logs.log[logs.num].channel = tool_sysf_atoi((TOOL_INT8*)&pst_searchlogs_d2c->log_info[i].acChannCode[3])-1;
			}
			else if (tool_mem_memcmp(pst_searchlogs_d2c->log_info[i].acChannCode, "A<-", 3) == 0)
			{
				logs.log[logs.num].ch_type = PTC_CH_TYPE_LOCAL_ALARM_IN;
				logs.log[logs.num].channel = tool_sysf_atoi((TOOL_INT8*)&pst_searchlogs_d2c->log_info[i].acChannCode[3])-1;
			}
			else
			{
				logs.log[logs.num].ch_type = PTC_CH_TYPE_LOCAL_RTS_MAIN;
				logs.log[logs.num].channel = 0;
			}
			tool_mem_memcpy(logs.log[logs.num].ip, &pst_searchlogs_d2c->log_info[i].ip_addr.be32Ipv4Addr, 4);
			logs.num ++;
		}
	}while (0);
	tool_mem_free(dev->ys_search);
	dev->ys_search = NULL;


	log_debug("logs.num(%d)", logs.num);

	tool_thread_lockMutex(&dev->mutex);
	tool_mem_memcpy(&dev->cfg_logs, &logs, sizeof(ptc_logs_t));
	dev->result_logs = result;
	dev->last_logs_req = dev->cur_logs_req;
	tool_thread_broadcastCond(&dev->cond_logs);
	tool_thread_unlockMutex(&dev->mutex);
	return ret;
}

TOOL_INT32 nc_ys_dev_doPbs(nc_ys_dev_t* dev)
{
	if (dev->pbs.error_flag)
	{
//		log_error("dev->pbs.error_flag");
//		return -1;
		if (nc_ys_pbs_stop(&dev->pbs) < 0)
		{
			log_error("nc_ys_pbs_stop error");
			return -1;
		}
		log_state("YS_CMD_PBS_STOP success");
		tool_sock_setTick(&dev->pbs.reconnect_tick_sec);
	}
	TOOL_INT32 pbs_enable;
	ptc_cfg_record_t ptc_record;
	tool_thread_lockMutex(&dev->mutex);	
	pbs_enable = dev->cur_pbs_enable;
	tool_mem_memcpy(&ptc_record, &dev->pbs.ptc_record, sizeof(ptc_cfg_record_t));
	tool_thread_unlockMutex(&dev->mutex);

	if (dev->pbs.state != pbs_enable)
	{
		if (pbs_enable == 0)
		{
			if (nc_ys_pbs_stop(&dev->pbs) < 0)
			{
				log_error("nc_ys_pbs_stop error");
				return -1;
			}
			log_state("YS_CMD_PBS_STOP success");
		}
		else
		{
			if (tool_sock_isTickTimeout(dev->pbs.reconnect_tick_sec, 5) == 0)
			{
				dev->pbs.online = 0;
				return 0;		
			}
			
			if (nc_ys_pbs_start(&dev->pbs, &ptc_record) < 0)
			{
				log_error("nc_ys_pbs_start error");
				return -1;
			}
			dev->pbs.tick_sec = 0;
			log_state("YS_CMD_PBS_START success");
		}
	}

	if (dev->pbs.state)
	{
		if (nc_ys_pbs_get(&dev->pbs) < 0)
		{
			log_error("nc_ys_pbs_get error");
			return -1;
		}
		if (dev->pbs_pause)
		{
			if (tool_stream3_state(&dev->pbs.stream) < 0)
			{
				if (nc_ys_pbs_resume(&dev->pbs) < 0)
				{
					log_error("nc_ys_pbs_resume");
					return -1;
				}
				dev->pbs_pause = 0;
			}
		}
		else
		{
			if (tool_stream3_state(&dev->pbs.stream) > 0)
			{
				if (nc_ys_pbs_pause(&dev->pbs) < 0)
				{
					log_error("nc_ys_pbs_pause");
					return -1;
				}
				dev->pbs_pause = 1;
			}
		}
	}

	return 0;
}

TOOL_INT32 nc_ys_dev_doTalk(nc_ys_dev_t* dev)
{
	if (dev->talk.error_flag)
	{
		if (nc_ys_talk_stop(&dev->talk) < 0)
		{
			log_error("nc_ys_talk_stop error");
			return -1;
		}
		log_state("nc_ys_talk_stop success");
		tool_sock_setTick(&dev->talk.tick);
	//	log_error("dev->talk.error_flag");
	//	return -1;		
	}
	
	TOOL_INT32 talk_enable;
	tool_thread_lockMutex(&dev->mutex);
	talk_enable = dev->cur_talk_enable;
	tool_thread_unlockMutex(&dev->mutex);

	if (dev->talk.state != talk_enable)
	{
		if (talk_enable == 0)
		{
			if (nc_ys_talk_stop(&dev->talk) < 0)
			{
				log_error("nc_ys_talk_stop error");
				return -1;
			}
			log_state("nc_ys_talk_stop success");
		}
		else
		{
			if (tool_sock_isTickTimeout(dev->talk.tick, 5) == 0)
			{
				dev->talk.online = 0;
				return 0;		
			}
			
			if (nc_ys_talk_start(&dev->talk) < 0)
			{	
				if (dev->ys_msg.head.u16Status == YS_ERR_TALK_EXISTED)
				{
					tool_sock_setTick(&dev->talk.tick);
					dev->talk.online = PTC_FRAME_STREAM_TALK_BUSY;
					return 0;
				}
				log_error("nc_ys_talk_start error");
				return -1;
			}
			log_state("nc_ys_talk_start success");
		}
 	}
	return 0;
}

TOOL_INT32 nc_ys_dev_doSyncTime(nc_ys_dev_t* dev)
{
	TOOL_UINT32 utc_sec;
	TOOL_INT32 zone;
	TOOL_INT32 dst_hour;
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

	tool_mem_memset(&dev->ys_msg.head, sizeof(ys_msg_head_t));
	dev->ys_msg.head.u16Command = YS_MSG_ID_QUERY_TIME;
	dev->ys_msg.head.u32ContentLength = sizeof(ys_msg_gettime_c2d_t);
	ys_msg_gettime_c2d_t* pst_gettime_c2d = (ys_msg_gettime_c2d_t*)dev->ys_msg.data;
	tool_mem_memset(pst_gettime_c2d, sizeof(ys_msg_gettime_c2d_t));
	tool_ptc_ys_fillIeHead_gettime_c2d(pst_gettime_c2d);
	tool_mem_memcpy(&pst_gettime_c2d->user_log_info, &dev->login_d2c.user_log_info, sizeof(ys_ie_user_log_info_t));
	if (tool_ptc_ys_sendMsg(&dev->msg_sock, &dev->ys_msg) < 0)
	{
		log_error("tool_ptc_ys_sendMsg");
		return -1;
	}
	if (nc_ys_dev_recvMsg(dev, YS_MSG_ID_QUERY_TIME) < 0)
	{
		log_error("nc_ys_dev_recvMsg");
		return -1;
	}
	ys_msg_gettime_d2c_t st_msg_gettime_d2c;
	tool_mem_memcpy(&st_msg_gettime_d2c, dev->ys_msg.data, sizeof(ys_msg_gettime_d2c_t));

	tool_mem_memset(&dev->ys_msg.head, sizeof(ys_msg_head_t));
	dev->ys_msg.head.u16Command = YS_MSG_ID_SET_TIME;
	dev->ys_msg.head.u32ContentLength = sizeof(ys_msg_settime_c2d_t);
	ys_msg_settime_c2d_t* settime_c2d = (ys_msg_settime_c2d_t*)dev->ys_msg.data;
	tool_mem_memset(settime_c2d, sizeof(ys_msg_settime_c2d_t));
	tool_ptc_ys_fillIeHead_settime_c2d(settime_c2d);
	tool_mem_memcpy(&settime_c2d->user_log_info,  &dev->login_d2c.user_log_info,      sizeof(ys_ie_user_log_info_t));
	tool_mem_memcpy(&settime_c2d->time_zone_info, &st_msg_gettime_d2c.time_zone_info, sizeof(ys_ie_time_zone_info_t));
	tool_mem_memcpy(&settime_c2d->modify_time,    &st_msg_gettime_d2c.modify_time,    sizeof(ys_ie_modify_time_t));
	tool_mem_memcpy(&settime_c2d->ntp,            &st_msg_gettime_d2c.ntp,            sizeof(ys_ie_ntp_t));
	tool_mem_memcpy(&settime_c2d->time,           &st_msg_gettime_d2c.time,           sizeof(ys_ie_time_t));
	tool_mem_memcpy(&settime_c2d->dst,            &st_msg_gettime_d2c.dst,            sizeof(ys_ie_dst_t));
	if (zone == PTC_TIME_ZONE_W1200)
		settime_c2d->time_zone_info.u8TimeZone = YS_TIME_ZONE_1200M;
	else if (zone == PTC_TIME_ZONE_W1100)
		settime_c2d->time_zone_info.u8TimeZone = YS_TIME_ZONE_1100M;
	else if (zone == PTC_TIME_ZONE_W1000)
		settime_c2d->time_zone_info.u8TimeZone = YS_TIME_ZONE_1000M;
	else if (zone == PTC_TIME_ZONE_W0900)
		settime_c2d->time_zone_info.u8TimeZone = YS_TIME_ZONE_0900M;
	else if (zone == PTC_TIME_ZONE_W0800)
		settime_c2d->time_zone_info.u8TimeZone = YS_TIME_ZONE_0800M;
	else if (zone == PTC_TIME_ZONE_W0700)
		settime_c2d->time_zone_info.u8TimeZone = YS_TIME_ZONE_0700M;
	else if (zone == PTC_TIME_ZONE_W0600)
		settime_c2d->time_zone_info.u8TimeZone = YS_TIME_ZONE_0600M;
	else if (zone == PTC_TIME_ZONE_W0500)
		settime_c2d->time_zone_info.u8TimeZone = YS_TIME_ZONE_0500M;
	else if (zone == PTC_TIME_ZONE_W0430)
		settime_c2d->time_zone_info.u8TimeZone = YS_TIME_ZONE_0430M;
	else if (zone == PTC_TIME_ZONE_W0400)
		settime_c2d->time_zone_info.u8TimeZone = YS_TIME_ZONE_0400M;
	else if (zone == PTC_TIME_ZONE_W0330)
		settime_c2d->time_zone_info.u8TimeZone = YS_TIME_ZONE_0330M;
	else if (zone == PTC_TIME_ZONE_W0300)
		settime_c2d->time_zone_info.u8TimeZone = YS_TIME_ZONE_0300M;
	else if (zone == PTC_TIME_ZONE_W0200)
		settime_c2d->time_zone_info.u8TimeZone = YS_TIME_ZONE_0200M;
	else if (zone == PTC_TIME_ZONE_W0100)
		settime_c2d->time_zone_info.u8TimeZone = YS_TIME_ZONE_0100M;
	else if (zone == PTC_TIME_ZONE_C0000)
		settime_c2d->time_zone_info.u8TimeZone = YS_TIME_ZONE_0000;
	else if (zone == PTC_TIME_ZONE_E0100)
		settime_c2d->time_zone_info.u8TimeZone = YS_TIME_ZONE_0100P;
	else if (zone == PTC_TIME_ZONE_E0200)
		settime_c2d->time_zone_info.u8TimeZone = YS_TIME_ZONE_0200P;
	else if (zone == PTC_TIME_ZONE_E0300)
		settime_c2d->time_zone_info.u8TimeZone = YS_TIME_ZONE_0300P;
	else if (zone == PTC_TIME_ZONE_E0330)
		settime_c2d->time_zone_info.u8TimeZone = YS_TIME_ZONE_0330P;
	else if (zone == PTC_TIME_ZONE_E0400)
		settime_c2d->time_zone_info.u8TimeZone = YS_TIME_ZONE_0400P;
	else if (zone == PTC_TIME_ZONE_E0430)
		settime_c2d->time_zone_info.u8TimeZone = YS_TIME_ZONE_0430P;
	else if (zone == PTC_TIME_ZONE_E0500)
		settime_c2d->time_zone_info.u8TimeZone = YS_TIME_ZONE_0500P;
	else if (zone == PTC_TIME_ZONE_E0530)
		settime_c2d->time_zone_info.u8TimeZone = YS_TIME_ZONE_0530P;
	else if (zone == PTC_TIME_ZONE_E0545)
		settime_c2d->time_zone_info.u8TimeZone = YS_TIME_ZONE_0545P;
	else if (zone == PTC_TIME_ZONE_E0600)
		settime_c2d->time_zone_info.u8TimeZone = YS_TIME_ZONE_0600P;
	else if (zone == PTC_TIME_ZONE_E0630)
		settime_c2d->time_zone_info.u8TimeZone = YS_TIME_ZONE_0630P;
//	else if (zone == PTC_TIME_ZONE_E0645)
//		settime_c2d->time_zone_info.u8TimeZone = YS_TIME_ZONE_1100M;
	else if (zone == PTC_TIME_ZONE_E0700)
		settime_c2d->time_zone_info.u8TimeZone = YS_TIME_ZONE_0700P;
	else if (zone == PTC_TIME_ZONE_E0800)
		settime_c2d->time_zone_info.u8TimeZone = YS_TIME_ZONE_0800P;
	else if (zone == PTC_TIME_ZONE_E0900)
		settime_c2d->time_zone_info.u8TimeZone = YS_TIME_ZONE_0900P;
	else if (zone == PTC_TIME_ZONE_E0930)
		settime_c2d->time_zone_info.u8TimeZone = YS_TIME_ZONE_0930P;
	else if (zone == PTC_TIME_ZONE_E1000)
		settime_c2d->time_zone_info.u8TimeZone = YS_TIME_ZONE_1000P;
	else if (zone == PTC_TIME_ZONE_E1100)
		settime_c2d->time_zone_info.u8TimeZone = YS_TIME_ZONE_1100P;
	else if (zone == PTC_TIME_ZONE_E1200)
		settime_c2d->time_zone_info.u8TimeZone = YS_TIME_ZONE_1200P;
	else if (zone == PTC_TIME_ZONE_E1300)
		settime_c2d->time_zone_info.u8TimeZone = YS_TIME_ZONE_1300P;
	else
	{
		log_error("zone(%d)", zone);
		dev->last_time_req = dev->cur_time_req;
		return 0;
	}
	settime_c2d->modify_time.u8ModifyTimeType = 9;	//unknown
	settime_c2d->time.u32Time = utc_sec+(zone*15*60)+(dst_hour*60*60);
	if (dst_hour)
		settime_c2d->dst.bIsDSTEnable = 1;
	else
		settime_c2d->dst.bIsDSTEnable = 0;		
	settime_c2d->dst.u8StartMonth = 0;
	settime_c2d->dst.u8StartWeekofMonth = 1;
	settime_c2d->dst.u8StartWeekDay = 0;	
	settime_c2d->dst.u8StartHour = 0;	
	settime_c2d->dst.u8EndMonth = 11;
	settime_c2d->dst.u8EndWeekofMonth = 5;
	settime_c2d->dst.u8EndWeekDay = 6;	
	settime_c2d->dst.u8EndHour = 23;	
	settime_c2d->dst.u8Period = dst_hour*60;	
//	log_debug("nc_ys_dev_doSyncTime zone(%d) sec(%d)(%s) dst_hour(%d)", 
//		settime_c2d->time_zone_info.u8TimeZone, utc_sec, tool_sysf_dateLocal_1(utc_sec), dst_hour);
	if (tool_ptc_ys_sendMsg(&dev->msg_sock, &dev->ys_msg) < 0)
	{
		log_error("tool_ptc_ys_sendMsg");
		return -1;
	}
	if (nc_ys_dev_recvMsg(dev, YS_MSG_ID_SET_TIME) < 0)
	{
		log_error("nc_ys_dev_recvMsg");
		return -1;
	}
	dev->last_time_req = dev->cur_time_req;
	return 0;
}

TOOL_INT32 nc_ys_dev_doGetStatus(nc_ys_dev_t* dev)
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

	TOOL_INT32 ret = 0;
	TOOL_INT32 result = 0;
	TOOL_INT32 i = 0;
	ys_msg_getAO_c2d_t* pst_getAO_c2d = (ys_msg_getAO_c2d_t*)dev->ys_msg.data;
	ys_msg_getAO_d2c_t* pst_getAO_d2c = (ys_msg_getAO_d2c_t*)dev->ys_msg.data;
	cfg_status.io_out = 0;
	for (i = 0; i < dev->login_d2c.dev_ability.stDevInfo.u8AlarmOutPortNum; i++)
	{
		tool_mem_memset(&dev->ys_msg.head, sizeof(ys_msg_head_t));
		dev->ys_msg.head.u16Command = YS_MSG_ID_QUERY_ALARM_OUTPUT;
		dev->ys_msg.head.u32ContentLength = sizeof(ys_msg_getAO_c2d_t);
		tool_mem_memset(pst_getAO_c2d, sizeof(ys_msg_getAO_c2d_t));
		tool_ptc_ys_fillIeHead_getAO_c2d(pst_getAO_c2d);
		tool_mem_memcpy(&pst_getAO_c2d->user_log_info, &dev->login_d2c.user_log_info, sizeof(ys_ie_user_log_info_t));
		pst_getAO_c2d->resource_code.stResourceCode.u8DevType = YS_DEV_TYPE_ANALOG_ENCODER;
		pst_getAO_c2d->resource_code.stResourceCode.u8ResType = YS_RES_TYPE_ALARM_OUT_CHL;
		pst_getAO_c2d->resource_code.stResourceCode.u8ChlID = i+1;
		if (tool_ptc_ys_sendMsg(&dev->msg_sock, &dev->ys_msg) < 0)
		{
			ret = NC_ERROR_SEND;
			result = NC_ERROR_SEND;
			break;
		}
		if (nc_ys_dev_recvMsg(dev, YS_MSG_ID_QUERY_ALARM_OUTPUT) < 0)
		{
			ret = NC_ERROR_RECV;
			result = NC_ERROR_RECV;
			break;
		}
		if (pst_getAO_d2c->pstIEAlarmOutInfo.u8DefaultStatus == YS_AO_MODE_OPEN)
			cfg_status.io_out |= (1<<i);
	}
	log_state("nc_ys_dev_doGetStatus io_out(%016llx)", cfg_status.io_out);

	tool_thread_lockMutex(&dev->mutex);
	tool_mem_memcpy(&dev->cfg_get_status, &cfg_status, sizeof(ptc_cfg_status_t));
	dev->result_status = result;
	dev->last_get_status_req = dev->cur_get_status_req;
	tool_thread_broadcastCond(&dev->cond_status);
	tool_thread_unlockMutex(&dev->mutex);
	return ret; 
}

TOOL_INT32 nc_ys_dev_doSetStatus(nc_ys_dev_t* dev)
{
	TOOL_INT32 set_status_req;
	ptc_cfg_status_t cfg_status;
	tool_thread_lockMutex(&dev->mutex);
	set_status_req = dev->cur_set_status_req;
	tool_mem_memcpy(&cfg_status, &dev->cfg_set_status, sizeof(ptc_cfg_status_t));
	tool_thread_unlockMutex(&dev->mutex);
	if (dev->last_set_status_req == set_status_req)
		return 0;

	log_state("nc_ys_dev_doSetStatus io_out(%016llx)", cfg_status.io_out);
	ys_msg_getAO_c2d_t* pst_getAO_c2d = (ys_msg_getAO_c2d_t*)dev->ys_msg.data;
	ys_msg_getAO_d2c_t* pst_getAO_d2c = (ys_msg_getAO_d2c_t*)dev->ys_msg.data;
	ys_msg_setAO_c2d_t* pst_setAO_c2d = (ys_msg_setAO_c2d_t*)dev->ys_msg.data;
	ys_ie_alarmout_t st_alarmout;
	TOOL_INT32 result = 0;
	TOOL_INT32 i = 0;
	for (i = 0; i < dev->login_d2c.dev_ability.stDevInfo.u8AlarmOutPortNum; i++)
	{
		tool_mem_memset(&dev->ys_msg.head, sizeof(ys_msg_head_t));
		dev->ys_msg.head.u16Command = YS_MSG_ID_QUERY_ALARM_OUTPUT;
		dev->ys_msg.head.u32ContentLength = sizeof(ys_msg_getAO_c2d_t);
		tool_mem_memset(pst_getAO_c2d, sizeof(ys_msg_getAO_c2d_t));
		tool_ptc_ys_fillIeHead_getAO_c2d(pst_getAO_c2d);
		tool_mem_memcpy(&pst_getAO_c2d->user_log_info, &dev->login_d2c.user_log_info, sizeof(ys_ie_user_log_info_t));
		pst_getAO_c2d->resource_code.stResourceCode.u8DevType = YS_DEV_TYPE_ANALOG_ENCODER;
		pst_getAO_c2d->resource_code.stResourceCode.u8ResType = YS_RES_TYPE_ALARM_OUT_CHL;
		pst_getAO_c2d->resource_code.stResourceCode.u8ChlID = i+1;
		if (tool_ptc_ys_sendMsg(&dev->msg_sock, &dev->ys_msg) < 0)
		{
			log_error("YS_MSG_ID_QUERY_ALARM_OUTPUT send");
			return -1;
		}
		if (nc_ys_dev_recvMsg(dev, YS_MSG_ID_QUERY_ALARM_OUTPUT) < 0)
		{
			log_error("YS_MSG_ID_QUERY_ALARM_OUTPUT recv");
			return -1;
		}
		tool_mem_memcpy(&st_alarmout, &pst_getAO_d2c->pstIEAlarmOutInfo, sizeof(ys_ie_alarmout_t));

		result = cfg_status.io_out & (1<<i);
		if (result == 0)
		{
			if (st_alarmout.u8DefaultStatus == YS_AO_MODE_CLOSE)
				continue;
			 st_alarmout.u8DefaultStatus = YS_AO_MODE_CLOSE;
		}
		else
		{
			if (st_alarmout.u8DefaultStatus == YS_AO_MODE_OPEN)
				continue;
			 st_alarmout.u8DefaultStatus = YS_AO_MODE_OPEN;
		}
		log_debug("result(%d) i(%d) st_alarmout.u8DefaultStatus(%d)", result, i, st_alarmout.u8DefaultStatus);
		tool_mem_memset(&dev->ys_msg.head, sizeof(ys_msg_head_t));
		dev->ys_msg.head.u16Command = YS_MSG_ID_SET_ALARM_OUTPUT;
		dev->ys_msg.head.u32ContentLength = sizeof(ys_msg_setAO_c2d_t);
		tool_mem_memset(pst_setAO_c2d, sizeof(ys_msg_setAO_c2d_t));
		tool_ptc_ys_fillIeHead_setAO_c2d(pst_setAO_c2d);
		tool_mem_memcpy(&pst_setAO_c2d->user_log_info, &dev->login_d2c.user_log_info, sizeof(ys_ie_user_log_info_t));
		pst_setAO_c2d->resource_code.stResourceCode.u8DevType = YS_DEV_TYPE_ANALOG_ENCODER;
		pst_setAO_c2d->resource_code.stResourceCode.u8ResType = YS_RES_TYPE_ALARM_OUT_CHL;
		pst_setAO_c2d->resource_code.stResourceCode.u8ChlID = i+1;
		tool_mem_memcpy(&pst_setAO_c2d->pstIEAlarmOutInfo, &st_alarmout, sizeof(ys_ie_alarmout_t));
		if (tool_ptc_ys_sendMsg(&dev->msg_sock, &dev->ys_msg) < 0)
		{
			log_error("YS_MSG_ID_SET_ALARM_OUTPUT send");
			return -1;
		}
		if (nc_ys_dev_recvMsg(dev, YS_MSG_ID_SET_ALARM_OUTPUT) < 0)
		{
			log_error("YS_MSG_ID_SET_ALARM_OUTPUT recv");
			return -1;
		}
	}

	dev->last_set_status_req = set_status_req;
	return 0;
}

TOOL_INT32 nc_ys_dev_doRecv(nc_ys_dev_t* dev)
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
	if (tool_ptc_ys_recvMsg(&dev->msg_sock, &dev->ys_msg) < 0)
	{
		log_error("tool_ptc_ys_recvMsg dev(%d)", dev->dev_id);
		return -1;
	}
	log_state("u16Command(0d%d)", dev->ys_msg.head.u16Command);
	dev->is_sleep = 0;
	return 0;
}

TOOL_VOID* nc_ys_dev_runLogin(TOOL_VOID* param)
{
	nc_ys_dev_t* dev = (nc_ys_dev_t*)param;
	log_state("dev(%d) Login start", dev->dev_id);
	nc_ys_dev_setOnline(dev, 0);

	while (dev->state)
	{
		if (nc_ys_dev_login(dev) < 0)
			continue;
		dev->is_sleep = 1;
		if (nc_ys_dev_heartbeat(dev) < 0 ||
			nc_ys_dev_doRts(dev) < 0 ||
			nc_ys_dev_doPtz(dev) < 0 ||
			nc_ys_dev_doRecords(dev) < 0 ||
			nc_ys_dev_doLogs(dev) < 0 ||
			nc_ys_dev_doPbs(dev) < 0 ||
			nc_ys_dev_doTalk(dev) < 0 ||
			nc_ys_dev_doSyncTime(dev) < 0 ||
			nc_ys_dev_doGetStatus(dev) < 0 ||
			nc_ys_dev_doSetStatus(dev) < 0 ||
			nc_ys_dev_doRecv(dev) < 0)
		{
			nc_ys_dev_logout(dev);
			continue;
		}

		if (dev->is_sleep)
			tool_sysf_usleep(20*1000);
	}

	log_state("dev(%d) Login stop", dev->dev_id);
	nc_ys_dev_logout(dev);
    return NULL;
}


TOOL_INT32 nc_ys_dev_start(nc_ys_dev_t* dev, nc_dev* pst_nc_dev)
{
	tool_mem_memcpy(&dev->st_nc_dev, pst_nc_dev, sizeof(nc_dev));
	tool_sock_item_init(&dev->msg_sock);
	tool_sysf_setTick(&dev->state_tick_sec, &dev->state_tick_usec);	
	dev->state = 1;
	if (tool_thread_create(&dev->login_pid, NULL, nc_ys_dev_runLogin, (TOOL_VOID*)dev) < 0)
		log_fatalNo("tool_thread_create");
	return 0;	
}

TOOL_VOID nc_ys_dev_stop(nc_ys_dev_t* dev)
{
	if (dev->state == 0)
		return;
	
	dev->state = 0;
	tool_thread_join(dev->login_pid);
}

TOOL_INT32 nc_ys_dev_startRts(nc_ys_dev_t* dev, TOOL_INT32 channel_no, TOOL_INT32 stream_type, NC_CB cb, TOOL_VOID* param)
{
	TOOL_INT32 ret = 0;
	tool_thread_lockMutex(&dev->mutex);
	if (dev->cur_rts_enable[channel_no][stream_type])
	{
		log_error("nc_ys_dev_startRts repeat[%d]", channel_no);
		tool_thread_unlockMutex(&dev->mutex);
		return NC_ERROR_REPEAT;
	}
//	tool_stream3_init(&dev->rts[channel_no][stream_type].stream, NC_YS_RTS_POOL_SIZE);
//	dev->rts[channel_no][stream_type].recv_frame = (ptc_frame_video_t*)tool_mem_malloc(sizeof(ptc_frame_video_t), 0);
//	dev->rts[channel_no][stream_type].cb_frame   = (ptc_frame_video_t*)tool_mem_malloc(sizeof(ptc_frame_video_t), 0);
	dev->rts[channel_no][stream_type].cb = cb;
	dev->rts[channel_no][stream_type].param = param;
	dev->cur_rts_enable[channel_no][stream_type] = 1;
//	tool_stream3_initReadPos(&dev->rts[channel_no][stream_type].stream, &dev->rts[channel_no][stream_type].read_pos);
//	if (tool_thread_create(&dev->rts_pid[channel_no][stream_type], NULL, nc_ys_rts_runPlay, (TOOL_VOID*)&dev->rts[channel_no][stream_type]) < 0)
//		log_fatalNo("tool_thread_create");
//	if (tool_thread_create(&dev->recv_rts_pid[channel_no][stream_type], NULL, nc_ys_rts_runRecv, (TOOL_VOID*)&dev->rts[channel_no][stream_type]) < 0)
//		log_fatalNo("tool_thread_create");
	ret = (dev->st_nc_dev.ptc | (dev->dev_id<<8) | (channel_no<<2) | stream_type);
	tool_sysf_setTick(&dev->state_tick_sec, &dev->state_tick_usec);
	tool_thread_unlockMutex(&dev->mutex);
	log_state("nc_ys_dev_startRts[%d][%d]", channel_no, stream_type);

	return ret;
}

TOOL_INT32 nc_ys_dev_stopRts(nc_ys_dev_t* dev, TOOL_INT32 channel_no, TOOL_INT32 stream_type)
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
//		tool_mem_free(dev->rts[channel_no][stream_type].recv_frame);
//		dev->rts[channel_no][stream_type].recv_frame = NULL;
//		tool_mem_free(dev->rts[channel_no][stream_type].cb_frame);
//		dev->rts[channel_no][stream_type].cb_frame = NULL;
//		tool_stream3_done(&dev->rts[channel_no][stream_type].stream);
	}
	
	tool_sysf_setTick(&dev->state_tick_sec, &dev->state_tick_usec);
	tool_thread_unlockMutex(&dev->mutex);
	return ret;
}

TOOL_INT32 nc_ys_dev_ptz(nc_ys_dev_t* dev, TOOL_INT32 channel_no, PTC_PTZ_CMD_E cmd, TOOL_INT32 param)
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

TOOL_INT32 nc_ys_dev_searchRecords(nc_ys_dev_t* dev, ptc_cfg_records_t* records)
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

TOOL_INT32 nc_ys_dev_searchLogs(nc_ys_dev_t* dev, ptc_logs_t* logs)
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

TOOL_INT32 nc_ys_dev_startPbs(nc_ys_dev_t* dev, ptc_cfg_record_t* record, NC_CB cb, TOOL_VOID* param)
{
	TOOL_INT32 ret = 0;
	tool_thread_lockMutex(&dev->mutex);
	if (dev->cur_pbs_enable)
	{
		log_error("nc_ys_dev_startPbs repeat");
		tool_thread_unlockMutex(&dev->mutex);
		return NC_ERROR_REPEAT;
	}
	tool_mem_memcpy(&dev->pbs.ptc_record, record, sizeof(ptc_cfg_record_t));
//	tool_stream3_init(&dev->pbs.stream, NC_YS_PBS_POOL_SIZE);
//	dev->pbs.recv_frame = (ptc_frame_video_t*)tool_mem_malloc(PTC_VIDEO_FRAME_SIZE, 0);
//	dev->pbs.cb_frame   = (ptc_frame_video_t*)tool_mem_malloc(PTC_VIDEO_FRAME_SIZE, 0);
	dev->pbs.cb = cb;
	dev->pbs.param = param;
	dev->cur_pbs_enable = 1;
//	tool_stream3_initReadPos(&dev->pbs.stream, &dev->pbs.read_pos);
//	if (tool_thread_create(&dev->pbs_pid, NULL, nc_ys_pbs_runPlay, (TOOL_VOID*)&dev->pbs) < 0)
//		log_fatalNo("tool_thread_create");
//	if (tool_thread_create(&dev->recv_pbs_pid, NULL, nc_ys_pbs_runRecv, (TOOL_VOID*)&dev->pbs) < 0)
//		log_fatalNo("tool_thread_create");	
	ret = (dev->st_nc_dev.ptc | (dev->dev_id<<8) | dev->pbs.pbs_no);
	tool_sysf_setTick(&dev->state_tick_sec, &dev->state_tick_usec);
	tool_thread_unlockMutex(&dev->mutex);
	log_state("nc_ys_dev_startPbs");
	return ret;
}

TOOL_INT32 nc_ys_dev_stopPbs(nc_ys_dev_t* dev)
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
	
	tool_sysf_setTick(&dev->state_tick_sec, &dev->state_tick_usec);
	tool_thread_unlockMutex(&dev->mutex);
	return ret;
}


TOOL_INT32 nc_ys_dev_startTalk(nc_ys_dev_t* dev, NC_CB cb, TOOL_VOID* param)
{
	TOOL_INT32 ret = 0;
	tool_thread_lockMutex(&dev->mutex);
	if (dev->cur_talk_enable)
	{
		log_error("nc_ys_dev_startTalk repeat");
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
	log_state("nc_ys_dev_startTalk");
	return ret;

	
/*	
	TOOL_INT32 ret = 0;
	tool_thread_lockMutex(&dev->mutex);
	if (dev->cur_talk_enable)
	{
		log_error("nc_ys_dev_startTalk repeat");
		tool_thread_unlockMutex(&dev->mutex);
		return NC_ERROR_REPEAT;
	}

	dev->talk.send_stream = (tool_stream2_t*)tool_mem_malloc(sizeof(tool_stream2_t), 0);
	tool_stream2_init(dev->talk.send_stream);
	tool_stream2_initReadPos(dev->talk.send_stream, &dev->talk.send_pos);
	dev->talk.cb = cb;
	dev->talk.param = param;
	dev->cur_talk_enable = 1;
	if (tool_thread_create(&dev->talk_recv_pid, NULL, nc_ys_talk_runRecv, (TOOL_VOID*)&dev->talk) < 0)
			log_fatalNo("tool_thread_create");
	if (tool_thread_create(&dev->talk_send_pid, NULL, nc_ys_talk_runSend, (TOOL_VOID*)&dev->talk) < 0)
			log_fatalNo("tool_thread_create");
	ret = (dev->st_nc_dev.ptc | (dev->dev_id<<8) | dev->talk.talk_no);
	tool_sysf_setTick(&dev->state_tick_sec, &dev->state_tick_usec);
	tool_thread_unlockMutex(&dev->mutex);
	log_state("nc_ys_dev_startTalk");
	return ret;
*/	
}

TOOL_INT32 nc_ys_dev_stopTalk(nc_ys_dev_t* dev)
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

TOOL_INT32 nc_ys_dev_syncTime(nc_ys_dev_t* dev, TOOL_UINT32 utc_sec, TOOL_INT32 zone, TOOL_INT32 dst_hour)
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

TOOL_INT32 nc_ys_dev_getStatus(nc_ys_dev_t* dev, ptc_cfg_status_t* cfg_status)
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

TOOL_INT32 nc_ys_dev_setStatus(nc_ys_dev_t* dev, ptc_cfg_status_t* cfg_status)
{
	tool_thread_lockMutex(&dev->mutex);
	dev->cur_set_status_req ++;
	tool_mem_memcpy(&dev->cfg_set_status, cfg_status, sizeof(ptc_cfg_status_t));
	tool_thread_unlockMutex(&dev->mutex);
	return 0;
}

TOOL_INT32 nc_ys_dev_isSame(nc_dev* pst_nc_dev, nc_ys_dev_t* pst_ys_dev)
{
	if (pst_nc_dev->ptc == NC_PTC_YuShi_Cloud && pst_ys_dev->st_nc_dev.ptc == NC_PTC_YuShi_Cloud)
	{
		if (tool_str_strncmp(pst_ys_dev->st_nc_dev.dev_id, pst_nc_dev->dev_id, PTC_STR_SIZE-1) == 0 &&
			tool_str_strncmp(pst_ys_dev->st_nc_dev.user, pst_nc_dev->user, PTC_ID_SIZE-1) == 0 &&
			tool_str_strncmp(pst_ys_dev->st_nc_dev.pswd, pst_nc_dev->pswd, PTC_ID_SIZE-1) == 0 &&			
			pst_ys_dev->state)
			return 1;
		return 0;
	}
	
	if (tool_str_strncmp(pst_ys_dev->st_nc_dev.ip, pst_nc_dev->ip, PTC_IP_SIZE-1) == 0 &&
		tool_str_strncmp(pst_ys_dev->st_nc_dev.user, pst_nc_dev->user, PTC_ID_SIZE-1) == 0 &&
		tool_str_strncmp(pst_ys_dev->st_nc_dev.pswd, pst_nc_dev->pswd, PTC_ID_SIZE-1) == 0 &&		
		pst_ys_dev->st_nc_dev.msg_port== pst_nc_dev->msg_port &&
		pst_ys_dev->state)
		return 1;
	return 0;
}

#define __NC_YS_INFO__

TOOL_VOID* nc_ys_run(TOOL_VOID* param)
{
	TOOL_INT32 i = 0;
	TOOL_INT32 j = 0;
	TOOL_INT32 k = 0;
	TOOL_INT32 ret = 0;
	TOOL_INT32 count = 0;
	nc_ys_t* nc = (nc_ys_t*)param;
	nc_ys_dev_t* dev = NULL;
	while (nc->state)
	{
		count = 0;
		tool_thread_lockMutex(&nc->mutex);
		for (i = 0; i < NC_YS_USER_SIZE; i++)
		{
			dev = &nc->dev[i];
			if (dev->state == 0)
				continue;
			
			ret = 0;
			for (j = 0; j < NC_YS_CHANNEL_SIZE; j++)
			{
				for (k = 0; k < NC_YS_STREAM_SIZE; k++)
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
				nc_ys_dev_stop(dev);
			}
			else
			{
				count ++;
			}
		}
		tool_thread_unlockMutex(&nc->mutex);
	//	log_debug("nc_ys_run count(%d)", count);
		tool_sysf_sleep(1);
	}
	return NULL;
}

TOOL_VOID nc_ys_init()
{
	if (g_nc_ys)
		return ;
	log_state("init ys sdk(v%04x.%04x)", PTC_VERSION, PTC_DATA);
	g_nc_ys = (nc_ys_t*)tool_mem_malloc(sizeof(nc_ys_t), 1);
    tool_thread_initMutex(&g_nc_ys->mutex);
	g_nc_ys->buffer_usec = 300*1000;
	TOOL_INT32 i = 0;
	for (i = 0; i < NC_YS_USER_SIZE; i++)
		nc_ys_dev_init(&g_nc_ys->dev[i], i, g_nc_ys);
	g_nc_ys->state = 1;
	if (tool_thread_create(&g_nc_ys->pid, NULL, nc_ys_run, (TOOL_VOID*)g_nc_ys) < 0)
		log_fatal("tool_thread_create");
}

TOOL_INT32 nc_ys_setBuffer(TOOL_INT32 buffer_usec)
{
	if (g_nc_ys == NULL)
	{
		log_error("nc");
		return NC_ERROR_UNINIT;
	}
	if (buffer_usec < 0 || buffer_usec > 3000*1000)
	{
		log_error("buffer_usec(%d)", buffer_usec);
		return NC_ERROR_INPUT;
	}
	g_nc_ys->buffer_usec = buffer_usec;
	return 0;
}

TOOL_VOID nc_ys_done()
{
    if (g_nc_ys == NULL)
        return;
	g_nc_ys->state = 0;
	tool_thread_join(g_nc_ys->pid);
    log_state("done ys sdk(v%04x.%04x)", PTC_VERSION, PTC_DATA);
	TOOL_INT32 i = 0;
	for (i = 0; i < NC_YS_USER_SIZE; i++)
		nc_ys_dev_done(&g_nc_ys->dev[i]);
	tool_thread_doneMutex(&g_nc_ys->mutex);
	tool_mem_free(g_nc_ys);
	g_nc_ys = NULL;
	tool_sock_done();
}

#include <stdio.h>


TOOL_INT32 nc_ys_searchDev(TOOL_VOID* param)
{
	if (g_nc_ys == NULL)
	{
		log_error("nc");
		return NC_ERROR_UNINIT;
	}
	ptc_mcast_searchs_t* searchs = (ptc_mcast_searchs_t*)param;
	if (searchs == NULL)
	{
		log_error("nc_ys_searchDev arg");
		return NC_ERROR_INPUT;
	}		
	searchs->num = 0;

	TOOL_INT8 uuid1[40] = {0};
	TOOL_INT8 uuid2[40] = {0};
	tool_ran_uuid(uuid1);
	tool_ran_uuid(uuid2);
//	log_debug("uuid1(%s)", uuid1);
//	log_debug("uuid2(%s)", uuid2);

	TOOL_INT32 tick_sec = 0;
	TOOL_INT32 tick_usec = 0;
	TOOL_INT32 ret = 0;
	TOOL_INT32 result = -1;
	TOOL_INT8 send_buf[4096] = {0};
	snprintf(send_buf, sizeof(send_buf),
		"<?xml version=\"1.0\" encoding=\"utf-8\"?>"
		"<Envelope xmlns:tds=\"http://www.onvif.org/ver10/device/wsdl\" xmlns=\"http://www.w3.org/2003/05/soap-envelope\">"
			"<Header>"
				"<wsa:MessageID xmlns:wsa=\"http://schemas.xmlsoap.org/ws/2004/08/addressing\">"
					"uuid:%s"
				"</wsa:MessageID>"
				"<wsa:To xmlns:wsa=\"http://schemas.xmlsoap.org/ws/2004/08/addressing\">"
					"urn:schemas-xmlsoap-org:ws:2005:04:discovery"
				"</wsa:To>"
				"<wsa:Action xmlns:wsa=\"http://schemas.xmlsoap.org/ws/2004/08/addressing\">"
					"http://schemas.xmlsoap.org/ws/2005/04/discovery/Probe"
				"</wsa:Action>"
			"</Header>"
			"<Body>"
				"<Probe xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\" xmlns:xsd=\"http://www.w3.org/2001/XMLSchema\" xmlns=\"http://schemas.xmlsoap.org/ws/2005/04/discovery\">"
					"<Types>tds:Device</Types>"
					"<Scopes />"
				"</Probe>"
			"</Body>"
		"</Envelope>"
		"<?xml version=\"1.0\" encoding=\"utf-8\"?>"
		"<Envelope xmlns:dn=\"http://www.onvif.org/ver10/network/wsdl\" xmlns=\"http://www.w3.org/2003/05/soap-envelope\">"
			"<Header>"
				"<wsa:MessageID xmlns:wsa=\"http://schemas.xmlsoap.org/ws/2004/08/addressing\">"
					"uuid:%s"
				"</wsa:MessageID>"
				"<wsa:To xmlns:wsa=\"http://schemas.xmlsoap.org/ws/2004/08/addressing\">"
					"urn:schemas-xmlsoap-org:ws:2005:04:discovery"
				"</wsa:To>"
				"<wsa:Action xmlns:wsa=\"http://schemas.xmlsoap.org/ws/2004/08/addressing\">"
					"http://schemas.xmlsoap.org/ws/2005/04/discovery/Probe"
				"</wsa:Action>"
			"</Header>"
			"<Body>"
				"<Probe xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\" xmlns:xsd=\"http://www.w3.org/2001/XMLSchema\" xmlns=\"http://schemas.xmlsoap.org/ws/2005/04/discovery\">"
					"<Types>dn:NetworkVideoTransmitter</Types>"
					"<Scopes />"
				"</Probe>"
			"</Body>"
		"</Envelope>", uuid1, uuid2);
//	log_debug("send_buf(%s)", send_buf);
		
	TOOL_INT8 recv_buf[4096] = {0};
	tool_sock_item_t sock_s2c;
	do
	{
		if (tool_sock_item_open_v2(&sock_s2c, TOOL_SOCK_TYPE1_MCAST, TOOL_SOCK_TYPE2_NONE, YS_MCAST_IP, YS_MCAST_PORT, NULL, YS_MCAST_PORT) < 0)
		{
			log_error("tool_sock_createMcast sock_s2c");
			result = NC_ERROR_INTERNAL;
			break;
		}

		if (tool_sock_item_sendto(&sock_s2c, send_buf, tool_str_strlen(send_buf)) < 0)
		{
			log_error("tool_sock_item_sendto");
			result = NC_ERROR_SEND;
			break;
		}

		TOOL_INT8* index = NULL;
		TOOL_INT32 ip_part0 = 0;
		TOOL_INT32 ip_part1 = 0;
		TOOL_INT32 ip_part2 = 0;
		TOOL_INT32 ip_part3 = 0;
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

			tool_mem_memset(recv_buf, sizeof(recv_buf));
			if (tool_sock_item_recvfrom(&sock_s2c, recv_buf, sizeof(recv_buf)) < 0)
			{
				log_error("tool_sock_item_sendto");
				break;
			}
	
//			log_debug("recv_buf(%s)", recv_buf);
			if (tool_str_strstr(recv_buf, "onvif://www.onvif.org/name/NVR") == NULL)
				continue;
			index = tool_str_strstr(recv_buf, "XAddrs>http://");
			if (index == NULL)
			{
				log_error("recv_buf(%s)", recv_buf);
				continue;
			}

			ret = sscanf(index, "XAddrs>http://%d.%d.%d.%d", &ip_part0, &ip_part1, &ip_part2, &ip_part3);
			if (ret != 4)
			{
				log_error("ret(%d) index(%s)", ret, index);
				continue;
			}
//			log_debug("%d.%d.%d.%d", ip_part0, ip_part1, ip_part2, ip_part3);
			tool_mem_memset(&searchs->search[searchs->num], sizeof(ptc_mcast_search_t));
			searchs->search[searchs->num].net.ip1[0] = ip_part0;
			searchs->search[searchs->num].net.ip1[1] = ip_part1;
			searchs->search[searchs->num].net.ip1[2] = ip_part2;
			searchs->search[searchs->num].net.ip1[3] = ip_part3;
			searchs->search[searchs->num].net.web_port = 80;
			searchs->search[searchs->num].net.dev_port = YS_MSG_PORT;
			searchs->search[searchs->num].net.stream_port = YS_STREAM_PORT;
			searchs->search[searchs->num].nc_ptc_type = NC_PTC_YuShi;
			searchs->num ++;
			
		}
		result = 0;		
	}while (0);
	tool_sock_item_close(&sock_s2c);

	return result;
}

TOOL_INT32 nc_ys_getChannelNum_v2(nc_dev* pst_nc_dev)
{
	if (g_nc_ys == NULL)
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
	nc_ys_dev_t dev;
	tool_mem_memset(&dev, sizeof(nc_ys_dev_t));
	tool_mem_memcpy(&dev.st_nc_dev, pst_nc_dev, sizeof(nc_dev));
	dev.state = 1;
	tool_sock_item_init(&dev.msg_sock);
	ret = nc_ys_dev_login(&dev);
	if (ret < 0)
	{
		dev.state = 0;
		return ret;
	}
	ch_num = dev.login_d2c.dev_ability.stDevInfo.u8IPChanNum;
	nc_ys_dev_logout(&dev);
	dev.state = 0;
	return ch_num;
}

TOOL_INT32 nc_ys_startRts_v2(nc_dev* info, TOOL_INT32 channel, TOOL_INT32 stream_type, NC_CB cb, void* param)
{
	if (g_nc_ys == NULL)
	{
		log_error("nc");
		return NC_ERROR_UNINIT;
	}
	if (info == NULL || channel < 0 || channel >= NC_YS_CHANNEL_SIZE || stream_type < 0 || stream_type >= NC_YS_STREAM_SIZE)
	{
		log_error("info(0x%08x), channel(%d), stream_type(%d)", info, channel, stream_type);
		return NC_ERROR_INPUT;
	}

	TOOL_INT32 i = 0;
	TOOL_INT32 ret = 0;
	tool_thread_lockMutex(&g_nc_ys->mutex);
	for (i = 0; i < NC_YS_USER_SIZE; i++)
	{
		if (nc_ys_dev_isSame(info, &g_nc_ys->dev[i]))
		{
			ret = nc_ys_dev_startRts(&g_nc_ys->dev[i], channel, stream_type, cb, param);
			tool_thread_unlockMutex(&g_nc_ys->mutex);
			return ret;
		}
	}
	for (i = 0; i < NC_YS_USER_SIZE; i++)
	{
		if (g_nc_ys->dev[i].state == 0)
			break;
	}
	if (i == NC_YS_USER_SIZE)
	{
		log_error("nc full");
		tool_thread_unlockMutex(&g_nc_ys->mutex);
		return NC_ERROR_FULL;
	}
	
	ret = nc_ys_dev_start(&g_nc_ys->dev[i], info);
	if (ret < 0)
	{
		log_error("nc_ys_dev_start id(%s), addr(%s:%d)", info->dev_id, info->ip, info->msg_port);
		tool_thread_unlockMutex(&g_nc_ys->mutex);
		return ret;
	}
	ret = nc_ys_dev_startRts(&g_nc_ys->dev[i], channel, stream_type, cb, param);
	if (ret < 0)
	{
		log_error("nc_ys_dev_startRts[%d][%d]", channel, stream_type);
		nc_ys_dev_stop(&g_nc_ys->dev[i]);
	}
	tool_thread_unlockMutex(&g_nc_ys->mutex);
	return ret;
}

TOOL_INT32 nc_ys_stopRts(TOOL_INT32 id)
{
	if (g_nc_ys == NULL)
	{
		log_error("nc");
		return NC_ERROR_UNINIT;
	}
	TOOL_INT32 dev_id = (id >> 8) & 0xFF;
	TOOL_INT32 channel_no = (id>>2) & 0x3F;
	TOOL_INT32 stream_type = id & 0x3;
	if (dev_id < 0 || dev_id >= NC_YS_USER_SIZE || channel_no < 0 || channel_no >= NC_YS_CHANNEL_SIZE ||
		stream_type < 0 || stream_type >= NC_YS_STREAM_SIZE)
	{
		log_error("id(0x%08x)", id);
		return NC_ERROR_INPUT;
	}
	tool_thread_lockMutex(&g_nc_ys->mutex);
	nc_ys_dev_t* dev = (nc_ys_dev_t*)&g_nc_ys->dev[dev_id];
	nc_ys_dev_stopRts(dev, channel_no, stream_type);
	tool_thread_unlockMutex(&g_nc_ys->mutex);
	return 0;
}

TOOL_INT32 nc_ys_controlPTZ_v2(nc_dev* info, TOOL_INT32 channel, PTC_PTZ_CMD_E cmd, TOOL_INT32 param)
{
	if (g_nc_ys == NULL)
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
	tool_thread_lockMutex(&g_nc_ys->mutex);
	for (i = 0; i < NC_YS_USER_SIZE; i++)
	{
		if (nc_ys_dev_isSame(info, &g_nc_ys->dev[i]))
		{
			break;
		}
	}
	if (i == NC_YS_USER_SIZE)
	{
		tool_thread_unlockMutex(&g_nc_ys->mutex);
		return NC_ERROR_PTZNORTS;
	}
	ret = nc_ys_dev_ptz(&g_nc_ys->dev[i], channel, cmd, param);
	tool_thread_unlockMutex(&g_nc_ys->mutex);
	return ret;
}

TOOL_INT32 nc_ys_searchRecords(nc_dev* info, ptc_cfg_records_t* records)
{
	if (g_nc_ys == NULL)
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
	tool_thread_lockMutex(&g_nc_ys->mutex);
	for (i = 0; i < NC_YS_USER_SIZE; i++)
	{
		if (nc_ys_dev_isSame(info, &g_nc_ys->dev[i]))
		{
			ret = nc_ys_dev_searchRecords(&g_nc_ys->dev[i], records);
			tool_thread_unlockMutex(&g_nc_ys->mutex);
			return ret;
		}
	}
	for (i = 0; i < NC_YS_USER_SIZE; i++)
	{
		if (g_nc_ys->dev[i].state == 0)
			break;
	}
	if (i == NC_YS_USER_SIZE)
	{
		log_error("nc full");
		tool_thread_unlockMutex(&g_nc_ys->mutex);
		return NC_ERROR_FULL;
	}
	
	ret = nc_ys_dev_start(&g_nc_ys->dev[i], info);
	if (ret < 0)
	{
		log_error("nc_ys_dev_start id(%s), addr(%s:%d)", info->dev_id, info->ip, info->msg_port);
		tool_thread_unlockMutex(&g_nc_ys->mutex);
		return ret;
	}
	ret = nc_ys_dev_searchRecords(&g_nc_ys->dev[i], records);
	if (ret < 0)
	{
		log_error("nc_ys_dev_searchRecords");
		nc_ys_dev_stop(&g_nc_ys->dev[i]);
	}
	tool_thread_unlockMutex(&g_nc_ys->mutex);
	return ret;	
}

TOOL_INT32 nc_ys_startPbs(nc_dev* info, ptc_cfg_record_t* ptc_record, NC_CB cb, void* param)
{
	if (g_nc_ys == NULL)
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
	tool_thread_lockMutex(&g_nc_ys->mutex);
	for (i = 0; i < NC_YS_USER_SIZE; i++)
	{
		if (nc_ys_dev_isSame(info, &g_nc_ys->dev[i]))
		{
			ret = nc_ys_dev_startPbs(&g_nc_ys->dev[i], ptc_record, cb, param);
			tool_thread_unlockMutex(&g_nc_ys->mutex);
			return ret;
		}
	}
	for (i = 0; i < NC_YS_USER_SIZE; i++)
	{
		if (g_nc_ys->dev[i].state == 0)
			break;
	}
	if (i == NC_YS_USER_SIZE)
	{
		log_error("nc full");
		tool_thread_unlockMutex(&g_nc_ys->mutex);
		return NC_ERROR_FULL;
	}
	
	ret = nc_ys_dev_start(&g_nc_ys->dev[i], info);
	if (ret < 0)
	{
		log_error("nc_ys_dev_start id(%s), addr(%s:%d)", info->dev_id, info->ip, info->msg_port);
		tool_thread_unlockMutex(&g_nc_ys->mutex);
		return ret;
	}
	ret = nc_ys_dev_startPbs(&g_nc_ys->dev[i], ptc_record, cb, param);
	if (ret < 0)
	{
		log_error("nc_ys_dev_startPbs");
		nc_ys_dev_stop(&g_nc_ys->dev[i]);
	}
	tool_thread_unlockMutex(&g_nc_ys->mutex);
	return ret;
}

TOOL_INT32 nc_ys_stopPbs(TOOL_INT32 id)
{
	if (g_nc_ys == NULL)
	{
		log_error("nc");
		return NC_ERROR_UNINIT;
	}
	TOOL_INT32 dev_id = (id >> 8) & 0xFF;
	TOOL_INT32 pbs_no = (id) & 0xFF;
	if (dev_id < 0 || dev_id >= NC_YS_USER_SIZE || pbs_no != NC_YS_CHANNEL_SIZE)
	{
		log_error("id(0x%08x)", id);
		return NC_ERROR_INPUT;
	}
	tool_thread_lockMutex(&g_nc_ys->mutex);
	nc_ys_dev_t* dev = (nc_ys_dev_t*)&g_nc_ys->dev[dev_id];
	nc_ys_dev_stopPbs(dev);
	tool_thread_unlockMutex(&g_nc_ys->mutex);
	return 0;	
}

TOOL_INT32 nc_ys_searchLogs(nc_dev* info, ptc_logs_t* logs)
{
	if (g_nc_ys == NULL)
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
	tool_thread_lockMutex(&g_nc_ys->mutex);
	for (i = 0; i < NC_YS_USER_SIZE; i++)
	{
		if (nc_ys_dev_isSame(info, &g_nc_ys->dev[i]))
		{
			ret = nc_ys_dev_searchLogs(&g_nc_ys->dev[i], logs);
			tool_thread_unlockMutex(&g_nc_ys->mutex);
			return ret;
		}
	}
	for (i = 0; i < NC_YS_USER_SIZE; i++)
	{
		if (g_nc_ys->dev[i].state == 0)
			break;
	}
	if (i == NC_YS_USER_SIZE)
	{
		log_error("nc full");
		tool_thread_unlockMutex(&g_nc_ys->mutex);
		return NC_ERROR_FULL;
	}
	
	ret = nc_ys_dev_start(&g_nc_ys->dev[i], info);
	if (ret < 0)
	{
		log_error("nc_ys_dev_start id(%s), addr(%s:%d)", info->dev_id, info->ip, info->msg_port);
		tool_thread_unlockMutex(&g_nc_ys->mutex);
		return ret;
	}
	ret = nc_ys_dev_searchLogs(&g_nc_ys->dev[i], logs);
	if (ret < 0)
	{
		log_error("nc_ys_dev_searchLogs");
		nc_ys_dev_stop(&g_nc_ys->dev[i]);
	}
	tool_thread_unlockMutex(&g_nc_ys->mutex);
	return ret;	
}

TOOL_INT32 nc_ys_startTalk_v2(nc_dev* info, NC_CB cb, void* param)
{
	if (g_nc_ys == NULL)
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
	tool_thread_lockMutex(&g_nc_ys->mutex);
	for (i = 0; i < NC_YS_USER_SIZE; i++)
	{
		if (nc_ys_dev_isSame(info, &g_nc_ys->dev[i]))
		{
			ret = nc_ys_dev_startTalk(&g_nc_ys->dev[i], cb, param);
			tool_thread_unlockMutex(&g_nc_ys->mutex);
			return ret;
		}
	}
	for (i = 0; i < NC_YS_USER_SIZE; i++)
	{
		if (g_nc_ys->dev[i].state == 0)
			break;
	}
	if (i == NC_YS_USER_SIZE)
	{
		log_error("nc full");
		tool_thread_unlockMutex(&g_nc_ys->mutex);
		return NC_ERROR_FULL;
	}
	
	ret = nc_ys_dev_start(&g_nc_ys->dev[i], info);
	if (ret < 0)
	{
		log_error("nc_ys_dev_start id(%s), addr(%s:%d)", info->dev_id, info->ip, info->msg_port);
		tool_thread_unlockMutex(&g_nc_ys->mutex);
		return ret;
	}
	ret = nc_ys_dev_startTalk(&g_nc_ys->dev[i], cb, param);
	if (ret < 0)
	{
		log_error("nc_ys_dev_startTalk");
		nc_ys_dev_stop(&g_nc_ys->dev[i]);
	}
	tool_thread_unlockMutex(&g_nc_ys->mutex);
	return ret;

}

TOOL_INT32 nc_ys_stopTalk(TOOL_INT32 id)
{
	if (g_nc_ys == NULL)
	{
		log_error("nc");
		return NC_ERROR_UNINIT;
	}
	TOOL_INT32 dev_id = (id >> 8) & 0xFF;
	TOOL_INT32 talk_no = (id) & 0xFF;
	if (dev_id < 0 || dev_id >= NC_YS_USER_SIZE || talk_no != 2*NC_YS_CHANNEL_SIZE)
	{
		log_error("id(0x%08x)", id);
		return NC_ERROR_INPUT;
	}
	tool_thread_lockMutex(&g_nc_ys->mutex);
	nc_ys_dev_t* dev = (nc_ys_dev_t*)&g_nc_ys->dev[dev_id];
	nc_ys_dev_stopTalk(dev);
	tool_thread_unlockMutex(&g_nc_ys->mutex);
	return 0;
}

TOOL_INT32 nc_ys_sendTalk(TOOL_INT32 id, TOOL_UINT8* buf, TOOL_INT32 len)
{
	if (g_nc_ys == NULL)
	{
		log_error("nc");
		return NC_ERROR_UNINIT;
	}
	TOOL_INT32 dev_id = (id >> 8) & 0xFF;
	TOOL_INT32 talk_no = (id) & 0xFF;
	if (dev_id < 0 || dev_id >= NC_YS_USER_SIZE || talk_no != 2*NC_YS_CHANNEL_SIZE || buf == NULL || len != YS_TALK_FRAME_SIZE+sizeof(ptc_frame_head_t))
	{
		log_error("id(0x%08x) buf(0x%x) len(%d)", id, buf, len);
		return NC_ERROR_INPUT;
	}
	if (g_nc_ys->dev[dev_id].talk.state && g_nc_ys->dev[dev_id].talk.send_stream)
		tool_stream2_set(g_nc_ys->dev[dev_id].talk.send_stream, buf, len);
	return 0;
}

TOOL_INT32 nc_ys_getAbility_v2(nc_dev* pst_nc_dev, ptc_cfg_ability_t* ability)
{
	if (g_nc_ys == NULL)
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
	nc_ys_dev_t dev;
	tool_mem_memset(&dev, sizeof(nc_ys_dev_t));
	tool_mem_memcpy(&dev.st_nc_dev, pst_nc_dev, sizeof(nc_dev));
	dev.state = 1;
	tool_sock_item_init(&dev.msg_sock);
	ret = nc_ys_dev_login(&dev);
	if (ret < 0)
	{
		dev.state = 0;
		return ret;
	}
	tool_mem_memset(ability, sizeof(ptc_cfg_ability_t));
	ability->video_input_num = dev.login_d2c.dev_ability.stDevInfo.u8IPChanNum;
	ability->audio_input_num = dev.login_d2c.dev_ability.stDevInfo.u8AudioInNum;
	ability->alarm_input_num = dev.login_d2c.dev_ability.stDevInfo.u8AlarmInPortNum;
	ability->alarm_output_num = dev.login_d2c.dev_ability.stDevInfo.u8AlarmOutPortNum;
	ability->base.video_input_num = dev.login_d2c.dev_ability.stDevInfo.u8IPChanNum;
	ability->base.audio_input_num = dev.login_d2c.dev_ability.stDevInfo.u8AudioInNum;
	ability->base.alarm_input_num = dev.login_d2c.dev_ability.stDevInfo.u8AlarmInPortNum;
	ability->base.alarm_output_num = dev.login_d2c.dev_ability.stDevInfo.u8AlarmOutPortNum;
	ability->base.device_type = PTC_DEVTYPE_YUSHI_BASE | dev.login_d2c.dev_ability.stDevInfo.u8DevType;
	ability->audio_ability.enable = 1;
	ability->audio_ability.sample_rate = PTC_AUDIO_RATE_8K;
	ability->audio_ability.bit_width = PTC_AUDIO_WIDTH_16BIT;
	ability->audio_ability.encode_type = PTC_AUDIO_ENCODE_G711U;
	ability->audio_ability.frame_size = 320;
	nc_ys_dev_logout(&dev);
	dev.state = 0;
	return 0;	
}

TOOL_INT32 nc_ys_syncTime(nc_dev* info, TOOL_UINT32 utc_sec, TOOL_INT32 zone, TOOL_INT32 dst_hour)
{
	if (g_nc_ys == NULL)
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
	tool_thread_lockMutex(&g_nc_ys->mutex);
	for (i = 0; i < NC_YS_USER_SIZE; i++)
	{
		if (nc_ys_dev_isSame(info, &g_nc_ys->dev[i]))
		{
			ret = nc_ys_dev_syncTime(&g_nc_ys->dev[i], utc_sec, zone, dst_hour);
			tool_thread_unlockMutex(&g_nc_ys->mutex);
			return ret;
		}
	}
	for (i = 0; i < NC_YS_USER_SIZE; i++)
	{
		if (g_nc_ys->dev[i].state == 0)
			break;
	}
	if (i == NC_YS_USER_SIZE)
	{
		log_error("nc full");
		tool_thread_unlockMutex(&g_nc_ys->mutex);
		return NC_ERROR_FULL;
	}
	
	ret = nc_ys_dev_start(&g_nc_ys->dev[i], info);
	if (ret < 0)
	{
		log_error("nc_ys_dev_start id(%s), addr(%s:%d)", info->dev_id, info->ip, info->msg_port);
		tool_thread_unlockMutex(&g_nc_ys->mutex);
		return ret;
	}
	ret = nc_ys_dev_syncTime(&g_nc_ys->dev[i], utc_sec, zone, dst_hour);
	if (ret < 0)
	{
		log_error("nc_ys_dev_syncTime");
		nc_ys_dev_stop(&g_nc_ys->dev[i]);
	}
	tool_thread_unlockMutex(&g_nc_ys->mutex);
	return ret;

}


TOOL_INT32 nc_ys_getDeviceStatus_v2(nc_dev* info, ptc_cfg_status_t* status)
{
	if (g_nc_ys == NULL)
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
	tool_thread_lockMutex(&g_nc_ys->mutex);
	for (i = 0; i < NC_YS_USER_SIZE; i++)
	{
		if (nc_ys_dev_isSame(info, &g_nc_ys->dev[i]))
		{
			ret = nc_ys_dev_getStatus(&g_nc_ys->dev[i], status);
			tool_thread_unlockMutex(&g_nc_ys->mutex);
			return ret;
		}
	}
	for (i = 0; i < NC_YS_USER_SIZE; i++)
	{
		if (g_nc_ys->dev[i].state == 0)
			break;
	}
	if (i == NC_YS_USER_SIZE)
	{
		log_error("nc full");
		tool_thread_unlockMutex(&g_nc_ys->mutex);
		return NC_ERROR_FULL;
	}
	
	ret = nc_ys_dev_start(&g_nc_ys->dev[i], info);
	if (ret < 0)
	{
		log_error("nc_ys_dev_start id(%s), addr(%s:%d)", info->dev_id, info->ip, info->msg_port);
		tool_thread_unlockMutex(&g_nc_ys->mutex);
		return ret;
	}
	ret = nc_ys_dev_getStatus(&g_nc_ys->dev[i], status);
	if (ret < 0)
	{
		log_error("nc_ys_dev_getStatus ret(%d)", ret);
		nc_ys_dev_stop(&g_nc_ys->dev[i]);
	}
	tool_thread_unlockMutex(&g_nc_ys->mutex);
	return ret;		
}

TOOL_INT32 nc_ys_setDeviceStatus_v2(nc_dev* info, ptc_cfg_status_t* status)
{
	if (g_nc_ys == NULL)
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
	tool_thread_lockMutex(&g_nc_ys->mutex);
	for (i = 0; i < NC_YS_USER_SIZE; i++)
	{
		if (nc_ys_dev_isSame(info, &g_nc_ys->dev[i]))
		{
			ret = nc_ys_dev_setStatus(&g_nc_ys->dev[i], status);
			tool_thread_unlockMutex(&g_nc_ys->mutex);
			return ret;
		}
	}
	for (i = 0; i < NC_YS_USER_SIZE; i++)
	{
		if (g_nc_ys->dev[i].state == 0)
			break;
	}
	if (i == NC_YS_USER_SIZE)
	{
		log_error("nc full");
		tool_thread_unlockMutex(&g_nc_ys->mutex);
		return NC_ERROR_FULL;
	}
	
	ret = nc_ys_dev_start(&g_nc_ys->dev[i], info);
	if (ret < 0)
	{
		log_error("nc_ys_dev_start id(%s), addr(%s:%d)", info->dev_id, info->ip, info->msg_port);
		tool_thread_unlockMutex(&g_nc_ys->mutex);
		return ret;
	}
	ret = nc_ys_dev_setStatus(&g_nc_ys->dev[i], status);
	if (ret < 0)
	{
		log_error("nc_ys_dev_setStatus ret(%d)", ret);
		nc_ys_dev_stop(&g_nc_ys->dev[i]);
	}
	tool_thread_unlockMutex(&g_nc_ys->mutex);
	return ret;			
}

TOOL_INT32 nc_ys_getDevInfo(nc_dev* pst_dev)
{
	if ((pst_dev->ptc & 0xFF000000) != NC_PTC_YuShi_Cloud)
		return 0;

	tool_json_t st_json;
	tool_http_t send_http;
	tool_http_t recv_http;
	tool_http_Init(&send_http);
	tool_str_strncpy(send_http.command, "POST", tool_str_strlen("POST"));
	tool_str_strncpy(send_http.url, "/s", tool_str_strlen("/s"));
	tool_str_strncpy(send_http.version, "HTTP/1.0", tool_str_strlen("HTTP/1.0"));
	send_http.ContentLength = snprintf(send_http.body, sizeof(send_http.body), 
			"{"
				"\"t\":\"Register\","
				"\"sn\":\"%s\","
				"\"n\":\"%s\","
				"\"cf\":\"false\","
				"\"u\":\"hdcctv7\","
				"\"p\":\"00d6c769749b7b29901a01311bc58ae2\""
			"}",
			pst_dev->dev_id, pst_dev->dev_id);

	tool_sock_item_t sock;
	tool_mem_memset(&sock, sizeof(tool_sock_item_t));
	if (tool_sock_item_open_v2(&sock, TOOL_SOCK_TYPE1_TCP, TOOL_SOCK_TYPE2_CLIENT, "www.ez4view.com", 80, NULL, 0) < 0)
	{
		log_error("tool_sock_item_open");
		return NC_ERROR_CONNECT;
	}
	TOOL_INT32 ret = 0;
	do
	{
		tool_http_Init(&recv_http);	
		if (tool_http_Send(&send_http, &sock, NULL, NULL) < 0)
		{
			log_error("tool_http_Send");
			ret = NC_ERROR_SEND;
			break;
		}
		if (tool_http_Recv(&recv_http, &sock, NULL, NULL) < 0)
		{
			log_error("tool_http_Send");
			ret = NC_ERROR_RECV;
			break;
		}
		tool_json_parse(&st_json, recv_http.body);
		TOOL_INT8* pch_result = tool_json_getValue(&st_json, "r");
		if (pch_result == NULL)
		{
			log_error("recv_http.body(%s)", recv_http.body);
			ret = NC_ERROR_PROTOCOL;
			break;
		}
		if (tool_str_strncmp(pch_result, "Illegal", tool_str_strlen("Illegal")) == 0)
		{
			log_error("Illegal id(%s)", pst_dev->dev_id);
			ret = NC_ERROR_ILLEGAL_ID;
			break;
		}
	} while (0);
	tool_sock_item_close(&sock);
	if (ret < 0)
		return ret;

	tool_mem_memset(&sock, sizeof(tool_sock_item_t));
	tool_http_Init(&send_http);
	tool_http_Init(&recv_http);
	tool_str_strncpy(send_http.command, "GET", tool_str_strlen("GET"));
	snprintf(send_http.url, sizeof(send_http.url), "/m/%s", pst_dev->dev_id);
	tool_str_strncpy(send_http.version, "HTTP/1.0", tool_str_strlen("HTTP/1.0"));
	tool_str_strncpy(send_http.Authorization, " Basic aGRjY3R2NzowMGQ2Yzc2OTc0OWI3YjI5OTAxYTAxMzExYmM1OGFlMg==", 
		tool_str_strlen(" Basic aGRjY3R2NzowMGQ2Yzc2OTc0OWI3YjI5OTAxYTAxMzExYmM1OGFlMg=="));

	if (tool_sock_item_open_v2(&sock, TOOL_SOCK_TYPE1_TCP, TOOL_SOCK_TYPE2_CLIENT, "www.ez4view.com", 80, NULL, 0) < 0)
	{
		log_error("tool_sock_item_open");
		return NC_ERROR_CONNECT;
	}
	do
	{
		if (tool_http_Send(&send_http, &sock, NULL, NULL) < 0)
		{
			log_error("tool_http_Send");
			ret = NC_ERROR_SEND;
			break;
		}
		if (tool_http_Recv(&recv_http, &sock, NULL, NULL) < 0)
		{
			log_error("tool_http_Send");
			ret = NC_ERROR_RECV;
			break;
		}
		tool_json_t st_json;
		tool_json_parse(&st_json, recv_http.body);
		TOOL_INT8* pch_ip = tool_json_getValue(&st_json, "ip");
		TOOL_INT8* pch_pl = tool_json_getValue(&st_json, "pl");
		TOOL_INT8* pch_np = tool_json_getValue(&st_json, "np");
		if (pch_ip == NULL || pch_pl == NULL || pch_np == NULL)
		{
			log_error("recv_http.body(%s)", recv_http.body);
			ret = NC_ERROR_OFFLINE;
			break;
		}
		
		log_debug("ip(%s) pl(%s) np(%s)", pch_ip, pch_pl, pch_np);
		tool_str_strncpy(pst_dev->ip, pch_ip, sizeof(pst_dev->ip)-1);
		TOOL_INT8 str[8][128] = {{0}};
		TOOL_INT32 port[8] = {0};
		TOOL_INT32 i = 0;				
		sscanf(pch_pl, "%[^;];%[^;];%[^;];%[^;];%[^;];%[^;];%[^;];%[^;]", 
			str[0], str[1], str[2], str[3], str[4], str[5], str[6], str[7]);
		sscanf(pch_np, "%d;%d;%d;%d;%d;%d;%d;%d", 
			&port[0], &port[1], &port[2], &port[3], &port[4], &port[5], &port[6], &port[7]);
		for (i = 0; i < 8; i++)
		{
//			log_debug("str(%s) port(%d)", str[i], port[i]);
			if (tool_str_strncmp(str[i], "sdk", 3) == 0)
				pst_dev->msg_port = port[i];
			else if (tool_str_strncmp(str[i], "live", 4) == 0)
				pst_dev->stream_port = port[i];
		}
	} while (0);
	tool_sock_item_close(&sock);
	log_state("ip(%s) port(%d.%d) user(%s) pswd(%s)", pst_dev->ip, pst_dev->msg_port, pst_dev->stream_port, pst_dev->user, pst_dev->pswd);
	return ret;
}

#if defined __cplusplus
}
#endif


