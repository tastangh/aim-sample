/* SPDX-FileCopyrightText: 2001-2022 AIM GmbH <info@aim-online.com> */
/* SPDX-License-Identifier: MIT */

/*! \file AiDevcon.cpp
 *
 *  This file contains helper functions to search for discrete or analog devices of 3rd party vendors
 *
 */

#include "Ai_cdef.h"

#if defined(_AIM_WINDOWS) || defined(_AIM_VMEWINDOWS) || defined(_AIM_VXI)
    #include <windows.h>
#endif
#include <stdio.h>

#include "Ai_cdef.h"
#ifdef _API3910
  #include "Ai3910i_def.h"
  #include "Ai3910i_fnc.h"
  #include "Ai1553_Internal_def.h"
  #include "Ai3910_Internal_def.h"
#else
  #include "Ai1553i_def.h"
  #include "Ai1553i_fnc.h"
  #include "Ai1553_Internal_def.h"
#endif

#include <string.h>
#include "DEFS_V.H"
#include "Dll_Fnc.h"

#include "AiDevcon.h"
int TestEnumerateDevices(LPCTSTR BaseName, LPCTSTR Machine, DWORD Flags, int argc, char* argv[]);

DEFINE_GUID(GUID_INTERFACE_AIMMIL, 
0x7a56411a, 0x9dd9, 0x445e, 0xa4, 0xf8, 0x6, 0xbc, 0x4a, 0x76, 0x75, 0x3);
DEFINE_GUID(GUID_INTERFACE_IP408, 
0x4d36e971, 0xe325, 0x11ce, 0xbf, 0xc1, 0x8, 0x0, 0x2b, 0xe1, 0x3, 0x18);


struct IdEntry {
    LPCTSTR String;     // string looking for
    LPCTSTR Wild;       // first wild character if any
    BOOL    InstanceId;
};


#ifdef wegdamit
void FormatToStream(__in FILE * stream, __in DWORD fmt,...)
/*++

Routine Description:

    Format text to stream using a particular msg-id fmt
    Used for displaying localizable messages

Arguments:

    stream              - file stream to output to, stdout or stderr
    fmt                 - message id
    ...                 - parameters %1...

Return Value:

    none

--*/
{
    va_list arglist;
    LPTSTR locbuffer = NULL;
    DWORD count;

    va_start(arglist, fmt);
    count = FormatMessage(FORMAT_MESSAGE_FROM_HMODULE|FORMAT_MESSAGE_ALLOCATE_BUFFER,
                          NULL,
                          fmt,
                          0,              // LANGID
                          (LPTSTR) &locbuffer,
                          0,              // minimum size of buffer
                          &arglist);

    if(locbuffer) {
        if(count) {
            int c;
            int back = 0;
            //
            // strip any trailing "\r\n"s and replace by a single "\n"
            //
            while(((c = *CharPrev(locbuffer,locbuffer+count)) == TEXT('\r')) ||
                  (c == TEXT('\n'))) {
                count--;
                back++;
            }
            if(back) {
                locbuffer[count++] = TEXT('\n');
                locbuffer[count] = TEXT('\0');
            }
            //
            // now write to apropriate stream
            //
            _fputts(locbuffer,stream);
        }
        LocalFree(locbuffer);
    }
}
#endif

BOOL WildCompareHwIds(__in LPTSTR * Array, __in const IdEntry & MatchEntry);


void Padding(__in int pad)
/*++

Routine Description:

    Insert padding into line before text

Arguments:

    pad - number of padding tabs to insert

Return Value:

    none

--*/
{
    int c;

    for(c=0;c<pad;c++) {
        fputs("    ",stdout);
    }
}


void Usage(__in LPCTSTR BaseName)
/*++

Routine Description:

    Display simple usage text

Arguments:

    BaseName            - name of executable

Return Value:

    none

--*/
{
//    FormatToStream(stderr,MSG_USAGE,BaseName);
}

void CommandUsage(__in LPCTSTR BaseName, __in LPCTSTR Cmd)
/*++

Routine Description:

    Invalid command usage
    Display how to get help on command

Arguments:

    BaseName            - name of executable

Return Value:

    none

--*/
{
//    FormatToStream(stderr,MSG_COMMAND_USAGE,BaseName,Cmd);
}

void Failure(__in LPCTSTR BaseName, __in LPCTSTR Cmd)
/*++

Routine Description:

    Display simple error text for general failure

Arguments:

    BaseName            - name of executable

Return Value:

    none

--*/
{
//    FormatToStream(stderr,MSG_FAILURE,BaseName,Cmd);
}

