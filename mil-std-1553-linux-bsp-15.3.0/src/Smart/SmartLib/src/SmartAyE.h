/* SPDX-FileCopyrightText: 2011-2021 AIM GmbH <info@aim-online.com> */
/* SPDX-License-Identifier: MIT  */

#ifndef __SMART_AYE_H__
#define __SMART_AYE_H__

#ifndef _AIM_VME
#pragma warning (disable : 4996 ) /* obsolte sscanf */
#endif


/* function ApiReadMem...() */
/* function ApiWriteMem...() */
#define API_MEMTYPE_GLOBAL           0
#define API_MEMTYPE_SHARED           1
#define API_MEMTYPE_LOCAL            2
#define API_MEMTYPE_IO               3
#define API_MEMTYPE_GLOBAL_UNCACHED  4
#define API_MEMTYPE_GLOBAL_EXTENSION 5


#define API_OK 0


typedef struct TyMyMonUserData 
{
    AiUInt32       ulModHandle;
}TyMyMonUserData;



AiBoolean WriteIo(        TyMyMonContext * pContext, AiUInt32 ulByteOffset, AiUInt32 ulData[], AiUInt32 ulLWCount );
AiBoolean WriteGlobalExt( TyMyMonContext * pContext, AiUInt32 ulByteOffset, AiUInt32 ulData[], AiUInt32 ulLWCount );
AiBoolean ReadIo(         TyMyMonContext * pContext, AiUInt32 ulByteOffset, AiUInt32 ulData[], AiUInt32 ulLWCount );
AiBoolean ReadGlobalExt(  TyMyMonContext * pContext, AiUInt32 ulByteOffset, AiUInt32 ulData[], AiUInt32 ulLWCount );
AiBoolean WriteIo(        TyMyMonContext * pContext, AiUInt32 ulByteOffset, AiUInt32 ulData[], AiUInt32 ulLWCount );
AiBoolean Wait(           TyMyMonContext * pContext, AiUInt32 ulMicroSeconds );

/* prototype */
TyMyMonContext *CreateContext( AiUInt32 ulModuleHandle );
AiBoolean CloseContext( TyMyMonContext *pxMyMonContext);


#endif /* __SMART_AYE_H__ */







