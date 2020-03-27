//
//  tool_sysf.c
//  SocketThread
//
//  Created by lanbh on 14-3-6.
//  Copyright (c) 2014å¹´ com.vision. All rights reserved.
//

#if defined __cplusplus
extern "C"
{
#endif

#include <signal.h>
#include <stdio.h>
#include <time.h>
#ifdef WIN32
	#include <share.h>
	#include <winsock.h>
	#include <windows.h>
	#include <io.h>
	#include <direct.h>
	#include <stdlib.h>
//	#include <ws2tcpip.h>
	#pragma comment(lib,"ws2_32.lib")
	#pragma comment(lib, "pthreadVC2.lib")
#else
	#include <sys/time.h>
	#include <unistd.h>
	#include <sys/socket.h>
	#include <arpa/inet.h>
	#include <poll.h>
	#include <netinet/tcp.h>
	#include <netdb.h>
	#include <netinet/in.h>
	#include <net/if.h>
	#include <sys/ioctl.h>
#endif

#include <string.h>
#include <stdarg.h>
#include <stdlib.h>
#include <errno.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <semaphore.h>
#include <ctype.h>
#include <pthread.h>

//#include <linux/types.h>





#ifdef WIN32
	int gettimeofday(struct timeval *tp, void *tzp)
	{
		time_t clock;
		struct tm tm;
		SYSTEMTIME wtm;
		GetLocalTime(&wtm);
		tm.tm_year	= wtm.wYear - 1900;
		tm.tm_mon	= wtm.wMonth - 1;
		tm.tm_mday	= wtm.wDay;
		tm.tm_hour	= wtm.wHour;
		tm.tm_min	= wtm.wMinute;
		tm.tm_sec	= wtm.wSecond;
		tm.tm_isdst	= -1;
		clock = mktime(&tm);
		tp->tv_sec	= (long)clock;
		tp->tv_usec	= wtm.wMilliseconds * 1000;
		return (0);
	}
#endif


#include "tool_type.h"
#include "tool_sysf.h"
#include "tool_log.h"

static TOOL_INT32 g_tool_mem_count = 0;
static TOOL_UINT32 g_tool_mem_size = 0;
static TOOL_INT32 g_tool_thread_count = 0;
static TOOL_INT32 g_tool_mutex_count = 0;
static TOOL_INT32 g_tool_cond_count = 0;

TOOL_VOID* tool_mem_malloc(TOOL_UINT32 size, TOOL_UINT32 isInit)
{
	TOOL_UINT8 *true_ptr = (TOOL_UINT8*)malloc(size+4);
	if (true_ptr == NULL)
		log_fatalNo("malloc");
	if (isInit)
		tool_mem_memset(true_ptr, size+4);

	TOOL_UINT32* psize = (TOOL_UINT32*)true_ptr;
	*psize = size+4;
	g_tool_mem_count ++;
	g_tool_mem_size += *psize;
    //log_debug("%d, %d   %08x %d", g_tool_mem_count, g_tool_mem_size, true_ptr, *psize);
	return true_ptr+4;
}

TOOL_VOID tool_mem_free(TOOL_VOID* ptr)
{
	if (ptr == NULL)
		log_fatal("ptr");

	TOOL_UINT8* true_ptr = (TOOL_UINT8*)ptr;
	true_ptr = true_ptr-4;
	TOOL_UINT32* psize = (TOOL_UINT32*)true_ptr;
	g_tool_mem_count --;
	g_tool_mem_size -= *psize;
    //log_debug("%d, %d   %08x %d", g_tool_mem_count, g_tool_mem_size, true_ptr, *psize);
	free(true_ptr);
}

TOOL_VOID tool_mem_free_raw(TOOL_VOID* ptr)
{
	if (ptr)
		free(ptr);
}

TOOL_VOID tool_mem_getState(TOOL_INT32* mem_count, TOOL_UINT32* mem_size, TOOL_INT32* thread_count, TOOL_INT32* mutex_count, TOOL_INT32* cond_count)
{
    *mem_count = g_tool_mem_count;
    *mem_size = g_tool_mem_size;
	*thread_count = g_tool_thread_count;
	*mutex_count = g_tool_thread_count;
	*cond_count = g_tool_thread_count;
}


TOOL_VOID tool_mem_memset(TOOL_VOID* mem, TOOL_UINT32 len)
{
    memset(mem, 0, len);
}

TOOL_VOID tool_mem_memcpy(TOOL_VOID* dst, TOOL_CONST TOOL_VOID* src, TOOL_UINT32 len)
{
    memcpy(dst, src, len);
}

TOOL_VOID tool_mem_memmove(TOOL_VOID* dst, TOOL_VOID* src, TOOL_UINT32 len)
{
    memmove(dst, src, len);
}

TOOL_INT32 tool_mem_memcmp(TOOL_VOID* dst, TOOL_VOID* src, TOOL_UINT32 len)
{
	return memcmp(dst, src, len);
}

TOOL_UINT32 tool_str_strlen(TOOL_CONST TOOL_INT8* str)
{
    return (TOOL_INT32)strlen(str);
}

TOOL_INT8* tool_str_strncpy(TOOL_INT8* dst, TOOL_CONST TOOL_INT8* src, TOOL_UINT32 len)
{
#ifdef WIN32
	strncpy(dst, src, len);
	return dst;
#else
    return strncpy(dst, src, len);
#endif
}

TOOL_INT8* tool_str_strncat(TOOL_INT8* dst, TOOL_CONST TOOL_INT8* src, TOOL_UINT32 len)
{
	return strncat(dst, src, len);
}

TOOL_INT8* tool_str_strcat(TOOL_INT8* dst, TOOL_CONST TOOL_INT8* src)
{
	return strncat(dst, src, tool_str_strlen(src));
}

TOOL_INT32 tool_str_strncmp(TOOL_CONST TOOL_INT8* dst, TOOL_CONST TOOL_INT8* src, TOOL_UINT32 len)
{
    return strncmp(dst, src, len);
}

TOOL_INT32 tool_str_strncasecmp(TOOL_CONST TOOL_INT8* dst, TOOL_CONST TOOL_INT8* src, TOOL_UINT32 len)
{
#ifdef WIN32
	return _strnicmp(dst, src, len);
#else
	return strncasecmp(dst, src, len);
#endif
}

TOOL_INT8* tool_str_strerror()
{
#ifdef WIN32
	static TOOL_INT8 buf[128] = {0};
    strerror_s(buf, sizeof(buf)-1, errno);
	return buf;
#else
	return strerror(errno);
#endif
}

TOOL_INT32 tool_str_strtol(TOOL_INT8* str, TOOL_INT32 base)
{
	return strtol(str, NULL, base);
}

TOOL_INT32 tool_str_strtoul(TOOL_INT8* str, TOOL_INT32 base)
{
	return strtoul(str, NULL, base);
}

TOOL_INT8* tool_str_strtok_r(TOOL_INT8* str, TOOL_INT8* delim, TOOL_INT8**saveptr)
{
#ifdef WIN32
	return strtok_s(str, delim, saveptr);
#else
	return strtok_r(str, delim, saveptr);
#endif
}

TOOL_VOID tool_str_str2num(TOOL_INT8* num, TOOL_INT8* str, TOOL_UINT32 len)
{
	TOOL_INT8 h = 0;
	TOOL_INT8 l = 0;
	TOOL_UINT32 i = 0;
	for (i = 0; i < len; i++)
	{
		h = str[2*i];
		l = str[2*i+1];
		if (h == '\0' || l == '\0')
			break;
		
		if (h >= 'a' && h <= 'f')
			h = h-'a'+10;
		else if (h >= 'A' && h <= 'F') 
			h = h-'A'+10;
		else if (h >= '0' && h <= '9')
			h = h-'0';
		if (l >= 'a' && l <= 'f') 
			l = l-'a'+10;
		else if (l >= 'A' && l <= 'F') 
			l = l-'A'+10;
		else if (l >= '0' && l <= '9') 
			l = l-'0';
		num[i] = h*16+l;
	}
}

TOOL_INT8* tool_str_strstr(TOOL_CONST TOOL_INT8* haystack, TOOL_CONST TOOL_INT8* needle)
{
	return (TOOL_INT8*)strstr(haystack, needle);
}

TOOL_VOID tool_str_byte2str(TOOL_UINT8* byte, TOOL_INT32 byte_len, TOOL_INT8* str)
{	
	TOOL_CONST TOOL_INT8 szTable[] = "0123456789abcdef";
	TOOL_INT32 i = 0;
	for (i = 0; i < byte_len; i++)   
	{   
		*str++ = szTable[byte[i] >> 4];   
		*str++ = szTable[byte[i] & 0x0f];   
	}   
	*str = '\0';   
}

TOOL_VOID tool_str_str2byte(TOOL_INT8* str, TOOL_INT32 str_len, TOOL_UINT8* byte)
{
	TOOL_INT8 h = 0;
	TOOL_INT8 l = 0;
	TOOL_UINT32 i = 0;
	for (i = 0; i < str_len; i+=2)
	{
		h = str[i];
		l = str[i+1];
		if (h == '\0' || l == '\0')
			break;
		
		if (h >= 'a' && h <= 'f')
			h = h-'a'+10;
		else if (h >= 'A' && h <= 'F') 
			h = h-'A'+10;
		else if (h >= '0' && h <= '9')
			h = h-'0';
		if (l >= 'a' && l <= 'f') 
			l = l-'a'+10;
		else if (l >= 'A' && l <= 'F') 
			l = l-'A'+10;
		else if (l >= '0' && l <= '9') 
			l = l-'0';
		byte[i/2] = h*16+l;
	}
}

TOOL_INT32 tool_str_isIpv4(TOOL_INT8* str)
{
	if (str == NULL)
		return 0;

	TOOL_INT32 num1 = 0;
	TOOL_INT32 num2 = 0;
	TOOL_INT32 num3 = 0;
	TOOL_INT32 num4 = 0;
	
	TOOL_INT32 num = sscanf(str, "%d.%d.%d.%d", &num1, &num2, &num3, &num4);
	if (num != 4)
		return 0;
	if (num1 <= 0 || num1 > 255 ||
		num2 <  0 || num2 > 255 ||
		num3 <  0 || num3 > 255 ||
		num4 <  0 || num4 > 255)
		return 0;
//	log_debug("str(%s) is ip", str);
	return 1;	
}

TOOL_INT32 tool_str_xorInt32(TOOL_INT32 data)
{
	TOOL_INT32 i = 0x51484B4A;
	return data^i;
}

TOOL_INT16 tool_str_xorInt16(TOOL_INT16 data)
{
	TOOL_INT16 i = 0x4B4A;
	return data^i;
}

TOOL_INT32 tool_str_check(TOOL_UINT8* buf, TOOL_UINT32 len)
{
	return 0;
}



TOOL_INT32 tool_ctype_isprint(TOOL_INT32 c)
{
	return isprint(c);
}

TOOL_INT8* tool_std_getenv(TOOL_INT8* name)
{
#ifdef WIN32
	static TOOL_INT8* pValue;
	size_t len = 0;
	_dupenv_s( &pValue, &len, name);
	return pValue;
#else
	return getenv(name);
#endif
}

TOOL_INT32 tool_sysf_atoi(TOOL_CONST TOOL_INT8* str)
{
	return atoi(str);
}

TOOL_INT64 tool_sysf_atoll(TOOL_CONST TOOL_INT8* str)
{
	return atoll(str);
}

TOOL_VOID tool_sysf_setTick(TOOL_INT32* tv_sec, TOOL_INT32* tv_usec)
{
	struct timeval tv;
	gettimeofday(&tv, NULL);
	*tv_sec = tv.tv_sec;
	*tv_usec = tv.tv_usec;
}

TOOL_INT32 tool_sysf_isTickTimeout(TOOL_INT32 tv_sec, TOOL_INT32 tv_usec, TOOL_INT32 inteval_usec)
{
	struct timeval tv;
	gettimeofday(&tv, NULL);
	TOOL_INT32 result = (tv.tv_sec-tv_sec)*1000*1000 + (tv.tv_usec-tv_usec);
//	log_debug("result(%d)(%d.%d-%d.%d)", result, tv.tv_sec, tv.tv_usec, tv_sec, tv_usec);
	if (result < 0 || result > inteval_usec)
		return 1;
	return 0;	
}

TOOL_VOID tool_sysf_waitTickTimeout(TOOL_INT32 tv_sec, TOOL_INT32 tv_usec, TOOL_INT32 inteval_usec)
{
	struct timeval tv;
	gettimeofday(&tv, NULL);
	TOOL_INT32 result = (tv.tv_sec-tv_sec)*1000*1000 + (tv.tv_usec-tv_usec);
	TOOL_INT32 wait_usec = inteval_usec - result;
	if (wait_usec > 0 && wait_usec < 10*1000*1000)
		tool_sysf_usleep(wait_usec);	
}

TOOL_VOID tool_sysf_time(tool_time_t* tt)
{
	struct timeval tv;
	gettimeofday(&tv, NULL);
	tt->sec = tv.tv_sec;
	tt->usec = tv.tv_usec;
}

TOOL_VOID tool_sysf_dateGMT(tool_date_t* date, TOOL_UINT32 tt)
{
	time_t tmp = (time_t)tt;
	if (tmp == 0)
		tmp = time(NULL);
	struct tm result;
#ifdef WIN32
	gmtime_s(&result, &tmp);
#else
	gmtime_r(&tmp, &result);
#endif
    date->year = result.tm_year+1900;
    date->month = result.tm_mon+1;
    date->day = result.tm_mday;
    date->hour = result.tm_hour;
    date->minute = result.tm_min;
    date->second = result.tm_sec;	
}

TOOL_VOID tool_sysf_dateLocal(tool_date_t* date, TOOL_UINT32 tt)
{
	time_t tmp = (time_t)tt;
	if (tmp == 0)
		tmp = time(NULL);
	struct tm result;
#ifdef WIN32
	localtime_s(&result, &tmp);
#else
	localtime_r(&tmp, &result);
#endif
    date->year = result.tm_year+1900;
    date->month = result.tm_mon+1;
    date->day = result.tm_mday;
    date->hour = result.tm_hour;
    date->minute = result.tm_min;
    date->second = result.tm_sec;
}

TOOL_INT8* tool_sysf_tt2str(TOOL_UINT32 tt, TOOL_INT32 type, TOOL_INT8* str, TOOL_INT32 size)
{
	tool_date_t date;
	tool_sysf_dateLocal(&date, tt);
	snprintf(str, size, "%04d-%02d-%02d %02d:%02d:%02d", date.year, date.month, date.day, date.hour, date.minute, date.second);
	return str;
}

TOOL_INT32 tool_sysf_str2tt(TOOL_INT8* str, TOOL_INT32 type, TOOL_UINT32* tt)
{
	tool_date_t date;
	if (sscanf(str, "%d-%d-%d %d:%d:%d", &date.year, &date.month, &date.day, &date.hour, &date.minute, &date.second) != 6)
		return -1;
	date.year = date.year-1900;
	date.month = date.month-1;
	tool_time_Date2Time(date.year, date.month, date.day, date.hour, date.minute, date.second, tt);
	return 0;
}


TOOL_VOID tool_sysf_sleep(TOOL_INT32 sec)
{
#ifdef WIN32
    Sleep(sec*1000);
#else
	sleep(sec);
#endif 
}

TOOL_VOID tool_sysf_usleep(TOOL_INT32 usec)
{
#ifdef WIN32
	Sleep(usec/1000);
#else
	usleep(usec);
#endif    
}

TOOL_VOID tool_sysf_exit()
{
    exit(0);
}

typedef struct
{
	TOOL_MUTEX mutex;
}tool_rand_t;

tool_rand_t* g_tool_rand = NULL;

TOOL_VOID tool_sysf_srand(TOOL_INT32 seed)
{
	if (seed == 0)
	{
		tool_time_t tt;
		tool_sysf_time(&tt);	
		seed = tt.usec;
	}
	srand(seed);
}

TOOL_INT32 tool_sysf_rand()
{
	return rand();
}

TOOL_VOID tool_ran_init()
{
	if (g_tool_rand)
		return;
	
	g_tool_rand = (tool_rand_t*)tool_mem_malloc(sizeof(tool_rand_t), 1);
	tool_thread_initMutex(&g_tool_rand->mutex);
}

TOOL_INT32 tool_ran_rand(TOOL_INT32 seed)
{
	tool_ran_init();

	TOOL_INT32 ret = 0;
	tool_thread_lockMutex(&g_tool_rand->mutex);
	tool_sysf_srand(seed);
	ret = tool_sysf_rand();
	tool_thread_unlockMutex(&g_tool_rand->mutex);
	return ret;
}

TOOL_VOID tool_ran_wash(TOOL_INT32 seed, TOOL_INT32* array, TOOL_INT32 len)
{
	TOOL_INT32 i = 0; 
	for (i = 0; i < len; i++)
	{
		array[i] = i;
	}

	tool_ran_init();
	TOOL_INT32 pos = 0;
	TOOL_INT32 tmp = 0;
	tool_thread_lockMutex(&g_tool_rand->mutex);
	tool_sysf_srand(seed);
	for (i = 0; i < len-1; i++)
	{
		pos = tool_sysf_rand()%(len-1);
		tmp = array[i];
		array[i] = array[pos];
		array[pos] = tmp;
	}
	tool_thread_unlockMutex(&g_tool_rand->mutex);
/*
	for (i = 0; i < len; i++)
	{
		if (i % 40 == 0)
			printf("\n");		
		printf("%04d ", array[i]);
	}
	printf("\n");
*/
	
}

TOOL_VOID tool_ran_uuid(TOOL_INT8* uuid)
{
	TOOL_INT8* p = uuid;
	TOOL_INT32 c = 0;
	TOOL_INT32 i = 0;
	for (i = 0; i < 16; i++)
	{
		c = tool_ran_rand(0)%256;
		sprintf(p, "%02x", c);
		p += 2;
		if (i == 3 || i == 5 || i == 7 || i == 9)
		{
			*p = '-';
			p++;
		}
	}
}

TOOL_INT32 tool_sysf_system(TOOL_INT8* cmd)
{
//	log_debug("cmd(%s)", cmd);
	return system(cmd);    
//	return 0;
}

/*
 type: 0=none; 1=tcp; 2=udp; 3=mcast; 4=stun
 */
TOOL_INT32 tool_sysf_socket(TOOL_INT32 type)
{
    TOOL_INT32 ret = 0;
    if (type == 1)
    {
        ret = socket(AF_INET, SOCK_STREAM, 0);
    }
    else if(type == 2 || type == 3 || type == 4)
    {
        ret = socket(AF_INET, SOCK_DGRAM, 0);
    }
    if (ret < 0)
        log_fatalNo("socket");
    return ret;
}

TOOL_UINT16 tool_sysf_htons(TOOL_UINT16 num)
{
    return htons(num);
}

TOOL_UINT16 tool_sysf_ntohs(TOOL_UINT16 num)
{
    return ntohs(num);
}

TOOL_INT32 tool_sysf_htonl(TOOL_INT32 num)
{
	return htonl(num);
}

TOOL_INT32 tool_sysf_ntohl(TOOL_INT32 num)
{
	return ntohl(num);
}

TOOL_INT32 tool_sysf_bind(TOOL_INT32 sock, TOOL_VOID* addr)
{
	if (bind(sock, (struct sockaddr*)addr, sizeof(struct sockaddr_in)) < 0)
	{
		log_errorNo("bind(%d)", sock);
		return -1;
	}
	return 0;
}

TOOL_INT32 tool_sysf_listen(TOOL_INT32 lsfd)
{
	if (listen(lsfd, 10) < 0)
	{
		log_errorNo("listen");
		return -1;
	}
	return 0;
}

TOOL_INT32 tool_sysf_accept(TOOL_INT32 lsfd, TOOL_VOID* addr)
{
	
	TOOL_INT32 sock = 0, len = 0;
	len = sizeof(struct	sockaddr_in);
#ifdef WIN32
	sock = accept(lsfd, (struct sockaddr*)addr, (TOOL_INT32*)&len);
#else
	sock = accept(lsfd, (struct sockaddr*)addr, (socklen_t*)&len);
#endif
	if (sock < 0)
	{
		log_errorNo("accept");
		return -1;
	}

	struct sockaddr_in* tmp_addr = (struct sockaddr_in*)addr;
	TOOL_INT8 ip[40] = {0};
	tool_sysf_inet_ntop(&tmp_addr->sin_addr, ip, 40);
	TOOL_INT32 port = tool_sysf_ntohs(tmp_addr->sin_port);	
	log_state("addr(%s:%d)", ip, port);
	return sock;
}

/*
 type:  0=none;  1=NONBLOCK;  2=BLOCK
 */
TOOL_VOID tool_sysf_setSockBlock(TOOL_INT32 sock, TOOL_INT32 type)
{
#ifdef WIN32
	TOOL_UNLONG ul = 0;
	if (type == 1)
	{
		ul = 1;
		ioctlsocket(sock, FIONBIO, &ul);
	}
	else if (type == 2)
	{
		ul = 0;
		ioctlsocket(sock, FIONBIO, &ul);
	}
#else
	TOOL_INT32 flags = 0;
	if (type == 1)
	{
		flags = fcntl(sock, F_GETFL, 0);  
		fcntl(sock, F_SETFL, flags | O_NONBLOCK); 
	}
	else if (type == 2)
	{
		flags = fcntl(sock, F_GETFL, 0);   
		fcntl(sock, F_SETFL, flags & ~O_NONBLOCK);
	}
#endif
}

TOOL_VOID tool_sysf_inet_pton(TOOL_INT8* src, TOOL_VOID* dst)
{
#ifdef WIN32
	TOOL_INT32 tmp0 = 0;
	TOOL_INT32 tmp1 = 0;
	TOOL_INT32 tmp2 = 0;
	TOOL_INT32 tmp3 = 0;
	sscanf_s(src, "%d.%d.%d.%d", &tmp0, &tmp1, &tmp2, &tmp3);
	TOOL_UINT8* tmp_dst = (TOOL_UINT8*)dst;
	tmp_dst[0] = (TOOL_UINT8)tmp0;
	tmp_dst[1] = (TOOL_UINT8)tmp1;
	tmp_dst[2] = (TOOL_UINT8)tmp2;
	tmp_dst[3] = (TOOL_UINT8)tmp3;
#else
    inet_pton(AF_INET, src, dst);
#endif
}

TOOL_VOID tool_sysf_inet_ntop(TOOL_VOID* src, TOOL_INT8* dst, TOOL_INT32 size)
{
#ifdef WIN32
	TOOL_UINT8* tmp = (TOOL_UINT8*)src;
	_snprintf_s(dst, size, size, "%u.%u.%u.%u", tmp[0], tmp[1], tmp[2], tmp[3]);
#else
	inet_ntop(AF_INET, src, dst, size);
#endif
}

TOOL_INT32 tool_sysf_connect(TOOL_INT32 sock, TOOL_VOID *addr)
{
	TOOL_INT32 ret = 0;
	TOOL_INT32 err = 0;
	ret = connect(sock, (struct sockaddr*)addr, sizeof(struct sockaddr_in));
	err = tool_err_getErrno();
	if (ret < 0 && err != TOOL_ERR_EAGAIN && err != TOOL_ERR_EINPROGRESS)
	{
		log_errorNo("connect");
		return -1;
	}
    return 0;
}

TOOL_INT32 tool_sysf_pollRead(TOOL_INT32 sock, TOOL_INT32 msec)
{
	TOOL_INT32 ret = 0;
	TOOL_INT32 err = 0;
#ifdef WIN32
	fd_set fdRead;
	FD_ZERO(&fdRead);
	FD_SET(sock, &fdRead);
	timeval time;
	time.tv_sec = msec/1000;
	time.tv_usec = (msec%1000)*1000;
again:
	ret = select(0, &fdRead, NULL, NULL, &time);
	if (ret < 0)
	{
		err = tool_err_getErrno();
		if (err == TOOL_ERR_EINTR)
			goto again;
		else
			log_errorNo("select");
	}
#else
	struct pollfd pset;
	pset.fd = sock;
	pset.events = POLLIN;
again:
	ret = poll(&pset, 1, msec);
	if (ret < 0)
	{
		err = tool_err_getErrno();
		if (err == TOOL_ERR_EINTR)
			goto again;
		else
			log_errorNo("poll");
	}
#endif
	return ret;
}

TOOL_INT32 tool_sysf_pollWrite(TOOL_INT32 sock, TOOL_INT32 msec)
{
	TOOL_INT32 ret = 0;
	TOOL_INT32 err = 0;
#ifdef WIN32
	fd_set fdWrite;
	FD_ZERO(&fdWrite);
	FD_SET(sock, &fdWrite);
	timeval time;
	time.tv_sec = msec/1000;
	time.tv_usec = (msec%1000)*1000;
again:
	ret = select(0, NULL, &fdWrite, NULL, &time);
	if (ret < 0)
	{
		err = tool_err_getErrno();
		if (err == TOOL_ERR_EINTR)
			goto again;
		else
			log_errorNo("poll");
	}
#else
	struct pollfd pset;
	pset.fd = sock;
	pset.events = POLLOUT;
again:
	ret = poll(&pset, 1, msec);
	if (ret < 0)
	{
		err = tool_err_getErrno();
		if (err == TOOL_ERR_EINTR)
			goto again;
		else
			log_errorNo("poll");
	}
#endif
	return ret;
}


/*
 type:  0=none;  1=read; 2=write; 3=read & write;
 return:  -1=error;  0=timeout;  1=read;  2=write;  3=read & write;
 */
TOOL_INT32 tool_sysf_poll(TOOL_INT32 sock, TOOL_INT32* type, TOOL_INT32 msec)
{
	TOOL_INT32 ret = 0;
	TOOL_INT32 err = 0;
#ifdef WIN32
	fd_set fdRead;
	fd_set fdWrite;
	fd_set fdExcept;
	FD_ZERO(&fdRead);
	FD_ZERO(&fdWrite);
	FD_ZERO(&fdExcept);
	FD_SET(sock, &fdRead);
	FD_SET(sock, &fdWrite);
	timeval time;
	time.tv_sec = msec/1000;
	time.tv_usec = 0;
again:
	if (*type == 1)
		ret = select(0, &fdRead, NULL, NULL, &time);
	else if (*type == 2)
		ret = select(0, NULL, &fdWrite, NULL, &time);
	else if (*type == 3)
		ret = select(0, &fdRead, &fdWrite, NULL, &time);
	if (ret < 0)
	{
		err = tool_err_getErrno();
		if (err == TOOL_ERR_EINTR)
			goto again;
		else
			log_errorNo("poll");
	}
	*type = 0;
	if (FD_ISSET(sock, &fdRead))
		*type |= 1;
	if (FD_ISSET(sock, &fdWrite))
		*type |= 2;
#else
	struct pollfd pset;
	pset.fd = sock;
	if (*type == 1)
		pset.events = POLLIN;
	else if (*type == 2)
		pset.events = POLLOUT;
	else if (*type == 3)
		pset.events = (POLLIN | POLLOUT);
again:
	ret = poll(&pset, 1, msec);
	if (ret < 0)
	{
		err = tool_err_getErrno();
		if (err == TOOL_ERR_EINTR)
			goto again;
		else
			log_errorNo("poll");
	}
	*type = 0;
	if (pset.revents & POLLIN)
		*type |= 1;
	if (pset.revents & POLLOUT)
		*type |= 2;
#endif
	return ret;
}

/*
 type: 0=none; 1=SO_ERROR; 6=SO_RCVBUF; 7=SO_SNDBUF;
 */
TOOL_INT32 tool_sysf_getsockopt(TOOL_INT32 sock, TOOL_INT32 type)
{
#ifdef WIN32
	if (type == 1)
	{
		TOOL_INT32 error;
		TOOL_INT32 len = sizeof(error);
		if (getsockopt(sock, SOL_SOCKET, SO_ERROR, (TOOL_INT8*)&error, &len) < 0)
		{
			log_errorNo("getsockopt failure");
			return -1;
		}
		if	(error != 0)
			return -1;
	}
	else if (type == 6)
	{
		TOOL_INT32 size = 100*1024;
		TOOL_INT32 len = sizeof(size);
		len = sizeof(size);
		if (getsockopt(sock, SOL_SOCKET, SO_SNDBUF, (TOOL_INT8*)&size, &len))
		{
			log_errorNo("getsockopt SO_SNDBUF");
			return -1;
		}
		return size;
	}
	else if (type == 7)
	{
		TOOL_INT32 size = 100*1024;
		TOOL_INT32 len = sizeof(size);
		len = sizeof(size);
		if (getsockopt(sock, SOL_SOCKET, SO_RCVBUF, (TOOL_INT8*)&size, &len))
		{
			log_errorNo("getsockopt SO_SNDBUF");
			return -1;
		}
		return size;
	}
#else 
    if (type == 1)
    {
        TOOL_INT32 error = 0;
        socklen_t len = sizeof(error);
        if (getsockopt(sock, SOL_SOCKET, SO_ERROR, &error, &len) < 0)
        {
            log_errorNo("getsockopt failure");
            return -1;
        }
        if	(error != 0)
        {
			log_error("error(%d)", error);
			return -1;
        }
    }
	else if (type == 6)
    {
        TOOL_INT32 size = 100*1024;
		socklen_t len = sizeof(size);
		if (getsockopt(sock, SOL_SOCKET, SO_SNDBUF, &size, &len))
		{
			log_errorNo("getsockopt SO_SNDBUF");
			return -1;
		}
		return size;
    }
	else if (type == 7)
    {
        TOOL_INT32 size = 100*1024;
		socklen_t len = sizeof(size);
		if (getsockopt(sock, SOL_SOCKET, SO_RCVBUF, &size, &len))
		{
			log_errorNo("getsockopt SO_SNDBUF");
			return -1;
		}
		return size;
    }
#endif
    return 0;
}

/*
 type: 0=none; 1=SO_REUSEADDR; 2=TCP_NODELAY; 3=SO_LINGER; 4=SO_RCVTIMEO;
        5=SO_SNDTIMEO; 6=SO_RCVBUF; 7=SO_SNDBUF; 8=IP_MULTICAST_LOOP;
        9=IP_ADD_MEMBERSHIP; 10=IP_DROP_MEMBERSHIP; 11=SO_BROADCAST;
 */
TOOL_INT32 tool_sysf_setsockopt(TOOL_INT32 sock, TOOL_INT32 type, TOOL_VOID* param1, TOOL_INT32 param2)
{
#ifdef WIN32
	if (type == 1)
	{
		TOOL_INT32 flag = 1;
		if (setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, (TOOL_CONST TOOL_INT8 *)&flag, sizeof(TOOL_INT32)))
		{
			log_errorNo("setsockopt SO_REUSEADDR");
			return -1;
		}
	}
	else if (type ==2)
	{
		TOOL_INT32 nagle = 1;
		if (setsockopt(sock, IPPROTO_TCP, TCP_NODELAY, (TOOL_CONST TOOL_INT8*)&nagle, sizeof(TOOL_INT32)))
		{
			log_errorNo("setsockopt TCP_NODELAY");
			return -1;
		}
	}
	else if (type == 3)
	{
		struct linger lg;
		lg.l_onoff = 1;
		lg.l_linger = 0;
		if (setsockopt(sock, SOL_SOCKET, SO_LINGER, (TOOL_CONST TOOL_INT8*)&lg, sizeof(lg)) < 0)
		{
			log_errorNo("setsockopt SO_LINGER");
			return -1;
		}
	}
	else if (type == 4)
	{
		struct timeval	tv;
		tv.tv_sec = 5;
		tv.tv_usec = 0;
		if (setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO, (TOOL_CONST TOOL_INT8*)&tv, sizeof(tv)))
		{
			log_errorNo("setsockopt SO_RCVTIMEO");
			return -1;
		}

	}
	else if (type == 5)
	{
		struct	timeval	tv;
		tv.tv_sec = 5;
		tv.tv_usec = 0;
		if (setsockopt(sock, SOL_SOCKET, SO_RCVBUF, (TOOL_CONST TOOL_INT8*)&tv, sizeof(tv)))
		{
			log_errorNo("setsockopt SO_SNDTIMEO");
			return -1;
		}
	}
	else if (type == 6)
	{
		TOOL_INT32 size = 100*1024;
		TOOL_INT32 len = sizeof(size);
		if (setsockopt(sock, SOL_SOCKET, SO_SNDBUF, (TOOL_CONST TOOL_INT8*)&size, len))
		{
			log_errorNo("setsockopt SO_SNDBUF");
			return -1;
		}
	}
	else if (type == 7)
	{
		TOOL_INT32 size = 100*1024;
		TOOL_INT32 len = sizeof(size);
		if (setsockopt(sock, SOL_SOCKET, SO_RCVBUF, (TOOL_CONST TOOL_INT8*)&size, len))
		{
			log_errorNo("setsockopt SO_RCVBUF");
			return -1;
		}
	}
	else if (type == 8)
	{
		TOOL_INT32 loop = 0;
		if (setsockopt(sock, IPPROTO_IP, IP_MULTICAST_LOOP, (TOOL_CONST TOOL_INT8*)&loop, sizeof(loop)) < 0)
		{
			log_errorNo("setsockopt IP_MULTICAST_LOOP!");
			return -1;
		}
	}
	else if (type == 9)
	{
		if (param1 == NULL)
		{
			log_error("param1 is NULL");
			return -1;
		}
		struct ip_mreq mcast;
		tool_sysf_inet_pton((TOOL_INT8*)param1, &mcast.imr_multiaddr.s_addr);
		mcast.imr_interface.s_addr = tool_sysf_htonl(INADDR_ANY);
		if(setsockopt(sock, IPPROTO_IP, IP_ADD_MEMBERSHIP, (TOOL_CONST TOOL_INT8*)&mcast, sizeof(mcast)) < 0)
		{
			log_errorNo("setsockopt IP_ADD_MEMBERSHIP!");
			return -1;
		}
	}
	else if (type == 10)
	{
		if (param1 == NULL)
		{
			log_error("param1 is NULL");
			return -1;
		}
		struct ip_mreq mcast;
		tool_sysf_inet_pton((TOOL_INT8*)param1, &mcast.imr_multiaddr.s_addr);
		mcast.imr_interface.s_addr = tool_sysf_htonl(INADDR_ANY);
		if(setsockopt(sock, IPPROTO_IP, IP_DROP_MEMBERSHIP, (TOOL_CONST TOOL_INT8*)&mcast, sizeof(mcast)) < 0)
		{
			log_errorNo("setsockopt IP_DROP_MEMBERSHIP!");
			return -1;
		}
	}
