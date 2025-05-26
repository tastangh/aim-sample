/* SPDX-FileCopyrightText: 2011-2021 AIM GmbH <info@aim-online.com> */
/* SPDX-License-Identifier: MIT  */

#include "hw/AiMyMon.h"
#include "Ai_cdef.h"
#include "Wrapper.h"
#include "SmartAyE.h"
#include "AiOs.h"



TyMyMonContext *CreateContext( AiUInt32 ulModuleHandle )
{
    TyMyMonUserData  *pxMyMonUser;
    TyMyMonContext *pxMyMonContext;
    pxMyMonUser = (TyMyMonUserData*) malloc( sizeof(TyMyMonUserData));
    pxMyMonContext = (TyMyMonContext*) malloc( sizeof(TyMyMonContext));

    pxMyMonUser->ulModHandle = ulModuleHandle;
    pxMyMonContext->pxUserData              = pxMyMonUser;
    pxMyMonContext->pxUserData->ulModHandle = ulModuleHandle;
    pxMyMonContext->pfReadIo                = ReadIo;
    pxMyMonContext->pfWriteIo               = WriteIo;
    pxMyMonContext->pfReadGlobalExt         = ReadGlobalExt;
    pxMyMonContext->pfWriteGlobalExt        = WriteGlobalExt;
    pxMyMonContext->pfWait                  = Wait;

    return pxMyMonContext;
}

AiBoolean CloseContext( TyMyMonContext *pxMyMonContext)
{
    free(pxMyMonContext->pxUserData);
    free(pxMyMonContext);

    return 0;
}







AiBoolean WriteIo( TyMyMonContext * pContext, AiUInt32 ulByteOffset, AiUInt32 ulData[], AiUInt32 ulLWCount )
{
    AiUInt32 ulIndex = 0;
    AiReturn retVal = 0;

    for( ulIndex = 0; ulIndex < ulLWCount; ulIndex++ )
    {
        retVal = SlWrapWriteBlockMemData( pContext->pxUserData->ulModHandle,
                                   API_MEMTYPE_IO,
                                   ulByteOffset + ulIndex*4,
                                   4,
                                   1,
                                   &ulData[ulIndex] );
        if( retVal != API_OK )
            return AiFalse;
    }

    return AiTrue;
}

AiBoolean WriteGlobalExt( TyMyMonContext * pContext, AiUInt32 ulByteOffset, AiUInt32 ulData[], AiUInt32 ulLWCount )
{
    AiUInt32 ulIndex = 0;
    AiReturn retVal = 0;

    for( ulIndex = 0; ulIndex < ulLWCount; ulIndex++ )
    {
        retVal = SlWrapWriteBlockMemData( pContext->pxUserData->ulModHandle,
                                   API_MEMTYPE_GLOBAL_EXTENSION,
                                   ulByteOffset + ulIndex*4,
                                   4,
                                   1,
                                   &ulData[ulIndex] );
                         
        if( retVal != API_OK )
            return AiFalse;
    }

    return AiTrue;
}

AiBoolean ReadIo( TyMyMonContext * pContext, AiUInt32 ulByteOffset, AiUInt32 ulData[], AiUInt32 ulLWCount )
{
    AiUInt32 ulIndex = 0;
    AiReturn retVal = 0;

    for( ulIndex = 0; ulIndex < ulLWCount; ulIndex++ )
    {
        retVal = SlWrapReadMemLWord( pContext->pxUserData->ulModHandle,
                                   API_MEMTYPE_IO,
                                   ulByteOffset + ulIndex*4,
                                   &ulData[ulIndex] );
                         
        if( retVal != API_OK )
            return AiFalse;
    }

    return AiTrue;
}

AiBoolean ReadGlobalExt( TyMyMonContext * pContext, AiUInt32 ulByteOffset, AiUInt32 ulData[], AiUInt32 ulLWCount )
{
    AiUInt32 ulIndex = 0;
    AiReturn retVal = 0;

    for( ulIndex = 0; ulIndex < ulLWCount; ulIndex++ )
    {
        retVal = SlWrapReadMemLWord( pContext->pxUserData->ulModHandle,
                                   API_MEMTYPE_GLOBAL_EXTENSION,
                                   ulByteOffset + ulIndex*4,
                                   &ulData[ulIndex] );
                         
        if( retVal != API_OK )
            return AiFalse;
    }

    return AiTrue;
}

AiBoolean Wait( TyMyMonContext * pContext, AiUInt32 ulMicroSeconds )
{
    AiOsSleep( ulMicroSeconds );

    return AiTrue;
}


