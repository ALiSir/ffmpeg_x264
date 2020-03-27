
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
(1) 添加NS-QH协议MOBIPC设备开始对讲和结束对讲时调用开关接口函数；
(2) 添加HDCCTV协议报警联动云录像、云录像回放功能、云配置功能；
(3) 修复HDCCTV-CLINET协议中未登陆时调用获取设备列表接口阻塞问题；
(4) 添加远程录像日期功能；
(5) 优化HDCCTV协议中回放远程速度过快问题；

	0x2222: 2015/06/08
(1) 同步兼容WIN32编译环境；
(2) 优化NS-QH协议，区分压力测试relay服务器和msgs服务器模式；
(3) 添加NS-XM协议；
(4) 添加HDCCTV协议中查询和设置WiFi参数、AV参数，录像参数；
(5) 添加NC-QH协议中搜索指定设备接口nc_searchOneDev();
(6) 添加HDCCTV协议中本地直连和云端互联的自动切换模式；
(7) 添加HDCCTV-CLIENT协议回调状态帧；
(8) 添加HDCCTV协议支持三层网络中的手机访问二层网络中的设备功能；
(9) 添加HDCCTV-CLIENT协议支持nc_loginUser()和nc_logoutUser()接口；
(10)修改HDCCTV-CLIENT协议和NC协议中设置参数动作为同步动作；
(11)优化HDCCTV-DEVICE协议，将心跳信息修改为异步模式；
(12)优化HDCCTV-SERVER协议，将线程池线程数增加到100，将设备数减少到1000；
(13)优化HDCCTV_SERVER程序在发送报警消息时CPU过高问题；
(14)优化HDCCTV-CLIENT协议，解决设备wifiap模式下没有网关导致搜索不到设备，
	无法拉流问题；
(15)添加HDCCTV-CLIENT协议中的超时状态帧和定位状态帧；
(16)修复NC-HY协议中获取通道数量崩溃问题；
(17)优化NC-QH协议和HDCCTV_CLIENT协议，在对讲通道被占用时回调状态帧；
(18)修复NC-HDCCTV协议中调用获取设备列表时密码错误返回网络错误问题；

	0x2221: 2015/05/22
(1) 修复HDCCTV协议中nc_getAbility_v2()无法获取dev_id问题；
(2) 修复HDCCTV协议中设备名中文乱码问题；
(3) 修复QPNS协议中无法推送Android客户端报警问题；
(4) 添加HDCCTV协议语音对讲功能、录像回放功能；
(5) 添加NS-QH协议；
(6) 优化NC-HY,NC-YS和NC-XM协议中设备掉线问题；
(7) 修复对讲流stream2对象未注销问题；
(8) 修复ffmpeg分析视频帧返回-1时，无法分析出宽高问题；
(9) 修复NC-HY,NC-YS和NC-XM协议中判断相同设备时没有比较用户名和密码问题；
(10)修复NC-YS协议中同步时间时没有设置夏令时问题；

	0x2220: 2015/05/07
(1) 重构HDCCTV-SERVER协议中poll逻辑和android对象状态处理机制；
(2) 添加HDCCTV-SERVER协议中对ios-idarling客户端支持；
(3) 添加HDCCTV协议中对消息转发服务器支持；

	0x2210: 2015/04/20
(1)	添加云模块: 获取注册验证码、注册用户、获取找回密码验证码、找回密码、
	修改密码、绑定/解绑设备、获取设备列表、开始云视频、停止云视频；
(2) 优化开始/停止云视频接口，支持邮箱+密码，手机号+密码登陆；
(3) 优化定位接口，在连接域名www.hdcctvddns.com失败之后，再次连接固定IP地址
	208.109.240.232；
(4) 优化搜索设备接口，改为多线程并行搜索，减少等待时间；
(5) 添加NC-XM协议，支持搜索设备、实时流和云台控制、录像搜索和录像回放、查询
	日志、语音对讲、同步时间、设置报警输出；
	其中查询日志接口调整为nc_searchLogs_v2；
(6) 删除FFMPEG模块中的打印消息；
(7) 优化NC-XM协议，在查询录像文件结果为0时不断线；
(8) 修复tool_json.h中json2查询name时字符重叠导致结果异常问题；
(9) 在HDCCTV-DEVICE协议中，上传实时流音视频帧时在帧头中添加通道类型和通道号；
(10)修复nc_searchLogs_v2()接口查询IPC日志为空的问题；
(11)在NC-XM协议中，添加支持G711U音频格式；
(12)修复tool_sysf_getsockname()函数中，由于参数port为NULL而崩溃问题；
(13)优化NC-XM协议，连续4秒没有接受到数据帧则上报结束帧；
(14)修复搜索设备接口中XM设备IP地址为255.255.255.127问题；
(15)统一版本信息和打包时间；

	0x2202: 2015/03/02
(1) 修改nc_getAlarms()输入参数no由服务器维护；
(2) 整理tool_sock_t对象API函数，将mcast和bcast从udp中分离出来；
(3) 在tool_sysf_initAddr()函数中判断ip是否ipv4，如果是则跳过getaddrinfo()；
(4) 修复tool_sysf_initAddr()函数错误之后tool_sock_item_t对象tick未更新问题；
(5) 修复报警消息过频部分推送失败问题；
(6) 解决QH-IPC实时流停止之后仍然停留在上一副画面的问题；
(7) 抛出nc_ParseSPS()接口，给上层APP检测H264视频帧的分辨率；

	0x2201: 2015/03/02
(1) 添加HDCCTV-QPNS模块，使用方法:
	a. 程序启动时调用nc_startQpns()，开启旗瀚推送通知服务QPNS;
	b. 添加设备时调用nc_getAbility_v2()，得到设备唯一标示
	    ptc_cfg_ability_t.ptc_ability_base_t.dev_id；
	c. 调用nc_getQpnsDeviceMaps()和nc_setQpnsDeviceMaps()，查询/设置单台设备
	   QPNS服务；
	d. 前端设备发送报警时，android通过nc_startQpns()接口的第三个参数回调函数
	   上报给应用层，ios通过苹果推送通知服务上报给ios系统；
	e. ios调用nc_responseReadCount()将角标清零；
	f. 调用nc_testQpns()测试QPNS服务；
	g. 调用nc_stopQpns()停止QPNS服务；
(2) 重新整理makefile文件，删除多余模块；
(3) 添加ptc_frame_head_t帧头结构体中的padding_bytes字段；
(4) 添加QPNS服务器端t_alarm表自动清理功能；
(5) 添加QPNS服务器端t_qpns_map表自动清理功能；
(6) 添加nc_addQpnsDeviceMaps()接口和nc_delQpnsDeviceMaps()接口；
(7) 使用nc_getAlarms()替换nc_responseReadCount()接口；
(8) 添加nc_getAlarms_divide()
		nc_getQpnsDeviceMaps_divide()
        nc_setQpnsDeviceMaps_divide()
        nc_addQpnsDeviceMaps_divide()
        nc_delQpnsDeviceMaps_divide()
        nc_testQpns_divide()接口；
(9) 修正IPC设备查询网络参数接口中MAC地址不正确问题；       
(10)添加支持Mobile-IPC设备；
(11)添加TOOL-LOG工具将日志信息写入文件功能；

	0x2103: 2015/01/19
(1) 更改ffmpeg模块的变量名称、函数名称、文件名称，以防冲突；
(2) 解决NC-YS协议中解决只能播放单通道问题；
(3) 添加NC-YS协议控制报警输出口；
(4) 解决NC-YS协议添加设备时密码错误返回网络错误-7
(5) 解决NC-YS协议搜索日志提示网络错误，错误码为-6
(6) 添加NC-YS协议视频通道上报消息帧；
(7) 解决NC-YS协议远程录像回放可以搜到，但播不出来问题；
(8) 添加NC-YS-Cloud协议；
(9) 解决NC-YS协议实时视频回调接口的Handle参数错误问题；
(10)解决NC-YS协议中Diamond-NVR-Cloud无法自动重连问题；
(11)添加HDCCTV协议获取获取短信验证码，注册用户，修改密码，绑定设备接口；
(12)解决NC-YS协议无法搜索到YS-NVR设备问题；
(13)删除NC-YS协议中云台控制开始自动巡航和停止自动巡航功能；
(14)修改NC-YS协议中实时流和回放流回调消息帧的机制，解决停止实时流崩溃问题；
(15)修改NC-YS协议搜索录像文件机制，采用search替换msg，扩大消息包长度，最大
	支持1024个录像文件；
(16)解决NC-YS协议中对接通道被占用时开启对讲导致设备掉线问题；	

	0x2102: 2015/01/05
