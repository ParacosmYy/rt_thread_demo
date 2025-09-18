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

struct gs_list_node 
{
    struct gs_list_node *prev;
    struct gs_list_node *next;
};
typedef struct gs_list_node gs_list_t ;			

struct gs_object
{
    char name [GS_NAME_MAX] ;
    gs_uint8_t type ;
    gs_uint8_t flags; 
    gs_list_t list;
};
typedef struct gs_object *gs_object_t ;


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
};
typedef struct gs_thread *gs_thread_t;

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
#endif  /* __GS_DEF_H__ */
