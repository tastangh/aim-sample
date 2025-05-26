/* SPDX-FileCopyrightText: 2001-2022 AIM GmbH <info@aim-online.com> */
/* SPDX-License-Identifier: MIT */

/*! \file Ai1553Cvi.h
 *
 *  This module contain wrapper functions, which are used for for CVI access.
 *
 */

#ifndef Ai1553Cvi_H
#define Ai1553Cvi_H

#if defined (_AIM_VXI)                                           //AXI-SECTION
    /******************************************************************************
     *  Includes                                                      //AXI-SECTION
    *******************************************************************************/

    #include <windows.h>
    #include <vpptype.h>
    #include "ai1553axi.h"
    #include "defs_v.h"
    #include "visa_io.h"
                                                        // end #ifdef _VXI_PP_WIN32
#elif defined (_AIM_WINDOWS)                                         //AVI-SECTION
    /******************************************************************************
     *  Includes                                                      //AVI-SECTION
    *******************************************************************************/

    #include <windows.h>
    #include <stdio.h>
    // Include API1553 Application Interface Library
    // Header File contains C-prototypes and structures
  #include "api1553.h"
  #include "api1553cvi.h"
#elif defined (_AIM_LINUX)                                         //AVI-SECTION
    /******************************************************************************
     *  Includes                                                      //AVI-SECTION
    *******************************************************************************/

    #include <stdio.h>
    // Include API1553 Application Interface Library
    // Header File contains C-prototypes and structures
  #include "Api1553.h"
  #include "Api1553Cvi.h"
#endif                                                  // end #ifdef _API_APPL_WIN32


#if defined (_AIM_VXI)    && !defined(_AIM_LVRT)                                     //AXI-SECTION
    /******************************************************************************
     *  Defines                                                          //AXI-SECTION
    *******************************************************************************/

    #define VI_SESSION     AiSession
    #define VI_STATUS      AiStatus      // was AiInt32; 02.02.01 mr

    #define M_BOARDID      InstrumentHandle

    #define M_GETMODULE(InstrumentHandle, bModule)    GetModuleNo( InstrumentHandle, bModule )

  #define M_RETVAL_ERR   (Ai1553Axi_ERROR_OFFSET + retVal)
    #define M_RETVAL_OK    VI_SUCCESS
    #define M_RETVAL_TYPE  VI_STATUS
    #define M_RETVAL_INI   _VI_ERROR


    /*****************************************************************************
     *   function names for AXI3910-board */

#if defined (_AXI3910)

  #define M_APIINIT                   Ai3910AxiInit
  #define M_APIEXIT                   Ai3910AxiExit
    #define M_APIOPEN                   Ai3910AxiOpen
    #define M_APIOPENEX                 Ai3910AxiOpenEx
  #define M_APICLOSE                  Ai3910AxiClose
  #define M_APIGETDEVICEINFO           Ai3910AxiGetDeviceInfo
  #define M_APIGETBOARDINFO           Ai3910AxiGetBoardInfo
  #define M_APIGETLIBRARYINFO         Ai3910AxiGetLibraryInfo

    #define M_APIREADRECDATA             Ai3910AxiLsReadRecData
    #define M_APIWRITEREPDATA             Ai3910AxiLsWriteRepData

    #define M_APISETSYSDBGLEVEL          Ai3910AxiSetSysDbgLevel
    #define M_APISETDLLDBGLEVEL          Ai3910AxiSetDllDbgLevel


    /* System Commands */
    #define M_APICMDINI                 Ai3910AxiCmdLsIni
    #define M_APICMDRESET                 Ai3910AxiCmdLsReset
    #define M_APICMDBITE                 Ai3910AxiCmdLsBite
    #define M_APICMDDEFRESPTOUT         Ai3910AxiCmdLsDefRespTout
    #define M_APICMDREADSWVERSION         Ai3910AxiCmdLsReadSWVersion
    #define M_APICMDLOADSREC             Ai3910AxiCmdLsLoadSRec
    #define M_APICMDPROGFLASH             Ai3910AxiCmdLsProgFlash
    #define M_APICMDEXECSYS             Ai3910AxiCmdLsExecSys
    #define M_APICMDSETIRIGTIME         Ai3910AxiCmdLsSetIrigTime
    #define M_APICMDGETIRIGTIME         Ai3910AxiCmdLsGetIrigTime
    #define M_APICMDDEFMILBUSPROTOCOL     Ai3910AxiCmdLsDefMilbusProtocol

    #define M_APIREADBSPVERSION         Ai3910AxiReadBSPVersion
    #define M_APICMDFIFOINI             Ai3910AxiCmdLsFifoIni
    #define M_APICMDSYSTAGDEF           Ai3910AxiCmdLsSystagDef
    #define M_APICMDSYSTAGCON           Ai3910AxiCmdLsSystagCon
    #define M_APICMDTRACKDEF            Ai3910AxiCmdLsTrackDef
    #define M_APICMDTRACKDEFEX          Ai3910AxiCmdLsTrackDefEx
    #define M_APICMDTRACKREAD           Ai3910AxiCmdLsTrackRead
    #define M_APICMDTRACKREADEX         Ai3910AxiCmdLsTrackReadEx
    #define M_APICMDTRACKSCAN           Ai3910AxiCmdLsTrackScan
    #define M_APICMDTRACKPREALLOC       Ai3910AxiCmdLsTrackPreAlloc
    #define M_APICMDREADDISCRETES       Ai3910AxiCmdLsReadDiscretes
    #define M_APICMDREADDISCRETESCONFIG Ai3910AxiCmdLsReadDiscretesConfig
    #define M_APICMDWRITEDISCRETES      Ai3910AxiCmdLsWriteDiscretes
    #define M_APICMDINITDISCRETES       Ai3910AxiCmdLsInitDiscretes
    #define M_APICMDSYSTRIGGEREDGEINPUTSET Ai3910AxiCmdLsSysTriggerEdgeInputSet
    #define M_APICMDSYSTRIGGEREDGEINPUTGET Ai3910AxiCmdLsSysTriggerEdgeInputGet

    /* Buffer Commands */
    #define M_APICMDBUFDEF                 Ai3910AxiCmdLsBufDef
    #define M_APICMDBUFREAD             Ai3910AxiCmdLsBufRead
    #define M_APICMDRAMWRITEDATASET     Ai3910AxiCmdLsRamWriteDataset
    #define M_APICMDRAMREADDATASET         Ai3910AxiCmdLsRamReadDataset
    #define M_APICMDBUFWRITE             Ai3910AxiCmdLsBufWrite

    #define M_APICMDBUFC1760CON         Ai3910AxiCmdLsBufC1760Con
    #define M_APIREADMEMDATA             Ai3910AxiReadMemData
    #define M_APIWRITEMEMDATA             Ai3910AxiWriteMemData
    #define M_APIWRITEBLOCKMEMDATA         Ai3910AxiWriteBlockMemData
    #define M_APIREADBLOCKMEMDATA         Ai3910AxiReadBlockMemData
    #define M_APICMDFIFOWRITE             Ai3910AxiCmdLsFifoWrite
    #define M_APICMDFIFOREADSTATUS         Ai3910AxiCmdLsFifoReadStatus
    #define M_APIBHMODIFY                Ai3910AxiLsBHModify

  #define M_APICMDSYNCCOUNTERSET      Ai3910AxiCmdLsSyncCounterSet
  #define M_APICMDSYNCCOUNTERGET      Ai3910AxiCmdLsSyncCounterGet
  #define M_APICMDSYSPXICON           Ai3910AxiCmdLsSysPXICon
  #define M_APICMDSYSGETMEMLOC        Ai3910AxiCmdLsSysGetMemLocation

    /* BC Commands */
    #define M_APICMDBCINI                 Ai3910AxiCmdLsBCIni
    #define M_APICMDBCBHDEF             Ai3910AxiCmdLsBCBHDef
    #define M_APICMDBCBHREAD      Ai3910AxiCmdLsBCBHRead
    #define M_APICMDBCXFERDEF           Ai3910AxiCmdLsBCXferDef
    #define M_APICMDBCXFERDEFERR        Ai3910AxiCmdLsBCXferDefErr
    #define M_APICMDBCXFERCTRL             Ai3910AxiCmdLsBCXferCtrl
    #define M_APICMDBCDYTAGDEF          Ai3910AxiCmdLsBCDytagDef
    #define M_APICMDBCGETDYTAGDEF          Ai3910AxiCmdLsBCGetDytagDef 
    #define M_APICMDBCFRAMEDEF             Ai3910AxiCmdLsBCFrameDef
    #define M_APICMDBCMFRAMEDEF         Ai3910AxiCmdLsBCMFrameDef
    #define M_APICMDBCMFRAMEDEFEX        Ai3910AxiCmdLsBCMFrameDefEx
    #define M_APICMDBCGETMAJFRAMEDEF Ai3910AxiCmdLsBCGetMajorFrameDefinition
    #define M_APICMDBCGETFRAMEDEF Ai3910AxiCmdLsBCGetMinorFrameDefinition
    #define M_APICMDBCSTART             Ai3910AxiCmdLsBCStart
    #define M_APICMDBCHALT                 Ai3910AxiCmdLsBCHalt
    #define M_APICMDBCSTATUSREAD         Ai3910AxiCmdLsBCStatusRead
    #define M_APICMDBCXFERREAD             Ai3910AxiCmdLsBCXferRead
    #define M_APICMDBCXFERREADEX        Ai3910AxiCmdLsBCXferReadEx
    #define M_APICMDBCACYCPREP             Ai3910AxiCmdLsBCAcycPrep
    #define M_APICMDBCACYCSEND             Ai3910AxiCmdLsBCAcycSend
    #define M_APICMDBCASSIGNFIFO         Ai3910AxiCmdLsBCAssignFifo
    #define M_APICMDBCSRVREQVECSTATUS    Ai3910AxiCmdLsBCSrvReqVecStatus
    #define M_APICMDBCSRVREQVECCON    Ai3910AxiCmdLsBCSrvReqVecCon
    #define M_APICMDBCGETXFERBHINFO Ai3910AxiCmdLsBcGetXferBufferHeaderInfo 
    #define M_APICMDBCINSTRTBLINI       Ai3910AxiCmdLsBCInstrTblIni           // Brett Crossley, AIM-USA, 15Apr2004
    #define M_APICMDBCINSTRTBLGETADDRFROMLABEL       Ai3910AxiCmdLsBCInstrTblGetAddrFromLabel  // Brett Crossley, AIM-USA, 15Apr2004
    #define M_APICMDBCINSTRTBLGEN       Ai3910AxiCmdLsBCInstrTblGen           // Brett Crossley, AIM-USA, 15Apr2004
  #define M_APICMDBCMODECTRL      Ai3910AxiCmdLsBCModeCtrl                // MR, AIM-Gmbh, 05Jan2007


    /* RT Commands */
    #define M_APICMDRTINI                 Ai3910AxiCmdLsRTIni
    #define M_APICMDRTGETSIMULATIONINFO Ai3910AxiCmdLsRTGetSimulationInfo
    #define M_APICMDRTENADIS            Ai3910AxiCmdLsRTEnaDis
    #define M_APICMDRTRESPTIME             Ai3910AxiCmdLsRTRespTime
    #define M_APICMDRTRESPTIMEGET        Ai3910AxiCmdLsRTRespTimeGet
    #define M_APICMDRTNXW                 Ai3910AxiCmdLsRTNXW
    #define M_APICMDRTLCW                 Ai3910AxiCmdLsRTLCW
    #define M_APICMDRTLSW                 Ai3910AxiCmdLsRTLSW
    #define M_APICMDRTBHDEF             Ai3910AxiCmdLsRTBHDef
    #define M_APICMDRTBHREAD             Ai3910AxiCmdLsRTBHRead
    #define M_APICMDRTSACON             Ai3910AxiCmdLsRTSACon
    #define M_APICMDRTSACONERR             Ai3910AxiCmdLsRTSAConErr
    #define M_APICMDRTGETSACONERR             Ai3910AxiCmdLsRTGetSAConErr 
    #define M_APICMDRTGETSABHINFO             Ai3910AxiCmdLsRTGetSABufferHeaderInfo
    #define M_APICMDRTDYTAGDEF             Ai3910AxiCmdLsRTDytagDef
    #define M_APICMDRTGETDYTAGDEF     Ai3910AxiCmdLsRTGetDytagDef 
    #define M_APICMDRTSTART             Ai3910AxiCmdLsRTStart
    #define M_APICMDRTHALT                 Ai3910AxiCmdLsRTHalt
    #define M_APICMDRTSTATUSREAD         Ai3910AxiCmdLsRTStatusRead
    #define M_APICMDRTMSGREAD             Ai3910AxiCmdLsRTMsgRead
    #define M_APICMDRTMSGREADALL         Ai3910AxiCmdLsRTMsgReadAll
    #define M_APICMDRTSAMSGREAD         Ai3910AxiCmdLsRTSAMsgRead
    #define M_APICMDRTSAMSGREADEX     Ai3910AxiCmdLsRTSAMsgReadEx
    #define M_APICMDRTGLOBALCON         Ai3910AxiCmdLsRTGlobalCon
    #define M_APICMDRTSAASSIGNFIFO         Ai3910AxiCmdLsRTSAAssignFifo
  #define M_APICMDRTMODECTRL      Ai3910AxiCmdLsRTModeCtrl

    /* BM Commands */
    #define M_APICMDBMINI                 Ai3910AxiCmdLsBMIni
    #define M_APICMDBMINTRMODE             Ai3910AxiCmdLsBMIntrMode
    #define M_APICMDBMCAPMODE             Ai3910AxiCmdLsBMCapMode
    #define M_APICMDBMSWXMINI             Ai3910AxiCmdLsBMSWXMIni
    #define M_APICMDBMSTART             Ai3910AxiCmdLsBMStart
    #define M_APICMDBMHALT                 Ai3910AxiCmdLsBMHalt
    #define M_APICMDBMSTATUSREAD         Ai3910AxiCmdLsBMStatusRead
    #define M_APICMDBMACTREAD             Ai3910AxiCmdLsBMActRead
    #define M_APICMDBMSTACKPREAD         Ai3910AxiCmdLsBMStackpRead
    #define M_APICMDBMSTACKENTRYREAD     Ai3910AxiCmdLsBMStackEntryRead
    #define M_APICMDBMSTACKENTRYFIND     Ai3910AxiCmdLsBMStackEntryFind
    #define M_APICMDBMRECSTATUSREAD     Ai3910AxiCmdLsBMRecStatusRead
    #define M_APICMDBMCOPYRECDAT         Ai3910AxiCmdLsBMCopyRecDat
    #define M_APICMDBMFILTERINI         Ai3910AxiCmdLsBMFilterIni
    #define M_APICMDBMILLEGALINI         Ai3910AxiCmdLsBMIllegalIni
    #define M_APICMDBMFTWINI             Ai3910AxiCmdLsBMFTWIni
    #define M_APICMDBMTIWINI             Ai3910AxiCmdLsBMTIWIni
    #define M_APICMDBMTCIINI             Ai3910AxiCmdLsBMTCIIni
    #define M_APICMDBMTCBINI             Ai3910AxiCmdLsBMTCBIni
    #define M_APICMDBMRTACTREAD         Ai3910AxiCmdLsBMRTActRead
    #define M_APICMDBMRTSAACTREAD         Ai3910AxiCmdLsBMRTSAActRead
    #define M_APICMDBMINIMSGFLTREC         Ai3910AxiCmdLsBMIniMsgFltRec
    #define M_APICMDBMREADMSGFLTREC     Ai3910AxiCmdLsBMReadMsgFltRec
    #define M_APICMDBMDYTAGMONDEF     Ai3910AxiCmdLsBMDytagMonDef
    #define M_APICMDBMDYTAGMONREAD     Ai3910AxiCmdLsBMDytagMonRead


    /* Calibration Commands */
    #define M_APICMDCALSIGCON             Ai3910AxiCmdLsCalSigCon
    #define M_APICMDCALCPLCON             Ai3910AxiCmdLsCalCplCon
    #define M_APICMDCALXMTCON             Ai3910AxiCmdLsCalXmtCon
    #define M_APICMDCALTRANSCON           Ai3910AxiCmdLsCalTransCon

    /* Queueing Commands */
    #define M_APICMDQUEUEINI             Ai3910AxiCmdLsQueueIni
    #define M_APICMDQUEUEREAD             Ai3910AxiCmdLsQueueRead
    #define M_APICMDQUEUEFLUSH             Ai3910AxiCmdLsQueueFlush
    #define M_APICMDQUEUESTART             Ai3910AxiCmdLsQueueStart
    #define M_APICMDQUEUEHALT             Ai3910AxiCmdLsQueueHalt

    /* Replay Commands */
    #define M_APICMDREPLAYINI             Ai3910AxiCmdLsReplayIni
    #define M_APICMDREPLAYSTART         Ai3910AxiCmdLsReplayStart
    #define M_APICMDREPLAYSTOP             Ai3910AxiCmdLsReplayStop
    #define M_APICMDREPLAYSTATUS         Ai3910AxiCmdLsReplayStatus
    #define M_APICMDREPLAYRT             Ai3910AxiCmdLsReplayRT

    #define M_AXI_INSTINTHANDLER        Ai3910AxiInstIntHandler
    #define M_AXI_DELINTHANDLER         Ai3910AxiDelIntHandler

