/* SPDX-FileCopyrightText: 2013-2021 AIM GmbH <info@aim-online.com> */
/* SPDX-License-Identifier: MIT  */

/*! \file ANS_MemChunk.c
 *
 * This module provides memory chunk access functions
 * used by ANS
 *
 *
 ******************************************************************************/

/* Includes *******************************************************************/

#include <stdlib.h>
#include <string.h>

#include "Ai_Validate.h"
#include "ANS_Log.h"
#include "ANS_MemChunk.h"

/* Constants ******************************************************************/

#define INIT_MARKER_VALUE       0x12239131
                                /*!< Memory chunk initialization marker     */
#define RANGE_END_MARKER_VALUE  0x22334411uL
                                /*!< Allocated memory range end marker value */
                                /*!< (used in debug mode to detect access   */
                                /*!< violations)                            */

#if defined(_DEBUG) || defined(DEBUG)
# define USE_RANGE_CHECKS       1   /*!< Flag: Add code for range checks    */
# define RANGE_END_MARKER_SIZE  sizeof(unsigned long)
#else
# define USE_RANGE_CHECKS       0
# define RANGE_END_MARKER_SIZE  0   /*!< size of the range end marker       */
#endif

/* Macros *********************************************************************/

/* Type definitions ***********************************************************/

/* Prototypes *****************************************************************/

#if USE_RANGE_CHECKS
static void performRangeChecks (
    MemChunk        *pChunk,
    char const      *pContext);
#endif

/* Variables ******************************************************************/

#if USE_RANGE_CHECKS
static const unsigned long g_endMarkerValue = RANGE_END_MARKER_VALUE;
#endif

/* Functions ******************************************************************/

/**************************************************************************//**
 * This function initializes the specified memory chunk.
 * It should be called as very first function before accessing the memory 
 * chunk by other functions.
 ******************************************************************************/
extern void MemChunk_init (
    MemChunk        *pChunk)    /*!< [out] Memory chunk to initialize   */
{
    if ( NULL == pChunk )
    {
        ANSLogError("MemChunk_init: NULL\n");
        return;
    }

    (void) memset(pChunk, 0, sizeof(*pChunk));
    pChunk->initMarker = INIT_MARKER_VALUE;
}

/**************************************************************************//**
 * This function initializes the specified memory chunk with an
 * allocated memory.
 ******************************************************************************/
extern AiBoolean    MemChunk_allocate (
    MemChunk        *pChunk,    /*!< [out] Memory chunk to initialize   */
    size_t          nBytes)     /*!< [in] Number of bytes to allocate   */
{
    size_t          bytesNeeded;
    AiBoolean       boolRc = AiTrue;
#if USE_RANGE_CHECKS
    unsigned char   *pRangeEndMarker;
#endif

    VALIDATE_PTR(pChunk, AiFalse);  /** \return AiFalse - invalid arguments */

    MemChunk_init(pChunk);

    if ( nBytes > 0 )
    {
        bytesNeeded = nBytes + RANGE_END_MARKER_SIZE;

        pChunk->pMemory = malloc(bytesNeeded);
        if ( NULL == pChunk->pMemory )
        {
            ANSLogError("MemChunk_allocate: out of memory allocating %lu bytes!", (unsigned long) bytesNeeded);
            boolRc = AiFalse;       /** \return AiFalse - out of memory */
        }
        else
        {
            pChunk->initMarker = INIT_MARKER_VALUE;
            pChunk->chunkSize = nBytes;

#if USE_RANGE_CHECKS
            /*
             * Append the end of memory range marker that allows to detect
             * write overflows when releasing the memory.
             */
            pRangeEndMarker = (unsigned char *) pChunk->pMemory;
            pRangeEndMarker += nBytes;
            memcpy(pRangeEndMarker, &g_endMarkerValue, sizeof(g_endMarkerValue));
#endif
        }
    }
    else
    {
        pChunk->initMarker = INIT_MARKER_VALUE;
        pChunk->pMemory = NULL;
    }

    return boolRc;                  /** \return AiTrue - success        */
}

/**************************************************************************//**
 * This function reallocates memory provided by the specified memory chunk.
 * Note: The memory chunk does not perform any shrink operations.
 ******************************************************************************/