#else
    if (type == 1)
    {
        TOOL_INT32 flag = 1;
        if (setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, (TOOL_VOID *)&flag, sizeof(TOOL_INT32)))
        {
            log_errorNo("setsockopt SO_REUSEADDR");
            return -1;
        }
		flag = 1;
		if (setsockopt(sock, SOL_SOCKET, SO_BROADCAST, (TOOL_VOID *)&flag, sizeof(TOOL_INT32)))
        {
            log_errorNo("setsockopt SO_BROADCAST");
            return -1;
        }
    }
    else if (type ==2 )
    {
        TOOL_INT32 nagle = 1;
        if (setsockopt(sock, IPPROTO_TCP, TCP_NODELAY, (TOOL_VOID*)&nagle, sizeof(TOOL_INT32)))
        {
            log_errorNo("setsockopt TCP_NODELAY");
            return -1;
        }
    }
    else if (type == 3)
    {
        struct linger lg;
        lg.l_onoff = 1;
        lg.l_linger = 0;
        if (setsockopt(sock, SOL_SOCKET, SO_LINGER, &lg, sizeof(lg)) < 0)
        {
            log_errorNo("setsockopt SO_LINGER");
            return -1;
        }
    }
    else if (type == 4)
    {
        struct	timeval	tv;
        tv.tv_sec = 5;
		tv.tv_usec = 0;
		if (setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(tv)))
		{
			log_errorNo("setsockopt SO_RCVTIMEO");
			return -1;
		}

    }
    else if (type == 5)
    {
        struct	timeval	tv;
        tv.tv_sec = 5;
		tv.tv_usec = 0;
		if (setsockopt(sock, SOL_SOCKET, SO_RCVBUF, &tv, sizeof(tv)))
		{
			log_errorNo("setsockopt SO_SNDTIMEO");
			return -1;
		}
    }
	else if (type == 6)
    {
        TOOL_INT32 size = 100*1024;
		TOOL_INT32 len = sizeof(size);
		if (setsockopt(sock, SOL_SOCKET, SO_SNDBUF, &size, len))
		{
			log_errorNo("setsockopt SO_SNDBUF");
			return -1;
		}
    }
	else if (type == 7)
    {
        TOOL_INT32 size = 100*1024;
		TOOL_INT32 len = sizeof(size);
		if (setsockopt(sock, SOL_SOCKET, SO_RCVBUF, &size, len))
		{
			log_errorNo("setsockopt SO_RCVBUF");
			return -1;
		}
    }
	else if (type == 8)
	{
		TOOL_INT32 loop = 0;
		if (setsockopt(sock, IPPROTO_IP, IP_MULTICAST_LOOP, &loop, sizeof(loop)) < 0)
		{
			log_errorNo("setsockopt IP_MULTICAST_LOOP!");
			return -1;
		}
	}
	else if (type == 9)
	{
		if (param1 == NULL)
		{
			log_error("param1 is NULL");
			return -1;
		}
		struct ip_mreq mcast;
		tool_sysf_inet_pton(param1, &mcast.imr_multiaddr.s_addr);
		mcast.imr_interface.s_addr = tool_sysf_htonl(INADDR_ANY);
		TOOL_INT32 ret = setsockopt(sock, IPPROTO_IP, IP_ADD_MEMBERSHIP, (TOOL_INT8*)&mcast, sizeof(mcast));
		if (ret < 0)
		{
			log_errorNo("setsockopt IP_ADD_MEMBERSHIP!(%d) ret(%d) (%d)", sock, ret, errno);
			return -1;
		}
	}
	else if (type == 10)
	{
		if (param1 == NULL)
		{
			log_error("param1 is NULL");
			return -1;
		}
		struct ip_mreq mcast;
		tool_sysf_inet_pton(param1, &mcast.imr_multiaddr.s_addr);
		mcast.imr_interface.s_addr = tool_sysf_htonl(INADDR_ANY);
		if(setsockopt(sock, IPPROTO_IP, IP_DROP_MEMBERSHIP, (TOOL_INT8*)&mcast, sizeof(mcast)) < 0)
		{
			log_errorNo("setsockopt IP_DROP_MEMBERSHIP!");
			return -1;
		}
	}
	else if (type == 11)
	{
		TOOL_INT32 enable = 1;
		if(setsockopt(sock, SOL_SOCKET, SO_BROADCAST, (TOOL_INT8*)&enable, sizeof(enable)) < 0)
		{
			log_errorNo("setsockopt SO_BROADCAST!");
			return -1;
		}
	}
