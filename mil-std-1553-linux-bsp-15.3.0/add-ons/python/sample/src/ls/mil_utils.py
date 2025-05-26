# SPDX-FileCopyrightText: 2023-2024 AIM GmbH <info@aim-online.com>
# SPDX-License-Identifier: MIT

"""This file contains helper functions that are used in the samples.
"""

import os

from aim_mil import mil_bindings_defines as ambd
from aim_mil import mil_bindings_types as ambt

g_bc_hid = 1
g_rt_hid = 1
g_bid = [1,100,200,300]

xConfig = {}


# Reset ids when the simulation is stopped and is reconfigured
def MilUtilResetIds():
    global g_bc_hid 
    global g_rt_hid
    global g_bid

    g_bc_hid    = 1
    g_rt_hid    = 1
    g_bid[0]    = 1
    g_bid[1]    = 101
    g_bid[2]    = 201
    g_bid[3]    = 301


# BC buffer header ids are uniqe for each stream
def MilUtilNextBCHid():
    global g_bc_hid
    id = g_bc_hid
    g_bc_hid= g_bc_hid+1
    return id


# RT buffer header ids are uniqe for each stream
def MilUtilNextRTHid():
    global g_rt_hid
    id = g_rt_hid
    g_rt_hid = g_rt_hid+1
    return id


# Buffer Ids are shared between RT,BC and between all streams on one module
def MilUtilNextBid(handle, qSize):
    '''
    Note: No check is applied here if we reached the next section since the
    sample does uses only a few buffers.
    There might be more buffers available depending on the amount
    of memory equiped on your module. See ApiCmdSysGetMemPartition
    x_Sim[0].ul_BufCount for details. 
    '''
    id = g_bid[ambd.GET_STREAM_ID(handle) - 1]
    g_bid[ambd.GET_STREAM_ID(handle) - 1] += 1<<qSize
    return id


def MilUtilIsBcActive(pymilapi, handle):
    xBcStatus = pymilapi.PyApiCmdBCStatusRead(handle)

    if (xBcStatus.status == ambd.API_BC_STATUS_BUSY):
        return True
    else:
        return False


def MilUtilIsBcActiveCheckCount(pymilapi, handle, ulTransferCount):
    xBcStatus = pymilapi.PyApiCmdBCStatusRead(handle)

    if ((xBcStatus.glb_msg_cnt + xBcStatus.glb_err_cnt) < ulTransferCount):
        # Return count of transfer left until BC done
        return ulTransferCount - (xBcStatus.glb_msg_cnt + xBcStatus.glb_err_cnt)
    else:
        return 0


def MilUtilConfigureHardware(pymilapi, handle, coupling):
    '''
     Set the primary coupling 
    
        0 API_CAL_CPL_ISOLATED           Isolated, on-board termination
        1 API_CAL_CPL_TRANSFORM          Transformer coupled MILbus
        2 API_CAL_CPL_DIRECT             Direct coupled MILbus
        3 API_CAL_CPL_EXTERNAL           Externally coupled MILbus with on-board transformer-coupled MILbus termination
        4 API_CAL_CPL_WRAP_AROUND_LOOP   On-board, digital wrap-around loop between MILbus Encoder and Decoder
        without relying on the external MILbus.
    '''    
    # Set the primary coupling 
    pymilapi.PyApiCmdCalCplCon(handle, ambd.API_CAL_BUS_PRIMARY, coupling)
    # Set the secondary coupling 
    pymilapi.PyApiCmdCalCplCon(handle, ambd.API_CAL_BUS_SECONDARY, coupling)


def MilUtilEnableMailboxRT(pymilapi, handle,  ucRt):
    pymilapi.PyApiCmdRTIni(handle, ucRt, ambd.API_RT_ENABLE_MONITORING, ambd.API_RT_RSP_BOTH_BUSSES, 8, ucRt << 11)
    # Enable RT
    pymilapi.PyApiCmdRTEnaDis(handle, ucRt, ambd.API_ENA)


