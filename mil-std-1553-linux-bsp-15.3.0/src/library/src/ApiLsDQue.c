/* SPDX-FileCopyrightText: 2001-2022 AIM GmbH <info@aim-online.com> */
/* SPDX-License-Identifier: MIT */

/*! \file ApiLsDQue.c
 *
 * LS Data Queue functions
 * 
 */

#include "net_io.h"
#include "AiOs.h"
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
#include "DEFS_V.H"
#include "Dll_Fnc.h"
#include "ApiLsDQue.h"

#include "string.h"  /* for memset and strncpy */


//****************************************************
// platform specific specific includes
//****************************************************
#ifdef _AIM_WINDOWS
#include "AiMPW_io.h"
#endif //_AIM_WINDOWS

#ifdef _AIM_RTX64
#include "AimRtx64_io.h"
#endif

#ifdef _AIM_VME
#include "Aim1553_io.h"
#endif //_AIM_VME


#ifdef _AIM_VXI
#include <visa.h>
#include "visa_io.h"
#endif //_AIM_VXI

#ifdef _AIM_LINUX
#include "Aim1553_io.h"
#endif //Linux

#include "mil_buffer.h" /* TSW ringbuffer functions */
#include "AiMapDllToTsw.h"


//***************************************************************************
//***       D A T A     Q U E U E     C O M M A N D S
//***************************************************************************
//***************************************************************************
//
//   Module : ApiLsDQue                Submodule : ApiCmdDataQueueOpen
//
//   Author : Patrick Giesel           Project   : E2HLBA
//
//---------------------------------------------------------------------------
//   Create : 03.12.02
//---------------------------------------------------------------------------
//
//    Description : This function setup a data queue for recording/replay
//
//***************************************************************************

AI_LIB_FUNC AiReturn AI_CALL_CONV ApiCmdDataQueueOpen(AiUInt32 ul_Module, AiUInt32 id, AiUInt32 * queue_size)
{
    AiUInt32 internal_queue_size = 0;
    AiInt16 uw_RetVal = API_OK;
    TY_DEVICE_INFO * pDevice = _ApiGetDeviceInfoPtrByModule(ul_Module);
    
    if (pDevice == NULL)
        return API_ERR_NO_MODULE_EXTENSION;

    if (Aiul_DbgLevel & DBG_PARAMCHK)
    {
        if (id > MAX_DATA_QUEUE_ID)
            uw_RetVal = API_ERR_DQUEUE_ID_NOT_IN_RANGE;
    }

    if (API_OK == uw_RetVal)
    {
        uw_RetVal = ApiCmdDataQueueOpen_(ul_Module, 0, id, &internal_queue_size);
    }

    if (queue_size) *queue_size = internal_queue_size;

    v_ExamineRetVal("ApiCmdDataQueueOpen", uw_RetVal);
    TRACE_BEGIN
    TRACE1("           id = 0x%x;\n", id);
    TRACE1("           queue_size = 0x%x;\n", internal_queue_size);
    TRACE_FCTA("ApiCmdDataQueueOpen", uw_RetVal);
    TRACE_PARA(ul_Module);
    TRACE_FCTE;
    TRACE_END
        return(uw_RetVal);
}


//**************************************************************************
//
//  _ApiCmdDataQueueOpen
//
//**************************************************************************
AiInt16 ApiCmdDataQueueOpen_(AiUInt32 module, AiUInt8 biu, AiUInt32 id, AiUInt32 * queue_size)
{
    AiInt16 uw_RetVal = API_OK;
    TY_DEVICE_INFO * pDevice = _ApiGetDeviceInfoPtrByModule(module);

    if (pDevice == NULL)
        return API_ERR_NO_MODULE_EXTENSION;

    if ((pDevice->x_Config.uc_DataQueueMode == 1) && _ApiCmdDataQueueDirectIsBmRecording(id))
    {
        uw_RetVal = ApiCmdDataQueueOpenDirect_(module, 0, id, queue_size);
    }
    else if (GET_SERVER_ID(module))
    {
        uw_RetVal = _MilNetDataQueueOpen(module, biu, id, queue_size);
    }
    else
    {
        /* Queue is ringbuffer (shared) filled by TSW */
        return ApiCmdDataQueueOpenBuffer_(module, biu, id, queue_size);
    }

    return uw_RetVal;
}


#define API_FW_BM_GET  0x20
#define API_FW_BM_BASE 0x80
#define API_FW_BM_MBFP 0x84
#define API_FW_BM_MSTCB 0x88



//**************************************************************************
//
//  _ApiCmdDataQueueDirectOpen
//
//**************************************************************************
AiInt16 ApiCmdDataQueueOpenDirect_(AiUInt32 ulModHandle, AiUInt8 ucBiu, AiUInt32 id, AiUInt32 * queue_size)
{
    TY_DEVICE_INFO * pDevice = _ApiGetDeviceInfoPtrByModule(ulModHandle);
    AiUInt32         ulCbOffset = 0;
    AiUInt32         ulBmBaseOffset = 0;
    AiUInt32         ulSystemControlWord = 0;
    AiInt16          wRetVal = 0;
    AiUInt8          ucHs = 0;
    TY_API_DATA_QUEUE_DIRECT_SETUP * pSetup = NULL;

    *queue_size = 0;

    ucBiu = _ApiCmdDataQueueDirectGetBiu(id);

    ulCbOffset = pDevice->x_MemInfo.ax_BiuAddr[ucBiu].ul_Cb;


    /* -- allocate and initialize the setup information structure --- */

    if (pDevice->pxDataQueueDirectSetup[id] == NULL)
        pDevice->pxDataQueueDirectSetup[id] = (TY_API_DATA_QUEUE_DIRECT_SETUP*)AiOsMalloc(sizeof(TY_API_DATA_QUEUE_DIRECT_SETUP));


    memset(pDevice->pxDataQueueDirectSetup[id], 0, sizeof(TY_API_DATA_QUEUE_DIRECT_SETUP));


    ucHs = _ApiCmdDataQueueDirectIsHs(pDevice->x_BoardInfo.ul_DeviceType, ucBiu);

    /* -- Store data queue info for later acces --- */

    pSetup = pDevice->pxDataQueueDirectSetup[id];

    pSetup->ulId = id;

    /* -- Read BM base offset and size --- */

    wRetVal = ApiReadMemData(ulModHandle, API_MEMTYPE_GLOBAL, ulCbOffset + API_FW_BM_BASE, 4, &ulBmBaseOffset);

    if (wRetVal != API_OK)
        return wRetVal;

    pSetup->ulBmBaseOffset = ulBmBaseOffset;


    wRetVal = ApiReadMemData(ulModHandle, API_MEMTYPE_GLOBAL, ulCbOffset, 4, &ulSystemControlWord);

    if (wRetVal != API_OK)
        return wRetVal;

    if (ucHs)
        pSetup->ulBmSize = (((ulSystemControlWord >> 10) & 0xFF) + 1) * 64 * 1024;
    else
        pSetup->ulBmSize = (((ulSystemControlWord >> 8) & 0xFF) + 1) * 64 * 1024;


    pSetup->ulBmEndOffset = ulBmBaseOffset + pSetup->ulBmSize;

    *queue_size = pSetup->ulBmSize;

    return API_OK;
}