#else

    //***************************************************************************
    //  API1553-DS Functions - External References                  //AXI-SECTION
    //***************************************************************************

    //###############################################################
    //    API Functions
    //###############################################################
  #define M_APIINIT                   Ai1553AxiInit
  #define M_APIEXIT                   Ai1553AxiExit
    #define M_APIOPEN                   Ai1553AxiOpen
    #define M_APIOPENEX                 Ai1553AxiOpenEx
    #define M_APICLOSE                  Ai1553AxiClose
  #define M_APIGETBOARDINFO           Ai1553AxiGetBoardInfo
  #define M_APIGETLIBRARYINFO         Ai1553AxiGetLibraryInfo

    #define M_APIREADRECDATA             Ai1553AxiReadRecData
    #define M_APIWRITEREPDATA             Ai1553AxiWriteRepData

    #define M_APISETSYSDBGLEVEL          Ai1553AxiSetSysDbgLevel
    #define M_APISETDLLDBGLEVEL          Ai1553AxiSetDllDbgLevel


    /* System Commands */
    #define M_APICMDINI                 Ai1553AxiCmdIni
    #define M_APICMDRESET                 Ai1553AxiCmdReset
    #define M_APICMDBITE                 Ai1553AxiCmdBite
    #define M_APICMDDEFRESPTOUT         Ai1553AxiCmdDefRespTout
    #define M_APICMDREADSWVERSION         Ai1553AxiCmdReadSWVersion
    #define M_APICMDLOADSREC             Ai1553AxiCmdLoadSRec
    #define M_APICMDPROGFLASH             Ai1553AxiCmdProgFlash
    #define M_APICMDEXECSYS             Ai1553AxiCmdExecSys
    #define M_APICMDSETIRIGTIME         Ai1553AxiCmdSetIrigTime
    #define M_APICMDGETIRIGTIME         Ai1553AxiCmdGetIrigTime
    #define M_APICMDDEFMILBUSPROTOCOL     Ai1553AxiCmdDefMilbusProtocol

    #define M_APIREADBSPVERSION         Ai1553AxiReadBSPVersion
    #define M_APICMDFIFOINI             Ai1553AxiCmdFifoIni
    #define M_APICMDSYSTAGDEF           Ai1553AxiCmdSystagDef
    #define M_APICMDSYSTAGCON           Ai1553AxiCmdSystagCon
    #define M_APICMDTRACKDEF            Ai1553AxiCmdTrackDef
    #define M_APICMDTRACKDEFEX          Ai1553AxiCmdTrackDefEx
    #define M_APICMDTRACKREAD           Ai1553AxiCmdTrackRead
    #define M_APICMDTRACKREADEX         Ai1553AxiCmdTrackReadEx
    #define M_APICMDTRACKSCAN           Ai1553AxiCmdTrackScan
    #define M_APICMDTRACKPREALLOC       Ai1553AxiCmdTrackPreAlloc
    #define M_APICMDREADDISCRETES       Ai1553AxiCmdLsReadDiscretes
    #define M_APICMDREADDISCRETESCONFIG Ai1553AxiCmdLsReadDiscretesConfig
    #define M_APICMDWRITEDISCRETES      Ai1553AxiCmdLsWriteDiscretes
    #define M_APICMDINITDISCRETES       Ai1553AxiCmdLsInitDiscretes
    #define M_APICMDSYSTRIGGEREDGEINPUTSET Ai1553AxiCmdLsSysTriggerEdgeInputSet
    #define M_APICMDSYSTRIGGEREDGEINPUTGET Ai1553AxiCmdLsSysTriggerEdgeInputGet

    /* Buffer Commands */
    #define M_APICMDBUFDEF                 Ai1553AxiCmdBufDef
    #define M_APICMDBUFREAD             Ai1553AxiCmdBufRead
    #define M_APICMDRAMWRITEDATASET     Ai1553AxiCmdRamWriteDataset
    #define M_APICMDRAMREADDATASET         Ai1553AxiCmdRamReadDataset
    #define M_APICMDBUFWRITE             Ai1553AxiCmdBufWrite

    #define M_APICMDBUFC1760CON         Ai1553AxiCmdBufC1760Con
    #define M_APIREADMEMDATA             Ai1553AxiReadMemData
    #define M_APIWRITEMEMDATA             Ai1553AxiWriteMemData
    #define M_APIREADBLOCKMEMDATA         Ai1553AxiReadBlockMemData
    #define M_APIWRITEBLOCKMEMDATA         Ai1553AxiWriteBlockMemData
    #define M_APICMDFIFOWRITE             Ai1553AxiCmdFifoWrite
    #define M_APICMDFIFOREADSTATUS         Ai1553AxiCmdFifoReadStatus
    #define M_APIBHMODIFY                Ai1553AxiBHModify

  #define M_APICMDSYNCCOUNTERSET      Ai1553AxiCmdSyncCounterSet
  #define M_APICMDSYNCCOUNTERGET      Ai1553AxiCmdSyncCounterGet
  #define M_APICMDSYSPXICON           Ai1553AxiCmdSysPXICon
  #define M_APICMDSYSGETMEMLOC        ApiCmdSysGetMemLocation

    /* BC Commands */
    #define M_APICMDBCINI                 Ai1553AxiCmdBCIni
    #define M_APICMDBCBHDEF             Ai1553AxiCmdBCBHDef
    #define M_APICMDBCBHREAD      Ai1553AxiCmdBCBHRead
    #define M_APICMDBCXFERDEF           Ai1553AxiCmdBCXferDef
    #define M_APICMDBCXFERDEFERR           Ai1553AxiCmdBCXferDefErr
    #define M_APICMDBCXFERCTRL             Ai1553AxiCmdBCXferCtrl
    #define M_APICMDBCDYTAGDEF          Ai1553AxiCmdBCDytagDef
    #define M_APICMDBCGETDYTAGDEF          Ai1553AxiCmdBCGetDytagDef 
    #define M_APICMDBCFRAMEDEF             Ai1553AxiCmdBCFrameDef
    #define M_APICMDBCMFRAMEDEF         Ai1553AxiCmdBCMFrameDef
    #define M_APICMDBCMFRAMEDEFEX        Ai1553AxiCmdBCMFrameDefEx
    #define M_APICMDBCGETMAJFRAMEDEF Ai1553AxiCmdBCGetMajorFrameDefinition
    #define M_APICMDBCGETFRAMEDEF Ai1553AxiCmdBCGetMinorFrameDefinition
    #define M_APICMDBCSTART             Ai1553AxiCmdBCStart
    #define M_APICMDBCHALT                 Ai1553AxiCmdBCHalt
    #define M_APICMDBCSTATUSREAD         Ai1553AxiCmdBCStatusRead
    #define M_APICMDBCXFERREAD             Ai1553AxiCmdBCXferRead
    #define M_APICMDBCXFERREADEX         Ai1553AxiCmdBCXferReadEx
    #define M_APICMDBCACYCPREP             Ai1553AxiCmdBCAcycPrep
    #define M_APICMDBCACYCSEND             Ai1553AxiCmdBCAcycSend
    #define M_APICMDBCASSIGNFIFO         Ai1553AxiCmdBCAssignFifo
    #define M_APICMDBCSRVREQVECSTATUS    Ai1553AxiCmdBCSrvReqVecStatus
    #define M_APICMDBCSRVREQVECCON    Ai1553AxiCmdBCSrvReqVecCon
    #define M_APICMDBCGETXFERBHINFO Ai1553AxiCmdBcGetXferBufferHeaderInfo 
    #define M_APICMDBCINSTRTBLINI       Ai1553AxiCmdBCInstrTblIni
    #define M_APICMDBCINSTRTBLGETADDRFROMLABEL       Ai1553AxiCmdBCInstrTblGetAddrFromLabel
    #define M_APICMDBCINSTRTBLGEN       Ai1553AxiCmdBCInstrTblGen
  #define M_APICMDBCMODECTRL      Ai1553AxiCmdBCModeCtrl

    /* RT Commands */
    #define M_APICMDRTINI                 Ai1553AxiCmdRTIni
    #define M_APICMDRTGETSIMULATIONINFO Ai1553AxiCmdRTGetSimulationInfo
    #define M_APICMDRTENADIS            Ai1553AxiCmdRTEnaDis
    #define M_APICMDRTRESPTIME             Ai1553AxiCmdRTRespTime
    #define M_APICMDRTRESPTIMEGET        Ai1553AxiCmdRTRespTimeGet
    #define M_APICMDRTNXW                 Ai1553AxiCmdRTNXW
    #define M_APICMDRTLCW                 Ai1553AxiCmdRTLCW
    #define M_APICMDRTLSW                 Ai1553AxiCmdRTLSW
    #define M_APICMDRTBHDEF             Ai1553AxiCmdRTBHDef
    #define M_APICMDRTBHREAD             Ai1553AxiCmdRTBHRead
    #define M_APICMDRTSACON             Ai1553AxiCmdRTSACon
    #define M_APICMDRTSACONERR             Ai1553AxiCmdRTSAConErr
    #define M_APICMDRTGETSACONERR             Ai1553AxiCmdRTGetSAConErr
    #define M_APICMDRTGETSABHINFO             Ai1553AxiCmdRTGetSABufferHeaderInfo 
    #define M_APICMDRTDYTAGDEF             Ai1553AxiCmdRTDytagDef
    #define M_APICMDRTGETDYTAGDEF     Ai1553AxiCmdRTGetDytagDef 
    #define M_APICMDRTSTART             Ai1553AxiCmdRTStart
    #define M_APICMDRTHALT                 Ai1553AxiCmdRTHalt
    #define M_APICMDRTSTATUSREAD         Ai1553AxiCmdRTStatusRead
    #define M_APICMDRTMSGREAD             Ai1553AxiCmdRTMsgRead
    #define M_APICMDRTMSGREADALL         Ai1553AxiCmdRTMsgReadAll
    #define M_APICMDRTSAMSGREAD         Ai1553AxiCmdRTSAMsgRead
    #define M_APICMDRTSAMSGREADEX        Ai1553AxiCmdRTSAMsgReadEx
    #define M_APICMDRTGLOBALCON         Ai1553AxiCmdRTGlobalCon
    #define M_APICMDRTSAASSIGNFIFO         Ai1553AxiCmdRTSAAssignFifo
  #define M_APICMDRTMODECTRL      Ai1553AxiCmdRTModeCtrl

    /* BM Commands */
    #define M_APICMDBMINI                 Ai1553AxiCmdBMIni
    #define M_APICMDBMINTRMODE             Ai1553AxiCmdBMIntrMode
    #define M_APICMDBMCAPMODE             Ai1553AxiCmdBMCapMode
    #define M_APICMDBMSWXMINI             Ai1553AxiCmdBMSWXMIni
    #define M_APICMDBMSTART             Ai1553AxiCmdBMStart
    #define M_APICMDBMHALT                 Ai1553AxiCmdBMHalt
    #define M_APICMDBMSTATUSREAD         Ai1553AxiCmdBMStatusRead
    #define M_APICMDBMACTREAD             Ai1553AxiCmdBMActRead
    #define M_APICMDBMSTACKPREAD         Ai1553AxiCmdBMStackpRead
    #define M_APICMDBMSTACKENTRYREAD     Ai1553AxiCmdBMStackEntryRead
    #define M_APICMDBMSTACKENTRYFIND     Ai1553AxiCmdBMStackEntryFind
    #define M_APICMDBMRECSTATUSREAD     Ai1553AxiCmdBMRecStatusRead
    #define M_APICMDBMCOPYRECDAT         Ai1553AxiCmdBMCopyRecDat
    #define M_APICMDBMFILTERINI         Ai1553AxiCmdBMFilterIni
    #define M_APICMDBMILLEGALINI         Ai1553AxiCmdBMIllegalIni
    #define M_APICMDBMFTWINI             Ai1553AxiCmdBMFTWIni
    #define M_APICMDBMTIWINI             Ai1553AxiCmdBMTIWIni
    #define M_APICMDBMTCIINI             Ai1553AxiCmdBMTCIIni
    #define M_APICMDBMTCBINI             Ai1553AxiCmdBMTCBIni
    #define M_APICMDBMRTACTREAD         Ai1553AxiCmdBMRTActRead
    #define M_APICMDBMRTSAACTREAD         Ai1553AxiCmdBMRTSAActRead
    #define M_APICMDBMINIMSGFLTREC         Ai1553AxiCmdBMIniMsgFltRec
    #define M_APICMDBMREADMSGFLTREC     Ai1553AxiCmdBMReadMsgFltRec
    #define M_APICMDBMDYTAGMONDEF     Ai1553AxiCmdBMDytagMonDef
    #define M_APICMDBMDYTAGMONREAD     Ai1553AxiCmdBMDytagMonRead

    /* Calibration Commands */
    #define M_APICMDCALSIGCON             Ai1553AxiCmdCalSigCon
    #define M_APICMDCALCPLCON             Ai1553AxiCmdCalCplCon
    #define M_APICMDCALXMTCON             Ai1553AxiCmdCalXmtCon  
    #define M_APICMDCALTRANSCON           Ai1553AxiCmdCalTransCon

    /* Queueing Commands */
    #define M_APICMDQUEUEINI             Ai1553AxiCmdQueueIni
    #define M_APICMDQUEUEREAD             Ai1553AxiCmdQueueRead
    #define M_APICMDQUEUEFLUSH             Ai1553AxiCmdQueueFlush
    #define M_APICMDQUEUESTART             Ai1553AxiCmdQueueStart
    #define M_APICMDQUEUEHALT             Ai1553AxiCmdQueueHalt

    /* Replay Commands */
    #define M_APICMDREPLAYINI             Ai1553AxiCmdReplayIni
    #define M_APICMDREPLAYSTART         Ai1553AxiCmdReplayStart
    #define M_APICMDREPLAYSTOP             Ai1553AxiCmdReplayStop
    #define M_APICMDREPLAYSTATUS         Ai1553AxiCmdReplayStatus
    #define M_APICMDREPLAYRT             Ai1553AxiCmdReplayRT

    #define M_AXI_INSTINTHANDLER        Ai1553AxiInstIntHandler
    #define M_AXI_DELINTHANDLER         Ai1553AxiDelIntHandler

