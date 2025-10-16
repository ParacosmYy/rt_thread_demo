#include <gsthread.h>
#include <gshw.h>


static gs_list_t gs_timer_list[GS_TIMER_SKIP_LIST_LEVEL];


void gs_system_timer_init(void)
{
    int i;

    for (i = 0; i < sizeof(gs_timer_list) / sizeof(gs_timer_list[0]); i++)
    {
        gs_list_init(gs_timer_list + i);// ȫ������ͷ��ʼ����eg��0����Թ���a��b��c��ʱ����1����Թ���d��e��f��ʱ�����Դ�����
    }
}


static void _gs_timer_init(gs_timer_t timer,
                           void (*timeout)(void *parameter),
                           void      *parameter,
                           gs_tick_t  time,
                           gs_uint8_t flag)//��ʼ����ʱ��
{
    int i;

    /* ���ñ�־ */
    timer->parent.flag  = flag;

    /* ������Ϊ�Ǽ���̬ */
    timer->parent.flag &= ~GS_TIMER_FLAG_ACTIVATED;

    timer->timeout_func = timeout; // ���ó�ʱ���� ��gs_thread_timeout��
    timer->parameter    = parameter;

    /* ��ʼ�� ��ʱ��ʵ�ʳ�ʱʱ��ϵͳ������ */
    timer->timeout_tick = 0;//ʲôʱ��ʱ��������ʱ���
    /* ��ʼ�� ��ʱ����Ҫ��ʱ�Ľ����� */
    timer->init_tick    = time;  //�ȶ�ó�ʱ�������ʱ����

    /* ��ʼ����ʱ�������ýڵ� */
    for (i = 0; i < GS_TIMER_SKIP_LIST_LEVEL; i++)
    {
        gs_list_init(&(timer->row[i]));//������ʱ���Ľڵ��ʼ�� eg��a��ʱ���ڵ��ʼ����b��ʱ���ڵ��ʼ����c��ʱ���ڵ��ʼ�����Դ�����
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
        gs_list_remove(&timer->row[i]); //����ʱ����ϵͳ��ʱ���б�ɾ��
    }
}