LPTSTR GetDeviceStringProperty(__in HDEVINFO Devs, __in PSP_DEVINFO_DATA DevInfo, __in DWORD Prop)
/*++

Routine Description:

    Return a string property for a device, otherwise NULL

Arguments:

    Devs    )_ uniquely identify device
    DevInfo )
    Prop     - string property to obtain

Return Value:

    string containing description

--*/
{
    LPTSTR buffer;
    DWORD size;
    DWORD reqSize;
    DWORD dataType;
    DWORD szChars;

    size = 1024; // initial guess
    buffer = new TCHAR[(size/sizeof(TCHAR))+1];
    if(!buffer) {
        return NULL;
    }
    while(!SetupDiGetDeviceRegistryProperty(Devs,DevInfo,Prop,&dataType,(LPBYTE)buffer,size,&reqSize)) {
        if(GetLastError() != ERROR_INSUFFICIENT_BUFFER) {
            goto failed;
        }
        if(dataType != REG_SZ) {
            goto failed;
        }
        size = reqSize;
        delete [] buffer;
        buffer = new TCHAR[(size/sizeof(TCHAR))+1];
        if(!buffer) {
            goto failed;
        }
    }
    szChars = reqSize/sizeof(TCHAR);
    buffer[szChars] = TEXT('\0');
    return buffer;

failed:
    if(buffer) {
        delete [] buffer;
    }
    return NULL;
}

LPTSTR GetDeviceDescription(__in HDEVINFO Devs, __in PSP_DEVINFO_DATA DevInfo)
/*++

Routine Description:

    Return a string containing a description of the device, otherwise NULL
    Always try friendly name first

Arguments:

    Devs    )_ uniquely identify device
    DevInfo )

Return Value:

    string containing description

--*/
{
    LPTSTR desc;
    desc = GetDeviceStringProperty(Devs,DevInfo,SPDRP_FRIENDLYNAME);
    if(!desc) {
        desc = GetDeviceStringProperty(Devs,DevInfo,SPDRP_DEVICEDESC);
    }
    return desc;
}

IdEntry GetIdType(__in LPCTSTR Id)
/*++

Routine Description:

    Determine if this is instance id or hardware id and if there's any wildcards
    instance ID is prefixed by '@'
    wildcards are '*'


Arguments:

    Id - ptr to string to check

Return Value:

    IdEntry

--*/
{
    IdEntry Entry;

    Entry.InstanceId = FALSE;
    Entry.Wild = NULL;
    Entry.String = Id;

    if(Entry.String[0] == INSTANCEID_PREFIX_CHAR) {
        Entry.InstanceId = TRUE;
        Entry.String = CharNext(Entry.String);
    }
    if(Entry.String[0] == QUOTE_PREFIX_CHAR) {
        //
        // prefix to treat rest of string literally
        //
        Entry.String = CharNext(Entry.String);
    } else {
        //
        // see if any wild characters exist
        //
        Entry.Wild = _tcschr(Entry.String,WILD_CHAR);
    }
    return Entry;
}

//#define FUTURE

BOOL DumpDeviceResourcesOfType(__in DEVINST DevInst, __in HMACHINE MachineHandle, __in LOG_CONF Config, __in RESOURCEID ReqResId,AiUInt32* pini)
{
    RES_DES prevResDes = (RES_DES)Config;
    RES_DES resDes = 0;
    RESOURCEID resId = ReqResId;
    ULONG dataSize;
    PBYTE resDesData;
    BOOL  retval = FALSE;
    ULONG RegCnt;


    UNREFERENCED_PARAMETER(DevInst);

    RegCnt=4;

    printf("DumpDeviceResourcesOfType: \r\n");
    
    while(CM_Get_Next_Res_Des_Ex(&resDes,prevResDes,ReqResId,&resId,0,MachineHandle)==CR_SUCCESS) {
        if(prevResDes != Config) {
            CM_Free_Res_Des_Handle(prevResDes);
        }
        prevResDes = resDes;
        if(CM_Get_Res_Des_Data_Size_Ex(&dataSize,resDes,0,MachineHandle)!=CR_SUCCESS) {
            continue;
        }
        resDesData = new BYTE[dataSize];
        if(!resDesData) {
            continue;
        }
        if(CM_Get_Res_Des_Data_Ex(resDes,resDesData,dataSize,0,MachineHandle)!=CR_SUCCESS) {
            delete [] resDesData;
            continue;
        }
        switch(resId) {
            case ResType_Mem: {

                PMEM_RESOURCE  pMemData = (PMEM_RESOURCE)resDesData;
                if(pMemData->MEM_Header.MD_Alloc_End-pMemData->MEM_Header.MD_Alloc_Base+1) {
                    Padding(2);
                    _tprintf(TEXT("MEM : %08I64x-%08I64x\n"),pMemData->MEM_Header.MD_Alloc_Base,pMemData->MEM_Header.MD_Alloc_End);
                    pini[RegCnt++] = (AiUInt32)pMemData->MEM_Header.MD_Alloc_Base;
                    retval = TRUE;
                }
                break;
            }

            case ResType_IO: {

                PIO_RESOURCE   pIoData = (PIO_RESOURCE)resDesData;
                if(pIoData->IO_Header.IOD_Alloc_End-pIoData->IO_Header.IOD_Alloc_Base+1) {
                    Padding(2);
                    _tprintf(TEXT("IO  : %04I64x-%04I64x\n"),pIoData->IO_Header.IOD_Alloc_Base,pIoData->IO_Header.IOD_Alloc_End);
                    pini[RegCnt++] = (AiUInt32)pIoData->IO_Header.IOD_Alloc_Base;
                    retval = TRUE;
                }
                break;
            }

            case ResType_DMA: {

                PDMA_RESOURCE  pDmaData = (PDMA_RESOURCE)resDesData;
                Padding(2);
                _tprintf(TEXT("DMA : %u\n"),pDmaData->DMA_Header.DD_Alloc_Chan);
                retval = TRUE;
                break;
            }

            case ResType_IRQ: {

                PIRQ_RESOURCE  pIrqData = (PIRQ_RESOURCE)resDesData;

                Padding(2);
                _tprintf(TEXT("IRQ : %u\n"),pIrqData->IRQ_Header.IRQD_Alloc_Num);
                retval = TRUE;
                break;
            }
        }
        delete [] resDesData;
    }
    if(prevResDes != Config) {
        CM_Free_Res_Des_Handle(prevResDes);
    }
    return retval;
}

