
#if defined __cplusplus
extern "C"
{
#endif

#ifndef __TOOL_TPOOL_H__
#define __TOOL_TPOOL_H__

#include "tool_type.h"

#define TPOOL_SIZE			100
#define TPOOL_WORK_SIZE		10000

typedef struct tpool_work
{
    TOOL_FUNC			func;       			/* ������ */
    TOOL_VOID*			arg;       			/* �����������Ĳ��� */
    struct tpool_work*	next;                    
}tpool_work_t;

typedef struct 
{
    TOOL_INT32			state;       			/* �̳߳��Ƿ����� */
	TOOL_THREAD			thd[TPOOL_SIZE];		/* �߳�ID���� */
	TOOL_INT32 			thd_num;		
	TOOL_INT32			active_num;
    tpool_work_t*		work_head;     			/* �߳����� */
	TOOL_INT32			work_num;
	TOOL_INT32 			work_size;
    TOOL_MUTEX			mutex;                     
    TOOL_COND			cond;    
}tpool_t;



TOOL_VOID tpool_start(tpool_t* tpool, TOOL_INT32 thd_num, TOOL_INT32 work_size);
TOOL_VOID tpool_stop(tpool_t* tpool);
TOOL_INT32 tpool_addWork(tpool_t* tpool, TOOL_FUNC func, TOOL_VOID *arg);

#endif

#if defined __cplusplus
}
#endif

