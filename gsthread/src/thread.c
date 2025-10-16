#include <gsthread.h>
#include <gshw.h>
#include <object.h>



extern struct gs_thread *gs_current_thread;
extern gs_uint32_t gs_thread_ready_priority_group ;
extern gs_list_t gs_thread_priority_table[GS_THREAD_PRIORITY_MAX];

extern gs_thread_t thread_array[GS_THREAD_NUM_MAX];
extern gs_size_t thread_cnt;

gs_err_t gs_thread_init(struct gs_thread *thread,char *name , void(*entry)(void *parameter),void* parameter , void* stack_addr,gs_uint32_t stack_size,gs_uint8_t priority,gs_uint32_t tick)
{
    gs_object_init((gs_object_t)thread, GS_Object_Class_Thread,name);
    gs_list_init(&thread->tlist);
    
    thread_array[thread_cnt]=thread; //���浽�߳�������
	thread_cnt++;//��¼�߳�����
    
    thread->entry = (void*)entry;
    thread->parameter = parameter;
    
    thread->stack_addr = stack_addr;
    thread->stack_size = stack_size;
    
    thread->sp = (void *)gs_hw_stack_init( thread->entry, 
		                                   thread->parameter,
							               (void *)((char *)thread->stack_addr + thread->stack_size - 4) );
    
    thread->current_priority = priority ;
    thread->init_priority = priority ;
    thread->number_mask = 0;//�߳����ȼ�����
    
    thread->error = GS_EOK;
    thread->stat = GS_THREAD_INIT; //��ʼ״̬Ϊ��ʼ��̬
    
    thread->init_tick = tick;//�̳߳�ʼ��ʱ�δ���
    thread->remaining_tick = tick;//�߳�ʣ����ʱ�δ���
    
    gs_timer_init(&(thread->thread_timer),     
                  thread->name,                
                  gs_thread_timeout,           
                  thread,                      
                  0,                           
                  GS_TIMER_FLAG_ONE_SHOT);    //�̶߳�ʱ����ʼ�� 
    
    return GS_EOK;
}

gs_err_t gs_thread_resume(struct gs_thread * thread)
{
    register gs_base_t temp ;
    
    if((thread->stat & GS_THREAD_STAT_MASK) != GS_THREAD_SUSPEND)
    {
        return -GS_ERROR;
    }
    
    temp = gs_hw_interrupt_disable();
    
    gs_list_remove(&(thread->tlist));
    
    gs_hw_interrupt_enable(temp);
    
    gs_schedule_insert_thread(thread);
    
    return GS_EOK ;
}

gs_err_t gs_thread_startup(struct gs_thread * thread)
{
    thread->current_priority = thread->init_priority;//���õ�ǰ���ȼ�Ϊ��ʼ���ȼ�
    thread->number_mask = 1L << thread->current_priority ;//�����߳����ȼ�����
    
    thread->stat = GS_THREAD_SUSPEND;
    
    gs_thread_resume(thread);
    
    if(gs_thread_self() != GS_NULL) // �������߳�
    {
        gs_schedule();
    }
    
    return GS_EOK;
}

//void gs_thread_delay(gs_tick_t tick)
//{
//    struct gs_thread *thread;
//    register gs_ubase_t temp;
//    
//    temp = gs_hw_interrupt_disable();
//    
//    thread = gs_current_thread;   
//    thread->remaining_tick = tick ;
//    
//    thread->current_priority = GS_THREAD_SUSPEND;
//    gs_thread_ready_priority_group &= ~thread->number_mask;
//    
//    gs_hw_interrupt_enable(temp);
//    
//    
//    gs_schedule();
//    
//}

gs_err_t gs_thread_delay(gs_tick_t tick)
{
    return gs_thread_sleep(tick);
} 

void gs_thread_delay_ms(gs_tick_t ms)
{
    gs_thread_delay(ms);
}

void gs_thread_delay_s(gs_tick_t s)
{
    gs_thread_delay(s*1000);
}