def MilUtilEnableRT(pymilapi, handle, ucRt, rep_time):
    pymilapi.PyApiCmdRTIni(handle, ucRt, ambd.API_RT_ENABLE_SIMULATION, ambd.API_RT_RSP_BOTH_BUSSES, rep_time, ucRt << 11)
    # Enable RT 
    pymilapi.PyApiCmdRTEnaDis(handle, ucRt, ambd.API_ENA)


def MilUtilEnableRTSA(pymilapi, handle, ucRt,  ucSa,  ucType, uwHeaderId,  ucQueueSize=ambd.API_QUEUE_SIZE_1):
    uwBufferId = MilUtilNextBid(handle, ucQueueSize)
    bid_count  = 1 << ucQueueSize

    pymilapi.PyApiCmdRTBHDef(handle, uwHeaderId, uwBufferId, 
                             ucQueueSize,                  #buffer/status queue size in pow(2,n)
                             ambd.API_BQM_CYCLIC, 
                             ambd.API_BSM_RX_KEEP_CURRENT, # keep same buffer on error
                             ambd.API_SQM_AS_QSIZE         # enable status queue 
                             )

    auwMessageData = 32 * [0]
    for i in range(bid_count):
        auwMessageData[0] = ((ucSa <<8) | i)

        pymilapi.PyApiCmdBufDef(handle, ambd.API_BUF_RT_MSG, uwHeaderId, i, auwMessageData)
        
    pymilapi.PyApiCmdRTSACon(handle, ucRt, ucSa, uwHeaderId, ucType, ambd.API_RT_ENABLE_SA)


def MilUtilBCCreateTransfer(pymilapi, handle,
                            uwTransferId,  
                            ucType, 
                            ucTransmitRt, 
                            ucTransmitSa, 
                            ucReceiveRt, 
                            ucReceiveSa,
                            ucWordCount, 
                            uwHeaderId, 
                            ucQueueSize=ambd.API_QUEUE_SIZE_1,
                            ulInterruptControl=ambd.API_BC_TIC_NO_INT,
                            gap_mode=0,
                            gap=0):

    ##############################################
    #                                            #
    # --            BC -> RT3RcvSA13         --- #
    #                                            #
    ##############################################
    xXferDescriptor = ambt.PY_TY_API_BC_XFER()
    xXferDescriptor.xid = uwTransferId
    xXferDescriptor.hid = uwHeaderId
    xXferDescriptor.type = ucType
    xXferDescriptor.chn = ambd.API_BC_XFER_BUS_PRIMARY
    xXferDescriptor.xmt_rt = ucTransmitRt
    xXferDescriptor.xmt_sa = ucTransmitSa
    xXferDescriptor.rcv_rt = ucReceiveRt
    xXferDescriptor.rcv_sa = ucReceiveSa
    xXferDescriptor.wcnt = ucWordCount
    xXferDescriptor.tic = ulInterruptControl
    xXferDescriptor.gap_mode = gap_mode
    xXferDescriptor.gap = gap

    # Install the transfer descriptor
    pymilapi.PyApiCmdBCXferDef(handle, xXferDescriptor)

    uwBufferId = MilUtilNextBid(handle, ucQueueSize)
    pymilapi.PyApiCmdBCBHDef(handle, 
                             uwHeaderId, 
                             uwBufferId,
                             ucQueueSize,                 # buffer/status queue size in pow(2,n)
                             ambd.API_BQM_CYCLIC,         # cyclic buffer queue
                             ambd.API_BSM_TX_KEEP_SAME,   # keep same buffer on error
                             ambd.API_SQM_AS_QSIZE,       # enable status queue
                             )

    # Fill all buffers in queue with buffer id
    auwMessageData = 32 * [0]
    ulBufferCount = 1 << ucQueueSize
    for uwBufferOffset in range(ulBufferCount):
        uwBufferOffset = uwBufferOffset + 1
        MilUtilSetData(pymilapi, handle, uwHeaderId, ambd.API_BUF_BC_MSG, uwBufferOffset, auwMessageData)


