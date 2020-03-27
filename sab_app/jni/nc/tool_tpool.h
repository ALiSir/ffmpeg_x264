
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
    TOOL_FUNC			func;       			/* 任务函数 */
    TOOL_VOID*			arg;       			/* 传入任务函数的参数 */
    struct tpool_work*	next;                    
}tpool_work_t;

typedef struct 
{
    TOOL_INT32			state;       			/* 线程池是否销毁 */
	TOOL_THREAD			thd[TPOOL_SIZE];		/* 线程ID数组 */
	TOOL_INT32 			thd_num;		
	TOOL_INT32			active_num;
    tpool_work_t*		work_head;     			/* 线程链表 */
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

