# SPDX-FileCopyrightText: 2023-2024 AIM GmbH <info@aim-online.com>
# SPDX-License-Identifier: MIT

"""This sample shows how to record 1553 bus monitor data and how replay the data afterwards.
This sample/test consists of two parts. The recording part and the replay part.

The recording part:
The sample creates a cyclic framing with a BC2RT transfer. 
The transfer has a dynamic data word generation in data word position 1 enabled. 
The transfer is send 5000 times with RT and BM enabled. 
The test reads the BM with a threshold of a minimum of 128KB per BM read. 
The BM data is parsed to count the number of transfers in the data. 
Then the data is stored in a temporary file. 
At the end of the framing the test checks the recording file size and the transfer count obtained from the data.

The replay part:
This part is only executed if there was no error in the recording part. 
The test reads the temporary file generated in the recording step and replays it with the RT enabled. 
At the end of the replay the RT status is checked if the correct amount of transfers has been received. 
This test is executed with internal coupling. No external connection is required.
"""

import time
import tempfile
import os

from aim_mil import mil_bindings_defines as ambd
from aim_mil import mil_bindings_types as ambt

from .mil_utils import MilUtilNextRTHid, \
                        MilUtilEnableRT, \
                        MilUtilEnableMailboxRT, \
                        MilUtilEnableRTSA, \
                        MilUtilNextBCHid, \
                        MilUtilSetData, \
                        MilUtilBCCreateTransfer, \
                        MilUtilBCCreateSingleFrameFraming, \
                        MilUtilIsBcActive, \
                        MilUtilBMGetLogicalBiuFromStream, \
                        MilUtilBMLsInterpretData


# Global constants
VERBOSE = False
szGlobalFileName = None
global_recording_buffer_size = (128*1024) # Read  blocks of 128 KB
global_replay_buffer_size = 0x10000


#######################################################################
#                                                                    *
#                                                                    *
#                            Recording                               *
#                                                                    *
#                                                                    *
#######################################################################
ucStreamToDataQueueIdMax = 8
aucStreamToDataQueueId = [  0, # invalid stream number
                            ambd.API_DATA_QUEUE_ID_BM_REC_BIU1,
                            ambd.API_DATA_QUEUE_ID_BM_REC_BIU2,
                            ambd.API_DATA_QUEUE_ID_BM_REC_BIU3,
                            ambd.API_DATA_QUEUE_ID_BM_REC_BIU4,
                            ambd.API_DATA_QUEUE_ID_BM_REC_BIU5,
                            ambd.API_DATA_QUEUE_ID_BM_REC_BIU6,
                            ambd.API_DATA_QUEUE_ID_BM_REC_BIU7,
                            ambd.API_DATA_QUEUE_ID_BM_REC_BIU8 ]


def IsQueueDataAvailable(pymilapi, handle, ucQueueId):
    # read 0 bytes to check if data is available
    _, xDataQueuStatus = pymilapi.PyApiCmdDataQueueRead(handle,  ambt.PY_TY_API_DATA_QUEUE_READ(ucQueueId, 0))
    xDataAvailable = xDataQueuStatus.bytes_in_queue
    return xDataAvailable


def ReadData(pymilapi, handle, ucQueueId, pFile, ulDataAvailable):
    aucRecordingBuffer , xDataQueuStatus = pymilapi.PyApiCmdDataQueueRead(handle, ambt.PY_TY_API_DATA_QUEUE_READ(ucQueueId, ulDataAvailable))

    if (xDataQueuStatus.status & 0xFFFF):
        raise Exception('Error : ApiCmdDataQueueRead failed with status 0x%X' % (xDataQueuStatus.status))

    # Save buffer to file 
    pFile.write(aucRecordingBuffer)

    xBmInterpretedMessages = MilUtilBMLsInterpretData(aucRecordingBuffer)

    if VERBOSE:
        for m in xBmInterpretedMessages:
            print(hex(m.ttag), m.rt_addr, m.sa_mc, m.sa_type, len(m.buffer), hex(m.buffer[0]), hex(m.buffer[1]), hex(m.buffer[30]), hex(m.buffer[31]),  hex(m.status_word1))

    print("Monitor received %.2f KB with %d transfers" % 
          (xDataQueuStatus.bytes_transfered / 1024,
          len(xBmInterpretedMessages)))

    return len(xBmInterpretedMessages)


