//
//  tool_log.c
//  SocketThread
//
//  Created by lanbh on 14-3-6.
//  Copyright (c) 2014年 com.vision. All rights reserved.
//

#if defined __cplusplus
extern "C"
{
#endif

#include <stdio.h>
#include <stdarg.h>
#include "tool_log.h"
#include "tool_type.h"
#include "tool_sysf.h"

typedef struct
{
	TOOL_VOID* fp;
	TOOL_INT32 pos;
}tool_log_t;

tool_log_t* g_tool_log = NULL;

TOOL_VOID tool_log_init()
{
	if (g_tool_log)
	{
		log_error("log already init");
		return;
	}

	g_tool_log = (tool_log_t*)tool_mem_malloc(sizeof(tool_log_t), 1);
	tool_io_mkdir("./log");
	TOOL_INT8 oldpath[128] = "./log/hdcctv_server.log";
	tool_date_t td;
	tool_sysf_dateLocal(&td, 0);
	TOOL_INT8 newpath[128] = {0};
	snprintf(newpath, sizeof(newpath), "./log/%04d%02d%02d_%02d%02d%02d.log", td.year, td.month, td.day, td.hour, td.minute, td.second);
	tool_io_rename(oldpath, newpath);
	g_tool_log->fp = tool_io_fopen(oldpath, "w+");
}

TOOL_VOID tool_log_init2(TOOL_INT8* dev_id)
{
	if (g_tool_log)
	{
		log_error("log already init");
		return;
	}

	g_tool_log = (tool_log_t*)tool_mem_malloc(sizeof(tool_log_t), 1);
	TOOL_INT8 oldpath[128] = {0};
	snprintf(oldpath, sizeof(oldpath), "./log/%s.log", dev_id);
	g_tool_log->fp = tool_io_fopen(oldpath, "w+");
}	

TOOL_VOID tool_log_done()
{
	if (g_tool_log == NULL)
	{
		log_error("log already done");
		return;
	}

	tool_io_fclose(g_tool_log->fp);
	g_tool_log->fp = NULL;
	tool_mem_free(g_tool_log);
	g_tool_log = NULL;
}

TOOL_VOID tool_log_write(TOOL_INT32 level, const TOOL_INT8*function, TOOL_UINT32 line, TOOL_INT8* pFile, TOOL_INT8 *fmt, ...)
{
	if ((level & TOOL_LOG_DEFUALT) == 0)
		return;
    
    static TOOL_INT32 flag = 0;
    static TOOL_MUTEX mutex;
    static TOOL_INT8 type[16][8] = {"", "FATAL", "ERROR", "","STATE", "", "", "", "DEBUG"};
    
    if (flag == 0)
    {
        tool_thread_initMutex(&mutex);
        flag = 1;
    }
    
    TOOL_INT8 file[32] = {0};
	TOOL_INT8* pTmp = pFile+tool_str_strlen(pFile)-1;
	while (1)
	{
		if (*(pTmp-1) == '/' || pTmp == pFile)
			break;
		pTmp--;
	}
    tool_str_strncpy(file, pTmp, sizeof(file)-1);
    
	va_list	ap;
	va_start(ap, fmt);
	TOOL_INT32 level_abs = level;
    if (level_abs < 0)
        level_abs *= -1;
    tool_time_t time;
	tool_date_t date;
	TOOL_INT32 len = 0;
    tool_sysf_time(&time);
	tool_sysf_dateLocal(&date, 0);
	tool_thread_lockMutex(&mutex);
	if (g_tool_log && g_tool_log->fp)
	{
		len = fprintf((FILE *)g_tool_log->fp, "%04d-%02d-%02d %02d:%02d:%02d.%06d %s [%s %d] ",
			   date.year, date.month, date.day, date.hour, date.minute, date.second, time.usec,
			   type[level_abs], file, line);
		g_tool_log->pos += len;
		
		len = vfprintf((FILE *)g_tool_log->fp, fmt, ap);
		g_tool_log->pos += len;

		if (level < 0)
		{
			len = fprintf((FILE *)g_tool_log->fp, "::%s", tool_str_strerror());
			g_tool_log->pos += len;
		}
		
		fprintf((FILE *)g_tool_log->fp, "\n");
		g_tool_log->pos ++;
		
		tool_io_fflush(g_tool_log->fp);
		if (g_tool_log->pos > 10*1024*1024)
		{
			tool_io_fseek(g_tool_log->fp, 0);
			g_tool_log->pos = 0;
		}
	}
	else
	{
		 printf("%04d-%02d-%02d %02d:%02d:%02d.%06d %s [%s %d] ",
			   date.year, date.month, date.day, date.hour, date.minute, date.second, time.usec,
			   type[level_abs], file, line);		
	    vfprintf(stdout, fmt, ap);
		if (level < 0)
			printf("::%s", tool_str_strerror());
		printf("\n");
	}
	tool_thread_unlockMutex(&mutex);
	va_end(ap);
    
	if (level & TOOL_LOG_FATAL)
		tool_sysf_exit();
}


#if defined __cplusplus
}
#endif

