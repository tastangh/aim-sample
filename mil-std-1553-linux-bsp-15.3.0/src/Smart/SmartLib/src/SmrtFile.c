/* SPDX-FileCopyrightText: 2011-2021 AIM GmbH <info@aim-online.com> */
/* SPDX-License-Identifier: MIT  */

/*!
SmrtFile.cpp
This module includes all file relevant functions
*/

#include "stdio.h"
#include "stdlib.h"
#include "string.h"
#include "Ai_cdef.h"
#include "SmrtFile.h"

#ifndef false
  #define false 0
#endif
#ifndef true
  #define true 1
#endif

/****************************************************/
/* DLL internal structure                           */
/****************************************************/


/****************************************************/
/* DLL internal prototypes                          */
/****************************************************/


/****************************************************/
/* DLL functions                                    */
/****************************************************/

/*!
This function retrieves an integer associated with a key in the specified section
of an update file. 

@param  sz_SectionName   Pointer to a null-terminated string specifying the name
                         of the section in the update file.
@param  sz_KeyName   Pointer to the null-terminated string specifying the name of
                     the key whose value is to be retrieved. This value is in the
                     form of a string; the uw_SLGetPrivateProfileInt() function converts
                     the string into an integer and returns the integer.
@param  uw_Default   Specifies the default value to return if the key name cannot
                     be found in the update file. 
@param  sz_FileName   Pointer to a null-terminated string that specifies the name
                      of the update file. 

@return The return value is the integer equivalent of the string following the
        specified key name in the specified update file. If the key is
        not found, the return value is the specified default value. If the value
        of the key is less than zero, the return value is zero. 

@remarks This function is nearly equivalent to the according Microsoft Windows function
         GetPrivateProfileInt() included in the Microsoft Platform SDK, but instead
         it is valid for all operating systems (e.g. VxWorks, Linux, etc.) because
         it is coded in ANSI C.
@par
         The function searches the file for a key that matches the name specified
         by the sz_KeyName parameter under the section name specified by the sz_SectionName
         parameter. A section in the update file must have the following form: 
@par
        [section]\n
        key=value\n
            "."\n
            "."\n
            "."

@see ul_SLGetPrivateProfileString()

@author Patrick Giesel @date 21.06.2010
*/
AiUInt16 uw_SLGetPrivateProfileInt(
  char *sz_SectionName, /* section name */
  char *sz_KeyName,     /* key name */
  AiUInt16 uw_Default,  /* return value if key name not found */
  char *sz_FileName     /* initialization file name */
)
{
  char auc_Str[500];
  FILE *p_File = fopen(sz_FileName,"r");
  AiUInt16 uw_RetVal = uw_Default;
  char c_Abort = false;
  char *sz_EqualSign;

  if( NULL != p_File )
  {
    rewind(p_File);
    while( (fgets(auc_Str,80,p_File) != NULL) && !c_Abort )
    {
      if( NULL != strstr(auc_Str,sz_SectionName) ) /* found section */
      {
        while( (fgets(auc_Str,80,p_File) != NULL) && !c_Abort )
        {
          if( NULL != strstr(auc_Str,sz_KeyName) ) /* found key in section */
          {
            sz_EqualSign = strstr(auc_Str,"=");
            uw_RetVal = atoi(sz_EqualSign+1);
            c_Abort = true;
          }

          /* abort, if start of the next section is detected */
          if( auc_Str[0] == '[')
          {
            c_Abort = true;
          }
        }
      }
    }
    fclose(p_File);
  }

  return uw_RetVal;
}

