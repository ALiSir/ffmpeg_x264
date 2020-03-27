//
//  tool_log.h
//  SocketThread
//
//  Created by lanbh on 14-3-6.
//  Copyright (c) 2014年 com.vision. All rights reserved.
//

#if defined __cplusplus
extern "C"
{
#endif

#ifndef __TOOL_LOG_H__
#define __TOOL_LOG_H__

#include "tool_type.h"

/*
 *  === LOG LEVEL LIST ===
 *	LOG_FATAL		: critical condition.	<EXIT>		(highest priority)
 *	LOG_ERROR		: error condition.
 *	LOG_STATE		: information for real time state.
 *	LOG_DEBUG		: information for debug.			(lowest priority)
 */
#define	TOOL_LOG_FATAL			0x01
#define	TOOL_LOG_ERROR			0x02
#define	TOOL_LOG_STATE			0x04
#define	TOOL_LOG_DEBUG			0x08
#define	TOOL_LOG_ALL			0x0f
#define TOOL_LOG_DEFUALT		0x0f

TOOL_VOID tool_log_init();
TOOL_VOID tool_log_init2(TOOL_INT8* dev_id);

TOOL_VOID tool_log_done();

TOOL_VOID tool_log_write(TOOL_INT32 level, const TOOL_INT8*function, TOOL_UINT32 line, TOOL_INT8* pFile, TOOL_INT8 *fmt, ...);

/*
#include "alog.h"
#define	log_fatal		LOGA
#define	log_error		LOGE
#define	log_state		LOGI
#define	log_debug		LOGD
#define	log_fatalNo		LOGA
#define	log_errorNo	 	LOGE
*/

#define	log_fatal(...)		tool_log_write(TOOL_LOG_FATAL, __FUNCTION__, __LINE__, __FILE__, __VA_ARGS__)
#define	log_error(...)		tool_log_write(TOOL_LOG_ERROR, __FUNCTION__, __LINE__, __FILE__, __VA_ARGS__)
#define	log_state(...)		tool_log_write(TOOL_LOG_STATE, __FUNCTION__, __LINE__, __FILE__, __VA_ARGS__)
#define	log_debug(...)		tool_log_write(TOOL_LOG_DEBUG, __FUNCTION__, __LINE__, __FILE__, __VA_ARGS__)
#define	log_fatalNo(...)	tool_log_write(-TOOL_LOG_FATAL, __FUNCTION__, __LINE__, __FILE__, __VA_ARGS__)
#define	log_errorNo(...) 	tool_log_write(-TOOL_LOG_ERROR, __FUNCTION__, __LINE__, __FILE__, __VA_ARGS__)

#endif


#if defined __cplusplus
}
#endif
