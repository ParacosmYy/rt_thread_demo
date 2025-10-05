
#include "stm32f4xx.h"
#include "./usart/bsp_debug_usart.h"
#include <gsthread.h>

gs_uint8_t flag1 ;
gs_uint8_t flag2 ;
gs_uint8_t flag3 ;

struct gs_thread gs_thread_flag1;
struct gs_thread gs_thread_flag2;
struct gs_thread gs_thread_flag3;

extern gs_list_t gs_thread_priority_table[GS_THREAD_PRIORITY_MAX];

ALIGN(GS_ALIGN_SIZE)
/* �����߳�ջ */
gs_uint8_t gs_flag1_thread_stack[512];
gs_uint8_t gs_flag2_thread_stack[512];
gs_uint8_t gs_flag3_thread_stack[512];

/* �߳����� */
void flag1_thread_entry(void *p_arg);
void flag2_thread_entry(void *p_arg);
void flag3_thread_entry(void *p_arg);




int main(void)
{	
    

	gs_thread_init( &gs_thread_flag1,   
                    "thread1vs",
	                flag1_thread_entry,               
	                GS_NULL,                          
	                &gs_flag1_thread_stack[0],        
	                sizeof(gs_flag1_thread_stack),
                    2,
                    4);  
	
	//gs_list_insert_before( &(gs_thread_priority_table[0]),&(gs_thread_flag1.tlist) );
	gs_thread_startup(&gs_thread_flag1);

	gs_thread_init( &gs_thread_flag2, 
                    "thread2vs",
	                flag2_thread_entry,             
	                GS_NULL,                        
	                &gs_flag2_thread_stack[0],      
	                sizeof(gs_flag2_thread_stack) ,
                    4,
                    2);

//  gs_list_insert_before( &(gs_thread_priority_table[1]),&(gs_thread_flag2.tlist) );
	gs_thread_startup(&gs_thread_flag2);

    gs_thread_init( &gs_thread_flag3, 
                    "thread3vs",
	                flag3_thread_entry,             
	                GS_NULL,                        
	                &gs_flag3_thread_stack[0],      
	                sizeof(gs_flag3_thread_stack) ,
                    4,
                    3);                
    gs_thread_startup(&gs_thread_flag3);     
                    
	gs_system_scheduler_start(); 
}

void delay (uint32_t count)
{
	for(; count!=0; count--);
}

/* �߳�1 */
void flag1_thread_entry( void *p_arg )
{
	for( ;; )
	{
		flag1 = 1;
		gs_thread_delay(3);
		flag1 = 0;
		gs_thread_delay(3);
       // gs_schedule();
	}
}

/* �߳�2 */
void flag2_thread_entry( void *p_arg )
{
	for( ;; )
	{
		flag2 = 1;
	//	gs_thread_delay(2);
        delay(100);
		flag2 = 0;
	//	gs_thread_delay(2);
		//gs_schedule();
        delay(100);
	}
}

void flag3_thread_entry( void *p_arg )
{
	for( ;; )
	{
		flag3 = 1;
	//	gs_thread_delay(3);
        delay(100);
		flag3 = 0;
	//	gs_thread_delay(3);
		//gs_schedule();
        delay(100);

	}
}

void SysTick_Handler(void)
{
    /* �����ж� */
    gs_interrupt_enter();

    gs_tick_increase();

    /* �뿪�ж� */
    gs_interrupt_leave();
}










//int main(void)
//{	
//  /*��ʼ��USART ����ģʽΪ 115200 8-N-1���жϽ���*/
//  Debug_USART_Config();
//	
//	/* ����һ���ַ��� */
//	Usart_SendString( DEBUG_USART,"����һ�������жϽ��ջ���ʵ��\n");
//	printf("����һ�������жϽ��ջ���ʵ��\n");
//	
//  while(1)
//	{	
//		
//	}	
//}



/*********************************************END OF FILE**********************/

