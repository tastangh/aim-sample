/* SPDX-FileCopyrightText: 2011-2021 AIM GmbH <info@aim-online.com> */
/* SPDX-License-Identifier: MIT  */

/* SmartLib.h */

#pragma once

AiUInt16 uw_SLGetPrivateProfileInt(  char *sz_SectionName, /* section name */
                                     char *sz_KeyName,     /* key name */
                                     AiUInt16 uw_Default,  /* return value if key name not found */
                                     char *sz_FileName     /* initialization file name */
                                     );

AiUInt32 ul_SLGetPrivateProfileString( char *sz_SectionName,    /* section name */
                                       char *sz_KeyName,        /* key name */
                                       char *sz_Default,        /* default string */
                                       char *sz_ReturnedString, /* destination buffer */
                                       AiUInt32 ul_Size,        /* size of destination buffer */
                                       char *sz_FileName        /* initialization file name */
                                       );
