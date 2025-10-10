#ifndef __RINGBUFFER_H
#define __RINGBUFFER_H

#include "gsthread.h"

enum gs_ringbuffer_state
{
    GS_RINGBUFFER_EMPTY,
    GS_RINGBUFFER_FULL,
    GS_RINGBUFFER_HALFFULL,
};

struct gs_ringbuffer
{
		gs_uint8_t *ringbuffer;
	
		gs_uint16_t read_mirror:1;
		gs_uint16_t read_index:15;
	
		gs_uint16_t write_mirror:1;
		gs_uint16_t write_index:15;
	
		gs_int16_t buffer_size;
}; 
typedef struct gs_ringbuffer * gs_ringbuffer_t ;


void gs_ringbuffer_init(struct gs_ringbuffer * rb , gs_uint8_t * pool , gs_uint16_t size );
gs_size_t gs_ringbuffer_getchar(struct gs_ringbuffer * rb , gs_uint8_t *ch) ;
gs_size_t gs_ringbuffer_putchar(struct gs_ringbuffer * rb , gs_uint8_t ch);
gs_size_t gs_ringbuffer_isempty(struct gs_ringbuffer * rb);

#endif