(1) 修改NC-HY协议中RTS和PBS登陆状态帧实现方式；
(2) 解决NC-HY协议中录像回放复现反复播放问题；
(3) 在能力结构体中添加网络库版本号和打包日期；
(4) 解决NS-NVR设备端设置网络参数等级时，码流位数字超过最大范围问题；
(5) 添加NS-IPC设备端G711U音频帧类型；
(6) 添加支持IPC设备的数据加密功能；
(7) 添加NC-YS协议；
(8) 修改NS-IPC设备中ns_dev_getRtsFrame()获取帧数据方式，使用拷贝帧头方式
	替代原有的使用指针方式；

	0x2101: 2014/11/12
(1) 添加线程、互斥锁、条件变量计数；
(2) 添加设备端qpnd模块；
(3) 添加设备端db模块和sqlite模块；
(4) 添加服务器端openssl模块；
(5) 添加服务器端APNS协议模块；
(6) 修改makefile文件，在Rules.mak文件中添加模块使能标志；
(7) 去除IPC设备的时区调整机制；
(8) 实现S7108/7104全功能；
(9) 修改DVR,NVR设备的对讲帧大小为840；
(10)修复NC-HY协议中nc_hy_run()函数计算cur_rts_enable错误；
(11)实现NC-HY协议中的查询和设置状态参数接口；
(12)优化NC-HY协议中的接受消息接口，兼容报警上报消息；
(13)修复NC-HY协议中无法实时回调登陆状态消息问题: 在nc_hy_dev_runCbRts()
	中注释rts->state==0部分代码；
(14)添加ffmpeg模块: 实现分析SPS和PPS功能；
(15)优化NC-HY协议: 在nc_hy_dev_doRecords()去除搜索录像文件结果中不符合
	开始结束时间段的录像文件；
(16)优化NC-HY协议: 修改tool_ptc_hy_hy2qh(), tool_ptc_hy_recvPbs()和
	tool_ptc_hy_recvRts()函数，添加双指针参数；修改tool_ptc_hy_recvPbs()
	函数，当出现空0xBB包时，返回0，当出现异常时间时，直接丢失剩余0xBB包；
(17)优化NC-HY协议，在远程录像回放超过截止时间时，上报255帧类型；
(18)优化NC-HY协议，录像回放时记忆回放位置，断线重连成功之后从断线位置开始
	播放；
(19)为消减内存使用量，减少NC-HY协议最大允许设备数量为8个；
(20)优化NC-HY协议，录像回放修改为动态开启关闭；每次开启录像回放时，以视频帧
	时间戳是否等于开始时间为条件，丢弃上一次回放遗留的帧数据；


	0x2100: 2014/11/04
(1) 将现有代码通过Win32编译；
(2) 修改Win32线程接口，使用POSIX Threads标准；
(3) 添加nginx的Fast-CGI模块；

	0x2015: 2014/10/20
(1) 优化HY协议超过最大连接数问题；
(2) 添加HY协议查询远程录像和远程录像回放接口；
(3) 添加HY协议查询日志信息协议；
(4) 添加HY协议云台控制接口；
(5) 添加HY协议语音对讲接口；
(6) 添加HY协议同步时间接口；
(7) 解决网络不通时，连接设备超时过长问题；
(8) 重构HY协议代码，所有服务公用一个登录状态；

	0x2014: 2014/09/11
(1) 修改播放缓冲丢帧机制，在写入缓冲区时丢帧取代读出时丢帧；
	只丢弃视频帧，不丢弃音频帧；
(2) 在实时流和回放流回调函数中，增加状态帧，类型定义参考PTC_FRAME_E；
(3) 加入HY-AHD-DVR网络库，在QH和HY网络库上再封一层接口层；
(4) HY接口实现: 初始化网络库、销毁网络库、搜索设备、查询设备通道数、
	获取能力集、开始实时音视频流、停止音视频流；
	不支持接口将返回NC_ERROR_NOSUPPORTED错误；
(5) 增加获取通道数接口nc_getChannelNum_v2();
(6) 修复IPC-GM8138设备开始实时流应用程序崩溃问题；
(7) 修改设备端语音对讲功能，当一秒内收到帧数大于帧率时，则丢弃帧数据；
(8) 减少客户端退出函数等待时长，单次调用最多等待20ms；

	0x2013: 2014/08/07
(1) 修改搜索功能实现方式，使用广播替代多播，兼容更多设备；
(2) 解决DVR-V4.0设备，出现小于NS_DEV_PBS_MIN_INTEVAL时间长度的录像文件
	之后，无法返回剩余文件问题；
(3) 修改IPC设备的搜索消息，当检测到DHCP打开时，重新查找动态IP地址；
(4) 修改搜索功能只能搜索一台设备问题；

	0x2012: 2014/08/07  DVR-V4.0.2
(1) 变更DVR-V4.0查询事件接口，添加视频丢失报警事件；
(2) 修改DVR-V4.0的时间和时区结构体，保证四字节对齐；
(3) 修改设备端透传命令接口，添加用户名参数和IP地址参数；
(4) 延迟多播启动时刻，修复DVR-V4.0设备加入多播组失败问题；
(5) 修改客户端设置码流等级接口没有写入通道问题；

	0x2011: 2014/07/29
(1) 对接DVR-V4.0配置时间和日期功能；
(2) 解决非阻塞connect函数内循环poll时输入参数未赋值问题；

	0x2010: 2014/07/28
(1) 添加获取默认参数接口;
(2) 合并linux和windows平台代码；

	0x2008: 2014/07/25
(1) 修改设置时间和日期协议；

	0x2007: 2014/07/23
(1) 增加IPC设备端时区修正机制； 
(2) 增加同步时间功能；
(3) 修复了ISS Mobile-V1.0.3版本的BUG;
(4) 优化客户端缓存数据，当缓存数据超过2倍预定数据时，采取丢帧机制；

	0x2006: 2014/07/10
(1) 修改DVR-v5.0将ptc_cfg_t的参数长度从10K增加到200K。
(2) 修改DVR-v5.0将设备端监听端口从5000改成从设备中获取；
(3) 修改DVR-v5.0将设备接口和网络库接口分开，分别列为ns.h 和 dev_func.h
(4) 增加客户端缓存数据功能；
(5) 修复DVR-v3.0设备端搜索日志没有报警信息问题，修复搜索日志信息接口中
内存未释放问题；

	0x2005: 2014/07/01
