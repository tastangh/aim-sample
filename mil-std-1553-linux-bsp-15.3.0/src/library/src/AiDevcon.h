/* SPDX-FileCopyrightText: 2001-2022 AIM GmbH <info@aim-online.com> */
/* SPDX-License-Identifier: MIT */

/*! \file AiDevcon.h
 *
 *  This file contains helper functions to search for discrete or analog devices of 3rd party vendors
 *
 */

#include <windows.h>
#include <tchar.h>
#include <stdlib.h>
#include <stdio.h>
#include <setupapi.h>
#include <regstr.h>
//TJ #include <infstr.h>
#include <cfgmgr32.h>
#include <string.h>
#include <malloc.h>
#include <newdev.h>
#include <objbase.h>
#include <strsafe.h>

//#include "msg.h"
//#include "rc_ids.h"

#ifndef ARRAYSIZE
#define ARRAYSIZE(a)                (sizeof(a)/sizeof(a[0]))
#endif


int cmdResources(__in LPCTSTR BaseName, __in LPCTSTR Machine, __in DWORD Flags, __in int argc, __in_ecount(argc) TCHAR* argv[]);




typedef int (*DispatchFunc)(__in LPCTSTR BaseName, __in LPCTSTR Machine, __in DWORD Flags, __in int argc, __in LPTSTR argv[]);
typedef int (*CallbackFunc)(__in HDEVINFO Devs, __in PSP_DEVINFO_DATA DevInfo, __in DWORD Index, __in LPVOID Context);

typedef struct {
    LPCTSTR         cmd;
    DispatchFunc    func;
    DWORD           shortHelp;
    DWORD           longHelp;
} DispatchEntry;

#ifndef ARRAYSIZE
#define ARRAYSIZE(x) (sizeof(x)/sizeof(x[0]))
#endif

#define INSTANCEID_PREFIX_CHAR TEXT('@') // character used to prefix instance ID's
#define CLASS_PREFIX_CHAR      TEXT('=') // character used to prefix class name
#define WILD_CHAR              TEXT('*') // wild character
#define QUOTE_PREFIX_CHAR      TEXT('\'') // prefix character to ignore wild characters
#define SPLIT_COMMAND_SEP      TEXT(":=") // whole word, indicates end of id's

//
// Devcon.exe command line flags
//
#define DEVCON_FLAG_FORCE       0x00000001

void FormatToStream(__in FILE * stream, __in DWORD fmt,...);
void Padding(__in int pad);
int EnumerateDevices(__in LPCTSTR BaseName, __in LPCTSTR Machine, __in DWORD Flags, __in int argc, __in PZPWSTR argv, __in CallbackFunc Callback, __in LPVOID Context);
LPTSTR GetDeviceStringProperty(__in HDEVINFO Devs, __in PSP_DEVINFO_DATA DevInfo, __in DWORD Prop);
LPTSTR GetDeviceDescription(__in HDEVINFO Devs, __in PSP_DEVINFO_DATA DevInfo);
//TJ __drv_allocatesMem(object) LPTSTR * GetDevMultiSz(__in HDEVINFO Devs, __in PSP_DEVINFO_DATA DevInfo, __in DWORD Prop);
//TJ__drv_allocatesMem(object) LPTSTR * GetRegMultiSz(__in HKEY hKey, __in LPCTSTR Val);
//TJ__drv_allocatesMem(object) LPTSTR * GetMultiSzIndexArray(__in __drv_aliasesMem LPTSTR MultiSz);
//TJ void DelMultiSz(__in_opt __drv_freesMem(object) PZPWSTR Array);
//TJ__drv_allocatesMem(object) LPTSTR * CopyMultiSz(__in_opt PZPWSTR Array);

BOOL DumpDevice(__in HDEVINFO Devs, __in PSP_DEVINFO_DATA DevInfo);
BOOL DumpDeviceDescr(__in HDEVINFO Devs, __in PSP_DEVINFO_DATA DevInfo);
BOOL DumpDeviceResources(__in HDEVINFO Devs, __in PSP_DEVINFO_DATA DevInfo);
BOOL DumpDeviceWithInfo(__in HDEVINFO Devs, __in PSP_DEVINFO_DATA DevInfo, __in_opt LPCTSTR Info);


//
// UpdateDriverForPlugAndPlayDevices
//
typedef BOOL (WINAPI *UpdateDriverForPlugAndPlayDevicesProto)(__in HWND hwndParent,
                                                              __in LPCTSTR HardwareId,
                                                              __in LPCTSTR FullInfPath,
                                                              __in DWORD InstallFlags,
                                                              __out_opt PBOOL bRebootRequired
                                                         );
typedef BOOL (WINAPI *SetupSetNonInteractiveModeProto)(__in BOOL NonInteractiveFlag
                                                      );
typedef BOOL (WINAPI *SetupUninstallOEMInfProto)(__in LPCTSTR InfFileName,
                                                 __in DWORD Flags,
                                                 __reserved PVOID Reserved
                                                 );

#if _SETUPAPI_VER >= _WIN32_WINNT_WINXP
typedef BOOL (WINAPI *SetupVerifyInfFileProto)(__in LPCTSTR InfName,
                                               __in_opt PSP_ALTPLATFORM_INFO_V2 AltPlatformInfo,
                                               __inout PSP_INF_SIGNER_INFO InfSignerInfo );
#endif

#ifdef _UNICODE
#define UPDATEDRIVERFORPLUGANDPLAYDEVICES "UpdateDriverForPlugAndPlayDevicesW"
#define SETUPUNINSTALLOEMINF "SetupUninstallOEMInfW"
#else
#define UPDATEDRIVERFORPLUGANDPLAYDEVICES "UpdateDriverForPlugAndPlayDevicesA"
#define SETUPUNINSTALLOEMINF "SetupUninstallOEMInfA"
#endif
#define SETUPSETNONINTERACTIVEMODE "SetupSetNonInteractiveMode"
#define SETUPVERIFYINFFILE "SetupVerifyInfFile"

//
// exit codes
//
#define EXIT_OK      (0)
#define EXIT_REBOOT  (1)
#define EXIT_FAIL    (2)
#define EXIT_USAGE   (3)

