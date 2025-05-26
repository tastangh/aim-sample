# SPDX-FileCopyrightText: 2023-2024 AIM GmbH <info@aim-online.com>
# SPDX-License-Identifier: MIT

"""This sample shows how to send transfers with external bus coupling.
This sample creates a framing with two different transfers (BC2RT, RT2BC).
The transfers are send to RT19 wrap around SA30. 
The sample needs a bus with transformer termination and an external RT19.
This sample can be used in combination with the ls_ptp_vtp_sample 
which implements a RT19 with external coupling.
"""

import time

from aim_mil import mil_bindings_defines as ambd

from .mil_utils import  MilUtilNextBCHid, \
                        MilUtilSetData, \
                        MilUtilBCCreateTransfer, \
                        MilUtilBCCreateSingleFrameFraming, \
                        MilUtilIsBcActive

rt = 19
shared_buffer_sa = 30

def PrepareBusController(pymilapi, handle):
    auwTransferIds = [ 1, 2 ]
    #############################################
    #                                           #
    # --  Initialize the Bus Controller     --- #
    #                                           #
    #############################################
    pymilapi.PyApiCmdBCIni( handle )

    print()
    print(  "Framing :      1000 times  \n",
            "-------------------------- \n",
            "BC          -> RT19RcvSA30 \n",
            "RT19XmtSA30 -> BC          \n",
            "-------------------------- \n",
            "     total 2000 transfers \n" )

    print('PrepareBusController ... done')

    #############################################
    #                                           #
    # --            BC -> RT19RcvSA30       --- #
    #                                           #
    #############################################
    uwHeaderId   = MilUtilNextBCHid()

    MilUtilBCCreateTransfer( pymilapi, handle, 
                            auwTransferIds[0],
                            ambd.API_BC_TYPE_BCRT, 
                            0,               # Transmit RT
                            0,               # Transmit SA
                            rt,              # Receive  RT 
                            shared_buffer_sa,# Receive  SA 
                            0,               # Word count 32 #
                            uwHeaderId)

    # Data to send to the RT
    auwMessageData = 32 * [0]
    for i in range (32):
        auwMessageData[ i ] = 0xBC00 + i + 1

    MilUtilSetData(pymilapi, handle, uwHeaderId, ambd.API_BUF_BC_MSG, 
                           1, # First buffer in queue
                           auwMessageData)

    #############################################
    #                                           #
    # --         RT19XmtSA30 -> BC          --- #
    #                                           #
    #############################################
    uwHeaderId   = MilUtilNextBCHid()

    MilUtilBCCreateTransfer( pymilapi, handle, 
                            auwTransferIds[1],
                            ambd.API_BC_TYPE_RTBC, 
                            rt,               # Transmit RT 
                            shared_buffer_sa, # Transmit SA 
                            0,                # Receive  RT 
                            0,                # Receive  SA 
                            0,                # Word count 32 
                            uwHeaderId)

    #############################################
    #                     BC                    #
    # --      Setup Major/Minor Framing  ---    #
    #                                           #
    #############################################
    MilUtilBCCreateSingleFrameFraming(pymilapi, handle, auwTransferIds)


#######################################################################################
#                                                                                     #
#                                     Sample entry point                              #
#                                                                                     #
#######################################################################################
def LsBcSample(pymilapi, handle):
    print('Running LsBcSample ')
    print( "Note: This sample implements only the BC part. \n",
           "      The coupling is set to transformer.\n",
           "      To get successful transfers the LsPtpVtpSample, or any other RT19\n", 
           "      wrap around SA30 implementation, needs to be started on the connected stream.\n")

    try:
        # Setup BC
        PrepareBusController(pymilapi, handle )

        # Start BC 
        pymilapi.PyApiCmdBCStart( handle, ambd.API_BC_START_IMMEDIATELY, 1000,  10.0 )

        # Print status while BC active
        while MilUtilIsBcActive(pymilapi, handle):
            time.sleep(1)

            xBcStatus = pymilapi.PyApiCmdBCStatusRead( handle )
            print('BC  ( %d messages / %d errors )' % (xBcStatus.glb_msg_cnt, xBcStatus.glb_err_cnt ))

    finally:
        # Stop Bus Controller
        pymilapi.PyApiCmdBCHalt(handle)