BOOL DumpDeviceResources(__in HDEVINFO Devs, __in PSP_DEVINFO_DATA DevInfo,AiUInt32* pini )
/*++

Routine Description:

    Dump Resources to stdout

Arguments:

    Devs    )_ uniquely identify device
    DevInfo )

Return Value:

    none

--*/
{
    SP_DEVINFO_LIST_DETAIL_DATA devInfoListDetail;
    ULONG status = 0;
    ULONG problem = 0;
    LOG_CONF config = 0;
    BOOL haveConfig = FALSE;

    
    printf("DumpDeviceResources: ");

    //
    // see what state the device is in
    //
    devInfoListDetail.cbSize = sizeof(devInfoListDetail);
    if((!SetupDiGetDeviceInfoListDetail(Devs,&devInfoListDetail)) ||
            (CM_Get_DevNode_Status_Ex(&status,&problem,DevInfo->DevInst,0,devInfoListDetail.RemoteMachineHandle)!=CR_SUCCESS)) {
        return FALSE;
    }

    //
    // see if the device is running and what resources it might be using
    //
    if(!(status & DN_HAS_PROBLEM)) {
        //
        // If this device is running, does this devinst have a ALLOC log config?
        //
        if (CM_Get_First_Log_Conf_Ex(&config,
                                     DevInfo->DevInst,
                                     ALLOC_LOG_CONF,
                                     devInfoListDetail.RemoteMachineHandle) == CR_SUCCESS) {
            haveConfig = TRUE;
        }
    }
    if(!haveConfig) {
        //
        // If no config so far, does it have a FORCED log config?
        // (note that technically these resources might be used by another device
        // but is useful info to show)
        //
        if (CM_Get_First_Log_Conf_Ex(&config,
                                     DevInfo->DevInst,
                                     FORCED_LOG_CONF,
                                     devInfoListDetail.RemoteMachineHandle) == CR_SUCCESS) {
            haveConfig = TRUE;
        }
    }

    if(!haveConfig) {
        //
        // if there's a hardware-disabled problem, boot-config isn't valid
        // otherwise use this if we don't have anything else
        //
        if(!(status & DN_HAS_PROBLEM) || (problem != CM_PROB_HARDWARE_DISABLED)) {
            //
            // Does it have a BOOT log config?
            //
            if (CM_Get_First_Log_Conf_Ex(&config,
                                         DevInfo->DevInst,
                                         BOOT_LOG_CONF,
                                         devInfoListDetail.RemoteMachineHandle) == CR_SUCCESS) {
                haveConfig = TRUE;
            }
        }
    }

    if(!haveConfig) {
        //
        // if we don't have any configuration, display an apropriate message
        //
        printf("No recources found\r\n");
        return FALSE;
    }
    printf("Active resources\r\n");

    //
    // dump resources
    //
    if (pini != NULL)
       DumpDeviceResourcesOfType(DevInfo->DevInst,devInfoListDetail.RemoteMachineHandle,config,ResType_All, pini);

    //
    // release handle
    //
    CM_Free_Log_Conf_Handle(config);

    return TRUE;
}

