#include <gsthread.h>

struct gs_semaphore semaphore[GS_SEM_NUM_MAX] ;
gs_size_t sem_index = 0;


struct gs_mutex mutexs[GS_MUTEX_NUM_MAX];         //互斥量
gs_size_t mutex_index=0;

gs_sem_t gs_sem_create(const char* name , gs_uint16_t value , gs_uint8_t flag )
{
    struct gs_semaphore * sem = &semaphore[sem_index];
    sem_index ++ ;
    
    gs_list_init(&(sem->suspend_thread)); // 初始化挂载链表
    
    gs_strncpy(sem->name , name , sizeof(sem->name)-1);
    sem->name[sizeof(sem->name) - 1] = '\0';
    
    sem->value = value;
    sem->flag = flag;
    
    return sem;
}



gs_err_t gs_ipc_list_resume(gs_list_t * list)
{
    struct gs_thread * thread;
    thread = gs_list_entry(list->next , struct gs_thread , tlist);
    
    gs_thread_resume(thread);//恢复线程
    return GS_EOK ;
}



gs_err_t gs_ipc_list_resume_all(gs_list_t * list)
{
    struct gs_thread *thread;
    register gs_ubase_t temp;

    
    while (!gs_list_isempty(list))
    {
        
        temp = gs_hw_interrupt_disable();

      
        thread = gs_list_entry(list->next, struct gs_thread, tlist);
       
        thread->error = -GS_ERROR; // ipc发生异常 导致线程恢复

        gs_thread_resume(thread);

        
        gs_hw_interrupt_enable(temp);
    }

    return GS_EOK;
}

gs_err_t gs_sem_delete(gs_sem_t sem)
{
    gs_ipc_list_resume_all((&sem->suspend_thread));
    return GS_EOK;
}

gs_inline gs_err_t gs_ipc_list_suspend(gs_list_t        *list,
                                       struct gs_thread *thread,
                                       gs_uint8_t        flag)
{
    
    gs_thread_suspend(thread);//挂起 线程

    switch (flag)
    {
    case GS_IPC_FLAG_FIFO:
        gs_list_insert_before(list, &(thread->tlist));
        break;

    case GS_IPC_FLAG_PRIO:
        {
            struct gs_list_node *n; // list_t
            struct gs_thread *sthread;

           
            for (n = list->next; n != list; n = n->next) //遍历链表 （就绪or挂载）
            {
                sthread = gs_list_entry(n, struct gs_thread, tlist);

               
                if (thread->current_priority < sthread->current_priority)
                {
     
                    gs_list_insert_before(&(sthread->tlist), &(thread->tlist));
                    break;
                }
            }

            if (n == list)
                gs_list_insert_before(list, &(thread->tlist));
        }//哪个比当前线程优先级低  插到前面
        break;
    }

    return GS_EOK;
}


gs_err_t gs_sem_take(gs_sem_t sem, gs_int32_t time)
{
    register gs_base_t temp;
    struct gs_thread *thread;

  
    temp = gs_hw_interrupt_disable();

    if (sem->value > 0) //信号量获取成功
    {
        sem->value --;
        gs_hw_interrupt_enable(temp);
    }
    else
    {
        if (time == 0)
        {
            gs_hw_interrupt_enable(temp);

            return -GS_ETIMEOUT;
        }
        else
        {
            thread = gs_thread_self();         
            thread->error = GS_EOK;
            
            thread->error = gs_ipc_list_suspend(&(sem->suspend_thread), // 当前线程 挂载到 信号量中的suspend线程中
                                thread,
                                sem->flag);
            if (time > 0)
            {
                gs_timer_control(&(thread->thread_timer),
                                 GS_TIMER_CTRL_SET_TIME,
                                 &time);
                gs_timer_start(&(thread->thread_timer)); //超时后自动自动唤醒线程
            }

           
            gs_hw_interrupt_enable(temp);
            gs_schedule();

            if (thread->error != GS_EOK)
            {
                return thread->error;
            }
        }
    }

    return GS_EOK;
}

gs_err_t gs_sem_release(gs_sem_t sem)
{
    register gs_base_t temp;
    register gs_bool_t need_schedule;

    need_schedule = GS_FALSE;

    temp = gs_hw_interrupt_disable();


    if (!gs_list_isempty(&sem->suspend_thread))
    {
      
        gs_ipc_list_resume(&(sem->suspend_thread)); //释放当前信号量中的suspend线程
        need_schedule = GS_TRUE;
    }
    else
        sem->value ++; 

    
    gs_hw_interrupt_enable(temp);

    if (need_schedule == GS_TRUE)
        gs_schedule();

    return GS_EOK;
}