def MilUtilBCCreateSingleFrameFraming(pymilapi, handle, aulTransferIds):
    ##############################################
    #                                            #
    # --         Prepare Minor Frame         --- #
    #                                            #
    ##############################################
    '''
    Each minor frame can contain up to 128 [cnt] instructions:
    For transfers the [xid] contains the Transfer id and [instr] is set to API_BC_INSTR_TRANSFER
    Other instructions possible are API_BC_INSTR_SKIP (Used to generate BC branch interrupts), API_BC_INSTR_WAIT and API_BC_INSTR_STROBE
    '''
    xMinorFrame = ambt.PY_TY_API_BC_FRAME()
    xMinorFrame.id    = 1
    xMinorFrame.cnt   = len(aulTransferIds)
    xMinorFrame.xid   = aulTransferIds
    xMinorFrame.instr = [ambd.API_BC_INSTR_TRANSFER] * len(aulTransferIds)

    ##############################################
    #                                            #
    # --         Install Minor Frame         --- #
    #                                            #
    ##############################################
    '''
    The API can handle up to 64 uniqe Minor Frames.
    The minor frame is address by its id [id].
    '''
    pymilapi.PyApiCmdBCFrameDef(handle, xMinorFrame)

    ##############################################
    #                                            #
    # --         Install Major Frame         --- #
    #                                            #
    ##############################################
    '''
        The API can handle only a single Major Frame.
        This major frame can contain up to 512 [cnt] Minor Frames idendified by their ids [fid].
    '''
    xMajorFrame = ambt.PY_TY_API_BC_MFRAME_EX()
    xMajorFrame.cnt = 1
    xMajorFrame.fid = [1]
    pymilapi.PyApiCmdBCMFrameDefEx(handle, xMajorFrame)


def MilUtilGetData(pymilapi, handle, uwHeaderId, ucBufferType, uwIndex, ucWordCount):
    return pymilapi.PyApiCmdBufRead(handle,
        ucBufferType,
        uwHeaderId,
        uwIndex,
        ucWordCount )


def MilUtilSetData(pymilapi, handle, uwHeaderId, ucBufferType, uwIndex,  auwMessageData):
    _, address = pymilapi.PyApiCmdBufDef(handle,
        ucBufferType,
        uwHeaderId,
        uwIndex, # write to buffer at index 1..n #
        auwMessageData)   
    return address


def MilUtilBMGetLogicalBiuFromStream(pymilapi, handle, ulStream):
    # On 1553 boards stream is equal to logical BIU 
    return ulStream


def MilUtilBMLsInterpretData(aucRecordingBuffer):
    """MilUtilBMLsInterpretData parses the data and returns a list of messages.
    """
    messages = []
    offset   = 0
    while offset < len(aucRecordingBuffer):
        m, offset = ambt.PY_TY_API_QUEUE_BUF.fromBytes(aucRecordingBuffer, offset)
        messages.append(m)
    
    return messages


def MilUtilBMCWString(cw):
    rt = ((cw >> 11) & 0x1F)
    tx = ((cw >> 10) & 0x1)
    sa_mc = ((cw >> 5) & 0x1F)
    wc = ((cw)& 0x1F)

    if (cw == 0):
        return('N/A')
    else:
        # C <RT>_<T/R>_<SA>_<WC>
        return('C%02d_%c_%02d_%02d' % (rt, 'R' if tx == 0 else 'T', sa_mc, wc))


def MilUtilBMSWString(sw):
    rt = ((sw >> 11) & 0x1F)

    error_string = ''
    if ((sw & 0x7FF) == 0): error_string = "_CS"
    if ((sw & 1 << 0) != 0): error_string+= "_TF"
    if ((sw & 1 << 1) != 0): error_string+="_DBCA"
    if ((sw & 1 << 2) != 0): error_string+= "_SSF"
    if ((sw & 1 << 3) != 0): error_string+="_BUSY"
    if ((sw & 1 << 4) != 0): error_string+="_BCR"
    if ((sw & 1 << 5) != 0): error_string+="_RES1"
    if ((sw & 1 << 6) != 0): error_string+="_RES2"
    if ((sw & 1 << 7) != 0): error_string+="_RES3"
    if ((sw & 1 << 8) != 0): error_string+="_SRQ"
    if ((sw & 1 << 9) != 0): error_string+="_INST"
    if ((sw & 1 << 10) != 0): error_string+="_ME"

    return("S%02d%s" % (rt,error_string))


