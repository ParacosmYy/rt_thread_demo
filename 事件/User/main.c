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

static rt_thread_t send_thread = RT_NULL;
static rt_thread_t recv_thread = RT_NULL;
static rt_event_t test_event = RT_NULL;// 定义事件控制块

/*
*************************************************************************
*                             函数声明
*************************************************************************
*/
static void send_thread_entry(void* parameter);
static void recv_thread_entry(void* parameter);

uint8_t value[2]= {0, 0}; 


#define KEY1_EVENT (0x01 << 0)//设置事件掩码的位 0 
#define KEY2_EVENT (0x01 << 1)//设置事件掩码的位 1
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
    rt_kprintf("这是一个[野火]- STM32 全系列开发板-RTT 事件标志组实验！\n");
    test_event = rt_event_create("test_event",RT_IPC_FLAG_FIFO); // 创建事件控制块
    if (test_event == RT_NULL)
    {
        rt_kprintf("Event creation failed!\n");
        return -1;
    }
    else
    {
        rt_kprintf("Event created successfully!\n");
    }
    /* 创建发送线程 */
    send_thread = rt_thread_create("send_thread", 
                                send_thread_entry, 
                                RT_NULL, 
                                1024, 
                                2, 
                                50);
    if (send_thread != RT_NULL)
    {
        rt_err_t ret = rt_thread_startup(send_thread);
        rt_kprintf("send_thread startup ret: %d\r\n", ret);  // 打印返回值
        rt_kprintf("send_thread started successfully!\r\n");
    }
    else
    {
        rt_kprintf("send_thread create failed!\r\n");
        return -1;
    }
    /* 创建接收线程 */
    recv_thread = rt_thread_create("recv_thread", 
                                recv_thread_entry, 
                                RT_NULL, 
                                1024, 
                                3, 
                                50);
    if (recv_thread != RT_NULL)
    {
        rt_err_t ret = rt_thread_startup(recv_thread);
        rt_kprintf("recv_thread startup ret: %d\r\n", ret);  // 打印返回值
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
*                             线程定义
*************************************************************************
*/
// 修改后的发送线程
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

// 修改后的接收线程
static void recv_thread_entry(void* parameter)
{
 rt_uint32_t recved;
 while(1)
 {
  rt_event_recv(test_event,KEY1_EVENT|KEY2_EVENT,RT_EVENT_FLAG_OR|RT_EVENT_FLAG_CLEAR,RT_WAITING_FOREVER,&recved);
  if(recved == KEY1_EVENT | KEY2_EVENT)//连续两次标志位必须在同一次recv中接收到。不然的话会在线程结束后清除对应的标志位。
  {
   rt_kprintf("KEY1 and KEY2 are pressed!\n");
   LED1_TOGGLE;
  }
  else{
    rt_kprintf("事件错误");
  }
 }
}
/********************************END OF FILE****************************/
