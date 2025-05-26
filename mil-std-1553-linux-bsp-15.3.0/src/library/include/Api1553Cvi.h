/* SPDX-FileCopyrightText: 2001-2024 AIM GmbH <info@aim-online.com> */
/* SPDX-License-Identifier: MIT */

/*! \file Api1553Cvi.h
 *
 *  CVI Wrapper DLL for 1553 Application Interface. Header file with function prototypes
 *
 */

#ifndef CVI_1553
#define CVI_1553

#include "AiOs.h"


//***************************************************************************
//  API1553 Functions - External References
//***************************************************************************

AI_LIB_FUNC AiReturn AI_CALL_CONV ApiInitCvi(void);
AI_LIB_FUNC AiReturn AI_CALL_CONV ApiExitCvi(void);

AI_LIB_FUNC AiReturn AI_CALL_CONV ApiOpenCvi(AiUInt32 bModule, AiChar *ac_SrvName, AiUInt32 *bModuleHandle);
AI_LIB_FUNC AiReturn AI_CALL_CONV ApiOpenExCvi(AiUInt32 bModule, AiUInt32 ul_Stream, AiChar *ac_SrvName, AiUInt32 *pul_ModuleHandle);
AI_LIB_FUNC AiReturn AI_CALL_CONV ApiCloseCvi(AiUInt32 bModule);


AI_LIB_FUNC AiReturn AI_CALL_CONV ApiGetDriverInfoCvi(    AiUInt32   ulModule,
                                                          AiUInt8  * puc_PlattformGroup,
                                                          AiUInt8  * puc_ReservedB2, 
                                                          AiUInt8  * puc_ReservedB3,
                                                          AiUInt8  * puc_ReservedB4, 
                                                          AiUInt16 * puw_ReservedW1,
                                                          AiUInt16 * puw_ReservedW2, 
                                                          AiUInt32 * pul_DriverFlags,
                                                          AiUInt32 * pul_SN, 
                                                          AiUInt32 * pul_BoardConfig,
                                                          AiUInt32 * pul_BoardType, 
                                                          AiUInt32 * pul_ReservedLW5,
                                                          AiUInt32 * pul_ReservedLW6,
                                                          AiUInt32 * pul_ReservedLW7,
                                                          AiUInt32 * pul_ReservedLW8 );
AI_LIB_FUNC AiReturn AI_CALL_CONV ApiSetDeviceConfigCvi(  AiUInt32 ulModHandle, AiUInt8 uc_DmaEnabled, AiUInt8 uc_DataQueueMemoryType,
                                                        AiUInt8 uc_ReservedB3, AiUInt8 uc_ReservedB4, AiUInt16 uw_ReservedW1, AiUInt16 uw_ReservedW2, 
                                                        AiUInt32 ul_DmaMinimumSize, AiUInt32 ul_IntRequestCount, AiUInt32 ulDriverFlags,
                                                        AiUInt32 ul_ReservedLW4, AiUInt32 ul_ReservedLW5, AiUInt32 ul_ReservedLW6, 
                                                        AiUInt32 ul_ReservedLW7, AiUInt32 ul_ReservedLW8 );
AI_LIB_FUNC AiReturn AI_CALL_CONV ApiGetDeviceConfigCvi(AiUInt32 ulModHandle, AiUInt8 *uc_DmaEnabled, AiUInt8 *uc_DataQueueMemoryType,
                                                        AiUInt8 *uc_ReservedB3, AiUInt8 *uc_ReservedB4, AiUInt16 *uw_ReservedW1, AiUInt16 *uw_ReservedW2, 
                                                        AiUInt32 *ul_DmaMinimumSize, AiUInt32 *ul_IntRequestCount, AiUInt32 *ul_DriverFlags,
                                                        AiUInt32 *ul_ReservedLW4, AiUInt32 *ul_ReservedLW5, AiUInt32 *ul_ReservedLW6, 
                                                        AiUInt32 *ul_ReservedLW7, AiUInt32 *ul_ReservedLW8 );

AI_LIB_FUNC AiReturn AI_CALL_CONV ApiGetBoardInfoCvi(AiUInt32 bModule, AiUInt32 *pul_DeviceType,
                                                  AiUInt32 *pul_NumberOfChannels, AiUInt32 *pul_NumberOfBiu,
                                                  AiUInt32 *pul_NovRamBoardType, AiUInt32 *pul_NovRamBoardConfig,
                                                  AiUInt32 *pul_SerialNumber, AiUInt32 *pul_PartNumber,
                                                  AiUInt32 *pul_GlobalRamSize, AiUInt32 *pul_SharedRamSize,
                                                  AiUInt32 *pul_CanChangeAmplitude, AiUInt32 *pul_CouplingCapabilities,
                                                  AiUInt32 *pul_IrigCapabilities );
AI_LIB_FUNC AiReturn AI_CALL_CONV ApiGetLibraryInfoCvi(AiUInt32 *pul_AttachedApps);
AI_LIB_FUNC const char *  AI_CALL_CONV ApiGetErrorDescriptionCvi(AiInt16 wErrorCode);
AI_LIB_FUNC const char *  AI_CALL_CONV ApiGetBoardNameCvi(AiUInt32 ulModHandle);
AI_LIB_FUNC const char *  AI_CALL_CONV ApiGetErrorMessageCvi(AiReturn error_code);
AI_LIB_FUNC AiReturn AI_CALL_CONV ApiInstIntHandlerCvi(AiUInt32 bModule, AiUInt8 biu, AiUInt8 uc_Type, TY_INT_FUNC_PTR pf_IntFunc);
AI_LIB_FUNC AiReturn AI_CALL_CONV ApiDelIntHandlerCvi(AiUInt32 bModule, AiUInt8 biu, AiUInt8 uc_Type);

//###############################################################
//    System Commands
//###############################################################

AI_LIB_FUNC AiReturn AI_CALL_CONV ApiCmdIniCvi(AiUInt32 bModule, AiUInt8 mode,
    AiUInt8 bt[2],
    AiUInt8 *chns,
    AiUInt8 *prot,
    AiUInt8 *emod,
    AiUInt8 *irg,
    AiUInt8 *res1,
    AiUInt16 *pbi_id_biu1,
    AiUInt16 *asp_mon_id,
    AiUInt16 *asp_bite_id,
    AiUInt16 *pbi_id_biu2,
    AiUInt32 *board_config,
    AiUInt32 *glb_mem_size,
    AiUInt32 *glb_mem_addr,
    AiUInt32 *loc_dram_size,
    AiUInt32 *loc_dram_addr,
    AiUInt32 *shared_dram_size,
    AiUInt32 *shared_dram_addr,
    AiUInt32 *flash_ram_size,
    AiUInt32 *flash_ram_addr,
    AiUInt32 pci[16],
    AiUInt32 * board_type,
    AiUInt32 * board_sub_type,
    AiUInt32 * hardware_variant );
AI_LIB_FUNC AiReturn AI_CALL_CONV ApiCmdResetCvi(AiUInt32 bModule, AiUInt8 biu, AiUInt8 rc,
    AiUInt8 *mbufs,
    AiUInt8 *sbufs,
    AiUInt32 *mon_addr,
    AiUInt32 *sim_addr);
