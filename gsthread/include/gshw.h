#ifndef __GS_HW_H__
#define __GS_HW_H__

#include <gsthread.h>
gs_uint8_t *gs_hw_stack_init(void       *tentry,
                             void       *parameter,
                             gs_uint8_t *stack_addr);
														 


void gs_hw_context_switch(gs_uint32_t from, gs_uint32_t to);
void gs_hw_context_switch_first(gs_uint32_t to);
void gs_hw_context_switch_interrupt(gs_uint32_t from, gs_uint32_t to);

														 
#endif 



