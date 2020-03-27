
#if defined __cplusplus
extern "C"
{
#endif

#ifndef __TOOL_PTC_H__
#define __TOOL_PTC_H__

#include "hy_ptc.h"
#include "qh_ptc.h"
#include "ys_ptc.h"
#include "xm_ptc.h"
#include "tool_type.h"



TOOL_INT32 tool_ptc_qh_sendMsg(TOOL_VOID* sock_item, ptc_msg_t* msg);
TOOL_INT32 tool_ptc_qh_recvMsg(TOOL_VOID* sock_item, ptc_msg_t* msg);
TOOL_INT32 tool_ptc_qh_sendtoMsg(TOOL_VOID* sock_item, ptc_msg_t* msg);
TOOL_INT32 tool_ptc_qh_recvfromMsg(TOOL_VOID* sock_item, ptc_msg_t* msg);
TOOL_INT32 tool_ptc_qh_sendFrame(TOOL_VOID* sock_item, ptc_frame_t* frame);
TOOL_INT32 tool_ptc_qh_recvFrame(TOOL_VOID* sock_item, ptc_frame_t* frame);

TOOL_INT32 tool_ptc_qh_sendFrame_v2(TOOL_VOID* sock_item, ptc_frame_t* frame, TOOL_INT32 seed);
TOOL_INT32 tool_ptc_qh_recvFrame_v2(TOOL_VOID* sock_item, ptc_frame_t* frame, TOOL_INT32 seed);


TOOL_INT8* tool_ptc_qh_buildRect(ptc_rect_t* pst_rect, TOOL_INT8* buf, TOOL_INT32 len);
TOOL_INT32 tool_ptc_qh_parseRect(ptc_rect_t* pst_rect, TOOL_INT8* buf);

TOOL_INT8* tool_ptc_qh_buildIPv4(TOOL_UINT8* pch_ip, TOOL_INT8* buf, TOOL_INT32 len);
TOOL_INT32 tool_ptc_qh_parseIPv4(TOOL_UINT8* pch_ip, TOOL_INT8* buf);

TOOL_INT8* tool_ptc_qh_encodeNonce(TOOL_INT32 nonce, TOOL_INT8* buf);

TOOL_INT32 tool_ptc_qh_c2d_encode_ctrlPtz(ptc_msg_t* pst_msg, TOOL_INT32 channel, PTC_PTZ_CMD_E ptz, TOOL_INT32 param);
TOOL_INT32 tool_ptc_qh_c2d_decode_ctrlPtz(ptc_msg_t* pst_msg, TOOL_INT32* pi_channel, PTC_PTZ_CMD_E* pe_ptz, TOOL_INT32* pi_param);

TOOL_INT32 tool_ptc_qh_c2d_encode_syncTime(ptc_msg_t* pst_msg, TOOL_UINT32 utc_sec, TOOL_INT32 zone, TOOL_INT32 dst_hour);
TOOL_INT32 tool_ptc_qh_c2d_decode_syncTime(ptc_msg_t* pst_msg, TOOL_UINT32* pu_utc_sec, TOOL_INT32* pi_zone, TOOL_INT32* pi_dst_hour);

TOOL_INT32 tool_ptc_qh_c2d_encode_getAbility(ptc_frame_t* pst_frame);
TOOL_INT32 tool_ptc_qh_c2d_decode_getAbility(ptc_frame_t* pst_frame);
TOOL_INT32 tool_ptc_qh_d2c_encode_getAbility(ptc_frame_t* pst_frame, ptc_cfg_ability_t* pst_cfg_ability);
TOOL_INT32 tool_ptc_qh_d2c_decode_getAbility(ptc_frame_t* pst_frame, ptc_cfg_ability_t* pst_cfg_ability);

TOOL_INT32 tool_ptc_qh_c2d_encode_getLogs(ptc_frame_t* pst_frame, ptc_logs_v2_t* pst_logs_v2);
TOOL_INT32 tool_ptc_qh_c2d_decode_getLogs(ptc_frame_t* pst_frame, ptc_logs_v2_t* pst_logs_v2);
TOOL_INT32 tool_ptc_qh_d2c_encode_getLogs(ptc_frame_t* pst_frame, ptc_logs_v2_t* pst_logs_v2);
TOOL_INT32 tool_ptc_qh_d2c_decode_getLogs(ptc_frame_t* pst_frame, ptc_logs_v2_t* pst_logs_v2);

TOOL_INT32 tool_ptc_qh_c2d_encode_getRecords(ptc_frame_t* pst_frame, ptc_cfg_records_t* pst_records);
TOOL_INT32 tool_ptc_qh_c2d_decode_getRecords(ptc_frame_t* pst_frame, ptc_cfg_records_t* pst_records);
TOOL_INT32 tool_ptc_qh_d2c_encode_getRecords(ptc_frame_t* pst_frame, ptc_cfg_records_t* pst_records);
TOOL_INT32 tool_ptc_qh_d2c_decode_getRecords(ptc_frame_t* pst_frame, ptc_cfg_records_t* pst_records);

TOOL_INT32 tool_ptc_qh_c2d_encode_getStatus(ptc_msg_t* pst_msg);
TOOL_INT32 tool_ptc_qh_c2d_decode_getStatus(ptc_msg_t* pst_msg);
TOOL_INT32 tool_ptc_qh_d2c_encode_getStatus(ptc_msg_t* pst_msg, ptc_cfg_status_t* pst_cfg_status);
TOOL_INT32 tool_ptc_qh_d2c_decode_getStatus(ptc_msg_t* pst_msg, ptc_cfg_status_t* pst_cfg_status);

TOOL_INT32 tool_ptc_qh_c2d_encode_setStatus(ptc_msg_t* pst_msg, ptc_cfg_status_t* pst_cfg_status);
TOOL_INT32 tool_ptc_qh_c2d_decode_setStatus(ptc_msg_t* pst_msg, ptc_cfg_status_t* pst_cfg_status);
TOOL_INT32 tool_ptc_qh_d2c_encode_setStatus(ptc_msg_t* pst_msg);
TOOL_INT32 tool_ptc_qh_d2c_decode_setStatus(ptc_msg_t* pst_msg);

TOOL_INT32 tool_ptc_qh_c2d_encode_getAlarm(ptc_msg_t* pst_msg, PTC_ALARM_TYPE_E alarm_type);
TOOL_INT32 tool_ptc_qh_c2d_decode_getAlarm(ptc_msg_t* pst_msg, PTC_ALARM_TYPE_E* pe_alarm_type);
TOOL_INT32 tool_ptc_qh_d2c_encode_getAlarm(ptc_msg_t* pst_msg, ptc_cfg_alarm_t* pst_alarm);
TOOL_INT32 tool_ptc_qh_d2c_decode_getAlarm(ptc_msg_t* pst_msg, ptc_cfg_alarm_t* pst_alarm);

TOOL_INT32 tool_ptc_qh_c2d_encode_setAlarm(ptc_msg_t* pst_msg, PTC_ALARM_TYPE_E alarm_type, ptc_cfg_alarm_t* pst_cfg_alarm);
TOOL_INT32 tool_ptc_qh_c2d_decode_setAlarm(ptc_msg_t* pst_msg, PTC_ALARM_TYPE_E* pe_alarm_type, ptc_cfg_alarm_t* pst_cfg_alarm);
TOOL_INT32 tool_ptc_qh_d2c_encode_setAlarm(ptc_msg_t* pst_msg);
TOOL_INT32 tool_ptc_qh_d2c_decode_setAlarm(ptc_msg_t* pst_msg);

TOOL_INT32 tool_ptc_qh_c2d_encode_getWorkMode(ptc_msg_t* pst_msg);
TOOL_INT32 tool_ptc_qh_c2d_decode_getWorkMode(ptc_msg_t* pst_msg);
TOOL_INT32 tool_ptc_qh_d2c_encode_getWorkMode(ptc_msg_t* pst_msg, ptc_cfg_workMode_t* pst_cfg_workMode);
TOOL_INT32 tool_ptc_qh_d2c_decode_getWorkMode(ptc_msg_t* pst_msg, ptc_cfg_workMode_t* pst_cfg_workMode);

TOOL_INT32 tool_ptc_qh_c2d_encode_setWorkMode(ptc_msg_t* pst_msg, ptc_cfg_workMode_t* pst_cfg_workMode);
TOOL_INT32 tool_ptc_qh_c2d_decode_setWorkMode(ptc_msg_t* pst_msg, ptc_cfg_workMode_t* pst_cfg_workMode);
TOOL_INT32 tool_ptc_qh_d2c_encode_setWorkMode(ptc_msg_t* pst_msg);
TOOL_INT32 tool_ptc_qh_d2c_decode_setWorkMode(ptc_msg_t* pst_msg);

TOOL_INT32 tool_ptc_qh_c2d_encode_getMobile(ptc_msg_t* pst_msg);
TOOL_INT32 tool_ptc_qh_c2d_decode_getMobile(ptc_msg_t* pst_msg);
TOOL_INT32 tool_ptc_qh_d2c_encode_getMobile(ptc_msg_t* pst_msg, ptc_cfg_mobile_t* pst_cfg_mobile);
TOOL_INT32 tool_ptc_qh_d2c_decode_getMobile(ptc_msg_t* pst_msg, ptc_cfg_mobile_t* pst_cfg_mobile);

TOOL_INT32 tool_ptc_qh_c2d_encode_setMobile(ptc_msg_t* pst_msg, ptc_cfg_mobile_t* pst_cfg_mobile);
TOOL_INT32 tool_ptc_qh_c2d_decode_setMobile(ptc_msg_t* pst_msg, ptc_cfg_mobile_t* pst_cfg_mobile);
TOOL_INT32 tool_ptc_qh_d2c_encode_setMobile(ptc_msg_t* pst_msg);
TOOL_INT32 tool_ptc_qh_d2c_decode_setMobile(ptc_msg_t* pst_msg);

TOOL_INT32 tool_ptc_qh_c2d_encode_getWifi(ptc_msg_t* pst_msg);
TOOL_INT32 tool_ptc_qh_c2d_decode_getWifi(ptc_msg_t* pst_msg);
TOOL_INT32 tool_ptc_qh_d2c_encode_getWifi(ptc_msg_t* pst_msg, ptc_cfg_wifi_t* pst_cfg_wifi);
TOOL_INT32 tool_ptc_qh_d2c_decode_getWifi(ptc_msg_t* pst_msg, ptc_cfg_wifi_t* pst_cfg_wifi);

TOOL_INT32 tool_ptc_qh_c2d_encode_setWifi(ptc_msg_t* pst_msg, ptc_cfg_wifi_t* pst_cfg_wifi);
TOOL_INT32 tool_ptc_qh_c2d_decode_setWifi(ptc_msg_t* pst_msg, ptc_cfg_wifi_t* pst_cfg_wifi);
TOOL_INT32 tool_ptc_qh_d2c_encode_setWifi(ptc_msg_t* pst_msg);
TOOL_INT32 tool_ptc_qh_d2c_decode_setWifi(ptc_msg_t* pst_msg);

TOOL_INT32 tool_ptc_qh_c2d_encode_getDevRecord(ptc_msg_t* pst_msg);
TOOL_INT32 tool_ptc_qh_c2d_decode_getDevRecord(ptc_msg_t* pst_msg);
TOOL_INT32 tool_ptc_qh_d2c_encode_getDevRecord(ptc_msg_t* pst_msg, ptc_cfg_devRecord_t* pst_cfg_devRecord);
TOOL_INT32 tool_ptc_qh_d2c_decode_getDevRecord(ptc_msg_t* pst_msg, ptc_cfg_devRecord_t* pst_cfg_devRecord);

TOOL_INT32 tool_ptc_qh_c2d_encode_setDevRecord(ptc_msg_t* pst_msg, ptc_cfg_devRecord_t* pst_cfg_devRecord);
TOOL_INT32 tool_ptc_qh_c2d_decode_setDevRecord(ptc_msg_t* pst_msg, ptc_cfg_devRecord_t* pst_cfg_devRecord);
TOOL_INT32 tool_ptc_qh_d2c_encode_setDevRecord(ptc_msg_t* pst_msg);
TOOL_INT32 tool_ptc_qh_d2c_decode_setDevRecord(ptc_msg_t* pst_msg);

TOOL_INT32 tool_ptc_qh_c2d_encode_getAv(ptc_msg_t* pst_msg);
TOOL_INT32 tool_ptc_qh_c2d_decode_getAv(ptc_msg_t* pst_msg);
TOOL_INT32 tool_ptc_qh_d2c_encode_getAv(ptc_msg_t* pst_msg, ptc_cfg_av_t* pst_cfg_av);
TOOL_INT32 tool_ptc_qh_d2c_decode_getAv(ptc_msg_t* pst_msg, ptc_cfg_av_t* pst_cfg_av);

TOOL_INT32 tool_ptc_qh_c2d_encode_setAv(ptc_msg_t* pst_msg, ptc_cfg_av_t* pst_cfg_av);
TOOL_INT32 tool_ptc_qh_c2d_decode_setAv(ptc_msg_t* pst_msg, ptc_cfg_av_t* pst_cfg_av);
TOOL_INT32 tool_ptc_qh_d2c_encode_setAv(ptc_msg_t* pst_msg);
TOOL_INT32 tool_ptc_qh_d2c_decode_setAv(ptc_msg_t* pst_msg);

TOOL_INT32 tool_ptc_qh_c2d_encode_searchDates(ptc_frame_t* pst_frame, ptc_cfg_dates_t* pst_dates);
TOOL_INT32 tool_ptc_qh_c2d_decode_searchDates(ptc_frame_t* pst_frame, ptc_cfg_dates_t* pst_dates);
TOOL_INT32 tool_ptc_qh_d2c_encode_searchDates(ptc_frame_t* pst_frame, ptc_cfg_dates_t* pst_dates);
TOOL_INT32 tool_ptc_qh_d2c_decode_searchDates(ptc_frame_t* pst_frame, ptc_cfg_dates_t* pst_dates);




TOOL_INT32 tool_ptc_hy_sendMsg(TOOL_VOID* sock_item, hy_msg_t* msg);
TOOL_INT32 tool_ptc_hy_recvMsg(TOOL_VOID* sock_item, hy_msg_t* msg);
TOOL_INT32 tool_ptc_hy_recvRts(TOOL_VOID* sock_item, TOOL_UINT8* data, TOOL_UINT32 *size, ptc_frame_head_t** frame_head);
TOOL_INT32 tool_ptc_hy_recvPbs(TOOL_VOID* sock_item, TOOL_UINT8* buf, TOOL_UINT32* len, TOOL_UINT32 size, 
	ptc_frame_head_t** frame_head, TOOL_UINT32* frame_len,
	TOOL_UINT8** left_buf, TOOL_UINT32* left_len);
TOOL_INT32 tool_ptc_hy_sendTalk_msg(TOOL_VOID* sock_item, hy_msg_t* msg);
TOOL_INT32 tool_ptc_hy_sendTalk_frame(TOOL_VOID* sock_item, TOOL_UINT8* data, TOOL_UINT32 len);


#ifdef __IS_SSL__

typedef struct
{
	TOOL_INT32 identifier;		
	TOOL_INT8 token[32];
	TOOL_INT8 load_alert[HDCCTV_QPNS_CONTENT_SIZE];
	TOOL_INT32 load_badge;
	TOOL_INT8 load_sound[PTC_ID_SIZE];
}ptc_qpns_msg_s2c_t;

typedef struct
{
	TOOL_INT8 cmd;				//8
	TOOL_INT8 status;		
	TOOL_INT32 identifier;
}ptc_qpns_msg_c2s_t;


//TOOL_INT32 tool_ptc_apple_sendQpns(TOOL_VOID* tssl, ptc_qpns_msg_s2c_t* msg);
TOOL_INT32 tool_ptc_apple_sendQpns(TOOL_VOID* tssl, hdcctv_qpns_alarm_t* pst_alarm, TOOL_INT8* alert, TOOL_INT32 load_badge);
TOOL_INT32 tool_ptc_apple_recvQpns(TOOL_VOID* tssl, ptc_qpns_msg_c2s_t* msg);

#endif


TOOL_INT32 tool_ptc_rtp_recvTcp_video(TOOL_VOID* sock_item, ptc_frame_video_t* frame_data);
TOOL_INT32 tool_ptc_rtp_recvTcp_talk(TOOL_VOID* sock_item, ptc_frame_audio_t* frame_data);
TOOL_INT32 tool_ptc_rtp_sendTcp_talk(TOOL_VOID* sock_item, ptc_frame_audio_t* frame_data);


TOOL_VOID tool_ptc_ys_fillIeHead_login_c2d(ys_msg_login_c2d_t* login_c2d);
TOOL_VOID tool_ptc_ys_fillIeHead_logout_c2d(ys_msg_logout_c2d_t* logout_c2d);
TOOL_VOID tool_ptc_ys_fillIeHead_heartbeat_c2d(ys_msg_heartbeat_c2d_t* heartbeat_c2d);
TOOL_VOID tool_ptc_ys_fillIeHead_startrts_c2d(ys_msg_startrts_c2d_t* startrts_c2d);
TOOL_VOID tool_ptc_ys_fillIeHead_stoprts_c2d(ys_msg_stoprts_c2d_t* stoprts_c2d);
TOOL_VOID tool_ptc_ys_fillIeHead_ptz_c2d(ys_msg_ptz_c2d_t* ptz_c2d);
TOOL_VOID tool_ptc_ys_fillIeHead_searchrecords_c2d(ys_msg_searchrecords_c2d_t* pst_searchrecords_c2d);
TOOL_VOID tool_ptc_ys_fillIeHead_searchlogs_c2d(ys_msg_searchlogs_c2d_t* pst_searchlogs_c2d);
TOOL_VOID tool_ptc_ys_fillIeHead_startpbs_c2d(ys_msg_startpbs_c2d_t* pst_startpbs_c2d);
TOOL_VOID tool_ptc_ys_fillIeHead_setpbs_c2d(ys_msg_setpbs_c2d_t* pst_setpbs_c2d);
TOOL_VOID tool_ptc_ys_fillIeHead_stoppbs_c2d(ys_msg_stoppbs_c2d_t* pst_stoppbs_c2d);
TOOL_VOID tool_ptc_ys_fillIeHead_getpbs_c2d(ys_msg_getpbs_c2d_t* pst_getpbs_c2d);
TOOL_VOID tool_ptc_ys_fillIeHead_starttalk_c2d(ys_msg_starttalk_c2d_t* pst_starttalk_c2d);
TOOL_VOID tool_ptc_ys_fillIeHead_stoptalk_c2d(ys_msg_stoptalk_c2d_t* pst_stoptalk_c2d);
TOOL_VOID tool_ptc_ys_fillIeHead_gettime_c2d(ys_msg_gettime_c2d_t* pst_gettime_c2d);
TOOL_VOID tool_ptc_ys_fillIeHead_settime_c2d(ys_msg_settime_c2d_t* pst_settime_c2d);
TOOL_VOID tool_ptc_ys_fillIeHead_getAO_c2d(ys_msg_getAO_c2d_t* pst_getAO_c2d);
TOOL_VOID tool_ptc_ys_fillIeHead_setAO_c2d(ys_msg_setAO_c2d_t* pst_setAO_c2d);


TOOL_INT32 tool_ptc_ys_sendMsg(TOOL_VOID* sock_item, ys_msg_t* msg);
TOOL_INT32 tool_ptc_ys_recvMsg(TOOL_VOID* sock_item, ys_msg_t* msg);
TOOL_INT32 tool_ptc_ys_sendSearch(TOOL_VOID* sock_item, ys_search_t* search);
TOOL_INT32 tool_ptc_ys_recvSearch(TOOL_VOID* sock_item, ys_search_t* search);

TOOL_INT32 tool_ptc_xm_sendMsg(TOOL_VOID* sock_item, xm_msg_t* msg);
TOOL_INT32 tool_ptc_xm_recvMsg(TOOL_VOID* sock_item, xm_msg_t* msg);
TOOL_INT32 tool_ptc_xm_recvSearch(TOOL_VOID* sock_item, xm_search_t* search);
TOOL_INT32 tool_ptc_xm_recvRts(TOOL_VOID* sock_item, TOOL_UINT8* buf, TOOL_UINT32* len, TOOL_UINT32 size, 
	ptc_frame_head_t** frame_head, TOOL_UINT32* frame_len,
	TOOL_UINT8** left_buf, TOOL_UINT32* left_len);
TOOL_INT32 tool_ptc_xm_recvTalk(TOOL_VOID* sock_item, ptc_frame_audio_t* pst_ptc_frame);
TOOL_INT32 tool_ptc_xm_sendTalk(TOOL_VOID* sock_item, ptc_frame_audio_t* pst_ptc_frame, TOOL_UINT32 session_id);

#endif

#if defined __cplusplus
}
#endif

