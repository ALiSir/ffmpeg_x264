
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

/* nc_devĬ��ֵ��--��ʾ����
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
	NC_PTC_E ptc;					//Э������
	char dev_id[PTC_STR_SIZE];		//�豸Ψһ��ʾ������ϱ�
	char ip[PTC_IP_SIZE];			//�豸IP��ַ������ϱ�
	int msg_port;					//��Ϣ�˿ڣ�����ϱ�
	int stream_port;				//���˿ڣ�����ϱ�
	char user[PTC_ID_SIZE];			//�û���������ϱ�
	char pswd[PTC_ID_SIZE];			//���룬����ϱ�
}nc_dev;

typedef void (*NC_CB)(int handle, void *data, unsigned int len, void* param);

#define NC_SUCCESS				(0)		//�ɹ�
#define NC_ERROR_UNINIT  		(-1)	//δ��ʼ��
#define NC_ERROR_INPUT			(-2)	//�����������
#define NC_ERROR_FULL			(-3)	//��������û���
#define NC_ERROR_INTERNAL		(-4)	//������ڲ�����
#define NC_ERROR_CONNECT		(-5)	//�����豸ʧ�ܣ���������
#define NC_ERROR_SEND			(-6)	//������Ϣʧ��
#define NC_ERROR_RECV			(-7)	//������Ϣʧ��/�豸��æ�����Ժ�����
#define NC_ERROR_PROTOCOL		(-8)	//ͨ��Э�����
#define NC_ERROR_AUTH			(-9)	//�û���������֤ʧ��
#define NC_ERROR_REENTRANT		(-10)	//�ú�����������
#define NC_ERROR_NOSPACE		(-11)	//��������ռ䲻��
#define NC_ERROR_TIMEOUT		(-12)	//��ʱ
#define NC_ERROR_NOSUPPORTED	(-13)	//���豸��֧�ִ˽ӿ�
#define NC_ERROR_MAXCONNECT		(-14)	//�������������
#define NC_ERROR_REPEAT			(-15)	//�ظ��򿪸�ͨ��
#define NC_ERROR_PTZNORTS		(-16)	//���ȴ���Ƶͨ�����ٿ�����̨
#define NC_ERROR_OFFLINE		(-17)	//�豸������
#define NC_ERROR_FULL_TOKEN		(-18)	//�������TOKEN��
#define NC_ERROR_ILLEGAL_ID		(-19)	//�Ƿ�Ψһ��ʾ
#define NC_ERROR_QPNS_CLOSED	(-20)	//δ����QPNS����
#define NC_ERROR_QPNS_LOCATION	(-21)	//����QPNS������ʧ��
#define NC_ERROR_NO_LOGIN		(-22)	//�û�δ��½


#define NC_ERROR_CLOUD_BEGIN				(-10000)
#define NC_ERROR_MOBILE_NUM					(-10001)	//�ֻ��Ÿ�ʽ����
#define NC_ERROR_CONFLICT_MOBILE			(-10002)	//�ֻ����Ѿ���ע��
#define NC_ERROR_TODAY_COUNT				(-10003)	//�ﵽÿ�����������֤���������
#define NC_ERROR_TOO_FREQUENT				(-10004)	//���������֤�����Ƶ����
#define NC_ERROR_PASSWORD_LEN				(-10005)	//���볤�ȴ���
#define NC_ERROR_PASSWORD_CONTENT			(-10006)	//���볤�ȴ���
#define NC_ERROR_USERNAME_LEN				(-10007)	//�û������ȹ���
#define NC_ERROR_USERNAME_CONTENT			(-10008)	//�û������ݴ���
#define NC_ERROR_REPEAT_USERNAME			(-10009)	//�û����Ѿ���ע��
#define NC_ERROR_NONE_CODE					(-10010)	//��֤�벻����
#define NC_ERROR_WRONG_CODE 				(-10011)	//��֤�벻��ȷ
#define NC_ERROR_CODE_LEN					(-10013)	//��֤�볤�ȴ���
#define NC_ERROR_EMAIL_LEN					(-10014)	//�����ַ���ȴ���
#define NC_ERROR_EMAIL_CONTENT				(-10015)	//�����ַ���ݴ���
#define NC_ERROR_UDID_LEN					(-10016)	//�ֻ���ʾ���ȴ���
#define NC_ERROR_UDID_CONTENT				(-10017)	//�ֻ���ʾ���ݴ���
#define NC_ERROR_DEVID_LEN					(-10018)	//�豸��ʾ���ȴ���
#define NC_ERROR_DEVID_CONTENT				(-10019)	//�豸��ʾ���ݴ���
#define NC_ERROR_IP_LEN 					(-10020)	//�����ַ���ȴ���
#define NC_ERROR_IP_CONTENT 				(-10021)	//�����ַ���ݴ���
#define NC_ERROR_PORT_NAME_LEN				(-10022)	//�˿��б����Ƴ��ȴ���
#define NC_ERROR_PORT_NAME_CONTENT			(-10023)	//�˿��б��������ݴ���
#define NC_ERROR_PORT_VLAUE_LEN 			(-10024)	//�˿��б�ֵ���ȴ���
#define NC_ERROR_PORT_VLAUE_CONTENT 		(-10025)	//�˿��б�ֵ���ݴ���
#define NC_ERROR_NONE_DEVID 				(-10026)	//�豸��ʾ������
#define NC_ERROR_NONE_USER					(-10027)	//�û���������
#define NC_ERROR_WRONG_PASSWORD 			(-10028)	//�������
#define NC_ERROR_REPEAT_DEVICE				(-10029)	//�豸�Ѿ��������û���
#define NC_ERROR_BIND_ACTION_CONTENT		(-10030)	//�󶨲����ֶ����ݴ���
#define NC_ERROR_DEV_NAME_LEN				(-10031)	//�豸���Ƴ��ȴ���
#define NC_ERROR_MAX_DEVICE_NUM 			(-10032)	//�ﵽ����豸��������
#define NC_ERROR_TIME_LEN					(-10033)	//������Ϣʱ�䳤�ȴ���
#define NC_ERROR_TIME_CONTENT				(-10034)	//������Ϣʱ�����ݴ���
#define NC_ERROR_CHANNEL_LEN				(-10035)	//������Ϣͨ�����ȴ���
#define NC_ERROR_CHANNEL_CONTENT			(-10036)	//������Ϣͨ�����ݴ���
#define NC_ERROR_TYPE_LEN					(-10037)	//������Ϣ���ͳ��ȴ���
#define NC_ERROR_TYPE_CONTENT				(-10038)	//������Ϣ�������ݴ���
#define NC_ERROR_CONTENT_LEN				(-10039)	//������Ϣ���ݳ��ȴ���
#define NC_ERROR_TOKEN_TYPE_LEN 			(-10040)	//QPNSӳ���ֻ����ͳ��ȴ���
#define NC_ERROR_TOKEN_TYPE_CONTENT 		(-10041)	//QPNSӳ���ֻ��������ݴ���
#define NC_ERROR_QPNS_IDLE					(-10042)	//QPNS����δ����
#define NC_ERROR_NONE_TOKEN 				(-10043)	//�û���ʾ������
#define NC_ERROR_ALARM_NO_LEN				(-10044)	//������ʾ���ȴ���
#define NC_ERROR_ALARM_NO_CONTENT			(-10045)	//������ʾ���ݴ���
#define NC_ERROR_MAP_ACTION_CONTENT 		(-10046)	//ӳ������ֶδ���
#define NC_ERROR_LANGUAGE_LEN				(-10047)	//�����ֶγ��ȴ���
#define NC_ERROR_LANGUAGE_CONTENT			(-10048)	//�����ֶ����ݴ���
#define NC_ERROR_REPEAT_EMAIL				(-10049)	//�����ַ�Ѿ���ע��
#define NC_ERROR_NONE_BINDING				(-10050)	//�豸δ��
#define NC_ERROR_NONE_AUTHORITY 			(-10051)	//û��Ȩ��
#define NC_ERROR_SMS_ACTION_CONTENT 		(-10052)	//���������֤������ֶ����ݴ���
#define NC_ERROR_USER_OR_PWSD				(-10053)	//�û����������������֤ʧ��
#define NC_ERROR_CODE_TIMEOUT				(-10054)	//��֤�����


#define NC_ERROR_CLOUD_END					(-19999)




// SDK��ʼ��
void nc_init();
int nc_setBuffer(int buffer_usec);		//buffer_usec:0-3000*1000
void nc_done();

/* sps: �Թ� 0x00 0x00 0x00 0x01 0x67 */
int nc_ParseSPS(unsigned char* sps, int len, int* width, int* height);