#endif
	return 0;
}


/*
 type: 0=none; 1=peek;
 */
TOOL_INT32 tool_sysf_recv(TOOL_INT32 sock, TOOL_VOID*data, TOOL_UINT32 len, TOOL_INT32 type)
{
    TOOL_INT32 ret = 0;
	TOOL_INT32 err = 0;
    if (type == 1)
    {
        ret = (TOOL_INT32)recv(sock, (TOOL_INT8*)data, len, MSG_PEEK);
        if (ret < 0)
        {
			err = tool_err_getErrno();
            if (err == TOOL_ERR_EINTR || err == TOOL_ERR_EAGAIN)
            {
                tool_sysf_usleep(20*1000);
                return 0;
            }
            else
                log_errorNo("write sock(%d)", sock);
        }
    }

    return ret;
}

TOOL_INT32 tool_sysf_openFile(TOOL_INT8* file)
{
	TOOL_INT32 fd = 0;
#ifdef WIN32
	_sopen_s(&fd, file, O_RDWR|O_CREAT, _SH_DENYNO, _S_IREAD | _S_IWRITE);
	return fd;
#else
	fd = open(file, O_RDWR|O_CREAT, 777);
#endif
	if (fd < 0)
		log_errorNo("open");
	return fd;

}

TOOL_INT32 tool_sysf_closeFile(TOOL_INT32 fd)
{
#ifdef WIN32
	return _close(fd);
#else
	return close(fd);
#endif
}

