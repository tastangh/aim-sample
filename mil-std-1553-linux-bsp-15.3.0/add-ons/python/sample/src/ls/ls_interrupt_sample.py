# SPDX-FileCopyrightText: 2023-2024 AIM GmbH <info@aim-online.com>
# SPDX-License-Identifier: MIT

"""This sample shows how to handle user event callbacks to reload a trasmit data buffer.
This sample creates a cyclic frame with one BC2RT transfer. 
The transfer has a buffer queue with 2 buffers enabled on the transmitter side. 
The transfer has the user interrupt callback enabled. 
The frame is sent 1000 times with RT and BM enabled. 
Each time the transfer is send the firmware emits a user interrupt. 
The sample interrupt callback handler checks the interrupt source and updates the transmit data in the inactive buffer. 
This shows how to ensure transmit buffer data consistency. 
The BM is used to continuously read the transmitted data and check the first and last data word of each transfer. 
At the end of the framing the BC, RT status and the received interrupt count is printed. 
The transfer count and interrupt count should be as expected and no errors are displayed. 
This test is executed with internal coupling. No external connection is required.
"""


import time

from aim_mil import mil_bindings_defines as ambd
from aim_mil import mil_bindings_types as ambt

from .mil_utils import MilUtilNextRTHid, \
                        MilUtilEnableRT, \
                        MilUtilEnableRTSA, \
                        MilUtilNextBCHid, \
                        MilUtilSetData, \
                        MilUtilBCCreateTransfer, \
                        MilUtilBCCreateSingleFrameFraming, \
                        MilUtilIsBcActive

class AimDeviceEventHandler:
    def __init__(self, pymilapi, handle, biu=ambd.API_INT_LS, type=ambd.API_INT_BC):
        self._pymilapi    = pymilapi
        self._handle      = handle
        self._biu         = biu
        self._type        = type
        self._callback    = ambd.TY_INT_FUNC_PTR(self.__callback_handler)
        self.uwGlobalDataCounter = 1
        self.ucBufferQueueSelect = 1
        self.ulIntCounter        = 0
        self.ulBufAddr = [0,0]

    def __callback_handler(self, handle, ucBiu, ucType, ll_address):
        pxInfo = ambt.PY_TY_API_INTR_LOGLIST_ENTRY.fromPointer(ll_address)

        # handle only BC interrupts with PGI and UDF set
        if (pxInfo.ul_Lla & 0xffff0000) !=  0x30400000:
            return

        self.ulIntCounter = self.ulIntCounter + 1

        '''
        When the first interrupt occures we update the first buffer because the BC 
        is currently sending buffer two. When the second interrupt occures the BC is sending
        buffer one again and we update buffer two. 
        '''
        self.ucBufferQueueSelect = 0 if (self.ucBufferQueueSelect == 1) else 1

        # Update data
        auwData = [0] * 32
        for  i in range(32):
            auwData[i] = self.uwGlobalDataCounter
            self.uwGlobalDataCounter = self.uwGlobalDataCounter + 1

        self._pymilapi.PyApiWriteBlockMemData( self._handle, ambd.API_MEMTYPE_GLOBAL, self.ulBufAddr[self.ucBufferQueueSelect], 2, auwData, 32 )

    def install(self):
        self._pymilapi.PyApiInstIntHandler(self._handle, self._biu, self._type, self._callback)

    def remove(self):
        self._pymilapi.PyApiDelIntHandler( self._handle, self._biu, self._type)


def PrepareRemoteTerminals(pymilapi, handle):
    print("PrepareRemoteTerminals ... done")
    # -- Simulate RT3 receive SA13 ---
    MilUtilEnableRT(pymilapi, handle, 3, 8.0)
    MilUtilEnableRTSA(pymilapi, handle, 3, 13, ambd.API_RT_TYPE_RECEIVE_SA, MilUtilNextRTHid() )


def PrepareBusController(pymilapi, handle, event):
    auwTransferIds = [1]

    # Initialize the Bus Controller
    pymilapi.PyApiCmdBCIni( handle )

    # Framing
    print()
    print(  "Framing :     1000 times \n",
            "------------------------ \n",
            "BC         -> RT3RcvSA13 \n",
            "------------------------ \n",
            "    total 1000 transfers \n" )

    print("PrepareBusController ... done")

    # -- BC -> RT3RcvSA13 --
    uwHeaderId   = MilUtilNextBCHid()

    MilUtilBCCreateTransfer(pymilapi, handle,
        auwTransferIds[0],
        ambd.API_BC_TYPE_BCRT,
        0,  # Transmit RT
        0,  # Transmit SA
        3,  # Receive  RT
        13, # Receive  SA
        0,  # Word count 32
        uwHeaderId,
        ucQueueSize=ambd.API_QUEUE_SIZE_2,
        ulInterruptControl=ambd.API_BC_TIC_INT_ON_XFER_END )

    # Initial data to send in buffer one
    auwMessageData = [0] * 32
    for i in range(32):
        auwMessageData[ i ] = event.uwGlobalDataCounter
        event.uwGlobalDataCounter = event.uwGlobalDataCounter + 1

    event.ulBufAddr[0] = MilUtilSetData(pymilapi, handle, uwHeaderId, ambd.API_BUF_BC_MSG, 1, auwMessageData)

    # Initial data to send in buffer two
    for i in range(32):
        auwMessageData[ i ] = event.uwGlobalDataCounter
        event.uwGlobalDataCounter = event.uwGlobalDataCounter + 1

    event.ulBufAddr[1] = MilUtilSetData(pymilapi, handle, uwHeaderId, ambd.API_BUF_BC_MSG, 2, auwMessageData)

    ###############################################
    #                      BC                     #
    #  --      Setup Major/Minor Framing  ---     #
    #                                             #
    ###############################################
    MilUtilBCCreateSingleFrameFraming(pymilapi, handle, auwTransferIds)


