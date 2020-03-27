
#ifndef __NC_QH_H__
#define __NC_QH_H__

#if defined __cplusplus
extern "C"
{
#endif

#include "tool_type.h"


// SDK初始化
TOOL_VOID nc_qh_init();
TOOL_INT32 nc_qh_setBuffer(TOOL_INT32 buffer_usec);		//buffer_usec:0-3000*1000
TOOL_VOID nc_qh_done();

// 搜索设备和登陆登出
TOOL_INT32 nc_qh_getSearchById(nc_dev* pst_nc_dev);
TOOL_INT32 nc_qh_getSearch(ptc_mcast_searchs_t* searchs);
TOOL_INT32 nc_qh_searchDev(ptc_mcast_searchs_t* searchs);
TOOL_INT32 nc_qh_auth_v2(nc_dev* dev);
TOOL_INT32 nc_qh_login_v2(nc_dev* dev, NC_CB cb, TOOL_VOID* param);
TOOL_INT32 nc_qh_logout(TOOL_INT32 user_id);
TOOL_INT32 nc_qh_getOnlineStatus(TOOL_INT32 user_id);
TOOL_INT32 nc_qh_getChannelNum_v2(nc_dev* dev);

// 实时预览
TOOL_INT32 nc_qh_startRts_v2(nc_dev* dev, TOOL_INT32 channel, TOOL_INT32 stream_type, NC_CB cb, TOOL_VOID* param);
TOOL_INT32 nc_qh_forceRtsKeyFrame(TOOL_INT32 rts_id);
TOOL_INT32 nc_qh_stopRts(TOOL_INT32 rts_id);
TOOL_INT32 nc_qh_getRtsStatus(TOOL_INT32 rts_id);
TOOL_INT32 nc_qh_setRtsLevel(TOOL_INT32 rts_id, TOOL_INT32 level);	//level:1/2/3

// 云台控制
TOOL_INT32 nc_qh_controlPTZ_v2(nc_dev* dev, TOOL_INT32 channel, PTC_PTZ_CMD_E cmd, TOOL_INT32 param);

// 语音对讲
TOOL_INT32 nc_qh_getAudioAbility(TOOL_INT32 user_id, ptc_cfg_audio_ability_t* audio_ability);
TOOL_INT32 nc_qh_startTalk_v2(nc_dev* dev, NC_CB cb, TOOL_VOID* param);
TOOL_INT32 nc_qh_stopTalk(TOOL_INT32 talk_id);
TOOL_INT32 nc_qh_sendTalk(TOOL_INT32 talk_id, TOOL_UINT8* buf, TOOL_INT32 len);

// 参数配置
TOOL_INT32 nc_qh_getAbility_v2(nc_dev* dev, ptc_cfg_ability_t* ability);
TOOL_INT32 nc_qh_getAbility_v3(nc_dev* dev, ptc_cfg_ability_t* ability);
TOOL_INT32 nc_qh_getDeviceStatus_v2(nc_dev* dev, ptc_cfg_status_t* status);
TOOL_INT32 nc_qh_setDeviceStatus_v2(nc_dev* dev, ptc_cfg_status_t* status);

TOOL_INT32 nc_qh_getCfg(nc_dev* dev, ptc_cfg_t* cfg);
TOOL_INT32 nc_qh_getDefaultCfg(nc_dev* dev, ptc_cfg_t* cfg);
TOOL_INT32 nc_qh_setCfg(nc_dev* dev, ptc_cfg_t* cfg);
TOOL_INT32 nc_qh_getCfg_time(nc_dev* dev, ptc_cfg_time_t* cfg_time);
TOOL_INT32 nc_qh_setCfg_time(nc_dev* dev, ptc_cfg_time_t* cfg_time);
TOOL_INT32 nc_qh_syncTime(nc_dev* dev, TOOL_UINT32 utc_sec, TOOL_INT32 zone, TOOL_INT32 dst_hour);		//zone:PTC_TIME_ZONE_W****, dst_hour:1,2,3


typedef enum
{
	NC_QH_CFG_AV			= 0,
	NC_QH_CFG_WIFI			= 1,
	NC_QH_CFG_DEVRECORD		= 2,
	NC_QH_CFG_WORKMODE		= 3,
	NC_QH_CFG_NUM,
}NC_QH_CFG_E;


TOOL_INT32 nc_qh_getCfg_alarm(nc_dev* dev, PTC_ALARM_TYPE_E alarm_type, ptc_cfg_alarm_t* cfg_alarm);
TOOL_INT32 nc_qh_setCfg_alarm(nc_dev* dev, PTC_ALARM_TYPE_E alarm_type, ptc_cfg_alarm_t* cfg_alarm);

TOOL_INT32 nc_qh_getCfg_workMode(nc_dev* dev, ptc_cfg_workMode_t* cfg_workMode);
TOOL_INT32 nc_qh_setCfg_workMode(nc_dev* dev, ptc_cfg_workMode_t* cfg_workMode);

TOOL_INT32 nc_qh_getCfg_mobile(nc_dev* dev, ptc_cfg_mobile_t* cfg_mobile);
TOOL_INT32 nc_qh_setCfg_mobile(nc_dev* dev, ptc_cfg_mobile_t* cfg_mobile);

TOOL_INT32 nc_qh_getCfg_wifi(nc_dev* dev, ptc_cfg_wifi_t* cfg_wifi);
TOOL_INT32 nc_qh_setCfg_wifi(nc_dev* dev, ptc_cfg_wifi_t* cfg_wifi);

TOOL_INT32 nc_qh_getCfg_devRecord(nc_dev* dev, ptc_cfg_devRecord_t* cfg_devRecord);
TOOL_INT32 nc_qh_setCfg_devRecord(nc_dev* dev, ptc_cfg_devRecord_t* cfg_devRecord);

TOOL_INT32 nc_qh_getCfg_all(nc_dev* dev, TOOL_INT32 id, TOOL_VOID* cfg_all);
TOOL_INT32 nc_qh_setCfg_all(nc_dev* dev, TOOL_INT32 id, TOOL_VOID* cfg_all);


// 录像回放
TOOL_INT32 nc_qh_searchDates(nc_dev* dev, ptc_cfg_dates_t* dates);
TOOL_INT32 nc_qh_searchRecords(nc_dev* dev, ptc_cfg_records_t* records);
TOOL_INT32 nc_qh_startPbs(nc_dev* dev, ptc_cfg_record_t* record, NC_CB cb, TOOL_VOID* param);
TOOL_INT32 nc_qh_setPbsPos(TOOL_INT32 pbs_id, TOOL_UINT32 pos);
TOOL_INT32 nc_qh_pausePbs(TOOL_INT32 pbs_id);
TOOL_INT32 nc_qh_restartPbs(TOOL_INT32 pbs_id);
TOOL_INT32 nc_qh_setPbsSpeed(TOOL_INT32 pbs_id, TOOL_INT32 speed);
TOOL_INT32 nc_qh_stopPbs(TOOL_INT32 pbs_id);

// 日志搜索
TOOL_INT32 nc_qh_searchLogs(nc_dev* dev, ptc_logs_t* logs);

// 透明传送
TOOL_INT32 nc_qh_sendData(nc_dev* dev, TOOL_VOID* in_data, TOOL_UINT32 in_len, TOOL_VOID* out_data, TOOL_UINT32* out_len);

#if defined __cplusplus
}
#endif


#endif