AiInt16 ApiCmdDataQueueOpenBuffer_(AiUInt32 module, AiUInt8 biu, AiUInt32 id, AiUInt32 * queue_size)
{
    AiInt16   ret            = API_OK;
    TY_MIL_COM_LS_BM_DATAQUEUE_OPEN_INPUT  xInput;
    TY_MIL_COM_LS_BM_DATAQUEUE_OPEN_OUTPUT xOutput;

    TY_DEVICE_INFO * pDevice = _ApiGetDeviceInfoPtrByModule(module);

    *queue_size = 0;

    if (pDevice == NULL)
        return API_ERR_NO_MODULE_EXTENSION;

    if (DQUEUE_OPENED == pDevice->ax_QueueTable[id].uc_QueueFlag)
        /* DQ already open */
        return 0;

    /* -- Open DQ in TSW --- */
    xInput.id              = id;

    MIL_COM_INIT(&xInput.cmd, API_STREAM(module), 0, MIL_COM_LS_BM_DATAQUEUE_OPEN, sizeof(xInput), sizeof(xOutput));

    ret = API_CMD_STRUCT_BM_DATAQUEUE_OPEN(module, &xInput, &xOutput);

    if( ret != 0 )
    {
        pDevice->ax_QueueTable[id].ul_QueueStart = 0;
        return ret;
    }

    /* -- Store DQ header offset --- */
    pDevice->ax_QueueTable[id].ul_QueueStart               = xOutput.dataqueue_offset;

    *queue_size = xOutput.dataqueue_size;

    if (0 == pDevice->ax_QueueTable[id].ul_QueueStart)
        return API_ERR_DQUEUE_ASP_OPEN;

    /* -- Initialize DLL DQ --- */
    memset(&pDevice->ax_QueueTable[id].x_QueueStatus, 0, sizeof(TY_API_DATA_QUEUE_STATUS));

    pDevice->ax_QueueTable[id].uc_QueueFlag = DQUEUE_OPENED;

    return ret;
}



//***************************************************************************
//
//   Module : ApiLsDQue                Submodule : ApiCmdDataQueueClose
//
//   Author : Patrick Giesel           Project   : E2HLBA
//
//---------------------------------------------------------------------------
//   Create : 03.12.02
//---------------------------------------------------------------------------
//
//    Description : This function closes a data queue
//
//***************************************************************************

AI_LIB_FUNC AiReturn AI_CALL_CONV ApiCmdDataQueueClose(AiUInt32 module, AiUInt32 id)
{
    AiInt16 ret = API_OK;

    if (Aiul_DbgLevel & DBG_PARAMCHK)
    {
        if (id > MAX_DATA_QUEUE_ID)
            ret = API_ERR_DQUEUE_ID_NOT_IN_RANGE;
    }

    if (API_OK == ret)
    {
        ret = ApiCmdDataQueueClose_(module, 0, id );
    }

    v_ExamineRetVal("ApiCmdDataQueueClose", ret);
    TRACE_BEGIN
    TRACE1("        id = 0x%x;\n", id);
    TRACE_FCTA("ApiCmdDataQueueClose", ret);
    TRACE_PARA(module);
    TRACE_FCTE;
    TRACE_END
        return(ret);
}


AiInt16 ApiCmdDataQueueClose_(AiUInt32 module, AiUInt8 biu, AiUInt32 id)
{
    AiInt16 ret = API_OK;
    TY_DEVICE_INFO * pDevice = _ApiGetDeviceInfoPtrByModule(module);

    if (pDevice == NULL)
        return API_ERR_NO_MODULE_EXTENSION;

    if ((pDevice->x_Config.uc_DataQueueMode == 1) && _ApiCmdDataQueueDirectIsBmRecording(id))
    {
        ret = ApiCmdDataQueueCloseDirect_(module, 0, id);
    }
    else if (GET_SERVER_ID(module))
    {
        ret = _MilNetDataQueueClose(module, 0, id);
    }
    else
    {
        /* Close ringbuffer (shared) filled by TSW */
        ret = ApiCmdDataQueueCloseBuffer_(module, 0, id);
    }

    return ret;
}

//**************************************************************************
//
//  _ApiCmdDataQueueDirectClose
//
//**************************************************************************
AiInt16 ApiCmdDataQueueCloseDirect_(AiUInt32 ul_Module, AiUInt8 uc_Biu, AiUInt32 id )
{
    TY_DEVICE_INFO * pDevice = _ApiGetDeviceInfoPtrByModule(ul_Module);


    if (pDevice->pxDataQueueDirectSetup[id] == NULL)
        /* nothing to do */
        return API_OK;

    AiOsFree(pDevice->pxDataQueueDirectSetup[id]);

    pDevice->pxDataQueueDirectSetup[id] = NULL;

    return API_OK;
}