TOOL_INT32 tool_sysf_closeSock(TOOL_INT32 sock)
{
#ifdef WIN32
	return closesocket(sock);
#else
	return close(sock);
#endif
}

TOOL_INT32 tool_sysf_write(TOOL_INT32 fd, TOOL_VOID* data, TOOL_UINT32 len)
{
    TOOL_INT32 ret = 0;
	TOOL_INT32 err = 0;
#ifdef WIN32
	ret = send(fd, (TOOL_INT8*)data, len, 0);
#else
	ret = write(fd, data, len);
#endif
    if (ret < 0)
    {
		err = tool_err_getErrno();
        if (err == TOOL_ERR_EINTR || err == TOOL_ERR_EAGAIN)
        {
            tool_sysf_usleep(20*1000);
            return 0;
        }
        else
        {
          log_errorNo("write fd(%d)", fd);
        }
    }

    return ret;
}

TOOL_INT32 tool_sysf_read(TOOL_INT32 fd, TOOL_VOID* data, TOOL_UINT32 len)
{
    TOOL_INT32 ret = 0;
	TOOL_INT32 err = 0;
#ifdef WIN32
	ret = recv(fd, (TOOL_INT8*)data, len, 0);
#else
	ret = read(fd, data, len);
#endif
    if (ret < 0)
    {
		err = tool_err_getErrno();
        if (err == TOOL_ERR_EINTR || err == TOOL_ERR_EAGAIN)
        {
            tool_sysf_usleep(20*1000);
            return 0;
        }
        else
        {
        	log_errorNo("read fd(%d)", fd);
        }
    }

    return ret;
}

