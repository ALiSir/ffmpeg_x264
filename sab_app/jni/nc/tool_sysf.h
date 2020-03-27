
//
//  tool_sysf.h
//  SocketThread
//
//  Created by lanbh on 14-3-6.
//  Copyright (c) 2014年 com.vision. All rights reserved.
//

#if defined __cplusplus
extern "C"
{
#endif

#ifndef __TOOL_SYSF_H__
#define __TOOL_SYSF_H__

#include "tool_type.h"

#define TOOL_SOCK_SIZE		65536

TOOL_VOID* tool_mem_malloc(TOOL_UINT32 size, TOOL_UINT32 isInit);
TOOL_VOID tool_mem_free(TOOL_VOID* ptr);
TOOL_VOID tool_mem_free_raw(TOOL_VOID* ptr);
TOOL_VOID tool_mem_getState(TOOL_INT32* mem_count, TOOL_UINT32* mem_size, TOOL_INT32* thread_count, TOOL_INT32* mutex_count, TOOL_INT32* cond_count);
TOOL_VOID tool_mem_memset(TOOL_VOID* mem, TOOL_UINT32 len);
TOOL_VOID tool_mem_memcpy(TOOL_VOID* dst, TOOL_CONST TOOL_VOID* src, TOOL_UINT32 len);
TOOL_VOID tool_mem_memmove(TOOL_VOID* dst, TOOL_VOID* src, TOOL_UINT32 len);
TOOL_INT32 tool_mem_memcmp(TOOL_VOID* dst, TOOL_VOID* src, TOOL_UINT32 len);


TOOL_UINT32 tool_str_strlen(TOOL_CONST TOOL_INT8* str);
TOOL_INT8* tool_str_strncpy(TOOL_INT8* dst, TOOL_CONST TOOL_INT8* src, TOOL_UINT32 len);
TOOL_INT8* tool_str_strcat(TOOL_INT8* dst, TOOL_CONST TOOL_INT8* src);
TOOL_INT8* tool_str_strncat(TOOL_INT8* dst, TOOL_CONST TOOL_INT8* src, TOOL_UINT32 len);
TOOL_INT32 tool_str_strncmp(TOOL_CONST TOOL_INT8* dst, TOOL_CONST TOOL_INT8* src, TOOL_UINT32 len);
TOOL_INT32 tool_str_strncasecmp(TOOL_CONST TOOL_INT8* dst, TOOL_CONST TOOL_INT8* src, TOOL_UINT32 len);
TOOL_INT8* tool_str_strerror();
TOOL_INT32 tool_str_strtol(TOOL_INT8* str, TOOL_INT32 base);
TOOL_INT32 tool_str_strtoul(TOOL_INT8* str, TOOL_INT32 base);
TOOL_INT8* tool_str_strtok_r(TOOL_INT8* str, TOOL_INT8* delim, TOOL_INT8**saveptr);
TOOL_INT8* tool_str_strstr(TOOL_CONST TOOL_INT8* haystack, TOOL_CONST TOOL_INT8* needle);
TOOL_VOID tool_str_byte2str(TOOL_UINT8* byte, TOOL_INT32 byte_len, TOOL_INT8* str);
TOOL_VOID tool_str_str2byte(TOOL_INT8* str, TOOL_INT32 str_len, TOOL_UINT8* byte);
TOOL_INT32 tool_str_isIpv4(TOOL_INT8* str);
TOOL_INT32 tool_str_xorInt32(TOOL_INT32 data);
TOOL_INT16 tool_str_xorInt16(TOOL_INT16 data);
TOOL_INT32 tool_str_check(TOOL_UINT8* buf, TOOL_UINT32 len);



TOOL_INT32 tool_ctype_isprint(TOOL_INT32 c);

TOOL_INT8* tool_std_getenv(TOOL_INT8* name);

TOOL_INT32 tool_sysf_atoi(TOOL_CONST TOOL_INT8* str);
TOOL_INT64 tool_sysf_atoll(TOOL_CONST TOOL_INT8* str);

TOOL_VOID tool_sysf_setTick(TOOL_INT32* tv_sec, TOOL_INT32* tv_usec);
TOOL_INT32 tool_sysf_isTickTimeout(TOOL_INT32 tv_sec, TOOL_INT32 tv_usec, TOOL_INT32 inteval_usec);		// inteval_usec<10*1000*1000
TOOL_VOID tool_sysf_waitTickTimeout(TOOL_INT32 tv_sec, TOOL_INT32 tv_usec, TOOL_INT32 inteval_usec);	// inteval_usec<10*1000*1000
TOOL_VOID tool_sysf_time(tool_time_t* tt);
TOOL_VOID tool_sysf_dateGMT(tool_date_t* date, TOOL_UINT32 tt);
TOOL_VOID tool_sysf_dateLocal(tool_date_t* date, TOOL_UINT32 tt);
TOOL_INT8* tool_sysf_tt2str(TOOL_UINT32 tt, TOOL_INT32 type, TOOL_INT8* str, TOOL_INT32 size);
TOOL_INT32 tool_sysf_str2tt(TOOL_INT8* str, TOOL_INT32 type, TOOL_UINT32* tt);
TOOL_VOID tool_time_Date2Time(TOOL_INT32 year, TOOL_INT32 month, TOOL_INT32 day, TOOL_INT32 hour, TOOL_INT32 min, TOOL_INT32 sec, TOOL_UINT32* second);


TOOL_VOID tool_sysf_sleep(TOOL_INT32 sec);
TOOL_VOID tool_sysf_usleep(TOOL_INT32 usec);
TOOL_VOID tool_sysf_exit();

TOOL_INT32 tool_ran_rand(TOOL_INT32 seed);
TOOL_VOID tool_ran_wash(TOOL_INT32 seed, TOOL_INT32* array, TOOL_INT32 len);
TOOL_VOID tool_ran_uuid(TOOL_INT8* uuid);

TOOL_INT32 tool_sysf_system(TOOL_INT8* cmd);

TOOL_INT32 tool_sysf_socket(TOOL_INT32 type);
TOOL_UINT16 tool_sysf_htons(TOOL_UINT16 num);
TOOL_UINT16 tool_sysf_ntohs(TOOL_UINT16 num);
TOOL_INT32 tool_sysf_htonl(TOOL_INT32 num);
TOOL_INT32 tool_sysf_ntohl(TOOL_INT32 num);
TOOL_INT32 tool_sysf_bind(TOOL_INT32 sock, TOOL_VOID* addr);
TOOL_INT32 tool_sysf_listen(TOOL_INT32 lsfd);
TOOL_INT32 tool_sysf_accept(TOOL_INT32 lsfd, TOOL_VOID* addr);
TOOL_VOID tool_sysf_setSockBlock(TOOL_INT32 sock, TOOL_INT32 type);
TOOL_VOID tool_sysf_inet_pton(TOOL_INT8* src, TOOL_VOID* dst);
TOOL_VOID tool_sysf_inet_ntop(TOOL_VOID* src, TOOL_INT8* dst, TOOL_INT32 size);
TOOL_INT32 tool_sysf_connect(TOOL_INT32 sock, TOOL_VOID *addr);
TOOL_INT32 tool_sysf_pollRead(TOOL_INT32 sock, TOOL_INT32 msec);
TOOL_INT32 tool_sysf_pollWrite(TOOL_INT32 sock, TOOL_INT32 msec);
TOOL_INT32 tool_sysf_getsockopt(TOOL_INT32 sock, TOOL_INT32 type);
TOOL_INT32 tool_sysf_setsockopt(TOOL_INT32 sock, TOOL_INT32 type, TOOL_VOID* param1, TOOL_INT32 param2);
TOOL_INT32 tool_sysf_recv(TOOL_INT32 sock, TOOL_VOID*data, TOOL_UINT32 len, TOOL_INT32 type);
TOOL_INT32 tool_sysf_openFile(TOOL_INT8* file);
TOOL_INT32 tool_sysf_closeFile(TOOL_INT32 fd);
TOOL_INT32 tool_sysf_closeSock(TOOL_INT32 sock);
TOOL_INT32 tool_sysf_write(TOOL_INT32 fd, TOOL_VOID* data, TOOL_UINT32 len);
TOOL_INT32 tool_sysf_read(TOOL_INT32 fd, TOOL_VOID* data, TOOL_UINT32 len);
TOOL_INT32 tool_sysf_readLine(TOOL_INT32 fd, TOOL_VOID* buf, TOOL_INT32 lineSize);
TOOL_INT32 tool_sysf_recvPeek(TOOL_INT32 fd, TOOL_VOID* data, TOOL_UINT32 len);
TOOL_INT32 tool_sysf_initAddr(TOOL_VOID* addr, TOOL_INT8* ip, TOOL_UINT16 port);
TOOL_INT32 tool_sysf_sendto(TOOL_INT32 sock, TOOL_VOID*data, TOOL_UINT32 len, TOOL_VOID* addr);
TOOL_INT32 tool_sysf_recvfrom(TOOL_INT32 sock, TOOL_VOID*data, TOOL_UINT32 len, TOOL_VOID* addr);
TOOL_INT32 tool_sysf_singal(TOOL_INT32 type);
TOOL_INT32 tool_sysf_getlocalip_bySock(TOOL_INT32 sock, TOOL_VOID* outip);
TOOL_INT32 tool_sysf_getlocalip(TOOL_VOID* outip);
TOOL_INT32 tool_sysf_getsockname(TOOL_INT32 sock, TOOL_VOID* ip, TOOL_UINT16* port);
TOOL_INT32 tool_sysf_getpeername(TOOL_INT32 sock, TOOL_VOID* ip, TOOL_UINT16* port);

TOOL_INT32 tool_io_mkdir(const TOOL_INT8* pathname);
TOOL_INT32 tool_io_rename(const TOOL_INT8* oldpath, const TOOL_INT8* newpath);
TOOL_VOID* tool_io_fopen(const TOOL_INT8* pathname, TOOL_CONST TOOL_INT8* mode);
TOOL_INT32 tool_io_fread(TOOL_VOID* fp, TOOL_VOID* data, TOOL_UINT32 len);
TOOL_INT32 tool_io_fclose(TOOL_VOID* fp);
TOOL_INT32 tool_io_fseek(TOOL_VOID* fp, TOOL_INT32 pos);
TOOL_INT32 tool_io_fflush(TOOL_VOID* fp);


TOOL_INT32 tool_thread_create(TOOL_THREAD* thread, TOOL_VOID*attr, TOOL_FUNC func, TOOL_VOID* param);
TOOL_INT32 tool_thread_join(TOOL_THREAD thread);
TOOL_VOID tool_thread_exit(TOOL_VOID* ret);
TOOL_INT32 tool_thread_initMutex(TOOL_MUTEX* mutex);
TOOL_INT32 tool_thread_doneMutex(TOOL_MUTEX* mutex);
TOOL_VOID tool_thread_lockMutex(TOOL_MUTEX* mutex);
TOOL_INT32 tool_thread_trylockMutex(TOOL_MUTEX* mutex);
TOOL_VOID tool_thread_unlockMutex(TOOL_MUTEX* mutex);
TOOL_INT32 tool_thread_initCond(TOOL_COND* cond);
TOOL_INT32 tool_thread_doneCond(TOOL_COND* cond);
TOOL_INT32 tool_thread_waitCond(TOOL_COND* cond, TOOL_MUTEX* mutex);
TOOL_INT32 tool_thread_timedwaitCond(TOOL_COND* cond, TOOL_MUTEX* mutex, TOOL_INT32 sec);
TOOL_VOID tool_thread_signalCond(TOOL_COND* cond);
TOOL_VOID tool_thread_broadcastCond(TOOL_COND* cond);


#ifdef WIN32

#define TOOL_ERR_EINTR					WSAEINTR
#define TOOL_ERR_EAGAIN					WSAEWOULDBLOCK
#define TOOL_ERR_EINPROGRESS			WSAEINPROGRESS

#else

#define TOOL_ERR_EINTR					EINTR
#define TOOL_ERR_EAGAIN					EAGAIN
#define TOOL_ERR_EINPROGRESS			EINPROGRESS

#endif


TOOL_INT32 tool_err_getErrno();


#endif


#if defined __cplusplus
}
#endif