// �����豸�͵�½�ǳ�
int nc_searchDev(ptc_mcast_searchs_t* searchs);
int nc_auth_v2(nc_dev* dev);
int nc_login_v2(nc_dev* dev, NC_CB cb, void* param);
int nc_logout(int user_id);
int nc_getOnlineStatus(int user_id);
int nc_getChannelNum_v2(nc_dev* dev);


// ʵʱԤ��
/****************************************************************************************************************
 *	�����ܡ���ʼʵʱ��
 *	��������
 *		dev: ���������������дptc
 *		channel: ���������ͨ���ţ���0��ʼ����
 *		stream_type: ���������ͨ�����ͣ�0=��������1=������
 *		cb: �������������Ƶ���ݻص�����
 *		param: ����������ص�����������������ص���������
 *	������ֵ��0=�ɹ�������=���󣬲ο�NC_ERROR_***�궨��
*****************************************************************************************************************/
int nc_startRts_v2(nc_dev* dev, int channel, int stream_type, NC_CB cb, void* param);
int nc_forceRtsKeyFrame(int rts_id);
int nc_stopRts(int rts_id);
int nc_getRtsStatus(int rts_id);
int nc_setRtsLevel(int rts_id, int level);	//level:1/2/3

// ��̨���ơ�PIR����
int nc_controlPTZ_v2(nc_dev* dev, int channel, PTC_PTZ_CMD_E cmd, int param);

