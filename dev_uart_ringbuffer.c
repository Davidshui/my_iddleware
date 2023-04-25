#include "dev_uart_ringbuffer.h"

/**
 * @file UArt_RX_ringbuffer.c
 * @author your name 
 * @brief 实现环形缓冲区的形式有使用数组的，也可以使用链表。我这里为了实现简单，就用数组作为 ringbuffer 的内存来实现。
          在实现 ringbuffer 时，要有两个指针，读指针和写指针。每当向 ringbuffer 中写入一个数据时，写指针加1；同理从 ringbuffer 中读取一个数据时，读指针加1。
          对于 ringbuffer 的读写操作，我们有重点问题需要考虑：1.读写指针移动到 ringbuffer 的最大长度之后，如何返回首位置？
                                                            答：ringbuffer 的读写指针位置的计算，精髓就在于对读写指针进行取模运算。即当读写指针移动一个位置时，
                                                                然后对 ringbuffer 的大小进行取模运算，这样当读写指针移动到最末尾时，取模运算的结果就是 0，即返回的 ringbuffer 的首位置了。
                                                                （取模运算是求两个数相除的余数）        
 * @version 0.1
 * @date 2023-01-17
 * 
 * @copyright Copyright (c) 2023
 * 
 */
//#include <stdio.h>


unsigned char rx_ringbuf[rx_ringbuf_size] = {0};
ringbuffer_t rx_ringbuf_handle = {0};

/*
 * 函数作用 : 初始化ringbuffer结构体（句柄）
 * 参数  rb   : 指向ringbuffer句柄
 * 参数  pool : 指向ringbuffer缓冲区，用户调用时一般提供一个数组
 * 参数  size : 缓冲区的大小 
 * 返回值 : 无
 */
void ringbuffer_init(ringbuffer_t *rb, unsigned char *pool, unsigned int size)
{
    /* initialize read and write index */
    rb->read_index = 0;
    rb->write_index = 0;

    /* set buffer pool and size */
    rb->buffer_ptr = pool;
    rb->buffer_size = size;
}

/*
 * 函数作用 : 向目标缓冲区(ch)写入一个字节数据
 * 参数  ch : 要写入ringbuffer的数据
 * 参数  rb : 指向ringbuffer句柄
 * 返回值   : 写入成功返回0，失败返回-1
 */
int ringbuffer_write(unsigned char ch, ringbuffer_t *rb)
{
    if (rb->read_index == ((rb->write_index + 1) % rb->buffer_size))    //当写指针的下一个位置等于读指针的位置时，那么 ringbuffer 为满。
    {
        return -1;
    }
    else
    {
        rb->buffer_ptr[rb->write_index] = ch;
        rb->write_index = (rb->write_index + 1) % rb->buffer_size;
        return 0;
    }
}

/*
 * 函数作用 : 向目标缓冲区读取一个字节数据
 * 参数  ch : 把读取到的数据保存到ch所指向的内存
 * 参数  rb : 指向ringbuffer句柄
 * 返回值   : 读取成功返回0，失败返回-1
 */
int ringbuffer_read(unsigned char *ch, ringbuffer_t *rb)
{
    if (rb->read_index == rb->write_index)  //读写指针的位置相等时，说明 ringbuffer 为空。
    {
        return -1;
    }
    else
    {
        *ch = rb->buffer_ptr[rb->read_index];
        rb->read_index = (rb->read_index + 1) % rb->buffer_size;
        return 0;
    }
}

/**
 * @brief 将ringbuffer所保存的数据全部通过串口发出去
 * 
 * @param UARTx 哪一个串口
 * @param rb 指向ringbuffer句柄
 * @return int 0：发送成功
 */
int ringbuffer_all_read2PC(UART_TypeDef *UARTx, ringbuffer_t *rb)
{
    unsigned char ch_temp;

    if (rb->read_index == rb->write_index)  //读写指针的位置相等时，说明 ringbuffer 为空。
    {
        return -1;
    }

    do{       
        ringbuffer_read(&ch_temp, rb);
        UART_WriteByte(UARTx, (uint32_t)ch_temp);   //发送一个一个字节的数据
    }while(rb->read_index != rb->write_index);

    
    return 0;
    
}





