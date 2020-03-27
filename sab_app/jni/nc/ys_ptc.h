
#if defined __cplusplus
extern "C"
{
#endif

#ifndef __PTC_YS_H__
#define __PTC_YS_H__

#define YS_MSG_SIZE				(10*1024)
#define YS_SEARCH_SIZE			(500*1024)
#define YS_MSG_FLAG				0xBCBC
#define YS_MSG_PORT				6060
#define YS_STREAM_PORT			7070
#define YS_TALK_FRAME_SIZE		320

/**
    设备类型类型
**/
typedef enum 
{
    YS_DEV_TYPE_ANALOG_ENCODER = 0x10,  /**< 模拟编码设备 */
    YS_DEV_TYPE_DIGITAL_ENCODER,        /**< 数字编码设备 */
    YS_DEV_TYPE_SOFTWARE_DECODER,       /**< 软件解码器 */
    YS_DEV_TYPE_HARDWARE_DECODER,       /**< 硬件解码器 */
    YS_DEV_TYPE_STORAGE_RECORD,         /**< 存储纪录设备 */
    YS_DEV_TYPE_VIRTUAL_DEVICE = 0x20,  /**< 虚拟设备 */
    YS_DEV_TYPE_BUTT
}YS_DEV_TYPE_E;

/**
    设备下子资源类型
**/
typedef enum tagResType
{
    YS_RES_TYPE_VIDEO_IN_CHL = 0x01,            /**< 视频输入编码通道 */
    YS_RES_TYPE_VIDEO_DISPLAY_OUTPUT_PORT,      /**< 视频显示输出端口 */
    YS_RES_TYPE_INDEPENDENT_AUDIO_INPUT_CHL,    /**< 独立音频输入通道 */
    YS_RES_TYPE_INDEPENDENT_AUDIO_OUTPUT_CHL,   /**< 独立音频输出通道 */
    YS_RES_TYPE_ATTACHED_AUDIO_INPUT_CHL,       /**< 随路音频输出通道 */
    YS_RES_TYPE_ATTACHED_AUDIO_OUTPUT_CHL,      /**< 随路音频输出通道 */
    YS_RES_TYPE_ALARM_IN_CHL,                   /**< 告警输入通道 */
    YS_RES_TYPE_ALARM_OUT_CHL,                  /**< 告警输出通道 */
    YS_RES_TYPE_SERIAL_CHL,                     /**< 串口通道 */
    YS_RES_TYPE_DISK_SLOT,                      /**< 磁盘槽位号 */
    YS_RES_TYPE_ALARM_BUZZER_CHL,               /**< 蜂鸣器通道 */
    YS_RES_TYPE_NETWARK_CHL,                    /**< 网卡通道 */
    YS_RES_TYPE_DECODE_CHL,                     /**< 解码通道 */
    YS_RES_TYPE_BUTT
}YS_RES_TYPE_E;


typedef enum 
{
    /* 登录业务0~99*/
    YS_MSG_ID_USER_LOGIN = 0,                     /**< 用户登录*/
    YS_MSG_ID_USER_LOGOUT,                        /**< 用户注销*/
    YS_MSG_ID_USER_KEEPLIVE,                      /**< 用户保活*/
    YS_MSG_ID_BIND_NOTIFY,                        /**< 订阅推送消息(绑定反向通讯端口) */
    YS_MSG_ID_UNBIND_NOTIFY,                      /**< 取消订阅*/
    YS_MSG_ID_NOTIFY_DEV_STATUS,                  /**< 上报设备状态（上下线、视频丢失）*/

    /* 实况、语音业务100~199*/
    YS_MSG_ID_START_LIVE = 100,                   /**< 开启普通实况 */
    YS_MSG_ID_START_LIVE_MMI,                     /**< 开启人机实况 */
    YS_MSG_ID_STOP_LIVE,                          /**< 关闭实况 */
    YS_MSG_ID_START_TALK,                         /**< 开启普通语音对讲 */
    YS_MSG_ID_START_TALK_MMI,                     /**< 开启人机语音对讲 */
    YS_MSG_ID_STOP_TALK,                          /**< 停止语音对讲*/
    YS_MSG_ID_NOTIFY_LIVE_EXCEPTION,              /**< 实况类异常上报消息 */
    YS_MSG_ID_MAKE_KEY_FRAME,                     /**< 设置关键帧 */

    /* 录像查询、回放业务200~299*/
    YS_MSG_ID_VOD_QRY_MONTH = 200,                /**< 按天查询录像状态 */
    YS_MSG_ID_VOD_QRY,                            /**< 录像查询 */
    YS_MSG_ID_VOD_OPEN,                           /**< 启动回放业务 */
    YS_MSG_ID_VOD_OPEN_MMI,                       /**< 启动人机回放业务 */
    YS_MSG_ID_VOD_PLAY,                           /**< 播放(控制)回放业务*/
    YS_MSG_ID_VOD_CLOSE,                          /**< 停止回放业务 */
    YS_MSG_ID_VOD_GET_PLAYSTATUS,                 /**< 获取回放信息 */
    YS_MSG_ID_NOTIFY_VOD_EXCEPTION,               /**< 回放类异常上报消息 */
    YS_MSG_ID_VOD_JOIN_GROUP,
    YS_MSG_ID_VOD_JOIN_GROUP_MMI,
    YS_MSG_ID_VOD_LEAVE_GROUP,
    YS_MSG_ID_VOD_QUERY_LOCK_FILE,
    YS_MSG_ID_VOD_SET_LOCK_FILE,
    YS_MSG_ID_VOD_QUERY_LOCK_FILE_INFO,
    YS_MSG_ID_VOD_QUERY_TIME_RANGE,

    /* 录像、下载300~399*/
    YS_MSG_ID_RECORD = 300,                       /**< 启动/停止录像 */
    YS_MSG_ID_QUERY_RECORD_STATUS,                /**< 查询当前录像状态 */

    /* 云台配置、控制 400~499*/
    YS_MSG_ID_SET_PRESET = 400,                   /**< 预置位设置 */
    YS_MSG_ID_QUERY_PRESET,                       /**< 预置位查询 */
    YS_MSG_ID_QUERY_TRACK,                        /**< 巡航轨迹查询 */
    YS_MSG_ID_START_RECORD_TRACK,                 /**< 巡航轨迹录制开始 */
    YS_MSG_ID_STOP_RECORD_TRACK,                  /**< 巡航轨迹录制结束 */
    YS_MSG_ID_QUERY_CRUISE,                       /**< 巡航线路查询 */
    YS_MSG_ID_SET_CRUISE,                         /**< 巡航线路设置 */
    YS_MSG_ID_PTZ_COMMAND,                        /**< 云台命令 */

    /* 能力获取500~599 */
    YS_MSG_ID_QUERY_VIDEO_CHL_LIST = 500,         /**< 查询视频通道列表(简单信息) */
    YS_MSG_ID_QUERY_VIDEO_CHL_DETAIL_LIST,        /**< 查询视频通道列表(详细信息) */
    YS_MSG_ID_QUERY_ALARM_IN_LIST,                /**< 查询输入开关量列表 */
    YS_MSG_ID_QUERY_ALARM_OUT_LIST,               /**< 查询输出开关量列表 */
    YS_MSG_ID_QUERY_SERIAL_LIST,                  /**< 查询串口列表 */
    YS_MSG_ID_QUERY_AUDIO_LIST,                   /**< 查询语音资源列表 */

    /* 基本配置600~699 */
    YS_MSG_ID_QUERY_DEV_INFO = 600,               /**< 设备信息查询 */
    YS_MSG_ID_SET_DEV_INFO,                       /**< 设备信息设置 */
    YS_MSG_ID_QUERY_TIME,                         /**< 设备时间查询 */
    YS_MSG_ID_SET_TIME,                           /**< 设备时间设置 */

    /* 通道配置700~899 */
    YS_MSG_ID_QUERY_CHANNEL_BASIC_INFO = 700,     /**< 通道基本信息查询 */
    YS_MSG_ID_SET_CHANNEL_BASIC_INFO,             /**< 通道基本信息设置 */
    YS_MSG_ID_QUERY_VIDEO_PARAMETER,              /**< 通道光学参数查询 */
    YS_MSG_ID_SET_VIDEO_PARAMETER,                /**< 通道光学参数设置 */
    YS_MSG_ID_QUERY_CHANNEL_NAME,                 /**< 通道名称批量查询 */
    YS_MSG_ID_SET_CHANNEL_NAME,                   /**< 通道名称批量设置 */
    YS_MSG_ID_QUERY_STREAM_INFO,                  /**< 通道视频参数查询 */
    YS_MSG_ID_SET_STREAM_INFO,                    /**< 通道视频参数设置 */
    YS_MSG_ID_QUERY_PTZ,                          /**< 云台配置查询 */
    YS_MSG_ID_SET_PTZ,                            /**< 云台配置 */
    YS_MSG_ID_QUERY_RECORD_PLAN,                  /**< 录像计划查询 */
    YS_MSG_ID_SET_RECORD_PLAN,                    /**< 录像计划设置 */
    YS_MSG_ID_QUERY_MOTION_DETECTION,             /**< 运动检测查询 */
    YS_MSG_ID_SET_MOTION_DETECTION,               /**< 运动检测设置 */
    YS_MSG_ID_QUERY_VIDEO_LOSS,                   /**< 视频丢失查询 */
    YS_MSG_ID_SET_VIDEO_LOSS,                     /**< 视频丢失设置 */
    YS_MSG_ID_QUERY_PRIVACY_MASK,                 /**< 视频遮盖查询 */
    YS_MSG_ID_SET_PRIVACY_MASK,                   /**< 视频遮盖设置 */
    YS_MSG_ID_QUERY_TAMPER_PROOF,                 /**< 视频遮挡查询 */
    YS_MSG_ID_SET_TAMPER_PROOF,                   /**< 视频遮挡设置 */
    YS_MSG_ID_QUERY_TEXT_OVERLAY,                 /**< OSD 叠加查询 */
    YS_MSG_ID_SET_TEXT_OVERLAY,                   /**< OSD 叠加设置 */
    YS_MSG_ID_QUERY_HOLIDAY,                      /**< 假日查询 */
    YS_MSG_ID_SET_HOLIDAY,                        /**< 假日设置 */
    YS_MSG_ID_SET_AUDIO_CHL_FOR_MMI,              /**< 人机设置音频通道 */

    /* 用户权限配置 900~999*/
    YS_MSG_ID_QUERY_USER_LIST = 900,              /**< 获取用户列表 */
    YS_MSG_ID_QUERY_USER,                         /**< 查询单个用户具体信息 */
    YS_MSG_ID_ADD_USER,                           /**< 新增用户 */
    YS_MSG_ID_MODIFY_USER,                        /**< 修改用户 */
    YS_MSG_ID_DELETE_USER,                        /**< 删除用户 */

    /* 开关量配置 1000~1099 */
    YS_MSG_ID_QUERY_ALARM_INPUT = 1000,           /**< 输入开关量查询 */
    YS_MSG_ID_SET_ALARM_INPUT,                    /**< 输入开关量设置 */
    YS_MSG_ID_QUERY_ALARM_OUTPUT,                 /**< 输出开关量查询 */
    YS_MSG_ID_SET_ALARM_OUTPUT,                   /**< 输出开关量设置 */
    YS_MSG_ID_MANUALLY_ACTION_OUTPUT,             /**< 触发输出开关量 */
    YS_MSG_ID_GET_ALARM_OUTPUT_STATUS,            /**< 获取开关量状态 */

    /* 串口配置 1100~1199 */
    YS_MSG_ID_QUERY_SERIAL = 1100,                /**< 串口配置查询 */
    YS_MSG_ID_SET_SERIAL,                         /**< 串口配置 */

    /* 网络配置 1200~1299 */
    YS_MSG_ID_QUERY_NETWORK = 1200,               /**< 网络基本配置查询 */
    YS_MSG_ID_SET_NETWORK,                        /**< 网络基本配置设置 */
    YS_MSG_ID_QUERY_PPPOE,                        /**< PPPoE信息查询 */
    YS_MSG_ID_SET_PPPOE,                          /**< PPPoE信息设置 */
    YS_MSG_ID_QUERY_DDNS,                         /**< DDNS信息查询 */
    YS_MSG_ID_SET_DDNS,                           /**< DDNS信息设置 */
    YS_MSG_ID_QUERY_UPNP,                         /**< UPNP信息查询 */
    YS_MSG_ID_SET_UPNP,                           /**< UPNP信息设置 */
    YS_MSG_ID_QUERY_SMTP,                         /**< SMTP信息查询 */
    YS_MSG_ID_SET_SMTP,                           /**< SMTP信息设置 */
    YS_MSG_ID_QUERY_PORT,                         /**< 端口获取 */
    YS_MSG_ID_SET_PORT,                           /**< 端口设置 */

    /* 异常配置 1300~1399 */
    YS_MSG_ID_QUERY_EXCEPTION = 1300,             /**< 异常查询 */
    YS_MSG_ID_SET_EXCEPTION,                      /**< 异常设置 */

    /* 磁盘配置 1400~1499 */
    YS_MSG_ID_QUERY_DISK_LIST = 1400,             /**< 磁盘信息列表 */
    YS_MSG_ID_SET_DISK_PROPERTY,                  /**< 磁盘属性设置 */
    YS_MSG_ID_INIT_DISK,                          /**< 磁盘初始化 */

    /* 维护业务 1500~1599 */
    YS_MSG_ID_DEV_RESTART = 1500,                 /**< 设备重启 */
    YS_MSG_ID_DEV_SHUTDOWN,                       /**< 设备关机 */
    YS_MSG_ID_SIMPLE_RECOVER_DEFAULT_CONFIG,      /**< 简单恢复默认配置 */
    YS_MSG_ID_RECOVER_DEFAULT_CONFIG,             /**< 完全恢复默认配置 */
    YS_MSG_ID_CONFIG_IMPORT,                      /**< 配置导入 */
    YS_MSG_ID_CONFIG_EXPORT,                      /**< 配置导出 */
    YS_MSG_ID_SYSTEM_UPGRADE,                     /**< 设备重启 */
    YS_MSG_ID_SYSTEM_DYNAMIC_INFO,                /**< 系统动态信息 */
    YS_MSG_ID_SET_AUTO_MAINTAIN,                  /**< 设置自动维护信息 */ 
    YS_MSG_ID_QUERY_AUTO_MAINTAIN,                /**< 查询自动维护信息 */

    /* 告警、日志 1600~1699*/
    YS_MSG_ID_ALARM_ACTION_MONITOR = 1600,        /**< 告警联动到监视器通知消息 */
    YS_MSG_ID_QUERY_LOG,                          /**< 查询日志 */

    /* IPC 1700~1899 */
    YS_MSG_ID_ADD_IPC = 1700,                     /**< 添加IPC */
    YS_MSG_ID_MOD_IPC,                            /**< 修改IPC */
    YS_MSG_ID_DEL_IPC,                            /**< 删除IPC */
    YS_MSG_ID_QUERY_SINGLE_IPC,                   /**< 查询单个IPC信息 */
    YS_MSG_ID_QUERY_ALL_IPC,                      /**< 查询IPC列表信息 */
    YS_MSG_ID_PROBE_BY_FAMILY,                    /**< 组播探测 */
    YS_MSG_ID_PROBE_BY_IP,                        /**< 按网段探测 */
    YS_MSG_ID_GET_PROBED_IPC,                     /**< 获取搜索到的信息 */

    /* 内部VMP消息类型 10000~*/
    YS_MSG_ID_BUTT = 0xFFFF                    /**< 无效值 */
}YS_MSG_ID_E;

/* IEID */
typedef enum 
{
    /* 公用IE 0~499*/
    YS_IE_ID_USER_SIMPLE_INFO = 0,                   /**< 用户账号（用户名＋密码） */
    YS_IE_ID_IP_ADDRESS       = 1,                   /**< IP地址（类型＋地址＋端口） */
    YS_IE_ID_USER_LOG_INFO    = 2,                   /**< 用户登录信息（用户名＋登录ID＋登录IP） */
    YS_IE_ID_TASK_NO          = 3,                   /**< 业务号 */
    YS_IE_ID_TIME_RANGE       = 4,                   /**< 开始结束时间段 */
    YS_IE_ID_RESOURCE_CODE    = 5,                   /**< 资源编号信息 */
    YS_IE_ID_RESOURCE_INFO    = 6,                   /**< 资源编码＋名称 */
    YS_IE_ID_COPY_TO          = 7,                   /**< 复制到 */
    YS_IE_ID_ENABLED          = 8,                   /**< 使能标志 */
    YS_IE_ID_SCHEDULE         = 9,                   /**< 计划 */
    YS_IE_ID_DECODING_CHANNEL_CODE = 10,             /**< 通道编号 */
    YS_IE_ID_DEV_ABILITY_INFO      = 11,             /**< 设备能力集 */

    /* 其他IE 500~ */
    YS_IE_ID_PORT_MAPPING = 500,                     /**< 客户端需要的端口信息 */
    YS_IE_ID_MEDIA_IP     = 501,                     /**< 用于业务协商交互的收发流IP信息 */
    YS_IE_ID_MEDIA_TRANSPORT = 502,                  /**< 传输协议 */
    YS_IE_ID_MEDIA_VIDEO_STREAM_INDEX = 503,         /**< 视频流索引 */
    YS_IE_ID_MEDIA_AUDIO_STREAM_INFO  = 504,         /**< 音频参数 */
    YS_IE_ID_EXCEPTION_REASON = 505,                 /**< 异常原因 */
    YS_IE_ID_VOD_MONTH_STATUS = 506,                 /**< 按位表示某天的录像状态，最多支持64天 */
    YS_IE_ID_STORAGE_TYPE     = 507,                 /**< 录像类型 */
    YS_IE_ID_RECORD_CTRL      = 508,                 /**< 录像类型＋控制 */
    YS_IE_ID_STORAGE_STATUS   = 509,                 /**< 录像状态，所有通道 */
    YS_IE_ID_VOD_FILE         = 510,                 /**< 录像段信息 */
    YS_IE_ID_PLAY_STATUS      = 511,                 /**< 回放控制信息 */
    YS_IE_ID_DEV_BASIC_INFO   = 512,                 /**< 设备基本信息 */
    YS_IE_ID_DEV_FIRMWARE_INFO = 513,                /**< 设备固件信息 */
    YS_IE_ID_DEV_TIME_ZONE_INFO = 514,               /**< 设备时区信息 */
    YS_IE_ID_MODIFY_TIME_TYPE = 515,                 /**< 修改时间类型 */
    YS_IE_ID_DEV_NTP     = 516,                      /**< NTP服务信息 */
    YS_IE_ID_DEV_TIME    = 517,                      /**< 设备时间信息 */
    YS_IE_ID_OSD_TIME    = 518,                      /**< 时间OSD */
    YS_IE_ID_OSD_NAME    = 519,                      /**< 名称OSD */
    YS_IE_ID_OSD_PROPERTY = 520,                     /**< OSD属性 */
    YS_IE_ID_VIDEO_DISPLAY_INFO = 521,               /**< 视频显示参数信息 */
    YS_IE_ID_AUDIO_STREAM_INFO  = 522,               /**< 音频参数 */
    YS_IE_ID_VIDEO_STREAM_INFO  = 523,               /**< 视频流参数 */
    YS_IE_ID_RECORD_ADVANCED    = 524,               /**< 录像高级配置 */
    YS_IE_ID_AREA_SENSITIVITY   = 525,               /**< 区域灵敏度 */
    YS_IE_ID_AREA_INFO          = 526,               /**< 区域配置 */
    YS_IE_ID_LINKAGE_METHOD     = 527,               /**< 告警联动动作 */
    YS_IE_ID_OSD_TEXT_OVERLAY   = 528,               /**< 字符叠加OSD */
    YS_IE_ID_HOLIDAY_INFO       = 529,               /**< 假日信息 */
    YS_IE_ID_USER_NAME          = 530,               /**< 用户名 */
    YS_IE_ID_USER_LIST_INFO     = 531,               /**< 用户基本信息（列表用） */
    YS_IE_ID_USER_BASE_INFO     = 532,               /**< 用户信息 */
    YS_IE_ID_USER_PERMISSION    = 533,               /**< 用户权限信息 */
    YS_IE_ID_ALARM_INPUT_INFO   = 534,               /**< 输入开关量基本信息 */
    YS_IE_ID_ALARM_OUTPUT_INFO  = 535,               /**< 输出开关量基本信息 */
    YS_IE_ID_ALARM_OUTPUT_STATUS = 536,              /**< 输出开关量状态 */
    YS_IE_ID_IPV4_INFO    = 537,                     /**< IPv4信息 */
    YS_IE_ID_IPV6_INFO    = 538,                     /**< IPv6信息 */
    YS_IE_ID_NIC_INFO     = 539,                     /**< 网卡基本信息 */
    YS_IE_ID_DNS_INFO     = 540,                     /**< DNS信息 */
    /**add by lzj*/
    YS_IE_ID_DDNS_SERIAL_NUM        = 541,
    YS_IE_ID_DDNS_CONNECT_STATUS    = 542,
    YS_IE_ID_DDNS_CONFIGURABLE      = 543,
    YS_IE_ID_DDNS_SHOW_INFO         = 544,
    YS_IE_ID_DYNAMIC_PORT_MAPPING   = 545,           /**< 动态端口映射 */
    YS_IE_ID_PORT_STATUS            = 546,           /**< 端口状态 */
    YS_IE_ID_SMTP_SERVER_INFO       = 547,           /**< 邮件服务器信息 */
    YS_IE_ID_ATTACHED_IMAGE         = 548,           /**< 图片附件信息 */
    YS_IE_ID_SENDER_INFO            = 549,           /**< 发件人信息 */
    YS_IE_ID_RECIEVER_INFO          = 550,           /**< 收件人信息 */
    YS_IE_ID_STATIC_PORT_MAPPING    = 551,           /**< 静态端口映射 */
    YS_IE_ID_FILENAME_PATH          = 552,           /**< 文件路径和文件名称 */
    YS_IE_ID_LOG_TYPE               = 553,           /**< 日志类型 */
    YS_IE_ID_OPT_LOG_INFO           = 554,           /**< 日志信息 */
    YS_IE_ID_PRESET_INDEX           = 555,           /**< 预置位索引 */
    YS_IE_ID_PRESET_LIST            = 556,           /**< 预置位列表 */
    YS_IE_ID_TRACK_INDEX            = 557,           /**< 轨迹索引 */
    YS_IE_ID_TRACK_LIST             = 558,           /**< 轨迹列表 */
    YS_IE_ID_CRUISE_INFO            = 559,           /**< 巡航线路信息 */
    YS_IE_ID_PTZ_COMMAND            = 560,           /**< 云台命令 */
    YS_IE_ID_PTZ_PARAM              = 561,           /**< 云台配置参数 */
    YS_IE_ID_SERIAL_INFO            = 562,           /**< 串口信息 */
    YS_IE_ID_CONNECT_STATUS         = 563,           /**< 连接状态 */
    YS_IE_ID_MAPPING_MODE           = 564,           /**< 映射模式 */
    YS_IE_ID_DISK_INFO              = 565,           /**< 磁盘信息 */
    YS_IE_ID_DISK_PROPERTY          = 566,           /**< 磁盘属性 */
    YS_IE_ID_EXCEPTION_TYPE         = 567,           /**< 异常类型 */
    YS_IE_ID_IPC_DEV_INFO           = 568,           /**< IPC设备信息 */
    YS_IE_ID_IPC_LIST_DEV_INFO      = 569,           /**< IPC设备列表信息 */
    YS_IE_ID_PROBE_REQUEST_BY_FAMILY = 570,          /**< 组播探测 */
    YS_IE_ID_PROBE_REQUEST_BY_IP     = 571,          /**< 根据网段来探测设备 */
    YS_IE_ID_DISCOVERY_DEV           = 572,          /**< 探测到的设备的信息 */
    YS_IE_ID_SESSION_ID              = 573,          /**< 引流会话ID         */
    YS_IE_ID_VIDEO_CHL_SIMPLE_INFO_LIST  = 574,      /**< 视频通道简单信息列表（只返回绑定的通道）*/
    YS_IE_ID_VIDEO_CHL_DETAIL_INFO_LIST  = 575,      /**< 视频通道详细信息列表（只返回绑定的通道）*/
    YS_IE_ID_SERIAL_LIST                 = 576,      /**< 串口列表 */
    YS_IE_ID_IP_INFO                     = 577,      /**< IP地址信息 */
    YS_IE_ID_ALARM_INPUT_INNER           = 578,      /**< 输入开关量内部信息*/
    YS_IE_ID_ALARM_OUTPUT_INNER          = 579,      /**< 输出开关量内部信息 */
    YS_IE_ID_SYS_DYNAMIC_INFO            = 580,      /**< 系统动态信息 */
    YS_IE_ID_USER_LOGIN_TYPE             = 581,      /**< 用户登录类型 */
    YS_IE_ID_AUTO_MAINTAIN_INFO          = 582,      /**< 自动维护信息 */
    YS_IE_ID_AUDIO_CHL_INFO_LIST         = 583,      /**< 语音对讲资源列表信息 */
    YS_IE_ID_AUDIO_CHL_BIND              = 584,
    YS_IE_ID_REC_FILE_NAME               = 585,
    YS_IE_ID_RECORD_FILE                 = 586,
    YS_IE_ID_REC_FILE_LOCK_STATUS        = 587,
    YS_IE_ID_LOCK_FILE_PAGE_INFO         = 588,
    YS_IE_ID_LOCK_TOTAL_FILE_COUNT       = 589,
    YS_IE_ID_LOCK_RECORD_FILE_LIST       = 590,
    /* 内部IE 5000*/

	YS_IE_ID_DST						 = 625,

    YS_IE_ID_BUTT = 0xFFFF                     /**< 无效值 */
}YS_IE_ID_E;

typedef enum 
{
    YS_AF_INET 		= 2,      /* for IPv4, AF_INET = 2 */
    YS_AF_INET6 	= 10,     /* for IPv6, AF_INET6 = 10 */
}YS_IP_DOMAIN_TYPE_E;

/* 用户登录类型 */
typedef enum 
{
    YS_USER_LOGIN_TYPE_WEB,                    /**< web侧登录 */
    YS_USER_LOGIN_TYPE_MMI,                    /**< 人机侧登录 */
    YS_USER_LOGIN_TYPE_MMI_SPEC,               /**< 人机特殊登录(目前只用于处理不记录用户登录操作日志) */
    YS_USER_LOGIN_TYPE_THIRD,                  /**< 第三方登录 */
    YS_USER_LOGIN_TYPE_MOBILE,                 /**< 手机客户端登录 */

    YS_USER_LOGIN_TYPE_MAX,                    /**< 用户登录类型最大值 */
    YS_USER_LOGIN_TYPE_INVALID = 0XFFFF        /**< 用户登录类型无效值 */
}YS_USER_LOGIN_TYPE_E;

/* 云台命令 */
typedef enum 
{
    YS_PTZ_CMD_IRISCLOSESTOP        =0x0101,       /**< 光圈关停止 */
    YS_PTZ_CMD_IRISCLOSE            =0x0102,       /**< 光圈关 */
    YS_PTZ_CMD_IRISOPENSTOP         =0x0103,       /**< 光圈开停止 */
    YS_PTZ_CMD_IRISOPEN             =0x0104,       /**< 光圈开 */

    YS_PTZ_CMD_FOCUSNEARSTOP        =0x0201,       /**< 近聚集停止 */
    YS_PTZ_CMD_FOCUSNEAR            =0x0202,       /**< 近聚集 */
    YS_PTZ_CMD_FOCUSFARSTOP         =0x0203,       /**< 远聚集 停止*/
    YS_PTZ_CMD_FOCUSFAR             =0x0204,       /**< 远聚集 */

    YS_PTZ_CMD_ZOOMTELESTOP         =0x0301,       /**< 放大停止 */
    YS_PTZ_CMD_ZOOMTELE             =0x0302,       /**< 放大 */
    YS_PTZ_CMD_ZOOMWIDESTOP         =0x0303,       /**< 缩小停止 */
    YS_PTZ_CMD_ZOOMWIDE             =0x0304,       /**< 缩小 */

    YS_PTZ_CMD_TILTUPSTOP           =0x0401,       /**< 向上停止 */
    YS_PTZ_CMD_TILTUP               =0x0402,       /**< 向上 */
    YS_PTZ_CMD_TILTDOWNSTOP         =0x0403,       /**< 向下停止 */
    YS_PTZ_CMD_TILTDOWN             =0x0404,       /**< 向下 */
    YS_PTZ_CMD_UPTELESTOP           =0x0411,       /**< 向上放大停止 */
    YS_PTZ_CMD_UPTELE               =0x0412,       /**< 向上放大 */
    YS_PTZ_CMD_DOWNTELESTOP         =0x0413,       /**< 向下放大停止 */
    YS_PTZ_CMD_DOWNTELE             =0x0414,       /**< 向下放大 */
    YS_PTZ_CMD_UPWIDESTOP           =0x0421,       /**< 向上缩小停止 */
    YS_PTZ_CMD_UPWIDE               =0x0422,       /**< 向上缩小 */
    YS_PTZ_CMD_DOWNWIDESTOP         =0x0423,       /**< 向下缩小停止 */
    YS_PTZ_CMD_DOWNWIDE             =0x0424,       /**< 向下缩小 */

    YS_PTZ_CMD_PANRIGHTSTOP         =0x0501,       /**< 向右停止 */
    YS_PTZ_CMD_PANRIGHT             =0x0502,       /**< 向右 */
    YS_PTZ_CMD_PANLEFTSTOP          =0x0503,       /**< 向左停止 */
    YS_PTZ_CMD_PANLEFT              =0x0504,       /**< 向左 */
    YS_PTZ_CMD_RIGHTTELESTOP        =0x0511,       /**< 向右放大停止 */
    YS_PTZ_CMD_RIGHTTELE            =0x0512,       /**< 向右放大 */
    YS_PTZ_CMD_LEFTTELESTOP         =0x0513,       /**< 向左放大停止 */
    YS_PTZ_CMD_LEFTTELE             =0x0514,       /**< 向左放大 */

    YS_PTZ_CMD_RIGHTWIDESTOP        =0x0521,       /**< 向右缩小停止 */
    YS_PTZ_CMD_RIGHTWIDE            =0x0522,       /**< 向右缩小 */
    YS_PTZ_CMD_LEFTWIDESTOP         =0x0523,       /**< 向左缩小停止 */
    YS_PTZ_CMD_LEFTWIDE             =0x0524,       /**< 向左缩小 */

    YS_PTZ_CMD_PRESAVE              =0x0601,       /**< 预置位保存 */
    YS_PTZ_CMD_PRECALL              =0x0602,       /**< 预置位调用 */
    YS_PTZ_CMD_PREDEL               =0x0603,       /**< 预置位删除 */

    YS_PTZ_CMD_LEFTUPSTOP           =0x0701,       /**< 左上停止 */
    YS_PTZ_CMD_LEFTUP               =0x0702,       /**< 左上 */
    YS_PTZ_CMD_LEFTDOWNSTOP         =0x0703,       /**< 左下停止 */
    YS_PTZ_CMD_LEFTDOWN             =0x0704,       /**< 左下 */
    YS_PTZ_CMD_LEFTUPTELESTOP       =0x0711,       /**< 左上放大停止 */
    YS_PTZ_CMD_LEFTUPTELE           =0x0712,       /**< 左上放大 */
    YS_PTZ_CMD_LEFTDOWNTELESTOP     =0x0713,       /**< 左下放大停止 */
    YS_PTZ_CMD_LEFTDOWNTELE         =0x0714,       /**< 左下放大 */
    YS_PTZ_CMD_LEFTUPWIDESTOP       =0x0721,       /**< 左上缩小停止 */
    YS_PTZ_CMD_LEFTUPWIDE           =0x0722,       /**< 左上缩小 */
    YS_PTZ_CMD_LEFTDOWNWIDESTOP     =0x0723,       /**< 左下缩小停止 */
    YS_PTZ_CMD_LEFTDOWNWIDE         =0x0724,       /**< 左下缩小 */

    YS_PTZ_CMD_RIGHTUPSTOP          =0x0801,       /**< 右上停止 */
    YS_PTZ_CMD_RIGHTUP              =0x0802,       /**< 右上 */
    YS_PTZ_CMD_RIGHTDOWNSTOP        =0x0803,       /**< 右下停止 */
    YS_PTZ_CMD_RIGHTDOWN            =0x0804,       /**< 右下 */
    YS_PTZ_CMD_RIGHTUPTELESTOP      =0x0811,       /**< 右上放大停止 */
    YS_PTZ_CMD_RIGHTUPTELE          =0x0812,       /**< 右上放大 */
    YS_PTZ_CMD_RIGHTDOWNTELESTOP    =0x0813,       /**< 右下放大停止 */
    YS_PTZ_CMD_RIGHTDOWNTELE        =0x0814,       /**< 右下放大 */
    YS_PTZ_CMD_RIGHTUPWIDESTOP      =0x0821,       /**< 右上缩小停止 */
    YS_PTZ_CMD_RIGHTUPWIDE          =0x0822,       /**< 右上缩小 */
    YS_PTZ_CMD_RIGHTDOWNWIDESTOP    =0x0823,       /**< 右下缩小停止 */
    YS_PTZ_CMD_RIGHTDOWNWIDE        =0x0824,       /**< 右下缩小 */

    YS_PTZ_CMD_ALLSTOP              = 0x0901,      /**< 全停命令字 */
    YS_PTZ_CMD_ABSPOSITION          = 0x0902,      /**< 设置摄像机绝对角度 */
    YS_PTZ_CMD_GUARD                = 0x0903,      /**< 回看守位，该命令不在云台模板体现 */
    YS_PTZ_CMD_SPEEDSET             = 0x0904,      /**< 设置速度 */

    YS_PTZ_CMD_BRUSHON              = 0x0A01,      /**< 雨刷开 */
    YS_PTZ_CMD_BRUSHOFF             = 0x0A02,      /**< 雨刷关 */

    YS_PTZ_CMD_LIGHTON              = 0x0B01,      /**< 灯开 */
    YS_PTZ_CMD_LIGHTOFF             = 0x0B02,      /**< 灯关 */

    YS_PTZ_CMD_HEATON               = 0x0C01,      /**< 加热开 */
    YS_PTZ_CMD_HEATOFF              = 0x0C02,      /**< 加热关 */

    YS_PTZ_CMD_INFRAREDON           = 0x0D01,      /**< 红外开 */
    YS_PTZ_CMD_INFRAREDOFF          = 0x0D02,      /**< 红外关 */

    YS_PTZ_CMD_SCANCRUISE           = 0x0E01,      /**< 云台线性扫猫 */
    YS_PTZ_CMD_SCANCRUISESTOP       = 0x0E02,      /**< 云台线性扫猫 */

    YS_PTZ_CMD_TRACKCRUISE          = 0x0F01,      /**< 云台轨迹巡航 */
    YS_PTZ_CMD_TRACKCRUISESTOP      = 0x0F02,      /**< 停止云台轨迹巡航 */
    YS_PTZ_CMD_TRACKCRUISEREC       = 0x0F03,      /**< 开始录制轨迹 */
    YS_PTZ_CMD_TRACKCRUISERECSTOP   = 0x0F04,      /**< 停止录制轨迹 */

    YS_PTZ_CMD_PRESETCRUISE         = 0x1001,      /**< 云台按预置位巡航 ，该命令字不在云台模板体现 */
    YS_PTZ_CMD_PRESETCRUISESTOP     = 0x1002,      /**< 云台按预置位巡航停止，该命令字不在云台模板体现 */
    YS_PTZ_CMD_CRUISEDEL            = 0X1003,      /**< 删除线路 */
    YS_PTZ_CMD_CRUISEADDPRESET      = 0x1004,      /**< 往线路中添加预置位 */
    YS_PTZ_CMD_CRUISEADDSTART       = 0x1005,      /**< 开始往线路中添加预置位 */
    YS_PTZ_CMD_CRUISEADDSTOP        = 0x1006,      /**< 结束往线路中添加预置位 */

    YS_PTZ_CMD_AREAZOOMIN           = 0x1101,      /**< 拉框放大 */
    YS_PTZ_CMD_AREAZOOMOUT          = 0x1102,      /**< 拉框缩小 */

    YS_PTZ_CMD_BUTT,
} YS_PTZ_CMD_E;

/* 录像存储类型 */
typedef enum 
{
    YS_RECORD_TYPE_NORMAL = 0,      /**< 常规存储 */
    YS_RECORD_TYPE_MOVEDETECT,      /**< 运动检测存储 */
    YS_RECORD_TYPE_ALARM,           /**< 告警存储 */
    YS_RECORD_TYPE_MA,              /**< 运动检测和告警存储 */
    YS_RECORD_TYPE_M_A,             /**< 运动检测或告警存储存储 */
    YS_RECORD_TYPE_UW_MANUAL,       /**< 手动存储 */

    YS_RECORD_TYPE_MAX,             /**< 存储类型最大值 */
    YS_RECORD_TYPE_INVALID = 0xFF   /**< 存储类型无效值 */
}YS_RECORD_TYPE_E;

/* 播放状态枚举定义 */
typedef enum 
{
    /** 播放状态 */
    YS_PBS_STATUS_16_BACKWARD = 0,     /**< 16倍速后退播放 */
    YS_PBS_STATUS_8_BACKWARD = 1,      /**< 8倍速后退播放 */
    YS_PBS_STATUS_4_BACKWARD = 2,      /**< 4倍速后退播放 */
    YS_PBS_STATUS_2_BACKWARD = 3,      /**< 2倍速后退播放 */
    YS_PBS_STATUS_1_BACKWARD = 4,      /**< 正常速度后退播放 */
    YS_PBS_STATUS_HALF_BACKWARD = 5,   /**< 1/2倍速后退播放 */
    YS_PBS_STATUS_QUARTER_BACKWARD = 6,/**< 1/4倍速后退播放 */
    YS_PBS_STATUS_QUARTER_FORWARD = 7, /**< 1/4倍速播放 */
    YS_PBS_STATUS_HALF_FORWARD = 8,    /**< 1/2倍速播放 */
    YS_PBS_STATUS_1_FORWARD = 9,       /**< 正常速度前进播放 */
    YS_PBS_STATUS_2_FORWARD = 10,      /**< 2倍速前进播放 */
    YS_PBS_STATUS_4_FORWARD = 11,      /**< 4倍速前进播放 */
    YS_PBS_STATUS_8_FORWARD = 12,      /**< 8倍速前进播放 */
    YS_PBS_STATUS_16_FORWARD = 13,     /**< 16倍速前进播放 */

    /** 下载状态 */
    YS_PBS_STATUS_1_DOWN = 51,         /**< 正常速度下载 */
    YS_PBS_STATUS_2_DOWN = 52,         /**< 2倍速下载 */
    YS_PBS_STATUS_4_DOWN = 53,         /**< 4倍速下载 */
    YS_PBS_STATUS_8_DOWN = 54,         /**< 8倍速下载 */

    /** 扩展播放状态 */
    YS_PBS_STATUS_PAUSE = 100,         /**< 暂停播放 */
    YS_PBS_STATUS_RESUME = 101,        /**< 恢复播放 */
    YS_PBS_STATUS_STEP_FORWARD = 102,  /**< 单步前进播放 */
    YS_PBS_STATUS_STEP_BACKWARD = 103, /**< 单步后退播放 */
    YS_PBS_STATUS_JUMP = 104,          /**< 跳转播放 */

    YS_PBS_STATUS_INVALID
}YS_PBS_STATUS_E;

/* 主类型枚举 */
typedef enum 
{
    YS_LOG_MAINTYPE_ALL = 0,              /* 全部类型的日志*/

    YS_LOG_MAINTYPE_ALARM = 1,            /* 告警类 */
    YS_LOG_MAINTYPE_EXCEPTION,            /* 异常类 */
    YS_LOG_MAINTYPE_OPERATION,            /* 操作类 */
    YS_LOG_MAINTYPE_MESSAGE,              /* 信息类 */

    YS_LOG_MAINTYPE_BUTT                  /* 无效值 */
}YS_LOG_MAINTYPE_E;

/* 日志次类型ID 列表 */
typedef enum 
{
    YS_LOG_ALL_SUB_TYPES = 0x0101,                 /*  所有信息类日志*/

    /* 信息类日志*/
    YS_LOG_MSG_HDD_INFO       = 300,               /* 硬盘信息 */
    YS_LOG_MSG_SMART_INFO     = 301,               /* S.M.A.R.T信息 */
    YS_LOG_MSG_REC_OVERDUE    = 302,               /* 过期录像删除 */
    YS_LOG_MSG_PIC_REC_OVERDUE = 303,              /* 过期图片文件删除 */

    /* 告警类日志的子类型日志ID */
    YS_LOG_ALARM_MOTION_DETECT = 350,              /* 移动侦测告警 */
    YS_LOG_ALARM_MOTION_DETECT_RESUME = 351,       /* 移动侦测告警恢复 */
    YS_LOG_ALARM_VIDEO_LOST = 352,                 /* 视频丢失告警 */
    YS_LOG_ALARM_VIDEO_LOST_RESUME = 353,          /* 视频丢失告警恢复 */
    YS_LOG_ALARM_VIDEO_TAMPER_DETECT = 354,        /* 遮挡侦测告警 */
    YS_LOG_ALARM_VIDEO_TAMPER_RESUME = 355,        /* 遮挡侦测告警恢复 */
    YS_LOG_ALARM_INPUT_SW = 356,                   /* 输入开关量告警 */
    YS_LOG_ALARM_INPUT_SW_RESUME = 357,            /* 输入开关量告警恢复 */
    YS_LOG_ALARM_IPC_ONLINE = 358,                 /* IPC 设备上线 */
    YS_LOG_ALARM_IPC_OFFLINE = 359,                /* IPC 设备下线 */

    /* 异常类日志的子类型日志ID */
    YS_LOG_EXCEP_DISK_ONLINE = 400,                /* 磁盘上线 */
    YS_LOG_EXCEP_DISK_OFFLINE = 401,               /* 磁盘下线 */
    YS_LOG_EXCEP_DISK_ERR = 402,                   /* 磁盘异常 */
    YS_LOG_EXCEP_STOR_ERR = 403,                   /* 存储错误 */
    YS_LOG_EXCEP_STOR_ERR_RECOVER = 404,           /* 存储错误恢复 */
    YS_LOG_EXCEP_STOR_DISOBEY_PLAN = 405,          /* 未按计划存储 */
    YS_LOG_EXCEP_STOR_DISOBEY_PLAN_RECOVER = 406,  /* 未按计划存储恢复 */
    YS_LOG_EXCEP_ILLEGAL_ACCESS = 407,             /* 非法访问 */
    YS_LOG_EXCEP_IP_CONFLICT = 408,                /* IP地址冲突 */
    YS_LOG_EXCEP_NET_BROKEN = 409,                 /* 网络断开 */
    YS_LOG_EXCEP_PIC_REC_ERR = 410,                /* 抓图出错,获取图片文件失败 */
    YS_LOG_EXCEP_VIDEO_EXCEPTION = 411,            /* 视频输入异常(只针对模拟通道) */
    YS_LOG_EXCEP_VIDEO_MISMATCH = 412,             /* 视频制式不匹配  */
    YS_LOG_EXCEP_RESO_MISMATCH = 413,              /* 编码分辨率和前端分辨率不匹配 */
    YS_LOG_EXCEP_TEMP_EXCE = 414,                  /* 温度异常 */

    /* 操作类日志的子类型日志ID */
    /* 业务类 */
    YS_LOG_OPSET_LOGIN = 450,                      /* 用户登录 */
    YS_LOG_OPSET_LOGOUT = 451,                     /* 注销登陆 */
    YS_LOG_OPSET_USER_ADD = 452,                   /* 用户添加 */
    YS_LOG_OPSET_USER_DEL = 453,                   /* 用户删除 */
    YS_LOG_OPSET_USER_MODIFY = 454,                /* 用户修改 */

    YS_LOG_OPSET_START_REC = 455,                  /* 开始录像 */
    YS_LOG_OPSET_STOP_REC = 456,                   /* 停止录像 */
    YS_LOG_OPSETR_PLAY = 457,                      /* 回放 */
    YS_LOG_OPSET_DOWNLOAD = 458,                   /* 下载 */
    YS_LOG_OPSET_PTZCTRL = 459,                    /* 云台控制 */
    YS_LOG_OPSET_PREVIEW = 460,                    /* 实况预览 */
    YS_LOG_OPSET_REC_TRACK_START = 461,            /* 轨迹录制开始 */
    YS_LOG_OPSET_REC_TRACK_STOP = 462,             /* 轨迹录制停止 */
    YS_LOG_OPSET_START_TALKBACK = 463,             /* 开始语音对讲 */
    YS_LOG_OPSET_STOP_TALKBACK = 464,              /* 停止语音对讲 */

    YS_LOG_OPSET_IPC_ADD = 465,                    /* 添加IPC */
    YS_LOG_OPSET_IPC_DEL = 466,                    /* 删除IPC */
    YS_LOG_OPSET_IPC_SET = 467,                    /* 设置IPC */

    /* 配置类 */
    YS_LOG_OPSET_DEV_BAS_CFG = 500,                /* 设备基本信息配置 */
    YS_LOG_OPSET_TIME_CFG = 501,                   /* 设备时间配置 */
    YS_LOG_OPSET_SERIAL_CFG = 502,                 /* 设备串口配置 */

    YS_LOG_OPSET_CHL_BAS_CFG = 503,                /* 通道基本配置 */
    YS_LOG_OPSET_CHL_NAME_CFG = 504,               /* 通道名称配置 */
    YS_LOG_OPSET_CHL_ENC_VIDEO = 505,              /* 视频编码参数配置 */
    YS_LOG_OPSET_CHL_DIS_VIDEO = 506,              /* 通道视频显示参数配置 */
    YS_LOG_OPSET_PTZ_CFG = 507,                    /* 云台配置 */
    YS_LOG_OPSET_CRUISE_CFG = 508,                 /* 巡航线路设置 */
    YS_LOG_OPSET_PRESET_CFG = 509,                 /* 预置点设置 */
    YS_LOG_OPSET_VIDPLAN_CFG = 510,                /* 录像计划配置 */
    YS_LOG_OPSET_MOTION_CFG = 511,                 /* 运动检测配置 */
    YS_LOG_OPSET_VIDLOSS_CFG = 512,                /* 视频丢失配置 */
    YS_LOG_OPSET_COVER_CFG = 513,                  /* 视频遮挡配置 */
    YS_LOG_OPSET_MASK_CFG = 514,                   /* 视频遮盖配置 */
    YS_LOG_OPSET_SCREEN_OSD_CFG = 515,             /* OSD叠加配置 */

    YS_LOG_OPSET_ALARMIN_CFG = 516,                /* 报警输入配置 */
    YS_LOG_OPSET_ALARMOUT_CFG = 517,               /* 报警输出配置 */
    YS_LOG_OPSET_ALARMOUT_OPEN_MAN = 518,          /* 手动开启报警输出,人机 */
    YS_LOG_OPSET_ALARMOUT_CLOSE_MAN = 519,         /* 手动关闭报警输出,人机 */

    YS_LOG_OPSET_ABNORMAL_CFG = 520,               /* 异常配置 */
    YS_LOG_OPSET_HDD_CFG = 521,                    /* 硬盘配置 */

    YS_LOG_OPSET_NET_IP_CFG = 522 ,                /* TCP/IP配置 */
    YS_LOG_OPSET_NET_PPPOE_CFG = 523,              /* PPPOE配置 */
    YS_LOG_OPSET_NET_PORT_CFG = 524,               /* 端口配置 */

    YS_LOG_OPSET_NET_DDNS_CFG = 525,               /**DDNS配置*/
    /* 维护类 */
    YS_LOG_OPSET_START_DVR = 600,                  /* 开机 */
    YS_LOG_OPSET_STOP_DVR = 601,                   /* 关机 */
    YS_LOG_OPSET_REBOOT_DVR = 602,                 /* 重启设备 */
    YS_LOG_OPSET_UPGRADE = 603,                    /* 版本升级 */
    YS_LOG_OPSET_LOGFILE_EXPORT = 604,             /* 导出日志文件 */
    YS_LOG_OPSET_CFGFILE_EXPORT = 605,             /* 导出配置文件 */
    YS_LOG_OPSET_CFGFILE_IMPORT = 606,             /* 导入配置文件 */
    YS_LOG_OPSET_CONF_SIMPLE_INIT = 607,           /* 简单恢复配置 */
    YS_LOG_OPSET_CONF_ALL_INIT = 608,              /* 恢复出厂配置 */
    YS_LOG_OPSET_CONF_AUTO_MAINTAIN = 609          /* 自动维护 */

} YS_LOG_SUBTYPE_E;

/* GMT时区枚举定义 */
typedef enum 
{
    YS_TIME_ZONE_1200M,    /** 比GMT时间延迟12小时 */
    YS_TIME_ZONE_1100M,    /** 比GMT时间延迟11小时 */
    YS_TIME_ZONE_1000M,    /** 比GMT时间延迟10小时 */
    YS_TIME_ZONE_0900M,    /** 比GMT时间延迟9小时 */
    YS_TIME_ZONE_0800M,    /** 比GMT时间延迟8小时*/
    YS_TIME_ZONE_0700M,    /** 比GMT时间延迟7小时 */
    YS_TIME_ZONE_0600M,    /** 比GMT时间延迟6小时 */
    YS_TIME_ZONE_0500M,    /** 比GMT时间延迟5小时 */
    YS_TIME_ZONE_0430M,    /** 比GMT时间延迟4个半小时 */
    YS_TIME_ZONE_0400M,    /** 比GMT时间延迟4小时 */
    YS_TIME_ZONE_0330M,    /** 比GMT时间延迟3个半小时 */
    YS_TIME_ZONE_0300M,    /** 比GMT时间延迟3小时 */
    YS_TIME_ZONE_0200M,    /** 比GMT时间延迟2小时 */
    YS_TIME_ZONE_0100M,    /** 比GMT时间延迟1小时 */
    YS_TIME_ZONE_0000,     /** 标准GMT时间 */
    YS_TIME_ZONE_0100P,    /** 比GMT时间提早1小时 */
    YS_TIME_ZONE_0200P,    /** 比GMT时间提早2小时 */
    YS_TIME_ZONE_0300P,    /** 比GMT时间提早3小时 */
    YS_TIME_ZONE_0330P,    /** 比GMT时间提早3个半小时 */
    YS_TIME_ZONE_0400P,    /** 比GMT时间提早4小时 */
    YS_TIME_ZONE_0430P,    /** 比GMT时间提早4个半小时 */
    YS_TIME_ZONE_0500P,    /** 比GMT时间提早5小时 */
    YS_TIME_ZONE_0530P,    /** 比GMT时间提早5个半小时 */
    YS_TIME_ZONE_0545P,    /** 比GMT时间提早5小时45分钟 */
    YS_TIME_ZONE_0600P,    /** 比GMT时间提早6小时 */
    YS_TIME_ZONE_0630P,    /** 比GMT时间提早6个半小时 */
    YS_TIME_ZONE_0700P,    /** 比GMT时间提早7小时 */
    YS_TIME_ZONE_0800P,    /** 比GMT时间提早8小时 */
    YS_TIME_ZONE_0900P,    /** 比GMT时间提早9小时 */
    YS_TIME_ZONE_0930P,    /** 比GMT时间提早9个半小时 */
    YS_TIME_ZONE_1000P,    /** 比GMT时间提早10小时 */
    YS_TIME_ZONE_1100P,    /** 比GMT时间提早11小时 */
    YS_TIME_ZONE_1200P,    /** 比GMT时间提早12小时 */
    YS_TIME_ZONE_1300P,    /** 比GMT时间提早13小时 */

    YS_TIME_ZONE_MAX,              /** GMT时间最大枚举值 */
    YS_TIME_ZONE_INVALID = 0xFFFF  /** 无效GMT时间枚举值 */
}YS_TIME_ZONE_E;

/* 设备校时类型枚举 */
typedef enum 
{
    YS_TIME_MODIFY_HANDLE = 0,                 /**< 手动修改时间 */
    YS_TIME_MODIFY_NTP,                        /**< NTP修改时间 */
    YS_TIME_MODIFY_SERVER,                     /**< 服务器修改时间 */

    YS_TIME_MODIFY_MAX,                        /**< 修改时间枚举最大值 */
    YS_TIME_MODIFY_INVALID = 0xFFFF            /**< 修改时间枚举无效值 */
}YS_TIME_MODIFY_E;

 /* 输出开关量运行模式 */
typedef enum 
{
    YS_AO_MODE_OPEN   = 1,    /**< 常开 */
    YS_AO_MODE_CLOSE  = 2,    /**< 常闭 */
    YS_AO_MODE_BUTT
}YS_AO_MODE_E;

#define YS_ERR_BASE                 50000      /*业务相关模块错误码基数, 该数值由BP统一分配 */

typedef enum tagUwareErrCode
{
    /* 内部业务处理错误 */
    YS_ERR_SYSCALL_FALIED             = (YS_ERR_BASE + 1),       /* 系统函数调用错误 */
    YS_ERR_CREATE_VMP_MSG_FAILED      = (YS_ERR_BASE + 2),       /* VMP消息创建失败 */
    YS_ERR_CHANNEL_OVER_SPEC          = (YS_ERR_BASE + 3),       /* 通道数超规格 */
    YS_ERR_NOT_SUPPORT_DIGITAL_DEVICE = (YS_ERR_BASE + 4),       /* 不支持数字设备 */

    /* 请求消息内容错误 */
    YS_ERR_RESCODE_NO_EXIST           = (YS_ERR_BASE + 100),     /* 资源编码不存在 */
    YS_ERR_DECODE_IE_FAILED           = (YS_ERR_BASE + 101),     /* 获取消息IE失败 */
    YS_ERR_ENCODE_IE_FAILED           = (YS_ERR_BASE + 102),     /* 添加消息IE失败 */
    YS_ERR_MSG_DATA_INVALID           = (YS_ERR_BASE + 103),     /* 消息内容错误 */

    /* 配置错误,包含调用驱动以及内部消息下发 */
    YS_ERR_CALL_DRV_COMMON            = (YS_ERR_BASE + 200),     /* 调用驱动通用失败 */
    YS_ERR_GET_CFG_FAILED             = (YS_ERR_BASE + 201),     /* 获取配置信息出错 */
    YS_ERR_SET_CFG_FAILED             = (YS_ERR_BASE + 202),     /* 设置配置信息出错 */
    YS_ERR_AREA_PARAM_ILLEGAL         = (YS_ERR_BASE + 203),     /* 区域参数非法 */
    YS_ERR_SYNC_CFG_FAILED            = (YS_ERR_BASE + 204),     /* 配置同步出错，用于系统重启时，同步保存配置 */
    YS_ERR_SIMPLE_RECOVER_CFG_FAILED  = (YS_ERR_BASE + 205),     /* 简单恢复配置出错 */
    YS_ERR_RECOVER_CFG_FAILED         = (YS_ERR_BASE + 206),     /* 恢复配置出错 */
    YS_ERR_FILE_CHECK_FAILED          = (YS_ERR_BASE + 207),     /* 文件校验出错 */
    YS_ERR_EXPORT_CFG_FAILED          = (YS_ERR_BASE + 208),     /* 导出配置出错 */
    YS_ERR_IMPORT_CFG_FAILED          = (YS_ERR_BASE + 209),     /* 导入配置出错 */
    YS_ERR_SET_DRV_FAILED             = (YS_ERR_BASE + 210),     /* 设置驱动信息出错 */
    YS_ERR_GET_DRV_FAILED             = (YS_ERR_BASE + 211),     /* 从驱动读取信息出错 */
    YS_ERR_QRY_LOG_NUM_FAILED         = (YS_ERR_BASE + 212),     /* 查询日志条数出错 */
    YS_ERR_READ_LOG_FAILED            = (YS_ERR_BASE + 213),     /* 读取具体日志时出错 */
    YS_ERR_SEND_CFG_MSG_FAILED        = (YS_ERR_BASE + 214),     /* 下发配置消息失败 */
    YS_ERR_CONF_FILE_NOEXIST          = (YS_ERR_BASE + 215),     /* 配置文件不存在 */
    YS_ERR_CONF_FILE_INVALID          = (YS_ERR_BASE + 216),     /* 配置文件不合法 */
    YS_ERR_CONF_FORBID_WRITE          = (YS_ERR_BASE + 217),     /* 禁止写入配置 */
    YS_ERR_CONF_NOEXIST               = (YS_ERR_BASE + 218),     /* 配置项不存在 */
    YS_ERR_COND_USER_NOEXIST          = (YS_ERR_BASE + 219),     /* 指定的用户不存在 */
    YS_ERR_COND_USER_EXIST            = (YS_ERR_BASE + 220),     /* 指定的用户已存在 */
    YS_ERR_UPDATE_VERSION_FAIL        = (YS_ERR_BASE + 221),     /* 设备升级，版本号不匹配 */
    YS_ERR_UPDATE_BOARDTYPE_NOT_MATCH = (YS_ERR_BASE + 222),     /* 设备升级，设备类型不匹配 */
    YS_ERR_UPDATE_OTHER_REASON        = (YS_ERR_BASE + 223),     /* 设备升级，可能申请内存或者其他原因失败 */
    uw_ERR_USER_FULL                  = (YS_ERR_BASE + 224),     /* 用户已满 */
    YS_ERR_CONFIGING                  = (YS_ERR_BASE + 225),     /* 配置下发过程中，请稍后 */
    /* Added by chenjian 00524, 2014-08-15 of 维护日志导出*/
    YS_ERR_MAINTAIN_EXPORT_RUNNING    = (YS_ERR_BASE + 226),     /* 维护信息正在导出 */

    /* 实况业务处理相关 */
    YS_ERR_LIVE_EXISTED               = (YS_ERR_BASE + 300),     /* 实况业务已经建立 */
    YS_ERR_LIVE_INPUT_NOT_READY       = (YS_ERR_BASE + 301),     /* 媒体流未准备就绪 */
    YS_ERR_LIVE_OUTPUT_BUSY           = (YS_ERR_BASE + 399),     /* 实况业务显示资源忙 */

    /* 回放业务处理相关 */
    YS_ERR_VOD_ABEND                  = (YS_ERR_BASE + 400),
    YS_ERR_VOD_END                    = (YS_ERR_BASE + 401),
    YS_ERR_VOD_CALL_DRV               = (YS_ERR_BASE + 402),
    YS_ERR_VOD_CB_NOT_FIND            = (YS_ERR_BASE + 403),
    YS_ERR_VOD_OVER_ABILITY           = (YS_ERR_BASE + 404),

    /* 云台控制相关 */
    YS_ERR_SET_PRESET_FAILED          = (YS_ERR_BASE + 500),     /* 预置位设置失败 */
    YS_ERR_QUERY_PRESET_FAILED        = (YS_ERR_BASE + 501),     /* 预置位查询失败 */
    YS_ERR_QUERY_TRACK_FAILED         = (YS_ERR_BASE + 502),     /* 轨迹查询失败 */
    YS_ERR_START_RECORD_TRACK_FAILED  = (YS_ERR_BASE + 503),     /* 开始录制轨迹失败 */
    YS_ERR_STOP_RECORD_TRACK_FAILED   = (YS_ERR_BASE + 504),     /* 结束录制轨迹失败 */
    YS_ERR_QUERY_CRUISE_FAILED        = (YS_ERR_BASE + 505),     /* 巡航线路查询失败 */
    YS_ERR_SET_CRUISE_FAILED          = (YS_ERR_BASE + 506),     /* 巡航线路设置失败 */
    YS_ERR_PTZ_COMMAND_FAILED         = (YS_ERR_BASE + 507),     /* 云台操作失败 */
    YS_ERR_PRESET_IN_CRUISE           = (YS_ERR_BASE + 508),     /* 预置位在巡航线路中使用, 无法删除 */

    /* RM模块 */
    YS_ERR_IPC_CHAN_EXISTED           = (YS_ERR_BASE + 600),     /* 绑定的IPC通道已存在*/
    YS_ERR_IPC_CB_NOT_EXISTED         = (YS_ERR_BASE + 601),     /* IPC控制块不存在 */
    YS_ERR_IPC_NOT_ONLINE             = (YS_ERR_BASE + 602),
    /* Added by c00524 2014/7/23 for IVSD10113,即插即用二期 */
    YS_ERR_IPADDR_USE_UP              = (YS_ERR_BASE + 603),     /* IP地址耗尽 */

    /* 网络模块相关错误码 */
    YS_ERR_IPADDR_CONFLICT            = (YS_ERR_BASE + 700),     /* ip地址冲突 */
    YS_ERR_UPNP_PORT_EMPLOY           = (YS_ERR_BASE + 701),     /* 端口已占用 */
    YS_ERR_DOWNLOAD                   = (YS_ERR_BASE + 702),     /* 下载异常 */
    /* Begin: Added by maqiang/01512, 2014-11-29, for Email Function IVSD14293 */
    YS_ERR_TEST_EMAIL                 = (YS_ERR_BASE + 703),     /* 测试邮件失败 */
    /* End: Added by maqiang/01512, 2014-11-29, for Email Function IVSD14293 */

    /* 用户模块错误码 */
    YS_ERR_USER_NOT_ONLINE            = (YS_ERR_BASE + 800),     /* 用户不在线 */
    YS_ERR_USER_WRONG_PASSWD          = (YS_ERR_BASE + 801),     /* 用户密码错误 */
    YS_ERR_USER_NO_SUCH_USER          = (YS_ERR_BASE + 802),     /* 没有该用户 */
    YS_ERR_USER_NO_AUTH               = (YS_ERR_BASE + 803),     /* 用户无权限 */
    YS_ERR_USER_MAX_NUM               = (YS_ERR_BASE + 804),     /* 用户上限-不能再被添加 */
    YS_ERR_USER_EXIST                 = (YS_ERR_BASE + 805),     /* 用户已存在 */
    YS_ERR_USER_LOGIN_MAX_NUM         = (YS_ERR_BASE + 806),     /* 用户登录数已达上限 */
    YS_ERR_TALK_EXISTED               = (YS_ERR_BASE + 900),    /* 语音对讲已存在 */
    YS_ERR_TALK_NO_EXISTED            = (YS_ERR_BASE + 901),    /* 语音业务不存在 */
    YS_ERR_TALK_RESCODE_INVALID       = (YS_ERR_BASE + 902),    /* 语音对讲资源编码无效 */
    YS_ERR_TALK_FAILED                = (YS_ERR_BASE + 903),    /* 语音对讲失败 */
    YS_ERR_AUDIO_RES_USED_BY_TALK,                       		/* 语音资源已被对讲使用*/

    /* OAG模块相关错误码 */
    YS_ERR_OAG_STREAM_NOEXIST         = (YS_ERR_BASE + 1000),     /* 流不存在 */

    YS_ERR_OAG_CFG_CONTENT_INVALID    = (YS_ERR_BASE + 1011),     /* xml无效内容 */
    YS_ERR_OAG_CFG_ITEMNUM_ERR        = (YS_ERR_BASE + 1012),     /* 获取xml节点个数异常 */
    YS_ERR_OAG_CFG_ALARM_OVER_MAX     = (YS_ERR_BASE + 1013),     /* 告警配置已达最大规格 */

    YS_ERR_OAG_OPEN_DLL_FAILED        = (YS_ERR_BASE + 1020),     /* 加载DLL文件失败 */
    YS_ERR_OAG_CLOSE_DLL_FAILED       = (YS_ERR_BASE + 1021),     /* 卸载DLL文件失败 */
    YS_ERR_OAG_GET_FUNC_ADDR_FAILED   = (YS_ERR_BASE + 1022),     /* 获取函数地址失败 */
    YS_ERR_OAG_LOAD_SDK_FAILED        = (YS_ERR_BASE + 1023),     /* 加载SDK失败 */
    YS_ERR_OAG_INIT_SDK_FAILED        = (YS_ERR_BASE + 1024),     /* 初始化SDK失败 */

    YS_ERR_OAG_CREATE_OBJECT_FAILED   = (YS_ERR_BASE + 1030),     /* 创建对象失败 */
    YS_ERR_OAG_CONN_DEV_TIMEOUT       = (YS_ERR_BASE + 1031),     /* 连接设备超时 */
    YS_ERR_OAG_OPENDEVICE_FAILED      = (YS_ERR_BASE + 1032),     /* 打开设备失败 */
    YS_ERR_OAG_CLOSEDEVICE_FAILED     = (YS_ERR_BASE + 1033),     /* 关闭设备失败 */
    YS_ERR_OAG_SETCONF_FAILED         = (YS_ERR_BASE + 1034),     /* 配置参数失败 */
    YS_ERR_OAG_GETCONF_FAILED         = (YS_ERR_BASE + 1035),     /* 获取配置参数失败 */
    YS_ERR_OAG_UNKNOWN_ALRAM_TYPE     = (YS_ERR_BASE + 1036),     /* 不可识别告警类型 */
    YS_ERR_OAG_CLOSECHL_FAILED        = (YS_ERR_BASE + 1037),     /* 关闭通道失败 */
    YS_ERR_OAG_MISDATA                = (YS_ERR_BASE + 1038),     /* 数据内容错误 */
    YS_ERR_OAG_CREATE_CHANOBJ_FAILED  = (YS_ERR_BASE + 1039),     /* 根据通道类型创建对象失败 */
    YS_ERR_OAG_GET_HANDLE_FAILED      = (YS_ERR_BASE + 1040),     /* 获取句柄失败 */
    YS_ERR_OAG_UNKNOWN_PTZ_CMD        = (YS_ERR_BASE + 1041),     /* 不识别的巡航命令 */
    YS_ERR_OAG_OPEN_ALARM_FAILED      = (YS_ERR_BASE + 1042),     /* 打开告警失败 */
	/* Added by c00524 2014/8/26 for 交换口设备接入 */
    YS_ERR_OAG_SWITCH_DEV_UNCONN      = (YS_ERR_BASE + 1043),     /* 交换口设备未连接 */
	YS_ERR_OAG_SET_TIME_FAILED        = (YS_ERR_BASE + 1044),     /* 时间同步失败 */ /* Added by z00348 2014.10.9 for IVSD12560 */
    YS_ERR_OAG_GET_NOALARM            = (YS_ERR_BASE + 1045),     /* 没有告警信息 */ /* Added by c00524 2014/12/11 for IVSD11305,布防下发时主动查告警 */
    YS_ERR_MAX_BUTT

} UWARE_ERR_CODE_E;


#pragma pack(4)

typedef struct
{	
    unsigned short	u16MessageFlag;  /*固定值0xBCBC VMP消息标识，表示是一个VMP消息*/
    unsigned char   u8Version:4;     /*协议版本号，固定填写5*/
    unsigned char   u8SubVersion:4;  /*协议子版本号，固定填写0*/
    unsigned char   u8Type;          /*消息类型，0时表示请求消息，1时表示通知请求消息，2时表示OK响应，3时表示FAILED响应*/
    unsigned short  u16Command;      /*消息ID 每一个消息类型都有自己的消息ID，详见VMP文档 一个字节够了???*/
    unsigned short  u16Status;       /*响应原因码 UWARE_ERR_CODE_E  */
    unsigned int  	u32SeqNum;       /*高4位表示模块名，后28位表示顺序号；请求和响应消息序列号保持一致,使用VMP_MSG_SEQ_NUM生成*/
    unsigned int  	u32SrcModId;     /*源模块ID*/
    unsigned int  	u32DstModId;     /*目的模块ID*/
    unsigned int  	u32FromHandle;   /*发送者私有，响应消息中带回*/
    unsigned int  	u32ToHandle;     /*发送者私有，响应消息中带回*/
    unsigned int  	u32ContentLength;/*VMP消息总长度*/
    unsigned char   u8EndianType;    /*大小端类型*/
    unsigned char   u8Res1;          /*保留字段*/
    unsigned short  u16Res2;         /*保留字段*/
}ys_msg_head_t;

typedef struct
{
	ys_msg_head_t	head;
	unsigned char	data[YS_MSG_SIZE];
}ys_msg_t;

typedef struct
{
	ys_msg_head_t	head;
	unsigned char	data[YS_SEARCH_SIZE];
}ys_search_t;

typedef struct 
{
    unsigned short	u16IEID;                /*IEID*/
    unsigned short	u16BodyLen;             /*IE长度*/
}ys_ie_head_t;

typedef struct
{
    unsigned short  u16DomainType;       	/* 仅支持两个值,  见BP_IP_DOMAIN_TYPE_E  */
    unsigned short  be16Rsvd;           	/* 预留,保持4字节对齐 */

    union
    {
        unsigned int be32Ipv4Addr;      	/* For IPv4,保存的是二进制形式,为网络的大端序 */
        unsigned short abe16Ipv6Addr[8];  	/* For IPv6 */
    };
}ys_ip_addr_t;

/* 设备能力集 */
typedef struct 
{
    unsigned char 	u8DevType;              /* 设备类型 */
    unsigned char	u8AlarmInPortNum;       /* 报警输入个数 */
    unsigned char	u8AlarmOutPortNum;      /* 报警输出个数 */
    unsigned char 	u8DiskNum;              /* 硬盘个数 */
    unsigned char 	u8ChanNum;              /* 模拟通道个数 */
    unsigned char 	u8IPChanNum;            /* 最大数字通道个数 */
    unsigned char 	u8ZeroChanNum;          /* 虚拟通道个数 */
    unsigned char 	u8AudioInwithRecordNum; /* 随录输入音频个数 */
    unsigned char 	u8AudioInNum;           /* 独立输入音频个数 */
    unsigned char 	u8AudioOutNum;          /* 独立输出音频个数 */
    unsigned char 	u8Com232Num;            /* 串口232个数 */
    unsigned char 	u8Com422Num;            /* 串口422个数 */
    unsigned char 	u8Com485Num;            /* 串口485个数 */
    char szReserved[31];       /* 预留 */
}ys_dev_ability_t;

/** 业务资源编码 */
typedef struct 
{
    unsigned char	u8DevType;              /**< 设备类别,参考YS_DEV_TYPE_E */
    unsigned char	u8DevIndex;             /**< 设备序号，不同类别的设备分别排序 */
    unsigned char	u8MappingChlIndex;      /**< 映射后的视频通道序号,不涉及通道的设备编码填00,从1开始编号 */
    unsigned char	u8Reserved;             /**< 保留 */
    unsigned char	u8ResType;              /**< 资源类别,参考YS_RES_TYPE_E */
    unsigned char	u8SlotID;               /**< 内部使用，保留 */
    unsigned char	u8PortID;               /**< 内部使用，保留 */
    unsigned char	u8ChlID;                /**< 资源序号,从1开始编号 */
}ys_resource_code_t;

typedef struct
{
	unsigned char	ucTcpFlag;
	unsigned char	ucTcpchnl;
	unsigned short	usBodySize;
}ys_tcp_head_t;

typedef struct  
{ 
	unsigned short	usMsgId; 
	unsigned short	usLen; 
	unsigned short	usCmd; 
	unsigned char	aucSession[16]; 
	unsigned short	usReserved; 
}ys_play_session_t;

//////////////////////////////////////////////////////////////////////

#define YS_USER_NAME_LEN	20
#define YS_PASSWD_LEN		64

/* YS_IE_ID_USER_SIMPLE_INFO
 * 用户账号（用户名＋密码） */
typedef struct 
{
    ys_ie_head_t	stIEHdr;

    char            szUserName[YS_USER_NAME_LEN];
    char            szUserPasswd[YS_PASSWD_LEN];	/*需要使用MD5加密*/
}ys_ie_user_simple_t;

/* YS_IE_ID_IP_ADDRESS
 * IP地址（类型＋地址＋端口） */
typedef struct
{
	ys_ie_head_t	stIEHdr;
	ys_ip_addr_t	stIPAddress;
	unsigned short	u16Port;
	char			szReserved[2];
}ys_ie_ip_address_t;

/* YS_IE_ID_USER_LOGIN_TYPE
 * 用户登录类型 */
typedef struct 
{
    ys_ie_head_t	stIEHdr;

    unsigned char	u8LoginType;    /* 详见UW_USER_LOGIN_TYPE_E */
    char			szReserved[3];
}ys_ie_user_log_type_t;

/* YS_IE_ID_USER_LOG_INFO
 * 用户登录信息（用户名＋登录ID＋登录IP） */
typedef struct
{
	ys_ie_head_t	stIEHdr;
	char			szUserName[YS_USER_NAME_LEN];
	unsigned int 	u32UserLoginHandle;
	ys_ip_addr_t	stUserLoginIP;
}ys_ie_user_log_info_t;

/* YS_IE_ID_PORT_MAPPING
 * 客户端需要的端口信息 */
typedef struct 
{
    ys_ie_head_t	stIEHdr;

    unsigned short	u16SDKPort;
    unsigned short	u16LivePort;
    unsigned short	u16VODPort;
    char			szReserved[2];
}ys_ie_port_mapping_t;

/* YS_IE_ID_DEV_ABILITY_INFO
 * 设备能力集信息 */
typedef struct 
{
    ys_ie_head_t		stIEHdr;
    ys_dev_ability_t    stDevInfo;
}ys_ie_dev_ability_t;

/* YS_IE_ID_RESOURCE_CODE
 * 资源编号信息 */
typedef struct 
{
    ys_ie_head_t		stIEHdr;
    ys_resource_code_t	stResourceCode;
}ys_ie_resource_code_t;

/* YS_IE_ID_MEDIA_IP
 * 用于业务协商交互的收发流IP信息 */
typedef struct 
{
    ys_ie_head_t		stIEHdr;
    unsigned char       u8RecvSendFlag;
    unsigned char		u8Reserved1;
    unsigned short		u16Port;
    ys_ip_addr_t		stIPAddress;
}ys_ie_media_ip_t;

/* YS_IE_ID_MEDIA_TRANSPORT
 * 传输协议 */
typedef struct 
{
    ys_ie_head_t		stIEHdr;
    unsigned int		u32TransportProtocal;
}ys_ie_media_transport_t;

/* YS_IE_ID_MEDIA_VIDEO_STREAM_INDEX
 * 视频流索引 */
typedef struct 
{
    ys_ie_head_t		stIEHdr;
    unsigned int		u32StreamIndex;
}ys_ie_video_index_t;

/* YS_IE_ID_TASK_NO
 * 业务号 */
typedef struct 
{
    ys_ie_head_t		stIEHdr;
    unsigned int		u32Task_No;
}ys_ie_task_no_t;

/* YS_IE_ID_SESSION_ID
 * 引流报文中的SessionID */
typedef struct 
{
    ys_ie_head_t		stIEHdr;
    char				szSessionID[16];
}ys_ie_session_id_t;

/* YS_IE_ID_PTZ_COMMAND
 * 云台配置参数 */
typedef struct 
{
	ys_ie_head_t		stIEHdr;
	unsigned short		u16Command;
	unsigned char		u8Param1;
	unsigned char		u8Param2;
	char				szReserved[8];
}ys_ie_ptz_command_t;

/* YS_IE_ID_TIME_RANGE
 * 开始结束时间段 */
typedef struct
{
	ys_ie_head_t		stIEHdr;
	unsigned int		u32Begin;
	unsigned int		u32End;
}ys_ie_time_range_t;

/* YS_IE_ID_VOD_FILE
 * 录像段信息 */
typedef struct 
{
    ys_ie_head_t		stIEHdr;
    char	            szRecordFileName[64];
    unsigned int		u32RecordType;			//YS_RECORD_TYPE_E	
    unsigned int		u32Begin;
    unsigned int		u32End;
}ys_ie_vod_file_t;

/* YS_IE_ID_STORAGE_TYPE
 * 录像类型 */
typedef struct 
{
    ys_ie_head_t		stIEHdr;
	unsigned int		u32RecordType;			//YS_RECORD_TYPE_E	
}ys_ie_storage_type_t;

/* YS_IE_ID_PLAY_STATUS
 * 回放控制信息 */
typedef struct 
{
    ys_ie_head_t		stIEHdr;
    unsigned int		u32PlayTime;
    unsigned int		u32PlayStatus;		//YS_PBS_STATUS_E
}ys_ie_play_status_t;

/* YS_IE_ID_LOG_TYPE
 * 日志类型 */
typedef struct 
{
    ys_ie_head_t		stIEHdr;
    unsigned short      u16MainType;		//YS_LOG_MAINTYPE_E
    unsigned short      u16SubType;			//YS_LOG_SUBTYPE_E
}ys_ie_log_type_t;

#define YS_NAME_MAX_LEN		64

/* YS_IE_ID_OPT_LOG_INFO
 * 日志信息 */
typedef struct 
{
    ys_ie_head_t		stIEHdr;
    unsigned int		u32Time;
    unsigned short		u16MainType;		//YS_LOG_MAINTYPE_E
    unsigned short		u16SubType;			//YS_LOG_SUBTYPE_E
    unsigned char		acChannCode[8];
    unsigned char		szUserName[YS_NAME_MAX_LEN];
    ys_ip_addr_t 		ip_addr;
}ys_ie_log_info_t;

/* YS_IE_ID_MEDIA_AUDIO_STREAM_INFO
 * 音频参数 */
typedef struct 
{
    ys_ie_head_t		stIEHdr;
    unsigned int		u32SampleRate;
    unsigned int		u32AudioFormat;
    unsigned char		u8RecvSend;
    unsigned char		u8Reserved1;
    unsigned short		u16Reserved2;
}ys_ie_audio_info_t;

/* YS_IE_ID_DEV_TIME_ZONE_INFO
 * 设备时区信息 */
typedef struct 
{
    ys_ie_head_t		stIEHdr;
    unsigned int		bSummerTimeEnable;
    unsigned char		u8TimeZone;				//YS_TIME_ZONE_E
    unsigned char		u8Reserved1;
    unsigned short		u16Reserved2;
}ys_ie_time_zone_info_t;

/* YS_IE_ID_MODIFY_TIME_TYPE
 * 校时类型 */
typedef struct 
{
    ys_ie_head_t		stIEHdr;
    unsigned char		u8ModifyTimeType;		//YS_TIME_MODIFY_E
    char				szReserved[3];
}ys_ie_modify_time_t;

/* YS_IE_ID_DEV_NTP
 * NTP服务信息 */
typedef struct 
{
    ys_ie_head_t		stIEHdr;
    ys_ip_addr_t		stNTPServerIP;
    unsigned short		u16NTPPort;
    unsigned short		u16UpdateMin;
}ys_ie_ntp_t;

/* YS_IE_ID_DEV_TIME
 * 设备时间信息 */
typedef struct 
{
    ys_ie_head_t		stIEHdr;
    unsigned int		u32Time;
}ys_ie_time_t;

/* YS_IE_ID_DST
 * 设备夏令时信息 */
typedef struct
{
	ys_ie_head_t		stIEHdr;
    unsigned int		bIsDSTEnable;
	unsigned char		u8StartMonth;
	unsigned char		u8StartWeekofMonth;
	unsigned char		u8StartWeekDay;
	unsigned char		u8StartHour;
	unsigned char		u8EndMonth;
	unsigned char		u8EndWeekofMonth;
	unsigned char		u8EndWeekDay;
	unsigned char		u8EndHour;
	unsigned char		u8Period;
	unsigned char		reserve[3];
}ys_ie_dst_t;

/* YS_IE_ID_ALARM_OUTPUT_INFO
 * 输出开关量基本信息 */
typedef struct 
{
    ys_ie_head_t		stIEHdr;
    char				szAlarmOutputName[64];
	unsigned char		u8DefaultStatus;		//YS_AO_MODE_E
	char				szReserved[7];
	unsigned int		u32DurationSec;
}ys_ie_alarmout_t;

//////////////////////////////////////////////////////////////////////

//login 
//YS_MSG_ID_USER_LOGIN
typedef struct
{
	ys_ie_user_simple_t		user_simple;
	ys_ie_ip_address_t		ip_address;
	ys_ie_user_log_type_t	user_log_type;
}ys_msg_login_c2d_t;

typedef struct
{
	ys_ie_user_log_info_t	user_log_info;
	ys_ie_port_mapping_t	port_mapping;
	ys_ie_dev_ability_t		dev_ability;
}ys_msg_login_d2c_t;

//logout 
//YS_MSG_ID_USER_LOGOUT
typedef struct
{
	ys_ie_user_log_info_t	user_log_info;
	ys_ie_user_log_type_t	user_log_type;
}ys_msg_logout_c2d_t;

typedef struct
{
	
}ys_msg_logout_d2c_t;

//heartbeat 
//YS_MSG_ID_USER_KEEPLIVE
typedef struct
{
	ys_ie_user_log_info_t	user_log_info;
}ys_msg_heartbeat_c2d_t;

typedef struct
{
	
}ys_msg_heartbeat_d2c_t;

//start rts 
//YS_MSG_ID_START_LIVE
typedef struct
{
	ys_ie_user_log_info_t	user_log_info;
	ys_ie_resource_code_t	resource_code;
	ys_ie_media_ip_t		media_ip;
	ys_ie_media_transport_t	media_transport;
	ys_ie_video_index_t		video_index;
}ys_msg_startrts_c2d_t;

typedef struct
{
	ys_ie_media_ip_t		media_ip;
	ys_ie_task_no_t			task_no;
	ys_ie_session_id_t		session_id;
}ys_msg_startrts_d2c_t;

//stop rts 
//YS_MSG_ID_STOP_LIVE
typedef struct
{
	ys_ie_user_log_info_t	user_log_info;
	ys_ie_task_no_t			task_no;
}ys_msg_stoprts_c2d_t;

typedef struct
{
	
}ys_msg_stoprts_d2c_t;

//rts data
typedef struct  
{ 
	ys_tcp_head_t			stHeader; 
	ys_play_session_t		stPlaySession; 
}ys_msg_play_session_tcp_t;

//ptz
//YS_MSG_ID_PTZ_COMMAND
typedef struct
{
	ys_ie_user_log_info_t	user_log_info;
	ys_ie_resource_code_t	resource_code;
	ys_ie_ptz_command_t		ptz_command;
}ys_msg_ptz_c2d_t;

typedef struct
{
	
}ys_msg_ptz_d2c_t;


//searchrecords 
//YS_MSG_ID_VOD_QRY 
typedef struct
{
	ys_ie_user_log_info_t	user_log_info;
	ys_ie_resource_code_t	resource_code;
	ys_ie_time_range_t		time_range;
//	ys_ie_storage_type_t	storage_type;
}ys_msg_searchrecords_c2d_t;

#define YS_VOID_FILE_NUM		128

typedef struct
{
	ys_ie_vod_file_t		stAr_vod_file[YS_VOID_FILE_NUM];
}ys_msg_searchrecords_d2c_t;

//startpbs 
//YS_MSG_ID_VOD_OPEN 
typedef struct
{
	ys_ie_user_log_info_t	user_log_info;
	ys_ie_resource_code_t	resource_code;
	ys_ie_media_ip_t		media_ip;
	ys_ie_vod_file_t		vod_file;
}ys_msg_startpbs_c2d_t;

typedef struct
{
	ys_ie_task_no_t			task_no;
	ys_ie_media_ip_t		media_ip;
	ys_ie_session_id_t		session_id;
}ys_msg_startpbs_d2c_t;

//setpbs 
//YS_MSG_ID_VOD_PLAY 
typedef struct
{
	ys_ie_user_log_info_t	user_log_info;
	ys_ie_task_no_t			task_no;
	ys_ie_play_status_t		play_status;
}ys_msg_setpbs_c2d_t;

typedef struct
{
	
}ys_msg_setpbs_d2c_t;

//getpbs 
//YS_MSG_ID_VOD_GET_PLAYSTATUS 
typedef struct
{
	ys_ie_user_log_info_t	user_log_info;
	ys_ie_task_no_t 		task_no;
}ys_msg_getpbs_c2d_t;

typedef struct
{
	ys_ie_task_no_t 		task_no;
	ys_ie_play_status_t		play_status;
}ys_msg_getpbs_d2c_t;

//stoppbs 
//YS_MSG_ID_VOD_CLOSE 
typedef struct
{
	ys_ie_user_log_info_t	user_log_info;			/* 用户登录信息，必填 */
	ys_ie_task_no_t 		task_no;				/* 需要停止的语音对讲资源业务号 */
}ys_msg_stoppbs_c2d_t;

typedef struct
{
	
}ys_msg_stoppbs_d2c_t;


//searchlogs
//YS_MSG_ID_QUERY_LOG
typedef struct 
{
	ys_ie_user_log_info_t	user_log_info;			/* 用户登录信息，必填 */
	ys_ie_log_type_t		log_type;
    ys_ie_time_range_t		time_range;
}ys_msg_searchlogs_c2d_t;

#define YS_LOG_MAX_NUM			2048

typedef struct
{
	ys_ie_log_info_t		log_info[YS_LOG_MAX_NUM];/* 日志信息,响应IE *//* 日志条数，最多返回2000条 */
}ys_msg_searchlogs_d2c_t;

//starttalk
//YS_MSG_ID_START_TALK
typedef struct
{
	ys_ie_user_log_info_t	user_log_info;			/* 用户登录信息，必填 */
	ys_ie_resource_code_t	resource_code_in;		/* 输入音频资源编码，必填 */
	ys_ie_resource_code_t	resource_code_out;		/* 输出音频资源编码，必填 */
	ys_ie_media_ip_t		media_ip_send_remote;	/* 远端发流端IP地址和端口，必填 */
	ys_ie_media_ip_t		media_ip_recv_remote;	/* 远端收流端IP地址和端口，必填 */
	ys_ie_media_transport_t	media_transport;		/* 传输协议，必填 */
}ys_msg_starttalk_c2d_t;

typedef struct
{
	ys_ie_task_no_t			task_no;				/* 业务号，必填 */
	ys_ie_media_ip_t		media_ip_send_local;	/* 设备端发流端IP地址和端口，必填 */
	ys_ie_media_ip_t		media_ip_recv_local;	/* 设备端收流端IP地址和端口，必填 */
	ys_ie_audio_info_t		audio_info_in_local;	/* 设备端收流音频参数，必填 */
	ys_ie_audio_info_t		audio_info_out_local;	/* 设备端发流音频参数，必填 */
	ys_ie_session_id_t		session_id;				/* 引流报文 */
}ys_msg_starttalk_d2c_t;

//stoptalk
//YS_MSG_ID_STOP_TALK
typedef struct
{
	ys_ie_user_log_info_t	user_log_info;			/* 用户登录信息，必填 */
	ys_ie_task_no_t 		task_no;				/* 需要停止的语音对讲资源业务号 */
}ys_msg_stoptalk_c2d_t;

typedef struct
{
	
}ys_msg_stoptalk_d2c_t;

//gettime
//YS_MSG_ID_QUERY_TIME
typedef struct
{
	ys_ie_user_log_info_t	user_log_info;			/* 用户登录信息，必填 */
}ys_msg_gettime_c2d_t;

typedef struct
{
	ys_ie_time_zone_info_t	time_zone_info;			/** 时区信息IE---YS_TIME_ZONE_E */
	ys_ie_modify_time_t		modify_time;			/** 修改类型IE(手动、NTP、服务器同步)---YS_TIME_MODIFY_E */
	ys_ie_ntp_t				ntp;					/** NTP服务器IE */
	ys_ie_time_t			time;					/** 时间信息IE */
	ys_ie_dst_t				dst;					/** 夏令时 */
}ys_msg_gettime_d2c_t;

//settime
//YS_MSG_ID_SET_TIME
typedef struct
{
	ys_ie_user_log_info_t	user_log_info;			/* 用户登录信息，必填 */
	ys_ie_time_zone_info_t	time_zone_info;			/** 时区信息IE---YS_TIME_ZONE_E */
	ys_ie_modify_time_t		modify_time;			/** 修改类型IE(手动、NTP、服务器同步)---YS_TIME_MODIFY_E */
	ys_ie_ntp_t				ntp;					/** NTP服务器IE */
	ys_ie_time_t			time;					/** 时间信息IE */
	ys_ie_dst_t				dst;					/** 夏令时 */
}ys_msg_settime_c2d_t;

typedef struct
{

}ys_msg_settime_d2c_t;



//getAO2 YS_MSG_ID_GET_ALARM_OUTPUT_STATUS
typedef struct
{
	ys_ie_resource_code_t	resource_code;			/** 资源编码 Req*/
	ys_ie_alarmout_t		pstIEAlarmOutInfo;	
}ys_msg_getAO2_c2d_t;

//getAO YS_MSG_ID_QUERY_ALARM_OUTPUT
typedef struct
{
	ys_ie_user_log_info_t	user_log_info;			/* 用户登录信息，必填 */
	ys_ie_resource_code_t	resource_code;			/** 资源编码 Req*/
}ys_msg_getAO_c2d_t;

typedef struct
{
	ys_ie_alarmout_t		pstIEAlarmOutInfo;
}ys_msg_getAO_d2c_t;

//setAO YS_MSG_ID_SET_ALARM_OUTPUT
typedef struct
{
	ys_ie_user_log_info_t	user_log_info;			/* 用户登录信息，必填 */
	ys_ie_resource_code_t	resource_code;			/** 资源编码 Req*/
	ys_ie_alarmout_t		pstIEAlarmOutInfo;		/** 报警输出信息 Req*/
}ys_msg_setAO_c2d_t;

typedef struct
{

}ys_msg_setAO_d2c_t;

#pragma pack()


#endif

#if defined __cplusplus
}
#endif

