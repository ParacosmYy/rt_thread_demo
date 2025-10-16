
#include <gsthread.h>
#include <gshw.h>
#include <list.h>


extern struct gs_thread idle;
extern struct gs_thread gs_thread_flag1;
extern struct gs_thread gs_thread_flag2;



struct gs_thread *gs_current_thread;




gs_list_t gs_thread_priority_table[GS_THREAD_PRIORITY_MAX];

gs_uint32_t gs_thread_ready_priority_group;

gs_uint8_t gs_current_thread_priority ;

gs_list_t gs_thread_defunct;


void gs_system_scheduler_init(void)//初始化调度器
{
    register gs_base_t offset ;
    
    for (offset = 0; offset < GS_THREAD_PRIORITY_MAX; offset ++)
	{
			gs_list_init(&gs_thread_priority_table[offset]); //0-31优先级的就绪链表全部初始化为空
	}
    
    gs_current_thread_priority = GS_THREAD_PRIORITY_MAX - 1;
 
    gs_current_thread = GS_NULL;
    
    gs_thread_ready_priority_group = 0 ;
}

void gs_system_scheduler_start(void)
{
    register struct gs_thread* to_thread;
    
//    to_thread = gs_list_entry(gs_thread_priority_table[0].next,struct gs_thread,tlist);
//    
//    gs_current_thread = to_thread;
//    
//    gs_hw_context_switch_first((gs_uint32_t)&to_thread->sp);
    
    register gs_ubase_t gs_thread_ready_priority ;
    
    gs_thread_ready_priority = __gs_ffs(gs_thread_ready_priority_group) ;//找到最高优先级
    
    to_thread = gs_list_entry(gs_thread_priority_table[gs_thread_ready_priority].next,struct gs_thread , tlist);
    
    gs_current_thread = to_thread ;
    
    gs_hw_context_switch_first ((gs_uint32_t)&to_thread->sp);
    
    
}

void gs_schedule_insert_thread(struct gs_thread * thread)
{
    register gs_ubase_t temp ;
    
    temp = gs_hw_interrupt_disable();
    
    thread->stat = GS_THREAD_READY;
    
    gs_list_insert_before(&(gs_thread_priority_table[thread->current_priority]),&(thread->tlist));
    
    gs_thread_ready_priority_group |= thread->number_mask ; //设置就绪优先级位
    
    gs_hw_interrupt_enable(temp);
}

void gs_schedule_remove_thread(struct gs_thread * thread)
{
    register gs_ubase_t temp;
    
    temp = gs_hw_interrupt_disable();
    
    gs_list_remove(&(thread->tlist));//从就绪链表中删除线程
    
    if(gs_list_isempty(&(gs_thread_priority_table[thread->current_priority])))
    {
        gs_thread_ready_priority_group &= ~thread->number_mask; //清除就绪优先级位
    }
    gs_hw_interrupt_enable(temp);
}

void gs_schedule(void)
{
    struct gs_thread *to_thread;
	struct gs_thread *from_thread;
    
//    if( gs_current_thread == gs_list_entry( gs_thread_priority_table[0].next,struct gs_thread,tlist) )
//    {
//        from_thread = gs_current_thread;
//		to_thread = gs_list_entry( gs_thread_priority_table[1].next,
//													               struct gs_thread,
//													               tlist);
//        gs_current_thread = to_thread;
//    }
//    else
//    {
//        from_thread = gs_current_thread;
//		to_thread = gs_list_entry( gs_thread_priority_table[0].next,
//																		 struct gs_thread,
//																		 tlist);
//	    gs_current_thread = to_thread;																		 
//    }
//    gs_hw_context_switch((gs_uint32_t)&from_thread->sp,(gs_uint32_t)&to_thread->sp);	       

//    if(gs_current_thread == &idle)
//    {
//        if(gs_thread_flag1.remaining_tick == 0)
//        {
//            from_thread = gs_current_thread ;
//            to_thread = &gs_thread_flag1 ;
//            gs_current_thread = to_thread;
//        }
//        else if(gs_thread_flag2.remaining_tick == 0)
//        {
//            from_thread = gs_current_thread ;
//            to_thread = &gs_thread_flag2 ;
//            gs_current_thread = to_thread;
//        }
//        else
//        {
//            return ;
//        }
//    }
//    else 
//    {
//        if(gs_current_thread ==  &gs_thread_flag1)
//        {
//            if(gs_thread_flag2.remaining_tick == 0)
//            {
//                from_thread = gs_current_thread;
//                to_thread = &gs_thread_flag2;
//                gs_current_thread = to_thread;
//            }
//            else if(gs_current_thread->remaining_tick != 0)
//            {
//                from_thread = gs_current_thread;
//                to_thread = &idle;
//                gs_current_thread = to_thread;
//            }
//            else
//            {
//                return ;
//            }
//        }
//        else if(gs_current_thread ==  &gs_thread_flag2)
//        {
//            if(gs_thread_flag1.remaining_tick == 0)
//            {
//                from_thread = gs_current_thread;
//                to_thread = &gs_thread_flag1;
//                gs_current_thread = to_thread;
//            }
//            else if(gs_current_thread->remaining_tick != 0)
//            {
//                from_thread = gs_current_thread;
//                to_thread = &idle;
//                gs_current_thread = to_thread;
//            }
//            else
//            {
//                return ;
//            }
//        }
//    }

//-------------------------------------------------------------------------------//
        gs_base_t level ;
        register gs_ubase_t gs_thread_ready_priority ;

        
        level = gs_hw_interrupt_disable();
        
        gs_thread_ready_priority = __gs_ffs(gs_thread_ready_priority_group) ;
        
        to_thread = gs_list_entry(gs_thread_priority_table[gs_thread_ready_priority].next , struct gs_thread , tlist);

        if(to_thread != gs_current_thread)
        {
            from_thread = gs_current_thread ;
            gs_current_thread = to_thread ;
            gs_current_thread_priority =  (gs_uint8_t)gs_thread_ready_priority;//更新当前线程优先级
            gs_hw_context_switch((gs_uint32_t)&from_thread->sp, (gs_uint32_t)&to_thread->sp);
            gs_hw_interrupt_enable(level);
        }
        else
        {
            gs_hw_interrupt_enable(level);
        }
        

}