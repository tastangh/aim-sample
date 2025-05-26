/* SPDX-FileCopyrightText: 2011-2021 AIM GmbH <info@aim-online.com> */
/* SPDX-License-Identifier: MIT  */

/*!
WrapList.cpp
Contains interface functions for the module information list handling.
*/

#ifdef _AIM_WINDOWS
#include "Windows.h"
#endif

#include <stdlib.h>
#include <string.h>
#include <Ai_cdef.h>
#include "Wrapper.h"
#include "WrapList.h"

/****************************************************/
/* DLL internal structure                           */
/****************************************************/
/*!
This is the anchor of the module information list.
*/
TY_DLL_MODULE_INFO_LIST *px_DllModuleInfoList = NULL;


/****************************************************/
/* DLL functions                                    */
/****************************************************/

/*!
Examines if the board referenced with parameter "ul_ModuleHandle" has already been opened (that means: a list entry exists).
If opened it returns the according module information list entry.

@param[in]  ul_ModuleHandle   Board Module Handle that is returned
                              by the protocol specific AIM API!
@param [in, out]  ppx_ListEntry    Returnes a pointer to the module information list entry of this board.
                                   If the board has not been opened or if ppx_ListEntry is a NULL pointer
								   this parameter is not modified.


@return 0 (false) Board has not been opened yet.\n
        1 (true)  Board has already been opened.

@remarks n/a 

@see uc_AddDLLModuleInfoListEntry\n
     uc_RemoveDLLModuleInfoListEntry()\n
     uc_GetDLLModuleInfoListEntry()

@author Patrick Giesel @date 25.10.2010
*/
AiUInt8 uc_IsAlreadyOpened( AiUInt32 ul_ModuleHandle, TY_DLL_MODULE_INFO_LIST **ppx_ListEntry )
{
  AiUInt8 uc_Opened = AiFalse;
  TY_DLL_MODULE_INFO_LIST *px_TempList = px_DllModuleInfoList;

  while( (px_TempList != NULL) && (AiFalse == uc_Opened))
  {
    if( px_TempList->ul_ModuleHandle == ul_ModuleHandle )
      uc_Opened = AiTrue;
    else
      px_TempList = (TY_DLL_MODULE_INFO_LIST *)px_TempList->next;
  }
  if( (AiTrue == uc_Opened) &&
      (NULL != ppx_ListEntry) )
    *ppx_ListEntry = px_TempList;

  return uc_Opened;
}

/*!
Creates and inserts a new module information list entry to the list according to the given
parameter "ul_ModuleHandle". The entry's information is cleared and has to be filled with data later.

@param[in]  ul_ModuleHandle   Board Module Handle that is returned
                              by the protocol specific AIM API!

@return 0 Succesful\n
        1 Function executed with error

@remarks If the module information list already exists, this function has no effect.
         The module information list entry is unchanged in this case!

@see uc_IsAlreadyOpened()\n
     uc_RemoveDLLModuleInfoListEntry()\n
     uc_GetDLLModuleInfoListEntry()

@author Patrick Giesel @date 25.10.2010
*/
AiUInt8 uc_AddDLLModuleInfoListEntry( AiUInt32 ul_ModuleHandle )
{
  AiUInt8 uc_RetVal = 0;
  TY_DLL_MODULE_INFO_LIST *px_TempListEntry;
  TY_DLL_MODULE_INFO_LIST *px_DllModuleInfoListTemp = px_DllModuleInfoList;

  if( AiTrue == uc_IsAlreadyOpened( ul_ModuleHandle, &px_TempListEntry ) )
    uc_RetVal = 0;
  else
  {
    /* there is no entry in the list */
    if(NULL == px_DllModuleInfoList)
    {
      px_DllModuleInfoList   = (TY_DLL_MODULE_INFO_LIST *)malloc(sizeof(TY_DLL_MODULE_INFO_LIST));
      px_TempListEntry       = px_DllModuleInfoList;
      px_TempListEntry->prev = NULL;
      px_TempListEntry->next = NULL;
    }
    else
    { /* there is at least one entry in the list --> search the last entry */
      while( px_DllModuleInfoListTemp != NULL )
      {
        if(px_DllModuleInfoListTemp->next)
          px_DllModuleInfoListTemp = (TY_DLL_MODULE_INFO_LIST *)px_DllModuleInfoListTemp->next;
        else
          break;
      }
      px_DllModuleInfoListTemp->next = (AiUInt32 *)malloc(sizeof(TY_DLL_MODULE_INFO_LIST));
      px_TempListEntry               = (TY_DLL_MODULE_INFO_LIST *)px_DllModuleInfoListTemp->next;
      px_TempListEntry->prev         = (AiUInt32 *)px_DllModuleInfoListTemp;
      px_TempListEntry->next         = NULL;
    }

    px_TempListEntry->ul_ModuleHandle = ul_ModuleHandle;
    memset(&px_TempListEntry->x_ModuleInfo, 0, sizeof(TY_MODULE_INFO));
  }
  return uc_RetVal;
}

