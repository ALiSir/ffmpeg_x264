//
//  tool_sock.c
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
#include "tool_type.h"
#include "tool_sock.h"
#include "tool_log.h"
#include "tool_sysf.h"

typedef struct
{
	TOOL_UINT8 state;
	TOOL_THREAD tick_pid;
	TOOL_INT32 tick;
	TOOL_INT32 count;
	TOOL_MUTEX mutex;

	TOOL_INT32 ys_port;
}tool_sock_manager_t;

static tool_sock_manager_t* g_tool_sock_manager = NULL;

TOOL_VOID* tool_sock_runTick(void* param)
{
	tool_sock_manager_t* manager = (tool_sock_manager_t*)param;
    TOOL_INT32 sock_count = 0;
    TOOL_INT32 tick = manager->tick;
    TOOL_INT32 mem_count = 0;
	TOOL_INT32 last_mem_count = 0;
    TOOL_UINT32 mem_size = 0;
	TOOL_UINT32 last_mem_size = 0;
	TOOL_INT32 thread_count = 0;
	TOOL_INT32 last_thread_count = 0;
	TOOL_INT32 mutex_count = 0;
	TOOL_INT32 last_mutex_count = 0;
	TOOL_INT32 cond_count = 0;
	TOOL_INT32 last_cond_count = 0;
	while (manager->state)
	{
		tool_sysf_sleep(1);
		manager->tick ++;
		tool_mem_getState(&mem_count, &mem_size, &thread_count, &mutex_count, &cond_count);
        if (manager->tick - tick >= 60 || manager->count != sock_count || mem_count != last_mem_count || mem_size != last_mem_size ||
			thread_count != last_thread_count || mutex_count != last_mutex_count || cond_count != last_cond_count)
        {
            if (mem_size < 1024)
                log_state("socket(%d) memory(%d,%dB) thread(%d) mutex(%d) cond(%d)", manager->count, mem_count, mem_size, thread_count, mutex_count, cond_count);
            else if (mem_size < 1024*1024)
                log_state("socket(%d) memory(%d,%.2fKB) thread(%d) mutex(%d) cond(%d)", manager->count, mem_count, mem_size*1.0/1024, thread_count, mutex_count, cond_count);
            else
                log_state("socket(%d) memory(%d,%.2fMB) thread(%d) mutex(%d) cond(%d)", manager->count, mem_count, mem_size*1.0/(1024*1024), thread_count, mutex_count, cond_count);
            sock_count = manager->count;
            tick = manager->tick;
			last_mem_count = mem_count;
			last_mem_size = mem_size;
			last_thread_count = thread_count;
			last_mutex_count = mutex_count;
			last_cond_count = cond_count;
        }
	}
	return NULL;
}

TOOL_VOID tool_sock_init()
{
	if (g_tool_sock_manager)
	{
		log_error("g_tool_sock_manager is init");
		return ;
	}
	g_tool_sock_manager = (tool_sock_manager_t*)tool_mem_malloc(sizeof(tool_sock_manager_t), 1);
	tool_sysf_singal(1);
	tool_thread_initMutex(&g_tool_sock_manager->mutex);
	g_tool_sock_manager->state = 1;
	g_tool_sock_manager->tick = 0;
	g_tool_sock_manager->ys_port = TOOL_SOCK_YS_BEGIN;
	if (tool_thread_create(&g_tool_sock_manager->tick_pid, NULL, tool_sock_runTick, (TOOL_VOID*)g_tool_sock_manager))
		log_fatalNo("pthread_create tick");
}

TOOL_VOID tool_sock_done()
{
	if (g_tool_sock_manager == NULL)
	{
		log_error("g_tool_sock_manager is done");
		return;
	}
	g_tool_sock_manager->state = 0;
	tool_thread_join(g_tool_sock_manager->tick_pid);
	tool_thread_doneMutex(&g_tool_sock_manager->mutex);
	tool_mem_free(g_tool_sock_manager);
	g_tool_sock_manager = NULL;
}

