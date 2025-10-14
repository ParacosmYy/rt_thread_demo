#ifndef __SHELL_H
#define __SHELL_H

#include "gsthread.h"

enum input_stat
{
    WAIT_NORMAL_KEY,   // �ȴ���ͨ����
    WAIT_DIR_KEY,      // �ȴ������
    WAIT_FUNC_KEY,     // �ȴ����ܼ�
	INPUT_END          // �������
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
		enum input_stat stat;//��ǰ����״̬
		
		char cmd[GS_SHELL_LENGSH];	//ָ������
	
		char* history[GS_SHELL_HISORY_MAX];//������ʷ��¼����
		gs_size_t history_cnt;//��ǰ��ʷ���������
		gs_size_t current_history;//��ǰѡ�е���ʷ������
	
		gs_size_t position;     	//��ǰ�г���
		gs_size_t curpos;     		//����λ��
};

void gs_thread_shell_entry(void * parameter);
void shell_match(char * cmd , gs_size_t length);
void cmd_list_add(char* name, gs_size_t length , void(*entry)(void * parameter) , void *parameter);
void system_cmd_init();
void gs_shell_init();

#endif