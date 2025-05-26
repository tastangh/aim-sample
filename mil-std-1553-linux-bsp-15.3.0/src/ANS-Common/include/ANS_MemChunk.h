/* SPDX-FileCopyrightText: 2013-2021 AIM GmbH <info@aim-online.com> */
/* SPDX-License-Identifier: MIT  */

/*! \file ANS_MemChunk.h
 *
 * \brief Memory Chunk Definition
 *
 * This module provides memory chunk access functions
 * used by ANS
 *
 *
 ******************************************************************************/
#ifndef ANS_MEMCHUNK_H
#define ANS_MEMCHUNK_H

/* Includes *******************************************************************/

#include <stdlib.h>
#include "Ai_cdef.h"

/* Constants ******************************************************************/

/* Macros *********************************************************************/

/* Type definitions ***********************************************************/

/**
 * Definition of a memory chunk.
 */
typedef struct tagMemChunk_t
{
    AiUInt32    initMarker;         /*!< initialization marker          */
    void        *pMemory;           /*!< allocated memory               */
    size_t      chunkSize;          /*!< size of allocated memory       */

} MemChunk;

/* Prototypes *****************************************************************/

#ifdef __cplusplus
extern "C" {
#endif

extern void MemChunk_init (
    MemChunk        *pChunk);

extern AiBoolean    MemChunk_allocate (
/*@out@*/
    MemChunk        *pChunk,
    size_t          nBytes);

extern AiBoolean    MemChunk_reallocate (
    MemChunk        *pChunk,
    size_t          nBytes);

extern void         MemChunk_free (
    MemChunk        *pChunk);

#ifdef __cplusplus
}
#endif

#endif  /* ANS_MEMCHUNK_H */
