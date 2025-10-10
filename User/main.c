
#include "stm32f4xx.h"
#include "./usart/bsp_debug_usart.h"
#include <gsthread.h>

gs_uint8_t flag1 ;
gs_uint8_t flag2 ;
gs_uint8_t flag3 ;

struct gs_thread gs_thread_flag1;
struct gs_thread gs_thread_flag2;
struct gs_thread gs_thread_flag3;
struct gs_thread gs_thread_flag4;
struct gs_thread gs_thread_flag5;
struct gs_thread gs_thread_flag6;

extern gs_list_t gs_thread_priority_table[GS_THREAD_PRIORITY_MAX];

ALIGN(GS_ALIGN_SIZE)

gs_uint8_t gs_flag1_thread_stack[128];
gs_uint8_t gs_flag2_thread_stack[128];
gs_uint8_t gs_flag3_thread_stack[128];
gs_uint8_t gs_flag4_thread_stack[128];
gs_uint8_t gs_flag5_thread_stack[128];
gs_uint8_t gs_flag6_thread_stack[128];

void flag1_thread_entry(void *p_arg);
void flag2_thread_entry(void *p_arg);
void flag3_thread_entry(void *p_arg);
void flag4_thread_entry(void *p_arg);
void flag5_thread_entry(void *p_arg);
void flag6_thread_entry(void *p_arg);

static gs_sem_t sem = GS_NULL ;
static gs_mutex_t mutex = GS_NULL ;

gs_err_t sem_take , mutex_take;
gs_uint8_t shared_count = 0 ;


int main(void)
{	
    gs_thread_all_init();

    sem = gs_sem_create("GS_SEM",1,GS_IPC_FLAG_FIFO) ;
    
    mutex = gs_mutex_create("GS_mutex",GS_IPC_FLAG_FIFO);
    
    //test = gs_ipc_list_resume(gs_thread_priority_table[3].next);
    
	gs_thread_init( &gs_thread_flag1,   
                    "led_red",
	                flag1_thread_entry,               
	                GS_NULL,                          
	                &gs_flag1_thread_stack[0],        
	                sizeof(gs_flag1_thread_stack),
                    2,
                    4);  
	
	//gs_list_insert_before( &(gs_thread_priority_table[0]),&(gs_thread_flag1.tlist) );
	gs_thread_startup(&gs_thread_flag1);

	gs_thread_init( &gs_thread_flag2, 
                    "led_green",
	                flag2_thread_entry,             
	                GS_NULL,                        
	                &gs_flag2_thread_stack[0],      
	                sizeof(gs_flag2_thread_stack) ,
                    4,
                    2);

//  gs_list_insert_before( &(gs_thread_priority_table[1]),&(gs_thread_flag2.tlist) );
	gs_thread_startup(&gs_thread_flag2);

    gs_thread_init( &gs_thread_flag3, 
                    "led_yellow",
	                flag3_thread_entry,             
	                GS_NULL,                        
	                &gs_flag3_thread_stack[0],      
	                sizeof(gs_flag3_thread_stack) ,
                    4,
                    3);                
    gs_thread_startup(&gs_thread_flag3);     
       
    gs_thread_init( &gs_thread_flag4, 
                    "SEM",
	                flag4_thread_entry,             
	                GS_NULL,                        
	                &gs_flag4_thread_stack[0],      
	                sizeof(gs_flag4_thread_stack) ,
                    5,
                    3);                
    gs_thread_startup(&gs_thread_flag4);     
     
    gs_thread_init( &gs_thread_flag5, 
                    "MUTEX_DRV",
                    flag5_thread_entry,             
                    GS_NULL,                        
                    &gs_flag5_thread_stack[0],      
                    sizeof(gs_flag5_thread_stack) ,
                    6,
                    3);                
    gs_thread_startup(&gs_thread_flag5);

    gs_thread_init( &gs_thread_flag6, 
                    "MUTEX_TST",
                    flag6_thread_entry,             
                    GS_NULL,                        
                    &gs_flag6_thread_stack[0],      
                    sizeof(gs_flag6_thread_stack) ,
                    7,
                    3);                
    gs_thread_startup(&gs_thread_flag6);     
                           
                 
                    
                        
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
		LED1_TOGGLE ;
		gs_thread_delay(1000);
		
	}
}

/* �߳�2 */
void flag2_thread_entry( void *p_arg )
{
	for( ;; )
	{
		LED2_TOGGLE ;
		gs_thread_delay_ms(1000);
	}
}

void flag3_thread_entry( void *p_arg )
{
	for( ;; )
	{
		LED3_TOGGLE ;
		gs_thread_delay(1000);

	}
}

void flag5_thread_entry( void *p_arg )
{
     int step;

    while(1)
    {
        gs_thread_delay(500);
//        printf("flag5: �ȴ�������...\n");
//        mutex_take = gs_mutex_take(mutex, -1);
//        if (mutex_take == GS_EOK)
//        {
//            printf("flag5: �����ٽ���\n");
//          
//            for (step = 0; step < 5; step++)
//            {
//                shared_count++;
//                printf("flag5: shared_count=%u (step %d)\n", shared_count, step + 1);
//                gs_thread_delay(50);
//            }

//            mutex_take = gs_mutex_release(mutex);
//            if (mutex_take != GS_EOK)
//            {
//                printf("flag5: �ͷŻ�����ʧ�ܣ�������=%d\n", mutex_take);
//            }
//            else
//            {
//                printf("flag5: ���ͷŻ�����\n");
//            }
//        }
//        else
//        {
//            printf("flag5: ��ȡ������ʧ�ܣ�������=%d\n", mutex_take);
//        }

//        gs_thread_delay(200);
    }
}

void flag6_thread_entry( void *p_arg )
{

//    gs_err_t release_ret;
    while(1)
    {
//        mutex_take = gs_mutex_take(mutex, 300);
//        if (mutex_take == GS_EOK)
//        {
//            shared_count++;
//            printf("flag6: �������������������=%u\n", shared_count);
//            gs_thread_delay(20);

//            release_ret = gs_mutex_release(mutex);
//            if (release_ret != GS_EOK)
//            {
//                printf("flag6: �ͷŻ�����ʧ�ܣ�������=%d\n", release_ret);
//            }
//            else
//            {
//                printf("flag6: ���ͷŻ�����\n");
//            }
//        }
//        else if (mutex_take == -GS_ETIMEOUT)
//        {
//            printf("flag6: �ȴ���������ʱ\n");
//        }
//        else
//        {
//            printf("flag6: ��ȡ���������ִ��󣬴�����=%d\n", mutex_take);
//        }

//        gs_thread_delay(100);
    gs_thread_delay(500);
    }
}

void flag4_thread_entry( void *p_arg )
{
    
    while(1)
    {
//        sem_take = gs_sem_take(sem,20);
//        if(sem_take ==GS_EOK)
//        {
//           printf("sem��ȡ�ɹ�\n"); 
//        }
//        else if(sem_take == -GS_ETIMEOUT)
//        {
//            printf("sem��ȡʧ�ܣ��Ѿ���ʱ\n");
//            sem= gs_sem_create("GS_SEM",1,GS_IPC_FLAG_FIFO);
//        }
//        gs_thread_delay(500);
       gs_thread_delay(500); 
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