LPTSTR * GetMultiSzIndexArray(LPTSTR MultiSz)
/*++

Routine Description:

    Get an index array pointing to the MultiSz passed in

Arguments:

    MultiSz - well formed multi-sz string

Return Value:

    array of strings. last entry+1 of array contains NULL
    returns NULL on failure

--*/
{
    LPTSTR scan;
    LPTSTR * array;
    int elements;

    for(scan = MultiSz, elements = 0; scan[0] ;elements++) {
        scan += lstrlen(scan)+1;
    }
    array = new LPTSTR[elements+2];
    if(!array) {
        return NULL;
    }
    array[0] = MultiSz;
    array++;
    if(elements) {
        for(scan = MultiSz, elements = 0; scan[0]; elements++) {
            array[elements] = scan;
            scan += lstrlen(scan)+1;
        }
    }
    array[elements] = NULL;
    return array;
}

void DelMultiSz(__in_opt LPTSTR* Array)
/*++

Routine Description:

    Deletes the string array allocated by GetDevMultiSz/GetRegMultiSz/GetMultiSzIndexArray

Arguments:

    Array - pointer returned by GetMultiSzIndexArray

Return Value:

    None

--*/
{
    if(Array) {
        Array--;
        if(Array[0]) {
            delete [] Array[0];
        }
        delete [] Array;
    }
}

//__drv_allocatesMem(object)
LPTSTR * GetDevMultiSz(HDEVINFO Devs, PSP_DEVINFO_DATA DevInfo, DWORD Prop)
/*++

Routine Description:

    Get a multi-sz device property
    and return as an array of strings

Arguments:

    Devs    - HDEVINFO containing DevInfo
    DevInfo - Specific device
    Prop    - SPDRP_HARDWAREID or SPDRP_COMPATIBLEIDS

Return Value:

    array of strings. last entry+1 of array contains NULL
    returns NULL on failure

--*/
{
    LPTSTR buffer;
    DWORD size;
    DWORD reqSize;
    DWORD dataType;
    LPTSTR * array;
    DWORD szChars;

    size = 8192; // initial guess, nothing magic about this
    buffer = new TCHAR[(size/sizeof(TCHAR))+2];
    if(!buffer) {
        return NULL;
    }
    while(!SetupDiGetDeviceRegistryProperty(Devs,DevInfo,Prop,&dataType,(LPBYTE)buffer,size,&reqSize)) {
        if(GetLastError() != ERROR_INSUFFICIENT_BUFFER) {
            goto failed;
        }
        if(dataType != REG_MULTI_SZ) {
            goto failed;
        }
        size = reqSize;
        delete [] buffer;
        buffer = new TCHAR[(size/sizeof(TCHAR))+2];
        if(!buffer) {
            goto failed;
        }
    }
    szChars = reqSize/sizeof(TCHAR);
    buffer[szChars] = TEXT('\0');
    buffer[szChars+1] = TEXT('\0');
    array = GetMultiSzIndexArray(buffer);
    if(array) {
        return array;
    }

failed:
    if(buffer) {
        delete [] buffer;
    }
    return NULL;
}

