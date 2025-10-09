#include <gsthread.h>
#include "stdio.h"
#include "string.h"
#include "shell.h"
#include "cmd.h"

gs_cmd_t cmd_table[GS_CMD_NUM_MAX];
gs_size_t current_cmd_num=0;


void cmd_list_add(char* name,gs_size_t length , void(*entry)(void * parameter) , void *parameter)   
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
    system_cmd_init();
    
    
}