(1) 增加强制关键帧帧接口；
(2) 修改控制类消息逻辑；
(3) 增加DVR-v3.0设备端日志查询接口；
(4) 更新DVR-v3.0设备端网络参数结构体；
(5) 增加DVR-v3.0设备端录像回放拖动操作；
(6) 修正 客户端在连接不到设备时，从实时页面切换到其他页面，软件卡住问题；

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
	PTC_PTZ_CMD_STOP			=0,  //停止
	PTC_PTZ_CMD_LEFT,                //左
	PTC_PTZ_CMD_RIGHT,               //右
	PTC_PTZ_CMD_UP,                  //上
	PTC_PTZ_CMD_DOWN,                //下
	PTC_PTZ_CMD_LEFT_UP,             //左上
	PTC_PTZ_CMD_LEFT_DOWN,           //左下
	PTC_PTZ_CMD_RIGHT_UP,            //右上
	PTC_PTZ_CMD_RIGHT_DOWN,          //右下
	PTC_PTZ_CMD_NEAR,                //拉近
	PTC_PTZ_CMD_FAR,                 //调远
	PTC_PTZ_CMD_ZOOM_OUT,            //缩小
	PTC_PTZ_CMD_ZOOM_IN,             //放大
	PTC_PTZ_CMD_IRIS_OPEN,           //开启光圈
	PTC_PTZ_CMD_IRIS_CLOSE,          //关闭光圈
	PTC_PTZ_CMD_PRESET_SET,          //设置预置点
	PTC_PTZ_CMD_PRESET_GO,           //到第几个预置点
	PTC_PTZ_CMD_PRESET_DEL,          //删除预置点
	PTC_PTZ_CMD_CRUISE_CFG,          //设置巡航线,相当于执行Enter、Set、Leave三个命令, 暂不支持
	PTC_PTZ_CMD_ENTER_CURISE_MODE,   //进入巡航模式,此后可以设置巡航的预置点, 暂不支持
	PTC_PTZ_CMD_CRUISE_SET,          //设置巡航线的预置点, 暂不支持
	PTC_PTZ_CMD_LEAVE_CURISE_MODE,   //退出设置巡航, 暂不支持
	PTC_PTZ_CMD_CRUISE_RUN,          //选择一个巡航线进行巡航, 暂不支持
	PTC_PTZ_CMD_CRUISE_STOP,         //停止巡航, 暂不支持
	PTC_PTZ_CMD_CRUISE_DEL,          //删除巡航线, 暂不支持
	PTC_PTZ_CMD_TRACK_START,         //开始轨迹, 暂不支持
	PTC_PTZ_CMD_TRACK_STOP,          //停止轨迹, 暂不支持
	PTC_PTZ_CMD_TRACK_SET,           //开始保存轨迹, 暂不支持
	PTC_PTZ_CMD_TRACK_DEL,           //停止保存轨迹, 暂不支持
	PTC_PTZ_CMD_AUTO_SCAN_START,     //开始自动扫描
	PTC_PTZ_CMD_AUTO_SCAN_STOP,      //停止自动扫描
	PTC_PTZ_CMD_POP_PTZ_MENU,        //弹出云台自带菜单, 暂不支持
	PTC_PTZ_CMD_SET_AUXILIARY,       //打开辅助开关, 暂不支持
	PTC_PTZ_CMD_CLEAR_AUXILIARY,     //关闭辅助开关, 暂不支持

	PTC_PIR_CMD_BASE = 0xA0,		
	PTC_PIR_CMD_RECORD_SOUND_START,  //开始录制声音
	PTC_PIR_CMD_RECORD_SOUND_STOP,	 //停止录像声音
	PTC_PIR_CMD_PLAY_SOUND_START,    //开始播放声音
	PTC_PIR_CMD_PLAY_SOUND_STOP,     //停止播放声音

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
	PTC_FRAME_I,							//视频I帧
	PTC_FRAME_P,							//视频P帧
	PTC_FRAME_A,							//音频帧

	PTC_FRAME_INFO					= 100,	//信息帧

	PTC_FRAME_STREAM_RIGHT			= 235, 	//无权限
	PTC_FRAME_STREAM_VIDEO_BUSY		= 236,	//视频通道繁忙
	PTC_FRAME_STREAM_TIMEOUT		= 237,	//超时
	PTC_FRAME_STREAM_POSTION 		= 238,	//定位服务器
	PTC_FRAME_STREAM_BROKEN 		= 239,	//通信错误，断开连接
	PTC_FRAME_STREAM_WAIT			= 240,	//等待重连...
	PTC_FRAME_STREAM_CONNECT,				//连接设备/服务器...
	PTC_FRAME_STREAM_CONNECT_FAIL,			//连接设备失败
	PTC_FRAME_STREAM_LOGIN,					//登陆设备...
	PTC_FRAME_STREAM_LOGIN_FAIL,			//登陆设备失败
	PTC_FRAME_STREAM_AUTH_FAIL,				//用户认证失败
	PTC_FRAME_STREAM_MAX_CONNECT,			//达到最大连接数
	PTC_FRAME_STREAM_START,					//请求流媒体...
	PTC_FRAME_STREAM_START_FAIL,			//请求流媒体失败
	PTC_FRAME_STREAM_RUNNING,				//接收流媒体...
	PTC_FRAME_STREAM_NOVIDEO,				//该通道没有视频,250
	PTC_FRAME_STREAM_TALK_BUSY,				//对讲通道被占用
	PTC_FRAME_STREAM_LOCATION,				//搜索服务器
	PTC_FRAME_STREAM_LOCATION_FAIL,			//搜索服务器失败  
	PTC_FRAME_STREAM_DEVICE_OFFLINE,		//设备不在线
	
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
	PTC_CH_TYPE_LOCAL_RTS_MAIN	= 0x00,		//本地主码流通道
	TPC_CH_TYPE_LOCAL_RTS_SUB	= 0x01,		//本地子码流通道
	PTC_CH_TYPE_LOCAL_RTS_PHONE	= 0x02,		//本地手机码流通道
	PTC_CH_TYPE_LOCAL_ALARM_IN 	= 0x03,		//本地报警输入通道
	PTC_CH_TYPE_LOCAL_ALARM_OUT = 0x04,		//本地报警输出通道
	PTC_CH_TYPE_LOCAL_PBS 		= 0x05,		//本地回放通道
	PTC_CH_TYPE_LOCAL_TALK		= 0x06, 	//本地对讲通道
	PTC_CH_TYPE_DIGIT_NONE		= 0x10,
	PTC_CH_TYPE_DIGIT,						//数字通道(远程IPC)
	
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
	//系统控制(Control)  旗瀚日志类型
	PTC_LOG_SYSTEM_CTRL		= 0,
	PTC_LOG_BOOT,							//系统开机
	PTC_LOG_SHUTDOWN,						//系统关机
	PTC_LOG_REBOOT,							//系统重启
	PTC_LOG_FORMAT_SUCC,					//格式化磁盘成功
	PTC_LOG_FORMAT_FAIL,					//格式化磁盘失败
	PTC_LOG_UPGRADE_SUCC,					//升级成功
	PTC_LOG_UPGRADE_FAIL,					//升级失败
	PTC_LOG_CLEAR_ALARM,					//清除报警
	PTC_LOG_OPEN_ALARM,						//开启报警
	PTC_LOG_MANUAL_START,					//开启手动录像
	PTC_LOG_MANUAL_STOP,					//停止手动录像
	PTC_LOG_PTZ_ENTER,						//开始云台控制
	PTC_LOG_PTZ_CTRL,						//云台操作
	PTC_LOG_PTZ_EXIT,						//结束云台控制
	PTC_LOG_AUDIO_CH_CHANGE,				//改变现场音频通道
	PTC_LOG_VOLUME_ADJUST,					//调节音量
	PTC_LOG_MUTE_ENABLE,					//开启静音
	PTC_LOG_MUTE_DISENABLE,					//关闭静音
	PTC_LOG_DWELL_ENABLE,					//开启轮循
	PTC_LOG_DWELL_DISENABLE,				//关闭轮循
	PTC_LOG_LOG_IN,							//登录
	PTC_LOG_LOG_OFF,						//登出
	PTC_LOG_CHANGE_TIME,					//修改系统时间
	PTC_LOG_MANUAL_SNAP_SUCC,               //手动抓图成功
	PTC_LOG_MANUAL_SNAP_FAIL,               //手动抓图失败 25

	//系统配置(Setup)
	PTC_LOG_CONFIG			= 40,
	PTC_LOG_CHGE_VIDEO_FORMAT,				//改变视频制式
	PTC_LOG_CHGE_VGA_RESOLUTION,			//改变显示器分辨率
	PTC_LOG_CHGE_LANGUAGE,					//调整语言
	PTC_LOG_CHGE_NET_USER_NUM,				//调整网络用户数目
	PTC_LOG_CHGE_TIME_ZONE,					//调整时区
	PTC_LOG_NTP_MANUAL,						//手动网络校时
	PTC_LOG_NTP_ON,							//开启自动网络校时
	PTC_LOG_NTP_OFF,						//关闭自动网络校时
	PTC_LOG_CHGE_NTP_SERVER,				//修改网络时间服务器地址
	PTC_LOG_CHGE_DST,						//调整夏令时设置
	PTC_LOG_PASSWD_ON,						//开启操作密码
	PTC_LOG_PASSWD_OFF,						//关闭操作密码
	PTC_LOG_CHGE_CAM_NAME,					//调整通道名称
	PTC_LOG_MODIFY_COLOR,					//调整图像色彩
	PTC_LOG_CHGE_HOST_MONITOR,				//调整主监视器画面设置
	PTC_LOG_CHGE_SPOT,						//调整辅助输出画面设置
	PTC_LOG_CHGE_OSD,						//调整字符叠加设置
	PTC_LOG_CHGE_LOCAL_ENCODE,				//调整录像流编码参数
	PTC_LOG_CHGE_REC_VIDEO_SWITCH,			//调整录像开关设置
	PTC_LOG_CHGE_REC_AUDIO_SWITCH,			//调整录制音频开关设置
	PTC_LOG_CHGE_REC_REDU_SWITCH,			//调整冗余录像开关设置
	PTC_LOG_CHGE_REC_PRE_TIME,				//调整景前录像时间
	PTC_LOG_CHGE_REC_POST_TIME,				//调整景后录像时间
	PTC_LOG_CHGE_REC_HOLD_TIME,				//调整录像数据过期时间
	PTC_LOG_CHGE_SCH_SCHEDULE,				//调整定时录像计划
	PTC_LOG_CHGE_SCH_MOTION,				//调整移动侦测录像计划
	PTC_LOG_CHGE_SCH_ALARM,					//调整报警录像计划
	PTC_LOG_CHGE_SENSOR_SWITCH,				//调整报警输入开关设置
	PTC_LOG_CHGE_SENSOR_TYPE,				//调整报警输入设备类型
	PTC_LOG_CHGE_SENSOR_TRIGGER,			//调整报警输入处理方式设置
	PTC_LOG_CHGE_SENSOR_SCH,				//调整报警输入侦测计划
	PTC_LOG_CHGE_MOTION_SWITCH,				//调整移动侦测开关设置
	PTC_LOG_CHGE_MOTION_SENS,				//调整移动侦测灵敏度
	PTC_LOG_CHGE_MOTION_AREA,				//调整移动侦测区域设置
	PTC_LOG_CHGE_MOTION_TRIGGER,			//调整移动侦测处理方式
	PTC_LOG_CHGE_MOTION_SCH,				//调整移动侦测计划
	PTC_LOG_CHGE_VL_TRIGGER,				//调整视频丢失处理方式设置
	PTC_LOG_CHGE_RELAY_SWITCH,				//调整报警输出开关设置
	PTC_LOG_CHGE_RELAY_SCH,					//调整报警输出计划
	PTC_LOG_BUZZER_ON,						//开启声音报警设备
	PTC_LOG_BUZZER_OFF,						//关闭声音报警设备
	PTC_LOG_CHGE_BUZZER_SCH,				//调整声音报警计划
	PTC_LOG_CHGE_HTTP_PORT,					//修改HTTP服务器端口
	PTC_LOG_CHGE_SER_PORT,					//修改网络服务器端口
	PTC_LOG_CHGE_IP,						//设置网络地址
	PTC_LOG_DHCP_SUCC,						//自动获取网络地址成功
	PTC_LOG_DHCP_FAIL,						//自动获取网络地址失败
	PTC_LOG_CHGE_PPPOE,						//设置PPPoE
	PTC_LOG_CHGE_DDNS,						//设置DDNS
	PTC_LOG_NET_STREAM_CFG,					//调整网络流编码设置
	PTC_LOG_CHGE_SERIAL,					//调整云台串口设置
	PTC_LOG_PRESET_MODIFY,					//调整预置点
	PTC_LOG_CRUISE_MODIFY,					//调整巡航线
	PTC_LOG_TRACK_MODIFY,					//调整轨迹
	PTC_LOG_USER_ADD,						//增加用户
	PTC_LOG_USER_MODIFY,					//调整用户权限
	PTC_LOG_USER_DELETE,					//删除用户
	PTC_LOG_CHANGE_PASSWD,					//修改用户密码
	PTC_LOG_LOAD_DEFAULT,					//恢复默认配置
	PTC_LOG_IMPORT_CONFIG,					//导入配置
	PTC_LOG_EXPORT_CONFIG,					//导出配置
	PTC_LOG_CHGE_IMAGE_MASK,				//图像遮挡
	PTC_LOG_RECYCLE_REC_ON,					//开启循环录像
	PTC_LOG_RECYCLE_REC_OFF,				//关闭循环录像
	PTC_LOG_CHGE_DISK_ALARM,				//调整磁盘报警空间
	PTC_LOG_CHGE_SEND_EMAIL,				//设置Email 发送人信息
	PTC_LOG_CHGE_RECV_EMAIL,				//设置Email 接收人信息
	PTC_LOG_CHGE_SNAP_SETTING,             	//调整抓图配置 108


	// IPC日志
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

	PTC_LOG_XARINA_START,					//暂时无用 179
	PTC_LOG_XARINA_STOP,					//暂时无用
	PTC_LOG_TA_DEFOUSE_START,				//虚焦报警开始 181
	PTC_LOG_TA_GLOBAL_START,				//遮挡报警开始 182
	PTC_LOG_VMF1_START,						//入侵报警开始 183
	PTC_LOG_VMF2_START,						//边界报警开始 184
	PTC_LOG_VMF3_START,						//物体丢失报警开始 185
	PTC_LOG_TA_DEFOUSE_STOP,				//虚焦报警结束
	PTC_LOG_TA_GLOBAL_STOP,					//遮挡报警结束
	PTC_LOG_VMF1_STOP,						//入侵报警结束
	PTC_LOG_VMF2_STOP,						//边界报警结束
	PTC_LOG_VMF3_STOP,						//物体丢失报警结束(190)

	PTC_LOG_VIDEOLOSS_START = 196,
	PTC_LOG_VIDEOLOSS_STOP = 197,
	PTC_LOG_VIDEOCOVER_START = 198,
	PTC_LOG_VIDEOCOVER_STOP = 199,

	//录像回放(Playback)
	PTC_LOG_PLAYBACK		= 200,
	PTC_LOG_PLAYBACK_PLAY,					//播放
	PTC_LOG_PLAYBACK_PAUSE,					//暂停
	PTC_LOG_PLAYBACK_RESUME,				//恢复播放
	PTC_LOG_PLAYBACK_FF,					//快进
	PTC_LOG_PLAYBACK_REW,					//快退
	PTC_LOG_PLAYBACK_STOP,					//停止
	PTC_LOG_PLAYBACK_NEXT_SECTION,			//下一段
	PTC_LOG_PLAYBACK_PREV_SECTION,			//上一段 208

	//数据备份(Backup)
	PTC_LOG_BACKUP			= 211,
	PTC_LOG_BACKUP_START,					//开始备份
	PTC_LOG_BACKUP_COMPLETE,				//备份完成
	PTC_LOG_BACKUP_CANCEL,					//放弃备份
	PTC_LOG_BACKUP_FAIL,					//备份失败 215

	//录像检索(Search)
	PTC_LOG_SEARCH			= 218,
	PTC_LOG_SEARCH_TIME,					//按时间检索
	PTC_LOG_SEARCH_EVENT,					//按事件检索
	PTC_LOG_SEARCH_FILE_MAN,				//文件管理
	PTC_LOG_DELETE_FILE,					//删除文件
	PTC_LOG_LOCK_FILE,						//锁定文件
	PTC_LOG_UNLOCK_FILE,					//解锁文件
	PTC_LOG_DELETE_PICTURE,                	//删除图片
	PTC_LOG_LOCK_PICTURE,                  	//锁定图片
	PTC_LOG_UNLOCK_PICTURE,                	//解锁图片 227

	//查看信息(View information)
	PTC_LOG_VIEW_INFO		= 230,
	PTC_LOG_VIEW_SYSTEM,					//查看系统信息
	PTC_LOG_VIEW_EVENT,						//查看事件
	PTC_LOG_VIEW_LOG,						//查看日志
	PTC_LOG_VIEW_NETWORK,					//查看网络状态
	PTC_LOG_VIEW_ONLINE_USER,				//查看在线用户
	PTC_LOG_VIEW_EXPORT_LOG,				//导出日志
	PTC_LOG_VIEW_EXPORT_EVENT,				//导出事件 237 

	//异常信息(Error)
	PTC_LOG_ERROR_INFO		= 240,
	PTC_LOG_IP_CONFLICT,					//网络地址冲突
	PTC_LOG_NETWORK_ERR,					//网络异常
	PTC_LOG_DDNS_ERR,						//DDNS错误
	PTC_LOG_DISK_IO_ERR,					//磁盘读写错误
	PTC_LOG_UNKNOWN_OFF,					//异常断电
	PTC_LOG_UNKNOWN_ERR,					//未知错误	
	PTC_LOG_ERR_9A9A9A9A,					//未知错误	
	PTC_LOG_ERR_9A000001,					//未知错误	
	PTC_LOG_ERR_9A000002,					//未知错误	
	PTC_LOG_DISK_WARNING,                	//磁盘衰减报警
	PTC_LOG_DISK_DISCONNECT,                //磁盘掉线报警 251
	//LOG_TYE_DISK_NOTFIND,					//开机无硬盘	
	PTC_LOG_DIAMOND_NVR 	= 255,			//Diamond-NVR 日志
	