def PrepareSimulation(pymilapi, handle):
    print("PrepareSimulation ... ")

    auwTransferIds =[1]
    #############################################
    #                                           #
    # --      Simulate RT3 receive SA13     --- #
    #                                           #
    #############################################
    uwHeaderId = MilUtilNextRTHid()
    MilUtilEnableRT(pymilapi, handle, 3 , 8.0 )
    MilUtilEnableRTSA(pymilapi, handle, 3 , 13 , ambd.API_RT_TYPE_RECEIVE_SA, uwHeaderId)

    #############################################
    #                                           #
    # --  Initialize the Bus Controller     --- #
    #                                           #
    #############################################
    pymilapi.PyApiCmdBCIni( handle, 0, 0, 0, 0 )
    print( "Framing :     5000 times \n",
           "------------------------ \n",
           "BC         -> RT3RcvSA13 \n",
           "------------------------ \n",
           "    total 5000 transfers \n" )

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
                            uwHeaderId)

    # Data to send to the RT
    auwMessageData = 32 * [0]
    for i in range (32):
        auwMessageData[ i ] = 0xBC00 + i + 1

    MilUtilSetData(pymilapi, handle, 
                   uwHeaderId, 
                   ambd.API_BUF_BC_MSG, 
                   1, # First buffer in queue
                   auwMessageData)
    
    ################################################
    #                                              #
    # -- Enable Dytag for dataword 1 in xfer ---   #
    #                                              #
    ################################################
    axDytag = [ambt.PY_TY_API_BC_DYTAG() for _ in range(4)]

    axDytag[ 0 ].tag_fct = ambd.API_DYTAG_FCT_POS_RAMP
    # Lower Limit
    axDytag[ 0 ].min = 0
    # Upper Limit
    axDytag[ 0 ].max = 0xFFFF
    # Step size
    axDytag[ 0 ].step = 1
    # Manipulate first data word
    axDytag[ 0 ].wpos = 0

    pymilapi.PyApiCmdBCDytagDef( handle, ambd.API_ENA, uwHeaderId, ambd.API_DYTAG_STD_MODE, axDytag )

    ################################################
    #                     BC                       #
    # --      Setup Major/Minor Framing  ---       #
    #                                              #
    ################################################
    MilUtilBCCreateSingleFrameFraming(pymilapi, handle, auwTransferIds)
     

def RunSimulation(pymilapi, handle):
    print("RunSimulation ... running")
    # Start all enabled RTs
    pymilapi.PyApiCmdRTStart( handle )
    # -- Start Bus Controller --- 
    pymilapi.PyApiCmdBCStart( handle, ambd.API_BC_START_IMMEDIATELY, 5000,  10.0 )


def StopSimulation(pymilapi, handle):
    print("StopSimulation ... done")
    # Stop Bus Controller
    pymilapi.PyApiCmdBCHalt( handle )
    # Stop all RTs
    pymilapi.PyApiCmdRTHalt( handle )
    # Stop Bus Monitor 
    pymilapi.PyApiCmdBMHalt( handle )


def StartRecording(pymilapi, handle, ucQueueId):
    print("StartRecording ... done")
    # Setup Bus Monitor for Low speed Recording
    pymilapi.PyApiCmdBMIni( handle)

    xCaptureMode = ambt.PY_TY_API_BM_CAP_SETUP(ambd.API_BM_CAPMODE_RECORDING)
    pymilapi.PyApiCmdBMCapMode( handle, xCaptureMode )

    # Start Bus Monitor
    pymilapi.PyApiCmdBMStart( handle )

    # -- open data queue ---
    size = pymilapi.PyApiCmdDataQueueOpen(handle, ucQueueId)
    print('queue_size(0x%x) ... ' % (size) )
    # -- start data queue ---
    pymilapi.PyApiCmdDataQueueControl(handle, ucQueueId, ambd.API_DATA_QUEUE_CTRL_MODE_START)


def StopRecording(pymilapi, handle, ucQueueId):
    print('StopRecording ... done')
    # boards without an ASP might require some additional time
    time.sleep(0.100)
    # The BM should already be stopped. #
    # This stop is only required in case of an error during the recording process. #
    pymilapi.PyApiCmdBMHalt( handle )
    pymilapi.PyApiCmdDataQueueControl(handle, ucQueueId, ambd.API_DATA_QUEUE_CTRL_MODE_STOP)


def CloseRecording(pymilapi, handle, ucQueueId):
    print('CloseRecording ... done')
    pymilapi.PyApiCmdDataQueueClose(handle, ucQueueId )


def CheckRecording(pymilapi, handle, ulTotalMessageCount ):
    if(ulTotalMessageCount != 5000):
        raise("Error : Expected to receive 5000 transfers got %d" % (ulTotalMessageCount))
    else:
        print("Received %d transfers" % (ulTotalMessageCount))

    # 5000 transfers with 36 entries out of 4 bytes 
    ulExpectedSize = 5000 * (36 * 4)

    # We do only check the filesize as a minimal condition
    ulFileSize = os.path.getsize(szGlobalFileName)

    if ulFileSize != ulExpectedSize:
        raise Exception("Error : Expected recording file size %d is %d\n" % ( ulExpectedSize, ulFileSize ))


