#include <gsthread.h>

struct gs_semaphore semaphore[GS_SEM_NUM_MAX] ;
gs_size_t sem_index = 0;

gs_inline gs_sem_t gs_sem_create(const char* name , gs_uint16_t value , gs_uint8_t flag )
{
    struct gs_semaphore * sem = &semaphore[sem_index];
    sem_index ++ ;
    
    gs_list_init(&(sem->suspend_thread));
    
    gs_strncpy(sem->name , name , sizeof(sem->name)-1);
    sem->name[sizeof(sem->name) - 1] = '\0';
    
    sem->value = value;
    sem->flag = flag;
    
    return sem;
}


gs_inline gs_err_t gs_ipc_list_resume(gs_list_t * list)
{
    struct gs_thread * thread;
    thread = gs_list_entry(list->next , struct gs_thread , tlist);
    
    gs_thread_resume(thread);
    return GS_EOK ;
}

gs_inline gs_err_t gs_ipc_list_resume_all(gs_list_t * list)
{
    struct gs_thread *thread;
    register gs_ubase_t temp;

    
    while (!gs_list_isempty(list))
    {
        
        temp = gs_hw_interrupt_disable();

      
        thread = gs_list_entry(list->next, struct gs_thread, tlist);
       
        thread->error = -GS_ERROR;

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
    
    gs_thread_suspend(thread);

    switch (flag)
    {
    case GS_IPC_FLAG_FIFO:
        gs_list_insert_before(list, &(thread->tlist));
        break;

    case GS_IPC_FLAG_PRIO:
        {
            struct gs_list_node *n;
            struct gs_thread *sthread;

           
            for (n = list->next; n != list; n = n->next)
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
        }
        break;
    }

    return GS_EOK;
}

gs_err_t gs_sem_take(gs_sem_t sem, gs_int32_t time)
{
    register gs_base_t temp;
    struct gs_thread *thread;

  
    temp = gs_hw_interrupt_disable();

    if (sem->value > 0)
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
            
          thread->error = gs_ipc_list_suspend(&(sem->suspend_thread),
                                thread,
                                sem->flag);
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
                return thread->error;
            }
        }
    }

    return GS_EOK;
}

