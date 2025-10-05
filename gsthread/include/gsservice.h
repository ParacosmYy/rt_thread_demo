#ifndef __GS_SERVICE_H__
#define __GS_SERVICE_H__

/* 已知一个结构体里面的成员的地址，反推出该结构体的首地址 */
#define gs_container_of(ptr, type, member) \
    ((type *)((char *)(ptr) - (unsigned long)(&((type *)0)->member)))
			
#define gs_list_entry(node, type, member) \
    gs_container_of(node, type, member)


    
    
    
    
    
    
    
    
    
    
#endif 
