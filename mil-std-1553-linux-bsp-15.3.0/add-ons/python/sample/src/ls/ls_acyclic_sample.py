# SPDX-FileCopyrightText: 2023-2024 AIM GmbH <info@aim-online.com>
# SPDX-License-Identifier: MIT

"""This sample shows how to to send acyclic frames.
This sample creates an acyclic frame with different transfer types (BC2RT, RT2RT and RT2BC). 
The BC is started in idle mode with the RT enabled and the acyclic frame 
is send 300 times producing a total of 900 tranfers. 
After each frame the received status and data is checked. 
This test is executed with internal coupling. No external connection is required.
"""

from aim_mil import mil_bindings_defines as ambd
from aim_mil import mil_bindings_types as ambt

from .mil_utils import MilUtilNextRTHid, \
                        MilUtilEnableRT, \
                        MilUtilEnableRTSA, \
                        MilUtilNextBCHid, \
                        MilUtilSetData, \
                        MilUtilGetData, \
                        MilUtilBCCreateTransfer

BC_TRANSFER_COUNT = 900

# BC Header ids are shared by all transfers on a stream #
transfer1_buffer_header_id=0
transfer2_buffer_header_id=0

# RT Header ids are shared by all RTs on a stream #
rt3_sa13_buffer_header_id = 0
rt4_sa14_buffer_header_id = 0

BC2RT_TRANSFER_ID=1
RT2BC_TRANSFER_ID=2
RT2RT_TRANSFER_ID=3


def PrepareRemoteTerminals(pymilapi, handle):

   global rt3_sa13_buffer_header_id
   global rt4_sa14_buffer_header_id
   print("PrepareRemoteTerminals ... done")

   # -- Simulate RT3 receive SA13 --- 
   rt3_sa13_buffer_header_id = MilUtilNextRTHid()
   MilUtilEnableRT(pymilapi, handle, 3 , 8.0 )
   MilUtilEnableRTSA(pymilapi,handle, 3 , 13 , ambd.API_RT_TYPE_RECEIVE_SA, rt3_sa13_buffer_header_id)

   # -- Simulate RT4 transmit SA14 --- 
   rt4_sa14_buffer_header_id = MilUtilNextRTHid()
   MilUtilEnableRT(pymilapi, handle, 4 , 8.0)
   MilUtilEnableRTSA(pymilapi,handle, 4 , 14 , ambd.API_RT_TYPE_TRANSMIT_SA, rt4_sa14_buffer_header_id)


def PrepareBusController(pymilapi, handle):
   # Initialize the Bus Controller 
   pymilapi.PyApiCmdBCIni( handle )

   print("PrepareBusController ... done")
   '''
       In this sample we do not send any cyclic transfers. Instead
       we include only a wait instruction in the minor major framing.
       This method can be used to send only acyclic frames.
   '''
   xMinorFrame = ambt.PY_TY_API_BC_FRAME()
   xMinorFrame.id    = 1
   xMinorFrame.cnt   = 1
   xMinorFrame.xid   = [10] # wait time 
   xMinorFrame.instr = [ambd.API_BC_INSTR_WAIT]

   # -- Install Minor Frame --- 
   pymilapi.PyApiCmdBCFrameDef( handle, xMinorFrame )

   # -- Install Major Frame --- 
   xMajorFrame = ambt.PY_TY_API_BC_MFRAME_EX()
   xMajorFrame.cnt = 1
   xMajorFrame.fid = [1]
   pymilapi.PyApiCmdBCMFrameDefEx( handle, xMajorFrame )