TOOL_VOID tool_sock_setTick(TOOL_INT32* tick)
{
	if (g_tool_sock_manager == NULL)
		log_fatal("g_tool_sock_manager is NULL");
	*tick = g_tool_sock_manager->tick;
}

TOOL_INT32 tool_sock_isTickTimeout(TOOL_INT32 tick, TOOL_INT32 sec)
{
	if (g_tool_sock_manager == NULL)
		log_fatal("g_tool_sock_manager is NULL");

	if (g_tool_sock_manager->tick - tick >= sec || g_tool_sock_manager->tick < tick)
		return 1;
	return 0;
}

TOOL_INT16 tool_sock_getYsPort()
{
	if (g_tool_sock_manager == NULL)
		log_fatal("g_tool_sock_manager is NULL");

	TOOL_INT32 ret = 0;
	tool_thread_lockMutex(&g_tool_sock_manager->mutex);
	if (g_tool_sock_manager->ys_port > TOOL_SOAK_YS_END)
		g_tool_sock_manager->ys_port = TOOL_SOCK_YS_BEGIN;
	ret = g_tool_sock_manager->ys_port;
	g_tool_sock_manager->ys_port += 2;
	tool_thread_unlockMutex(&g_tool_sock_manager->mutex);
	return ret;
}

TOOL_INT32 tool_sock_item_connect(tool_sock_item_t* item)
{
	tool_sysf_setSockBlock(item->fd, 1);
    TOOL_INT32 connect_ret = -1;
	TOOL_INT32 startTime = g_tool_sock_manager->tick;
	
    do
    {
        if (tool_sysf_connect(item->fd, item->remote_addr) < 0)
            break;
        TOOL_INT32 pool_ret = 0;
		while (1)
		{
			if (item->flag && *item->flag == 0)
				break;
			pool_ret = tool_sysf_pollWrite(item->fd, 100);
	        if (pool_ret != 0)
				break;
			if (g_tool_sock_manager->tick- startTime >= item->timeout)
				break;			
		}
		if (pool_ret < 0)
			break;
        else if (pool_ret == 0)
        {
            log_error("poll timeout(%d)", item->timeout);
            break;
        }

        if (tool_sysf_getsockopt(item->fd, 1) < 0)
        {
            log_error("connect(%d)", item->fd);
            break;
        }
        connect_ret = 0;
    } while (0);
	return connect_ret;
}

TOOL_VOID tool_sock_item_init(tool_sock_item_t* item)
{
	if (g_tool_sock_manager == NULL)
		log_fatal("g_tool_sock_manager is NULL");
	item->tick = -12345678;
	item->fd = 0;
}

TOOL_INT32 tool_sock_item_open_v2(tool_sock_item_t* item, TOOL_SOCK_TYPE1_E type1, TOOL_SOCK_TYPE2_E type2, TOOL_INT8* remote_ip, TOOL_UINT16 remote_port, TOOL_INT8* source_ip, TOOL_UINT16 source_port)
{
	if (g_tool_sock_manager == NULL)
	{
		tool_sock_init();
//		return;
//		log_fatal("g_tool_sock_manager is NULL");
	}

	// init
	item->tick = g_tool_sock_manager->tick;
	item->type1 = type1;
	item->type2 = type2;
	item->timeout = TOOL_SOCK_TIMEOUT;
	if (tool_sysf_initAddr(item->remote_addr, remote_ip, remote_port) < 0)
		return -1;
	if (tool_sysf_initAddr(item->source_addr, source_ip, source_port) < 0)
		return -1;
	log_state("remote-addr(%s:%d), source-addr(%s:%d)", remote_ip, remote_port, source_ip, source_port);
	item->width = 0;
	item->height = 0;
	item->fps_video = 0;
	item->sec = 0;
	item->usec = 0;
	item->seq = 0;
	item->frame_sub_type = 0;
	
	// socket
	item->fd = tool_sysf_socket(type1);
	tool_sysf_setsockopt(item->fd, 1, NULL, 0);
	if (tool_sysf_bind(item->fd, item->source_addr) < 0)
	{
		tool_sysf_closeSock(item->fd);
		item->fd = -1;
		return -1;
	}

	if (type1 == TOOL_SOCK_TYPE1_TCP)
	{
		if (type2 == TOOL_SOCK_TYPE2_SERVER1)
		{
			if (tool_sysf_listen(item->fd) < 0)
			{
				tool_sysf_closeSock(item->fd);
				item->fd = -1;
				return -1;
			}
		}
		else if (type2 == TOOL_SOCK_TYPE2_CLIENT)
		{
			if (tool_sock_item_connect(item) < 0)
			{
				tool_sysf_closeSock(item->fd);
				item->fd = -1;
				return -1;
			}
		}
	}
	else if (type1 == TOOL_SOCK_TYPE1_UDP)
	{
		
	}
	else if (type1 == TOOL_SOCK_TYPE1_MCAST)
	{	
		tool_sysf_setsockopt(item->fd, 8, NULL, 0);
		tool_sysf_setsockopt(item->fd, 9, remote_ip, 0);
	}
	else if (type1 == TOOL_SOCK_TYPE1_BCAST)
	{
		tool_sysf_setsockopt(item->fd, 11, NULL, 0);
	}

	tool_sysf_setsockopt(item->fd, 1, NULL, 0);
	tool_sysf_setsockopt(item->fd, 3, NULL, 0);
	tool_sysf_setSockBlock(item->fd, 1);

	item->is_error = 0;
	g_tool_sock_manager->count ++;
	return 0;	
}

