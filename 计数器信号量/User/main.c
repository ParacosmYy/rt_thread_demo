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

static rt_thread_t send_thread = RT_NULL;
static rt_thread_t recv_thread = RT_NULL;
static rt_sem_t test_sem = RT_NULL;  // �����ź���

/*
*************************************************************************
*                             ��������
*************************************************************************
*/
static void send_thread_entry(void* parameter);
static void recv_thread_entry(void* parameter);

uint8_t value[2]= {0, 0}; 

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
    rt_kprintf("����һ��[Ұ��]- STM32 ȫϵ�п����� RTT ��ֵ�ź���ͬ��ʵ�飡\n");
    rt_kprintf("ͬ���ɹ������ Successful,��֮��� Fail\n");
   
    test_sem = rt_sem_create("test_sem",0, RT_IPC_FLAG_FIFO);
    if (test_sem == RT_NULL)
    {
        rt_kprintf("Semaphore creation failed!\n");
        return -1;
    }
    /* ���������߳� */
    send_thread = rt_thread_create("send_thread", 
                                send_thread_entry, 
                                RT_NULL, 
                                1024, 
                                2, 
                                50);
    if (send_thread != RT_NULL)
    {
        rt_err_t ret = rt_thread_startup(send_thread);
        rt_kprintf("send_thread startup ret: %d\r\n", ret);  // ��ӡ����ֵ
        rt_kprintf("send_thread started successfully!\r\n");
    }
    else
    {
        rt_kprintf("send_thread create failed!\r\n");
        return -1;
    }
    /* ���������߳� */
    recv_thread = rt_thread_create("recv_thread", 
                                recv_thread_entry, 
                                RT_NULL, 
                                1024, 
                                3, 
                                50);
    if (recv_thread != RT_NULL)
    {
        rt_err_t ret = rt_thread_startup(recv_thread);
        rt_kprintf("recv_thread startup ret: %d\r\n", ret);  // ��ӡ����ֵ
        rt_kprintf("recv_thread started successfully!\r\n");
    }
    else
    {
        rt_kprintf("recv_thread create failed!\r\n");
        return -1;
    }
    
	return 0;
}

/*
*************************************************************************
*                             �̶߳���
*************************************************************************
*/
// �޸ĺ�ķ����߳�
static void send_thread_entry(void* parameter)
{
  while(1) {
    value[0]++; // ���µ�һ��ֵ
    rt_thread_delay(100); // ��ʱ
    value[1]++; // ���µڶ���ֵ
    rt_sem_release(test_sem); // ��������ź�
    rt_kprintf("Sent values: %d, %d\n", value[0], value[1]);
    rt_thread_delay(1000); // ���Ʒ���Ƶ��
  }
}

// �޸ĺ�Ľ����߳�
static void recv_thread_entry(void* parameter)
{
  while(1) {
    rt_sem_take(test_sem, RT_WAITING_FOREVER); // �ȴ��ź�
    if(value[0] == value[1]) {
      rt_kprintf("Consistent: %d\n", value[0]);
    } else {
      rt_kprintf("Inconsistent!\n");
    }
  }
}
/********************************END OF FILE****************************/
