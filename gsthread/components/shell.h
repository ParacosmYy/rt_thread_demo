#ifndef __SHELL_H
#define __SHELL_H

#include "gsthread.h"

enum input_stat
{
    WAIT_NORMAL_KEY,   // 等待普通按键
    WAIT_DIR_KEY,      // 等待方向键
    WAIT_FUNC_KEY,     // 等待功能键
};

struct cmd_node
{
		char * name;
		int cmd_length;
	
		void       (*entry)(void *parameter);
		void       *parameter;
};

typedef struct cmd_node gs_cmd_t; 
#endif