def StartRtMonitoring(pymilapi, handle):
    print("StartRtMonitoring ... done")
    '''
       By default all RTs are replayed by the Replay engine.
       If you need to responde with a real RT you may disable 
       the RT with ApiCmdReplayRT after the ApiCmdReplayIni call.

       The code below enables the RT for monitoring only. 
       The RT setup here will not responde on the bus.
       This is optional and is only done to obtain the statistics later.    
    '''

    #############################################
    #                                           #
    # --      Mailbox RT3 receive SA13      --- #
    #                                           #
    #############################################
    MilUtilEnableMailboxRT(pymilapi, handle, 
                           3 # RT
                           )

    MilUtilEnableRTSA(pymilapi, handle, 
                      3,  # RT
                      13, # SA
                      ambd.API_RT_TYPE_RECEIVE_SA, 
                      MilUtilNextRTHid())

    # -- Start all enabled RTs --- 
    pymilapi.PyApiCmdRTStart( handle)

    # -- Start Bus Montior to get transer statistics --- 
    pymilapi.PyApiCmdBMStart( handle)


def WriteReplayBufferFromFile( pymilapi, handle, pFile ):
    xReplayStatus = pymilapi.PyApiCmdReplayStatus( handle, )

    aucReplayBuffer = pFile.read( global_replay_buffer_size )
    ulBytes = len(aucReplayBuffer)
    if ulBytes > 0:
        # Got data from file 
        xReplayStatus.size = ulBytes

        xReplayStatus, ulBytes = pymilapi.PyApiWriteRepData( handle, xReplayStatus, aucReplayBuffer )

    return ulBytes


def Replaying(pymilapi, handle):
    print("Replaying ... ")
    ################################################
    #                                              #
    # --          Get input file size          --- #
    #                                              #
    ################################################
    ulFileSize = os.path.getsize( szGlobalFileName )

    if ulFileSize == 0:
        raise Exception("Error : could not get file size of %s" % (szGlobalFileName ))

    ################################################
    #                                              #
    # --           Open input file             --- #
    #                                              #
    ################################################
    with open( szGlobalFileName, "rb") as pFile:
        ################################################
        #                                              #
        # --      Initialize replay                --- #
        #                                              #
        ################################################

        '''
        Before the replay start command can be called we need to initialize both
        half replay buffers with data. This is required because the replay will 
        be started at once after the command is issued. 
        '''
        pymilapi.PyApiCmdReplayIni( handle,  1, ambd.API_REP_NO_ABS_TIME , ambd.API_REP_HFI_INT, ulFileSize )

        # Load first half replay buffer 
        ulBytesWritten = WriteReplayBufferFromFile( pymilapi, handle, pFile )

        if ulBytesWritten == 0 :
            raise Exception("Error : Not enough data in file %s for first half buffer." % (szGlobalFileName) )
        
        # Load second half replay buffer 
        ulBytesWritten = WriteReplayBufferFromFile( pymilapi, handle, pFile )

        if ulBytesWritten == 0:
            raise Exception("Error : Not enough data in file %s for second half buffer." % (szGlobalFileName))

        '''
        Both replay buffers are now filled with data. With the following command we start the
        onboard replay. Every time on half buffer is replayed the onboard software emits a 
        halfbuffer full interrupt (HFI). We use the status read command to poll the HFI count.
        If the HFI count is one higher than it was the last time then we now that exactly
        one half buffer was replayed. So we reload one half buffer. If the new HFI count
        increased by two since last time then our reload mechanism was to slow and we have a 
        buffer overrun. This means that one of the halfbuffers was replayed twice. 
        '''
        pymilapi.PyApiCmdReplayStart(handle)

        ################################################
        #                                              #
        # --          Write replay data            --- #
        #                                              #
        ################################################
        ulLastRpiCount = 0
        while True:
            # Write replay data
            xReplayStatus = pymilapi.PyApiCmdReplayStatus( handle )

            if( xReplayStatus.entry_cnt == 0 ):
                print("Replaying ...  %7d entries" % (xReplayStatus.entry_cnt))
                break

            #check for replay overflow
            ulRpiDelta = xReplayStatus.rpi_cnt - ulLastRpiCount

            if ulRpiDelta > 1:
                raise Exception("Error : Replay overflow %d/%d" % (xReplayStatus.rpi_cnt, ulLastRpiCount))

            if ulRpiDelta == 1:
                print("Replaying ...  %7d entries" % (xReplayStatus.entry_cnt))
                # one buffer was completely replayed reload data from file 
                WriteReplayBufferFromFile( pymilapi, handle, pFile )
                ulLastRpiCount = ulLastRpiCount + 1

            time.sleep(0.25)
    
    ################################################
    #                                              #
    # --          Stop replay data             --- #
    #                                              #
    ################################################
    pymilapi.PyApiCmdReplayStop(handle)


