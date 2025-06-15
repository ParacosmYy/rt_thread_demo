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
static rt_thread_t send_thread = RT_NULL;
static rt_thread_t recv_thread = RT_NULL;
static rt_mq_t test_mq = RT_NULL;

/*
*************************************************************************
*                             函数声明
*************************************************************************
*/
static void led_thread_entry(void* parameter);
static void send_thread_entry(void* parameter);
static void recv_thread_entry(void* parameter);


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
    rt_kprintf("这是一个[野火]-STM32 全系列开发板 RTT 消息队列实验！\n");
    rt_kprintf("按下 K1 或者 K2 发送队列消息\n");
    rt_kprintf("receive 线程接收到消息在串口回显\n");
    /* 初始化硬件 */
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
    /* 创建消息队列 */
    /* 创建一个消息队列，队列长度为10，每个消息大小为40字节 */
    /* RT_IPC_FLAG_FIFO 表示先进先出 */
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
    rt_thread_delay(200); // 延时0.2秒
  }
}
static void send_thread_entry(void* parameter)
{
  rt_err_t uwret;
  uint32_t send_data1 =1;
  uint32_t send_data2 =2;
  while(1)
  { 
    // 检测按键1
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
    // 检测按键2
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
    rt_thread_delay(100); // 延时0.1秒
  }

}
/********************************END OF FILE****************************/
