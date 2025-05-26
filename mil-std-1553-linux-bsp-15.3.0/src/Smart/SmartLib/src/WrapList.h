/* SPDX-FileCopyrightText: 2011-2021 AIM GmbH <info@aim-online.com> */
/* SPDX-License-Identifier: MIT  */

/*!
WrapList.h
Contains the definition of the module information list structure and the prototypes to handle it.
*/


#pragma once

/*!
This structure defines a module information list entry.

@author Patrick Giesel @date 25.10.2010
*/
typedef struct
{
  /*! Module Information returned by SlWrapReadModuleInfo() */
  TY_MODULE_INFO x_ModuleInfo; 
  /*! Module handle returned by SlWrapOpenModule() */
  AiUInt32       ul_ModuleHandle; 
  /*! Pointer to the previous list entry */
  AiUInt32       *prev; 
  /*! Pointer to the next list entry */
  AiUInt32       *next; 
} TY_DLL_MODULE_INFO_LIST;

_API_EXTERN unsigned char uc_IsAlreadyOpened              ( AiUInt32 ul_ModuleHandle, TY_DLL_MODULE_INFO_LIST **ppx_ListEntry );
_API_EXTERN unsigned char uc_AddDLLModuleInfoListEntry    ( AiUInt32 ul_ModuleHandle );
_API_EXTERN unsigned char uc_RemoveDLLModuleInfoListEntry ( AiUInt32 ul_ModuleHandle );
_API_EXTERN unsigned char uc_GetDLLModuleInfoListEntry    ( AiUInt32 ul_ModuleHandle, TY_DLL_MODULE_INFO_LIST **ppx_ListEntry );