def MilUtilBMErrorString( err):
    error_string = ''
    if ((err & 1 << 15) != 0): error_string+="E"
    if ((err & 1 << 14) != 0): error_string+="_AlternateBus"
    if ((err & 1 << 13) != 0): error_string+="_WordCountLow"
    if ((err & 1 << 12) != 0): error_string+="_WordCountHigh"
    if ((err & 1 << 11) != 0): error_string+="_SW"
    if ((err & 1 << 10) != 0): error_string+="_Address"
    if ((err & 1 << 9) != 0): error_string+="_EarlyResp"
    if ((err & 1 << 8) != 0): error_string+="_IllegalCW"
    if ((err & 1 << 7) != 0): error_string+="_BothBusses"
    if ((err & 1 << 6) != 0): error_string+="_Gap"
    if ((err & 1 << 5) != 0): error_string+="_InvSync"
    if ((err & 1 << 4) != 0): error_string+="_Parity"
    if ((err & 1 << 3) != 0): error_string+="_BitCountLow"
    if ((err & 1 << 2) != 0): error_string+="_BitCountHigh"
    if ((err & 1 << 1) != 0): error_string+="_Mancheser"
    if ((err & 1 << 0) != 0): error_string+="_NoResp"
    
    return error_string


def MilUtilBMLsShowData(data):
    ucBMEntryType = 0
    ulBMEntryData = 0
    ulCurrentEntry = 0
    iterator_position = 0
    buffer_end = len(data)

    if (buffer_end == 0):
        return ambd.API_ERR_QUEUE_EMPTY

    if ((iterator_position + (3*4)) > buffer_end):
        print('Error: Not enough data')
        return ambd.API_ERR
    
    while True:
        # Interpret BM entry 
        ulCurrentEntry = ((data[iterator_position+3]<<24) + 
                        (data[iterator_position+2]<<16) + 
                        (data[iterator_position+1]<<8) + 
                        (data[iterator_position+0]<<0))    
        ucBMEntryType = (ulCurrentEntry >> 28) & 0xF # Bits 28 .. 31 
        ulBMEntryData = (ulCurrentEntry & 0x3ffffff) # Bits  0 .. 26 

        # -- Message data interpretation ---

        if (ucBMEntryType == 1):
            # Error Word 
            print('ERROR:   0x%04X (%s)' % (ulBMEntryData & 0xFFFF,MilUtilBMErrorString(ulBMEntryData)))
        elif (ucBMEntryType == 2):
            # Time Tag Low         
            print('TTLOW:   0x%08X (%02d.%06ds) ' % ( ulBMEntryData, (ulBMEntryData >> 20) & 0x3F, ulBMEntryData & 0xFFFFF))
        elif (ucBMEntryType == 3):
            # Time Tag High        
            print('---------------')
            print('TTHIGH:  0x%08X (%03dd:%02dh:%02dm)' % (ulBMEntryData, (ulBMEntryData >> 11) & 0x1FF, (ulBMEntryData >> 6) & 0x1F, ulBMEntryData & 0x3F))
        elif (ucBMEntryType == 8):
            # Command Word   (Pri) 
            print("CW1_PRI: 0x%4X (%s)" % ((ulBMEntryData & 0xFFFF),MilUtilBMCWString(ulBMEntryData)))
        elif (ucBMEntryType == 9):
            # Command Word 2 (Pri) 
            print("CW2_PRI: 0x%4X (%s)" % ( ulBMEntryData & 0xFFFF,MilUtilBMCWString(ulBMEntryData)))
        elif (ucBMEntryType == 0xa):
            # Data Word      (Pri) 
            print("DW_PRI:  0x%04X (%4.2f us)" % (ulBMEntryData & 0xFFFF, ((ulBMEntryData >> 16) & 0x1FF) * 0.25))
        elif (ucBMEntryType == 0xb):
            # Status Word    (Pri) 
            print("SW_PRI:  0x%04X (%s)" % ( ulBMEntryData & 0xFFFF,MilUtilBMSWString(ulBMEntryData)))
        elif (ucBMEntryType == 0xc):
            # Command Word   (Sec) 
            print("CW1_SEC: 0x%4X (%s)" % (ulBMEntryData & 0xFFFF,MilUtilBMCWString(ulBMEntryData)))
        elif (ucBMEntryType == 0xd):
            # Command Word 2 (Sec)
            print("CW2_SEC: 0x%4X (%s)" % (ulBMEntryData & 0xFFFF,MilUtilBMCWString(ulBMEntryData)))
        elif (ucBMEntryType == 0xe):
            # Data Word      (Sec) 
            print("DW_SEC:  0x%04X (%4.2f us)" % (ulBMEntryData & 0xFFFF, ((ulBMEntryData >> 16) & 0x1FF) * 0.25))
        elif (ucBMEntryType == 0xf):
            # Status Word    (Sec) 
            print("SW_SEC:  0x%04X (%s)" % ( ulBMEntryData & 0xFFFF,MilUtilBMSWString(ulBMEntryData)))
        elif (ucBMEntryType == 0x0):
            # Entry not updated   
            print("INVALID: 0x%08X" % ulBMEntryData)
        else:
            # Reserved 4..7        
            # Ignored 
            print("Reserved: 0x%08X" % ulBMEntryData)

        iterator_position = iterator_position + 4

        if (iterator_position >= buffer_end):
            break
    # -- clean up ---- 
    return 0


