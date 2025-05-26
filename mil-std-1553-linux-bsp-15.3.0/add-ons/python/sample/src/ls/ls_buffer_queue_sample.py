# SPDX-FileCopyrightText: 2023-2024 AIM GmbH <info@aim-online.com>
# SPDX-License-Identifier: MIT

"""This sample shows how to handle receive buffer queues.
This sample creates a cyclic frame with one BC2RT transfer. 
The transfer has a buffer queue with 16 buffers enabled on both the transmitter and receiver side. 
Each data word in the transmit buffer queue is initialized with a unique data word. 
The BC framing is then send with a total of 2000 transfers and the RT enabled. 
During the framing the RT data is continuously read and the receive data buffer 
consistency is ensured through the use of the buffer queue. 
For each received transfer on the RT the first three and the last data words 
are checked to match the expected data for the transfer. 
At the end of the framing the BC and RT status is printed. 
This status should show the expected amount of transfers and no errors. 
This sample is executed with internal coupling. No external connection is required.
"""

import time

from aim_mil import mil_bindings_defines as ambd
from aim_mil import mil_bindings_types as ambt

from .mil_utils import  MilUtilNextRTHid, \
                        MilUtilEnableRT, \
                        MilUtilEnableRTSA, \
                        MilUtilNextBCHid, \
                        MilUtilGetData, \
                        MilUtilSetData, \
                        MilUtilBCCreateTransfer, \
                        MilUtilBCCreateSingleFrameFraming, \
                        MilUtilIsBcActive

uwGlobalLastTimeTag       = 0
uwGlobalTransferCount     = 0
rt3_sa13_buffer_header_id = 0

def PrepareRemoteTerminals(pymilapi,handle):
    global rt3_sa13_buffer_header_id
    print("PrepareRemoteTerminals ... done")

    #############################################
    #                                           #
    # --      Simulate RT3 receive SA13     --- #
    #                                           #
    #############################################
    rt3_sa13_buffer_header_id = MilUtilNextRTHid()
    MilUtilEnableRT(pymilapi,handle, 3, 8.0)
    MilUtilEnableRTSA(pymilapi,handle, 
                    3,  # RT
                    13, # SA
                    ambd.API_RT_TYPE_RECEIVE_SA, 
                    rt3_sa13_buffer_header_id, 
                    ambd.API_QUEUE_SIZE_16)


def PrepareBusController(pymilapi,handle):
    auwTransferIds = [1] 
    #############################################
    #                                           #
    # --  Initialize the Bus Controller     --- #
    #                                           #
    #############################################
    pymilapi.PyApiCmdBCIni( handle )

    print()
    print(  "Framing :     2000 times \n",
            "------------------------ \n",
            "BC         -> RT3RcvSA13 \n",
            "------------------------ \n",
            "    total 2000 transfers \n" )

    print("PrepareBusController ... done")

    #############################################
    #                                           #
    # --            BC -> RT3RcvSA13        --- #
    #                                           #
    #############################################
    uwHeaderId   = MilUtilNextBCHid()

    MilUtilBCCreateTransfer( pymilapi, handle, 
                                       auwTransferIds[0],
                                       ambd.API_BC_TYPE_BCRT, 
                                       0,  # Transmit RT
                                       0,  # Transmit SA 
                                       3,  # Receive  RT 
                                       13, # Receive  SA 
                                       0,  # Word count 32 
                                       uwHeaderId, 
                                       ucQueueSize=ambd.API_QUEUE_SIZE_16)

    auwMessageData = 32 * [0]
    # Initialize all 16 buffers in the queue #
    for uwBufferOffset in range(16):
        uwBufferOffset = uwBufferOffset + 1
        for i in range (32):
            # buffer  1 : auwMessageData[] = { 0x0001, 0x0002, 0x0003 ... 0x0032 } #
            # buffer  2 : auwMessageData[] = { 0x1001, 0x1002, 0x1003 ... 0x1032 } #
            # buffer  3 : auwMessageData[] = { 0x2001, 0x2002, 0x2003 ... 0x2032 } #
            # ...                                                                  #
            # buffer 16 : auwMessageData[] = { 0xF001, 0xF002, 0xF003 ... 0xF032 } #
            auwMessageData[ i ] = (0x1000 * (uwBufferOffset - 1)) + i + 1
        MilUtilSetData(pymilapi, handle, uwHeaderId, ambd.API_BUF_BC_MSG, uwBufferOffset, auwMessageData)

    #############################################
    #                     BC                    #
    # --      Setup Major/Minor Framing  ---    #
    #                                           #
    #############################################
    MilUtilBCCreateSingleFrameFraming(pymilapi, handle, auwTransferIds)


def CheckData(ulTimeTag, Data, ulStatus):
    global uwGlobalTransferCount
    global uwGlobalLastTimeTag

    # -- Check timetag ---
    if( ulTimeTag <= uwGlobalLastTimeTag ):
        print("Error : Timetag not greater than last timetag for transfer %d" % (uwGlobalTransferCount) )
        return 1

    uwGlobalLastTimeTag = ulTimeTag

    # -- Check data buffer ---
    ucQueue = (uwGlobalTransferCount % 16)

    uwExpectedWord_00 = (ucQueue * 0x1000) + 0x0001
    uwExpectedWord_01 = (ucQueue * 0x1000) + 0x0002
    uwExpectedWord_02 = (ucQueue * 0x1000) + 0x0003
    uwExpectedWord_31 = (ucQueue * 0x1000) + 0x0020

    if(    (Data[ 0] != uwExpectedWord_00)
        or (Data[ 1] != uwExpectedWord_01)
        or (Data[ 2] != uwExpectedWord_02)
        or (Data[31] != uwExpectedWord_31) ):

        print("Error : Invalid data for transfer %d" % (uwGlobalTransferCount) )
        return 2

    uwGlobalTransferCount = uwGlobalTransferCount + 1
    return 0


