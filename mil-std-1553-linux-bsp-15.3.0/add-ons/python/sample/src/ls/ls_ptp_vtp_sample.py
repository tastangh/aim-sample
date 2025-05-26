# SPDX-FileCopyrightText: 2023-2024 AIM GmbH <info@aim-online.com>
# SPDX-License-Identifier: MIT

"""This sample shows how to implement a RT with external bus coupling.
This sample creates a RT19 with various SA and MC enabled.
The sample needs a bus with transformer termination and an external BC.
This sample can be used in combination with the ls_bc_sample
which implements a BC/RT19 transfers on SA30.
"""

from aim_mil import mil_bindings_defines as ambd

from .mil_utils import MilUtilNextRTHid, \
                        MilUtilCheckIfDeviceIsEmbedded, \
                        MilUtilResetIds, \
                        MilUtilEnableRTSA

SHARED_BUFFER_SA  = 30
BROADCAST_RT      = 31

ucTFBit = 0
ucSSFBit = 0
ucBUSYBit = 0
ucSRQBit = 0
ucFailSaftCondition = 0
ucRtParityErr = 0

rt_addr     = 19
rt_sa  =    [# SA/MC, TYPE, Word Count
            [ 3, ambd.API_RT_TYPE_RECEIVE_SA, 1 ],
            [ SHARED_BUFFER_SA, ambd.API_RT_TYPE_RECEIVE_SA, 0 ], #Buffer and Status shared between TX and RX by default
            [ 5, ambd.API_RT_TYPE_RECEIVE_SA, 1 ],
            [ 26, ambd.API_RT_TYPE_RECEIVE_SA, 1 ], # HSSA enabled by default

            [ 1, ambd.API_RT_TYPE_TRANSMIT_SA, 1 ],
            [ 3, ambd.API_RT_TYPE_TRANSMIT_SA, 6 ],
            [ 20, ambd.API_RT_TYPE_TRANSMIT_SA, 1 ],
            [ 5, ambd.API_RT_TYPE_TRANSMIT_SA, 1 ],
            [ 22, ambd.API_RT_TYPE_TRANSMIT_SA, 1 ],
            [ SHARED_BUFFER_SA, ambd.API_RT_TYPE_TRANSMIT_SA, 0 ],
            [ 21, ambd.API_RT_TYPE_TRANSMIT_SA, 1 ],
            [ 26, ambd.API_RT_TYPE_TRANSMIT_SA, 0 ],

            [ 17, ambd.API_RT_TYPE_RECEIVE_MODECODE, 0 ], # Synchronize With Data

            [ 0, ambd.API_RT_TYPE_TRANSMIT_MODECODE, 0 ], # Dynamic Bus Control
            [ 1, ambd.API_RT_TYPE_TRANSMIT_MODECODE, 0 ], # Synchronize
            [ 2, ambd.API_RT_TYPE_TRANSMIT_MODECODE, 0 ], # Transmit Status Word
            [ 3, ambd.API_RT_TYPE_TRANSMIT_MODECODE, 0 ], # Initiate Selftest
            [ 4, ambd.API_RT_TYPE_TRANSMIT_MODECODE, 0 ], # Transmitter Shutdown
            [ 5, ambd.API_RT_TYPE_TRANSMIT_MODECODE, 0 ], # Overwrite Transmitter Shutdown
            [ 6, ambd.API_RT_TYPE_TRANSMIT_MODECODE, 0 ], # Inhibit Terminal Flag
            [ 7, ambd.API_RT_TYPE_TRANSMIT_MODECODE, 0 ], # Overwrite Inhibit Terminal Flag
            [ 8, ambd.API_RT_TYPE_TRANSMIT_MODECODE, 0 ], # Reset RT
            [ 16, ambd.API_RT_TYPE_TRANSMIT_MODECODE, 0 ], # Transmit Vector Word
            [ 18, ambd.API_RT_TYPE_TRANSMIT_MODECODE, 0 ], # Transmit Last Command
            [ 19, ambd.API_RT_TYPE_TRANSMIT_MODECODE, 0 ] # Transmit BIT Word
            ]

