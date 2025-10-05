#include <gsthread.h>

/* 用于存储上一个线程的栈的sp的指针 */
gs_uint32_t gs_interrupt_from_thread;

/* 用于存储下一个将要运行的线程的栈的sp的指针 */
gs_uint32_t gs_interrupt_to_thread;

/* PendSV中断服务函数执行标志 */
gs_uint32_t gs_thread_switch_interrupt_flag;



struct exception_stack_frame
{
    /* 异常发生时自动保存的寄存器 */
	gs_uint32_t r0;
    gs_uint32_t r1;
    gs_uint32_t r2;
    gs_uint32_t r3;
    gs_uint32_t r12;
    gs_uint32_t lr;
    gs_uint32_t pc;
    gs_uint32_t psr;
};

struct stack_frame
{
    /* r4 ~ r11 register 
	  异常发生时需手动保存的寄存器 */
    gs_uint32_t r4;
    gs_uint32_t r5;
    gs_uint32_t r6;
    gs_uint32_t r7;
    gs_uint32_t r8;
    gs_uint32_t r9;
    gs_uint32_t r10;
    gs_uint32_t r11;

    struct exception_stack_frame exception_stack_frame;
};

gs_uint8_t * gs_hw_stack_init(void *tentry, void *parameter , gs_uint8_t* stack_addr)
{
    struct stack_frame *stack_frame ;
    gs_uint8_t *stk;
    int i ;
    
    stk = stack_addr+sizeof(gs_uint32_t);
    
    stk = (gs_uint8_t *)GS_ALIGN_DOWN((gs_uint32_t)stk,8);
    stk = stk - sizeof(struct stack_frame);
    
    stack_frame = (struct stack_frame *)stk;
    
    for (i = 0; i < sizeof(struct stack_frame) / sizeof(gs_uint32_t); i ++)
    {
        ((gs_uint32_t *)stack_frame)[i] = 0xdeadbeef;
    }
    
	stack_frame->exception_stack_frame.r0  = (unsigned long)parameter; /* r0 : argument */
	stack_frame->exception_stack_frame.r1  = 0;                        /* r1 */
	stack_frame->exception_stack_frame.r2  = 0;                        /* r2 */
	stack_frame->exception_stack_frame.r3  = 0;                        /* r3 */
	stack_frame->exception_stack_frame.r12 = 0;                        /* r12 */
	stack_frame->exception_stack_frame.lr  = 0;                        /* lr */
	stack_frame->exception_stack_frame.pc  = (unsigned long)tentry;    /* entry point, pc */
	stack_frame->exception_stack_frame.psr = 0x01000000L;              /* PSR */
	
	
	return stk;
}