AI_LIB_FUNC AiReturn AI_CALL_CONV ApiCmdBiteCvi(AiUInt32 bModule, AiUInt8 biu, AiUInt8 sc, AiUInt8 bite_status[]);
AI_LIB_FUNC AiReturn AI_CALL_CONV ApiCmdDefRespToutCvi(AiUInt32 bModule, AiUInt8 biu, AiFloat resp_tout);
AI_LIB_FUNC AiReturn AI_CALL_CONV ApiCmdReadSWVersionCvi(AiUInt32 bModule, AiUInt8 biu, AiUInt16 *fw_id, AiUInt16 *sw_id, AiUInt16 *lca_id, AiUInt32 *lca_chks);
AI_LIB_FUNC AiReturn AI_CALL_CONV ApiCmdLoadSRecCvi(AiUInt32 bModule, AiUInt8 mode, AiUInt8 cnt, AiUInt32 offset, AiUInt8 *srec, AiUInt8 *st, AiUInt32 *fsize);
AI_LIB_FUNC AiReturn AI_CALL_CONV ApiCmdProgFlashCvi(AiUInt32 bModule, AiUInt32 src_addr, AiUInt32 sector, AiUInt32 size, AiUInt8 *st);
AI_LIB_FUNC AiReturn AI_CALL_CONV ApiCmdExecSysCvi(AiUInt32 bModule, AiUInt8 mode, AiUInt8 con);
AI_LIB_FUNC AiReturn AI_CALL_CONV ApiCmdSetIrigTimeCvi(AiUInt32 bModule, AiUInt32 day, AiUInt32 hour, AiUInt32 minute, AiUInt32 second, AiUInt32 microsecond);
AI_LIB_FUNC AiReturn AI_CALL_CONV ApiCmdGetIrigTimeCvi(AiUInt32 bModule, AiUInt32 *day, AiUInt32 *hour, AiUInt32 *minute, AiUInt32 *second, AiUInt32 *microsecond);
AI_LIB_FUNC AiReturn AI_CALL_CONV ApiCmdSetIrigStatusCvi(AiUInt32 bModule, TY_API_IRIG_SOURCE irig_source);
AI_LIB_FUNC AiReturn AI_CALL_CONV ApiCmdGetIrigStatusCvi(AiUInt32 bModule, TY_API_IRIG_SOURCE * irig_source, AiBoolean * in_sync);

AI_LIB_FUNC AiReturn AI_CALL_CONV ApiCmdDefMilbusProtocolCvi(AiUInt32 bModule, AiUInt8 biu, AiUInt8 mil_prot, AiUInt8 mode, AiUInt8 rt);
AI_LIB_FUNC AiReturn AI_CALL_CONV ApiReadRecDataCvi(AiUInt32 bModule, AiUInt8 biu,
                                  void *lpBuf,
                                  AiUInt8 *status,
                                  AiUInt32 *hfi_cnt,
                                  AiUInt32 *saddr,
                                  AiUInt32 *size,
                                  AiUInt32 *lBytesRead);
AI_LIB_FUNC AiReturn AI_CALL_CONV ApiWriteRepDataCvi(AiUInt32 bModule, AiUInt8 biu,
        AiUInt8  *status,
        AiUInt8  *padding1,
        AiUInt16 *padding2,
        AiUInt32 *rpi_cnt,
        AiUInt32 saddr,
        AiUInt32 size,
        AiUInt32 *entry_cnt,
        void *lpBuf,
        AiUInt32 *lBytesWritten);


AI_LIB_FUNC AiReturn AI_CALL_CONV ApiCmdFifoIniCvi(AiUInt32 bModule, AiUInt8 biu, AiUInt8 fifo_nbr, AiUInt16 buf_nbr);
AI_LIB_FUNC AiReturn AI_CALL_CONV ApiReadBSPVersionCvi(AiUInt32 bModule,
                                                       AiUInt32 *pul_FirmwareVer, AiUInt32 *pul_TargetVer, AiUInt32 *pul_TargetBuild,
                                                       AiUInt32 *pul_LcaVer1, AiUInt32 *pul_LcaVer2, // added 08.01.01 pg
                                                       AiUInt32 *pul_LcaCheckSum,
                                                       AiUInt32 *pul_SysDrvVer, AiUInt32 *pul_SysDrvBuild, AiUInt32 *pul_DllVer,
                                                       AiUInt32 *pul_DllBuild,
                                                       AiUInt32 *pul_BoardSerialNr,
                                                       AiUInt8  *puc_BspCompatibility);             //added MR 31.10.2000
AI_LIB_FUNC AiReturn AI_CALL_CONV ApiReadBSPVersionExCvi(AiUInt32 bModule,
                                                       AiUInt32 aul_TcpVer[5], 
                                                       AiUInt32 aul_AslLcaVer[5],
                                                       AiUInt32 aul_PciLcaVer[5],
                                                       AiUInt32 aul_IoLcaBiu1Ver[5],
                                                       AiUInt32 aul_CoreLcaBiu1Ver[5],
                                                       AiUInt32 aul_IoLcaBiu2Ver[5],
                                                       AiUInt32 aul_EndDecLcaBiu2Ver[5],
                                                       AiUInt32 aul_IoLcaBiu3Ver[5],
                                                       AiUInt32 aul_CoreLcaBiu3Ver[5],
                                                       AiUInt32 aul_IoLcaBiu4Ver[5],
                                                       AiUInt32 aul_EndDecLcaBiu4Ver[5],
                                                       AiUInt32 aul_FirmwareBiu1Ver[5],
                                                       AiUInt32 aul_FirmwareBiu2Ver[5],
                                                       AiUInt32 aul_FirmwareBiu3Ver[5],
                                                       AiUInt32 aul_FirmwareBiu4Ver[5],
                                                       AiUInt32 aul_TargetSWVer[5],
                                                       AiUInt32 aul_SysDrvVer[5],
                                                       AiUInt32 aul_DllVer[5],
                                                       AiUInt32 aul_MonitorVer[5],
                                                       AiUInt32 *pul_BoardSerialNo,
                                                       AiUInt8  *puc_BspCompatibility);
AI_LIB_FUNC AiReturn AI_CALL_CONV ApiReadVersionCvi(AiUInt32 board_handle, TY_E_VERSION_ID eId, AiUInt32* version_type, AiChar description[AI_DESCRIPTION_STRINGL], AiUInt32* major,                              /*!< Major version. Must be increased when component interface gets incompatible (e.g. API call signature change) */
                                                      AiUInt32* minor, AiUInt32* patch, AiUInt32* build, AiChar full_version[AI_VERSION_STRINGL]);
AI_LIB_FUNC AiReturn AI_CALL_CONV ApiCmdSystagDefCvi(AiUInt32 bModule, AiUInt8 biu,
    AiUInt8 id, AiUInt8 con, AiUInt8 mode,
    AiUInt16 xid_rtsa,
    AiUInt16 fct,
    AiUInt16 min,
    AiUInt16 max,
    AiUInt16 step,
    AiUInt16 wpos,
    AiUInt16 bpos);
AI_LIB_FUNC AiReturn AI_CALL_CONV ApiCmdSystagConCvi(AiUInt32 bModule, AiUInt8 biu, AiUInt8 id, AiUInt8 con);
AI_LIB_FUNC AiReturn AI_CALL_CONV ApiCmdTrackDefCvi(AiUInt32 bModule, AiUInt8 biu, AiUInt8 uc_TrackId, AiUInt8 uc_BT,
                                        AiUInt16 uw_XidRtSa, AiUInt8 uc_Mode, AiUInt16 uw_TrackStartPos,
                                        AiUInt8 uc_TrackBPos, AiUInt8 uc_TrackBLen, AiUInt16 uw_TrackLen,
                                        AiUInt16 uw_MultiplexedTrackNb, AiUInt16 uw_ContinousTrackNb,
                                        AiUInt32 *pul_TrackBufferStartAddr);
