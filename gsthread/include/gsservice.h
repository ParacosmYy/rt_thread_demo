#ifndef __GS_SERVICE_H__
#define __GS_SERVICE_H__

/* ��֪һ���ṹ������ĳ�Ա�ĵ�ַ�����Ƴ��ýṹ����׵�ַ */
#define gs_container_of(ptr, type, member) \
    ((type *)((char *)(ptr) - (unsigned long)(&((type *)0)->member)))
			
#define gs_list_entry(node, type, member) \
    gs_container_of(node, type, member)


    
    
    
    
    
    
    
    
    
    
#endif 
