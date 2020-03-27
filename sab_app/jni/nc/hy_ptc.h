
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
	unsigned int  second :6;		//秒 0-59
	unsigned int  minute :6;		//分 0-59
	unsigned int  hour :5;			//时 0-23
	unsigned int  day :5;			//日 1-31
	unsigned int  month :4; 		//月 1-12
	unsigned int  year :6;			//年 2000-2063		
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
	unsigned int avHead;			//帧头标志
	unsigned char avSliceType;		//帧类型FD
	unsigned char avSubSliceType;	//子帧类型0
	unsigned char avChnNum; 		//通道号
	unsigned char expandLength; 	//扩展长度
	unsigned int avSequence;		//序列号
	unsigned int avSliceLength; 	//帧长度= 帧头+ 码流数据+ 帧尾
//	unsigned int ClockTimes;		//墙上时钟s 
	hy_time_t ClockTimes;			//墙上时钟s 
	unsigned int msTimes;			//时间戳ms, hy_time_t
}hy_0frame_head_t;

typedef struct 
{
	hy_0frame_head_t head0;
	
	unsigned char PicMask;			//图像标志位
	unsigned char PicInterlace;		//接交错
	unsigned char PicWidth;			//图像宽度8个像素点单位
	unsigned char PicHeight;		//图像高度8个像素点单位
	unsigned char VStreamMask;		//码流类型标志
	unsigned char VStreamRev;		//保留
	unsigned char VStreamType;		//码流类型MPEG H264 HI_H264 等类型
	unsigned char VFps;				//帧率
	unsigned char DataCheckMask;	//数据校验标志
	unsigned char DataCheckRest[4];	//码流数据校验结果
	unsigned char Rev1[2];			//保留
	unsigned char DataCheckType;	//码流数据校验类型
}hy_iframe_head_t;

typedef struct 
{
	hy_0frame_head_t head0;
	
	unsigned char DataCheckMask;	//码流数据校验标志
	unsigned char DataCheckRest[4];	//码流数据校验结果
	unsigned char Rev1[2];			//保留
	unsigned char DataCheckType;	//码流数据校验类型
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
	
	unsigned int channel_no:8;	//通道号，从1开始
	unsigned int year:16;		//开始时间
	unsigned int month:8;

	unsigned char day;
	unsigned char hour;
	unsigned char minute;
	unsigned char second;
	
	unsigned char type; 		//录象文件类型  0：普通录象；1：报警录象；2：移动检测；3：卡号录象；4：图片
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
	
	unsigned char type;				//录象文件类型  0：普通录象；1：报警录象；2：移动检测；3：卡号录象；4：图片
	unsigned char reserve3[15];

	hy_msg_record_t record[HY_MSG_RECORD_NUM];
}hy_msg_record_d2c_t;

typedef struct
{
	unsigned char cmd;
	unsigned char reserve1[3];
	unsigned int  len;
	unsigned int channel_no:8;		//通道号，从1开始
	unsigned int year:16;			//开始时间
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
	HY_LOG_TYPE_ALL = 0,							// 所有日志
	HY_LOG_TYPE_SYSTEM,								// 系统日志
	HY_LOG_TYPE_CONFIG,								// 配置日志
	HY_LOG_TYPE_STORAGE,							// 存储相关
	HY_LOG_TYPE_ALARM,								// 报警日志
	HY_LOG_TYPE_RECORD,								// 录象相关
	HY_LOG_TYPE_ACCOUNT,							// 帐号相关
	HY_LOG_TYPE_CLEAR,								// 清除日志
	HY_LOG_TYPE_PLAYBACK							// 回放相关
}HY_LOG_TYPE_E;

*/

typedef enum 
{
    HY_CFG_INDEX_GENERAL = 0,						        // 普通
	HY_CFG_INDEX_COMM,									// 串口
	HY_CFG_INDEX_NET,									    // 网络
	HY_CFG_INDEX_RECORD,									// 录像
	HY_CFG_INDEX_CAPTURE,								    // 图像设置
	HY_CFG_INDEX_PTZ,								    	// 云台
	HY_CFG_INDEX_DETECT,									// 动态检测
	HY_CFG_INDEX_ALARM,									// 报警
	HY_CFG_INDEX_DISPLAY,								    // 显示
	HY_CFG_INDEX_RESERVED,								// 保留，使类型连贯
	HY_CFG_INDEX_TITLE = 10,								// 通道标题
	HY_CFG_INDEX_MAIL = 11,								// 邮件功能
	HY_CFG_INDEX_EXCAPTURE = 12,							// 预览图像设置
	HY_CFG_INDEX_PPPOE = 13,								// pppoe设置
	HY_CFG_INDEX_DDNS = 14,								// DDNS设置
	HY_CFG_INDEX_SNIFFER	= 15,							// 网络监视捕获设置
	HY_CFG_INDEX_DSPINFO	= 16,							// 编码能力信息
	HY_CFG_INDEX_COLOR = 126,							    // 颜色配置信息
	HY_CFG_INDEX_ALL,									    // 保留
} HYNET_CFG_INDEX_E;


