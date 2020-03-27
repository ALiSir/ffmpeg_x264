//
//  tool_type.h
//  SocketThread
//
//  Created by lanbh on 14-3-6.
//  Copyright (c) 2014年 com.vision. All rights reserved.
//

#if defined __cplusplus
extern "C"
{
#endif

#ifndef __TOOL_TYPE_H__
#define __TOOL_TYPE_H__


#include <pthread.h>


#define TOOL_IN
#define TOOL_OUT
#define TOOL_CONST			const
#define TOOL_STATIC			static

typedef void                TOOL_VOID;
typedef char                TOOL_INT8;
typedef unsigned char       TOOL_UINT8;
typedef short               TOOL_INT16;
typedef unsigned short      TOOL_UINT16;
typedef int                 TOOL_INT32;
typedef unsigned int        TOOL_UINT32;
typedef long				TOOL_LONG;
typedef unsigned long		TOOL_UNLONG;
typedef double              TOOL_FLOAT64;
typedef long long           TOOL_INT64;
typedef unsigned long long  TOOL_UINT64;
typedef pthread_mutex_t		TOOL_MUTEX;
typedef pthread_cond_t		TOOL_COND;
typedef pthread_t			TOOL_THREAD;
typedef void* (*TOOL_FUNC)(void* param);

typedef struct
{
	TOOL_INT32 sec;
	TOOL_INT32 usec;
}tool_time_t;

typedef struct
{
    TOOL_INT32 year;
    TOOL_INT32 month;
    TOOL_INT32 day;
    TOOL_INT32 hour;
    TOOL_INT32 minute;
    TOOL_INT32 second;
}tool_date_t;

#ifdef WIN32
#define snprintf _snprintf
#endif

#endif

#if defined __cplusplus
}
#endif