extern AiBoolean    MemChunk_reallocate (
    MemChunk        *pChunk,    /*!< [in,out] Memory chunk to reallocate */
    size_t          nBytes)     /*!< [in] Number of bytes to allocate   */
{
    size_t          bytesNeeded;
    AiBoolean       boolRc = AiFalse;
    void            *tmpPtr;
#if USE_RANGE_CHECKS
    unsigned char   *pRangeEndMarker;
#endif

    VALIDATE_PTR(pChunk, AiFalse);  /** \return AiFalse - invalid arguments */

    if ( INIT_MARKER_VALUE != pChunk->initMarker || 0 == pChunk->chunkSize )
    {
        boolRc = MemChunk_allocate(pChunk, nBytes);
        return boolRc;
    }

#if USE_RANGE_CHECKS
    performRangeChecks(pChunk, "reallocate");
#endif

    /* Don't shrink an existing chunk */
    if ( pChunk->chunkSize > nBytes )
    {
        boolRc = AiTrue;
        return boolRc;
    }

    bytesNeeded = nBytes + RANGE_END_MARKER_SIZE;

    /* Try to allocate a new chunk */
    tmpPtr = realloc(pChunk->pMemory, bytesNeeded);
    if ( NULL == tmpPtr )
    {
        ANSLogError("MemChunk_reallocate: out of memory allocating %lu bytes!", (unsigned long) bytesNeeded);
        boolRc = AiFalse;       /** \return AiFalse - out of memory */
    }
    else
    {
        pChunk->pMemory = tmpPtr;
        pChunk->chunkSize = nBytes;
        boolRc = AiTrue;

#if defined(_DEBUG) || defined(DEBUG)
        /*
         * Append the end of memory range marker that allows to detect
         * write overflows when releasing the memory.
         */
        pRangeEndMarker = (unsigned char *) pChunk->pMemory;
        pRangeEndMarker += nBytes;
        memcpy(pRangeEndMarker, &g_endMarkerValue, sizeof(g_endMarkerValue));
#endif
    }

    return boolRc;                  /** \return AiTrue - success        */
}

/**************************************************************************//**
 * This function reallocates the memory provided by the specified memory chunk.
 ******************************************************************************/
extern void         MemChunk_free (
    MemChunk        *pChunk)    /*!< [in,out] Memory chunk to release */
{
    VALIDATE_PTR_VOID(pChunk);

    if ( INIT_MARKER_VALUE != pChunk->initMarker || 0 == pChunk->chunkSize )
    {
        ANSLogDebug("MemChunk_free: not allocated: 0x%p",  pChunk);
        return;
    }

#if USE_RANGE_CHECKS
    performRangeChecks(pChunk, "free");
#endif

    if ( NULL != pChunk->pMemory && pChunk->chunkSize > 0 )
    {
        free(pChunk->pMemory);
    }

    pChunk->chunkSize = 0;
    pChunk->pMemory = NULL;
    pChunk->initMarker = 0;
}

#if USE_RANGE_CHECKS
/**************************************************************************//**
 * This function checks the range of allocated memory for any access 
 * violations that may occur as result of out-of-bounds memory writes.
 *****************************************************************************/
static void performRangeChecks (
    MemChunk        *pChunk,    /*!< [in] Memory chunk                  */
    char const      *pContext)  /*!< [in] Context of memory use         */
{
    unsigned long   value;
    unsigned char   *pRangeEndMarker;

    if ( NULL == pChunk || NULL == pChunk->pMemory || pChunk->chunkSize < 1 )
        return;

    pRangeEndMarker = (unsigned char *) pChunk->pMemory;
    pRangeEndMarker += pChunk->chunkSize;
    if ( 0 == memcmp(
        pRangeEndMarker, &g_endMarkerValue, sizeof(g_endMarkerValue)) )
    {
        return;
    }

    (void) memcpy(&value, pRangeEndMarker, sizeof(value));
    ANSLogError("ERR: Corrupted MemChunk detected on %.32s: 0x%p",
        pContext, pChunk);
    ANSLogError("ERR: Marker expected: 0x%08lX - got 0x%08lX",
        (unsigned long) RANGE_END_MARKER_VALUE , (unsigned long) value);
}
#endif