AiInt16 ApiCmdDataQueueCloseBuffer_(AiUInt32 module, AiUInt8 biu, AiUInt32 id)
{
    AiInt16   ret            = API_OK;
    TY_MIL_COM_LS_BM_DATAQUEUE_OPEN_INPUT  xInput;
    TY_MIL_COM_ACK                         xOutput;

    TY_DEVICE_INFO * pDevice = _ApiGetDeviceInfoPtrByModule(module);

    if (pDevice == NULL)
        return API_ERR_NO_MODULE_EXTENSION;

    if (DQUEUE_OPENED != pDevice->ax_QueueTable[id].uc_QueueFlag)
        /* DQ not opened */
        return 0;

    pDevice->ax_QueueTable[id].uc_QueueFlag = DQUEUE_CLOSED;

    xInput.id   = id;

    MIL_COM_INIT(&xInput.cmd, API_STREAM(module), 0, MIL_COM_LS_BM_DATAQUEUE_CLOSE, sizeof(xInput), sizeof(xOutput));

    ret = API_CMD_STRUCT_BM_DATAQUEUE_CLOSE(module, &xInput, &xOutput);

    return ret;
}
//***************************************************************************
//
//   Module : ApiLsDQue                Submodule : ApiCmdDataQueueControl
//
//   Author : Patrick Giesel           Project   : E2HLBA
//
//---------------------------------------------------------------------------
//   Create : 03.12.02
//---------------------------------------------------------------------------
//
//    Description : This function controls a data queue (Start, Stop, Resume, Flush)
//
//***************************************************************************

AI_LIB_FUNC AiReturn AI_CALL_CONV ApiCmdDataQueueControl(AiUInt32 ul_Module, AiUInt32 id, AiUInt32 mode)
{
    AiInt16 uw_RetVal = API_OK;
    TY_DEVICE_INFO * pDevice = _ApiGetDeviceInfoPtrByModule(ul_Module);

    if (pDevice == NULL)
        return API_ERR_NO_MODULE_EXTENSION;

    if (Aiul_DbgLevel & DBG_PARAMCHK)
    {
        if (id > MAX_DATA_QUEUE_ID)
            uw_RetVal = API_ERR_DQUEUE_ID_NOT_IN_RANGE;
        else if (mode > MAX_DATA_QUEUE_CONTROL_MODE)
            uw_RetVal = API_ERR_DQUEUE_CONTROL_MODE_NOT_IN_RANGE;
    }

    if (API_OK == uw_RetVal)
    {
        uw_RetVal = ApiCmdDataQueueControl_(ul_Module, 0, id, mode );
    }


    v_ExamineRetVal("ApiCmdDataQueueControl", uw_RetVal);
    TRACE_BEGIN
    TRACE1("        id   = 0x%x;\n", id);
    TRACE1("        mode = 0x%x;\n", mode);
    TRACE_FCTA("ApiCmdDataQueueControl", uw_RetVal);
    TRACE_PARA(ul_Module);
    TRACE_FCTE;
    TRACE_END
        return(uw_RetVal);
}


//**************************************************************************
//
//  ApiCmdDataQueueControl_
//
//**************************************************************************
AiInt16 ApiCmdDataQueueControl_(AiUInt32 module, AiUInt8 biu, AiUInt32 id, AiUInt32 mode)
{
    AiInt16 ret = API_OK;

    TY_DEVICE_INFO * pDevice = _ApiGetDeviceInfoPtrByModule(module);

    if (pDevice == NULL)
        return API_ERR_NO_MODULE_EXTENSION;


    if ((pDevice->x_Config.uc_DataQueueMode == 1) && _ApiCmdDataQueueDirectIsBmRecording(id))
    {
        ret = ApiCmdDataQueueControlDirect_(module, biu, id, mode);
    }
    else if (GET_SERVER_ID(module))
    {
        ret = _MilNetDataQueueControl(module, biu, id, mode);
    }
    else
    {
        /* Queue control: Start stop/recording in TSW */
        /* Same code for buffer and pipe */
        return ApiCmdDataQueueControlTsw_(module, biu, id, mode);
    }

    return ret;
}

//**************************************************************************
//
//  ApiCmdDataQueueDirectControl_
//
//**************************************************************************
AiInt16 ApiCmdDataQueueControlDirect_(AiUInt32 module, AiUInt8 biu, AiUInt32 id, AiUInt32 mode)
{
    AiUInt32         ulCbOffset = 0;
    AiUInt8          ucBiu      = 0;
    AiInt16          wRetVal = 0;

    TY_DEVICE_INFO * pDevice = _ApiGetDeviceInfoPtrByModule(module);
    TY_API_DATA_QUEUE_DIRECT_SETUP * pSetup = pDevice->pxDataQueueDirectSetup[id];

    ucBiu = _ApiCmdDataQueueDirectGetBiu(id);
    ulCbOffset = pDevice->x_MemInfo.ax_BiuAddr[ucBiu].ul_Cb;

    switch (mode)
    {
    case API_DATA_QUEUE_CTRL_MODE_START:
        pSetup->ucTriggered = 0;
        break;
    case API_DATA_QUEUE_CTRL_MODE_FLUSH:
        /* Nothing to do here */
        break;
    case API_DATA_QUEUE_CTRL_MODE_STOP:
        /* Nothing to do here */
        break;
    default:
        return API_ERR_PLATTFORM_NOT_SUPPORTED;
        break;
    }


    return API_OK;
}

