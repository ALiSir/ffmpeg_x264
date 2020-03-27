
#if defined __cplusplus
extern "C"
{
#endif

#ifndef __NC_HY_H__
#define __NC_HY_H__

#include "nc.h"
#include "tool_type.h"
#include "qh_ptc.h"

#define HY_BCAST_IP 		"255.255.255.255" 
#define HY_BCAST_PORT		5050


TOOL_VOID nc_hy_init();
TOOL_INT32 nc_hy_setBuffer(TOOL_INT32 buffer_usec);
TOOL_VOID nc_hy_done();

TOOL_INT32 nc_hy_searchDev(TOOL_VOID* param);

TOOL_INT32 nc_hy_login_v2(nc_dev* dev, NC_CB cb, TOOL_VOID* param);
TOOL_INT32 nc_hy_logout(TOOL_INT32 dev_id);


TOOL_INT32 nc_hy_getChannelNum_v2(nc_dev* dev);
TOOL_INT32 nc_hy_startRts_v2(nc_dev* dev, TOOL_INT32 channel, TOOL_INT32 stream_type, NC_CB cb, void* param);
TOOL_INT32 nc_hy_stopRts(TOOL_INT32 dev_id);

TOOL_INT32 nc_hy_controlPTZ_v2(nc_dev* dev, TOOL_INT32 channel, PTC_PTZ_CMD_E cmd, TOOL_INT32 param);

TOOL_INT32 nc_hy_searchRecords(nc_dev* dev, ptc_cfg_records_t* records);
TOOL_INT32 nc_hy_startPbs(nc_dev* dev, ptc_cfg_record_t* record, NC_CB cb, void* param);
TOOL_INT32 nc_hy_stopPbs(TOOL_INT32 user_id);

TOOL_INT32 nc_hy_searchLogs(nc_dev* dev, ptc_logs_t* logs);

TOOL_INT32 nc_hy_startTalk_v2(nc_dev* dev, NC_CB cb, void* param);
TOOL_INT32 nc_hy_stopTalk(TOOL_INT32 user_id);
TOOL_INT32 nc_hy_sendTalk(TOOL_INT32 user_id, TOOL_UINT8* buf, TOOL_INT32 len);

TOOL_INT32 nc_hy_getAbility_v2(nc_dev* dev, ptc_cfg_ability_t* ability);


TOOL_INT32 nc_hy_getDeviceStatus_v2(nc_dev* dev, ptc_cfg_status_t* status);
TOOL_INT32 nc_hy_setDeviceStatus_v2(nc_dev* dev, ptc_cfg_status_t* status);



TOOL_INT32 nc_hy_syncTime(nc_dev* dev, TOOL_UINT32 utc_sec, TOOL_INT32 zone, TOOL_INT32 dst_hour);

#endif

#if defined __cplusplus
}
#endif
