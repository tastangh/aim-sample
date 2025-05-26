# SPDX-FileCopyrightText: 2023-2024 AIM GmbH <info@aim-online.com>
# SPDX-License-Identifier: MIT

"""This sample shows how to to send different transfer types and record them with the BM.
This sample creates a framing with different transfer types (BC2RT, RT2BC, RT2RT, MC)
including mode codes and runs the framing with BC, RT and BM active. 
The BC, RT and BM status as well as the transmitted data words are checked for errors. 
This sample is executed with internal coupling. No external connection is required.
"""

import time

from aim_mil import mil_bindings_defines as ambd

from .mil_utils import MilUtilNextRTHid, \
                        MilUtilEnableRT, \
                        MilUtilEnableRTSA, \
                        MilUtilNextBCHid, \
                        MilUtilSetData, \
                        MilUtilBCCreateTransfer, \
                        MilUtilBCCreateSingleFrameFraming, \
                        MilUtilIsBcActive

def PrepareRemoteTerminals(pymilapi, handle):
    print('PrepareRemoteTerminals ... done')
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
    # --       Simulate RT6 transmit SA16   --- #
    #                                           #
    #############################################
    uwHeaderId = MilUtilNextRTHid()
    MilUtilEnableRT(pymilapi, handle, 6 , 8.0 )
    MilUtilEnableRTSA(pymilapi, handle, 6 , 16 , ambd.API_RT_TYPE_TRANSMIT_SA, uwHeaderId)

    # Data to send from the RT6 TxSA16
    auwMessageData = 32 * [0]
    for i in range(32):
        auwMessageData[i] = 0xCE00 + i + 1

    MilUtilSetData(pymilapi, handle, uwHeaderId, ambd.API_BUF_RT_MSG, 1 , auwMessageData)

    #############################################
    #                                           #
    # --  Simulate RT6 receive modecode 2   --- #
    #                                           #
    #############################################
    uwHeaderId = MilUtilNextRTHid()
    # RT6 is already configured above
    MilUtilEnableRTSA(pymilapi, handle, 6 , 2 , ambd.API_RT_TYPE_TRANSMIT_MODECODE, uwHeaderId)


def PrepareBusController(pymilapi, handle):
    auwTransferIds = [ 1, 2, 3, 4 ]
    uwHeaderId    = 0
    #############################################
    #                                           #
    # --  Initialize the Bus Controller     --- #
    #                                           #
    #############################################
    pymilapi.PyApiCmdBCIni( handle, 0, 0, 0, 0 )

    print()
    print(  "Framing :     1000 times \n",
            "------------------------ \n",
            "BC         -> RT3RcvSA13 \n",
            "RT6XmtSA16 -> BC         \n",
            "RT6XmtSA16 -> RT3RcvSA13 \n",
            "RT6XmtMC02 -> BC         \n",
            "------------------------ \n",
            "    total 4000 transfers \n" )

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
                            uwHeaderId)

    # Data to send to the RT #
    auwMessageData = 32 * [0]
    for i in range (32):
        auwMessageData[ i ] = 0xBC00 + i + 1

    MilUtilSetData(pymilapi, handle, 
                    uwHeaderId, 
                    ambd.API_BUF_BC_MSG, 
                    1, #First buffer in queue*/
                    auwMessageData)

    #############################################
    #                                           #
    # --          RT6XmtSA16 -> BC          --- #
    #                                           #
    #############################################
    uwHeaderId   = MilUtilNextBCHid()

    MilUtilBCCreateTransfer( pymilapi, handle, 
                            auwTransferIds[1],
                            ambd.API_BC_TYPE_RTBC, 
                            6,  # Transmit RT 
                            16, # Transmit SA 
                            0,  # Receive  RT 
                            0,  # Receive  SA 
                            0,  # Word count 32 
                            uwHeaderId)

    #############################################
    #                                           #
    # --     RT6XmtSA16 -> RT3RcvSA13       --- #
    #                                           #
    #############################################
    uwHeaderId   = MilUtilNextBCHid()

    MilUtilBCCreateTransfer( pymilapi, handle, 
                            auwTransferIds[2],
                            ambd.API_BC_TYPE_RTRT, 
                            6,   # Transmit RT 
                            16,  # Transmit SA 
                            3,   # Receive  RT 
                            13,  # Receive  SA 
                            0,   # Word count 32 
                            uwHeaderId)

    #############################################
    #                                           #
    # --           RT6XmtMC02 -> BC         --- #
    #                                           #
    #############################################
    uwHeaderId   = MilUtilNextBCHid()

    MilUtilBCCreateTransfer( pymilapi, handle, 
                            auwTransferIds[3],
                            ambd.API_BC_TYPE_RTBC, 
                            6,   # Transmit RT 
                            0,   # Transmit SA 
                            0,   # Receive  RT 
                            0,   # Receive  SA 
                            2,   # Mode Code  
                            uwHeaderId)

    #############################################
    #                     BC                    #
    # --      Setup Major/Minor Framing  ---    #
    #                                           #
    #############################################
    MilUtilBCCreateSingleFrameFraming(pymilapi, handle, auwTransferIds)


def PrepareMonitoring(pymilapi, handle):
    print("PrepareMonitoring ... done")

    pymilapi.PyApiCmdQueueIni( handle )
    pymilapi.PyApiCmdBMIntrMode( handle )


