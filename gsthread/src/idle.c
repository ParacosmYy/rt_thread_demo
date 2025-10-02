#include <gsthread.h>
#include <gshw.h>

#define IDLE_THREAD_STACK_SIZE      512 

ALIGN(GS_ALIGN_SIZE)
static gs_uint8_t gs_thread_stack[IDLE_THREAD_STACK_SIZE];

struct gs_thread idle ;

gs_ubase_t gs_idletask_ctr  =  0;

extern gs_list_t gs_thread_priority_table[GS_THREAD_PRIORITY_MAX];



void gs_thread_idle_entry(void *parameter)
{
    parameter =parameter ;
    while(1)
    {
         gs_idletask_ctr ++;
    }
}

void gs_thread_idle_init()
{
    gs_thread_init(&idle, "idle", gs_thread_idle_entry,GS_NULL,&gs_thread_stack[0],sizeof(gs_thread_stack) , GS_THREAD_PRIORITY_MAX - 1 ,2);
    
   // gs_list_insert_before(&gs_thread_priority_table[GS_THREAD_PRIORITY_MAX-1],&(idle.tlist));
    
    gs_thread_startup(&idle);
    
}