gs_thread_t gs_thread_self(void)
{
    return gs_current_thread;
}

gs_err_t gs_thread_suspend(gs_thread_t thread)
{
    register gs_base_t temp;


    
    if ((thread->stat & GS_THREAD_STAT_MASK) != GS_THREAD_READY) //����̬�����
    {
        return -GS_ERROR;
    }

   
    temp = gs_hw_interrupt_disable();

   
    thread->stat = GS_THREAD_SUSPEND;

    gs_schedule_remove_thread(thread);//�Ӿ���������ɾ�� Ȼ��λ������0


    gs_timer_stop(&(thread->thread_timer));//ֹͣ�̶߳�ʱ��

  
    gs_hw_interrupt_enable(temp);

    return GS_EOK;
}


gs_err_t gs_thread_sleep(gs_tick_t tick)//�߳����ߺ���
{
    register gs_base_t temp;
    struct gs_thread *thread;

   
    temp = gs_hw_interrupt_disable();
    
   
    thread = gs_current_thread;

    
    gs_thread_suspend(thread); //����ǰ�߳�

    gs_timer_control(&(thread->thread_timer), GS_TIMER_CTRL_SET_TIME, &tick);
    

    gs_timer_start(&(thread->thread_timer));


    gs_hw_interrupt_enable(temp);

    gs_schedule();

    return GS_EOK;
}


void gs_thread_timeout(void *parameter) //�̶߳�ʱ�����е���
{
    struct gs_thread *thread;

    thread = (struct gs_thread *)parameter; 

    /* ���ô�����Ϊ��ʱ */
    thread->error = -GS_ETIMEOUT;

    /* ���̴߳ӹ����б���ɾ�� */
    gs_list_remove(&(thread->tlist));

    /* ���̲߳��뵽�����б� */
    gs_schedule_insert_thread(thread);

    /* ϵͳ���� */
    gs_schedule();
}

gs_err_t gs_thread_yield(void)
{
    register gs_base_t level;
    struct gs_thread * thread;
    
    level = gs_hw_interrupt_disable();
    
    thread = gs_current_thread;
    
    if((thread->stat & GS_THREAD_STAT_MASK) == GS_THREAD_READY && thread->tlist.next != thread->tlist.prev) //����̬�Ҿ��������в�ֹһ���߳�
    {
        gs_list_remove(&(thread->tlist)); //�Ӿ���������ɾ����ǰ�߳�
        
        gs_list_insert_before(&(gs_thread_priority_table[thread->current_priority]),&(thread->tlist));//�ӵ�ǰ���ȼ�����ɾ������뵽��ǰ���ȼ����еĶ�β
        
        gs_hw_interrupt_enable(level);
        
        gs_schedule();
        
        return GS_EOK;
    }
    
    gs_hw_interrupt_enable(level);
    
    return GS_EOK;    
}

gs_err_t  gs_thread_control(gs_thread_t thread , int cmd , void *arg)
{
    register gs_base_t temp ;
    
    switch(cmd)
    {
        case GS_THREAD_CTRL_CHANGE_PRIORITY :
             temp = gs_hw_interrupt_disable();
        
        if(thread->stat & GS_THREAD_STAT_MASK == GS_THREAD_READY) //����̬
        {
            gs_schedule_remove_thread(thread); //�Ӿ���������ɾ��
            
            thread->current_priority = *(gs_uint8_t *)arg ;
            
            thread->number_mask = 1<<thread->current_priority; //�����µ����ȼ�����
            
            gs_schedule_insert_thread(thread); 
        }
        else //�Ǿ���ֱ̬�Ӹı��̵߳����ȼ�
        {
            thread->current_priority = *(gs_uint8_t *)arg ;
            
            thread->number_mask = 1<<thread->current_priority;
        }
        gs_hw_interrupt_enable(temp);
        
        break ;
    }
    return GS_EOK ;
}