
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
	XM_RET_ERROR_NO_RIGHT	= 107, //没有权限
	XM_RET_ERROR_NO_RECORD	= 119, //没有找到录像文件
	
	XM_RET_ERROR_PSWD		= 203, //密码错误
	XM_RET_ERROR_USER		= 205, //用户名不存在

	XM_RET_ERROR_TALK_BUSY	= 503, //对讲已经打开
}XM_RET_E;

typedef enum
{
	XM_CMD_LOGIN_REQ		= 1000, //登陆请求
	XM_CMD_LOGIN_RSP		= 1001, //登陆响应

	XM_CMD_HEARTBEAT_REQ	= 1006, //心跳请求
	XM_CMD_HEARTBEAT_RSP	= 1007, //心跳响应


	XM_CMD_SYSINFO_REQ		= 1020, //查询设备信息请求
	XM_CMD_SYSINFO_RSP		= 1021, //查询设备信息响应

	XM_CMD_SET_CFG_REQ		= 1040, //设置参数请求
	XM_CMD_SET_CFG_RSP		= 1041, //设置参数响应
	XM_CMD_GET_CFG_REQ		= 1042, //查询参数请求
	XM_CMD_GET_CFG_RSP		= 1043, //查询参数响应	

	XM_CMD_PTZ_REQ			= 1400, //云台控制请求
	XM_CMD_PTZ_RSP			= 1401, //云台控制响应

	XM_CMD_CTRL_RTS_REQ		= 1410, //控制实时流请求
	XM_CMD_CTRL_RTS_RSP		= 1411, //控制实时流响应
	XM_CMD_FRAME_RTS		= 1412, //实时流数据
	XM_CMD_LOGIN_RTS_REQ	= 1413, //登陆实时流请求
	XM_CMD_LOGIN_RTS_RSP	= 1414, //登陆实时流响应

	XM_CMD_CTRL_PBS_REQ		= 1420, //控制回放流请求
	XM_CMD_CTRL_PBS_RSP		= 1421, //控制回放流响应
	XM_CMD_FRAME_PBS		= 1422, //回放流数据
	XM_CMD_EOF_PBS			= 1423, //回放流完毕
	XM_CMD_LOGIN_PBS_REQ	= 1424, //登陆回放流请求
	XM_CMD_LOGIN_PBS_RSP	= 1425, //登陆回放流响应

	XM_CMD_CTRL_TALK_REQ	= 1430, //控制对讲流请求
	XM_CMD_CTRL_TALK_RSP	= 1431, //控制对讲流响应
	XM_CMD_FRAME_TALK_C2D	= 1432, //对讲流流数据，客户端=>设备端
	XM_CMD_FRAME_TALK_D2C	= 1433, //对讲流流完毕，设备端=>客户端
	XM_CMD_LOGIN_TALK_REQ	= 1434, //登陆对讲流请求
	XM_CMD_LOGIN_TALK_RSP	= 1435, //登陆对讲流响应

	XM_CMD_SEARCH_REC_REQ	= 1440, //查询录像文件请求
	XM_CMD_SEARCH_REC_RSP	= 1441, //查询录像文件响应
	XM_CMD_SEARCH_LOG_REQ	= 1442, //查询日志请求
	XM_CMD_SEARCH_LOG_RSP	= 1443, //查询日志响应


	XM_CMD_MANAGE_SYS_REQ	= 1450, //系统管理请求
	XM_CMD_MANAGE_SYS_RSP	= 1451, //系统管理响应
	
	XM_CMD_SEARCH_DEV_REQ	= 1530,	//搜索设备请求
	XM_CMD_SEARCH_DEV_RSP	= 1531,	//搜索设备响应


	XM_CMD_ALL				= 9999,
}XM_CMD_E;

typedef struct
{
	unsigned char head_flag;		//0xFF
	unsigned char version;			//1
	unsigned char reserve1;			
	unsigned char reserve2;
	unsigned int session_id;		//会话-ID
	unsigned int sequence_number;	//包序号
	unsigned char total_packet;		//消息帧=分包总数，视频帧=通道号
	unsigned char cur_packet;		//消息帧=当前分包号，视频帧=结束标志，录像回放和下载最后一针
	unsigned short command_id;		//消息码 XM_CMD_E
	unsigned int data_len;			//数据长度
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


#define	XM_FRAME_TYPE_VI	0xFC010000	//视频I帧
#define	XM_FRAME_TYPE_VP	0xFD010000	//视频P帧
#define	XM_FRAME_TYPE_A		0xFA010000	//音频帧
#define XM_FRAME_TYPE_PIC	0xFE010000	//图片帧
#define XM_FRAME_TYPE_INFO	0xF9010000	//信息帧

typedef struct
{
	unsigned int type;				//主类型: 视频I帧XM_FRAME_TYPE_VI
	unsigned char sub_type:4;		//子类型，视频帧: 0x1=MPG4,0x2=H264 
									//        信息帧: 0x0=车载信息 
									//        音频帧: 0xe=G711A
									//        图片帧: 0x0=JPG
	unsigned char width_h:2;		//分辨率宽度高位
	unsigned char height_h:2;		//分辨率高度高位
	
	unsigned char frame_rate:5;		//帧率
	unsigned char reserved:3;		//

	unsigned char width_l;			//分辨率宽度低位
	unsigned char height_l;			//分辨率高度低位
	
	unsigned int second:6;			//秒	1-60		
	unsigned int minute:6;			//分	1-60		
	unsigned int hour:5;			//时	1-24		
	unsigned int day:5;				//日	1-31		
	unsigned int month:4;			//月	1-12		
	unsigned int year:6;			//年	2000为基准，0-63	

	unsigned int length;			//帧数据长度
}xm_iframe_head_t, xm_picframe_head_t;

typedef struct
{
	unsigned int type;				//主类型: 视频P帧XM_FRAME_TYPE_VP
	unsigned int length;			//帧数据长度
}xm_pframe_head_t;

typedef struct
{
	unsigned int type;				//主类型: 视频P帧XM_FRAME_TYPE_A
	unsigned char sub_type:4;		//子类型，视频帧: 0x1=MPG4,0x2=H264 
									//        信息帧: 0x0=车载信息 
									//        音频帧: 0xe=G711A, 0xa=G711U
									//        图片帧: 0x0=JPG
	unsigned char reserve:4;		
	unsigned char sample;			//采样率，0x02=8K
	unsigned short length;			//帧数据长度
}xm_aframe_head_t, xm_infoframe_head_t;


#pragma pack()


#endif

#if defined __cplusplus
}
#endif

