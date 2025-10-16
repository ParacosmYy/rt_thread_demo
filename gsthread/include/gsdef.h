#ifndef __GS_DEF_H__
#define __GS_DEF_H__

#include <gsconfig.h>


/*
*************************************************************************
*                               ��������
*************************************************************************
*/

typedef signed   char                   gs_int8_t;      /**<  8bit integer type */
typedef signed   short                  gs_int16_t;     /**< 16bit integer type */
typedef signed   long                   gs_int32_t;     /**< 32bit integer type */
typedef unsigned char                   gs_uint8_t;     /**<  8bit unsigned integer type */
typedef unsigned short                  gs_uint16_t;    /**< 16bit unsigned integer type */
typedef unsigned long                   gs_uint32_t;    /**< 32bit unsigned integer type */
typedef int                             gs_bool_t;      /**< boolean type */
                                        
/* 32bit CPU */                         
typedef long                            gs_base_t;      /**< Nbit CPU related date type */
typedef unsigned long                   gs_ubase_t;     /**< Nbit unsigned CPU related data type */
                                     
typedef gs_base_t                       gs_err_t;       /**< Type for error number */
typedef gs_uint32_t                     gs_time_t;      /**< Type for time stamp */
typedef gs_uint32_t                     gs_tick_t;      /**< Type for tick count */
typedef gs_base_t                       gs_flag_t;      /**< Type for flags */
typedef gs_ubase_t                      gs_size_t;      /**< Type for size number */
typedef gs_ubase_t                      gs_dev_t;       /**< Type for device */
typedef gs_base_t                       gs_off_t;       /**< Type for offset */


/* �������ͺ궨�� */
#define GS_TRUE                         1               /**< boolean true  */
#define GS_FALSE                        0               /**< boolean fails */

/* maximum value of base type */
#define GS_UINT8_MAX                    0xff            /**< Maxium number of UINT8 */
#define GS_UINT16_MAX                   0xffff          /**< Maxium number of UINT16 */
#define GS_UINT32_MAX                   0xffffffff      /**< Maxium number of UINT32 */
#define GS_TICK_MAX                     GS_UINT32_MAX   /**< Maxium number of tick */

/* boolean type definitions */
#define GS_TRUE                         1               /**< boolean true  */
#define GS_FALSE                        0               /**< boolean fails */

#ifdef __CC_ARM
		#define gs_inline                   static __inline
		#define ALIGN(n)                    __attribute__((aligned(n)))

#elif defined (__IAR_SYSTEMS_ICC__)
    #define gs_inline                   static inline
		#define ALIGN(n)                    PRAGMA(data_alignment=n)
		
#elif defined (__GNUC__)
    #define gs_inline                   static __inline
		#define ALIGN(n)                    __attribute__((aligned(n)))
#else
    #error not supported tool chain			
#endif

		
#define GS_ALIGN(size, align)           (((size) + (align) - 1) & ~((align) - 1))
#define GS_ALIGN_DOWN(size, align)      ((size) & ~((align) - 1))
      
		
#define GS_NULL                         (0)

/*
*************************************************************************
*                               �����붨��
*************************************************************************
*/

#define GS_EOK                          0               /**< There is no error */
#define GS_ERROR                        1               /**< A generic error happens */
#define GS_ETIMEOUT                     2               /**< Timed out */
#define GS_EFULL                        3               /**< The resource is full */
#define GS_EEMPTY                       4               /**< The resource is empty */
#define GS_ENOMEM                       5               /**< No memory */
#define GS_ENOSYS                       6               /**< No system */
#define GS_EBUSY                        7               /**< Busy */
#define GS_EIO                          8               /**< IO error */
#define GS_EINTR                        9               /**< Interrupted system call */
#define GS_EINVAL                       10              /**< Invalid argument */


//˫������ڵ�
struct gs_list_node 
{
    struct gs_list_node *prev;
    struct gs_list_node *next;
};
typedef struct gs_list_node gs_list_t ;			
//�ں˶������
struct gs_object
{
    char name [GS_NAME_MAX] ;
    gs_uint8_t type ;
    gs_uint8_t flag; 
    gs_list_t list;
};
typedef struct gs_object *gs_object_t ;




