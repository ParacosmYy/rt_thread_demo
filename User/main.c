#include "ARMCM4.h"                     // Device header
#include <gsthread.h>


gs_uint8_t flag1 ;
gs_uint8_t flag2 ;


struct gs_thread gs_thread_flag1;
struct gs_thread gs_thread_flag2;

extern gs_list_t gs_thread_priority_table[GS_THREAD_PRIORITY_MAX];

ALIGN(GS_ALIGN_SIZE)
/* 定义线程栈 */
gs_uint8_t gs_flag1_thread_stack[512];
gs_uint8_t gs_flag2_thread_stack[512];

/* 线程声明 */
void flag1_thread_entry(void *p_arg);
void flag2_thread_entry(void *p_arg);





int main(void)
{	
	gs_system_scheduler_init();
	
	

	gs_thread_init( &gs_thread_flag1,                 
	                flag1_thread_entry,               
	                GS_NULL,                          
	                &gs_flag1_thread_stack[0],        
	                sizeof(gs_flag1_thread_stack) );  
	
	gs_list_insert_before( &(gs_thread_priority_table[0]),&(gs_thread_flag1.tlist) );
	

	gs_thread_init( &gs_thread_flag2,               
	                flag2_thread_entry,             
	                GS_NULL,                        
	                &gs_flag2_thread_stack[0],      
	                sizeof(gs_flag2_thread_stack) );

	gs_list_insert_before( &(gs_thread_priority_table[1]),&(gs_thread_flag2.tlist) );
	

	gs_system_scheduler_start(); 
}

void delay (uint32_t count)
{
	for(; count!=0; count--);
}

/* 线程1 */
void flag1_thread_entry( void *p_arg )
{
	for( ;; )
	{
		flag1 = 1;
		delay( 100 );		
		flag1 = 0;
		delay( 100 );

	}
}

/* 线程2 */
void flag2_thread_entry( void *p_arg )
{
	for( ;; )
	{
		flag2 = 1;
		delay( 100 );		
		flag2 = 0;
		delay( 100 );
		

	}
}
