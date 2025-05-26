/* SPDX-FileCopyrightText: 2017-2021 AIM GmbH <info@aim-online.com> */
/* SPDX-License-Identifier: MIT OR GPL-2.0-or-later */

/*! \file Ai_ringbuffer.h
 *
 *  This header file contains inline helper functions for ring buffer implementations
 */

#ifndef _AI_RINGBUFFER_H_
#define _AI_RINGBUFFER_H_

#include "Ai_compiler.h"

/*
// Example for a 2KB circular buffer with PUT/GET/ERROR (Total 512LW)
//
// 1) Create some defines to describe your buffer.
//    The defines need to be equal for both the producer and consumer.
//    PUT/GET and the data can be located at different locations.
#define MY_RINGBUF_ADDRESS  0x0
#define MY_RINGBUF_GET      (MY_RINGBUF_ADDRESS + 0x0)
#define MY_RINGBUF_PUT      (MY_RINGBUF_ADDRESS + 0x4)
#define MY_RINGBUF_ERROR    (MY_RINGBUF_ADDRESS + 0x8)
#define MY_RINGBUF_DATA     (MY_RINGBUF_ADDRESS + 0xC)


#define MY_RINGBUF_ENTRY_SIZE       4
#define MY_RINGBUF_ENTRY_COUNT      509
#define MY_RINGBUF_BYTE_SIZE        (MY_RINGBUF_SIZE * MY_RINGBUF_COUNT)

// 2) Producer/Consumer initialization:
//    Initialize PUT/GET in the shared memory with the MY_RINGBUF_DATA offset.
//    PUT==GET                       => circular queue is empty
//    PUT==GET-MY_RINGBUF_ENTRY_SIZE => circular queue is full
void init()
{
    memory[MY_RINGBUF_GET]   = MY_RINGBUF_DATA;
    memory[MY_RINGBUF_PUT]   = MY_RINGBUF_DATA;
    memory[MY_RINGBUF_ERROR] = 0;
}

//  3) Produce write data:
void produce()
{
    // Read PUT/GET only once and store local copy.
    get = memory[MY_RINGBUF_GET];
    put = memory[MY_RINGBUF_PUT];

    // Calculate available space and available space until end of data buffer
    free_bytes         = ai_ringbuffer_producer_free_bytes(MY_RINGBUF_ENTRY_SIZE, MY_RINGBUF_BYTE_SIZE, put, get);
    free_bytes_til_end = ai_ringbuffer_bytes_to_end(MY_RINGBUF_BYTE_SIZE, MY_RINGBUF_DATA, put);

    // If data to write is more than free space we have several options:
    //     1) Set overflow flag and limit the size to write.
    //     2) Limit the size and write the rest of the data later
    //     3) Return immediately and try again later
    if (write_size > free_bytes)
    {
        memory[MY_RINGBUF_ERROR] = MY_RINGBUF_OVERFLOW;
        write_size = free_bytes;
    }

    // Calculate chunk_1 and chunk_2
    if (data_queue_free_bytes > data_queue_free_bytes_til_end)
    {
        chunk_1 = data_queue_free_bytes_til_end;
        chunk_2 = write_size - data_queue_free_bytes_til_end;
    }
    else
    {
        chunk_1 = write_size;
        chunk_2 = 0;
    }

    // Write data for chunk_1 and chunk_2
    if(chunk_1)
        memory[ MY_RINGBUF_DATA + put ... MY_RINGBUF_DATA + chunk_1 ] = data[0] .. data[chunk_1];

    if(chunk_2)
        memory[ MY_RINGBUF_DATA ... MY_RINGBUF_DATA + chunk_2 ] = data[chunk_2] .. data[write_size];

    // increment local copy of put
    ai_ringbuffer_increment_offset( &put, write_size, MY_RINGBUF_DATA, MY_RINGBUF_BYTE_SIZE);

    // Write back new put to signal data availability
    memory[MY_RINGBUF_PUT] = put;

}




//  4) Consume read data:
void consume()
{
    // Read PUT/GET only once and store local copy.
    get = memory[MY_RINGBUF_GET);
    put = memory[MY_RINGBUF_PUT);

    // Calculate available data and available data until end of data buffer
    available_bytes         = ai_ringbuffer_consumer_available_bytes(MY_RINGBUF_BYTE_SIZE, put, get);
    available_bytes_til_end = ai_ringbuffer_bytes_to_end(MY_RINGBUF_BYTE_SIZE, MY_RINGBUF_DATA, get);

    // If data to read is less than data available we have several options:
    //     1) Set underrun flag and limit the size to read.
    //     2) Limit the size and read the rest of the data later
    //     3) Return immediately and try again later
    if (read_size > available_bytes)
    {
        memory[MY_RINGBUF_ERROR] = MY_RINGBUF_UNDERRUN;
        read_size = available_bytes;
    }

    // Calculate chunk_1 and chunk_2
    // For greedy mode where we want to read all available data substitute read_size below with available_bytes.
    if( read_size > available_bytes_til_end )
    {
        chunk_1 = available_bytes_til_end;
        chunk_2 = read_size - available_bytes_til_end;
    }
    else
    {
        chunk_1 = read_size;
        chunk_2 = 0;
    }

    // Read data for chunk_1 and chunk_2
    if(chunk_1)
        data[0] .. data[chunk_1] = memory[ MY_RINGBUF_DATA + get ... MY_RINGBUF_DATA + chunk_1 ];

    if(chunk_2)
        data[chunk_2] .. data[read_size] = memory[ MY_RINGBUF_DATA ... MY_RINGBUF_DATA + chunk_2 ];

    // increment local copy of get
    ai_ringbuffer_increment_offset( &get, read_size, MY_RINGBUF_DATA, MY_RINGBUF_BYTE_SIZE);

    // Write back new get to signal data availability
    memory[MY_RINGBUF_GET] = get;
}

*/


static AI_INLINE int ai_ringbuffer_consumer_available_bytes(int  buffer_size, int  put, int  get)
{
    if (put >= get)
    {
        /* easy case */
        return (put - get);
    }
    else
    {
        /* handle wrap around */
        return ((put + buffer_size) - get);
    }
}


static AI_INLINE int ai_ringbuffer_producer_free_bytes(int entry_size, int  buffer_size, int  put, int  get)
{
    int bytes = 0;

    bytes = buffer_size - ai_ringbuffer_consumer_available_bytes(buffer_size, put, get);

    /* reserve one entry to detect wrap around conditions */

    if (bytes >= entry_size)
        bytes -= entry_size;
    else
        bytes = 0;

    return bytes;
}

/* provides the free bytes to the buffer end */
static AI_INLINE int ai_ringbuffer_bytes_to_end(int buffer_size, int buffer_start, int offset)
{
    /* end - put => consumer case */
    /* end - put => producer case */
    return (buffer_start + buffer_size) - offset;
}


static AI_INLINE int ai_ringbuffer_increment_offset(int * offset, int increment, int buffer_start, int buffer_size)
{
    if ((*offset + increment) < (buffer_start + buffer_size))
    {
        /* easy case */
        *offset += increment;
    }
    else
    {
        /* wrap around */
        *offset += (increment - buffer_size);
    }

    return 0;
}

#endif /* _AI_RINGBUFFER_H_ */