TOOL_INT32 tool_sysf_readLine(TOOL_INT32 fd, TOOL_VOID* buf, TOOL_INT32 lineSize)
{
	TOOL_UINT8* pIndex = (TOOL_UINT8*)buf;
	TOOL_UINT8* pStart = (TOOL_UINT8*)buf;
	while (1)
	{
		if (tool_sysf_read(fd, pIndex, 1) != 1)
		{
            return -1;
		}

		pIndex ++;
		if (*(pIndex-1) == '\n')
			break;
		if (pIndex-pStart >= lineSize-1)
		{
			log_error("the line is too long");
			return -1;
		}
	}
	return (TOOL_INT32)(pIndex - pStart);
}


TOOL_INT32 tool_sysf_recvPeek(TOOL_INT32 fd, TOOL_VOID* data, TOOL_UINT32 len)
{
    TOOL_INT32 ret = 0;
	TOOL_INT32 err = 0;

	ret = recv(fd, (TOOL_INT8*)data, len, MSG_PEEK);
    if (ret < 0)
    {
		err = tool_err_getErrno();
        if (err == TOOL_ERR_EINTR || err == TOOL_ERR_EAGAIN)
        {
            tool_sysf_usleep(20*1000);
            return 0;
        }
        else
        {
        	log_errorNo("read fd(%d)", fd);
        }
    }

    return ret;	
}