rt31_sa =   [
            # SA/MC, TYPE, Word Count 
            [ 17, ambd.API_RT_TYPE_RECEIVE_SA, 8 ],
            [ 19, ambd.API_RT_TYPE_RECEIVE_SA, 1 ],
            [ 26, ambd.API_RT_TYPE_RECEIVE_SA, 1 ], # HSSA enabled by default

            [ 17, ambd.API_RT_TYPE_RECEIVE_MODECODE, 0 ], # Synchronize With Data

            [ 1, ambd.API_RT_TYPE_TRANSMIT_MODECODE, 0 ], # Synchronize
            [ 3, ambd.API_RT_TYPE_TRANSMIT_MODECODE, 0 ], # Initiate Selftest
            [ 4, ambd.API_RT_TYPE_TRANSMIT_MODECODE, 0 ], # Transmitter Shutdown
            [ 5, ambd.API_RT_TYPE_TRANSMIT_MODECODE, 0 ], # Overwrite Transmitter Shutdown
            [ 6, ambd.API_RT_TYPE_TRANSMIT_MODECODE, 0 ], # Inhibit Terminal Flag
            [ 7, ambd.API_RT_TYPE_TRANSMIT_MODECODE, 0 ], # Overwrite Inhibit Terminal Flag
            [ 8, ambd.API_RT_TYPE_TRANSMIT_MODECODE, 0 ],  # Reset RT
            ]

sa_type_name = ["RECEIVE", "TRANSMIT", "RECEIVE_MODECODE", "TRANSMIT_MODECODE"]


def LsPtpVtpSample_SetupRT(pymilapi, handle, ulRtAddr):
    isEmbedded = MilUtilCheckIfDeviceIsEmbedded(pymilapi, handle)

    if isEmbedded and (ulRtAddr != BROADCAST_RT):
        print("Enable RT%d (dual redundancy enabled, SA word count check %s)" %  (ulRtAddr, "enabled" if isEmbedded==1 else "disabled" ))
        pymilapi.PyApiCmdRTIni(handle, ulRtAddr, ambd.API_RT_ENABLE_DUAL_REDUNDANT, ambd.API_RT_RSP_BOTH_BUSSES, 8, ulRtAddr << 11)
    else:
        print("Enable RT%d (dual redundancy disabled, SA word count check %s)" % (ulRtAddr, "enabled" if isEmbedded==1 else "disabled"))
        pymilapi.PyApiCmdRTIni(handle, ulRtAddr, ambd.API_RT_ENABLE_SIMULATION, ambd.API_RT_RSP_BOTH_BUSSES, 8, ulRtAddr << 11)

    uwSharedHeaderId = MilUtilNextRTHid()

    # -- Setup RT --- 
    if ulRtAddr != BROADCAST_RT:
        for i in range(len(rt_sa)):
            if rt_sa[i][0] == SHARED_BUFFER_SA:
                uwHeaderId = uwSharedHeaderId
            else:
                uwHeaderId = MilUtilNextRTHid()

            # Enable SA/MC 
            print("Enable RT%d SA%3d  TYPE=%-20s WC=%3d" % (
                   ulRtAddr, #rt
                   rt_sa[i][0], #SA/MC
                   sa_type_name[rt_sa[i][1]], #TYPE
                   rt_sa[i][2])) # wc
                  
            MilUtilEnableRTSA(pymilapi, handle, ulRtAddr, 
                              rt_sa[i][0], # SA/MC
                              rt_sa[i][1], # TYPE
                              uwHeaderId)

            if (rt_sa[i][1] == ambd.API_RT_TYPE_RECEIVE_SA) or (rt_sa[i][1] == ambd.API_RT_TYPE_TRANSMIT_SA):
                if (isEmbedded):
                    pymilapi.PyApiCmdRTSADWCSet(handle, ulRtAddr, 
                                                rt_sa[i][0], #SA/MC
                                                rt_sa[i][1], #TYPE
                                                rt_sa[i][2]) # WORDCNT
    else:
        uwHeaderId = MilUtilNextRTHid()
        # -- Setup RT31 --- 
        for i in range(len(rt31_sa)):
            # Enable SA/MC 
            print("Enable RT%d SA%3d  TYPE=%-20s WC=%3d" % (
                  31, # rt
                  rt31_sa[i][0], #SA/MC
                  sa_type_name[rt31_sa[i][1]], #TYPE
                  rt31_sa[i][2] ))

            MilUtilEnableRTSA(pymilapi, handle, BROADCAST_RT,
                              rt31_sa[i][0], #SA/MC
                              rt31_sa[i][1], #TYPE
                              uwHeaderId)

            if (rt31_sa[i][1] == ambd.API_RT_TYPE_RECEIVE_SA) or (rt31_sa[i][1] == ambd.API_RT_TYPE_TRANSMIT_SA):
                if isEmbedded:
                    pymilapi.PyApiCmdRTSADWCSet(handle, BROADCAST_RT, 
                                                rt31_sa[i][0], # SA/MC
                                                rt31_sa[i][1], # TYPE
                                                rt31_sa[i][2])  # WORDCNT 


def LsPtpVtpSample_DisableRT(pymilapi, handle, ulRtAddr):
    pymilapi.PyApiCmdRTIni(handle, ulRtAddr, 
                           ambd.API_RT_DISABLE_OPERATION, 
                           ambd.API_RT_RSP_BOTH_BUSSES, 8, ulRtAddr << 11)


