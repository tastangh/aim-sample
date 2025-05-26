# SPDX-FileCopyrightText: 2023-2024 AIM GmbH <info@aim-online.com>
# SPDX-License-Identifier: MIT

"""This sample shows how to implement the AyE milscope with external bus coupling.
This sample configures the AyE Milscope and sends a single transfer. 
The Milscope data is received and stored into a CSV file.
The sample needs a bus with transformer termination and no external bus traffic.

Note: If the package matplotlib is installed the function PlotData can be used to plot the Milscope data.
"""

import tempfile
import os
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
                        MilUtilHasMilscope

SCOPE_BUFFER_COUNT = 4
SCOPE_RETRIGGER    = 1
fp = None # Global file pointer to store data in buffer callback.

def ReadData(handle, buffer_pointer ):
    global fp
    buffer = ambt.PY_TY_API_SCOPE_BUFFER.fromPointer(buffer_pointer)

    print("Buffer Handler ID %d" % buffer.ulID )

    if buffer.ulFlags & ambd.SCOPE_BUFFER_FLAG_CANCELED :
        return

    if buffer.ulFlags & ambd.SCOPE_BUFFER_FLAG_TRIGGER:
        print( "Trigger buffer with data size %d and trigger ID %d complete" % (buffer.ulDataSize, buffer.ulTriggerId) )

    values = buffer.samples

    if buffer.eBufferType == ambd.SCOPE_BUFFER_PRIMARY:
        # channel A data
        if buffer.ulID > 0:
            fp.write(",")
        fp.write(",".join(map(str, values)))
    else:
        # channel B data
        pass


BufferHandler = ambd.TY_API_SCOPE_BUFFER_HANDLER(ReadData)

def PlotData( filename ):
    import matplotlib.pyplot as plt
    import csv
    
    data = []    
    with open(filename,'r') as csvfile:
        lines = csv.reader(csvfile, delimiter=',')
        for row in lines:
            data = [int(d) for d in row]

    plt.plot(data, label = "Milscope")
    plt.show()


def PrepareMilscope(pymilapi, handle):
    setup = ambt.PY_TY_API_SCOPE_SETUP()

    # Test if stopping is possible before setup without crash
    pymilapi.PyApiCmdScopeReset(handle)

    setup.ul_OperationMode = ambd.API_SCOPE_OPR_CHANNEL_A
    setup.ul_CouplingPri = ambd.API_SCOPE_COUPLING_STUB
    setup.ul_CaptureMode = ambd.API_SCOPE_START_SINGLE
    setup.ul_SamplingRate = ambd.API_SCOPE_RATE_EVERY

    pymilapi.PyApiCmdScopeSetup(handle, setup)

    time.sleep(1)

    trigger = ambt.PY_TY_API_SCOPE_TRG_EX()
    trigger.ul_TrgMode = ambd.API_SCOPE_MODE_GREATER_THAN
    trigger.ul_TrgSrc = ambd.API_SCOPE_SRC_PRIMARY
    trigger.ul_TrgValue = 600
    trigger.ul_TrgFlags = 0

    pymilapi.PyApiCmdScopeTriggerDefEx(handle, trigger)

    return  pymilapi.PyApiCreateScopeBufferList( handle, SCOPE_BUFFER_COUNT, BufferHandler )

def StartMilscope( pymilapi, handle, pxScopeBufferList ):
    pymilapi.PyApiProvideScopeBuffers(handle, pxScopeBufferList )

    _, BufferCount = pymilapi.PyApiCmdScopeStatus( handle )

    print("Prepared scope with %d buffers" % (BufferCount) )

    pymilapi.PyApiCmdScopeStart(handle)

def WaitForMilscopeFinished( pymilapi, handle):
    finished = False
    while not finished :
        flags = pymilapi.PyApiWaitForScopeFinished( handle, 10000 )

        if flags & ambd.SCOPE_WAIT_OVERFLOW :
            pymilapi.PyApiCmdScopeStop(handle)
            raise Exception("Overflow in scope")

        elif flags & ambd.SCOPE_WAIT_TIMEOUT :
            status, buffer_count = pymilapi.PyApiCmdScopeStatus( handle )

            if status != ambd.SCOPE_STATUS_WAIT_FOR_TRIGGER :
                pymilapi.PyApiCmdScopeStop(handle)
                raise Exception("Time out before all buffers were processed!")
            else:
                print("Time out but continuing to wait for trigger with %d buffers" % (buffer_count) )
        else:
            _, buffer_count = pymilapi.PyApiCmdScopeStatus( handle )

            print("Scope finished buffer count %d" % (buffer_count) )

            finished = True