TOOL_INT32 tool_sysf_initAddr(TOOL_VOID* addr, TOOL_INT8* ip, TOOL_UINT16 port)
{
	struct sockaddr_in* index = (struct sockaddr_in*)addr;
	index->sin_family = AF_INET;
	if (ip == NULL)
		index->sin_addr.s_addr = htonl(INADDR_ANY);
	else if (tool_str_isIpv4(ip))
	{
		tool_sysf_inet_pton(ip, &index->sin_addr);	
	}
	else
	{
//		struct hostent *host =gethostbyname(ip);
//		if (host)
//			tool_mem_memcpy(&(index->sin_addr), host->h_addr_list[0], 4);
//		tool_sysf_inet_pton(ip, &index->sin_addr);	
#ifdef WIN32

#else
		struct addrinfo hints;
		struct addrinfo* res = NULL;
		tool_mem_memset(&hints, sizeof(struct addrinfo));
	    hints.ai_family = AF_INET; /* Allow IPv4 */
	    hints.ai_flags = AI_PASSIVE; /* For wildcard IP address */
		if (getaddrinfo(ip, NULL, &hints, &res) != 0 || res == NULL)
		{
			log_errorNo("getaddrinfo(%s)(%d)", ip, port);
			return -1;
		}
		struct sockaddr_in* tmp = (struct sockaddr_in*)res->ai_addr;
		tool_mem_memcpy(&(index->sin_addr), &(tmp->sin_addr), 4);
		freeaddrinfo(res);
#endif
	}
	index->sin_port = tool_sysf_htons(port); 
	return 0;
}