BOOL WildCardMatch(__in LPCTSTR Item, __in const IdEntry & MatchEntry)
/*++

Routine Description:

    Compare a single item against wildcard
    I'm sure there's better ways of implementing this
    Other than a command-line management tools
    it's a bad idea to use wildcards as it implies
    assumptions about the hardware/instance ID
    eg, it might be tempting to enumerate root\* to
    find all root devices, however there is a CfgMgr
    API to query status and determine if a device is
    root enumerated, which doesn't rely on implementation
    details.

Arguments:

    Item - item to find match for eg a\abcd\c
    MatchEntry - eg *\*bc*\*

Return Value:

    TRUE if any match, otherwise FALSE

--*/
{
    LPCTSTR scanItem;
    LPCTSTR wildMark;
    LPCTSTR nextWild;
    size_t matchlen;

    //
    // before attempting anything else
    // try and compare everything up to first wild
    //
    if(!MatchEntry.Wild) {
        return _tcsicmp(Item,MatchEntry.String) ? FALSE : TRUE;
    }
    if(_tcsnicmp(Item,MatchEntry.String,MatchEntry.Wild-MatchEntry.String) != 0) {
        return FALSE;
    }
    wildMark = MatchEntry.Wild;
    scanItem = Item + (MatchEntry.Wild-MatchEntry.String);

    for(;wildMark[0];) {
        //
        // if we get here, we're either at or past a wildcard
        //
        if(wildMark[0] == WILD_CHAR) {
            //
            // so skip wild chars
            //
            wildMark = CharNext(wildMark);
            continue;
        }
        //
        // find next wild-card
        //
        nextWild = _tcschr(wildMark,WILD_CHAR);
        if(nextWild) {
            //
            // substring
            //
            matchlen = nextWild-wildMark;
        } else {
            //
            // last portion of match
            //
            size_t scanlen = lstrlen(scanItem);
            matchlen = lstrlen(wildMark);
            if(scanlen < matchlen) {
                return FALSE;
            }
            return _tcsicmp(scanItem+scanlen-matchlen,wildMark) ? FALSE : TRUE;
        }
        if(_istalpha(wildMark[0])) {
            //
            // scan for either lower or uppercase version of first character
            //

            //
            // the code suppresses the warning 28193 for the calls to _totupper
            // and _totlower.  This suppression is done because those functions
            // have a check return annotation on them.  However, they don't return
            // error codes and the check return annotation is really being used
            // to indicate that the return value of the function should be looked
            // at and/or assigned to a variable.  The check return annotation means
            // the return value should always be checked in all code paths.
            // We assign the return values to variables but the while loop does not 
            // examine both values in all code paths (e.g. when scanItem[0] == 0, 
            // neither u nor l will be examined) and it doesn't need to examine 
            // the values in all code paths.
            //
#pragma warning( suppress: 28193)
            TCHAR u = _totupper(wildMark[0]);
#pragma warning( suppress: 28193)
            TCHAR l = _totlower(wildMark[0]);
            while(scanItem[0] && scanItem[0]!=u && scanItem[0]!=l) {
                scanItem = CharNext(scanItem);
            }
            if(!scanItem[0]) {
                //
                // ran out of string
                //
                return FALSE;
            }
        } else {
            //
            // scan for first character (no case)
            //
            scanItem = _tcschr(scanItem,wildMark[0]);
            if(!scanItem) {
                //
                // ran out of string
                //
                return FALSE;
            }
        }
        //
        // try and match the sub-string at wildMark against scanItem
        //
        if(_tcsnicmp(scanItem,wildMark,matchlen)!=0) {
            //
            // nope, try again
            //
            scanItem = CharNext(scanItem);
            continue;
        }
        //
        // substring matched
        //
        scanItem += matchlen;
        wildMark += matchlen;
    }
    return (wildMark[0] ? FALSE : TRUE);
}

