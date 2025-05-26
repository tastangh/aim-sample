# SPDX-FileCopyrightText: 2023-2024 AIM GmbH <info@aim-online.com>
# SPDX-License-Identifier: MIT

"""This sample shows how to implement FW based Dytags and TSW based Systags.
This sample creates a cyclic frame with two BC2RT transfers. 
The first transfer has two dynamic data tags (ApiCmdBCDytagDef) enabled in order 
to enable an automatic firmware counter on the first and the last data word. 
The second transfer has a Systag (ApiCmdSystagDef) enabled 
to send the data based on a data set. 
The framing is started with the RT enabled and a total of 2000 transfers. 
At the end of the framing the BC status is checked to ensure 
the correct transfer count has been send and no errors occurred. 
This test is executed with internal coupling. No external connection is required.
"""

import time

from aim_mil import mil_bindings_defines as ambd
from aim_mil import mil_bindings_types as ambt

from .mil_utils import  MilUtilNextRTHid, \
                        MilUtilEnableRT, \
                        MilUtilEnableRTSA, \
                        MilUtilNextBCHid, \
                        MilUtilBCCreateTransfer, \
                        MilUtilBCCreateSingleFrameFraming, \
                        MilUtilIsBcActive

def PrepareRemoteTerminals(pymilapi, handle):
    print("PrepareRemoteTerminals ... done")
    ###############################################
    #                                             #
    #  -- Simulate RT3 receive SA13 and SA15 ---  #
    #                                             #
    ###############################################
    MilUtilEnableRT(pymilapi, handle, 3, 8.0)
    MilUtilEnableRTSA(pymilapi, handle, 3, 13, ambd.API_RT_TYPE_RECEIVE_SA, MilUtilNextRTHid())
    MilUtilEnableRTSA(pymilapi, handle, 3, 15, ambd.API_RT_TYPE_RECEIVE_SA, MilUtilNextRTHid())


def PrepareDytagOnDataBuffer(pymilapi, handle, uwHid):
    '''
       Dytags are handled in the firmware. A maximum of 4 Dytags can be
       assigned to each data buffer. This means only 4 words of each buffer
       may be automatically manipulated by the firmware. If more than 4 words
       of a buffer need to be manipulated Systags may be used.

       See description of ApiCmdBCDytagDef in the reference manual for 
       more details about Dytags.
    ''' 

    # The dytag array must alway have a size of 4
    axDytag = [ambt.PY_TY_API_BC_DYTAG() for _ in range(4)]

    ###############################################
    #                                             #
    #  --  Configure Dytag 1 in data word 1  ---  #
    #                                             #
    ###############################################
    '''
        This Dytag will create the following progression in buffer word 1 :
        The start value is set to 0 in the function PrepareBusController()

        0, 1, 2, 3, ... 100, 99, 98, 97, ... 51, 50, 51, 52 ... 99, 100, 99, 98 ... 51, 50, 51, ...
        start value  ... max              ...    min        ...     max         ...     min
    '''
    axDytag[ 0 ].tag_fct = ambd.API_DYTAG_FCT_POS_TRIANGLE
    # Lower Limit
    axDytag[ 0 ].min = 50
    # Upper Limit
    axDytag[ 0 ].max = 100
    # Step size
    axDytag[ 0 ].step = 1
    # Maniputlate first data word
    axDytag[ 0 ].wpos = 0

    ###############################################
    #                                             #
    #  --  Configure Dytag 2 in data word 32 ---  #
    #                                             #
    ###############################################
    '''
        This Dytag will create the following progression in buffer word 32 :
        The start value is set to 0 in the function PrepareBusController()

        0, 1, 2, 3,  ... 90,  40, 41, 42, ... 90,   40, 41, 42 ... 90, 40, ...
        start value  ... max, min         ... max, min         ... max, min 
    '''
    axDytag[ 1 ].tag_fct = ambd.API_DYTAG_FCT_POS_RAMP
    axDytag[ 1 ].min     = 40
    axDytag[ 1 ].max     = 90
    axDytag[ 1 ].step    = 1
    # Manipulate last data word
    axDytag[ 1 ].wpos    = 31 

    ###############################################
    #                                             #
    #  --  Configure Dytag 3/4 disabled  ---      #
    #                                             #
    ###############################################
    axDytag[ 2 ].tag_fct = ambd.API_DYTAG_FCT_DISABLE
    axDytag[ 3 ].tag_fct = ambd.API_DYTAG_FCT_DISABLE

    ###############################################
    #                                             #
    #  --       Install Dytag on board       ---  #
    #                                             #
    ###############################################
    pymilapi.PyApiCmdBCDytagDef( handle, ambd.API_ENA, uwHid, ambd.API_DYTAG_STD_MODE, axDytag )


