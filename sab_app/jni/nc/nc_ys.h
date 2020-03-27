

#ifndef __NC_YS_H__
#define __NC_YS_H__

#if defined __cplusplus
extern "C"
{
#endif

#include "nc.h"
#include "tool_type.h"
#include "qh_ptc.h"

#define YS_MCAST_IP 		"239.255.255.250" 
#define YS_MCAST_PORT		3702


TOOL_VOID nc_ys_init();
TOOL_INT32 nc_ys_setBuffer(TOOL_INT32 buffer_usec);
TOOL_VOID nc_ys_done();

TOOL_INT32 nc_ys_searchDev(TOOL_VOID* param);

TOOL_INT32 nc_ys_getChannelNum_v2(nc_dev* dev);
TOOL_INT32 nc_ys_startRts_v2(nc_dev* dev, TOOL_INT32 channel, TOOL_INT32 stream_type, NC_CB cb, void* param);
TOOL_INT32 nc_ys_stopRts(TOOL_INT32 dev_id);

TOOL_INT32 nc_ys_controlPTZ_v2(nc_dev* dev, TOOL_INT32 channel, PTC_PTZ_CMD_E cmd, TOOL_INT32 param);

TOOL_INT32 nc_ys_searchRecords(nc_dev* dev, ptc_cfg_records_t* records);
TOOL_INT32 nc_ys_startPbs(nc_dev* dev, ptc_cfg_record_t* record, NC_CB cb, void* param);
TOOL_INT32 nc_ys_stopPbs(TOOL_INT32 user_id);

TOOL_INT32 nc_ys_searchLogs(nc_dev* dev, ptc_logs_t* logs);

TOOL_INT32 nc_ys_startTalk_v2(nc_dev* dev, NC_CB cb, void* param);
TOOL_INT32 nc_ys_stopTalk(TOOL_INT32 user_id);
TOOL_INT32 nc_ys_sendTalk(TOOL_INT32 user_id, TOOL_UINT8* buf, TOOL_INT32 len);

TOOL_INT32 nc_ys_getAbility_v2(nc_dev* info, ptc_cfg_ability_t* ability);
TOOL_INT32 nc_ys_syncTime(nc_dev* dev, TOOL_UINT32 utc_sec, TOOL_INT32 zone, TOOL_INT32 dst_hour);

TOOL_INT32 nc_ys_getDeviceStatus_v2(nc_dev* info, ptc_cfg_status_t* status);
TOOL_INT32 nc_ys_setDeviceStatus_v2(nc_dev* info, ptc_cfg_status_t* status);


TOOL_INT32 nc_ys_getDevInfo(nc_dev* pst_dev);

#if defined __cplusplus
}
#endif

#endif


