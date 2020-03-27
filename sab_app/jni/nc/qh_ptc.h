
#if defined __cplusplus
extern "C"
{
#endif

#ifndef __QH_PTC_H__
#define __QH_PTC_H__

#define PTC_VERSION		0x2223
#define PTC_DATA		0x0720

/*
	0x2223: 2015/07/20
(1) ���NS-QHЭ��MOBIPC�豸��ʼ�Խ��ͽ����Խ�ʱ���ÿ��ؽӿں�����
(2) ���HDCCTVЭ�鱨��������¼����¼��طŹ��ܡ������ù��ܣ�
(3) �޸�HDCCTV-CLINETЭ����δ��½ʱ���û�ȡ�豸�б�ӿ��������⣻
(4) ���Զ��¼�����ڹ��ܣ�
(5) �Ż�HDCCTVЭ���лط�Զ���ٶȹ������⣻

	0x2222: 2015/06/08
(1) ͬ������WIN32���뻷����
(2) �Ż�NS-QHЭ�飬����ѹ������relay��������msgs������ģʽ��
(3) ���NS-XMЭ�飻
(4) ���HDCCTVЭ���в�ѯ������WiFi������AV������¼�������
(5) ���NC-QHЭ��������ָ���豸�ӿ�nc_searchOneDev();
(6) ���HDCCTVЭ���б���ֱ�����ƶ˻������Զ��л�ģʽ��
(7) ���HDCCTV-CLIENTЭ��ص�״̬֡��
(8) ���HDCCTVЭ��֧�����������е��ֻ����ʶ��������е��豸���ܣ�
(9) ���HDCCTV-CLIENTЭ��֧��nc_loginUser()��nc_logoutUser()�ӿڣ�
(10)�޸�HDCCTV-CLIENTЭ���NCЭ�������ò�������Ϊͬ��������
(11)�Ż�HDCCTV-DEVICEЭ�飬��������Ϣ�޸�Ϊ�첽ģʽ��
(12)�Ż�HDCCTV-SERVERЭ�飬���̳߳��߳������ӵ�100�����豸�����ٵ�1000��
(13)�Ż�HDCCTV_SERVER�����ڷ��ͱ�����ϢʱCPU�������⣻
(14)�Ż�HDCCTV-CLIENTЭ�飬����豸wifiapģʽ��û�����ص������������豸��
	�޷��������⣻
(15)���HDCCTV-CLIENTЭ���еĳ�ʱ״̬֡�Ͷ�λ״̬֡��
(16)�޸�NC-HYЭ���л�ȡͨ�������������⣻
(17)�Ż�NC-QHЭ���HDCCTV_CLIENTЭ�飬�ڶԽ�ͨ����ռ��ʱ�ص�״̬֡��
(18)�޸�NC-HDCCTVЭ���е��û�ȡ�豸�б�ʱ������󷵻�����������⣻

	0x2221: 2015/05/22
(1) �޸�HDCCTVЭ����nc_getAbility_v2()�޷���ȡdev_id���⣻
(2) �޸�HDCCTVЭ�����豸�������������⣻
(3) �޸�QPNSЭ�����޷�����Android�ͻ��˱������⣻
(4) ���HDCCTVЭ�������Խ����ܡ�¼��طŹ��ܣ�
(5) ���NS-QHЭ�飻
(6) �Ż�NC-HY,NC-YS��NC-XMЭ�����豸�������⣻
(7) �޸��Խ���stream2����δע�����⣻
(8) �޸�ffmpeg������Ƶ֡����-1ʱ���޷�������������⣻
(9) �޸�NC-HY,NC-YS��NC-XMЭ�����ж���ͬ�豸ʱû�бȽ��û������������⣻
(10)�޸�NC-YSЭ����ͬ��ʱ��ʱû����������ʱ���⣻

	0x2220: 2015/05/07
(1) �ع�HDCCTV-SERVERЭ����poll�߼���android����״̬������ƣ�
(2) ���HDCCTV-SERVERЭ���ж�ios-idarling�ͻ���֧�֣�
(3) ���HDCCTVЭ���ж���Ϣת��������֧�֣�

	0x2210: 2015/04/20
(1)	�����ģ��: ��ȡע����֤�롢ע���û�����ȡ�һ�������֤�롢�һ����롢
	�޸����롢��/����豸����ȡ�豸�б���ʼ����Ƶ��ֹͣ����Ƶ��
(2) �Ż���ʼ/ֹͣ����Ƶ�ӿڣ�֧������+���룬�ֻ���+�����½��
(3) �Ż���λ�ӿڣ�����������www.hdcctvddns.comʧ��֮���ٴ����ӹ̶�IP��ַ
	208.109.240.232��
(4) �Ż������豸�ӿڣ���Ϊ���̲߳������������ٵȴ�ʱ�䣻
(5) ���NC-XMЭ�飬֧�������豸��ʵʱ������̨���ơ�¼��������¼��طš���ѯ
	��־�������Խ���ͬ��ʱ�䡢���ñ��������
	���в�ѯ��־�ӿڵ���Ϊnc_searchLogs_v2��
(6) ɾ��FFMPEGģ���еĴ�ӡ��Ϣ��
(7) �Ż�NC-XMЭ�飬�ڲ�ѯ¼���ļ����Ϊ0ʱ�����ߣ�
(8) �޸�tool_json.h��json2��ѯnameʱ�ַ��ص����½���쳣���⣻
(9) ��HDCCTV-DEVICEЭ���У��ϴ�ʵʱ������Ƶ֡ʱ��֡ͷ�����ͨ�����ͺ�ͨ���ţ�
(10)�޸�nc_searchLogs_v2()�ӿڲ�ѯIPC��־Ϊ�յ����⣻
(11)��NC-XMЭ���У����֧��G711U��Ƶ��ʽ��
(12)�޸�tool_sysf_getsockname()�����У����ڲ���portΪNULL���������⣻
(13)�Ż�NC-XMЭ�飬����4��û�н��ܵ�����֡���ϱ�����֡��
(14)�޸������豸�ӿ���XM�豸IP��ַΪ255.255.255.127���⣻
(15)ͳһ�汾��Ϣ�ʹ��ʱ�䣻

	0x2202: 2015/03/02
(1) �޸�nc_getAlarms()�������no�ɷ�����ά����
(2) ����tool_sock_t����API��������mcast��bcast��udp�з��������
(3) ��tool_sysf_initAddr()�������ж�ip�Ƿ�ipv4�������������getaddrinfo()��
(4) �޸�tool_sysf_initAddr()��������֮��tool_sock_item_t����tickδ�������⣻
(5) �޸�������Ϣ��Ƶ��������ʧ�����⣻
(6) ���QH-IPCʵʱ��ֹ֮ͣ����Ȼͣ������һ����������⣻
(7) �׳�nc_ParseSPS()�ӿڣ����ϲ�APP���H264��Ƶ֡�ķֱ��ʣ�

	0x2201: 2015/03/02
(1) ���HDCCTV-QPNSģ�飬ʹ�÷���:
	a. ��������ʱ����nc_startQpns()�������������֪ͨ����QPNS;
	b. ����豸ʱ����nc_getAbility_v2()���õ��豸Ψһ��ʾ
	    ptc_cfg_ability_t.ptc_ability_base_t.dev_id��
	c. ����nc_getQpnsDeviceMaps()��nc_setQpnsDeviceMaps()����ѯ/���õ�̨�豸
	   QPNS����
	d. ǰ���豸���ͱ���ʱ��androidͨ��nc_startQpns()�ӿڵĵ����������ص�����
	   �ϱ���Ӧ�ò㣬iosͨ��ƻ������֪ͨ�����ϱ���iosϵͳ��
	e. ios����nc_responseReadCount()���Ǳ����㣻
	f. ����nc_testQpns()����QPNS����
	g. ����nc_stopQpns()ֹͣQPNS����
(2) ��������makefile�ļ���ɾ������ģ�飻
(3) ���ptc_frame_head_t֡ͷ�ṹ���е�padding_bytes�ֶΣ�
(4) ���QPNS��������t_alarm���Զ������ܣ�
(5) ���QPNS��������t_qpns_map���Զ������ܣ�
(6) ���nc_addQpnsDeviceMaps()�ӿں�nc_delQpnsDeviceMaps()�ӿڣ�
(7) ʹ��nc_getAlarms()�滻nc_responseReadCount()�ӿڣ�
(8) ���nc_getAlarms_divide()
		nc_getQpnsDeviceMaps_divide()
        nc_setQpnsDeviceMaps_divide()
        nc_addQpnsDeviceMaps_divide()
        nc_delQpnsDeviceMaps_divide()
        nc_testQpns_divide()�ӿڣ�
(9) ����IPC�豸��ѯ��������ӿ���MAC��ַ����ȷ���⣻       
(10)���֧��Mobile-IPC�豸��
(11)���TOOL-LOG���߽���־��Ϣд���ļ����ܣ�

	0x2103: 2015/01/19
(1) ����ffmpegģ��ı������ơ��������ơ��ļ����ƣ��Է���ͻ��
(2) ���NC-YSЭ���н��ֻ�ܲ��ŵ�ͨ�����⣻
(3) ���NC-YSЭ����Ʊ�������ڣ�
(4) ���NC-YSЭ������豸ʱ������󷵻��������-7
(5) ���NC-YSЭ��������־��ʾ������󣬴�����Ϊ-6
(6) ���NC-YSЭ����Ƶͨ���ϱ���Ϣ֡��
(7) ���NC-YSЭ��Զ��¼��طſ����ѵ����������������⣻
(8) ���NC-YS-CloudЭ�飻
(9) ���NC-YSЭ��ʵʱ��Ƶ�ص��ӿڵ�Handle�����������⣻
(10)���NC-YSЭ����Diamond-NVR-Cloud�޷��Զ��������⣻
(11)���HDCCTVЭ���ȡ��ȡ������֤�룬ע���û����޸����룬���豸�ӿڣ�
(12)���NC-YSЭ���޷�������YS-NVR�豸���⣻
(13)ɾ��NC-YSЭ������̨���ƿ�ʼ�Զ�Ѳ����ֹͣ�Զ�Ѳ�����ܣ�
(14)�޸�NC-YSЭ����ʵʱ���ͻط����ص���Ϣ֡�Ļ��ƣ����ֹͣʵʱ���������⣻
(15)�޸�NC-YSЭ������¼���ļ����ƣ�����search�滻msg��������Ϣ�����ȣ����
	֧��1024��¼���ļ���
(16)���NC-YSЭ���жԽ�ͨ����ռ��ʱ�����Խ������豸�������⣻	

	0x2102: 2015/01/05
(1) �޸�NC-HYЭ����RTS��PBS��½״̬֡ʵ�ַ�ʽ��
(2) ���NC-HYЭ����¼��طŸ��ַ����������⣻
(3) �������ṹ������������汾�źʹ�����ڣ�
(4) ���NS-NVR�豸��������������ȼ�ʱ������λ���ֳ������Χ���⣻
(5) ���NS-IPC�豸��G711U��Ƶ֡���ͣ�
(6) ���֧��IPC�豸�����ݼ��ܹ��ܣ�
(7) ���NC-YSЭ�飻
(8) �޸�NS-IPC�豸��ns_dev_getRtsFrame()��ȡ֡���ݷ�ʽ��ʹ�ÿ���֡ͷ��ʽ
	���ԭ�е�ʹ��ָ�뷽ʽ��

	0x2101: 2014/11/12
(1) ����̡߳�����������������������
(2) ����豸��qpndģ�飻
(3) ����豸��dbģ���sqliteģ�飻
(4) ��ӷ�������opensslģ�飻
(5) ��ӷ�������APNSЭ��ģ�飻
(6) �޸�makefile�ļ�����Rules.mak�ļ������ģ��ʹ�ܱ�־��
(7) ȥ��IPC�豸��ʱ���������ƣ�
(8) ʵ��S7108/7104ȫ���ܣ�
(9) �޸�DVR,NVR�豸�ĶԽ�֡��СΪ840��
(10)�޸�NC-HYЭ����nc_hy_run()��������cur_rts_enable����
(11)ʵ��NC-HYЭ���еĲ�ѯ������״̬�����ӿڣ�
(12)�Ż�NC-HYЭ���еĽ�����Ϣ�ӿڣ����ݱ����ϱ���Ϣ��
(13)�޸�NC-HYЭ�����޷�ʵʱ�ص���½״̬��Ϣ����: ��nc_hy_dev_runCbRts()
	��ע��rts->state==0���ִ��룻
(14)���ffmpegģ��: ʵ�ַ���SPS��PPS���ܣ�
(15)�Ż�NC-HYЭ��: ��nc_hy_dev_doRecords()ȥ������¼���ļ�����в�����
	��ʼ����ʱ��ε�¼���ļ���
(16)�Ż�NC-HYЭ��: �޸�tool_ptc_hy_hy2qh(), tool_ptc_hy_recvPbs()��
	tool_ptc_hy_recvRts()���������˫ָ��������޸�tool_ptc_hy_recvPbs()
	�����������ֿ�0xBB��ʱ������0���������쳣ʱ��ʱ��ֱ�Ӷ�ʧʣ��0xBB����
(17)�Ż�NC-HYЭ�飬��Զ��¼��طų�����ֹʱ��ʱ���ϱ�255֡���ͣ�
(18)�Ż�NC-HYЭ�飬¼��ط�ʱ����ط�λ�ã����������ɹ�֮��Ӷ���λ�ÿ�ʼ
	���ţ�
(19)Ϊ�����ڴ�ʹ����������NC-HYЭ����������豸����Ϊ8����
(20)�Ż�NC-HYЭ�飬¼��ط��޸�Ϊ��̬�����رգ�ÿ�ο���¼��ط�ʱ������Ƶ֡
	ʱ����Ƿ���ڿ�ʼʱ��Ϊ������������һ�λط�������֡���ݣ�


	0x2100: 2014/11/04
(1) �����д���ͨ��Win32���룻
(2) �޸�Win32�߳̽ӿڣ�ʹ��POSIX Threads��׼��
(3) ���nginx��Fast-CGIģ�飻

	0x2015: 2014/10/20
(1) �Ż�HYЭ�鳬��������������⣻
(2) ���HYЭ���ѯԶ��¼���Զ��¼��طŽӿڣ�
(3) ���HYЭ���ѯ��־��ϢЭ�飻
(4) ���HYЭ����̨���ƽӿڣ�
(5) ���HYЭ�������Խ��ӿڣ�
(6) ���HYЭ��ͬ��ʱ��ӿڣ�
(7) ������粻ͨʱ�������豸��ʱ�������⣻
(8) �ع�HYЭ����룬���з�����һ����¼״̬��

	0x2014: 2014/09/11
(1) �޸Ĳ��Ż��嶪֡���ƣ���д�뻺����ʱ��֡ȡ������ʱ��֡��
	ֻ������Ƶ֡����������Ƶ֡��
(2) ��ʵʱ���ͻط����ص������У�����״̬֡�����Ͷ���ο�PTC_FRAME_E��
(3) ����HY-AHD-DVR����⣬��QH��HY��������ٷ�һ��ӿڲ㣻
(4) HY�ӿ�ʵ��: ��ʼ������⡢��������⡢�����豸����ѯ�豸ͨ������
	��ȡ����������ʼʵʱ����Ƶ����ֹͣ����Ƶ����
	��֧�ֽӿڽ�����NC_ERROR_NOSUPPORTED����
(5) ���ӻ�ȡͨ�����ӿ�nc_getChannelNum_v2();
(6) �޸�IPC-GM8138�豸��ʼʵʱ��Ӧ�ó���������⣻
(7) �޸��豸�������Խ����ܣ���һ�����յ�֡������֡��ʱ������֡���ݣ�
(8) ���ٿͻ����˳������ȴ�ʱ�������ε������ȴ�20ms��

	0x2013: 2014/08/07
(1) �޸���������ʵ�ַ�ʽ��ʹ�ù㲥����ಥ�����ݸ����豸��
(2) ���DVR-V4.0�豸������С��NS_DEV_PBS_MIN_INTEVALʱ�䳤�ȵ�¼���ļ�
	֮���޷�����ʣ���ļ����⣻
(3) �޸�IPC�豸��������Ϣ������⵽DHCP��ʱ�����²��Ҷ�̬IP��ַ��
(4) �޸���������ֻ������һ̨�豸���⣻

	0x2012: 2014/08/07  DVR-V4.0.2
(1) ���DVR-V4.0��ѯ�¼��ӿڣ������Ƶ��ʧ�����¼���
(2) �޸�DVR-V4.0��ʱ���ʱ���ṹ�壬��֤���ֽڶ��룻
(3) �޸��豸��͸������ӿڣ�����û���������IP��ַ������
(4) �ӳٶಥ����ʱ�̣��޸�DVR-V4.0�豸����ಥ��ʧ�����⣻
(5) �޸Ŀͻ������������ȼ��ӿ�û��д��ͨ�����⣻

	0x2011: 2014/07/29
(1) �Խ�DVR-V4.0����ʱ������ڹ��ܣ�
(2) ���������connect������ѭ��pollʱ�������δ��ֵ���⣻

	0x2010: 2014/07/28
(1) ��ӻ�ȡĬ�ϲ����ӿ�;
(2) �ϲ�linux��windowsƽ̨���룻

	0x2008: 2014/07/25
(1) �޸�����ʱ�������Э�飻

	0x2007: 2014/07/23
(1) ����IPC�豸��ʱ���������ƣ� 
(2) ����ͬ��ʱ�书�ܣ�
(3) �޸���ISS Mobile-V1.0.3�汾��BUG;
(4) �Ż��ͻ��˻������ݣ����������ݳ���2��Ԥ������ʱ����ȡ��֡���ƣ�

	0x2006: 2014/07/10
(1) �޸�DVR-v5.0��ptc_cfg_t�Ĳ������ȴ�10K���ӵ�200K��
(2) �޸�DVR-v5.0���豸�˼����˿ڴ�5000�ĳɴ��豸�л�ȡ��
(3) �޸�DVR-v5.0���豸�ӿں������ӿڷֿ����ֱ���Ϊns.h �� dev_func.h
(4) ���ӿͻ��˻������ݹ��ܣ�
(5) �޸�DVR-v3.0�豸��������־û�б�����Ϣ���⣬�޸�������־��Ϣ�ӿ���
�ڴ�δ�ͷ����⣻

	0x2005: 2014/07/01
(1) ����ǿ�ƹؼ�֡֡�ӿڣ�
(2) �޸Ŀ�������Ϣ�߼���
(3) ����DVR-v3.0�豸����־��ѯ�ӿڣ�
(4) ����DVR-v3.0�豸����������ṹ�壻
(5) ����DVR-v3.0�豸��¼��ط��϶�������
(6) ���� �ͻ��������Ӳ����豸ʱ����ʵʱҳ���л�������ҳ�棬�����ס���⣻

	0x2004: 2014/06/30
	0x2003: 2014/06/11
	0x2002: 2014/05/19
	0x2001: 2014/04/30
	0x2000: 2014/04/21
 */

//#define QH_PTC_DEBUG_LOCAL
//#define QH_PTC_DEBUG_MSGS

#ifdef QH_PTC_DEBUG_MSGS
#define PTC_VIDEO_FRAME_SIZE	(1024)

#else
#define PTC_VIDEO_FRAME_SIZE	(500*1024)

#endif



#define PTC_FLAG				0x51484B4A
#define PTC_FRAME_FLAG			0x53535353
#define PTC_MSG_SIZE			(4*1024)
#define PTC_MSG2_SIZE			(60*1024)
#define PTC_PACK_SIZE			(1024)
#define PTC_PACK_NUM			(2000)
#define PTC_AUDIO_FRAME_SIZE	(4*1024)
#define PTC_MAC_SIZE			8
#define PTC_SMS_CODE_SIZE		8
#define PTC_ID_SIZE				32
#define PTC_IP_SIZE				40
#define PTC_STR_SIZE			64

/* PTC_DEVTYPE_E
 *  8 Bit:	00000000 Qihan-Device
 			01000000 HuaYi-Device
 			02000000 YuShi-Device
 			0A010000 iOS-Mobile
 			0A020000 Android-Mobile
 * 64 Bit:	ID content
 */
#define PTC_NAME_SIZE			128
#define PTC_DATES_SIZE			1024
#define PTC_RECORDS_SIZE		1024
#define PTC_LOGS_SIZE			1024
#define PTC_PORT_OFFSET			5

#define PTC_BCAST_IP			"255.255.255.255" 
#define PTC_BCAST_C2S_PORT		13001
#define PTC_BCAST_S2C_PORT		13002


#pragma pack(4)

typedef enum
{
	PTC_DEVTYPE_QIHAN_BASE = 		0x00000000,
	PTC_DEVTYPE_IPC_BASE = 			0x00010000,
	PTC_DEVTYPE_DVR_BASE = 			0x00020000,
	PTC_DEVTYPE_DVR5_BASE = 		0x00030000,
	PTC_DEVTYPE_SDIDVR_BASE = 		0x00040000,
	
	PTC_DEVTYPE_HUAYI_BASE = 		0x01000000,
	PTC_DEVTYPE_HUYYI_D5004 = 		0x010D5004,
	PTC_DEVTYPE_HUYYI_D6004 = 		0x010D6004,
	PTC_DEVTYPE_HUYYI_D6008 = 		0x010D6008,
	PTC_DEVTYPE_HUYYI_D7108 = 		0x010D7108,
	PTC_DEVTYPE_HUYYI_D7116 = 		0x010D7116,

	PTC_DEVTYPE_YUSHI_BASE = 		0x02000000,
	


	PTC_DEVTYPE_MOBILE_BASE = 		0x0A000000,
	PTC_DEVTYPE_MOBILE_IOS = 		0x0A010000,
	PTC_DEVTYPE_MOBILE_ANDROID = 	0x0A020000,
	
}PTC_DEVTYPE_E;

typedef enum
{
	PTC_CMD_USER_BASE = 0,
	PTC_CMD_USER_LOGIN,
	PTC_CMD_USRE_HEARTBEAT,
	PTC_CMD_USER_LOGOUT,

	PTC_CMD_RTS_BASE = 0x00010000,
	PTC_CMD_RTS_START,
	PTC_CMD_RTS_FORCE_KEY_FRAME,
	PTC_CMD_RTS_FRAME,
	PTC_CMD_RTS_STOP,
	PTC_CMD_RTS_PTZ,

	PTC_CMD_TALK_BASE = 0x00020000,
	PTC_CMD_TALK_START,
	PTC_CMD_TALK_C2S,
	PTC_CMD_TALK_S2C,

	PTC_CMD_CFG_BASE = 0x00030000,
	PTC_CMD_CFG_GET_ABILITY,
	PTC_CMD_CFG_CONTROL_PTZ,
	PTC_CMD_CFG_GET_STATUS,
	PTC_CMD_CFG_SET_STATUS,
	PTC_CMD_CFG_GET_VIDEO_DEAD,
	PTC_CMD_CFG_SET_VIDEO_DEAD,
	PTC_CMD_CFG_SET_VIDEOLEVEL_DEAD,
	PTC_CMD_CFG_GET_RECORDS_DEAD,
	PTC_CMD_CFG_GET_LOGS_DEAD,
	PTC_CMD_CFG_AUTH,
	PTC_CMD_CFG_GET_TIME_DEAD,
	PTC_CMD_CFG_SET_TIME_DEAD,
	PTC_CMD_CFG_GET_TIME,
	PTC_CMD_CFG_SET_TIME,
	PTC_CMD_CFG_GET_AUDIO_DEAD,
	PTC_CMD_CFG_SET_AUDIO_DEAD,
	PTC_CMD_CFG_GET_TALK_DEAD,
	PTC_CMD_CFG_SET_TALK_DEAD,
	PTC_CMD_CFG_OPEN_QPND,
	PTC_CMD_CFG_CLOSE_QPND,	

	PTC_CMD_PBS_BASE = 0x00040000,
	PTC_CMD_PBS_START,
	PTC_CMD_PBS_FRAME,
	PTC_CMD_PBS_STOP,
	PTC_CMD_PBS_POS,
	PTC_CMD_PBS_PAUSE,
	PTC_CMD_PBS_RESTART,
	PTC_CMD_PBS_SPEED,
	
	PTC_CMD_ALARM_BASE = 0x00050000,
	PTC_CMD_ALARM_REPORT,
	PTC_CMD_ALARM2_REPORT,

	PTC_CMD_MCAST_BASE = 0x00060000,
	PTC_CMD_MCAST_SEARCH_REQ,
	PTC_CMD_MCAST_SEARCH_RESP,

	PTC_CMD_SEARCH_BASE = 0x00070000,
	PTC_CMD_SEARCH_RECORDS,
	PTC_CMD_SEARCH_LOGS,
	PTC_CMD_SEARCH_DATA,
	PTC_CMD_SEARCH_GET,
	PTC_CMD_SEARCH_SET,
	PTC_CMD_SEARCH_GET_DEFAULT,

	PTC_CMD_CFG2_BASE = 0x00080000,
	PTC_CMD_CFG2_GET_ALARM,
	PTC_CMD_CFG2_SET_ALARM,
	PTC_CMD_CFG2_GET_WORKMODE,
	PTC_CMD_CFG2_SET_WORKMODE,
	PTC_CMD_CFG2_GET_MOBILE,
	PTC_CMD_CFG2_SET_MOBILE,
	PTC_CMD_CFG2_GET_WIFI,
	PTC_CMD_CFG2_SET_WIFI,
	PTC_CMD_CFG2_GET_DEVRECORD,
	PTC_CMD_CFG2_SET_DEVRECORD,	
	PTC_CMD_CFG2_CTRL_PTZ,
	PTC_CMD_CFG2_SYNC_TIME,
	PTC_CMD_CFG2_GET_STATUS,
	PTC_CMD_CFG2_SET_STATUS,	
	PTC_CMD_CFG2_GET_AV,
	PTC_CMD_CFG2_SET_AV,		

	PTC_CMD_SEARCH2_BASE = 0x00090000,
	PTC_CMD_SEARCH2_ABILITY,
	PTC_CMD_SEARCH2_LOGS,
	PTC_CMD_SEARCH2_RECORDS,
	PTC_CMD_SEARCH2_DATES,	
	

	PTC_CMD_QPNS_BASE = 0x00100000,
	PTC_CMD_QPNS_LOGIN,
	PTC_CMD_QPNS_LOGOUT,
	PTC_CMD_QPNS_HEARTBEAT,
	PTC_CMD_QPNS_ALARM,

	PTC_CMD_HDCCTV_BASE = 0x00200000,
	//dev <=> relay
	PTC_CMD_HDCCTV_DEV_LOGIN_RELAY 		= 0x00200004,
	PTC_CMD_HDCCTV_DEV_HB_RELAY 		= 0x00200005,
	PTC_CMD_HDCCTV_RELAY_START_DEV		= 0x00200006,
	PTC_CMD_HDCCTV_DEV_RTS_RELAY		= 0x00200007,
	PTC_CMD_HDCCTV_RELAY_STOP_DEV		= 0x00200008,
	PTC_CMD_HDCCTV_DEV_LOGOUT_RELAY 	= 0x00200009,
	PTC_CMD_HDCCTV_RELAY_START_PBS		= 0x00200011,
	PTC_CMD_HDCCTV_DEV_PBS_RELAY		= 0x00200013,
	PTC_CMD_HDCCTV_RELAY_STOP_PBS		= 0x00200014,
	PTC_CMD_HDCCTV_RELAY_START_TALK		= 0x00200021,
	PTC_CMD_HDCCTV_RELAY_TALK_DEV		= 0x00200022,
	PTC_CMD_HDCCTV_DEV_TALK_RELAY		= 0x00200023,
	PTC_CMD_HDCCTV_RELAY_STOP_TALK		= 0x00200024,
	PTC_CMD_HDCCTV_DEV_ALARM_RELAY		= 0x00200030,


	//dev <=> msgs
	PTC_CMD_HDCCTV_DEV_LOGIN_MSGS		= 0x00200104,
	PTC_CMD_HDCCTV_DEV_HB_MSGS			= 0x00200105,
	PTC_CMD_HDCCTV_DEV_LOGOUT_MSGS		= 0x00200106,

	//client <=> relay
	PTC_CMD_HDCCTV_CLIENT_HB_RELAY 		= 0x00210005,
	PTC_CMD_HDCCTV_CLIENT_START_RELAY	= 0x00210006,
	PTC_CMD_HDCCTV_RELAY_RTS_CLIENT		= 0x00210007,
	PTC_CMD_HDCCTV_CLIENT_STOP_RELAY	= 0x00210008,
	PTC_CMD_HDCCTV_CLIENT_START_PBS		= 0x00210011,
	PTC_CMD_HDCCTV_RELAY_PBS_CLIENT		= 0x00210013,
	PTC_CMD_HDCCTV_CLIENT_STOP_PBS		= 0x00210014,
	PTC_CMD_HDCCTV_CLIENT_START_TALK	= 0x00210021,
	PTC_CMD_HDCCTV_CLIENT_TALK_RELAY	= 0x00210022,
	PTC_CMD_HDCCTV_RELAY_TALK_CLIENT	= 0x00210023,
	PTC_CMD_HDCCTV_CLIENT_STOP_TALK		= 0x00210024,
	PTC_CMD_HDCCTV_CLIENT_SEARCH_CLOUD	= 0x00210030,
	PTC_CMD_HDCCTV_CLIENT_START_CLOUD	= 0x00210031,
	PTC_CMD_HDCCTV_CLOUD_PBS_CLOUD		= 0x00210032,
	PTC_CMD_HDCCTV_CLIENT_POS_CLOUD		= 0x00210033,
	PTC_CMD_HDCCTV_CLIENT_STOP_CLOUD	= 0x00210034,
	PTC_CMD_HDCCTV_CLIENT_SEARCH_DATE	= 0x00210035,


	//client <=> msgs
	PTC_CMD_HDCCTV_CLIENT_LOGIN_MSGS	= 0x00210104,
	PTC_CMD_HDCCTV_CLIENT_HB_MSGS		= 0x00210105,
	PTC_CMD_HDCCTV_CLIENT_LOGOUT_MSGS	= 0x00210106,

/*
	PTC_CMD_HDCCTV_START_DEV_PBS		= 0x00220001,
	PTC_CMD_HDCCTV_FRAME_DEV_PBS		= 0x00220003,
	PTC_CMD_HDCCTV_STOP_DEV_PBS			= 0x00220004,
*/
	
}PTC_CMD_E;

typedef enum 
{
	PTC_PTZ_CMD_STOP			=0,  //ֹͣ
	PTC_PTZ_CMD_LEFT,                //��
	PTC_PTZ_CMD_RIGHT,               //��
	PTC_PTZ_CMD_UP,                  //��
	PTC_PTZ_CMD_DOWN,                //��
	PTC_PTZ_CMD_LEFT_UP,             //����
	PTC_PTZ_CMD_LEFT_DOWN,           //����
	PTC_PTZ_CMD_RIGHT_UP,            //����
	PTC_PTZ_CMD_RIGHT_DOWN,          //����
	PTC_PTZ_CMD_NEAR,                //����
	PTC_PTZ_CMD_FAR,                 //��Զ
	PTC_PTZ_CMD_ZOOM_OUT,            //��С
	PTC_PTZ_CMD_ZOOM_IN,             //�Ŵ�
	PTC_PTZ_CMD_IRIS_OPEN,           //������Ȧ
	PTC_PTZ_CMD_IRIS_CLOSE,          //�رչ�Ȧ
	PTC_PTZ_CMD_PRESET_SET,          //����Ԥ�õ�
	PTC_PTZ_CMD_PRESET_GO,           //���ڼ���Ԥ�õ�
	PTC_PTZ_CMD_PRESET_DEL,          //ɾ��Ԥ�õ�
	PTC_PTZ_CMD_CRUISE_CFG,          //����Ѳ����,�൱��ִ��Enter��Set��Leave��������, �ݲ�֧��
	PTC_PTZ_CMD_ENTER_CURISE_MODE,   //����Ѳ��ģʽ,�˺��������Ѳ����Ԥ�õ�, �ݲ�֧��
	PTC_PTZ_CMD_CRUISE_SET,          //����Ѳ���ߵ�Ԥ�õ�, �ݲ�֧��
	PTC_PTZ_CMD_LEAVE_CURISE_MODE,   //�˳�����Ѳ��, �ݲ�֧��
	PTC_PTZ_CMD_CRUISE_RUN,          //ѡ��һ��Ѳ���߽���Ѳ��, �ݲ�֧��
	PTC_PTZ_CMD_CRUISE_STOP,         //ֹͣѲ��, �ݲ�֧��
	PTC_PTZ_CMD_CRUISE_DEL,          //ɾ��Ѳ����, �ݲ�֧��
	PTC_PTZ_CMD_TRACK_START,         //��ʼ�켣, �ݲ�֧��
	PTC_PTZ_CMD_TRACK_STOP,          //ֹͣ�켣, �ݲ�֧��
	PTC_PTZ_CMD_TRACK_SET,           //��ʼ����켣, �ݲ�֧��
	PTC_PTZ_CMD_TRACK_DEL,           //ֹͣ����켣, �ݲ�֧��
	PTC_PTZ_CMD_AUTO_SCAN_START,     //��ʼ�Զ�ɨ��
	PTC_PTZ_CMD_AUTO_SCAN_STOP,      //ֹͣ�Զ�ɨ��
	PTC_PTZ_CMD_POP_PTZ_MENU,        //������̨�Դ��˵�, �ݲ�֧��
	PTC_PTZ_CMD_SET_AUXILIARY,       //�򿪸�������, �ݲ�֧��
	PTC_PTZ_CMD_CLEAR_AUXILIARY,     //�رո�������, �ݲ�֧��

	PTC_PIR_CMD_BASE = 0xA0,		
	PTC_PIR_CMD_RECORD_SOUND_START,  //��ʼ¼������
	PTC_PIR_CMD_RECORD_SOUND_STOP,	 //ֹͣ¼������
	PTC_PIR_CMD_PLAY_SOUND_START,    //��ʼ��������
	PTC_PIR_CMD_PLAY_SOUND_STOP,     //ֹͣ��������

}PTC_PTZ_CMD_E;


typedef enum
{
	PTC_ACK_SUCCESS = 0,
	PTC_ACK_ERROR_AUTH,
	PTC_ACK_ERROR_CHANNEL,
	PTC_ACK_ERROR_STREAMTYPE,
	PTC_ACK_ERROR_BUSY,
	PTC_ACK_ERROR_PBS_SPEED,
	PTC_ACK_ERROR_PBS_POS,
	PTC_ACK_ERROR_PBS_STATE,
	PTC_ACK_ERROR_CFG_VALUE,
	PTC_ACK_ERROR_CFG_CMD,
	PTC_ACK_ERROR_CFG_FULL_TOKEN,
	PTC_ACK_ERROR_OTHER = 255
}PTC_ACK_E;

typedef enum
{
	PTC_ENABLE_FLASE = 0,
	PTC_ENABLE_TRUE,
}PTC_ENABLE_E;

typedef enum
{
	PTC_FRAME_NONE 					= 0,
	PTC_FRAME_I,							//��ƵI֡
	PTC_FRAME_P,							//��ƵP֡
	PTC_FRAME_A,							//��Ƶ֡

	PTC_FRAME_INFO					= 100,	//��Ϣ֡

	PTC_FRAME_STREAM_RIGHT			= 235, 	//��Ȩ��
	PTC_FRAME_STREAM_VIDEO_BUSY		= 236,	//��Ƶͨ����æ
	PTC_FRAME_STREAM_TIMEOUT		= 237,	//��ʱ
	PTC_FRAME_STREAM_POSTION 		= 238,	//��λ������
	PTC_FRAME_STREAM_BROKEN 		= 239,	//ͨ�Ŵ��󣬶Ͽ�����
	PTC_FRAME_STREAM_WAIT			= 240,	//�ȴ�����...
	PTC_FRAME_STREAM_CONNECT,				//�����豸/������...
	PTC_FRAME_STREAM_CONNECT_FAIL,			//�����豸ʧ��
	PTC_FRAME_STREAM_LOGIN,					//��½�豸...
	PTC_FRAME_STREAM_LOGIN_FAIL,			//��½�豸ʧ��
	PTC_FRAME_STREAM_AUTH_FAIL,				//�û���֤ʧ��
	PTC_FRAME_STREAM_MAX_CONNECT,			//�ﵽ���������
	PTC_FRAME_STREAM_START,					//������ý��...
	PTC_FRAME_STREAM_START_FAIL,			//������ý��ʧ��
	PTC_FRAME_STREAM_RUNNING,				//������ý��...
	PTC_FRAME_STREAM_NOVIDEO,				//��ͨ��û����Ƶ,250
	PTC_FRAME_STREAM_TALK_BUSY,				//�Խ�ͨ����ռ��
	PTC_FRAME_STREAM_LOCATION,				//����������
	PTC_FRAME_STREAM_LOCATION_FAIL,			//����������ʧ��  
	PTC_FRAME_STREAM_DEVICE_OFFLINE,		//�豸������
	
	PTC_FRAME_PBS_END 		= 255,
}PTC_FRAME_E;

typedef enum
{
	PTC_RECORD_TYPE_NONE 		= 0x00,
	PTC_RECORD_TYPE_TIME 		= 0x01,
	PTC_RECORD_TYPE_ALARM 		= 0x02,
	PTC_RECORD_TYPE_MANUAL 		= 0x04,
	PTC_RECORD_TYPE_ALL			= 0xFF,
}PTC_RECORD_TYPE_E;

typedef enum
{
	PTC_RECORD_SUBTYPE_NONE 	= 0x00,
	PTC_RECORD_SUBTYPE_MOTION 	= 0x01,
	PTC_RECORD_SUBTYPE_SENSOR 	= 0x02,
	PTC_RECORD_SUBTYPE_PIR 		= 0x04,
	PTC_RECORD_SUBTYPE_COVER 	= 0x08,
	PTC_RECORD_SUBTYPE_ALL		= 0xFF,
}PTC_RECORD_SUBTYPE_E;

typedef enum
{
	PTC_CH_TYPE_LOCAL_RTS_MAIN	= 0x00,		//����������ͨ��
	TPC_CH_TYPE_LOCAL_RTS_SUB	= 0x01,		//����������ͨ��
	PTC_CH_TYPE_LOCAL_RTS_PHONE	= 0x02,		//�����ֻ�����ͨ��
	PTC_CH_TYPE_LOCAL_ALARM_IN 	= 0x03,		//���ر�������ͨ��
	PTC_CH_TYPE_LOCAL_ALARM_OUT = 0x04,		//���ر������ͨ��
	PTC_CH_TYPE_LOCAL_PBS 		= 0x05,		//���ػط�ͨ��
	PTC_CH_TYPE_LOCAL_TALK		= 0x06, 	//���ضԽ�ͨ��
	PTC_CH_TYPE_DIGIT_NONE		= 0x10,
	PTC_CH_TYPE_DIGIT,						//����ͨ��(Զ��IPC)
	
}PTC_CH_TYPE_E;

typedef enum
{
	PTC_LOG_TYPE_NONE 		= 0,
	PTC_LOG_TYPE_SYSTEM,
	PTC_LOG_TYPE_ALARM,
	PTC_LOG_TYPE_OPERATE,
	PTC_LOG_TYPE_ADS		= 0x10,
}PTC_LOG_TYPE_E;

typedef enum
{
	//ϵͳ����(Control)  �����־����
	PTC_LOG_SYSTEM_CTRL		= 0,
	PTC_LOG_BOOT,							//ϵͳ����
	PTC_LOG_SHUTDOWN,						//ϵͳ�ػ�
	PTC_LOG_REBOOT,							//ϵͳ����
	PTC_LOG_FORMAT_SUCC,					//��ʽ�����̳ɹ�
	PTC_LOG_FORMAT_FAIL,					//��ʽ������ʧ��
	PTC_LOG_UPGRADE_SUCC,					//�����ɹ�
	PTC_LOG_UPGRADE_FAIL,					//����ʧ��
	PTC_LOG_CLEAR_ALARM,					//�������
	PTC_LOG_OPEN_ALARM,						//��������
	PTC_LOG_MANUAL_START,					//�����ֶ�¼��
	PTC_LOG_MANUAL_STOP,					//ֹͣ�ֶ�¼��
	PTC_LOG_PTZ_ENTER,						//��ʼ��̨����
	PTC_LOG_PTZ_CTRL,						//��̨����
	PTC_LOG_PTZ_EXIT,						//������̨����
	PTC_LOG_AUDIO_CH_CHANGE,				//�ı��ֳ���Ƶͨ��
	PTC_LOG_VOLUME_ADJUST,					//��������
	PTC_LOG_MUTE_ENABLE,					//��������
	PTC_LOG_MUTE_DISENABLE,					//�رվ���
	PTC_LOG_DWELL_ENABLE,					//������ѭ
	PTC_LOG_DWELL_DISENABLE,				//�ر���ѭ
	PTC_LOG_LOG_IN,							//��¼
	PTC_LOG_LOG_OFF,						//�ǳ�
	PTC_LOG_CHANGE_TIME,					//�޸�ϵͳʱ��
	PTC_LOG_MANUAL_SNAP_SUCC,               //�ֶ�ץͼ�ɹ�
	PTC_LOG_MANUAL_SNAP_FAIL,               //�ֶ�ץͼʧ�� 25

	//ϵͳ����(Setup)
	PTC_LOG_CONFIG			= 40,
	PTC_LOG_CHGE_VIDEO_FORMAT,				//�ı���Ƶ��ʽ
	PTC_LOG_CHGE_VGA_RESOLUTION,			//�ı���ʾ���ֱ���
	PTC_LOG_CHGE_LANGUAGE,					//��������
	PTC_LOG_CHGE_NET_USER_NUM,				//���������û���Ŀ
	PTC_LOG_CHGE_TIME_ZONE,					//����ʱ��
	PTC_LOG_NTP_MANUAL,						//�ֶ�����Уʱ
	PTC_LOG_NTP_ON,							//�����Զ�����Уʱ
	PTC_LOG_NTP_OFF,						//�ر��Զ�����Уʱ
	PTC_LOG_CHGE_NTP_SERVER,				//�޸�����ʱ���������ַ
	PTC_LOG_CHGE_DST,						//��������ʱ����
	PTC_LOG_PASSWD_ON,						//������������
	PTC_LOG_PASSWD_OFF,						//�رղ�������
	PTC_LOG_CHGE_CAM_NAME,					//����ͨ������
	PTC_LOG_MODIFY_COLOR,					//����ͼ��ɫ��
	PTC_LOG_CHGE_HOST_MONITOR,				//��������������������
	PTC_LOG_CHGE_SPOT,						//�������������������
	PTC_LOG_CHGE_OSD,						//�����ַ���������
	PTC_LOG_CHGE_LOCAL_ENCODE,				//����¼�����������
	PTC_LOG_CHGE_REC_VIDEO_SWITCH,			//����¼�񿪹�����
	PTC_LOG_CHGE_REC_AUDIO_SWITCH,			//����¼����Ƶ��������
	PTC_LOG_CHGE_REC_REDU_SWITCH,			//��������¼�񿪹�����
	PTC_LOG_CHGE_REC_PRE_TIME,				//������ǰ¼��ʱ��
	PTC_LOG_CHGE_REC_POST_TIME,				//��������¼��ʱ��
	PTC_LOG_CHGE_REC_HOLD_TIME,				//����¼�����ݹ���ʱ��
	PTC_LOG_CHGE_SCH_SCHEDULE,				//������ʱ¼��ƻ�
	PTC_LOG_CHGE_SCH_MOTION,				//�����ƶ����¼��ƻ�
	PTC_LOG_CHGE_SCH_ALARM,					//��������¼��ƻ�
	PTC_LOG_CHGE_SENSOR_SWITCH,				//�����������뿪������
	PTC_LOG_CHGE_SENSOR_TYPE,				//�������������豸����
	PTC_LOG_CHGE_SENSOR_TRIGGER,			//�����������봦��ʽ����
	PTC_LOG_CHGE_SENSOR_SCH,				//���������������ƻ�
	PTC_LOG_CHGE_MOTION_SWITCH,				//�����ƶ���⿪������
	PTC_LOG_CHGE_MOTION_SENS,				//�����ƶ����������
	PTC_LOG_CHGE_MOTION_AREA,				//�����ƶ������������
	PTC_LOG_CHGE_MOTION_TRIGGER,			//�����ƶ���⴦��ʽ
	PTC_LOG_CHGE_MOTION_SCH,				//�����ƶ����ƻ�
	PTC_LOG_CHGE_VL_TRIGGER,				//������Ƶ��ʧ����ʽ����
	PTC_LOG_CHGE_RELAY_SWITCH,				//�������������������
	PTC_LOG_CHGE_RELAY_SCH,					//������������ƻ�
	PTC_LOG_BUZZER_ON,						//�������������豸
	PTC_LOG_BUZZER_OFF,						//�ر����������豸
	PTC_LOG_CHGE_BUZZER_SCH,				//�������������ƻ�
	PTC_LOG_CHGE_HTTP_PORT,					//�޸�HTTP�������˿�
	PTC_LOG_CHGE_SER_PORT,					//�޸�����������˿�
	PTC_LOG_CHGE_IP,						//���������ַ
	PTC_LOG_DHCP_SUCC,						//�Զ���ȡ�����ַ�ɹ�
	PTC_LOG_DHCP_FAIL,						//�Զ���ȡ�����ַʧ��
	PTC_LOG_CHGE_PPPOE,						//����PPPoE
	PTC_LOG_CHGE_DDNS,						//����DDNS
	PTC_LOG_NET_STREAM_CFG,					//������������������
	PTC_LOG_CHGE_SERIAL,					//������̨��������
	PTC_LOG_PRESET_MODIFY,					//����Ԥ�õ�
	PTC_LOG_CRUISE_MODIFY,					//����Ѳ����
	PTC_LOG_TRACK_MODIFY,					//�����켣
	PTC_LOG_USER_ADD,						//�����û�
	PTC_LOG_USER_MODIFY,					//�����û�Ȩ��
	PTC_LOG_USER_DELETE,					//ɾ���û�
	PTC_LOG_CHANGE_PASSWD,					//�޸��û�����
	PTC_LOG_LOAD_DEFAULT,					//�ָ�Ĭ������
	PTC_LOG_IMPORT_CONFIG,					//��������
	PTC_LOG_EXPORT_CONFIG,					//��������
	PTC_LOG_CHGE_IMAGE_MASK,				//ͼ���ڵ�
	PTC_LOG_RECYCLE_REC_ON,					//����ѭ��¼��
	PTC_LOG_RECYCLE_REC_OFF,				//�ر�ѭ��¼��
	PTC_LOG_CHGE_DISK_ALARM,				//�������̱����ռ�
	PTC_LOG_CHGE_SEND_EMAIL,				//����Email ��������Ϣ
	PTC_LOG_CHGE_RECV_EMAIL,				//����Email ��������Ϣ
	PTC_LOG_CHGE_SNAP_SETTING,             	//����ץͼ���� 108


	// IPC��־
	PTC_LOG_UPDATE_NET_TIME = 128,
	PTC_LOG_UPDATE_RTC_TIME,
	PTC_LOG_MODIFY_SYSCONFIG,
	PTC_LOG_MODIFY_VIDEO_ENCODE,
	PTC_LOG_MODIFY_IMAGE_CONFIG,
	PTC_LOG_MODIFY_ISP,
	PTC_LOG_MODIFT_OSD,	
	PTC_LOG_MODIFY_SERIAL_CONFIG,
	PTC_LOG_MODIFY_OVERLAY_CONFIG, 
	PTC_LOG_MODIFY_AUDIO,		
	PTC_LOG_MODIFY_PPPOE,
	PTC_LOG_MODIFY_DDNS,	
	PTC_LOG_MODIFY_INSTANT_CAP,
	PTC_LOG_MODIFY_TIME_CAP,	
	PTC_LOG_MODIFY_MOTION,
	PTC_LOG_MODIFY_PIR,		
	PTC_LOG_MODIFY_PTZ,	
	PTC_LOG_MODIFY_FTP,
	PTC_LOG_MODIFY_MAIL,
	PTC_LOG_MODIFY_NTP,
	PTC_LOG_MODIFY_C2G,
	PTC_LOG_MODIFY_LUX,
	PTC_LOG_MODIFY_DEVICE,
	PTC_LOG_MODIFY_USER_INFO,
	PTC_LOG_MODIFY_VERSION,	
	PTC_LOG_DEFAULT_ALL_APP,	
	PTC_LOG_SENSOR_ERROR,	
	PTC_LOG_DHCP_START,		
	PTC_LOG_WIFI_DHCP_START,
	PTC_LOG_SYSTEM_REBOOT,
	PTC_LOG_INIT_SYSTEM_TIME,
	PTC_LOG_DISK_FULL,
	PTC_LOG_DISK_NOT_EXIST,					//160
	PTC_LOG_MOTION_START,
	PTC_LOG_MOTION_STOP,
	PTC_LOG_PIR_START,	
	PTC_LOG_PIR_STOP,
	PTC_LOG_PROBER_START,
	PTC_LOG_PROBER_STOP,
	PTC_LOG_UPDATE_SYSTEM_SUCCESS,
	PTC_LOG_MODIFY_CVBS,
	PTC_LOG_SYSTEM_START,
	PTC_LOG_MODIFY_HSD_ISP,					//170
	PTC_LOG_MODIFY_HSD_PATROL,
	PTC_LOG_MODIFY_HSD_ADVANCEDPTZ,
	PTC_LOG_MODIFY_HSD_IRCONTROL,
	PTC_LOG_MODIFY_ONVIF,
	PTC_LOG_3G_LINK_OK,
	PTC_LOG_3G_LINK_FAIL,
	PTC_LOG_3G_LINK_DISCONNECT,
	PTC_LOG_UPDATE_SYSTEM_FAIL_CRC,			//(178)

	PTC_LOG_XARINA_START,					//��ʱ���� 179
	PTC_LOG_XARINA_STOP,					//��ʱ����
	PTC_LOG_TA_DEFOUSE_START,				//�齹������ʼ 181
	PTC_LOG_TA_GLOBAL_START,				//�ڵ�������ʼ 182
	PTC_LOG_VMF1_START,						//���ֱ�����ʼ 183
	PTC_LOG_VMF2_START,						//�߽籨����ʼ 184
	PTC_LOG_VMF3_START,						//���嶪ʧ������ʼ 185
	PTC_LOG_TA_DEFOUSE_STOP,				//�齹��������
	PTC_LOG_TA_GLOBAL_STOP,					//�ڵ���������
	PTC_LOG_VMF1_STOP,						//���ֱ�������
	PTC_LOG_VMF2_STOP,						//�߽籨������
	PTC_LOG_VMF3_STOP,						//���嶪ʧ��������(190)

	PTC_LOG_VIDEOLOSS_START = 196,
	PTC_LOG_VIDEOLOSS_STOP = 197,
	PTC_LOG_VIDEOCOVER_START = 198,
	PTC_LOG_VIDEOCOVER_STOP = 199,

	//¼��ط�(Playback)
	PTC_LOG_PLAYBACK		= 200,
	PTC_LOG_PLAYBACK_PLAY,					//����
	PTC_LOG_PLAYBACK_PAUSE,					//��ͣ
	PTC_LOG_PLAYBACK_RESUME,				//�ָ�����
	PTC_LOG_PLAYBACK_FF,					//���
	PTC_LOG_PLAYBACK_REW,					//����
	PTC_LOG_PLAYBACK_STOP,					//ֹͣ
	PTC_LOG_PLAYBACK_NEXT_SECTION,			//��һ��
	PTC_LOG_PLAYBACK_PREV_SECTION,			//��һ�� 208

	//���ݱ���(Backup)
	PTC_LOG_BACKUP			= 211,
	PTC_LOG_BACKUP_START,					//��ʼ����
	PTC_LOG_BACKUP_COMPLETE,				//�������
	PTC_LOG_BACKUP_CANCEL,					//��������
	PTC_LOG_BACKUP_FAIL,					//����ʧ�� 215

	//¼�����(Search)
	PTC_LOG_SEARCH			= 218,
	PTC_LOG_SEARCH_TIME,					//��ʱ�����
	PTC_LOG_SEARCH_EVENT,					//���¼�����
	PTC_LOG_SEARCH_FILE_MAN,				//�ļ�����
	PTC_LOG_DELETE_FILE,					//ɾ���ļ�
	PTC_LOG_LOCK_FILE,						//�����ļ�
	PTC_LOG_UNLOCK_FILE,					//�����ļ�
	PTC_LOG_DELETE_PICTURE,                	//ɾ��ͼƬ
	PTC_LOG_LOCK_PICTURE,                  	//����ͼƬ
	PTC_LOG_UNLOCK_PICTURE,                	//����ͼƬ 227

	//�鿴��Ϣ(View information)
	PTC_LOG_VIEW_INFO		= 230,
	PTC_LOG_VIEW_SYSTEM,					//�鿴ϵͳ��Ϣ
	PTC_LOG_VIEW_EVENT,						//�鿴�¼�
	PTC_LOG_VIEW_LOG,						//�鿴��־
	PTC_LOG_VIEW_NETWORK,					//�鿴����״̬
	PTC_LOG_VIEW_ONLINE_USER,				//�鿴�����û�
	PTC_LOG_VIEW_EXPORT_LOG,				//������־
	PTC_LOG_VIEW_EXPORT_EVENT,				//�����¼� 237 

	//�쳣��Ϣ(Error)
	PTC_LOG_ERROR_INFO		= 240,
	PTC_LOG_IP_CONFLICT,					//�����ַ��ͻ
	PTC_LOG_NETWORK_ERR,					//�����쳣
	PTC_LOG_DDNS_ERR,						//DDNS����
	PTC_LOG_DISK_IO_ERR,					//���̶�д����
	PTC_LOG_UNKNOWN_OFF,					//�쳣�ϵ�
	PTC_LOG_UNKNOWN_ERR,					//δ֪����	
	PTC_LOG_ERR_9A9A9A9A,					//δ֪����	
	PTC_LOG_ERR_9A000001,					//δ֪����	
	PTC_LOG_ERR_9A000002,					//δ֪����	
	PTC_LOG_DISK_WARNING,                	//����˥������
	PTC_LOG_DISK_DISCONNECT,                //���̵��߱��� 251
	//LOG_TYE_DISK_NOTFIND,					//������Ӳ��	
	PTC_LOG_DIAMOND_NVR 	= 255,			//Diamond-NVR ��־
	
//	PTC_LOG_YS = 299,				   				/*  ����������Ϣ����־*/
	/* ��Ϣ����־*/
	PTC_LOG_MSG_HDD_INFO       = 300,               /* Ӳ����Ϣ */
	PTC_LOG_MSG_SMART_INFO     = 301,               /* S.M.A.R.T��Ϣ */
	PTC_LOG_MSG_REC_OVERDUE    = 302,               /* ����¼��ɾ�� */
	PTC_LOG_MSG_PIC_REC_OVERDUE = 303,              /* ����ͼƬ�ļ�ɾ�� */
    PTC_LOG_MSG_IPC_ONLINE = 304,                   /* IPC���� */
    PTC_LOG_MSG_IPC_OFFLINE = 305,                  /* IPC���� */

	/* �澯����־����������־ID */
	PTC_LOG_ALARM_MOTION_DETECT = 350,              /* �ƶ����澯 */
	PTC_LOG_ALARM_MOTION_DETECT_RESUME = 351,       /* �ƶ����澯�ָ� */
	PTC_LOG_ALARM_VIDEO_LOST = 352,                 /* ��Ƶ��ʧ�澯 */
	PTC_LOG_ALARM_VIDEO_LOST_RESUME = 353,          /* ��Ƶ��ʧ�澯�ָ� */
	PTC_LOG_ALARM_VIDEO_TAMPER_DETECT = 354,        /* �ڵ����澯 */
	PTC_LOG_ALARM_VIDEO_TAMPER_RESUME = 355,        /* �ڵ����澯�ָ� */
	PTC_LOG_ALARM_INPUT_SW = 356,                   /* ���뿪�����澯 */
	PTC_LOG_ALARM_INPUT_SW_RESUME = 357,            /* ���뿪�����澯�ָ� */
	PTC_LOG_ALARM_IPC_ONLINE = 358,                 /* IPC �豸���� */
	PTC_LOG_ALARM_IPC_OFFLINE = 359,                /* IPC �豸���� */

	/* �쳣����־����������־ID */
	PTC_LOG_EXCEP_DISK_ONLINE = 400,                /* �������� */
	PTC_LOG_EXCEP_DISK_OFFLINE = 401,               /* �������� */
	PTC_LOG_EXCEP_DISK_ERR = 402,                   /* �����쳣 */
	PTC_LOG_EXCEP_STOR_ERR = 403,                   /* �洢���� */
	PTC_LOG_EXCEP_STOR_ERR_RECOVER = 404,           /* �洢����ָ� */
	PTC_LOG_EXCEP_STOR_DISOBEY_PLAN = 405,          /* δ���ƻ��洢 */
	PTC_LOG_EXCEP_STOR_DISOBEY_PLAN_RECOVER = 406,  /* δ���ƻ��洢�ָ� */
	PTC_LOG_EXCEP_ILLEGAL_ACCESS = 407,             /* �Ƿ����� */
	PTC_LOG_EXCEP_IP_CONFLICT = 408,                /* IP��ַ��ͻ */
	PTC_LOG_EXCEP_NET_BROKEN = 409,                 /* ����Ͽ� */
	PTC_LOG_EXCEP_PIC_REC_ERR = 410,                /* ץͼ����,��ȡͼƬ�ļ�ʧ�� */
	PTC_LOG_EXCEP_VIDEO_EXCEPTION = 411,            /* ��Ƶ�����쳣(ֻ���ģ��ͨ��) */
	PTC_LOG_EXCEP_VIDEO_MISMATCH = 412,             /* ��Ƶ��ʽ��ƥ��  */
	PTC_LOG_EXCEP_RESO_MISMATCH = 413,              /* ����ֱ��ʺ�ǰ�˷ֱ��ʲ�ƥ�� */
	PTC_LOG_EXCEP_TEMP_EXCE = 414,                  /* �¶��쳣 */

	/* ��������־����������־ID */
	/* ҵ���� */
	PTC_LOG_OPSET_LOGIN = 450,                      /* �û���¼ */
	PTC_LOG_OPSET_LOGOUT = 451,                     /* ע����½ */
	PTC_LOG_OPSET_USER_ADD = 452,                   /* �û���� */
	PTC_LOG_OPSET_USER_DEL = 453,                   /* �û�ɾ�� */
	PTC_LOG_OPSET_USER_MODIFY = 454,                /* �û��޸� */

	PTC_LOG_OPSET_START_REC = 455,                  /* ��ʼ¼�� */
	PTC_LOG_OPSET_STOP_REC = 456,                   /* ֹͣ¼�� */
	PTC_LOG_OPSETR_PLAY = 457,                      /* �ط� */
	PTC_LOG_OPSET_DOWNLOAD = 458,                   /* ���� */
	PTC_LOG_OPSET_PTZCTRL = 459,                    /* ��̨���� */
	PTC_LOG_OPSET_PREVIEW = 460,                    /* ʵ��Ԥ�� */
	PTC_LOG_OPSET_REC_TRACK_START = 461,            /* �켣¼�ƿ�ʼ */
	PTC_LOG_OPSET_REC_TRACK_STOP = 462,             /* �켣¼��ֹͣ */
	PTC_LOG_OPSET_START_TALKBACK = 463,             /* ��ʼ�����Խ� */
	PTC_LOG_OPSET_STOP_TALKBACK = 464,              /* ֹͣ�����Խ� */

	PTC_LOG_OPSET_IPC_ADD = 465,                    /* ���IPC */
	PTC_LOG_OPSET_IPC_DEL = 466,                    /* ɾ��IPC */
	PTC_LOG_OPSET_IPC_SET = 467,                    /* ����IPC */
    PTC_LOG_OPSET_IPC_QUICK_ADD = 468,				/* �������IPC */
    PTC_LOG_OPSET_IPC_NET_ADD   = 469,				/* �������IPC */

	/* ������ */
	PTC_LOG_OPSET_DEV_BAS_CFG = 500,                /* �豸������Ϣ���� */
	PTC_LOG_OPSET_TIME_CFG = 501,                   /* �豸ʱ������ */
	PTC_LOG_OPSET_SERIAL_CFG = 502,                 /* �豸�������� */

	PTC_LOG_OPSET_CHL_BAS_CFG = 503,                /* ͨ���������� */
	PTC_LOG_OPSET_CHL_NAME_CFG = 504,               /* ͨ���������� */
	PTC_LOG_OPSET_CHL_ENC_VIDEO = 505,              /* ��Ƶ����������� */
	PTC_LOG_OPSET_CHL_DIS_VIDEO = 506,              /* ͨ����Ƶ��ʾ�������� */
	PTC_LOG_OPSET_PTZ_CFG = 507,                    /* ��̨���� */
	PTC_LOG_OPSET_CRUISE_CFG = 508,                 /* Ѳ����·���� */
	PTC_LOG_OPSET_PRESET_CFG = 509,                 /* Ԥ�õ����� */
	PTC_LOG_OPSET_VIDPLAN_CFG = 510,                /* ¼��ƻ����� */
	PTC_LOG_OPSET_MOTION_CFG = 511,                 /* �˶�������� */
	PTC_LOG_OPSET_VIDLOSS_CFG = 512,                /* ��Ƶ��ʧ���� */
	PTC_LOG_OPSET_COVER_CFG = 513,                  /* ��Ƶ�ڵ����� */
	PTC_LOG_OPSET_MASK_CFG = 514,                   /* ��Ƶ�ڸ����� */
	PTC_LOG_OPSET_SCREEN_OSD_CFG = 515,             /* OSD�������� */

	PTC_LOG_OPSET_ALARMIN_CFG = 516,                /* ������������ */
	PTC_LOG_OPSET_ALARMOUT_CFG = 517,               /* ����������� */
	PTC_LOG_OPSET_ALARMOUT_OPEN_MAN = 518,          /* �ֶ������������,�˻� */
	PTC_LOG_OPSET_ALARMOUT_CLOSE_MAN = 519,         /* �ֶ��رձ������,�˻� */

	PTC_LOG_OPSET_ABNORMAL_CFG = 520,               /* �쳣���� */
	PTC_LOG_OPSET_HDD_CFG = 521,                    /* Ӳ������ */

	PTC_LOG_OPSET_NET_IP_CFG = 522 ,                /* TCP/IP���� */
	PTC_LOG_OPSET_NET_PPPOE_CFG = 523,              /* PPPOE���� */
	PTC_LOG_OPSET_NET_PORT_CFG = 524,               /* �˿����� */

	PTC_LOG_OPSET_NET_DDNS_CFG = 525,               /**DDNS����*/
	/* ά���� */
	PTC_LOG_OPSET_START_DVR = 600,                  /* ���� */
	PTC_LOG_OPSET_STOP_DVR = 601,                   /* �ػ� */
	PTC_LOG_OPSET_REBOOT_DVR = 602,                 /* �����豸 */
	PTC_LOG_OPSET_UPGRADE = 603,                    /* �汾���� */
	PTC_LOG_OPSET_LOGFILE_EXPORT = 604,             /* ������־�ļ� */
	PTC_LOG_OPSET_CFGFILE_EXPORT = 605,             /* ���������ļ� */
	PTC_LOG_OPSET_CFGFILE_IMPORT = 606,             /* ���������ļ� */
	PTC_LOG_OPSET_CONF_SIMPLE_INIT = 607,           /* �򵥻ָ����� */
	PTC_LOG_OPSET_CONF_ALL_INIT = 608,              /* �ָ��������� */
	PTC_LOG_OPSET_CONF_AUTO_MAINTAIN = 609,         /* �Զ�ά�� */

//	PTC_LOG_XM = 1000,				   /*  ����������Ϣ����־*/
	PTC_LOG_XM_Reboot				= 1001,	//����				Reboot
	PTC_LOG_XM_ShutDown				= 1002, //�ػ�				Shut Down
	PTC_LOG_XM_SaveConfig			= 1003, //��������			Save Configuration
	PTC_LOG_XM_FileAccessError		= 1004, //�����ļ�����		File Access Error
	PTC_LOG_XM_SetDriverType		= 1005, //��������������	Set Driver Type
	PTC_LOG_XM_ClearDriver			= 1006, //�������������	Clear Driver
	PTC_LOG_XM_StorageDeviceError	= 1007, //�洢�豸����		Storage Device Error
	PTC_LOG_XM_EventStart			= 1008, //������ʼ			Alarm Start
	PTC_LOG_XM_EventStop			= 1009, //��������			Alarm Stop
	PTC_LOG_XM_LogIn				= 1010, //�û���½			Log In
	PTC_LOG_XM_LogOut				= 1011, //�û��ǳ�			Log Out
	PTC_LOG_XM_AddUser				= 1012, //�����û�			Add User
	PTC_LOG_XM_DeleteUser			= 1013, //ɾ���û�			Delete User
	PTC_LOG_XM_ModifyUser			= 1014, //�޸��û�			Modify User
	PTC_LOG_XM_ModifyPassword		= 1015, //�޸�����			Modify Password
	PTC_LOG_XM_AddGroup				= 1016, //�����			Add Group
	PTC_LOG_XM_DeleteGroup			= 1017, //ɾ����			Delete Group
	PTC_LOG_XM_ModifyGroup			= 1018, //�޸���			Modify Group
	PTC_LOG_XM_ClearLog				= 1019, //�����־			Clear Log
	PTC_LOG_XM_FileSearch			= 1020, //�ļ���ѯ			File Search
	PTC_LOG_XM_FileAccess			= 1021, //�ļ���ȡ			File Access
	PTC_LOG_XM_Record				= 1022, //¼�����			Week Day
	PTC_LOG_XM_ModifyTime			= 1023, //�޸�ϵͳʱ��		Modify System Time
	PTC_LOG_XM_ZeroBitrate			= 1024, //������			No Bitrate
	PTC_LOG_XM_AccountRestore		= 1025, //�˺Ż�ԭĬ��		Account Restore
	PTC_LOG_XM_Upgrade				= 1026, //ϵͳ����			Upgrade
	PTC_LOG_XM_DiskChanged			= 1027, //�洢�豸�䶯		Disk Changed
	PTC_LOG_XM_Exception			= 1028, //ϵͳ�쳣			Exception
	PTC_LOG_XM_SaveSystemState		= 1029, //����ϵͳ״̬		Save System State
	
	PTC_LOG_QPNS_TEST				= 9999,
}PTC_LOG_SUBTYPE_E;

typedef struct
{
	unsigned int flag;		//QHKJ
	unsigned int cmd;
	unsigned int len;
	unsigned short ver;
	unsigned short ack;
	unsigned short pack_count;
	unsigned short pack_no;
	unsigned int pack_len;
	unsigned char channel_type;		//ͨ�����ͣ�0=��������1=������
	unsigned char channel_no;		//ͨ���ţ���0��ʼ
	unsigned char reserved[6];		
}ptc_head_t;

typedef struct
{
	ptc_head_t head;
	unsigned char data[PTC_MSG_SIZE];
}ptc_msg_t;

typedef struct
{
	ptc_head_t head;
	unsigned char data[PTC_VIDEO_FRAME_SIZE];
}ptc_frame_t;

typedef struct
{
	unsigned int len;		
	unsigned int no;
	unsigned int sec;
	unsigned int usec;
	unsigned short width;
	unsigned short height;
	unsigned char frame_type;			//1=I, 2=P, 3=A, 255=¼�����֡ PTC_FRAME_E
	unsigned char frame_sub_type;		//frame_type=1/2: PTC_VIDEO_ENCODE_E PTC_AUDIO_ENCODE_E
	unsigned char frame_rate;
	unsigned char security;
	unsigned char padding_bytes;
	unsigned char channel_type;			//ͨ������ PTC_CH_TYPE_E
	unsigned char channel_no;			//���㿪ʼ
	unsigned char reserve[1];
	unsigned int flag;					//PTC_FRAME_FLAG
}ptc_frame_head_t;

typedef struct
{
	ptc_frame_head_t frame_head;
	unsigned char frame_body[PTC_VIDEO_FRAME_SIZE];
}ptc_frame_video_t;

typedef struct
{
	ptc_frame_head_t frame_head;
	unsigned char frame_body[PTC_AUDIO_FRAME_SIZE];
}ptc_frame_audio_t;

typedef struct
{
	char user[PTC_ID_SIZE];
	char pswd[PTC_ID_SIZE];
	unsigned int type;
	unsigned char reserved[12];
}ptc_user_login_t;

typedef struct
{
	unsigned int cmd;
	unsigned int len;
	unsigned char reserve[8];
	unsigned char data[200*1024];
}ptc_cfg_t;

typedef struct
{
	unsigned char enable;
	unsigned char sample_rate;		
	unsigned char bit_width;			
	unsigned char encode_type;			//PTC_AUDIO_ENCODE_E
	unsigned int frame_size;		
}ptc_cfg_audio_ability_t;

typedef struct
{
	unsigned int device_type;			//�豸����
	
	unsigned char video_input_num;		//��Ƶ������Ŀ
	unsigned char audio_input_num;		//��Ƶ������Ŀ
	unsigned char alarm_input_num;		//����������Ŀ
	unsigned char alarm_output_num;		//���������Ŀ
	
	unsigned char max_login;			//����½��Ŀ��0��ʾû������
	unsigned char max_liveview;			//���ʵʱԤ����Ŀ��0��ʾû������
	unsigned char max_playback;			//���ط���Ŀ��0��ʾû������
	unsigned char max_talk;				//��Ƶ�Խ���Ŀ

	unsigned short nc_ver;				//�����汾�� 0x2102	
	unsigned short nc_date;				//�������ʱ�� 0x0105

	char dev_id[64];					//�豸Ψһ��ʾ

	unsigned char reserve[47];			//����
	char flag;							//NC�˼���
}ptc_ability_base_t;

typedef struct
{		
//	unsigned int device_type;			
//	unsigned char chip;				
//	unsigned char sensor;			
	unsigned char reserve1[6];

	unsigned char video_input_num;	
	unsigned char audio_input_num;	
	unsigned char alarm_input_num;	
	unsigned char alarm_output_num;		

//	unsigned char is_talk;			
//	unsigned char is_wifi;			
//	unsigned char is_ir_led;		
//	unsigned char is_ir_cut;		
//	unsigned char is_pir;			
//	unsigned char is_3g;			
//	unsigned char reserve1;			
//	unsigned char stream_num;		
//	unsigned char live_num;				
//	unsigned char playback_num;		
//	unsigned char d1_format;		
//	unsigned char rs485_num;		
//	unsigned char speed_min;		
//	unsigned char speed_max;		
//	unsigned char ch_preset_num;		
//	unsigned char ch_cruise_num;	
//	unsigned char cruise_preset_num;
//	unsigned char ch_track_num;		
//	ptc_cfg_video_ability_t main_video_ability[4];		//104*4
//	ptc_cfg_video_ability_t sub_video_ability[4];		//104*4
//	ptc_cfg_video_ability_t phone_video_ability;		//104*1
	unsigned char reserve2[952];

	ptc_cfg_audio_ability_t audio_ability;	
	ptc_ability_base_t base;
}ptc_cfg_ability_t;

typedef enum
{
	PTC_AUDIO_RATE_NONE = 0,
	PTC_AUDIO_RATE_8K,
}PTC_AUDIO_RATE_E;

typedef enum
{
	PTC_AUDIO_WIDTH_NONE,
	PTC_AUDIO_WIDTH_8BIT,
	PTC_AUDIO_WIDTH_16BIT,
}PTC_AUDIO_WIDTH_E;

typedef enum
{
	PTC_AUDIO_ENCODE_NONE,
	PTC_AUDIO_ENCODE_G711A,
	PTC_AUDIO_ENCODE_ADPCM,
	PTC_AUDIO_ENCODE_G711U,
	PTC_AUDIO_ENCODE_PCM,
}PTC_AUDIO_ENCODE_E;

typedef enum
{
	PTC_VIDEO_RESOLUTION_NONE = 0,
	PTC_VIDEO_RESOLUTION_QCIF,
	PTC_VIDEO_RESOLUTION_CIF,
	PTC_VIDEO_RESOLUTION_VGA,
	PTC_VIDEO_RESOLUTION_D1,
	PTC_VIDEO_RESOLUTION_720P,
	PTC_VIDEO_RESOLUTION_960H,
	PTC_VIDEO_RESOLUTION_960P,
	PTC_VIDEO_RESOLUTION_1080P,
}PTC_VIDEO_RESOLUTION_E;

typedef enum
{
	PTC_VIDEO_ENCODE_NONE = 0,
	PTC_VIDEO_ENCODE_H264,
	PTC_VIDEO_ENCODE_MJPEG,
	PTC_VIDEO_ENCODE_MPEG4,
}PTC_VIDEO_ENCODE_E;

typedef enum
{
	PTC_VIDEO_BITTYPE_NONE = 0,
	PTC_VIDEO_BITTYPE_CBR,
	PTC_VIDEO_BITTYPE_VBR,
}PTC_VIDEO_BITTYPE_E;


typedef enum
{
	PTC_SERIAL_PROTOCOL_NONE = 0,
	PTC_SERIAL_PROTOCOL_PELCO_D,
	PTC_SERIAL_PROTOCOL_PELCO_P,
}PTC_SERIAL_PROTOCOL_E;

typedef enum
{
	PTC_SERIAL_DATABIT_NONE = 0,
	PTC_SERIAL_DATABIT_5 = 5,
	PTC_SERIAL_DATABIT_6,
	PTC_SERIAL_DATABIT_7,
	PTC_SERIAL_DATABIT_8,
}PTC_SERIAL_DATABIT_E;

typedef enum
{
	PTC_SERIAL_STOPBIT_NONE = 0,
	PTC_SERIAL_STOPBIT_1,
	PTC_SERIAL_STOPBIT_1_5,
	PTC_SERIAL_STOPBIT_2,
}PTC_SERIAL_STOPBIT_E;

typedef enum
{
	PTC_SERIAL_PARITY_NULL = 0,
	PTC_SERIAL_PARITY_NONE,
	PTC_SERIAL_PARITY_ODD,
	PTC_SERIAL_PARITY_EVEN,
}PTC_SERIAL_PARITY_E;


#define PTC_YEAR_MIN			1970
#define PTC_YEAR_MAX			2030
#define PTC_MONTH_MIN			1
#define PTC_MONTH_MAX			12
#define PTC_DAY_MIN				1
#define PTC_DAY_MAX				31
#define PTC_WEEKINMONTH_MIN		1
#define PTC_WEEKINMONTH_MAX		5
#define PTC_WEEKDAY_MIN			0
#define PTC_WEEKDAY_MAX			6
#define PTC_HOUR_MIN			0
#define PTC_HOUR_MAX			23
#define PTC_MIN_MIN				0
#define PTC_MIN_MAX				59
#define PTC_SEC_MIN				0
#define PTC_SEC_MAX				59

typedef enum
{
	PTC_TIME_DATEMODE_NONE = 0,
	PTC_TIME_DATEMODE_YMD,
	PTC_TIME_DATEMODE_MDY,
	PTC_TIME_DATEMODE_DMY,
}PTC_TIME_DATEMODE_E;

typedef enum
{
	PTC_TIME_TIMEMODE_NONE = 0,
	PTC_TIME_TIMEMODE_12,
	PTC_TIME_TIMEMODE_24,
}PTC_TIME_TIMEMODE_E;

typedef enum
{
	PTC_TIME_DSTMODE_NONE = 0,
	PTC_TIME_DSTMODE_WEEK,
	PTC_TIME_DSTMODE_DATE,
}PTC_TIME_DSTMODE_E;

typedef enum
{
	PTC_TIME_SHIFTHOUR_NONE = 0,
	PTC_TIME_SHIFTHOUR_1,
	PTC_TIME_SHIFTHOUR_2,
	PTC_TIME_SHIFTHOUR_3,
}PTC_TIME_SHIFTHOUR_E;


#define PTC_TIME_ZONE_W1200		(-48)	//(-12*60*60)
#define PTC_TIME_ZONE_W1100		(-44)	//(-11*60*60)
#define PTC_TIME_ZONE_W1000		(-40)	//(-10*60*60)
#define PTC_TIME_ZONE_W0900		(-36)	//(-9*60*60)
#define PTC_TIME_ZONE_W0800		(-32)	//(-8*60*60)
#define PTC_TIME_ZONE_W0700		(-28)	//(-7*60*60)
#define PTC_TIME_ZONE_W0600		(-24)	//(-6*60*60)
#define PTC_TIME_ZONE_W0500		(-20)	//(-5*60*60)
#define PTC_TIME_ZONE_W0430		(-18)	//(-4*60*60-30*60)
#define PTC_TIME_ZONE_W0400		(-16)	//(-4*60*60)
#define PTC_TIME_ZONE_W0330		(-14)	//(-3*60*60-30*60)
#define PTC_TIME_ZONE_W0300		(-12)	//(-3*60*60)
#define PTC_TIME_ZONE_W0200		(-8)	//(-2*60*60)
#define PTC_TIME_ZONE_W0100		(-4)	//(-1*60*60)
#define PTC_TIME_ZONE_C0000		(0)		//(0)
#define PTC_TIME_ZONE_E0100		(4)		//(1*60*60)
#define PTC_TIME_ZONE_E0200		(8)		//(2*60*60)
#define PTC_TIME_ZONE_E0300		(12)	//(3*60*60)
#define PTC_TIME_ZONE_E0330		(14)	//(3*60*60+30*60)
#define PTC_TIME_ZONE_E0400		(16)	//(4*60*60)
#define PTC_TIME_ZONE_E0430		(18)	//(4*60*60+30*60)
#define PTC_TIME_ZONE_E0500		(20)	//(5*60*60)
#define PTC_TIME_ZONE_E0530		(22)	//(5*60*60+30*60)
#define PTC_TIME_ZONE_E0545		(23)	//(5*60*60+45*60)
#define PTC_TIME_ZONE_E0600		(24)	//(6*60*60)
#define PTC_TIME_ZONE_E0630		(26)	//(6*60*60+30*60)
#define PTC_TIME_ZONE_E0645		(27)	//(6*60*60+30*60)
#define PTC_TIME_ZONE_E0700		(28)	//(7*60*60)
#define PTC_TIME_ZONE_E0800		(32)	//(8*60*60)
#define PTC_TIME_ZONE_E0900		(36)	//(9*60*60)
#define PTC_TIME_ZONE_E0930		(38)	//(9*60*60+30*60)
#define PTC_TIME_ZONE_E1000		(40)	//(10*60*60)
#define PTC_TIME_ZONE_E1100		(44)	//(11*60*60)
#define PTC_TIME_ZONE_E1200		(48)	//(12*60*60)
#define PTC_TIME_ZONE_E1300		(52)	//(13*60*60)

typedef struct
{
	unsigned int utc_sec;				//ʱ������
	int zone;							//ʱ��ƫ����

	char ntp_ip[PTC_IP_SIZE];			//ntp ��ַ
	
	unsigned short ntp_port;			//ntp�˿�
	unsigned char ntp_enable;			//PTC_ENABLE_E
	unsigned char dst_enable; 			//PTC_ENABLE_E
	
	unsigned short start_year;			//��ʱ���ã�������
	unsigned char start_month; 			//1-12
	unsigned char start_day; 			//1-31
	
	unsigned char start_weekinMonth; 	//1-5
	unsigned char start_dayofWeek;		//0-6	 0������
	unsigned char start_hour; 			//0-23
	unsigned char start_min; 			//0-59
	
	unsigned char start_sec; 			//0-59
	unsigned short end_year;			//��ʱ���ã�������
	unsigned char end_month; 			//1-12
	
	unsigned char end_day;	 			//1-31
	unsigned char end_weekinMonth; 		//1-5
	unsigned char end_dayofWeek; 		//0-6	 0������
	unsigned char end_hour; 			//0-23
	
	unsigned char end_min; 				//0-59
	unsigned char end_sec; 				//0-59
	unsigned char shift_hour; 			//PTC_TIME_SHIFTHOUR_E
	unsigned char status;				//��ʱ���ã������� 0δƫ��  1��ƫ��
	
	unsigned char dst_hour;				//�Ѿ�ƫ����
	unsigned char dst_mode;				//PTC_TIME_DSTMODE_E
	unsigned char date_mode;			//���ڸ�ʽ
	unsigned char time_mode;			//ʱ���ʽ

	unsigned char reserve[12];			
}ptc_cfg_time_t;

typedef struct
{
	int year;				//��
	int month;				//��
	int day;				//��
}ptc_cfg_date_t;

typedef struct
{
	unsigned long long channel;
	unsigned char type;					//PTC_RECORD_TYPE_E
	unsigned char sub_type;				//PTC_RECORD_SUBTYPE_E
	unsigned char reserve[10];		
	unsigned int num;
	ptc_cfg_date_t date[PTC_DATES_SIZE];
}ptc_cfg_dates_t;

typedef struct
{
	unsigned int start_time;
	unsigned int end_time;
	unsigned int size;
	unsigned char channel;				//���㿪ʼ
	unsigned char type;					//PTC_RECORD_TYPE_E
	unsigned char sub_type;				//PTC_RECORD_SUBTYPE_E
	unsigned char reserve[17];			
	unsigned char file_info[128];		//�豸�ļ���Ϣ
}ptc_cfg_record_t;

typedef struct
{
	unsigned int start_time;
	unsigned int end_time;
	unsigned long long channel;
	unsigned char type;					//PTC_RECORD_TYPE_E
	unsigned char sub_type;				//PTC_RECORD_SUBTYPE_E
	unsigned char reserve[10];			
	unsigned int num;
	ptc_cfg_record_t record[PTC_RECORDS_SIZE];
}ptc_cfg_records_t;

typedef struct
{
	unsigned long long io_in;			//IO��������״̬: 0=�رգ�1=��
	unsigned long long io_out;			//IO�������״̬ 0=�رգ�1=��
	unsigned long long motion;			//�ƶ����״̬ 0=�رգ�1=��
	unsigned long long loss;			//��Ƶ��ʧ״̬ 0=�رգ�1=��
	unsigned long long dick;			//���̱���״̬ 0=�رգ�1=��
	unsigned long long manual_record;	//�ֶ�¼��״̬ 0=�رգ�1=��
	unsigned long long time_record;		//��ʱ¼��״̬ 0=�رգ�1=��
	unsigned long long motion_record;	//�ƶ����¼��״̬ 0=�رգ�1=��
	unsigned long long io_record;		//IO����¼�� 0=�رգ�1=��
	unsigned char ir_cut;				//ir-cut״̬ 0=���죬1=����
	unsigned char ir_input;				//��������״̬ 0=�رգ�1=��
	unsigned char pir;					//PIR״̬ 0=�رգ�1=��
	unsigned char alarm_led;			//�����״̬ 0=�رգ�1=��
	unsigned char volume;				//����:0-10
	unsigned char reserve[75];
	unsigned long long poc;				//POC״̬ 0=�رգ�1=��
}ptc_cfg_status_t;

typedef struct 
{
	unsigned char enable1;
	unsigned char dhcp1;
	unsigned char enable2;
	unsigned char dhcp2;
	
	//eth
	unsigned char ip1[4];
	unsigned char mask1[4];
	unsigned char gate1[4];
	unsigned char mac1[PTC_MAC_SIZE];

	//ro
	unsigned char ip2[4];
	unsigned char mask2[4];
	unsigned char gate2[4];
	unsigned char mac2[PTC_MAC_SIZE];

	//port
	unsigned short web_port;
	unsigned short dev_port;
	unsigned short rtsp_port;
	unsigned short onvif_port;

	//dns
	unsigned char dns1[4];
	unsigned char dns2[4];	

	// multicast
	unsigned char multicast_ip[4];
	unsigned short multicast_port;

	unsigned short stream_port;
	unsigned char reserve2[188];
}ptc_cfg_net_t;

typedef struct
{
	char username[PTC_ID_SIZE];
	char password[PTC_ID_SIZE];
	
	unsigned char reserve[64];
}ptc_cfg_user_t;

typedef struct
{
	char mac[PTC_MAC_SIZE];
}ptc_cfg_bindmac_t;

#define PTC_USER_MAX		64
#define PTC_BINDMAC_MAX		16

typedef struct
{
	ptc_cfg_user_t user[PTC_USER_MAX];
	
	int enable;
	ptc_cfg_bindmac_t macs[PTC_BINDMAC_MAX];
	
	unsigned char reserve[124];
}ptc_cfg_security_t;



typedef struct
{
	ptc_ability_base_t	base;
	ptc_cfg_net_t 		net;
	unsigned char 		reserve[124];
	unsigned int 		nc_ptc_type;	//NC_PTC_E
}ptc_mcast_search_t;

#define PTC_MCAST_SEARCH_SIZE		256

typedef struct
{
	unsigned short num;
	unsigned char reserve[6];
	ptc_mcast_search_t search[PTC_MCAST_SEARCH_SIZE];
}ptc_mcast_searchs_t;

typedef struct
{
	unsigned char channel_no;
	unsigned char channel_type;
	unsigned char reserve[14];
}ptc_rts_start_t;

typedef struct
{
	unsigned char channel_no;
	unsigned char cmd;
	unsigned char param;
	unsigned char reserve[13];
}ptc_control_ptz_t;

typedef struct
{
	unsigned int time;
	unsigned char type;			//PTC_LOG_TYPE_E
	unsigned char sub_type;		//PTC_LOG_SUBTYPE_E
	unsigned char channel;		//
	unsigned char ch_type;		//PTC_CH_TYPE_E
	unsigned short sub_type2;	//PTC_LOG_SUBTYPE_E
	unsigned char reserve[2];
	unsigned char ip[4];
}ptc_log_t;

typedef struct
{
	unsigned int start_time;
	unsigned int end_time;
	unsigned char reserve[4];
	unsigned int num;
	ptc_log_t log[PTC_LOGS_SIZE];
}ptc_logs_t;

typedef struct
{
	unsigned int time;
	unsigned char type;			//PTC_LOG_TYPE_E
	unsigned char sub_type;		//PTC_LOG_SUBTYPE_E
	unsigned char channel;		//
	unsigned char ch_type;		//PTC_CH_TYPE_E
	unsigned short sub_type2;	//PTC_LOG_SUBTYPE_E
	unsigned char reserve[2];
	unsigned char ip[4];

	char data[64];
	unsigned char reserve2[16];
}ptc_log_v2_t;

typedef struct
{
	unsigned int start_time;
	unsigned int end_time;
	unsigned char reserve[4];
	unsigned int num;
	ptc_log_v2_t log_v2[PTC_LOGS_SIZE];
}ptc_logs_v2_t;

////////////////////////////////////////////////////////////////////////////

typedef enum
{
	PTC_OK						= 0,
	PTC_ERROR_JSON				= 1,
	PTC_ERROR_CMD				= 2,
	PTC_ERROR_NO_ALARM_TYPE		= 3,
}PTC_ERROR_E;

typedef enum
{
	PTC_ALARM_TYPE_VMF_ENTER	= 0, //���ֱ���VMF(Video Motion Filter) 
	PTC_ALARM_TYPE_VMF_LOSS		= 1, //��ʧ����
	PTC_ALARM_TYPE_VMF_LINE 	= 2, //�߽籨��
	PTC_ALARM_TYPE_TA_FOCUS		= 3, //�齹����TA(Tampering alarm)
	PTC_ALARM_TYPE_TA_COVER		= 4, //�ڵ�����
	
	PTC_ALARM_TYPE_MAX,
}PTC_ALARM_TYPE_E;

typedef struct
{
	float x;						//x�����ߣ��ٷֱ�
	float y;						//y�����꣬�ٷֱ�
}ptc_point_t;

typedef struct
{
	ptc_point_t right_down;			//���µ�
	ptc_point_t left_down;			//���µ�
	ptc_point_t left_up;			//���ϵ�
	ptc_point_t right_up;			//���ϵ�
}ptc_rect_t;

typedef enum
{
	PTC_VMF_LINE_DRT_BOTH		= 0,//˫��
	PTC_VMF_LINE_DRT_RIGHT		= 1,//����
	PTC_VMF_LINE_DRT_LEFT		= 2,//����
}PTC_VMF_LINE_DRT_E;

typedef struct
{
	int enable;						//ʹ�ܣ�0=�رգ�1=����
	ptc_rect_t rect;				//���ο�
	PTC_VMF_LINE_DRT_E drt;			//���򣬽��߽籨����Ч
}ptc_cfg_alarm_t;

typedef enum
{
	PTC_WORKMODE_POWER		= 0,	//��籦ģʽ
	PTC_WORKMODE_3G2WiFi	= 1,	//3GתWiFiģʽ
	PTC_WORKMODE_WiFi		= 2,	//��Ƶģʽ
	PTC_WORKMODE_SAVE		= 3,	//ʡ��ģʽ

	PTC_WORKMODE_MAX,
}PTC_WORK_MODE_E;

typedef struct
{
	PTC_WORK_MODE_E mode;			//����ģʽ
}ptc_cfg_workMode_t;

typedef struct
{
	int enable_3g;					//3Gʹ��: 0=��ʹ�ܣ�1=ʹ��
	int status_3g;					//3G״̬: 0:����ʧ�ܣ�1=���ųɹ���(ֻ��)
}ptc_cfg_mobile_t;

typedef enum
{
	PTC_WiFi_MODE_AP		= 0,	//APģʽ
	PTC_WiFi_MODE_Client	= 1,	//�ͻ���ģʽ

	PTC_WiFi_MODE_MAX
}PTC_WiFi_MODE_E;

typedef struct
{
	PTC_WiFi_MODE_E mode;			//WiFiģʽ

	char client_ssid[64];			//�ͻ���ģʽ-SSID
	char client_pswd[64];			//�ͻ���ģʽ-����
	int  client_level;				//�ͻ���ģʽ-�ź�ǿ�ȣ�ֻ��
	int  client_dhcp;				//�ͻ���ģʽ-�Ƿ���DHCP, 0=�ֶ�, 1=DHCP
	unsigned char client_ip[4];		//�ͻ���ģʽ-IP��ַ
	unsigned char client_mask[4];	//�ͻ���ģʽ-��������
	unsigned char client_gate[4];	//�ͻ���ģʽ-����
	unsigned char client_dns1[4];	//�ͻ���ģʽ-DNS1
	unsigned char client_dns2[4];	//�ͻ���ģʽ-DNS2

	char ap_ssid[64];				//�ȵ�ģʽ-SSID��ֻ��
	char ap_pswd[64];				//�ȵ�ģʽ-����

}ptc_cfg_wifi_t;

typedef struct
{
	int enable;						//ʹ�ܣ�0=�رգ�1=����
	int cover_type;					//�洢���� 0=�Զ�����,1=������

	int state;						//Ӳ��״̬��0=�޴��̣�1=���У�2=ʹ����
	int total_size;					//����������λMB
	int left_size;					//ʣ����������λMB
}ptc_cfg_devRecord_t;

typedef struct
{
	int mask;						//��Ƶ�ڵ� 0=���ڵ���1=�ڵ�
}ptc_cfg_av_t;

typedef struct
{
	int power;							//����1-100
	int wifisg;							//wifi�ź�	1-100
}ptc_cfg_info_t;


///////////////////////////////////////////////////////// HDCCTV-Cloud
////////////////////////////////////////////////////////////////////
#define HDCCTV_QPNS_MOBILE_MAP_NUM				8
#define HDCCTV_QPNS_DEVICE_MAP_NUM				32
#define HDCCTV_QPNS_TOKEN_SIZE					72
#define HDCCTV_QPNS_CONTENT_SIZE				128

#define HDCCTV_QPNS_CN_SERVER_IP				"www.hdcctv7.com"

#ifdef QH_PTC_DEBUG_LOCAL
#define HDCCTV_QPNS_IN_SERVER_DOMAIN			"192.168.8.226"
#define HDCCTV_QPNS_IN_SERVER_IP				"192.168.8.226"
#else
#define HDCCTV_QPNS_IN_SERVER_DOMAIN			"www.hdcctvddns.com"
#define HDCCTV_QPNS_IN_SERVER_IP				"208.109.240.232"
#endif

#define HDCCTV_QPNS_SERVER_PORT					80
#define HDCCTV_QPNS_QPNS_PORT					81
#define HDCCTV_MSGS_DEVICE_PORT					82
#define HDCCTV_MSGS_CLIENT_PORT					83

typedef enum
{
	HDCCTV_ERROR_SERVER_BASE				= 0,		
	HDCCTV_ERROR_OPEN_DATABASE				= 1,		//�����ݿ�ʧ��
	HDCCTV_ERROR_USE_DATABASE				= 2,		//ѡ�����ݿ�ʧ��
	HDCCTV_ERROR_SELECT_SMS_BY_MOBILE		= 3,		//�����ֻ��Ų�ѯSMS����ʧ��
	HDCCTV_ERROR_SELECT_USER_BY_MOBILE		= 4,		//�����ֻ��Ų�ѯ�û�����ʧ��
	HDCCTV_ERROR_INSERT_SMS					= 5,		//����SMS��¼ʧ��
	HDCCTV_ERROR_UPDATA_MOBILE				= 6,		//����SMS��¼ʧ��
	HDCCTV_ERROR_SELECT_USER_BY_NAME		= 7,		//�����û�����ѯ�û�ʧ��
	HDCCTV_ERROR_INSERT_USER_BY_NAME		= 8,		//�����û���¼ʧ��
	HDCCTV_ERROR_SELECT_DEVICE_BY_ID		= 9,		//���ݱ�ʾ��ѯ�豸����ʧ��
	HDCCTV_ERROR_INSERT_DEVICE				= 10,		//�����豸��¼ʧ��
	HDCCTV_ERROR_UPDATA_DEVICE				= 11,		//�����豸��¼ʧ��
	HDCCTV_ERROR_UPDATA_USER				= 12,		//�����û���¼ʧ��
	HDCCTV_ERROR_SELECT_MAP_BY_TOKEN		= 13,		//�����ֻ���ʾ��ѯ�豸�б�ʧ��
	HDCCTV_ERROR_DELETE_MAP_BY_TOKEN		= 14,		//�����ֻ���ʾɾ���豸�б�ʧ��
	HDCCTV_ERROR_INSERT_MAP					= 15,		//����QPNSӳ���¼ʧ��
	HDCCTV_ERROR_INSERT_ALARM 				= 16,		//���뱨����Ϣ��¼ʧ��
	HDCCTV_ERROR_SELECT_MAP_BY_ID			= 17,		//�����豸��ʾ��ѯ�豸�б�ʧ��
	HDCCTV_ERROR_SELECT_USER_BY_TOKEN		= 18,		//�����û���ʾ��ѯ�û�ʧ��
	HDCCTV_ERROR_INSERT_USER_BY_TOKEN		= 19,		//�����û���¼ʧ��
	HDCCTV_ERROR_DELETE_MAP_BY_ID_AND_TOKEN	= 20,		//�����ֻ���ʾ���豸��ʾɾ���豸�б�ʧ��
	HDCCTV_ERROR_SELECT_MAP_BY_ID_AND_TOKEN	= 21,		//�����ֻ���ʾ���豸��ʾ��ѯ�豸�б�ʧ��
	HDCCTV_ERROR_SELECT_ALARM_BY_TOKEN		= 22,		//�����ֻ���ʾ����������Ϣʧ��
	HDCCTV_ERROR_SELECT_RELAYMAP_BY_DEVID	= 23,		//�����豸��ʾ����relayӳ���б�
	HDCCTV_ERROR_INSERT_RELAYMAP			= 24,		//����Relayӳ���¼ʧ��
	HDCCTV_ERROR_SELECT_SERVER_BY_ID		= 25,		//���ݷ�������ʾ��ѯ������ʧ��
	HDCCTV_ERROR_SELECT_USER_BY_EMAIL		= 26,		//���������ַ��ѯ�û�ʧ��
	HDCCTV_ERROR_SELECT_SERVICE_BY_USERNAME	= 27,		//�����û�����ѯ����ӳ���б�ʧ��
	HDCCTV_ERROR_DELETE_SERVICE_BY_ID_AND_U = 28,		//�����豸��ʾ���û���ɾ��SERVICE��¼��
	HDCCTV_ERROR_UPDATA_RELAYMAP			= 29,		//����Relayӳ���¼ʧ��
	HDCCTV_ERROR_SELECT_SERVICE_BY_ID_AND_U	= 30,		//�����豸��ʾ���û���ʾ����SERVICE��¼ʧ��

	HDCCTV_ERROR_SELECT_USER_BY_NAME_AND_PSWD		= 31,		//�����û���ʾ����������Cloud�û�ʧ��
	HDCCTV_ERROR_SELECT_USER_BY_MOBILE_AND_PSWD		= 32,		//�����ֻ��������������Cloud�û�ʧ��
	HDCCTV_ERROR_SELECT_USER_BY_EMAIL_AND_PSWD		= 33,		//���������ַ����������Cloud�û�ʧ��
	HDCCTV_ERROR_SELECT_SERVER_BY_INFO				= 34,		//���ݷ�������Ϣ��ѯ������ʧ��
	HDCCTV_ERROR_SELECT_SERVICE_BY_SERVERID			= 35,		//���ݷ�������ʾ��ѯSERVICE��¼ʧ��
	HDCCTV_ERROR_SELECT_DEVICE_FLOW_BY_ID	 		= 36,		//�����豸��ʾ��ѯ�豸����ʧ��
	HDCCTV_ERROR_SELECT_CLIENT_FLOW_BY_ID			= 37,		//�����û���ʾ��ѯ�û�����ʧ��
	HDCCTV_ERROR_INSERT_DEVICE_FLOW					= 38,		//�����豸������¼ʧ��
	HDCCTV_ERROR_INSERT_CLIENT_FLOW					= 39,		//�����û�������¼ʧ��
	HDCCTV_ERROR_UPDATE_DEVICE_FLOW 				= 40,		//�����豸������¼ʧ��
	HDCCTV_ERROR_UPDATE_CLIENT_FLOW 				= 41,		//�����û�������¼ʧ��
	HDCCTV_ERROR_SELECT_DEVICE_SPACE_BY_ID			= 42,		//�����豸��ʾ��ѯ�豸�ռ�ʧ��
	HDCCTV_ERROR_INSERT_DEVICE_SPACE 				= 43,		//�����豸�ռ��¼ʧ��
	HDCCTV_ERROR_UPDATE_DEVICE_SPACE 				= 44,		//�����豸�ռ��¼ʧ��
	HDCCTV_ERROR_UPDATE_SERVICE_BY_ID_AND_U			= 45,		//�����豸��ʾ���û���ʾ����SERVICE��¼ʧ��

	
	HDCCTV_ERROR_CLIENT_BASE				= 10000,
	HDCCTV_ERROR_MOBILE_NUM					= 10001,	//�ֻ��Ÿ�ʽ����
	HDCCTV_ERROR_REPEAT_MOBILE				= 10002,	//�ֻ����Ѿ���ע��
	HDCCTV_ERROR_TODAY_COUNT				= 10003,	//�ﵽÿ�����������֤���������
	HDCCTV_ERROR_TOO_FREQUENT				= 10004,	//���������֤�����Ƶ����
	HDCCTV_ERROR_PASSWORD_LEN				= 10005,	//���볤�ȴ���
	HDCCTV_ERROR_PASSWORD_CONTENT			= 10006,	//�������ݴ���
	HDCCTV_ERROR_USERNAME_LEN				= 10007,	//�û������ȹ���
	HDCCTV_ERROR_USERNAME_CONTENT			= 10008,	//�û������ݴ���
	HDCCTV_ERROR_REPEAT_USERNAME			= 10009,	//�û����Ѿ���ע��
	HDCCTV_ERROR_NONE_CODE					= 10010,	//��֤�벻����
	HDCCTV_ERROR_WRONG_CODE					= 10011,	//��֤�벻��ȷ
	HDCCTV_ERROR_CODE_LEN					= 10013,	//��֤�볤�ȴ���
	HDCCTV_ERROR_EMAIL_LEN					= 10014,	//�����ַ���ȴ���
	HDCCTV_ERROR_EMAIL_CONTENT				= 10015,	//�����ַ���ݴ���
	HDCCTV_ERROR_UDID_LEN					= 10016,	//�ֻ���ʾ���ȴ���
	HDCCTV_ERROR_UDID_CONTENT				= 10017,	//�ֻ���ʾ���ݴ���
	HDCCTV_ERROR_DEVID_LEN					= 10018,	//�豸��ʾ���ȴ���
	HDCCTV_ERROR_DEVID_CONTENT				= 10019,	//�豸��ʾ���ݴ���
	HDCCTV_ERROR_IP_LEN						= 10020,	//�����ַ���ȴ���
	HDCCTV_ERROR_IP_CONTENT					= 10021,	//�����ַ���ݴ���
	HDCCTV_ERROR_PORT_NAME_LEN				= 10022,	//�˿��б����Ƴ��ȴ���
	HDCCTV_ERROR_PORT_NAME_CONTENT			= 10023,	//�˿��б��������ݴ���
	HDCCTV_ERROR_PORT_VLAUE_LEN				= 10024,	//�˿��б�ֵ���ȴ���
	HDCCTV_ERROR_PORT_VLAUE_CONTENT			= 10025,	//�˿��б�ֵ���ݴ���
	HDCCTV_ERROR_NONE_DEVID					= 10026,	//�豸��ʾ������
	HDCCTV_ERROR_NONE_USER					= 10027,	//�û���������
	HDCCTV_ERROR_WRONG_PASSWORD				= 10028,	//�������
	HDCCTV_ERROR_REPEAT_DEVICE				= 10029,	//�豸�Ѿ��������û���
	HDCCTV_ERROR_BIND_ACTION_CONTENT		= 10030,	//�󶨲����ֶ����ݴ���
	HDCCTV_ERROR_DEV_NAME_LEN				= 10031,	//�豸���Ƴ��ȴ���
	HDCCTV_ERROR_MAX_DEVICE_NUM				= 10032,	//�ﵽ����豸��������
	HDCCTV_ERROR_TIME_LEN					= 10033,	//������Ϣʱ�䳤�ȴ���
	HDCCTV_ERROR_TIME_CONTENT				= 10034,	//������Ϣʱ�����ݴ���
	HDCCTV_ERROR_CHANNEL_LEN				= 10035,	//������Ϣͨ�����ȴ���
	HDCCTV_ERROR_CHANNEL_CONTENT			= 10036,	//������Ϣͨ�����ݴ���
	HDCCTV_ERROR_TYPE_LEN					= 10037,	//������Ϣ���ͳ��ȴ���
	HDCCTV_ERROR_TYPE_CONTENT				= 10038,	//������Ϣ�������ݴ���
	HDCCTV_ERROR_CONTENT_LEN				= 10039,	//������Ϣ���ݳ��ȴ���
	HDCCTV_ERROR_TOKEN_TYPE_LEN				= 10040,	//QPNSӳ���ֻ����ͳ��ȴ���
	HDCCTV_ERROR_TOKEN_TYPE_CONTENT			= 10041,	//QPNSӳ���ֻ��������ݴ���
	HDCCTV_ERROR_QPNS_IDLE					= 10042,	//QPNS����δ����
	HDCCTV_ERROR_NONE_TOKEN					= 10043,	//�û���ʾ������
	HDCCTV_ERROR_ALARM_NO_LEN				= 10044,	//������ʾ���ȴ���
	HDCCTV_ERROR_ALARM_NO_CONTENT			= 10045,	//������ʾ���ݴ���
	HDCCTV_ERROR_MAP_ACTION_CONTENT 		= 10046,	//ӳ������ֶδ���
	HDCCTV_ERROR_LANGUAGE_LEN 				= 10047,	//�����ֶγ��ȴ���
	HDCCTV_ERROR_LANGUAGE_CONTENT 			= 10048,	//�����ֶ����ݴ���
	HDCCTV_ERROR_REPEAT_EMAIL				= 10049,	//�����ַ�Ѿ���ע��
	HDCCTV_ERROR_NONE_BINDING				= 10050,	//�豸δ��
	HDCCTV_ERROR_NONE_AUTHORITY				= 10051,	//û��Ȩ��
	HDCCTV_ERROR_SMS_ACTION_CONTENT			= 10052,	//���������֤������ֶ����ݴ���
	HDCCTV_ERROR_USER_OR_PWSD		 		= 10053,	//�û��������������
	HDCCTV_ERROR_CODE_TIMEOUT		 		= 10054,	//��֤�����
	HDCCTV_ERROR_SERVER_ID_LEN				= 10055,	//��������ʾ���ȴ���
	HDCCTV_ERROR_SERVER_ID_CONTENT			= 10056,	//��������ʾ���ݴ���	
	HDCCTV_ERROR_SERVER_PORT_CONTENT		= 10057,	//�������˿����ݴ���	
	HDCCTV_ERROR_SERVER_PORT_VALUE			= 10058,	//�������˿���ֵ������Χ
	HDCCTV_ERROR_SERVER_TYPE_CONTENT		= 10059,	//�������������ݴ���	
	HDCCTV_ERROR_SERVER_TYPE_VALUE			= 10060,	//������������ֵ������Χ
	HDCCTV_ERROR_SERVER_NAME_LEN			= 10061,	//���������Ƴ��ȴ���
	HDCCTV_ERROR_NONE_SERVER				= 10062,	//������������
	HDCCTV_ERROR_FLOW_CONTENT				= 10063,	//�������ݴ���
	HDCCTV_ERROR_AUTHORITY					= 10064,	//Ȩ�޴���
	HDCCTV_ERROR_DEV_OFFLINE				= 10065,	//�豸������
	HDCCTV_ERROR_MAX_PBS_NUM				= 10066,	//�ﵽ���ط���
	HDCCTV_ERROR_PROTOCOL					= 10067,	//Э�����
	HDCCTV_ERROR_OPEN_PBS					= 10068,	//�򿪻ط���ʧ��
	HDCCTV_ERROR_TALK_BUSY					= 10069,	//�Խ��Ѿ���
	HDCCTV_ERROR_OPEN_TALK					= 10070,	//�򿪶Խ�ʧ��

	
	HDCCTV_ERROR_SMS_BASE					= 20000,
	
}HDCCTV_ERROR_E;

typedef enum
{
	HDCCTV_LOCATION_US		= 0, 				//����
	HDCCTV_LOCATION_CN		= 10000000, 		//�й�
	
}HDCCTV_LOCATION_E;

typedef struct
{
	char dev_id[HDCCTV_QPNS_TOKEN_SIZE];		//�豸Ψһ��ʾ
	char dev_name[PTC_ID_SIZE];					//�豸�� utf8
	char in_ipv4[PTC_IP_SIZE];					//������ַ
	char ex_ipv4[PTC_IP_SIZE];					//������ַ
	int http_port;								//
	int sdk_port;								//
	int stream_port;							//
	int rtsp_port;								//
	int onvif_port;								//
}hdcctv_device_t;

#define HDCCTV_CLIENT_DEVICE_NUM		100

typedef struct
{
	int num;									//�豸����
	hdcctv_device_t device[HDCCTV_CLIENT_DEVICE_NUM];
}hdcctv_device_list_t;

typedef struct
{
	char username[PTC_STR_SIZE];			//�û���
	char password[PTC_STR_SIZE];			//����
	char mobile[PTC_STR_SIZE];				//�ֻ���
	char email[PTC_STR_SIZE];				//��������
	HDCCTV_LOCATION_E location;				//HDCCTV_LOCATION_E
	char user_ipv4[PTC_IP_SIZE];			//�û�������ַ
}hdcctv_cloud_user_t;


typedef enum
{
	HDCCTV_QPNS_TOKEN_TYPE_ANDROID			= 0,
	HDCCTV_QPNS_TOKEN_TYPE_IOS_ISSMOBILE	= 1000,
	HDCCTV_QPNS_TOKEN_TYPE_IOS_IDARLING 	= 2000,
}HDCCTV_QPNS_TOKEN_TYPE_E;

typedef enum
{
	HDCCTV_LANUAGE_EN		= 0,
	HDCCTV_LANUAGE_ZH_CN	= 10,
	
}HDCCTV_LANUAGE_E;

typedef struct
{
	char dev_id[HDCCTV_QPNS_TOKEN_SIZE];
	char token[HDCCTV_QPNS_TOKEN_SIZE];
	HDCCTV_QPNS_TOKEN_TYPE_E token_type;
	int push_count;
	int success_count;
	int fail_count;
	int last_time;
}hdcctv_qpns_map_t;

typedef struct
{
	int num;
	hdcctv_qpns_map_t mobile_map[HDCCTV_QPNS_MOBILE_MAP_NUM];
}hdcctv_qpns_mobile_maps_t;

typedef struct
{
	int num;
	hdcctv_qpns_map_t device_map[HDCCTV_QPNS_DEVICE_MAP_NUM];
}hdcctv_qpns_device_maps_t;

typedef enum
{
	HDCCTV_QPNS_ALARM_STATE_RECV				= 0,
	HDCCTV_QPNS_ALARM_STATE_SEND				= 1,
	HDCCTV_QPNS_ALARM_STATE_ARRIVE				= 2,
	HDCCTV_QPNS_ALARM_STATE_ERROR				= 9,
}HDCCTV_QPNS_ALARM_STATE_E;

typedef struct
{
	unsigned int no;						//�豸���ø��ֶ���Ϊ�Ƿ��ͱ�־λ
	char dev_id[HDCCTV_QPNS_TOKEN_SIZE];	
	char token[HDCCTV_QPNS_TOKEN_SIZE];
	HDCCTV_QPNS_TOKEN_TYPE_E token_type;
	int time;
	int channel;				//32λ��λ��
	int type;
	char content[HDCCTV_QPNS_CONTENT_SIZE];
	HDCCTV_QPNS_ALARM_STATE_E state;
}hdcctv_qpns_alarm_t;

#define HDCCTV_QPNS_ALARM_NUM		32

typedef struct
{
	int num;
	hdcctv_qpns_alarm_t alarm[HDCCTV_QPNS_ALARM_NUM];
}hdcctv_qpns_alarms_t;

typedef struct
{
	char token[HDCCTV_QPNS_TOKEN_SIZE];		//�ֻ�Ψһ��ʾ
	HDCCTV_QPNS_TOKEN_TYPE_E token_type;	//�ֻ�����
	HDCCTV_LOCATION_E location;				//HDCCTV_LOCATION_E
	HDCCTV_LANUAGE_E language;				//����	
	char ipv4[PTC_IP_SIZE];					//
	int send_count;							//
	int read_count;							//
	int read_no;							//
	int reg_time;							//
	int last_time;							//
	int login_count;						//	
}hdcctv_qpns_user_t;

typedef struct
{
	char dev_id[HDCCTV_QPNS_TOKEN_SIZE];
	char username[PTC_STR_SIZE];			//�û���
	char qpns_server_id[PTC_STR_SIZE];		
	char relay_server_id[PTC_STR_SIZE];		//�û���
	char msgs_server_id[PTC_STR_SIZE];			
	int nonce;
	char nonce_md5[PTC_STR_SIZE];			
	int bind_time;
	int last_time;
	int state;
}hdcctv_service_map_t;

#define HDCCTV_SERVICE_MAP_NUM			100000

typedef struct
{
	int num;
	hdcctv_service_map_t service_map[HDCCTV_SERVICE_MAP_NUM];
}hdcctv_service_maps_t;

typedef struct
{
	char ip[PTC_IP_SIZE];
	int port;
	int nonce;
	char nonce_md5[PTC_STR_SIZE];	
}hdcctv_service_addr_t;

typedef struct
{
	long long used_flow;				//��ʹ������(B)��ֻ�ɶ�
	long long total_flow;				//������(B),0=�����ƣ�ֻ�ɶ�
	long long used_space;				//��ʹ�ÿռ�(B)��ֻ�ɶ�
	long long total_space;				//�ܿռ�(B),0=�����ƣ�ֻ�ɶ�
	int record_enable;					//���������ƶ�¼��0=��ʹ�ܣ�1=ʹ��
}hdcctv_cloud_cfg_t;


#pragma pack()



#endif

#if defined __cplusplus
}
#endif