def MilUtilPrintVersionInfo(pymilapi, handle):
    i  = 0
    sn = 0
    version = ambt.PY_TY_VER_INFO()
    ids = { ambd.AI_SYS_DRV_VER, ambd.AI_DLL_VER, ambd.AI_TARGET_VER }
    
    sn = pymilapi.PyApiCmdSysGetBoardInfo( handle, ambd.TY_BOARD_INFO_SERIAL)

    print('Device info : \n')
    print('Device %s serial number %04d '% (pymilapi.PyApiGetBoardName(handle), sn ))

    #Read basic versions 
    for i in ids:
        version = pymilapi.PyApiReadVersion( handle, i )
        if version != None:
            print('%-30s:  %-40s' % ( version.ac_Description, version.ac_FullVersion))


def MilUtilCheckIfDeviceIsEmbedded(pymilapi, handle):
    ulBoardInfo = pymilapi.PyApiCmdSysGetBoardInfo(handle, ambd.TY_BOARD_INFO_APPLICATION_TYPE)

    if (ambd.API_DEVICE_MODE_IS_EMBEDDED(ulBoardInfo&0xFF)):
        # First Stream is embedded, asume the same for the whole board #
        return 1
    else:
        return 0


def MilUtilCheckIfDeviceIsSimulatorOrSingleFunction(pymilapi, handle):
    ulBoardInfo = pymilapi.PyApiCmdSysGetBoardInfo(handle, ambd.TY_BOARD_INFO_APPLICATION_TYPE)

    if (ambd.API_DEVICE_MODE_IS_SIMULATOR(ulBoardInfo & 0xFF)):
        # First Stream is simulator, asume the same for the whole board #
        return 1

    if (ambd.API_DEVICE_MODE_IS_SINGLEFUNCTION(ulBoardInfo & 0xFF)):
        # First Stream is single function, asume the same for the whole board #
        return 1

    if (ambd.API_DEVICE_MODE_IS_DISABLE_BC(ulBoardInfo & 0xFF)):
        # First Stream is disable BC, asume the same for the whole board #
        return 1
    return 0


def MilUtil1553SampleList():
    return [
        "Samples :",
        "-- Samples with internal coupling ----",
        " 0  TemplateSample",
        " 1  PrintBoardVersions",
        " 2  LsBcRtBmSample",
        " 3  LsAcyclicSample",
        " 4  LsInstructionListSample",
        " 5  LsRecordingReplaySample",
        " 6  LsDynamicDataSample",
        " 7  LsBufferQueueSample",
        " 8  LsInterruptSample",
        " 9  LsFifoSample",
        "",
        "-- Samples with external coupling ----",
        "20  LsMilscopeSample (local only)",
        "21  LsPtpVtpSample (RT UUT Setup)",
        "22  LsBmSample",
        "23  LsBcSample",
        "",
    ]


