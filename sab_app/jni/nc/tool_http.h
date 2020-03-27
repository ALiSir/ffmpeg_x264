
/************************************************************
 * Copyright	: 
 * FileName		: http.h
 * Description	: [HTTP/1.0] [HTTP/1.1] [RFC2616]
 * Created		: lanbh (loooooobh@gmail.com) 2011-12-23
**************************************************************/



#ifndef	__TOOL_HTTP_H__
#define	__TOOL_HTTP_H__


#define TOOL_HTTP_STR_SIZE			64
#define TOOL_HTTP_PARAM_SIZE		128
#define	TOOL_HTTP_URL_SIZE			1024
#define TOOL_HTTP_LINE_SIZE			2048
#define TOOL_HTTP_BODY_SIZE			(10*1024)
#define TOOL_HTTP_MSG_SIZE			(12*1024)

#include "tool_type.h"
#include "tool_sock.h"


typedef struct 
{
	TOOL_INT8	command[TOOL_HTTP_PARAM_SIZE];
	TOOL_INT8	url[TOOL_HTTP_URL_SIZE];
	TOOL_INT8	version[TOOL_HTTP_PARAM_SIZE];

	TOOL_INT32 	Seq;
	TOOL_INT8	Session[TOOL_HTTP_PARAM_SIZE];
	TOOL_INT8	x_sessioncookie[TOOL_HTTP_PARAM_SIZE];
	TOOL_INT8	ContentType[TOOL_HTTP_PARAM_SIZE];
	TOOL_INT8	Authorization[TOOL_HTTP_PARAM_SIZE];
	TOOL_INT8	Host[TOOL_HTTP_PARAM_SIZE];
	
	TOOL_INT32 	ContentLength;
	TOOL_INT8	body[TOOL_HTTP_BODY_SIZE];
}tool_http_t;



TOOL_VOID tool_http_Init(tool_http_t *http);
TOOL_INT32 tool_http_Recv(tool_http_t *http, tool_sock_item_t* sock, TOOL_INT8* buf, TOOL_INT32* size);
TOOL_INT32 tool_http_Send(tool_http_t *http, tool_sock_item_t* sock, TOOL_INT8* buf, TOOL_INT32* size);


#endif

