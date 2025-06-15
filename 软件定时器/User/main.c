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

static rt_timer_t timer1;
static rt_timer_t timer2;


uint32_t timer1_count = 0;
uint32_t timer2_count = 0;
/*
*************************************************************************
*                             函数声明
*************************************************************************
*/
void timer1_callback(void* parameter);
void timer2_callback(void* parameter);


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
  rt_kprintf("这是一个[野火]- STM32 全系列开发板-RTT 软件定时器实验！\n");
  rt_kprintf("定时器超时函数 2 只执行一次就被销毁\n");
  rt_kprintf("定时器超时函数 1 则循环执行\n");
   timer1 = rt_timer_create("timer1",timer1_callback,RT_NULL,1000,RT_TIMER_FLAG_PERIODIC);//默认配置软件定时器，如果配置的是硬件参数，则会绑定到tim上面！
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
*                             线程定义
*************************************************************************
*/

static void timer1_callback(void* parameter)
{
  uint32_t tick1 = 0;
  tick1 = rt_tick_get();
  rt_kprintf("系统滴答定时器数值1：%d\n",tick1);
  timer1_count++;
  rt_kprintf("回调函数执行次数：%d\n",timer1_count);
}

static void timer2_callback(void* parameter)
{
  uint32_t tick2 = 0;
  tick2 = rt_tick_get();
  rt_kprintf("系统滴答定时器数值2：%d\n",tick2);
  timer2_count++;
  rt_kprintf("回调函数执行次数：%d\n",timer2_count);
}

/********************************END OF FILE****************************/