//	PTC_LOG_YS = 299,				   				/*  宇视所有信息类日志*/
	/* 信息类日志*/
	PTC_LOG_MSG_HDD_INFO       = 300,               /* 硬盘信息 */
	PTC_LOG_MSG_SMART_INFO     = 301,               /* S.M.A.R.T信息 */
	PTC_LOG_MSG_REC_OVERDUE    = 302,               /* 过期录像删除 */
	PTC_LOG_MSG_PIC_REC_OVERDUE = 303,              /* 过期图片文件删除 */
    PTC_LOG_MSG_IPC_ONLINE = 304,                   /* IPC上线 */
    PTC_LOG_MSG_IPC_OFFLINE = 305,                  /* IPC离线 */

	/* 告警类日志的子类型日志ID */
	PTC_LOG_ALARM_MOTION_DETECT = 350,              /* 移动侦测告警 */
	PTC_LOG_ALARM_MOTION_DETECT_RESUME = 351,       /* 移动侦测告警恢复 */
	PTC_LOG_ALARM_VIDEO_LOST = 352,                 /* 视频丢失告警 */
	PTC_LOG_ALARM_VIDEO_LOST_RESUME = 353,          /* 视频丢失告警恢复 */
	PTC_LOG_ALARM_VIDEO_TAMPER_DETECT = 354,        /* 遮挡侦测告警 */
	PTC_LOG_ALARM_VIDEO_TAMPER_RESUME = 355,        /* 遮挡侦测告警恢复 */
	PTC_LOG_ALARM_INPUT_SW = 356,                   /* 输入开关量告警 */
	PTC_LOG_ALARM_INPUT_SW_RESUME = 357,            /* 输入开关量告警恢复 */
	PTC_LOG_ALARM_IPC_ONLINE = 358,                 /* IPC 设备上线 */
	PTC_LOG_ALARM_IPC_OFFLINE = 359,                /* IPC 设备下线 */

	/* 异常类日志的子类型日志ID */
	PTC_LOG_EXCEP_DISK_ONLINE = 400,                /* 磁盘上线 */
	PTC_LOG_EXCEP_DISK_OFFLINE = 401,               /* 磁盘下线 */
	PTC_LOG_EXCEP_DISK_ERR = 402,                   /* 磁盘异常 */
	PTC_LOG_EXCEP_STOR_ERR = 403,                   /* 存储错误 */
	PTC_LOG_EXCEP_STOR_ERR_RECOVER = 404,           /* 存储错误恢复 */
	PTC_LOG_EXCEP_STOR_DISOBEY_PLAN = 405,          /* 未按计划存储 */
	PTC_LOG_EXCEP_STOR_DISOBEY_PLAN_RECOVER = 406,  /* 未按计划存储恢复 */
	PTC_LOG_EXCEP_ILLEGAL_ACCESS = 407,             /* 非法访问 */
	PTC_LOG_EXCEP_IP_CONFLICT = 408,                /* IP地址冲突 */
	PTC_LOG_EXCEP_NET_BROKEN = 409,                 /* 网络断开 */
	PTC_LOG_EXCEP_PIC_REC_ERR = 410,                /* 抓图出错,获取图片文件失败 */
	PTC_LOG_EXCEP_VIDEO_EXCEPTION = 411,            /* 视频输入异常(只针对模拟通道) */
	PTC_LOG_EXCEP_VIDEO_MISMATCH = 412,             /* 视频制式不匹配  */
	PTC_LOG_EXCEP_RESO_MISMATCH = 413,              /* 编码分辨率和前端分辨率不匹配 */
	PTC_LOG_EXCEP_TEMP_EXCE = 414,                  /* 温度异常 */

	/* 操作类日志的子类型日志ID */
	/* 业务类 */
	PTC_LOG_OPSET_LOGIN = 450,                      /* 用户登录 */
	PTC_LOG_OPSET_LOGOUT = 451,                     /* 注销登陆 */
	PTC_LOG_OPSET_USER_ADD = 452,                   /* 用户添加 */
	PTC_LOG_OPSET_USER_DEL = 453,                   /* 用户删除 */
	PTC_LOG_OPSET_USER_MODIFY = 454,                /* 用户修改 */

	PTC_LOG_OPSET_START_REC = 455,                  /* 开始录像 */
	PTC_LOG_OPSET_STOP_REC = 456,                   /* 停止录像 */
	PTC_LOG_OPSETR_PLAY = 457,                      /* 回放 */
	PTC_LOG_OPSET_DOWNLOAD = 458,                   /* 下载 */
	PTC_LOG_OPSET_PTZCTRL = 459,                    /* 云台控制 */
	PTC_LOG_OPSET_PREVIEW = 460,                    /* 实况预览 */
	PTC_LOG_OPSET_REC_TRACK_START = 461,            /* 轨迹录制开始 */
	PTC_LOG_OPSET_REC_TRACK_STOP = 462,             /* 轨迹录制停止 */
	PTC_LOG_OPSET_START_TALKBACK = 463,             /* 开始语音对讲 */
	PTC_LOG_OPSET_STOP_TALKBACK = 464,              /* 停止语音对讲 */

	PTC_LOG_OPSET_IPC_ADD = 465,                    /* 添加IPC */
	PTC_LOG_OPSET_IPC_DEL = 466,                    /* 删除IPC */
	PTC_LOG_OPSET_IPC_SET = 467,                    /* 设置IPC */
    PTC_LOG_OPSET_IPC_QUICK_ADD = 468,				/* 快速添加IPC */
    PTC_LOG_OPSET_IPC_NET_ADD   = 469,				/* 网段添加IPC */

	/* 配置类 */
	PTC_LOG_OPSET_DEV_BAS_CFG = 500,                /* 设备基本信息配置 */
	PTC_LOG_OPSET_TIME_CFG = 501,                   /* 设备时间配置 */
	PTC_LOG_OPSET_SERIAL_CFG = 502,                 /* 设备串口配置 */

	PTC_LOG_OPSET_CHL_BAS_CFG = 503,                /* 通道基本配置 */
	PTC_LOG_OPSET_CHL_NAME_CFG = 504,               /* 通道名称配置 */
	PTC_LOG_OPSET_CHL_ENC_VIDEO = 505,              /* 视频编码参数配置 */
	PTC_LOG_OPSET_CHL_DIS_VIDEO = 506,              /* 通道视频显示参数配置 */
	PTC_LOG_OPSET_PTZ_CFG = 507,                    /* 云台配置 */
	PTC_LOG_OPSET_CRUISE_CFG = 508,                 /* 巡航线路设置 */
	PTC_LOG_OPSET_PRESET_CFG = 509,                 /* 预置点设置 */
	PTC_LOG_OPSET_VIDPLAN_CFG = 510,                /* 录像计划配置 */
	PTC_LOG_OPSET_MOTION_CFG = 511,                 /* 运动检测配置 */
	PTC_LOG_OPSET_VIDLOSS_CFG = 512,                /* 视频丢失配置 */
	PTC_LOG_OPSET_COVER_CFG = 513,                  /* 视频遮挡配置 */
	PTC_LOG_OPSET_MASK_CFG = 514,                   /* 视频遮盖配置 */
	PTC_LOG_OPSET_SCREEN_OSD_CFG = 515,             /* OSD叠加配置 */

	PTC_LOG_OPSET_ALARMIN_CFG = 516,                /* 报警输入配置 */
	PTC_LOG_OPSET_ALARMOUT_CFG = 517,               /* 报警输出配置 */
	PTC_LOG_OPSET_ALARMOUT_OPEN_MAN = 518,          /* 手动开启报警输出,人机 */
	PTC_LOG_OPSET_ALARMOUT_CLOSE_MAN = 519,         /* 手动关闭报警输出,人机 */

	PTC_LOG_OPSET_ABNORMAL_CFG = 520,               /* 异常配置 */
	PTC_LOG_OPSET_HDD_CFG = 521,                    /* 硬盘配置 */

	PTC_LOG_OPSET_NET_IP_CFG = 522 ,                /* TCP/IP配置 */
	PTC_LOG_OPSET_NET_PPPOE_CFG = 523,              /* PPPOE配置 */
	PTC_LOG_OPSET_NET_PORT_CFG = 524,               /* 端口配置 */

	PTC_LOG_OPSET_NET_DDNS_CFG = 525,               /**DDNS配置*/
	/* 维护类 */
	PTC_LOG_OPSET_START_DVR = 600,                  /* 开机 */
	PTC_LOG_OPSET_STOP_DVR = 601,                   /* 关机 */
	PTC_LOG_OPSET_REBOOT_DVR = 602,                 /* 重启设备 */
	PTC_LOG_OPSET_UPGRADE = 603,                    /* 版本升级 */
	PTC_LOG_OPSET_LOGFILE_EXPORT = 604,             /* 导出日志文件 */
	PTC_LOG_OPSET_CFGFILE_EXPORT = 605,             /* 导出配置文件 */
	PTC_LOG_OPSET_CFGFILE_IMPORT = 606,             /* 导入配置文件 */
	PTC_LOG_OPSET_CONF_SIMPLE_INIT = 607,           /* 简单恢复配置 */
	PTC_LOG_OPSET_CONF_ALL_INIT = 608,              /* 恢复出厂配置 */
	PTC_LOG_OPSET_CONF_AUTO_MAINTAIN = 609,         /* 自动维护 */