#endif  // end #if defined (_AXI3910)
    /******************************************************************************
     *  local Vars (Modul)                                              //AXI-SECTION
    *******************************************************************************/


    /******************************************************************************
     *  extern Vars                                                      //AXI-SECTION
    *******************************************************************************/
    extern AiBoolean GetModuleNo (AiSession instHandle, AiUInt32 *Module);


    /******************************************************************************
     *  local prototypes                                              //AXI-SECTION
    *******************************************************************************/

                                                        // end #ifdef _VXI_PP_WIN32
#elif defined (_AIM_WINDOWS) || defined(_AIM_LVRT) || defined(_AIM_LINUX)            //AVI-SECTION
    /******************************************************************************
     *  Defines                                                          //AVI-SECTION
    *******************************************************************************/

    #define VI_SESSION     AiUInt32
    #define VI_STATUS      AiReturn

    #define M_BOARDID      bModule

    #define M_GETMODULE(InstrumentHandle, bModule)     GetModuleNo( InstrumentHandle, bModule )

    static AiBoolean GetModuleNo( VI_SESSION InstrumentHandle, AiUInt32* bModule );
    static AiBoolean GetModuleNo( VI_SESSION InstrumentHandle, AiUInt32* bModule ) { *bModule = InstrumentHandle; return TRUE; }

    #define M_RETVAL_ERR   retVal
    #define M_RETVAL_OK    retVal
    #define M_RETVAL_TYPE  VI_STATUS
    #define M_RETVAL_INI   -1

    //***************************************************************************
    //  API1553-DS Functions - External References                    //AVI-SECTION
    //***************************************************************************

    //###############################################################
    //    API Functions
    //###############################################################
  #define M_APIINIT                   ApiInitCvi
  #define M_APIEXIT                   ApiExitCvi
    #define M_APIOPEN                   ApiOpenCvi
    #define M_APIOPENEX                 ApiOpenExCvi
    #define M_APICLOSE                  ApiCloseCvi
    #define M_APIGETDRIVERINFO          ApiGetDriverInfoCvi
    #define M_APIGETDEVICECONFIG        ApiGetDeviceConfigCvi
    #define M_APISETDEVICECONFIG        ApiSetDeviceConfigCvi
    #define M_APIGETBOARDINFO           ApiGetBoardInfoCvi
    #define M_APIGETLIBRARYINFO         ApiGetLibraryInfoCvi
    #define M_APIGETERRORDESCRIPTION    ApiGetErrorDescriptionCvi
    #define M_APIGETERRORMESSAGE        ApiGetErrorMessageCvi
    #define M_APIGETBOARDNAME           ApiGetBoardNameCvi

    #define M_APIREADRECDATA             ApiReadRecDataCvi
    #define M_APIWRITEREPDATA             ApiWriteRepDataCvi

    #define M_APISETSYSDBGLEVEL          ApiSetSysDbgLevelCvi
    #define M_APISETDLLDBGLEVEL          ApiSetDllDbgLevelCvi

    /* System Commands */
    #define M_APICMDINI                 ApiCmdIniCvi
    #define M_APICMDRESET                 ApiCmdResetCvi
    #define M_APICMDBITE                 ApiCmdBiteCvi
    #define M_APICMDDEFRESPTOUT         ApiCmdDefRespToutCvi
    #define M_APICMDREADSWVERSION         ApiCmdReadSWVersionCvi
    #define M_APICMDLOADSREC             ApiCmdLoadSRecCvi
    #define M_APICMDPROGFLASH             ApiCmdProgFlashCvi
    #define M_APICMDEXECSYS             ApiCmdExecSysCvi

    #define M_APICMDSETIRIGTIME         ApiCmdSetIrigTimeCvi
    #define M_APICMDSETIRIGSTATUS        ApiCmdSetIrigStatusCvi
    #define M_APICMDGETIRIGTIME         ApiCmdGetIrigTimeCvi
    #define M_APICMDGETIRIGSTATUS       ApiCmdGetIrigStatusCvi

    #define M_APICMDDEFMILBUSPROTOCOL   ApiCmdDefMilbusProtocolCvi
    #define M_APIREADBSPVERSION         ApiReadBSPVersionCvi
    #define M_APICMDFIFOINI             ApiCmdFifoIniCvi
    #define M_APICMDSYSTAGDEF           ApiCmdSystagDefCvi
    #define M_APICMDSYSTAGCON           ApiCmdSystagConCvi
    #define M_APICMDTRACKDEF            ApiCmdTrackDefCvi
    #define M_APICMDTRACKDEFEX          ApiCmdTrackDefExCvi
    #define M_APICMDTRACKREAD           ApiCmdTrackReadCvi
    #define M_APICMDTRACKREADEX         ApiCmdTrackReadExCvi
    #define M_APICMDTRACKSCAN           ApiCmdTrackScanCvi
    #define M_APICMDTRACKPREALLOC       ApiCmdTrackPreAllocCvi
    #define M_APICMDREADDISCRETES       ApiCmdReadDiscretesCvi
    #define M_APICMDREADDISCRETESCONFIG ApiCmdReadDiscretesConfigCvi
    #define M_APICMDWRITEDISCRETES      ApiCmdWriteDiscretesCvi
    #define M_APICMDINITDISCRETES       ApiCmdInitDiscretesCvi
    #define M_APICMDSYSTRIGGEREDGEINPUTSET ApiCmdSysTriggerEdgeInputSetCvi
    #define M_APICMDSYSTRIGGEREDGEINPUTGET ApiCmdSysTriggerEdgeInputGetCvi

    #define M_APICMDSYSMALLOC             ApiCmdSysMallocCvi
    #define M_APICMDSYSFREE             ApiCmdSysFreeCvi

    /* Buffer Commands */
    #define M_APICMDBUFDEF                 ApiCmdBufDefCvi
    #define M_APICMDBUFREAD             ApiCmdBufReadCvi
    #define M_APICMDRAMWRITEDATASET     ApiCmdRamWriteDatasetCvi
    #define M_APICMDRAMREADDATASET         ApiCmdRamReadDatasetCvi
    #define M_APICMDBUFWRITE             ApiCmdBufWriteCvi
    #define M_APICMDBUFWRITEBLOCK          ApiCmdBufWriteBlockCvi
    #define M_APICMDBUFHOSTCONTROL             ApiCmdBufHostControlCvi
    #define M_APICMDBUFC1760CON         ApiCmdBufC1760ConCvi
    #define M_APIREADMEMDATA             ApiReadMemDataCvi
    #define M_APIWRITEMEMDATA             ApiWriteMemDataCvi
    #define M_APIREADBLOCKMEMDATA         ApiReadBlockMemDataCvi
    #define M_APIWRITEBLOCKMEMDATA         ApiWriteBlockMemDataCvi
    #define M_APICMDFIFOWRITE             ApiCmdFifoWriteCvi
    #define M_APICMDFIFOREADSTATUS         ApiCmdFifoReadStatusCvi
    #define M_APIBHMODIFY                ApiBHModifyCvi

  #define M_APICMDSYNCCOUNTERSET      ApiCmdSyncCounterSetCvi
  #define M_APICMDSYNCCOUNTERGET      ApiCmdSyncCounterGetCvi
  #define M_APICMDSYSPXICON           ApiCmdSysPXIConCvi
  #define M_APICMDSYSGETMEMLOC        ApiCmdSysGetMemLocationCvi