def StopMilscope(pymilapi, handle):
    pymilapi.PyApiCmdScopeStop(handle)

def FinalizeMilscope( pymilapi, handle, pxScopeBufferList):
    for buffer in pxScopeBufferList :
        pymilapi.PyApiFreeScopeBuffer( handle, buffer )



def PrepareSimulation(pymilapi, handle):
    print("PrepareSimulation ... done")

    ###############################################
    #                                             #
    #  --      Simulate RT3 receive SA13     ---  #
    #                                             #
    ###############################################
    MilUtilEnableRT(pymilapi, handle, 3 , 8.0)

    MilUtilEnableRTSA(pymilapi, handle, 3, 13, ambd.API_RT_TYPE_RECEIVE_SA, MilUtilNextRTHid() )

    ###############################################
    #                                             #
    #  --  Initialize the Bus Controller     ---  #
    #                                             #
    ###############################################
    pymilapi.PyApiCmdBCIni( handle )

    print()
    print(  "Framing :        1 times \n",
            "------------------------ \n",
            "BC         -> RT3RcvSA13 \n",
            "------------------------ \n",
            "       total 1 transfers \n" )

    ###############################################
    #                                             #
    #  --           BC -> RT3 SA13           ---  #
    #                                             #
    ###############################################
    auwTransferIds = [ 1 ]
    uwHeaderId     = MilUtilNextBCHid()

    MilUtilBCCreateTransfer(pymilapi, handle,
        auwTransferIds[0],
        ambd.API_BC_TYPE_BCRT,
        0,  # Transmit RT
        0,  # Transmit SA
        3,  # Receive  RT
        13, # Receive  SA
        0,  # Word count 32
        uwHeaderId,
        ambd.API_QUEUE_SIZE_1,
        ambd.API_BC_TIC_NO_INT)

    # Data to send to the RT
    auwMessageData = [0] * 32
    for i in range(32):
        # auwMessageData[] = { 0xBC01, 0xBC02, 0xBC03 ... 0xBC32 }
        auwMessageData[ i ] = 0xBC00 + i + 1

    MilUtilSetData(pymilapi, handle, uwHeaderId, ambd.API_BUF_BC_MSG, 1, auwMessageData)

    MilUtilBCCreateSingleFrameFraming(pymilapi, handle, auwTransferIds)



def RunSimulation(pymilapi, handle):
    print("RunSimulation ... done")
    pymilapi.PyApiCmdRTStart( handle )
    pymilapi.PyApiCmdBCStart( handle, ambd.API_BC_START_IMMEDIATELY, 1, 10.0 )


def StopSimulation(pymilapi, handle):
    print("StopSimulation ... done")
    pymilapi.PyApiCmdBCHalt( handle )
    # Stop all RTs
    pymilapi.PyApiCmdRTHalt( handle )


def LsMilscopeSampleAPE(pymilapi, handle):
    print("Running LsMilscopeSampleAPE")

    if not MilUtilHasMilscope(pymilapi,handle):
        raise Exception("Device ist no Milscope")

    try:
        PrepareSimulation(pymilapi,handle)

        buffer_addresses = PrepareMilscope(pymilapi, handle)

        szCsvPathAPE = os.path.join(tempfile.gettempdir(), "milscope_data_ape.csv")

        print("Path to csv file: %s" % (szCsvPathAPE))

        global fp
        with open(szCsvPathAPE, "w") as fp:
            ''' In order to implement a retrigger mode the following for loop
                can be changed. While in the loop the Milscope will 
                continously capture data. 
            '''
            for _ in range(1):
                try:
                    StartMilscope(pymilapi, handle, buffer_addresses)

                    RunSimulation(pymilapi,handle)

                    WaitForMilscopeFinished(pymilapi, handle)

                    # No need to read the data
                    # The library will call our buffer callback function instead
                finally:
                    StopMilscope(pymilapi, handle)
                    StopSimulation(pymilapi,handle)

        #PlotData(szCsvPathAPE)
    finally:
        StopMilscope(pymilapi, handle)
        StopSimulation(pymilapi,handle)
        FinalizeMilscope(pymilapi, handle, buffer_addresses)