AI_LIB_FUNC AiReturn AI_CALL_CONV ApiCmdTrackDefExCvi(AiUInt32 bModule, AiUInt8 biu, AiUInt32 ul_TrackId,
                                        AiUInt32 ul_BT,
                                        AiUInt32 ul_XidRtSa, AiUInt32 ul_Mode, AiUInt32 ul_TrackStartPos,
                                        AiUInt32 ul_TrackBPos, AiUInt32 ul_TrackBLen, AiUInt32 ul_TrackLen,
                                        AiUInt32 ul_MultiplexedTrackNb, AiUInt32 ul_ContinousTrackNb,
                                        AiInt32 l_Offset, AiUInt32 *pul_TrackBufferStartAddr);
AI_LIB_FUNC AiReturn AI_CALL_CONV ApiCmdTrackReadCvi(AiUInt32 bModule, AiUInt8 biu, AiUInt8 uc_TrackId, AiUInt16 uw_MultiplexedTrackIndex,
                                         AiUInt8 uc_ReadMode, AiUInt8 *puc_DataValid, AiUInt16 *puw_TrackDataWords);
AI_LIB_FUNC AiReturn AI_CALL_CONV ApiCmdTrackReadExCvi(AiUInt32 bModule, AiUInt8 biu, AiUInt32 ul_TrackId,
                                         AiUInt32 ul_MultiplexedTrackIndex,
                                         AiUInt32 ul_ReadMode, AiUInt32 *pul_DataValid, AiUInt32 *pul_LastTT,
                                         AiUInt16 *puw_TrackDataWords);
AI_LIB_FUNC AiReturn AI_CALL_CONV ApiCmdTrackScanCvi(AiUInt32 bModule, AiUInt8 biu, AiUInt32 ul_TrackId,
                                         AiUInt32 ul_Chunk, AiUInt32 ul_ChunkSize, AiUInt32 *pul_NumberOfReturnedStates,
                                         AiUInt32 *pul_MoreData,
                                         AiUInt32 *pul_ReturnedStates);
AI_LIB_FUNC AiReturn AI_CALL_CONV ApiCmdReadDiscretesCvi(AiUInt32 ul_Module, AiUInt32 *pul_Value);
AI_LIB_FUNC AiReturn AI_CALL_CONV ApiCmdReadDiscretesConfigCvi(AiUInt32 ul_Module, AiUInt32 *pul_Value);
AI_LIB_FUNC AiReturn AI_CALL_CONV ApiCmdWriteDiscretesCvi(AiUInt32 ul_Module, AiUInt32 ul_Mask, AiUInt32 ul_Value);
AI_LIB_FUNC AiReturn AI_CALL_CONV ApiCmdInitDiscretesCvi(AiUInt32 ul_Module, AiUInt32 ul_DiscreteSetup);
AI_LIB_FUNC AiReturn AI_CALL_CONV ApiCmdSyncCounterSetCvi(AiUInt32 ul_Module, AiUInt8 uc_Biu, AiUInt32 ul_SyncCntVal);
AI_LIB_FUNC AiReturn AI_CALL_CONV ApiCmdSyncCounterGetCvi(AiUInt32 ul_Module, AiUInt8 uc_Biu, 
                     AiUInt32 *pul_SyncCntVal, AiUInt32 *pul_SyncCntInit, AiUInt32 *pul_SyncCntInitLow,
                     AiUInt32 *pul_SyncCntInitHigh);
AI_LIB_FUNC AiReturn AI_CALL_CONV ApiCmdSysGetMemLocationcvi(AiUInt32 ul_Module, AiUInt8 uc_Biu, AiUInt32 ul_Mode, AiUInt32 ul_Id, AiUInt32 *pul_MemType, AiUInt32 *pul_Offset);

AI_LIB_FUNC AiReturn AI_CALL_CONV ApiCmdSysMallocCvi(AiUInt32 ulModule, AiUInt8 ucMemtype, AiUInt32 ulSize,   AiUInt32 ulTag, AiUInt32 * pulOffset );
AI_LIB_FUNC AiReturn AI_CALL_CONV ApiCmdSysFreeCvi(AiUInt32 ulModule, AiUInt8 ucMemtype, AiUInt32 ulOffset, AiUInt32 ulTag );

AI_LIB_FUNC AiReturn AI_CALL_CONV ApiCmdSysTriggerEdgeInputSetCvi(AiUInt32 ul_Module, AiUInt8 biu, AiUInt32 edge_setup);
AI_LIB_FUNC AiReturn AI_CALL_CONV ApiCmdSysTriggerEdgeInputGetCvi(AiUInt32 ul_Module, AiUInt8 biu, AiUInt32 * edge_setup);

//###############################################################
//    Buffer Commands
//###############################################################

AI_LIB_FUNC AiReturn AI_CALL_CONV ApiCmdBufDefCvi(AiUInt32 bModule, AiUInt8 biu, AiUInt8 bt, AiUInt16 hid,
    AiUInt16 bid, AiUInt8 len, AiUInt16* data, AiUInt16* rid, AiUInt32* raddr);
AI_LIB_FUNC AiReturn AI_CALL_CONV ApiCmdBufReadCvi(AiUInt32 bModule, AiUInt8 biu, AiUInt8 bt, AiUInt16 hid,
                                   AiUInt16 bid, AiUInt8 len, AiUInt16 *data, AiUInt16 *rid, AiUInt32 *raddr);
AI_LIB_FUNC AiReturn AI_CALL_CONV ApiCmdBufWriteCvi(AiUInt32 bModule, AiUInt8 biu, AiUInt8 bt, AiUInt16 hid, AiUInt16 bid,
                                            AiUInt8 d_pos, AiUInt8 b_pos, AiUInt8 b_len, AiUInt16 data);
AI_LIB_FUNC AiReturn AI_CALL_CONV ApiCmdBufWriteBlockCvi(AiUInt32 bModule, AiUInt8 biu, AiUInt8 bt, AiUInt16 hid,
    AiUInt16 bid, AiUInt8 offset, AiUInt8 len, AiUInt16* data);
AI_LIB_FUNC AiReturn AI_CALL_CONV ApiCmdBufHostControlCvi(AiUInt32 bModule, AiUInt8 biu, AiUInt8 bt, AiUInt16 hid, AiUInt32 con);

/* 13.10.99 */
AI_LIB_FUNC AiReturn AI_CALL_CONV ApiCmdBufC1760ConCvi(AiUInt32 bModule, AiUInt8 biu,
    AiUInt8  mode,
    AiUInt16 buf_id,
    AiUInt8  c01,
    AiUInt8  c02[MAX_API_C1760_C02]);
AI_LIB_FUNC AiReturn AI_CALL_CONV ApiReadMemDataCvi(AiUInt32 bModule, AiUInt8 memtype, AiUInt32 offset,
                                                  AiUInt8 width, void* data_p);
AI_LIB_FUNC AiReturn AI_CALL_CONV ApiWriteMemDataCvi(AiUInt32 bModule, AiUInt8 memtype, AiUInt32 offset,
                                                   AiUInt8 width, void* data_p);
AI_LIB_FUNC AiReturn AI_CALL_CONV ApiReadBlockMemDataCvi(AiUInt32 bModule, AiUInt8 memtype, AiUInt32 offset, AiUInt8 width,
                                                       void* data_p, AiUInt32 size, AiUInt32 *pul_BytesRead);
AI_LIB_FUNC AiReturn AI_CALL_CONV ApiWriteBlockMemDataCvi(AiUInt32 bModule, AiUInt8 memtype, AiUInt32 offset,
                                                        AiUInt8 width, void* data_p, AiUInt32 size, AiUInt32 *pul_BytesWritten);
AI_LIB_FUNC AiReturn AI_CALL_CONV ApiCmdFifoWriteCvi(AiUInt32 bModule, AiUInt8 biu, AiUInt8 f_id,
                                                   AiUInt16 size, AiUInt16 *data);