AiInt16 ApiCmdDataQueueControlTsw_(AiUInt32 module, AiUInt8 biu, AiUInt32 id, AiUInt32 mode)
{
    TY_MIL_COM_LS_BM_DATAQUEUE_CONTROL_INPUT xInput;
    TY_MIL_COM_ACK                           xOutput;
    AiInt16  ret = API_OK;

    TY_DEVICE_INFO * pDevice = _ApiGetDeviceInfoPtrByModule(module);

    if (pDevice == NULL)
        return API_ERR_NO_MODULE_EXTENSION;

    if (DQUEUE_OPENED != pDevice->ax_QueueTable[id].uc_QueueFlag)
        return API_ERR_DQUEUE_ASP_OPEN;

    xInput.id   = id;
    xInput.mode = mode;

    MIL_COM_INIT(&xInput.cmd, API_STREAM(module), 0, MIL_COM_LS_BM_DATAQUEUE_CONTROL, sizeof(xInput), sizeof(xOutput));

    ret = API_CMD_STRUCT_BM_DATAQUEUE_CONTROL(module, &xInput, &xOutput);

    return ret;
}


//***************************************************************************
//
//   Module : ApiLsDQue                Submodule : ApiCmdDataQueueRead
//
//   Author : Patrick Giesel           Project   : E2HLBA
//
//---------------------------------------------------------------------------
//   Create : 03.12.02
//---------------------------------------------------------------------------
//
//    Description : This function reads data from a data queue
//
//***************************************************************************
char dbgBuf2[1000];
AI_LIB_FUNC AiReturn AI_CALL_CONV ApiCmdDataQueueRead(AiUInt32 ul_Module, TY_API_DATA_QUEUE_READ * queue_read, TY_API_DATA_QUEUE_STATUS * info)
{
    AiInt16 uw_RetVal = API_OK;
    TY_DEVICE_INFO * pDevice = _ApiGetDeviceInfoPtrByModule(ul_Module);

    if (pDevice == NULL)
        return API_ERR_NO_MODULE_EXTENSION;

    if (Aiul_DbgLevel & DBG_PARAMCHK)
    {
        if (NULL == queue_read)
            uw_RetVal = API_ERR_PARAM2_IS_NULL;
        else if (NULL == info)
            uw_RetVal = API_ERR_PARAM3_IS_NULL;
        else if (queue_read->id > MAX_DATA_QUEUE_ID)
            uw_RetVal = API_ERR_DQUEUE_ID_NOT_IN_RANGE;
        else if ( (queue_read->bytes_to_read > 0) && (queue_read->buffer == NULL))
            uw_RetVal = API_ERR_BUFFER_NOT_ALLOCATED;
    }

    if (API_OK == uw_RetVal)
    {
        uw_RetVal = ApiCmdDataQueueRead_(ul_Module, 0, queue_read, info );
    }

    v_ExamineRetVal("ApiCmdDataQueueRead", uw_RetVal);
    TRACE_BEGIN
    TRACE("        TY_API_DATA_QUEUE_READ queue_read;\n");
    TRACE1("        queue_read.pv_MemBuf       = 0x%p;\n", queue_read->buffer);
    TRACE1("        queue_read.uc_Id           = 0x%x;\n", queue_read->id);
    TRACE1("        queue_read.bytes_to_read   = 0x%x;\n", queue_read->bytes_to_read);
    TRACE_FCTA("ApiCmdDataQueueRead", uw_RetVal);
    TRACE_PARA(ul_Module);
    TRACE_RPARE("&info");
    TRACE_FCTE;
    TRACE1("    --> info.status                 = 0x%x\n", info->status);
    TRACE1("    --> info.bytes_transfered       = 0x%x\n", info->bytes_transfered);
    TRACE1("    --> info.bytes_in_queue         = 0x%x\n", info->bytes_in_queue);
    TRACE1("    --> info.total_bytes_transfered = 0x%llx\n", info->total_bytes_transfered);
    TRACE_END
        return(uw_RetVal);
}


//**************************************************************************
//
//  ApiCmdDataQueueRead_
//
//**************************************************************************
AiInt16 ApiCmdDataQueueRead_(AiUInt32 module, AiUInt8 biu, TY_API_DATA_QUEUE_READ *data_queue_read, TY_API_DATA_QUEUE_STATUS * info)
{
    TY_DEVICE_INFO * pDevice = _ApiGetDeviceInfoPtrByModule(module);

    if ((pDevice->x_Config.uc_DataQueueMode == 1) && _ApiCmdDataQueueDirectIsBmRecording(data_queue_read->id))
    {
        /* Direct mode: Data is copied from GRAM into user buffer without queue
                        This mode is the default for AYE and AYS. The reason is that
                        we want to use DMA in this case.
                        */
        return ApiCmdDataQueueReadDirect_(module, 0, data_queue_read, info);
    }
    else if (GET_SERVER_ID(module))
    {
        /* Network: In case of Network we always read the data from ANS. */
        return _MilNetDataQueueRead(module, biu, data_queue_read, info);
    }
    else
    {
        /* Queue is ringbuffer (shared) filled by TSW */
        return ApiCmdDataQueueReadBuffer_(module, biu, data_queue_read, info);
    }

    return 0;
}




/*

The direct recording from ApiCmdDataQueueReadDirect_ shall be implemented in separate functions as soon as the FW
supports correct put/get pointer handling.

AiInt16 _ApiBMDataReadGetPutGet( AiUInt32 ulModHandle, AiUInt8 ucBiu, AIUInt8 ulHs, AiUInt32 & pulPut, AiUInt32 & pulGet );
AiInt16 _ApiBMDataReadSetGut(    AiUInt32 ulModHandle, AiUInt8 ucBiu, AIUInt8 ulHs, AiUInt32 ulPut );
AiInt16 ApiBMDataRead(           AiUInt32 ulModHandle, AiUInt8 ucBiu, AIUInt8 ulHs, AiUInt32 ulSize, AiUInt32 * pData, AiUInt32 & pulBytesRead );
*/


