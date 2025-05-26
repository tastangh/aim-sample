# SPDX-FileCopyrightText: 2023-2024 AIM GmbH <info@aim-online.com>
# SPDX-License-Identifier: MIT

"""This sample shows how to implement a BC instruction list.
This sample creates an instruction list with two frames. 
The first frame contains a Strobe instruction and two BC2RT transfers. 
The second frame contains only two BC2RT transfers. 
The BC is started in instruction list mode with the RT enabled and 
the instruction list is send with a total of 4000 transfers. 
The pass criteria is that all 4000 transfer are send without an error. 
This sample is executed with internal coupling. No external connection is required.
"""

import time

from aim_mil import mil_bindings_defines as ambd
from aim_mil import mil_bindings_types as ambt


from .mil_utils import  MilUtilNextRTHid, \
                        MilUtilEnableRT, \
                        MilUtilEnableRTSA, \
                        MilUtilNextBCHid, \
                        MilUtilSetData, \
                        MilUtilIsBcActiveCheckCount, \
                        MilUtilBCCreateTransfer

def PrepareRemoteTerminals(pymilapi, handle):
    print("PrepareRemoteTerminals ... done")

    #############################################
    #                                           #
    # --      Simulate RT3 receive SA13     --- #
    #                                           #
    #############################################
    uwHeaderId = MilUtilNextRTHid()
    MilUtilEnableRT(pymilapi, handle, 3 , 8.0 )
    MilUtilEnableRTSA(pymilapi, handle, 3 , 13 , ambd.API_RT_TYPE_RECEIVE_SA, uwHeaderId)
 

def PrepareBusController(pymilapi, handle):
    # Get maximum count of intructions for instruction list

    xMemInfo = pymilapi.PyApiCmdSysGetMemPartition(handle)
    ulMaxInstructions = int(xMemInfo.ax_BiuSize[0].ul_BcHipInstr / 4)
    print("Instruction list size %d with maximum %d instructions." % (xMemInfo.ax_BiuSize[0].ul_BcHipInstr, ulMaxInstructions))

    #############################################
    #                                           #
    # --  Initialize the Bus Controller     --- #
    #                                           #
    #############################################
    pymilapi.PyApiCmdBCIni( handle )

    print(  "Framing :         1000 times \n",
            "---------------------------- \n",
            "FRAME1:\n",
            "    Strobe                   \n",
            "    BC         -> RT3RcvSA13 \n",
            "    BC         -> RT3RcvSA13 \n",
            "FRAME2:\n",
            "    BC         -> RT3RcvSA13 \n",
            "    BC         -> RT3RcvSA13 \n",
            "---------------------------- \n",
            "        total 4000 transfers \n" )

    print("PrepareBusController ... done")

    #############################################
    #                                           #
    # --            BC -> RT3RcvSA13        --- #
    #                                           #
    #############################################
    uwHeaderId   = MilUtilNextBCHid()
    MilUtilBCCreateTransfer( pymilapi, handle, 
                            1,
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
                    1, # First buffer in queue
                    auwMessageData)

    #############################################
    #                                           #
    # --            Instruction Table       --- #
    #                                           #
    #############################################
    # Get BC resources 
    pymilapi.PyApiCmdBCInstrTblIni( handle)
    axInstructionTable    = [ambt.PY_TY_API_BC_FW_INSTR() for _ in range(ulMaxInstructions)]

    # Clear BC Instruction Table structure #
    axInstructionTable, _, _, _ = pymilapi.PyApiCmdBCInstrTblGen( handle,
                                                                    0,  # clear
                                                                    ulMaxInstructions,
                                                                    0,  # dest_cnt 
                                                                    0,  # dest_offset
                                                                    axInstructionTable)

    ulInstrNr = 0
    axInstructionTable[ulInstrNr].op    = ambd.API_BC_FWI_JUMP  # GOTO Start of Major Frame 
    axInstructionTable[ulInstrNr].par1  = 0x1111                # Major frame start label   

    # FRAME 1 
    ulInstrNr = ulInstrNr + 1
    axInstructionTable[ulInstrNr].label = 0x0001                # Start label of Frame 1    
    axInstructionTable[ulInstrNr].op    = ambd.API_BC_FWI_STRB  # Send strobe output       
    ulInstrNr = ulInstrNr + 1
    axInstructionTable[ulInstrNr].op    = ambd.API_BC_FWI_XFER  # Send transfer             
    axInstructionTable[ulInstrNr].par1  = 1                     # transfer id               
    ulInstrNr = ulInstrNr + 1
    axInstructionTable[ulInstrNr].op    = ambd.API_BC_FWI_XFER  # Send transfer             
    axInstructionTable[ulInstrNr].par1  = 1                     # transfer id              
    ulInstrNr = ulInstrNr + 1
    axInstructionTable[ulInstrNr].op    = ambd.API_BC_FWI_WMFT  # Wait for minor frame time 
    ulInstrNr = ulInstrNr + 1
    axInstructionTable[ulInstrNr].op    = ambd.API_BC_FWI_RET   # Return from minor frame 1 

    # FRAME 2
    ulInstrNr = ulInstrNr + 1
    axInstructionTable[ulInstrNr].label = 0x0002               # Start label of Frame 2    
    axInstructionTable[ulInstrNr].op    = ambd.API_BC_FWI_XFER # Send transfer             
    axInstructionTable[ulInstrNr].par1  = 1                    # transfer id               
    ulInstrNr = ulInstrNr + 1
    axInstructionTable[ulInstrNr].op    = ambd.API_BC_FWI_XFER # Send transfer             
    axInstructionTable[ulInstrNr].par1  = 1                    # transfer id               
    ulInstrNr = ulInstrNr + 1
    axInstructionTable[ulInstrNr].op    = ambd.API_BC_FWI_WMFT # Wait for minor frame time
    ulInstrNr = ulInstrNr + 1
    axInstructionTable[ulInstrNr].op    = ambd.API_BC_FWI_RET  # Return from minor frame 2


    # Start of Major Frame / Call minor frames 
    ulInstrNr = ulInstrNr + 1
    axInstructionTable[ulInstrNr].label = 0x1111           # Start of Major Frame Label
    axInstructionTable[ulInstrNr].op    = ambd.API_BC_FWI_CALL  # Send frame 
    axInstructionTable[ulInstrNr].par1  = 0x0001           # frame 1 label             
    ulInstrNr = ulInstrNr + 1
    axInstructionTable[ulInstrNr].op    = ambd.API_BC_FWI_CALL # Send frame 
    axInstructionTable[ulInstrNr].par1  = 0x0002               # frame 2 label             
    ulInstrNr = ulInstrNr + 1
    axInstructionTable[ulInstrNr].op    = ambd.API_BC_FWI_DJZ  # Decrement major frame count and jump if zero 
    axInstructionTable[ulInstrNr].par1  = 0xFFFF               # Destination label                            
    ulInstrNr = ulInstrNr + 1
    axInstructionTable[ulInstrNr].op    = ambd.API_BC_FWI_JUMP # Jump to start label
    axInstructionTable[ulInstrNr].par1  = 0x1111               # Destination label

    # End of framing 
    ulInstrNr = ulInstrNr + 1
    axInstructionTable[ulInstrNr].label = 0xFFFF               # End label   
    axInstructionTable[ulInstrNr].op    = ambd.API_BC_FWI_HALT # Do nothing  

    # -- Convert and install instruction table ---
    # mode 3 => convert and write table #
    axInstructionTable, _, _, _ = pymilapi.PyApiCmdBCInstrTblGen( handle, 3, 
                                                                  ulMaxInstructions, 
                                                                  ulInstrNr,
                                                                  0, # offset
                                                                  axInstructionTable)


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
    pymilapi.PyApiCmdBCStart( handle, ambd.API_BC_START_INSTR_TABLE, 1000,  10.0 )


