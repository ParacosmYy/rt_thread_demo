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
static rt_thread_t led_thread = RT_NULL;
static rt_thread_t send_thread = RT_NULL;
static rt_thread_t recv_thread = RT_NULL;
static rt_mq_t test_mq = RT_NULL;

/*
*************************************************************************
*                             ��������
*************************************************************************
*/
static void led_thread_entry(void* parameter);
static void send_thread_entry(void* parameter);
static void recv_thread_entry(void* parameter);


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
    rt_kprintf("����һ��[Ұ��]-STM32 ȫϵ�п����� RTT ��Ϣ����ʵ�飡\n");
    rt_kprintf("���� K1 ���� K2 ���Ͷ�����Ϣ\n");
    rt_kprintf("receive �߳̽��յ���Ϣ�ڴ��ڻ���\n");
    /* ��ʼ��Ӳ�� */
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
    /* ������Ϣ���� */
    /* ����һ����Ϣ���У����г���Ϊ10��ÿ����Ϣ��СΪ40�ֽ� */
    /* RT_IPC_FLAG_FIFO ��ʾ�Ƚ��ȳ� */
    test_mq = rt_mq_create("test_mq", 
                            40, 
                            10, 
                            RT_IPC_FLAG_FIFO);
    if (test_mq == RT_NULL)
    {
        rt_kprintf("Message queue creation failed!\n");
        return -1;
    }
 
	return 0;
}

/*
*************************************************************************
*                             �̶߳���
*************************************************************************
*/

static void led_thread_entry(void* parameter)
{   
    while (1)
    {
        LED2_ON;
        rt_kprintf("LED ON\r\n");
        rt_thread_delay(500);
        
        LED2_OFF;
        rt_kprintf("LED OFF\r\n");
        rt_thread_delay(500);
    }
}
static void recv_thread_entry(void* parameter)
{
  rt_err_t uwret;
  uint32_t r_queue;
    while(1)
  {
    uwret = rt_mq_recv(test_mq,&r_queue,sizeof(r_queue),RT_WAITING_FOREVER);
    if(uwret == RT_EOK)
    {
      rt_kprintf("receive thread get message: %d\r\n", r_queue);
    }
    else
    {
      rt_kprintf("receive thread get message failed: 0x%lx\r\n", uwret);
    }
    rt_thread_delay(200); // ��ʱ0.2��
  }
}
static void send_thread_entry(void* parameter)
{
  rt_err_t uwret;
  uint32_t send_data1 =1;
  uint32_t send_data2 =2;
  while(1)
  { 
    // ��ⰴ��1
    if(Key_Scan(KEY1_GPIO_PORT,KEY1_PIN) == KEY_ON)
    {
      uwret = rt_mq_send(test_mq,&send_data1,sizeof(send_data1));
      if(uwret == RT_EOK)
      {
        rt_kprintf("send thread send message: %d\r\n", send_data1);
      }
      else
      {
        rt_kprintf("send thread send message failed: 0x%lx\r\n", uwret);
      }
    }
    // ��ⰴ��2
    if(Key_Scan(KEY2_GPIO_PORT,KEY2_PIN) == KEY_ON)
    {
      uwret = rt_mq_send(test_mq,&send_data2,sizeof(send_data2));
      if(uwret == RT_EOK)
      {
        rt_kprintf("send thread send message: %d\r\n", send_data2);
      }
      else
      {
        rt_kprintf("send thread send message failed: 0x%lx\r\n", uwret);
      }
    }
    rt_thread_delay(100); // ��ʱ0.1��
  }

}
/********************************END OF FILE****************************/