AI_LIB_FUNC AiReturn AI_CALL_CONV ApiCmdFifoReadStatusCvi(AiUInt32 bModule, AiUInt8 biu,
                                                        AiUInt8 f_id, AiUInt16 *status);
AI_LIB_FUNC AiReturn AI_CALL_CONV ApiBHModifyCvi(AiUInt32 bModule, AiUInt32 ul_BHAddr, AiUInt32 ul_DataBufferStoreMode, AiUInt32 ul_BufferSizeMode,
                                                      AiUInt32 ul_BufferSize, AiUInt32 ul_BufferQueueMode, AiUInt32 ul_BufferQueueSize,
                                                      AiUInt32 ul_StatusQueueSize, AiUInt32 ul_EventQueueSize, AiUInt32 ul_CurrentBufferIndex);


//###############################################################
//    BC Commands
//###############################################################

AI_LIB_FUNC AiReturn AI_CALL_CONV ApiCmdBCIniCvi(AiUInt32 bModule, AiUInt8 biu, AiUInt8 retr, AiUInt8 svrq, AiUInt8 tbm, AiUInt8 gsb);
AI_LIB_FUNC AiReturn AI_CALL_CONV ApiCmdBCBHDefCvi(AiUInt32 bModule, AiUInt8 biu, AiUInt16 hid,
       AiUInt16 bid, AiUInt16 sid, AiUInt16 eid, AiUInt8 qsize, AiUInt8 bqm, AiUInt8 bsm,
       AiUInt8 sqm, AiUInt8 eqm, AiUInt8 res,              // changed 08.01.01 pg/mr
       AiUInt16 *bh_bid,
       AiUInt16 *bh_sid,
       AiUInt16 *bh_eid,
       AiUInt16 *bh_nbufs,
       AiUInt32 *bh_hid_addr,
       AiUInt32 *bh_bq_addr,
       AiUInt32 *bh_sq_addr,
       AiUInt32 *bh_eq_addr);
AI_LIB_FUNC AiReturn AI_CALL_CONV ApiCmdBCXferDefCvi(AiUInt32 bModule, AiUInt8 biu,
    AiUInt16 xid,
    AiUInt16 hid,
    AiUInt8 type,
    AiUInt8 chn,
    AiUInt8 xmt_rt,
    AiUInt8 rcv_rt,
    AiUInt8 xmt_sa,
    AiUInt8 rcv_sa,
    AiUInt8 wcnt,
    AiUInt8 tic,
    AiUInt8 hlt,
    AiUInt8 rte,
    AiUInt8 res,
    AiUInt8 sxh,    // changed 08.01.01 mr, was rsp1
    AiUInt8 rsp,    // changed 08.01.01 mr, was rsp2
    AiUInt8 gap_mode,
    AiUInt16 swxm,
    AiUInt8  err_type,
    AiUInt8  err_sync,
    AiUInt8  err_contig,
    AiUInt8  err_padding1,
    AiUInt32 err_spec,
    AiUInt16 gap,
    AiUInt32 *desc_addr);
AI_LIB_FUNC AiReturn AI_CALL_CONV ApiCmdBCXferDefErrCvi(AiUInt32 bModule, AiUInt8 biu,
    AiUInt16 xid, 
    AiUInt8  err_type,
    AiUInt8  err_sync,
    AiUInt8  err_contig,
    AiUInt8  err_padding1,
    AiUInt32 err_spec);
AI_LIB_FUNC AiReturn AI_CALL_CONV ApiCmdBCGetXferDefCvi(AiUInt32 bModule, AiUInt8 biu, AiUInt16 uw_XferId,
    AiUInt16 *xid, AiUInt16 *hid, AiUInt8 *type, AiUInt8 *chn, AiUInt8 *xmt_rt,
    AiUInt8 *rcv_rt, AiUInt8 *xmt_sa, AiUInt8 *rcv_sa, AiUInt8 *wcnt,
    AiUInt8 *tic, AiUInt8 *hlt, AiUInt8 *rte, AiUInt8 *res,
    AiUInt8 *sxh, AiUInt8 *rsp, AiUInt8 *gap_mode, AiUInt16 *swxm,
    AiUInt8 *err_type, AiUInt8 *err_sync, AiUInt8 *err_contig,
    AiUInt8 *err_padding1, AiUInt32 *err_spec, AiUInt16 *gap);

AI_LIB_FUNC AiReturn AI_CALL_CONV ApiCmdBCXferCtrlCvi(AiUInt32 bModule, AiUInt8 biu, AiUInt16 xid, AiUInt8 mode);
AI_LIB_FUNC AiReturn AI_CALL_CONV ApiCmdBCDytagDefCvi(AiUInt32 bModule, AiUInt8 biu, AiUInt8 con,
    AiUInt16 bc_hid,
    AiUInt16 mode,
    AiUInt16 tag_fct[4],
    AiUInt16 min[4],
    AiUInt16 max[4],
    AiUInt16 step[4],
    AiUInt16 wpos[4]);
AI_LIB_FUNC AiReturn AI_CALL_CONV ApiCmdBCXferDescGetCvi(AiUInt32 bModule, AiUInt8 biu, AiUInt16 xid, AiUInt32 ulMode, AiUInt32 * pulValue1, AiUInt32 * pulValue2, AiUInt32 * pulValue3, AiUInt32 * pulValue4 );
AI_LIB_FUNC AiReturn AI_CALL_CONV ApiCmdBCXferDescModCvi(AiUInt32 bModule, AiUInt8 biu, AiUInt16 xid, AiUInt32 ulMode, AiUInt32   ulValue1,  AiUInt32   ulValue2,  AiUInt32   ulValue3,  AiUInt32   ulValue4  );
/*AI_LIB_FUNC AiReturn AI_CALL_CONV ApiCmdBCXferDytagCvi(AiUInt32 bModule, AiUInt8 biu, AiUInt8 con,
    AiUInt16 xid,
    AiUInt8  mode,
    AiUInt8  mode1,
    AiUInt16 min,
    AiUInt16 max,
    AiUInt16 step,
    AiUInt16 wpos,
    AiUInt16 min1,
    AiUInt16 max1,
    AiUInt16 step1,
    AiUInt16 wpos1);
*/
AI_LIB_FUNC AiReturn AI_CALL_CONV ApiCmdBCFrameDefCvi(AiUInt32 bModule, AiUInt8 biu,
    AiUInt8 id,
    AiUInt8 cnt,
    AiUInt16 instr[MAX_API_BC_XFRAME],
    AiUInt16 xid[MAX_API_BC_XFRAME]);
AI_LIB_FUNC AiReturn AI_CALL_CONV ApiCmdBCGetMinorFrameDefinitionCvi(AiUInt32 bModule, AiUInt8 biu,
    AiUInt8 id,
    AiUInt8 *cnt,
    AiUInt16 instr[MAX_API_BC_XFRAME],
    AiUInt16 xid[MAX_API_BC_XFRAME]);
AI_LIB_FUNC AiReturn AI_CALL_CONV ApiCmdBCMFrameDefCvi(AiUInt32 bModule, AiUInt8 biu,
    AiUInt8 cnt,
    AiUInt8 fid[MAX_API_BC_MFRAME]);
AI_LIB_FUNC AiReturn AI_CALL_CONV ApiCmdBCMFrameDefExCvi(AiUInt32 bModule, AiUInt8 biu,
    AiUInt8 cnt,
    AiUInt8 fid[MAX_API_BC_MFRAME_EX]);
AI_LIB_FUNC AiReturn AI_CALL_CONV ApiCmdBCGetMajorFrameDefinitionCvi(AiUInt32 bModule, AiUInt8 biu,
    AiUInt8 *cnt,
    AiUInt8 fid[MAX_API_BC_MFRAME_EX]);
