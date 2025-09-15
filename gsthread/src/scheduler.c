
#include <gsthread.h>
#include <gshw.h>

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
    
    if( gs_current_thread == gs_list_entry( gs_thread_priority_table[0].next,struct gs_thread,tlist) )
    {
        from_thread = gs_current_thread;
		to_thread = gs_list_entry( gs_thread_priority_table[1].next,
													               struct gs_thread,
													               tlist);
        gs_current_thread = to_thread;
    }
    else
    {
        from_thread = gs_current_thread;
		to_thread = gs_list_entry( gs_thread_priority_table[0].next,
																		 struct gs_thread,
																		 tlist);
	    gs_current_thread = to_thread;																		 
    }
    gs_hw_context_switch((gs_uint32_t)&from_thread->sp,(gs_uint32_t)&to_thread->sp);	                                                                   
}