def RunSimulation(pymilapi, handle):
    print('RunSimulation ... running')

    #############################################
    #                                           #
    # --          Start Bus Monitor         --- #
    #                                           #
    #############################################
    pymilapi.PyApiCmdQueueStart(handle)

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
    '''
        BC start modes :
        1  API_BC_START_IMMEDIATELY            Start immediately
        2  API_BC_START_EXT_TRIGGER            Start by external BC trigger input
        3  API_BC_START_RT_MODECODE            Start by RT Modecode Dynamic Bus Control
        4  API_BC_START_EXTERNAL_PULSE         Start on external pulse (BC trigger input)
        5  API_BC_START_SETUP                  Prepare for API_BC_START_FAST
        6  API_BC_START_FAST                   Start fast defined with API_BC_START_SETUP.
        7  API_BC_START_INSTR_TABLE            Start with Instruction Table
        8  API_BC_START_CONTINUE_ON_BC_HALT    Start after BC halt (not possible on AMC1553-4)
    '''
    pymilapi.PyApiCmdBCStart( handle, ambd.API_BC_START_IMMEDIATELY, 1000,  10.0 )


def StopSimulation(pymilapi, handle):

    print('StopSimulation ... done')

    # Stop Bus Controller
    pymilapi.PyApiCmdBCHalt(handle)

    # Stop all RTs
    pymilapi.PyApiCmdRTHalt(handle)

    # Stop Bus Monitor
    pymilapi.PyApiCmdQueueHalt(handle)


def CheckTransfer(ulTransferNo, xMessage):
    '''
    Framing : 
       ------------------------
       BC         -> RT3RcvSA13
       RT6XmtSA16 -> BC
       RT6XmtSA16 -> RT3RcvSA13
       RT6XmtMC02 -> BC
    '''
    aucExpectedRts           = [ 3,      6,      3,      6 ]
    aucExpectedRts2          = [ 0,      0,      6,      0 ]
    auwExpectedFirstDataWord = [ 0xBC01, 0xCE01, 0xCE01, 0 ]
    if( aucExpectedRts[ ulTransferNo % 4 ] != xMessage.rt_addr ):
        print('Expected RT %d got %d at transfer %d' % (
                                               aucExpectedRts[ ulTransferNo % 4 ],
                                               xMessage.rt_addr,
                                               ulTransferNo ))
        return 1

    if( aucExpectedRts2[ ulTransferNo % 4 ] != xMessage.rt_addr2 ):
        print('Expected RT (2) %d got %d at transfer %d' % (
                                               aucExpectedRts2[ ulTransferNo % 4 ],
                                               xMessage.rt_addr2,
                                               ulTransferNo ))
        return 1

    if( xMessage.word_cnt > 1 ):
    # Skip MC2 because it has no data
        if( auwExpectedFirstDataWord[ ulTransferNo % 4 ] != xMessage.buffer[0] ):
            print('Expected first data word 0x%04X got 0x%04X at transfer %d' % (
                                                   auwExpectedFirstDataWord[ ulTransferNo % 4 ],
                                                   xMessage.buffer[0],
                                                   ulTransferNo ))
            return 1
    return 0


def Monitor(pymilapi, handle):
    ulBmMessageCount = 0
    print('Monitor ... ')

    ################################################
    #                                              #
    # --           Wait for BC start           --- #
    #                                              #
    ################################################
    while not MilUtilIsBcActive(pymilapi, handle ): continue

    ################################################
    #                                              #
    # --  Read BM data as long as BC is active --- #
    #                                              # 
    ################################################
    while( True ):
        xQueueMsg = pymilapi.PyApiCmdQueueRead( handle )

        if( xQueueMsg == None ):
            # No data available #
            if MilUtilIsBcActive(pymilapi, handle):
                # The BC is active so we may get data the next time #
                time.sleep(0.2)
            else:
                break
        else:
            # Received data #
            ret = CheckTransfer( ulBmMessageCount, xQueueMsg )

            if( ret != 0 ):
                break

            ulBmMessageCount = ulBmMessageCount + 1

        if ((ulBmMessageCount % 500) == 0):
            print("Bus Monitor received %d transfers" % (ulBmMessageCount))
        
    print()

    if( ulBmMessageCount == 4000 ):
        print('Monitor ... done')
    else:
        print('Expected to receive %d transfers got %d' % ( 4000, ulBmMessageCount ))
        print('Monitor ... failed')


def PrintSimulationStatistic(pymilapi, handle):
    print('PrintSimulationStatistic ...')

    xBcStatus = pymilapi.PyApiCmdBCStatusRead( handle )
    print('BC  ( %d messages / %d errors )' % (xBcStatus.glb_msg_cnt, xBcStatus.glb_err_cnt ))

    xRtStatus = pymilapi.PyApiCmdRTStatusRead( handle)
    print('RTs ( %d messages  / %d errors )' % (xRtStatus.glb_msg_cnt, xRtStatus.glb_err_cnt ))

    xBmStatus = pymilapi.PyApiCmdBMStatusRead( handle)
    print('BM  ( %d messages  / %d errors / %.2f%% avg bus load )' % (xBmStatus.glb_msg_cnt, xBmStatus.glb_err_cnt, xBmStatus.bus_load_pri_avg * 0.01 ))

    print('Please note that one RT to RT transfer counts as two messages in RTs and BM')
    print('PrintSimulationStatistic ... done')

########################################################################################
#                                                                                      #
#                                     Sample entry point                               #
#                                                                                      #
########################################################################################
def LsBcRtBmSample(pymilapi, handle):
    print('Running LsBcRtBmSample')
    try:
        # Setup RTs and BC #
        PrepareMonitoring(pymilapi, handle )
        PrepareRemoteTerminals(pymilapi, handle )
        PrepareBusController(pymilapi, handle )
        # Start the simulation and the bus monitor #
        RunSimulation(pymilapi, handle )
        # Read Bus Monitor data as long as the BC is active #
        Monitor(pymilapi, handle )

    finally:
        # Stop the simulation and the bus monitor #
        StopSimulation(pymilapi, handle )
        PrintSimulationStatistic(pymilapi, handle )



