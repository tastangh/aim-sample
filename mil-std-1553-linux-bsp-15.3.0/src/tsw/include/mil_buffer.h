/* SPDX-FileCopyrightText: 2016-2022 AIM GmbH <info@aim-online.com> */
/* SPDX-License-Identifier: MIT OR GPL-2.0-or-later */

/*! \file mil_buffer.h
 *
 *  AIM ringbuffer head file
 *
 */

#ifndef MIL_BUFFER_H_
#define MIL_BUFFER_H_






static AiUInt32 mil_tsw_buffer_consumer_available_bytes(AiUInt32  buffer_size, AiUInt32  put, AiUInt32  get)
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





static AiUInt32 mil_tsw_buffer_producer_free_bytes(AiUInt32 entry_size, AiUInt32  buffer_size, AiUInt32  put, AiUInt32  get)
{
    AiUInt32 bytes = 0;

    bytes = buffer_size - mil_tsw_buffer_consumer_available_bytes(buffer_size, put, get);

    /* reserve one Entry to detect wrap around conditions */

    if (bytes >= entry_size)
        bytes -= entry_size;
    else
        bytes = 0;

    return bytes;
}

/* provides the free bytes to the buffer end */
static AiUInt32 mil_tsw_buffer_bytes_to_end(AiUInt32 buffer_size, AiUInt32 buffer_start, AiUInt32 offset)
{
    /* end - put => consumer case */
    /* end - put => producer case */
    return (buffer_start + buffer_size) - offset;
}


static AiUInt32 mil_tsw_buffer_increment_offset(AiUInt32 * offset, AiUInt32 increment, AiUInt32 buffer_start, AiUInt32 buffer_size)
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












#endif /* MIL_BUFFER_H_ */