//**************************************************************************
//
//  ApiCmdDataQueueReadDirect_
//
//**************************************************************************
AiInt16 ApiCmdDataQueueReadDirect_(AiUInt32 ulModHandle, AiUInt8 ucBiu, TY_API_DATA_QUEUE_READ *pxQueue, TY_API_DATA_QUEUE_STATUS * info)
{
    AiInt16          wRetVal = API_OK;
    AiUInt8          ucHs = 0;
    AiUInt32         ulBytesTransfered = 0;
    AiUInt32         ulBmGet = 0;
    AiUInt32         ulBmPut = 0;
    AiUInt32         ulCopyPart1 = 0;
    AiUInt32         ulCopyPart2 = 0;
    TY_DEVICE_INFO * pDevice = _ApiGetDeviceInfoPtrByModule(ulModHandle);
    AiUInt32         ulCbOffset = 0;
    AiUInt32         ulLoad = 0;
    char             buf[200];
    AiUInt32         ulStatus = 0;
    AiUInt32         ulMonitorStatusOverflowBitpos = 0;

    TY_API_DATA_QUEUE_DIRECT_SETUP * pSetup = pDevice->pxDataQueueDirectSetup[pxQueue->id];

    TY_API_BM_STACKP_DSP    xLsStack;
#ifdef _API3910    
    TY_API_HS_BM_STACKP_DSP xHsStack;
    memset(&xHsStack, 0, sizeof(xHsStack));
#endif
    
    memset(&xLsStack, 0, sizeof(xLsStack));

    ucBiu = _ApiCmdDataQueueDirectGetBiu(pxQueue->id);
    ulCbOffset = pDevice->x_MemInfo.ax_BiuAddr[ucBiu].ul_Cb;


    /* -- Check environment --- */
    if (pSetup == NULL)
    {
        /* Queue is not opened */
        info->status &= ~(1 << 31);
        return API_OK;
    }



    /* -- check if queue is HS or LS --- */

    ucHs = _ApiCmdDataQueueDirectIsHs(pDevice->x_BoardInfo.ul_DeviceType, ucBiu);

    if (ucHs)
        ulMonitorStatusOverflowBitpos = 24;
    else
        ulMonitorStatusOverflowBitpos = 18;

    /* -- Initialize output --- */

    info->bytes_in_queue         = 0;
    info->bytes_transfered       = 0;
    info->total_bytes_transfered = pSetup->ulTotalBytesTransfered;
    


    /* -- get trigger and BM pointers --- */

    if (pSetup->ucTriggered == 0)
    {
        if (ucHs)
        {
#ifdef _API3910
            wRetVal = Api3910CmdHsBMStackpRead(ulModHandle, &xHsStack);

            if (wRetVal != API_OK)
                return wRetVal;

            pSetup->ucTriggered = xHsStack.uc_Status;

            ulBmGet = xHsStack.ul_Stp; /* Start of trace before trigger data */
            wRetVal = ApiReadMemData(ulModHandle, API_MEMTYPE_GLOBAL, ulCbOffset + API_FW_BM_MBFP, 4, &ulBmPut);

            if (wRetVal != API_OK)
                return wRetVal;
#endif
        }
        else
        {
            wRetVal = ApiCmdBMStackpRead(ulModHandle, ucBiu, &xLsStack);

            if (wRetVal != API_OK)
                return wRetVal;

            /* Do not increment etp if no data is available */
            if (xLsStack.stp == xLsStack.etp)
            {
                /* The BM triggered but did not store any data in the buffer yet.
                   We delay the trigger info until we get some data to avoid dealing with put==get cases
                */
                ulBmGet = xLsStack.stp;     /* Start of trigger, no trace before trigger handling */
                ulBmPut = xLsStack.etp;     /* End of BM data                                     */
            }
            else
            {
                /* This is the current behavior where etp points to the last written entry */
                /* The BM triggered and we have data in the buffer.
                   Store the trigger info in order to read the data directly next time.
                */
                ulBmGet = xLsStack.stp;     /* Start of trigger, no trace before trigger handling */
                ulBmPut = xLsStack.etp;     /* End of BM data                                     */

                pSetup->ucTriggered = xLsStack.status;
            }

        }

        /* Store trace before trigger start of data */
        ApiWriteMemData(ulModHandle, API_MEMTYPE_GLOBAL, ulCbOffset + API_FW_BM_GET, 4, &ulBmGet);

    }
    else
    {
        /* We know the BM triggered already so we avoid parsing the whole stack again with StackpRead */

        wRetVal = ApiReadMemData(ulModHandle, API_MEMTYPE_GLOBAL, ulCbOffset + API_FW_BM_MBFP, 4, &ulBmPut);

        if (wRetVal != API_OK)
            return wRetVal;

        wRetVal = ApiReadMemData(ulModHandle, API_MEMTYPE_GLOBAL, ulCbOffset + API_FW_BM_GET, 4, &ulBmGet);

        if (wRetVal != API_OK)
            return wRetVal;

    }


    /* Set queue enabled bit */
    info->status|= 1 << 31;


    /* -- Check if data is available --- */

    if (pSetup->ucTriggered == 0)
        /* No trigger - no data */
        return API_OK;


    if (ulBmGet == ulBmPut)
        /* No data */
        return API_OK;


    /* -- Calculate available size --- */

    if (ulBmGet < ulBmPut)
    {
        ulCopyPart1 = ulBmPut - ulBmGet;
        ulCopyPart2 = 0;
    }
    else
    {
        ulCopyPart1 = pSetup->ulBmEndOffset - ulBmGet;
        ulCopyPart2 = ulBmPut - pSetup->ulBmBaseOffset;
    }

    info->bytes_in_queue = ulCopyPart1 + ulCopyPart2;


    ulLoad = (info->bytes_in_queue * 100) / pSetup->ulBmSize;

    
    wRetVal = ApiReadMemData(ulModHandle, API_MEMTYPE_GLOBAL, ulCbOffset + API_FW_BM_MSTCB, 4, &ulStatus);
    /* FW reports overflow */
    if ((ulStatus >> ulMonitorStatusOverflowBitpos) & 0x1)
    {
       info->status |= 1 << 15;
    }


    if (pxQueue->bytes_to_read == 0)
        /* Read available size only */
        return API_OK;


    pSetup->ulLoadCount++;
    pSetup->ulLoadTotal = pSetup->ulLoadTotal + ulLoad;

    if (ulLoad > pSetup->ulLoadMax)
    {
        pSetup->ulLoadMax = ulLoad;

        sprintf(buf, "DQUEUE[%d] size = 0x%08X max = %d%% load / avg = %d%% load\n", pxQueue->id, pSetup->ulBmSize, pSetup->ulLoadMax, pSetup->ulLoadTotal / pSetup->ulLoadCount);
        DEBUGOUT(DBG_DQUEUE, __FUNCTION__, buf);
    }




    /* -- Read available data (limited by request size)--- */

    if (pxQueue->bytes_to_read < info->bytes_in_queue)
    {
        /* limit read size to requested size */
        if (pxQueue->bytes_to_read <= ulCopyPart1)
        {
            ulCopyPart2 = 0;
            ulCopyPart1 = pxQueue->bytes_to_read;
        }
        else
        {
            ulCopyPart2 = pxQueue->bytes_to_read - ulCopyPart1;
        }
    }

    /* -- Read Data between get and put --- */

    if (ulCopyPart1 > 0)
    {
        wRetVal = ApiReadBlockMemData(ulModHandle, API_MEMTYPE_GLOBAL, ulBmGet, 1, pxQueue->buffer, ulCopyPart1, &ulBytesTransfered);

        if (wRetVal != API_OK)
            return wRetVal;

        info->bytes_transfered = ulCopyPart1;


        ulBmGet = ulBmGet + ulCopyPart1;

        if (ulBmGet > pSetup->ulBmEndOffset)
            /* handle wrap around */
            ulBmGet -= pSetup->ulBmSize;

        /* write back get pointer */
        ApiWriteMemData(ulModHandle, API_MEMTYPE_GLOBAL, ulCbOffset + API_FW_BM_GET, 4, &ulBmGet);
    }

    /* -- Read Data between start of buffer --- */

    if (ulCopyPart2 > 0)
    {
        wRetVal = ApiReadBlockMemData(ulModHandle, API_MEMTYPE_GLOBAL, pSetup->ulBmBaseOffset, 1, (AiUInt8 *)pxQueue->buffer + ulCopyPart1, ulCopyPart2, &ulBytesTransfered);

        if (wRetVal != API_OK)
            return wRetVal;

        ulBmGet = ulBmGet + ulCopyPart2;

        if (ulBmGet > pSetup->ulBmEndOffset)
            /* handle wrap around */
            ulBmGet -= pSetup->ulBmSize;

        info->bytes_transfered += ulCopyPart2;


        /* write back get pointer */
        ApiWriteMemData(ulModHandle, API_MEMTYPE_GLOBAL, ulCbOffset + API_FW_BM_GET, 4, &ulBmGet);
    }


    /* -- Calculate and set available size / transfered size / remaining size --- */

    info->bytes_in_queue           -= info->bytes_transfered; /* Bytes transfered are no longer within the queue */
    pSetup->ulTotalBytesTransfered += info->bytes_transfered;
    info->total_bytes_transfered    = pSetup->ulTotalBytesTransfered;

    return API_OK;
}



