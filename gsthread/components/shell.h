#ifndef __SHELL_H
#define __SHELL_H

#include "gsthread.h"

enum input_stat
{
    WAIT_NORMAL_KEY,   // 等待普通按键
    WAIT_DIR_KEY,      // 等待方向键
    WAIT_FUNC_KEY,     // 等待功能键
	INPUT_END          // 输入结束
};

struct cmd_node
{
		char * name;
		int  cmd_length;
	
		void       (*entry)(void *parameter);
		void       *parameter;
};

typedef struct cmd_node gs_cmd_t;

struct gs_shell
{
		enum input_stat stat;//当前输入状态
		
		char cmd[GS_SHELL_LENGSH];	//指令内容
	
		char* history[GS_SHELL_HISORY_MAX];//命令历史记录数组
		gs_size_t history_cnt;//当前历史命令的数量
		gs_size_t current_history;//当前选中的历史命令编号
	
		gs_size_t position;     	//当前行长度
		gs_size_t curpos;     		//光标的位置
};

void gs_thread_shell_entry(void * parameter);
void shell_match(char * cmd , gs_size_t length);
void cmd_list_add(char* name, gs_size_t length , void(*entry)(void * parameter) , void *parameter);
void system_cmd_init();
void gs_shell_init();

#endif