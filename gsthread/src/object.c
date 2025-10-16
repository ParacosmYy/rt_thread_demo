#include <gsthread.h>
#include <gshw.h>
#include <list.h>

/*
 * 对象容器数组的下标定义，决定容器的大小
 */
enum gs_object_info_type
{
    GS_Object_Info_Thread = 0,                         /* 对象是线程 */
#ifdef GS_USING_SEMAPHORE
    GS_Object_Info_Semaphore,                          /* 对象是信号量 */
#endif
#ifdef GS_USING_MUTEX
    GS_Object_Info_Mutex,                              /* 对象是互斥量 */
#endif
#ifdef GS_USING_EVENT
    GS_Object_Info_Event,                              /* 对象是事件 */
#endif
#ifdef GS_USING_MAILBOX
    GS_Object_Info_MailBox,                            /* 对象是邮箱 */
#endif
#ifdef GS_USING_MESSAGEQUEUE
    GS_Object_Info_MessageQueue,                       /* 对象是消息队列 */
#endif
#ifdef GS_USING_MEMHEAP
    GS_Object_Info_MemHeap,                            /* 对象是内存堆 */
#endif
#ifdef GS_USING_MEMPOOL
    GS_Object_Info_MemPool,                            /* 对象是内存池 */
#endif
#ifdef GS_USING_DEVICE
    GS_Object_Info_Device,                             /* 对象是设备 */
#endif
    GS_Object_Info_Timer,                              /* 对象是定时器 */
#ifdef GS_USING_MODULE
    GS_Object_Info_Module,                             /* 对象是模块 */
#endif
    GS_Object_Info_Unknown,                            /* 对象未知 */
};



#define _OBJ_CONTAINER_LIST_INIT(c)     \
    {&(gs_object_container[c].object_list), &(gs_object_container[c].object_list)} //初始化对象容器链表头
		
static struct gs_object_information gs_object_container[GS_Object_Info_Unknown] =
{
    /* 初始化对象容器 - 线程 */
    {
        GS_Object_Class_Thread, 
        _OBJ_CONTAINER_LIST_INIT(GS_Object_Info_Thread), 
        sizeof(struct gs_thread)
    },
		
#ifdef GS_USING_SEMAPHORE
    /* 初始化对象容器 - 信号量 */
    {
        GS_Object_Class_Semaphore, 
        _OBJ_CONTAINER_LIST_INIT(GS_Object_Info_Semaphore), 
        sizeof(struct gs_semaphore)
    },
#endif		
		
#ifdef GS_USING_MUTEX
    /* 初始化对象容器 - 互斥量 */
    {
        GS_Object_Class_Mutex, 
        _OBJ_CONTAINER_LIST_INIT(GS_Object_Info_Mutex), 
        sizeof(struct gs_mutex)
    },
#endif		
		
#ifdef GS_USING_EVENT
    /* 初始化对象容器 - 事件 */
    {
        GS_Object_Class_Event, 
        _OBJ_CONTAINER_LIST_INIT(GS_Object_Info_Event), 
        sizeof(struct gs_event)
    },
#endif
		
#ifdef _USING_MAILBOX
    /* 初始化对象容器 - 邮箱 */
    {
        GS_Object_Class_MailBox, 
        _OBJ_CONTAINER_LIST_INIT(GS_Object_Info_MailBox), 
        sizeof(struct gs_mailbox)
    },
#endif	
		
#ifdef GS_USING_MESSAGEQUEUE
    /* 初始化对象容器 - 消息队列 */
    {
        GS_Object_Class_MessageQueue, 
        _OBJ_CONTAINER_LIST_INIT(GS_Object_Info_MessageQueue), 
        sizeof(struct gs_messagequeue)
    },
#endif		
		
#ifdef GS_USING_MEMHEAP
    /* 初始化对象容器 - 内存堆 */
    {
        GS_Object_Class_MemHeap, 
        _OBJ_CONTAINER_LIST_INIT(GS_Object_Info_MemHeap), 
        sizeof(struct gs_memheap)
    },
#endif		
		
#ifdef GS_USING_MEMPOOL
    /* 初始化对象容器 - 内存池 */
    {
        GS_Object_Class_MemPool, 
        _OBJ_CONTAINER_LIST_INIT(GS_Object_Info_MemPool), 
        sizeof(struct gs_mempool)
    },
#endif		
	
#ifdef GS_USING_DEVICE
    /* 初始化对象容器 - 设备 */
    {
        GS_Object_Class_Device, 
        _OBJ_CONTAINER_LIST_INIT(GS_Object_Info_Device), sizeof(struct gs_device)},
#endif
    /* 初始化对象容器 - 定时器 */
    /*
    {
        RT_Object_Class_Timer, 
        _OBJ_CONTAINER_LIST_INIT(RT_Object_Info_Timer), 
        sizeof(struct rt_timer)
    },
    */
#ifdef GS_USING_MODULE
    /* 初始化对象容器 - 模块 */
    {
        GS_Object_Class_Module, 
        _OBJ_CONTAINER_LIST_INIT(GS_Object_Info_Module), 
        sizeof(struct gs_module)
    },
#endif		
};

struct gs_object_information * gs_object_get_infomation(enum gs_object_class_type type)
{
    int index ;
    for(index = 0 ; index <= GS_Object_Info_Unknown ; index++ )
    {
        if(gs_object_container[index].type == type )
        {
            return &gs_object_container[index];  //找到了对应的对象信息
        }
    }
    return GS_NULL;
}

void gs_object_init(struct gs_object *object , enum gs_object_class_type type , const char *name)
{
    register gs_base_t temp;
    struct gs_object_information * information;
    
    information = gs_object_get_infomation(type); //获取对象信息

    object->type = type | GS_Object_Class_Static;
    
    gs_strncpy(object->name,name,GS_NAME_MAX);
    
    temp = gs_hw_interrupt_disable();
    
    gs_list_insert_after((&object->list),(&information->object_list));
    
    gs_hw_interrupt_enable(temp);
    
}