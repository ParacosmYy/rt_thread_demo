#include <gsthread.h>
#include <gshw.h>


static gs_tick_t gs_tick = 0; /* 系统时基计数器 */
extern gs_list_t gs_thread_priority_table[GS_THREAD_PRIORITY_MAX];
extern gs_uint32_t gs_thread_ready_priority_group ;


//void gs_tick_increase(void)
//{
//    struct gs_thread * thread;
//    gs_ubase_t i;
//    gs_tick++;
//    for(i = 0 ; i < GS_THREAD_PRIORITY_MAX ; i++ )
//    {   
//        thread = gs_list_entry(gs_thread_priority_table[i].next,struct gs_thread , tlist);
//        if(thread->remaining_tick > 0 )
//        {
//            thread->remaining_tick -- ;
//            if(thread->remaining_tick == 0)
//            {
//                gs_thread_ready_priority_group |= thread->number_mask ;
//            }
//        }
//    }
//    gs_schedule();
//   
//}

void gs_tick_increase(void)
{
    struct gs_thread * thread;
    
    ++ gs_tick;
    
    thread = gs_thread_self();
    
    --thread->remaining_tick;

    if(thread->remaining_tick == 0)
    {
        thread->remaining_tick = thread->init_tick;
        gs_thread_yield();
    }
    gs_timer_check();
}


gs_tick_t gs_tick_get(void)
{

    return gs_tick;
}

