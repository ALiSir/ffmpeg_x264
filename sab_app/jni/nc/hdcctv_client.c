
#if defined __cplusplus
extern "C"
{
#endif

#include <stdio.h>
#include "hdcctv_client.h"
#include "tool_type.h"
#include "tool_log.h"
#include "tool_sysf.h"
#include "tool_http.h"
#include "tool_sock.h"
#include "tool_json.h"
#include "tool_md5.h"
#include "tool_ptc.h"
#include "tool_stream3.h"
#include "qh_ptc.h"
#include "nc.h"
#include "tool_base64.h"
#include "tool_stream2.h"

#define __HDCCTV_CLIENT_QPNS__


typedef struct
{
	//qpnc
	TOOL_INT32 state;
	TOOL_THREAD pid;
	TOOL_MUTEX mutex;

	NC_CB cb;
	TOOL_VOID* param;

	//location
	TOOL_INT32 loc_enable;
	TOOL_INT32 loc_tick;
	TOOL_INT32 ios_online;
	TOOL_INT32 ios_login_tick;
	TOOL_INT32 android_online;
	TOOL_INT32 android_login_tick;
	TOOL_INT32 android_heartbeat_tick;
	tool_sock_item_t sock_item;
	ptc_msg_t msg;
	TOOL_INT32 is_sleep;
	HDCCTV_LOCATION_E location;
	TOOL_INT8 server_ip[PTC_IP_SIZE];
	TOOL_INT32 server_port;

	nc_dev st_nc_dev;

	//service
	TOOL_INT32 qpns_enable;
	TOOL_INT32 qpns_tick;
	hdcctv_service_addr_t qpns_addr;

	TOOL_VOID* father_client;
}hdcctv_client_qpns_t;

typedef struct
{	
	hdcctv_client_qpns_t qpns;

	HDCCTV_QPNS_TOKEN_TYPE_E token_type;
	TOOL_INT8 token[HDCCTV_QPNS_TOKEN_SIZE];
	HDCCTV_LANUAGE_E language;
}hdcctv_client_t;


#define HDCCTV_CLIENT_USLEEP_CYCLE				(500*1000)
#define HDCCTV_QPNS_ANDROID_LOGIN_CYCLE_SEC		30
#define HDCCTV_QPNS_HEARTBEAT_CYCLE_SEC			30
#define HDCCTV_QPNS_LOCALTION_CYCLE_SEC			30
#define HDCCTV_QPNS_IOS_LOGIN_CYCLE_SEC			30
#define HDCCTV_QPNS_SERVICE_CYCLE_SEC			30

#define HDCCTV_CLIENT2_RECONNECT_CYCLE			10
#define HDCCTV_CLIENT2_HEARTBEAT_CYCLE			10
#define HDCCTV_CLIENT2_SERVICE_CYCLE			10
#define HDCCTV_CLIENT2_STREAM_TIMEOUT			10
#define HDCCTV_CLIENT2_USLEEP_CYCLE				(100*1000)
#define HDCCTV_CLIENT2_RELAY_SIZE				2
#define HDCCTV_CLIENT2_RELAY_CYCLE				10
#define HDCCTV_CLIENT2_RELAY_RTS_BUFFER			(4*1024*1024)
#define HDCCTV_CLIENT2_MSGS_SIZE				16
#define HDCCTV_CLIENT2_RELAY_PBS_BUFFER			(4*1024*1024)
#define HDCCTV_CLIENT2_LOGIN_CYCLE				60


TOOL_INT32 hdcctv_client_location(HDCCTV_LOCATION_E* location, TOOL_INT8* server_ip, TOOL_INT32* server_port)
{
	if (location == NULL || server_ip == NULL || server_port == NULL)
	{
		log_error("server_ip(%x) server_port(%x) location(%x) qpns_port(%x)", server_ip, server_port, location);
		return -1;
	}

	tool_http_t send_http;
	tool_http_t recv_http;
	tool_http_Init(&send_http);
	tool_str_strncpy(send_http.command, "POST", tool_str_strlen("POST"));
	tool_str_strncpy(send_http.url, "/u/loc_c.php", tool_str_strlen("/u/loc_c.php"));
	tool_str_strncpy(send_http.version, "HTTP/1.0", tool_str_strlen("HTTP/1.0"));

	tool_sock_item_t sock;
	tool_mem_memset(&sock, sizeof(tool_sock_item_t));
	if (
//		tool_sock_item_open_v2(&sock, TOOL_SOCK_TYPE1_TCP, TOOL_SOCK_TYPE2_CLIENT, HDCCTV_QPNS_IN_SERVER_DOMAIN, HDCCTV_QPNS_SERVER_PORT, NULL, 0) < 0 &&
		tool_sock_item_open_v2(&sock, TOOL_SOCK_TYPE1_TCP, TOOL_SOCK_TYPE2_CLIENT, HDCCTV_QPNS_IN_SERVER_IP, HDCCTV_QPNS_SERVER_PORT, NULL, 0) < 0)
	{
		log_error("tool_sock_item_open");
		return -2;
	}
	TOOL_INT32 ret = 0;
	do
	{
		tool_http_Init(&recv_http);	
		if (tool_http_Send(&send_http, &sock, NULL, NULL) < 0)
		{
			log_error("tool_http_Send");
			ret = -3;
			break;
		}
		if (tool_http_Recv(&recv_http, &sock, NULL, NULL) < 0)
		{
			log_error("tool_http_Send");
			ret = -4;
			break;
		}
		if (tool_str_strncmp(recv_http.command, "200", tool_str_strlen("200") != 0))
		{
			log_error("recv_http.command(%s)", recv_http.command);
			ret = -5;
			break;
		}
		
		tool_json_t st_json;
		if (tool_json_parse(&st_json, recv_http.body) < 0)
		{
			log_error("recv_http.body(%s)", recv_http.body);
			ret = -6;
			break;
		}

		TOOL_INT8* result = tool_json_getValue(&st_json, "result");
		if (result == NULL)
		{
			log_error("result(%d)", result);
			ret = -7;
			break;
		}
		if (tool_str_strncmp(result, "0", 1) != 0)
		{
			log_error("result(%s)", result);
			ret = -8;
			break;
		}
		
		TOOL_INT8* pch_location = tool_json_getValue(&st_json, "location");
		TOOL_INT8* pch_server_ip = tool_json_getValue(&st_json, "server_ip");
		TOOL_INT8* pch_server_port = tool_json_getValue(&st_json, "server_port");
		if (pch_location == NULL || pch_server_ip == NULL || pch_server_port == NULL)
		{
			log_error("pch_location(%d) pch_server_ip(%d) pch_server_port(%d)", pch_location, pch_server_ip, pch_server_port);
			ret = -9;
			break;
		}
		*location = (HDCCTV_LOCATION_E)tool_sysf_atoi(pch_location);
		tool_str_strncpy(server_ip, pch_server_ip, PTC_IP_SIZE-1);
		*server_port = tool_sysf_atoi(pch_server_port);
		log_state("*location(%d) server_ip(%s) *server_port(%d) *qpns_port(%d)", *location, server_ip, *server_port);
	} while (0);
	tool_sock_item_close(&sock);
	return ret;	
}

TOOL_INT32 hdcctv_client_getServiceAddr(TOOL_INT8* server_ip, TOOL_INT32 server_port, nc_dev* pst_dev, TOOL_INT8* action, hdcctv_service_addr_t* pst_service_addr)
{
	if (server_ip == NULL || pst_dev == NULL || pst_service_addr == NULL)
	{
		log_error("server_ip(%d) pst_dev(%d) pst_service_addr(%d) ", server_ip, pst_dev, pst_service_addr);
		return NC_ERROR_INPUT;	
	}

	TOOL_UINT8 password_byte[16] = {0};
	tool_md5_calc((TOOL_UINT8*)pst_dev->pswd, tool_str_strlen(pst_dev->pswd), password_byte);
	TOOL_INT8 password_str[64] = {0};
	tool_str_byte2str(password_byte, 16, password_str);
//	log_debug("password_str(%s)", password_str);

	tool_http_t send_http;
	tool_http_t recv_http;
	tool_http_Init(&send_http);
	tool_str_strncpy(send_http.command, "POST", tool_str_strlen("POST"));
	tool_str_strncpy(send_http.url, "/u/service_c.php", tool_str_strlen("/u/service_c.php"));
	tool_str_strncpy(send_http.version, "HTTP/1.0", tool_str_strlen("HTTP/1.0"));
	send_http.ContentLength = snprintf(send_http.body, sizeof(send_http.body), 
		"{\"username\":\"%s\",\"password\":\"%s\",\"dev_id\":\"%s\",\"action\":\"%s\"}", 
		pst_dev->user, password_str, pst_dev->dev_id, action);
	log_debug("send_http.body(%s)", send_http.body);

	tool_sock_item_t sock;
	tool_mem_memset(&sock, sizeof(tool_sock_item_t));
	if (tool_sock_item_open_v2(&sock, TOOL_SOCK_TYPE1_TCP, TOOL_SOCK_TYPE2_CLIENT, server_ip, server_port, NULL, 0) < 0)
	{
		log_error("tool_sock_item_open(%s:%d)", server_ip, server_port);
		return NC_ERROR_CONNECT;
	}
	TOOL_INT32 ret = 0;
	do
	{
		tool_http_Init(&recv_http);	
		if (tool_http_Send(&send_http, &sock, NULL, NULL) < 0)
		{
			log_error("tool_http_Send");
			ret = NC_ERROR_SEND;
			break;
		}
		if (tool_http_Recv(&recv_http, &sock, NULL, NULL) < 0)
		{
			log_error("tool_http_Send");
			ret = NC_ERROR_RECV;
			break;
		}
		if (tool_str_strncmp(recv_http.command, "200", tool_str_strlen("200") != 0))
		{
			log_error("recv_http.command(%s)", recv_http.command);
			ret = NC_ERROR_PROTOCOL;
			break;
		}
		tool_json_t st_json;
		if (tool_json_parse(&st_json, recv_http.body) < 0)
		{
			log_error("recv_http.body(%s)", recv_http.body);
			ret = NC_ERROR_PROTOCOL;
			break;
		}
		TOOL_INT8* result = tool_json_getValue(&st_json, "result");
		if (result == NULL || tool_str_strncmp(result, "0", 1) != 0)
		{
			log_error("recv_http.body(%s)", recv_http.body);
			ret = NC_ERROR_PROTOCOL;
			break;
		}

		TOOL_INT8* pch_ip = tool_json_getValue(&st_json, "ip");
		TOOL_INT8* pch_port = tool_json_getValue(&st_json, "port");
		TOOL_INT8* pch_nonce = tool_json_getValue(&st_json, "nonce");
		if (pch_ip == NULL || pch_port == NULL || pch_nonce == NULL)
		{
			log_error("pch_ip(%d) pch_port(%d)  pch_nonce(%d)", pch_ip, pch_port,  pch_nonce);
			ret = -9;
			break;
		}
		tool_str_strncpy(pst_service_addr->ip, pch_ip, PTC_IP_SIZE-1);
		pst_service_addr->port = tool_sysf_atoi(pch_port);
		pst_service_addr->nonce = tool_sysf_atoi(pch_nonce);
		tool_ptc_qh_encodeNonce(pst_service_addr->nonce, pst_service_addr->nonce_md5);
		log_state("addr(%s:%d) *nonce(%d:%s)", pst_service_addr->ip, pst_service_addr->port, pst_service_addr->nonce, pst_service_addr->nonce_md5);
	} while (0);
	tool_sock_item_close(&sock);
	return ret;		
}


TOOL_VOID hdcctv_client_qpns_doAlarm(hdcctv_client_qpns_t* qpns)
{
	tool_json_t st_json;
	if (tool_json_parse(&st_json, (TOOL_INT8*)qpns->msg.data) < 0)
	{
		log_error("qpns->msg.data(%s)", qpns->msg.data);
		return ;
	}

	TOOL_INT8* dev_id  = tool_json_getValue(&st_json, "dev_id");
	TOOL_INT8* time    = tool_json_getValue(&st_json, "time");
	TOOL_INT8* channel = tool_json_getValue(&st_json, "content");
	TOOL_INT8* type    = tool_json_getValue(&st_json, "type");
	TOOL_INT8* content = tool_json_getValue(&st_json, "content");
	hdcctv_qpns_alarm_t alarm;
	tool_mem_memset(&alarm, sizeof(hdcctv_qpns_alarm_t));
	if (dev_id)
		tool_str_strncpy(alarm.dev_id, dev_id, sizeof(alarm.dev_id)-1);
	if (time)
		alarm.time = tool_sysf_atoi(time);
	if (channel)
		alarm.channel = tool_sysf_atoi(channel);
	if (type)
		alarm.type = tool_sysf_atoi(type);
	if (content)
		tool_str_strncpy(alarm.content, content, sizeof(alarm.content)-1);
	if (qpns->cb)
		qpns->cb(0, &alarm, sizeof(hdcctv_qpns_alarm_t), qpns->param);
}

TOOL_INT32 hdcctv_client_qpns_android_recvMsg(hdcctv_client_qpns_t* qpns, TOOL_INT32 cmd)
{
	TOOL_INT32 ret = 0;
	while (1)
	{
		ret = tool_ptc_qh_recvMsg(&qpns->sock_item, &qpns->msg);
		if (ret < 0)
			break;
		
		if (qpns->msg.head.cmd == PTC_CMD_QPNS_ALARM)
			hdcctv_client_qpns_doAlarm(qpns);
		
		if (qpns->msg.head.cmd == cmd)
			break;		
		log_state("invalid cmd(0x%02x) cmd(0x%02x)", qpns->msg.head.cmd);
	}
	return ret;
}


TOOL_VOID hdcctv_client_qpns_init(hdcctv_client_qpns_t* qpns, TOOL_VOID* father_client)
{
	qpns->father_client = father_client;
}

TOOL_INT32 hdcctv_client_qpns_ios_login(hdcctv_client_qpns_t* qpns)
{
	if (qpns->ios_online)
		return 0;

	if (tool_sock_isTickTimeout(qpns->ios_login_tick, HDCCTV_QPNS_IOS_LOGIN_CYCLE_SEC) == 0)
	{
		tool_sysf_usleep(HDCCTV_CLIENT_USLEEP_CYCLE);
		return -1;
	}
	tool_sock_setTick(&qpns->ios_login_tick);

	hdcctv_client_t* client = (hdcctv_client_t*)qpns->father_client;
	tool_http_t send_http;
	tool_http_t recv_http;
	tool_http_Init(&send_http);
	tool_str_strncpy(send_http.command, "POST", tool_str_strlen("POST"));
	tool_str_strncpy(send_http.url, "/u/qpns_ios_login.php", tool_str_strlen("/u/qpns_ios_login.php"));
	tool_str_strncpy(send_http.version, "HTTP/1.0", tool_str_strlen("HTTP/1.0"));
	send_http.ContentLength = snprintf(send_http.body, sizeof(send_http.body), "{\"token\":\"%s\",\"token_type\":\"%d\",\"language\":\"%d\"}", client->token, client->token_type, client->language);
	if (tool_sock_item_open_v2(&qpns->sock_item, TOOL_SOCK_TYPE1_TCP, TOOL_SOCK_TYPE2_CLIENT, qpns->server_ip, qpns->server_port, NULL, 0) < 0)
	{
		log_error("tool_sock_item_open");
		return -1;
	}
	TOOL_INT32 ret = 0;
	do
	{
		tool_http_Init(&recv_http);	
		if (tool_http_Send(&send_http, &qpns->sock_item, NULL, NULL) < 0)
		{
			log_error("tool_http_Send");
			ret = -1;
			break;
		}
		if (tool_http_Recv(&recv_http, &qpns->sock_item, NULL, NULL) < 0)
		{
			log_error("tool_http_Send");
			ret = -1;
			break;
		}
		if (tool_str_strncmp(recv_http.command, "200", tool_str_strlen("200") != 0))
		{
			log_error("recv_http.command(%s)", recv_http.command);
			ret = -1;
			break;
		}
		
		tool_json_t st_json;
		if (tool_json_parse(&st_json, recv_http.body) < 0)
		{
			log_error("recv_http.body(%s)", recv_http.body);
			ret = -1;
			break;
		}

		TOOL_INT8* result = tool_json_getValue(&st_json, "result");
		if (result == NULL)
		{
			log_error("result(%d)", result);
			ret = -1;
			break;
		}
		if (tool_str_strncmp(result, "0", 1) != 0)
		{
			log_error("result(%s)", result);
			ret = -1;
			break;
		}
		qpns->ios_online = 1;
	} while (0);
	tool_sock_item_close(&qpns->sock_item);
	return ret;	
}

TOOL_INT32 hdcctv_client_qpns_ios_logout(hdcctv_client_qpns_t* qpns)
{
	qpns->ios_online = 0;
	return 0;
}

TOOL_INT32 hdcctv_client_qpns_android_login(hdcctv_client_qpns_t* qpns)
{
	if (qpns->android_online)
		return 0;

	if (tool_sock_isTickTimeout(qpns->android_login_tick, HDCCTV_QPNS_ANDROID_LOGIN_CYCLE_SEC) == 0)
	{
		tool_sysf_usleep(HDCCTV_CLIENT_USLEEP_CYCLE);
		return -1;
	}
	tool_sock_setTick(&qpns->android_login_tick);

	do
	{
		hdcctv_client_t* client = (hdcctv_client_t*)qpns->father_client;
		qpns->android_online = 1;
		if (tool_sock_item_open_v2(&qpns->sock_item, TOOL_SOCK_TYPE1_TCP, TOOL_SOCK_TYPE2_CLIENT, qpns->qpns_addr.ip, qpns->qpns_addr.port, NULL, 0) < 0)
		{
			log_error("tool_sock_item_open");
			qpns->android_online = 0;
			break;
		}
		
		tool_mem_memset(&qpns->msg.head, sizeof(ptc_head_t));
		qpns->msg.head.cmd = PTC_CMD_QPNS_LOGIN;
		qpns->msg.head.len = snprintf((TOOL_INT8*)qpns->msg.data, sizeof(qpns->msg.data), "{\"token\":\"%s\"}", client->token);
		if (tool_ptc_qh_sendMsg(&qpns->sock_item, &qpns->msg) < 0)
		{
			log_error("hdcctv_client_qpns_android_login send");
			qpns->android_online = 0;
			break;
		}
		if (hdcctv_client_qpns_android_recvMsg(qpns, PTC_CMD_QPNS_LOGIN) < 0)
		{
			log_error("hdcctv_client_qpns_android_login recv");
			qpns->android_online = 0;
			break;
		}
			
		qpns->android_online = 2;
		tool_sock_setTick(&qpns->android_heartbeat_tick);
	}while (0);
	if (qpns->android_online == 0)
	{
		if (tool_sock_item_isLive(&qpns->sock_item))
			tool_sock_item_close(&qpns->sock_item);
		return -1;
	}
	return 0;
}

TOOL_INT32 hdcctv_client_qpns_android_logout(hdcctv_client_qpns_t* qpns)
{
	if (qpns->android_online == 0)
		return 0;
	
	tool_mem_memset(&qpns->msg.head, sizeof(ptc_head_t));
	qpns->msg.head.cmd = PTC_CMD_QPNS_LOGOUT;
	if (tool_ptc_qh_sendMsg(&qpns->sock_item, &qpns->msg) < 0 || hdcctv_client_qpns_android_recvMsg(qpns, PTC_CMD_QPNS_LOGOUT) < 0)
	{
		log_error("hdcctv_client_qpns_android_logout");
//		return -1;
	}
	if (tool_sock_item_isLive(&qpns->sock_item))
		tool_sock_item_close(&qpns->sock_item);
	qpns->android_online = 0;
	return 0;
}

TOOL_INT32 hdcctv_client_qpns_android_heartbeat(hdcctv_client_qpns_t* qpns)
{
	if (qpns->android_online == 0)
		return -1;
	
	if (tool_sock_isTickTimeout(qpns->android_heartbeat_tick, HDCCTV_QPNS_HEARTBEAT_CYCLE_SEC) == 0)
	{
		return 0;
	}
	tool_sock_setTick(&qpns->android_heartbeat_tick);

	tool_mem_memset(&qpns->msg.head, sizeof(ptc_head_t));
	qpns->msg.head.cmd = PTC_CMD_QPNS_HEARTBEAT;
	if (tool_ptc_qh_sendMsg(&qpns->sock_item, &qpns->msg) < 0)
	{
		log_error("tool_ptc_qh_sendMsg");
		return NC_ERROR_SEND;
	}
	if (hdcctv_client_qpns_android_recvMsg(qpns, PTC_CMD_QPNS_HEARTBEAT) < 0)
	{
		log_error("hdcctv_client_qpns_android_recvMsg");
		return NC_ERROR_RECV;
	}
	log_state("hdcctv_client_qpns_android_heartbeat");
	return 0;
}

TOOL_INT32 hdcctv_client_qpns_android_recv(hdcctv_client_qpns_t* qpns)
{
	if (qpns->android_online == 0)
		return -1;
	
	TOOL_INT32 ret = 0;
	ret = tool_sock_item_pollRead(&qpns->sock_item, 0);
	if (ret < 0)
	{
		log_error("tool_sock_item_pollRead");
		return -1;
	}
	else if (ret == 0)
		return 0;
	if (tool_ptc_qh_recvMsg(&qpns->sock_item, &qpns->msg) < 0)
	{
		log_error("tool_ptc_qh_recvMsg");
		return -1;
	}

	if (qpns->msg.head.cmd == PTC_CMD_QPNS_ALARM)
		hdcctv_client_qpns_doAlarm(qpns);
	else
		log_error("cmd(0x%08x)", qpns->msg.head.cmd);
	
	qpns->is_sleep = 0;
	return 0;	
}

TOOL_VOID hdcctv_client_qpns_sleep(hdcctv_client_qpns_t* qpns)
{
	if (qpns->is_sleep)
		tool_sysf_usleep(HDCCTV_CLIENT_USLEEP_CYCLE);
	qpns->is_sleep = 1;
}

TOOL_VOID* hdcctv_client_qpns_run(TOOL_VOID* param)
{
	log_state("hdcctv_client_qpns_start");
	hdcctv_client_qpns_t* qpns = (hdcctv_client_qpns_t*)param;
	hdcctv_client_t* client = (hdcctv_client_t*)qpns->father_client;
	qpns->loc_tick = -12345678;
	qpns->ios_login_tick = -12345678;
	qpns->android_login_tick = -12345678;
	qpns->qpns_tick = -12345678;

	while (qpns->state)
	{
		if (qpns->loc_enable == 0)
		{
			if (tool_sock_isTickTimeout(qpns->loc_tick, HDCCTV_QPNS_LOCALTION_CYCLE_SEC) == 0)
				continue;
			tool_sock_setTick(&qpns->loc_tick);
			
			if (hdcctv_client_location(&qpns->location, qpns->server_ip, &qpns->server_port) < 0)
			{
				tool_sysf_usleep(HDCCTV_CLIENT_USLEEP_CYCLE);
				continue;
			}
			qpns->loc_enable = 1;
		}

		if (qpns->qpns_enable == 0)
		{
			if (tool_sock_isTickTimeout(qpns->qpns_tick, HDCCTV_QPNS_SERVICE_CYCLE_SEC) == 0)
			{
				tool_sysf_usleep(HDCCTV_CLIENT_USLEEP_CYCLE);
				continue;
			}
			tool_sock_setTick(&qpns->qpns_tick);
			
			if (hdcctv_client_getServiceAddr(qpns->server_ip, qpns->server_port, &qpns->st_nc_dev, "qpns", &qpns->qpns_addr) < 0)
			{
				log_error("hdcctv_client_getServiceAddr");
				continue;
			}
			qpns->qpns_enable = 1;
			log_state("hdcctv_client_getServiceAddr success");
		}
		
		if (client->token_type == HDCCTV_QPNS_TOKEN_TYPE_IOS_ISSMOBILE || client->token_type == HDCCTV_QPNS_TOKEN_TYPE_IOS_IDARLING)
		{
			hdcctv_client_qpns_ios_login(qpns);
			tool_sysf_usleep(HDCCTV_CLIENT_USLEEP_CYCLE);
		}
		else
		{
			if (hdcctv_client_qpns_android_login(qpns) < 0 ||
				hdcctv_client_qpns_android_heartbeat(qpns) < 0 ||
				hdcctv_client_qpns_android_recv(qpns) < 0)
			{
				hdcctv_client_qpns_android_logout(qpns);
				continue;
			}
			hdcctv_client_qpns_sleep(qpns);			
		}
	}

	if (client->token_type == HDCCTV_QPNS_TOKEN_TYPE_IOS_ISSMOBILE || client->token_type == HDCCTV_QPNS_TOKEN_TYPE_IOS_IDARLING)
	{
		hdcctv_client_qpns_ios_logout(qpns);
	}
	else
	{
		hdcctv_client_qpns_android_logout(qpns);
	}
	qpns->loc_enable = 0;
	log_state("hdcctv_client_qpns_stop");
	return NULL;
}

TOOL_VOID hdcctv_client_qpns_start(hdcctv_client_qpns_t* qpns, NC_CB cb, TOOL_VOID* param)
{
	qpns->cb = cb;
	qpns->param = param;
	qpns->state = 1;
	if (tool_thread_create(&qpns->pid, NULL, hdcctv_client_qpns_run, (TOOL_VOID*)qpns))
		log_fatalNo("tool_thread_create");
}

TOOL_VOID hdcctv_client_qpns_stop(hdcctv_client_qpns_t* qpns)
{
	if (qpns->state)
	{
		qpns->state = 0;
		tool_thread_join(qpns->pid);
	}
}

TOOL_VOID hdcctv_client_qpns_done(hdcctv_client_qpns_t* qpns)
{

}

#define __HDCCTV_CLIENT__




hdcctv_client_t* g_hdcctv_client = NULL;

TOOL_INT32 hdcctv_client_startQpns(HDCCTV_QPNS_TOKEN_TYPE_E token_type, TOOL_INT8* mobile_token, HDCCTV_LANUAGE_E language, NC_CB cb, TOOL_VOID* param)
{
	if (mobile_token == NULL)
	{
		log_error("mobile_token(0x%08x)", mobile_token);
		return NC_ERROR_INPUT;
	}
	
	if (g_hdcctv_client)
	{
		log_error("hdcctv client already start");
		return 0;
	}
	
	g_hdcctv_client = (hdcctv_client_t*)tool_mem_malloc(sizeof(hdcctv_client_t), 1);
	hdcctv_client_qpns_init(&g_hdcctv_client->qpns, g_hdcctv_client);

	g_hdcctv_client->token_type = token_type;
	tool_str_strncpy(g_hdcctv_client->token, mobile_token, sizeof(g_hdcctv_client->token)-1);
	g_hdcctv_client->language = language;

	hdcctv_client_qpns_start(&g_hdcctv_client->qpns, cb, param);

	return 0;
}

TOOL_VOID hdcctv_client_stopQpns()
{
	if (g_hdcctv_client == NULL)
	{
		log_error("hdcctv client already stop");
		return ;
	}
	
	hdcctv_client_qpns_stop(&g_hdcctv_client->qpns);
	
	hdcctv_client_qpns_done(&g_hdcctv_client->qpns);

	tool_mem_free(g_hdcctv_client);
	g_hdcctv_client = NULL;
}

#define __HDCCTV_RELAY__

typedef struct 
{
	TOOL_INT32 relay_id;
	TOOL_VOID* father_client2;
	TOOL_INT32 is_used;
	TOOL_MUTEX mutex;

	TOOL_UINT32 type;
	TOOL_INT32 rts_state;
	TOOL_INT32 pbs_state;
	TOOL_INT32 talk_state;
	TOOL_THREAD recv_pid;
	TOOL_THREAD play_pid;
	tool_sock_item_t sock_item;
	ptc_msg_t msg;
	ptc_frame_t* frame;	
	tool_stream3_t stream3_recv;
	tool_stream3_pos_t stream3_read_pos;
	ptc_frame_video_t* stream3_frame_data;

	nc_dev st_nc_dev;
	TOOL_INT32 channel_no;
	TOOL_INT32 stream_type;	
	NC_CB cb;
	TOOL_VOID* param;

	TOOL_INT32 relay_tick;
	TOOL_INT32 stream_tick;
	TOOL_INT32 last_type;
	TOOL_INT32 relay_enable;
	hdcctv_service_addr_t relay_addr;

	ptc_cfg_record_t st_record;
	TOOL_INT32 ch_no;
	TOOL_INT32 ch_type;

	tool_stream2_t* send_stream;
	TOOL_UINT32 send_pos;

	TOOL_INT32 hb_tick;

}hdcctv_client2_relay_t;

typedef struct
{
	//cfg set
	TOOL_INT32 last_set_req;
	TOOL_INT32 cur_set_req;
	TOOL_INT32 set_result;
	TOOL_COND set_cond;
	//cfg get
	TOOL_INT32 last_get_req;
	TOOL_INT32 cur_get_req;
	TOOL_INT32 get_result;
	TOOL_COND get_cond;
}hdcctv_client_cfg_t;



typedef struct
{
	TOOL_INT32 msgs_id;		//*
	TOOL_VOID* father_client2;	//*
	TOOL_MUTEX mutex;		// for msg_sock

	TOOL_INT32 state;		//*
	TOOL_THREAD pid;
	TOOL_INT32 state_tick;	

	TOOL_INT8 user[PTC_ID_SIZE];	//用户名，详见上表
	TOOL_INT8 pswd[PTC_ID_SIZE];	//密码，详见上表

	tool_sock_item_t sock_item;
	TOOL_INT32 hb_tick;
	ptc_msg_t st_msg;
	ptc_frame_t* pst_frame;
	nc_dev st_nc_dev;

	TOOL_INT32 msgs_tick;
	TOOL_INT32 msgs_enable;
	hdcctv_service_addr_t msgs_addr;
	TOOL_INT32 is_sleep;

	//ptz
	TOOL_INT32 last_ptz_req;
	TOOL_INT32 cur_ptz_req;
	TOOL_INT32 ptz_channel;
	PTC_PTZ_CMD_E ptz_cmd;
	TOOL_INT32 ptz_param;

	//time
	TOOL_INT32 last_time_req;
	TOOL_INT32 cur_time_req;
	TOOL_UINT32 utc_sec;
	TOOL_INT32 zone;
	TOOL_INT32 dst_hour;

	//cfg-dates
	TOOL_INT32 last_dates_req;
	TOOL_INT32 cur_dates_req;
	ptc_cfg_dates_t cfg_dates;
	TOOL_INT32 result_dates;
	TOOL_COND cond_dates;
	//cfg-records
	TOOL_INT32 last_records_req;
	TOOL_INT32 cur_records_req;
	ptc_cfg_records_t cfg_records;
	TOOL_INT32 result_records;
	TOOL_COND cond_records;
	//cfg-logs
	TOOL_INT32 last_logs_v2_req;
	TOOL_INT32 cur_logs_v2_req;
	ptc_logs_v2_t cfg_logs_v2;
	TOOL_INT32 result_logs_v2;
	TOOL_COND cond_logs_v2;
	//cfg-status set
	TOOL_INT32 last_set_status_req;
	TOOL_INT32 cur_set_status_req;
	ptc_cfg_status_t cfg_set_status;
	//cfg-status get
	TOOL_INT32 last_get_status_req;
	TOOL_INT32 cur_get_status_req;
	ptc_cfg_status_t cfg_get_status;
	TOOL_INT32 result_status;
	TOOL_COND cond_status;
	//cfg-ability
	TOOL_INT32 last_get_ability_flag;
	TOOL_INT32 cur_get_ability_flag;
	ptc_cfg_ability_t cfg_get_ability;
	TOOL_INT32 result_ability;
	TOOL_COND cond_ability;
	//cfg-alarm set
	TOOL_INT32 last_set_alarm_req;
	TOOL_INT32 cur_set_alarm_req;
	PTC_ALARM_TYPE_E cfg_set_type;
	ptc_cfg_alarm_t cfg_set_alarm;
	TOOL_INT32 result_set_alarm;
	TOOL_COND cond_set_alarm;
	//cfg-alarm get
	TOOL_INT32 last_get_alarm_req;
	TOOL_INT32 cur_get_alarm_req;
	PTC_ALARM_TYPE_E cfg_get_type;
	ptc_cfg_alarm_t cfg_get_alarm;
	TOOL_INT32 result_get_alarm;
	TOOL_COND cond_get_alarm;

	//cfg-wifi set
	TOOL_INT32 last_set_wifi_req;
	TOOL_INT32 cur_set_wifi_req;
	ptc_cfg_wifi_t cfg_set_wifi;
	//cfg-wifi get
	TOOL_INT32 last_get_wifi_req;
	TOOL_INT32 cur_get_wifi_req;
	ptc_cfg_wifi_t cfg_get_wifi;
	TOOL_INT32 result_wifi;
	TOOL_COND cond_wifi;

	//cfg-devRecord set
	TOOL_INT32 last_set_devRecord_req;
	TOOL_INT32 cur_set_devRecord_req;
	ptc_cfg_devRecord_t cfg_set_devRecord;
	//cfg-devRecord get
	TOOL_INT32 last_get_devRecord_req;
	TOOL_INT32 cur_get_devRecord_req;
	ptc_cfg_devRecord_t cfg_get_devRecord;
	TOOL_INT32 result_devRecord;
	TOOL_COND cond_devRecord;

	ptc_cfg_av_t cfg_get_av;
	ptc_cfg_av_t cfg_set_av;	

	ptc_cfg_workMode_t cfg_get_workMode;
	ptc_cfg_workMode_t cfg_set_workMode;
	hdcctv_client_cfg_t cfg[HDCCTV_CLIENT_CFG_NUM];

}hdcctv_client2_msgs_t;

typedef struct
{
	TOOL_INT32 state;
	TOOL_THREAD pid;
	
	hdcctv_qpns_user_t qpns_user;

	TOOL_INT32 loc_enable;
	HDCCTV_LOCATION_E location;
	TOOL_INT8 server_ip[PTC_IP_SIZE];
	TOOL_INT32 server_port;

	TOOL_MUTEX mutex;
	TOOL_INT32 buffer_usec;

	hdcctv_client2_relay_t relay[HDCCTV_CLIENT2_RELAY_SIZE];	//relay server
	hdcctv_client2_msgs_t  msgs[HDCCTV_CLIENT2_MSGS_SIZE];		//msgs server

	TOOL_INT32 login_tick;					//登陆时间戳，上次连接服务器时间戳
	TOOL_INT32 login_state;					//登陆状态，调用loginUser之后=1，调用logoutUser之后=0
	hdcctv_cloud_user_t st_cloud_user;		//用户名和密码
	TOOL_INT32 list_state;					//查询设备列表状态，查询设备列表成功=1，未查询设备列表=0
	TOOL_INT32 list_result;					//查询设备列表结果，返回值
	hdcctv_device_list_t st_device_list;	//设备列表
}hdcctv_client2_t;

TOOL_VOID hdcctv_client2_relay_doState(hdcctv_client2_relay_t* relay, TOOL_INT32 type)
{
	if (type == relay->last_type)
		return;

	ptc_frame_head_t head;
	tool_mem_memset(&head, sizeof(ptc_frame_head_t));
	head.frame_type = type;
//	log_debug("type(%d)",type);
	tool_stream3_set_noDrop(&relay->stream3_recv, &head, sizeof(ptc_frame_head_t), NULL);
	relay->last_type = type;
	tool_sysf_usleep(100*1000);
}

TOOL_VOID hdcctv_client2_relay_init(hdcctv_client2_relay_t* relay, TOOL_INT32 relay_id, TOOL_VOID* father_client2)
{
	relay->relay_id = NC_PTC_QiHan_Cloud | relay_id;
	relay->father_client2 = father_client2;
	tool_thread_initMutex(&relay->mutex);
}

TOOL_VOID* hdcctv_client2_relay_runRecvRts(TOOL_VOID* param)
{	
	hdcctv_client2_relay_t* pst_relay = (hdcctv_client2_relay_t*)param;
	hdcctv_client2_t* pst_client2 = (hdcctv_client2_t*)pst_relay->father_client2;
	log_state("hdcctv_client2_relay_runRecvRts enter(%08x)", pst_relay->relay_id);
	TOOL_INT32 ret = 0;

	pst_relay->relay_tick = -12345678;
	tool_sock_setTick(&pst_relay->stream_tick);
	
	while (pst_relay->rts_state)
	{
		if (tool_sock_isTickTimeout(pst_relay->stream_tick, HDCCTV_CLIENT2_STREAM_TIMEOUT))
		{
			tool_sock_setTick(&pst_relay->stream_tick);
			hdcctv_client2_relay_doState(pst_relay, PTC_FRAME_STREAM_TIMEOUT);
		}
	
		if (pst_client2->loc_enable == 0)
		{
			tool_sysf_usleep(HDCCTV_CLIENT2_USLEEP_CYCLE);
			hdcctv_client2_relay_doState(pst_relay, PTC_FRAME_STREAM_POSTION);
			continue;
		}
		
		if (pst_relay->relay_enable == 0)
		{
			if (tool_sock_isTickTimeout(pst_relay->relay_tick, HDCCTV_CLIENT2_SERVICE_CYCLE) == 0)
			{
				tool_sysf_usleep(HDCCTV_CLIENT2_USLEEP_CYCLE);
				hdcctv_client2_relay_doState(pst_relay, PTC_FRAME_STREAM_WAIT);
				continue;
			}
			tool_sock_setTick(&pst_relay->relay_tick);

			hdcctv_client2_relay_doState(pst_relay, PTC_FRAME_STREAM_LOCATION);
			if (hdcctv_client_getServiceAddr(pst_client2->server_ip, pst_client2->server_port, &pst_relay->st_nc_dev, "relay", &pst_relay->relay_addr) < 0)
			{
				hdcctv_client2_relay_doState(pst_relay, PTC_FRAME_STREAM_LOCATION_FAIL);
				log_error("hdcctv_client_getServiceAddr");
				continue;
			}
			pst_relay->relay_enable = 1;
			log_state("hdcctv_client_getServiceAddr success");
		}

		if (tool_sock_item_isLive(&pst_relay->sock_item) == 0)
		{
			if (tool_sock_item_isTimeout(&pst_relay->sock_item, HDCCTV_CLIENT2_RELAY_CYCLE) == 0)
			{
				tool_sysf_usleep(HDCCTV_CLIENT2_USLEEP_CYCLE);
				hdcctv_client2_relay_doState(pst_relay, PTC_FRAME_STREAM_WAIT);
				continue;
			}

			hdcctv_client2_relay_doState(pst_relay, PTC_FRAME_STREAM_CONNECT);
			if (tool_sock_item_open_v2(&pst_relay->sock_item, TOOL_SOCK_TYPE1_TCP, TOOL_SOCK_TYPE2_CLIENT, pst_relay->relay_addr.ip, pst_relay->relay_addr.port, NULL, 0) < 0)
			{
				hdcctv_client2_relay_doState(pst_relay, PTC_FRAME_STREAM_CONNECT_FAIL);
				log_error("tool_sock_item_open_v2");
				continue;
			}

			hdcctv_client2_relay_doState(pst_relay, PTC_FRAME_STREAM_START);
			tool_mem_memset(&pst_relay->msg.head, sizeof(ptc_head_t));
			pst_relay->msg.head.cmd = PTC_CMD_HDCCTV_CLIENT_START_RELAY;
			pst_relay->msg.head.len = snprintf((TOOL_INT8*)pst_relay->msg.data, sizeof(pst_relay->msg.data), 
				"{"
					"\"username\":\"%s\","
					"\"password\":\"%s\","
					"\"nonce_md5\":\"%s\","
					"\"dev_id\":\"%s\","
					"\"ch_no\":\"%d\","
					"\"ch_type\":\"%d\""
				"}",
				pst_relay->st_nc_dev.user, "password", pst_relay->relay_addr.nonce_md5, pst_relay->st_nc_dev.dev_id, pst_relay->channel_no, pst_relay->stream_type);
			if (tool_ptc_qh_sendMsg(&pst_relay->sock_item, &pst_relay->msg) < 0)
			{
				hdcctv_client2_relay_doState(pst_relay, PTC_FRAME_STREAM_START_FAIL);
				log_error("tool_ptc_qh_sendMsg");
				tool_sock_item_close(&pst_relay->sock_item);
				continue;
			}
			if (tool_ptc_qh_recvMsg(&pst_relay->sock_item, &pst_relay->msg) < 0)
			{
				hdcctv_client2_relay_doState(pst_relay, PTC_FRAME_STREAM_START_FAIL);
				log_error("tool_ptc_qh_recvMsg");
				tool_sock_item_close(&pst_relay->sock_item);
				continue;
			}
			if (pst_relay->msg.head.cmd != PTC_CMD_HDCCTV_CLIENT_START_RELAY)
			{
				hdcctv_client2_relay_doState(pst_relay, PTC_FRAME_STREAM_START_FAIL);
				log_error("pst_relay->msg.head.cmd(0x%08x) data(%s)", pst_relay->msg.head.cmd, pst_relay->msg.data);
				tool_sock_item_close(&pst_relay->sock_item);
				continue;
			}
			if (tool_str_strstr((TOOL_INT8*)pst_relay->msg.data, "\"result\":\"0\"") == NULL)
			{
				if (tool_str_strstr((TOOL_INT8*)pst_relay->msg.data, "\"result\":\"30001\""))
				{
					hdcctv_client2_relay_doState(pst_relay, PTC_FRAME_STREAM_DEVICE_OFFLINE);
					log_error("set relay_enable 0 ");
					pst_relay->relay_enable = 0;
				}
				else
				{
					hdcctv_client2_relay_doState(pst_relay, PTC_FRAME_STREAM_START_FAIL);
					log_error("data(%s)", pst_relay->msg.data);
				}
				tool_sock_item_close(&pst_relay->sock_item);
				continue;
			}
			hdcctv_client2_relay_doState(pst_relay, PTC_FRAME_STREAM_RUNNING);
			log_debug("pst_relay->msg.head.cmd(0x%08x) data(%s)", pst_relay->msg.head.cmd, pst_relay->msg.data);
		}

		if (tool_sock_isTickTimeout(pst_relay->hb_tick, HDCCTV_CLIENT2_HEARTBEAT_CYCLE))
		{
			tool_sock_setTick(&pst_relay->hb_tick);
			tool_mem_memset(&pst_relay->msg.head, sizeof(ptc_head_t));
			pst_relay->msg.head.cmd = PTC_CMD_HDCCTV_CLIENT_HB_RELAY;
			if (tool_ptc_qh_sendMsg(&pst_relay->sock_item, &pst_relay->msg) < 0)
			{
				hdcctv_client2_relay_doState(pst_relay, PTC_FRAME_STREAM_BROKEN);
				log_error("tool_ptc_qh_sendMsg");
				tool_sock_item_close(&pst_relay->sock_item);
				continue;
			}
		}

		ret = tool_sock_item_pollRead(&pst_relay->sock_item, 1000);
		if (ret < 0)
		{
			hdcctv_client2_relay_doState(pst_relay, PTC_FRAME_STREAM_BROKEN);
			log_error("tool_sock_item_pollRead");
			tool_sock_item_close(&pst_relay->sock_item);
			continue;
		}
		else if (ret == 0)
			continue;

		if (tool_ptc_qh_recvFrame(&pst_relay->sock_item, pst_relay->frame) < 0)
		{
			hdcctv_client2_relay_doState(pst_relay, PTC_FRAME_STREAM_BROKEN);
			log_error("tool_ptc_qh_recvFrame");
			tool_sock_item_close(&pst_relay->sock_item);
			continue;
		}

		if (pst_relay->frame->head.cmd == PTC_CMD_HDCCTV_RELAY_RTS_CLIENT)
		{
//			ptc_frame_head_t* frame_head = (ptc_frame_head_t*)pst_relay->frame->data;
//			log_debug("no(%d) len(%d)", frame_head->no, frame_head->len);	
			tool_stream3_set_withDrop(&pst_relay->stream3_recv, pst_relay->frame->data, pst_relay->frame->head.len, pst_client2->buffer_usec);
		}
		else if (pst_relay->frame->head.cmd == PTC_CMD_HDCCTV_CLIENT_HB_RELAY)
		{
			log_state("PTC_CMD_HDCCTV_CLIENT_HB_RELAY");
		}
		else
		{
			log_state("pst_relay->frame->head.cmd(0x%08x)", pst_relay->frame->head.cmd);
		}
		tool_sock_setTick(&pst_relay->stream_tick);
//		tool_sysf_usleep(HDCCTV_CLIENT2_USLEEP_CYCLE);
	}

	if (tool_sock_item_isLive(&pst_relay->sock_item))
	{
		tool_mem_memset(&pst_relay->msg.head, sizeof(ptc_head_t));
		pst_relay->msg.head.cmd = PTC_CMD_HDCCTV_CLIENT_STOP_RELAY;
		pst_relay->msg.head.len = snprintf((TOOL_INT8*)pst_relay->msg.data, sizeof(pst_relay->msg.data), 
			"{"
				"\"ch_no\":\"%d\","
				"\"ch_type\":\"%d\""
			"}",
			pst_relay->channel_no, pst_relay->stream_type);
		tool_ptc_qh_sendMsg(&pst_relay->sock_item, &pst_relay->msg);
		tool_sock_item_close(&pst_relay->sock_item);
	}
	
	log_state("hdcctv_client2_relay_runRecvRts quit(%08x)", pst_relay->relay_id);
	return NULL;
}

TOOL_VOID* hdcctv_client2_relay_runPlayRts(TOOL_VOID* param)
{
	hdcctv_client2_relay_t* pst_relay = (hdcctv_client2_relay_t*)param;
	hdcctv_client2_t* pst_client2 = (hdcctv_client2_t*)pst_relay->father_client2;
	log_state("hdcctv_client2_relay_runPlayRts enter(0x%08x)", pst_relay->relay_id);
	TOOL_UINT32 len = 0;
	while (pst_relay->rts_state)
	{
		len = sizeof(ptc_frame_t);
		if (tool_stream3_get_withRts(&pst_relay->stream3_recv, &pst_relay->stream3_read_pos, pst_relay->stream3_frame_data, &len, pst_client2->buffer_usec) == 0)
		{
			tool_sysf_usleep(10*1000);
			continue;
		}

		if (pst_relay->cb)
			pst_relay->cb(pst_relay->relay_id, pst_relay->stream3_frame_data, len, pst_relay->param);
	}

	log_state("hdcctv_client2_relay_runPlayRts quit(0x%08x)", pst_relay->relay_id);
	return NULL;

}

TOOL_VOID* hdcctv_client2_relay_runRecvDevPbs(TOOL_VOID* param)
{	
	hdcctv_client2_relay_t* pst_relay = (hdcctv_client2_relay_t*)param;
	hdcctv_client2_t* pst_client2 = (hdcctv_client2_t*)pst_relay->father_client2;
	log_state("hdcctv_client2_relay_runRecvDevPbs enter(%08x)", pst_relay->relay_id);
	TOOL_INT32 ret = 0;
	TOOL_INT8 file_info[256] = {0};
	ptc_frame_head_t* frame_head = (ptc_frame_head_t*)pst_relay->frame->data;

	pst_relay->relay_tick = -12345678;
	tool_sock_setTick(&pst_relay->stream_tick);
	
	while (pst_relay->pbs_state)
	{
		if (tool_sock_isTickTimeout(pst_relay->stream_tick, HDCCTV_CLIENT2_STREAM_TIMEOUT))
		{
			tool_sock_setTick(&pst_relay->stream_tick);
			hdcctv_client2_relay_doState(pst_relay, PTC_FRAME_STREAM_TIMEOUT);
		}
	
		if (pst_client2->loc_enable == 0)
		{
			tool_sysf_usleep(HDCCTV_CLIENT2_USLEEP_CYCLE);
			hdcctv_client2_relay_doState(pst_relay, PTC_FRAME_STREAM_POSTION);
			continue;
		}
				
		if (pst_relay->relay_enable == 0)
		{
			if (tool_sock_isTickTimeout(pst_relay->relay_tick, HDCCTV_CLIENT2_SERVICE_CYCLE) == 0)
			{
				tool_sysf_usleep(HDCCTV_CLIENT2_USLEEP_CYCLE);
				hdcctv_client2_relay_doState(pst_relay, PTC_FRAME_STREAM_WAIT);
				continue;
			}
			tool_sock_setTick(&pst_relay->relay_tick);

			hdcctv_client2_relay_doState(pst_relay, PTC_FRAME_STREAM_LOCATION);
			if (hdcctv_client_getServiceAddr(pst_client2->server_ip, pst_client2->server_port, &pst_relay->st_nc_dev, "relay", &pst_relay->relay_addr) < 0)
			{
				hdcctv_client2_relay_doState(pst_relay, PTC_FRAME_STREAM_LOCATION_FAIL);
				log_error("hdcctv_client_getServiceAddr");
				continue;
			}
			pst_relay->relay_enable = 1;
			log_state("hdcctv_client_getServiceAddr success");
		}

		if (tool_sock_item_isLive(&pst_relay->sock_item) == 0)
		{
			if (tool_sock_item_isTimeout(&pst_relay->sock_item, HDCCTV_CLIENT2_RELAY_CYCLE) == 0)
			{
				tool_sysf_usleep(HDCCTV_CLIENT2_USLEEP_CYCLE);
				hdcctv_client2_relay_doState(pst_relay, PTC_FRAME_STREAM_WAIT);
				continue;
			}

			hdcctv_client2_relay_doState(pst_relay, PTC_FRAME_STREAM_CONNECT);
			if (tool_sock_item_open_v2(&pst_relay->sock_item, TOOL_SOCK_TYPE1_TCP, TOOL_SOCK_TYPE2_CLIENT, pst_relay->relay_addr.ip, pst_relay->relay_addr.port, NULL, 0) < 0)
			{
				hdcctv_client2_relay_doState(pst_relay, PTC_FRAME_STREAM_CONNECT_FAIL);
				log_error("tool_sock_item_open_v2");
				continue;
			}

			hdcctv_client2_relay_doState(pst_relay, PTC_FRAME_STREAM_START);
			base64_encode(pst_relay->st_record.file_info, file_info, sizeof(pst_relay->st_record.file_info));
		
			tool_mem_memset(&pst_relay->msg.head, sizeof(ptc_head_t));
			pst_relay->msg.head.cmd = PTC_CMD_HDCCTV_CLIENT_START_PBS;
			pst_relay->msg.head.len = snprintf((TOOL_INT8*)pst_relay->msg.data, sizeof(pst_relay->msg.data), 
				"{"
					"\"username\":\"%s\","
					"\"password\":\"%s\","
					"\"nonce_md5\":\"%s\","
					"\"dev_id\":\"%s\","
					"\"channel\":\"%d\","
					"\"file_info\":\"%s\","
					"\"start_time\":\"%d\","
					"\"end_time\":\"%d\""
				"}",
				pst_relay->st_nc_dev.user, "password", pst_relay->relay_addr.nonce_md5, pst_relay->st_nc_dev.dev_id, 
				pst_relay->st_record.channel, file_info, pst_relay->st_record.start_time, pst_relay->st_record.end_time);
			if (tool_ptc_qh_sendMsg(&pst_relay->sock_item, &pst_relay->msg) < 0)
			{
				hdcctv_client2_relay_doState(pst_relay, PTC_FRAME_STREAM_START_FAIL);
				log_error("tool_ptc_qh_sendMsg");
				tool_sock_item_close(&pst_relay->sock_item);
				continue;
			}
			if (tool_ptc_qh_recvMsg(&pst_relay->sock_item, &pst_relay->msg) < 0)
			{
				hdcctv_client2_relay_doState(pst_relay, PTC_FRAME_STREAM_START_FAIL);
				log_error("tool_ptc_qh_recvMsg");
				tool_sock_item_close(&pst_relay->sock_item);
				continue;
			}
			if (pst_relay->msg.head.cmd != PTC_CMD_HDCCTV_CLIENT_START_PBS)
			{
				hdcctv_client2_relay_doState(pst_relay, PTC_FRAME_STREAM_START_FAIL);
				log_error("pst_relay->msg.head.cmd(0x%08x) data(%s)", pst_relay->msg.head.cmd, pst_relay->msg.data);
				tool_sock_item_close(&pst_relay->sock_item);
				continue;
			}
			if (tool_str_strstr((TOOL_INT8*)pst_relay->msg.data, "\"result\":\"0\"") == NULL)
			{
				if (tool_str_strstr((TOOL_INT8*)pst_relay->msg.data, "\"result\":\"30001\""))
				{
					hdcctv_client2_relay_doState(pst_relay, PTC_FRAME_STREAM_DEVICE_OFFLINE);
					log_error("set relay_enable 0 ");
					pst_relay->relay_enable = 0;
				}
				else
				{
					log_error("data(%s)", pst_relay->msg.data);
					hdcctv_client2_relay_doState(pst_relay, PTC_FRAME_STREAM_START_FAIL);
				}
				tool_sock_item_close(&pst_relay->sock_item);
				continue;
			}

			if (tool_json2_getNumber(pst_relay->msg.data, "ch_no", &pst_relay->ch_no) < 0 ||
				tool_json2_getNumber(pst_relay->msg.data, "ch_type", &pst_relay->ch_type) < 0)
			{
				hdcctv_client2_relay_doState(pst_relay, PTC_FRAME_STREAM_START_FAIL);
				log_error("pst_relay->msg.data(%s)", pst_relay->msg.data);
				tool_sock_item_close(&pst_relay->sock_item);
				continue;
			}
			hdcctv_client2_relay_doState(pst_relay, PTC_FRAME_STREAM_RUNNING);
			log_debug("pst_relay->msg.head.cmd(0x%08x) data(%s)", pst_relay->msg.head.cmd, pst_relay->msg.data);
		}

		if (tool_sock_isTickTimeout(pst_relay->hb_tick, HDCCTV_CLIENT2_HEARTBEAT_CYCLE))
		{
			tool_sock_setTick(&pst_relay->hb_tick);
			tool_mem_memset(&pst_relay->msg.head, sizeof(ptc_head_t));
			pst_relay->msg.head.cmd = PTC_CMD_HDCCTV_CLIENT_HB_RELAY;
			if (tool_ptc_qh_sendMsg(&pst_relay->sock_item, &pst_relay->msg) < 0)
			{
				hdcctv_client2_relay_doState(pst_relay, PTC_FRAME_STREAM_BROKEN);
				log_error("tool_ptc_qh_sendMsg");
				tool_sock_item_close(&pst_relay->sock_item);
				continue;
			}
		}

		ret = tool_sock_item_pollRead(&pst_relay->sock_item, 1000);
		if (ret < 0)
		{
			hdcctv_client2_relay_doState(pst_relay, PTC_FRAME_STREAM_BROKEN);
			log_error("tool_sock_item_pollRead");
			tool_sock_item_close(&pst_relay->sock_item);
			continue;
		}
		else if (ret == 0)
			continue;

		if (tool_ptc_qh_recvFrame(&pst_relay->sock_item, pst_relay->frame) < 0)
		{
			hdcctv_client2_relay_doState(pst_relay, PTC_FRAME_STREAM_BROKEN);
			log_error("tool_ptc_qh_recvFrame");
			tool_sock_item_close(&pst_relay->sock_item);
			continue;
		}

		if (frame_head->frame_type == PTC_FRAME_PBS_END)
		{
			log_state("hdcctv_client2_relay_runRecvPbs end(%08x)", pst_relay->relay_id);
			break;
		}

		if (pst_relay->frame->head.cmd == PTC_CMD_HDCCTV_RELAY_PBS_CLIENT)
		{
//			ptc_frame_head_t* frame_head = (ptc_frame_head_t*)pst_relay->frame->data;
//			log_debug("no(%d) len(%d)", frame_head->no, frame_head->len);	
			tool_stream3_set_withDrop(&pst_relay->stream3_recv, pst_relay->frame->data, pst_relay->frame->head.len, pst_client2->buffer_usec);
		}
		else if (pst_relay->frame->head.cmd == PTC_CMD_HDCCTV_CLIENT_HB_RELAY)
		{
			log_state("PTC_CMD_HDCCTV_CLIENT_HB_RELAY");
		}
		else
		{
			log_state("pst_relay->frame->head.cmd(0x%08x)", pst_relay->frame->head.cmd);
		}
		tool_sock_setTick(&pst_relay->stream_tick);
		
//		tool_sysf_usleep(HDCCTV_CLIENT2_USLEEP_CYCLE);
	}

	if (tool_sock_item_isLive(&pst_relay->sock_item))
	{
		tool_mem_memset(&pst_relay->msg.head, sizeof(ptc_head_t));
		pst_relay->msg.head.cmd = PTC_CMD_HDCCTV_CLIENT_STOP_PBS;
		pst_relay->msg.head.len = snprintf((TOOL_INT8*)pst_relay->msg.data, sizeof(pst_relay->msg.data), 
			"{"
				"\"ch_no\":\"%d\","
				"\"ch_type\":\"%d\""
			"}",
			pst_relay->ch_no, pst_relay->ch_type);
		tool_ptc_qh_sendMsg(&pst_relay->sock_item, &pst_relay->msg);
		tool_ptc_qh_recvMsg(&pst_relay->sock_item, &pst_relay->msg);
		tool_sock_item_close(&pst_relay->sock_item);
	}
	
	log_state("hdcctv_client2_relay_runRecvDevPbs quit(%08x)", pst_relay->relay_id);
	return NULL;
}

TOOL_VOID* hdcctv_client2_relay_runRecvCloudPbs(TOOL_VOID* param)
{	
	hdcctv_client2_relay_t* pst_relay = (hdcctv_client2_relay_t*)param;
	hdcctv_client2_t* pst_client2 = (hdcctv_client2_t*)pst_relay->father_client2;
	log_state("hdcctv_client2_relay_runRecvCloudPbs enter(%08x)", pst_relay->relay_id);
	TOOL_INT32 ret = 0;
	ptc_frame_head_t* frame_head = (ptc_frame_head_t*)pst_relay->frame->data;

	pst_relay->relay_tick = -12345678;
	tool_sock_setTick(&pst_relay->stream_tick);
	
	while (pst_relay->pbs_state)
	{
		if (tool_sock_isTickTimeout(pst_relay->stream_tick, HDCCTV_CLIENT2_STREAM_TIMEOUT))
		{
			tool_sock_setTick(&pst_relay->stream_tick);
			hdcctv_client2_relay_doState(pst_relay, PTC_FRAME_STREAM_TIMEOUT);
		}
	
		if (pst_client2->loc_enable == 0)
		{
			tool_sysf_usleep(HDCCTV_CLIENT2_USLEEP_CYCLE);
			hdcctv_client2_relay_doState(pst_relay, PTC_FRAME_STREAM_POSTION);
			continue;
		}
				
		if (pst_relay->relay_enable == 0)
		{
			if (tool_sock_isTickTimeout(pst_relay->relay_tick, HDCCTV_CLIENT2_SERVICE_CYCLE) == 0)
			{
				tool_sysf_usleep(HDCCTV_CLIENT2_USLEEP_CYCLE);
				hdcctv_client2_relay_doState(pst_relay, PTC_FRAME_STREAM_WAIT);
				continue;
			}
			tool_sock_setTick(&pst_relay->relay_tick);

			hdcctv_client2_relay_doState(pst_relay, PTC_FRAME_STREAM_LOCATION);
			if (hdcctv_client_getServiceAddr(pst_client2->server_ip, pst_client2->server_port, &pst_relay->st_nc_dev, "relay", &pst_relay->relay_addr) < 0)
			{
				hdcctv_client2_relay_doState(pst_relay, PTC_FRAME_STREAM_LOCATION_FAIL);
				log_error("hdcctv_client_getServiceAddr");
				continue;
			}
			pst_relay->relay_enable = 1;
			log_state("hdcctv_client_getServiceAddr success");
		}

		if (tool_sock_item_isLive(&pst_relay->sock_item) == 0)
		{
			if (tool_sock_item_isTimeout(&pst_relay->sock_item, HDCCTV_CLIENT2_RELAY_CYCLE) == 0)
			{
				tool_sysf_usleep(HDCCTV_CLIENT2_USLEEP_CYCLE);
				hdcctv_client2_relay_doState(pst_relay, PTC_FRAME_STREAM_WAIT);
				continue;
			}

			hdcctv_client2_relay_doState(pst_relay, PTC_FRAME_STREAM_CONNECT);
			if (tool_sock_item_open_v2(&pst_relay->sock_item, TOOL_SOCK_TYPE1_TCP, TOOL_SOCK_TYPE2_CLIENT, pst_relay->relay_addr.ip, pst_relay->relay_addr.port, NULL, 0) < 0)
			{
				hdcctv_client2_relay_doState(pst_relay, PTC_FRAME_STREAM_CONNECT_FAIL);
				log_error("tool_sock_item_open_v2");
				continue;
			}

			hdcctv_client2_relay_doState(pst_relay, PTC_FRAME_STREAM_START);
		
			tool_mem_memset(&pst_relay->msg.head, sizeof(ptc_head_t));
			pst_relay->msg.head.cmd = PTC_CMD_HDCCTV_CLIENT_POS_CLOUD;
			pst_relay->msg.head.len = snprintf((TOOL_INT8*)pst_relay->msg.data, sizeof(pst_relay->msg.data), 
				"{"
					"\"username\":\"%s\","
					"\"password\":\"%s\","
					"\"nonce_md5\":\"%s\","
					"\"dev_id\":\"%s\","
					"\"channel\":\"%d\","
					"\"path\":\"%s\","
					"\"position\":\"%d\""
				"}",
				pst_relay->st_nc_dev.user, "password", pst_relay->relay_addr.nonce_md5, pst_relay->st_nc_dev.dev_id, 
				pst_relay->st_record.channel, pst_relay->st_record.file_info, pst_relay->st_record.start_time);
			if (tool_ptc_qh_sendMsg(&pst_relay->sock_item, &pst_relay->msg) < 0)
			{
				hdcctv_client2_relay_doState(pst_relay, PTC_FRAME_STREAM_START_FAIL);
				log_error("tool_ptc_qh_sendMsg");
				tool_sock_item_close(&pst_relay->sock_item);
				continue;
			}
			if (tool_ptc_qh_recvMsg(&pst_relay->sock_item, &pst_relay->msg) < 0)
			{
				hdcctv_client2_relay_doState(pst_relay, PTC_FRAME_STREAM_START_FAIL);
				log_error("tool_ptc_qh_recvMsg");
				tool_sock_item_close(&pst_relay->sock_item);
				continue;
			}
			if (pst_relay->msg.head.cmd != PTC_CMD_HDCCTV_CLIENT_POS_CLOUD)
			{
				hdcctv_client2_relay_doState(pst_relay, PTC_FRAME_STREAM_START_FAIL);
				log_error("pst_relay->msg.head.cmd(0x%08x) data(%s)", pst_relay->msg.head.cmd, pst_relay->msg.data);
				tool_sock_item_close(&pst_relay->sock_item);
				continue;
			}
			if (tool_str_strstr((TOOL_INT8*)pst_relay->msg.data, "\"result\":\"0\"") == NULL)
			{
	//			if (tool_str_strstr((TOOL_INT8*)pst_relay->msg.data, "\"result\":\"30001\""))
	//			{
	//				hdcctv_client2_relay_doState(pst_relay, PTC_FRAME_STREAM_DEVICE_OFFLINE);
	//				log_error("set relay_enable 0 ");
	//				pst_relay->relay_enable = 0;
	//			}
	//			else
				{
					log_error("data(%s)", pst_relay->msg.data);
					hdcctv_client2_relay_doState(pst_relay, PTC_FRAME_STREAM_START_FAIL);
				}
				tool_sock_item_close(&pst_relay->sock_item);
				continue;
			}

			hdcctv_client2_relay_doState(pst_relay, PTC_FRAME_STREAM_RUNNING);
			log_debug("pst_relay->msg.head.cmd(0x%08x) data(%s)", pst_relay->msg.head.cmd, pst_relay->msg.data);
		}

		if (tool_sock_isTickTimeout(pst_relay->hb_tick, HDCCTV_CLIENT2_HEARTBEAT_CYCLE))
		{
			tool_sock_setTick(&pst_relay->hb_tick);
			tool_mem_memset(&pst_relay->msg.head, sizeof(ptc_head_t));
			pst_relay->msg.head.cmd = PTC_CMD_HDCCTV_CLIENT_HB_RELAY;
			if (tool_ptc_qh_sendMsg(&pst_relay->sock_item, &pst_relay->msg) < 0)
			{
				hdcctv_client2_relay_doState(pst_relay, PTC_FRAME_STREAM_BROKEN);
				log_error("tool_ptc_qh_sendMsg");
				tool_sock_item_close(&pst_relay->sock_item);
				continue;
			}
		}

		ret = tool_sock_item_pollRead(&pst_relay->sock_item, 1000);
		if (ret < 0)
		{
			hdcctv_client2_relay_doState(pst_relay, PTC_FRAME_STREAM_BROKEN);
			log_error("tool_sock_item_pollRead");
			tool_sock_item_close(&pst_relay->sock_item);
			continue;
		}
		else if (ret == 0)
			continue;

		if (tool_ptc_qh_recvFrame(&pst_relay->sock_item, pst_relay->frame) < 0)
		{
			hdcctv_client2_relay_doState(pst_relay, PTC_FRAME_STREAM_BROKEN);
			log_error("tool_ptc_qh_recvFrame");
			tool_sock_item_close(&pst_relay->sock_item);
			continue;
		}

		if (frame_head->frame_type == PTC_FRAME_PBS_END)
		{
			log_state("hdcctv_client2_relay_runRecvPbs end(%08x)", pst_relay->relay_id);
			break;
		}

		if (pst_relay->frame->head.cmd == PTC_CMD_HDCCTV_CLOUD_PBS_CLOUD)
		{
//			ptc_frame_head_t* frame_head = (ptc_frame_head_t*)pst_relay->frame->data;
//			log_debug("no(%d) len(%d)", frame_head->no, frame_head->len);	
			tool_stream3_set_withDrop(&pst_relay->stream3_recv, pst_relay->frame->data, pst_relay->frame->head.len, pst_client2->buffer_usec);
		}
		else if (pst_relay->frame->head.cmd == PTC_CMD_HDCCTV_CLIENT_HB_RELAY)
		{
			log_state("PTC_CMD_HDCCTV_CLIENT_HB_RELAY");
		}
		else
		{
			log_state("pst_relay->frame->head.cmd(0x%08x)", pst_relay->frame->head.cmd);
		}
		tool_sock_setTick(&pst_relay->stream_tick);
		
//		tool_sysf_usleep(HDCCTV_CLIENT2_USLEEP_CYCLE);
	}

	if (tool_sock_item_isLive(&pst_relay->sock_item))
	{
		tool_mem_memset(&pst_relay->msg.head, sizeof(ptc_head_t));
		pst_relay->msg.head.cmd = PTC_CMD_HDCCTV_CLIENT_STOP_CLOUD;
		tool_ptc_qh_sendMsg(&pst_relay->sock_item, &pst_relay->msg);
		tool_ptc_qh_recvMsg(&pst_relay->sock_item, &pst_relay->msg);
		tool_sock_item_close(&pst_relay->sock_item);
	}
	
	log_state("hdcctv_client2_relay_runRecvCloudPbs quit(%08x)", pst_relay->relay_id);
	return NULL;
}

TOOL_VOID* hdcctv_client2_relay_runPlayPbs(TOOL_VOID* param)
{
	hdcctv_client2_relay_t* pst_relay = (hdcctv_client2_relay_t*)param;
	hdcctv_client2_t* pst_client2 = (hdcctv_client2_t*)pst_relay->father_client2;
	log_state("hdcctv_client2_relay_runPlayPbs enter(0x%08x)", pst_relay->relay_id);
	TOOL_UINT32 len = 0;
	while (pst_relay->pbs_state)
	{
		len = sizeof(ptc_frame_t);
		if (tool_stream3_get_withRts(&pst_relay->stream3_recv, &pst_relay->stream3_read_pos, pst_relay->stream3_frame_data, &len, pst_client2->buffer_usec) == 0)
		{
			tool_sysf_usleep(10*1000);
			continue;
		}

		if (pst_relay->cb)
			pst_relay->cb(pst_relay->relay_id, pst_relay->stream3_frame_data, len, pst_relay->param);
	}

	log_state("hdcctv_client2_relay_runPlayPbs quit(0x%08x)", pst_relay->relay_id);
	return NULL;
}

TOOL_VOID* hdcctv_client2_relay_runRecvTalk(TOOL_VOID* param)
{	
	hdcctv_client2_relay_t* pst_relay = (hdcctv_client2_relay_t*)param;
	hdcctv_client2_t* pst_client2 = (hdcctv_client2_t*)pst_relay->father_client2;
	log_state("hdcctv_client2_relay_runRecvTalk enter(%08x)", pst_relay->relay_id);
	TOOL_INT32 ret = 0;
	TOOL_INT32 flag = 0;
	ptc_frame_head_t head;
	pst_relay->relay_tick = -12345678;
	
	while (pst_relay->talk_state)
	{
		if (pst_client2->loc_enable == 0)
		{
			tool_sysf_usleep(HDCCTV_CLIENT2_USLEEP_CYCLE);
			continue;
		}
		
		if (pst_relay->relay_enable == 0)
		{
			if (tool_sock_isTickTimeout(pst_relay->relay_tick, HDCCTV_CLIENT2_SERVICE_CYCLE) == 0)
			{
				tool_sysf_usleep(HDCCTV_CLIENT2_USLEEP_CYCLE);
				continue;
			}
			tool_sock_setTick(&pst_relay->relay_tick);

			if (hdcctv_client_getServiceAddr(pst_client2->server_ip, pst_client2->server_port, &pst_relay->st_nc_dev, "relay", &pst_relay->relay_addr) < 0)
			{
				log_error("hdcctv_client_getServiceAddr");
				continue;
			}
			pst_relay->relay_enable = 1;
			log_state("hdcctv_client_getServiceAddr success");
		}

		if (tool_sock_item_isLive(&pst_relay->sock_item) == 0)
		{
			if (tool_sock_item_isTimeout(&pst_relay->sock_item, HDCCTV_CLIENT2_RELAY_CYCLE) == 0)
			{
				tool_sysf_usleep(HDCCTV_CLIENT2_USLEEP_CYCLE);
				continue;
			}

			if (tool_sock_item_open_v2(&pst_relay->sock_item, TOOL_SOCK_TYPE1_TCP, TOOL_SOCK_TYPE2_CLIENT, pst_relay->relay_addr.ip, pst_relay->relay_addr.port, NULL, 0) < 0)
			{
				log_error("tool_sock_item_open_v2");
				continue;
			}

			tool_mem_memset(&pst_relay->msg.head, sizeof(ptc_head_t));
			pst_relay->msg.head.cmd = PTC_CMD_HDCCTV_CLIENT_START_TALK;
			pst_relay->msg.head.len = snprintf((TOOL_INT8*)pst_relay->msg.data, sizeof(pst_relay->msg.data), 
				"{"
					"\"username\":\"%s\","
					"\"password\":\"%s\","
					"\"nonce_md5\":\"%s\","
					"\"dev_id\":\"%s\""
				"}",
				pst_relay->st_nc_dev.user, "password", pst_relay->relay_addr.nonce_md5, pst_relay->st_nc_dev.dev_id);
			if (tool_ptc_qh_sendMsg(&pst_relay->sock_item, &pst_relay->msg) < 0)
			{
				log_error("tool_ptc_qh_sendMsg");
				tool_sock_item_close(&pst_relay->sock_item);
				continue;
			}
			if (tool_ptc_qh_recvMsg(&pst_relay->sock_item, &pst_relay->msg) < 0)
			{
				log_error("tool_ptc_qh_recvMsg");
				tool_sock_item_close(&pst_relay->sock_item);
				continue;
			}
			if (pst_relay->msg.head.cmd != PTC_CMD_HDCCTV_CLIENT_START_TALK)
			{
				log_error("pst_relay->msg.head.cmd(0x%08x) data(%s)", pst_relay->msg.head.cmd, pst_relay->msg.data);
				tool_sock_item_close(&pst_relay->sock_item);
				continue;
			}
			if (tool_str_strstr((TOOL_INT8*)pst_relay->msg.data, "\"result\":\"0\"") == NULL)
			{
				if (tool_str_strstr((TOOL_INT8*)pst_relay->msg.data, "\"result\":\"30001\""))
				{
					log_error("set relay_enable 0 ");
					pst_relay->relay_enable = 0;
				}
				else if (tool_str_strstr((TOOL_INT8*)pst_relay->msg.data, "\"result\":\"10069\""))
				{
					if (pst_relay->cb)
					{
						tool_mem_memset(&head, sizeof(ptc_frame_head_t));
						head.frame_type = PTC_FRAME_STREAM_TALK_BUSY;
						pst_relay->cb(pst_relay->relay_id, &head, sizeof(ptc_frame_head_t), pst_relay->param);
					}
				}
				else
				{
					log_error("data(%s)", pst_relay->msg.data);
				}
				tool_sock_item_close(&pst_relay->sock_item);
				continue;
			}
			log_debug("pst_relay->msg.head.cmd(0x%08x) data(%s)", pst_relay->msg.head.cmd, pst_relay->msg.data);
		}

		flag = 0;
		tool_mem_memset(&pst_relay->msg.head, sizeof(ptc_head_t));
		pst_relay->msg.head.cmd = PTC_CMD_HDCCTV_CLIENT_TALK_RELAY;
		pst_relay->msg.head.channel_type = PTC_CH_TYPE_LOCAL_TALK;
		pst_relay->msg.head.channel_no = 0;
		pst_relay->msg.head.len = PTC_MSG_SIZE;
		if (tool_stream2_get(pst_relay->send_stream, &pst_relay->send_pos, pst_relay->msg.data, &pst_relay->msg.head.len))
		{
			if (tool_ptc_qh_sendMsg(&pst_relay->sock_item, &pst_relay->msg) < 0)
			{
				log_error("tool_ptc_qh_sendMsg");
				tool_sock_item_close(&pst_relay->sock_item);
				continue;
			}
			log_debug("tool_ptc_qh_sendMsg ... ");
			flag ++;
		}

		ret = tool_sock_item_pollRead(&pst_relay->sock_item, 0);
		if (ret < 0)
		{
			log_error("tool_sock_item_pollRead");
			tool_sock_item_close(&pst_relay->sock_item);
			continue;
		}
		else if (ret > 0)
		{
			if (tool_ptc_qh_recvMsg(&pst_relay->sock_item, &pst_relay->msg) < 0)
			{
				log_error("tool_ptc_qh_recvMsg");
				tool_sock_item_close(&pst_relay->sock_item);
				continue;
			}
			
			if (pst_relay->msg.head.cmd == PTC_CMD_HDCCTV_RELAY_TALK_CLIENT)
			{
				if (pst_relay->cb)
					pst_relay->cb(pst_relay->relay_id, pst_relay->msg.data, pst_relay->msg.head.len, pst_relay->param);
			}
			else
			{
				log_state("pst_relay->frame->head.cmd(0x%08x)", pst_relay->msg.head.cmd);
			}
			flag ++;
		}

		if (flag == 0)
			tool_sysf_usleep(HDCCTV_CLIENT2_USLEEP_CYCLE);
	}

	if (tool_sock_item_isLive(&pst_relay->sock_item))
	{
		tool_mem_memset(&pst_relay->msg.head, sizeof(ptc_head_t));
		pst_relay->msg.head.cmd = PTC_CMD_HDCCTV_CLIENT_STOP_TALK;
		tool_ptc_qh_sendMsg(&pst_relay->sock_item, &pst_relay->msg);
		tool_ptc_qh_recvMsg(&pst_relay->sock_item, &pst_relay->msg);
		tool_sock_item_close(&pst_relay->sock_item);
	}
	
	log_state("hdcctv_client2_relay_runRecvTalk quit(%08x)", pst_relay->relay_id);
	return NULL;
}

TOOL_INT32 hdcctv_client2_relay_startRts(hdcctv_client2_relay_t* pst_relay, nc_dev* pst_dev, TOOL_INT32 channel_no, TOOL_INT32 stream_type, NC_CB cb, TOOL_VOID* param)
{
	tool_mem_memcpy(&pst_relay->st_nc_dev, pst_dev, sizeof(nc_dev));
	pst_relay->channel_no = channel_no;
	pst_relay->stream_type = stream_type;
	pst_relay->cb = cb;
	pst_relay->param = param;
	pst_relay->frame = (ptc_frame_t*)tool_mem_malloc(sizeof(ptc_frame_t), 0);
	tool_stream3_init(&pst_relay->stream3_recv, HDCCTV_CLIENT2_RELAY_RTS_BUFFER);
	tool_stream3_initReadPos(&pst_relay->stream3_recv, &pst_relay->stream3_read_pos);
	pst_relay->stream3_frame_data = (ptc_frame_video_t*)tool_mem_malloc(sizeof(ptc_frame_video_t), 0);
	tool_sock_item_init(&pst_relay->sock_item);	
	tool_sock_item_setFlag(&pst_relay->sock_item, &pst_relay->rts_state);

	pst_relay->rts_state = 1;
	if (tool_thread_create(&pst_relay->recv_pid, NULL, hdcctv_client2_relay_runRecvRts, (TOOL_VOID*)pst_relay))
		log_fatalNo("tool_thread_create");
	if (tool_thread_create(&pst_relay->play_pid, NULL, hdcctv_client2_relay_runPlayRts, (TOOL_VOID*)pst_relay))
		log_fatalNo("tool_thread_create");
	return pst_relay->relay_id;	
}

TOOL_VOID hdcctv_client2_relay_stopRts(hdcctv_client2_relay_t* relay)
{
	if (relay->rts_state == 0)
		return;
	
	relay->rts_state = 0;
	tool_thread_join(relay->recv_pid);
	tool_thread_join(relay->play_pid);
	if (relay->frame)
	{
		tool_mem_free(relay->frame);
		relay->frame = NULL;
	}
	tool_stream3_done(&relay->stream3_recv);
	tool_mem_free(relay->stream3_frame_data);
	relay->stream3_frame_data = NULL;
	
/*	
	else if (relay->type == PTC_CMD_TALK_BASE)
	{
		tool_stream2_done(relay->audio_send);
		tool_mem_free(relay->audio_send);
		relay->audio_send = NULL;
	}
	else if (relay->type == PTC_CMD_PBS_BASE)
	{
		tool_thread_join(relay->play_pid);
		if (relay->frame)
		{
			tool_mem_free(relay->frame);
			relay->frame = NULL;
		}
		tool_stream3_done(&relay->stream3_recv);
		tool_mem_free(relay->stream3_frame_data);
		relay->stream3_frame_data = NULL;
	}
	else if (relay->type == PTC_CMD_CFG_BASE)
	{
	}
*/	
	relay->cb = NULL;
	relay->param = NULL;
	relay->is_used = 0;
}

TOOL_INT32 hdcctv_client2_relay_startDevPbs(hdcctv_client2_relay_t* pst_relay, nc_dev* pst_dev, ptc_cfg_record_t* pst_record, NC_CB cb, TOOL_VOID* param)
{
	tool_mem_memcpy(&pst_relay->st_nc_dev, pst_dev, sizeof(nc_dev));
	tool_mem_memcpy(&pst_relay->st_record, pst_record, sizeof(ptc_cfg_record_t));
	pst_relay->cb = cb;
	pst_relay->param = param;
	pst_relay->frame = (ptc_frame_t*)tool_mem_malloc(sizeof(ptc_frame_t), 0);
	tool_stream3_init(&pst_relay->stream3_recv, HDCCTV_CLIENT2_RELAY_PBS_BUFFER);
	tool_stream3_initReadPos(&pst_relay->stream3_recv, &pst_relay->stream3_read_pos);
	pst_relay->stream3_frame_data = (ptc_frame_video_t*)tool_mem_malloc(sizeof(ptc_frame_video_t), 0);
	tool_sock_item_init(&pst_relay->sock_item);	
	tool_sock_item_setFlag(&pst_relay->sock_item, &pst_relay->pbs_state);

	pst_relay->pbs_state = 1;
	if (tool_thread_create(&pst_relay->recv_pid, NULL, hdcctv_client2_relay_runRecvDevPbs, (TOOL_VOID*)pst_relay))
		log_fatalNo("tool_thread_create");
	if (tool_thread_create(&pst_relay->play_pid, NULL, hdcctv_client2_relay_runPlayPbs, (TOOL_VOID*)pst_relay))
		log_fatalNo("tool_thread_create");
	return pst_relay->relay_id;	
}

TOOL_VOID hdcctv_client2_relay_stopDevPbs(hdcctv_client2_relay_t* relay)
{
	if (relay->pbs_state == 0)
		return;
	
	relay->pbs_state = 0;
	tool_thread_join(relay->recv_pid);
	tool_thread_join(relay->play_pid);
	if (relay->frame)
	{
		tool_mem_free(relay->frame);
		relay->frame = NULL;
	}
	tool_stream3_done(&relay->stream3_recv);
	tool_mem_free(relay->stream3_frame_data);
	relay->stream3_frame_data = NULL;

	relay->cb = NULL;
	relay->param = NULL;
	relay->is_used = 0;
}

TOOL_INT32 hdcctv_client2_relay_startCloudPbs(hdcctv_client2_relay_t* pst_relay, nc_dev* pst_dev, ptc_cfg_record_t* pst_record, NC_CB cb, TOOL_VOID* param)
{
	tool_mem_memcpy(&pst_relay->st_nc_dev, pst_dev, sizeof(nc_dev));
	tool_mem_memcpy(&pst_relay->st_record, pst_record, sizeof(ptc_cfg_record_t));
	pst_relay->cb = cb;
	pst_relay->param = param;
	pst_relay->frame = (ptc_frame_t*)tool_mem_malloc(sizeof(ptc_frame_t), 0);
	tool_stream3_init(&pst_relay->stream3_recv, HDCCTV_CLIENT2_RELAY_PBS_BUFFER);
	tool_stream3_initReadPos(&pst_relay->stream3_recv, &pst_relay->stream3_read_pos);
	pst_relay->stream3_frame_data = (ptc_frame_video_t*)tool_mem_malloc(sizeof(ptc_frame_video_t), 0);
	tool_sock_item_init(&pst_relay->sock_item);	
	tool_sock_item_setFlag(&pst_relay->sock_item, &pst_relay->pbs_state);

	pst_relay->pbs_state = 1;
	if (tool_thread_create(&pst_relay->recv_pid, NULL, hdcctv_client2_relay_runRecvCloudPbs, (TOOL_VOID*)pst_relay))
		log_fatalNo("tool_thread_create");
	if (tool_thread_create(&pst_relay->play_pid, NULL, hdcctv_client2_relay_runPlayPbs, (TOOL_VOID*)pst_relay))
		log_fatalNo("tool_thread_create");
	return pst_relay->relay_id;	
}

TOOL_VOID hdcctv_client2_relay_stopCloudPbs(hdcctv_client2_relay_t* relay)
{
	if (relay->pbs_state == 0)
		return;
	
	relay->pbs_state = 0;
	tool_thread_join(relay->recv_pid);
	tool_thread_join(relay->play_pid);
	if (relay->frame)
	{
		tool_mem_free(relay->frame);
		relay->frame = NULL;
	}
	tool_stream3_done(&relay->stream3_recv);
	tool_mem_free(relay->stream3_frame_data);
	relay->stream3_frame_data = NULL;

	relay->cb = NULL;
	relay->param = NULL;
	relay->is_used = 0;
}

TOOL_INT32 hdcctv_client2_relay_startTalk(hdcctv_client2_relay_t* pst_relay, nc_dev* pst_dev, NC_CB cb, TOOL_VOID* param)
{
	tool_mem_memcpy(&pst_relay->st_nc_dev, pst_dev, sizeof(nc_dev));
	pst_relay->cb = cb;
	pst_relay->param = param;

	pst_relay->send_stream = (tool_stream2_t*)tool_mem_malloc(sizeof(tool_stream2_t), 0);
	tool_stream2_init(pst_relay->send_stream);
	tool_stream2_initReadPos(pst_relay->send_stream, &pst_relay->send_pos);

	tool_sock_item_init(&pst_relay->sock_item);	
	tool_sock_item_setFlag(&pst_relay->sock_item, &pst_relay->talk_state);

	pst_relay->talk_state = 1;
	if (tool_thread_create(&pst_relay->recv_pid, NULL, hdcctv_client2_relay_runRecvTalk, (TOOL_VOID*)pst_relay))
		log_fatalNo("tool_thread_create");
//	if (tool_thread_create(&pst_relay->play_pid, NULL, hdcctv_client2_relay_runSendTalk, (TOOL_VOID*)pst_relay))
//		log_fatalNo("tool_thread_create");
	return pst_relay->relay_id;	
}

TOOL_VOID hdcctv_client2_relay_stopTalk(hdcctv_client2_relay_t* relay)
{
	if (relay->talk_state == 0)
		return;
	
	relay->talk_state = 0;
	tool_thread_join(relay->recv_pid);
//	tool_thread_join(relay->play_pid);

	tool_stream2_done(relay->send_stream);
	tool_mem_free(relay->send_stream);
	relay->send_stream = NULL;

	relay->cb = NULL;
	relay->param = NULL;
	relay->is_used = 0;
}

TOOL_VOID hdcctv_client2_relay_done(hdcctv_client2_relay_t* relay)
{
	hdcctv_client2_relay_stopRts(relay);
	hdcctv_client2_relay_stopDevPbs(relay);
	hdcctv_client2_relay_stopTalk(relay);
	tool_thread_doneMutex(&relay->mutex);
}

TOOL_INT32 hdcctv_client2_relay_searchCloudDates(hdcctv_client2_relay_t* pst_relay, hdcctv_client2_t* pst_client2, nc_dev* pst_dev, ptc_cfg_dates_t* dates)
{
	tool_mem_memcpy(&pst_relay->st_nc_dev, pst_dev, sizeof(nc_dev));
	TOOL_INT32 channel = 0;
	TOOL_INT32 stream_type = 0;

	TOOL_INT32 i = 0;
	for (i = 0; i < 32; i++)
	{
		if ((TOOL_UINT64)(1<<i) & dates->channel)
		{
			channel = i;
			break;
		}
	}

	dates->num = 0;

	tool_sock_setTick(&pst_relay->stream_tick);
	while (1)
	{
		if (tool_sock_isTickTimeout(pst_relay->stream_tick, HDCCTV_CLIENT2_STREAM_TIMEOUT))
		{
			return NC_ERROR_CONNECT;
		}
	
		if (pst_client2->loc_enable)
		{
			break;
		}
		tool_sysf_usleep(HDCCTV_CLIENT2_USLEEP_CYCLE);
	}

	if (hdcctv_client_getServiceAddr(pst_client2->server_ip, pst_client2->server_port, &pst_relay->st_nc_dev, "relay", &pst_relay->relay_addr) < 0)
	{
		log_error("hdcctv_client_getServiceAddr");
		return NC_ERROR_CONNECT;
	}

	if (tool_sock_item_open_v2(&pst_relay->sock_item, TOOL_SOCK_TYPE1_TCP, TOOL_SOCK_TYPE2_CLIENT, pst_relay->relay_addr.ip, pst_relay->relay_addr.port, NULL, 0) < 0)
	{
		log_error("tool_sock_item_open_v2");
		return NC_ERROR_CONNECT;
	}

	TOOL_INT32 num = 0;
	TOOL_INT32 ret = 0;
	TOOL_INT32 array_x = 1024;
	TOOL_INT32 array_y = 256;
	TOOL_INT8* pch_array_value = (TOOL_INT8*)tool_mem_malloc(array_x*array_y, 0);	
	pst_relay->frame = (ptc_frame_t*)tool_mem_malloc(sizeof(ptc_frame_t), 0);
	do
	{
		tool_mem_memset(&pst_relay->frame->head, sizeof(ptc_head_t));
		pst_relay->frame->head.cmd = PTC_CMD_HDCCTV_CLIENT_SEARCH_DATE;
		pst_relay->frame->head.len = snprintf((TOOL_INT8*)pst_relay->frame->data, sizeof(pst_relay->frame->data), 
			"{"
				"\"username\":\"%s\","
				"\"password\":\"%s\","
				"\"nonce_md5\":\"%s\","
				"\"dev_id\":\"%s\","
				"\"channel\":\"%d\","
				"\"type\":\"%d\""
			"}",
			pst_relay->st_nc_dev.user, "password", pst_relay->relay_addr.nonce_md5, pst_relay->st_nc_dev.dev_id, channel, stream_type);
		if (tool_ptc_qh_sendFrame(&pst_relay->sock_item, pst_relay->frame) < 0)
		{
			log_error("tool_ptc_qh_sendFrame");
			ret = NC_ERROR_SEND;
			break;
		}
		if (tool_ptc_qh_recvFrame(&pst_relay->sock_item, pst_relay->frame) < 0)
		{
			log_error("tool_ptc_qh_recvFrame");
			ret = NC_ERROR_RECV;
			break;
		}
		if (pst_relay->frame->head.cmd != PTC_CMD_HDCCTV_CLIENT_SEARCH_DATE)
		{
			log_error("pst_relay->frame->head.cmd(0x%08x) data(%s)", pst_relay->frame->head.cmd, pst_relay->frame->data);
			ret = NC_ERROR_PROTOCOL;
			break;
		}
		if (tool_str_strstr((TOOL_INT8*)pst_relay->frame->data, "\"result\":\"0\"") == NULL)
		{
			log_debug("pst_relay->msg.data(%s)", pst_relay->frame->data);
			break;
		}

		num = tool_json2_getArray(pst_relay->frame->data, "date", pch_array_value, array_x, array_y);
		if (num < 0)
		{
			break;
		}
		for (i = 0; i < num ; i++)
		{
			if (dates->num >= PTC_RECORDS_SIZE)
				break;
			
			if (tool_json2_getNumber(&pch_array_value[i*array_y], "year", (TOOL_INT32*)&dates->date[dates->num].year) < 0 ||
				tool_json2_getNumber(&pch_array_value[i*array_y], "month", (TOOL_INT32*)&dates->date[dates->num].month) < 0 ||
				tool_json2_getNumber(&pch_array_value[i*array_y], "day", (TOOL_INT32*)&dates->date[dates->num].day) < 0)
			{
				log_error("tool_json2_getArray(%s)", pst_relay->frame->data);
				ret = NC_ERROR_PROTOCOL;
				break;
			}
			dates->num ++;
		}
	}while (0);
	tool_sock_item_close(&pst_relay->sock_item);
	tool_mem_free(pch_array_value);
	pch_array_value = NULL;
	tool_mem_free(pst_relay->frame);
	pst_relay->frame = NULL;
	return ret;
}

TOOL_INT32 hdcctv_client2_relay_searchCloudRecords(hdcctv_client2_relay_t* pst_relay, hdcctv_client2_t* pst_client2, nc_dev* pst_dev, ptc_cfg_records_t* records)
{
	tool_mem_memcpy(&pst_relay->st_nc_dev, pst_dev, sizeof(nc_dev));
	TOOL_INT32 channel = 0;
	TOOL_INT32 stream_type = 0;
	TOOL_INT32 i = 0;
	for (i = 0; i < 32; i++)
	{
		if ((TOOL_UINT64)(1<<i) & records->channel)
		{
			channel = i;
			break;
		}
	}
	records->num = 0;

	tool_sock_setTick(&pst_relay->stream_tick);
	while (1)
	{
		if (tool_sock_isTickTimeout(pst_relay->stream_tick, HDCCTV_CLIENT2_STREAM_TIMEOUT))
		{
			return NC_ERROR_CONNECT;
		}
	
		if (pst_client2->loc_enable)
		{
			break;
		}
		tool_sysf_usleep(HDCCTV_CLIENT2_USLEEP_CYCLE);
	}

	if (hdcctv_client_getServiceAddr(pst_client2->server_ip, pst_client2->server_port, &pst_relay->st_nc_dev, "relay", &pst_relay->relay_addr) < 0)
	{
		log_error("hdcctv_client_getServiceAddr");
		return NC_ERROR_CONNECT;
	}

	if (tool_sock_item_open_v2(&pst_relay->sock_item, TOOL_SOCK_TYPE1_TCP, TOOL_SOCK_TYPE2_CLIENT, pst_relay->relay_addr.ip, pst_relay->relay_addr.port, NULL, 0) < 0)
	{
		log_error("tool_sock_item_open_v2");
		return NC_ERROR_CONNECT;
	}

	TOOL_INT32 num = 0;
	TOOL_INT32 ret = 0;
	TOOL_INT32 array_x = 1024;
	TOOL_INT32 array_y = 256;
	TOOL_INT8* pch_array_value = (TOOL_INT8*)tool_mem_malloc(array_x*array_y, 0);	
	pst_relay->frame = (ptc_frame_t*)tool_mem_malloc(sizeof(ptc_frame_t), 0);
	do
	{
		tool_mem_memset(&pst_relay->frame->head, sizeof(ptc_head_t));
		pst_relay->frame->head.cmd = PTC_CMD_HDCCTV_CLIENT_SEARCH_CLOUD;
		pst_relay->frame->head.len = snprintf((TOOL_INT8*)pst_relay->frame->data, sizeof(pst_relay->frame->data), 
			"{"
				"\"username\":\"%s\","
				"\"password\":\"%s\","
				"\"nonce_md5\":\"%s\","
				"\"dev_id\":\"%s\","
				"\"channel\":\"%d\","
				"\"type\":\"%d\","
				"\"start\":\"%d\","
				"\"end\":\"%d\""
			"}",
			pst_relay->st_nc_dev.user, "password", pst_relay->relay_addr.nonce_md5, pst_relay->st_nc_dev.dev_id, channel, stream_type, records->start_time, records->end_time);
		if (tool_ptc_qh_sendFrame(&pst_relay->sock_item, pst_relay->frame) < 0)
		{
			log_error("tool_ptc_qh_sendFrame");
			ret = NC_ERROR_SEND;
			break;
		}
		if (tool_ptc_qh_recvFrame(&pst_relay->sock_item, pst_relay->frame) < 0)
		{
			log_error("tool_ptc_qh_recvFrame");
			ret = NC_ERROR_RECV;
			break;
		}
		if (pst_relay->frame->head.cmd != PTC_CMD_HDCCTV_CLIENT_SEARCH_CLOUD)
		{
			log_error("pst_relay->frame->head.cmd(0x%08x) data(%s)", pst_relay->frame->head.cmd, pst_relay->frame->data);
			ret = NC_ERROR_PROTOCOL;
			break;
		}
		if (tool_str_strstr((TOOL_INT8*)pst_relay->frame->data, "\"result\":\"0\"") == NULL)
		{
			log_debug("pst_relay->msg.data(%s)", pst_relay->frame->data);
			break;
		}

		num = tool_json2_getArray(pst_relay->frame->data, "list", pch_array_value, array_x, array_y);
		if (num < 0)
		{
			break;
		}
		for (i = 0; i < num ; i++)
		{
			if (records->num >= PTC_RECORDS_SIZE)
				break;
			
			records->record[records->num].channel = channel;
			records->record[records->num].type = records->type;
			records->record[records->num].sub_type = records->sub_type;
			if (tool_json2_getNumber(&pch_array_value[i*array_y], "start", (TOOL_INT32*)&records->record[records->num].start_time) < 0 ||
				tool_json2_getNumber(&pch_array_value[i*array_y], "end", (TOOL_INT32*)&records->record[records->num].end_time) < 0 ||
				tool_json2_getNumber(&pch_array_value[i*array_y], "size", (TOOL_INT32*)&records->record[records->num].size) < 0 ||
				tool_json2_getValue(&pch_array_value[i*array_y], "path", (TOOL_INT8*)records->record[records->num].file_info, 128) < 0)
			{
				log_error("tool_json2_getArray(%s)", pst_relay->frame->data);
				ret = NC_ERROR_PROTOCOL;
				break;
			}
			records->num ++;
		}
	}while (0);
	tool_sock_item_close(&pst_relay->sock_item);
	tool_mem_free(pch_array_value);
	pch_array_value = NULL;
	tool_mem_free(pst_relay->frame);
	pst_relay->frame = NULL;
	return ret;
}


#define __HDCCTV_CLIENT2_MSGS__

TOOL_INT32 hdcctv_client2_msgs_recvMsg(hdcctv_client2_msgs_t* msgs, TOOL_INT32 cmd)
{
	TOOL_INT32 ret = 0;
	while (1)
	{
		ret = tool_ptc_qh_recvMsg(&msgs->sock_item, &msgs->st_msg);
		if (ret < 0)
			break;
		
		if (msgs->st_msg.head.cmd == cmd || cmd == PTC_CMD_HDCCTV_BASE)
			break;		
		log_state("invalid cmd(0x%04x) cmd(0x%04x)", msgs->st_msg.head.cmd, cmd);
	}
	return ret;	
}

TOOL_INT32 hdcctv_client2_msgs_recvFrame(hdcctv_client2_msgs_t* msgs, TOOL_INT32 cmd)
{
	TOOL_INT32 ret = 0;
	while (1)
	{
		ret = tool_ptc_qh_recvFrame(&msgs->sock_item, msgs->pst_frame);
		if (ret < 0)
			break;
		
		if (msgs->pst_frame->head.cmd == cmd || cmd == PTC_CMD_HDCCTV_BASE)
			break;		
		log_state("invalid cmd(0x%04x) cmd(0x%04x)", msgs->pst_frame->head.cmd, cmd);
	}
	return ret;	
}

TOOL_VOID hdcctv_client2_msgs_init(hdcctv_client2_msgs_t* msgs, TOOL_INT32 msgs_id, TOOL_VOID* father_client2)
{
	msgs->msgs_id = NC_PTC_QiHan_Cloud | msgs_id;
	msgs->father_client2 = father_client2;
	tool_thread_initMutex(&msgs->mutex);
	tool_thread_initCond(&msgs->cond_dates);
	tool_thread_initCond(&msgs->cond_records);
	tool_thread_initCond(&msgs->cond_logs_v2);
	tool_thread_initCond(&msgs->cond_status);
	tool_thread_initCond(&msgs->cond_ability);
	tool_thread_initCond(&msgs->cond_get_alarm);
	tool_thread_initCond(&msgs->cond_set_alarm);
	tool_thread_initCond(&msgs->cond_wifi);
	tool_thread_initCond(&msgs->cond_devRecord);

	TOOL_INT32 i = 0; 
	for (i = 0; i < HDCCTV_CLIENT_CFG_NUM; i++)
	{
		tool_thread_initCond(&msgs->cfg[i].get_cond);
		tool_thread_initCond(&msgs->cfg[i].set_cond);
	}
}

TOOL_INT32 hdcctv_client2_msgs_heartbeat(hdcctv_client2_msgs_t* msgs)
{
	if (tool_sock_isTickTimeout(msgs->hb_tick, HDCCTV_CLIENT2_HEARTBEAT_CYCLE) == 0)
		return 0;
	tool_sock_setTick(&msgs->hb_tick);

	tool_mem_memset(&msgs->st_msg.head, sizeof(ptc_head_t));
	msgs->st_msg.head.cmd = PTC_CMD_HDCCTV_CLIENT_HB_MSGS;
	if (tool_ptc_qh_sendMsg(&msgs->sock_item, &msgs->st_msg) < 0)
	{
		return NC_ERROR_SEND;
	}
	if (hdcctv_client2_msgs_recvMsg(msgs, PTC_CMD_HDCCTV_CLIENT_HB_MSGS) < 0)
	{
		return NC_ERROR_RECV;
	}
	return 0;
}

TOOL_INT32 hdcctv_client2_msgs_login(hdcctv_client2_msgs_t* msgs)
{
	if (tool_sock_item_isLive(&msgs->sock_item))
		return 0;
	if (tool_sock_item_isTimeout(&msgs->sock_item, HDCCTV_CLIENT2_RECONNECT_CYCLE) == 0)
	{
		tool_sysf_usleep(20*1000);
		return -1;
	}

	TOOL_INT32 ret = 0;
	do
	{
		tool_sock_item_setFlag(&msgs->sock_item, &msgs->state);
		if (tool_sock_item_open_v2(&msgs->sock_item, TOOL_SOCK_TYPE1_TCP, TOOL_SOCK_TYPE2_CLIENT, msgs->msgs_addr.ip, msgs->msgs_addr.port, NULL, 0) < 0)
		{
			log_error("hdcctv_client2_msgs_login tool_sock_item_open");
			ret = NC_ERROR_CONNECT;
			break;
		}

		tool_mem_memset(&msgs->st_msg.head, sizeof(ptc_head_t));
		msgs->st_msg.head.cmd = PTC_CMD_HDCCTV_CLIENT_LOGIN_MSGS;
		msgs->st_msg.head.len = snprintf((TOOL_INT8*)msgs->st_msg.data, sizeof(msgs->st_msg.data),
			"{"
				"\"cmd\":\"%d\","
				"\"username\":\"%s\","
				"\"nonce_md5\":\"%s\","
				"\"dev_id\":\"%s\""
			"}",
			PTC_CMD_HDCCTV_CLIENT_LOGIN_MSGS, msgs->st_nc_dev.user, msgs->msgs_addr.nonce_md5, msgs->st_nc_dev.dev_id);
		if (tool_ptc_qh_sendMsg(&msgs->sock_item, &msgs->st_msg) < 0)
		{
			ret = NC_ERROR_SEND;
			break;
		}
		if (hdcctv_client2_msgs_recvMsg(msgs, PTC_CMD_HDCCTV_CLIENT_LOGIN_MSGS) < 0)
		{
			ret = NC_ERROR_RECV;
			break;
		}

		tool_sock_setTick(&msgs->hb_tick);
		log_state("hdcctv_client2_msgs_login user(%s)", msgs->st_nc_dev.user);
	} while (0);
	if (ret < 0)
	{
        if (tool_sock_item_isLive(&msgs->sock_item))
			tool_sock_item_close(&msgs->sock_item);
    }

	return ret;
}

TOOL_INT32 hdcctv_client2_msgs_logout(hdcctv_client2_msgs_t* msgs)
{	
	tool_mem_memset(&msgs->st_msg.head, sizeof(ptc_head_t));
	msgs->st_msg.head.cmd = PTC_CMD_HDCCTV_CLIENT_LOGOUT_MSGS;
	if (tool_ptc_qh_sendMsg(&msgs->sock_item, &msgs->st_msg) < 0)
		log_error("tool_ptc_qh_sendMsg");
	if (tool_sock_item_isLive(&msgs->sock_item))
		tool_sock_item_close(&msgs->sock_item);
	return 0;
}

TOOL_INT32 hdcctv_client2_msgs_doGetAbility(hdcctv_client2_msgs_t* msgs)
{
	ptc_cfg_ability_t cfg_get_ability;
	tool_thread_lockMutex(&msgs->mutex);
	if (msgs->last_get_ability_flag == msgs->cur_get_ability_flag)
	{
		tool_thread_unlockMutex(&msgs->mutex);
		return 0;
	}
	tool_mem_memcpy(&cfg_get_ability, &msgs->cfg_get_ability, sizeof(ptc_cfg_ability_t));
	tool_thread_unlockMutex(&msgs->mutex);	

	TOOL_INT32 ret = 0;
	TOOL_INT32 result = 0;
	msgs->pst_frame = (ptc_frame_t*)tool_mem_malloc(sizeof(ptc_frame_t), 0);
	do
	{
		tool_mem_memset(&msgs->pst_frame->head, sizeof(ptc_head_t));
		msgs->pst_frame->head.cmd = PTC_CMD_SEARCH2_ABILITY;		
		tool_ptc_qh_c2d_encode_getAbility(msgs->pst_frame);
		if (tool_ptc_qh_sendFrame(&msgs->sock_item, msgs->pst_frame) < 0)
		{
			log_error("tool_ptc_qh_sendMsg");
			result = NC_ERROR_SEND;
			ret = -1;
			break;
		}
		if (hdcctv_client2_msgs_recvFrame(msgs, PTC_CMD_SEARCH2_ABILITY) < 0)
		{
			log_error("hdcctv_client2_msgs_recvMsg");
			result = NC_ERROR_RECV;
			ret = -1;
			break;
		}
		result = tool_ptc_qh_d2c_decode_getAbility(msgs->pst_frame, &cfg_get_ability);
		if (result < 0)
		{
			log_error("tool_ptc_qh_d2c_decode_getAbility");
			break;
		}
	} while (0);
	tool_mem_free(msgs->pst_frame);
	msgs->pst_frame = NULL;

	tool_thread_lockMutex(&msgs->mutex);
	tool_mem_memcpy(&msgs->cfg_get_ability, &cfg_get_ability, sizeof(ptc_cfg_ability_t));
	msgs->result_ability = result;
	msgs->last_get_ability_flag = msgs->cur_get_ability_flag;
	tool_thread_broadcastCond(&msgs->cond_status);
	tool_thread_unlockMutex(&msgs->mutex);
	return ret; 
}

TOOL_INT32 hdcctv_client2_msgs_doPtz(hdcctv_client2_msgs_t* msgs)
{
	TOOL_INT32 ptz_channel_no;
	TOOL_INT32 ptz_cmd;
	TOOL_INT32 ptz_param;

	tool_thread_lockMutex(&msgs->mutex);
	if (msgs->last_ptz_req == msgs->cur_ptz_req)
	{
		tool_thread_unlockMutex(&msgs->mutex);
		return 0;
	}
	ptz_channel_no = msgs->ptz_channel;
	ptz_cmd = msgs->ptz_cmd;
	ptz_param = msgs->ptz_param;
	tool_thread_unlockMutex(&msgs->mutex);


	tool_mem_memset(&msgs->st_msg, sizeof(ptc_msg_t));
	msgs->st_msg.head.cmd = PTC_CMD_CFG2_CTRL_PTZ;		
	tool_ptc_qh_c2d_encode_ctrlPtz(&msgs->st_msg, ptz_channel_no, (PTC_PTZ_CMD_E)ptz_cmd, ptz_param);
	
	if (tool_ptc_qh_sendMsg(&msgs->sock_item, &msgs->st_msg) < 0)
	{
		log_error("tool_ptc_qh_sendMsg");
		return -1;
	}

	msgs->last_ptz_req = msgs->cur_ptz_req;
	log_state("cmd(%d) channel(%d) param(%d)", ptz_cmd, ptz_channel_no, ptz_param);
	return 0;
}

TOOL_INT32 hdcctv_client2_msgs_doDates(hdcctv_client2_msgs_t* msgs)
{
	ptc_cfg_dates_t dates;
	tool_thread_lockMutex(&msgs->mutex);
	if (msgs->last_dates_req == msgs->cur_dates_req)
	{
		tool_thread_unlockMutex(&msgs->mutex);
		return 0;
	}
	tool_mem_memcpy(&dates, &msgs->cfg_dates, sizeof(ptc_cfg_dates_t));
	tool_thread_unlockMutex(&msgs->mutex);	

	TOOL_INT32 result = 0;
	TOOL_INT32 ret = 0;
	msgs->pst_frame = (ptc_frame_t*)tool_mem_malloc(sizeof(ptc_frame_t), 0);
	do
	{
		tool_mem_memset(&msgs->pst_frame->head, sizeof(ptc_head_t));
		msgs->pst_frame->head.cmd = PTC_CMD_SEARCH2_DATES;		
		tool_ptc_qh_c2d_encode_searchDates(msgs->pst_frame, &dates);
		if (tool_ptc_qh_sendFrame(&msgs->sock_item, msgs->pst_frame) < 0)
		{
			log_error("tool_ptc_qh_sendFrame");
			result = NC_ERROR_SEND;
			ret = -1;
			break;
		}
		if (hdcctv_client2_msgs_recvFrame(msgs, PTC_CMD_SEARCH2_DATES) < 0)
		{
			log_error("hdcctv_client2_msgs_recvFrame");
			result = NC_ERROR_RECV;
			ret = -1;
			break;
		}
		result = tool_ptc_qh_d2c_decode_searchDates(msgs->pst_frame, &dates);
		if (result < 0)
		{
			log_error("tool_ptc_qh_d2c_decode_searchDates");
			break;
		}
	} while (0);
	tool_mem_free(msgs->pst_frame);
	msgs->pst_frame = NULL;

	tool_thread_lockMutex(&msgs->mutex);
	tool_mem_memcpy(&msgs->cfg_dates, &dates, sizeof(ptc_cfg_dates_t));
	msgs->result_dates = result;
	msgs->last_dates_req = msgs->cur_dates_req;
	tool_thread_broadcastCond(&msgs->cond_dates);
	tool_thread_unlockMutex(&msgs->mutex);
	return ret; 
}

TOOL_INT32 hdcctv_client2_msgs_doRecords(hdcctv_client2_msgs_t* msgs)
{
	ptc_cfg_records_t records;
	tool_thread_lockMutex(&msgs->mutex);
	if (msgs->last_records_req == msgs->cur_records_req)
	{
		tool_thread_unlockMutex(&msgs->mutex);
		return 0;
	}
	tool_mem_memcpy(&records, &msgs->cfg_records, sizeof(ptc_cfg_records_t));
	tool_thread_unlockMutex(&msgs->mutex);	

	TOOL_INT32 result = 0;
	TOOL_INT32 ret = 0;
	msgs->pst_frame = (ptc_frame_t*)tool_mem_malloc(sizeof(ptc_frame_t), 0);
	do
	{
		tool_mem_memset(&msgs->pst_frame->head, sizeof(ptc_head_t));
		msgs->pst_frame->head.cmd = PTC_CMD_SEARCH2_RECORDS;		
		tool_ptc_qh_c2d_encode_getRecords(msgs->pst_frame, &records);
		if (tool_ptc_qh_sendFrame(&msgs->sock_item, msgs->pst_frame) < 0)
		{
			log_error("tool_ptc_qh_sendFrame");
			result = NC_ERROR_SEND;
			ret = -1;
			break;
		}
		if (hdcctv_client2_msgs_recvFrame(msgs, PTC_CMD_SEARCH2_RECORDS) < 0)
		{
			log_error("hdcctv_client2_msgs_recvFrame");
			result = NC_ERROR_RECV;
			ret = -1;
			break;
		}
		result = tool_ptc_qh_d2c_decode_getRecords(msgs->pst_frame, &records);
		if (result < 0)
		{
			log_error("tool_ptc_qh_d2c_decode_getRecords");
			break;
		}
	} while (0);
	tool_mem_free(msgs->pst_frame);
	msgs->pst_frame = NULL;

	tool_thread_lockMutex(&msgs->mutex);
	tool_mem_memcpy(&msgs->cfg_records, &records, sizeof(ptc_cfg_records_t));
	msgs->result_records = result;
	msgs->last_records_req = msgs->cur_records_req;
	tool_thread_broadcastCond(&msgs->cond_records);
	tool_thread_unlockMutex(&msgs->mutex);
	return ret; 
}

TOOL_INT32 hdcctv_client2_msgs_doLogs(hdcctv_client2_msgs_t* msgs)
{
	ptc_logs_v2_t logs_v2;
	tool_thread_lockMutex(&msgs->mutex);
	if (msgs->last_logs_v2_req == msgs->cur_logs_v2_req)
	{
		tool_thread_unlockMutex(&msgs->mutex);
		return 0;
	}
	tool_mem_memcpy(&logs_v2, &msgs->cfg_logs_v2, sizeof(ptc_logs_v2_t));
	tool_thread_unlockMutex(&msgs->mutex);	

	TOOL_INT32 ret = 0;
	TOOL_INT32 result = 0;
	msgs->pst_frame = (ptc_frame_t*)tool_mem_malloc(sizeof(ptc_frame_t), 0);
	do
	{
		tool_mem_memset(&msgs->pst_frame->head, sizeof(ptc_head_t));
		msgs->pst_frame->head.cmd = PTC_CMD_SEARCH2_LOGS;		
		tool_ptc_qh_c2d_encode_getLogs(msgs->pst_frame, &logs_v2);
		if (tool_ptc_qh_sendFrame(&msgs->sock_item, msgs->pst_frame) < 0)
		{
			log_error("tool_ptc_qh_sendFrame");
			result = NC_ERROR_SEND;
			ret = -1;
			break;
		}
		if (hdcctv_client2_msgs_recvFrame(msgs, PTC_CMD_SEARCH2_LOGS) < 0)
		{
			log_error("hdcctv_client2_msgs_recvFrame");
			result = NC_ERROR_RECV;
			ret = -1;
			break;
		}
		result = tool_ptc_qh_d2c_decode_getLogs(msgs->pst_frame, &logs_v2);
		if (result < 0)
		{
			log_error("tool_ptc_qh_d2c_decode_getLogs");
			break;
		}
	} while (0);
	tool_mem_free(msgs->pst_frame);
	msgs->pst_frame = NULL;

	tool_thread_lockMutex(&msgs->mutex);
	tool_mem_memcpy(&msgs->cfg_logs_v2, &logs_v2, sizeof(ptc_logs_v2_t));
	msgs->result_logs_v2 = result;
	msgs->last_logs_v2_req = msgs->cur_logs_v2_req;
	tool_thread_broadcastCond(&msgs->cond_logs_v2);
	tool_thread_unlockMutex(&msgs->mutex);
	return ret;
}

TOOL_INT32 hdcctv_client2_msgs_doSyncTime(hdcctv_client2_msgs_t* msgs)
{
	TOOL_UINT32 utc_sec;
	TOOL_INT32 zone;
	TOOL_INT32 dst_hour;
	tool_thread_lockMutex(&msgs->mutex);
	if (msgs->last_time_req == msgs->cur_time_req)
	{
		tool_thread_unlockMutex(&msgs->mutex);
		return 0;
	}
	utc_sec = msgs->utc_sec;
	zone = msgs->zone;
	dst_hour = msgs->dst_hour;
	tool_thread_unlockMutex(&msgs->mutex);

	TOOL_INT32 result = 0;
	TOOL_INT32 ret = 0;
	do
	{
		tool_mem_memset(&msgs->st_msg.head, sizeof(ptc_head_t));
		msgs->st_msg.head.cmd = PTC_CMD_CFG2_SYNC_TIME;		
		tool_ptc_qh_c2d_encode_syncTime(&msgs->st_msg, utc_sec, zone, dst_hour);
		if (tool_ptc_qh_sendMsg(&msgs->sock_item, &msgs->st_msg) < 0)
		{
			log_error("tool_ptc_qh_sendMsg");
			result = NC_ERROR_SEND;
			ret = -1;
			break;
		}
	} while (0);

	msgs->last_time_req = msgs->cur_time_req;
	return ret;
}

TOOL_INT32 hdcctv_client2_msgs_doGetStatus(hdcctv_client2_msgs_t* msgs)
{
	ptc_cfg_status_t cfg_status;
	tool_thread_lockMutex(&msgs->mutex);
	if (msgs->last_get_status_req == msgs->cur_get_status_req)
	{
		tool_thread_unlockMutex(&msgs->mutex);
		return 0;
	}
	tool_mem_memcpy(&cfg_status, &msgs->cfg_get_status, sizeof(ptc_cfg_status_t));
	tool_thread_unlockMutex(&msgs->mutex);	

	TOOL_INT32 ret = 0;
	TOOL_INT32 result = 0;
	do
	{
		tool_mem_memset(&msgs->st_msg.head, sizeof(ptc_head_t));
		msgs->st_msg.head.cmd = PTC_CMD_CFG2_GET_STATUS;		
		tool_ptc_qh_c2d_encode_getStatus(&msgs->st_msg);
		if (tool_ptc_qh_sendMsg(&msgs->sock_item, &msgs->st_msg) < 0)
		{
			log_error("tool_ptc_qh_sendMsg");
			result = NC_ERROR_SEND;
			ret = -1;
			break;
		}
		if (hdcctv_client2_msgs_recvMsg(msgs, PTC_CMD_CFG2_GET_STATUS) < 0)
		{
			log_error("hdcctv_client2_msgs_recvMsg");
			result = NC_ERROR_RECV;
			ret = -1;
			break;
		}
		result = tool_ptc_qh_d2c_decode_getStatus(&msgs->st_msg, &cfg_status);
		if (result < 0)
		{
			log_error("tool_ptc_qh_d2c_decode_getStatus(%d)", result);
			break;
		}
	} while (0);

	tool_thread_lockMutex(&msgs->mutex);
	tool_mem_memcpy(&msgs->cfg_get_status, &cfg_status, sizeof(ptc_cfg_status_t));
	msgs->result_status = result;
	msgs->last_get_status_req = msgs->cur_get_status_req;
	tool_thread_broadcastCond(&msgs->cond_status);
	tool_thread_unlockMutex(&msgs->mutex);
	return ret; 
}

TOOL_INT32 hdcctv_client2_msgs_doSetStatus(hdcctv_client2_msgs_t* msgs)
{
	ptc_cfg_status_t cfg_status;
	tool_thread_lockMutex(&msgs->mutex);
	if (msgs->last_set_status_req == msgs->cur_set_status_req)
	{
		tool_thread_unlockMutex(&msgs->mutex);
		return 0;
	}
	tool_mem_memcpy(&cfg_status, &msgs->cfg_set_status, sizeof(ptc_cfg_status_t));
	tool_thread_unlockMutex(&msgs->mutex);

	TOOL_INT32 result = 0;
	TOOL_INT32 ret = 0;
	do
	{
		tool_mem_memset(&msgs->st_msg.head, sizeof(ptc_head_t));
		msgs->st_msg.head.cmd = PTC_CMD_CFG2_SET_STATUS;		
		tool_ptc_qh_c2d_encode_setStatus(&msgs->st_msg, &cfg_status);
		if (tool_ptc_qh_sendMsg(&msgs->sock_item, &msgs->st_msg) < 0)
		{
			log_error("tool_ptc_qh_sendMsg");
			result = NC_ERROR_SEND;
			ret = -1;
			break;
		}
		if (hdcctv_client2_msgs_recvMsg(msgs, PTC_CMD_CFG2_SET_STATUS) < 0)
		{
			log_error("hdcctv_client2_msgs_recvMsg");
			result = NC_ERROR_RECV;
			ret = -1;
			break;
		}
		result = tool_ptc_qh_d2c_decode_setStatus(&msgs->st_msg);
		if (result < 0)
		{
			log_error("tool_ptc_qh_d2c_decode_setStatus");
			break;
		}
	} while (0);

	msgs->last_set_status_req = msgs->cur_set_status_req;
	return ret;
}

TOOL_INT32 hdcctv_client2_msgs_doGetAlarm(hdcctv_client2_msgs_t* msgs)
{
	ptc_cfg_alarm_t cfg_alarm;
	PTC_ALARM_TYPE_E cfg_type;
	tool_thread_lockMutex(&msgs->mutex);
	if (msgs->last_get_alarm_req == msgs->cur_get_alarm_req)
	{
		tool_thread_unlockMutex(&msgs->mutex);
		return 0;
	}
	tool_mem_memcpy(&cfg_alarm, &msgs->cfg_get_alarm, sizeof(ptc_cfg_alarm_t));
	cfg_type = msgs->cfg_get_type;
	tool_thread_unlockMutex(&msgs->mutex);	

	TOOL_INT32 result = 0;
	TOOL_INT32 ret = 0;
	do
	{
		tool_mem_memset(&msgs->st_msg.head, sizeof(ptc_head_t));
		msgs->st_msg.head.cmd = PTC_CMD_CFG2_GET_ALARM;		
		tool_ptc_qh_c2d_encode_getAlarm(&msgs->st_msg, cfg_type);
		if (tool_ptc_qh_sendMsg(&msgs->sock_item, &msgs->st_msg) < 0)
		{
			log_error("tool_ptc_qh_sendMsg");
			result = NC_ERROR_SEND;
			ret = -1;
			break;
		}
		if (hdcctv_client2_msgs_recvMsg(msgs, PTC_CMD_CFG2_GET_ALARM) < 0)
		{
			log_error("hdcctv_client2_msgs_recvMsg");
			result = NC_ERROR_RECV;
			ret = -1;
			break;
		}
		result = tool_ptc_qh_d2c_decode_getAlarm(&msgs->st_msg, &cfg_alarm);
		if (result < 0)
		{
			log_error("tool_ptc_qh_d2c_decode_getAlarm");
			break;
		}
	} while (0);

	tool_thread_lockMutex(&msgs->mutex);
	tool_mem_memcpy(&msgs->cfg_get_alarm, &cfg_alarm, sizeof(ptc_cfg_alarm_t));
	msgs->result_get_alarm = result;
	msgs->last_get_alarm_req = msgs->cur_get_alarm_req;
	tool_thread_broadcastCond(&msgs->cond_get_alarm);
	tool_thread_unlockMutex(&msgs->mutex);
	return ret; 
}

TOOL_INT32 hdcctv_client2_msgs_doSetAlarm(hdcctv_client2_msgs_t* msgs)
{
	ptc_cfg_alarm_t cfg_alarm;
	PTC_ALARM_TYPE_E cfg_type;
	tool_thread_lockMutex(&msgs->mutex);
	if (msgs->last_set_alarm_req == msgs->cur_set_alarm_req)
	{
		tool_thread_unlockMutex(&msgs->mutex);
		return 0;
	}
	tool_mem_memcpy(&cfg_alarm, &msgs->cfg_set_alarm, sizeof(ptc_cfg_alarm_t));
	cfg_type = msgs->cfg_set_type;
	tool_thread_unlockMutex(&msgs->mutex);

	TOOL_INT32 result = 0;
	TOOL_INT32 ret = 0;
	do
	{
		tool_mem_memset(&msgs->st_msg.head, sizeof(ptc_head_t));
		msgs->st_msg.head.cmd = PTC_CMD_CFG2_SET_ALARM;		
		tool_ptc_qh_c2d_encode_setAlarm(&msgs->st_msg, cfg_type, &cfg_alarm);
		if (tool_ptc_qh_sendMsg(&msgs->sock_item, &msgs->st_msg) < 0)
		{
			log_error("tool_ptc_qh_sendMsg");
			result = NC_ERROR_SEND;
			ret = -1;
			break;
		}
		if (hdcctv_client2_msgs_recvMsg(msgs, PTC_CMD_CFG2_SET_ALARM) < 0)
		{
			log_error("hdcctv_client2_msgs_recvMsg");
			result = NC_ERROR_RECV;
			ret = -1;
			break;
		}
		result = tool_ptc_qh_d2c_decode_setAlarm(&msgs->st_msg);
		if (result < 0)
		{
			log_error("tool_ptc_qh_d2c_decode_setAlarm");
			break;
		}
	} while (0);

	tool_thread_lockMutex(&msgs->mutex);
	tool_mem_memcpy(&msgs->cfg_set_alarm, &cfg_alarm, sizeof(ptc_cfg_alarm_t));
	msgs->result_set_alarm = result;
	msgs->last_set_alarm_req = msgs->cur_set_alarm_req;
	tool_thread_broadcastCond(&msgs->cond_set_alarm);
	tool_thread_unlockMutex(&msgs->mutex);
	return ret;
}

TOOL_INT32 hdcctv_client2_msgs_doGetWifi(hdcctv_client2_msgs_t* msgs)
{
	ptc_cfg_wifi_t cfg_wifi;
	tool_thread_lockMutex(&msgs->mutex);
	if (msgs->last_get_wifi_req == msgs->cur_get_wifi_req)
	{
		tool_thread_unlockMutex(&msgs->mutex);
		return 0;
	}
	tool_mem_memcpy(&cfg_wifi, &msgs->cfg_get_wifi, sizeof(ptc_cfg_wifi_t));
	tool_thread_unlockMutex(&msgs->mutex);	

	TOOL_INT32 result = 0;
	TOOL_INT32 ret = 0;
	do
	{
		tool_mem_memset(&msgs->st_msg.head, sizeof(ptc_head_t));
		msgs->st_msg.head.cmd = PTC_CMD_CFG2_GET_WIFI;		
		tool_ptc_qh_c2d_encode_getWifi(&msgs->st_msg);
		if (tool_ptc_qh_sendMsg(&msgs->sock_item, &msgs->st_msg) < 0)
		{
			log_error("tool_ptc_qh_sendMsg");
			result = NC_ERROR_SEND;
			ret = -1;
			break;
		}
		if (hdcctv_client2_msgs_recvMsg(msgs, PTC_CMD_CFG2_GET_WIFI) < 0)
		{
			log_error("hdcctv_client2_msgs_recvMsg");
			result = NC_ERROR_RECV;
			ret = -1;
			break;
		}
		result = tool_ptc_qh_d2c_decode_getWifi(&msgs->st_msg, &cfg_wifi);
		if (result < 0)
		{
			log_error("tool_ptc_qh_d2c_decode_getWifi");
			break;
		}
	} while (0);

	tool_thread_lockMutex(&msgs->mutex);
	tool_mem_memcpy(&msgs->cfg_get_wifi, &cfg_wifi, sizeof(ptc_cfg_wifi_t));
	msgs->result_wifi = result;
	msgs->last_get_wifi_req = msgs->cur_get_wifi_req;
	tool_thread_broadcastCond(&msgs->cond_wifi);
	tool_thread_unlockMutex(&msgs->mutex);
	return ret; 
}

TOOL_INT32 hdcctv_client2_msgs_doSetWifi(hdcctv_client2_msgs_t* msgs)
{
	ptc_cfg_wifi_t cfg_wifi;
	tool_thread_lockMutex(&msgs->mutex);
	if (msgs->last_set_wifi_req == msgs->cur_set_wifi_req)
	{
		tool_thread_unlockMutex(&msgs->mutex);
		return 0;
	}
	tool_mem_memcpy(&cfg_wifi, &msgs->cfg_set_wifi, sizeof(ptc_cfg_wifi_t));
	tool_thread_unlockMutex(&msgs->mutex);

	TOOL_INT32 result = 0;
	TOOL_INT32 ret = 0;
	do
	{
		tool_mem_memset(&msgs->st_msg.head, sizeof(ptc_head_t));
		msgs->st_msg.head.cmd = PTC_CMD_CFG2_SET_WIFI;		
		tool_ptc_qh_c2d_encode_setWifi(&msgs->st_msg, &cfg_wifi);
		if (tool_ptc_qh_sendMsg(&msgs->sock_item, &msgs->st_msg) < 0)
		{
			log_error("tool_ptc_qh_sendMsg");
			result = NC_ERROR_SEND;
			ret = -1;
			break;
		}
		if (hdcctv_client2_msgs_recvMsg(msgs, PTC_CMD_CFG2_SET_WIFI) < 0)
		{
			log_error("hdcctv_client2_msgs_recvMsg");
			result = NC_ERROR_RECV;
			ret = -1;
			break;
		}
		result = tool_ptc_qh_d2c_decode_setWifi(&msgs->st_msg);
		if (result < 0)
		{
			log_error("tool_ptc_qh_d2c_decode_setWifi");
			break;
		}
	} while (0);

	msgs->last_set_wifi_req = msgs->cur_set_wifi_req;
	return ret;
}

TOOL_INT32 hdcctv_client2_msgs_doGetDevRecord(hdcctv_client2_msgs_t* msgs)
{
	ptc_cfg_devRecord_t cfg_devRecord;
	tool_thread_lockMutex(&msgs->mutex);
	if (msgs->last_get_devRecord_req == msgs->cur_get_devRecord_req)
	{
		tool_thread_unlockMutex(&msgs->mutex);
		return 0;
	}
	tool_mem_memcpy(&cfg_devRecord, &msgs->cfg_get_devRecord, sizeof(ptc_cfg_devRecord_t));
	tool_thread_unlockMutex(&msgs->mutex);	

	TOOL_INT32 result = 0;
	TOOL_INT32 ret = 0;
	do
	{
		tool_mem_memset(&msgs->st_msg.head, sizeof(ptc_head_t));
		msgs->st_msg.head.cmd = PTC_CMD_CFG2_GET_DEVRECORD;		
		tool_ptc_qh_c2d_encode_getDevRecord(&msgs->st_msg);
		if (tool_ptc_qh_sendMsg(&msgs->sock_item, &msgs->st_msg) < 0)
		{
			log_error("tool_ptc_qh_sendMsg");
			result = NC_ERROR_SEND;
			ret = -1;
			break;
		}
		if (hdcctv_client2_msgs_recvMsg(msgs, PTC_CMD_CFG2_GET_DEVRECORD) < 0)
		{
			log_error("hdcctv_client2_msgs_recvMsg");
			result = NC_ERROR_RECV;
			ret = -1;
			break;
		}
		result = tool_ptc_qh_d2c_decode_getDevRecord(&msgs->st_msg, &cfg_devRecord);
		if (result < 0)
		{
			log_error("tool_ptc_qh_d2c_decode_getDevRecord");
			break;
		}
	} while (0);

	tool_thread_lockMutex(&msgs->mutex);
	tool_mem_memcpy(&msgs->cfg_get_devRecord, &cfg_devRecord, sizeof(ptc_cfg_devRecord_t));
	msgs->result_devRecord = result;
	msgs->last_get_devRecord_req = msgs->cur_get_devRecord_req;
	tool_thread_broadcastCond(&msgs->cond_devRecord);
	tool_thread_unlockMutex(&msgs->mutex);
	return ret; 
}

TOOL_INT32 hdcctv_client2_msgs_doSetDevRecord(hdcctv_client2_msgs_t* msgs)
{
	ptc_cfg_devRecord_t cfg_devRecord;
	tool_thread_lockMutex(&msgs->mutex);
	if (msgs->last_set_devRecord_req == msgs->cur_set_devRecord_req)
	{
		tool_thread_unlockMutex(&msgs->mutex);
		return 0;
	}
	tool_mem_memcpy(&cfg_devRecord, &msgs->cfg_set_devRecord, sizeof(ptc_cfg_devRecord_t));
	tool_thread_unlockMutex(&msgs->mutex);

	TOOL_INT32 result = 0;
	TOOL_INT32 ret = 0;
	do
	{
		tool_mem_memset(&msgs->st_msg.head, sizeof(ptc_head_t));
		msgs->st_msg.head.cmd = PTC_CMD_CFG2_SET_DEVRECORD;		
		tool_ptc_qh_c2d_encode_setDevRecord(&msgs->st_msg, &cfg_devRecord);
		if (tool_ptc_qh_sendMsg(&msgs->sock_item, &msgs->st_msg) < 0)
		{
			log_error("tool_ptc_qh_sendMsg");
			result = NC_ERROR_SEND;
			ret = -1;
			break;
		}
		if (hdcctv_client2_msgs_recvMsg(msgs, PTC_CMD_CFG2_SET_DEVRECORD) < 0)
		{
			log_error("hdcctv_client2_msgs_recvMsg");
			result = NC_ERROR_RECV;
			ret = -1;
			break;
		}
		result = tool_ptc_qh_d2c_decode_setDevRecord(&msgs->st_msg);
		if (result < 0)
		{
			log_error("tool_ptc_qh_d2c_decode_setDevRecord");
			break;
		}
	} while (0);

	msgs->last_set_devRecord_req = msgs->cur_set_devRecord_req;
	return ret;
}

TOOL_INT32 hdcctv_client2_msgs_doGetCfg(hdcctv_client2_msgs_t* msgs)
{
	TOOL_INT32 ret = 0;
	TOOL_INT32 result = 0;
	TOOL_INT32 cmd = 0;	
	TOOL_INT32 i = 0;
	//////////////////////////////////////
	ptc_cfg_av_t cfg_av;
	ptc_cfg_wifi_t cfg_wifi;
	ptc_cfg_devRecord_t cfg_devRecord;
	ptc_cfg_workMode_t cfg_workMode;

	for (i = 0; i < HDCCTV_CLIENT_CFG_NUM; i++)
	{
		tool_thread_lockMutex(&msgs->mutex);
		if (msgs->cfg[i].last_get_req == msgs->cfg[i].cur_get_req)
		{
			tool_thread_unlockMutex(&msgs->mutex);
			continue;
		}
		//////////////////////////////////////
		if (i == HDCCTV_CLIENT_CFG_AV)		
		{
			tool_mem_memcpy(&cfg_av, &msgs->cfg_get_av, sizeof(ptc_cfg_av_t));
		}
		else if (i == HDCCTV_CLIENT_CFG_WIFI)
		{
			tool_mem_memcpy(&cfg_wifi, &msgs->cfg_get_wifi, sizeof(ptc_cfg_wifi_t));
		}
		else if (i == HDCCTV_CLIENT_CFG_DEVRECORD)
		{
			tool_mem_memcpy(&cfg_devRecord, &msgs->cfg_get_devRecord, sizeof(ptc_cfg_devRecord_t));
		}
		else if (i == HDCCTV_CLIENT_CFG_WORKMODE)
		{
			tool_mem_memcpy(&cfg_workMode, &msgs->cfg_get_workMode, sizeof(ptc_cfg_workMode_t));
		}
		else
		{
			log_error("i(%d)", i);
			tool_thread_unlockMutex(&msgs->mutex);	
			continue;
		}
		tool_thread_unlockMutex(&msgs->mutex);	
		
		ret = 0;
		result = 0;
		do
		{
			tool_mem_memset(&msgs->st_msg.head, sizeof(ptc_head_t));
			//////////////////////////////////
			if (i == HDCCTV_CLIENT_CFG_AV)
			{
				msgs->st_msg.head.cmd = PTC_CMD_CFG2_GET_AV; 	
				tool_ptc_qh_c2d_encode_getAv(&msgs->st_msg);
			}
			else if (i == HDCCTV_CLIENT_CFG_WIFI)
			{
				msgs->st_msg.head.cmd = PTC_CMD_CFG2_GET_WIFI; 	
				tool_ptc_qh_c2d_encode_getWifi(&msgs->st_msg);
			}
			else if (i == HDCCTV_CLIENT_CFG_DEVRECORD)
			{
				msgs->st_msg.head.cmd = PTC_CMD_CFG2_GET_DEVRECORD; 	
				tool_ptc_qh_c2d_encode_getDevRecord(&msgs->st_msg);
			}
			else if (i == HDCCTV_CLIENT_CFG_WORKMODE)
			{
				msgs->st_msg.head.cmd = PTC_CMD_CFG2_GET_WORKMODE; 	
				tool_ptc_qh_c2d_encode_getWorkMode(&msgs->st_msg);
			}
			else
			{
				log_error("i(%d)", i);
				break;
			}

			cmd = msgs->st_msg.head.cmd;
			if (tool_ptc_qh_sendMsg(&msgs->sock_item, &msgs->st_msg) < 0)
			{
				log_error("tool_ptc_qh_sendMsg");
				result = NC_ERROR_SEND;
				ret = -1;
				break;
			}
			if (hdcctv_client2_msgs_recvMsg(msgs, cmd) < 0)
			{
				log_error("hdcctv_client2_msgs_recvMsg");
				result = NC_ERROR_RECV;
				ret = -1;
				break;
			}

			////////////////////////////////
			if (i == HDCCTV_CLIENT_CFG_AV)
			{
				result = tool_ptc_qh_d2c_decode_getAv(&msgs->st_msg, &cfg_av);
			}
			else if (i == HDCCTV_CLIENT_CFG_WIFI)
			{
				result = tool_ptc_qh_d2c_decode_getWifi(&msgs->st_msg, &cfg_wifi);
			}
			else if (i == HDCCTV_CLIENT_CFG_DEVRECORD)
			{
				result = tool_ptc_qh_d2c_decode_getDevRecord(&msgs->st_msg, &cfg_devRecord);
			}
			else if (i == HDCCTV_CLIENT_CFG_WORKMODE)
			{
				result = tool_ptc_qh_d2c_decode_getWorkMode(&msgs->st_msg, &cfg_workMode);
			}
			else
			{
				log_error("i(%d)", i);
				break;
			}
			if (result < 0)
			{
				log_error("tool_ptc_qh_d2c_decode_getxxxxx");
				break;
			}
		} while (0);
		
		tool_thread_lockMutex(&msgs->mutex);
		////////////////////
		if (i == HDCCTV_CLIENT_CFG_AV)
		{
			tool_mem_memcpy(&msgs->cfg_get_av, &cfg_av, sizeof(ptc_cfg_av_t));
		}
		else if (i == HDCCTV_CLIENT_CFG_WIFI)
		{
			tool_mem_memcpy(&msgs->cfg_get_wifi, &cfg_wifi, sizeof(ptc_cfg_wifi_t));
		}
		else if (i == HDCCTV_CLIENT_CFG_DEVRECORD)
		{
			tool_mem_memcpy(&msgs->cfg_get_devRecord, &cfg_devRecord, sizeof(ptc_cfg_devRecord_t));
		}
		else if (i == HDCCTV_CLIENT_CFG_WORKMODE)
		{
			tool_mem_memcpy(&msgs->cfg_get_workMode, &cfg_workMode, sizeof(ptc_cfg_workMode_t));
		}
		else
		{
			log_error("i(%d)", i);
			tool_thread_unlockMutex(&msgs->mutex);
			continue;
		}
		msgs->cfg[i].get_result = result;
		msgs->cfg[i].last_get_req = msgs->cfg[i].cur_get_req;
		tool_thread_broadcastCond(&msgs->cfg[i].get_cond);
		tool_thread_unlockMutex(&msgs->mutex);

		if (ret < 0)
			break;
	}
	
	return ret; 
}

TOOL_INT32 hdcctv_client2_msgs_doSetCfg(hdcctv_client2_msgs_t* msgs)
{
	TOOL_INT32 result = 0;
	TOOL_INT32 ret = 0;
	TOOL_INT32 cmd = 0;
	TOOL_INT32 i = 0;

	//////////////////////////////////
	ptc_cfg_av_t cfg_av;
	ptc_cfg_wifi_t cfg_wifi;
	ptc_cfg_devRecord_t cfg_devRecord;
	ptc_cfg_workMode_t cfg_workMode;

	for (i = 0; i < HDCCTV_CLIENT_CFG_NUM; i++)
	{
		tool_thread_lockMutex(&msgs->mutex);
//		log_debug("i(%d) (%d.%d)", i, msgs->cfg[i].last_set_req, msgs->cfg[i].cur_set_req);
		if (msgs->cfg[i].last_set_req == msgs->cfg[i].cur_set_req)
		{
			tool_thread_unlockMutex(&msgs->mutex);
			continue;
		}
		/////////////////////////////////////
		if (i == HDCCTV_CLIENT_CFG_AV)
		{
			tool_mem_memcpy(&cfg_av, &msgs->cfg_set_av, sizeof(ptc_cfg_av_t));
		}
		else if (i == HDCCTV_CLIENT_CFG_WIFI)
		{
			tool_mem_memcpy(&cfg_wifi, &msgs->cfg_set_wifi, sizeof(ptc_cfg_wifi_t));
		}
		else if (i == HDCCTV_CLIENT_CFG_DEVRECORD)
		{
			tool_mem_memcpy(&cfg_devRecord, &msgs->cfg_set_devRecord, sizeof(ptc_cfg_devRecord_t));
		}
		else if (i == HDCCTV_CLIENT_CFG_WORKMODE)
		{
			tool_mem_memcpy(&cfg_workMode, &msgs->cfg_set_workMode, sizeof(ptc_cfg_workMode_t));
		}
		else
		{
			log_error("i(%d)", i);
			tool_thread_unlockMutex(&msgs->mutex);	
			continue;
		}
		tool_thread_unlockMutex(&msgs->mutex);
		
		ret = 0;
		result = 0;
		do
		{
			tool_mem_memset(&msgs->st_msg.head, sizeof(ptc_head_t));
			//////////////////////////////////
			if (i == HDCCTV_CLIENT_CFG_AV)
			{
				msgs->st_msg.head.cmd = PTC_CMD_CFG2_SET_AV; 	
				tool_ptc_qh_c2d_encode_setAv(&msgs->st_msg, &cfg_av);
			}
			else if (i == HDCCTV_CLIENT_CFG_WIFI)
			{
				msgs->st_msg.head.cmd = PTC_CMD_CFG2_SET_WIFI; 	
				tool_ptc_qh_c2d_encode_setWifi(&msgs->st_msg, &cfg_wifi);
			}
			else if (i == HDCCTV_CLIENT_CFG_DEVRECORD)
			{
				msgs->st_msg.head.cmd = PTC_CMD_CFG2_SET_DEVRECORD; 	
				tool_ptc_qh_c2d_encode_setDevRecord(&msgs->st_msg, &cfg_devRecord);
			}
			else if (i == HDCCTV_CLIENT_CFG_WORKMODE)
			{
				msgs->st_msg.head.cmd = PTC_CMD_CFG2_SET_WORKMODE; 	
				tool_ptc_qh_c2d_encode_setWorkMode(&msgs->st_msg, &cfg_workMode);
			}
			else
			{
				log_error("i(%d)", i);
				break;
			}
			cmd = msgs->st_msg.head.cmd;
			if (tool_ptc_qh_sendMsg(&msgs->sock_item, &msgs->st_msg) < 0)
			{
				log_error("tool_ptc_qh_sendMsg");
				result = NC_ERROR_SEND;
				ret = -1;
				break;
			}
			if (hdcctv_client2_msgs_recvMsg(msgs, cmd) < 0)
			{
				log_error("hdcctv_client2_msgs_recvMsg");
				result = NC_ERROR_RECV;
				ret = -1;
				break;
			}
			
			//////////////////////////////////////////
			if (i == HDCCTV_CLIENT_CFG_AV)
			{
				result = tool_ptc_qh_d2c_decode_setAv(&msgs->st_msg);
			}
			else if (i == HDCCTV_CLIENT_CFG_WIFI)
			{
				result = tool_ptc_qh_d2c_decode_setWifi(&msgs->st_msg);
			}
			else if (i == HDCCTV_CLIENT_CFG_DEVRECORD)
			{
				result = tool_ptc_qh_d2c_decode_setDevRecord(&msgs->st_msg);
			}
			else if (i == HDCCTV_CLIENT_CFG_WORKMODE)
			{
				result = tool_ptc_qh_d2c_decode_setWorkMode(&msgs->st_msg);
			}
			else
			{
				log_error("i(%d)", i);
				break;
			}
			if (result < 0)
			{
				log_error("tool_ptc_qh_d2c_decode_setxxxx");
				break;
			}
		} while (0);
		
		tool_thread_lockMutex(&msgs->mutex);
		////////////////////
		if (i == HDCCTV_CLIENT_CFG_AV)
		{
			tool_mem_memcpy(&msgs->cfg_set_av, &cfg_av, sizeof(ptc_cfg_av_t));
		}
		else if (i == HDCCTV_CLIENT_CFG_WIFI)
		{
			tool_mem_memcpy(&msgs->cfg_set_wifi, &cfg_wifi, sizeof(ptc_cfg_wifi_t));
		}
		else if (i == HDCCTV_CLIENT_CFG_DEVRECORD)
		{
			tool_mem_memcpy(&msgs->cfg_set_devRecord, &cfg_devRecord, sizeof(ptc_cfg_devRecord_t));
		}
		else if (i == HDCCTV_CLIENT_CFG_WORKMODE)
		{
			tool_mem_memcpy(&msgs->cfg_set_workMode, &cfg_workMode, sizeof(ptc_cfg_workMode_t));
		}
		else
		{
			log_error("i(%d)", i);
			tool_thread_unlockMutex(&msgs->mutex);
			continue;
		}
		msgs->cfg[i].set_result = result;
		msgs->cfg[i].last_set_req = msgs->cfg[i].cur_set_req;
		tool_thread_broadcastCond(&msgs->cfg[i].set_cond);
		tool_thread_unlockMutex(&msgs->mutex);

		if (ret < 0)
			break;
	}
	
	return ret;
}


TOOL_INT32 hdcctv_client2_msgs_doRecv(hdcctv_client2_msgs_t* msgs)
{
	TOOL_INT32 ret = 0;
	ret = tool_sock_item_pollRead(&msgs->sock_item, 0);
	if (ret < 0)
	{
		log_error("tool_sock_item_pollRead msgs(%d)", msgs->msgs_id);
		return -1;
	}
	else if (ret == 0)
		return 0;
	if (hdcctv_client2_msgs_recvMsg(msgs, PTC_CMD_HDCCTV_BASE) < 0)
	{
		log_error("tool_ptc_client2_recvMsg msgs(%d)", msgs->msgs_id);
		return -1;
	}
	log_state("cmd(0d%d)", msgs->st_msg.head.cmd);
	msgs->is_sleep = 0;
	return 0;
}


TOOL_VOID* hdcctv_client2_msgs_run(TOOL_VOID* param)
{
	hdcctv_client2_msgs_t* msgs = (hdcctv_client2_msgs_t*)param;
	hdcctv_client2_t* client2 = (hdcctv_client2_t*)msgs->father_client2;
	log_state("msgs(0x%08x) Login start", msgs->msgs_id);
	msgs->msgs_tick = -12345678;

	while (msgs->state)
	{
		if (client2->loc_enable == 0)
		{
			tool_sysf_usleep(HDCCTV_CLIENT2_USLEEP_CYCLE);
			continue;
		}

		if (msgs->msgs_enable == 0)
		{
			if (tool_sock_isTickTimeout(msgs->msgs_tick, HDCCTV_CLIENT2_SERVICE_CYCLE) == 0)
			{
				tool_sysf_usleep(HDCCTV_CLIENT2_USLEEP_CYCLE);
				continue;
			}
			tool_sock_setTick(&msgs->msgs_tick);
			
			if (hdcctv_client_getServiceAddr(client2->server_ip, client2->server_port, &msgs->st_nc_dev, "msgs", &msgs->msgs_addr) < 0)
			{
				log_error("hdcctv_client_getServiceAddr");
				continue;
			}
			msgs->msgs_enable = 1;
			log_state("hdcctv_client_getServiceAddr success");
		}
		
		if (hdcctv_client2_msgs_login(msgs) < 0)
			continue;
		msgs->is_sleep = 1;
		if (hdcctv_client2_msgs_heartbeat(msgs) < 0 ||
			hdcctv_client2_msgs_doGetAbility(msgs) < 0 ||
			hdcctv_client2_msgs_doPtz(msgs) < 0 ||
			hdcctv_client2_msgs_doRecords(msgs) < 0 ||
			hdcctv_client2_msgs_doLogs(msgs) < 0 ||
			hdcctv_client2_msgs_doSyncTime(msgs) < 0 ||
			hdcctv_client2_msgs_doGetStatus(msgs) < 0 ||
			hdcctv_client2_msgs_doSetStatus(msgs) < 0 ||
			hdcctv_client2_msgs_doGetAlarm(msgs) < 0 ||
			hdcctv_client2_msgs_doSetAlarm(msgs) < 0 ||
			hdcctv_client2_msgs_doGetWifi(msgs) < 0 ||
			hdcctv_client2_msgs_doSetWifi(msgs) < 0 ||		
			hdcctv_client2_msgs_doGetDevRecord(msgs) < 0 ||
			hdcctv_client2_msgs_doSetDevRecord(msgs) < 0 ||			
			hdcctv_client2_msgs_doGetCfg(msgs) < 0 ||
			hdcctv_client2_msgs_doSetCfg(msgs) < 0 ||	
			hdcctv_client2_msgs_doDates(msgs) < 0 ||
			hdcctv_client2_msgs_doRecv(msgs) < 0)
		{
			hdcctv_client2_msgs_logout(msgs);
			continue;
		}

		if (msgs->is_sleep)
			tool_sysf_usleep(20*1000);
	}

	log_state("(0x%08x) Login stop", msgs->msgs_id);
	hdcctv_client2_msgs_logout(msgs);
    return NULL;
}

TOOL_INT32 hdcctv_client2_msgs_start(hdcctv_client2_msgs_t* pst_msgs, nc_dev* pst_nc_dev)
{
	tool_mem_memcpy(&pst_msgs->st_nc_dev, pst_nc_dev, sizeof(nc_dev));
	tool_sock_item_init(&pst_msgs->sock_item);	
	tool_sock_item_setFlag(&pst_msgs->sock_item, &pst_msgs->state);

	pst_msgs->state = 1;
	if (tool_thread_create(&pst_msgs->pid, NULL, hdcctv_client2_msgs_run, (TOOL_VOID*)pst_msgs))
		log_fatalNo("tool_thread_create");
	return pst_msgs->msgs_id;	
}

TOOL_VOID hdcctv_client2_msgs_stop(hdcctv_client2_msgs_t* msgs)
{
	if (msgs->state == 0)
		return;
	
	msgs->state = 0;
	tool_thread_join(msgs->pid);
}

TOOL_VOID hdcctv_client2_msgs_done(hdcctv_client2_msgs_t* msgs)
{
	hdcctv_client2_msgs_stop(msgs);
	tool_thread_doneMutex(&msgs->mutex);
	tool_thread_doneCond(&msgs->cond_dates);
	tool_thread_doneCond(&msgs->cond_records);
	tool_thread_doneCond(&msgs->cond_logs_v2);
	tool_thread_doneCond(&msgs->cond_status);	
	tool_thread_doneCond(&msgs->cond_ability);	
	tool_thread_doneCond(&msgs->cond_get_alarm);	
	tool_thread_doneCond(&msgs->cond_set_alarm);
	tool_thread_doneCond(&msgs->cond_wifi);
	tool_thread_doneCond(&msgs->cond_devRecord);

	TOOL_INT32 i = 0; 
	for (i = 0; i < HDCCTV_CLIENT_CFG_NUM; i++)
	{
		tool_thread_doneCond(&msgs->cfg[i].get_cond);
		tool_thread_doneCond(&msgs->cfg[i].set_cond);
	}
}

TOOL_INT32 hdcctv_client2_msgs_isSame(hdcctv_client2_msgs_t* msgs, nc_dev* pst_nc_dev)
{
	if (pst_nc_dev->ptc == NC_PTC_QiHan_Cloud && msgs->st_nc_dev.ptc == NC_PTC_QiHan_Cloud &&
		tool_str_strncmp(msgs->st_nc_dev.dev_id, pst_nc_dev->dev_id, PTC_STR_SIZE-1) == 0 &&
		tool_str_strncmp(msgs->st_nc_dev.user, pst_nc_dev->user, PTC_ID_SIZE-1) == 0 &&
		tool_str_strncmp(msgs->st_nc_dev.pswd, pst_nc_dev->pswd, PTC_ID_SIZE-1) == 0 &&
		msgs->state)
		return 1;
	return 0;
}

TOOL_INT32 hdcctv_client2_msgs_getAbility_v2(hdcctv_client2_msgs_t* msgs, ptc_cfg_ability_t* ability)
{	
	TOOL_INT32 ret = 0;
	tool_thread_lockMutex(&msgs->mutex);
	tool_sock_setTick(&msgs->state_tick);
	msgs->cur_get_ability_flag ++;
	tool_mem_memcpy(&msgs->cfg_get_ability, ability, sizeof(ptc_cfg_ability_t));
	if (tool_thread_timedwaitCond(&msgs->cond_status, &msgs->mutex, 5) < 0)
	{
		ret = NC_ERROR_TIMEOUT;
	}
	else
	{
		tool_mem_memcpy(ability, &msgs->cfg_get_ability, sizeof(ptc_cfg_ability_t));
		ret = msgs->result_ability;
	}
	tool_sock_setTick(&msgs->state_tick);
	tool_thread_unlockMutex(&msgs->mutex);
	return ret;
}

TOOL_INT32 hdcctv_client2_msgs_ctrlPtz(hdcctv_client2_msgs_t* msgs, TOOL_INT32 channel, PTC_PTZ_CMD_E cmd, TOOL_INT32 param)
{
	tool_thread_lockMutex(&msgs->mutex);
	msgs->cur_ptz_req ++;
	msgs->ptz_channel = channel;
	msgs->ptz_cmd = cmd;
	msgs->ptz_param = param;
	tool_sock_setTick(&msgs->state_tick);
	tool_thread_unlockMutex(&msgs->mutex);	
	return 0;
}

TOOL_INT32 hdcctv_client2_msgs_searchRecords(hdcctv_client2_msgs_t* msgs, ptc_cfg_records_t* records)
{
	TOOL_INT32 ret = 0;
	tool_thread_lockMutex(&msgs->mutex);
	tool_sock_setTick(&msgs->state_tick);
	msgs->cur_records_req ++;
	tool_mem_memcpy(&msgs->cfg_records, records, sizeof(ptc_cfg_records_t));
	if (tool_thread_timedwaitCond(&msgs->cond_records, &msgs->mutex, 5) < 0)
	{
		ret = NC_ERROR_TIMEOUT;
	}
	else
	{
		tool_mem_memcpy(records, &msgs->cfg_records, sizeof(ptc_cfg_records_t));
		ret = msgs->result_records;
	}
	tool_sock_setTick(&msgs->state_tick);
	tool_thread_unlockMutex(&msgs->mutex);
	return ret; 
}

TOOL_INT32 hdcctv_client2_msgs_searchDates(hdcctv_client2_msgs_t* msgs, ptc_cfg_dates_t* dates)
{
	TOOL_INT32 ret = 0;
	tool_thread_lockMutex(&msgs->mutex);
	tool_sock_setTick(&msgs->state_tick);
	msgs->cur_dates_req ++;
	tool_mem_memcpy(&msgs->cfg_dates, dates, sizeof(ptc_cfg_dates_t));
	if (tool_thread_timedwaitCond(&msgs->cond_dates, &msgs->mutex, 5) < 0)
	{
		ret = NC_ERROR_TIMEOUT;
	}
	else
	{
		tool_mem_memcpy(dates, &msgs->cfg_dates, sizeof(ptc_cfg_dates_t));
		ret = msgs->result_dates;
	}
	tool_sock_setTick(&msgs->state_tick);
	tool_thread_unlockMutex(&msgs->mutex);
	return ret; 
}

TOOL_INT32 hdcctv_client2_msgs_searchLogs(hdcctv_client2_msgs_t* msgs, ptc_logs_v2_t* logs_v2)
{
	TOOL_INT32 ret = 0;
	tool_thread_lockMutex(&msgs->mutex);
	tool_sock_setTick(&msgs->state_tick);
	msgs->cur_logs_v2_req ++;
	tool_mem_memcpy(&msgs->cfg_logs_v2, logs_v2, sizeof(ptc_logs_v2_t));
	if (tool_thread_timedwaitCond(&msgs->cond_logs_v2, &msgs->mutex, 5) < 0)
	{
		ret = NC_ERROR_TIMEOUT;
	}
	else
	{
		tool_mem_memcpy(logs_v2, &msgs->cfg_logs_v2, sizeof(ptc_logs_v2_t));
		ret = msgs->result_logs_v2;
	}
	tool_sock_setTick(&msgs->state_tick);
	tool_thread_unlockMutex(&msgs->mutex);
	return ret;
}

TOOL_INT32 hdcctv_client2_msgs_syncTime(hdcctv_client2_msgs_t* msgs, TOOL_UINT32 utc_sec, TOOL_INT32 zone, TOOL_INT32 dst_hour)
{
	tool_thread_lockMutex(&msgs->mutex);
	msgs->cur_time_req ++;
	msgs->utc_sec = utc_sec;
	msgs->zone = zone;
	msgs->dst_hour = dst_hour;
	tool_sock_setTick(&msgs->state_tick);
	tool_thread_unlockMutex(&msgs->mutex);
	return 0;
}


TOOL_INT32 hdcctv_client2_msgs_getStatus(hdcctv_client2_msgs_t* msgs, ptc_cfg_status_t* cfg_status)
{
	TOOL_INT32 ret = 0;
	tool_thread_lockMutex(&msgs->mutex);
	tool_sock_setTick(&msgs->state_tick);
	msgs->cur_get_status_req ++;
//	log_debug("msgs->last_get_status_req(%d, %d)", msgs->last_get_status_req, msgs->cur_get_status_req);
	tool_mem_memcpy(&msgs->cfg_get_status, cfg_status, sizeof(ptc_cfg_status_t));
	if (tool_thread_timedwaitCond(&msgs->cond_status, &msgs->mutex, 5) < 0)
	{
		ret = NC_ERROR_TIMEOUT;
	}
	else
	{
		tool_mem_memcpy(cfg_status, &msgs->cfg_get_status, sizeof(ptc_cfg_status_t));
		ret = msgs->result_status;
	}
	tool_sock_setTick(&msgs->state_tick);
	tool_thread_unlockMutex(&msgs->mutex);
	return ret;
}

TOOL_INT32 hdcctv_client2_msgs_setStatus(hdcctv_client2_msgs_t* msgs, ptc_cfg_status_t* cfg_status)
{
	tool_thread_lockMutex(&msgs->mutex);
	msgs->cur_set_status_req ++;
	tool_mem_memcpy(&msgs->cfg_set_status, cfg_status, sizeof(ptc_cfg_status_t));
	tool_thread_unlockMutex(&msgs->mutex);
	return 0;
}


TOOL_INT32 hdcctv_client2_msgs_getAlarm(hdcctv_client2_msgs_t* msgs, PTC_ALARM_TYPE_E cfg_type, ptc_cfg_alarm_t* cfg_alarm)
{
	TOOL_INT32 ret = 0;
	tool_thread_lockMutex(&msgs->mutex);
	tool_sock_setTick(&msgs->state_tick);
	msgs->cur_get_alarm_req ++;
	tool_mem_memcpy(&msgs->cfg_get_alarm, cfg_alarm, sizeof(ptc_cfg_alarm_t));
	msgs->cfg_get_type = cfg_type;
	if (tool_thread_timedwaitCond(&msgs->cond_get_alarm, &msgs->mutex, 5) < 0)
	{
		ret = NC_ERROR_TIMEOUT;
	}
	else
	{
		tool_mem_memcpy(cfg_alarm, &msgs->cfg_get_alarm, sizeof(ptc_cfg_alarm_t));
		ret = msgs->result_get_alarm;
	}
	tool_sock_setTick(&msgs->state_tick);
	tool_thread_unlockMutex(&msgs->mutex);
	return ret;
}

TOOL_INT32 hdcctv_client2_msgs_setAlarm(hdcctv_client2_msgs_t* msgs, PTC_ALARM_TYPE_E cfg_type, ptc_cfg_alarm_t* cfg_alarm)
{
	TOOL_INT32 ret = 0;
	tool_thread_lockMutex(&msgs->mutex);
	msgs->cur_set_alarm_req ++;
	tool_mem_memcpy(&msgs->cfg_set_alarm, cfg_alarm, sizeof(ptc_cfg_alarm_t));
	msgs->cfg_set_type = cfg_type;
	if (tool_thread_timedwaitCond(&msgs->cond_set_alarm, &msgs->mutex, 5) < 0)
	{
		ret = NC_ERROR_TIMEOUT;
	}
	else
	{
		tool_mem_memcpy(cfg_alarm, &msgs->cfg_set_alarm, sizeof(ptc_cfg_alarm_t));
		ret = msgs->result_set_alarm;
	}
	tool_sock_setTick(&msgs->state_tick);
	tool_thread_unlockMutex(&msgs->mutex);
	return ret;
}

TOOL_INT32 hdcctv_client2_msgs_getWifi(hdcctv_client2_msgs_t* msgs, ptc_cfg_wifi_t* pst_wifi)
{
	TOOL_INT32 ret = 0;
	tool_thread_lockMutex(&msgs->mutex);
	tool_sock_setTick(&msgs->state_tick);
	msgs->cur_get_wifi_req ++;
	tool_mem_memcpy(&msgs->cfg_get_wifi, pst_wifi, sizeof(ptc_cfg_wifi_t));
	if (tool_thread_timedwaitCond(&msgs->cond_wifi, &msgs->mutex, 5) < 0)
	{
		ret = NC_ERROR_TIMEOUT;
	}
	else
	{
		tool_mem_memcpy(pst_wifi, &msgs->cfg_get_wifi, sizeof(ptc_cfg_wifi_t));
		ret = msgs->result_wifi;
	}
	tool_sock_setTick(&msgs->state_tick);
	tool_thread_unlockMutex(&msgs->mutex);
	return ret;
}

TOOL_INT32 hdcctv_client2_msgs_setWifi(hdcctv_client2_msgs_t* msgs, ptc_cfg_wifi_t* pst_wifi)
{
	tool_thread_lockMutex(&msgs->mutex);
	msgs->cur_set_wifi_req ++;
	tool_mem_memcpy(&msgs->cfg_set_wifi, pst_wifi, sizeof(ptc_cfg_wifi_t));
	tool_thread_unlockMutex(&msgs->mutex);
	return 0;
}

TOOL_INT32 hdcctv_client2_msgs_getDevRecord(hdcctv_client2_msgs_t* msgs, ptc_cfg_devRecord_t* pst_devRecord)
{
	TOOL_INT32 ret = 0;
	tool_thread_lockMutex(&msgs->mutex);
	tool_sock_setTick(&msgs->state_tick);
	msgs->cur_get_devRecord_req ++;
	tool_mem_memcpy(&msgs->cfg_get_devRecord, pst_devRecord, sizeof(ptc_cfg_devRecord_t));
	if (tool_thread_timedwaitCond(&msgs->cond_devRecord, &msgs->mutex, 5) < 0)
	{
		ret = NC_ERROR_TIMEOUT;
	}
	else
	{
		tool_mem_memcpy(pst_devRecord, &msgs->cfg_get_devRecord, sizeof(ptc_cfg_devRecord_t));
		ret = msgs->result_devRecord;
	}
	tool_sock_setTick(&msgs->state_tick);
	tool_thread_unlockMutex(&msgs->mutex);
	return ret;
}

TOOL_INT32 hdcctv_client2_msgs_setDevRecord(hdcctv_client2_msgs_t* msgs, ptc_cfg_devRecord_t* pst_devRecord)
{
	tool_thread_lockMutex(&msgs->mutex);
	msgs->cur_set_devRecord_req ++;
	tool_mem_memcpy(&msgs->cfg_set_devRecord, pst_devRecord, sizeof(ptc_cfg_devRecord_t));
	tool_thread_unlockMutex(&msgs->mutex);
	return 0;
}

TOOL_INT32 hdcctv_client2_msgs_getCfg(hdcctv_client2_msgs_t* msgs, TOOL_INT32 id, TOOL_VOID* cfg)
{
	TOOL_INT32 ret = 0;
	tool_thread_lockMutex(&msgs->mutex);
	tool_sock_setTick(&msgs->state_tick);
	msgs->cfg[id].cur_get_req ++;
	/////////////////////////////////////////////
	if (id == HDCCTV_CLIENT_CFG_AV)
	{
		tool_mem_memcpy(&msgs->cfg_get_av, cfg, sizeof(ptc_cfg_av_t));
	}
	else if (id == HDCCTV_CLIENT_CFG_WIFI)
	{
		tool_mem_memcpy(&msgs->cfg_get_wifi, cfg, sizeof(ptc_cfg_wifi_t));
	}
	else if (id == HDCCTV_CLIENT_CFG_DEVRECORD)
	{
		tool_mem_memcpy(&msgs->cfg_get_devRecord, cfg, sizeof(ptc_cfg_devRecord_t));
	}
	else if (id == HDCCTV_CLIENT_CFG_WORKMODE)
	{
		tool_mem_memcpy(&msgs->cfg_get_workMode, cfg, sizeof(ptc_cfg_workMode_t));
	}
	else
	{
		log_error("id(%d)", id);
		tool_thread_unlockMutex(&msgs->mutex);
		return NC_ERROR_INTERNAL;
	}
	
	if (tool_thread_timedwaitCond(&msgs->cfg[id].get_cond, &msgs->mutex, 10) < 0)
	{
		ret = NC_ERROR_TIMEOUT;
	}
	else
	{
		//////////////////////////////////////
		if (id == HDCCTV_CLIENT_CFG_AV)
		{
			tool_mem_memcpy(cfg, &msgs->cfg_get_av, sizeof(ptc_cfg_av_t));
		}
		else if (id == HDCCTV_CLIENT_CFG_WIFI)
		{
			tool_mem_memcpy(cfg, &msgs->cfg_get_wifi, sizeof(ptc_cfg_wifi_t));
		}	
		else if (id == HDCCTV_CLIENT_CFG_DEVRECORD)
		{
			tool_mem_memcpy(cfg, &msgs->cfg_get_devRecord, sizeof(ptc_cfg_devRecord_t));
		}
		else if (id == HDCCTV_CLIENT_CFG_WORKMODE)
		{
			tool_mem_memcpy(cfg, &msgs->cfg_get_workMode, sizeof(ptc_cfg_workMode_t));
		}
		else
		{
			log_error("id(%d)", id);
			tool_thread_unlockMutex(&msgs->mutex);
			return NC_ERROR_INTERNAL;
		}
		ret = msgs->cfg[id].get_result;
	}
	tool_sock_setTick(&msgs->state_tick);
	tool_thread_unlockMutex(&msgs->mutex);
	return ret;
}

TOOL_INT32 hdcctv_client2_msgs_setCfg(hdcctv_client2_msgs_t* msgs, TOOL_INT32 id, TOOL_VOID* cfg)
{
	TOOL_INT32 ret = 0;
	tool_thread_lockMutex(&msgs->mutex);
	msgs->cfg[id].cur_set_req ++;
	/////////////////////////////////////////////////
	if (id == HDCCTV_CLIENT_CFG_AV)
	{
		tool_mem_memcpy(&msgs->cfg_set_av, cfg, sizeof(ptc_cfg_av_t));
	}
	else if (id == HDCCTV_CLIENT_CFG_WIFI)
	{
		tool_mem_memcpy(&msgs->cfg_set_wifi, cfg, sizeof(ptc_cfg_wifi_t));
	}
	else if (id == HDCCTV_CLIENT_CFG_DEVRECORD)
	{
		tool_mem_memcpy(&msgs->cfg_set_devRecord, cfg, sizeof(ptc_cfg_devRecord_t));
	}
	else if (id == HDCCTV_CLIENT_CFG_WORKMODE)
	{
		tool_mem_memcpy(&msgs->cfg_set_workMode, cfg, sizeof(ptc_cfg_workMode_t));
	}
	else
	{
		log_error("id(%d)", id);
		tool_thread_unlockMutex(&msgs->mutex);
		return NC_ERROR_INTERNAL;
	}

	if (tool_thread_timedwaitCond(&msgs->cfg[id].set_cond, &msgs->mutex, 10) < 0)
	{
		ret = NC_ERROR_TIMEOUT;
	}
	else
	{
		//////////////////////////////////////
		if (id == HDCCTV_CLIENT_CFG_AV)
		{
			tool_mem_memcpy(cfg, &msgs->cfg_set_av, sizeof(ptc_cfg_av_t));
		}
		else if (id == HDCCTV_CLIENT_CFG_WIFI)
		{
			tool_mem_memcpy(cfg, &msgs->cfg_set_wifi, sizeof(ptc_cfg_wifi_t));
		}
		else if (id == HDCCTV_CLIENT_CFG_DEVRECORD)
		{
			tool_mem_memcpy(cfg, &msgs->cfg_set_devRecord, sizeof(ptc_cfg_devRecord_t));
		}
		else if (id == HDCCTV_CLIENT_CFG_WORKMODE)
		{
			tool_mem_memcpy(cfg, &msgs->cfg_set_workMode, sizeof(ptc_cfg_workMode_t));
		}
		else
		{
			log_error("id(%d)", id);
			tool_thread_unlockMutex(&msgs->mutex);
			return NC_ERROR_INTERNAL;
		}
		ret = msgs->cfg[id].set_result;
	}
	tool_sock_setTick(&msgs->state_tick);
	tool_thread_unlockMutex(&msgs->mutex);
	return ret;
}


#define __HDCCTV_CLIENT2__



hdcctv_client2_t* g_hdcctv_client2 = NULL;

TOOL_VOID* hdcctv_client2_run(TOOL_VOID* param)
{
	hdcctv_client2_t* client2 = (hdcctv_client2_t*)param;
	TOOL_INT32 i = 0;
	client2->login_tick = -12345678;
	hdcctv_cloud_user_t st_cloud_user;
	hdcctv_device_list_t st_device_list;

	while (client2->state)
	{
		if (tool_sock_isTickTimeout(client2->login_tick, HDCCTV_CLIENT2_LOGIN_CYCLE))
		{			
			tool_thread_lockMutex(&client2->mutex);
			if (client2->login_state == 0)
			{
				tool_thread_unlockMutex(&client2->mutex);	
				tool_sysf_usleep(HDCCTV_CLIENT2_USLEEP_CYCLE);
				continue;
			}
			tool_mem_memcpy(&st_cloud_user, &client2->st_cloud_user, sizeof(hdcctv_cloud_user_t));
			tool_thread_unlockMutex(&client2->mutex);			

			client2->list_result= hdcctv_client2_doDevList(&st_cloud_user, &st_device_list);
			tool_sock_setTick(&client2->login_tick);
			if (client2->list_result < 0)
			{
				client2->login_tick -= 5;
				log_error("hdcctv_client2_doDevList error");
			}
			else
			{
				log_debug("hdcctv_client2_doDevList success");
				TOOL_INT32 i = 0;
//				log_debug("st_device_list->num(%d)", st_device_list.num);
				for (i = 0; i < st_device_list.num; i++)
				{
//					log_debug("i(%d) dev(%s,%s)", i, st_device_list.device[i].dev_id, st_device_list.device[i].dev_name);
				}

				
				tool_thread_lockMutex(&client2->mutex);
				client2->list_state = 1;
				tool_mem_memcpy(&client2->st_cloud_user, &st_cloud_user, sizeof(hdcctv_cloud_user_t));
				tool_mem_memcpy(&client2->st_device_list, &st_device_list, sizeof(hdcctv_device_list_t));
				tool_thread_unlockMutex(&client2->mutex);	
			}
		}
		
		tool_thread_lockMutex(&client2->mutex);
		for (i = 0; i < HDCCTV_CLIENT2_MSGS_SIZE; i++)
		{
			if (client2->msgs[i].state == 0)
				continue;
			if (tool_sock_isTickTimeout(client2->msgs[i].state_tick, 60))
				hdcctv_client2_msgs_stop(&client2->msgs[i]);
		}
		tool_thread_unlockMutex(&client2->mutex);
		tool_sysf_sleep(1);		
	}
	
	return NULL;
}

TOOL_VOID hdcctv_client2_init()
{
	if (g_hdcctv_client2)
	{
		log_error("hdcctv client2 already start");
		return ;
	}
	
	g_hdcctv_client2 = (hdcctv_client2_t*)tool_mem_malloc(sizeof(hdcctv_client2_t), 1);
    tool_thread_initMutex(&g_hdcctv_client2->mutex);
	g_hdcctv_client2->buffer_usec = 300*1000;
	TOOL_INT32 i = 0;
	for (i = 0; i < HDCCTV_CLIENT2_RELAY_SIZE; i++)
		hdcctv_client2_relay_init(&g_hdcctv_client2->relay[i], i, g_hdcctv_client2);
	for (i = 0; i < HDCCTV_CLIENT2_MSGS_SIZE; i++)
		hdcctv_client2_msgs_init(&g_hdcctv_client2->msgs[i], i, g_hdcctv_client2);

	g_hdcctv_client2->state = 1;
	if (tool_thread_create(&g_hdcctv_client2->pid, NULL, hdcctv_client2_run, (TOOL_VOID*)g_hdcctv_client2) < 0)
		log_fatal("tool_thread_create");
}

TOOL_INT32 hdcctv_client2_setBuffer(TOOL_INT32 buffer_usec)
{
	if (g_hdcctv_client2 == NULL)
	{
		log_error("nc");
		return NC_ERROR_UNINIT;
	}
	if (buffer_usec < 0 || buffer_usec > 3000*1000)
	{
		log_error("buffer_usec(%d)", buffer_usec);
		return NC_ERROR_INPUT;
	}
	g_hdcctv_client2->buffer_usec = buffer_usec;
	return 0;
}

TOOL_VOID hdcctv_client2_done()
{
	if (g_hdcctv_client2 == NULL)
	{
		log_error("hdcctv client2 already stop");
		return ;
	}

	g_hdcctv_client2->state = 0;
	tool_thread_join(g_hdcctv_client2->pid);
	
	TOOL_INT32 i = 0;
	for (i = 0; i < HDCCTV_CLIENT2_RELAY_SIZE; i++)
		hdcctv_client2_relay_done(&g_hdcctv_client2->relay[i]);
	for (i = 0; i < HDCCTV_CLIENT2_MSGS_SIZE; i++)
		hdcctv_client2_msgs_done(&g_hdcctv_client2->msgs[i]);
	tool_thread_doneMutex(&g_hdcctv_client2->mutex);

	tool_mem_free(g_hdcctv_client2);
	g_hdcctv_client2 = NULL;

}

TOOL_INT32 hdcctv_client2_location(TOOL_INT8* server_ip, TOOL_INT32* server_port)
{
	if (g_hdcctv_client2 == NULL)
	{
		log_error("g_hdcctv_client2");
		return NC_ERROR_UNINIT;
	}

	if (server_ip == NULL || server_port == NULL)
	{
		log_error("server_ip(%d) server_port(%d)", server_ip, server_port);
		return NC_ERROR_INPUT;	
	}

	if (g_hdcctv_client2->loc_enable)
	{
		tool_str_strncpy(server_ip, g_hdcctv_client2->server_ip, PTC_IP_SIZE-1);
		*server_port = g_hdcctv_client2->server_port;
		return 0;
	}
	
	HDCCTV_LOCATION_E location = HDCCTV_LOCATION_US;
	if (hdcctv_client_location(&location, server_ip, server_port) < 0)
	{
		log_error("hdcctv_client_location");
		return NC_ERROR_QPNS_LOCATION;
	}
	tool_thread_lockMutex(&g_hdcctv_client2->mutex);
	g_hdcctv_client2->loc_enable = 1;
	g_hdcctv_client2->location = location;
	tool_str_strncpy(g_hdcctv_client2->server_ip, server_ip, PTC_IP_SIZE-1);
	g_hdcctv_client2->server_port = *server_port;
	tool_thread_unlockMutex(&g_hdcctv_client2->mutex);

	return 0;
}

TOOL_INT32 hdcctv_client2_findFreeRequest(hdcctv_client2_relay_t** ppst_relay)
{
	if (g_hdcctv_client2 == NULL)
	{
		log_error("g_hdcctv_client2");
		return NC_ERROR_UNINIT;
	}

	TOOL_INT32 i = 0;
	tool_thread_lockMutex(&g_hdcctv_client2->mutex);
	for (i = 0; i < HDCCTV_CLIENT2_RELAY_SIZE; i++)
	{
		if (g_hdcctv_client2->relay[i].is_used == 0)
		{
			g_hdcctv_client2->relay[i].is_used = 1;
			break;
		}
	}
	tool_thread_unlockMutex(&g_hdcctv_client2->mutex);
	if (i == HDCCTV_CLIENT2_RELAY_SIZE)
	{
		log_error("HDCCTV_CLIENT2_RELAY_SIZE(%d)", HDCCTV_CLIENT2_RELAY_SIZE);
		return NC_ERROR_FULL;
	}

	*ppst_relay = &g_hdcctv_client2->relay[i];
	return 0;
}

TOOL_INT32 hdcctv_client2_findRequestById(hdcctv_client2_relay_t** ppst_relay, TOOL_INT32 relay_id)
{
	if (g_hdcctv_client2 == NULL)
	{
		log_error("g_hdcctv_client2");
		return NC_ERROR_UNINIT;
	}

	TOOL_INT32 i = 0;
	tool_thread_lockMutex(&g_hdcctv_client2->mutex);
	for (i = 0; i < HDCCTV_CLIENT2_RELAY_SIZE; i++)
	{
		if (g_hdcctv_client2->relay[i].is_used == 1 && g_hdcctv_client2->relay[i].relay_id == relay_id)
			break;
	}
	tool_thread_unlockMutex(&g_hdcctv_client2->mutex);
	if (i == HDCCTV_CLIENT2_RELAY_SIZE)
	{
		log_error("relay_id(%d)", relay_id);
		return NC_ERROR_INPUT;
	}

	*ppst_relay = &g_hdcctv_client2->relay[i];
	return 0;
}

TOOL_INT32 hdcctv_client2_getAlarms(HDCCTV_QPNS_TOKEN_TYPE_E token_type, TOOL_INT8* mobile_token, TOOL_INT32 no, hdcctv_qpns_alarms_t* alarms)
{
	if (mobile_token == NULL || no < 0 || alarms == NULL)
	{
		log_error("mobile_token(%d) no(%d) alarms(%d)", mobile_token, no, alarms);
		return NC_ERROR_INPUT;
	}

	TOOL_INT8 server_ip[PTC_IP_SIZE] = {0};
	TOOL_INT32 server_port = 0;
	TOOL_INT32 ret = hdcctv_client2_location(server_ip, &server_port);
	if (ret < 0)
	{
		log_error("hdcctv_client2_location");
		return ret;
	}

	alarms->num = 0;

	tool_http_t send_http;
	tool_http_t recv_http;
	tool_http_Init(&send_http);
	tool_str_strncpy(send_http.command, "POST", tool_str_strlen("POST"));
	tool_str_strncpy(send_http.url, "/u/qpns_ios_get.php", tool_str_strlen("/u/qpns_ios_get.php"));
	tool_str_strncpy(send_http.version, "HTTP/1.0", tool_str_strlen("HTTP/1.0"));
	send_http.ContentLength = snprintf(send_http.body, sizeof(send_http.body), 
		"{\"token\":\"%s\",\"token_type\":\"%d\",\"no\":\"%d\"}", mobile_token, token_type, no);

	tool_sock_item_t sock;
	tool_mem_memset(&sock, sizeof(tool_sock_item_t));
	if (tool_sock_item_open_v2(&sock, TOOL_SOCK_TYPE1_TCP, TOOL_SOCK_TYPE2_CLIENT, server_ip, server_port, NULL, 0) < 0)
	{
		log_error("tool_sock_item_open");
		return NC_ERROR_CONNECT;
	}
	ret = 0;
	do
	{
		tool_http_Init(&recv_http);	
		if (tool_http_Send(&send_http, &sock, NULL, NULL) < 0)
		{
			log_error("tool_http_Send");
			ret = NC_ERROR_SEND;
			break;
		}
		if (tool_http_Recv(&recv_http, &sock, NULL, NULL) < 0)
		{
			log_error("tool_http_Send");
			ret = NC_ERROR_RECV;
			break;
		}
		if (tool_str_strncmp(recv_http.command, "200", tool_str_strlen("200") != 0))
		{
			log_error("recv_http.command(%s)", recv_http.command);
			ret = NC_ERROR_PROTOCOL;
			break;
		}
		
		tool_json_t st_json;
		if (tool_json_parse(&st_json, recv_http.body) < 0)
		{
			log_error("recv_http.body(%s)", recv_http.body);
			ret = NC_ERROR_PROTOCOL;
			break;
		}
		
		TOOL_INT8* result = tool_json_getValue(&st_json, "result");
		if (result == NULL)
		{
			log_error("result(%d)", result);
			ret = NC_ERROR_PROTOCOL;
			break;
		}
		if (tool_str_strncmp(result, "0", 1) != 0)
		{
			log_error("result(%s)", result);
			ret = NC_ERROR_PROTOCOL;
			break;
		}
		log_state("result(%s)", result);

		TOOL_INT32 i = 0;
		TOOL_INT8* no = NULL;
		TOOL_INT8* dev_id = NULL;
		TOOL_INT8* time = NULL;
		TOOL_INT8* channel = NULL;
		TOOL_INT8* type = NULL;
		for (i = 0; i < HDCCTV_QPNS_ALARM_NUM; i++)
		{
			no      = tool_json_getValue_V2(&st_json, "alarm_map", i, "no");
			dev_id  = tool_json_getValue_V2(&st_json, "alarm_map", i, "dev_id");
			time    = tool_json_getValue_V2(&st_json, "alarm_map", i, "time");
			channel = tool_json_getValue_V2(&st_json, "alarm_map", i, "channel");
			type    = tool_json_getValue_V2(&st_json, "alarm_map", i, "type");
			if (no == NULL || dev_id == NULL || time == NULL || channel == NULL || type == NULL)
				break;
			tool_mem_memset(&alarms->alarm[alarms->num], sizeof(hdcctv_qpns_alarm_t));
			tool_str_strncpy(alarms->alarm[alarms->num].dev_id, dev_id, HDCCTV_QPNS_TOKEN_SIZE-1);
			alarms->alarm[alarms->num].no = tool_sysf_atoi(no);
			alarms->alarm[alarms->num].time = tool_sysf_atoi(time);
			alarms->alarm[alarms->num].channel = tool_sysf_atoi(channel);
			alarms->alarm[alarms->num].type = tool_sysf_atoi(type);
			alarms->num ++;
		}
	} while (0);
	tool_sock_item_close(&sock);
	return ret;		
}

TOOL_INT32 hdcctv_client2_getQpnsDeviceMaps(HDCCTV_QPNS_TOKEN_TYPE_E token_type, TOOL_INT8* mobile_token, hdcctv_qpns_device_maps_t* device_maps)
{
	if (device_maps == NULL || mobile_token == NULL)
	{
		log_error("device_maps(%d) mobile_token(%d)", device_maps, mobile_token);
		return NC_ERROR_INPUT;
	}

////////////////////////////////////////////////
////////////////////////////////////////////////
////          hdcctv_client2_location
///////////////////////////////////////////////
////////////////////////////////////////////////
/////////////////////////////////////////////

	TOOL_INT8 server_ip[PTC_IP_SIZE] = {0};
	TOOL_INT32 server_port = 0;
	TOOL_INT32 ret = hdcctv_client2_location(server_ip, &server_port);
	if (ret < 0)
	{
		log_error("hdcctv_client2_location");
		return ret;
	}

	device_maps->num = 0;

	tool_http_t send_http;
	tool_http_t recv_http;
	tool_http_Init(&send_http);
	tool_str_strncpy(send_http.command, "POST", tool_str_strlen("POST"));
	tool_str_strncpy(send_http.url, "/u/qpns_c.php", tool_str_strlen("/u/qpns_c.php"));
	tool_str_strncpy(send_http.version, "HTTP/1.0", tool_str_strlen("HTTP/1.0"));
	send_http.ContentLength = snprintf(send_http.body, sizeof(send_http.body), 
		"{\"token\":\"%s\",\"token_type\":\"%d\",\"action\":\"get\"}", mobile_token, token_type);

	tool_sock_item_t sock;
	tool_mem_memset(&sock, sizeof(tool_sock_item_t));
	if (tool_sock_item_open_v2(&sock, TOOL_SOCK_TYPE1_TCP, TOOL_SOCK_TYPE2_CLIENT, server_ip, server_port, NULL, 0) < 0)
	{
		log_error("tool_sock_item_open");
		return NC_ERROR_CONNECT;
	}
	ret = 0;
	do
	{
		tool_http_Init(&recv_http);	
		if (tool_http_Send(&send_http, &sock, NULL, NULL) < 0)
		{
			log_error("tool_http_Send");
			ret = NC_ERROR_SEND;
			break;
		}
		if (tool_http_Recv(&recv_http, &sock, NULL, NULL) < 0)
		{
			log_error("tool_http_Send");
			ret = NC_ERROR_RECV;
			break;
		}
		if (tool_str_strncmp(recv_http.command, "200", tool_str_strlen("200") != 0))
		{
			log_error("recv_http.command(%s)", recv_http.command);
			ret = NC_ERROR_PROTOCOL;
			break;
		}
		
		tool_json_t st_json;
		if (tool_json_parse(&st_json, recv_http.body) < 0)
		{
			log_error("recv_http.body(%s)", recv_http.body);
			ret = NC_ERROR_PROTOCOL;
			break;
		}
		
		TOOL_INT8* result = tool_json_getValue(&st_json, "result");
		if (result == NULL)
		{
			log_error("result(%d)", result);
			ret = NC_ERROR_PROTOCOL;
			break;
		}
		if (tool_str_strncmp(result, "0", 1) != 0)
		{
			log_error("result(%s)", result);
			ret = NC_ERROR_PROTOCOL;
			break;
		}
		log_state("result(%s)", result);

		TOOL_INT32 i = 0;
		TOOL_INT8* dev_id = NULL;
		for (i = 0; i < HDCCTV_QPNS_DEVICE_MAP_NUM; i++)
		{
			dev_id = tool_json_getValue_V2(&st_json, "device_map", i, "dev_id");
			if (dev_id == NULL)
				break;
			tool_mem_memset(device_maps->device_map[device_maps->num].dev_id, HDCCTV_QPNS_TOKEN_SIZE);
			tool_str_strncpy(device_maps->device_map[device_maps->num].dev_id, dev_id, HDCCTV_QPNS_TOKEN_SIZE-1);
			device_maps->num ++;
		}
	} while (0);
	tool_sock_item_close(&sock);
	return ret;		
}

TOOL_INT32 hdcctv_client2_setQpnsDeviceMaps(HDCCTV_QPNS_TOKEN_TYPE_E token_type, TOOL_INT8* mobile_token, hdcctv_qpns_device_maps_t* device_maps)
{
	if (mobile_token == NULL || device_maps == NULL || device_maps->num > HDCCTV_QPNS_DEVICE_MAP_NUM)
	{
		log_error("mobile_token(%d) device_maps(%d) num(%d)", mobile_token, device_maps, device_maps->num);
		return NC_ERROR_INPUT;
	}

	TOOL_INT8 server_ip[PTC_IP_SIZE] = {0};
	TOOL_INT32 server_port = 0;
	TOOL_INT32 ret = hdcctv_client2_location(server_ip, &server_port);
	if (ret < 0)
	{
		log_error("hdcctv_client2_location");
		return ret;
	}

	tool_http_t send_http;
	tool_http_t recv_http;
	tool_http_Init(&send_http);
	tool_str_strncpy(send_http.command, "POST", tool_str_strlen("POST"));
	tool_str_strncpy(send_http.url, "/u/qpns_c.php", tool_str_strlen("/u/qpns_c.php"));
	tool_str_strncpy(send_http.version, "HTTP/1.0", tool_str_strlen("HTTP/1.0"));
	send_http.ContentLength = snprintf(send_http.body, sizeof(send_http.body), 
		"{\"token\":\"%s\",\"token_type\":\"%d\",\"action\":\"set\",\"device_map\":", mobile_token, token_type);
	log_debug("device_maps->num(%d)", device_maps->num);
	if (device_maps->num == 0)
	{
		tool_str_strncat(send_http.body, "null", sizeof(send_http.body)-send_http.ContentLength);
		send_http.ContentLength += 4;
	}
	else
	{
		tool_str_strncat(send_http.body, "[", sizeof(send_http.body)-send_http.ContentLength);
		send_http.ContentLength ++;
		TOOL_INT8 buf[1024]= {0};
		TOOL_INT32 buf_len = 0;
		TOOL_INT32 i = 0;
		for (i = 0; i < device_maps->num; i++)
		{
			buf_len = snprintf(buf, sizeof(buf), "{\"dev_id\":\"%s\"}", device_maps->device_map[i].dev_id);
			tool_str_strncat(send_http.body, buf, sizeof(send_http.body)-send_http.ContentLength);
			send_http.ContentLength += buf_len;
			
			if (i != device_maps->num-1)
			{
				tool_str_strncat(send_http.body, ",", sizeof(send_http.body)-send_http.ContentLength);
				send_http.ContentLength ++;
			}
		}
		tool_str_strncat(send_http.body, "]", sizeof(send_http.body)-send_http.ContentLength);
		send_http.ContentLength ++;
	}
	tool_str_strncat(send_http.body, "}", sizeof(send_http.body)-send_http.ContentLength);
	send_http.ContentLength ++;
	log_debug("send_http.body(%s)", send_http.body);

	tool_sock_item_t sock;
	tool_mem_memset(&sock, sizeof(tool_sock_item_t));
	if (tool_sock_item_open_v2(&sock, TOOL_SOCK_TYPE1_TCP, TOOL_SOCK_TYPE2_CLIENT, server_ip, server_port, NULL, 0) < 0)
	{
		log_error("tool_sock_item_open");
		return NC_ERROR_CONNECT;
	}
	ret = 0;
	do
	{
		tool_http_Init(&recv_http);	
		if (tool_http_Send(&send_http, &sock, NULL, NULL) < 0)
		{
			log_error("tool_http_Send");
			ret = NC_ERROR_SEND;
			break;
		}
		if (tool_http_Recv(&recv_http, &sock, NULL, NULL) < 0)
		{
			log_error("tool_http_Send");
			ret = NC_ERROR_RECV;
			break;
		}
		if (tool_str_strncmp(recv_http.command, "200", tool_str_strlen("200") != 0))
		{
			log_error("recv_http.command(%s)", recv_http.command);
			ret = NC_ERROR_PROTOCOL;
			break;
		}

		tool_json_t st_json;
		if (tool_json_parse(&st_json, recv_http.body) < 0)
		{
			log_error("recv_http.body(%s)", recv_http.body);
			ret = NC_ERROR_PROTOCOL;
			break;
		}
		
		TOOL_INT8* result = tool_json_getValue(&st_json, "result");
		if (result == NULL)
		{
			log_error("result(%d)", result);
			ret = NC_ERROR_PROTOCOL;
			break;
		}
		if (tool_str_strncmp(result, "0", 1) != 0)
		{
			log_error("result(%s)", result);
			ret = NC_ERROR_PROTOCOL;
			break;
		}
		log_state("result(%s)", result);
	} while (0);
	tool_sock_item_close(&sock);
	return ret;		
}

TOOL_INT32 hdcctv_client2_addQpnsDeviceMaps(HDCCTV_QPNS_TOKEN_TYPE_E token_type, TOOL_INT8* mobile_token, hdcctv_qpns_device_maps_t* device_maps)
{
	if (mobile_token == NULL || device_maps == NULL || device_maps->num <= 0 || device_maps->num > HDCCTV_QPNS_DEVICE_MAP_NUM)
	{
		log_error("mobile_token(%d) device_maps(%d) num(%d)", mobile_token, device_maps, device_maps->num);
		return NC_ERROR_INPUT;
	}

	TOOL_INT8 server_ip[PTC_IP_SIZE] = {0};
	TOOL_INT32 server_port = 0;
	TOOL_INT32 ret = hdcctv_client2_location(server_ip, &server_port);
	if (ret < 0)
	{
		log_error("hdcctv_client2_location");
		return ret;
	}

	tool_http_t send_http;
	tool_http_t recv_http;
	tool_http_Init(&send_http);
	tool_str_strncpy(send_http.command, "POST", tool_str_strlen("POST"));
	tool_str_strncpy(send_http.url, "/u/qpns_c.php", tool_str_strlen("/u/qpns_c.php"));
	tool_str_strncpy(send_http.version, "HTTP/1.0", tool_str_strlen("HTTP/1.0"));
	send_http.ContentLength = snprintf(send_http.body, sizeof(send_http.body), 
		"{\"token\":\"%s\",\"token_type\":\"%d\",\"action\":\"add\",\"device_map\":", mobile_token, token_type);

	tool_str_strncat(send_http.body, "[", sizeof(send_http.body)-send_http.ContentLength);
	send_http.ContentLength ++;
	TOOL_INT8 buf[1024]= {0};
	TOOL_INT32 buf_len = 0;
	TOOL_INT32 i = 0;
	for (i = 0; i < device_maps->num; i++)
	{
		buf_len = snprintf(buf, sizeof(buf), "{\"dev_id\":\"%s\"}", device_maps->device_map[i].dev_id);
		tool_str_strncat(send_http.body, buf, sizeof(send_http.body)-send_http.ContentLength);
		send_http.ContentLength += buf_len;
		
		if (i != device_maps->num-1)
		{
			tool_str_strncat(send_http.body, ",", sizeof(send_http.body)-send_http.ContentLength);
			send_http.ContentLength ++;
		}
	}
	tool_str_strncat(send_http.body, "]", sizeof(send_http.body)-send_http.ContentLength);
	send_http.ContentLength ++;
		
	tool_str_strncat(send_http.body, "}", sizeof(send_http.body)-send_http.ContentLength);
	send_http.ContentLength ++;
	log_debug("send_http.body(%s)", send_http.body);

	tool_sock_item_t sock;
	tool_mem_memset(&sock, sizeof(tool_sock_item_t));
	if (tool_sock_item_open_v2(&sock, TOOL_SOCK_TYPE1_TCP, TOOL_SOCK_TYPE2_CLIENT, server_ip, server_port, NULL, 0) < 0)
	{
		log_error("tool_sock_item_open");
		return NC_ERROR_CONNECT;
	}
	ret = 0;
	do
	{
		tool_http_Init(&recv_http);	
		if (tool_http_Send(&send_http, &sock, NULL, NULL) < 0)
		{
			log_error("tool_http_Send");
			ret = NC_ERROR_SEND;
			break;
		}
		if (tool_http_Recv(&recv_http, &sock, NULL, NULL) < 0)
		{
			log_error("tool_http_Send");
			ret = NC_ERROR_RECV;
			break;
		}
		if (tool_str_strncmp(recv_http.command, "200", tool_str_strlen("200") != 0))
		{
			log_error("recv_http.command(%s)", recv_http.command);
			ret = NC_ERROR_PROTOCOL;
			break;
		}

		tool_json_t st_json;
		if (tool_json_parse(&st_json, recv_http.body) < 0)
		{
			log_error("recv_http.body(%s)", recv_http.body);
			ret = NC_ERROR_PROTOCOL;
			break;
		}
		
		TOOL_INT8* result = tool_json_getValue(&st_json, "result");
		if (result == NULL)
		{
			log_error("result(%d)", result);
			ret = NC_ERROR_PROTOCOL;
			break;
		}
		if (tool_str_strncmp(result, "0", 1) != 0)
		{
			log_error("result(%s)", result);
			ret = NC_ERROR_PROTOCOL;
			break;
		}
		log_state("result(%s)", result);
	} while (0);
	tool_sock_item_close(&sock);
	return ret;		
}

TOOL_INT32 hdcctv_client2_delQpnsDeviceMaps(HDCCTV_QPNS_TOKEN_TYPE_E token_type, TOOL_INT8* mobile_token, hdcctv_qpns_device_maps_t* device_maps)
{
	if (mobile_token == NULL || device_maps == NULL || device_maps->num <= 0 || device_maps->num > HDCCTV_QPNS_DEVICE_MAP_NUM)
	{
		log_error("mobile_token(%d) device_maps(%d) num(%d)", mobile_token, device_maps, device_maps->num);
		return NC_ERROR_INPUT;
	}

	TOOL_INT8 server_ip[PTC_IP_SIZE] = {0};
	TOOL_INT32 server_port = 0;
	TOOL_INT32 ret = hdcctv_client2_location(server_ip, &server_port);
	if (ret < 0)
	{
		log_error("hdcctv_client2_location");
		return ret;
	}

	tool_http_t send_http;
	tool_http_t recv_http;
	tool_http_Init(&send_http);
	tool_str_strncpy(send_http.command, "POST", tool_str_strlen("POST"));
	tool_str_strncpy(send_http.url, "/u/qpns_c.php", tool_str_strlen("/u/qpns_c.php"));
	tool_str_strncpy(send_http.version, "HTTP/1.0", tool_str_strlen("HTTP/1.0"));
	send_http.ContentLength = snprintf(send_http.body, sizeof(send_http.body), 
		"{\"token\":\"%s\",\"token_type\":\"%d\",\"action\":\"del\",\"device_map\":", mobile_token, token_type);

	tool_str_strncat(send_http.body, "[", sizeof(send_http.body)-send_http.ContentLength);
	send_http.ContentLength ++;
	TOOL_INT8 buf[1024]= {0};
	TOOL_INT32 buf_len = 0;
	TOOL_INT32 i = 0;
	for (i = 0; i < device_maps->num; i++)
	{
		buf_len = snprintf(buf, sizeof(buf), "{\"dev_id\":\"%s\"}", device_maps->device_map[i].dev_id);
		tool_str_strncat(send_http.body, buf, sizeof(send_http.body)-send_http.ContentLength);
		send_http.ContentLength += buf_len;
		
		if (i != device_maps->num-1)
		{
			tool_str_strncat(send_http.body, ",", sizeof(send_http.body)-send_http.ContentLength);
			send_http.ContentLength ++;
		}
	}
	tool_str_strncat(send_http.body, "]", sizeof(send_http.body)-send_http.ContentLength);
	send_http.ContentLength ++;
		
	tool_str_strncat(send_http.body, "}", sizeof(send_http.body)-send_http.ContentLength);
	send_http.ContentLength ++;
	log_debug("send_http.body(%s)", send_http.body);

	tool_sock_item_t sock;
	tool_mem_memset(&sock, sizeof(tool_sock_item_t));
	if (tool_sock_item_open_v2(&sock, TOOL_SOCK_TYPE1_TCP, TOOL_SOCK_TYPE2_CLIENT, server_ip, server_port, NULL, 0) < 0)
	{
		log_error("tool_sock_item_open");
		return NC_ERROR_CONNECT;
	}
	ret = 0;
	do
	{
		tool_http_Init(&recv_http);	
		if (tool_http_Send(&send_http, &sock, NULL, NULL) < 0)
		{
			log_error("tool_http_Send");
			ret = NC_ERROR_SEND;
			break;
		}
		if (tool_http_Recv(&recv_http, &sock, NULL, NULL) < 0)
		{
			log_error("tool_http_Send");
			ret = NC_ERROR_RECV;
			break;
		}
		if (tool_str_strncmp(recv_http.command, "200", tool_str_strlen("200") != 0))
		{
			log_error("recv_http.command(%s)", recv_http.command);
			ret = NC_ERROR_PROTOCOL;
			break;
		}

		tool_json_t st_json;
		if (tool_json_parse(&st_json, recv_http.body) < 0)
		{
			log_error("recv_http.body(%s)", recv_http.body);
			ret = NC_ERROR_PROTOCOL;
			break;
		}
		
		TOOL_INT8* result = tool_json_getValue(&st_json, "result");
		if (result == NULL)
		{
			log_error("result(%d)", result);
			ret = NC_ERROR_PROTOCOL;
			break;
		}
		if (tool_str_strncmp(result, "0", 1) != 0)
		{
			log_error("result(%s)", result);
			ret = NC_ERROR_PROTOCOL;
			break;
		}
		log_state("result(%s)", result);
	} while (0);
	tool_sock_item_close(&sock);
	return ret;		
}


TOOL_INT32 hdcctv_client2_testQpns(HDCCTV_QPNS_TOKEN_TYPE_E token_type, TOOL_INT8* mobile_token, TOOL_INT8* dev_id)
{
	if (mobile_token == NULL || dev_id == NULL)
	{
		log_error("mobile_token(%d) dev_id(%x)", mobile_token, dev_id);
		return NC_ERROR_INPUT;
	}

	TOOL_INT8 server_ip[PTC_IP_SIZE] = {0};
	TOOL_INT32 server_port = 0;
	TOOL_INT32 ret = hdcctv_client2_location(server_ip, &server_port);
	if (ret < 0)
	{
		log_error("hdcctv_client2_location");
		return ret;
	}

	tool_time_t tt;
	tool_sysf_time(&tt);
	TOOL_INT32 channel = 0;
	TOOL_INT32 type = PTC_LOG_QPNS_TEST;
	TOOL_INT8 content[HDCCTV_QPNS_CONTENT_SIZE] = "This is a test message from [HDCCTV]!";

	tool_http_t send_http;
	tool_http_t recv_http;
	tool_http_Init(&send_http);
	tool_str_strncpy(send_http.command, "POST", tool_str_strlen("POST"));
	tool_str_strncpy(send_http.url, "/u/qpns_d.php", tool_str_strlen("/u/qpns_d.php"));
	tool_str_strncpy(send_http.version, "HTTP/1.0", tool_str_strlen("HTTP/1.0"));
	send_http.ContentLength = snprintf(send_http.body, sizeof(send_http.body), 
		"{\"dev_id\":\"%s\",\"alarm\":[{\"time\":\"%d\",\"channel\":\"%d\",\"type\":\"%d\",\"content\":\"%s\"}]}", 
		dev_id, tt.sec, channel, type, content);

	tool_sock_item_t sock;
	tool_mem_memset(&sock, sizeof(tool_sock_item_t));
	if (tool_sock_item_open_v2(&sock, TOOL_SOCK_TYPE1_TCP, TOOL_SOCK_TYPE2_CLIENT, server_ip, server_port, NULL, 0) < 0)
	{
		log_error("tool_sock_item_open");
		return NC_ERROR_CONNECT;
	}
	ret = 0;
	do
	{
		tool_http_Init(&recv_http);	
		if (tool_http_Send(&send_http, &sock, NULL, NULL) < 0)
		{
			log_error("tool_http_Send");
			ret = NC_ERROR_SEND;
			break;
		}
		if (tool_http_Recv(&recv_http, &sock, NULL, NULL) < 0)
		{
			log_error("tool_http_Send");
			ret = NC_ERROR_RECV;
			break;
		}
		if (tool_str_strncmp(recv_http.command, "200", tool_str_strlen("200") != 0))
		{
			log_error("recv_http.command(%s)", recv_http.command);
			ret = NC_ERROR_PROTOCOL;
			break;
		}
		
		tool_json_t st_json;
		if (tool_json_parse(&st_json, recv_http.body) < 0)
		{
			log_error("recv_http.body(%s)", recv_http.body);
			ret = NC_ERROR_PROTOCOL;
			break;
		}
		
		TOOL_INT8* result = tool_json_getValue(&st_json, "result");
		if (result == NULL)
		{
			log_error("result(%d)", result);
			ret = NC_ERROR_PROTOCOL;
			break;
		}
		if (tool_str_strncmp(result, "0", 1) != 0)
		{
			log_error("result(%s)", result);
			ret = NC_ERROR_PROTOCOL;
			break;
		}
		log_state("result(%s)", result);
	} while (0);
	tool_sock_item_close(&sock);
	return ret;	
}

TOOL_INT32 hdcctv_client2_getSms(hdcctv_cloud_user_t* pst_cloud_user, TOOL_INT8* action)
{	
	if (pst_cloud_user == NULL)
	{
		log_error("pst_cloud_user(%x)", pst_cloud_user);
		return NC_ERROR_INPUT;
	}

	TOOL_INT8 server_ip[PTC_IP_SIZE] = {0};
	TOOL_INT32 server_port = 0;
	TOOL_INT32 ret = hdcctv_client2_location(server_ip, &server_port);
	if (ret < 0)
	{
		log_error("hdcctv_client2_location");
		return ret;
	}

	tool_http_t send_http;
	tool_http_t recv_http;
	tool_http_Init(&send_http);
	tool_str_strncpy(send_http.command, "POST", tool_str_strlen("POST"));
	tool_str_strncpy(send_http.url, "/u/sms.php", tool_str_strlen("/u/sms.php"));
	tool_str_strncpy(send_http.version, "HTTP/1.0", tool_str_strlen("HTTP/1.0"));
	send_http.ContentLength = snprintf(send_http.body, sizeof(send_http.body), 
		"{\"number\":\"%s\",\"username\":\"%s\",\"email\":\"%s\",\"action\":\"%s\"}", 
		pst_cloud_user->mobile, pst_cloud_user->username, pst_cloud_user->email, action);

	tool_sock_item_t sock;
	tool_mem_memset(&sock, sizeof(tool_sock_item_t));
	if (tool_sock_item_open_v2(&sock, TOOL_SOCK_TYPE1_TCP, TOOL_SOCK_TYPE2_CLIENT, server_ip, server_port, NULL, 0) < 0)
	{
		log_error("tool_sock_item_open");
		return NC_ERROR_CONNECT;
	}
	ret = 0;
	do
	{
		tool_http_Init(&recv_http);	
		if (tool_http_Send(&send_http, &sock, NULL, NULL) < 0)
		{
			log_error("tool_http_Send");
			ret = NC_ERROR_SEND;
			break;
		}
		if (tool_http_Recv(&recv_http, &sock, NULL, NULL) < 0)
		{
			log_error("tool_http_Send");
			ret = NC_ERROR_RECV;
			break;
		}
		if (tool_str_strncmp(recv_http.command, "200", tool_str_strlen("200") != 0))
		{
			log_error("recv_http.command(%s)", recv_http.command);
			ret = NC_ERROR_PROTOCOL;
			break;
		}
		tool_json_t st_json;
		if (tool_json_parse(&st_json, recv_http.body) < 0)
		{
			log_error("recv_http.body(%s)", recv_http.body);
			ret = NC_ERROR_PROTOCOL;
			break;
		}
		TOOL_INT8* result = tool_json_getValue(&st_json, "result");
		if (result == NULL)
		{
			log_error("result(%d)", result);
			ret = NC_ERROR_PROTOCOL;
			break;
		}
		if (tool_str_strncmp(result, "0", 1) != 0)
		{
			TOOL_INT32 result_num = tool_sysf_atoi(result);
			result_num *= (-1);
			if (result_num < NC_ERROR_CLOUD_BEGIN && result_num > NC_ERROR_CLOUD_END)
				ret = result_num;
			else
				ret = NC_ERROR_PROTOCOL;
			log_error("result(%s)", result);
			break;
		}
	} while (0);
	tool_sock_item_close(&sock);
	return ret;	
}

TOOL_INT32 hdcctv_client2_regUser(hdcctv_cloud_user_t* pst_user, TOOL_INT8* code)
{
	if (pst_user == NULL || code == NULL)
	{
		log_error("pst_user(%x) code(%x)", pst_user, code);
		return NC_ERROR_INPUT;
	}

	TOOL_INT8 server_ip[PTC_IP_SIZE] = {0};
	TOOL_INT32 server_port = 0;
	TOOL_INT32 ret = hdcctv_client2_location(server_ip, &server_port);
	if (ret < 0)
	{
		log_error("hdcctv_client2_location");
		return ret;
	}	

	TOOL_UINT8 password_byte[16] = {0};
	tool_md5_calc((TOOL_UINT8*)pst_user->password, tool_str_strlen(pst_user->password), password_byte);
	TOOL_INT8 password_str[64] = {0};
	tool_str_byte2str(password_byte, 16, password_str);
	log_debug("password_str(%s)", password_str);
	tool_http_t send_http;
	tool_http_t recv_http;
	tool_http_Init(&send_http);
	tool_str_strncpy(send_http.command, "POST", tool_str_strlen("POST"));
	tool_str_strncpy(send_http.url, "/u/reg.php", tool_str_strlen("/u/sms.php"));
	tool_str_strncpy(send_http.version, "HTTP/1.0", tool_str_strlen("HTTP/1.0"));
	send_http.ContentLength = snprintf(send_http.body, sizeof(send_http.body), 
		"{\"number\":\"%s\",\"username\":\"%s\",\"password\":\"%s\",\"code\":\"%s\",\"email\":\"%s\"}", 
		pst_user->mobile, pst_user->username, password_str, code, pst_user->email);

	tool_sock_item_t sock;
	tool_mem_memset(&sock, sizeof(tool_sock_item_t));
	if (tool_sock_item_open_v2(&sock, TOOL_SOCK_TYPE1_TCP, TOOL_SOCK_TYPE2_CLIENT, server_ip, server_port, NULL, 0) < 0)
	{
		log_error("tool_sock_item_open");
		return NC_ERROR_CONNECT;
	}
	ret = 0;
	do
	{
		tool_http_Init(&recv_http);	
		if (tool_http_Send(&send_http, &sock, NULL, NULL) < 0)
		{
			log_error("tool_http_Send");
			ret = NC_ERROR_SEND;
			break;
		}
		if (tool_http_Recv(&recv_http, &sock, NULL, NULL) < 0)
		{
			log_error("tool_http_Send");
			ret = NC_ERROR_RECV;
			break;
		}
		if (tool_str_strncmp(recv_http.command, "200", tool_str_strlen("200") != 0))
		{
			log_error("recv_http.command(%s)", recv_http.command);
			ret = NC_ERROR_PROTOCOL;
			break;
		}
		tool_json_t st_json;
		if (tool_json_parse(&st_json, recv_http.body) < 0)
		{
			log_error("recv_http.body(%s)", recv_http.body);
			ret = NC_ERROR_PROTOCOL;
			break;
		}
		TOOL_INT8* result = tool_json_getValue(&st_json, "result");
		if (result == NULL)
		{
			log_error("result(%d)", result);
			ret = NC_ERROR_PROTOCOL;
			break;
		}
		if (tool_str_strncmp(result, "0", 1) != 0)
		{
			TOOL_INT32 result_num = tool_sysf_atoi(result);
			result_num *= (-1);
			if (result_num < NC_ERROR_CLOUD_BEGIN && result_num > NC_ERROR_CLOUD_END)
				ret = result_num;
			else
				ret = NC_ERROR_PROTOCOL;
			log_error("result(%s)", result);
			break;
		}
	} while (0);
	tool_sock_item_close(&sock);
	return ret;		
}

TOOL_INT32 hdcctv_client2_resetPswd(hdcctv_cloud_user_t* pst_user, TOOL_INT8* code)
{
	if (pst_user == NULL || code == NULL)
	{
		log_error("pst_user(%x) code(%x)", pst_user, code);
		return NC_ERROR_INPUT;
	}

	TOOL_INT8 server_ip[PTC_IP_SIZE] = {0};
	TOOL_INT32 server_port = 0;
	TOOL_INT32 ret = hdcctv_client2_location(server_ip, &server_port);
	if (ret < 0)
	{
		log_error("hdcctv_client2_location");
		return ret;
	}	

	TOOL_UINT8 password_byte[16] = {0};
	tool_md5_calc((TOOL_UINT8*)pst_user->password, tool_str_strlen(pst_user->password), password_byte);
	TOOL_INT8 password_str[64] = {0};
	tool_str_byte2str(password_byte, 16, password_str);
	log_debug("password_str(%s)", password_str);
	tool_http_t send_http;
	tool_http_t recv_http;
	tool_http_Init(&send_http);
	tool_str_strncpy(send_http.command, "POST", tool_str_strlen("POST"));
	tool_str_strncpy(send_http.url, "/u/reset.php", tool_str_strlen("/u/reset.php"));
	tool_str_strncpy(send_http.version, "HTTP/1.0", tool_str_strlen("HTTP/1.0"));
	send_http.ContentLength = snprintf(send_http.body, sizeof(send_http.body), 
		"{\"number\":\"%s\",\"username\":\"%s\",\"password\":\"%s\",\"code\":\"%s\",\"email\":\"%s\"}", 
		pst_user->mobile, pst_user->username, password_str, code, pst_user->email);

	tool_sock_item_t sock;
	tool_mem_memset(&sock, sizeof(tool_sock_item_t));
	if (tool_sock_item_open_v2(&sock, TOOL_SOCK_TYPE1_TCP, TOOL_SOCK_TYPE2_CLIENT, server_ip, server_port, NULL, 0) < 0)
	{
		log_error("tool_sock_item_open");
		return NC_ERROR_CONNECT;
	}
	ret = 0;
	do
	{
		tool_http_Init(&recv_http);	
		if (tool_http_Send(&send_http, &sock, NULL, NULL) < 0)
		{
			log_error("tool_http_Send");
			ret = NC_ERROR_SEND;
			break;
		}
		if (tool_http_Recv(&recv_http, &sock, NULL, NULL) < 0)
		{
			log_error("tool_http_Send");
			ret = NC_ERROR_RECV;
			break;
		}
		if (tool_str_strncmp(recv_http.command, "200", tool_str_strlen("200") != 0))
		{
			log_error("recv_http.command(%s)", recv_http.command);
			ret = NC_ERROR_PROTOCOL;
			break;
		}
		tool_json_t st_json;
		if (tool_json_parse(&st_json, recv_http.body) < 0)
		{
			log_error("recv_http.body(%s)", recv_http.body);
			ret = NC_ERROR_PROTOCOL;
			break;
		}
		TOOL_INT8* result = tool_json_getValue(&st_json, "result");
		if (result == NULL || tool_str_strncmp(result, "0", 1) != 0)
		{
			log_error("recv_http.body(%s)", recv_http.body);
			ret = NC_ERROR_PROTOCOL;
			break;
		}
	} while (0);
	tool_sock_item_close(&sock);
	return ret;		
}

TOOL_INT32 hdcctv_client2_loginUser(hdcctv_cloud_user_t* pst_user)
{
	if (g_hdcctv_client2 == NULL)
	{
		log_error("g_hdcctv_client2");
		return NC_ERROR_UNINIT;
	}
	if (pst_user == NULL)
	{
		log_error("pst_user(%x)", pst_user);
		return NC_ERROR_INPUT;
	}

	tool_thread_lockMutex(&g_hdcctv_client2->mutex);
	g_hdcctv_client2->login_state = 1;
	g_hdcctv_client2->list_state = 0;
	g_hdcctv_client2->list_result = 0;
	g_hdcctv_client2->login_tick = -12345678;	
	tool_mem_memcpy(&g_hdcctv_client2->st_cloud_user, pst_user, sizeof(hdcctv_cloud_user_t));
	tool_thread_unlockMutex(&g_hdcctv_client2->mutex);
	return 0;
}

TOOL_INT32 hdcctv_client2_logout()
{
	if (g_hdcctv_client2 == NULL)
	{
		log_error("g_hdcctv_client2");
		return NC_ERROR_UNINIT;
	}

	tool_thread_lockMutex(&g_hdcctv_client2->mutex);
	g_hdcctv_client2->login_state = 0;
	g_hdcctv_client2->list_state = 0;
	g_hdcctv_client2->list_result = 0;
	g_hdcctv_client2->login_tick = -12345678;	
	tool_thread_unlockMutex(&g_hdcctv_client2->mutex);
	return 0;
}

TOOL_INT32 hdcctv_client2_setPswd(hdcctv_cloud_user_t* pst_user, TOOL_INT8* new_pswd)
{
	if (pst_user == NULL || new_pswd == NULL)
	{
		log_error("pst_user(%x) new_pswd(%x)", pst_user, new_pswd);
		return NC_ERROR_INPUT;
	}

	TOOL_INT8 server_ip[PTC_IP_SIZE] = {0};
	TOOL_INT32 server_port = 0;
	TOOL_INT32 ret = hdcctv_client2_location(server_ip, &server_port);
	if (ret < 0)
	{
		log_error("hdcctv_client2_location");
		return ret;
	}
	
	TOOL_UINT8 old_password_byte[16] = {0};
	TOOL_UINT8 new_password_byte[16] = {0};
	tool_md5_calc((TOOL_UINT8*)pst_user->password, tool_str_strlen(pst_user->password), old_password_byte);
	tool_md5_calc((TOOL_UINT8*)new_pswd, tool_str_strlen(new_pswd), new_password_byte);
	TOOL_INT8 old_password_str[64] = {0};
	TOOL_INT8 new_password_str[64] = {0};
	tool_str_byte2str(old_password_byte, 16, old_password_str);
	tool_str_byte2str(new_password_byte, 16, new_password_str);
	log_debug("old_password_byte(%s) new_password_str(%s)", old_password_str, new_password_str);
	tool_http_t send_http;
	tool_http_t recv_http;
	tool_http_Init(&send_http);
	tool_str_strncpy(send_http.command, "POST", tool_str_strlen("POST"));
	tool_str_strncpy(send_http.url, "/u/pswd.php", tool_str_strlen("/u/pswd.php"));
	tool_str_strncpy(send_http.version, "HTTP/1.0", tool_str_strlen("HTTP/1.0"));
	send_http.ContentLength = snprintf(send_http.body, sizeof(send_http.body), 
		"{\"username\":\"%s\",\"number\":\"%s\",\"email\":\"%s\",\"old_password\":\"%s\",\"new_password\":\"%s\"}", 
		pst_user->username, pst_user->mobile, pst_user->email, old_password_str, new_password_str);

	tool_sock_item_t sock;
	tool_mem_memset(&sock, sizeof(tool_sock_item_t));
	if (tool_sock_item_open_v2(&sock, TOOL_SOCK_TYPE1_TCP, TOOL_SOCK_TYPE2_CLIENT, server_ip, server_port, NULL, 0) < 0)
	{
		log_error("tool_sock_item_open");
		return NC_ERROR_CONNECT;
	}
	ret = 0;
	do
	{
		tool_http_Init(&recv_http);	
		if (tool_http_Send(&send_http, &sock, NULL, NULL) < 0)
		{
			log_error("tool_http_Send");
			ret = NC_ERROR_SEND;
			break;
		}
		if (tool_http_Recv(&recv_http, &sock, NULL, NULL) < 0)
		{
			log_error("tool_http_Send");
			ret = NC_ERROR_RECV;
			break;
		}
		if (tool_str_strncmp(recv_http.command, "200", tool_str_strlen("200") != 0))
		{
			log_error("recv_http.command(%s)", recv_http.command);
			ret = NC_ERROR_PROTOCOL;
			break;
		}
		tool_json_t st_json;
		if (tool_json_parse(&st_json, recv_http.body) < 0)
		{
			log_error("recv_http.body(%s)", recv_http.body);
			ret = NC_ERROR_PROTOCOL;
			break;
		}
		TOOL_INT8* result = tool_json_getValue(&st_json, "result");
		if (result == NULL)
		{
			log_error("result(%d)", result);
			ret = NC_ERROR_PROTOCOL;
			break;
		}
		if (tool_str_strncmp(result, "0", 1) != 0)
		{
			TOOL_INT32 result_num = tool_sysf_atoi(result);
			result_num *= (-1);
			if (result_num < NC_ERROR_CLOUD_BEGIN && result_num > NC_ERROR_CLOUD_END)
				ret = result_num;
			else
				ret = NC_ERROR_PROTOCOL;
			log_error("result(%s)", result);
			break;
		}
	} while (0);
	tool_sock_item_close(&sock);
	return ret;			
}

TOOL_INT32 hdcctv_client2_bindDev(hdcctv_cloud_user_t* pst_user, hdcctv_device_t* pst_device, TOOL_INT8* action)
{
	if (pst_user == NULL || pst_device == NULL)
	{
		log_error("pst_user(%x) pst_device(%x) action(%x)", pst_user, pst_device, action);
		return NC_ERROR_INPUT;
	}

	TOOL_INT8 server_ip[PTC_IP_SIZE] = {0};
	TOOL_INT32 server_port = 0;
	TOOL_INT32 ret = hdcctv_client2_location(server_ip, &server_port);
	if (ret < 0)
	{
		log_error("hdcctv_client2_location");
		return ret;
	}

	TOOL_UINT8 password_byte[16] = {0};
	tool_md5_calc((TOOL_UINT8*)pst_user->password, tool_str_strlen(pst_user->password), password_byte);
	TOOL_INT8 password_str[64] = {0};
	tool_str_byte2str(password_byte, 16, password_str);
	log_debug("password_str(%s)", password_str);

	tool_http_t send_http;
	tool_http_t recv_http;
	tool_http_Init(&send_http);
	tool_str_strncpy(send_http.command, "POST", tool_str_strlen("POST"));
	tool_str_strncpy(send_http.url, "/u/bind.php", tool_str_strlen("/u/bind.php"));
	tool_str_strncpy(send_http.version, "HTTP/1.0", tool_str_strlen("HTTP/1.0"));
	send_http.ContentLength = snprintf(send_http.body, sizeof(send_http.body), 
		"{\"username\":\"%s\",\"number\":\"%s\",\"email\":\"%s\",\"password\":\"%s\",\"dev_id\":\"%s\",\"dev_name\":\"%s\",\"action\":\"%s\"}", 
		pst_user->username, pst_user->mobile, pst_user->email, password_str, pst_device->dev_id, pst_device->dev_name, action);

	tool_sock_item_t sock;
	tool_mem_memset(&sock, sizeof(tool_sock_item_t));
	if (tool_sock_item_open_v2(&sock, TOOL_SOCK_TYPE1_TCP, TOOL_SOCK_TYPE2_CLIENT, server_ip, server_port, NULL, 0) < 0)
	{
		log_error("tool_sock_item_open");
		return NC_ERROR_CONNECT;
	}
	ret = 0;
	do
	{
		tool_http_Init(&recv_http);	
		if (tool_http_Send(&send_http, &sock, NULL, NULL) < 0)
		{
			log_error("tool_http_Send");
			ret = NC_ERROR_SEND;
			break;
		}
		if (tool_http_Recv(&recv_http, &sock, NULL, NULL) < 0)
		{
			log_error("tool_http_Send");
			ret = NC_ERROR_RECV;
			break;
		}
		if (tool_str_strncmp(recv_http.command, "200", tool_str_strlen("200") != 0))
		{
			log_error("recv_http.command(%s)", recv_http.command);
			ret = NC_ERROR_PROTOCOL;
			break;
		}
		tool_json_t st_json;
		if (tool_json_parse(&st_json, recv_http.body) < 0)
		{
			log_error("recv_http.body(%s)", recv_http.body);
			ret = NC_ERROR_PROTOCOL;
			break;
		}
		TOOL_INT8* result = tool_json_getValue(&st_json, "result");
		if (result == NULL)
		{
			log_error("result(%d)", result);
			ret = NC_ERROR_PROTOCOL;
			break;
		}
		if (tool_str_strncmp(result, "0", 1) != 0)
		{
			TOOL_INT32 result_num = tool_sysf_atoi(result);
			result_num *= (-1);
			if (result_num < NC_ERROR_CLOUD_BEGIN && result_num > NC_ERROR_CLOUD_END)
				ret = result_num;
			else
				ret = NC_ERROR_PROTOCOL;
			log_error("result(%s)", result);
			break;
		}
	} while (0);
	tool_sock_item_close(&sock);
	return ret;		
}

TOOL_INT32 hdcctv_client2_doDevList(hdcctv_cloud_user_t* pst_cloud_user, hdcctv_device_list_t* pst_device_list)
{
	pst_device_list->num = 0;

	TOOL_INT8 server_ip[PTC_IP_SIZE] = {0};
	TOOL_INT32 server_port = 0;
	TOOL_INT32 ret = hdcctv_client2_location(server_ip, &server_port);
	if (ret < 0)
	{
		log_error("hdcctv_client2_location");
		return ret;
	}	

	TOOL_UINT8 password_byte[16] = {0};
	tool_md5_calc((TOOL_UINT8*)pst_cloud_user->password, tool_str_strlen(pst_cloud_user->password), password_byte);
	TOOL_INT8 password_str[64] = {0};
	tool_str_byte2str(password_byte, 16, password_str);
//	log_debug("password_str(%s)", password_str);

	tool_http_t send_http;
	tool_http_t recv_http;
	tool_http_Init(&send_http);
	tool_str_strncpy(send_http.command, "POST", tool_str_strlen("POST"));
	tool_str_strncpy(send_http.url, "/u/list.php", tool_str_strlen("/u/list.php"));
	tool_str_strncpy(send_http.version, "HTTP/1.0", tool_str_strlen("HTTP/1.0"));
	send_http.ContentLength = snprintf(send_http.body, sizeof(send_http.body), 
		"{\"username\":\"%s\",\"number\":\"%s\",\"email\":\"%s\",\"password\":\"%s\"}", 
		pst_cloud_user->username, pst_cloud_user->mobile, pst_cloud_user->email, password_str);

//	log_debug("send_http.body(%s)", send_http.body);

	tool_sock_item_t sock;
	tool_mem_memset(&sock, sizeof(tool_sock_item_t));
	if (tool_sock_item_open_v2(&sock, TOOL_SOCK_TYPE1_TCP, TOOL_SOCK_TYPE2_CLIENT, server_ip, server_port, NULL, 0) < 0)
	{
		log_error("tool_sock_item_open");
		return NC_ERROR_CONNECT;
	}
	ret = 0;
	do
	{
		tool_http_Init(&recv_http);	
		if (tool_http_Send(&send_http, &sock, NULL, NULL) < 0)
		{
			log_error("tool_http_Send");
			ret = NC_ERROR_SEND;
			break;
		}
		if (tool_http_Recv(&recv_http, &sock, NULL, NULL) < 0)
		{
			log_error("tool_http_Send");
			ret = NC_ERROR_RECV;
			break;
		}
		if (tool_str_strncmp(recv_http.command, "200", tool_str_strlen("200") != 0))
		{
			log_error("recv_http.command(%s)", recv_http.command);
			ret = NC_ERROR_PROTOCOL;
			break;
		}
		tool_json_t st_json;
		if (tool_json_parse(&st_json, recv_http.body) < 0)
		{
			log_error("recv_http.body(%s)", recv_http.body);
			ret = NC_ERROR_PROTOCOL;
			break;
		}
		TOOL_INT8* result = tool_json_getValue(&st_json, "result");
		if (result == NULL)
		{
			log_error("result(%d)", result);
			ret = NC_ERROR_PROTOCOL;
			break;
		}
		if (tool_str_strncmp(result, "0", 1) != 0)
		{
			TOOL_INT32 result_num = tool_sysf_atoi(result);
			result_num *= (-1);
			if (result_num < NC_ERROR_CLOUD_BEGIN && result_num > NC_ERROR_CLOUD_END)
				ret = result_num;
			else
				ret = NC_ERROR_PROTOCOL;
			log_error("recv_http.body(%s)", recv_http.body);
			log_error("result(%s)", result);
			break;
		}

		TOOL_INT8* pch_username = tool_json_getValue(&st_json, "username");
		TOOL_INT8* pch_number = tool_json_getValue(&st_json, "number");
		TOOL_INT8* pch_email = tool_json_getValue(&st_json, "email");
		TOOL_INT8* pch_user_ipv4 = tool_json_getValue(&st_json, "user_ipv4");
		if (pch_username == NULL || pch_number == NULL || pch_email == NULL || pch_user_ipv4 == NULL)
		{
			ret = NC_ERROR_PROTOCOL;
			log_error("pch_username(%d) pch_number(%d) pch_email(%d)", pch_username, pch_number, pch_email);
			break;
		}
		tool_mem_memset(pst_cloud_user->username, sizeof(pst_cloud_user->username));
		tool_str_strncpy(pst_cloud_user->username, pch_username, sizeof(pst_cloud_user->username)-1);
		tool_mem_memset(pst_cloud_user->mobile, sizeof(pst_cloud_user->mobile));
		tool_str_strncpy(pst_cloud_user->mobile, pch_number, sizeof(pst_cloud_user->mobile)-1);
		tool_mem_memset(pst_cloud_user->email, sizeof(pst_cloud_user->email));
		tool_str_strncpy(pst_cloud_user->email, pch_email, sizeof(pst_cloud_user->email)-1);
		tool_mem_memset(pst_cloud_user->user_ipv4, sizeof(pst_cloud_user->user_ipv4));
		base64_decode(pch_user_ipv4, (TOOL_UINT8*)pst_cloud_user->user_ipv4);

		TOOL_INT8* dev_id = NULL;
		TOOL_INT8* dev_name = NULL;		
		TOOL_INT8* in_ipv4 = NULL;		
		TOOL_INT8* ex_ipv4 = NULL;		
		TOOL_INT8* port_name = NULL;
		TOOL_INT8* port_value = NULL;
		TOOL_INT32 i = 0;
		for (i = 0; i < HDCCTV_QPNS_ALARM_NUM; i++)
		{
			dev_id  = tool_json_getValue_V2(&st_json, "dev_list", i, "dev_id");
			dev_name  = tool_json_getValue_V2(&st_json, "dev_list", i, "dev_name");
			in_ipv4 = tool_json_getValue_V2(&st_json, "dev_list", i, "in_ipv4");
			ex_ipv4 = tool_json_getValue_V2(&st_json, "dev_list", i, "ex_ipv4");
			port_name = tool_json_getValue_V2(&st_json, "dev_list", i, "port_name");
			port_value = tool_json_getValue_V2(&st_json, "dev_list", i, "port_value");
			
			if (dev_id == NULL || dev_name == NULL || in_ipv4 == NULL || ex_ipv4 == NULL || port_name == NULL || port_value == NULL)
			{
//				log_error("dev_id(0x%08x) dev_name(0x%08x) in_ipv4(0x%08x) ex_ipv4(0x%08x) port_name(0x%08x) port_value(0x%08x)", 
//					dev_id, dev_name, in_ipv4, ex_ipv4, port_name, port_value);
				break;
			}
			
			tool_mem_memset(&pst_device_list->device[pst_device_list->num], sizeof(hdcctv_qpns_alarm_t));
			tool_str_strncpy(pst_device_list->device[pst_device_list->num].dev_id, dev_id, HDCCTV_QPNS_TOKEN_SIZE-1);
			base64_decode(dev_name, (TOOL_UINT8*)pst_device_list->device[pst_device_list->num].dev_name);
			base64_decode(in_ipv4, (TOOL_UINT8*)pst_device_list->device[pst_device_list->num].in_ipv4);
			base64_decode(ex_ipv4, (TOOL_UINT8*)pst_device_list->device[pst_device_list->num].ex_ipv4);
			if (tool_str_strncmp(port_name, "http;sdk;stream;rtsp;onvif", tool_str_strlen("http;sdk;stream;rtsp;onvif")) != 0)
			{
//				log_error("port_name(%s)", port_name);
//				break;
			}
			if (sscanf(port_value, "%d;%d;%d;%d;%d", 
				&pst_device_list->device[pst_device_list->num].http_port,
				&pst_device_list->device[pst_device_list->num].sdk_port,
				&pst_device_list->device[pst_device_list->num].stream_port,
				&pst_device_list->device[pst_device_list->num].rtsp_port,
				&pst_device_list->device[pst_device_list->num].onvif_port) != 5)
			{
//				log_error("port_value(%s)", port_value);
//				break;
			}
			
			pst_device_list->num ++;
		}
	} while (0);
	tool_sock_item_close(&sock);
	return ret;			
}

TOOL_INT32 hdcctv_client2_getDevList(hdcctv_cloud_user_t* pst_cloud_user, hdcctv_device_list_t* pst_device_list)
{
	if (g_hdcctv_client2 == NULL)
	{
		log_error("g_hdcctv_client2");
		return NC_ERROR_UNINIT;
	}
	if (pst_cloud_user == NULL || pst_device_list == NULL)
	{
		log_error("pst_cloud_user(%x) pst_device_list(%x)", pst_cloud_user, pst_device_list);
		return NC_ERROR_INPUT;
	}
	if (g_hdcctv_client2->login_state == 0)
	{
		log_error("g_hdcctv_client2->loc_enable");
		return NC_ERROR_NO_LOGIN;
	}

	TOOL_INT32 tick = 0;
	TOOL_INT32 ret = 0;
	tool_sock_setTick(&tick);
	while (1)
	{
/*
		if (tool_sock_isTickTimeout(tick, 5))
		{
			log_error("hdcctv_client2_getDevList timeout");
			return NC_ERROR_CONNECT;
		}
*/		
		tool_thread_lockMutex(&g_hdcctv_client2->mutex);

		if (g_hdcctv_client2->list_result)
		{
			ret = g_hdcctv_client2->list_result;
			tool_thread_unlockMutex(&g_hdcctv_client2->mutex);
			return ret;
		}

		if (g_hdcctv_client2->list_state == 0)
		{
			tool_thread_unlockMutex(&g_hdcctv_client2->mutex);
			tool_sysf_usleep(100*1000);
			continue;
		}
		tool_mem_memcpy(pst_cloud_user, &g_hdcctv_client2->st_cloud_user, sizeof(hdcctv_cloud_user_t));
		tool_mem_memcpy(pst_device_list, &g_hdcctv_client2->st_device_list, sizeof(hdcctv_device_list_t));
		tool_thread_unlockMutex(&g_hdcctv_client2->mutex);
		break;
	}
	return 0;
}

TOOL_INT32 hdcctv_client2_getCloudCfg(hdcctv_cloud_user_t* pst_cloud_user, TOOL_INT8* dev_id, hdcctv_cloud_cfg_t* pst_cloud_cfg)
{
	if (g_hdcctv_client2 == NULL)
	{
		log_error("g_hdcctv_client2");
		return NC_ERROR_UNINIT;
	}
	if (pst_cloud_user == NULL || dev_id == NULL || pst_cloud_cfg == NULL)
	{
		log_error("pst_cloud_user(%x) dev_id(%x) pst_cloud_cfg(%x)", pst_cloud_user, dev_id, pst_cloud_cfg);
		return NC_ERROR_INPUT;
	}
	if (g_hdcctv_client2->login_state == 0)
	{
		log_error("g_hdcctv_client2->loc_enable");
		return NC_ERROR_NO_LOGIN;
	}

	TOOL_INT8 server_ip[PTC_IP_SIZE] = {0};
	TOOL_INT32 server_port = 0;
	TOOL_INT32 ret = hdcctv_client2_location(server_ip, &server_port);
	if (ret < 0)
	{
		log_error("hdcctv_client2_location");
		return ret;
	}	

	TOOL_UINT8 password_byte[16] = {0};
	tool_md5_calc((TOOL_UINT8*)pst_cloud_user->password, tool_str_strlen(pst_cloud_user->password), password_byte);
	TOOL_INT8 password_str[64] = {0};
	tool_str_byte2str(password_byte, 16, password_str);
//	log_debug("password_str(%s)", password_str);

	tool_http_t send_http;
	tool_http_t recv_http;
	tool_http_Init(&send_http);
	tool_str_strncpy(send_http.command, "POST", tool_str_strlen("POST"));
	tool_str_strncpy(send_http.url, "/u/getcfg.php", tool_str_strlen("/u/getcfg.php"));
	tool_str_strncpy(send_http.version, "HTTP/1.0", tool_str_strlen("HTTP/1.0"));
	send_http.ContentLength = snprintf(send_http.body, sizeof(send_http.body), 
		"{\"username\":\"%s\",\"number\":\"%s\",\"email\":\"%s\",\"password\":\"%s\",\"dev_id\":\"%s\"}", 
		pst_cloud_user->username, pst_cloud_user->mobile, pst_cloud_user->email, password_str,dev_id);

//	log_debug("send_http.body(%s)", send_http.body);

	tool_sock_item_t sock;
	tool_mem_memset(&sock, sizeof(tool_sock_item_t));
	if (tool_sock_item_open_v2(&sock, TOOL_SOCK_TYPE1_TCP, TOOL_SOCK_TYPE2_CLIENT, server_ip, server_port, NULL, 0) < 0)
	{
		log_error("tool_sock_item_open");
		return NC_ERROR_CONNECT;
	}
	ret = 0;
	do
	{
		tool_http_Init(&recv_http);	
		if (tool_http_Send(&send_http, &sock, NULL, NULL) < 0)
		{
			log_error("tool_http_Send");
			ret = NC_ERROR_SEND;
			break;
		}
		if (tool_http_Recv(&recv_http, &sock, NULL, NULL) < 0)
		{
			log_error("tool_http_Send");
			ret = NC_ERROR_RECV;
			break;
		}
		if (tool_str_strncmp(recv_http.command, "200", tool_str_strlen("200") != 0))
		{
			log_error("recv_http.command(%s)", recv_http.command);
			ret = NC_ERROR_PROTOCOL;
			break;
		}

		TOOL_INT32 result = 0;
		if (tool_json2_getNumber(recv_http.body, "result", &result) < 0)
		{
			result *= -1;
			if (result < NC_ERROR_CLOUD_BEGIN && result > NC_ERROR_CLOUD_END)
				ret = result;
			else
				ret = NC_ERROR_PROTOCOL;
			log_error("recv_http.body(%s)", recv_http.body);
			log_error("result(%s)", result);
			break;
		}
		if (tool_json2_getInt64(recv_http.body, "used_flow", &pst_cloud_cfg->used_flow) < 0 ||
			tool_json2_getInt64(recv_http.body, "total_flow", &pst_cloud_cfg->total_flow) < 0 ||
			tool_json2_getInt64(recv_http.body, "used_space", &pst_cloud_cfg->used_space) < 0 ||
			tool_json2_getInt64(recv_http.body, "total_space", &pst_cloud_cfg->total_space) < 0 ||
			tool_json2_getNumber(recv_http.body, "record_enable", &pst_cloud_cfg->record_enable) < 0)
		{
			ret = NC_ERROR_PROTOCOL;
			log_error("recv_http.body(%s)", recv_http.body);
			break;
		}
	} while (0);
	tool_sock_item_close(&sock);
	return ret;				
}

TOOL_INT32 hdcctv_client2_setCloudCfg(hdcctv_cloud_user_t* pst_cloud_user, TOOL_INT8* dev_id, hdcctv_cloud_cfg_t* pst_cloud_cfg)
{
	if (g_hdcctv_client2 == NULL)
	{
		log_error("g_hdcctv_client2");
		return NC_ERROR_UNINIT;
	}
	if (pst_cloud_user == NULL || dev_id == NULL || pst_cloud_cfg == NULL)
	{
		log_error("pst_cloud_user(%x) dev_id(%x) pst_cloud_cfg(%x)", pst_cloud_user, dev_id, pst_cloud_cfg);
		return NC_ERROR_INPUT;
	}
	if (g_hdcctv_client2->login_state == 0)
	{
		log_error("g_hdcctv_client2->loc_enable");
		return NC_ERROR_NO_LOGIN;
	}

	TOOL_INT8 server_ip[PTC_IP_SIZE] = {0};
	TOOL_INT32 server_port = 0;
	TOOL_INT32 ret = hdcctv_client2_location(server_ip, &server_port);
	if (ret < 0)
	{
		log_error("hdcctv_client2_location");
		return ret;
	}	

	TOOL_UINT8 password_byte[16] = {0};
	tool_md5_calc((TOOL_UINT8*)pst_cloud_user->password, tool_str_strlen(pst_cloud_user->password), password_byte);
	TOOL_INT8 password_str[64] = {0};
	tool_str_byte2str(password_byte, 16, password_str);
//	log_debug("password_str(%s)", password_str);

	tool_http_t send_http;
	tool_http_t recv_http;
	tool_http_Init(&send_http);
	tool_str_strncpy(send_http.command, "POST", tool_str_strlen("POST"));
	tool_str_strncpy(send_http.url, "/u/setcfg.php", tool_str_strlen("/u/setcfg.php"));
	tool_str_strncpy(send_http.version, "HTTP/1.0", tool_str_strlen("HTTP/1.0"));
	send_http.ContentLength = snprintf(send_http.body, sizeof(send_http.body), 
		"{\"username\":\"%s\",\"number\":\"%s\",\"email\":\"%s\",\"password\":\"%s\",\"dev_id\":\"%s\",\"record_enable\":\"%d\"}", 
		pst_cloud_user->username, pst_cloud_user->mobile, pst_cloud_user->email, password_str, dev_id, pst_cloud_cfg->record_enable);

//	log_debug("send_http.body(%s)", send_http.body);

	tool_sock_item_t sock;
	tool_mem_memset(&sock, sizeof(tool_sock_item_t));
	if (tool_sock_item_open_v2(&sock, TOOL_SOCK_TYPE1_TCP, TOOL_SOCK_TYPE2_CLIENT, server_ip, server_port, NULL, 0) < 0)
	{
		log_error("tool_sock_item_open");
		return NC_ERROR_CONNECT;
	}
	ret = 0;
	do
	{
		tool_http_Init(&recv_http);	
		if (tool_http_Send(&send_http, &sock, NULL, NULL) < 0)
		{
			log_error("tool_http_Send");
			ret = NC_ERROR_SEND;
			break;
		}
		if (tool_http_Recv(&recv_http, &sock, NULL, NULL) < 0)
		{
			log_error("tool_http_Send");
			ret = NC_ERROR_RECV;
			break;
		}
		if (tool_str_strncmp(recv_http.command, "200", tool_str_strlen("200") != 0))
		{
			log_error("recv_http.command(%s)", recv_http.command);
			ret = NC_ERROR_PROTOCOL;
			break;
		}

		TOOL_INT32 result = 0;
		if (tool_json2_getNumber(recv_http.body, "result", &result) < 0)
		{
			result *= -1;
			if (result < NC_ERROR_CLOUD_BEGIN && result > NC_ERROR_CLOUD_END)
				ret = result;
			else
				ret = NC_ERROR_PROTOCOL;
			log_error("recv_http.body(%s)", recv_http.body);
			log_error("result(%s)", result);
			break;
		}
	} while (0);
	tool_sock_item_close(&sock);
	return ret;				
}

TOOL_INT32 hdcctv_client2_getDevById(nc_dev* pst_nc_dev)
{
	if (g_hdcctv_client2 == NULL)
	{
		log_error("hdcctv");
		return NC_ERROR_UNINIT;
	}
	if (pst_nc_dev == NULL)
	{
		log_error("pst_nc_dev(0x%08x)", pst_nc_dev);
		return NC_ERROR_INPUT;
	}

	TOOL_INT32 i = 0;
	tool_thread_lockMutex(&g_hdcctv_client2->mutex);
	if (g_hdcctv_client2->list_state == 0)
	{
		tool_thread_unlockMutex(&g_hdcctv_client2->mutex);
		return 0;
	}
	for (i = 0; i < g_hdcctv_client2->st_device_list.num; i++)
	{
		if (tool_str_strncmp(pst_nc_dev->dev_id, g_hdcctv_client2->st_device_list.device[i].dev_id, 64) == 0 &&
			tool_str_strncmp(g_hdcctv_client2->st_cloud_user.user_ipv4, g_hdcctv_client2->st_device_list.device[i].ex_ipv4, PTC_IP_SIZE) == 0)
		{
			pst_nc_dev->ptc = NC_PTC_Qihan;
			tool_str_strncpy(pst_nc_dev->ip, g_hdcctv_client2->st_device_list.device[i].in_ipv4, PTC_IP_SIZE-1);
			pst_nc_dev->msg_port = g_hdcctv_client2->st_device_list.device[i].sdk_port;
			pst_nc_dev->stream_port = 0;
			tool_mem_memset(pst_nc_dev->user, PTC_ID_SIZE);
			tool_str_strncpy(pst_nc_dev->user, "qihan", 5);
			tool_mem_memset(pst_nc_dev->pswd, PTC_ID_SIZE);
			tool_str_strncpy(pst_nc_dev->pswd, "nahiq", 5);			
			log_debug("dev_id(%s) addr(%s:%d) user(%s.%s)", pst_nc_dev->dev_id, pst_nc_dev->ip, pst_nc_dev->msg_port, pst_nc_dev->user, pst_nc_dev->pswd);
			tool_thread_unlockMutex(&g_hdcctv_client2->mutex);
			return 1;
		}
	}
	tool_thread_unlockMutex(&g_hdcctv_client2->mutex);
	log_debug("dev_id(%s)", pst_nc_dev->dev_id);
	return 0;
}


TOOL_INT32 hdcctv_client2_startRts(nc_dev* pst_dev, TOOL_INT32 channel_no, TOOL_INT32 stream_type, NC_CB cb, TOOL_VOID* param)
{
	if (pst_dev == NULL)
	{
		log_error("pst_dev(%x)", pst_dev);
		return NC_ERROR_INPUT;
	}

	if (g_hdcctv_client2 == NULL)
	{
		log_error("g_hdcctv_client2");
		return NC_ERROR_UNINIT;
	}

	if (g_hdcctv_client2->login_state == 0)
	{
		log_error("g_hdcctv_client2->loc_enable");
		return NC_ERROR_NO_LOGIN;
	}

	hdcctv_client2_relay_t* pst_relay = NULL;
	TOOL_INT32 ret = hdcctv_client2_findFreeRequest(&pst_relay);
	if (ret < 0)
		return ret;

	return hdcctv_client2_relay_startRts(pst_relay, pst_dev, channel_no, stream_type, cb, param);
}

TOOL_INT32 hdcctv_client2_stopRts(TOOL_INT32 user_id)
{
	if (g_hdcctv_client2 == NULL)
	{
		log_error("g_hdcctv_client2");
		return NC_ERROR_UNINIT;
	}

	if (g_hdcctv_client2->login_state == 0)
	{
		log_error("g_hdcctv_client2->loc_enable");
		return NC_ERROR_NO_LOGIN;
	}
	
	hdcctv_client2_relay_t* pst_relay = NULL;
	TOOL_INT32 ret = hdcctv_client2_findRequestById(&pst_relay, user_id);
	if (ret < 0)
		return ret;

	hdcctv_client2_relay_stopRts(pst_relay);
	return 0;
}

TOOL_INT32 hdcctv_client2_startDevPbs(nc_dev* pst_dev, ptc_cfg_record_t* record, NC_CB cb, TOOL_VOID* param)
{
	if (pst_dev == NULL)
	{
		log_error("pst_dev(%x)", pst_dev);
		return NC_ERROR_INPUT;
	}

	if (g_hdcctv_client2 == NULL)
	{
		log_error("g_hdcctv_client2");
		return NC_ERROR_UNINIT;
	}

	if (g_hdcctv_client2->login_state == 0)
	{
		log_error("g_hdcctv_client2->loc_enable");
		return NC_ERROR_NO_LOGIN;
	}

	hdcctv_client2_relay_t* pst_relay = NULL;
	TOOL_INT32 ret = hdcctv_client2_findFreeRequest(&pst_relay);
	if (ret < 0)
		return ret;

	return hdcctv_client2_relay_startDevPbs(pst_relay, pst_dev, record, cb, param);

}

TOOL_INT32 hdcctv_client2_stopDevPbs(TOOL_INT32 pbs_id)
{
	if (g_hdcctv_client2 == NULL)
	{
		log_error("g_hdcctv_client2");
		return NC_ERROR_UNINIT;
	}

	if (g_hdcctv_client2->login_state == 0)
	{
		log_error("g_hdcctv_client2->loc_enable");
		return NC_ERROR_NO_LOGIN;
	}
	
	hdcctv_client2_relay_t* pst_relay = NULL;
	TOOL_INT32 ret = hdcctv_client2_findRequestById(&pst_relay, pbs_id);
	if (ret < 0)
		return ret;

	hdcctv_client2_relay_stopDevPbs(pst_relay);
	return 0;
}

TOOL_INT32 hdcctv_client2_startCloudPbs(nc_dev* pst_dev, ptc_cfg_record_t* record, NC_CB cb, TOOL_VOID* param)
{
	if (pst_dev == NULL)
	{
		log_error("pst_dev(%x)", pst_dev);
		return NC_ERROR_INPUT;
	}

	if (g_hdcctv_client2 == NULL)
	{
		log_error("g_hdcctv_client2");
		return NC_ERROR_UNINIT;
	}

	if (g_hdcctv_client2->login_state == 0)
	{
		log_error("g_hdcctv_client2->loc_enable");
		return NC_ERROR_NO_LOGIN;
	}

	hdcctv_client2_relay_t* pst_relay = NULL;
	TOOL_INT32 ret = hdcctv_client2_findFreeRequest(&pst_relay);
	if (ret < 0)
		return ret;

	return hdcctv_client2_relay_startCloudPbs(pst_relay, pst_dev, record, cb, param);

}

TOOL_INT32 hdcctv_client2_stopCloudPbs(TOOL_INT32 pbs_id)
{
	if (g_hdcctv_client2 == NULL)
	{
		log_error("g_hdcctv_client2");
		return NC_ERROR_UNINIT;
	}

	if (g_hdcctv_client2->login_state == 0)
	{
		log_error("g_hdcctv_client2->loc_enable");
		return NC_ERROR_NO_LOGIN;
	}
	
	hdcctv_client2_relay_t* pst_relay = NULL;
	TOOL_INT32 ret = hdcctv_client2_findRequestById(&pst_relay, pbs_id);
	if (ret < 0)
		return ret;

	hdcctv_client2_relay_stopCloudPbs(pst_relay);
	return 0;
}

TOOL_INT32 hdcctv_client2_getChannelNum_v2(nc_dev* pst_dev)
{
	ptc_cfg_ability_t ability;
	TOOL_INT32 ret = hdcctv_client2_getAbility_v2(pst_dev, &ability);
	if (ret < 0)
		return ret;
	return ability.video_input_num;
}


TOOL_INT32 hdcctv_client2_getAbility_v2(nc_dev* pst_dev, ptc_cfg_ability_t* ability)
{
	if (g_hdcctv_client2 == NULL)
	{
		log_error("g_hdcctv_client2");
		return NC_ERROR_UNINIT;
	}
	if (g_hdcctv_client2->login_state == 0)
	{
		log_error("g_hdcctv_client2->loc_enable");
		return NC_ERROR_NO_LOGIN;
	}
	if (pst_dev == NULL)
	{
		log_error("pst_dev(0x%08x)", pst_dev);
		return NC_ERROR_INPUT;
	}

	TOOL_INT32 i = 0;
	TOOL_INT32 ret = 0;
	tool_thread_lockMutex(&g_hdcctv_client2->mutex);
	for (i = 0; i < HDCCTV_CLIENT2_MSGS_SIZE; i++)
	{
		if (hdcctv_client2_msgs_isSame(&g_hdcctv_client2->msgs[i], pst_dev))
		{
			tool_thread_unlockMutex(&g_hdcctv_client2->mutex);
			return hdcctv_client2_msgs_getAbility_v2(&g_hdcctv_client2->msgs[i], ability);
		}
	}
	for (i = 0; i < HDCCTV_CLIENT2_MSGS_SIZE; i++)
	{
		if (g_hdcctv_client2->msgs[i].state == 0)
			break;
	}
	if (i == HDCCTV_CLIENT2_MSGS_SIZE)
	{
		log_error("nc full");
		tool_thread_unlockMutex(&g_hdcctv_client2->mutex);
		return NC_ERROR_FULL;
	}
	
	ret = hdcctv_client2_msgs_start(&g_hdcctv_client2->msgs[i], pst_dev);
	if (ret < 0)
	{
		log_error("hdcctv_client2_msgs_start id(%s)", pst_dev->dev_id);
		tool_thread_unlockMutex(&g_hdcctv_client2->mutex);
		return ret;
	}
	tool_thread_unlockMutex(&g_hdcctv_client2->mutex);

	return hdcctv_client2_msgs_getAbility_v2(&g_hdcctv_client2->msgs[i], ability);	
}

TOOL_INT32 hdcctv_client2_ctrlPtz(nc_dev* pst_dev, TOOL_INT32 channel, PTC_PTZ_CMD_E cmd, TOOL_INT32 param)
{
	if (g_hdcctv_client2 == NULL)
	{
		log_error("g_hdcctv_client2");
		return NC_ERROR_UNINIT;
	}
	if (g_hdcctv_client2->login_state == 0)
	{
		log_error("g_hdcctv_client2->loc_enable");
		return NC_ERROR_NO_LOGIN;
	}
	if (pst_dev == NULL)
	{
		log_error("pst_dev(0x%08x)", pst_dev);
		return NC_ERROR_INPUT;
	}

	TOOL_INT32 i = 0;
	TOOL_INT32 ret = 0;
	tool_thread_lockMutex(&g_hdcctv_client2->mutex);
	for (i = 0; i < HDCCTV_CLIENT2_MSGS_SIZE; i++)
	{
		if (hdcctv_client2_msgs_isSame(&g_hdcctv_client2->msgs[i], pst_dev))
		{
			tool_thread_unlockMutex(&g_hdcctv_client2->mutex);
			return hdcctv_client2_msgs_ctrlPtz(&g_hdcctv_client2->msgs[i], channel, cmd, param);
		}
	}
	for (i = 0; i < HDCCTV_CLIENT2_MSGS_SIZE; i++)
	{
		if (g_hdcctv_client2->msgs[i].state == 0)
			break;
	}
	if (i == HDCCTV_CLIENT2_MSGS_SIZE)
	{
		log_error("nc full");
		tool_thread_unlockMutex(&g_hdcctv_client2->mutex);
		return NC_ERROR_FULL;
	}
	
	ret = hdcctv_client2_msgs_start(&g_hdcctv_client2->msgs[i], pst_dev);
	if (ret < 0)
	{
		log_error("hdcctv_client2_msgs_start id(%s)", pst_dev->dev_id);
		tool_thread_unlockMutex(&g_hdcctv_client2->mutex);
		return ret;
	}
	tool_thread_unlockMutex(&g_hdcctv_client2->mutex);

	return hdcctv_client2_msgs_ctrlPtz(&g_hdcctv_client2->msgs[i], channel, cmd, param);	
}

TOOL_INT32 hdcctv_client2_syncTime(nc_dev* pst_dev, TOOL_UINT32 utc_sec, TOOL_INT32 zone, TOOL_INT32 dst_hour)
{
	if (g_hdcctv_client2 == NULL)
	{
		log_error("g_hdcctv_client2");
		return NC_ERROR_UNINIT;
	}
	if (g_hdcctv_client2->login_state == 0)
	{
		log_error("g_hdcctv_client2->loc_enable");
		return NC_ERROR_NO_LOGIN;
	}
	if (pst_dev == NULL)
	{
		log_error("pst_dev(0x%08x)", pst_dev);
		return NC_ERROR_INPUT;
	}

	TOOL_INT32 i = 0;
	TOOL_INT32 ret = 0;
	tool_thread_lockMutex(&g_hdcctv_client2->mutex);
	for (i = 0; i < HDCCTV_CLIENT2_MSGS_SIZE; i++)
	{
		if (hdcctv_client2_msgs_isSame(&g_hdcctv_client2->msgs[i], pst_dev))
		{
			tool_thread_unlockMutex(&g_hdcctv_client2->mutex);
			return hdcctv_client2_msgs_syncTime(&g_hdcctv_client2->msgs[i], utc_sec, zone, dst_hour);
		}
	}
	for (i = 0; i < HDCCTV_CLIENT2_MSGS_SIZE; i++)
	{
		if (g_hdcctv_client2->msgs[i].state == 0)
			break;
	}
	if (i == HDCCTV_CLIENT2_MSGS_SIZE)
	{
		log_error("nc full");
		tool_thread_unlockMutex(&g_hdcctv_client2->mutex);
		return NC_ERROR_FULL;
	}
	
	ret = hdcctv_client2_msgs_start(&g_hdcctv_client2->msgs[i], pst_dev);
	if (ret < 0)
	{
		log_error("hdcctv_client2_msgs_start id(%s)", pst_dev->dev_id);
		tool_thread_unlockMutex(&g_hdcctv_client2->mutex);
		return ret;
	}
	tool_thread_unlockMutex(&g_hdcctv_client2->mutex);

	return hdcctv_client2_msgs_syncTime(&g_hdcctv_client2->msgs[i], utc_sec, zone, dst_hour);
}

TOOL_INT32 hdcctv_client2_getDeviceStatus_v2(nc_dev* pst_dev, ptc_cfg_status_t* status)
{
	if (g_hdcctv_client2 == NULL)
	{
		log_error("g_hdcctv_client2");
		return NC_ERROR_UNINIT;
	}
	if (g_hdcctv_client2->login_state == 0)
	{
		log_error("g_hdcctv_client2->loc_enable");
		return NC_ERROR_NO_LOGIN;
	}
	if (pst_dev == NULL)
	{
		log_error("pst_dev(0x%08x)", pst_dev);
		return NC_ERROR_INPUT;
	}

	TOOL_INT32 i = 0;
	TOOL_INT32 ret = 0;
	tool_thread_lockMutex(&g_hdcctv_client2->mutex);
	for (i = 0; i < HDCCTV_CLIENT2_MSGS_SIZE; i++)
	{
		if (hdcctv_client2_msgs_isSame(&g_hdcctv_client2->msgs[i], pst_dev))
		{
			tool_thread_unlockMutex(&g_hdcctv_client2->mutex);
			return hdcctv_client2_msgs_getStatus(&g_hdcctv_client2->msgs[i], status);
		}
	}
	for (i = 0; i < HDCCTV_CLIENT2_MSGS_SIZE; i++)
	{
		if (g_hdcctv_client2->msgs[i].state == 0)
			break;
	}
	if (i == HDCCTV_CLIENT2_MSGS_SIZE)
	{
		log_error("nc full");
		tool_thread_unlockMutex(&g_hdcctv_client2->mutex);
		return NC_ERROR_FULL;
	}
	
	ret = hdcctv_client2_msgs_start(&g_hdcctv_client2->msgs[i], pst_dev);
	if (ret < 0)
	{
		log_error("hdcctv_client2_msgs_start id(%s)", pst_dev->dev_id);
		tool_thread_unlockMutex(&g_hdcctv_client2->mutex);
		return ret;
	}
	tool_thread_unlockMutex(&g_hdcctv_client2->mutex);

	return hdcctv_client2_msgs_getStatus(&g_hdcctv_client2->msgs[i], status);	
}


TOOL_INT32 hdcctv_client2_setDeviceStatus_v2(nc_dev* pst_dev, ptc_cfg_status_t* status)
{
	if (g_hdcctv_client2 == NULL)
	{
		log_error("g_hdcctv_client2");
		return NC_ERROR_UNINIT;
	}
	if (g_hdcctv_client2->login_state == 0)
	{
		log_error("g_hdcctv_client2->loc_enable");
		return NC_ERROR_NO_LOGIN;
	}
	if (pst_dev == NULL)
	{
		log_error("pst_dev(0x%08x)", pst_dev);
		return NC_ERROR_INPUT;
	}

	TOOL_INT32 i = 0;
	TOOL_INT32 ret = 0;
	tool_thread_lockMutex(&g_hdcctv_client2->mutex);
	for (i = 0; i < HDCCTV_CLIENT2_MSGS_SIZE; i++)
	{
		if (hdcctv_client2_msgs_isSame(&g_hdcctv_client2->msgs[i], pst_dev))
		{
			tool_thread_unlockMutex(&g_hdcctv_client2->mutex);
			return hdcctv_client2_msgs_setStatus(&g_hdcctv_client2->msgs[i], status);
		}
	}
	for (i = 0; i < HDCCTV_CLIENT2_MSGS_SIZE; i++)
	{
		if (g_hdcctv_client2->msgs[i].state == 0)
			break;
	}
	if (i == HDCCTV_CLIENT2_MSGS_SIZE)
	{
		log_error("nc full");
		tool_thread_unlockMutex(&g_hdcctv_client2->mutex);
		return NC_ERROR_FULL;
	}
	
	ret = hdcctv_client2_msgs_start(&g_hdcctv_client2->msgs[i], pst_dev);
	if (ret < 0)
	{
		log_error("hdcctv_client2_msgs_start id(%s)", pst_dev->dev_id);
		tool_thread_unlockMutex(&g_hdcctv_client2->mutex);
		return ret;
	}
	tool_thread_unlockMutex(&g_hdcctv_client2->mutex);

	return hdcctv_client2_msgs_setStatus(&g_hdcctv_client2->msgs[i], status);	
}

TOOL_INT32 hdcctv_client2_getAlarmCfg(nc_dev* pst_dev, PTC_ALARM_TYPE_E alarm_type, ptc_cfg_alarm_t* alarm)
{
	if (g_hdcctv_client2 == NULL)
	{
		log_error("g_hdcctv_client2");
		return NC_ERROR_UNINIT;
	}
	if (g_hdcctv_client2->login_state == 0)
	{
		log_error("g_hdcctv_client2->loc_enable");
		return NC_ERROR_NO_LOGIN;
	}
	if (pst_dev == NULL)
	{
		log_error("pst_dev(0x%08x)", pst_dev);
		return NC_ERROR_INPUT;
	}

	TOOL_INT32 i = 0;
	TOOL_INT32 ret = 0;
	tool_thread_lockMutex(&g_hdcctv_client2->mutex);
	for (i = 0; i < HDCCTV_CLIENT2_MSGS_SIZE; i++)
	{
		if (hdcctv_client2_msgs_isSame(&g_hdcctv_client2->msgs[i], pst_dev))
		{
			tool_thread_unlockMutex(&g_hdcctv_client2->mutex);
			return hdcctv_client2_msgs_getAlarm(&g_hdcctv_client2->msgs[i], alarm_type, alarm);
		}
	}
	for (i = 0; i < HDCCTV_CLIENT2_MSGS_SIZE; i++)
	{
		if (g_hdcctv_client2->msgs[i].state == 0)
			break;
	}
	if (i == HDCCTV_CLIENT2_MSGS_SIZE)
	{
		log_error("nc full");
		tool_thread_unlockMutex(&g_hdcctv_client2->mutex);
		return NC_ERROR_FULL;
	}
	
	ret = hdcctv_client2_msgs_start(&g_hdcctv_client2->msgs[i], pst_dev);
	if (ret < 0)
	{
		log_error("hdcctv_client2_msgs_start id(%s)", pst_dev->dev_id);
		tool_thread_unlockMutex(&g_hdcctv_client2->mutex);
		return ret;
	}
	tool_thread_unlockMutex(&g_hdcctv_client2->mutex);

	return hdcctv_client2_msgs_getAlarm(&g_hdcctv_client2->msgs[i], alarm_type, alarm);	
}


TOOL_INT32 hdcctv_client2_setAlarmCfg(nc_dev* pst_dev, PTC_ALARM_TYPE_E alarm_type, ptc_cfg_alarm_t* alarm)
{
	if (g_hdcctv_client2 == NULL)
	{
		log_error("g_hdcctv_client2");
		return NC_ERROR_UNINIT;
	}
	if (g_hdcctv_client2->login_state == 0)
	{
		log_error("g_hdcctv_client2->loc_enable");
		return NC_ERROR_NO_LOGIN;
	}
	if (pst_dev == NULL)
	{
		log_error("pst_dev(0x%08x)", pst_dev);
		return NC_ERROR_INPUT;
	}

	TOOL_INT32 i = 0;
	TOOL_INT32 ret = 0;
	tool_thread_lockMutex(&g_hdcctv_client2->mutex);
	for (i = 0; i < HDCCTV_CLIENT2_MSGS_SIZE; i++)
	{
		if (hdcctv_client2_msgs_isSame(&g_hdcctv_client2->msgs[i], pst_dev))
		{
			tool_thread_unlockMutex(&g_hdcctv_client2->mutex);
			return hdcctv_client2_msgs_setAlarm(&g_hdcctv_client2->msgs[i], alarm_type, alarm);	
		}
	}
	for (i = 0; i < HDCCTV_CLIENT2_MSGS_SIZE; i++)
	{
		if (g_hdcctv_client2->msgs[i].state == 0)
			break;
	}
	if (i == HDCCTV_CLIENT2_MSGS_SIZE)
	{
		log_error("nc full");
		tool_thread_unlockMutex(&g_hdcctv_client2->mutex);
		return NC_ERROR_FULL;
	}
	
	ret = hdcctv_client2_msgs_start(&g_hdcctv_client2->msgs[i], pst_dev);
	if (ret < 0)
	{
		log_error("hdcctv_client2_msgs_start id(%s)", pst_dev->dev_id);
		tool_thread_unlockMutex(&g_hdcctv_client2->mutex);
		return ret;
	}
	tool_thread_unlockMutex(&g_hdcctv_client2->mutex);

	return hdcctv_client2_msgs_setAlarm(&g_hdcctv_client2->msgs[i], alarm_type, alarm);		
}

TOOL_INT32 hdcctv_client2_getWiFiCfg(nc_dev* pst_dev, ptc_cfg_wifi_t* pst_wifi)
{
	if (g_hdcctv_client2 == NULL)
	{
		log_error("g_hdcctv_client2");
		return NC_ERROR_UNINIT;
	}
	if (g_hdcctv_client2->login_state == 0)
	{
		log_error("g_hdcctv_client2->loc_enable");
		return NC_ERROR_NO_LOGIN;
	}
	if (pst_dev == NULL)
	{
		log_error("pst_dev(0x%08x)", pst_dev);
		return NC_ERROR_INPUT;
	}

	TOOL_INT32 i = 0;
	TOOL_INT32 ret = 0;
	tool_thread_lockMutex(&g_hdcctv_client2->mutex);
	for (i = 0; i < HDCCTV_CLIENT2_MSGS_SIZE; i++)
	{
		if (hdcctv_client2_msgs_isSame(&g_hdcctv_client2->msgs[i], pst_dev))
		{
			tool_thread_unlockMutex(&g_hdcctv_client2->mutex);
			return hdcctv_client2_msgs_getWifi(&g_hdcctv_client2->msgs[i], pst_wifi);
		}
	}
	for (i = 0; i < HDCCTV_CLIENT2_MSGS_SIZE; i++)
	{
		if (g_hdcctv_client2->msgs[i].state == 0)
			break;
	}
	if (i == HDCCTV_CLIENT2_MSGS_SIZE)
	{
		log_error("nc full");
		tool_thread_unlockMutex(&g_hdcctv_client2->mutex);
		return NC_ERROR_FULL;
	}
	
	ret = hdcctv_client2_msgs_start(&g_hdcctv_client2->msgs[i], pst_dev);
	if (ret < 0)
	{
		log_error("hdcctv_client2_msgs_start id(%s)", pst_dev->dev_id);
		tool_thread_unlockMutex(&g_hdcctv_client2->mutex);
		return ret;
	}
	tool_thread_unlockMutex(&g_hdcctv_client2->mutex);

	return hdcctv_client2_msgs_getWifi(&g_hdcctv_client2->msgs[i], pst_wifi);	

}

TOOL_INT32 hdcctv_client2_setWiFiCfg(nc_dev* pst_dev, ptc_cfg_wifi_t* pst_wifi)
{
	if (g_hdcctv_client2 == NULL)
	{
		log_error("g_hdcctv_client2");
		return NC_ERROR_UNINIT;
	}
	if (g_hdcctv_client2->login_state == 0)
	{
		log_error("g_hdcctv_client2->loc_enable");
		return NC_ERROR_NO_LOGIN;
	}
	if (pst_dev == NULL)
	{
		log_error("pst_dev(0x%08x)", pst_dev);
		return NC_ERROR_INPUT;
	}

	TOOL_INT32 i = 0;
	TOOL_INT32 ret = 0;
	tool_thread_lockMutex(&g_hdcctv_client2->mutex);
	for (i = 0; i < HDCCTV_CLIENT2_MSGS_SIZE; i++)
	{
		if (hdcctv_client2_msgs_isSame(&g_hdcctv_client2->msgs[i], pst_dev))
		{
			tool_thread_unlockMutex(&g_hdcctv_client2->mutex);
			return hdcctv_client2_msgs_setWifi(&g_hdcctv_client2->msgs[i], pst_wifi); 
		}
	}
	for (i = 0; i < HDCCTV_CLIENT2_MSGS_SIZE; i++)
	{
		if (g_hdcctv_client2->msgs[i].state == 0)
			break;
	}
	if (i == HDCCTV_CLIENT2_MSGS_SIZE)
	{
		log_error("nc full");
		tool_thread_unlockMutex(&g_hdcctv_client2->mutex);
		return NC_ERROR_FULL;
	}
	
	ret = hdcctv_client2_msgs_start(&g_hdcctv_client2->msgs[i], pst_dev);
	if (ret < 0)
	{
		log_error("hdcctv_client2_msgs_start id(%s)", pst_dev->dev_id);
		tool_thread_unlockMutex(&g_hdcctv_client2->mutex);
		return ret;
	}
	tool_thread_unlockMutex(&g_hdcctv_client2->mutex);

	return hdcctv_client2_msgs_setWifi(&g_hdcctv_client2->msgs[i], pst_wifi); 	
}

TOOL_INT32 hdcctv_client2_getDevRecordCfg(nc_dev* pst_dev, ptc_cfg_devRecord_t* pst_devRecord)
{
	if (g_hdcctv_client2 == NULL)
	{
		log_error("g_hdcctv_client2");
		return NC_ERROR_UNINIT;
	}
	if (g_hdcctv_client2->login_state == 0)
	{
		log_error("g_hdcctv_client2->loc_enable");
		return NC_ERROR_NO_LOGIN;
	}
	if (pst_dev == NULL)
	{
		log_error("pst_dev(0x%08x)", pst_dev);
		return NC_ERROR_INPUT;
	}

	TOOL_INT32 i = 0;
	TOOL_INT32 ret = 0;
	tool_thread_lockMutex(&g_hdcctv_client2->mutex);
	for (i = 0; i < HDCCTV_CLIENT2_MSGS_SIZE; i++)
	{
		if (hdcctv_client2_msgs_isSame(&g_hdcctv_client2->msgs[i], pst_dev))
		{
			tool_thread_unlockMutex(&g_hdcctv_client2->mutex);
			return hdcctv_client2_msgs_getDevRecord(&g_hdcctv_client2->msgs[i], pst_devRecord);
		}
	}
	for (i = 0; i < HDCCTV_CLIENT2_MSGS_SIZE; i++)
	{
		if (g_hdcctv_client2->msgs[i].state == 0)
			break;
	}
	if (i == HDCCTV_CLIENT2_MSGS_SIZE)
	{
		log_error("nc full");
		tool_thread_unlockMutex(&g_hdcctv_client2->mutex);
		return NC_ERROR_FULL;
	}
	
	ret = hdcctv_client2_msgs_start(&g_hdcctv_client2->msgs[i], pst_dev);
	if (ret < 0)
	{
		log_error("hdcctv_client2_msgs_start id(%s)", pst_dev->dev_id);
		tool_thread_unlockMutex(&g_hdcctv_client2->mutex);
		return ret;
	}
	tool_thread_unlockMutex(&g_hdcctv_client2->mutex);

	return hdcctv_client2_msgs_getDevRecord(&g_hdcctv_client2->msgs[i], pst_devRecord);	

}

TOOL_INT32 hdcctv_client2_setDevRecordCfg(nc_dev* pst_dev, ptc_cfg_devRecord_t* pst_devRecord)
{
	if (g_hdcctv_client2 == NULL)
	{
		log_error("g_hdcctv_client2");
		return NC_ERROR_UNINIT;
	}
	if (g_hdcctv_client2->login_state == 0)
	{
		log_error("g_hdcctv_client2->loc_enable");
		return NC_ERROR_NO_LOGIN;
	}
	if (pst_dev == NULL)
	{
		log_error("pst_dev(0x%08x)", pst_dev);
		return NC_ERROR_INPUT;
	}

	TOOL_INT32 i = 0;
	TOOL_INT32 ret = 0;
	tool_thread_lockMutex(&g_hdcctv_client2->mutex);
	for (i = 0; i < HDCCTV_CLIENT2_MSGS_SIZE; i++)
	{
		if (hdcctv_client2_msgs_isSame(&g_hdcctv_client2->msgs[i], pst_dev))
		{
			tool_thread_unlockMutex(&g_hdcctv_client2->mutex);
			return hdcctv_client2_msgs_setDevRecord(&g_hdcctv_client2->msgs[i], pst_devRecord); 
		}
	}
	for (i = 0; i < HDCCTV_CLIENT2_MSGS_SIZE; i++)
	{
		if (g_hdcctv_client2->msgs[i].state == 0)
			break;
	}
	if (i == HDCCTV_CLIENT2_MSGS_SIZE)
	{
		log_error("nc full");
		tool_thread_unlockMutex(&g_hdcctv_client2->mutex);
		return NC_ERROR_FULL;
	}
	
	ret = hdcctv_client2_msgs_start(&g_hdcctv_client2->msgs[i], pst_dev);
	if (ret < 0)
	{
		log_error("hdcctv_client2_msgs_start id(%s)", pst_dev->dev_id);
		tool_thread_unlockMutex(&g_hdcctv_client2->mutex);
		return ret;
	}
	tool_thread_unlockMutex(&g_hdcctv_client2->mutex);

	return hdcctv_client2_msgs_setDevRecord(&g_hdcctv_client2->msgs[i], pst_devRecord); 	
}

TOOL_INT32 hdcctv_client2_getCfg(nc_dev* pst_dev, TOOL_INT32 id, TOOL_VOID* cfg)
{
	if (g_hdcctv_client2 == NULL)
	{
		log_error("g_hdcctv_client2");
		return NC_ERROR_UNINIT;
	}
	if (g_hdcctv_client2->login_state == 0)
	{
		log_error("g_hdcctv_client2->loc_enable");
		return NC_ERROR_NO_LOGIN;
	}
	if (pst_dev == NULL || id < 0 || id >= HDCCTV_CLIENT_CFG_NUM || cfg == NULL)
	{
		log_error("pst_dev(0x%08x) id(%d) cfg(0x%08x)", pst_dev, id, cfg);
		return NC_ERROR_INPUT;
	}

	TOOL_INT32 i = 0;
	TOOL_INT32 ret = 0;
	tool_thread_lockMutex(&g_hdcctv_client2->mutex);
	for (i = 0; i < HDCCTV_CLIENT2_MSGS_SIZE; i++)
	{
		if (hdcctv_client2_msgs_isSame(&g_hdcctv_client2->msgs[i], pst_dev))
		{
			tool_thread_unlockMutex(&g_hdcctv_client2->mutex);
			return hdcctv_client2_msgs_getCfg(&g_hdcctv_client2->msgs[i], id, cfg);
		}
	}
	for (i = 0; i < HDCCTV_CLIENT2_MSGS_SIZE; i++)
	{
		if (g_hdcctv_client2->msgs[i].state == 0)
			break;
	}
	if (i == HDCCTV_CLIENT2_MSGS_SIZE)
	{
		log_error("nc full");
		tool_thread_unlockMutex(&g_hdcctv_client2->mutex);
		return NC_ERROR_FULL;
	}
	
	ret = hdcctv_client2_msgs_start(&g_hdcctv_client2->msgs[i], pst_dev);
	if (ret < 0)
	{
		log_error("hdcctv_client2_msgs_start id(%s)", pst_dev->dev_id);
		tool_thread_unlockMutex(&g_hdcctv_client2->mutex);
		return ret;
	}
	tool_thread_unlockMutex(&g_hdcctv_client2->mutex);

	return hdcctv_client2_msgs_getCfg(&g_hdcctv_client2->msgs[i], id, cfg);	

}

TOOL_INT32 hdcctv_client2_setCfg(nc_dev* pst_dev, TOOL_INT32 id, TOOL_VOID* cfg)
{
	if (g_hdcctv_client2 == NULL)
	{
		log_error("g_hdcctv_client2");
		return NC_ERROR_UNINIT;
	}
	if (g_hdcctv_client2->login_state == 0)
	{
		log_error("g_hdcctv_client2->loc_enable");
		return NC_ERROR_NO_LOGIN;
	}
	if (pst_dev == NULL || id < 0 || id >= HDCCTV_CLIENT_CFG_NUM || cfg == NULL)
	{
		log_error("pst_dev(0x%08x) id(%d) cfg(0x%08x)", pst_dev, id, cfg);
		return NC_ERROR_INPUT;
	}

	TOOL_INT32 i = 0;
	TOOL_INT32 ret = 0;
	tool_thread_lockMutex(&g_hdcctv_client2->mutex);
	for (i = 0; i < HDCCTV_CLIENT2_MSGS_SIZE; i++)
	{
		if (hdcctv_client2_msgs_isSame(&g_hdcctv_client2->msgs[i], pst_dev))
		{
			tool_thread_unlockMutex(&g_hdcctv_client2->mutex);
			return hdcctv_client2_msgs_setCfg(&g_hdcctv_client2->msgs[i], id, cfg); 
		}
	}
	for (i = 0; i < HDCCTV_CLIENT2_MSGS_SIZE; i++)
	{
		if (g_hdcctv_client2->msgs[i].state == 0)
			break;
	}
	if (i == HDCCTV_CLIENT2_MSGS_SIZE)
	{
		log_error("nc full");
		tool_thread_unlockMutex(&g_hdcctv_client2->mutex);
		return NC_ERROR_FULL;
	}
	
	ret = hdcctv_client2_msgs_start(&g_hdcctv_client2->msgs[i], pst_dev);
	if (ret < 0)
	{
		log_error("hdcctv_client2_msgs_start id(%s)", pst_dev->dev_id);
		tool_thread_unlockMutex(&g_hdcctv_client2->mutex);
		return ret;
	}
	tool_thread_unlockMutex(&g_hdcctv_client2->mutex);

	return hdcctv_client2_msgs_setCfg(&g_hdcctv_client2->msgs[i], id, cfg); 	
}

TOOL_INT32 hdcctv_client2_searchDevDates(nc_dev* pst_dev, ptc_cfg_dates_t* dates)
{
	if (g_hdcctv_client2 == NULL)
	{
		log_error("g_hdcctv_client2");
		return NC_ERROR_UNINIT;
	}
	if (g_hdcctv_client2->login_state == 0)
	{
		log_error("g_hdcctv_client2->loc_enable");
		return NC_ERROR_NO_LOGIN;
	}
	if (pst_dev == NULL)
	{
		log_error("pst_dev(0x%08x)", pst_dev);
		return NC_ERROR_INPUT;
	}

	TOOL_INT32 i = 0;
	TOOL_INT32 ret = 0;
	tool_thread_lockMutex(&g_hdcctv_client2->mutex);
	for (i = 0; i < HDCCTV_CLIENT2_MSGS_SIZE; i++)
	{
		if (hdcctv_client2_msgs_isSame(&g_hdcctv_client2->msgs[i], pst_dev))
		{
			tool_thread_unlockMutex(&g_hdcctv_client2->mutex);
			return hdcctv_client2_msgs_searchDates(&g_hdcctv_client2->msgs[i], dates);	
		}
	}
	for (i = 0; i < HDCCTV_CLIENT2_MSGS_SIZE; i++)
	{
		if (g_hdcctv_client2->msgs[i].state == 0)
			break;
	}
	if (i == HDCCTV_CLIENT2_MSGS_SIZE)
	{
		log_error("nc full");
		tool_thread_unlockMutex(&g_hdcctv_client2->mutex);
		return NC_ERROR_FULL;
	}
	
	ret = hdcctv_client2_msgs_start(&g_hdcctv_client2->msgs[i], pst_dev);
	if (ret < 0)
	{
		log_error("hdcctv_client2_msgs_start id(%s)", pst_dev->dev_id);
		tool_thread_unlockMutex(&g_hdcctv_client2->mutex);
		return ret;
	}
	tool_thread_unlockMutex(&g_hdcctv_client2->mutex);

	return hdcctv_client2_msgs_searchDates(&g_hdcctv_client2->msgs[i], dates);	
}


TOOL_INT32 hdcctv_client2_searchDevRecords(nc_dev* pst_dev, ptc_cfg_records_t* records)
{
	if (g_hdcctv_client2 == NULL)
	{
		log_error("g_hdcctv_client2");
		return NC_ERROR_UNINIT;
	}
	if (g_hdcctv_client2->login_state == 0)
	{
		log_error("g_hdcctv_client2->loc_enable");
		return NC_ERROR_NO_LOGIN;
	}
	if (pst_dev == NULL)
	{
		log_error("pst_dev(0x%08x)", pst_dev);
		return NC_ERROR_INPUT;
	}

	TOOL_INT32 i = 0;
	TOOL_INT32 ret = 0;
	tool_thread_lockMutex(&g_hdcctv_client2->mutex);
	for (i = 0; i < HDCCTV_CLIENT2_MSGS_SIZE; i++)
	{
		if (hdcctv_client2_msgs_isSame(&g_hdcctv_client2->msgs[i], pst_dev))
		{
			tool_thread_unlockMutex(&g_hdcctv_client2->mutex);
			return hdcctv_client2_msgs_searchRecords(&g_hdcctv_client2->msgs[i], records);	
		}
	}
	for (i = 0; i < HDCCTV_CLIENT2_MSGS_SIZE; i++)
	{
		if (g_hdcctv_client2->msgs[i].state == 0)
			break;
	}
	if (i == HDCCTV_CLIENT2_MSGS_SIZE)
	{
		log_error("nc full");
		tool_thread_unlockMutex(&g_hdcctv_client2->mutex);
		return NC_ERROR_FULL;
	}
	
	ret = hdcctv_client2_msgs_start(&g_hdcctv_client2->msgs[i], pst_dev);
	if (ret < 0)
	{
		log_error("hdcctv_client2_msgs_start id(%s)", pst_dev->dev_id);
		tool_thread_unlockMutex(&g_hdcctv_client2->mutex);
		return ret;
	}
	tool_thread_unlockMutex(&g_hdcctv_client2->mutex);

	return hdcctv_client2_msgs_searchRecords(&g_hdcctv_client2->msgs[i], records);	
}

TOOL_INT32 hdcctv_client2_searchCloudDates(nc_dev* pst_dev, ptc_cfg_dates_t* dates)
{
	if (g_hdcctv_client2 == NULL)
	{
		log_error("g_hdcctv_client2");
		return NC_ERROR_UNINIT;
	}
	if (g_hdcctv_client2->login_state == 0)
	{
		log_error("g_hdcctv_client2->loc_enable");
		return NC_ERROR_NO_LOGIN;
	}
	if (pst_dev == NULL)
	{
		log_error("pst_dev(0x%08x)", pst_dev);
		return NC_ERROR_INPUT;
	}

/*
	hdcctv_client2_relay_t* pst_relay = NULL;
	TOOL_INT32 ret = hdcctv_client2_findFreeRequest(&pst_relay);
	if (ret < 0)
		return ret;
*/

	hdcctv_client2_relay_t relay;
	tool_mem_memset(&relay, sizeof(hdcctv_client2_relay_t));
	return hdcctv_client2_relay_searchCloudDates(&relay, g_hdcctv_client2, pst_dev, dates);	
}

TOOL_INT32 hdcctv_client2_searchCloudRecords(nc_dev* pst_dev, ptc_cfg_records_t* records)
{
	if (g_hdcctv_client2 == NULL)
	{
		log_error("g_hdcctv_client2");
		return NC_ERROR_UNINIT;
	}
	if (g_hdcctv_client2->login_state == 0)
	{
		log_error("g_hdcctv_client2->loc_enable");
		return NC_ERROR_NO_LOGIN;
	}
	if (pst_dev == NULL)
	{
		log_error("pst_dev(0x%08x)", pst_dev);
		return NC_ERROR_INPUT;
	}

/*
	hdcctv_client2_relay_t* pst_relay = NULL;
	TOOL_INT32 ret = hdcctv_client2_findFreeRequest(&pst_relay);
	if (ret < 0)
		return ret;
*/

	hdcctv_client2_relay_t relay;
	tool_mem_memset(&relay, sizeof(hdcctv_client2_relay_t));
	return hdcctv_client2_relay_searchCloudRecords(&relay, g_hdcctv_client2, pst_dev, records);	
}

TOOL_INT32 hdcctv_client2_searchLogs(nc_dev* pst_dev, ptc_logs_v2_t* logs_v2)
{
	if (g_hdcctv_client2 == NULL)
	{
		log_error("g_hdcctv_client2");
		return NC_ERROR_UNINIT;
	}
	if (g_hdcctv_client2->login_state == 0)
	{
		log_error("g_hdcctv_client2->loc_enable");
		return NC_ERROR_NO_LOGIN;
	}
	if (pst_dev == NULL)
	{
		log_error("pst_dev(0x%08x)", pst_dev);
		return NC_ERROR_INPUT;
	}

	TOOL_INT32 i = 0;
	TOOL_INT32 ret = 0;
	tool_thread_lockMutex(&g_hdcctv_client2->mutex);
	for (i = 0; i < HDCCTV_CLIENT2_MSGS_SIZE; i++)
	{
		if (hdcctv_client2_msgs_isSame(&g_hdcctv_client2->msgs[i], pst_dev))
		{
			tool_thread_unlockMutex(&g_hdcctv_client2->mutex);
			return hdcctv_client2_msgs_searchLogs(&g_hdcctv_client2->msgs[i], logs_v2);	
		}
	}
	for (i = 0; i < HDCCTV_CLIENT2_MSGS_SIZE; i++)
	{
		if (g_hdcctv_client2->msgs[i].state == 0)
			break;
	}
	if (i == HDCCTV_CLIENT2_MSGS_SIZE)
	{
		log_error("nc full");
		tool_thread_unlockMutex(&g_hdcctv_client2->mutex);
		return NC_ERROR_FULL;
	}
	
	ret = hdcctv_client2_msgs_start(&g_hdcctv_client2->msgs[i], pst_dev);
	if (ret < 0)
	{
		log_error("hdcctv_client2_msgs_start id(%s)", pst_dev->dev_id);
		tool_thread_unlockMutex(&g_hdcctv_client2->mutex);
		return ret;
	}
	tool_thread_unlockMutex(&g_hdcctv_client2->mutex);

	return hdcctv_client2_msgs_searchLogs(&g_hdcctv_client2->msgs[i], logs_v2);	
}

TOOL_INT32 hdcctv_client2_startTalk(nc_dev* pst_dev, NC_CB cb, TOOL_VOID* param)
{
	if (pst_dev == NULL)
	{
		log_error("pst_dev(%x)", pst_dev);
		return NC_ERROR_INPUT;
	}

	if (g_hdcctv_client2 == NULL)
	{
		log_error("g_hdcctv_client2");
		return NC_ERROR_UNINIT;
	}

	if (g_hdcctv_client2->login_state == 0)
	{
		log_error("g_hdcctv_client2->loc_enable");
		return NC_ERROR_NO_LOGIN;
	}

	hdcctv_client2_relay_t* pst_relay = NULL;
	TOOL_INT32 ret = hdcctv_client2_findFreeRequest(&pst_relay);
	if (ret < 0)
		return ret;

	return hdcctv_client2_relay_startTalk(pst_relay, pst_dev, cb, param);

}

TOOL_INT32 hdcctv_client2_stopTalk(TOOL_INT32 talk_id)
{
	if (g_hdcctv_client2 == NULL)
	{
		log_error("g_hdcctv_client2");
		return NC_ERROR_UNINIT;
	}

	if (g_hdcctv_client2->login_state == 0)
	{
		log_error("g_hdcctv_client2->loc_enable");
		return NC_ERROR_NO_LOGIN;
	}
	
	hdcctv_client2_relay_t* pst_relay = NULL;
	TOOL_INT32 ret = hdcctv_client2_findRequestById(&pst_relay, talk_id);
	if (ret < 0)
		return ret;

	hdcctv_client2_relay_stopTalk(pst_relay);
	return 0;
}

TOOL_INT32 hdcctv_client2_sendTalk(TOOL_INT32 talk_id, TOOL_UINT8* buf, TOOL_INT32 len)
{
	if (g_hdcctv_client2 == NULL)
	{
		log_error("g_hdcctv_client2");
		return NC_ERROR_UNINIT;
	}

	if (g_hdcctv_client2->login_state == 0)
	{
		log_error("g_hdcctv_client2->loc_enable");
		return NC_ERROR_NO_LOGIN;
	}
	
	hdcctv_client2_relay_t* pst_relay = NULL;
	TOOL_INT32 ret = hdcctv_client2_findRequestById(&pst_relay, talk_id);
	if (ret < 0)
		return ret;
	if (buf == NULL)
	{
		log_error("buf(0x%x)", buf);
		return NC_ERROR_INPUT;
	}
	
	if (pst_relay->talk_state)
		tool_stream2_set(pst_relay->send_stream, buf, len);
	return 0;
}


#if defined __cplusplus
}
#endif