/*!
This function retrieves a string associated with a key in the specified section
of an update file. 

@param  sz_SectionName   Pointer to a null-terminated string specifying the name
                         of the section in the update file.
@param  sz_KeyName   Pointer to the null-terminated string specifying the name of
                     the key whose associated string is to be retrieved. If this
                     parameter is NULL, all key names in the section specified by
                     the sz_SectionName parameter are copied to the buffer specified
                     by the sz_ReturnedString parameter. 
@param  uw_Default   Pointer to a null-terminated default string. If the sz_KeyName key
                     cannot be found in the update file, ul_SLGetPrivateProfileString()
                     copies the default string to the sz_ReturnedString buffer.
                     This parameter cannot be NULL.  
@param  sz_ReturnedString   Pointer to the buffer that receives the retrieved string. 
@param  ul_Size   Specifies the size, in characters, of the buffer pointed to by the
                  sz_ReturnedString parameter. 
@param  sz_FileName   Pointer to a null-terminated string that specifies the name
                      of the update file. 

@return The return value is the number of characters copied to the buffer, not including
        the terminating null character. 
@par
        If neither sz_SectionName nor sz_KeyName is NULL and the supplied destination buffer is
        too small to hold the requested string, the string is truncated and followed by a
        null character, and the return value is equal to nSize minus one. 

@remarks This function is nearly equivalent to the according Microsoft Windows function
         GetPrivateProfileString() included in the Microsoft Platform SDK, but instead
         it is valid for all operating systems (e.g. VxWorks, Linux, etc.) because
         it is coded in ANSI C.
@par
         The function searches the specified update file for a key that matches the name
         specified by the sz_KeyName parameter under the section heading specified by the
         sz_SectionName parameter. If it finds the key, the function copies the corresponding
         string to the buffer. If the key does not exist, the function copies the default
         character string specified by the uw_Default parameter. 
         A section in the update file must have the following form: 
@par
        [section]\n
        key=string\n
            "."\n
            "."\n
            "."

@see uw_SLGetPrivateProfileInt()

@author Patrick Giesel @date 21.06.2010
*/
AiUInt32 ul_SLGetPrivateProfileString(
  char *sz_SectionName,    /* section name */
  char *sz_KeyName,        /* key name */
  char *sz_Default,        /* default string */
  char *sz_ReturnedString, /* destination buffer */
  AiUInt32 ul_Size,        /* size of destination buffer */
  char *sz_FileName        /* initialization file name */
)
{
  char auc_Str[80];
  FILE *p_File = fopen(sz_FileName,"r");
  AiUInt32 ul_RetVal = ul_Size;
  AiUInt32 ul_Len;
  char c_Abort        = false;
  char c_Found        = false;
  char c_FoundSection = false;
  char *sz_EqualSign;

  if( NULL == p_File )
    return(0);
  else
  {
    rewind(p_File);
    while( (fgets(auc_Str,80,p_File) != NULL) && !c_Abort )
    {
      if( NULL != strstr(auc_Str,sz_SectionName) ) /* found section */
      {
        c_FoundSection = true;
        while( (fgets(auc_Str,80,p_File) != NULL) && !c_Abort )
        {
          if( NULL != strstr(auc_Str,"[") ) /* found next section */
            c_Abort = true;
          else
          {
            if( NULL != strstr(auc_Str,sz_KeyName) ) /* found key in section */
            {
              sz_EqualSign = strstr(auc_Str,"=");
              ul_Len = (AiUInt32) strlen(sz_EqualSign+1);
              if( ul_Len > ul_Size )
                ul_Len = ul_Size-1;

              strncpy(sz_ReturnedString, sz_EqualSign+1, ul_Len-1);
              sz_ReturnedString[ul_Len-1] = '\0';
              ul_RetVal = ul_Len;
              c_Abort = true;
              c_Found = true;
            }
          }
        }
        c_Abort = true;
      }
    }
    fclose(p_File);
  }

  if( !c_FoundSection )
    return 0;

  if( !c_Found )
  {
    ul_Len = (AiUInt32) strlen(sz_Default);
    if( ul_Len > ul_Size )
    {
      ul_Len = ul_Size-1;
      strncpy(sz_ReturnedString, sz_Default, ul_Len);
    }
    else
      strcpy(sz_ReturnedString, sz_Default);
    ul_RetVal = ul_Len;
  }

  return ul_RetVal;
}
