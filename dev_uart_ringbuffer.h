#ifndef __DEV_UART_RINGBUFFER_H__
#define __DEV_UART_RINGBUFFER_H__
#include "main.h"

#define rx_ringbuf_size 10

typedef struct ringbuffer_t
{
	volatile unsigned int read_index;           /* 当前读位置 */
	volatile unsigned int write_index;          /* 当前写位置 */  
	unsigned int buffer_size;					/* ringbuffer大小 */
    unsigned char *buffer_ptr;  				/* 指向ringbuffer */    
} ringbuffer_t;

void ringbuffer_init(ringbuffer_t *rb, unsigned char *pool, unsigned int size);
int ringbuffer_write(unsigned char ch, ringbuffer_t *rb);
int ringbuffer_read(unsigned char *ch, ringbuffer_t *rb);
int ringbuffer_all_read2PC(UART_TypeDef *UARTx, ringbuffer_t *rb);

extern unsigned char rx_ringbuf[];
extern ringbuffer_t rx_ringbuf_handle;

#endif
