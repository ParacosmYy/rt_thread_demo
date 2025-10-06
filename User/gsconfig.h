#ifndef __GSTHREAD_CFG_H__
#define __GSTHREAD_CFG_H__


#define GS_THREAD_PRIORITY_MAX  32     /* 最大优先级 */
#define GS_ALIGN_SIZE           4      /* 多少个字节对齐 */ 

#define GS_NAME_MAX	            8      /* 内核对象的名字长度，字符串形式 */

#define GS_TICK_PER_SECOND	    1000

#define GS_USING_SEMAPHORE      1
#define GS_SEM_NUM_MAX			50

#define GS_IPC_FLAG_FIFO 0
#define GS_IPC_FLAG_PRIO 1

#endif /* __RTTHREAD_CFG_H__ */