// �����Խ�
int nc_startTalk_v2(nc_dev* dev, NC_CB cb, void* param);
int nc_stopTalk(int talk_id);
int nc_sendTalk(int talk_id, unsigned char* buf, int len);

// ��������
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
 *	�����ܡ���ѯͼ�����
 *	��������
 *		dev: ����������豸��Ϣ����д��ʽ��nc_dev��ʾ
 *		av: ���������ͼ�����
 *	������ֵ��0=�ɹ�������=���󣬲ο�NC_ERROR_***�궨��
*****************************************************************************************************************/
int nc_getCfg_av(nc_dev* dev, ptc_cfg_av_t* av);

/****************************************************************************************************************
 *	�����ܡ�����ͼ�����
 *	��������
 *		dev: ����������豸��Ϣ����д��ʽ��nc_dev��ʾ
 *		av: ���������ͼ�����
 *	������ֵ��0=�ɹ�������=���󣬲ο�NC_ERROR_***�궨��
*****************************************************************************************************************/
int nc_setCfg_av(nc_dev* dev, ptc_cfg_av_t* av);



// ¼��ط�
int nc_searchDates(nc_dev* dev, ptc_cfg_dates_t* dates);
int nc_searchRecords(nc_dev* dev, ptc_cfg_records_t* records);
int nc_startPbs(nc_dev* dev, ptc_cfg_record_t* record, NC_CB cb, void* param);
int nc_setPbsPos(int pbs_id, unsigned int pos);
int nc_pausePbs(int pbs_id);
int nc_restartPbs(int pbs_id);
int nc_setPbsSpeed(int pbs_id, int speed);
int nc_stopPbs(int pbs_id);

// ��־����
int nc_searchLogs(nc_dev* dev, ptc_logs_t* logs);
int nc_searchLogs_v2(nc_dev* dev, ptc_logs_v2_t* logs_v2);


// ͸������
int nc_sendData(nc_dev* dev, void* in_data, unsigned int in_len, void* out_data, unsigned int* out_len);

// ����֪ͨ
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
 *	�����ܡ���ȡ������֤��
 *	��������
 *		pst_user: ���������������дmobile
 *	������ֵ��0=�ɹ�������=���󣬲ο�NC_ERROR_***�궨��
*****************************************************************************************************************/
int nc_getSms_reg(hdcctv_cloud_user_t* pst_cloud_user);

/****************************************************************************************************************
 *	�����ܡ�ע���û�
 *	��������
 *		pst_user: ���������������дusername password mobile email
 *		code: ���������������֤��
 *	������ֵ��0=�ɹ�������=���󣬲ο�NC_ERROR_***�궨��
*****************************************************************************************************************/
int nc_regUser(hdcctv_cloud_user_t* pst_cloud_user, char* code);

/****************************************************************************************************************
 *	�����ܡ���ȡ������֤��
 *	��������
 *		pst_user: ���������������дusername����mobile����email
 *	������ֵ��0=�ɹ�������=���󣬲ο�NC_ERROR_***�궨��
*****************************************************************************************************************/
int nc_getSms_pswd(hdcctv_cloud_user_t* pst_cloud_user);

/****************************************************************************************************************
 *	�����ܡ��һ�����
 *	��������
 *		pst_user: ���������������дusername����mobile����email��������дpassword
 *		code: ���������������֤��
 *	������ֵ��0=�ɹ�������=���󣬲ο�NC_ERROR_***�궨��
*****************************************************************************************************************/
int nc_resetPswd(hdcctv_cloud_user_t* pst_cloud_user, char* code);

/****************************************************************************************************************
 *	�����ܡ���½
 *	�������� 
 *		pst_user: ���������������дusername����mobile����email��������дpassword
 *	������ֵ��0=�ɹ�������=���󣬲ο�NC_ERROR_***�궨��
*****************************************************************************************************************/
int nc_loginUser(hdcctv_cloud_user_t* pst_cloud_user);

/****************************************************************************************************************
 *	�����ܡ�ע��
 *	����������
 *	������ֵ��0=�ɹ�������=���󣬲ο�NC_ERROR_***�궨��
*****************************************************************************************************************/
int nc_logoutUser();