//	PTC_LOG_XM = 1000,				   /*  熊迈所有信息类日志*/
	PTC_LOG_XM_Reboot				= 1001,	//重启				Reboot
	PTC_LOG_XM_ShutDown				= 1002, //关机				Shut Down
	PTC_LOG_XM_SaveConfig			= 1003, //保存配置			Save Configuration
	PTC_LOG_XM_FileAccessError		= 1004, //访问文件出错		File Access Error
	PTC_LOG_XM_SetDriverType		= 1005, //设置驱动器类型	Set Driver Type
	PTC_LOG_XM_ClearDriver			= 1006, //清除驱动器数据	Clear Driver
	PTC_LOG_XM_StorageDeviceError	= 1007, //存储设备出错		Storage Device Error
	PTC_LOG_XM_EventStart			= 1008, //报警开始			Alarm Start
	PTC_LOG_XM_EventStop			= 1009, //报警结束			Alarm Stop
	PTC_LOG_XM_LogIn				= 1010, //用户登陆			Log In
	PTC_LOG_XM_LogOut				= 1011, //用户登出			Log Out
	PTC_LOG_XM_AddUser				= 1012, //增加用户			Add User
	PTC_LOG_XM_DeleteUser			= 1013, //删除用户			Delete User
	PTC_LOG_XM_ModifyUser			= 1014, //修改用户			Modify User
	PTC_LOG_XM_ModifyPassword		= 1015, //修改密码			Modify Password
	PTC_LOG_XM_AddGroup				= 1016, //添加组			Add Group
	PTC_LOG_XM_DeleteGroup			= 1017, //删除组			Delete Group
	PTC_LOG_XM_ModifyGroup			= 1018, //修改组			Modify Group
	PTC_LOG_XM_ClearLog				= 1019, //清除日志			Clear Log
	PTC_LOG_XM_FileSearch			= 1020, //文件查询			File Search
	PTC_LOG_XM_FileAccess			= 1021, //文件存取			File Access
	PTC_LOG_XM_Record				= 1022, //录像操作			Week Day
	PTC_LOG_XM_ModifyTime			= 1023, //修改系统时间		Modify System Time
	PTC_LOG_XM_ZeroBitrate			= 1024, //无码流			No Bitrate
	PTC_LOG_XM_AccountRestore		= 1025, //账号还原默认		Account Restore
	PTC_LOG_XM_Upgrade				= 1026, //系统升级			Upgrade
	PTC_LOG_XM_DiskChanged			= 1027, //存储设备变动		Disk Changed
	PTC_LOG_XM_Exception			= 1028, //系统异常			Exception
	PTC_LOG_XM_SaveSystemState		= 1029, //保存系统状态		Save System State
	
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
	unsigned char channel_type;		//通道类型，0=主码流，1=子码流
	unsigned char channel_no;		//通道号，从0开始
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
	unsigned char frame_type;			//1=I, 2=P, 3=A, 255=录像结束帧 PTC_FRAME_E
	unsigned char frame_sub_type;		//frame_type=1/2: PTC_VIDEO_ENCODE_E PTC_AUDIO_ENCODE_E
	unsigned char frame_rate;
	unsigned char security;
	unsigned char padding_bytes;
	unsigned char channel_type;			//通道类型 PTC_CH_TYPE_E
	unsigned char channel_no;			//从零开始
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
	unsigned int device_type;			//设备类型
	
	unsigned char video_input_num;		//视频输入数目
	unsigned char audio_input_num;		//音频输入数目
	unsigned char alarm_input_num;		//报警输入数目
	unsigned char alarm_output_num;		//报警输出数目
	
	unsigned char max_login;			//最大登陆数目，0表示没有限制
	unsigned char max_liveview;			//最大实时预览数目，0表示没有限制
	unsigned char max_playback;			//最大回放数目，0表示没有限制
	unsigned char max_talk;				//音频对讲数目

	unsigned short nc_ver;				//网络库版本号 0x2102	
	unsigned short nc_date;				//网络库打包时间 0x0105

	char dev_id[64];					//设备唯一标示

	unsigned char reserve[47];			//保留
	char flag;							//NC端计数
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
	unsigned int utc_sec;				//时间秒数
	int zone;							//时区偏移量

	char ntp_ip[PTC_IP_SIZE];			//ntp 地址
	
	unsigned short ntp_port;			//ntp端口
	unsigned char ntp_enable;			//PTC_ENABLE_E
	unsigned char dst_enable; 			//PTC_ENABLE_E
	
	unsigned short start_year;			//暂时无用，保留用
	unsigned char start_month; 			//1-12
	unsigned char start_day; 			//1-31
	
	unsigned char start_weekinMonth; 	//1-5
	unsigned char start_dayofWeek;		//0-6	 0星期日
	unsigned char start_hour; 			//0-23
	unsigned char start_min; 			//0-59
	
	unsigned char start_sec; 			//0-59
	unsigned short end_year;			//暂时无用，保留用
	unsigned char end_month; 			//1-12
	
	unsigned char end_day;	 			//1-31
	unsigned char end_weekinMonth; 		//1-5
	unsigned char end_dayofWeek; 		//0-6	 0星期日
	unsigned char end_hour; 			//0-23
	
	unsigned char end_min; 				//0-59
	unsigned char end_sec; 				//0-59
	unsigned char shift_hour; 			//PTC_TIME_SHIFTHOUR_E
	unsigned char status;				//暂时无用，保留用 0未偏移  1已偏移
	
	unsigned char dst_hour;				//已经偏移量
	unsigned char dst_mode;				//PTC_TIME_DSTMODE_E
	unsigned char date_mode;			//日期格式
	unsigned char time_mode;			//时间格式

	unsigned char reserve[12];			
}ptc_cfg_time_t;