TOOL_INT32 tool_sock_item_accept(tool_sock_item_t* listen_item, tool_sock_item_t* recv_item)
{
	if (g_tool_sock_manager == NULL)
		log_fatal("g_tool_sock_manager is NULL");

	if (listen_item->is_error)
		return -1;
	
	recv_item->fd = tool_sysf_accept(listen_item->fd, recv_item->remote_addr);
	if (recv_item->fd < 0)
	{
		listen_item->is_error = 1;
		recv_item->is_error = 1;
		return -1;
	}
		
	recv_item->type1 = TOOL_SOCK_TYPE1_TCP;
	recv_item->type2 = TOOL_SOCK_TYPE2_SERVER2;
	recv_item->timeout = listen_item->timeout;
	tool_mem_memcpy(recv_item->source_addr, listen_item->source_addr, sizeof(recv_item->source_addr));
	tool_sysf_setsockopt(recv_item->fd, 1, NULL, 0);
	tool_sysf_setsockopt(recv_item->fd, 1, NULL, 0);
	tool_sysf_setsockopt(recv_item->fd, 3, NULL, 0);
	tool_sysf_setSockBlock(recv_item->fd, 1);
	recv_item->tick = g_tool_sock_manager->tick;
	recv_item->is_error = 0;
	g_tool_sock_manager->count ++;
	return 0;
}

TOOL_VOID tool_sock_item_setTimeout(tool_sock_item_t* item, TOOL_INT8 timeout)
{
	if (g_tool_sock_manager == NULL)
		log_fatal("g_tool_sock_manager is NULL");
	item->timeout = timeout;
}

TOOL_VOID tool_sock_item_setFlag(tool_sock_item_t* item, TOOL_INT32* flag)
{
	if (g_tool_sock_manager == NULL)
		log_fatal("g_tool_sock_manager is NULL");
	item->flag = flag;
}

TOOL_INT32 tool_sock_item_isLive(tool_sock_item_t* item)
{
	if (g_tool_sock_manager == NULL)
		log_fatal("g_tool_sock_manager is NULL");
	
	if (item->fd > 0)
		return 1;
	else 
		return 0;
}

TOOL_INT32 tool_sock_item_isTimeout(tool_sock_item_t* item, TOOL_INT32 inteval)
{
	if (g_tool_sock_manager == NULL)
		log_fatal("g_tool_sock_manager is NULL");
//	log_debug("sock(%d) (%d-%d-%d)", item->fd, g_tool_sock_manager->tick, item->tick, inteval);
	if (g_tool_sock_manager->tick - item->tick >= inteval || g_tool_sock_manager->tick < item->tick)
		return 1;
	return 0;
}