/****************************************************************************************************************
 *	�����ܡ��޸�����
 *	��������
 *		pst_user: ���������������дusername����mobile����email��������дpassword
 *		new_pswd: ���������������
 *	������ֵ��0=�ɹ�������=���󣬲ο�NC_ERROR_***�궨��
*****************************************************************************************************************/
int nc_setPswd(hdcctv_cloud_user_t* pst_cloud_user, char* new_pswd);

/****************************************************************************************************************
 *	�����ܡ�����豸
 *	��������
 *		pst_user: ���������������дusername����mobile����email��������дpassword
 *		pst_device: ���������������дdev_id dev_name
 *	������ֵ��0=�ɹ�������=���󣬲ο�NC_ERROR_***�궨��
*****************************************************************************************************************/
int nc_addDev(hdcctv_cloud_user_t* pst_cloud_user, hdcctv_device_t* pst_device);

/****************************************************************************************************************
 *	�����ܡ�ɾ���豸
 *	��������
 *		pst_user: ���������������дusername����mobile����email��������дpassword
 *		pst_device: ���������������дdev_id dev_name
 *	������ֵ��0=�ɹ�������=���󣬲ο�NC_ERROR_***�궨��
*****************************************************************************************************************/
int nc_delDev(hdcctv_cloud_user_t* pst_cloud_user, hdcctv_device_t* pst_device);

/****************************************************************************************************************
 *	�����ܡ���ȡ�豸�б�
 *	��������
 *		pst_user: ���������������дusername����mobile����email��������дpassword
 				  ���������username��mobile��email
 *		pst_device: �������
 *	������ֵ��0=�ɹ�������=���󣬲ο�NC_ERROR_***�궨��
*****************************************************************************************************************/
int nc_getDevList(hdcctv_cloud_user_t* pst_cloud_user, hdcctv_device_list_t* pst_device_list);

/****************************************************************************************************************
 *	�����ܡ���ѯ�ƶ������Ϳռ�
 *	��������
 *		pst_user: ���������������дusername����mobile����email��������дpassword
 *		pst_cloud_cfg: ����������ƶ������Ϳռ����
 *	������ֵ��0=�ɹ�������=���󣬲ο�NC_ERROR_***�궨��
*****************************************************************************************************************/
int nc_getCloudCfg(hdcctv_cloud_user_t* pst_cloud_user, char* dev_id, hdcctv_cloud_cfg_t* pst_cloud_cfg);

/****************************************************************************************************************
 *	�����ܡ���ѯ�ƶ������Ϳռ�
 *	��������
 *		pst_user: ���������������дusername����mobile����email��������дpassword
 *		pst_cloud_cfg: ����������ƶ������Ϳռ����
 *	������ֵ��0=�ɹ�������=���󣬲ο�NC_ERROR_***�궨��
*****************************************************************************************************************/
int nc_setCloudCfg(hdcctv_cloud_user_t* pst_cloud_user, char* dev_id, hdcctv_cloud_cfg_t* pst_cloud_cfg);

/****************************************************************************************************************
 *	�����ܡ���ѯ�ƶ�¼���ļ�
 *	��������
 *		dev: ���������������дptc,dev_id,user,pswd
 *		dates: ���������num��date
 *	������ֵ��0=�ɹ�������=���󣬲ο�NC_ERROR_***�궨��
*****************************************************************************************************************/
int nc_searchCloudDates(nc_dev* dev, ptc_cfg_dates_t* dates);

/****************************************************************************************************************
 *	�����ܡ���ѯ�ƶ�¼���ļ�
 *	��������
 *		dev: ���������������дptc,dev_id,user,pswd
 *		records: ���������������дstart_time,end_time,channel,type,sub_type
 				 ���������num��record
 *	������ֵ��0=�ɹ�������=���󣬲ο�NC_ERROR_***�궨��
*****************************************************************************************************************/
int nc_searchCloudRecords(nc_dev* dev, ptc_cfg_records_t* records);

/****************************************************************************************************************
 *	�����ܡ���ʼ�ƶ�¼��ط�
 *	��������
 *		dev: ���������������дptc,dev_id,user,pswd
 *		record: ���������¼���ļ�
 *		cb: ����������ص�����
 *		param: ����������ص�����
 *	������ֵ�����ڵ���0=�طž��������=���󣬲ο�NC_ERROR_***�궨��
*****************************************************************************************************************/
int nc_startCloudPbs(nc_dev* dev, ptc_cfg_record_t* record, NC_CB cb, void* param);

/****************************************************************************************************************
 *	�����ܡ�ֹͣ�ƶ�¼��ط�
 *	��������
 *		pbs_id: ����������طž��
 *	������ֵ��0=�ɹ�������=���󣬲ο�NC_ERROR_***�궨��
*****************************************************************************************************************/
int nc_stopCloudPbs(int pbs_id);


#endif

#if defined __cplusplus
}
#endif