def PrepareSystagOnDataBuffer(pymilapi, handle, uwXid):
    '''
       Systags are handled in the onboard software. Systags may be used 
       if Dytags are not sufficient. A maximum of 255 Systags can be handled.
       See description of ApiCmdSystagDef in the reference manual for a 
       description of the different Systag types.
    '''

    ######################################################
    #                                                    #
    #  -- Prepare 3 data buffers to send alternatly ---  #
    #                                                    #
    ######################################################

    # Dataset Write (buffer 1)
    pymilapi.PyApiCmdRamWriteDataset( handle, 0, [0xDD01] * 32 )

    # Dataset Write (buffer 2)
    pymilapi.PyApiCmdRamWriteDataset( handle, 1 , [0xDD02] * 32 )

    # Dataset Write (buffer 3)
    pymilapi.PyApiCmdRamWriteDataset( handle, 2 , [0xDD03] * 32 )

    ###################################################
    #                                                 #
    #  -- Configure Systag 1 for transfer uwXid  ---  #
    #                                                 #
    ###################################################
    xSystag = ambt.PY_TY_API_SYSTAG()
    # Transfer ID
    xSystag.xid_rtsa = uwXid
    # Systag type
    xSystag.fct      = ambd.API_SYSTAG_FCT_DATASET
    # Dataset: buffer start ID
    xSystag.min      = 0
    # Dataset: amount of buffers
    xSystag.max      = 3
    xSystag.step     = ambd.API_SYSTAG_STEP_CYCLIC
    xSystag.wpos     = 0
    xSystag.bpos     = 0

    ###############################################
    #                                             #
    #  --       Install Systag on board      ---  #
    #                                             #
    ###############################################
    pymilapi.PyApiCmdSystagDef( handle, 1, ambd.API_ENA, ambd.API_BC_MODE, xSystag )


def PrepareBusController(pymilapi, handle):
    auwTransferIds = [ 1, 2 ]

    pymilapi.PyApiCmdBCIni( handle )

    print()
    print(  "Framing :             1000 times \n",
            "-------------------------------- \n",
            "BC         -> RT3RcvSA13 (Dytag )\n",
            "BC         -> RT3RcvSA13 (Systag)\n",
            "-------------------------------- \n",
            "            total 2000 transfers \n" )

    print("PrepareBusController ... ")


    ###############################################
    #                                             #
    #  --       BC -> RT3RcvSA13 (Dytag)     ---  #
    #                                             #
    ###############################################
    uwHeaderId   = MilUtilNextBCHid()

    MilUtilBCCreateTransfer(pymilapi, handle,
                            auwTransferIds[0],
                            ambd.API_BC_TYPE_BCRT,
                            0,  # Transmit RT
                            0,  # Transmit SA
                            3,  # Receive  RT
                            13, # Receive  SA
                            0,  # Word count 32
                            uwHeaderId)

    # Setup Dytag for this transfer
    PrepareDytagOnDataBuffer(pymilapi, handle, uwHeaderId)

    ###############################################
    #                                             #
    #  --      BC -> RT3RcvSA15 (Systag)     ---  #
    #                                             #
    ###############################################
    uwHeaderId   = MilUtilNextBCHid()

    MilUtilBCCreateTransfer(pymilapi, handle,
        auwTransferIds[1],
        ambd.API_BC_TYPE_BCRT,
        0,  # Transmit RT
        0,  # Transmit SA
        3,  # Receive  RT
        15, # Receive  SA
        0,  # Word count 32
        uwHeaderId,
        ambd.API_QUEUE_SIZE_1,
        ambd.API_BC_TIC_NO_INT)

    # Setup Systag for this transfer
    PrepareSystagOnDataBuffer(pymilapi, handle, auwTransferIds[1])

    ###############################################
    #                      BC                     #
    #  --      Setup Major/Minor Framing  ---     #
    #                                             #
    ###############################################
    MilUtilBCCreateSingleFrameFraming(pymilapi, handle, auwTransferIds)


def RunSimulation(pymilapi, handle):
    print("RunSimulation ... running")

    # Start all enabled RTs
    pymilapi.PyApiCmdRTStart( handle )

    # Start BC
    # ApiCmdBCStart(   module, 0, mode,                              count, mframe time )
    pymilapi.PyApiCmdBCStart( handle, ambd.API_BC_START_IMMEDIATELY, 1000,  10.0 )


def PollStatus(pymilapi, handle):
    scale = 1

    while not MilUtilIsBcActive(pymilapi, handle): continue # wait for BC start

    while MilUtilIsBcActive(pymilapi, handle):
        # Read status while BC is active
        status = pymilapi.PyApiCmdBCStatusRead( handle )

        if status.glb_msg_cnt >= scale*100 :
            print("BC message count %d" % (status.glb_msg_cnt))
            scale = scale + 1
        time.sleep(0.500)

    # Read status after BC stop
    status = pymilapi.PyApiCmdBCStatusRead( handle )

    print("BC message count %d" % (status.glb_msg_cnt))

    if (status.glb_msg_cnt != 2000) or (status.glb_err_cnt != 0):
        raise Exception("Error : expected to receive 2000 transfers and 0 errors.")


def StopSimulation(pymilapi, handle):
    print("StopSimulation ... done")

    # Stop Bus Controller
    pymilapi.PyApiCmdBCHalt( handle )

    # Stop all RTs
    pymilapi.PyApiCmdRTHalt( handle )


def PrintSimulationStatistic(pymilapi, handle):
    print("PrintSimulationStatistic ...")

    status = pymilapi.PyApiCmdBCStatusRead( handle )
    print("BC  ( %d messages  / %d errors )" % (status.glb_msg_cnt, status.glb_err_cnt) )
    print("PrintSimulationStatistic ... done")


def LsDynamicDataSample(pymilapi, handle):
    print("Running LsDynamicDataSample")

    try:
        # Setup RTs and BC
        PrepareRemoteTerminals( pymilapi, handle )
        PrepareBusController( pymilapi, handle )
        # Start the simulation and the bus monitor
        RunSimulation( pymilapi, handle )
        # Poll and print status as long as the BC is active
        PollStatus( pymilapi, handle )

    finally:
        # Stop the simulation and the bus monitor
        StopSimulation( pymilapi, handle )
        PrintSimulationStatistic( pymilapi, handle )