#define M_APICMDSYSPXIGEOGRAPHICALADDRESSGET           ApiCmdSysPXIGeographicalAddressGetCvi

    /* BC Commands */
    #define M_APICMDBCINI                    ApiCmdBCIniCvi
    #define M_APICMDBCBHDEF                ApiCmdBCBHDefCvi
    #define M_APICMDBCBHREAD                 ApiCmdBCBHReadCvi
    #define M_APICMDBCXFERDEF                ApiCmdBCXferDefCvi
    #define M_APICMDBCXFERDEFERR         ApiCmdBCXferDefErrCvi
    #define M_APICMDBCGETXFERDEF         ApiCmdBCGetXferDefCvi
    #define M_APICMDBCXFERCTRL                ApiCmdBCXferCtrlCvi
    #define M_APICMDBCXFERDESCGET              ApiCmdBCXferDescGetCvi
    #define M_APICMDBCXFERDESCMOD              ApiCmdBCXferDescModCvi
    #define M_APICMDBCDYTAGDEF                ApiCmdBCDytagDefCvi
    #define M_APICMDBCGETDYTAGDEF          ApiCmdBCGetDytagDefCvi
    #define M_APICMDBCFRAMEDEF             ApiCmdBCFrameDefCvi
    #define M_APICMDBCMFRAMEDEF         ApiCmdBCMFrameDefCvi
    #define M_APICMDBCMFRAMEDEFEX        ApiCmdBCMFrameDefExCvi
    #define M_APICMDBCGETMAJFRAMEDEF ApiCmdBCGetMajorFrameDefinitionCvi
    #define M_APICMDBCGETFRAMEDEF ApiCmdBCGetMinorFrameDefinitionCvi
    #define M_APICMDBCSTART             ApiCmdBCStartCvi
    #define M_APICMDBCHALT                 ApiCmdBCHaltCvi
    #define M_APICMDBCSTATUSREAD         ApiCmdBCStatusReadCvi
    #define M_APICMDBCXFERREAD             ApiCmdBCXferReadCvi
    #define M_APICMDBCXFERREADEX         ApiCmdBCXferReadExCvi
    #define M_APICMDBCACYCPREP             ApiCmdBCAcycPrepCvi
    #define M_APICMDBCACYCSEND             ApiCmdBCAcycSendCvi
    #define M_APICMDBCASSIGNFIFO         ApiCmdBCAssignFifoCvi
    #define M_APICMDBCSRVREQVECSTATUS    ApiCmdBCSrvReqVecStatusCvi
    #define M_APICMDBCSRVREQVECCON    ApiCmdBCSrvReqVecConCvi
    #define M_APICMDBCGETXFERBHINFO ApiCmdBCGetXferBufferHeaderInfoCvi
      #define M_APICMDBCINSTRTBLINI       ApiCmdBCInstrTblIniCvi          // Brett Crossley, AIM-USA, 15Apr2004
    #define M_APICMDBCINSTRTBLGETADDRFROMLABEL       ApiCmdBCInstrTblGetAddrFromLabelCvi // Brett Crossley, AIM-USA, 15Apr2004
    #define M_APICMDBCINSTRTBLGEN       ApiCmdBCInstrTblGenCvi            // Brett Crossley, AIM-USA, 15Apr2004
  #define M_APICMDBCMODECTRL      ApiCmdBCModeCtrlCvi           // MR, AIM-Gmbh, 05Jan2007
  
  

    /* RT Commands */
    #define M_APICMDRTINI                 ApiCmdRTIniCvi
    #define M_APICMDRTGETSIMULATIONINFO ApiCmdRTGetSimulationInfoCvi
    #define M_APICMDRTENADIS             ApiCmdRTEnaDisCvi
    #define M_APICMDRTRESPTIME             ApiCmdRTRespTimeCvi
    #define M_APICMDRTRESPTIMEGET        ApiCmdRTRespTimeGetCvi
    #define M_APICMDRTNXW                 ApiCmdRTNXWCvi
    #define M_APICMDRTLCW                 ApiCmdRTLCWCvi
    #define M_APICMDRTLSW                 ApiCmdRTLSWCvi
    #define M_APICMDRTBHDEF             ApiCmdRTBHDefCvi
    #define M_APICMDRTBHREAD             ApiCmdRTBHReadCvi
    #define M_APICMDRTSACON             ApiCmdRTSAConCvi
    #define M_APICMDRTSACONERR             ApiCmdRTSAConErrCvi
    #define M_APICMDRTGETSACONERR             ApiCmdRTGetSAConErrCvi 
    #define M_APICMDRTGETSABHINFO             ApiCmdRTGetSABufferHeaderInfoCvi
    #define M_APICMDRTDYTAGDEF      ApiCmdRTDytagDefCvi
    #define M_APICMDRTGETDYTAGDEF      ApiCmdRTGetDytagDefCvi 
    #define M_APICMDRTSTART             ApiCmdRTStartCvi
    #define M_APICMDRTHALT                 ApiCmdRTHaltCvi
    #define M_APICMDRTSTATUSREAD         ApiCmdRTStatusReadCvi
    #define M_APICMDRTMSGREAD             ApiCmdRTMsgReadCvi
    #define M_APICMDRTMSGREADALL         ApiCmdRTMsgReadAllCvi
    #define M_APICMDRTSAMSGREAD         ApiCmdRTSAMsgReadCvi
    #define M_APICMDRTSAMSGREADEX        ApiCmdRTSAMsgReadExCvi
    #define M_APICMDRTGLOBALCON         ApiCmdRTGlobalConCvi
    #define M_APICMDRTSAASSIGNFIFO         ApiCmdRTSAAssignFifoCvi
  #define M_APICMDRTMODECTRL      ApiCmdRTModeCtrlCvi

    /* BM Commands */
    #define M_APICMDBMINI                 ApiCmdBMIniCvi
    #define M_APICMDBMINTRMODE             ApiCmdBMIntrModeCvi
    #define M_APICMDBMCAPMODE             ApiCmdBMCapModeCvi
    #define M_APICMDBMSWXMINI             ApiCmdBMSWXMIniCvi
    #define M_APICMDBMSTART             ApiCmdBMStartCvi
    #define M_APICMDBMHALT                 ApiCmdBMHaltCvi
    #define M_APICMDBMSTATUSREAD         ApiCmdBMStatusReadCvi
    #define M_APICMDBMACTREAD             ApiCmdBMActReadCvi
    #define M_APICMDBMSTACKPREAD         ApiCmdBMStackpReadCvi
    #define M_APICMDBMSTACKENTRYREAD    ApiCmdBMStackEntryReadCvi
    #define M_APICMDBMSTACKENTRYFIND    ApiCmdBMStackEntryFindCvi
    #define M_APICMDBMRECSTATUSREAD     ApiCmdBMRecStatusReadCvi
    #define M_APICMDBMCOPYRECDAT         ApiCmdBMCopyRecDatCvi
    #define M_APICMDBMFILTERINI         ApiCmdBMFilterIniCvi
    #define M_APICMDBMILLEGALINI         ApiCmdBMIllegalIniCvi
    #define M_APICMDBMFTWINI             ApiCmdBMFTWIniCvi
    #define M_APICMDBMTIWINI             ApiCmdBMTIWIniCvi
    #define M_APICMDBMTCIINI             ApiCmdBMTCIIniCvi
    #define M_APICMDBMTCBINI             ApiCmdBMTCBIniCvi
    #define M_APICMDBMRTACTREAD         ApiCmdBMRTActReadCvi
    #define M_APICMDBMRTSAACTREAD         ApiCmdBMRTSAActReadCvi
    #define M_APICMDBMINIMSGFLTREC         ApiCmdBMIniMsgFltRecCvi
    #define M_APICMDBMREADMSGFLTREC     ApiCmdBMReadMsgFltRecCvi
    #define M_APICMDBMDYTAGMONDEF     ApiCmdBMDytagMonDefCvi
    #define M_APICMDBMDYTAGMONREAD     ApiCmdBMDytagMonReadCvi

    /* Data Queue Commands (Milscope) */
    #define M_APICMDDATAQUEUECLOSE        ApiCmdDataQueueCloseCvi
    #define M_APICMDDATAQUEUECONTROL    ApiCmdDataQueueControlCvi
    #define M_APICMDDATAQUEUEOPEN        ApiCmdDataQueueOpenCvi
    #define M_APICMDDATAQUEUEREAD        ApiCmdDataQueueReadCvi
    #define M_APICMDSCOPESETUP            ApiCmdScopeSetupCvi
    #define M_APICMDSCOPETRIGGERDEF        ApiCmdScopeTriggerDefCvi
    #define M_APICMDSCOPECALIBRATE        ApiCmdScopeCalibrateCvi
  #define M_APICMDSCOPESTART        ApiCmdScopeStartCvi

    /* Calibration Commands */
    #define M_APICMDCALSIGCON             ApiCmdCalSigConCvi
    #define M_APICMDCALCPLCON             ApiCmdCalCplConCvi
    #define M_APICMDCALXMTCON             ApiCmdCalXmtConCvi
    #define M_APICMDCALTRANSCON           ApiCmdCalTransConCvi

    /* Queueing Commands */
    #define M_APICMDQUEUEINI             ApiCmdQueueIniCvi
    #define M_APICMDQUEUEREAD             ApiCmdQueueReadCvi
    #define M_APICMDQUEUEFLUSH             ApiCmdQueueFlushCvi
    #define M_APICMDQUEUESTART             ApiCmdQueueStartCvi
    #define M_APICMDQUEUEHALT             ApiCmdQueueHaltCvi

    /* Replay Commands */
    #define M_APICMDREPLAYINI             ApiCmdReplayIniCvi
    #define M_APICMDREPLAYSTART         ApiCmdReplayStartCvi
    #define M_APICMDREPLAYSTOP             ApiCmdReplayStopCvi
    #define M_APICMDREPLAYSTATUS         ApiCmdReplayStatusCvi
    #define M_APICMDREPLAYRT             ApiCmdReplayRTCvi

    /******************************************************************************
     *  local Vars (Modul)                                              //AVI-SECTION
    *******************************************************************************/


    /******************************************************************************
     *  extern Vars                                                      //AVI-SECTION
    *******************************************************************************/


    /******************************************************************************
     *  local prototypes                                              //AVI-SECTION
    *******************************************************************************/


AI_LIB_FUNC M_RETVAL_TYPE AI_CALL_CONV M_APIOPEN(VI_SESSION M_BOARDID, AiChar *SrvName, AiUInt32 *ModuleHandle);

AI_LIB_FUNC M_RETVAL_TYPE AI_CALL_CONV M_APIOPENEX(VI_SESSION M_BOARDID, AiUInt32 ul_Stream, AiChar *ac_SrvName, AiUInt32 *pul_ModuleHandle);
AI_LIB_FUNC M_RETVAL_TYPE AI_CALL_CONV M_APICLOSE(VI_SESSION M_BOARDID);


AI_LIB_FUNC M_RETVAL_TYPE AI_CALL_CONV M_APIGETDRIVERINFO(VI_SESSION M_BOARDID, 
                                                           AiUInt8  * puc_DeviceGroup,
                                                           AiUInt8  * puc_ReservedB2, 
                                                           AiUInt8  * puc_ReservedB3,
                                                           AiUInt8  * puc_ReservedB4, 
                                                           AiUInt16 * puw_ReservedW1,
                                                           AiUInt16 * puw_ReservedW2, 
                                                           AiUInt32 * pul_DriverFlags,
                                                           AiUInt32 * pul_SN, 
                                                           AiUInt32 * pul_BoardConfig,
                                                           AiUInt32 * pul_BoardType, 
                                                           AiUInt32 * pul_OpenConnections,
                                                           AiUInt32 * pul_ReservedLW6,
                                                           AiUInt32 * pul_ReservedLW7,
                                                           AiUInt32 * pul_ReservedLW8 );

AI_LIB_FUNC M_RETVAL_TYPE AI_CALL_CONV M_APIGETDEVICECONFIG(VI_SESSION M_BOARDID, AiUInt8 *uc_DmaEnabled, AiUInt8 *uc_DataQueueMemoryType,
                                                             AiUInt8 *uc_DataQueueMode, AiUInt8 *uc_ReservedB4, AiUInt16 *uw_ReservedW1, AiUInt16 *uw_ReservedW2, 
                                                             AiUInt32 *ul_DmaMinimumSize, AiUInt32 *ul_IntRequestCount, AiUInt32 *ul_DriverFlags,
                                                             AiUInt32 *ul_ReservedLW4, AiUInt32 *ul_ReservedLW5, AiUInt32 *ul_ReservedLW6, 
                                                             AiUInt32 *ul_ReservedLW7, AiUInt32 *ul_ReservedLW8 );

AI_LIB_FUNC M_RETVAL_TYPE AI_CALL_CONV M_APISETDEVICECONFIG(VI_SESSION M_BOARDID, AiUInt8 uc_DmaEnabled, AiUInt8 uc_DataQueueMemoryType,
                                                             AiUInt8 uc_DataQueueMode, AiUInt8 uc_ReservedB4, AiUInt16 uw_ReservedW1, AiUInt16 uw_ReservedW2, 
                                                             AiUInt32 ul_DmaMinimumSize, AiUInt32 ul_IntRequestCount, AiUInt32 ul_DriverFlags,
                                                             AiUInt32 ul_ReservedLW4, AiUInt32 ul_ReservedLW5, AiUInt32 ul_ReservedLW6, 
                                                             AiUInt32 ul_ReservedLW7, AiUInt32 ul_ReservedLW8 );



AI_LIB_FUNC M_RETVAL_TYPE AI_CALL_CONV M_APIGETBOARDINFO(VI_SESSION M_BOARDID, AiUInt32 *pul_DeviceType,
                                                          AiUInt32 *pul_NumberOfChannels, AiUInt32 *pul_NumberOfBiu,
                                                          AiUInt32 *pul_NovRamBoardType, AiUInt32 *pul_NovRamBoardConfig,
                                                          AiUInt32 *pul_SerialNumber, AiUInt32 *pul_PartNumber,
                                                          AiUInt32 *pul_GlobalRamSize, AiUInt32 *pul_SharedRamSize,
                                                          AiUInt32 *pul_CanChangeAmplitude, AiUInt32 *pul_CouplingCapabilities,
                                                          AiUInt32 *pul_IrigCapabilities );

AI_LIB_FUNC M_RETVAL_TYPE AI_CALL_CONV M_APIGETLIBRARYINFO(AiUInt32 *pul_AttachedApps);

AI_LIB_FUNC const char * AI_CALL_CONV M_APIGETERRORDESCRIPTION(AiInt16 wErrorCode);

AI_LIB_FUNC const char * AI_CALL_CONV M_APIGETERRORMESSAGE(AiReturn error_code);


AI_LIB_FUNC const char * AI_CALL_CONV M_APIGETBOARDNAME(AiUInt32 ulModHandle);


AI_LIB_FUNC M_RETVAL_TYPE AI_CALL_CONV M_APICMDINI(VI_SESSION M_BOARDID, AiUInt8 mode,
                                                    AiUInt8 bt[4],
                                                    AiUInt8* chns,
                                                    AiUInt8* prot,
                                                    AiUInt8* emod,
                                                    AiUInt8* irg,
                                                    AiUInt8* res1,
                                                    AiUInt16* pbi_id_biu1,
                                                    AiUInt16* asp_mon_id,
                                                    AiUInt16* asp_bite_id,
                                                    AiUInt16* pbi_id_biu2,
                                                    AiUInt32* board_config,
                                                    AiUInt32* glb_mem_size,
                                                    AiUInt32* glb_mem_addr,
                                                    AiUInt32* loc_dram_size,
                                                    AiUInt32* loc_dram_addr,
                                                    AiUInt32* shared_dram_size,
                                                    AiUInt32* shared_dram_addr,
                                                    AiUInt32* flash_ram_size,
                                                    AiUInt32* flash_ram_addr,
                                                    AiUInt32 pci[16],
                                                    AiUInt32 * board_type,
                                                    AiUInt32 * board_sub_type,
                                                    AiUInt32 * hardware_variant );


AI_LIB_FUNC M_RETVAL_TYPE AI_CALL_CONV M_APICMDRESET(VI_SESSION M_BOARDID, AiUInt8 biu, AiUInt8 rc,
                                                      AiUInt8* mbufs,
                                                      AiUInt8* sbufs,
                                                      AiUInt32* mon_addr,
                                                      AiUInt32* sim_addr);

AI_LIB_FUNC M_RETVAL_TYPE AI_CALL_CONV M_APICMDBITE(VI_SESSION M_BOARDID, AiUInt8 biu, AiUInt8 sc, AiUInt8 bite_status[]);

AI_LIB_FUNC M_RETVAL_TYPE AI_CALL_CONV M_APICMDDEFRESPTOUT(VI_SESSION M_BOARDID, AiUInt8 biu, AiFloat resp_tout);

AI_LIB_FUNC M_RETVAL_TYPE AI_CALL_CONV M_APICMDREADSWVERSION(VI_SESSION M_BOARDID, AiUInt8 biu, AiUInt16* fw_id, AiUInt16* sw_id, AiUInt16* lca_id, AiUInt32* lca_chks);

AI_LIB_FUNC M_RETVAL_TYPE AI_CALL_CONV M_APICMDLOADSREC(VI_SESSION M_BOARDID, AiUInt8 mode, AiUInt8 cnt, AiUInt32 offset, AiUInt8* srec, AiUInt8* st, AiUInt32* fsize);

AI_LIB_FUNC M_RETVAL_TYPE AI_CALL_CONV M_APICMDPROGFLASH(VI_SESSION M_BOARDID, AiUInt32 src_addr, AiUInt32 sector, AiUInt32 size, AiUInt8* st);

AI_LIB_FUNC M_RETVAL_TYPE AI_CALL_CONV M_APICMDEXECSYS(VI_SESSION M_BOARDID, AiUInt8 mode, AiUInt8 con);

AI_LIB_FUNC M_RETVAL_TYPE AI_CALL_CONV M_APICMDSETIRIGTIME(VI_SESSION M_BOARDID,
                                                            AiUInt32 day,
                                                            AiUInt32 hour,
                                                            AiUInt32 minute,
                                                            AiUInt32 second,
                                                            AiUInt32 microsecond);

