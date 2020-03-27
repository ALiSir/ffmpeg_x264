
#if defined __cplusplus
extern "C"
{
#endif

#ifndef __PTC_HY_H__
#define __PTC_HY_H__

#define HY_MSG_SIZE			(1024)
#define HY_TALK_FRAME_SIZE	2048

#pragma pack(4)

typedef enum
{
	HY_CMD_RTS				= 0x11,
	HY_CMD_PTZ				= 0x12,
	HY_CMD_SET_AO			= 0x1B,

	HY_CMD_GET_AO_C2D		= 0x23,
	HY_CMD_PARAM_TIME		= 0x24,

	HY_CMD_ALARM			= 0x69,
	
	HY_CMD_HB_C2D			= 0xA1,
	HY_CMD_LOG_C2D			= 0xA2,
	HY_CMD_GET_C2D			= 0xA3,
	HY_CMD_GET_SYSTEM_C2D	= 0xA4,
	HY_CMD_GET_RECODR_C2D	= 0xA5,
	
	HY_CMD_LOGIN0_D2C		= 0xB0,
	HY_CMD_HB_D2C			= 0xB1,
	HY_CMD_LOG_D2C			= 0xB2,
	HY_CMD_GET_D2C			= 0xB3,
	HY_CMD_GET_SYSTEM_D2C	= 0xB4,
	HY_CMD_GET_RECORD_D2C	= 0xB6,
	HY_CMD_PBS_FRAME		= 0xBB,
	HY_CMD_RTS_FRAME		= 0xBC,

	HY_CMD_TALK				= 0xC0,
	HY_CMD_PBS_START		= 0xC2,
	HY_CMD_PBS_STOP			= 0xC9,
	HY_CMD_PBS_POS			= 0xCA,
	HY_CMD_PBS_PAUSE		= 0xCC,

	HY_CMD_LOGIN0_C2D		= 0xD2,

	HY_CMD_GET_AO_D2C		= 0xE3,

	HY_CMD_LOGIN1_C2D		= 0xF1,
	
	HY_CMD_ALL				= 0xFF,
}HY_CMD_E;

typedef struct
{
	unsigned char cmd;
	unsigned char reserve1[3];
	unsigned int  len;
	unsigned char reserve2[24];
}hy_msg_head_t;

typedef struct
{
	hy_msg_head_t head;
	unsigned char data[HY_MSG_SIZE];
}hy_msg_t;

typedef struct
{
	unsigned int  second :6;		//�� 0-59
	unsigned int  minute :6;		//�� 0-59
	unsigned int  hour :5;			//ʱ 0-23
	unsigned int  day :5;			//�� 1-31
	unsigned int  month :4; 		//�� 1-12
	unsigned int  year :6;			//�� 2000-2063		
}hy_time_t;

typedef struct
{
	unsigned char cmd;
	unsigned char reserve1[3];
	unsigned int  len;
	char user[8];
	char pswd[8];
	unsigned int  ver;
	unsigned int  flag;
}hy_msg_login0_c2d_t;

typedef struct
{
	unsigned char cmd;
	unsigned char reserve1[3];
	unsigned int  len;
	unsigned char fail;
	unsigned char fail_code;
	unsigned char ch_num;
	unsigned char reserve2[5];
	unsigned int  id;
	unsigned char reserve3[12];
}hy_msg_login0_d2c_t;

typedef struct
{
	unsigned char cmd;
	unsigned char reserve1[3];
	unsigned int  len;
	unsigned int  id;
	unsigned char type;			//1=rts, 2=pbs
	unsigned char ch_no;
	unsigned char reserve2[18];
}hy_msg_login1_c2d_t;

typedef struct
{
	unsigned char cmd;
	unsigned char reserve1[3];
	unsigned int  len;
	unsigned char enable[16];
	unsigned char reserve2[8];
	unsigned char stream[16];
}hy_msg_rts_t;

typedef struct
{
	unsigned int avHead;			//֡ͷ��־
	unsigned char avSliceType;		//֡����FD
	unsigned char avSubSliceType;	//��֡����0
	unsigned char avChnNum; 		//ͨ����
	unsigned char expandLength; 	//��չ����
	unsigned int avSequence;		//���к�
	unsigned int avSliceLength; 	//֡����= ֡ͷ+ ��������+ ֡β
//	unsigned int ClockTimes;		//ǽ��ʱ��s 
	hy_time_t ClockTimes;			//ǽ��ʱ��s 
	unsigned int msTimes;			//ʱ���ms, hy_time_t
}hy_0frame_head_t;

typedef struct 
{
	hy_0frame_head_t head0;
	
	unsigned char PicMask;			//ͼ���־λ
	unsigned char PicInterlace;		//�ӽ���
	unsigned char PicWidth;			//ͼ����8�����ص㵥λ
	unsigned char PicHeight;		//ͼ��߶�8�����ص㵥λ
	unsigned char VStreamMask;		//�������ͱ�־
	unsigned char VStreamRev;		//����
	unsigned char VStreamType;		//��������MPEG H264 HI_H264 ������
	unsigned char VFps;				//֡��
	unsigned char DataCheckMask;	//����У���־
	unsigned char DataCheckRest[4];	//��������У����
	unsigned char Rev1[2];			//����
	unsigned char DataCheckType;	//��������У������
}hy_iframe_head_t;

typedef struct 
{
	hy_0frame_head_t head0;
	
	unsigned char DataCheckMask;	//��������У���־
	unsigned char DataCheckRest[4];	//��������У����
	unsigned char Rev1[2];			//����
	unsigned char DataCheckType;	//��������У������
}hy_pframe_head_t;

typedef struct 
{
	hy_0frame_head_t head0;
	
	unsigned char AFomatMask;
	unsigned char AenSoundmode;
	unsigned char AenPayloadType;
	unsigned char AenSamplerate;
}hy_aframe_head_t;

typedef struct
{
	hy_0frame_head_t head0;
	unsigned char AFomatMask;
	unsigned char AenSoundmode;
	unsigned char AenPayloadType;
	unsigned char AenSamplerate;
	unsigned char reserve[8];
}hy_tframe_head_t;

typedef struct 
{
	unsigned int avTail;
	unsigned int avSliceLength; 	
}hy_frame_tail_t;

typedef struct
{
	unsigned char cmd;
	unsigned char reserve1[3];
	unsigned int  len;
	unsigned char reserve2[24];
}hy_msg_search_c2d_t;

typedef struct
{
	unsigned char cmd;
	unsigned char reserve1[3];
	unsigned int  len;
	unsigned char reserve2[24];
	unsigned char reserve3[24];
	unsigned char ip[4];
	unsigned char mask[4];
	unsigned char gate[4];
	unsigned char reserve4[24];
	unsigned short port1;
	unsigned short web_port;
	unsigned short port2;
	unsigned short dev_port;
	unsigned char reserve5[28];
	unsigned char mac[20];
	
}hy_msg_search_d2c_t;

typedef struct
{
	unsigned char cmd;			//0xA5
	unsigned char reserve1[3];
	
	unsigned int  len;
	
	unsigned int channel_no:8;	//ͨ���ţ���1��ʼ
	unsigned int year:16;		//��ʼʱ��
	unsigned int month:8;

	unsigned char day;
	unsigned char hour;
	unsigned char minute;
	unsigned char second;
	
	unsigned char type; 		//¼���ļ�����  0����ͨ¼��1������¼��2���ƶ���⣻3������¼��4��ͼƬ
	unsigned char reserve2[15];
}hy_msg_record_c2d_t;

typedef struct
{
	unsigned char channel_no:4;	
	unsigned char reserve1:4;
	unsigned char reserve2[3];
	hy_time_t start_time;
	hy_time_t end_time;
	unsigned int size;		//KB
	unsigned char start_pos[4];
	unsigned char reserve3[4];
}hy_msg_record_t;

#define HY_MSG_RECORD_NUM		16
#define HY_MSG_LOG_NUM			61

typedef struct
{
	unsigned char cmd;			//0xB6
	unsigned char reserve1[3];
	unsigned int  len;
	unsigned char reserve2[8];
	
	unsigned char type;				//¼���ļ�����  0����ͨ¼��1������¼��2���ƶ���⣻3������¼��4��ͼƬ
	unsigned char reserve3[15];

	hy_msg_record_t record[HY_MSG_RECORD_NUM];
}hy_msg_record_d2c_t;

typedef struct
{
	unsigned char cmd;
	unsigned char reserve1[3];
	unsigned int  len;
	unsigned int channel_no:8;		//ͨ���ţ���1��ʼ
	unsigned int year:16;			//��ʼʱ��
	unsigned int month:8;

	unsigned char day;
	unsigned char hour;
	unsigned char minute;
	unsigned char second;

	unsigned char disk_id;
	unsigned char start_pos[4];
	unsigned char reserve2[11];
}hy_msg_pbs_start_t;

typedef struct
{
	unsigned char cmd;				//HY_CMD_PBS_POS
	unsigned char reserve1[3];
	unsigned int  len;
	unsigned char channel_no;		//begin with 1
	unsigned char reserve2[23];		//
}hy_msg_pbs_stop_t;

typedef struct
{
	unsigned char cmd;				//HY_CMD_PBS_PAUSE
	unsigned char sub_cmb;			//0=pause,1=restore
	unsigned char reserve1[2];
	unsigned int len;
	unsigned char channel_no;		//begin with 1
	unsigned char reserve2[23];
}hy_msg_pbs_pause_t;

typedef struct
{
	unsigned char cmd;				//HY_CMD_PTZ
	unsigned char reserve1[3];
	unsigned int len;
	unsigned char protocol;
	unsigned char channel_no;
	unsigned char type;				//0x00=up, 0x01=down, 0x02=left, 0x03=right,
									//0x10=goPreset, 0x11=addPreset, 0x12=delPreset
									//0x20=left-up, 0x21=right-up, 0x22=left-down, 0x23=right-down
									//0x04-0x05=zoom, 0x07-0x08=focus, 0x09-0x0a=iris
	unsigned char step;				//N=N*50ms
	unsigned char speed;			//
	unsigned char reserve2[19];		//
}hy_msg_ptz_t;

typedef struct
{
	unsigned char cmd;				//HY_CMD_LOG_C2D
	unsigned char reserve1[3];
	unsigned int len;
	unsigned char reserve2[24];
}hy_msg_log_c2d_t;

/*
typedef enum 
{
	HY_LOG_TYPE_ALL = 0,							// ������־
	HY_LOG_TYPE_SYSTEM,								// ϵͳ��־
	HY_LOG_TYPE_CONFIG,								// ������־
	HY_LOG_TYPE_STORAGE,							// �洢���
	HY_LOG_TYPE_ALARM,								// ������־
	HY_LOG_TYPE_RECORD,								// ¼�����
	HY_LOG_TYPE_ACCOUNT,							// �ʺ����
	HY_LOG_TYPE_CLEAR,								// �����־
	HY_LOG_TYPE_PLAYBACK							// �ط����
}HY_LOG_TYPE_E;

*/

typedef enum 
{
    HY_CFG_INDEX_GENERAL = 0,						        // ��ͨ
	HY_CFG_INDEX_COMM,									// ����
	HY_CFG_INDEX_NET,									    // ����
	HY_CFG_INDEX_RECORD,									// ¼��
	HY_CFG_INDEX_CAPTURE,								    // ͼ������
	HY_CFG_INDEX_PTZ,								    	// ��̨
	HY_CFG_INDEX_DETECT,									// ��̬���
	HY_CFG_INDEX_ALARM,									// ����
	HY_CFG_INDEX_DISPLAY,								    // ��ʾ
	HY_CFG_INDEX_RESERVED,								// ������ʹ��������
	HY_CFG_INDEX_TITLE = 10,								// ͨ������
	HY_CFG_INDEX_MAIL = 11,								// �ʼ�����
	HY_CFG_INDEX_EXCAPTURE = 12,							// Ԥ��ͼ������
	HY_CFG_INDEX_PPPOE = 13,								// pppoe����
	HY_CFG_INDEX_DDNS = 14,								// DDNS����
	HY_CFG_INDEX_SNIFFER	= 15,							// ������Ӳ�������
	HY_CFG_INDEX_DSPINFO	= 16,							// ����������Ϣ
	HY_CFG_INDEX_COLOR = 126,							    // ��ɫ������Ϣ
	HY_CFG_INDEX_ALL,									    // ����
} HYNET_CFG_INDEX_E;


typedef enum 
{
	HY_LOG_TYPE_REBOOT = 0x0000,				// �豸����
	HY_LOG_TYPE_SHUT,							// �豸�ػ�
	HY_LOG_TYPE_UPGRADE = 0x0004,				// �豸����
	HY_LOG_TYPE_CONFSAVE = 0x0100,				// ��������
	HY_LOG_TYPE_CONFLOAD,						// ��ȡ����
	HY_LOG_TYPE_FSERROR = 0x0200,				// �ļ�ϵͳ����
	HY_LOG_TYPE_HDD_WERR,						// Ӳ��д����
	HY_LOG_TYPE_HDD_RERR,						// Ӳ�̶�����
	HY_LOG_TYPE_HDD_TYPE,						// ����Ӳ������
	HY_LOG_TYPE_HDD_FORMAT,						// ��ʽ��Ӳ��
	HY_LOG_TYPE_HDD_NOSPACE,					// ��ǰ�����̿ռ䲻��
	HY_LOG_TYPE_HDD_TYPE_RW,					// ����Ӳ������Ϊ��д��
	HY_LOG_TYPE_HDD_TYPE_RO,					// ����Ӳ������Ϊֻ����	
	HY_LOG_TYPE_HDD_TYPE_RE,					// ����Ӳ������Ϊ������
	HY_LOG_TYPE_HDD_TYPE_SS,					// ����Ӳ������Ϊ������
	HY_LOG_TYPE_HDD_NONE,						// ��Ӳ�̼�¼��־
	HY_LOG_TYPE_HDD_NOWORKHDD,					// �޹�����(û�ж�д��)
	HY_LOG_TYPE_HDD_TYPE_BK,					// ����Ӳ������Ϊ������
	HY_LOG_TYPE_HDD_TYPE_REVERSE,				// ����Ӳ������Ϊ��������
	HY_LOG_TYPE_ALM_IN = 0x0300,				// �ⲿ���뱨����ʼ
	HY_LOG_TYPE_NETALM_IN,						// ���籨��
	HY_LOG_TYPE_ALM_END = 0x0302,				// �ⲿ���뱨��ֹͣ
	HY_LOG_TYPE_LOSS_IN,						// ��Ƶ��ʧ������ʼ
	HY_LOG_TYPE_LOSS_END,						// ��Ƶ��ʧ��������
	HY_LOG_TYPE_MOTION_IN,						// ��̬��ⱨ����ʼ
	HY_LOG_TYPE_MOTION_END,						// ��̬��ⱨ������
	HY_LOG_TYPE_ALM_BOSHI,						// ��������������
	HY_LOG_TYPE_NET_ABORT = 0x0308,				// ����Ͽ�
	HY_LOG_TYPE_NET_ABORT_RESUME,				// ����ָ�
	HY_LOG_TYPE_CODER_BREAKDOWN,				// ����������
	HY_LOG_TYPE_CODER_BREAKDOWN_RESUME,			// ���������ϻָ�
	HY_LOG_TYPE_BLIND_IN,						// ��Ƶ�ڵ�
	HY_LOG_TYPE_BLIND_END,						// ��Ƶ�ڵ��ָ�
	HY_LOG_TYPE_ALM_TEMP_HIGH,					// �¶ȹ���
	HY_LOG_TYPE_ALM_VOLTAGE_LOW,				// ��ѹ����
	HY_LOG_TYPE_ALM_BATTERY_LOW,				// �����������
	HY_LOG_TYPE_ALM_ACC_BREAK,					// ACC�ϵ�
	HY_LOG_TYPE_INFRAREDALM_IN = 0x03a0,		// ���߱�����ʼ
	HY_LOG_TYPE_INFRAREDALM_END,				// ���߱�������
	HY_LOG_TYPE_IPCONFLICT,						// IP��ͻ
	HY_LOG_TYPE_IPCONFLICT_RESUME,				// IP�ָ�
	HY_LOG_TYPE_SDPLUG_IN,						// SD������
	HY_LOG_TYPE_SDPLUG_OUT,						// SD���γ�
	HY_LOG_TYPE_NET_PORT_BIND_FAILED,			// ����˿ڰ�ʧ��
	HY_LOG_TYPE_AUTOMATIC_RECORD = 0x0400,		// �Զ�¼��
	HY_LOG_TYPE_MANUAL_RECORD,					// �ֶ�¼��
	HY_LOG_TYPE_CLOSED_RECORD,					// ֹͣ¼��
	HY_LOG_TYPE_LOGIN = 0x0500,					// ��¼
	HY_LOG_TYPE_LOGOUT,							// ע��
	HY_LOG_TYPE_ADD_USER,						// ����û�
	HY_LOG_TYPE_DELETE_USER,					// ɾ���û�
	HY_LOG_TYPE_MODIFY_USER,					// �޸��û�
	HY_LOG_TYPE_ADD_GROUP,						// ����û���
	HY_LOG_TYPE_DELETE_GROUP,					// ɾ���û���
	HY_LOG_TYPE_MODIFY_GROUP,					// �޸��û���
	HY_LOG_TYPE_NET_LOGIN = 0x0508,				// �����û���¼
	HY_LOG_TYPE_CLEAR = 0x0600,					// �����־
	HY_LOG_TYPE_SEARCHLOG,						// ��ѯ��־
	HY_LOG_TYPE_SEARCH = 0x0700,				// ¼���ѯ
	HY_LOG_TYPE_DOWNLOAD,						// ¼������
	HY_LOG_TYPE_PLAYBACK,						// ¼��ط�
	HY_LOG_TYPE_BACKUP,							// ����¼���ļ�
	HY_LOG_TYPE_BACKUPERROR,					// ����¼���ļ�ʧ��
	
	HY_LOG_TYPE_NR = 8,		
} HY_LOG_TYPE_E;

typedef struct
{
	hy_time_t 		time;
    unsigned short	type;					// ���� HY_LOG_TYPE_E
    unsigned char	reserved;				// ����
    unsigned char	data;					// ����  HYNET_CFG_INDEX_E
    unsigned char	context[8];				// ����
}hy_msg_log_t;

typedef struct
{
	unsigned char cmd;				//HY_CMD_LOG_D2C
	unsigned char reserve1[3];
	unsigned int len;
	unsigned char reserve2[24];	
	hy_msg_log_t log[HY_MSG_LOG_NUM];
}hy_msg_log_d2c_t;

typedef struct
{
	unsigned char cmd;				//HY_CMD_TALK
	unsigned char reserve1[3];
	unsigned int len;
	unsigned char reserve2[24];		
}hy_msg_talk_t;

typedef struct
{
	unsigned char cmd;				//HY_CMD_PARAM_TIME
	unsigned char reserve1[3];
	unsigned int len;
	unsigned char reserve2[24];			
}hy_msg_param_time_t;

typedef struct
{
	unsigned char cmd;				//HY_CMD_ALARM
	unsigned char reserve1[3];	
	unsigned int len;
	unsigned char reserve2[24];	
	unsigned char data[4];
}hy_msg_alarm_t;

typedef struct
{
	unsigned char cmd;				//HY_CMD_GET_AO_C2D, HY_CMD_GET_AO_D2C, HY_CMD_SET_AO
	unsigned char reserve1[3];
	unsigned int len;
	unsigned char reserve2[4];		
	unsigned char out[20];			// 0=low, 1=high, 2=none
}hy_msg_ao_t;

typedef struct
{
	unsigned char cmd;				//HY_CMD_GET_SYSTEM_C2D
	unsigned char reserve1[3];
	unsigned int len;
	unsigned char type;				//1=channel_num, 8=device_type
	unsigned char reserve2[23];		
}hy_msg_param_ability_c2d_t;

typedef struct
{
	unsigned char cmd;				//HY_CMD_GET_SYSTEM_D2C
	unsigned char reserve1[3];
	unsigned int len;
	unsigned char type;				//1=channel_num
	unsigned char reserve2[23];		
	unsigned char unknown1;
	unsigned char unknown2;
	unsigned char video_in_num;
	unsigned char unknown4;
	unsigned char unknown5;
	unsigned char unknown6;
	unsigned char alarm_in_num;
	unsigned char alarm_out_num;
	unsigned char unknown9;
	unsigned char unknown10;
	unsigned char unknown11;
	unsigned char unknown12;
	unsigned short build_year;
	unsigned char build_month;
	unsigned char build_day;
	unsigned char reserve3[16];	
}hy_msg_param_ability1_d2c_t;

typedef struct
{
	unsigned char cmd;				//HY_CMD_GET_SYSTEM_D2C
	unsigned char reserve1[3];
	unsigned int len;
	unsigned char type;				//8=device_type
	unsigned char reserve2[23];		
	char data[32];	
}hy_msg_param_ability8_d2c_t;


#pragma pack()


#endif

#if defined __cplusplus
}
#endif

