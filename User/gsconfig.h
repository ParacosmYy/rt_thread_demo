#ifndef __GSTHREAD_CFG_H__
#define __GSTHREAD_CFG_H__


#define GS_THREAD_PRIORITY_MAX  32     /* ������ȼ� */
#define GS_THREAD_NUM_MAX       100
#define GS_ALIGN_SIZE           4      /* ���ٸ��ֽڶ��� */ 

#define GS_NAME_MAX	            15      /* �ں˶�������ֳ��ȣ��ַ�����ʽ */

#define GS_TICK_PER_SECOND	    1000

#define GS_USING_SEMAPHORE      1
#define GS_SEM_NUM_MAX			50

#define GS_IPC_FLAG_FIFO        0
#define GS_IPC_FLAG_PRIO        1

#define GS_USING_MUTEX         		  1
#define GS_MUTEX_NUM_MAX			  50

#define GS_CMD_NUM_MAX              200

#define GS_SHELL_STACK_SIZE     1000
#define GS_SHELL_PRIORITY       10 

#define GS_SHELL_LENGSH  50//��������󳤶�
#define GS_SHELL_HISORY_MAX 30//��ʷ�����������

#endif /* __RTTHREAD_CFG_H__ */