def StopRtSimulation(pymilapi, handle):
   print("StopRtSimulation ... done")
   # Stop all RTs 
   pymilapi.PyApiCmdRTHalt(handle)
   # Stop Bus Monitor 
   pymilapi.PyApiCmdBMHalt(handle)


def CheckReceivedData(pymilapi, handle):
    xRtStatus = pymilapi.PyApiCmdRTStatusRead( handle )

    if(    (xRtStatus.glb_msg_cnt != 5000)
        or (xRtStatus.glb_err_cnt != 0   )):
        print("Error : Expected RT to receive %5d transfer got %5d" % (5000, xRtStatus.glb_msg_cnt ))
        print("Error : Expected RT to receive %5d errors   got %5d" % (   0, xRtStatus.glb_err_cnt ))
        raise Exception("Error : Transfer count missmatch")


def Recording(pymilapi, handle, ulStream):
    ulTotalMessageCount = 0

    ulBiu = MilUtilBMGetLogicalBiuFromStream(pymilapi, handle, ulStream)
    if (ulBiu > ucStreamToDataQueueIdMax):
        raise Exception('Invalid biu %d' % (ulBiu))
    # convert stream to data queue id
    ucQueueId = aucStreamToDataQueueId[ulBiu]

    try:
        # Setup RTs and BC 
        PrepareSimulation( pymilapi, handle )
        # Start the simulation and the bus monitor 
        StartRecording( pymilapi, handle, ucQueueId)
        RunSimulation( pymilapi, handle )

        print("Recording :")
        ################################################
        #                                              #
        # --    Open output file in tmp directory  --- #
        #                                              #
        ################################################
        global szGlobalFileName
        with open( szGlobalFileName, "wb" ) as pWriteFile:
            print("Recording file %s" % (szGlobalFileName))

            ################################################
            #                                              #
            # --  Read BM data as long as BC is active --- #
            #                                              #
            ################################################
            while MilUtilIsBcActive( pymilapi, handle ):
                ulDataAvailable = IsQueueDataAvailable(pymilapi, handle, ucQueueId)

                if ulDataAvailable < global_recording_buffer_size:
                    '''   
                        We want to read blocks of 128 KB. 
                        no enough data available       
                        wait some time and check again 
                    '''
                    time.sleep(0.05)
                else:
                    # -- read available data --- 
                    ulMessageCount = ReadData(pymilapi, handle, ucQueueId, pWriteFile, ulDataAvailable)

                    ulTotalMessageCount = ulTotalMessageCount + ulMessageCount

            ################################################
            #                                              #
            # --  Stopp the BM and the recording queue --- #
            #                                              #
            ################################################
            StopRecording( pymilapi, handle, ucQueueId )

            ################################################
            #                                              #
            # --  Read leftovers from recording queue  --- #
            #                                              #
            ################################################
            time.sleep(0.3)

            while(1):
                ulDataAvailable = IsQueueDataAvailable(pymilapi, handle, ucQueueId)

                if  ulDataAvailable == 0:
                    # The simulation and the monitor are stopped 
                    # If the queue is empty we should be done    
                    break

                ulMessageCount = ReadData(pymilapi, handle, ucQueueId, pWriteFile,ulDataAvailable)

                ulTotalMessageCount = ulTotalMessageCount + ulMessageCount

    finally:
        ################################################
        #                                              #
        # --               Clean up                --- #
        #                                              #
        ################################################
        StopSimulation( pymilapi, handle )
        CloseRecording( pymilapi, handle, ucQueueId )

        # Check if the file recorded is ok
        CheckRecording( pymilapi, handle , ulTotalMessageCount )


######################################################################
#                                                                    #
#                                  Replay                            #
#                                                                    #
#                                                                    #
######################################################################
def Replay(pymilapi, handle):
    try:
        StartRtMonitoring(pymilapi, handle)
        Replaying(pymilapi, handle)

    finally:
        StopRtSimulation(pymilapi, handle)
        CheckReceivedData(pymilapi, handle)


########################################################################################
#                                                                                      #
#                                     Sample entry point                               #
#                                                                                      #
########################################################################################
def LsRecordingReplaySample(pymilapi, handle, ulStream):
    print("Running LsRecordingReplaySample")

    # Create tmp file name with path
    global szGlobalFileName
    path = tempfile.gettempdir()
    szGlobalFileName = os.path.join(path, "ls_recording_replay_sample.prc")

    # Recording sample
    Recording( pymilapi, handle, ulStream)

    # Replay sample
    Replay(pymilapi, handle)

