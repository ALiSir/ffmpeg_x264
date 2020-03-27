//
//  tool_sock.h
//  SocketThread
//
//  Created by lanbh on 14-3-6.
//  Copyright (c) 2014年 com.vision. All rights reserved.
//

#if defined __cplusplus
extern "C"
{
#endif

#ifndef __TOOL_SOCK_H__
#define __TOOL_SOCK_H__

#include "tool_type.h"

#define TOOL_SOCK_TIMEOUT   10

TOOL_VOID tool_sock_init();
TOOL_VOID tool_sock_done();


TOOL_VOID tool_sock_setTick(TOOL_INT32* tick);
TOOL_INT32 tool_sock_isTickTimeout(TOOL_INT32 tick, TOOL_INT32 sec);




#define TOOL_SOCK_YS_BEGIN		22000
#define TOOL_SOAK_YS_END		23000

TOOL_INT16 tool_sock_getYsPort();


typedef enum
{
	TOOL_SOCK_TYPE1_NONE = 0,
	TOOL_SOCK_TYPE1_TCP,
	TOOL_SOCK_TYPE1_UDP,
	TOOL_SOCK_TYPE1_MCAST,
	TOOL_SOCK_TYPE1_BCAST,
}TOOL_SOCK_TYPE1_E;

typedef enum
{
	TOOL_SOCK_TYPE2_NONE = 0,
	TOOL_SOCK_TYPE2_SERVER1,
	TOOL_SOCK_TYPE2_SERVER2,
	TOOL_SOCK_TYPE2_CLIENT,
}TOOL_SOCK_TYPE2_E;


typedef struct
{
	TOOL_UINT8 message_id[8];
	TOOL_INT32 state;
	TOOL_INT32 second;
}tool_sock_stun_t;

typedef struct
{
	TOOL_SOCK_TYPE1_E type1;	
	TOOL_SOCK_TYPE2_E type2;	
	TOOL_UINT8 timeout;
	TOOL_UINT8 is_error;
	TOOL_UINT8 reserve[2];
	TOOL_INT8 remote_addr[16];		// tcp:server_addr, udp:remote_addr
	TOOL_INT8 source_addr[16];		// tcp:client_addr, udp:source_addr
	TOOL_INT32* flag;
	TOOL_INT32 tick;

	TOOL_INT32 fd;
//	TOOL_INT32 readable;
	
	TOOL_INT32 width;
	TOOL_INT32 height;
	TOOL_INT32 sec;
	TOOL_INT32 usec;
	TOOL_INT32 fps_video;
	TOOL_INT32 count_video[10];
	TOOL_INT32 fps_auido;
	TOOL_INT32 count_audio[10];
	TOOL_INT32 seq;
	TOOL_INT32 frame_sub_type;
}tool_sock_item_t;



TOOL_VOID tool_sock_item_init(tool_sock_item_t* item);
TOOL_INT32 tool_sock_item_open_v2(tool_sock_item_t* item, TOOL_SOCK_TYPE1_E type1, TOOL_SOCK_TYPE2_E type2, TOOL_INT8* remote_ip, TOOL_UINT16 remote_port, TOOL_INT8* source_ip, TOOL_UINT16 source_port);
TOOL_INT32 tool_sock_item_accept(tool_sock_item_t* listen_item, tool_sock_item_t* recv_item);
TOOL_VOID tool_sock_item_setTimeout(tool_sock_item_t* item, TOOL_INT8 timeout);
TOOL_VOID tool_sock_item_setFlag(tool_sock_item_t* item, TOOL_INT32* flag);
TOOL_INT32 tool_sock_item_isLive(tool_sock_item_t* item);
TOOL_INT32 tool_sock_item_isTimeout(tool_sock_item_t* item, TOOL_INT32 inteval);
TOOL_VOID tool_sock_item_setTick(tool_sock_item_t* item);
TOOL_INT32 tool_sock_item_pollRead(tool_sock_item_t* item, TOOL_INT32 msec);
TOOL_INT32 tool_sock_item_send(tool_sock_item_t* item, TOOL_VOID* buf, TOOL_INT32 n);
TOOL_INT32 tool_sock_item_recv(tool_sock_item_t* item, TOOL_VOID* buf, TOOL_INT32 n);
TOOL_INT32 tool_sock_item_recvPeek(tool_sock_item_t* item, TOOL_VOID* buf, TOOL_INT32 n);
TOOL_INT32 tool_sock_item_recvLine(tool_sock_item_t* item, TOOL_VOID* buf, TOOL_INT32 lineSize);
TOOL_INT32 tool_sock_item_sendto(tool_sock_item_t* item, TOOL_VOID* buf, TOOL_INT32 n);
TOOL_INT32 tool_sock_item_recvfrom(tool_sock_item_t* item, TOOL_VOID* buf, TOOL_INT32 n);
TOOL_VOID tool_sock_item_close(tool_sock_item_t* item);
TOOL_INT32 tool_sock_item_getSourceAddr(tool_sock_item_t* item, TOOL_VOID* ip, TOOL_UINT16* port);
TOOL_INT32 tool_sock_item_getRemoteAddr(tool_sock_item_t* item, TOOL_VOID* ip, TOOL_UINT16* port);

TOOL_INT32 tool_sock_item_ping(TOOL_INT8* remote_ip, TOOL_UINT16 remote_port, TOOL_INT32 timeout);


#endif

#if defined __cplusplus
}
#endif

