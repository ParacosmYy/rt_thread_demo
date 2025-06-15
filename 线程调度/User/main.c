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
static rt_thread_t key_thread = RT_NULL;

/*
*************************************************************************
*                             ��������
*************************************************************************
*/
static void led_thread_entry(void* parameter);
static void key_thread_entry(void* parameter);


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
    rt_kprintf("RT-Thread 3.0 + STM32 �̼�������\r\n");      
    /* ��ʼ��Ӳ�� */
    led_thread = rt_thread_create("led_thread", 
                                led_thread_entry, 
                                RT_NULL, 
                                1024, 
                                1, 
                                50);
    if (led_thread != RT_NULL)
    {
      rt_err_t ret = rt_thread_startup(led_thread);
      rt_kprintf("led_thread startup ret: %d\r\n", ret);  // ��ӡ����ֵ
      rt_kprintf("led_thread started successfully!\r\n");
    }
    else
    {
      rt_kprintf("led_thread create failed!\r\n");
      return -1;
    }
    /* ���������߳� */
    key_thread = rt_thread_create("key_thread", 
                                key_thread_entry, 
                                RT_NULL, 
                                1024, 
                                2, 
                                20);
    if (key_thread != RT_NULL)
    {
      rt_thread_startup(key_thread);
    }
    else
    {
      rt_kprintf("key_thread create failed!\r\n");
      return -1;
    }
    rt_kprintf("Main thread priority: %d\n", rt_thread_self()->current_priority);
    rt_thread_delay(500);   /* ��ʱ500��tick */		
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
static void key_thread_entry(void* parameter) {
    static rt_bool_t is_suspended = RT_FALSE;
    
    while (1) {
        if (Key_Scan(KEY1_GPIO_PORT, KEY1_PIN) == KEY_ON && !is_suspended) {
            if (led_thread->stat == RT_THREAD_READY) {
                rt_err_t ret = rt_thread_suspend(led_thread);
                if (ret == RT_EOK) {
                    is_suspended = RT_TRUE;
                    rt_kprintf("����ɹ�!\n");
                } else {
                    rt_kprintf("����ʧ��! ������:0x%lx\n", ret);
                }
            }
            rt_thread_delay(200); // ����
        }
        
        if (Key_Scan(KEY2_GPIO_PORT, KEY2_PIN) == KEY_ON && is_suspended) {
            rt_err_t ret = rt_thread_resume(led_thread);
            if (ret == RT_EOK) {
                is_suspended = RT_FALSE;
                rt_kprintf("�ָ��ɹ�!\n");
            } else {
                rt_kprintf("�ָ�ʧ��! ������:0x%lx\n", ret);
            }
            rt_thread_delay(200); // ����
        }
        
        rt_thread_delay(50);
    }
}

/********************************END OF FILE****************************/
