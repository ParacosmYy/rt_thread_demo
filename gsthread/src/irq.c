#include <gsthread.h>
#include <gshw.h>

volatile gs_uint8_t gs_interrupt_nest;

void gs_interrupt_enter()
{
    gs_base_t level ;
    level = gs_hw_interrupt_disable();
    gs_interrupt_nest++;//进入中断嵌套加1
    gs_hw_interrupt_enable(level);
    
}

void gs_interrupt_leave()
{
    gs_base_t level ;
    level = gs_hw_interrupt_disable();
    gs_interrupt_nest--;//离开中断嵌套减1
    gs_hw_interrupt_enable(level);
    
}