def MilUtilPrintAvailableLocalModules(pymilapi):
    print('List available local modules:')
    module_count = pymilapi.PyApiInit()

    for i in range(module_count):
        handle = pymilapi.PyApiOpenEx( ambt.PY_TY_API_OPEN(i, ambd.API_STREAM_1, "local") )

        sn = pymilapi.PyApiCmdSysGetBoardInfo(handle, ambd.TY_BOARD_INFO_SERIAL)
        xDriverInfo =  pymilapi.PyApiGetDriverInfo(handle)
        print('%2d: %-16s SN %4d tasks connected to module %d' %  (i, pymilapi.PyApiGetBoardName(handle), sn, xDriverInfo.ul_OpenConnections - 1))

    pymilapi.PyApiExit()


def MilUtilProcessSampleUtilityCommands(pymilapi, name, command, samples):
    # only one argument, print usage or list modules
    if (command == "list"):
        # List available local modules
        MilUtilPrintAvailableLocalModules(pymilapi)
        input('--- press any key ---')
        exit(0)
    elif (command == "help"):
        # Show available sample command line arguments
        print('%s help' % name)
        print('%s list' % name)
        print('%s server module_nr stream_nr sample_nr' % (name))
        for s in samples:
            print(s)
        input('--- press any key ---')
        exit(0)


def MilUtilReadConfiguration(pymilapi, argv, samples = MilUtil1553SampleList() ):
    argc = len(argv)
   
    if( argc == 5 ):
        # use command line arguments 
        xConfig["acServer"] = argv[1]
        xConfig["ulDevice"] = int(argv[2])
        xConfig["ulStream"] = int(argv[3])
        xConfig["ulTest"]   = int(argv[4])
    elif( argc == 2 ):
        MilUtilProcessSampleUtilityCommands(pymilapi, argv[0], argv[1], samples)
    else:
        # no arguments, ask user
        szDummy = input('action [sample, help, list] ? ') or 'sample'
        MilUtilProcessSampleUtilityCommands(pymilapi, argv[0], szDummy, samples)

        xConfig["acServer"] = input('server [local] ? ') or 'local'
        xConfig["ulDevice"] = int( input('module number [%d..%d] ? ' % (ambd.API_MODULE_1, ambd.API_MODULE_32)) or 0 )
        xConfig["ulStream"] = int( input('stream number [%d..%d] ? ' % (ambd.API_STREAM_1, ambd.API_STREAM_8)) or ambd.API_STREAM_1 )

        for s in samples:
            print(s)
        xConfig["ulTest"] = int(input('sample number ? ') or "1" )

    if( "local" == xConfig["acServer"] ):
        xConfig["ucUseServer"]   = False
    else:
        xConfig["ucUseServer"]   = True

    if (xConfig["ulTest"] >= 20) and (xConfig["ulTest"] < 30):
        xConfig["ulCoupling"] = ambd.API_CAL_CPL_TRANSFORM
    else:
        xConfig["ulCoupling"] = ambd.API_CAL_CPL_ISOLATED

    return xConfig


def MilUtilHasMilscope(pymilapi, handle):
    ulMilscopeType = pymilapi.PyApiCmdSysGetBoardInfo(handle, ambd.TY_BOARD_INFO_MILSCOPE_TYPE)

    if (ulMilscopeType == ambd.API_MILSCOPE_TYPE_AYX):
        # AyX is no longer supported with BSP 15.x
        return False
    elif (ulMilscopeType == ambd.API_MILSCOPE_TYPE_AYE):
        return True
    else:
        return False


def MilUtilOsKbHit():
    if os.name == 'nt':
        import msvcrt
        return msvcrt.kbhit()
    else:
        # Requires return to be pressed after key
        import sys
        from select import select
        dr,dw,de = select([sys.stdin], [], [], 0)
        return dr != []