AI_LIB_FUNC AiReturn AI_CALL_CONV ApiCmdBCStartCvi        (AiUInt32 bModule, AiUInt8 biu, AiUInt8 smod, AiUInt32 cnt, AiFloat frame_time,
                                                           AiUInt32 saddr, AiUInt32 *maj_frame_addr, AiUInt32 *min_frame_addr);
AI_LIB_FUNC AiReturn AI_CALL_CONV ApiCmdBCHaltCvi(AiUInt32 bModule, AiUInt8 biu);
AI_LIB_FUNC AiReturn AI_CALL_CONV ApiCmdBCStatusReadCvi(AiUInt32 bModule, AiUInt8 biu,
    AiUInt8  *status,
    AiUInt16 *hxfer,
    AiUInt32 *glb_msg_cnt,
    AiUInt32 *glb_err_cnt,
    AiUInt32* hip,
    AiUInt32* mfc);
AI_LIB_FUNC AiReturn AI_CALL_CONV ApiCmdBCXferReadCvi(AiUInt32 bModule, AiUInt8 biu, AiUInt16 xid, AiUInt16 clr,
    AiUInt16 *cw1,
    AiUInt16 *st1,
    AiUInt16 *cw2,
    AiUInt16 *st2,
    AiUInt16 *bid,
    AiUInt16 *brw,
    AiUInt32 *bufp,
    AiUInt32 *ttag,
    AiUInt32 *msg_cnt,
    AiUInt32 *err_cnt,
  AiUInt32 *lvw,
  AiUInt32 *srvreq_cnt);
AI_LIB_FUNC AiReturn AI_CALL_CONV ApiCmdBCXferReadExCvi(AiUInt32 bModule, 
   AiUInt32 ul_XferId, 
   AiUInt32 ul_Clear,
    AiUInt32 ul_Biu,
    AiUInt32 aul_SqCtrlWord[256],
    AiUInt32 aul_SqStatusWords[256],
    AiUInt32 aul_SqActBufPtr[256],
    AiUInt32 aul_SqTimeTag[256],
    AiUInt32 aul_EqEntryWord1[1],
    AiUInt32 aul_EqEntryWord2[1],
    AiUInt32 aul_EqEntryWord3[1],
    AiUInt32 aul_EqEntryWord4[1],
    AiUInt32 *pul_ActBufferId,
    AiUInt32 *pul_ActionWord,
    AiUInt32 *pul_XferCnt,
    AiUInt32 *pul_ErrCnt,
    AiUInt32 *pul_LastVectorWord,
    AiUInt32 *pul_ServiceRequestCnt,
    AiUInt32 *pul_BufferQueueSize);

AI_LIB_FUNC AiReturn AI_CALL_CONV ApiCmdBCAcycPrepCvi(AiUInt32 bModule, AiUInt8 biu,
    AiUInt8 cnt,
    AiUInt16 instr[MAX_API_BC_XACYC],
    AiUInt16 xid[MAX_API_BC_XACYC]);
AI_LIB_FUNC AiReturn AI_CALL_CONV ApiCmdBCAcycSendCvi(AiUInt32 bModule, AiUInt8 biu, AiUInt8 mode, AiUInt32 timetag_high, AiUInt32 timetag_low);
AI_LIB_FUNC AiReturn AI_CALL_CONV ApiCmdBCAssignFifoCvi(AiUInt32 bModule, AiUInt8 biu, AiUInt8 con,
                                                      AiUInt8 f_id, AiUInt16 xid);
AI_LIB_FUNC AiReturn AI_CALL_CONV ApiCmdBCSrvReqVecStatusCvi(AiUInt32 bModule, AiUInt8 uc_Biu, AiUInt8 uc_RtAddr,
                                                             AiUInt16 *uw_XferId, AiUInt16 *uw_LastVecWord,
                                                             AiUInt32 *ul_SrvReqCnt);
AI_LIB_FUNC AiReturn AI_CALL_CONV ApiCmdBCSrvReqVecConCvi(AiUInt32 bModule, AiUInt8 uc_Biu, AiUInt8 uc_RtAddr,
                                                             AiUInt8 uc_SubAddress, AiUInt32 ul_Reserved1,
                                                             AiUInt32 ul_Reserved2, AiUInt32 ul_Reserved3);
AI_LIB_FUNC AiReturn AI_CALL_CONV ApiCmdBCGetXferBufferHeaderInfoCvi (AiUInt32 Module, AiUInt8 Biu, AiUInt32 XferId, AiUInt32 *BufHeaderIndex, AiUInt32 *BufHeaderAddr);
AI_LIB_FUNC AiReturn AI_CALL_CONV ApiCmdBCModeCtrlCvi(AiUInt32 Module, AiUInt8 uc_Biu, AiUInt32 ul_BcMode,
                                                          AiUInt32 ul_Ctrl, AiUInt32 ul_Param1,
                                                          AiUInt32 ul_Param2, AiUInt32 ul_Param3);

//###############################################################
//    RT Commands
//###############################################################

AI_LIB_FUNC AiReturn AI_CALL_CONV ApiCmdRTIniCvi(AiUInt32 bModule, AiUInt8 biu, AiUInt8 rt_addr,
                                   AiUInt8 con, AiUInt8 bus, AiFloat resp_time, AiUInt16 nxw);
AI_LIB_FUNC AiReturn AI_CALL_CONV ApiCmdRTGetSimulationInfoCvi(AiUInt32 bModule, AiUInt8 biu, AiUInt8 rt_addr,
                                   AiUInt8 *puc_Mode, AiUInt32 *pul_RxSA, AiUInt32 *pul_TxSA, AiUInt32 *pul_RxMC,
                                   AiUInt32 *pul_TxMC, AiUInt32 aul_HSRxMID[8], AiUInt32 aul_HSTxMID[8], AiUInt32 ul_HSMC);
AI_LIB_FUNC AiReturn AI_CALL_CONV ApiCmdRTEnaDisCvi(AiUInt32 bModule, AiUInt8 biu, AiUInt8 rt_addr, AiUInt8 con);
AI_LIB_FUNC AiReturn AI_CALL_CONV ApiCmdRTRespTimeCvi(AiUInt32 bModule, AiUInt8 biu, AiUInt8 rt_addr, AiFloat resp_time);
AI_LIB_FUNC AiReturn AI_CALL_CONV ApiCmdRTRespTimeCviGet(AiUInt32 bModule, AiUInt8 biu, AiUInt8 rt_addr, AiFloat *pResp_time);
AI_LIB_FUNC AiReturn AI_CALL_CONV ApiCmdRTNXWCvi(AiUInt32 bModule, AiUInt8 biu, AiUInt8 rt_addr, AiUInt16 nxw);
AI_LIB_FUNC AiReturn AI_CALL_CONV ApiCmdRTLCWCvi(AiUInt32 bModule, AiUInt8 biu, AiUInt8 rt_addr, AiUInt16 lcw);
AI_LIB_FUNC AiReturn AI_CALL_CONV ApiCmdRTLSWCvi(AiUInt32 bModule, AiUInt8 biu, AiUInt8 rt_addr, AiUInt16 lsw);
AI_LIB_FUNC AiReturn AI_CALL_CONV ApiCmdRTBHDefCvi(AiUInt32 bModule, AiUInt8 biu, AiUInt16 hid,
    AiUInt16 bid, AiUInt16 sid, AiUInt16 eid, AiUInt8 qsize, AiUInt8 bqm, AiUInt8 bsm,
    AiUInt8 sqm, AiUInt8 eqm, AiUInt8 res,           // changed 08.01.01 pg/mr
    AiUInt16 *bh_bid,
    AiUInt16 *bh_sid,
    AiUInt16 *bh_eid,
    AiUInt16 *bh_nbufs,
    AiUInt32 *bh_hid_addr,
    AiUInt32 *bh_bq_addr,
    AiUInt32 *bh_sq_addr,
    AiUInt32 *bh_eq_addr);
