/* SPDX-FileCopyrightText: 2001-2022 AIM GmbH <info@aim-online.com> */
/* SPDX-License-Identifier: MIT */

/*! \file Dll_Fnc.h
 *
 * External References and Prototypes.
 * 
 */

#ifndef __DLL_FNH_H__
#define __DLL_FNH_H__

#if defined(_AIM_VME)
#ifdef __cplusplus
extern "C"{
#endif
#endif

// Prototypes


AiInt16 _ApiCmdGetSysDrvVersion(AiUInt32 bModule, TY_VER_INFO * version);
AiInt16 _ApiCmdSetDebugLevel(AiUInt32 bModule, AiUInt32 ul_DbgLevel);
AiInt16 _ApiExtInit(AiUInt32 bModule, AiUInt32 par1, AiUInt32 par2);
void    _ApiInitMiscGlobals(void);
AiInt16 _ApiExtIo(AiUInt32 bModule, AiUInt8 *cmd, AiUInt8 *ack);

AiInt16 _ApiOpen(TY_API_OPEN *px_ApiOpen, AiUInt32 *bModuleHandle);

AiInt16 _DelIntHandler( AiUInt32 bModule, AiUInt8 biu, AiUInt8 uc_Type );
AiInt16 _InstIntHandler(AiUInt32 bModule, AiUInt8 biu, AiUInt8 uc_Type, TY_INT_FUNC_PTR pf_IntFunc );
void    _StructureEndianChange(AiUInt16 strType, LPVOID pData, AiUInt32 strCount);
void    _SyncRelease(AiUInt32 bModule, AiHandle ah_Mutex );
void    _SyncWait(AiUInt32 bModule, AiHandle ah_Mutex );
AiHandle _ApiGetDrvHandle( AiUInt32 module);

AI_LIB_FUNC AiReturn AI_CALL_CONV ApiStartPerformanceTimer(AiUInt32 bModule);
AI_LIB_FUNC AiReturn AI_CALL_CONV _ApiOsExit(void);

AiInt16 _ApiReadMemData(AiUInt32 bModule, AiUInt8 memtype, AiUInt32 offset, AiUInt8 width, void* data_p);
AiInt16 _ApiWriteMemData(AiUInt32 bModule, AiUInt8 memtype, AiUInt32 offset, AiUInt8 width, void* data_p);
AiInt16 _ApiReadBlockMemData(AiUInt32 bModule, AiUInt8 memtype, AiUInt32 offset, AiUInt8 width, void* data_p, AiUInt32 size, AiUInt32 *pul_BytesRead );
AiInt16 _ApiWriteBlockMemData(AiUInt32 bModule, AiUInt8 memtype, AiUInt32 offset, AiUInt8 width, void* data_p, AiUInt32 size, AiUInt32 *pul_BytesWritten );








AiInt16 _ApiSetDeviceConfigDriverFlags( AiUInt32 ulModHandle, AiUInt32 ulDriverFlags );

extern TY_PNP_CALLBACK_FUNC_PTR v_MilPnPCallback;

void v_ExamineRetVal( const char *pc_FctName, AiUInt16 uw_RetVal );

/*! \brief OS specific function to flush the system driver interrupt queue.

    \param ul_ModuleHandle module handle of the board where scope is setup
    \return returns API_OK on success, an appropriate error code on failure*/
AiInt16 _ApiOsFlushInterrupts(AiUInt32 ul_ModuleHandle);

/*! \brief OS specific function read the driver info

    \param ul_ModuleHandle module handle of the board where scope is setup
    \param pxDriverInfo The driver info returned by the system driver
    \return returns API_OK on success, an appropriate error code on failure*/
AiInt16 _ApiOsGetDriverInfo(AiUInt32 ul_ModuleHandle, TY_API_DRIVER_INFO * pxDriverInfo );

/*! \brief OS specific function to read USB device specific memory areas

    Provides functionality to read from either EEProm or 8051 program RAM
    on USB based devices.by using OS specific driver calls
    \param [in] ulModHandle- The device handle of the board to read from
    \param [in] memtype - memory type to read from. Either EEProm or 8051 RAM. Defines are USB_EEPROM and USB_8051_RAM
    \param [in] offset - relative byte offset to start of corresponding memory area to read from
    \param [in] size - number of bytes to read
    \param [out] data - pointer to memory area to read data into
    \param [out] p_BytesRead - optional parameter that will receive number of bytes read
    \return returns API_OK on success */
AI_LIB_FUNC AiReturn AI_CALL_CONV _ApiReadUsbData( AiUInt32 ulModHandle, AiUInt8 memtype, AiUInt32 offset, AiUInt32 size, void* data, AiUInt32* p_BytesRead);


/*! \brief OS specific function to write USB device specific memory areas

    Provides functionality to write to either EEProm or 8051 program RAM
    on USB based devices.by using OS specific driver calls
    \param [in] ulModHandle - The module handle of the board to write to
    \param [in] memtype - memory type to write to. Either EEProm or 8051 RAM. Defines are USB_EEPROM and USB_8051_RAM
    \param [in] offset - relative byte offset to start of corresponding memory area to write to
    \param [in] size - number of bytes to write
    \param [in] data - pointer to memory area to write data from
    \param [out] p_BytesWritten - optional parameter that will receive number of bytes written
    \return returns API_OK on success */
AI_LIB_FUNC AiReturn AI_CALL_CONV _ApiWriteUsbData( AiUInt32 ulModHandle, AiUInt8 memtype, AiUInt32 offset, AiUInt32 size, void* data, AiUInt32* p_BytesWritten);

enum ScopeThreadStates
{
    ScopeThreadExited = 0,      /*!< scope thread is terminated */
    ScopeThreadSuspended = 1,   /*!< scope thread is suspended and waiting for restart or termination */
    ScopeThreadStarted = 2      /*!< scope thread is running */
};

/*! \brief Creates a scope buffer

    This function creates a scope buffer of type \ref TY_API_SCOPE_BUFFER ,
    that can be used for recording scope data. \n
    It has to be freed with \ref ApiFreeScopeBuffer when no more needed. \n
    A user specific ID, a handler function that is called when the buffer is completed or cancelled
    and a user definable data pointer can be specified for the buffer..
    \param ulModuleHandle handle to the device to create scope buffer for
    \param ulID ID of the scope buffer
    \param eBufferType the buffer's type according to \ref TY_API_SCOPE_BUFFER_TYPE enumeration.
    \param BufferHandler pointer to buffer handler function to set for the buffer.
    \param pvUserData pointer to user definable data that can be used in buffer handler
    \return returns API_OK on success,  an appropriate error code otherwise */
AI_LIB_FUNC TY_API_SCOPE_BUFFER* AI_CALL_CONV _ApiCreateScopeBuffer(AiUInt32 ulModuleHandle, AiUInt32 ulID, TY_API_SCOPE_BUFFER_TYPE eBufferType,
                                                                     TY_API_SCOPE_BUFFER_HANDLER BufferHandler, void* pvUserData);


AI_LIB_FUNC AiReturn AI_CALL_CONV _ApiFreeScopeBuffer(AiUInt32 ulModuleHandle, TY_API_SCOPE_BUFFER *px_Buffer);


/*! \brief Starts scope on a given board

    Branches off into an OS specific function.
    \param[in] ul_Module Handle of the board to start scope on
    \return returns API_OK on success */
AI_LIB_FUNC AiReturn AI_CALL_CONV _ApiCmdScopeStart(AiUInt32 ul_Module);


/*! \brief Gets the scope status of a given board

    This function returns the current scope status of a board and the number of scope buffers 
    that are currently provided for recording scope data. \n 
    It checks if board is remote and then branches into ANS function
    \param[in] ulModuleHandle handle of board to get status of
    \param[out] p_ScopeStatus receives the scope status
    \param[out] pulNumBuffersLeft receives number of scope buffers available
    \return returns API_OK on success, an appropriate error code on failure */
AI_LIB_FUNC AiReturn AI_CALL_CONV _ApiCmdScopeStatus(AiUInt32 ulModuleHandle, TY_API_SCOPE_STATUS* p_ScopeStatus, AiUInt32* pulNumBuffersLeft);


/*! \brief Stops the scope of a board

    This function checks if board is remote and then branches off into ANS function
    On local boards, the scope stop target command is issued
    \param[in] ulModuleHandle handle of board to stop scope of.
    \return returns API_OK on success, an appropriate error code on failure */
AI_LIB_FUNC AiReturn AI_CALL_CONV _ApiCmdScopeStop(AiUInt32 ulModuleHandle);

AI_LIB_FUNC AiReturn AI_CALL_CONV _ApiProvideScopeBuffers  (AiUInt32 ulModuleHandle, AiUInt32 ulNumBuffers, TY_API_SCOPE_BUFFER* axScopeBuffers[]);


/*! \brief OS specific function for  setting up scope

    This function initializes all OS specific parts that
    are mandatory for the scope thread.
    \param ul_ModuleHandle module handle of the board where scope is setup
    \return returns API_OK on success, an appropriate error code on failure*/
AiInt16 _API_DLL_FUNC _ApiCmdScopeSetup(AiUInt32 ul_ModuleHandle, TY_API_SCOPE_SETUP *px_ScopeSetup);
AiInt16 _API_DLL_FUNC _ApiOsCmdScopeSetup(AiUInt32 ul_ModuleHandle);


/*! \brief OS specific function for starting scope

    This function starts the scope thread which is waiting for scope buffers
    to be completed.
    \param ul_ModuleHandle module handle of the board where scope is started
    \return returns API_OK on success, an appropriate error code on failure*/
AiInt16 _ApiOsCmdScopeStart(AiUInt32 ul_ModuleHandle);

/*! \brief OS specific function for  stopping scope

    This function suspends the scope thread which is waiting for scope buffers
    to be completed.
    \param ul_ModuleHandle module handle of the board where scope is stopped
    \return returns API_OK on success, an appropriate error code on failure*/
AiInt16 _ApiOsCmdScopeStop(AiUInt32 ul_ModuleHandle);


/*! \brief OS specific function for defining strobe when trigger event happenes

    This function defines strobe in case of scope trigger event (type, target)
    \param ulModuleHandle module handle of the board where scope is stopped
    \param px_ScopeTrgStrobe pointer to structure that holds trigger strobe settings
    \return returns API_OK on success, an appropriate error code on failure*/
AI_LIB_FUNC AiReturn AI_CALL_CONV _ApiCmdScopeTriggerStrobeDef( AiUInt32 ulModuleHandle, TY_API_SCOPE_TRG_STROBE *px_ScopeTrgStrobe);


/*! \brief Waits until scope capturing has finished

    This function waits until all provided scope buffers are filled,
    or canceled. Or until the specified time out is reached. \n
    It branches off into OS specific functions.
    \param [in] ulModuleHandle handle to the device to wait for
    \param [in] lTimeOut time out in milliseconds to wait
    \param [out] p_WaitResultFlags receives the result of the wait 
    \return returns API_OK on success, an appropriate error code otherwise */
AI_LIB_FUNC AiReturn AI_CALL_CONV _ApiWaitForScopeFinished(AiUInt32 ulModuleHandle, AiInt32 lTimeOut,
                                                           TY_API_SCOPE_WAIT_STATE* p_WaitResultFlags);


/*! \brief OS specific function that waits until scope capturing has finished

    This function waits until either the scope thread suspends, this is
    the case if all buffers are filled, or until time out is reached. \n
    To do this, condition variables are used for communicating the scope threads
    execution state.
    \param [in] ulModuleHandle handle to the device to wait for
    \param [in] lTimeOut time out in milliseconds to wait
    \param [out] p_WaitResultFlags receives the result of the wait 
    \return returns API_OK on success, an appropriate error code otherwise */
AiInt16 _ApiOsWaitForScopeFinished(AiUInt32 ulModuleHandle, AiInt32 lTimeOut,
                                   TY_API_SCOPE_WAIT_STATE* p_WaitResultFlags);


/*! \brief Resets scope functionality of a board

    This resets all scope related settings to their default state.
    At least \ref ApiCmdScopeSetup has to be called if you want to
    start scope capturing again.
    \param ulModuleHandle handle to the device to reset scope on
    \return returns API_OK on success, an appropriate error code otherwise */
AI_LIB_FUNC AiReturn AI_CALL_CONV _ApiCmdScopeReset(AiUInt32 ulModuleHandle);


/*! \brief Creates a list of scope buffers

    This function takes an array of pointers to \ref TY_API_SCOPE_BUFFER structures.\n
    Each of these pointers is set to a newly allocated scope buffer structure which
    has to be freed with \ref ApiFreeScopeBuffer when no more needed. \n
    The buffers are created with consecutive IDs, beginning from 0.
    \param ulModuleHandle handle to the device to create scope buffers for
    \param ulNumBuffers number of scope buffers to create
    \param paxBufferList array of pointers to \ref TY_API_SCOPE_BUFFER structures. At least ulNumBuffers entries are needed.
    \param pBufferHandler pointer to buffer handler function to set for the buffers.
    \param pvUserData pointer to user definable data
    \return returns API_OK on success,  an appropriate error code otherwise */
AI_LIB_FUNC AiReturn AI_CALL_CONV _ApiCreateScopeBufferList(AiUInt32 ulModuleHandle, AiUInt32 ulNumBuffers, TY_API_SCOPE_BUFFER* paxBufferList[], 
                                                            TY_API_SCOPE_BUFFER_HANDLER pBufferHandler, void* pvUserData);


/*! \brief This function is used to compensate the scope offsets from zero-line

    Depending on the mode requested, it will either perform some measurement probes \n
    in order to determine the pitch of the scope data from the zero line of an idle bus, \n
    or it will read the current offset settings, or reset them.
    \param ulModuleHandle handle to the device to compensate scope offsets for.
    \param ucMode signifies what task the function has to perform, e.g. measure, reset
    \param pxOffsets optional pointer to \ref TY_API_SCOPE_OFFSETS structure where current offsets will be returned
    \return returns API_OK on success,  an appropriate error code otherwise */
AiInt16 _ApiCmdScopeOffsetCompensation(AiUInt32 ulModuleHandle, AiUInt8 ucMode, TY_API_SCOPE_OFFSETS* pxOffsets);



/* Device management functions */
TY_DEVICE_INFO_LIST * _ApiCreateMilDeviceListInfo(     char * DevicePath);
TY_DEVICE_INFO      * _ApiGetDeviceInfoPtrByName(   char * DevicePath);
TY_DEVICE_INFO      * _ApiGetDeviceInfoPtrByModule( AiUInt32 ul_Module );
AiUInt32              _ApiDeleteMilDeviceListInfo(     TY_DEVICE_INFO_LIST * px_DevListEntry, 
                                                    TY_DEVICE_INFO      * px_DeviceInfo );
void                  _ApiClearDeviceList ( void );



#if defined(_AIM_VME)
#ifdef __cplusplus
}
#endif
#endif


// Internal definitions for structure endian switching (rh)
#define STR_UINT16_TYPE                1
#define STR_UINT32_TYPE                2
#define STR_TY_API_RT_SA_TYPE        3
#define STR_TY_API_HS_BM_FW_MON_BUFFER_ENTRY 4

// define timeout for provide numbers of buffers to server (loop counter)
#define RPC_TIMEOUT_NUMBER_OF_BUFFERS_SYNC 50


#endif /*__DLL_FNH_H__*/