def RunSimulation(pymilapi, handle):
    print('RunSimulation ... running')

    #############################################
    #                                           #
    # --       Start all enabled RTs        --- #
    #                                           #
    #############################################
    pymilapi.PyApiCmdRTStart(handle)

    #############################################
    #                                           #
    # --         Start Bus Controller       --- #
    #                                           #
    #############################################
    pymilapi.PyApiCmdBCStart( handle, ambd.API_BC_START_IMMEDIATELY, 2000, 10.0 )


def PollRtReceiveBufferQueue(pymilapi, handle):
    ucContinue             = 1
    ulLastFirmwareIndex    = 0
    ulCurrentFirmwareIndex = 0
    ulCurrentIndex         = 0
    verbose = False

    print("Poll data and status buffer queue ... ")

    while( ucContinue ):
        ucContinue = MilUtilIsBcActive(pymilapi, handle)

        xStatus = pymilapi.PyApiCmdRTSAMsgReadEx( handle, ambt.PY_TY_API_RT_SA_MSG_READ_IN(3, 13, ambd.API_RT_TYPE_RECEIVE_SA) )
  
        ulCurrentFirmwareIndex = xStatus.x_XferInfo.ul_ActBufferId

        if( ulLastFirmwareIndex != ulCurrentFirmwareIndex ):
            # Read from last buffer index to current buffer index #
            # The current buffer index is not valid and will be read next time #
            ulCurrentIndex = ulLastFirmwareIndex

            if verbose:
               print()
               print("    Queue  | DW1  | DW2  | DW3  | ... | DW32 | Timetag   | Status   |" )
               print("---------------------------------------------------------------------" )

            while( ulCurrentIndex != ulCurrentFirmwareIndex ):
                auwData, _, _ = MilUtilGetData(pymilapi, handle,
                                                rt3_sa13_buffer_header_id,
                                                ambd.API_BUF_RT_MSG,
                                                ulCurrentIndex + 1,
                                                32)

                if verbose:
                    print("Buffer %2d  | %04X | %04X | %04X | ... | %04X | %08X  | %08X |" % (
                                                            ulCurrentIndex, 
                                                            auwData[0], auwData[1], auwData[2], auwData[31],
                                                            xStatus.x_XferSQueue[ulCurrentIndex].ul_SqTimeTag,
                                                            xStatus.x_XferSQueue[ulCurrentIndex].ul_SqStatusWords ))
                else:
                    if (uwGlobalTransferCount%100) == 0:
                        print("Processed %d buffers" % (uwGlobalTransferCount))

                ret = CheckData(
                                xStatus.x_XferSQueue[ulCurrentIndex].ul_SqTimeTag,
                                auwData,
                                xStatus.x_XferSQueue[ulCurrentIndex].ul_SqStatusWords 
                            )

                if( ret != ambd.API_OK ):
                    ucContinue = 0
                    break

                ucContinue = 1
                # ulCurrentIndex = 0, 1, 2, 3, ... 15 #
                ulCurrentIndex = (ulCurrentIndex+ 1) % 16

            ulLastFirmwareIndex = ulCurrentFirmwareIndex

        time.sleep(0.05)

    print("Processed %d buffers" % (uwGlobalTransferCount))


def StopSimulation(pymilapi, handle):
    print('StopSimulation ... done')

    # Stop Bus Controller #
    pymilapi.PyApiCmdBCHalt(handle)

    # Stop all RTs #
    pymilapi.PyApiCmdRTHalt(handle)


def PrintSimulationStatistic(pymilapi, handle):
    print('PrintSimulationStatistic ...')

    xBcStatus = pymilapi.PyApiCmdBCStatusRead( handle )
    print('BC  ( %d messages / %d errors )' % (xBcStatus.glb_msg_cnt, xBcStatus.glb_err_cnt))

    xRtStatus = pymilapi.PyApiCmdRTStatusRead( handle)
    print('RTs ( %d messages  / %d errors )' % (xRtStatus.glb_msg_cnt, xRtStatus.glb_err_cnt))

    print('Please note that one RT to RT transfer counts as two messages in RTs and BM')
    print('PrintSimulationStatistic ... done')


########################################################################################
#                                                                                      #
#                                     Sample entry point                               #
#                                                                                      #
########################################################################################
def LsBufferQueueSample(pymilapi, handle):
    print("Running LsBufferQueueSample")

    try:
        # Setup RTs and BC
        PrepareRemoteTerminals( pymilapi, handle )
        PrepareBusController(   pymilapi, handle )
        # Start the simulation and the bus monitor
        RunSimulation( pymilapi, handle )
        # Read data from RT
        PollRtReceiveBufferQueue(pymilapi, handle)

    finally:
        # Stop the simulation and the bus monitor
        StopSimulation( pymilapi, handle )
        PrintSimulationStatistic( pymilapi, handle )