TOOL_INT32 tool_sysf_sendto(TOOL_INT32 sock, TOOL_VOID*data, TOOL_UINT32 len, TOOL_VOID* addr)
{
#ifdef WIN32
	TOOL_INT32 addrlen = 0;
#else
	socklen_t addrlen = 0;
#endif
	addrlen = sizeof(struct sockaddr);
	TOOL_INT32 ret = sendto(sock, (TOOL_CONST TOOL_INT8*)data, len, 0, (TOOL_CONST struct sockaddr*)addr, addrlen);
	if (ret < 0)
		log_errorNo("sendto sock(%d)", sock);
	return ret;
}

TOOL_INT32 tool_sysf_recvfrom(TOOL_INT32 sock, TOOL_VOID*data, TOOL_UINT32 len, TOOL_VOID* addr)
{
#ifdef WIN32
	TOOL_INT32 addrlen = 0;
#else
	socklen_t addrlen = 0;
#endif
	addrlen = sizeof(struct sockaddr);
	TOOL_INT32 ret = recvfrom(sock, (TOOL_INT8*)data, len, 0, (struct sockaddr*)addr, &addrlen);
	if (ret < 0)
		log_errorNo("recvfrom sock(%d)", sock);
	return ret;
}

/*
 type: 0=none; 1=SIGPIPE
 */
TOOL_INT32 tool_sysf_singal(TOOL_INT32 type)
{
#ifdef WIN32
	WORD wVersionRequested;
	WSADATA wsaData;
	wVersionRequested = MAKEWORD( 2, 2 );
	WSAStartup( wVersionRequested, &wsaData );
#else
	if (type == 1)
		signal(SIGPIPE, SIG_IGN);
#endif
    return 0;
}

TOOL_INT32 tool_sysf_getlocalip_bySock(TOOL_INT32 sock, TOOL_VOID* outip)
{
#ifdef WIN32
	return 0;
#else
	TOOL_INT32 i = 0;
	struct ifconf ifconf;
	TOOL_INT8 buf[512] = {0};
	TOOL_INT8* ip = NULL;
	TOOL_INT8 lo_ip[4] = {0};
	lo_ip[0] = 127;
	lo_ip[1] = 0;
	lo_ip[2] = 0;
	lo_ip[3] = 1;
	
	//³õÊ¼»¯ifconf
	ifconf.ifc_len = 512;
	ifconf.ifc_buf = buf;
	if (ioctl(sock, SIOCGIFCONF, &ifconf) < 0)    //»ñÈ¡ËùÓÐ½Ó¿ÚÐÅÏ¢
		return -1;
	
	//½ÓÏÂÀ´Ò»¸öÒ»¸öµÄ»ñÈ¡IPµØÖ·
	struct ifreq *ifr = (struct ifreq*)buf;
	for (i = 0; i < ifconf.ifc_len/sizeof(struct ifreq); i++)
	{
		ip = (TOOL_INT8*)&(((struct sockaddr_in*)&(ifr->ifr_addr))->sin_addr);
		if (tool_str_strncmp(ip, lo_ip, 4)==0)  //ÅÅ³ý127.0.0.1£¬¼ÌÐøÏÂÒ»¸ö
		{
			ifr ++;
			continue;
		}
		tool_str_strncpy(outip, ip, 4);
		return 0;
	}
	return -1;	
#endif
}

TOOL_INT32 tool_sysf_getlocalip(TOOL_VOID* outip)
{
	TOOL_INT32 sockfd = tool_sysf_socket(2);
	TOOL_INT32 ret = tool_sysf_getlocalip_bySock(sockfd, outip);
	tool_sysf_closeSock(sockfd);
	return ret;
}

TOOL_INT32 tool_sysf_getsockname(TOOL_INT32 sock, TOOL_VOID* ip, TOOL_UINT16* port)
{
#ifdef WIN32
	struct sockaddr_in addr;   
	TOOL_INT32 len = sizeof(struct sockaddr_in);   
	if (getsockname(sock, (struct sockaddr*)&addr, &len) < 0)
	{
		log_error("getsockname sock(%d)", sock);
		return -1;
	}
	tool_mem_memcpy(ip, &addr.sin_addr, 4);
	if (port)
		*port = addr.sin_port;
	return 0;
#else
	struct sockaddr_in addr;   
	socklen_t len = sizeof(struct sockaddr_in);   
	if (getsockname(sock, (struct sockaddr*)&addr, &len) < 0)
	{
		log_error("getsockname sock(%d)", sock);
		return -1;
	}
	tool_mem_memcpy(ip, &addr.sin_addr, 4);
	if (port)
		*port = addr.sin_port;
return 0;
#endif
}

