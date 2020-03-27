
#if defined __cplusplus
extern "C"
{
#endif

#include "tool_log.h"
#include "tool_sysf.h"
#include "tool_type.h"
#include "tool_tpool.h"

TOOL_VOID* tpool_run(TOOL_VOID* param)
{
	tpool_t* tpool = (tpool_t*)param;
    tpool_work_t* work = NULL;
    while(1)
	{
        /* 如果线程池没有被销毁且没有任务要执行，则等待 */
        tool_thread_lockMutex(&tpool->mutex);
        while (!tpool->work_head && tpool->state) 
		{
            tool_thread_waitCond(&tpool->cond, &tpool->mutex);
        }
        if (tpool->state == 0)
		{
            tool_thread_unlockMutex(&tpool->mutex);
            break;
        }
        work = tpool->work_head;
        tpool->work_head = tpool->work_head->next;		
		tpool->work_num --;
        tool_thread_unlockMutex(&tpool->mutex);

		tpool->active_num ++;
        work->func(work->arg);
		tpool->active_num --;

        tool_mem_free(work);
    }
    return NULL;   	
}

TOOL_VOID tpool_start(tpool_t* tpool, TOOL_INT32 thd_num, TOOL_INT32 work_size)
{
	if (tpool == NULL || thd_num < 0 || thd_num > TPOOL_SIZE || work_size < 0 || work_size > TPOOL_WORK_SIZE)
		log_fatal("tpool(%d) thd_num(%d) work_size(%d)", tpool, thd_num, work_size);

	if (tpool->state)
	{
		log_error("tpool_start already start");
		return;
	}

	tool_mem_memset(tpool, sizeof(tpool_t));
	tool_thread_initMutex(&tpool->mutex);
	tool_thread_initCond(&tpool->cond);
	tpool->state = 1;
	tpool->thd_num = thd_num;
	tpool->work_size = work_size;
    TOOL_INT32 i = 0;
    for (i = 0; i < thd_num; i++)
	{
		if (tool_thread_create(&tpool->thd[i], NULL, tpool_run, (TOOL_VOID*)tpool) < 0)
			log_fatal("tool_thread_create");
    }    
	log_state("tpool start");
}

TOOL_VOID tpool_stop(tpool_t* tpool)
{
	if (tpool == NULL)
		log_fatal("tpool(%d)", tpool);

	if (tpool->state == 0)
	{
		log_error("tpool already stop");
		return ;
	}
    tpool->state = 0;
    tool_thread_lockMutex(&tpool->mutex);
    tool_thread_broadcastCond(&tpool->cond);
    tool_thread_unlockMutex(&tpool->mutex);
    TOOL_INT32 i = 0;
    for (i = 0; i < tpool->thd_num; i++) 
	{
        tool_thread_join(tpool->thd[i]);
    }
 	tpool_work_t* work = NULL;
    while (tpool->work_head)
	{
        work = tpool->work_head;
        tpool->work_head = tpool->work_head->next;
        tool_mem_free(work);
    }
    tool_thread_doneMutex(&tpool->mutex);    
    tool_thread_doneCond(&tpool->cond);
}

TOOL_INT32 tpool_addWork(tpool_t* tpool, TOOL_FUNC func, TOOL_VOID *arg)
{
	if (tpool == NULL || func == NULL)
		log_fatal("tpool(%d) func(%d)", tpool, func);
	
    tool_thread_lockMutex(&tpool->mutex); 
	if (tpool->work_num >= tpool->work_size)
	{
		log_error("tpool_addWork");
		tool_thread_unlockMutex(&tpool->mutex);
		return -1;
	}

    tpool_work_t* work = (tpool_work_t *)tool_mem_malloc(sizeof(tpool_work_t), 1);
    work->func = func;
    work->arg = arg; 
	tpool_work_t* index = NULL;
	index = tpool->work_head;
    if (index == NULL) 
	{
        tpool->work_head = work;
    } 
	else 
	{
        while (index->next)
		{
            index = index->next;
        }
        index->next = work;
    }
	tpool->work_num ++;
    tool_thread_signalCond(&tpool->cond);
    tool_thread_unlockMutex(&tpool->mutex);
	return 0;
}

#if defined __cplusplus
}
#endif