/*!
Deletes an existing module information list entry from the list according to the given parameter "ul_ModuleHandle".

@param[in]  ul_ModuleHandle   Board Module Handle that is returned
                              by the protocol specific AIM API!

@return 0 Succesful\n
        1 Function executed with error

@remarks n/a 

@see uc_IsAlreadyOpened()\n
     uc_AddDLLModuleInfoListEntry()\n
     uc_GetDLLModuleInfoListEntry()

@author Patrick Giesel @date 25.10.2010
*/
AiUInt8 uc_RemoveDLLModuleInfoListEntry( AiUInt32 ul_ModuleHandle )
{
  AiUInt8 uc_RetVal = 1;
  TY_DLL_MODULE_INFO_LIST *px_DllModuleInfoListTemp = px_DllModuleInfoList;
  TY_DLL_MODULE_INFO_LIST *px_TempList;
  TY_DLL_MODULE_INFO_LIST *px_TempList2;

  if(NULL == px_DllModuleInfoListTemp)
    return 0;

  while(px_DllModuleInfoListTemp != NULL)
  {
    if( px_DllModuleInfoListTemp->ul_ModuleHandle == ul_ModuleHandle )
    { /* entry found -> remove it */
      uc_RetVal         = 0;
      px_TempList       = (TY_DLL_MODULE_INFO_LIST *)px_DllModuleInfoListTemp->prev;
      if( NULL == px_TempList )
      { /* we remove the first item in the list */
        px_DllModuleInfoList       = (TY_DLL_MODULE_INFO_LIST *)px_DllModuleInfoListTemp->next;
        if(NULL != px_DllModuleInfoList)
          px_DllModuleInfoList->prev = NULL;
        free(px_DllModuleInfoListTemp);
        px_DllModuleInfoListTemp = px_DllModuleInfoList;
      }
      else
      {
        if( NULL == px_DllModuleInfoListTemp->next )
        { /* we remove the last item in the list */
          px_TempList->next = NULL;
        }
        else
        {
          px_TempList2 = (TY_DLL_MODULE_INFO_LIST *)px_DllModuleInfoListTemp->next;
          px_TempList2->prev = (AiUInt32 *)px_TempList;
          px_TempList->next = px_DllModuleInfoListTemp->next;
        }
        free(px_DllModuleInfoListTemp);
        px_DllModuleInfoListTemp = NULL;
      }
    }
    else
      px_DllModuleInfoListTemp = (TY_DLL_MODULE_INFO_LIST *)px_DllModuleInfoListTemp->next;
  }

  return uc_RetVal;
}

/*!
Returns an existing module information list entry according to the given parameter "ul_ModuleHandle".

@param[in]  ul_ModuleHandle   Board Module Handle that is returned
                              by the protocol specific AIM API!
@param [in, out]  ppx_ListEntry    Returnes a pointer to the module information list entry of this board.

@return 0 Succesful\n
        1 Function executed with error

@remarks n/a 

@see uc_IsAlreadyOpened()\n
     uc_AddDLLModuleInfoListEntry()\n
     uc_RemoveDLLModuleInfoListEntry()

@author Patrick Giesel @date 25.10.2010
*/
AiUInt8 uc_GetDLLModuleInfoListEntry( AiUInt32 ul_ModuleHandle, TY_DLL_MODULE_INFO_LIST **ppx_ListEntry )
{
  AiUInt8 uc_RetVal = 1;
  TY_DLL_MODULE_INFO_LIST *px_DllModuleInfoListTemp = px_DllModuleInfoList;

  if(NULL == px_DllModuleInfoListTemp)
    return uc_RetVal;

  while(px_DllModuleInfoListTemp != NULL)
  {
    if( px_DllModuleInfoListTemp->ul_ModuleHandle == ul_ModuleHandle )
    {
      *ppx_ListEntry = px_DllModuleInfoListTemp;
      uc_RetVal = 0;
      break;
    }
    else
      px_DllModuleInfoListTemp = (TY_DLL_MODULE_INFO_LIST *)px_DllModuleInfoListTemp->next;
  }

  return uc_RetVal;
}