AI_LIB_FUNC AiReturn AI_CALL_CONV ApiCmdRTSAConCvi(AiUInt32 bModule, AiUInt8 biu, AiUInt8 rt_addr,
                                   AiUInt8 sa, AiUInt16 hid, AiUInt8 sa_type, AiUInt8 con,
                                   AiUInt8 rmod, AiUInt8 smod, AiUInt16 swm);
AI_LIB_FUNC AiReturn AI_CALL_CONV ApiCmdRTSAConErrCvi(AiUInt32 bModule, AiUInt8 biu, AiUInt8 rt_addr,
    AiUInt8 sa, AiUInt8 sa_type,
    AiUInt8 type,
    AiUInt8 sync,
    AiUInt8 config,
    AiUInt8 padding1,
    AiUInt32 err_spec);
AI_LIB_FUNC AiReturn AI_CALL_CONV ApiCmdRTGetSAConErrCvi(AiUInt32 bModule, AiUInt8 biu, AiUInt8 rt_addr,
    AiUInt8 sa, AiUInt8 sa_type,
    AiUInt8 *type,
    AiUInt8 *sync,
    AiUInt8 *config,
    AiUInt8 *padding1,
    AiUInt32 *err_spec);
AI_LIB_FUNC AiReturn AI_CALL_CONV ApiCmdRTDytagDefCvi(AiUInt32 bModule, AiUInt8 biu, AiUInt8 con,
    AiUInt16 rt_hid, AiUInt16 mode,
    AiUInt16 tag_fct[4],
    AiUInt16 min[4],
    AiUInt16 max[4],
    AiUInt16 step[4],
    AiUInt16 wpos[4]);
AI_LIB_FUNC AiReturn AI_CALL_CONV ApiCmdRTGetDytagDefCvi(AiUInt32 bModule, AiUInt8 biu, AiUInt16 rt_hid,
    AiUInt16 *mode,
    AiUInt16 tag_fct[4],
    AiUInt16 min[4],
    AiUInt16 max[4],
    AiUInt16 step[4],
    AiUInt16 wpos[4]);
AI_LIB_FUNC AiReturn AI_CALL_CONV ApiCmdRTStartCvi(AiUInt32 bModule, AiUInt8 biu);
AI_LIB_FUNC AiReturn AI_CALL_CONV ApiCmdRTHaltCvi(AiUInt32 bModule, AiUInt8 biu);
AI_LIB_FUNC AiReturn AI_CALL_CONV ApiCmdRTStatusReadCvi(AiUInt32 bModule, AiUInt8 biu,
    AiUInt8  *status,
    AiUInt32 *glb_msg_cnt,
    AiUInt32 *glb_err_cnt);
AI_LIB_FUNC AiReturn AI_CALL_CONV ApiCmdRTMsgReadCvi(AiUInt32 bModule, AiUInt8 biu, AiUInt8 rt_addr,
    AiUInt16 *nxw,
    AiUInt16 *lsw,
    AiUInt16 *lcw,
    AiUInt32 *msg_cnt,
    AiUInt32 *err_cnt);
AI_LIB_FUNC AiReturn AI_CALL_CONV ApiCmdRTMsgReadAllCvi(AiUInt32 bModule, AiUInt8 biu,
    AiUInt32 rt_msg[32],
    AiUInt32 rt_err[32]);
AI_LIB_FUNC AiReturn AI_CALL_CONV ApiCmdRTSAMsgReadCvi(AiUInt32 bModule, AiUInt8 biu, AiUInt8 rt_addr,
                                   AiUInt8 sa, AiUInt8 sa_type, AiUInt8 clr,
    AiUInt16 *bid,
    AiUInt16 *trw,
    AiUInt16 *lcw,
    AiUInt16 *lsw,
    AiUInt32 *bufp,
    AiUInt32 *ttag);
AI_LIB_FUNC AiReturn AI_CALL_CONV ApiCmdRTSAMsgReadExCvi(AiUInt32 bModule, 
   AiUInt32 ul_RtAddr, 
   AiUInt32 ul_SA,
    AiUInt32 ul_SaType,
    AiUInt32 ul_Biu,
    AiUInt32 aul_SqCtrlWord[256],
    AiUInt32 aul_SqStatusWords[256],
    AiUInt32 aul_SqActBufPtr[256],
    AiUInt32 aul_SqTimeTag[256],
    AiUInt32 aul_EqEntryWord1[1],
    AiUInt32 aul_EqEntryWord2[1],
    AiUInt32 aul_EqEntryWord3[1],
    AiUInt32 aul_EqEntryWord4[1],
    AiUInt32 *pul_ActBufferId,
    AiUInt32 *pul_ActionWord,
    AiUInt32 *pul_XferCnt,
    AiUInt32 *pul_ErrCnt,
    AiUInt32 *pul_BufferQueueSize);
AI_LIB_FUNC AiReturn AI_CALL_CONV ApiCmdRTGlobalConCvi(AiUInt32 bModule, AiUInt8 biu, AiUInt16 count,
    AiUInt16 buffer[512][32],
    AiUInt8  mode[512],
    AiUInt8  rt[512],
    AiUInt8  rt_con[512],
    AiUInt8  sa_mc[512],
    AiUInt8  sa_type[512],
    AiUInt8  sa_con[512],
    AiUInt8  resp_time[512],
    AiUInt8  smod[512],
    AiUInt16 nxw[512],
    AiUInt16 swm[512],
    AiUInt16 hid[512],
    AiUInt16 bid[512]);
AI_LIB_FUNC AiReturn AI_CALL_CONV ApiCmdRTSAAssignFifoCvi(AiUInt32 bModule, AiUInt8 biu, AiUInt8 con,
                                                        AiUInt8 f_id, AiUInt8 rt, AiUInt8 sa);
AI_LIB_FUNC AiReturn AI_CALL_CONV ApiCmdRTModeCtrlCvi(AiUInt32 Module, AiUInt8 uc_Biu, AiUInt32 ul_RtMode,
                                                          AiUInt32 ul_Ctrl, AiUInt32 ul_Param1,
                                                          AiUInt32 ul_Param2, AiUInt32 ul_Param3);


//###############################################################
//    BM Commands
//###############################################################

AI_LIB_FUNC AiReturn AI_CALL_CONV ApiCmdBMIniCvi(AiUInt32 bModule, AiUInt8 biu);
AI_LIB_FUNC AiReturn AI_CALL_CONV ApiCmdBMIntrModeCvi(AiUInt32 bModule, AiUInt8 biu, AiUInt8 imod, AiUInt8 smod, AiUInt8 res);
AI_LIB_FUNC AiReturn AI_CALL_CONV ApiCmdBMCapModeCvi(AiUInt32 bModule, AiUInt8 biu,
    AiUInt8  cap_mode,
    AiUInt32 cap_tat,
    AiUInt16 cap_mcc,
    AiUInt16 cap_fsize);
