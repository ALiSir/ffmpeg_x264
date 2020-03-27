
#if defined __cplusplus
extern "C"
{
#endif

#ifndef __HDCCTV_CLIENT_H__
#define __HDCCTV_CLIENT_H__

#include "tool_type.h"
#include "qh_ptc.h"
#include "nc.h"


typedef enum
{
	HDCCTV_CLIENT_CFG_AV			= 0,
	HDCCTV_CLIENT_CFG_WIFI			= 1,
	HDCCTV_CLIENT_CFG_DEVRECORD 	= 2,
	HDCCTV_CLIENT_CFG_WORKMODE		= 3,
	HDCCTV_CLIENT_CFG_NUM,
}HDCCTV_CLIENT_CFG_E;


TOOL_INT32 hdcctv_client_startQpns(HDCCTV_QPNS_TOKEN_TYPE_E token_type, TOOL_INT8* mobile_token, HDCCTV_LANUAGE_E language, NC_CB cb, TOOL_VOID* param);
TOOL_VOID hdcctv_client_stopQpns();

TOOL_VOID hdcctv_client2_init();
TOOL_INT32 hdcctv_client2_setBuffer(TOOL_INT32 buffer_usec);
TOOL_VOID hdcctv_client2_done();

TOOL_INT32 hdcctv_client2_getAlarms(HDCCTV_QPNS_TOKEN_TYPE_E token_type, TOOL_INT8* mobile_token, TOOL_INT32 no, hdcctv_qpns_alarms_t* alarms);
TOOL_INT32 hdcctv_client2_getQpnsDeviceMaps(HDCCTV_QPNS_TOKEN_TYPE_E token_type, TOOL_INT8* mobile_token, hdcctv_qpns_device_maps_t* device_maps);
TOOL_INT32 hdcctv_client2_setQpnsDeviceMaps(HDCCTV_QPNS_TOKEN_TYPE_E token_type, TOOL_INT8* mobile_token, hdcctv_qpns_device_maps_t* device_maps);
TOOL_INT32 hdcctv_client2_addQpnsDeviceMaps(HDCCTV_QPNS_TOKEN_TYPE_E token_type, TOOL_INT8* mobile_token, hdcctv_qpns_device_maps_t* device_maps);
TOOL_INT32 hdcctv_client2_delQpnsDeviceMaps(HDCCTV_QPNS_TOKEN_TYPE_E token_type, TOOL_INT8* mobile_token, hdcctv_qpns_device_maps_t* device_maps);
TOOL_INT32 hdcctv_client2_testQpns(HDCCTV_QPNS_TOKEN_TYPE_E token_type, TOOL_INT8* mobile_token, TOOL_INT8* dev_id);

TOOL_INT32 hdcctv_client2_getSms(hdcctv_cloud_user_t* pst_user, TOOL_INT8* action);
TOOL_INT32 hdcctv_client2_regUser(hdcctv_cloud_user_t* pst_user, TOOL_INT8* code);
TOOL_INT32 hdcctv_client2_resetPswd(hdcctv_cloud_user_t* pst_user, TOOL_INT8* code);
TOOL_INT32 hdcctv_client2_loginUser(hdcctv_cloud_user_t* pst_user);
TOOL_INT32 hdcctv_client2_logout();
TOOL_INT32 hdcctv_client2_setPswd(hdcctv_cloud_user_t* pst_user, TOOL_INT8* new_pswd);
TOOL_INT32 hdcctv_client2_bindDev(hdcctv_cloud_user_t* pst_user, hdcctv_device_t* pst_device, TOOL_INT8* action);
TOOL_INT32 hdcctv_client2_doDevList(hdcctv_cloud_user_t* pst_user, hdcctv_device_list_t* pst_device_list);
TOOL_INT32 hdcctv_client2_getDevList(hdcctv_cloud_user_t* pst_user, hdcctv_device_list_t* pst_device_list);
TOOL_INT32 hdcctv_client2_getCloudCfg(hdcctv_cloud_user_t* pst_user, TOOL_INT8* dev_id, hdcctv_cloud_cfg_t* pst_cloud_cfg);
TOOL_INT32 hdcctv_client2_setCloudCfg(hdcctv_cloud_user_t* pst_user, TOOL_INT8* dev_id, hdcctv_cloud_cfg_t* pst_cloud_cfg);
TOOL_INT32 hdcctv_client2_getDevById(nc_dev* pst_nc_dev);
TOOL_INT32 hdcctv_client2_startRts(nc_dev* pst_dev, TOOL_INT32 channel_no, TOOL_INT32 stream_type, NC_CB cb, TOOL_VOID* param);
TOOL_INT32 hdcctv_client2_stopRts(TOOL_INT32 user_id);

TOOL_INT32 hdcctv_client2_startDevPbs(nc_dev* pst_dev, ptc_cfg_record_t* record, NC_CB cb, TOOL_VOID* param);
TOOL_INT32 hdcctv_client2_stopDevPbs(TOOL_INT32 pbs_id);
TOOL_INT32 hdcctv_client2_startCloudPbs(nc_dev* pst_dev, ptc_cfg_record_t* record, NC_CB cb, TOOL_VOID* param);
TOOL_INT32 hdcctv_client2_stopCloudPbs(TOOL_INT32 pbs_id);


TOOL_INT32 hdcctv_client2_getChannelNum_v2(nc_dev* dev);
TOOL_INT32 hdcctv_client2_getAbility_v2(nc_dev* pst_dev, ptc_cfg_ability_t* ability);
TOOL_INT32 hdcctv_client2_ctrlPtz(nc_dev* pst_dev, TOOL_INT32 channel, PTC_PTZ_CMD_E cmd, TOOL_INT32 param);
TOOL_INT32 hdcctv_client2_syncTime(nc_dev* pst_dev, TOOL_UINT32 utc_sec, TOOL_INT32 zone, TOOL_INT32 dst_hour);
TOOL_INT32 hdcctv_client2_getDeviceStatus_v2(nc_dev* pst_dev, ptc_cfg_status_t* status);
TOOL_INT32 hdcctv_client2_setDeviceStatus_v2(nc_dev* pst_dev, ptc_cfg_status_t* status);
TOOL_INT32 hdcctv_client2_getAlarmCfg(nc_dev* pst_dev, PTC_ALARM_TYPE_E alarm_type, ptc_cfg_alarm_t* alarm);
TOOL_INT32 hdcctv_client2_setAlarmCfg(nc_dev* pst_dev, PTC_ALARM_TYPE_E alarm_type, ptc_cfg_alarm_t* alarm);
TOOL_INT32 hdcctv_client2_getWiFiCfg(nc_dev* pst_dev, ptc_cfg_wifi_t* pst_wifi);
TOOL_INT32 hdcctv_client2_setWiFiCfg(nc_dev* pst_dev, ptc_cfg_wifi_t* pst_wifi);
TOOL_INT32 hdcctv_client2_getDevRecordCfg(nc_dev* pst_dev, ptc_cfg_devRecord_t* pst_devRecord);
TOOL_INT32 hdcctv_client2_setDevRecordCfg(nc_dev* pst_dev, ptc_cfg_devRecord_t* pst_devRecord);
TOOL_INT32 hdcctv_client2_getCfg(nc_dev* pst_dev, TOOL_INT32 id, TOOL_VOID* cfg);
TOOL_INT32 hdcctv_client2_setCfg(nc_dev* pst_dev, TOOL_INT32 id, TOOL_VOID* cfg);


TOOL_INT32 hdcctv_client2_searchDevDates(nc_dev* pst_dev, ptc_cfg_dates_t* dates);
TOOL_INT32 hdcctv_client2_searchDevRecords(nc_dev* pst_dev, ptc_cfg_records_t* records);
TOOL_INT32 hdcctv_client2_searchCloudDates(nc_dev* pst_dev, ptc_cfg_dates_t* dates);
TOOL_INT32 hdcctv_client2_searchCloudRecords(nc_dev* pst_dev, ptc_cfg_records_t* records);
TOOL_INT32 hdcctv_client2_searchLogs(nc_dev* pst_dev, ptc_logs_v2_t* logs_v2);

TOOL_INT32 hdcctv_client2_startTalk(nc_dev* pst_dev, NC_CB cb, TOOL_VOID* param);
TOOL_INT32 hdcctv_client2_stopTalk(TOOL_INT32 talk_id);
TOOL_INT32 hdcctv_client2_sendTalk(TOOL_INT32 talk_id, TOOL_UINT8* buf, TOOL_INT32 len);



#endif

#if defined __cplusplus
}
#endif

