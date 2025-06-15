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
static rt_event_t test_event = RT_NULL;// �����¼����ƿ�

/*
*************************************************************************
*                             ��������
*************************************************************************
*/
static void send_thread_entry(void* parameter);
static void recv_thread_entry(void* parameter);

uint8_t value[2]= {0, 0}; 


#define KEY1_EVENT (0x01 << 0)//�����¼������λ 0 
#define KEY2_EVENT (0x01 << 1)//�����¼������λ 1
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
    rt_kprintf("����һ��[Ұ��]- STM32 ȫϵ�п�����-RTT �¼���־��ʵ�飡\n");
    test_event = rt_event_create("test_event",RT_IPC_FLAG_FIFO); // �����¼����ƿ�
    if (test_event == RT_NULL)
    {
        rt_kprintf("Event creation failed!\n");
        return -1;
    }
    else
    {
        rt_kprintf("Event created successfully!\n");
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
 while(1)
  {
    if(Key_Scan(KEY1_GPIO_PORT,KEY1_PIN) == KEY_ON)
    {
    rt_kprintf("KEY1 is pressed!\n");
    rt_event_send(test_event,KEY1_EVENT);
    }
    if(Key_Scan(KEY2_GPIO_PORT,KEY2_PIN) == KEY_ON)
    {
    rt_kprintf("KEY2 is pressed!\n");
    rt_event_send(test_event,KEY2_EVENT);
    }
    rt_thread_delay(10);
  }
}

// �޸ĺ�Ľ����߳�
static void recv_thread_entry(void* parameter)
{
 rt_uint32_t recved;
 while(1)
 {
  rt_event_recv(test_event,KEY1_EVENT|KEY2_EVENT,RT_EVENT_FLAG_OR|RT_EVENT_FLAG_CLEAR,RT_WAITING_FOREVER,&recved);
  if(recved == KEY1_EVENT | KEY2_EVENT)//�������α�־λ������ͬһ��recv�н��յ�����Ȼ�Ļ������߳̽����������Ӧ�ı�־λ��
  {
   rt_kprintf("KEY1 and KEY2 are pressed!\n");
   LED1_TOGGLE;
  }
  else{
    rt_kprintf("�¼�����");
  }
 }
}
/********************************END OF FILE****************************/