AI_LIB_FUNC M_RETVAL_TYPE AI_CALL_CONV M_APICMDGETIRIGTIME(VI_SESSION M_BOARDID,
                                                            AiUInt32* day,
                                                            AiUInt32* hour,
                                                            AiUInt32* minute,
                                                            AiUInt32* second,
                                                            AiUInt32* microsecond);

AI_LIB_FUNC M_RETVAL_TYPE AI_CALL_CONV M_APICMDDEFMILBUSPROTOCOL(VI_SESSION M_BOARDID, AiUInt8 biu, AiUInt8 mil_prot, AiUInt8 mode, AiUInt8 rt);

AI_LIB_FUNC M_RETVAL_TYPE AI_CALL_CONV M_APIREADRECDATA(VI_SESSION M_BOARDID, AiUInt8 biu,
                                                         LPVOID lpBuf,
                                                         AiUInt8* status,
                                                         AiUInt32* hfi_cnt,
                                                         AiUInt32* saddr,
                                                         AiUInt32* size,
                                                         AiUInt32* lBytesRead);
AI_LIB_FUNC M_RETVAL_TYPE AI_CALL_CONV M_APIWRITEREPDATA(VI_SESSION M_BOARDID, AiUInt8 biu,
                                                          AiUInt8*  status,
                                                          AiUInt8*  padding1,
                                                          AiUInt16* padding2,
                                                          AiUInt32* rpi_cnt,
                                                          AiUInt32 saddr,
                                                          AiUInt32 size,
                                                          AiUInt32* entry_cnt,
                                                          AiAddr lpBuf,
                                                          AiUInt32* lBytesWritten);

AI_LIB_FUNC M_RETVAL_TYPE AI_CALL_CONV M_APICMDFIFOINI(VI_SESSION M_BOARDID, AiUInt8 biu, AiUInt8 fifo_nbr, AiUInt16 buf_nbr);


AI_LIB_FUNC M_RETVAL_TYPE AI_CALL_CONV M_APIREADBSPVERSION(VI_SESSION M_BOARDID,
                                                            AiUInt32* pul_FirmwareVer, AiUInt32* pul_TargetVer, AiUInt32 *pul_TargetBuild,
                                                            AiUInt32* pul_LcaVer1, AiUInt32* pul_LcaVer2,
                                                            AiUInt32* pul_LcaCheckSum,
                                                            AiUInt32* pul_SysDrvVer, AiUInt32 *pul_SysDrvBuild, AiUInt32* pul_DllVer,
                                                            AiUInt32* pul_DllBuild, AiUInt32* pul_BoardSerialNr,
                                                            AiUInt8*  puc_BspCompatibility);

AI_LIB_FUNC AiReturn AI_CALL_CONV ApiReadBSPVersionExCvi(AiUInt32 bModule, AiUInt32 aul_TcpVer[5], AiUInt32 aul_AslLcaVer[5], AiUInt32 aul_PciLcaVer[5],
                                                         AiUInt32 aul_IoLcaBiu1Ver[5], AiUInt32 aul_CoreLcaBiu1Ver[5], AiUInt32 aul_IoLcaBiu2Ver[5],
                                                         AiUInt32 aul_EndDecLcaBiu2Ver[5], AiUInt32 aul_IoLcaBiu3Ver[5], AiUInt32 aul_CoreLcaBiu3Ver[5],
                                                         AiUInt32 aul_IoLcaBiu4Ver[5], AiUInt32 aul_EndDecLcaBiu4Ver[5], AiUInt32 aul_FirmwareBiu1Ver[5],
                                                         AiUInt32 aul_FirmwareBiu2Ver[5], AiUInt32 aul_FirmwareBiu3Ver[5], AiUInt32 aul_FirmwareBiu4Ver[5],
                                                         AiUInt32 aul_TargetSWVer[5], AiUInt32 aul_SysDrvVer[5], AiUInt32 aul_DllVer[5], AiUInt32 aul_MonitorVer[5],
                                                         AiUInt32 *pul_BoardSerialNo, AiUInt8  *puc_BspCompatibility);
AI_LIB_FUNC M_RETVAL_TYPE AI_CALL_CONV M_APICMDSYSTAGDEF(VI_SESSION M_BOARDID, AiUInt8 biu,
                                                          AiUInt8 id, AiUInt8 con, AiUInt8 mode,
                                                          AiUInt16 xid_rtsa,
                                                          AiUInt16 fct,
                                                          AiUInt16 min,
                                                          AiUInt16 max,
                                                          AiUInt16 step,
                                                          AiUInt16 wpos,
                                                          AiUInt16 bpos
                                                          );
AI_LIB_FUNC M_RETVAL_TYPE AI_CALL_CONV M_APICMDSYSTAGCON(VI_SESSION M_BOARDID, AiUInt8 biu, AiUInt8 id, AiUInt8 con);

AI_LIB_FUNC M_RETVAL_TYPE AI_CALL_CONV M_APICMDTRACKDEF(VI_SESSION M_BOARDID, AiUInt8 biu, AiUInt8 uc_TrackId, AiUInt8 uc_BT,
                                                         AiUInt16 uw_XidRtSa, AiUInt8 uc_Mode, AiUInt16 uw_TrackStartPos,
                                                         AiUInt8 uc_TrackBPos, AiUInt8 uc_TrackBLen, AiUInt16 uw_TrackLen,
                                                         AiUInt16 uw_MultiplexedTrackNb, AiUInt16 uw_ContinousTrackNb,
                                                         AiUInt32 *pul_TrackBufferStartAddr);
AI_LIB_FUNC M_RETVAL_TYPE AI_CALL_CONV M_APICMDTRACKDEFEX(VI_SESSION M_BOARDID, AiUInt8 biu, AiUInt32 ul_TrackId,
                                                           AiUInt32 ul_BT,
                                                           AiUInt32 ul_XidRtSa, AiUInt32 ul_Mode, AiUInt32 ul_TrackStartPos,
                                                           AiUInt32 ul_TrackBPos, AiUInt32 ul_TrackBLen, AiUInt32 ul_TrackLen,
                                                           AiUInt32 ul_MultiplexedTrackNb, AiUInt32 ul_ContinousTrackNb,
                                                           AiInt32 l_Offset, AiUInt32 *pul_TrackBufferStartAddr);
AI_LIB_FUNC M_RETVAL_TYPE AI_CALL_CONV M_APICMDTRACKREAD(VI_SESSION M_BOARDID, AiUInt8 biu, AiUInt8 uc_TrackId, AiUInt16 uw_MultiplexedTrackIndex,
                                                          AiUInt8 uc_ReadMode, AiUInt8 *puc_DataValid, AiUInt16 *puw_TrackDataWords);
AI_LIB_FUNC M_RETVAL_TYPE AI_CALL_CONV M_APICMDTRACKREADEX(VI_SESSION M_BOARDID, AiUInt8 biu, AiUInt32 ul_TrackId,
                                                            AiUInt32 ul_MultiplexedTrackIndex,
                                                            AiUInt32 ul_ReadMode, AiUInt32 *pul_DataValid, AiUInt32 *pul_LastTT,
                                                            AiUInt16 *puw_TrackDataWords);
AI_LIB_FUNC M_RETVAL_TYPE AI_CALL_CONV M_APICMDTRACKSCAN(VI_SESSION M_BOARDID, AiUInt8 biu, AiUInt32 ul_TrackId,
                                                          AiUInt32 ul_ChunkNb, AiUInt32 ul_ChunkSize, 
                                                          AiUInt32 *pul_NumberOfReturnedStates,
                                                          AiUInt32 *pul_MoreData,
                                                          AiUInt32 *pul_ReturnedStates);
AI_LIB_FUNC M_RETVAL_TYPE AI_CALL_CONV M_APICMDTRACKPREALLOC(VI_SESSION M_BOARDID, AiUInt8 biu, AiUInt32 ul_TrackId,
                                                              AiUInt32 ul_PreAllocNb, AiUInt32 *pul_MuxStates, 
                                                              AiUInt32 *pul_TrackBufferStartAddr);

AI_LIB_FUNC M_RETVAL_TYPE AI_CALL_CONV M_APICMDREADDISCRETES(VI_SESSION M_BOARDID, AiUInt32 *pul_Value);
AI_LIB_FUNC M_RETVAL_TYPE AI_CALL_CONV M_APICMDREADDISCRETESCONFIG(VI_SESSION M_BOARDID, AiUInt32 *pul_Value);
AI_LIB_FUNC M_RETVAL_TYPE AI_CALL_CONV M_APICMDWRITEDISCRETES(VI_SESSION M_BOARDID, AiUInt32 ul_Mask, AiUInt32 ul_Value);
AI_LIB_FUNC M_RETVAL_TYPE AI_CALL_CONV M_APICMDSYSTRIGGEREDGEINPUTSET(VI_SESSION M_BOARDID, AiUInt8 biu, AiUInt32 edge_setup);
AI_LIB_FUNC M_RETVAL_TYPE AI_CALL_CONV M_APICMDSYSTRIGGEREDGEINPUTGET(VI_SESSION M_BOARDID, AiUInt8 biu, AiUInt32 * edge_setup);
AI_LIB_FUNC M_RETVAL_TYPE AI_CALL_CONV M_APICMDINITDISCRETES(VI_SESSION M_BOARDID, AiUInt32 ul_DiscreteSetup);
AI_LIB_FUNC M_RETVAL_TYPE AI_CALL_CONV M_APICMDSYSMALLOC(VI_SESSION M_BOARDID, AiUInt8 ucMemtype, AiUInt32 ulSize, AiUInt32 ulTag, AiUInt32 * pulOffset);
AI_LIB_FUNC M_RETVAL_TYPE AI_CALL_CONV M_APICMDSYSFREE(VI_SESSION M_BOARDID, AiUInt8 ucMemtype, AiUInt32 ulOffset, AiUInt32 ulTag);
AI_LIB_FUNC M_RETVAL_TYPE AI_CALL_CONV M_APICMDBUFDEF(VI_SESSION M_BOARDID, AiUInt8 biu, AiUInt8 bt, AiUInt16 hid,
                                                       AiUInt16 bid, AiUInt8 len, AiUInt16* data, AiUInt16* rid, AiUInt32* raddr);
AI_LIB_FUNC M_RETVAL_TYPE AI_CALL_CONV M_APICMDBUFREAD(VI_SESSION M_BOARDID, AiUInt8 biu, AiUInt8 bt, AiUInt16 hid,
                                                        AiUInt16 bid, AiUInt8 len, AiUInt16* data, AiUInt16* rid, AiUInt32* raddr);
AI_LIB_FUNC M_RETVAL_TYPE AI_CALL_CONV M_APICMDBUFWRITE(VI_SESSION M_BOARDID, AiUInt8 biu, AiUInt8 bt, AiUInt16 hid, AiUInt16 bid,
                                                         AiUInt8 d_pos, AiUInt8 b_pos, AiUInt8 b_len, AiUInt16 data);
AI_LIB_FUNC M_RETVAL_TYPE AI_CALL_CONV M_APICMDBUFWRITEBLOCK(VI_SESSION M_BOARDID, AiUInt8 biu, AiUInt8 bt, AiUInt16 hid,
                                                            AiUInt16 bid, AiUInt8 offset, AiUInt8 len, AiUInt16* data);
AI_LIB_FUNC M_RETVAL_TYPE AI_CALL_CONV M_APICMDBUFHOSTCONTROL(VI_SESSION M_BOARDID, AiUInt8 biu, AiUInt8 bt, AiUInt16 hid, AiUInt32 con);
AI_LIB_FUNC M_RETVAL_TYPE AI_CALL_CONV M_APICMDRAMWRITEDATASET(VI_SESSION M_BOARDID, AiUInt16 dsid, AiUInt16* data);
AI_LIB_FUNC M_RETVAL_TYPE AI_CALL_CONV M_APICMDRAMREADDATASET(VI_SESSION M_BOARDID, AiUInt16 dsid, AiUInt16* data);
AI_LIB_FUNC M_RETVAL_TYPE AI_CALL_CONV M_APICMDBUFC1760CON(VI_SESSION M_BOARDID, AiUInt8 biu,
                                                            AiUInt8  mode,
                                                            AiUInt16 buf_id,
                                                            AiUInt8  c01,
                                                            AiUInt8  c02[MAX_API_C1760_C02]);
AI_LIB_FUNC M_RETVAL_TYPE AI_CALL_CONV M_APIREADMEMDATA(VI_SESSION M_BOARDID, AiUInt8 memtype, AiUInt32 offset,
                                                         AiUInt8 width, AiAddr  data_p);
AI_LIB_FUNC M_RETVAL_TYPE AI_CALL_CONV M_APIWRITEMEMDATA(VI_SESSION M_BOARDID, AiUInt8 memtype, AiUInt32 offset,
                                                          AiUInt8 width, AiAddr  data_p);
AI_LIB_FUNC M_RETVAL_TYPE AI_CALL_CONV M_APIREADBLOCKMEMDATA(VI_SESSION M_BOARDID, AiUInt8 memtype, AiUInt32 offset, AiUInt8 width,
                                                              AiAddr  data_p, AiUInt32 size, AiUInt32 *pul_BytesRead);
AI_LIB_FUNC M_RETVAL_TYPE AI_CALL_CONV M_APIWRITEBLOCKMEMDATA(VI_SESSION M_BOARDID, AiUInt8 memtype, AiUInt32 offset,
                                                               AiUInt8 width, AiAddr  data_p, AiUInt32 size, AiUInt32 *pul_BytesWritten);
AI_LIB_FUNC M_RETVAL_TYPE AI_CALL_CONV M_APICMDFIFOWRITE(VI_SESSION M_BOARDID, AiUInt8 biu, AiUInt8 f_id,
                                                          AiUInt16 size, AiUInt16* data);
AI_LIB_FUNC M_RETVAL_TYPE AI_CALL_CONV M_APICMDFIFOREADSTATUS(VI_SESSION M_BOARDID, AiUInt8 biu,
                                                               AiUInt8 f_id, AiUInt16* status);

