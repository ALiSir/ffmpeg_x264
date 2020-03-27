
#if defined __cplusplus
extern "C"
{
#endif

#ifndef __TOOL_MD5_H__
#define __TOOL_MD5_H__

#include "tool_type.h"

typedef struct 
{
  TOOL_UINT32   u32State[4];      /* state (ABCD) */
  TOOL_UINT32   u32Cnt[2];        /* number of bits, modulo 2^64 (lsb first) */
  TOOL_UINT8	szBuffer[64];     /* input buffer */
}tool_md5_t;



TOOL_VOID tool_md5_calc(TOOL_IN TOOL_CONST TOOL_UINT8 *pucInput, TOOL_IN TOOL_UINT32 ulInlen, TOOL_OUT TOOL_UINT8 *pucOutput);
TOOL_INT32 tool_md5_check(TOOL_IN TOOL_UINT8 *authenticator, TOOL_IN TOOL_UINT8 *input, TOOL_IN TOOL_UINT32 inlen);


TOOL_VOID tool_md5_calc_16bit(TOOL_IN TOOL_CONST TOOL_UINT8 *pucInput, TOOL_IN TOOL_UINT32 ulInlen, TOOL_OUT TOOL_UINT8 *pucOutput);
TOOL_INT32 tool_md5_check_16bit(TOOL_IN TOOL_UINT8 *authenticator, TOOL_IN TOOL_UINT8 *input, TOOL_IN TOOL_UINT32 inlen);

TOOL_VOID tool_md5_calc_xm(TOOL_IN TOOL_CONST TOOL_UINT8 *pucInput, TOOL_IN TOOL_UINT32 ulInlen, TOOL_OUT TOOL_INT8 *pucOutput);
TOOL_INT32 tool_md5_check_xm(TOOL_IN TOOL_INT8 *authenticator, TOOL_IN TOOL_UINT8 *input, TOOL_IN TOOL_UINT32 inlen);



#endif

#if defined __cplusplus
}
#endif