TOOL_VOID tool_sock_item_setTick(tool_sock_item_t* item)
{
	if (g_tool_sock_manager == NULL)
		log_fatal("g_tool_sock_manager is NULL");
	item->tick = g_tool_sock_manager->tick;
}

TOOL_INT32 tool_sock_item_pollRead(tool_sock_item_t* item, TOOL_INT32 msec)
{
	if (g_tool_sock_manager == NULL)
		log_fatal("g_tool_sock_manager is NULL");

	if (item->is_error)
		return -1;

	TOOL_INT32 ret = tool_sysf_pollRead(item->fd, msec);
	if (ret < 0)
		item->is_error = 1;
	
	return ret;
}

TOOL_INT32 tool_sock_item_send(tool_sock_item_t* item, TOOL_VOID* buf, TOOL_INT32 n)
{
	if (g_tool_sock_manager == NULL)
		log_fatal("g_tool_sock_manager is NULL");

	if (item->is_error)
		return -1;

	TOOL_INT32 ret = 0;
	TOOL_INT32 startTime = g_tool_sock_manager->tick;
	TOOL_UINT8* pIndex = (TOOL_UINT8*)buf;
	TOOL_UINT8* pStart = (TOOL_UINT8*)buf;
	while (n > 0)
	{
		if (item->flag && *item->flag == 0)
			break;
		
		if (g_tool_sock_manager->tick- startTime >= item->timeout)
		{
			log_error("timeout(%d,%d,%d) n(%d)", g_tool_sock_manager->tick, startTime, item->timeout, n);
			break;
		}

		ret = tool_sysf_write(item->fd, pIndex, n);
		if(ret < 0)
		{
			item->is_error = 1;	
            return -1;
		}

		n -= ret;
		pIndex += ret;
	}

	item->tick = g_tool_sock_manager->tick;
	return (TOOL_INT32)(pIndex - pStart);
}

TOOL_INT32 tool_sock_item_recv(tool_sock_item_t* item, TOOL_VOID* buf, TOOL_INT32 n)
{
	if (g_tool_sock_manager == NULL)
		log_fatal("g_tool_sock_manager is NULL");

	if (item->is_error)
		return -1;

	TOOL_INT32 ret = 0;
	TOOL_UINT8* pIndex = (TOOL_UINT8*)buf;
	TOOL_UINT8* pStart = (TOOL_UINT8*)buf;
	TOOL_INT32 startTime = g_tool_sock_manager->tick;
	while (n > 0)
	{
		if (item->flag && *item->flag == 0)
			break;
		
		if (g_tool_sock_manager->tick - startTime >= item->timeout)
		{
			log_error("time out(%d,%d,%d), sock(%d)", g_tool_sock_manager->tick, startTime, item->timeout, item->fd);
			return -1;
		}

		ret = tool_sysf_read(item->fd, pIndex, n);
		if(ret < 0)
		{
			item->is_error = 1;
			return -1;
		}

		n -= ret;
		pIndex += ret;
	}

	item->tick = g_tool_sock_manager->tick;
	return (TOOL_INT32)(pIndex - pStart);
}

TOOL_INT32 tool_sock_item_recvPeek(tool_sock_item_t* item, TOOL_VOID* buf, TOOL_INT32 n)
{
	if (g_tool_sock_manager == NULL)
		log_fatal("g_tool_sock_manager is NULL");

	if (item->is_error)
		return -1;

	TOOL_INT32 ret = 0;
	TOOL_INT32 startTime = g_tool_sock_manager->tick;
	while (1)
	{
		if (item->flag && *item->flag == 0)
			break;
		
		if (g_tool_sock_manager->tick - startTime >= item->timeout)
		{
			log_error("time out(%d,%d,%d), sock(%d)", g_tool_sock_manager->tick, startTime, item->timeout, item->fd);
			return -1;
		}

		ret = tool_sysf_recvPeek(item->fd, buf, n);
		if (ret < 0)
			return -1;
		if (ret == n)
			break;
		tool_sysf_usleep(20*1000);
	}

	item->tick = g_tool_sock_manager->tick;
	return n;
}