typedef struct
{
	int year;				//年
	int month;				//月
	int day;				//日
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
	unsigned char channel;				//从零开始
	unsigned char type;					//PTC_RECORD_TYPE_E
	unsigned char sub_type;				//PTC_RECORD_SUBTYPE_E
	unsigned char reserve[17];			
	unsigned char file_info[128];		//设备文件信息
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
	unsigned long long io_in;			//IO报警输入状态: 0=关闭，1=打开
	unsigned long long io_out;			//IO报警输出状态 0=关闭，1=打开
	unsigned long long motion;			//移动侦测状态 0=关闭，1=打开
	unsigned long long loss;			//视频丢失状态 0=关闭，1=打开
	unsigned long long dick;			//磁盘报警状态 0=关闭，1=打开
	unsigned long long manual_record;	//手动录像状态 0=关闭，1=打开
	unsigned long long time_record;		//定时录像状态 0=关闭，1=打开
	unsigned long long motion_record;	//移动侦测录像状态 0=关闭，1=打开
	unsigned long long io_record;		//IO报警录像 0=关闭，1=打开
	unsigned char ir_cut;				//ir-cut状态 0=白天，1=晚上
	unsigned char ir_input;				//光敏电阻状态 0=关闭，1=打开
	unsigned char pir;					//PIR状态 0=关闭，1=打开
	unsigned char alarm_led;			//红外灯状态 0=关闭，1=打开
	unsigned char volume;				//音量:0-10
	unsigned char reserve[75];
	unsigned long long poc;				//POC状态 0=关闭，1=打开
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
	PTC_ALARM_TYPE_VMF_ENTER	= 0, //入侵报警VMF(Video Motion Filter) 
	PTC_ALARM_TYPE_VMF_LOSS		= 1, //丢失报警
	PTC_ALARM_TYPE_VMF_LINE 	= 2, //边界报警
	PTC_ALARM_TYPE_TA_FOCUS		= 3, //虚焦报警TA(Tampering alarm)
	PTC_ALARM_TYPE_TA_COVER		= 4, //遮挡报警
	
	PTC_ALARM_TYPE_MAX,
}PTC_ALARM_TYPE_E;

typedef struct
{
	float x;						//x轴坐边，百分比
	float y;						//y轴坐标，百分比
}ptc_point_t;

typedef struct
{
	ptc_point_t right_down;			//右下点
	ptc_point_t left_down;			//左下点
	ptc_point_t left_up;			//左上点
	ptc_point_t right_up;			//右上点
}ptc_rect_t;

typedef enum
{
	PTC_VMF_LINE_DRT_BOTH		= 0,//双向
	PTC_VMF_LINE_DRT_RIGHT		= 1,//向右
	PTC_VMF_LINE_DRT_LEFT		= 2,//向左
}PTC_VMF_LINE_DRT_E;

typedef struct
{
	int enable;						//使能，0=关闭，1=开启
	ptc_rect_t rect;				//矩形框
	PTC_VMF_LINE_DRT_E drt;			//方向，仅边界报警有效
}ptc_cfg_alarm_t;

typedef enum
{
	PTC_WORKMODE_POWER		= 0,	//充电宝模式
	PTC_WORKMODE_3G2WiFi	= 1,	//3G转WiFi模式
	PTC_WORKMODE_WiFi		= 2,	//视频模式
	PTC_WORKMODE_SAVE		= 3,	//省电模式

	PTC_WORKMODE_MAX,
}PTC_WORK_MODE_E;

