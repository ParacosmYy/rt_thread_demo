#include <gsthread.h>
#include <gshw.h>
#include "./led/bsp_led.h"   


void gs_thread_all_init()
{
     /* ���ж� */
    gs_hw_interrupt_disable();
    
    /* SysTick�ж�Ƶ������ */
    SysTick_Config( SystemCoreClock / GS_TICK_PER_SECOND );
	
    gs_system_timer_init();
    
	gs_system_scheduler_init();
    
    gs_thread_idle_init();

	LED_GPIO_Config();
}