def PrepareMonitoring(pymilapi, handle):
    print("PrepareMonitoring ... done")
    pymilapi.PyApiCmdQueueIni( handle )
    pymilapi.PyApiCmdBMIntrMode(handle)


def RunSimulation(pymilapi, handle):
    print("RunSimulation ... running")

    pymilapi.PyApiCmdQueueStart( handle )
    pymilapi.PyApiCmdRTStart( handle )

    #        ApiCmdBCStart(   module, mode,                          count, mframe time )
    pymilapi.PyApiCmdBCStart( handle, ambd.API_BC_START_IMMEDIATELY, 1000,  60.0 )


def StopSimulation(pymilapi, handle):
    print("StopSimulation ... done")

    pymilapi.PyApiCmdBCHalt(handle)
    pymilapi.PyApiCmdRTHalt(handle)
    pymilapi.PyApiCmdQueueHalt(handle)


def CheckTransfer(pymilapi, handle, ulTransferNo, pxMessage ):
    if pxMessage.buffer[0] != ((ulTransferNo  * 32) + 1):
        raise Exception("Expected first data word 0x%04X to be 0x%04X at transfer %d" %
                                               (pxMessage.buffer[0],
                                               ((ulTransferNo  * 32) + 1),
                                               ulTransferNo) )

    if pxMessage.buffer[31] != ((ulTransferNo + 1)  * 32):
        raise Exception("Expected last data word 0x%04X to be 0x%04X at transfer %d\n" %
                                               (pxMessage.buffer[31],
                                               ((ulTransferNo + 1)  * 32),
                                               ulTransferNo ))


def Monitor(pymilapi, handle, event):
    print("Monitor ... ")
    ulBmMessageCount = 0

    # Wait for BC start
    while not MilUtilIsBcActive( pymilapi, handle ): continue

    time.sleep(0.1)

    # Read BM data as long as BC is active
    while( True ):
        msg = pymilapi.PyApiCmdQueueRead( handle )

        if msg == None:
            if MilUtilIsBcActive(pymilapi,handle):
                time.sleep(0.5)
                continue
            else:
                break

        if msg != None :
            # Received data
            CheckTransfer( pymilapi, handle, ulBmMessageCount, msg )

            ulBmMessageCount = ulBmMessageCount + 1

        if (ulBmMessageCount % 100) == 0:
            print("Bus Monitor received %d transfers (%d ir)" % (ulBmMessageCount, event.ulIntCounter) )

        time.sleep(0.01)

    time.sleep(0.5)

    print("Bus Monitor received %d transfers (%d ir)" % (ulBmMessageCount, event.ulIntCounter) )
    print()

    if event.ulIntCounter != 1000 :
        raise Exception("Expected to receive %d interrupts got %d" % (1000, event.ulIntCounter) )

    if ulBmMessageCount == 1000:
        print("Monitor ... done")
    else:
        print("Expected to receive %d transfers got %d" % (1000, ulBmMessageCount) )
        print("Monitor ... failed")


def PrintSimulationStatistic(pymilapi, handle):
    print("PrintSimulationStatistic ...")
    status = pymilapi.PyApiCmdBCStatusRead( handle )
    print("BC  ( %d messages / %d errors )" % (status.glb_msg_cnt, status.glb_err_cnt) )
    print("PrintSimulationStatistic ... done")


def LsInterruptSample(pymilapi, handle):
    print("Running LsInterruptSample")

    # -- Install interrupt handler  ---
    event = AimDeviceEventHandler(pymilapi, handle)
    event.install()

    try:
        PrepareMonitoring( pymilapi, handle )
        # Setup RTs and BC
        PrepareRemoteTerminals( pymilapi, handle )
        PrepareBusController(pymilapi, handle, event)
        # Start the simulation and the bus monitor
        RunSimulation( pymilapi, handle )
        # Read Bus Monitor data as long as the BC is active
        Monitor( pymilapi, handle, event )

    finally:
        # Stop the simulation and the bus monitor
        StopSimulation(pymilapi, handle)
        PrintSimulationStatistic(pymilapi, handle)
        # -- Delete interrupt handler  ---
        event.remove()

