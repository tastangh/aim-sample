# SPDX-FileCopyrightText: 2023-2024 AIM GmbH <info@aim-online.com>
# SPDX-License-Identifier: MIT

"""This is the sample main function. 
This sample opens and resets the device and calls any of the sub-samples as requested.
"""


import sys

from aim_mil import mil_bindings_defines as ambd
from aim_mil import mil_bindings_types as ambt
from aim_mil.mil_bindings_api import MilApi

from src.ls.mil_utils import MilUtilReadConfiguration, \
                             MilUtilPrintVersionInfo, \
                             MilUtilConfigureHardware, \
                             MilUtilCheckIfDeviceIsSimulatorOrSingleFunction, \
                             MilUtilCheckIfDeviceIsEmbedded
from src.ls.template_sample import TemplateSample
from src.ls.print_versions import PrintBoardVersions
from src.ls.ls_bc_rt_bm_sample import LsBcRtBmSample
from src.ls.ls_acyclic_sample import LsAcyclicSample
from src.ls.ls_instruction_list_sample import LsInstructionListSample
from src.ls.ls_recording_replay_sample import LsRecordingReplaySample
from src.ls.ls_dynamic_data_sample import LsDynamicDataSample
from src.ls.ls_buffer_queue_sample import LsBufferQueueSample
from src.ls.ls_interrupt_sample import LsInterruptSample
from src.ls.ls_fifo_sample import LsFifoSample

from src.ls.ls_milscope_sample_ape import LsMilscopeSampleAPE
from src.ls.ls_ptp_vtp_sample import LsPtpVtpSample
from src.ls.ls_bm_sample import LsBmSample
from src.ls.ls_bc_sample import LsBcSample


if __name__ == '__main__':
    """AIM Python example for using the aim_mil bindings to access the AIM MIL-API library."""
    pymilapi = MilApi()

    print("*******************************************************************************")
    print("***          This program is licensed under the MIT license.                ***")
    print("***       Copyright (c) 2023-2024 AIM GmbH <info@aim-online.com>            ***")
    print("*******************************************************************************")
    
    xConfig = MilUtilReadConfiguration(pymilapi, sys.argv )

    # -- 1.) Open the API library --- 
    if xConfig["ucUseServer"]:
        num_devices = pymilapi.PyApiConnectToServer(xConfig["acServer"])
    else:    
        num_devices = pymilapi.PyApiInit()

    if num_devices < 1:
        raise Exception("No devices found")

    '''-- 2.) Open a stream on a board --- 
    This main program shows how to open a single stream
    on one device. If multiple streams should be opened
    the programming sequence would be:
    Call ApiOpenEx for all streams to be opened and store all module handles
    Call ApiCmdReset on all the module handles.   
    '''
    handle = pymilapi.PyApiOpenEx( ambt.PY_TY_API_OPEN(xConfig["ulDevice"], xConfig["ulStream"], xConfig["acServer"]) )
    try:
        MilUtilPrintVersionInfo(pymilapi, handle)
        # -- 3.) Check if a different process is already connected to this device  --- 
        xDriverInfo = pymilapi.PyApiGetDriverInfo( handle )

        ucResetStream = True
        if( xDriverInfo.ul_OpenConnections > 1 ):
            # Different application is connected 
            # Check for options                  
            print('Another application is already connected to this device')
            print('0 : Abort ')
            print('1 : Skip reset and continue')
            print('2 : Reset stream and continue')

            value = int(input('choice [0] ? ') or '0')
            if value == 1:
                ucResetStream = False
            elif value == 2:
                ucResetStream = True
            else:
                raise Exception('Abort')

        if ucResetStream:
            '''-- 5.) Reset the stream --- 
            ApiCmdReset can be used to reset the stream associated with the module handle. 
            The function does however also initialize the simulation buffer with zeros.
            Since the simulation buffer is shared between all streams calling ApiCmdReset 
            in a multi-tasking or multi-threading environment does affect the data transmitted 
            or received by all streams of the board. To avoid this use the option 
            API_RESET_WITHOUT_SIMBUF instead of API_RESET_ALL below.
            '''        
            if xDriverInfo.ul_OpenConnections > 1:
                ucResetMode = ambd.API_RESET_WITHOUT_SIMBUF
            else:
                ucResetMode = ambd.API_RESET_ALL

            pymilapi.PyApiCmdReset(handle, ucResetMode)
        
        # First configure the hardware of the current stream
        MilUtilConfigureHardware(pymilapi, handle, xConfig["ulCoupling"])

        ######################################################################################
        #                                                                                    # 
        #    6.)                      The board is now ready to be used                      # 
        #                                                                                    #  
        ######################################################################################
        ucIsSimulatorOrSingle = MilUtilCheckIfDeviceIsSimulatorOrSingleFunction(pymilapi, handle)
        ucIsEmbedded = MilUtilCheckIfDeviceIsEmbedded(pymilapi, handle)

        if (ucIsSimulatorOrSingle):
            # Device is simulator only or single function. Not all tests are possible 
            if (xConfig["ulTest"] > 1) and (xConfig["ulTest"] < 21):
                raise Exception('Error: only the samples 0, 1, 21 and 22 are possible on boards with limited funktionality')

        if (ucIsEmbedded):
            # Device is embedded. Not all tests are possible 
            if (xConfig["ulTest"] == 5) or (xConfig["ulTest"] == 6) or (xConfig["ulTest"] == 20):
                raise Exception('Error: the sample is not possible on embedded devices')

        # LS Samples with internal coupling
        if xConfig["ulTest"] == 0:
            TemplateSample( pymilapi, handle )
        elif xConfig["ulTest"] == 1:
            PrintBoardVersions(pymilapi, handle)
        elif xConfig["ulTest"] == 2:
            LsBcRtBmSample(pymilapi, handle)
        elif xConfig["ulTest"] == 3:
            LsAcyclicSample(pymilapi, handle)
        elif xConfig["ulTest"] == 4:
            LsInstructionListSample(pymilapi, handle)
        elif xConfig["ulTest"] == 5:
            LsRecordingReplaySample(pymilapi, handle, xConfig["ulStream"])
        elif xConfig["ulTest"] == 6:
            LsDynamicDataSample(pymilapi, handle)
        elif xConfig["ulTest"] == 7:
            LsBufferQueueSample(pymilapi, handle)
        elif xConfig["ulTest"] == 8:
            LsInterruptSample(pymilapi, handle)
        elif xConfig["ulTest"] == 9:
            LsFifoSample(pymilapi, handle)
        # LS Samples with external coupling
        elif xConfig["ulTest"] == 20:
            LsMilscopeSampleAPE(pymilapi, handle)		
        elif xConfig["ulTest"] == 21:
            LsPtpVtpSample(pymilapi, handle)
        elif xConfig["ulTest"] == 22:
            LsBmSample(pymilapi, handle, xConfig["ulStream"], xConfig["ucUseServer"])
        elif xConfig["ulTest"] == 23:
            LsBcSample(pymilapi, handle)
        else:
            print("Sample %d not found" % (xConfig["ulTest"]))
                    
    finally:
        #######################################################################################
        #                                                                                     # 
        #                                           done                                      #
        #                                                                                     #
        #######################################################################################
        # -- 7.) Close the connection to the board --- 
       pymilapi.PyApiClose(handle)

       if xConfig["ucUseServer"]:
          pymilapi.PyApiDisconnectFromServer(xConfig["acServer"])
       else:
          pymilapi.PyApiExit()
