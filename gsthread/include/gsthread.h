#ifndef __GS_THREAD_H__
#define __GS_THREAD_H__


#include <gsdef.h>
#include <gsconfig.h>
#include <gsservice.h>
#include <list.h>


gs_err_t gs_thread_init(struct gs_thread *thread,char* name ,void(*entry)(void *parameter),void* parameter , void* stack_addr,gs_uint32_t stack_size);

void gs_system_scheduler_init(void);
void gs_system_scheduler_start(void);
void gs_schedule(void);

#endif /* __RT_THREAD_H__ */