//***************************************************************************
//
//   Module : AiDevCon                  Submodule : EnumerateExistingDevices
//
//---------------------------------------------------------------------------
//    Descriptions
//    ------------
//    Inputs    : none
//
//    Outputs   : none
//
//    Description :
//
//***************************************************************************
AI_LIB_FUNC AiReturn AI_CALL_CONV AiEnumerateDevices( AiUInt32 bModule, const GUID* guid, AiUInt32 par1, AiUInt32 par2, AiUInt32* pini){

    HDEVINFO devs = INVALID_HANDLE_VALUE;
    IdEntry * templ = NULL;
    int failcode = EXIT_FAIL;
    int retcode;
    int argIndex;
    DWORD devIndex;
    SP_DEVINFO_DATA devInfo;
    SP_DEVINFO_LIST_DETAIL_DATA devInfoListDetail;
    BOOL doSearch = FALSE;
    BOOL match;
    BOOL all = FALSE;
    GUID cls;
    DWORD numClass = 0;
    int skip = 0;
    int argc = 1;
    AiChar *argv[10];
    int Flags = 0;
    LPCTSTR Machine = NULL;
    BOOL ret = 0;
    DWORD NumberOfBoards;

    argv[0] = (AiChar *) malloc(256);
    argv[1] = (AiChar *) malloc(256);

    sprintf_s(argv[0],256,"@PCI\\VEN_%04X&DEV_%04X&SUBSYS_%08X*",par1&0xFFFF,(par1>>16),par2);

    ret = NumberOfBoards = 0;

    if(!argc) {
        return EXIT_USAGE;
    }

    templ = new IdEntry[argc];
    if(!templ) {
        goto final;
    }

    //
    // determine if a class is specified
    //
    if(argc>skip && argv[skip][0]==CLASS_PREFIX_CHAR && argv[skip][1]) {
        if(!SetupDiClassGuidsFromNameEx(argv[skip]+1,&cls,1,&numClass,Machine,NULL) &&
            GetLastError() != ERROR_INSUFFICIENT_BUFFER) {
            goto final;
        }
        if(!numClass) {
            failcode = EXIT_OK;
            goto final;
        }
        skip++;
    }
    if(argc>skip && argv[skip][0]==WILD_CHAR && !argv[skip][1]) {
        //
        // catch convinient case of specifying a single argument '*'
        //
        all = TRUE;
        skip++;
    } else if(argc<=skip) {
        //
        // at least one parameter, but no <id>'s
        //
        all = TRUE;
    }

    //
    // determine if any instance id's were specified
    //
    // note, if =<class> was specified with no id's
    // we'll mark it as not doSearch
    // but will go ahead and add them all
    //
    for(argIndex=skip;argIndex<argc;argIndex++) {
        templ[argIndex] = GetIdType(argv[argIndex]);
        if(templ[argIndex].Wild || !templ[argIndex].InstanceId) {
            //
            // anything other than simple InstanceId's require a search
            //
            doSearch = TRUE;
        }
    }
    if(doSearch || all) {
        //
        // add all id's to list
        // if there's a class, filter on specified class
        //
        devs = SetupDiGetClassDevsEx(numClass ? &cls : NULL,
                                     NULL,
                                     NULL,
                                     (numClass ? 0 : DIGCF_ALLCLASSES) | Flags,
                                     NULL,
                                     Machine,
                                     NULL);

    } else {
        //
        // blank list, we'll add instance id's by hand
        //
        devs = SetupDiCreateDeviceInfoListEx(numClass ? &cls : NULL,
                                             NULL,
                                             Machine,
                                             NULL);
    }
    if(devs == INVALID_HANDLE_VALUE) {
        goto final;
    }
    for(argIndex=skip;argIndex<argc;argIndex++) {
        //
        // add explicit instances to list (even if enumerated all,
        // this gets around DIGCF_PRESENT)
        // do this even if wildcards appear to be detected since they
        // might actually be part of the instance ID of a non-present device
        //
        if(templ[argIndex].InstanceId) {
            SetupDiOpenDeviceInfo(devs,templ[argIndex].String,NULL,0,NULL);
        }
    }

    devInfoListDetail.cbSize = sizeof(devInfoListDetail);
    if(!SetupDiGetDeviceInfoListDetail(devs,&devInfoListDetail)) {
        goto final;
    }

    //
    // now enumerate them
    //
    if(all) {
        doSearch = FALSE;
    }

    devInfo.cbSize = sizeof(devInfo);
    for(devIndex=0;SetupDiEnumDeviceInfo(devs,devIndex,&devInfo);devIndex++) {

        if(doSearch) {
            for(argIndex=skip,match=FALSE;(argIndex<argc) && !match;argIndex++) {
                TCHAR devID[MAX_DEVICE_ID_LEN];
                LPTSTR *hwIds = NULL;
                LPTSTR *compatIds = NULL;
                //
                // determine instance ID
                //
                if(CM_Get_Device_ID_Ex(devInfo.DevInst,devID,MAX_DEVICE_ID_LEN,0,devInfoListDetail.RemoteMachineHandle)!=CR_SUCCESS) {
                    devID[0] = TEXT('\0');
                }

                if(templ[argIndex].InstanceId) {
                    //
                    // match on the instance ID
                    //
                    if(WildCardMatch(devID,templ[argIndex])) {
                        match = TRUE;
                    }
                } else {
                    //
                    // determine hardware ID's
                    // and search for matches
                    //
                    hwIds = GetDevMultiSz(devs,&devInfo,SPDRP_HARDWAREID);
                    compatIds = GetDevMultiSz(devs,&devInfo,SPDRP_COMPATIBLEIDS);

                    if(WildCompareHwIds(hwIds,templ[argIndex]) ||
                        WildCompareHwIds(compatIds,templ[argIndex])) {
                        match = TRUE;
                    }
                }
                DelMultiSz(hwIds);
                DelMultiSz(compatIds);
            }
        } else {
            match = TRUE;
        }
        if(match) {
//            retcode = Callback(devs,&devInfo,devIndex,Context);
            if(DumpDeviceResources(devs, &devInfo,&(pini[NumberOfBoards]))==TRUE){
               NumberOfBoards++; 
            }

            retcode = 0;
            if(retcode) {
                failcode = retcode;
                goto final;
            }
        }
    }

    failcode = EXIT_OK;

final:
    if(templ) {
        delete [] templ;
    }
    if(devs != INVALID_HANDLE_VALUE) {
        SetupDiDestroyDeviceInfoList(devs);
    }
    
    
    ret = NumberOfBoards;
    
    
    
    
    
#ifdef wegdamit    
//    strcpy(data[0],"@PCI\\VEN_10B5&DEV_6541&SUBSYS_11111447&REV_CB\\4&1C61A241&0&10F0");
    strcpy(data[0],"@PCI\\VEN_10B5&DEV_6541&SUBSYS_11111447*");
//  TestEnumerateDevices("PCI", NULL, 0, 1,&data[0][0]);
  TestEnumerateDevices("PCI", NULL, 0, 1,data);
#define IP_CARD
#ifdef IP_CARD    
    devs = SetupDiGetClassDevs(guid, NULL, NULL, DIGCF_PRESENT);
//    devs = SetupDiGetClassDevs(guid, NULL, NULL, DIGCF_PRESENT | DIGCF_INTERFACEDEVICE);
#else
    //
    // blank list, we'll add instance id's by hand
    //
    devs = SetupDiCreateDeviceInfoListEx(numClass ? &cls : NULL,
                                             NULL,
                                             NULL/*Machine*/,
                                             NULL);
    if(devs == INVALID_HANDLE_VALUE) {
        goto final;
    }
    
//    SetupDiOpenDeviceInfo(devs,"SUBSYS_11111447",NULL,0,NULL);

//    SetupDiOpenDeviceInfo(devs,"PCI\VEN_10B5&DEV_6541&SUBSYS_11111447&REV_CB\4&1C61A241&0&10F0",NULL,0,&devInfo);
//    SetupDiOpenDeviceInfo(devs,"PCI\VEN_10B5&DEV_6541&SUBSYS_11111447",NULL,0,NULL);
//    SetupDiClassGuidsFromNameEx("Multifunktionsadapter",&cls,1,&numClass,NULL,NULL);
//    SetupDiClassGuidsFromNameEx("AIM Components",&cls,1,&numClass,NULL,NULL);
//    devs = SetupDiGetClassDevs(&cls, NULL, NULL, DIGCF_PRESENT | DIGCF_INTERFACEDEVICE);
#endif
    if (devs == INVALID_HANDLE_VALUE)
        return 0;

    devInfoListDetail.cbSize = sizeof(devInfoListDetail);
    if(!SetupDiGetDeviceInfoListDetail(devs,&devInfoListDetail)) {
        goto final;
    }
    devInfo.cbSize = sizeof(devInfo);
    for(devIndex=0;SetupDiEnumDeviceInfo(devs,devIndex,&devInfo);devIndex++) {
       if(DumpDeviceResources(devs, &devInfo,&(pini[NumberOfBoards]))==TRUE){
            NumberOfBoards++; 
       }
       if (devIndex > 3) break; // only for 4 boards
    }
    ret = NumberOfBoards;
final:
    SetupDiDestroyDeviceInfoList(devs);
#endif
    return ret;

}