typedef enum 
{
	HY_LOG_TYPE_REBOOT = 0x0000,				// 设备重启
	HY_LOG_TYPE_SHUT,							// 设备关机
	HY_LOG_TYPE_UPGRADE = 0x0004,				// 设备升级
	HY_LOG_TYPE_CONFSAVE = 0x0100,				// 保存配置
	HY_LOG_TYPE_CONFLOAD,						// 读取配置
	HY_LOG_TYPE_FSERROR = 0x0200,				// 文件系统错误
	HY_LOG_TYPE_HDD_WERR,						// 硬盘写错误
	HY_LOG_TYPE_HDD_RERR,						// 硬盘读错误
	HY_LOG_TYPE_HDD_TYPE,						// 设置硬盘类型
	HY_LOG_TYPE_HDD_FORMAT,						// 格式化硬盘
	HY_LOG_TYPE_HDD_NOSPACE,					// 当前工作盘空间不足
	HY_LOG_TYPE_HDD_TYPE_RW,					// 设置硬盘类型为读写盘
	HY_LOG_TYPE_HDD_TYPE_RO,					// 设置硬盘类型为只读盘	
	HY_LOG_TYPE_HDD_TYPE_RE,					// 设置硬盘类型为冗余盘
	HY_LOG_TYPE_HDD_TYPE_SS,					// 设置硬盘类型为快照盘
	HY_LOG_TYPE_HDD_NONE,						// 无硬盘记录日志
	HY_LOG_TYPE_HDD_NOWORKHDD,					// 无工作盘(没有读写盘)
	HY_LOG_TYPE_HDD_TYPE_BK,					// 设置硬盘类型为备份盘
	HY_LOG_TYPE_HDD_TYPE_REVERSE,				// 设置硬盘类型为保留分区
	HY_LOG_TYPE_ALM_IN = 0x0300,				// 外部输入报警开始
	HY_LOG_TYPE_NETALM_IN,						// 网络报警
	HY_LOG_TYPE_ALM_END = 0x0302,				// 外部输入报警停止
	HY_LOG_TYPE_LOSS_IN,						// 视频丢失报警开始
	HY_LOG_TYPE_LOSS_END,						// 视频丢失报警结束
	HY_LOG_TYPE_MOTION_IN,						// 动态检测报警开始
	HY_LOG_TYPE_MOTION_END,						// 动态检测报警结束
	HY_LOG_TYPE_ALM_BOSHI,						// 报警器报警输入
	HY_LOG_TYPE_NET_ABORT = 0x0308,				// 网络断开
	HY_LOG_TYPE_NET_ABORT_RESUME,				// 网络恢复
	HY_LOG_TYPE_CODER_BREAKDOWN,				// 编码器故障
	HY_LOG_TYPE_CODER_BREAKDOWN_RESUME,			// 编码器故障恢复
	HY_LOG_TYPE_BLIND_IN,						// 视频遮挡
	HY_LOG_TYPE_BLIND_END,						// 视频遮挡恢复
	HY_LOG_TYPE_ALM_TEMP_HIGH,					// 温度过高
	HY_LOG_TYPE_ALM_VOLTAGE_LOW,				// 电压过低
	HY_LOG_TYPE_ALM_BATTERY_LOW,				// 电池容量不足
	HY_LOG_TYPE_ALM_ACC_BREAK,					// ACC断电
	HY_LOG_TYPE_INFRAREDALM_IN = 0x03a0,		// 无线报警开始
	HY_LOG_TYPE_INFRAREDALM_END,				// 无线报警结束
	HY_LOG_TYPE_IPCONFLICT,						// IP冲突
	HY_LOG_TYPE_IPCONFLICT_RESUME,				// IP恢复
	HY_LOG_TYPE_SDPLUG_IN,						// SD卡插入
	HY_LOG_TYPE_SDPLUG_OUT,						// SD卡拔出
	HY_LOG_TYPE_NET_PORT_BIND_FAILED,			// 网络端口绑定失败
	HY_LOG_TYPE_AUTOMATIC_RECORD = 0x0400,		// 自动录象
	HY_LOG_TYPE_MANUAL_RECORD,					// 手动录象开
	HY_LOG_TYPE_CLOSED_RECORD,					// 停止录象
	HY_LOG_TYPE_LOGIN = 0x0500,					// 登录
	HY_LOG_TYPE_LOGOUT,							// 注销
	HY_LOG_TYPE_ADD_USER,						// 添加用户
	HY_LOG_TYPE_DELETE_USER,					// 删除用户
	HY_LOG_TYPE_MODIFY_USER,					// 修改用户
	HY_LOG_TYPE_ADD_GROUP,						// 添加用户组
	HY_LOG_TYPE_DELETE_GROUP,					// 删除用户组
	HY_LOG_TYPE_MODIFY_GROUP,					// 修改用户组
	HY_LOG_TYPE_NET_LOGIN = 0x0508,				// 网络用户登录
	HY_LOG_TYPE_CLEAR = 0x0600,					// 清除日志
	HY_LOG_TYPE_SEARCHLOG,						// 查询日志
	HY_LOG_TYPE_SEARCH = 0x0700,				// 录像查询
	HY_LOG_TYPE_DOWNLOAD,						// 录像下载
	HY_LOG_TYPE_PLAYBACK,						// 录像回放
	HY_LOG_TYPE_BACKUP,							// 备份录像文件
	HY_LOG_TYPE_BACKUPERROR,					// 备份录像文件失败
	
	HY_LOG_TYPE_NR = 8,		
} HY_LOG_TYPE_E;

typedef struct
{
	hy_time_t 		time;
    unsigned short	type;					// 类型 HY_LOG_TYPE_E
    unsigned char	reserved;				// 保留
    unsigned char	data;					// 数据  HYNET_CFG_INDEX_E
    unsigned char	context[8];				// 内容
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

