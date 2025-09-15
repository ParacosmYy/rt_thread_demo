#include <gsthread.h>
#include <gshw.h>


gs_err_t gs_thread_init(struct gs_thread *thread,void(*entry)(void *parameter),void* parameter , void* stack_addr,gs_uint32_t stack_size)
{
    gs_list_init(&thread->tlist);
    
    thread->entry = (void*)entry;
    thread->parameter = parameter;
    
    thread->stack_addr = stack_addr;
    thread->stack_size = stack_size;
    
    thread->sp = (void *)gs_hw_stack_init( thread->entry, 
		                                   thread->parameter,
							               (void *)((char *)thread->stack_addr + thread->stack_size - 4) );
    
    return GS_EOK;
}