typedef struct
{
	PTC_WORK_MODE_E mode;			//工作模式
}ptc_cfg_workMode_t;

typedef struct
{
	int enable_3g;					//3G使能: 0=不使能，1=使能
	int status_3g;					//3G状态: 0:拨号失败，1=拨号成功，(只读)
}ptc_cfg_mobile_t;

typedef enum
{
	PTC_WiFi_MODE_AP		= 0,	//AP模式
	PTC_WiFi_MODE_Client	= 1,	//客户端模式

	PTC_WiFi_MODE_MAX
}PTC_WiFi_MODE_E;

typedef struct
{
	PTC_WiFi_MODE_E mode;			//WiFi模式

	char client_ssid[64];			//客户端模式-SSID
	char client_pswd[64];			//客户端模式-密码
	int  client_level;				//客户端模式-信号强度，只读
	int  client_dhcp;				//客户端模式-是否开启DHCP, 0=手动, 1=DHCP
	unsigned char client_ip[4];		//客户端模式-IP地址
	unsigned char client_mask[4];	//客户端模式-子网掩码
	unsigned char client_gate[4];	//客户端模式-网关
	unsigned char client_dns1[4];	//客户端模式-DNS1
	unsigned char client_dns2[4];	//客户端模式-DNS2

	char ap_ssid[64];				//热点模式-SSID，只读
	char ap_pswd[64];				//热点模式-密码

}ptc_cfg_wifi_t;

typedef struct
{
	int enable;						//使能，0=关闭，1=开启
	int cover_type;					//存储规则 0=自动覆盖,1=不覆盖

	int state;						//硬盘状态，0=无磁盘，1=空闲，2=使用中
	int total_size;					//总容量，单位MB
	int left_size;					//剩余容量，单位MB
}ptc_cfg_devRecord_t;

typedef struct
{
	int mask;						//视频遮挡 0=不遮挡，1=遮挡
}ptc_cfg_av_t;

typedef struct
{
	int power;							//电量1-100
	int wifisg;							//wifi信号	1-100
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
	HDCCTV_ERROR_OPEN_DATABASE				= 1,		//打开数据库失败
	HDCCTV_ERROR_USE_DATABASE				= 2,		//选择数据库失败
	HDCCTV_ERROR_SELECT_SMS_BY_MOBILE		= 3,		//根据手机号查询SMS数据失败
	HDCCTV_ERROR_SELECT_USER_BY_MOBILE		= 4,		//根据手机号查询用户数据失败
	HDCCTV_ERROR_INSERT_SMS					= 5,		//插入SMS记录失败
	HDCCTV_ERROR_UPDATA_MOBILE				= 6,		//更新SMS记录失败
	HDCCTV_ERROR_SELECT_USER_BY_NAME		= 7,		//根据用户名查询用户失败
	HDCCTV_ERROR_INSERT_USER_BY_NAME		= 8,		//插入用户记录失败
	HDCCTV_ERROR_SELECT_DEVICE_BY_ID		= 9,		//根据标示查询设备数据失败
	HDCCTV_ERROR_INSERT_DEVICE				= 10,		//插入设备记录失败
	HDCCTV_ERROR_UPDATA_DEVICE				= 11,		//更新设备记录失败
	HDCCTV_ERROR_UPDATA_USER				= 12,		//更新用户记录失败
	HDCCTV_ERROR_SELECT_MAP_BY_TOKEN		= 13,		//根据手机标示查询设备列表失败
	HDCCTV_ERROR_DELETE_MAP_BY_TOKEN		= 14,		//根据手机标示删除设备列表失败
	HDCCTV_ERROR_INSERT_MAP					= 15,		//插入QPNS映射记录失败
	HDCCTV_ERROR_INSERT_ALARM 				= 16,		//插入报警消息记录失败
	HDCCTV_ERROR_SELECT_MAP_BY_ID			= 17,		//根据设备标示查询设备列表失败
	HDCCTV_ERROR_SELECT_USER_BY_TOKEN		= 18,		//根据用户标示查询用户失败
	HDCCTV_ERROR_INSERT_USER_BY_TOKEN		= 19,		//插入用户记录失败
	HDCCTV_ERROR_DELETE_MAP_BY_ID_AND_TOKEN	= 20,		//根据手机标示和设备标示删除设备列表失败
	HDCCTV_ERROR_SELECT_MAP_BY_ID_AND_TOKEN	= 21,		//根据手机标示和设备标示查询设备列表失败
	HDCCTV_ERROR_SELECT_ALARM_BY_TOKEN		= 22,		//根据手机标示搜索报警消息失败
	HDCCTV_ERROR_SELECT_RELAYMAP_BY_DEVID	= 23,		//根据设备标示搜索relay映射列表
	HDCCTV_ERROR_INSERT_RELAYMAP			= 24,		//插入Relay映射记录失败
	HDCCTV_ERROR_SELECT_SERVER_BY_ID		= 25,		//根据服务器标示查询服务器失败
	HDCCTV_ERROR_SELECT_USER_BY_EMAIL		= 26,		//根据邮箱地址查询用户失败
	HDCCTV_ERROR_SELECT_SERVICE_BY_USERNAME	= 27,		//根据用户名查询服务映射列表失败
	HDCCTV_ERROR_DELETE_SERVICE_BY_ID_AND_U = 28,		//根据设备标示和用户名删除SERVICE记录；
	HDCCTV_ERROR_UPDATA_RELAYMAP			= 29,		//更新Relay映射记录失败
	HDCCTV_ERROR_SELECT_SERVICE_BY_ID_AND_U	= 30,		//根据设备标示和用户标示搜索SERVICE记录失败

	HDCCTV_ERROR_SELECT_USER_BY_NAME_AND_PSWD		= 31,		//根据用户标示和密码搜索Cloud用户失败
	HDCCTV_ERROR_SELECT_USER_BY_MOBILE_AND_PSWD		= 32,		//根据手机号码和密码搜索Cloud用户失败
	HDCCTV_ERROR_SELECT_USER_BY_EMAIL_AND_PSWD		= 33,		//根据邮箱地址和密码搜索Cloud用户失败
	HDCCTV_ERROR_SELECT_SERVER_BY_INFO				= 34,		//根据服务器信息查询服务器失败
	HDCCTV_ERROR_SELECT_SERVICE_BY_SERVERID			= 35,		//根据服务器标示查询SERVICE记录失败
	HDCCTV_ERROR_SELECT_DEVICE_FLOW_BY_ID	 		= 36,		//根据设备标示查询设备流量失败
	HDCCTV_ERROR_SELECT_CLIENT_FLOW_BY_ID			= 37,		//根据用户标示查询用户流量失败
	HDCCTV_ERROR_INSERT_DEVICE_FLOW					= 38,		//插入设备流量记录失败
	HDCCTV_ERROR_INSERT_CLIENT_FLOW					= 39,		//插入用户流量记录失败
	HDCCTV_ERROR_UPDATE_DEVICE_FLOW 				= 40,		//更新设备流量记录失败
	HDCCTV_ERROR_UPDATE_CLIENT_FLOW 				= 41,		//更新用户流量记录失败
	HDCCTV_ERROR_SELECT_DEVICE_SPACE_BY_ID			= 42,		//根据设备标示查询设备空间失败
	HDCCTV_ERROR_INSERT_DEVICE_SPACE 				= 43,		//插入设备空间记录失败
	HDCCTV_ERROR_UPDATE_DEVICE_SPACE 				= 44,		//更新设备空间记录失败
	HDCCTV_ERROR_UPDATE_SERVICE_BY_ID_AND_U			= 45,		//根据设备标示和用户标示更新SERVICE记录失败

	
	HDCCTV_ERROR_CLIENT_BASE				= 10000,
	HDCCTV_ERROR_MOBILE_NUM					= 10001,	//手机号格式错误
	HDCCTV_ERROR_REPEAT_MOBILE				= 10002,	//手机号已经被注册
	HDCCTV_ERROR_TODAY_COUNT				= 10003,	//达到每天请求短信验证码次数限制
	HDCCTV_ERROR_TOO_FREQUENT				= 10004,	//请求短信验证码过于频繁；
	HDCCTV_ERROR_PASSWORD_LEN				= 10005,	//密码长度错误
	HDCCTV_ERROR_PASSWORD_CONTENT			= 10006,	//密码内容错误
	HDCCTV_ERROR_USERNAME_LEN				= 10007,	//用户名长度过长
	HDCCTV_ERROR_USERNAME_CONTENT			= 10008,	//用户名内容错误
	HDCCTV_ERROR_REPEAT_USERNAME			= 10009,	//用户名已经被注册
	HDCCTV_ERROR_NONE_CODE					= 10010,	//验证码不存在
	HDCCTV_ERROR_WRONG_CODE					= 10011,	//验证码不正确
	HDCCTV_ERROR_CODE_LEN					= 10013,	//验证码长度错误
	HDCCTV_ERROR_EMAIL_LEN					= 10014,	//邮箱地址长度错误
	HDCCTV_ERROR_EMAIL_CONTENT				= 10015,	//邮箱地址内容错误
	HDCCTV_ERROR_UDID_LEN					= 10016,	//手机标示长度错误
	HDCCTV_ERROR_UDID_CONTENT				= 10017,	//手机标示内容错误
	HDCCTV_ERROR_DEVID_LEN					= 10018,	//设备标示长度错误
	HDCCTV_ERROR_DEVID_CONTENT				= 10019,	//设备标示内容错误
	HDCCTV_ERROR_IP_LEN						= 10020,	//网络地址长度错误
	HDCCTV_ERROR_IP_CONTENT					= 10021,	//网络地址内容错误
	HDCCTV_ERROR_PORT_NAME_LEN				= 10022,	//端口列表名称长度错误
	HDCCTV_ERROR_PORT_NAME_CONTENT			= 10023,	//端口列表名称内容错误
	HDCCTV_ERROR_PORT_VLAUE_LEN				= 10024,	//端口列表值长度错误
	HDCCTV_ERROR_PORT_VLAUE_CONTENT			= 10025,	//端口列表值内容错误
	HDCCTV_ERROR_NONE_DEVID					= 10026,	//设备标示不存在
	HDCCTV_ERROR_NONE_USER					= 10027,	//用户名不存在
	HDCCTV_ERROR_WRONG_PASSWORD				= 10028,	//密码错误
	HDCCTV_ERROR_REPEAT_DEVICE				= 10029,	//设备已经被其他用户绑定
	HDCCTV_ERROR_BIND_ACTION_CONTENT		= 10030,	//绑定操作字段内容错误
	HDCCTV_ERROR_DEV_NAME_LEN				= 10031,	//设备名称长度错误
	HDCCTV_ERROR_MAX_DEVICE_NUM				= 10032,	//达到最大设备数量限制
	HDCCTV_ERROR_TIME_LEN					= 10033,	//报警消息时间长度错误
	HDCCTV_ERROR_TIME_CONTENT				= 10034,	//报警消息时间内容错误
	HDCCTV_ERROR_CHANNEL_LEN				= 10035,	//报警消息通道长度错误
	HDCCTV_ERROR_CHANNEL_CONTENT			= 10036,	//报警消息通道内容错误
	HDCCTV_ERROR_TYPE_LEN					= 10037,	//报警消息类型长度错误
	HDCCTV_ERROR_TYPE_CONTENT				= 10038,	//报警消息类型内容错误
	HDCCTV_ERROR_CONTENT_LEN				= 10039,	//报警消息内容长度错误
	HDCCTV_ERROR_TOKEN_TYPE_LEN				= 10040,	//QPNS映射手机类型长度错误
	HDCCTV_ERROR_TOKEN_TYPE_CONTENT			= 10041,	//QPNS映射手机类型内容错误
	HDCCTV_ERROR_QPNS_IDLE					= 10042,	//QPNS服务未开启
	HDCCTV_ERROR_NONE_TOKEN					= 10043,	//用户标示不存在
	HDCCTV_ERROR_ALARM_NO_LEN				= 10044,	//报警标示长度错误
	HDCCTV_ERROR_ALARM_NO_CONTENT			= 10045,	//报警标示内容错误
	HDCCTV_ERROR_MAP_ACTION_CONTENT 		= 10046,	//映射操作字段错误
	HDCCTV_ERROR_LANGUAGE_LEN 				= 10047,	//语言字段长度错误
	HDCCTV_ERROR_LANGUAGE_CONTENT 			= 10048,	//语言字段内容错误
	HDCCTV_ERROR_REPEAT_EMAIL				= 10049,	//邮箱地址已经被注册
	HDCCTV_ERROR_NONE_BINDING				= 10050,	//设备未绑定
	HDCCTV_ERROR_NONE_AUTHORITY				= 10051,	//没有权限
	HDCCTV_ERROR_SMS_ACTION_CONTENT			= 10052,	//请求短信验证码操作字段内容错误
	HDCCTV_ERROR_USER_OR_PWSD		 		= 10053,	//用户名或者密码错误
	HDCCTV_ERROR_CODE_TIMEOUT		 		= 10054,	//验证码过期
	HDCCTV_ERROR_SERVER_ID_LEN				= 10055,	//服务器标示长度错误
	HDCCTV_ERROR_SERVER_ID_CONTENT			= 10056,	//服务器标示内容错误	
	HDCCTV_ERROR_SERVER_PORT_CONTENT		= 10057,	//服务器端口内容错误	
	HDCCTV_ERROR_SERVER_PORT_VALUE			= 10058,	//服务器端口数值超过范围
	HDCCTV_ERROR_SERVER_TYPE_CONTENT		= 10059,	//服务器类型内容错误	
	HDCCTV_ERROR_SERVER_TYPE_VALUE			= 10060,	//服务器类型数值超过范围
	HDCCTV_ERROR_SERVER_NAME_LEN			= 10061,	//服务器名称长度错误
	HDCCTV_ERROR_NONE_SERVER				= 10062,	//服务器不存在
	HDCCTV_ERROR_FLOW_CONTENT				= 10063,	//流量内容错误
	HDCCTV_ERROR_AUTHORITY					= 10064,	//权限错误
	HDCCTV_ERROR_DEV_OFFLINE				= 10065,	//设备不在线
	HDCCTV_ERROR_MAX_PBS_NUM				= 10066,	//达到最大回放数
	HDCCTV_ERROR_PROTOCOL					= 10067,	//协议错误
	HDCCTV_ERROR_OPEN_PBS					= 10068,	//打开回放流失败
	HDCCTV_ERROR_TALK_BUSY					= 10069,	//对讲已经打开
	HDCCTV_ERROR_OPEN_TALK					= 10070,	//打开对讲失败

	
	HDCCTV_ERROR_SMS_BASE					= 20000,
	
}HDCCTV_ERROR_E;

