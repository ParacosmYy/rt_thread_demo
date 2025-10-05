#ifndef __GS_THREAD_H__
#define __GS_THREAD_H__


#include <gsdef.h>
#include <gsconfig.h>
#include <gsservice.h>
#include <list.h>
//-------------------------线程启动初始化接口----------------------


//-------------------------线程接口--------------------------------
gs_err_t gs_thread_init(struct gs_thread *thread,char *name , void(*entry)(void *parameter),void* parameter , void* stack_addr,gs_uint32_t stack_size,gs_uint8_t priority,gs_uint32_t tick);
gs_err_t gs_thread_resume(struct gs_thread * thread);
gs_thread_t gs_thread_self(void);
gs_err_t gs_thread_delay(gs_tick_t tick);
gs_err_t gs_thread_startup(struct gs_thread * thread);

void gs_thread_timeout(void *parameter);
gs_err_t gs_thread_suspend(gs_thread_t thread);
gs_err_t gs_thread_sleep(gs_tick_t tick);
//-------------------------调度器接口------------------------------
void gs_system_scheduler_init(void);
void gs_system_scheduler_start(void);
void gs_schedule(void);
void gs_schedule_insert_thread(struct gs_thread * thread);
void gs_schedule_remove_thread(struct gs_thread * thread);
void gs_system_scheduler_start(void);


//-------------------------时钟相关接口-----------------------------
void gs_tick_increase(void);
gs_tick_t gs_tick_get(void);

void gs_system_timer_init(void);   
//------------------------定时器相关接口-----------------------------
void gs_timer_check(void);
void gs_timer_init(gs_timer_t  timer,
                   const char *name,
                   void (*timeout)(void *parameter),
                   void       *parameter,
                   gs_tick_t   time,
                   gs_uint8_t  flag);
gs_err_t gs_timer_start(gs_timer_t timer);
gs_err_t gs_timer_stop(gs_timer_t timer);
gs_inline void _gs_timer_remove(gs_timer_t timer);
                   
static void _gs_timer_init(gs_timer_t timer,
                           void (*timeout)(void *parameter),
                           void      *parameter,
                           gs_tick_t  time,
                           gs_uint8_t flag);                   
                   
//-------------------------空闲线程接口------------------------------
void gs_thread_idle_init(void);	

//-------------------------中断服务接口------------------------------

void gs_interrupt_enter(void);
void gs_interrupt_leave(void);
                   
gs_base_t gs_hw_interrupt_disable(void);
void gs_hw_interrupt_enable(gs_base_t level);
                   

//-------------------------内核服务接口------------------------------

char * gs_strncpy(char *des , const char *src , gs_ubase_t n);
int __gs_ffs(int value);

//-------------------------内核初始化接口-----------------------------

struct gs_object_information * gs_object_get_infomation(enum gs_object_class_type type);
void gs_object_init(struct gs_object *object , enum gs_object_class_type type , const char *name);    
#endif 

