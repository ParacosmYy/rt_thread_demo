#ifndef __GS_DEF_H__
#define __GS_DEF_H__

#include <gsconfig.h>


/*
*************************************************************************
*                               数据类型
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


/* 布尔类型宏定义 */
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
*                               错误码定义
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


//双向链表节点
struct gs_list_node 
{
    struct gs_list_node *prev;
    struct gs_list_node *next;
};
typedef struct gs_list_node gs_list_t ;			
//内核对象基类
struct gs_object
{
    char name [GS_NAME_MAX] ;
    gs_uint8_t type ;
    gs_uint8_t flag; 
    gs_list_t list;
};
typedef struct gs_object *gs_object_t ;




/**
 * 时钟 & 定时器 宏定义
 */
#define GS_TIMER_FLAG_DEACTIVATED       0x0     /* 定时器没有激活 */
#define GS_TIMER_FLAG_ACTIVATED         0x1     /* 定时器已经激活 */
#define GS_TIMER_FLAG_ONE_SHOT          0x0     /* 单次定时 */
#define GS_TIMER_FLAG_PERIODIC          0x2     /* 周期定时 */
        
#define GS_TIMER_FLAG_HARD_TIMER        0x0     /* 硬件定时器，定时器回调函数在 tick isr中调用 */
        
#define GS_TIMER_FLAG_SOFT_TIMER        0x4     /* 软件定时器，定时器回调函数在定时器线程中调用 */
        
        
#define GS_TIMER_CTRL_SET_TIME          0x0     /* 设置定时器定时时间 */
#define GS_TIMER_CTRL_GET_TIME          0x1     /* 获取定时器定时时间 */
#define GS_TIMER_CTRL_SET_ONESHOT       0x2     /* 修改定时器为一次定时 */
#define GS_TIMER_CTRL_SET_PERIODIC      0x3     /* 修改定时器为周期定时 */
        
#ifndef GS_TIMER_SKIP_LIST_LEVEL
#define GS_TIMER_SKIP_LIST_LEVEL          1
#endif

/* 1 or 3 */
#ifndef GS_TIMER_SKIP_LIST_MASK
#define GS_TIMER_SKIP_LIST_MASK         0x3
#endif

//定时器结构体
struct gs_timer
{
    struct gs_object parent;                         /* 从 rt_object 继承 */

    gs_list_t row[GS_TIMER_SKIP_LIST_LEVEL];// 定时器跳表节点

    void (*timeout_func)(void *parameter);           /* 超时函数 */
    void            *parameter;                      /* 超时函数形参 */

    gs_tick_t        init_tick;                      /* 定时器实际需要延时的时间 */
    gs_tick_t        timeout_tick;                   /* 定时器实际超时时的系统节拍数 */
};
typedef struct gs_timer *gs_timer_t;

/**
 * 对象类型由下面的宏来使能，这些宏通常在gsconfig.h中定义
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
     GS_Object_Class_Thread = 0,       /* 对象是线程 */
     GS_Object_Class_Semaphore,        /* 对象是信号量 */
     GS_Object_Class_Mutex,            /* 对象是互斥量 */
     GS_Object_Class_Event,            /* 对象是事件 */
     GS_Object_Class_MailBox,          /* 对象是邮箱 */
     GS_Object_Class_MessageQueue,     /* 对象是消息队列 */
     GS_Object_Class_MemHeap,          /* 对象是内存堆 */
     GS_Object_Class_MemPool,          /* 对象是内存池 */
     GS_Object_Class_Device,           /* 对象是设备 */
     GS_Object_Class_Timer,            /* 对象是定时器 */
     GS_Object_Class_Module,           /* 对象是模块 */
     GS_Object_Class_Unknown,          /* 对象未知 */
     GS_Object_Class_Static = 0x80     /* 对象是静态对象 */
};



/**
 * 内核对象信息结构体
 */
struct gs_object_information
{
    enum gs_object_class_type type;                     /* 对象类型 */
    gs_list_t                 object_list;              /* 对象列表节点 */
    gs_size_t                 object_size;              /* 对象大小 */
};
//线程控制块结构体
struct gs_thread
{
    char name [GS_NAME_MAX] ;
    gs_uint8_t type;
    gs_uint8_t flags;
    gs_list_t list;
    
    
	void        *sp;	          /* 线程栈指针 */
	void        *entry;	          /* 线程入口地址 */
	void        *parameter;	      /* 线程形参 */	
	void        *stack_addr;      /* 线程起始地址 */
	gs_uint32_t stack_size;       /* 线程栈大小，单位为字节 */
    

	
	gs_list_t   tlist;            /* 线程链表节点 */
    
    gs_ubase_t remaining_tick ;
    gs_ubase_t init_tick;
    
    
    gs_uint8_t  current_priority;     /* 当前优先级 */
    gs_uint8_t  init_priority;        /* 初始优先级 */
    gs_uint32_t number_mask;          /* 当前优先级掩码 */
    
    gs_err_t    error;                /* 错误码 */
    gs_uint8_t  stat;                 /* 线程的状态 */
    
    struct gs_timer thread_timer;     /* 内置的线程定时器 */
};
typedef struct gs_thread *gs_thread_t;

/*
 * 线程状态定义
 */
#define GS_THREAD_INIT                  0x00                /* 初始态 */
#define GS_THREAD_READY                 0x01                /* 就绪态 */
#define GS_THREAD_SUSPEND               0x02                /* 挂起态 */
#define GS_THREAD_RUNNING               0x03                /* 运行态 */
#define GS_THREAD_BLOCK                 GS_THREAD_SUSPEND   /* 阻塞态 */
#define GS_THREAD_CLOSE                 0x04                /* 关闭态 */
#define GS_THREAD_STAT_MASK             0x0f// 线程状态掩码
        
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
		gs_list_t suspend_thread;        //挂起队列
		gs_uint16_t value;
};
typedef struct gs_semaphore *gs_sem_t;

struct  gs_mutex
{       
        char name [GS_NAME_MAX] ;
		gs_uint8_t   flag;
		gs_uint16_t  value;
		gs_uint8_t   original_priority;    //持有互斥量的线程的原始优先级
		gs_uint8_t   hold;                 //持有互斥量的线程的持有次数
	    
		gs_list_t suspend_thread;        //挂起队列
		struct gs_thread *owner;              //持有互斥量的线程 
};
typedef struct gs_mutex *gs_mutex_t;


//#endif




#endif  /* __GS_DEF_H__ */