AI_LIB_FUNC AiReturn AI_CALL_CONV ApiCmdBMSWXMIniCvi(AiUInt32 bModule, AiUInt8 biu, AiUInt16 swxm);
AI_LIB_FUNC AiReturn AI_CALL_CONV ApiCmdBMStartCvi(AiUInt32 bModule, AiUInt8 biu);
AI_LIB_FUNC AiReturn AI_CALL_CONV ApiCmdBMHaltCvi(AiUInt32 bModule, AiUInt8 biu);
AI_LIB_FUNC AiReturn AI_CALL_CONV ApiCmdBMStatusReadCvi(AiUInt32 bModule, AiUInt8 biu,
    AiUInt8*  men,
    AiUInt8*  msw,
    AiUInt8*  msp,
    AiUInt32* glb_msg_cnt,
    AiUInt32* glb_err_cnt,
    AiUInt32* glb_word_cnt_pri,
    AiUInt32* glb_word_cnt_sec,
    AiUInt32* glb_msg_cnt_pri,
    AiUInt32* glb_msg_cnt_sec,
    AiUInt32* glb_err_cnt_pri,
    AiUInt32* glb_err_cnt_sec,
    AiUInt32* bus_load_pri,
    AiUInt32* bus_load_sec,
    AiUInt32* bus_load_pri_avg,
    AiUInt32* bus_load_sec_avg );

AI_LIB_FUNC AiReturn AI_CALL_CONV ApiCmdBMActReadCvi(AiUInt32 bModule, AiUInt8 biu, AiUInt16 entry_no,
    AiUInt8  *fnd,
    AiUInt8  *rt,
    AiUInt8  *tr,
    AiUInt8  *sa,
    AiUInt32 *cc,
    AiUInt32 *ec,
    AiUInt32 *et);
AI_LIB_FUNC AiReturn AI_CALL_CONV  ApiCmdBMStackpReadCvi(AiUInt32 bModule, AiUInt8 biu,
    AiUInt8  *status,
    AiUInt32 *stp,
    AiUInt32 *ctp,
    AiUInt32 *etp);
AI_LIB_FUNC AiReturn AI_CALL_CONV ApiCmdBMStackEntryReadCvi(AiUInt32 bModule, AiUInt8 biu, AiUInt8 ptype,
                                   AiUInt8 sgn, AiUInt32 offset,
    AiUInt32 *entry);
AI_LIB_FUNC AiReturn AI_CALL_CONV ApiCmdBMStackEntryFindCvi(AiUInt32 bModule, AiUInt8 biu, AiUInt8 ptype,
                                   AiUInt8 sgn, AiUInt32 offset,
                                   AiUInt16 fspec, AiUInt16 fdata, AiUInt16 fmask,
       AiUInt8  *efnd,
       AiUInt32 *eptr,
       AiUInt32 *entry);
/* MR REMOVED 02.11.2000
AI_LIB_FUNC AiReturn AI_CALL_CONV ApiCmdBMRecStatusReadCvi(AiUInt32 bModule, AiUInt8 biu,
    AiUInt8  *status,
    AiUInt32 *hfi_cnt,
    AiUInt32 *saddr,
    AiUInt32 *size);
*/
/* MR REMOVED 02.11.2000
AI_LIB_FUNC AiReturn AI_CALL_CONV ApiCmdBMCopyRecDatCvi(AiUInt32 bModule, AiUInt8 biu,
    AiUInt8  status,
    AiUInt32 hfi_cnt,
    AiUInt32 saddr,
    AiUInt32 size);
*/
AI_LIB_FUNC AiReturn AI_CALL_CONV ApiCmdBMFilterIniCvi(AiUInt32 bModule, AiUInt8 biu, AiUInt8 rt_addr,
                                   AiUInt32 rx_sa, AiUInt32 tx_sa, AiUInt32 rx_mc, AiUInt32 tx_mc);
AI_LIB_FUNC AiReturn AI_CALL_CONV ApiCmdBMIllegalIniCvi(AiUInt32 bModule, AiUInt8 biu, AiUInt8 rt_addr,
                                   AiUInt32 rx_sa, AiUInt32 tx_sa, AiUInt32 rx_mc, AiUInt32 tx_mc);
AI_LIB_FUNC AiReturn AI_CALL_CONV ApiCmdBMFTWIniCvi(AiUInt32 bModule, AiUInt8 biu, AiUInt8 htm, AiUInt8 con, AiUInt8 htc, AiUInt8 stm, AiUInt8 stc);
AI_LIB_FUNC AiReturn AI_CALL_CONV ApiCmdBMTIWIniCvi(AiUInt32 bModule, AiUInt8 biu, AiUInt8 eti, AiUInt8 con, AiUInt8 aei, AiUInt8 ti0, AiUInt8 ti1);
AI_LIB_FUNC AiReturn AI_CALL_CONV ApiCmdBMTCIIniCvi(AiUInt32 bModule, AiUInt8 biu, AiUInt8 rt, AiUInt8 sa,
                                   AiUInt8 sa_type, AiUInt8 tagm, AiUInt8 tfct, AiUInt8 tid);
AI_LIB_FUNC AiReturn AI_CALL_CONV ApiCmdBMTCBIniCvi(AiUInt32 bModule, AiUInt8 biu, AiUInt8 tid, AiUInt8 ten,
    AiUInt8  tt,
    AiUInt8  sot,
    AiUInt8  tri,
    AiUInt8  inv,
    AiUInt8  tres,
    AiUInt8  tset,
    AiUInt8  tsp,
    AiUInt8  next,
    AiUInt8  eom,
    AiUInt16 tdw,
    AiUInt16 tmw,
    AiUInt16 tuli,
    AiUInt16 tlli);
AI_LIB_FUNC AiReturn AI_CALL_CONV ApiCmdBMRTActReadCvi(AiUInt32 bModule, AiUInt8 biu, AiUInt8 rt,
    AiUInt32 *mc,
    AiUInt32 *ec,
    AiUInt32 *et);
AI_LIB_FUNC AiReturn AI_CALL_CONV ApiCmdBMRTSAActReadCvi(AiUInt32 bModule, AiUInt8 biu, AiUInt8 rt, AiUInt8 sa, AiUInt8 sa_type,
    AiUInt32 *mc,
    AiUInt32 *ec,
    AiUInt32 *et);
AI_LIB_FUNC AiReturn AI_CALL_CONV ApiCmdBMIniMsgFltRecCvi(AiUInt32 bModule, AiUInt8 biu, AiUInt8 cnt,
    AiUInt16 cw[40],
    AiUInt8 pos1[40],
    AiUInt8 pos2[40]);
AI_LIB_FUNC AiReturn AI_CALL_CONV ApiCmdBMReadMsgFltRecCvi(AiUInt32 bModule, AiUInt8 biu, AiUInt8 mode,
                                AiUInt8 con, AiUInt32 max_size, AiUInt16 max_msg,
                                void *lpBuf, AiUInt8 *ovfl, AiUInt32 *lBytesRead);
AI_LIB_FUNC AiReturn AI_CALL_CONV ApiCmdBMDytagMonDefCvi(AiUInt32 bModule, AiUInt8 biu, AiUInt8 uc_Id,
                                                         AiUInt8 uc_Con, AiUInt8 uc_RtAddr, AiUInt8 uc_SubAddrMsgId, AiUInt8 uc_SubAddrMsgIdType,
                                                         AiUInt16 uw_DytagType, AiUInt16 uw_DytagWPos, AiUInt16 uw_DytagBPos,
                                                         AiUInt16 uw_DytagBLen, AiUInt16 uw_DytagStep);
AI_LIB_FUNC AiReturn AI_CALL_CONV ApiCmdBMDytagMonReadCvi(AiUInt32 bModule, AiUInt8 biu, AiUInt8 uc_Id,
                                                          AiUInt32 *ul_Stale, AiUInt32 *ul_Bad, AiUInt32 *ul_Good);


//###############################################################
//    Queueing Commands
//###############################################################
AI_LIB_FUNC AiReturn AI_CALL_CONV ApiCmdQueueIniCvi(AiUInt32 bModule, AiUInt8 biu);
AI_LIB_FUNC AiReturn AI_CALL_CONV ApiCmdQueueReadCvi(AiUInt32 bModule, AiUInt8 biu,
       AiUInt8  *rt_addr,
       AiUInt8  *sa_mc,
       AiUInt8  *sa_type,
       AiUInt8  *rt_addr2,
       AiUInt8  *sa_mc2,
       AiUInt8  *sa_type2,
       AiUInt8  *word_cnt,
       AiUInt8  *rbf_trw,
       AiUInt16 *msg_trw,
       AiUInt16 *status_word1,
       AiUInt16 *status_word2,
       AiUInt16 buffer[32],
       AiUInt32 *ttag);