AiInt16 ApiCmdDataQueueReadGetTswStatus_(AiUInt32 module, AiUInt8 id, AiUInt32 * queue_status, AiUInt32 * queue_put, AiUInt32 * queue_get, AiUInt32 * queue_data_start, AiUInt32 * queue_data_size)
{
    AiInt16 ret;
    AiUInt32 bytes_read;
    AiUInt32 queue_header_offset;

    TY_DEVICE_INFO * device = _ApiGetDeviceInfoPtrByModule(module);

    /*
    TSW (TY_API_DQUEUE_HEADER):
    AiUInt32  status;   => synchronize in
    AiUInt32  put;      => synchronize in
    AiUInt32  get;      => synchronize out
    AiUInt32  start;
    AiUInt32  size;

    DLL (TY_API_QUEUE_TABLE.TY_API_DATA_QUEUE_STATUS):
    AiUInt32  status;   => synchronize in

    DLL (locals)
    AiUInt32  put;      => synchronize in
    AiUInt32  get;      => synchronize out
    */
    queue_header_offset = device->ax_QueueTable[id].ul_QueueStart;

    ret = _ApiOsReadMemData(module, API_MEMTYPE_SHARED, queue_header_offset + API_DATA_QUEUE_TSW_OFFSET_STATUS, sizeof(AiUInt32), queue_status, 1, &bytes_read);

    if( ret != 0 )
        return ret;

    device->ax_QueueTable[id].x_QueueStatus.status = *queue_status;


    ret = _ApiOsReadMemData(module, API_MEMTYPE_SHARED, queue_header_offset + API_DATA_QUEUE_TSW_OFFSET_PUT, sizeof(AiUInt32), queue_put, 1, &bytes_read);

    if (ret != 0)
        return ret;

    ret = _ApiOsReadMemData(module, API_MEMTYPE_SHARED, queue_header_offset + API_DATA_QUEUE_TSW_OFFSET_GET, sizeof(AiUInt32), queue_get, 1, &bytes_read);

    if (ret != 0)
        return ret;

    ret = _ApiOsReadMemData(module, API_MEMTYPE_SHARED, queue_header_offset + API_DATA_QUEUE_TSW_OFFSET_START, sizeof(AiUInt32), queue_data_start, 1, &bytes_read);

    if (ret != 0)
        return ret;

    ret = _ApiOsReadMemData(module, API_MEMTYPE_SHARED, queue_header_offset + API_DATA_QUEUE_TSW_OFFSET_SIZE, sizeof(AiUInt32), queue_data_size, 1, &bytes_read);

    if (ret != 0)
        return ret;

    return 0;
}


