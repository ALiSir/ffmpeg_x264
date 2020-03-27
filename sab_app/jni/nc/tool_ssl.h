
#if defined __cplusplus
extern "C"
{
#endif

#ifndef __TOOL_SSL_H__
#define __TOOL_SSL_H__

#ifdef __IS_SSL__

#include "bio.h"
#include "ssl.h"
#include "err.h"
#include "tool_type.h"

typedef struct
{
	SSL_CTX* ctx;
	BIO* bio;
	SSL* ssl;
}tool_ssl_t;

TOOL_VOID tool_ssl_init();
TOOL_INT32 tool_ssl_connect(tool_ssl_t* tssl, TOOL_INT8* client_cer, TOOL_INT8* client_key, TOOL_INT8* server_cer, TOOL_INT8* addr);
TOOL_INT32 tool_ssl_pend(tool_ssl_t* tssl);
TOOL_INT32 tool_ssl_read(tool_ssl_t* tssl, TOOL_VOID* buf, TOOL_INT32 len);
TOOL_INT32 tool_ssl_write(tool_ssl_t* tssl, TOOL_VOID* buf, TOOL_INT32 len);
TOOL_VOID tool_ssl_close(tool_ssl_t* tssl);
TOOL_VOID tool_ssl_done();



#endif

#endif

#if defined __cplusplus
}
#endif

