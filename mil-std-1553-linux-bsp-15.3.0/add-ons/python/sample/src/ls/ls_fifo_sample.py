# SPDX-FileCopyrightText: 2023-2024 AIM GmbH <info@aim-online.com>
# SPDX-License-Identifier: MIT

"""This sample shows how to implement a transmittion FIFO on a transmit buffer.
This sample creates a cyclic frame with one BC2RT transfer. 
The transfer has a transmit FIFO assigned. The frame is sent 2000 times with RT enabled. 
The sample polls the FIFO reload size and generates unique transfer data for each FIFO reload. 
At the end of the framing the BC and RT status is printed. 
The transfer count should be as expected and no errors are displayed. 
This sample is executed with internal coupling. No external connection is required.
"""

import time

from aim_mil import mil_bindings_defines as ambd

from .mil_utils import  MilUtilNextRTHid, \
                        MilUtilEnableRT, \
                        MilUtilEnableRTSA, \
                        MilUtilNextBCHid, \
                        MilUtilBCCreateTransfer, \
                        MilUtilBCCreateSingleFrameFraming, \
                        MilUtilIsBcActive

FIFO_BUFFER_COUNT       = 64
uwGlobalFirstDataWord   = 0
uwGlobalLastDataWord    = 0xFFFF


def PrepareSimulation(pymilapi, handle):
    print("PrepareRemoteTerminals ... done")
    auwTransferIds = [1]

    ###############################################
    #                  RT                         #
    #  --      Simulate RT3 receive SA13     ---  #
    #                                             #
    ###############################################
    MilUtilEnableRT(pymilapi, handle, 3, 8.0)
    MilUtilEnableRTSA(pymilapi, handle, 3, 13, ambd.API_RT_TYPE_RECEIVE_SA, MilUtilNextRTHid())

    ###############################################
    #                   BC                        #
    #  --  Initialize the Bus Controller     ---  #
    #                                             #
    ###############################################
    pymilapi.PyApiCmdBCIni(handle)

    print()
    print(  "Framing :     2000 times \n",
            "------------------------ \n",
            "BC         -> RT3RcvSA13 \n",
            "------------------------ \n",
            "    total 2000 transfers \n" )

    print("PrepareBusController ... done")

    ###############################################
    #                     BC                      #
    #  --            BC -> RT3RcvSA13        ---  #
    #                                             #
    ###############################################
    MilUtilBCCreateTransfer(pymilapi, handle,
                            auwTransferIds[0],   # Transfer Id
                            ambd.API_BC_TYPE_BCRT,
                            0,  # Transmit RT
                            0,  # Transmit SA
                            3,  # Receive  RT
                            13, # Receive  SA
                            0,  # Word count 32
                            MilUtilNextBCHid())

    ###############################################
    #                      BC                     #
    #  --      Setup Major/Minor Framing  ---     #
    #                                             #
    ###############################################
    MilUtilBCCreateSingleFrameFraming(pymilapi, handle, auwTransferIds)


def RunSimulation(pymilapi, handle):
    print("RunSimulation ... running")

    ###############################################
    #                                             #
    #  --       Start all enabled RTs        ---  #
    #                                             #
    ###############################################
    pymilapi.PyApiCmdRTStart( handle )
    
    ###############################################
    #                                             #
    #  --         Start Bus Controller       ---  #
    #                                             #
    ###############################################
    #    ApiCmdBCStart(   module,    0, mode,                       count, mframe time )
    pymilapi.PyApiCmdBCStart( handle, ambd.API_BC_START_IMMEDIATELY, 2000,  10 )


def PutDataInFifo(pymilapi, handle, uwReloadSize):
    global uwGlobalFirstDataWord
    global uwGlobalLastDataWord
    auwMessageData = [0] * uwReloadSize
    for i in range(int(uwReloadSize/32)):
        # Prepare data for the next FIFO half buffer
        auwMessageData[i*32 + 0 ] = uwGlobalFirstDataWord
        uwGlobalFirstDataWord = uwGlobalFirstDataWord + 1

        auwMessageData[i*32 + 31] = uwGlobalLastDataWord
        uwGlobalLastDataWord = uwGlobalLastDataWord - 1

    print("0x%X>" % (uwReloadSize), end="", flush=True )

    # write the next FIFO half buffer to the device
    pymilapi.PyApiCmdFifoWrite( handle, 1, uwReloadSize, auwMessageData )


def InitializeFifo(pymilapi, handle):
    # Initialize Fifo Queue
    pymilapi.PyApiCmdFifoIni( handle, 1, FIFO_BUFFER_COUNT )

    pymilapi.PyApiCmdBCAssignFifo( handle, ambd.API_ENA, 1, 1 )

    ''' The FIFO can only be reloaded in blocks of half buffers
        As soon as the FIFO transmitted half of its data the command
        ApiCmdFifoReadStatus will return a status containing the amount
        of AiUInt16 words that can be realoaded into the first half buffer
        This half buffer must then be realoaded as one block with the command
        ApiCmdFifoWrite. '''

    # Fill first half buffer of the fifo with initial data
    PutDataInFifo( pymilapi, handle, int(FIFO_BUFFER_COUNT/2)*32 )

    # Fill second half buffer of the fifo with initial data
    PutDataInFifo( pymilapi, handle, int(FIFO_BUFFER_COUNT/2)*32 )


def StopSimulation(pymilapi, handle):
    print("StopSimulation ... done")

    # Stop Bus Controller
    pymilapi.PyApiCmdBCHalt(handle)
    # Stop all RTs
    pymilapi.PyApiCmdRTHalt(handle)


def PrintSimulationStatistic(pymilapi, handle):
    print("PrintSimulationStatistic ...")

    xBcStatus = pymilapi.PyApiCmdBCStatusRead( handle )
    print("BC  ( %d messages / %d errors )" % (xBcStatus.glb_msg_cnt, xBcStatus.glb_err_cnt) )

    xRtStatus = pymilapi.PyApiCmdRTStatusRead( handle )
    print("RTs ( %d messages  / %d errors )" % (xRtStatus.glb_msg_cnt, xRtStatus.glb_err_cnt) )

    print("Please note that one RT to RT transfer counts as two messages in RTs and BM")
    print("PrintSimulationStatistic ... done")


def LsFifoSample(pymilapi, handle):
    print("Running LsFifoSample")

    uwRelaodSize = 0

    try:
        # Setup RTs and BC
        PrepareSimulation(pymilapi, handle)
        # Setup the Fifo and write initialial data to it
        InitializeFifo(pymilapi, handle)
        # Start the simulation and the bus monitor
        RunSimulation(pymilapi, handle)

        while MilUtilIsBcActive(pymilapi, handle):
            uwRelaodSize = pymilapi.PyApiCmdFifoReadStatus( handle, 1 )

            if( uwRelaodSize == 0xFFFF ):
                raise Exception("Error: Reload underrun.")

            if( uwRelaodSize != 0 ):
                # One half buffer can be relaoded
                PutDataInFifo( pymilapi, handle, int(uwRelaodSize) )
            else:
                # Relax CPU
                time.sleep(0.01)
        print()

    finally:        
        # Stop the simulation and the bus monitor
        StopSimulation( pymilapi, handle )
        PrintSimulationStatistic( pymilapi, handle )