//---------------------------------------互斥量---------------------------------------------


gs_mutex_t gs_mutex_create(const char* name , gs_uint8_t flag)
{
    struct gs_mutex *mutex = &mutexs[mutex_index++];
    gs_list_init(&mutex->suspend_thread);
    
    mutex->flag = flag;
    mutex->value = 1;
    mutex->hold = 0;
    gs_strncpy(mutex->name , name , sizeof(mutex->name)-1);
    mutex->name[sizeof((mutex->name) - 1)] = '\0';
    mutex->original_priority = 0xFF;
    mutex->owner = GS_NULL;
    
    return mutex;
}


gs_err_t gs_mutex_delete(gs_mutex_t mutex)
{
		gs_ipc_list_resume_all(&mutex->suspend_thread);
	
		return GS_EOK;
}

gs_err_t gs_mutex_take(gs_mutex_t mutex, gs_int32_t time)
{
    register gs_base_t temp;
    struct gs_thread *thread;


    thread = gs_thread_self();

    temp = gs_hw_interrupt_disable();

    thread->error = GS_EOK;

    if (mutex->owner == thread)
    {
        mutex->hold ++; //持有次数+1
    }
    else
    {
__again:
        
        if (mutex->value > 0)
        {
            
            mutex->value --;

            mutex->owner             = thread; //当前线程为互斥锁持有者
            mutex->original_priority = thread->current_priority;//记录原始优先级
            mutex->hold ++;
        }
        else
        {
            if (time == 0)
            {
                thread->error = -GS_ETIMEOUT;

               
                gs_hw_interrupt_enable(temp);

                return -GS_ETIMEOUT;
            }
            else
            {

                if (thread->current_priority < mutex->owner->current_priority)
                {
                   
                    gs_thread_control(mutex->owner,
                                      GS_THREAD_CTRL_CHANGE_PRIORITY,
                                      &thread->current_priority);
                }

                   gs_ipc_list_suspend(&(mutex->suspend_thread), // 恢复互斥锁所持有的挂起线程
                                       thread,
                                       mutex->flag);

            
                if (time > 0)
                {
             
                    gs_timer_control(&(thread->thread_timer),
                                     GS_TIMER_CTRL_SET_TIME,
                                     &time);
                    gs_timer_start(&(thread->thread_timer));
                }

              
                gs_hw_interrupt_enable(temp);

                gs_schedule();

                if (thread->error != GS_EOK)
                {
                	if (thread->error == -GS_EINTR) goto __again; //防止被信号中断（正常唤醒线程后会退出循环执行）

                 
                    return thread->error;
                }
                else
                {    
                    temp = gs_hw_interrupt_disable();
                }
            }
        }
    }


    gs_hw_interrupt_enable(temp);

    return GS_EOK;
}

gs_err_t gs_mutex_release(gs_mutex_t mutex)
{
    register gs_base_t temp;
    struct gs_thread *thread;
    gs_bool_t need_schedule;

    need_schedule = GS_FALSE;

   
    thread = gs_thread_self();

    temp = gs_hw_interrupt_disable();

    if (thread != mutex->owner) //当前线程不是互斥锁所持有的线程
    {
        thread->error = -GS_ERROR;

        gs_hw_interrupt_enable(temp);

        return -GS_ERROR;
    }
    mutex->hold --; //持有次数减一
    
    if (mutex->hold == 0)  //彻底释放互斥锁 
    {
       
        if (mutex->original_priority != mutex->owner->current_priority)
        {
            gs_thread_control(mutex->owner,
                              GS_THREAD_CTRL_CHANGE_PRIORITY,
                              &(mutex->original_priority));//恢复初始优先级
        }

      
        if (!gs_list_isempty(&mutex->suspend_thread))
        {
           
            thread = gs_list_entry(mutex->suspend_thread.next,
                                   struct gs_thread,
                                   tlist);


           
            mutex->owner             = thread;
            mutex->original_priority = thread->current_priority;
            mutex->hold ++;

         
            gs_ipc_list_resume(&(mutex->suspend_thread));//唤醒这个挂起线程

            need_schedule = GS_TRUE; //需要调度
        }
        else
        {
            
            mutex->value ++;

            
            mutex->owner             = GS_NULL; 
            mutex->original_priority = 0xff;
        }
    }

    
    gs_hw_interrupt_enable(temp);

    
    if (need_schedule == GS_TRUE)
        gs_schedule();

    return GS_EOK;
}














