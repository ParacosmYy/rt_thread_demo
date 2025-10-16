#include <gsthread.h>
#include <gshw.h>


static gs_list_t gs_timer_list[GS_TIMER_SKIP_LIST_LEVEL];


void gs_system_timer_init(void)
{
    int i;

    for (i = 0; i < sizeof(gs_timer_list) / sizeof(gs_timer_list[0]); i++)
    {
        gs_list_init(gs_timer_list + i);// 全局链表头初始化，eg：0层可以挂载a，b，c定时器，1层可以挂载d，e，f定时器，以此类推
    }
}


static void _gs_timer_init(gs_timer_t timer,
                           void (*timeout)(void *parameter),
                           void      *parameter,
                           gs_tick_t  time,
                           gs_uint8_t flag)//初始化定时器
{
    int i;

    /* 设置标志 */
    timer->parent.flag  = flag;

    /* 先设置为非激活态 */
    timer->parent.flag &= ~GS_TIMER_FLAG_ACTIVATED;

    timer->timeout_func = timeout; // 设置超时函数 （gs_thread_timeout）
    timer->parameter    = parameter;

    /* 初始化 定时器实际超时时的系统节拍数 */
    timer->timeout_tick = 0;//什么时候超时——绝对时间点
    /* 初始化 定时器需要超时的节拍数 */
    timer->init_tick    = time;  //等多久超时——相对时间间隔

    /* 初始化定时器的内置节点 */
    for (i = 0; i < GS_TIMER_SKIP_LIST_LEVEL; i++)
    {
        gs_list_init(&(timer->row[i]));//单个定时器的节点初始化 eg：a定时器节点初始化，b定时器节点初始化，c定时器节点初始化，以此类推
    }
}


void gs_timer_init(gs_timer_t  timer,
                   const char *name,
                   void (*timeout)(void *parameter),
                   void       *parameter,
                   gs_tick_t   time,
                   gs_uint8_t  flag)
{
    
    gs_object_init((gs_object_t)timer, GS_Object_Class_Timer, name);

    
    _gs_timer_init(timer, timeout, parameter, time, flag);
}

void _gs_timer_remove(gs_timer_t timer)
{
    int i;

    for (i = 0; i < GS_TIMER_SKIP_LIST_LEVEL; i++)
    {
        gs_list_remove(&timer->row[i]); //将定时器从系统定时器列表删除
    }
}

gs_err_t gs_timer_stop(gs_timer_t timer)
{
    register gs_base_t level;

   // If the timer is not active, return error
    if (!(timer->parent.flag & GS_TIMER_FLAG_ACTIVATED))
        return -GS_ERROR;

    /* 关中断 */
    level = gs_hw_interrupt_disable();

    /* 将定时器从定时器列表删除 */
    _gs_timer_remove(timer);

    /* 开中断 */
    gs_hw_interrupt_enable(level);

    /* 改变定时器的状态为非active */
    timer->parent.flag &= ~GS_TIMER_FLAG_ACTIVATED;

    return GS_EOK;
}

gs_err_t gs_timer_control(gs_timer_t timer, int cmd, void *arg)
{
    switch (cmd)
    {
    case GS_TIMER_CTRL_GET_TIME:
        *(gs_tick_t *)arg = timer->init_tick;
        break;

    case GS_TIMER_CTRL_SET_TIME:
        timer->init_tick = *(gs_tick_t *)arg;
        break;

    case GS_TIMER_CTRL_SET_ONESHOT:
        timer->parent.flag &= ~GS_TIMER_FLAG_PERIODIC;
        break;

    case GS_TIMER_CTRL_SET_PERIODIC:
        timer->parent.flag |= GS_TIMER_FLAG_PERIODIC;
        break;
    }

    return GS_EOK;
}