AiInt16 ApiCmdDataQueueWriteDataQueueGetValue_(AiUInt32 module, AiUInt8 id, AiUInt32 queue_get)
{
    AiUInt32 bytes_written;
    AiUInt32 queue_header_offset;

    TY_DEVICE_INFO * device = _ApiGetDeviceInfoPtrByModule(module);

    queue_header_offset = device->ax_QueueTable[id].ul_QueueStart;

    return _ApiOsWriteMemData(module, API_MEMTYPE_SHARED, queue_header_offset + API_DATA_QUEUE_TSW_OFFSET_GET, sizeof(AiUInt32), &queue_get, 1, &bytes_written);
}

AiInt16 ApiCmdDataQueueReadUpdateStatus_(AiUInt32 module, AiUInt8 biu, AiUInt32 available_bytes, AiUInt32 transfered_bytes, AiUInt32 id, TY_API_DATA_QUEUE_STATUS * info)
{
    TY_API_QUEUE_TABLE * dll_data_queue_header;
    TY_DEVICE_INFO * device = _ApiGetDeviceInfoPtrByModule(module);

    dll_data_queue_header = &device->ax_QueueTable[id];

    memset(info, 0, sizeof(TY_API_DATA_QUEUE_STATUS));

    dll_data_queue_header->x_QueueStatus.total_bytes_transfered += transfered_bytes;

    info->bytes_transfered       = transfered_bytes;
    info->bytes_in_queue         = available_bytes;
    info->total_bytes_transfered = dll_data_queue_header->x_QueueStatus.total_bytes_transfered;
    info->status                 = dll_data_queue_header->x_QueueStatus.status;

    return 0;
}

AiInt16 ApiCmdDataQueueReadBuffer_(AiUInt32 module, AiUInt8 biu, TY_API_DATA_QUEUE_READ *data_queue_read, TY_API_DATA_QUEUE_STATUS * info)
{
    AiInt16  ret;
    AiUInt32 queue_status;
    AiUInt32 queue_data_start;
    AiUInt32 queue_data_size;
    AiUInt32 queue_put;
    AiUInt32 queue_get;
    AiUInt32 queue_memtype;
    AiUInt32 bytes_available;
    AiUInt32 bytes_to_end;
    AiUInt32 bytes_copied;
    AiUInt32 bytes_chunk_1;
    AiUInt32 bytes_chunk_2;
    AiUInt32 bytes_read;

    TY_DEVICE_INFO * dev = _ApiGetDeviceInfoPtrByModule(module);

    if (DQUEUE_OPENED != dev->ax_QueueTable[data_queue_read->id].uc_QueueFlag)
        /* DQ not open */
        return API_ERR_DQUEUE_ASP_OPEN;


    /* -- Get configuration --- */
    queue_memtype = dev->x_Config.uc_DataQueueMemoryType;

    /*-- Read the pointers and status from the TSW queue --- */
    ret = ApiCmdDataQueueReadGetTswStatus_(module, data_queue_read->id, &queue_status, &queue_put, &queue_get, &queue_data_start, &queue_data_size);

    if( ret != 0 )
        return ret;

    /*-- Calculate available data size --- */

    bytes_available = mil_tsw_buffer_consumer_available_bytes(queue_data_size, queue_put, queue_get);

    ret = ApiCmdDataQueueReadUpdateStatus_(module, biu, bytes_available, 0, data_queue_read->id, info);

    if( ret != 0 ) return ret;

    if (data_queue_read->bytes_to_read == 0)
        /* We only return the available bytes and the status */
        return 0;


    /*-- Limit available data size --- */

    if (bytes_available > data_queue_read->bytes_to_read)
        bytes_available = data_queue_read->bytes_to_read;


    /*-- Detect which areas of the buffer have to be copied --- */

    bytes_to_end = mil_tsw_buffer_bytes_to_end(queue_data_size, queue_data_start, queue_get);

    if (bytes_available <= bytes_to_end)
    {
        bytes_chunk_1 = bytes_available;
        bytes_chunk_2 = 0;
    }
    else
    {
        bytes_chunk_1 = bytes_to_end;
        bytes_chunk_2 = bytes_available - bytes_to_end;
    }

    /* -- Copy chunk 1 --- */
    if (bytes_chunk_1 )
    {
        ret = _ApiOsReadMemData(module, queue_memtype, queue_get, 1, data_queue_read->buffer, bytes_chunk_1, &bytes_read);

        if( ret != 0 )
            return ret;
    }

    /* -- Copy chunk 2 --- */
    if (bytes_chunk_2)
    {
        ret = _ApiOsReadMemData(module, queue_memtype, queue_data_start, 1, (AiUInt8*)data_queue_read->buffer + bytes_chunk_1, bytes_chunk_2, &bytes_read);

        if( ret != 0 )
            return ret;
    }

    bytes_copied = bytes_available;

    /* -- Update get pointer --- */
    mil_tsw_buffer_increment_offset(&queue_get, bytes_available, queue_data_start, queue_data_size);

    /* -- Update status --- */
    ret = ApiCmdDataQueueWriteDataQueueGetValue_(module, data_queue_read->id, queue_get);

    if( ret != 0 )
        return ret;

    /* -- Recalculate size for next read --- */
    ret = ApiCmdDataQueueReadGetTswStatus_(module, data_queue_read->id, &queue_status, &queue_put, &queue_get, &queue_data_start, &queue_data_size);

    if (ret != 0)
        return ret;

    bytes_available = mil_tsw_buffer_consumer_available_bytes(queue_data_size, queue_put, queue_get);

    /* -- Update the user info --- */
    ret = ApiCmdDataQueueReadUpdateStatus_(module, biu, bytes_available, bytes_copied, data_queue_read->id, info);

    if (ret != 0)
        return ret;


    return 0;
}




