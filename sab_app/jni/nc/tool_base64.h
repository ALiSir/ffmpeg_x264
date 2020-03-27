
#if defined __cplusplus
extern "C"
{
#endif

#ifndef __TOOL_BASE64_H__
#define __TOOL_BASE64_H__

#include "tool_type.h"



char * base64_encode( const unsigned char * bindata, char * base64, int binlength );

int base64_decode( const char * base64, unsigned char * bindata );


#endif

#if defined __cplusplus
}
#endif


