
#include <gsthread.h>
#include <gshw.h>

extern struct gs_thread idle;
extern struct gs_thread gs_thread_flag1;
extern struct gs_thread gs_thread_flag2;



struct gs_thread *gs_current_thread;




gs_list_t gs_thread_priority_table[GS_THREAD_PRIORITY_MAX];

gs_list_t gs_thread_defunct;


void gs_system_scheduler_init(void)
{
    register gs_base_t offset ;
    
    for (offset = 0; offset < GS_THREAD_PRIORITY_MAX; offset ++)
	{
			gs_list_init(&gs_thread_priority_table[offset]);
	}
    
    gs_current_thread = GS_NULL;
    
}

void gs_system_scheduler_start(void)
{
    register struct gs_thread* to_thread;
    
    to_thread = gs_list_entry(gs_thread_priority_table[0].next,struct gs_thread,tlist);
    
    gs_current_thread = to_thread;
    
    gs_hw_context_switch_first((gs_uint32_t)&to_thread->sp);
    
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

    if(gs_current_thread == &idle)
    {
        if(gs_thread_flag1.remaining_tick == 0)
        {
            from_thread = gs_current_thread ;
            to_thread = &gs_thread_flag1 ;
            gs_current_thread = to_thread;
        }
        else if(gs_thread_flag2.remaining_tick == 0)
        {
            from_thread = gs_current_thread ;
            to_thread = &gs_thread_flag2 ;
            gs_current_thread = to_thread;
        }
        else
        {
            return ;
        }
    }
    else 
    {
        if(gs_current_thread ==  &gs_thread_flag1)
        {
            if(gs_thread_flag2.remaining_tick == 0)
            {
                from_thread = gs_current_thread;
                to_thread = &gs_thread_flag2;
                gs_current_thread = to_thread;
            }
            else if(gs_current_thread->remaining_tick != 0)
            {
                from_thread = gs_current_thread;
                to_thread = &idle;
                gs_current_thread = to_thread;
            }
            else
            {
                return ;
            }
        }
        else if(gs_current_thread ==  &gs_thread_flag2)
        {
            if(gs_thread_flag1.remaining_tick == 0)
            {
                from_thread = gs_current_thread;
                to_thread = &gs_thread_flag1;
                gs_current_thread = to_thread;
            }
            else if(gs_current_thread->remaining_tick != 0)
            {
                from_thread = gs_current_thread;
                to_thread = &idle;
                gs_current_thread = to_thread;
            }
            else
            {
                return ;
            }
        }
    }
    	gs_hw_context_switch((gs_uint32_t)&from_thread->sp,(gs_uint32_t)&to_thread->sp);	

}