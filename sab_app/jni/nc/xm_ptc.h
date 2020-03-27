
#if defined __cplusplus
extern "C"
{
#endif

#ifndef __PTC_XM_H__
#define __PTC_XM_H__

#define XM_MSG_SIZE				(4*1024)
#define XM_SEARCH_SIZE			(100*1024)
#define XM_PACK_SIZE			(10*1024)
#define XM_HEAD_FLAG			0xFF
#define XM_BCAST_IP				"255.255.255.255"
#define XM_BCAST_PORT			34569



typedef enum
{
	XM_RET_OK				= 100, //OK
	XM_RET_ERROR_NO_RIGHT	= 107, //û��Ȩ��
	XM_RET_ERROR_NO_RECORD	= 119, //û���ҵ�¼���ļ�
	
	XM_RET_ERROR_PSWD		= 203, //�������
	XM_RET_ERROR_USER		= 205, //�û���������

	XM_RET_ERROR_TALK_BUSY	= 503, //�Խ��Ѿ���
}XM_RET_E;

typedef enum
{
	XM_CMD_LOGIN_REQ		= 1000, //��½����
	XM_CMD_LOGIN_RSP		= 1001, //��½��Ӧ

	XM_CMD_HEARTBEAT_REQ	= 1006, //��������
	XM_CMD_HEARTBEAT_RSP	= 1007, //������Ӧ


	XM_CMD_SYSINFO_REQ		= 1020, //��ѯ�豸��Ϣ����
	XM_CMD_SYSINFO_RSP		= 1021, //��ѯ�豸��Ϣ��Ӧ

	XM_CMD_SET_CFG_REQ		= 1040, //���ò�������
	XM_CMD_SET_CFG_RSP		= 1041, //���ò�����Ӧ
	XM_CMD_GET_CFG_REQ		= 1042, //��ѯ��������
	XM_CMD_GET_CFG_RSP		= 1043, //��ѯ������Ӧ	

	XM_CMD_PTZ_REQ			= 1400, //��̨��������
	XM_CMD_PTZ_RSP			= 1401, //��̨������Ӧ

	XM_CMD_CTRL_RTS_REQ		= 1410, //����ʵʱ������
	XM_CMD_CTRL_RTS_RSP		= 1411, //����ʵʱ����Ӧ
	XM_CMD_FRAME_RTS		= 1412, //ʵʱ������
	XM_CMD_LOGIN_RTS_REQ	= 1413, //��½ʵʱ������
	XM_CMD_LOGIN_RTS_RSP	= 1414, //��½ʵʱ����Ӧ

	XM_CMD_CTRL_PBS_REQ		= 1420, //���ƻط�������
	XM_CMD_CTRL_PBS_RSP		= 1421, //���ƻط�����Ӧ
	XM_CMD_FRAME_PBS		= 1422, //�ط�������
	XM_CMD_EOF_PBS			= 1423, //�ط������
	XM_CMD_LOGIN_PBS_REQ	= 1424, //��½�ط�������
	XM_CMD_LOGIN_PBS_RSP	= 1425, //��½�ط�����Ӧ

	XM_CMD_CTRL_TALK_REQ	= 1430, //���ƶԽ�������
	XM_CMD_CTRL_TALK_RSP	= 1431, //���ƶԽ�����Ӧ
	XM_CMD_FRAME_TALK_C2D	= 1432, //�Խ��������ݣ��ͻ���=>�豸��
	XM_CMD_FRAME_TALK_D2C	= 1433, //�Խ�������ϣ��豸��=>�ͻ���
	XM_CMD_LOGIN_TALK_REQ	= 1434, //��½�Խ�������
	XM_CMD_LOGIN_TALK_RSP	= 1435, //��½�Խ�����Ӧ

	XM_CMD_SEARCH_REC_REQ	= 1440, //��ѯ¼���ļ�����
	XM_CMD_SEARCH_REC_RSP	= 1441, //��ѯ¼���ļ���Ӧ
	XM_CMD_SEARCH_LOG_REQ	= 1442, //��ѯ��־����
	XM_CMD_SEARCH_LOG_RSP	= 1443, //��ѯ��־��Ӧ


	XM_CMD_MANAGE_SYS_REQ	= 1450, //ϵͳ��������
	XM_CMD_MANAGE_SYS_RSP	= 1451, //ϵͳ������Ӧ
	
	XM_CMD_SEARCH_DEV_REQ	= 1530,	//�����豸����
	XM_CMD_SEARCH_DEV_RSP	= 1531,	//�����豸��Ӧ


	XM_CMD_ALL				= 9999,
}XM_CMD_E;

typedef struct
{
	unsigned char head_flag;		//0xFF
	unsigned char version;			//1
	unsigned char reserve1;			
	unsigned char reserve2;
	unsigned int session_id;		//�Ự-ID
	unsigned int sequence_number;	//�����
	unsigned char total_packet;		//��Ϣ֡=�ְ���������Ƶ֡=ͨ����
	unsigned char cur_packet;		//��Ϣ֡=��ǰ�ְ��ţ���Ƶ֡=������־��¼��طź��������һ��
	unsigned short command_id;		//��Ϣ�� XM_CMD_E
	unsigned int data_len;			//���ݳ���
}xm_msg_head_t;

typedef struct
{
	xm_msg_head_t head;
	unsigned char data[XM_MSG_SIZE];
}xm_msg_t;

typedef struct
{
	xm_msg_head_t head;
	unsigned char data[XM_SEARCH_SIZE];
}xm_search_t;


#define	XM_FRAME_TYPE_VI	0xFC010000	//��ƵI֡
#define	XM_FRAME_TYPE_VP	0xFD010000	//��ƵP֡
#define	XM_FRAME_TYPE_A		0xFA010000	//��Ƶ֡
#define XM_FRAME_TYPE_PIC	0xFE010000	//ͼƬ֡
#define XM_FRAME_TYPE_INFO	0xF9010000	//��Ϣ֡

typedef struct
{
	unsigned int type;				//������: ��ƵI֡XM_FRAME_TYPE_VI
	unsigned char sub_type:4;		//�����ͣ���Ƶ֡: 0x1=MPG4,0x2=H264 
									//        ��Ϣ֡: 0x0=������Ϣ 
									//        ��Ƶ֡: 0xe=G711A
									//        ͼƬ֡: 0x0=JPG
	unsigned char width_h:2;		//�ֱ��ʿ�ȸ�λ
	unsigned char height_h:2;		//�ֱ��ʸ߶ȸ�λ
	
	unsigned char frame_rate:5;		//֡��
	unsigned char reserved:3;		//

	unsigned char width_l;			//�ֱ��ʿ�ȵ�λ
	unsigned char height_l;			//�ֱ��ʸ߶ȵ�λ
	
	unsigned int second:6;			//��	1-60		
	unsigned int minute:6;			//��	1-60		
	unsigned int hour:5;			//ʱ	1-24		
	unsigned int day:5;				//��	1-31		
	unsigned int month:4;			//��	1-12		
	unsigned int year:6;			//��	2000Ϊ��׼��0-63	

	unsigned int length;			//֡���ݳ���
}xm_iframe_head_t, xm_picframe_head_t;

typedef struct
{
	unsigned int type;				//������: ��ƵP֡XM_FRAME_TYPE_VP
	unsigned int length;			//֡���ݳ���
}xm_pframe_head_t;

typedef struct
{
	unsigned int type;				//������: ��ƵP֡XM_FRAME_TYPE_A
	unsigned char sub_type:4;		//�����ͣ���Ƶ֡: 0x1=MPG4,0x2=H264 
									//        ��Ϣ֡: 0x0=������Ϣ 
									//        ��Ƶ֡: 0xe=G711A, 0xa=G711U
									//        ͼƬ֡: 0x0=JPG
	unsigned char reserve:4;		
	unsigned char sample;			//�����ʣ�0x02=8K
	unsigned short length;			//֡���ݳ���
}xm_aframe_head_t, xm_infoframe_head_t;


#pragma pack()


#endif

#if defined __cplusplus
}
#endif