TOOL_INT32 tool_sysf_getpeername(TOOL_INT32 sock, TOOL_VOID* ip, TOOL_UINT16* port)
{
#ifdef WIN32	
	struct sockaddr_in addr;   
	TOOL_INT32 len = sizeof(struct sockaddr_in);   
	if (getpeername(sock, (struct sockaddr*)&addr, &len) < 0)
	{
		log_error("getsockname sock(%d)", sock);
		return -1;
	}
	tool_mem_memcpy(ip, &addr.sin_addr, 4);
	*port = addr.sin_port;
	return 0;
#else
	struct sockaddr_in addr;   
	socklen_t len = sizeof(struct sockaddr_in);   
	if (getpeername(sock, (struct sockaddr*)&addr, &len) < 0)
	{
		log_error("getsockname sock(%d)", sock);
		return -1;
	}
	tool_mem_memcpy(ip, &addr.sin_addr, 4);
	*port = addr.sin_port;
	return 0;
#endif
}

TOOL_INT32 tool_io_mkdir(const TOOL_INT8* pathname)
{
#ifdef WIN32
	if (mkdir(pathname) < 0)
	{
//		log_errorNo("mkdir");
		return -1;
	}
	return 0;
#else
	if (mkdir(pathname, 777) < 0)
	{
//		log_errorNo("mkdir");
		return -1;
	}
	return 0;
#endif
}

TOOL_INT32 tool_io_rename(const TOOL_INT8* oldpath, const TOOL_INT8* newpath)
{
	if (rename(oldpath, newpath) < 0)
	{
		log_errorNo("rename");
		return -1;
	}
	return 0;
}

TOOL_VOID* tool_io_fopen(const TOOL_INT8* pathname, TOOL_CONST TOOL_INT8* mode)
{
	FILE* fp = fopen(pathname, mode);
	if (fp == NULL)
	{
		log_errorNo("fopen");
		return NULL;
	}
	return fp;
}

TOOL_INT32 tool_io_fread(TOOL_VOID* fp, TOOL_VOID* data, TOOL_UINT32 len)
{
	TOOL_INT32 ret = fread(data, 1, len, (FILE*)fp);
	if (ret < 0)
	{
		log_errorNo("fread");
		return -1;
	}
	return ret;
}

TOOL_INT32 tool_io_fclose(TOOL_VOID* fp)
{
	FILE* p_fp = (FILE*)fp;
	return fclose(p_fp);
}

TOOL_INT32 tool_io_fseek(TOOL_VOID* fp, TOOL_INT32 pos)
{
	FILE* p_fp = (FILE*)fp;
	return fseek(p_fp, pos, SEEK_SET);
}

TOOL_INT32 tool_io_fflush(TOOL_VOID* fp)
{
	FILE* p_fp = (FILE*)fp;
	return fflush(p_fp);
}

TOOL_INT32 tool_thread_create(TOOL_THREAD* thd, TOOL_VOID*attr, TOOL_FUNC func, TOOL_VOID* param)
{
	TOOL_INT32 ret = pthread_create(thd, NULL, func, param);
	if (ret == 0)
	{
		g_tool_thread_count ++;
		return 0;
	}
    return -1;
}

TOOL_INT32 tool_thread_join(TOOL_THREAD thd)
{
	TOOL_INT32 ret = pthread_join(thd, NULL);
	if (ret == 0)
	{
		g_tool_thread_count --;
		return 0;
	}
	return -1;
}

TOOL_VOID tool_thread_exit(TOOL_VOID* ret)
{
	pthread_exit(ret);
}

TOOL_INT32 tool_thread_initMutex(TOOL_MUTEX* mutex)
{
	TOOL_INT32 ret = pthread_mutex_init(mutex, NULL);
	if (ret == 0)
	{
		g_tool_mutex_count ++;
		return 0;
	}
	return -1;
}

TOOL_INT32 tool_thread_doneMutex(TOOL_MUTEX* mutex)
{
    TOOL_INT32 ret = pthread_mutex_destroy(mutex);
	if (ret == 0)
	{
		g_tool_mutex_count --;
		return 0;
	}
	return -1;
}

TOOL_VOID tool_thread_lockMutex(TOOL_MUTEX* mutex)
{
    pthread_mutex_lock(mutex);
}

TOOL_INT32 tool_thread_trylockMutex(TOOL_MUTEX* mutex)
{
	if (pthread_mutex_trylock(mutex) == 0)
		return 0;
	return -1;
}

TOOL_VOID tool_thread_unlockMutex(TOOL_MUTEX* mutex)
{
	 pthread_mutex_unlock(mutex);
}

TOOL_INT32 tool_thread_initCond(TOOL_COND* cond)
{
	TOOL_INT32 ret = pthread_cond_init(cond, NULL);
	if (ret == 0)
	{
		g_tool_cond_count ++;
		return 0;
	}
	return -1;
}
	
TOOL_INT32 tool_thread_doneCond(TOOL_COND* cond)
{
	TOOL_INT32 ret = pthread_cond_destroy(cond);
	if (ret == 0)
	{
		g_tool_cond_count --;
		return 0;
	}
	return -1;
}

TOOL_INT32 tool_thread_waitCond(TOOL_COND* cond, TOOL_MUTEX* mutex)
{
	if (pthread_cond_wait(cond, mutex) < 0)
	{
		log_errorNo("pthread_cond_wait");
		return -1;
	}
	return 0;
}

TOOL_INT32 tool_thread_timedwaitCond(TOOL_COND* cond, TOOL_MUTEX* mutex, TOOL_INT32 sec)
{
	tool_time_t tt;
	tool_sysf_time(&tt);
	struct timespec ts;
	ts.tv_sec  = tt.sec  + sec;
	ts.tv_nsec = tt.usec * 1000;
	if (pthread_cond_timedwait(cond, mutex, &ts) != 0)
	{
	//	log_errorNo("pthread_cond_timedwait ret(%d)", ret);
		return -1;
	}
	return 0;
}

TOOL_VOID tool_thread_signalCond(TOOL_COND* cond)
{
	pthread_cond_signal(cond);
}

TOOL_VOID tool_thread_broadcastCond(TOOL_COND* cond)
{
	pthread_cond_broadcast(cond);
}

TOOL_VOID tool_time_Date2Time(TOOL_INT32 year, TOOL_INT32 month, TOOL_INT32 day, TOOL_INT32 hour, TOOL_INT32 min, TOOL_INT32 sec, TOOL_UINT32* second)
{
	struct tm s_tm;
	time_t tt = time(NULL);
#ifdef WIN32
		localtime_s(&s_tm, &tt);
#else
		localtime_r(&tt, &s_tm);
#endif
	s_tm.tm_year = year;
	s_tm.tm_mon = month;
	s_tm.tm_mday = day;
	s_tm.tm_hour = hour;
	s_tm.tm_min = min;
	s_tm.tm_sec = sec;
	*second = (TOOL_UINT32)mktime(&s_tm);
//	log_debug("%04d-%02d-%02d %02d:%02d:%02d => %d", year, month, day, hour, min, sec, *second);
}

TOOL_INT32 tool_err_getErrno()
{
#ifdef WIN32
	return WSAGetLastError();
#else
	return errno;
#endif
}

#if defined __cplusplus
}
#endif

