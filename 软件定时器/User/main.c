/**
  *********************************************************************
  * @file    main.c
  * @author  fire
  * @version V1.0
  * @date    2018-xx-xx
  * @brief   RT-Thread 3.0 + STM32 �̼�������
  *********************************************************************
  * @attention
  *
  * ʵ��ƽ̨:Ұ��  STM32F407 ������ 
  * ��̳    :http://www.firebbs.cn
  * �Ա�    :https://fire-stm32.taobao.com
  *
  **********************************************************************
  */ 
 
/*
*************************************************************************
*                             ������ͷ�ļ�
*************************************************************************
*/ 
#include "board.h"
#include "rtthread.h"


/*
*************************************************************************
*                               ����
*************************************************************************
*/
/* �����߳̿��ƿ� */

static rt_timer_t timer1;
static rt_timer_t timer2;


uint32_t timer1_count = 0;
uint32_t timer2_count = 0;
/*
*************************************************************************
*                             ��������
*************************************************************************
*/
void timer1_callback(void* parameter);
void timer2_callback(void* parameter);


/*
*************************************************************************
*                             main ����
*************************************************************************
*/
/**
  * @brief  ������
  * @param  ��
  * @retval ��
  */
int main(void)
{	
  rt_kprintf("����һ��[Ұ��]- STM32 ȫϵ�п�����-RTT �����ʱ��ʵ�飡\n");
  rt_kprintf("��ʱ����ʱ���� 2 ִֻ��һ�ξͱ�����\n");
  rt_kprintf("��ʱ����ʱ���� 1 ��ѭ��ִ��\n");
   timer1 = rt_timer_create("timer1",timer1_callback,RT_NULL,1000,RT_TIMER_FLAG_PERIODIC);//Ĭ�����������ʱ����������õ���Ӳ�����������󶨵�tim���棡
   if(timer1 != RT_NULL)
   {
    rt_timer_start(timer1);
   }
   else {
    rt_kprintf("timer1 create failed!\n");
   }
   timer2 = rt_timer_create("timer2",timer2_callback,RT_NULL,5000,RT_TIMER_FLAG_ONE_SHOT);
   if(timer2 != RT_NULL)
   {
    rt_timer_start(timer2);
   }
   else {
    rt_kprintf("timer2 create failed!\n");
   }
	return 0;
}

/*
*************************************************************************
*                             �̶߳���
*************************************************************************
*/

static void timer1_callback(void* parameter)
{
  uint32_t tick1 = 0;
  tick1 = rt_tick_get();
  rt_kprintf("ϵͳ�δ�ʱ����ֵ1��%d\n",tick1);
  timer1_count++;
  rt_kprintf("�ص�����ִ�д�����%d\n",timer1_count);
}

static void timer2_callback(void* parameter)
{
  uint32_t tick2 = 0;
  tick2 = rt_tick_get();
  rt_kprintf("ϵͳ�δ�ʱ����ֵ2��%d\n",tick2);
  timer2_count++;
  rt_kprintf("�ص�����ִ�д�����%d\n",timer2_count);
}

/********************************END OF FILE****************************/