AI_LIB_FUNC M_RETVAL_TYPE AI_CALL_CONV M_APIBHMODIFY(VI_SESSION M_BOARDID, AiUInt32 ul_BHAddr, AiUInt32 ul_DataBufferStoreMode, AiUInt32 ul_BufferSizeMode,
                                                      AiUInt32 ul_BufferSize, AiUInt32 ul_BufferQueueMode, AiUInt32 ul_BufferQueueSize,
                                                      AiUInt32 ul_StatusQueueSize, AiUInt32 ul_EventQueueSize, AiUInt32 ul_CurrentBufferIndex);

AI_LIB_FUNC M_RETVAL_TYPE AI_CALL_CONV M_APICMDSYNCCOUNTERSET(VI_SESSION M_BOARDID, AiUInt8 uc_Biu, 
                                                               AiUInt32 ul_SyncCntVal);
AI_LIB_FUNC M_RETVAL_TYPE AI_CALL_CONV M_APICMDSYNCCOUNTERGET(VI_SESSION M_BOARDID, AiUInt8 uc_Biu, 
                                                               AiUInt32 *pul_SyncCntVal, AiUInt32 *pul_SyncCntInit, AiUInt32 *pul_SyncCntInitLow, AiUInt32 *pul_SyncCntInitHigh);
AI_LIB_FUNC M_RETVAL_TYPE AI_CALL_CONV M_APICMDSYSPXICON(VI_SESSION M_BOARDID, 
                                                          AiUInt32 ul_Mode, AiUInt32 ul_TrgSource, AiUInt32 ul_TrgDest, AiUInt32 ul_TTClear);

AI_LIB_FUNC M_RETVAL_TYPE AI_CALL_CONV M_APICMDSYSPXIGEOGRAPHICALADDRESSGET(VI_SESSION M_BOARDID, AiUInt32 *pxiGeographicalAddress);

AI_LIB_FUNC M_RETVAL_TYPE AI_CALL_CONV M_APICMDSYSGETMEMLOC(VI_SESSION M_BOARDID, 
                                                             AiUInt8 uc_Biu, AiUInt32 ul_Mode, AiUInt32 ul_Id, AiUInt32 *pul_MemType, AiUInt32 *pul_Offset);
AI_LIB_FUNC M_RETVAL_TYPE AI_CALL_CONV M_APICMDBCINI(VI_SESSION M_BOARDID, AiUInt8 biu, AiUInt8 retr, AiUInt8 svrq, AiUInt8 tbm, AiUInt8 gsb);
AI_LIB_FUNC M_RETVAL_TYPE AI_CALL_CONV M_APICMDBCBHDEF(VI_SESSION M_BOARDID, AiUInt8 biu, AiUInt16 hid,
                                                        AiUInt16 bid, AiUInt16 sid, AiUInt16 eid, AiUInt8 qsize, AiUInt8 bqm, AiUInt8 bsm,
                                                        AiUInt8 sqm, AiUInt8 eqm, AiUInt8 res,
                                                        AiUInt16* bh_bid,
                                                        AiUInt16* bh_sid,
                                                        AiUInt16* bh_eid,
                                                        AiUInt16* bh_nbufs,
                                                        AiUInt32* bh_hid_addr,
                                                        AiUInt32* bh_bq_addr,
                                                        AiUInt32* bh_sq_addr,
                                                        AiUInt32* bh_eq_addr);
AI_LIB_FUNC M_RETVAL_TYPE AI_CALL_CONV M_APICMDBCBHREAD(VI_SESSION M_BOARDID, AiUInt8 biu, AiUInt16 hid,
                                                         AiUInt16* bh_bid,
                                                         AiUInt16* bh_sid,
                                                         AiUInt16* bh_eid,
                                                         AiUInt16* bh_nbufs,
                                                         AiUInt32* bh_hid_addr,
                                                         AiUInt32* bh_bq_addr,
                                                         AiUInt32* bh_sq_addr,
                                                         AiUInt32* bh_eq_addr);

AI_LIB_FUNC M_RETVAL_TYPE AI_CALL_CONV M_APICMDBCXFERDEF(VI_SESSION M_BOARDID, AiUInt8 biu,
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
                                                          AiUInt8 sxh,
                                                          AiUInt8 rsp,
                                                          AiUInt8 gap_mode,
                                                          AiUInt16 swxm,
                                                          AiUInt8 err_type,
                                                          AiUInt8 err_sync,
                                                          AiUInt8 err_contig,
                                                          AiUInt8 err_padding1,
                                                          AiUInt32 err_spec,
                                                          AiUInt16 gap,
                                                          AiUInt32* desc_addr);

AI_LIB_FUNC M_RETVAL_TYPE AI_CALL_CONV M_APICMDBCXFERDEFERR(VI_SESSION M_BOARDID, AiUInt8 biu,
                                                             AiUInt16 xid,
                                                             AiUInt8 err_type,
                                                             AiUInt8 err_sync,
                                                             AiUInt8 err_contig,
                                                             AiUInt8 err_padding1,
                                                             AiUInt32 err_spec );
AI_LIB_FUNC M_RETVAL_TYPE AI_CALL_CONV M_APICMDBCGETXFERDEF(VI_SESSION M_BOARDID, AiUInt8 biu,
                                                             AiUInt16 uw_XferId,
                                                             AiUInt16 *xid,
                                                             AiUInt16 *hid,
                                                             AiUInt8 *type,
                                                             AiUInt8 *chn,
                                                             AiUInt8 *xmt_rt,
                                                             AiUInt8 *rcv_rt,
                                                             AiUInt8 *xmt_sa,
                                                             AiUInt8 *rcv_sa,
                                                             AiUInt8 *wcnt,
                                                             AiUInt8 *tic,
                                                             AiUInt8 *hlt,
                                                             AiUInt8 *rte,
                                                             AiUInt8 *res,
                                                             AiUInt8 *sxh,
                                                             AiUInt8 *rsp,
                                                             AiUInt8 *gap_mode,
                                                             AiUInt16 *swxm,
                                                             AiUInt8 *err_type,
                                                             AiUInt8 *err_sync,
                                                             AiUInt8 *err_contig,
                                                             AiUInt8 *err_padding1,
                                                             AiUInt32 *err_spec,
                                                             AiUInt16 *gap);
AI_LIB_FUNC M_RETVAL_TYPE AI_CALL_CONV M_APICMDBCXFERCTRL(VI_SESSION M_BOARDID, AiUInt8 biu, AiUInt16 xid, AiUInt8 mode);

AI_LIB_FUNC M_RETVAL_TYPE AI_CALL_CONV M_APICMDBCXFERDESCGET(VI_SESSION M_BOARDID, AiUInt8 biu, AiUInt16 xid, AiUInt32 ulMode, AiUInt32 * pulValue1, AiUInt32 * pulValue2, AiUInt32 * pulValue3, AiUInt32 * pulValue4 );

AI_LIB_FUNC M_RETVAL_TYPE AI_CALL_CONV M_APICMDBCXFERDESCMOD(VI_SESSION M_BOARDID, AiUInt8 biu, AiUInt16 xid, AiUInt32 ulField, AiUInt32 ulValue1, AiUInt32 ulValue2, AiUInt32 ulValue3, AiUInt32 ulValue4 );
AI_LIB_FUNC M_RETVAL_TYPE AI_CALL_CONV M_APICMDBCDYTAGDEF(VI_SESSION M_BOARDID, AiUInt8 biu, AiUInt8 con,
                                                           AiUInt16 bc_hid,
                                                           AiUInt16 mode,
                                                           AiUInt16 tag_fct[4],
                                                           AiUInt16 min[4],
                                                           AiUInt16 max[4],
                                                           AiUInt16 step[4],
                                                           AiUInt16 wpos[4]
);
AI_LIB_FUNC M_RETVAL_TYPE AI_CALL_CONV M_APICMDBCGETDYTAGDEF(VI_SESSION M_BOARDID, AiUInt8 biu, 
                                                              AiUInt16 bc_hid,
                                                              AiUInt16 *mode,
                                                              AiUInt16 tag_fct[4],
                                                              AiUInt16 min[4],
                                                              AiUInt16 max[4],
                                                              AiUInt16 step[4],
                                                              AiUInt16 wpos[4]
);
AI_LIB_FUNC M_RETVAL_TYPE AI_CALL_CONV M_APICMDBCFRAMEDEF(VI_SESSION M_BOARDID, AiUInt8 biu,
                                                           AiUInt8 id,
                                                           AiUInt8 cnt,
                                                           AiUInt16 instr[MAX_API_BC_XFRAME],
                                                           AiUInt16 xid[MAX_API_BC_XFRAME]);

AI_LIB_FUNC M_RETVAL_TYPE AI_CALL_CONV M_APICMDBCGETFRAMEDEF(VI_SESSION M_BOARDID, AiUInt8 biu,
                                                              AiUInt8 id,
                                                              AiUInt8 *cnt,
                                                              AiUInt16 *instr,
                                                              AiUInt16 *xid);

AI_LIB_FUNC M_RETVAL_TYPE AI_CALL_CONV M_APICMDBCMFRAMEDEF(VI_SESSION M_BOARDID, AiUInt8 biu,
                                                            AiUInt8 cnt,
                                                            AiUInt8 fid[MAX_API_BC_MFRAME]);

AI_LIB_FUNC M_RETVAL_TYPE AI_CALL_CONV M_APICMDBCGETMAJFRAMEDEF(VI_SESSION M_BOARDID, AiUInt8 biu,
                                                                 AiUInt8 *cnt, AiUInt8 *fid);


AI_LIB_FUNC M_RETVAL_TYPE AI_CALL_CONV M_APICMDBCMFRAMEDEFEX(VI_SESSION M_BOARDID, AiUInt8 biu,
                                                              AiUInt8 cnt,
                                                              AiUInt8 fid[MAX_API_BC_MFRAME]);


AI_LIB_FUNC M_RETVAL_TYPE AI_CALL_CONV M_APICMDBCSTART(VI_SESSION M_BOARDID, AiUInt8 biu, AiUInt8 smod, AiUInt32 cnt, AiFloat frame_time,
                                                        AiUInt32 saddr, AiUInt32 *maj_frame_addr, AiUInt32 *min_frame_addr);

AI_LIB_FUNC M_RETVAL_TYPE AI_CALL_CONV M_APICMDBCHALT(VI_SESSION M_BOARDID, AiUInt8 biu);
AI_LIB_FUNC M_RETVAL_TYPE AI_CALL_CONV M_APICMDBCSTATUSREAD(VI_SESSION M_BOARDID, AiUInt8 biu,
                                                             AiUInt8*  status,
                                                             AiUInt16* hxfer,
                                                             AiUInt32* glb_msg_cnt,
                                                             AiUInt32* glb_err_cnt,
                                                             AiUInt32* hip,
                                                             AiUInt32* mfc);

AI_LIB_FUNC M_RETVAL_TYPE AI_CALL_CONV M_APICMDBCXFERREAD(VI_SESSION M_BOARDID, AiUInt8 biu, AiUInt16 xid, AiUInt16 clr,
                                                           AiUInt16* cw1,
                                                           AiUInt16* st1,
                                                           AiUInt16* cw2,
                                                           AiUInt16* st2,
                                                           AiUInt16* bid,
                                                           AiUInt16* brw,
                                                           AiUInt32* bufp,
                                                           AiUInt32* ttag,
                                                           AiUInt32* msg_cnt,
                                                           AiUInt32* err_cnt,
                                                           AiUInt32* lvw,
                                                           AiUInt32* srvreq_cnt);