def PrepareAcyclicTransfers(pymilapi, handle):

   global transfer1_buffer_header_id
   global transfer2_buffer_header_id
   print("PrepareAcyclicTransfer ... done")

   # -- BC -> RT3RcvSA13 --
   # creating a acyclic transfer is equal to creating a cyclic transfer #
   transfer1_buffer_header_id = MilUtilNextBCHid()

   MilUtilBCCreateTransfer( pymilapi, handle, 
                            BC2RT_TRANSFER_ID,
                            ambd.API_BC_TYPE_BCRT, 
                            0,  # Transmit RT 
                            0,  # Transmit SA 
                            3,  # Receive  RT 
                            13, # Receive  SA 
                            0,  # Word count 32 
                            transfer1_buffer_header_id)

   # -- RT4RcvSA14 -> BC --
   transfer2_buffer_header_id = MilUtilNextBCHid()

   MilUtilBCCreateTransfer(pymilapi, handle,
                            RT2BC_TRANSFER_ID,
                            ambd.API_BC_TYPE_RTBC,
                            4,   # Transmit RT 
                            14,  # Transmit SA 
                            0,   # Receive  RT 
                            0,   # Receive  SA 
                            0,   # Word count 32 
                            transfer2_buffer_header_id)

   # -- RT4RcvSA14 -> RT3RcvSA13 -- 
   MilUtilBCCreateTransfer(pymilapi, handle,
                            RT2RT_TRANSFER_ID,
                            ambd.API_BC_TYPE_RTRT,
                            4,   # Transmit RT 
                            14,  # Transmit SA 
                            3,   # Receive  RT 
                            13,  # Receive  SA 
                            0,   # Word count 32 
                            MilUtilNextBCHid())


def RunSimulation(pymilapi, handle):
   print('RunSimulation ... running')
   # Start all enabled RTs #
   pymilapi.PyApiCmdRTStart(handle)
   # Start BC 
   # Start BC with count 0 means run forever 
   pymilapi.PyApiCmdBCStart( handle, ambd.API_BC_START_IMMEDIATELY, 0, 10.0 )


def StopSimulation(pymilapi, handle):
   print('StopSimulation ... done')
   # Stop Bus Controller #
   pymilapi.PyApiCmdBCHalt(handle)
   # Stop all RTs #
   pymilapi.PyApiCmdRTHalt(handle)


def SetTransmitData(pymilapi, handle, uwBufferValue, ucBufferType, uwTransmitBufferHeader):
   auwMessageData = 32 * [uwBufferValue]
   
   MilUtilSetData(pymilapi, handle, 
                  uwTransmitBufferHeader, 
                  ucBufferType,
                  1, # First buffer in queue*/
                  auwMessageData)

def CheckReceiveData(pymilapi, handle, uwBufferValue, ucBufferType, uwTransmitBufferHeader):
   auwMessageData, _, _ = MilUtilGetData(pymilapi, handle, 
                                        uwTransmitBufferHeader, 
                                        ucBufferType,
                                        0, #/* Read current buffer */
                                        32)
   # Check data #
   for i in range(32):
      if( auwMessageData[i] != uwBufferValue ):
            raise Exception("CheckReceiveData: Invalid at index=%d, %d != %d" % ( i, auwMessageData[i], uwBufferValue ))


def SendAcyclicDataBlocking(pymilapi, handle, uwTransferId ):
   # assign the transfer to the acyclic frame #
   xAcyclicFrame = ambt.PY_TY_API_BC_ACYC()
   xAcyclicFrame.cnt   = 1
   xAcyclicFrame.xid   = [uwTransferId]
   xAcyclicFrame.instr = [ambd.API_BC_INSTR_TRANSFER]

   # -- Install Acyclic Frame --- 
   pymilapi.PyApiCmdBCAcycPrep(handle, xAcyclicFrame)

   '''
       Clear transfer buffer status and set BRW/BUF_STAT to API_BUF_NOT_USED. 
       Please note that BRW/BUF_STAT are part of the status queue assigned to the transfer buffer header. 
       Please make sure that the Buffer Header id used for this transfer is not used for any other transfer.
       Please make also sure that only one status queue entry is used by specifying either "qsize=API_QUEUE_SIZE_1" 
       or "sqm=API_SQM_ONE_ENTRY_ONLY" when calling ApiCmdBCBHDef.
   '''
   xfer_status = pymilapi.PyApiCmdBCXferRead(handle,
                                             uwTransferId, 
                                             0x7 # clear status on read
                                             )

   '''
       ApiCmdBCAcycSend will hand over the acyclic frame pointer to the firmware. 
       This function is non blocking. The transfer will be sent by the firmware at an appropriate time.
       It is important to check the status of the last transfer in the acyclic frame to detect if the acyclic frame has been sent completely.
       To do so we use the ApiCmdBCXferRead, BRW/BUF_STAT value. See below.
   '''

   pymilapi.PyApiCmdBCAcycSend(handle, ambd.API_BC_ACYC_SEND_IMMEDIATELY)

   while ((xfer_status.brw >> 12) & 0x3) == ambd.API_BUF_NOT_USED:
        # uwTransferId must be the last Transfer in the acyclic frame and must not be part of the cyclic framing #
        xfer_status = pymilapi.PyApiCmdBCXferRead(handle, uwTransferId,
                                                   0x7 #clear status on read
                                                   )

        # Receive  buffers (RT to BC / RT to RT) will have BRW/BUF_STAT set to API_BUF_FULL  when the transfer is finished #
        # Transmit buffers (BC to RT)            will have BRW/BUF_STAT set to API_BUF_EMPTY when the transfer is finished #