/**
 * ʱ�� & ��ʱ�� �궨��
 */
#define GS_TIMER_FLAG_DEACTIVATED       0x0     /* ��ʱ��û�м��� */
#define GS_TIMER_FLAG_ACTIVATED         0x1     /* ��ʱ���Ѿ����� */
#define GS_TIMER_FLAG_ONE_SHOT          0x0     /* ���ζ�ʱ */
#define GS_TIMER_FLAG_PERIODIC          0x2     /* ���ڶ�ʱ */
        
#define GS_TIMER_FLAG_HARD_TIMER        0x0     /* Ӳ����ʱ������ʱ���ص������� tick isr�е��� */
        
#define GS_TIMER_FLAG_SOFT_TIMER        0x4     /* �����ʱ������ʱ���ص������ڶ�ʱ���߳��е��� */
        
        
#define GS_TIMER_CTRL_SET_TIME          0x0     /* ���ö�ʱ����ʱʱ�� */
#define GS_TIMER_CTRL_GET_TIME          0x1     /* ��ȡ��ʱ����ʱʱ�� */
#define GS_TIMER_CTRL_SET_ONESHOT       0x2     /* �޸Ķ�ʱ��Ϊһ�ζ�ʱ */
#define GS_TIMER_CTRL_SET_PERIODIC      0x3     /* �޸Ķ�ʱ��Ϊ���ڶ�ʱ */
        
#ifndef GS_TIMER_SKIP_LIST_LEVEL
#define GS_TIMER_SKIP_LIST_LEVEL          1
#endif

/* 1 or 3 */
#ifndef GS_TIMER_SKIP_LIST_MASK
#define GS_TIMER_SKIP_LIST_MASK         0x3
#endif

//��ʱ���ṹ��
struct gs_timer
{
    struct gs_object parent;                         /* �� rt_object �̳� */

    gs_list_t row[GS_TIMER_SKIP_LIST_LEVEL];// ��ʱ������ڵ�

    void (*timeout_func)(void *parameter);           /* ��ʱ���� */
    void            *parameter;                      /* ��ʱ�����β� */

    gs_tick_t        init_tick;                      /* ��ʱ��ʵ����Ҫ��ʱ��ʱ�� */
    gs_tick_t        timeout_tick;                   /* ��ʱ��ʵ�ʳ�ʱʱ��ϵͳ������ */
};
typedef struct gs_timer *gs_timer_t;

/**
 * ��������������ĺ���ʹ�ܣ���Щ��ͨ����gsconfig.h�ж���
 *  - Thread
 *  - Semaphore
 *  - Mutex
 *  - Event
 *  - MailBox
 *  - MessageQueue
 *  - MemHeap
 *  - MemPool
 *  - Device
 *  - Timer
 *  - Module
 *  - Unknown
 *  - Static
 */
enum gs_object_class_type
{
     GS_Object_Class_Thread = 0,       /* �������߳� */
     GS_Object_Class_Semaphore,        /* �������ź��� */
     GS_Object_Class_Mutex,            /* �����ǻ����� */
     GS_Object_Class_Event,            /* �������¼� */
     GS_Object_Class_MailBox,          /* ���������� */
     GS_Object_Class_MessageQueue,     /* ��������Ϣ���� */
     GS_Object_Class_MemHeap,          /* �������ڴ�� */
     GS_Object_Class_MemPool,          /* �������ڴ�� */
     GS_Object_Class_Device,           /* �������豸 */
     GS_Object_Class_Timer,            /* �����Ƕ�ʱ�� */
     GS_Object_Class_Module,           /* ������ģ�� */
     GS_Object_Class_Unknown,          /* ����δ֪ */
     GS_Object_Class_Static = 0x80     /* �����Ǿ�̬���� */
};



/**
 * �ں˶�����Ϣ�ṹ��
 */