TOOL_INT32 tool_sock_item_recvLine(tool_sock_item_t* item, TOOL_VOID* buf, TOOL_INT32 lineSize)
{
	if (g_tool_sock_manager == NULL)
		log_fatal("g_tool_sock_manager is NULL");

	if (item->is_error)
		return -1;

	TOOL_INT32 ret = 0;
	TOOL_UINT8* pIndex = (TOOL_UINT8*)buf;
	TOOL_UINT8* pStart = (TOOL_UINT8*)buf;
	TOOL_INT32 startTime = g_tool_sock_manager->tick;
	while (1)
	{
		if (item->flag && *item->flag == 0)
			break;
		
		if (g_tool_sock_manager->tick - startTime >= item->timeout)
		{
			log_error("time out(%d,%d,%d), sock(%d)", g_tool_sock_manager->tick, startTime, item->timeout, item->fd);
			return -1;
		}

		ret = tool_sysf_read(item->fd, pIndex, 1);
		if (ret < 0)
		{
			item->is_error = 1;
            return -1;
		}
		else if (ret == 0)
			continue;

		pIndex += ret;
		if (*(pIndex-1) == '\n')
			break;
		if (pIndex-pStart >= lineSize-1)
		{
			log_error("the line is too long");
			return -1;
		}
	}

	item->tick = g_tool_sock_manager->tick;
	return (TOOL_INT32)(pIndex - pStart);
}


TOOL_INT32 tool_sock_item_sendto(tool_sock_item_t* item, TOOL_VOID* buf, TOOL_INT32 n)
{
	if (g_tool_sock_manager == NULL)
		log_fatal("g_tool_sock_manager is NULL");
	return tool_sysf_sendto(item->fd, buf, n, item->remote_addr);
}

TOOL_INT32 tool_sock_item_recvfrom(tool_sock_item_t* item, TOOL_VOID* buf, TOOL_INT32 n)
{
	if (g_tool_sock_manager == NULL)
		log_fatal("g_tool_sock_manager is NULL");
	TOOL_INT8 remote_addr[16] = {0};
	TOOL_INT32 ret = tool_sysf_recvfrom(item->fd, buf, n, remote_addr);
	return ret;
}

TOOL_VOID tool_sock_item_close(tool_sock_item_t* item)
{
	if (g_tool_sock_manager == NULL)
		log_fatal("g_tool_sock_manager is NULL");
	
	if (item->fd > 0)
	{
		if (tool_sysf_closeSock(item->fd) == 0)
		{
			g_tool_sock_manager->count--;
			item->fd = -1;
		}
		item->tick = g_tool_sock_manager->tick;
	}
}

TOOL_INT32 tool_sock_item_getSourceAddr(tool_sock_item_t* item, TOOL_VOID* ip, TOOL_UINT16* port)
{
	return tool_sysf_getsockname(item->fd, ip, port);
}

TOOL_INT32 tool_sock_item_getRemoteAddr(tool_sock_item_t* item, TOOL_VOID* ip, TOOL_UINT16* port)
{
	return tool_sysf_getpeername(item->fd, ip, port);
}

TOOL_INT32 tool_sock_item_ping(TOOL_INT8* remote_ip, TOOL_UINT16 remote_port, TOOL_INT32 timeout)
{
	if (g_tool_sock_manager == NULL)
		tool_sock_init();

	// init
	tool_sock_item_t item;
	tool_mem_memset(&item, sizeof(tool_sock_item_t));
	item.tick = g_tool_sock_manager->tick;
	item.type1 = TOOL_SOCK_TYPE1_TCP;
	item.type2 = TOOL_SOCK_TYPE2_CLIENT;
	item.timeout = timeout;
	if (tool_sysf_initAddr(item.remote_addr, remote_ip, remote_port) < 0)
		return -1;
	log_state("remote-addr(%s:%d)", remote_ip, remote_port);

	// socket
	item.fd = tool_sysf_socket(TOOL_SOCK_TYPE1_TCP);
	TOOL_INT32 ret = tool_sock_item_connect(&item);
	tool_sysf_closeSock(item.fd);
	item.fd = -1;
	return ret;	
}


#if defined __cplusplus
}
#endif
