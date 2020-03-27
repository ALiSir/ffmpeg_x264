
#if defined __cplusplus
extern "C"
{
#endif

#ifndef __NC_H__
#define __NC_H__

#include "qh_ptc.h"


typedef enum
{
	NC_PTC_Qihan		= 0x00000000,
	NC_PTC_HuaYi		= 0x01000000,
	NC_PTC_YuShi		= 0x02000000,
	NC_PTC_YuShi_Cloud	= 0x03000000,
	NC_PTC_QiHan_Cloud	= 0x04000000,
	NC_PTC_XiongMai		= 0x05000000,

}NC_PTC_E;

/* nc_dev默认值，--表示无用
|---------------|-----------------------------------------------------------------------------------|-------------------|
|	protocol	|	id		ip				message_port	stream_port		username	password	|search		scan	|
|---------------|-----------------------------------------------------------------------------------|-------------------|
|	QiHan		|	--		192.168.1.10	5000			-----------		admin		NULL		|yes		no		|
|	HuaYi		|	--		192.168.1.88	8000			-----------		admin		NULL		|yes		no		|
|	YuShi		|	--		192.168.1.30	6060			7070			admin		123456		|yes		no		|
|	YuShi_Cloud	|	NULL	--				------------	-----------		admin		123456		|no			yes		|
|	Qihan_Cloud	|	NULL	--				------------	-----------		admin		NULL		|no			yes		|
|	XiongMai	|	--		192.168.1.10	34567			-----------		admin		NULL		|yes		no		|
|---------------|-----------------------------------------------------------------------------------|-------------------|
*/

typedef struct
{
	NC_PTC_E ptc;					//协议类型
	char dev_id[PTC_STR_SIZE];		//设备唯一标示，详见上表
	char ip[PTC_IP_SIZE];			//设备IP地址，详见上表
	int msg_port;					//消息端口，详见上表
	int stream_port;				//流端口，详见上表
	char user[PTC_ID_SIZE];			//用户名，详见上表
	char pswd[PTC_ID_SIZE];			//密码，详见上表
}nc_dev;

typedef void (*NC_CB)(int handle, void *data, unsigned int len, void* param);

#define NC_SUCCESS				(0)		//成功
#define NC_ERROR_UNINIT  		(-1)	//未初始化
#define NC_ERROR_INPUT			(-2)	//输入参数错误
#define NC_ERROR_FULL			(-3)	//超过最大用户数
#define NC_ERROR_INTERNAL		(-4)	//网络库内部错误
#define NC_ERROR_CONNECT		(-5)	//连接设备失败，请检查网络
#define NC_ERROR_SEND			(-6)	//发送消息失败
#define NC_ERROR_RECV			(-7)	//接受消息失败/设备繁忙，请稍后再试
#define NC_ERROR_PROTOCOL		(-8)	//通信协议错误
#define NC_ERROR_AUTH			(-9)	//用户名密码验证失败
#define NC_ERROR_REENTRANT		(-10)	//该函数不可重入
#define NC_ERROR_NOSPACE		(-11)	//输出参数空间不够
#define NC_ERROR_TIMEOUT		(-12)	//超时
#define NC_ERROR_NOSUPPORTED	(-13)	//该设备不支持此接口
#define NC_ERROR_MAXCONNECT		(-14)	//到达最大连接数
#define NC_ERROR_REPEAT			(-15)	//重复打开该通道
#define NC_ERROR_PTZNORTS		(-16)	//请先打开视频通道，再控制云台
#define NC_ERROR_OFFLINE		(-17)	//设备不在线
#define NC_ERROR_FULL_TOKEN		(-18)	//超过最大TOKEN数
#define NC_ERROR_ILLEGAL_ID		(-19)	//非法唯一标示
#define NC_ERROR_QPNS_CLOSED	(-20)	//未开启QPNS服务
#define NC_ERROR_QPNS_LOCATION	(-21)	//连接QPNS服务器失败
#define NC_ERROR_NO_LOGIN		(-22)	//用户未登陆


#define NC_ERROR_CLOUD_BEGIN				(-10000)
#define NC_ERROR_MOBILE_NUM					(-10001)	//手机号格式错误
#define NC_ERROR_CONFLICT_MOBILE			(-10002)	//手机号已经被注册
#define NC_ERROR_TODAY_COUNT				(-10003)	//达到每天请求短信验证码次数限制
#define NC_ERROR_TOO_FREQUENT				(-10004)	//请求短信验证码过于频繁；
#define NC_ERROR_PASSWORD_LEN				(-10005)	//密码长度错误
#define NC_ERROR_PASSWORD_CONTENT			(-10006)	//密码长度错误
#define NC_ERROR_USERNAME_LEN				(-10007)	//用户名长度过长
#define NC_ERROR_USERNAME_CONTENT			(-10008)	//用户名内容错误
#define NC_ERROR_REPEAT_USERNAME			(-10009)	//用户名已经被注册
#define NC_ERROR_NONE_CODE					(-10010)	//验证码不存在
#define NC_ERROR_WRONG_CODE 				(-10011)	//验证码不正确
#define NC_ERROR_CODE_LEN					(-10013)	//验证码长度错误
#define NC_ERROR_EMAIL_LEN					(-10014)	//邮箱地址长度错误
#define NC_ERROR_EMAIL_CONTENT				(-10015)	//邮箱地址内容错误
#define NC_ERROR_UDID_LEN					(-10016)	//手机标示长度错误
#define NC_ERROR_UDID_CONTENT				(-10017)	//手机标示内容错误
#define NC_ERROR_DEVID_LEN					(-10018)	//设备标示长度错误
#define NC_ERROR_DEVID_CONTENT				(-10019)	//设备标示内容错误
#define NC_ERROR_IP_LEN 					(-10020)	//网络地址长度错误
#define NC_ERROR_IP_CONTENT 				(-10021)	//网络地址内容错误
#define NC_ERROR_PORT_NAME_LEN				(-10022)	//端口列表名称长度错误
#define NC_ERROR_PORT_NAME_CONTENT			(-10023)	//端口列表名称内容错误
#define NC_ERROR_PORT_VLAUE_LEN 			(-10024)	//端口列表值长度错误
#define NC_ERROR_PORT_VLAUE_CONTENT 		(-10025)	//端口列表值内容错误
#define NC_ERROR_NONE_DEVID 				(-10026)	//设备标示不存在
#define NC_ERROR_NONE_USER					(-10027)	//用户名不存在
#define NC_ERROR_WRONG_PASSWORD 			(-10028)	//密码错误
#define NC_ERROR_REPEAT_DEVICE				(-10029)	//设备已经被其他用户绑定
#define NC_ERROR_BIND_ACTION_CONTENT		(-10030)	//绑定操作字段内容错误
#define NC_ERROR_DEV_NAME_LEN				(-10031)	//设备名称长度错误
#define NC_ERROR_MAX_DEVICE_NUM 			(-10032)	//达到最大设备数量限制
#define NC_ERROR_TIME_LEN					(-10033)	//报警消息时间长度错误
#define NC_ERROR_TIME_CONTENT				(-10034)	//报警消息时间内容错误
#define NC_ERROR_CHANNEL_LEN				(-10035)	//报警消息通道长度错误
#define NC_ERROR_CHANNEL_CONTENT			(-10036)	//报警消息通道内容错误
#define NC_ERROR_TYPE_LEN					(-10037)	//报警消息类型长度错误
#define NC_ERROR_TYPE_CONTENT				(-10038)	//报警消息类型内容错误
#define NC_ERROR_CONTENT_LEN				(-10039)	//报警消息内容长度错误
#define NC_ERROR_TOKEN_TYPE_LEN 			(-10040)	//QPNS映射手机类型长度错误
#define NC_ERROR_TOKEN_TYPE_CONTENT 		(-10041)	//QPNS映射手机类型内容错误
#define NC_ERROR_QPNS_IDLE					(-10042)	//QPNS服务未开启
#define NC_ERROR_NONE_TOKEN 				(-10043)	//用户标示不存在
#define NC_ERROR_ALARM_NO_LEN				(-10044)	//报警标示长度错误
#define NC_ERROR_ALARM_NO_CONTENT			(-10045)	//报警标示内容错误
#define NC_ERROR_MAP_ACTION_CONTENT 		(-10046)	//映射操作字段错误
#define NC_ERROR_LANGUAGE_LEN				(-10047)	//语言字段长度错误
#define NC_ERROR_LANGUAGE_CONTENT			(-10048)	//语言字段内容错误
#define NC_ERROR_REPEAT_EMAIL				(-10049)	//邮箱地址已经被注册
#define NC_ERROR_NONE_BINDING				(-10050)	//设备未绑定
#define NC_ERROR_NONE_AUTHORITY 			(-10051)	//没有权限
#define NC_ERROR_SMS_ACTION_CONTENT 		(-10052)	//请求短信验证码操作字段内容错误
#define NC_ERROR_USER_OR_PWSD				(-10053)	//用户名或者密码错误，认证失败
#define NC_ERROR_CODE_TIMEOUT				(-10054)	//验证码过期


#define NC_ERROR_CLOUD_END					(-19999)




// SDK初始化
void nc_init();
int nc_setBuffer(int buffer_usec);		//buffer_usec:0-3000*1000
void nc_done();

/* sps: 略过 0x00 0x00 0x00 0x01 0x67 */
int nc_ParseSPS(unsigned char* sps, int len, int* width, int* height);

// 搜索设备和登陆登出
int nc_searchDev(ptc_mcast_searchs_t* searchs);
int nc_auth_v2(nc_dev* dev);
int nc_login_v2(nc_dev* dev, NC_CB cb, void* param);
int nc_logout(int user_id);
int nc_getOnlineStatus(int user_id);
int nc_getChannelNum_v2(nc_dev* dev);


// 实时预览
/****************************************************************************************************************
 *	【功能】开始实时流
 *	【参数】
 *		dev: 输入参数，必须填写ptc
 *		channel: 输入参数，通道号，从0开始计算
 *		stream_type: 输入参数，通道类型，0=主码流，1=子码流
 *		cb: 输入参数，音视频数据回调函数
 *		param: 输入参数，回调函数参数，将伴随回调函数调用
 *	【返回值】0=成功，其他=错误，参考NC_ERROR_***宏定义
*****************************************************************************************************************/
int nc_startRts_v2(nc_dev* dev, int channel, int stream_type, NC_CB cb, void* param);
int nc_forceRtsKeyFrame(int rts_id);
int nc_stopRts(int rts_id);
int nc_getRtsStatus(int rts_id);
int nc_setRtsLevel(int rts_id, int level);	//level:1/2/3

// 云台控制、PIR控制
int nc_controlPTZ_v2(nc_dev* dev, int channel, PTC_PTZ_CMD_E cmd, int param);

// 语音对讲
int nc_startTalk_v2(nc_dev* dev, NC_CB cb, void* param);
int nc_stopTalk(int talk_id);
int nc_sendTalk(int talk_id, unsigned char* buf, int len);

// 参数配置
int nc_getAbility_v2(nc_dev* dev, ptc_cfg_ability_t* ability);

int nc_getDeviceStatus_v2(nc_dev* dev, ptc_cfg_status_t* status);
int nc_setDeviceStatus_v2(nc_dev* dev, ptc_cfg_status_t* status);

int nc_getCfg(nc_dev* dev, ptc_cfg_t* cfg);
int nc_getDefaultCfg(nc_dev* dev, ptc_cfg_t* cfg);
int nc_setCfg(nc_dev* dev, ptc_cfg_t* cfg);

int nc_syncTime(nc_dev* dev, unsigned int utc_sec, int zone, int dst_hour);		//zone:PTC_TIME_ZONE_W****, dst_hour:1,2,3

int nc_getCfg_alarm(nc_dev* dev, PTC_ALARM_TYPE_E alarm_type, ptc_cfg_alarm_t* alarm);
int nc_setCfg_alarm(nc_dev* dev, PTC_ALARM_TYPE_E alarm_type, ptc_cfg_alarm_t* alarm);

int nc_getCfg_workMode(nc_dev* dev, ptc_cfg_workMode_t* workMode);
int nc_setCfg_workMode(nc_dev* dev, ptc_cfg_workMode_t* workMode);

int nc_getCfg_mobile(nc_dev* dev, ptc_cfg_mobile_t* mobile);
int nc_setCfg_mobile(nc_dev* dev, ptc_cfg_mobile_t* mobile);

int nc_getCfg_wifi(nc_dev* dev, ptc_cfg_wifi_t* wifi);
int nc_setCfg_wifi(nc_dev* dev, ptc_cfg_wifi_t* wifi);

int nc_getCfg_devRecord(nc_dev* dev, ptc_cfg_devRecord_t* devRecord);
int nc_setCfg_devRecord(nc_dev* dev, ptc_cfg_devRecord_t* devRecord);

/****************************************************************************************************************
 *	【功能】查询图像参数
 *	【参数】
 *		dev: 输入参数，设备信息，填写方式如nc_dev所示
 *		av: 输出参数，图像参数
 *	【返回值】0=成功，其他=错误，参考NC_ERROR_***宏定义
*****************************************************************************************************************/
int nc_getCfg_av(nc_dev* dev, ptc_cfg_av_t* av);

/****************************************************************************************************************
 *	【功能】设置图像参数
 *	【参数】
 *		dev: 输入参数，设备信息，填写方式如nc_dev所示
 *		av: 输入参数，图像参数
 *	【返回值】0=成功，其他=错误，参考NC_ERROR_***宏定义
*****************************************************************************************************************/
int nc_setCfg_av(nc_dev* dev, ptc_cfg_av_t* av);



// 录像回放
int nc_searchDates(nc_dev* dev, ptc_cfg_dates_t* dates);
int nc_searchRecords(nc_dev* dev, ptc_cfg_records_t* records);
int nc_startPbs(nc_dev* dev, ptc_cfg_record_t* record, NC_CB cb, void* param);
int nc_setPbsPos(int pbs_id, unsigned int pos);
int nc_pausePbs(int pbs_id);
int nc_restartPbs(int pbs_id);
int nc_setPbsSpeed(int pbs_id, int speed);
int nc_stopPbs(int pbs_id);

// 日志搜索
int nc_searchLogs(nc_dev* dev, ptc_logs_t* logs);
int nc_searchLogs_v2(nc_dev* dev, ptc_logs_v2_t* logs_v2);


// 透明传送
int nc_sendData(nc_dev* dev, void* in_data, unsigned int in_len, void* out_data, unsigned int* out_len);

// 推送通知
int nc_startQpns(HDCCTV_QPNS_TOKEN_TYPE_E token_type, char* mobile_token, HDCCTV_LANUAGE_E language, NC_CB cb, void* param);
void nc_stopQpns();

// HDCCTV
int nc_getAlarms(HDCCTV_QPNS_TOKEN_TYPE_E token_type, char* mobile_token, int alarm_no, hdcctv_qpns_alarms_t* alarms);
int nc_getQpnsDeviceMaps(HDCCTV_QPNS_TOKEN_TYPE_E token_type, char* mobile_token, hdcctv_qpns_device_maps_t* device_maps);
int nc_setQpnsDeviceMaps(HDCCTV_QPNS_TOKEN_TYPE_E token_type, char* mobile_token, hdcctv_qpns_device_maps_t* device_maps);
int nc_addQpnsDeviceMaps(HDCCTV_QPNS_TOKEN_TYPE_E token_type, char* mobile_token, hdcctv_qpns_device_maps_t* device_maps);
int nc_delQpnsDeviceMaps(HDCCTV_QPNS_TOKEN_TYPE_E token_type, char* mobile_token, hdcctv_qpns_device_maps_t* device_maps);
int nc_testQpns(HDCCTV_QPNS_TOKEN_TYPE_E token_type, char* mobile_token, char* dev_id);

/****************************************************************************************************************
 *	【功能】获取短信验证码
 *	【参数】
 *		pst_user: 输入参数，必须填写mobile
 *	【返回值】0=成功，其他=错误，参考NC_ERROR_***宏定义
*****************************************************************************************************************/
int nc_getSms_reg(hdcctv_cloud_user_t* pst_cloud_user);

/****************************************************************************************************************
 *	【功能】注册用户
 *	【参数】
 *		pst_user: 输入参数，必须填写username password mobile email
 *		code: 输入参数，短信验证码
 *	【返回值】0=成功，其他=错误，参考NC_ERROR_***宏定义
*****************************************************************************************************************/
int nc_regUser(hdcctv_cloud_user_t* pst_cloud_user, char* code);

/****************************************************************************************************************
 *	【功能】获取短信验证码
 *	【参数】
 *		pst_user: 输入参数，必须填写username或者mobile或者email
 *	【返回值】0=成功，其他=错误，参考NC_ERROR_***宏定义
*****************************************************************************************************************/
int nc_getSms_pswd(hdcctv_cloud_user_t* pst_cloud_user);

/****************************************************************************************************************
 *	【功能】找回密码
 *	【参数】
 *		pst_user: 输入参数，必须填写username或者mobile或者email，必须填写password
 *		code: 输入参数，短信验证码
 *	【返回值】0=成功，其他=错误，参考NC_ERROR_***宏定义
*****************************************************************************************************************/
int nc_resetPswd(hdcctv_cloud_user_t* pst_cloud_user, char* code);

/****************************************************************************************************************
 *	【功能】登陆
 *	【参数】 
 *		pst_user: 输入参数，必须填写username或者mobile或者email，必须填写password
 *	【返回值】0=成功，其他=错误，参考NC_ERROR_***宏定义
*****************************************************************************************************************/
int nc_loginUser(hdcctv_cloud_user_t* pst_cloud_user);

/****************************************************************************************************************
 *	【功能】注销
 *	【参数】空
 *	【返回值】0=成功，其他=错误，参考NC_ERROR_***宏定义
*****************************************************************************************************************/
int nc_logoutUser();

/****************************************************************************************************************
 *	【功能】修改密码
 *	【参数】
 *		pst_user: 输入参数，必须填写username或者mobile或者email，必须填写password
 *		new_pswd: 输入参数，新密码
 *	【返回值】0=成功，其他=错误，参考NC_ERROR_***宏定义
*****************************************************************************************************************/
int nc_setPswd(hdcctv_cloud_user_t* pst_cloud_user, char* new_pswd);

/****************************************************************************************************************
 *	【功能】添加设备
 *	【参数】
 *		pst_user: 输入参数，必须填写username或者mobile或者email，必须填写password
 *		pst_device: 输入参数，必须填写dev_id dev_name
 *	【返回值】0=成功，其他=错误，参考NC_ERROR_***宏定义
*****************************************************************************************************************/
int nc_addDev(hdcctv_cloud_user_t* pst_cloud_user, hdcctv_device_t* pst_device);

/****************************************************************************************************************
 *	【功能】删除设备
 *	【参数】
 *		pst_user: 输入参数，必须填写username或者mobile或者email，必须填写password
 *		pst_device: 输入参数，必须填写dev_id dev_name
 *	【返回值】0=成功，其他=错误，参考NC_ERROR_***宏定义
*****************************************************************************************************************/
int nc_delDev(hdcctv_cloud_user_t* pst_cloud_user, hdcctv_device_t* pst_device);

/****************************************************************************************************************
 *	【功能】获取设备列表
 *	【参数】
 *		pst_user: 输入参数，必须填写username或者mobile或者email，必须填写password
 				  输出参数，username和mobile和email
 *		pst_device: 输出参数
 *	【返回值】0=成功，其他=错误，参考NC_ERROR_***宏定义
*****************************************************************************************************************/
int nc_getDevList(hdcctv_cloud_user_t* pst_cloud_user, hdcctv_device_list_t* pst_device_list);

/****************************************************************************************************************
 *	【功能】查询云端流量和空间
 *	【参数】
 *		pst_user: 输入参数，必须填写username或者mobile或者email，必须填写password
 *		pst_cloud_cfg: 输出参数，云端流量和空间参数
 *	【返回值】0=成功，其他=错误，参考NC_ERROR_***宏定义
*****************************************************************************************************************/
int nc_getCloudCfg(hdcctv_cloud_user_t* pst_cloud_user, char* dev_id, hdcctv_cloud_cfg_t* pst_cloud_cfg);

/****************************************************************************************************************
 *	【功能】查询云端流量和空间
 *	【参数】
 *		pst_user: 输入参数，必须填写username或者mobile或者email，必须填写password
 *		pst_cloud_cfg: 输入参数，云端流量和空间参数
 *	【返回值】0=成功，其他=错误，参考NC_ERROR_***宏定义
*****************************************************************************************************************/
int nc_setCloudCfg(hdcctv_cloud_user_t* pst_cloud_user, char* dev_id, hdcctv_cloud_cfg_t* pst_cloud_cfg);

/****************************************************************************************************************
 *	【功能】查询云端录像文件
 *	【参数】
 *		dev: 输入参数，必须填写ptc,dev_id,user,pswd
 *		dates: 输出参数，num和date
 *	【返回值】0=成功，其他=错误，参考NC_ERROR_***宏定义
*****************************************************************************************************************/
int nc_searchCloudDates(nc_dev* dev, ptc_cfg_dates_t* dates);

/****************************************************************************************************************
 *	【功能】查询云端录像文件
 *	【参数】
 *		dev: 输入参数，必须填写ptc,dev_id,user,pswd
 *		records: 输入参数，必须填写start_time,end_time,channel,type,sub_type
 				 输出参数，num和record
 *	【返回值】0=成功，其他=错误，参考NC_ERROR_***宏定义
*****************************************************************************************************************/
int nc_searchCloudRecords(nc_dev* dev, ptc_cfg_records_t* records);

/****************************************************************************************************************
 *	【功能】开始云端录像回放
 *	【参数】
 *		dev: 输入参数，必须填写ptc,dev_id,user,pswd
 *		record: 输入参数，录像文件
 *		cb: 输入参数，回调函数
 *		param: 输入参数，回调参数
 *	【返回值】大于等于0=回放句柄，其他=错误，参考NC_ERROR_***宏定义
*****************************************************************************************************************/
int nc_startCloudPbs(nc_dev* dev, ptc_cfg_record_t* record, NC_CB cb, void* param);

/****************************************************************************************************************
 *	【功能】停止云端录像回放
 *	【参数】
 *		pbs_id: 输入参数，回放句柄
 *	【返回值】0=成功，其他=错误，参考NC_ERROR_***宏定义
*****************************************************************************************************************/
int nc_stopCloudPbs(int pbs_id);


#endif

#if defined __cplusplus
}
#endif