AI_LIB_FUNC M_RETVAL_TYPE AI_CALL_CONV M_APICMDBCXFERREADEX(VI_SESSION M_BOARDID, 
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
AI_LIB_FUNC M_RETVAL_TYPE AI_CALL_CONV M_APICMDBCACYCPREP(VI_SESSION M_BOARDID, AiUInt8 biu,
                                                           AiUInt8 cnt,
                                                           AiUInt16 instr[MAX_API_BC_XACYC],
                                                           AiUInt16 xid[MAX_API_BC_XACYC]);
AI_LIB_FUNC M_RETVAL_TYPE AI_CALL_CONV M_APICMDBCACYCSEND(VI_SESSION M_BOARDID, AiUInt8 biu, AiUInt8 mode, AiUInt32 timetag_high, AiUInt32 timetag_low );
AI_LIB_FUNC M_RETVAL_TYPE AI_CALL_CONV M_APICMDBCASSIGNFIFO(VI_SESSION M_BOARDID, AiUInt8 biu, AiUInt8 con,
                                                             AiUInt8 f_id, AiUInt16 xid);
AI_LIB_FUNC M_RETVAL_TYPE AI_CALL_CONV M_APICMDBCSRVREQVECSTATUS(VI_SESSION M_BOARDID, AiUInt8 uc_Biu, AiUInt8 uc_RtAddr,
                                                                  AiUInt16* uw_XferId, AiUInt16* uw_LastVecWord,
                                                                  AiUInt32* ul_SrvReqCnt);
AI_LIB_FUNC M_RETVAL_TYPE AI_CALL_CONV M_APICMDBCSRVREQVECCON(VI_SESSION M_BOARDID, AiUInt8 uc_Biu, AiUInt8 uc_RtAddr,
                                                               AiUInt8 uc_SubAddress, AiUInt32 ul_Reserved1,
                                                               AiUInt32 ul_Reserved2, AiUInt32 ul_Reserved3);
AI_LIB_FUNC M_RETVAL_TYPE AI_CALL_CONV M_APICMDBCGETXFERBHINFO(VI_SESSION M_BOARDID, AiUInt8 biu, AiUInt32 XferId,
                                                                AiUInt32 *BufHeaderIndex, AiUInt32 *BufHeaderAddr);

AI_LIB_FUNC M_RETVAL_TYPE AI_CALL_CONV M_APICMDBCINSTRTBLINI(VI_SESSION M_BOARDID, AiUInt8 uc_Biu);
AI_LIB_FUNC M_RETVAL_TYPE AI_CALL_CONV M_APICMDBCINSTRTBLGETADDRFROMLABEL(VI_SESSION M_BOARDID, AiUInt8 uc_Biu,
                                                                           AiUInt32 label, AiUInt32 cnt, 
                                                                           AiUInt16 *tbl_label, AiUInt8  *tbl_op,
                                                                           AiUInt8  *tbl_res,   AiUInt32 *tbl_par1,  
                                                                           AiUInt32 *tbl_par2,  AiUInt32 *tbl_laddr,
                                                                           AiUInt32 *raddr, AiUInt32 *line);
AI_LIB_FUNC M_RETVAL_TYPE AI_CALL_CONV M_APICMDBCINSTRTBLGEN(VI_SESSION M_BOARDID, AiUInt8 uc_Biu,
                                                              AiUInt8 mode, AiUInt32 cnt, 
                                                              AiUInt32 dest_cnt, AiUInt32 dest_offset, 
                                                              AiUInt16 *tbl_label, AiUInt8  *tbl_op,
                                                              AiUInt8  *tbl_res,   AiUInt32 *tbl_par1,
                                                              AiUInt32 *tbl_par2,  AiUInt32 *tbl_laddr,
                                                              AiUInt32 *ctbl, 
                                                              AiUInt32 *err_line, AiUInt8 *status);

AI_LIB_FUNC M_RETVAL_TYPE AI_CALL_CONV M_APICMDBCMODECTRL(VI_SESSION M_BOARDID, AiUInt8 uc_Biu,
                                                           AiUInt32 ul_BcMode,
                                                           AiUInt32 ul_Ctrl,
                                                           AiUInt32 ul_Param1,
                                                           AiUInt32 ul_Param2,
                                                           AiUInt32 ul_Param3);

AI_LIB_FUNC M_RETVAL_TYPE AI_CALL_CONV M_APICMDRTINI(VI_SESSION M_BOARDID, AiUInt8 biu, AiUInt8 rt_addr,
                                                      AiUInt8 con, AiUInt8 bus, AiFloat resp_time, AiUInt16 nxw);
AI_LIB_FUNC M_RETVAL_TYPE AI_CALL_CONV M_APICMDRTGETSIMULATIONINFO(VI_SESSION M_BOARDID, AiUInt8 biu, AiUInt8 rt_addr,
                                                                    AiUInt8 *puc_Mode, AiUInt32 *pul_RxSA, AiUInt32 *pul_TxSA, AiUInt32 *pul_RxMC,
                                                                    AiUInt32 *pul_TxMC, AiUInt32 aul_HSRxMID[8], AiUInt32 aul_HSTxMID[8], AiUInt32 ul_HSMC);
AI_LIB_FUNC M_RETVAL_TYPE AI_CALL_CONV M_APICMDRTENADIS(VI_SESSION M_BOARDID, AiUInt8 biu, AiUInt8 rt_addr,
                                                         AiUInt8 con);
AI_LIB_FUNC M_RETVAL_TYPE AI_CALL_CONV M_APICMDRTRESPTIME(VI_SESSION M_BOARDID, AiUInt8 biu, AiUInt8 rt_addr, AiFloat resp_time);
AI_LIB_FUNC M_RETVAL_TYPE AI_CALL_CONV M_APICMDRTRESPTIMEGET(VI_SESSION M_BOARDID, AiUInt8 biu, AiUInt8 rt_addr, AiFloat *pResp_time);
AI_LIB_FUNC M_RETVAL_TYPE AI_CALL_CONV M_APICMDRTNXW(VI_SESSION M_BOARDID, AiUInt8 biu, AiUInt8 rt_addr, AiUInt16 nxw);
AI_LIB_FUNC M_RETVAL_TYPE AI_CALL_CONV M_APICMDRTLCW(VI_SESSION M_BOARDID, AiUInt8 biu, AiUInt8 rt_addr, AiUInt16 lcw);
AI_LIB_FUNC M_RETVAL_TYPE AI_CALL_CONV M_APICMDRTLSW(VI_SESSION M_BOARDID, AiUInt8 biu, AiUInt8 rt_addr, AiUInt16 lsw);
AI_LIB_FUNC M_RETVAL_TYPE AI_CALL_CONV M_APICMDRTBHDEF(VI_SESSION M_BOARDID, AiUInt8 biu, AiUInt16 hid,
                                                        AiUInt16 bid, AiUInt16 sid, AiUInt16 eid, AiUInt8 qsize, AiUInt8 bqm, AiUInt8 bsm,
                                                        AiUInt8 sqm, AiUInt8 eqm, AiUInt8 res,
                                                        AiUInt16* bh_bid,
                                                        AiUInt16* bh_sid,
                                                        AiUInt16* bh_eid,
                                                        AiUInt16* bh_nbufs,
                                                        AiUInt32* bh_hid_addr,
                                                        AiUInt32* bh_bq_addr,
                                                        AiUInt32* bh_sq_addr,
                                                        AiUInt32* bh_eq_addr);
AI_LIB_FUNC M_RETVAL_TYPE AI_CALL_CONV M_APICMDRTBHREAD(VI_SESSION M_BOARDID, AiUInt8 biu, AiUInt16 hid,
                                                         AiUInt16* bh_bid,
                                                         AiUInt16* bh_sid,
                                                         AiUInt16* bh_eid,
                                                         AiUInt16* bh_nbufs,
                                                         AiUInt32* bh_hid_addr,
                                                         AiUInt32* bh_bq_addr,
                                                         AiUInt32* bh_sq_addr,
                                                         AiUInt32* bh_eq_addr);
AI_LIB_FUNC M_RETVAL_TYPE AI_CALL_CONV M_APICMDRTSACON(VI_SESSION M_BOARDID, AiUInt8 biu, AiUInt8 rt_addr,
                                                        AiUInt8 sa, AiUInt16 hid, AiUInt8 sa_type, AiUInt8 con,
                                                        AiUInt8 rmod, AiUInt8 smod, AiUInt16 swm);
AI_LIB_FUNC M_RETVAL_TYPE AI_CALL_CONV M_APICMDRTSACONERR(VI_SESSION M_BOARDID, AiUInt8 biu, AiUInt8 rt_addr,
                                                           AiUInt8 sa, AiUInt8 sa_type,
                                                           AiUInt8 type,
                                                           AiUInt8 sync,
                                                           AiUInt8 config,
                                                           AiUInt8 padding1,
                                                           AiUInt32 err_spec
                                                           );
AI_LIB_FUNC M_RETVAL_TYPE AI_CALL_CONV M_APICMDRTGETSACONERR(VI_SESSION M_BOARDID, AiUInt8 biu, AiUInt8 rt_addr,
                                                              AiUInt8 sa, AiUInt8 sa_type,
                                                              AiUInt8 *type,
                                                              AiUInt8 *sync,
                                                              AiUInt8 *config,
                                                              AiUInt8 *padding1,
                                                              AiUInt32 *err_spec
                                                              );

AI_LIB_FUNC M_RETVAL_TYPE AI_CALL_CONV M_APICMDRTGETSABHINFO(VI_SESSION M_BOARDID, AiUInt8 biu, AiUInt8 rt_addr,
                                                              AiUInt8 type,    AiUInt8 SA_MC, AiUInt32 *pul_BufHeaderIndex,    AiUInt32 *pul_BufHeaderAddr);

AI_LIB_FUNC M_RETVAL_TYPE AI_CALL_CONV M_APICMDRTDYTAGDEF(VI_SESSION M_BOARDID, AiUInt8 biu, AiUInt8 con,
                                                           AiUInt16 rt_hid, AiUInt16 mode,
                                                           AiUInt16 tag_fct[4],
                                                           AiUInt16 min[4],
                                                           AiUInt16 max[4],
                                                           AiUInt16 step[4],
                                                           AiUInt16 wpos[4]
);

AI_LIB_FUNC M_RETVAL_TYPE AI_CALL_CONV M_APICMDRTGETDYTAGDEF(VI_SESSION M_BOARDID, AiUInt8 biu, AiUInt16 rt_hid, 
                                                              AiUInt16 *mode,
                                                              AiUInt16 tag_fct[4],
                                                              AiUInt16 min[4],
                                                              AiUInt16 max[4],
                                                              AiUInt16 step[4],
                                                              AiUInt16 wpos[4]);
AI_LIB_FUNC M_RETVAL_TYPE AI_CALL_CONV M_APICMDRTSTART(VI_SESSION M_BOARDID, AiUInt8 biu);
AI_LIB_FUNC M_RETVAL_TYPE AI_CALL_CONV M_APICMDRTHALT(VI_SESSION M_BOARDID, AiUInt8 biu);
AI_LIB_FUNC M_RETVAL_TYPE AI_CALL_CONV M_APICMDRTSTATUSREAD(VI_SESSION M_BOARDID, AiUInt8 biu,
                                                             AiUInt8*  status,
                                                             AiUInt32* glb_msg_cnt,
                                                             AiUInt32* glb_err_cnt);
AI_LIB_FUNC M_RETVAL_TYPE AI_CALL_CONV M_APICMDRTMSGREAD(VI_SESSION M_BOARDID, AiUInt8 biu, AiUInt8 rt_addr,
                                                          AiUInt16* nxw,
                                                          AiUInt16* lsw,
                                                          AiUInt16* lcw,
                                                          AiUInt32* msg_cnt,
                                                          AiUInt32* err_cnt);
AI_LIB_FUNC M_RETVAL_TYPE AI_CALL_CONV M_APICMDRTSAMSGREADEX(VI_SESSION M_BOARDID, 
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

AI_LIB_FUNC M_RETVAL_TYPE AI_CALL_CONV M_APICMDRTMSGREADALL(VI_SESSION M_BOARDID, AiUInt8 biu,
                                                             AiUInt32 rt_msg[32],
                                                             AiUInt32 rt_err[32]);

AI_LIB_FUNC M_RETVAL_TYPE AI_CALL_CONV M_APICMDRTSAMSGREAD(VI_SESSION M_BOARDID, AiUInt8 biu, AiUInt8 rt_addr,
                                                            AiUInt8 sa, AiUInt8 sa_type, AiUInt8 clr,
                                                            AiUInt16* bid,
                                                            AiUInt16* trw,
                                                            AiUInt16* lcw,
                                                            AiUInt16* lsw,
                                                            AiUInt32* bufp,
                                                            AiUInt32* ttag);
AI_LIB_FUNC M_RETVAL_TYPE AI_CALL_CONV M_APICMDRTGLOBALCON(VI_SESSION M_BOARDID, AiUInt8 biu, AiUInt16 count,
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
AI_LIB_FUNC M_RETVAL_TYPE AI_CALL_CONV M_APICMDRTSAASSIGNFIFO(VI_SESSION M_BOARDID, AiUInt8 biu, AiUInt8 con,
                                                               AiUInt8 f_id, AiUInt8 rt, AiUInt8 sa);
AI_LIB_FUNC M_RETVAL_TYPE AI_CALL_CONV M_APICMDRTMODECTRL(VI_SESSION M_BOARDID, AiUInt8 uc_Biu,
                                                           AiUInt32 ul_RtMode,
                                                           AiUInt32 ul_Ctrl,
                                                           AiUInt32 ul_Param1,
                                                           AiUInt32 ul_Param2,
                                                           AiUInt32 ul_Param3);
AI_LIB_FUNC M_RETVAL_TYPE AI_CALL_CONV M_APICMDBMINI(VI_SESSION M_BOARDID, AiUInt8 biu);
AI_LIB_FUNC M_RETVAL_TYPE AI_CALL_CONV M_APICMDBMINTRMODE(VI_SESSION M_BOARDID, AiUInt8 biu, AiUInt8 imod, AiUInt8 smod, AiUInt8 res);
AI_LIB_FUNC M_RETVAL_TYPE AI_CALL_CONV M_APICMDBMCAPMODE(VI_SESSION M_BOARDID, AiUInt8 biu,
                                                          AiUInt8  cap_mode,
                                                          AiUInt32 cap_tat,
                                                          AiUInt16 cap_mcc,
                                                          AiUInt16 cap_fsize);
AI_LIB_FUNC M_RETVAL_TYPE AI_CALL_CONV M_APICMDBMSWXMINI(VI_SESSION M_BOARDID, AiUInt8 biu, AiUInt16 swxm);
AI_LIB_FUNC M_RETVAL_TYPE AI_CALL_CONV M_APICMDBMSTART(VI_SESSION M_BOARDID, AiUInt8 biu);
AI_LIB_FUNC M_RETVAL_TYPE AI_CALL_CONV M_APICMDBMHALT(VI_SESSION M_BOARDID, AiUInt8 biu);
AI_LIB_FUNC M_RETVAL_TYPE AI_CALL_CONV M_APICMDBMSTATUSREAD(VI_SESSION M_BOARDID, AiUInt8 biu,
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
AI_LIB_FUNC M_RETVAL_TYPE AI_CALL_CONV M_APICMDBMACTREAD(VI_SESSION M_BOARDID, AiUInt8 biu, AiUInt16 entry_no,
                                                          AiUInt8*  fnd,
                                                          AiUInt8*  rt,
                                                          AiUInt8*  tr,
                                                          AiUInt8*  sa,
                                                          AiUInt32* cc,
                                                          AiUInt32* ec,
                                                          AiUInt32* et);
AI_LIB_FUNC M_RETVAL_TYPE AI_CALL_CONV  M_APICMDBMSTACKPREAD(VI_SESSION M_BOARDID, AiUInt8 biu,
                                                              AiUInt8*  status,
                                                              AiUInt32* stp,
                                                              AiUInt32* ctp,
                                                              AiUInt32* etp);
AI_LIB_FUNC M_RETVAL_TYPE AI_CALL_CONV M_APICMDBMSTACKENTRYREAD(VI_SESSION M_BOARDID, AiUInt8 biu, AiUInt8 ptype,
                                                                 AiUInt8 sgn, AiUInt32 offset,
                                                                 AiUInt32* entry);
AI_LIB_FUNC M_RETVAL_TYPE AI_CALL_CONV M_APICMDBMSTACKENTRYFIND(VI_SESSION M_BOARDID, AiUInt8 biu, AiUInt8 ptype,
                                                                 AiUInt8 sgn, AiUInt32 offset,
                                                                 AiUInt16 fspec, AiUInt16 fdata, AiUInt16 fmask,
                                                                 AiUInt8*  efnd,
                                                                 AiUInt32* eptr,
                                                                 AiUInt32* entry);
AI_LIB_FUNC M_RETVAL_TYPE AI_CALL_CONV M_APICMDBMRECSTATUSREAD(VI_SESSION M_BOARDID, AiUInt8 biu,
                                                                AiUInt8*  status,
                                                                AiUInt32* hfi_cnt,
                                                                AiUInt32* saddr,
                                                                AiUInt32* size);
AI_LIB_FUNC M_RETVAL_TYPE AI_CALL_CONV M_APICMDBMCOPYRECDAT(VI_SESSION M_BOARDID, AiUInt8 biu,
                                                             AiUInt8  status,
                                                             AiUInt32 hfi_cnt,
                                                             AiUInt32 saddr,
                                                             AiUInt32 size);
AI_LIB_FUNC M_RETVAL_TYPE AI_CALL_CONV M_APICMDBMFILTERINI(VI_SESSION M_BOARDID, AiUInt8 biu, AiUInt8 rt_addr,
                                                            AiUInt32 rx_sa, AiUInt32 tx_sa, AiUInt32 rx_mc, AiUInt32 tx_mc);
AI_LIB_FUNC M_RETVAL_TYPE AI_CALL_CONV M_APICMDBMILLEGALINI(VI_SESSION M_BOARDID, AiUInt8 biu, AiUInt8 rt_addr,
                                                             AiUInt32 rx_sa, AiUInt32 tx_sa, AiUInt32 rx_mc, AiUInt32 tx_mc);
AI_LIB_FUNC M_RETVAL_TYPE AI_CALL_CONV M_APICMDBMFTWINI(VI_SESSION M_BOARDID, AiUInt8 biu, AiUInt8 con, AiUInt8 htm, AiUInt8 htc, AiUInt8 stm, AiUInt8 stc);
AI_LIB_FUNC M_RETVAL_TYPE AI_CALL_CONV M_APICMDBMTIWINI(VI_SESSION M_BOARDID, AiUInt8 biu, AiUInt8 con, AiUInt8 eti, AiUInt8 aei, AiUInt8 ti0, AiUInt8 ti1);
AI_LIB_FUNC M_RETVAL_TYPE AI_CALL_CONV M_APICMDBMTCIINI(VI_SESSION M_BOARDID, AiUInt8 biu, AiUInt8 rt, AiUInt8 sa,
                                                         AiUInt8 sa_type, AiUInt8 tagm, AiUInt8 tfct, AiUInt8 tid);
AI_LIB_FUNC M_RETVAL_TYPE AI_CALL_CONV M_APICMDBMTCBINI(VI_SESSION M_BOARDID, AiUInt8 biu, AiUInt8 tid, AiUInt8 ten,
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
AI_LIB_FUNC M_RETVAL_TYPE AI_CALL_CONV M_APICMDBMRTACTREAD(VI_SESSION M_BOARDID, AiUInt8 biu, AiUInt8 rt,
                                                            AiUInt32* mc,
                                                            AiUInt32* ec,
                                                            AiUInt32* et);
AI_LIB_FUNC M_RETVAL_TYPE AI_CALL_CONV M_APICMDBMRTSAACTREAD(VI_SESSION M_BOARDID, AiUInt8 biu, AiUInt8 rt, AiUInt8 sa, AiUInt8 sa_type,
                                                              AiUInt32* mc,
                                                              AiUInt32* ec,
                                                              AiUInt32* et);
AI_LIB_FUNC M_RETVAL_TYPE AI_CALL_CONV M_APICMDBMINIMSGFLTREC(VI_SESSION M_BOARDID, AiUInt8 biu, AiUInt8 cnt,
                                                               AiUInt16 cw[40],
                                                               AiUInt8 pos1[40],
                                                               AiUInt8 pos2[40]);

AI_LIB_FUNC M_RETVAL_TYPE AI_CALL_CONV M_APICMDBMREADMSGFLTREC(VI_SESSION M_BOARDID, AiUInt8 biu, AiUInt8 mode,
                                                                AiUInt8 con, AiUInt32 max_size, AiUInt16 max_msg,
                                                                AiAddr lpBuf, AiUInt8* ovfl, AiUInt32* lBytesRead);
AI_LIB_FUNC M_RETVAL_TYPE AI_CALL_CONV M_APICMDBMDYTAGMONDEF(VI_SESSION M_BOARDID, AiUInt8 biu, AiUInt8 uc_Id,
                                                              AiUInt8 uc_Con, AiUInt8 uc_RtAddr, AiUInt8 uc_SubAddrMsgId, AiUInt8 uc_SubAddrMsgIdType,
                                                              AiUInt16 uw_DytagType, AiUInt16 uw_DytagWPos, AiUInt16 uw_DytagBPos,
                                                              AiUInt16 uw_DytagBLen, AiUInt16 uw_DytagStep);

AI_LIB_FUNC M_RETVAL_TYPE AI_CALL_CONV M_APICMDBMDYTAGMONREAD(VI_SESSION M_BOARDID, AiUInt8 biu, AiUInt8 uc_Id,
                                                               AiUInt32 *ul_Stale, AiUInt32 *ul_Bad, AiUInt32 *ul_Good);
AI_LIB_FUNC M_RETVAL_TYPE AI_CALL_CONV M_APICMDQUEUEINI(VI_SESSION M_BOARDID, AiUInt8 biu);
AI_LIB_FUNC M_RETVAL_TYPE AI_CALL_CONV M_APICMDQUEUEREAD(VI_SESSION M_BOARDID, AiUInt8 biu,
                                                          AiUInt8*  rt_addr,
                                                          AiUInt8*  sa_mc,
                                                          AiUInt8*  sa_type,
                                                          AiUInt8*  rt_addr2,
                                                          AiUInt8*  sa_mc2,
                                                          AiUInt8*  sa_type2,
                                                          AiUInt8*  word_cnt,
                                                          AiUInt8*  rbf_trw,
                                                          AiUInt16* msg_trw,
                                                          AiUInt16* status_word1,
                                                          AiUInt16* status_word2,
                                                          AiUInt16 buffer[32],
                                                          AiUInt32* ttag);
AI_LIB_FUNC M_RETVAL_TYPE AI_CALL_CONV M_APICMDQUEUEFLUSH(VI_SESSION M_BOARDID, AiUInt8 biu);
AI_LIB_FUNC M_RETVAL_TYPE AI_CALL_CONV M_APICMDQUEUESTART(VI_SESSION M_BOARDID, AiUInt8 biu);
AI_LIB_FUNC M_RETVAL_TYPE AI_CALL_CONV M_APICMDQUEUEHALT(VI_SESSION M_BOARDID, AiUInt8 biu);

AI_LIB_FUNC M_RETVAL_TYPE AI_CALL_CONV M_APICMDDATAQUEUECLOSE(VI_SESSION M_BOARDID, AiUInt32 id);

AI_LIB_FUNC M_RETVAL_TYPE AI_CALL_CONV M_APICMDDATAQUEUECONTROL(VI_SESSION M_BOARDID, AiUInt32 id, AiUInt32 mode );

AI_LIB_FUNC M_RETVAL_TYPE AI_CALL_CONV M_APICMDDATAQUEUEOPEN(VI_SESSION M_BOARDID, AiUInt32 id, AiUInt32 * queue_size);

AI_LIB_FUNC M_RETVAL_TYPE AI_CALL_CONV M_APICMDDATAQUEUEREAD(VI_SESSION M_BOARDID,
                                                                AiUInt32 id,
                                                                void * buffer,
                                                                AiUInt32 bytes_to_read,
                                                                AiUInt32 *status,
                                                                AiUInt32 *bytes_transfered,
                                                                AiUInt32 *bytes_in_queue,
                                                                AiUInt64 *total_bytes_transfered);

AI_LIB_FUNC M_RETVAL_TYPE AI_CALL_CONV M_APICMDSCOPESETUP(VI_SESSION M_BOARDID, AiUInt32 ul_CouplingPri, AiUInt32 ul_CouplingSec, 
                                                           AiUInt32 ul_SamplingRate, AiUInt32 ul_CaptureMode, AiUInt32 ul_OperationMode, 
                                                           AiUInt32 ul_SingleShotBuf);

AI_LIB_FUNC M_RETVAL_TYPE AI_CALL_CONV M_APICMDSCOPETRIGGERDEF(VI_SESSION M_BOARDID, AiUInt32 ul_TrgMode, AiUInt32 ul_TrgSrc, AiUInt32 ul_TrgValue, 
                                                                AiUInt32 ul_TrgNbSamples, AiUInt32 ul_TrgFlags, AiUInt32 ul_TrgDelay, AiUInt32 ul_TrgTbt);

AI_LIB_FUNC M_RETVAL_TYPE AI_CALL_CONV M_APICMDSCOPECALIBRATE(VI_SESSION M_BOARDID, AiUInt8 ucMode, AiUInt32 *pul_OffsetA, AiUInt32 *pul_OffsetB,
                                                               AiUInt32 *pul_OffsetA100, AiUInt32 *pul_OffsetB100, AiUInt32 *pul_OffsetA100Sec, 
                                                               AiUInt32 *pul_OffsetB100Sec);

AI_LIB_FUNC M_RETVAL_TYPE AI_CALL_CONV M_APICMDSCOPESTART(VI_SESSION M_BOARDID);

AI_LIB_FUNC M_RETVAL_TYPE AI_CALL_CONV M_APICMDCALSIGCON(VI_SESSION M_BOARDID, AiUInt8 biu, AiUInt8 bus, AiUInt8 con);

AI_LIB_FUNC M_RETVAL_TYPE AI_CALL_CONV M_APICMDCALCPLCON(VI_SESSION M_BOARDID, AiUInt8 biu, AiUInt8 bus, AiUInt8 cpl);

AI_LIB_FUNC M_RETVAL_TYPE AI_CALL_CONV M_APICMDCALXMTCON(VI_SESSION M_BOARDID, AiUInt8 biu, AiUInt8 bus, AiUInt8 amp);

AI_LIB_FUNC M_RETVAL_TYPE AI_CALL_CONV M_APICMDCALTRANSCON(VI_SESSION M_BOARDID, AiUInt8 biu, AiUInt8 trans_mode);

AI_LIB_FUNC M_RETVAL_TYPE AI_CALL_CONV M_APICMDREPLAYINI(VI_SESSION M_BOARDID, AiUInt8 biu, AiUInt8 cet,
                                                          AiUInt8 nct, AiUInt8 cyc, AiUInt8 nimg, AiUInt8 alt, AiUInt8 rlt,
                                                          AiUInt8 rint, AiUInt32 min, AiUInt32 msec, AiUInt8 sign,
                                                          AiUInt32 fsize);

AI_LIB_FUNC M_RETVAL_TYPE AI_CALL_CONV M_APICMDREPLAYSTART(VI_SESSION M_BOARDID, AiUInt8 biu);

AI_LIB_FUNC M_RETVAL_TYPE AI_CALL_CONV M_APICMDREPLAYSTOP(VI_SESSION M_BOARDID, AiUInt8 biu);

AI_LIB_FUNC M_RETVAL_TYPE AI_CALL_CONV M_APICMDREPLAYSTATUS(VI_SESSION M_BOARDID, AiUInt8 biu,
                                                             AiUInt8*  status,
                                                             AiUInt8*  padding1,
                                                             AiUInt16* padding2,
                                                             AiUInt32* rpi_cnt,
                                                             AiUInt32* saddr,
                                                             AiUInt32* size,
                                                             AiUInt32* entry_cnt);

AI_LIB_FUNC M_RETVAL_TYPE AI_CALL_CONV M_APICMDREPLAYRT(VI_SESSION M_BOARDID, AiUInt8 biu, AiUInt8 con, AiUInt8 mode, AiUInt8 rt);

AI_LIB_FUNC AiReturn AI_CALL_CONV ApiCmdGenIoAddrInitCvi(VI_SESSION M_BOARDID,  AiUInt8 genio_type, 
                                                         AiUInt8 io_type, AiUInt8 chn, AiUInt8 res, AiUInt32 addr);

AI_LIB_FUNC AiReturn AI_CALL_CONV ApiCmdGenIoOutChnWriteCvi(VI_SESSION M_BOARDID, AiUInt8 genio_type, 
                                                            AiUInt8 chn, AiUInt32 res, AiUInt32 val);

AI_LIB_FUNC AiReturn AI_CALL_CONV ApiCmdGenIoOutChnReadCvi(VI_SESSION M_BOARDID, AiUInt8 genio_type, 
                                                           AiUInt8 chn, AiUInt32 ctl, AiUInt32 *val);

AI_LIB_FUNC AiReturn AI_CALL_CONV ApiCmdGenIoInChnReadCvi(VI_SESSION M_BOARDID, AiUInt8 genio_type, 
                                                          AiUInt8 chn, AiUInt32 ctl, AiUInt32 *val);

AI_LIB_FUNC AiReturn AI_CALL_CONV ApiCmdGenIoSysSTaskCtrlCvi(VI_SESSION M_BOARDID, AiUInt8 genio_type, 
                                                             AiUInt8 con, AiUInt32 rate, AiUInt32 rsync, AiUInt32 ustep);



AI_LIB_FUNC AiReturn AI_CALL_CONV ApiInstIntHandlerCvi(AiUInt32 bModule, AiUInt8 biu, AiUInt8 uc_Type, TY_INT_FUNC_PTR pf_IntFunc);

AI_LIB_FUNC AiReturn AI_CALL_CONV ApiDelIntHandlerCvi(AiUInt32 bModule, AiUInt8 biu, AiUInt8 uc_Type);

AI_LIB_FUNC AiReturn AI_CALL_CONV ApiConnectToServerCvi( AiUChar * pszNetworkAdress, AiInt16 *w_ServerBoards);

AI_LIB_FUNC AiReturn AI_CALL_CONV ApiDisconnectFromServerCvi( AiUChar * pszNetworkAdress );

#if defined _AIM_LVRT
AI_LIB_FUNC AiStatus AI_CALL_CONV ApiWaitForInterruptsCvi(AiUInt32 bModule, AiUInt32 ul_Timeout, 
                                                           AiInt32* pl_NumInterrupts, 
                                                           AiUInt32 ul_Lla[MAX_IR_EVENT_BUF],
                                                           AiUInt32 ul_Llb[MAX_IR_EVENT_BUF],
                                                           AiUInt32 ul_Llc[MAX_IR_EVENT_BUF],
                                                           AiUInt32 ul_Lld[MAX_IR_EVENT_BUF]);
#endif
	
	
#endif                                                  // end #ifdef _API_APPL_WIN32
#endif