gs_err_t gs_timer_stop(gs_timer_t timer)
{
    register gs_base_t level;

   // If the timer is not active, return error
    if (!(timer->parent.flag & GS_TIMER_FLAG_ACTIVATED))
        return -GS_ERROR;

    /* ���ж� */
    level = gs_hw_interrupt_disable();

    /* ����ʱ���Ӷ�ʱ���б�ɾ�� */
    _gs_timer_remove(timer);

    /* ���ж� */
    gs_hw_interrupt_enable(level);

    /* �ı䶨ʱ����״̬Ϊ��active */
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


    /* ���ж� */
    level = gs_hw_interrupt_disable();
    
    /* ����ʱ����ϵͳ��ʱ���б��Ƴ� */
    _gs_timer_remove(timer);
    
    /* �ı䶨ʱ����״̬Ϊ��active */
    timer->parent.flag &= ~GS_TIMER_FLAG_ACTIVATED;
    
    /* ���ж� */
    gs_hw_interrupt_enable(level);

    /* ��ȡ timeout tick,
       ����timeout tick ���ܴ��� RT_TICK_MAX/2 */
    timer->timeout_tick = gs_tick_get() + timer->init_tick;

    /* ���ж� */
    level = gs_hw_interrupt_disable();


    /* ����ʱ�����뵽��ʱ���б� */
    /* ��ȡϵͳ��ʱ���б���ڵ��ַ��rt_timer_list��һ��ȫ�ֱ��� */
    timer_list = gs_timer_list;


    /* ��ȡϵͳ��ʱ���б��һ��������ڵ��ַ */
    row_head[0]  = &timer_list[0];
    
    /* ��ΪRT_TIMER_SKIP_LIST_LEVEL����1�����ѭ��ֻ��ִ��һ�� */
    for (row_lvl = 0; row_lvl < GS_TIMER_SKIP_LIST_LEVEL; row_lvl++)
    {
        /* �б�Ϊ�գ���û�ж�ʱ�������뵽ϵͳ��ʱ���б�ʱ����ѭ����ִ�� */
        for (; row_head[row_lvl] != timer_list[row_lvl].prev; row_head[row_lvl]  = row_head[row_lvl]->next)
        {
            struct gs_timer *t;
            
            /* ��ȡ��ʱ���б�ڵ��ַ */
            gs_list_t *p = row_head[row_lvl]->next;

            /* ���ݽڵ��ַ��ȡ���ṹ��ָ�� */
            t = gs_list_entry(p,                 /* �ڵ��ַ */ 
                              struct gs_timer,   /* �ڵ����ڸ��ṹ���������� */
                              row[row_lvl]);     /* �ڵ��ڸ��ṹ�н�ʲô�������� */
            
            /* ������ʱ���ĳ�ʱʱ����ͬ��������ڶ�ʱ���б���Ѱ����һ���ڵ� */
            if ((t->timeout_tick - timer->timeout_tick) == 0)
            {
                continue;
            }
            /* ������ʱ���ĳ�ʱʱ����ͬ��������ڶ�ʱ���б���Ѱ����һ���ڵ� */
            else if ((t->timeout_tick - timer->timeout_tick) < GS_TICK_MAX / 2)
            {
                break;
            }
            
        }
        /* ����������棬���ᱻִ�� */
        if (row_lvl != GS_TIMER_SKIP_LIST_LEVEL - 1)
        {
           row_head[row_lvl + 1] = row_head[row_lvl] + 1; 
        }            
    }

    /* random_nr��һ����̬���������ڼ�¼�����˶��ٸ���ʱ�� */
    random_nr++;
    tst_nr = random_nr;

    /* ����ʱ�����뵽ϵͳ��ʱ���б� */
    gs_list_insert_after(row_head[GS_TIMER_SKIP_LIST_LEVEL - 1],       /* ˫���б���ڵ��ַ */
                         &(timer->row[GS_TIMER_SKIP_LIST_LEVEL - 1])); /* Ҫ������Ľڵ�ĵ�ַ */
    
    /* RT_TIMER_SKIP_LIST_LEVEL ����1����forѭ����Զ����ִ�� */
    for (row_lvl = 2; row_lvl <= GS_TIMER_SKIP_LIST_LEVEL; row_lvl++)
    {
        if (!(tst_nr & GS_TIMER_SKIP_LIST_MASK))
            gs_list_insert_after(row_head[GS_TIMER_SKIP_LIST_LEVEL - row_lvl],
                                 &(timer->row[GS_TIMER_SKIP_LIST_LEVEL - row_lvl]));
        else
            break;

        tst_nr >>= (GS_TIMER_SKIP_LIST_MASK + 1) >> 1;
    }

    /* ���ö�ʱ����־λΪ����̬ */
    timer->parent.flag |= GS_TIMER_FLAG_ACTIVATED;

    /* ���ж� */
    gs_hw_interrupt_enable(level);

    return -GS_EOK;
}

void gs_timer_check(void)
{
    struct gs_timer *t;
    gs_tick_t current_tick;
    register gs_base_t level;

    current_tick = gs_tick_get();

    /* ���ж� */
    level = gs_hw_interrupt_disable();

    /* ϵͳ��ʱ���б�Ϊ�գ���ɨ�趨ʱ���б� */
    while (!gs_list_isempty(&gs_timer_list[GS_TIMER_SKIP_LIST_LEVEL - 1]))
    {
        /* ��ȡ��һ���ڵ㶨ʱ���ĵ�ַ */
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
                /* ������ʱ�� */
                t->parent.flag &= ~GS_TIMER_FLAG_ACTIVATED;
                gs_timer_start(t);
            }
            /* ���ζ�ʱ�� */
            else
            {
                /* ֹͣ��ʱ�� */
                t->parent.flag &= ~GS_TIMER_FLAG_ACTIVATED;
            }
        }
        else
            break;
    }

    /* ���ж� */
    gs_hw_interrupt_enable(level);
}