typedef enum
{
	HDCCTV_LOCATION_US		= 0, 				//美国
	HDCCTV_LOCATION_CN		= 10000000, 		//中国
	
}HDCCTV_LOCATION_E;

typedef struct
{
	char dev_id[HDCCTV_QPNS_TOKEN_SIZE];		//设备唯一标示
	char dev_name[PTC_ID_SIZE];					//设备名 utf8
	char in_ipv4[PTC_IP_SIZE];					//内网地址
	char ex_ipv4[PTC_IP_SIZE];					//外网地址
	int http_port;								//
	int sdk_port;								//
	int stream_port;							//
	int rtsp_port;								//
	int onvif_port;								//
}hdcctv_device_t;

#define HDCCTV_CLIENT_DEVICE_NUM		100

typedef struct
{
	int num;									//设备数量
	hdcctv_device_t device[HDCCTV_CLIENT_DEVICE_NUM];
}hdcctv_device_list_t;

typedef struct
{
	char username[PTC_STR_SIZE];			//用户名
	char password[PTC_STR_SIZE];			//密码
	char mobile[PTC_STR_SIZE];				//手机号
	char email[PTC_STR_SIZE];				//电子邮箱
	HDCCTV_LOCATION_E location;				//HDCCTV_LOCATION_E
	char user_ipv4[PTC_IP_SIZE];			//用户外网地址
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
	unsigned int no;						//设备端用该字段作为是否发送标志位
	char dev_id[HDCCTV_QPNS_TOKEN_SIZE];	
	char token[HDCCTV_QPNS_TOKEN_SIZE];
	HDCCTV_QPNS_TOKEN_TYPE_E token_type;
	int time;
	int channel;				//32位按位与
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
	char token[HDCCTV_QPNS_TOKEN_SIZE];		//手机唯一标示
	HDCCTV_QPNS_TOKEN_TYPE_E token_type;	//手机类型
	HDCCTV_LOCATION_E location;				//HDCCTV_LOCATION_E
	HDCCTV_LANUAGE_E language;				//语言	
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
	char username[PTC_STR_SIZE];			//用户名
	char qpns_server_id[PTC_STR_SIZE];		
	char relay_server_id[PTC_STR_SIZE];		//用户名
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
	long long used_flow;				//已使用流量(B)，只可读
	long long total_flow;				//总流量(B),0=无限制，只可读
	long long used_space;				//已使用空间(B)，只可读
	long long total_space;				//总空间(B),0=无限制，只可读
	int record_enable;					//报警联动云端录像，0=不使能，1=使能
}hdcctv_cloud_cfg_t;


#pragma pack()



#endif

#if defined __cplusplus
}
#endif

