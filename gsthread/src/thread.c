#include <gsthread.h>
#include <gshw.h>
#include <object.h>



extern struct gs_thread *gs_current_thread;
extern gs_uint32_t gs_thread_ready_priority_group ;
extern gs_list_t gs_thread_priority_table[GS_THREAD_PRIORITY_MAX];

extern gs_thread_t thread_array[GS_THREAD_NUM_MAX];
extern gs_size_t thread_cnt;

gs_err_t gs_thread_init(struct gs_thread *thread,char *name , void(*entry)(void *parameter),void* parameter , void* stack_addr,gs_uint32_t stack_size,gs_uint8_t priority,gs_uint32_t tick)
{
    gs_object_init((gs_object_t)thread, GS_Object_Class_Thread,name);
    gs_list_init(&thread->tlist);
    
    thread_array[thread_cnt]=thread; //保存到线程数组中
	thread_cnt++;//记录线程数量
    
    thread->entry = (void*)entry;
    thread->parameter = parameter;
    
    thread->stack_addr = stack_addr;
    thread->stack_size = stack_size;
    
    thread->sp = (void *)gs_hw_stack_init( thread->entry, 
		                                   thread->parameter,
							               (void *)((char *)thread->stack_addr + thread->stack_size - 4) );
    
    thread->current_priority = priority ;
    thread->init_priority = priority ;
    thread->number_mask = 0;//线程优先级掩码
    
    thread->error = GS_EOK;
    thread->stat = GS_THREAD_INIT; //初始状态为初始化态
    
    thread->init_tick = tick;//线程初始延时滴答数
    thread->remaining_tick = tick;//线程剩余延时滴答数
    
    gs_timer_init(&(thread->thread_timer),     
                  thread->name,                
                  gs_thread_timeout,           
                  thread,                      
                  0,                           
                  GS_TIMER_FLAG_ONE_SHOT);    //线程定时器初始化 
    
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
    thread->current_priority = thread->init_priority;//设置当前优先级为初始优先级
    thread->number_mask = 1L << thread->current_priority ;//设置线程优先级掩码
    
    thread->stat = GS_THREAD_SUSPEND;
    
    gs_thread_resume(thread);
    
    if(gs_thread_self() != GS_NULL) // 不是主线程
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

void gs_thread_delay_ms(gs_tick_t ms)
{
    gs_thread_delay(ms);
}

void gs_thread_delay_s(gs_tick_t s)
{
    gs_thread_delay(s*1000);
}

gs_thread_t gs_thread_self(void)
{
    return gs_current_thread;
}

gs_err_t gs_thread_suspend(gs_thread_t thread)
{
    register gs_base_t temp;


    
    if ((thread->stat & GS_THREAD_STAT_MASK) != GS_THREAD_READY) //就绪态则挂起
    {
        return -GS_ERROR;
    }

   
    temp = gs_hw_interrupt_disable();

   
    thread->stat = GS_THREAD_SUSPEND;

    gs_schedule_remove_thread(thread);//从就绪链表中删除 然后位掩码置0


    gs_timer_stop(&(thread->thread_timer));//停止线程定时器

  
    gs_hw_interrupt_enable(temp);

    return GS_EOK;
}


gs_err_t gs_thread_sleep(gs_tick_t tick)//线程休眠函数
{
    register gs_base_t temp;
    struct gs_thread *thread;

   
    temp = gs_hw_interrupt_disable();
    
   
    thread = gs_current_thread;

    
    gs_thread_suspend(thread); //挂起当前线程

    gs_timer_control(&(thread->thread_timer), GS_TIMER_CTRL_SET_TIME, &tick);
    

    gs_timer_start(&(thread->thread_timer));


    gs_hw_interrupt_enable(temp);

    gs_schedule();

    return GS_EOK;
}


void gs_thread_timeout(void *parameter) //线程定时器进行调用
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
    
    if((thread->stat & GS_THREAD_STAT_MASK) == GS_THREAD_READY && thread->tlist.next != thread->tlist.prev) //就绪态且就绪链表中不止一个线程
    {
        gs_list_remove(&(thread->tlist)); //从就绪链表中删除当前线程
        
        gs_list_insert_before(&(gs_thread_priority_table[thread->current_priority]),&(thread->tlist));//从当前优先级队列删除后插入到当前优先级队列的队尾
        
        gs_hw_interrupt_enable(level);
        
        gs_schedule();
        
        return GS_EOK;
    }
    
    gs_hw_interrupt_enable(level);
    
    return GS_EOK;    
}

gs_err_t  gs_thread_control(gs_thread_t thread , int cmd , void *arg)
{
    register gs_base_t temp ;
    
    switch(cmd)
    {
        case GS_THREAD_CTRL_CHANGE_PRIORITY :
             temp = gs_hw_interrupt_disable();
        
        if(thread->stat & GS_THREAD_STAT_MASK == GS_THREAD_READY) //就绪态
        {
            gs_schedule_remove_thread(thread); //从就绪链表中删除
            
            thread->current_priority = *(gs_uint8_t *)arg ;
            
            thread->number_mask = 1<<thread->current_priority; //设置新的优先级掩码
            
            gs_schedule_insert_thread(thread); 
        }
        else //非就绪态直接改变线程的优先级
        {
            thread->current_priority = *(gs_uint8_t *)arg ;
            
            thread->number_mask = 1<<thread->current_priority;
        }
        gs_hw_interrupt_enable(temp);
        
        break ;
    }
    return GS_EOK ;
}