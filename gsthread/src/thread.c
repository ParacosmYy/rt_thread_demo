#include <gsthread.h>
#include <gshw.h>
#include <object.h>



extern struct gs_thread *gs_current_thread;
extern gs_uint32_t gs_thread_ready_priority_group ;
extern gs_list_t gs_thread_priority_table[GS_THREAD_PRIORITY_MAX];

gs_err_t gs_thread_init(struct gs_thread *thread,char *name , void(*entry)(void *parameter),void* parameter , void* stack_addr,gs_uint32_t stack_size,gs_uint8_t priority,gs_uint32_t tick)
{
    gs_object_init((gs_object_t)thread,GS_Object_Class_Thread,name);
    gs_list_init(&thread->tlist);
    
    thread->entry = (void*)entry;
    thread->parameter = parameter;
    
    thread->stack_addr = stack_addr;
    thread->stack_size = stack_size;
    
    thread->sp = (void *)gs_hw_stack_init( thread->entry, 
		                                   thread->parameter,
							               (void *)((char *)thread->stack_addr + thread->stack_size - 4) );
    
    thread->current_priority = priority ;
    thread->init_priority = priority ;
    thread->number_mask = 0;
    
    thread->error = GS_EOK;
    thread->stat = GS_THREAD_INIT;
    
    thread->init_tick = tick;
    thread->remaining_tick = tick;
    
    gs_timer_init(&(thread->thread_timer),     
                  thread->name,                
                  gs_thread_timeout,           
                  thread,                      
                  0,                           
                  GS_TIMER_FLAG_ONE_SHOT);     
    
    return GS_EOK;
}

gs_err_t gs_thread_resume(struct gs_thread * thread)
{
    register gs_base_t temp ;
    
    if((thread->stat & GS_THREAD_STAT_MASK) != GS_THREAD_SUSPEND)
    {
        return -GS_ERROR;
    }
    
    temp = gs_hw_interrupt_disable();
    
    gs_list_remove(&(thread->tlist));
    
    gs_hw_interrupt_enable(temp);
    
    gs_schedule_insert_thread(thread);
    
    return GS_EOK ;
}

gs_err_t gs_thread_startup(struct gs_thread * thread)
{
    thread->current_priority = thread->init_priority;
    thread->number_mask = 1L << thread->current_priority ;
    
    thread->stat = GS_THREAD_SUSPEND;
    
    gs_thread_resume(thread);
    
    if(gs_thread_self() != GS_NULL)
    {
        gs_schedule();
    }
    
    return GS_EOK;
}

//void gs_thread_delay(gs_tick_t tick)
//{
//    struct gs_thread *thread;
//    register gs_ubase_t temp;
//    
//    temp = gs_hw_interrupt_disable();
//    
//    thread = gs_current_thread;   
//    thread->remaining_tick = tick ;
//    
//    thread->current_priority = GS_THREAD_SUSPEND;
//    gs_thread_ready_priority_group &= ~thread->number_mask;
//    
//    gs_hw_interrupt_enable(temp);
//    
//    
//    gs_schedule();
//    
//}

gs_err_t gs_thread_delay(gs_tick_t tick)
{
    return gs_thread_sleep(tick);
} 

gs_thread_t gs_thread_self(void)
{
    return gs_current_thread;
}

gs_err_t gs_thread_suspend(gs_thread_t thread)
{
    register gs_base_t temp;


    
    if ((thread->stat & GS_THREAD_STAT_MASK) != GS_THREAD_READY)
    {
        return -GS_ERROR;
    }

   
    temp = gs_hw_interrupt_disable();

   
    thread->stat = GS_THREAD_SUSPEND;

    gs_schedule_remove_thread(thread);


    gs_timer_stop(&(thread->thread_timer));

  
    gs_hw_interrupt_enable(temp);

    return GS_EOK;
}


gs_err_t gs_thread_sleep(gs_tick_t tick)
{
    register gs_base_t temp;
    struct gs_thread *thread;

   
    temp = gs_hw_interrupt_disable();
    
   
    thread = gs_current_thread;

    
    gs_thread_suspend(thread);

    /* 设置线程定时器的超时时间 */
    gs_timer_control(&(thread->thread_timer), GS_TIMER_CTRL_SET_TIME, &tick);
    

    gs_timer_start(&(thread->thread_timer));


    gs_hw_interrupt_enable(temp);

    gs_schedule();

    return GS_EOK;
}


void gs_thread_timeout(void *parameter)
{
    struct gs_thread *thread;

    thread = (struct gs_thread *)parameter;

    /* 设置错误码为超时 */
    thread->error = -GS_ETIMEOUT;

    /* 将线程从挂起列表中删除 */
    gs_list_remove(&(thread->tlist));

    /* 将线程插入到就绪列表 */
    gs_schedule_insert_thread(thread);

    /* 系统调度 */
    gs_schedule();
}

gs_err_t gs_thread_yield(void)
{
    register gs_base_t level;
    struct gs_thread * thread;
    
    level = gs_hw_interrupt_disable();
    
    thread = gs_current_thread;
    
    if((thread->stat & GS_THREAD_STAT_MASK) == GS_THREAD_READY && thread->tlist.next != thread->tlist.prev)
    {
        gs_list_remove(&(thread->tlist));
        
        gs_list_insert_before(&(gs_thread_priority_table[thread->current_priority]),&(thread->tlist));
        
        gs_hw_interrupt_enable(level);
        
        gs_schedule();
        
        return GS_EOK;
    }
    
    gs_hw_interrupt_enable(level);
    
    return GS_EOK;    
}