AiUInt8 _ApiCmdDataQueueDirectGetBiu(AiUInt8 ucId)
{
    AiUInt8 ucBiu = 0;

    /* BM Recording */
    if      (ucId == API_DATA_QUEUE_ID_BM_REC_BIU1)    ucBiu = 0;
    else if (ucId == API_DATA_QUEUE_ID_BM_REC_BIU2)    ucBiu = 1;
    else if (ucId == API_DATA_QUEUE_ID_BM_REC_BIU3)    ucBiu = 2;
    else if (ucId == API_DATA_QUEUE_ID_BM_REC_BIU4)    ucBiu = 3;
    else if (ucId == API_DATA_QUEUE_ID_BM_REC_BIU5)    ucBiu = 4;
    else if (ucId == API_DATA_QUEUE_ID_BM_REC_BIU6)    ucBiu = 5;
    else if (ucId == API_DATA_QUEUE_ID_BM_REC_BIU7)    ucBiu = 6;
    else if (ucId == API_DATA_QUEUE_ID_BM_REC_BIU8)    ucBiu = 7;
    /* Milscope */
    else if (ucId == API_DATA_QUEUE_ID_MIL_SCOPE)      ucBiu = 0;

    return ucBiu;
}

AiUInt8 _ApiCmdDataQueueDirectIsBmRecording(AiUInt8 ucId)
{
    AiUInt8 ucBmRecording = 0;

    /* BM Recording */
    if      (ucId == API_DATA_QUEUE_ID_BM_REC_BIU1)    ucBmRecording = 1;
    else if (ucId == API_DATA_QUEUE_ID_BM_REC_BIU2)    ucBmRecording = 1;
    else if (ucId == API_DATA_QUEUE_ID_BM_REC_BIU3)    ucBmRecording = 1;
    else if (ucId == API_DATA_QUEUE_ID_BM_REC_BIU4)    ucBmRecording = 1;
    else if (ucId == API_DATA_QUEUE_ID_BM_REC_BIU5)    ucBmRecording = 1;
    else if (ucId == API_DATA_QUEUE_ID_BM_REC_BIU6)    ucBmRecording = 1;
    else if (ucId == API_DATA_QUEUE_ID_BM_REC_BIU7)    ucBmRecording = 1;
    else if (ucId == API_DATA_QUEUE_ID_BM_REC_BIU8)    ucBmRecording = 1;
    /* Milscope */
    else if (ucId == API_DATA_QUEUE_ID_MIL_SCOPE)      ucBmRecording = 0;

    return ucBmRecording;
}



AiUInt8 _ApiCmdDataQueueDirectIsHs(AiUInt8 ucDeviceType, AiUInt8 ucId)
{
    switch (ucDeviceType)
    {
    case TYPE_APX3910:
    case TYPE_APX3910Xp:
    case TYPE_APXX3910:
    case TYPE_APXX3910Xp:
    case TYPE_APEX3910:
    case TYPE_APEX3910Xp:
    case TYPE_ACE3910:
    case TYPE_ACE3910Xp:
    case TYPE_AXC3910:
    case TYPE_AXC3910Xp:
    case TYPE_ACX3910:
    case TYPE_ACX3910Xp:
    case TYPE_AVX3910:
    case TYPE_AVX3910Xp:
    case TYPE_ANET3910:
    case TYPE_ANET3910Xp:
        if (ucId == 1)
            return 1;
        break;

    case TYPE_ACX3910_2:
    case TYPE_ACX3910Xp_2:
    case TYPE_AVX3910_2:
    case TYPE_AVX3910Xp_2:
        if ((ucId == 1) || (ucId == 3))
            return 1;
        break;

    case TYPE_ACX_EFA_1_TWO_PBIS:
    case TYPE_ACX_EFAXp_1_TWO_PBIS:
    case TYPE_ACX_EFA_1_DS_TWO_PBIS:
    case TYPE_ACX_EFAXp_1_DS_TWO_PBIS:
    case TYPE_AVX_EFA_1_TWO_PBIS:
    case TYPE_AVX_EFA_1_DS_TWO_PBIS:
    case TYPE_AVX_EFAXp_1_TWO_PBIS:
    case TYPE_AVX_EFAXp_1_DS_TWO_PBIS:
        if (ucId == 2)
            return 1;
        break;

    case TYPE_ACX_EFA_2_TWO_PBIS:
    case TYPE_ACX_EFAXp_2_TWO_PBIS:
    case TYPE_ACX_EFA_2_DS_TWO_PBIS:
    case TYPE_ACX_EFAXp_2_DS_TWO_PBIS:
    case TYPE_AVX_EFA_2_TWO_PBIS:
    case TYPE_AVX_EFAXp_2_TWO_PBIS:
    case TYPE_AVX_EFA_2_DS_TWO_PBIS:
    case TYPE_AVX_EFAXp_2_DS_TWO_PBIS:
        if (ucId == 3)
            return 1;
        break;

    case TYPE_ACX_EFA_4_TWO_PBIS:
    case TYPE_ACX_EFAXp_4_TWO_PBIS:
    case TYPE_AVX_EFA_4_TWO_PBIS:
    case TYPE_AVX_EFAXp_4_TWO_PBIS:
        if (ucId == 16)
            return 1;
        break;

    default:
        break;
    }

    return 0;
}



AI_LIB_FUNC AiReturn AI_CALL_CONV ApiAnsDataQueueOpen(AiUInt32 ul_Module, AiUInt32 id, AiUInt32 * queue_size)
{
    return ApiCmdDataQueueOpen(ul_Module, id, queue_size);
}

AI_LIB_FUNC AiReturn AI_CALL_CONV ApiAnsDataQueueClose(AiUInt32 ul_Module, AiUInt32 id)
{
    return ApiCmdDataQueueClose(ul_Module, id);
}

AI_LIB_FUNC AiReturn AI_CALL_CONV ApiAnsDataQueueControl(AiUInt32 ul_Module, AiUInt32 id, AiUInt32 mode)
{
    return ApiCmdDataQueueControl(ul_Module, id, mode);
}

AI_LIB_FUNC AiReturn AI_CALL_CONV ApiAnsDataQueueRead(AiUInt32 ul_Module, TY_API_DATA_QUEUE_READ *px_QueueData, TY_API_DATA_QUEUE_STATUS * info)
{
    return ApiCmdDataQueueRead(ul_Module, px_QueueData, info);
}