BOOL WildCompareHwIds(__in LPTSTR * Array, __in const IdEntry & MatchEntry)
/*++

Routine Description:

    Compares all strings in Array against Id
    Use WildCardMatch to do real compare

Arguments:

    Array - pointer returned by GetDevMultiSz
    MatchEntry - string to compare against

Return Value:

    TRUE if any match, otherwise FALSE

--*/
{
    if(Array) {
        while(Array[0]) {
            if(WildCardMatch( Array[0],MatchEntry)) {
                return TRUE;
            }
            Array++;
        }
    }
    return FALSE;
}


//int TestEnumerateDevices(LPCTSTR BaseName, LPCTSTR Machine, DWORD Flags, int argc, PZPCSTR argv, CallbackFunc Callback, LPVOID Context)
int TestEnumerateDevices(LPCTSTR BaseName, LPCTSTR Machine, DWORD Flags, int argc, char* argv[])
/*++

Routine Description:

    Generic enumerator for devices that will be passed the following arguments:
    <id> [<id>...]
    =<class> [<id>...]
    where <id> can either be @instance-id, or hardware-id and may contain wildcards
    <class> is a class name

Arguments:

    BaseName - name of executable
    Machine  - name of machine to enumerate
    Flags    - extra enumeration flags (eg DIGCF_PRESENT)
    argc/argv - remaining arguments on command line
    Callback - function to call for each hit
    Context  - data to pass function for each hit

Return Value:

    EXIT_xxxx

--*/
{
    HDEVINFO devs = INVALID_HANDLE_VALUE;
    IdEntry * templ = NULL;
    int failcode = EXIT_FAIL;
    int retcode;
    int argIndex;
    DWORD devIndex;
    SP_DEVINFO_DATA devInfo;
    SP_DEVINFO_LIST_DETAIL_DATA devInfoListDetail;
    BOOL doSearch = FALSE;
    BOOL match;
    BOOL all = FALSE;
    GUID cls;
    DWORD numClass = 0;
    int skip = 0;
    UNREFERENCED_PARAMETER(BaseName);

    if(!argc) {
        return EXIT_USAGE;
    }

    templ = new IdEntry[argc];
    if(!templ) {
        goto final;
    }

    //
    // determine if a class is specified
    //
    if(argc>skip && argv[skip][0]==CLASS_PREFIX_CHAR && argv[skip][1]) {
        if(!SetupDiClassGuidsFromNameEx(argv[skip]+1,&cls,1,&numClass,Machine,NULL) &&
            GetLastError() != ERROR_INSUFFICIENT_BUFFER) {
            goto final;
        }
        if(!numClass) {
            failcode = EXIT_OK;
            goto final;
        }
        skip++;
    }
    if(argc>skip && argv[skip][0]==WILD_CHAR && !argv[skip][1]) {
        //
        // catch convinient case of specifying a single argument '*'
        //
        all = TRUE;
        skip++;
    } else if(argc<=skip) {
        //
        // at least one parameter, but no <id>'s
        //
        all = TRUE;
    }

    //
    // determine if any instance id's were specified
    //
    // note, if =<class> was specified with no id's
    // we'll mark it as not doSearch
    // but will go ahead and add them all
    //
    for(argIndex=skip;argIndex<argc;argIndex++) {
        templ[argIndex] = GetIdType(argv[argIndex]);
        if(templ[argIndex].Wild || !templ[argIndex].InstanceId) {
            //
            // anything other than simple InstanceId's require a search
            //
            doSearch = TRUE;
        }
    }
    if(doSearch || all) {
        //
        // add all id's to list
        // if there's a class, filter on specified class
        //
        devs = SetupDiGetClassDevsEx(numClass ? &cls : NULL,
                                     NULL,
                                     NULL,
                                     (numClass ? 0 : DIGCF_ALLCLASSES) | Flags,
                                     NULL,
                                     Machine,
                                     NULL);

    } else {
        //
        // blank list, we'll add instance id's by hand
        //
        devs = SetupDiCreateDeviceInfoListEx(numClass ? &cls : NULL,
                                             NULL,
                                             Machine,
                                             NULL);
    }
    if(devs == INVALID_HANDLE_VALUE) {
        goto final;
    }
    for(argIndex=skip;argIndex<argc;argIndex++) {
        //
        // add explicit instances to list (even if enumerated all,
        // this gets around DIGCF_PRESENT)
        // do this even if wildcards appear to be detected since they
        // might actually be part of the instance ID of a non-present device
        //
        if(templ[argIndex].InstanceId) {
            SetupDiOpenDeviceInfo(devs,templ[argIndex].String,NULL,0,NULL);
        }
    }

    devInfoListDetail.cbSize = sizeof(devInfoListDetail);
    if(!SetupDiGetDeviceInfoListDetail(devs,&devInfoListDetail)) {
        goto final;
    }

    //
    // now enumerate them
    //
    if(all) {
        doSearch = FALSE;
    }

    devInfo.cbSize = sizeof(devInfo);
    for(devIndex=0;SetupDiEnumDeviceInfo(devs,devIndex,&devInfo);devIndex++) {

        if(doSearch) {
            for(argIndex=skip,match=FALSE;(argIndex<argc) && !match;argIndex++) {
                TCHAR devID[MAX_DEVICE_ID_LEN];
                LPTSTR *hwIds = NULL;
                LPTSTR *compatIds = NULL;
                //
                // determine instance ID
                //
                if(CM_Get_Device_ID_Ex(devInfo.DevInst,devID,MAX_DEVICE_ID_LEN,0,devInfoListDetail.RemoteMachineHandle)!=CR_SUCCESS) {
                    devID[0] = TEXT('\0');
                }

                if(templ[argIndex].InstanceId) {
                    //
                    // match on the instance ID
                    //
                    if(WildCardMatch(devID,templ[argIndex])) {
                        match = TRUE;
                    }
                } else {
                    //
                    // determine hardware ID's
                    // and search for matches
                    //
                    hwIds = GetDevMultiSz(devs,&devInfo,SPDRP_HARDWAREID);
                    compatIds = GetDevMultiSz(devs,&devInfo,SPDRP_COMPATIBLEIDS);

                    if(WildCompareHwIds(hwIds,templ[argIndex]) ||
                        WildCompareHwIds(compatIds,templ[argIndex])) {
                        match = TRUE;
                    }
                }
                DelMultiSz(hwIds);
                DelMultiSz(compatIds);
            }
        } else {
            match = TRUE;
        }
        if(match) {
//            retcode = Callback(devs,&devInfo,devIndex,Context);
            retcode = 0;
            if(retcode) {
                failcode = retcode;
                goto final;
            }
        }
    }

    failcode = EXIT_OK;

final:
    if(templ) {
        delete [] templ;
    }
    if(devs != INVALID_HANDLE_VALUE) {
        SetupDiDestroyDeviceInfoList(devs);
    }
    return failcode;

}