struct gs_object_information
{
    enum gs_object_class_type type;                     /* �������� */
    gs_list_t                 object_list;              /* �����б�ڵ� */
    gs_size_t                 object_size;              /* �����С */
};
//�߳̿��ƿ�ṹ��
struct gs_thread
{
    char name [GS_NAME_MAX] ;
    gs_uint8_t type;
    gs_uint8_t flags;
    gs_list_t list;
    
    
	void        *sp;	          /* �߳�ջָ�� */
	void        *entry;	          /* �߳���ڵ�ַ */
	void        *parameter;	      /* �߳��β� */	
	void        *stack_addr;      /* �߳���ʼ��ַ */
	gs_uint32_t stack_size;       /* �߳�ջ��С����λΪ�ֽ� */
    

	
	gs_list_t   tlist;            /* �߳�����ڵ� */
    
    gs_ubase_t remaining_tick ;
    gs_ubase_t init_tick;
    
    
    gs_uint8_t  current_priority;     /* ��ǰ���ȼ� */
    gs_uint8_t  init_priority;        /* ��ʼ���ȼ� */
    gs_uint32_t number_mask;          /* ��ǰ���ȼ����� */
    
    gs_err_t    error;                /* ������ */
    gs_uint8_t  stat;                 /* �̵߳�״̬ */
    
    struct gs_timer thread_timer;     /* ���õ��̶߳�ʱ�� */
};
typedef struct gs_thread *gs_thread_t;

/*
 * �߳�״̬����
 */
#define GS_THREAD_INIT                  0x00                /* ��ʼ̬ */
#define GS_THREAD_READY                 0x01                /* ����̬ */
#define GS_THREAD_SUSPEND               0x02                /* ����̬ */
#define GS_THREAD_RUNNING               0x03                /* ����̬ */
#define GS_THREAD_BLOCK                 GS_THREAD_SUSPEND   /* ����̬ */
#define GS_THREAD_CLOSE                 0x04                /* �ر�̬ */
#define GS_THREAD_STAT_MASK             0x0f// �߳�״̬����
        
#define GS_THREAD_STAT_SIGNAL           0x10
#define GS_THREAD_STAT_SIGNAL_READY     (GS_THREAD_STAT_SIGNAL | GS_THREAD_READY)
#define GS_THREAD_STAT_SIGNAL_SUSPEND   0x20
#define GS_THREAD_STAT_SIGNAL_MASK      0xf0


#define GS_THREAD_CTRL_STARTUP          0x00                /**< Startup thread. */
#define GS_THREAD_CTRL_CLOSE            0x01                /**< Close thread. */
#define GS_THREAD_CTRL_CHANGE_PRIORITY  0x02                /**< Change thread priority. */
#define GS_THREAD_CTRL_INFO             0x03                /**< Get thread information. */
#define GS_THREAD_CTRL_BIND_CPU         0x04                /**< Set thread bind cpu. */
        
#define GS_THREAD_CTRL_STARTUP          0x00                /**< Startup thread. */
#define GS_THREAD_CTRL_CLOSE            0x01                /**< Close thread. */
#define GS_THREAD_CTRL_CHANGE_PRIORITY  0x02                /**< Change thread priority. */
#define GS_THREAD_CTRL_INFO             0x03                /**< Get thread information. */



//#if GS_USING_SEMAPHORE
struct  gs_semaphore
{       
        char name [GS_NAME_MAX] ;
		gs_uint8_t flag;
		gs_list_t suspend_thread;        //�������
		gs_uint16_t value;
};
typedef struct gs_semaphore *gs_sem_t;

struct  gs_mutex
{       
        char name [GS_NAME_MAX] ;
		gs_uint8_t   flag;
		gs_uint16_t  value;
		gs_uint8_t   original_priority;    //���л��������̵߳�ԭʼ���ȼ�
		gs_uint8_t   hold;                 //���л��������̵߳ĳ��д���
	    
		gs_list_t suspend_thread;        //�������
		struct gs_thread *owner;              //���л��������߳� 
};
typedef struct gs_mutex *gs_mutex_t;


//#endif




#endif  /* __GS_DEF_H__ */
