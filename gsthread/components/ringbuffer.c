#include "gsthread.h"
#include "ringbuffer.h"

void gs_ringbuffer_init(struct gs_ringbuffer * rb , gs_uint8_t * pool , gs_uint16_t size )
{
        rb->read_mirror=0;
		rb->write_mirror=0;
		rb->read_index=0;
		rb->write_index=0;
	
		rb->ringbuffer=pool; //指向内存池
		rb->buffer_size = GS_ALIGN_DOWN(size, 4);
}

gs_size_t gs_ringbuffer_getchar(struct gs_ringbuffer * rb , gs_uint8_t *ch)//将ch字符从环形队列中读出
{
    *ch =  rb->ringbuffer[rb->read_index];
    if(rb->read_index == rb->buffer_size - 1)
    {
        rb->read_index = 0;
    }
    else 
    {
        rb->read_index ++ ;
    }   
    return 1;   
}

gs_size_t gs_ringbuffer_putchar(struct gs_ringbuffer * rb , gs_uint8_t ch)//将ch写入到环形队列中
{
    rb->ringbuffer[rb->write_index] = ch;
    if(rb->write_index == rb->buffer_size - 1)
    {
        rb->write_index = 0;
    }
    else 
    {
        rb->write_index ++;
    }
    return 1 ;
}

gs_size_t gs_ringbuffer_isempty(struct gs_ringbuffer * rb) //判断环形队列是否为空
{
    return rb->write_index == rb->read_index ;
}