gs_err_t gs_timer_start(gs_timer_t timer)
{
    unsigned int row_lvl = 0;
    gs_list_t *timer_list;
    register gs_base_t level;
    gs_list_t *row_head[GS_TIMER_SKIP_LIST_LEVEL];
    unsigned int tst_nr;
    static unsigned int random_nr;


    /* 关中断 */
    level = gs_hw_interrupt_disable();
    
    /* 将定时器从系统定时器列表移除 */
    _gs_timer_remove(timer);
    
    /* 改变定时器的状态为非active */
    timer->parent.flag &= ~GS_TIMER_FLAG_ACTIVATED;
    
    /* 开中断 */
    gs_hw_interrupt_enable(level);

    /* 获取 timeout tick,
       最大的timeout tick 不能大于 RT_TICK_MAX/2 */
    timer->timeout_tick = gs_tick_get() + timer->init_tick;

    /* 关中断 */
    level = gs_hw_interrupt_disable();


    /* 将定时器插入到定时器列表 */
    /* 获取系统定时器列表根节点地址，rt_timer_list是一个全局变量 */
    timer_list = gs_timer_list;


    /* 获取系统定时器列表第一条链表根节点地址 */
    row_head[0]  = &timer_list[0];
    
    /* 因为RT_TIMER_SKIP_LIST_LEVEL等于1，这个循环只会执行一次 */
    for (row_lvl = 0; row_lvl < GS_TIMER_SKIP_LIST_LEVEL; row_lvl++)
    {
        /* 列表不为空，当没有定时器被插入到系统定时器列表时，该循环不执行 */
        for (; row_head[row_lvl] != timer_list[row_lvl].prev; row_head[row_lvl]  = row_head[row_lvl]->next)
        {
            struct gs_timer *t;
            
            /* 获取定时器列表节点地址 */
            gs_list_t *p = row_head[row_lvl]->next;

            /* 根据节点地址获取父结构的指针 */
            t = gs_list_entry(p,                 /* 节点地址 */ 
                              struct gs_timer,   /* 节点所在父结构的数据类型 */
                              row[row_lvl]);     /* 节点在父结构中叫什么，即名字 */
            
            /* 两个定时器的超时时间相同，则继续在定时器列表中寻找下一个节点 */
            if ((t->timeout_tick - timer->timeout_tick) == 0)
            {
                continue;
            }
            /* 两个定时器的超时时间相同，则继续在定时器列表中寻找下一个节点 */
            else if ((t->timeout_tick - timer->timeout_tick) < GS_TICK_MAX / 2)
            {
                break;
            }
            
        }
        /* 条件不会成真，不会被执行 */
        if (row_lvl != GS_TIMER_SKIP_LIST_LEVEL - 1)
        {
           row_head[row_lvl + 1] = row_head[row_lvl] + 1; 
        }            
    }

    /* random_nr是一个静态变量，用于记录启动了多少个定时器 */
    random_nr++;
    tst_nr = random_nr;

    /* 将定时器插入到系统定时器列表 */
    gs_list_insert_after(row_head[GS_TIMER_SKIP_LIST_LEVEL - 1],       /* 双向列表根节点地址 */
                         &(timer->row[GS_TIMER_SKIP_LIST_LEVEL - 1])); /* 要被插入的节点的地址 */
    
    /* RT_TIMER_SKIP_LIST_LEVEL 等于1，该for循环永远不会执行 */
    for (row_lvl = 2; row_lvl <= GS_TIMER_SKIP_LIST_LEVEL; row_lvl++)
    {
        if (!(tst_nr & GS_TIMER_SKIP_LIST_MASK))
            gs_list_insert_after(row_head[GS_TIMER_SKIP_LIST_LEVEL - row_lvl],
                                 &(timer->row[GS_TIMER_SKIP_LIST_LEVEL - row_lvl]));
        else
            break;

        tst_nr >>= (GS_TIMER_SKIP_LIST_MASK + 1) >> 1;
    }

    /* 设置定时器标志位为激活态 */
    timer->parent.flag |= GS_TIMER_FLAG_ACTIVATED;

    /* 开中断 */
    gs_hw_interrupt_enable(level);

    return -GS_EOK;
}

void gs_timer_check(void)
{
    struct gs_timer *t;
    gs_tick_t current_tick;
    register gs_base_t level;

    current_tick = gs_tick_get();

    /* 关中断 */
    level = gs_hw_interrupt_disable();

    /* 系统定时器列表不为空，则扫描定时器列表 */
    while (!gs_list_isempty(&gs_timer_list[GS_TIMER_SKIP_LIST_LEVEL - 1]))
    {
        /* 获取第一个节点定时器的地址 */
        t = gs_list_entry
        (gs_timer_list[GS_TIMER_SKIP_LIST_LEVEL - 1].next,   
         struct gs_timer,                                    
         row[GS_TIMER_SKIP_LIST_LEVEL - 1]);                 
              
        if ((current_tick - t->timeout_tick) < GS_TICK_MAX / 2)
        {
        
            _gs_timer_remove(t);
            t->timeout_func(t->parameter);          
            current_tick = gs_tick_get();


            if ((t->parent.flag & GS_TIMER_FLAG_PERIODIC) &&
                (t->parent.flag & GS_TIMER_FLAG_ACTIVATED))
            {
                /* 启动定时器 */
                t->parent.flag &= ~GS_TIMER_FLAG_ACTIVATED;
                gs_timer_start(t);
            }
            /* 单次定时器 */
            else
            {
                /* 停止定时器 */
                t->parent.flag &= ~GS_TIMER_FLAG_ACTIVATED;
            }
        }
        else
            break;
    }

    /* 开中断 */
    gs_hw_interrupt_enable(level);
}