def WaitForEndOfSimulation(pymilapi, handle):
    transfers_left = 0
    '''
       The ApiCmdBCStatusRead status bit will always be busy in case of an instruction list. 
       This is required to be able to send acyclic frames even if the instruction
       list has reached its end. To check if we reached the end of the simulation
       we check the number of transfers sent. */
    '''
    while True:
        transfers_left = MilUtilIsBcActiveCheckCount(pymilapi, handle, 1000 * 4 )

        print("BC transfers to send %d" % (transfers_left) )

        if( transfers_left == 0 ):
            break

        time.sleep(1)
    print()


def StopSimulation(pymilapi, handle):
    print('StopSimulation ... done')

    # Stop Bus Controller #
    pymilapi.PyApiCmdBCHalt(handle)

    # Stop all RTs #
    pymilapi.PyApiCmdRTHalt(handle)


def PrintSimulationStatistic(pymilapi, handle):
    print('PrintSimulationStatistic ...')

    xBcStatus = pymilapi.PyApiCmdBCStatusRead( handle )
    print('BC  ( %d messages / %d errors )' % (xBcStatus.glb_msg_cnt, xBcStatus.glb_err_cnt ))

    xRtStatus = pymilapi.PyApiCmdRTStatusRead( handle)
    print('RTs ( %d messages  / %d errors )' % (xRtStatus.glb_msg_cnt, xRtStatus.glb_err_cnt ))

    print('Please note that one RT to RT transfer counts as two messages in RTs and BM')
    print('PrintSimulationStatistic ... done')


########################################################################################
#                                                                                      #
#                                     Sample entry point                               #
#                                                                                      #
########################################################################################
def LsInstructionListSample(pymilapi, handle):
    print("Running LsInstructionListSample")

    try:
        PrepareRemoteTerminals( pymilapi, handle )
        PrepareBusController( pymilapi, handle )
        RunSimulation( pymilapi, handle )
        WaitForEndOfSimulation( pymilapi, handle )

    finally:
        StopSimulation( pymilapi, handle )
        PrintSimulationStatistic( pymilapi, handle )

