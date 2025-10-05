#ifndef __LIST_H__
#define __LIST_H__

#include <gsdef.h>
	

gs_inline void gs_list_init(gs_list_t* list)
{
    list->prev = list ;
    list->next = list ;
}


gs_inline void gs_list_insert_after(gs_list_t* list, gs_list_t* new_node)
{
    list->next->prev = new_node;
    new_node->next = list->next;
    list->next = new_node ;
    new_node->prev = list ;
}

gs_inline void gs_list_insert_before(gs_list_t* list, gs_list_t* new_node)
{
    list->prev->next = new_node; // 前一个节点的 next 指向新节点
    new_node->prev = list->prev; // 新节点的prev指向前一个节点
    
   // new_node->next = list;
    list->prev = new_node;       // 当前节点的prev指向新节点
    new_node->next = list;       // 新节点的next指向当前节点
}

gs_inline void gs_list_remove(gs_list_t* new_node)
{
    new_node->next->prev = new_node->prev;
    new_node->prev->next = new_node->next;
   //new_node->prev = new_node->next;
   // new_node->prev=new_node->next=new_node;
    new_node->next = new_node ;
    new_node->prev = new_node ;
}

gs_inline int gs_list_isempty(const gs_list_t *l)
{
    return l->next == l;
}

#endif
