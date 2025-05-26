# SPDX-FileCopyrightText: 2023-2024 AIM GmbH <info@aim-online.com>
# SPDX-License-Identifier: MIT

"""This sample shows how to monitor a 1553 bus with external bus coupling.
This sample monitors and displays all traffic on the external bus. 
The sample needs a bus with transformer termination and external bus traffic.
"""

import time
import tempfile
import os

from aim_mil import mil_bindings_defines as ambd
from aim_mil import mil_bindings_types as ambt

from .mil_utils import MilUtilBMLsShowData, \
                        MilUtilBMGetLogicalBiuFromStream, \
                        MilUtilOsKbHit

# Global constants
szGlobalFileName = None
global_recording_buffer_size = (128*1024) # Read  blocks of 128 KB 


#######################################################################
#                                                                     #
#                                                                     #
#                            Recording                                #
#                                                                     #
#                                                                     #
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


def StartRecording(pymilapi, handle, ucQueueId, ucUseAns):
    print('StartRecording ... done')
    # Setup Bus Monitor for Low speed Recording #
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
    # The BM should already be stopped.
    # This stop is only required in case of an error during the recording process.
    pymilapi.PyApiCmdBMHalt( handle )
    pymilapi.PyApiCmdDataQueueControl(handle, ucQueueId, ambd.API_DATA_QUEUE_CTRL_MODE_STOP)


def CloseRecording(pymilapi, handle, ucQueueId):
    print('CloseRecording ... done')
    pymilapi.PyApiCmdDataQueueClose(handle, ucQueueId )


def IsQueueDataAvailable(pymilapi, handle, ucQueueId):
    # Read 0 bytes to check if data is available
    _, xDataQueuStatus = pymilapi.PyApiCmdDataQueueRead(handle,  ambt.PY_TY_API_DATA_QUEUE_READ(ucQueueId, 0))
    return xDataQueuStatus.bytes_in_queue


def ReadData(pymilapi, handle, ucQueueId, pFile, ulDataAvailable):
    aucRecordingBuffer , xDataQueuStatus = pymilapi.PyApiCmdDataQueueRead(handle, ambt.PY_TY_API_DATA_QUEUE_READ(ucQueueId, ulDataAvailable))

    if xDataQueuStatus.status & 0xFFFF:
        print('Error : ApiCmdDataQueueRead failed with status 0x%X' % (xDataQueuStatus.status) )
        return 1

    # Save buffer to file
    pFile.write(aucRecordingBuffer)

    MilUtilBMLsShowData(aucRecordingBuffer)


def Recording(pymilapi, handle, ulStream, ucUseAns):
    ulBiu = MilUtilBMGetLogicalBiuFromStream(pymilapi, handle, ulStream)

    if (ulBiu > ucStreamToDataQueueIdMax):
        print('Invalid biu %d' % ulBiu)
        return 1

    # convert stream to data queue id
    ucQueueId = aucStreamToDataQueueId[ulBiu]

    # Start the bus monitor
    StartRecording(pymilapi, handle, ucQueueId, ucUseAns )

    print('Recording :')
    try:

        ################################################
        #                                              #
        # --    Open output file in tmp directory  --- #
        #                                              #
        ################################################
        global szGlobalFileName
        with open( szGlobalFileName, "wb" ) as pWriteFile:
            ######################################################
            #                                                    #
            # --  Read BM data as long a Key has been pressed--- #
            #                                                    #
            ######################################################
            print('Press return to stop recording')
            timeout_counter = 0
            global global_recording_buffer_size
            while( not MilUtilOsKbHit() ):
                ulDataAvailable = IsQueueDataAvailable(pymilapi, handle, ucQueueId)

                if (timeout_counter < 10):
                    if (ulDataAvailable < global_recording_buffer_size):
                        # We want to read blocks > 128 KB. 
                        # no enough data available          
                        # wait some time and check again 
                        time.sleep(.05)
                        timeout_counter = timeout_counter + 1
                else:
                        timeout_counter = 0

                if (ulDataAvailable > 0 ):
                    # -- read available data ---
                    ReadData(pymilapi, handle, ucQueueId, pWriteFile,ulDataAvailable)

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

            time.sleep(0.300)

            while True:
                ulDataAvailable = IsQueueDataAvailable( pymilapi, handle, ucQueueId )

                if( ulDataAvailable == 0 ):
                    # The simulation and the monitor are stopped
                    # If the queue is empty we should be done
                    break

                ReadData( pymilapi, handle, ucQueueId, pWriteFile, ulDataAvailable)
    finally:
        ################################################
        #                                              #
        # --               Clean up                --- #
        #                                              #
        ################################################
        StopRecording( pymilapi, handle, ucQueueId )
        CloseRecording( pymilapi, handle, ucQueueId )


########################################################################################
#                                                                                      #
#                                     Sample entry point                               #
#                                                                                      #
########################################################################################
def LsBmSample(pymilapi, handle, ulStream, ucUseAns):
    print("Running LsBmSample")

    # Create tmp file name with path
    global szGlobalFileName
    path = tempfile.gettempdir()
    szGlobalFileName = os.path.join(path, "ls_bm_sample.prc")

    # Recording sample #
    Recording( pymilapi, handle, ulStream, ucUseAns )



