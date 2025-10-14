#include "gsthread.h"
#include "./usart/bsp_debug_usart.h"

gs_thread_t thread_array[GS_THREAD_NUM_MAX];
gs_size_t thread_cnt;

void cmd_version(void * parameter)
{
    show_version();
}

void cmd_clear(void * parameter)
{
    printf("                                      \r\n");
    printf("                                      \r\n");
    printf("                                      \r\n");
    printf("                                      \r\n");
    printf("                                      \r\n");
    printf("                                      \r\n");
    printf("                                      \r\n");
    printf("                                      \r\n");
    printf("                                      \r\n");
    printf("                                      \r\n");
    printf("                                      \r\n");
    printf("                                      \r\n");
    printf("                                      \r\n");
    printf("                                      \r\n");
    printf("                                      \r\n");
    printf("                                      \r\n");
    printf("                                      \r\n");
    printf("                                      \r\n");
    printf("                                      \r\n");
    printf("                                      \r\n");
}

void cmd_ps(void * parameter)
{
    int i ;
    struct gs_thread * thread;
    printf("thread    pri  status\r\n");
	printf("--------  ---  ------\r\n");
    for(i=0;i<thread_cnt;i++)
		{
                printf("%-8s  %-5d",thread_array[i]->name,thread_array[i]->current_priority);
				if(thread_array[i]->stat==GS_THREAD_READY)				printf("ready\r\n");
				else if(thread_array[i]->stat==GS_THREAD_RUNNING)		printf("running\r\n");
				else if(thread_array[i]->stat==GS_THREAD_SUSPEND)		printf("suspend\r\n");
				else if(thread_array[i]->stat==GS_THREAD_INIT)			printf("init\r\n");
				else if(thread_array[i]->stat==GS_THREAD_CLOSE)			printf("close\r\n");
		}
    
}

void cmd_help(void * parameter)
{
        printf("ps              	-List threads in the system\r\n");
		printf("version         	-show GS-Thread version information\r\n");
		printf("clear         		-clear the terminal screen\r\n");
		printf("reboot         		-Reboot System\r\n");
}

void cmd_reboot(void * parameter)
{
   NVIC_SystemReset(); 
}