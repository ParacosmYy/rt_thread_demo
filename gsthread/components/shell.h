#ifndef __SHELL_H
#define __SHELL_H

#include "gsthread.h"

enum input_stat
{
    WAIT_NORMAL_KEY,   // �ȴ���ͨ����
    WAIT_DIR_KEY,      // �ȴ������
    WAIT_FUNC_KEY,     // �ȴ����ܼ�
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