def PrintSimulationStatistic(pymilapi, handle):
   print("PrintSimulationStatistic ...")

   xBcStatus = pymilapi.PyApiCmdBCStatusRead( handle )

   if ((xBcStatus.glb_msg_cnt != BC_TRANSFER_COUNT) or (xBcStatus.glb_err_cnt != 0)):
        print("Expected to receive %d transfers and 0 errors got (%d/%d)" % (BC_TRANSFER_COUNT, xBcStatus.glb_msg_cnt, xBcStatus.glb_err_cnt) )
   else:
        print("BC  ( %d messages / %d errors )" % (xBcStatus.glb_msg_cnt, xBcStatus.glb_err_cnt))

   xRtStatus = pymilapi.PyApiCmdRTStatusRead(handle)

   print("RTs ( %d messages  / %d errors )" % ( xRtStatus.glb_msg_cnt, xRtStatus.glb_err_cnt))

   print("Please note that one RT to RT transfer counts as two messages in RTs and BM")
   print("PrintSimulationStatistic ... done")

########################################################################################
#                                                                                      #
#                                     Sample entry point                               #
#                                                                                      #
########################################################################################


def LsAcyclicSample(pymilapi, handle):
    print("Running LsAcyclicSample")

    try:
        PrepareRemoteTerminals(pymilapi, handle)
        PrepareBusController(pymilapi, handle)
        PrepareAcyclicTransfers(pymilapi, handle)
        RunSimulation(pymilapi, handle)

        print("Framing :      300 times \n",
                "------------------------ \n",
                "BC         -> RT3RcvSA13 \n",
                "RT4XmtSA14 -> BC         \n",
                "RT4XmtSA14 -> RT3RcvSA13 \n",
                "------------------------ \n",
                "    total  900 transfers \n")

        print("Sending acyclic transfers")

        uwBufferValue = 1
        global transfer1_buffer_header_id
        global transfer2_buffer_header_id
        global rt3_sa13_buffer_header_id
        global rt4_sa14_buffer_header_id

        while (uwBufferValue <= BC_TRANSFER_COUNT):
            # -- Send and verify BC to RT --- #

            SetTransmitData(pymilapi, handle, uwBufferValue,
                            ambd.API_BUF_BC_MSG, transfer1_buffer_header_id)

            SendAcyclicDataBlocking(pymilapi, handle, BC2RT_TRANSFER_ID)

            CheckReceiveData(pymilapi, handle, uwBufferValue,
                            ambd.API_BUF_RT_MSG, rt3_sa13_buffer_header_id)

            uwBufferValue = uwBufferValue + 1

            # -- Send and verify RT to BC ---

            SetTransmitData(pymilapi, handle, uwBufferValue,
                            ambd.API_BUF_RT_MSG, rt4_sa14_buffer_header_id)

            SendAcyclicDataBlocking(pymilapi, handle, RT2BC_TRANSFER_ID)

            CheckReceiveData(pymilapi, handle, uwBufferValue,
                            ambd.API_BUF_BC_MSG, transfer2_buffer_header_id)

            uwBufferValue = uwBufferValue + 1

            # -- Send and verify RT to RT --- #

            SetTransmitData(pymilapi, handle, uwBufferValue,
                            ambd.API_BUF_RT_MSG, rt4_sa14_buffer_header_id)

            SendAcyclicDataBlocking(pymilapi, handle, RT2RT_TRANSFER_ID)

            CheckReceiveData(pymilapi, handle, uwBufferValue,
                            ambd.API_BUF_RT_MSG, rt3_sa13_buffer_header_id)

            if ((uwBufferValue % 100) == 0):
                print("Transmitted %d acyclic transfers." % uwBufferValue)

            uwBufferValue = uwBufferValue + 1
        print()

    finally:
        StopSimulation(pymilapi, handle)
        PrintSimulationStatistic(pymilapi, handle)
