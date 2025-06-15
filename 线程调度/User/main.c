/**
  *********************************************************************
  * @file    main.c
  * @author  fire
  * @version V1.0
  * @date    2018-xx-xx
  * @brief   RT-Thread 3.0 + STM32 固件库例程
  *********************************************************************
  * @attention
  *
  * 实验平台:野火  STM32F407 开发板 
  * 论坛    :http://www.firebbs.cn
  * 淘宝    :https://fire-stm32.taobao.com
  *
  **********************************************************************
  */ 
 
/*
*************************************************************************
*                             包含的头文件
*************************************************************************
*/ 
#include "board.h"
#include "rtthread.h"


/*
*************************************************************************
*                               变量
*************************************************************************
*/
/* 定义线程控制块 */
static rt_thread_t led_thread = RT_NULL;
static rt_thread_t key_thread = RT_NULL;

/*
*************************************************************************
*                             函数声明
*************************************************************************
*/
static void led_thread_entry(void* parameter);
static void key_thread_entry(void* parameter);


/*
*************************************************************************
*                             main 函数
*************************************************************************
*/
/**
  * @brief  主函数
  * @param  无
  * @retval 无
  */
int main(void)
{	
    rt_kprintf("RT-Thread 3.0 + STM32 固件库例程\r\n");      
    /* 初始化硬件 */
    led_thread = rt_thread_create("led_thread", 
                                led_thread_entry, 
                                RT_NULL, 
                                1024, 
                                1, 
                                50);
    if (led_thread != RT_NULL)
    {
      rt_err_t ret = rt_thread_startup(led_thread);
      rt_kprintf("led_thread startup ret: %d\r\n", ret);  // 打印返回值
      rt_kprintf("led_thread started successfully!\r\n");
    }
    else
    {
      rt_kprintf("led_thread create failed!\r\n");
      return -1;
    }
    /* 创建按键线程 */
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
    rt_thread_delay(500);   /* 延时500个tick */		
	return 0;
}

/*
*************************************************************************
*                             线程定义
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
                    rt_kprintf("挂起成功!\n");
                } else {
                    rt_kprintf("挂起失败! 错误码:0x%lx\n", ret);
                }
            }
            rt_thread_delay(200); // 消抖
        }
        
        if (Key_Scan(KEY2_GPIO_PORT, KEY2_PIN) == KEY_ON && is_suspended) {
            rt_err_t ret = rt_thread_resume(led_thread);
            if (ret == RT_EOK) {
                is_suspended = RT_FALSE;
                rt_kprintf("恢复成功!\n");
            } else {
                rt_kprintf("恢复失败! 错误码:0x%lx\n", ret);
            }
            rt_thread_delay(200); // 消抖
        }
        
        rt_thread_delay(50);
    }
}

/********************************END OF FILE****************************/