AI_LIB_FUNC AiReturn AI_CALL_CONV ApiCmdQueueFlushCvi(AiUInt32 bModule, AiUInt8 biu);
AI_LIB_FUNC AiReturn AI_CALL_CONV ApiCmdQueueStartCvi(AiUInt32 bModule, AiUInt8 biu);
AI_LIB_FUNC AiReturn AI_CALL_CONV ApiCmdQueueHaltCvi(AiUInt32 bModule, AiUInt8 biu);

//###############################################################
//    Data Queue Commands (Milscope)
//###############################################################
AI_LIB_FUNC AiReturn AI_CALL_CONV ApiCmdDataQueueCloseCvi(AiUInt32 bModule, AiUInt32 id);
AI_LIB_FUNC AiReturn AI_CALL_CONV ApiCmdDataQueueControlCvi(AiUInt32 bModule, AiUInt32 id, AiUInt32 mode);
AI_LIB_FUNC AiReturn AI_CALL_CONV ApiCmdDataQueueOpenCvi(AiUInt32 bModule, AiUInt32 id, AiUInt32 * queue_size );
AI_LIB_FUNC AiReturn AI_CALL_CONV ApiCmdDataQueueReadCvi(AiUInt32 bModule,
                                                            AiUInt32 id,
                                                            void * buffer,
                                                            AiUInt32 bytes_to_read,
                                                            AiUInt32 *status,
                                                            AiUInt32 *bytes_transfered,
                                                            AiUInt32 *bytes_in_queue,
                                                            AiUInt64 *total_bytes_transfered);
AI_LIB_FUNC AiReturn AI_CALL_CONV ApiCmdScopeSetupCvi(AiUInt32 bModule, AiUInt32 ul_CouplingPri, AiUInt32 ul_CouplingSec, 
                                                         AiUInt32 ul_SamplingRate, AiUInt32 ul_CaptureMode, AiUInt32 ul_OperationMode, 
                                                         AiUInt32 ul_SingleShotBuf);
AI_LIB_FUNC AiReturn AI_CALL_CONV ApiCmdScopeTriggerDefCvi(AiUInt32 bModule, AiUInt32 ul_TrgMode, AiUInt32 ul_TrgSrc, AiUInt32 ul_TrgValue, 
                                                         AiUInt32 ul_TrgNbSamples, AiUInt32 ul_TrgFlags, AiUInt32 ul_TrgDelay, AiUInt32 ul_TrgTbt);
AI_LIB_FUNC AiReturn AI_CALL_CONV ApiCmdScopeCalibrateCvi(AiUInt32 bModule, AiUInt8 ucMode, AiUInt32 *pul_OffsetA, AiUInt32 *pul_OffsetB,
                                                         AiUInt32 *pul_OffsetA100, AiUInt32 *pul_OffsetB100, AiUInt32 *pul_OffsetA100Sec, 
                                                         AiUInt32 *pul_OffsetB100Sec);
AI_LIB_FUNC AiReturn AI_CALL_CONV ApiCmdScopeStartCvi(AiUInt32 ul_Module);


//###############################################################
//    Calibration Commands
//###############################################################

AI_LIB_FUNC AiReturn AI_CALL_CONV ApiCmdCalSigConCvi(AiUInt32 bModule, AiUInt8 biu, AiUInt8 bus, AiUInt8 con);
AI_LIB_FUNC AiReturn AI_CALL_CONV ApiCmdCalCplConCvi(AiUInt32 bModule, AiUInt8 biu, AiUInt8 bus, AiUInt8 cpl);
AI_LIB_FUNC AiReturn AI_CALL_CONV ApiCmdCalXmtConCvi(AiUInt32 bModule, AiUInt8 biu, AiUInt8 bus, AiUInt8 amp);
AI_LIB_FUNC AiReturn AI_CALL_CONV ApiCmdCalTransConCvi(AiUInt32 bModule, AiUInt8 biu, AiUInt8 trans_mode);

//###############################################################
//    Replay Commands
//###############################################################

AI_LIB_FUNC AiReturn AI_CALL_CONV ApiCmdReplayIniCvi(AiUInt32 bModule, AiUInt8 biu, AiUInt8 cet,
                                  AiUInt8 nct, AiUInt8 cyc, AiUInt8 nimg, AiUInt8 alt, AiUInt8 rlt,
                                  AiUInt8 rint, AiUInt32 min, AiUInt32 msec, AiUInt8 sign,
                                  AiUInt32 fsize);
AI_LIB_FUNC AiReturn AI_CALL_CONV ApiCmdReplayStartCvi(AiUInt32 bModule, AiUInt8 biu);
AI_LIB_FUNC AiReturn AI_CALL_CONV ApiCmdReplayStopCvi(AiUInt32 bModule, AiUInt8 biu);
AI_LIB_FUNC AiReturn AI_CALL_CONV ApiCmdReplayStatusCvi(AiUInt32 bModule, AiUInt8 biu,
       AiUInt8  *status,
       AiUInt8  *padding1,
       AiUInt16 *padding2,
       AiUInt32 *rpi_cnt,
       AiUInt32 *saddr,
       AiUInt32 *size,
       AiUInt32 *entry_cnt);
AI_LIB_FUNC AiReturn AI_CALL_CONV ApiCmdReplayRTCvi(AiUInt32 bModule, AiUInt8 biu, AiUInt8 con, AiUInt8 mode, AiUInt8 rt);

//###############################################################
//    GenIo Functions
//###############################################################
AI_LIB_FUNC AiReturn AI_CALL_CONV ApiCmdGenIoAddrInitCvi(AiUInt32 ul_Module, AiUInt8 genio_type, 
                                  AiUInt8 io_type, AiUInt8 chn, AiUInt8 res, AiUInt32 addr);
AI_LIB_FUNC AiReturn AI_CALL_CONV ApiCmdGenIoOutChnWriteCvi(AiUInt32 ul_Module, AiUInt8 genio_type, 
                                  AiUInt8 chn, AiUInt32 res, AiUInt32 val);
AI_LIB_FUNC AiReturn AI_CALL_CONV ApiCmdGenIoOutChnReadCvi(AiUInt32 ul_Module, AiUInt8 genio_type, 
                                  AiUInt8 chn, AiUInt32 ctl, AiUInt32 *val);
AI_LIB_FUNC AiReturn AI_CALL_CONV ApiCmdGenIoInChnReadCvi(AiUInt32 ul_Module, AiUInt8 genio_type, 
                                  AiUInt8 chn, AiUInt32 ctl, AiUInt32 *val);
AI_LIB_FUNC AiReturn AI_CALL_CONV ApiCmdGenIoSysSTaskCtrlCvi(AiUInt32 ul_Module, AiUInt8 genio_type, 
                                  AiUInt8 con, AiUInt32 rate, AiUInt32 rsync, AiUInt32 ustep);

//###############################################################
//    Client / Server Support
//###############################################################
// added 25.04.2001 MR
AI_LIB_FUNC AiReturn AI_CALL_CONV ApiConnectToServerCvi( AiUChar * pszNetworkAdress, AiInt16 *w_ServerBoards);
AI_LIB_FUNC AiReturn AI_CALL_CONV ApiDisconnectFromServerCvi( AiUChar * pszNetworkAdress );

#endif
// end: cvi1553.h

