#include <gsthread.h>
#include <gshw.h>
#include <list.h>

/*
 * ��������������±궨�壬���������Ĵ�С
 */
enum gs_object_info_type
{
    GS_Object_Info_Thread = 0,                         /* �������߳� */
#ifdef GS_USING_SEMAPHORE
    GS_Object_Info_Semaphore,                          /* �������ź��� */
#endif
#ifdef GS_USING_MUTEX
    GS_Object_Info_Mutex,                              /* �����ǻ����� */
#endif
#ifdef GS_USING_EVENT
    GS_Object_Info_Event,                              /* �������¼� */
#endif
#ifdef GS_USING_MAILBOX
    GS_Object_Info_MailBox,                            /* ���������� */
#endif
#ifdef GS_USING_MESSAGEQUEUE
    GS_Object_Info_MessageQueue,                       /* ��������Ϣ���� */
#endif
#ifdef GS_USING_MEMHEAP
    GS_Object_Info_MemHeap,                            /* �������ڴ�� */
#endif
#ifdef GS_USING_MEMPOOL
    GS_Object_Info_MemPool,                            /* �������ڴ�� */
#endif
#ifdef GS_USING_DEVICE
    GS_Object_Info_Device,                             /* �������豸 */
#endif
    GS_Object_Info_Timer,                              /* �����Ƕ�ʱ�� */
#ifdef GS_USING_MODULE
    GS_Object_Info_Module,                             /* ������ģ�� */
#endif
    GS_Object_Info_Unknown,                            /* ����δ֪ */
};



#define _OBJ_CONTAINER_LIST_INIT(c)     \
    {&(gs_object_container[c].object_list), &(gs_object_container[c].object_list)} //��ʼ��������������ͷ
		
static struct gs_object_information gs_object_container[GS_Object_Info_Unknown] =
{
    /* ��ʼ���������� - �߳� */
    {
        GS_Object_Class_Thread, 
        _OBJ_CONTAINER_LIST_INIT(GS_Object_Info_Thread), 
        sizeof(struct gs_thread)
    },
		
#ifdef GS_USING_SEMAPHORE
    /* ��ʼ���������� - �ź��� */
    {
        GS_Object_Class_Semaphore, 
        _OBJ_CONTAINER_LIST_INIT(GS_Object_Info_Semaphore), 
        sizeof(struct gs_semaphore)
    },
#endif		
		
#ifdef GS_USING_MUTEX
    /* ��ʼ���������� - ������ */
    {
        GS_Object_Class_Mutex, 
        _OBJ_CONTAINER_LIST_INIT(GS_Object_Info_Mutex), 
        sizeof(struct gs_mutex)
    },
#endif		
		
#ifdef GS_USING_EVENT
    /* ��ʼ���������� - �¼� */
    {
        GS_Object_Class_Event, 
        _OBJ_CONTAINER_LIST_INIT(GS_Object_Info_Event), 
        sizeof(struct gs_event)
    },
#endif
		
#ifdef _USING_MAILBOX
    /* ��ʼ���������� - ���� */
    {
        GS_Object_Class_MailBox, 
        _OBJ_CONTAINER_LIST_INIT(GS_Object_Info_MailBox), 
        sizeof(struct gs_mailbox)
    },
#endif	
		
#ifdef GS_USING_MESSAGEQUEUE
    /* ��ʼ���������� - ��Ϣ���� */
    {
        GS_Object_Class_MessageQueue, 
        _OBJ_CONTAINER_LIST_INIT(GS_Object_Info_MessageQueue), 
        sizeof(struct gs_messagequeue)
    },
#endif		
		
#ifdef GS_USING_MEMHEAP
    /* ��ʼ���������� - �ڴ�� */
    {
        GS_Object_Class_MemHeap, 
        _OBJ_CONTAINER_LIST_INIT(GS_Object_Info_MemHeap), 
        sizeof(struct gs_memheap)
    },
#endif		
		
#ifdef GS_USING_MEMPOOL
    /* ��ʼ���������� - �ڴ�� */
    {
        GS_Object_Class_MemPool, 
        _OBJ_CONTAINER_LIST_INIT(GS_Object_Info_MemPool), 
        sizeof(struct gs_mempool)
    },
#endif		
	
#ifdef GS_USING_DEVICE
    /* ��ʼ���������� - �豸 */
    {
        GS_Object_Class_Device, 
        _OBJ_CONTAINER_LIST_INIT(GS_Object_Info_Device), sizeof(struct gs_device)},
#endif
    /* ��ʼ���������� - ��ʱ�� */
    /*
    {
        RT_Object_Class_Timer, 
        _OBJ_CONTAINER_LIST_INIT(RT_Object_Info_Timer), 
        sizeof(struct rt_timer)
    },
    */
#ifdef GS_USING_MODULE
    /* ��ʼ���������� - ģ�� */
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
            return &gs_object_container[index];  //�ҵ��˶�Ӧ�Ķ�����Ϣ
        }
    }
    return GS_NULL;
}

void gs_object_init(struct gs_object *object , enum gs_object_class_type type , const char *name)
{
    register gs_base_t temp;
    struct gs_object_information * information;
    
    information = gs_object_get_infomation(type); //��ȡ������Ϣ

    object->type = type | GS_Object_Class_Static;
    
    gs_strncpy(object->name,name,GS_NAME_MAX);
    
    temp = gs_hw_interrupt_disable();
    
    gs_list_insert_after((&object->list),(&information->object_list));
    
    gs_hw_interrupt_enable(temp);
    
}