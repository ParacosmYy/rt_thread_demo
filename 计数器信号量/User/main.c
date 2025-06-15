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
static rt_sem_t test_sem = RT_NULL;  // 定义信号量

/*
*************************************************************************
*                             函数声明
*************************************************************************
*/
static void send_thread_entry(void* parameter);
static void recv_thread_entry(void* parameter);

uint8_t value[2]= {0, 0}; 

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
    rt_kprintf("这是一个[野火]- STM32 全系列开发板 RTT 二值信号量同步实验！\n");
    rt_kprintf("同步成功则输出 Successful,反之输出 Fail\n");
   
    test_sem = rt_sem_create("test_sem",0, RT_IPC_FLAG_FIFO);
    if (test_sem == RT_NULL)
    {
        rt_kprintf("Semaphore creation failed!\n");
        return -1;
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
  while(1) {
    value[0]++; // 更新第一个值
    rt_thread_delay(100); // 延时
    value[1]++; // 更新第二个值
    rt_sem_release(test_sem); // 发送完成信号
    rt_kprintf("Sent values: %d, %d\n", value[0], value[1]);
    rt_thread_delay(1000); // 控制发送频率
  }
}

// 修改后的接收线程
static void recv_thread_entry(void* parameter)
{
  while(1) {
    rt_sem_take(test_sem, RT_WAITING_FOREVER); // 等待信号
    if(value[0] == value[1]) {
      rt_kprintf("Consistent: %d\n", value[0]);
    } else {
      rt_kprintf("Inconsistent!\n");
    }
  }
}
/********************************END OF FILE****************************/