def LsPtpVtpSample_StartOperation(pymilapi, handle):
    # Start all RTs
    pymilapi.PyApiCmdRTStart(handle)


def LsPtpVtpSample_StopOperation(pymilapi, handle):
    # Stop all RTs 
    pymilapi.PyApiCmdRTHalt(handle)


def LsPtpVtpSample_Dump( count, data):
    for i in range(32):
        if (i % 8) == 0:
            print("\n%04X: " % (i),end="")
        print("%04X " % (data[i]),end="")


def LsPtpVtpSample_DTPHelp():
    szOn = "ON    "
    szOff = "OFF   "
    print("==============================================")
    print("Enabled RT = %d" % (rt_addr))
    print("==============================================")
    print("TF    SSF   BUSY  SRQ   FST   RT-ParityError")
    print("%s" % (szOn if ucTFBit==True else szOff), end="")
    print("%s" % (szOn if ucSSFBit==True else szOff), end="")
    print("%s" % (szOn if ucBUSYBit==True else szOff), end="")
    print("%s" % (szOn if ucSRQBit==True else szOff), end="")
    print("%s" % (szOn if ucFailSaftCondition==True else szOff), end="")
    print("%s" % (szOn if ucRtParityErr==True else szOff))
    print("==============================================")
    print("q: quit")
    print("1: Set TF on")
    print("2: Set TF off")
    print("3: Set SSF on")
    print("4: Set SSF off")
    print("5: Set BUSY on")
    print("6: Set BUSY off")
    print("7: Set SRQ on")
    print("8: Set SRQ off")
    print("9: Set Transmit Vector Word to 0x5555")
    print("a: Set Transmit BIT Word to 0x5555")
    print("b: Enable Fail Safe Timer Condition")
    print("c: Disable Fail Safe Timer Condition")
    print("d: Switch RT address")
    print("e: Set Error on the RT address lines on")


def LsPtpVtpSample_SetNXW(pymilapi, handle, ulRtAddr, ulBitMask, bSet):
    msg_dsp = pymilapi.PyApiCmdRTMsgRead(handle, ulRtAddr)
    ulNXW = msg_dsp.nxw

    if bSet == 0:
        # reset 
        ulNXW = ulNXW & (~ulBitMask)
    elif bSet == 1:
        # set 
        ulNXW = ulNXW | ulBitMask
    else:
        print("ERROR: set not in range")
    # redefine the Next RT Status Word 
    pymilapi.PyApiCmdRTNXW(handle, ulRtAddr, ulNXW)


def LsPtpVtpSample_DTP(pymilapi, handle):
    loop = 1
    data = 1 * [0]
    global rt_addr
    global ucTFBit
    global ucSSFBit
    global ucBUSYBit
    global ucSRQBit
    global ucFailSaftCondition
    global ucRtParityErr

    print("Ready for Diagnostic Test Procedure run:")
    LsPtpVtpSample_DTPHelp()

    while (loop):
        LsPtpVtpSample_DTPHelp()
        c = input("choice [h]? " or 'h')
        print("key = %c" % (c))
        if c == 'q':
            loop = 0
            LsPtpVtpSample_Help()
        elif c == '1':
            LsPtpVtpSample_SetNXW(pymilapi, handle, rt_addr, 0x1, True)
            ucTFBit = 1
        elif c == '2':
            LsPtpVtpSample_SetNXW(pymilapi, handle, rt_addr, 0x1, False)
            ucTFBit = 0
        elif c == '3':
            LsPtpVtpSample_SetNXW(pymilapi, handle, rt_addr, 0x4, True)
            ucSSFBit = 1
        elif c == '4':
            LsPtpVtpSample_SetNXW(pymilapi, handle, rt_addr, 0x4, False)
            ucSSFBit = 0
        elif c == '5':
            LsPtpVtpSample_SetNXW(pymilapi, handle, rt_addr, 0x8, True)
            ucBUSYBit = 1
        elif c == '6':
            LsPtpVtpSample_SetNXW(pymilapi, handle, rt_addr, 0x8, False)
            ucBUSYBit = 0
        elif c == '7':
            LsPtpVtpSample_SetNXW(pymilapi, handle, rt_addr, 0x100, True)
            ucSRQBit = 1
        elif c == '8':
            LsPtpVtpSample_SetNXW(pymilapi, handle, rt_addr, 0x100, False)
            ucSRQBit = 0
        elif c == '9':
            ulBufHeaderIndex,_ = pymilapi.PyApiCmdRTGetSABufferHeaderInfo(handle, 
                                                    rt_addr, 
                                                    ambd.API_RT_TYPE_TRANSMIT_MODECODE, 
                                                    16)# SA/MC
            data[0] = 0x5555
            pymilapi.PyApiCmdBufDef(handle, 
                                    ambd.API_BUF_RT_MSG, 
                                    ulBufHeaderIndex, 
                                    ambd.API_BUF_WRITE_TO_CURRENT, 
                                    data)
        elif c == 'a':
            ulBufHeaderIndex, _ = pymilapi.PyApiCmdRTGetSABufferHeaderInfo(handle, 
                                                     rt_addr, 
                                                     ambd.API_RT_TYPE_TRANSMIT_MODECODE, 
                                                     19) # SA/MC
            data[0] = 0x5555
            pymilapi.PyApiCmdBufDef(handle, ambd.API_BUF_RT_MSG, ulBufHeaderIndex, ambd.API_BUF_WRITE_TO_CURRENT, data)
        elif c == 'b':
            pymilapi.PyApiCmdCalSigCon(handle, ambd.API_CAL_BUS_PRIMARY, ambd.API_ENA)
            pymilapi.PyApiCmdCalSigCon(handle, ambd.API_CAL_BUS_SECONDARY, ambd.API_ENA)
            ucFailSaftCondition = 1
        elif c == 'c':
            pymilapi.PyApiCmdCalSigCon(handle, ambd.API_CAL_BUS_PRIMARY, ambd.API_DIS)
            pymilapi.PyApiCmdCalSigCon(handle, ambd.API_CAL_BUS_SECONDARY, ambd.API_DIS)
            ucFailSaftCondition = 0
        elif c == 'd':
            LsPtpVtpSample_DisableRT(pymilapi,handle, rt_addr)
            MilUtilResetIds()
            ulNewRtAddr = int(input('RT address to switch to: ') or int(0))
            LsPtpVtpSample_PrepareAndStartRTOperation(pymilapi, handle, ulNewRtAddr)
            rt_addr = ulNewRtAddr
        elif c == 'e':
            # disable RT on the fly to stimulate an address line error
            pymilapi.PyApiCmdRTEnaDis(handle, rt_addr, ambd.API_DIS)
        else:
            LsPtpVtpSample_DTPHelp()


