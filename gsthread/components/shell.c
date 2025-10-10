#include <gsthread.h>
#include "stdio.h"
#include "string.h"
#include "shell.h"
#include "cmd.h"
#include "ringbuffer.h"

gs_cmd_t cmd_table[GS_CMD_NUM_MAX];
gs_size_t current_cmd_num=0;

struct gs_ringbuffer shell_ringbuffer;
gs_uint8_t ringbuffer[200];

struct gs_thread gs_shell_thread ;
gs_uint8_t gs_shell_thread_stack[GS_SHELL_STACK_SIZE];

struct gs_shell my_shell;
struct gs_shell *shell = &my_shell; 

char shell_getchar()
{
    char ch ;
    if(!gs_ringbuffer_isempty(&shell_ringbuffer))
    {
        gs_ringbuffer_getchar(&shell_ringbuffer , (gs_uint8_t*)ch); //串口接受中断获取的字符存入ringbuffer，再取出ch
    }
    else
    {
        gs_thread_delay_ms(10);
    }
    return ch;
}

void gs_thread_shell_entry(void * parameter)
{
   
    int ch;
    shell->current_history = 0;
    shell->history_cnt = 0;
    printf("gs />");
    while(1)
    {
         //printf("gs />");
         ch = shell_getchar();
         if(ch==0XFF)
		{
		    continue;
		}
       
    }
   
}

void shell_match(char * cmd , gs_size_t length)
{
    int i ;
    for(i = 0 ; i < current_cmd_num ; i++)
    {
        if(length == cmd_table[i].cmd_length)
        {
            if(gs_strcmp(cmd , cmd_table[i].name))
            {
                cmd_table[i].entry(cmd_table[i].parameter);
                printf("gs />");
                return ;
            }
        }
    }
        printf("cmd not found!");
		printf("\r\n");
		printf("gs />");
}

void cmd_list_add(char* name, gs_size_t length , void(*entry)(void * parameter) , void *parameter)   
{
    strcpy(cmd_table[current_cmd_num].name, name);
    cmd_table[current_cmd_num].cmd_length=length;
    cmd_table[current_cmd_num].entry=entry;
    cmd_table[current_cmd_num].parameter=parameter;
	
    current_cmd_num++;
    
}

void system_cmd_init()
{
    	cmd_list_add("version",7,cmd_version,GS_NULL); //显示固件信息
		cmd_list_add("clear",5,cmd_clear,GS_NULL);//清屏幕
		cmd_list_add("ps",2,cmd_ps,GS_NULL);//显示系统状态
		cmd_list_add("reboot",6,cmd_reboot,GS_NULL);//重启
		cmd_list_add("help",4,cmd_help,GS_NULL);//显示支持命令
}


void gs_shell_init()
{
    system_cmd_init(); //命令集初始化

    gs_ringbuffer_init(&shell_ringbuffer , ringbuffer , sizeof(ringbuffer)); //shell线程的环形缓冲队列

    gs_thread_init(&gs_shell_thread , "shell" ,gs_thread_shell_entry  ,GS_NULL , &gs_shell_thread_stack[0] , sizeof(gs_shell_thread_stack) ,GS_SHELL_PRIORITY , 10 );

    gs_thread_startup(&gs_shell_thread);
    printf("shell启动成功over \r\n");
}

