#include <gsthread.h>
#include <gshw.h>


static gs_tick_t gs_tick = 0; /* 系统时基计数器 */
extern gs_list_t gs_thread_priority_table[GS_THREAD_PRIORITY_MAX];

void gs_tick_increase(void)
{
    struct gs_thread * thread;
    gs_ubase_t i;
    for(i = 0 ; i < GS_THREAD_PRIORITY_MAX ; i++ )
    {   
        thread = gs_list_entry(gs_thread_priority_table[i].next,struct gs_thread , tlist);
        if(thread->remaining_tick > 0 )
        {
            thread->remaining_tick -- ;
        }
    }
    gs_schedule();
   
}