def LsPtpVtpSample_DataDump(pymilapi, handle):
    # Dump Data of the receive SAs on RT 
    for  i in range(len(rt_sa)):
        if  rt_sa[i][1] == ambd.API_RT_TYPE_RECEIVE_SA:
            ulBufHeaderIndex, ulBufHeaderAddr= pymilapi.PyApiCmdRTGetSABufferHeaderInfo(handle,
                                                  rt_addr, 
                                                  ambd.API_RT_TYPE_RECEIVE_SA, 
                                                  rt_sa[i][0]) #SA/MC

            data, _,_ = pymilapi.PyApiCmdBufRead(handle, 
                                                ambd.API_BUF_RT_MSG, 
                                                ulBufHeaderIndex, 
                                                ambd.API_BUF_READ_FROM_LAST, 
                                                32)
            print("\nRT RCV SA%d" % (rt_sa[i][0]),end="")
            LsPtpVtpSample_Dump(32, data)


def LsPtpVtpSample_Status(pymilapi, handle):
    xRtStatus = pymilapi.PyApiCmdRTStatusRead(handle)
    print("RTs ( %d messages  / %d errors )" % (xRtStatus.glb_msg_cnt, xRtStatus.glb_err_cnt))


def LsPtpVtpSample_Help():
    print("q: quit")
    print("p: RT1553 PTP/VTP Mode")
    print("s: status")
    print("d: dump data")
    print("h: help")


def LsPtpVtpSample_PrepareAndStartRTOperation(pymilapi, handle, ucNewRtAddr):
    LsPtpVtpSample_SetupRT(pymilapi, handle, ucNewRtAddr)
    LsPtpVtpSample_SetupRT(pymilapi, handle, BROADCAST_RT)
    LsPtpVtpSample_StartOperation(pymilapi, handle)


#######################################################################################
#                                                                                     #
#                                     Sample entry point                              #
#                                                                                     #
#######################################################################################
def LsPtpVtpSample(pymilapi, handle):
    loop = 1
    try:
        print("Running LsPtpVtpSample")
        LsPtpVtpSample_PrepareAndStartRTOperation(pymilapi, handle, rt_addr)
        print("RT simulation running")
        LsPtpVtpSample_Help()

        while (loop):
            c = input()

            if c == 'q':
                loop = 0
            elif c == 'p':
                LsPtpVtpSample_DTP(pymilapi, handle)
            elif c == 'd':
                LsPtpVtpSample_DataDump(pymilapi, handle)
            elif c == 's':
                LsPtpVtpSample_Status(pymilapi, handle)
            else:
                LsPtpVtpSample_Help()
    finally:
        LsPtpVtpSample_StopOperation(pymilapi, handle)

