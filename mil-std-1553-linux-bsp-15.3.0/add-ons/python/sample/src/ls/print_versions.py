# SPDX-FileCopyrightText: 2023-2024 AIM GmbH <info@aim-online.com>
# SPDX-License-Identifier: MIT

"""This sample prints the board features, versions and BITE status.
This sample reads all the versions from the boards and displays them. 
The build in test (BITE) is executed and the result is displayed. 
See 1553 Reference Manual function ApiCmdBite for details. 
This test is executed with internal coupling. No external connection is required.
"""

from aim_mil import mil_bindings_defines as ambd
from aim_mil import mil_bindings_types as ambt

def PrintBoardVersions(pymilapi, handle):
    print("Running PrintBoardVersions:")
    board_info = pymilapi.PyApiCmdSysGetBoardInfo(handle, 0, ambd.TY_BOARD_INFO_MAX)

    print("Board info:")
    print('TY_BOARD_INFO_SERIAL :               %d' % (board_info[ambd.TY_BOARD_INFO_SERIAL]))
    print('TY_BOARD_INFO_BOARD_TYPE :           0x%04X' % board_info[ambd.TY_BOARD_INFO_BOARD_TYPE])
    print('TY_BOARD_INFO_BOARD_CONFIG :         0x%04X' % board_info[ambd.TY_BOARD_INFO_BOARD_CONFIG])
    print('TY_BOARD_INFO_DEVICE_TYPE :          %d' % board_info[ambd.TY_BOARD_INFO_DEVICE_TYPE])
    print('TY_BOARD_INFO_CHANNEL_COUNT :        %d' % board_info[ambd.TY_BOARD_INFO_CHANNEL_COUNT])
    print('TY_BOARD_INFO_DISCRETE_CNT :         %d' % board_info[ambd.TY_BOARD_INFO_DISCRETE_CNT])
    print('TY_BOARD_INFO_HAS_EXTERNAL_IRIG :    %d' % board_info[ambd.TY_BOARD_INFO_HAS_EXTERNAL_IRIG])
    print('TY_BOARD_INFO_HAS_EXTERNAL_TRIGGER : %d' % board_info[ambd.TY_BOARD_INFO_HAS_EXTERNAL_TRIGGER])

    if ((board_info[ambd.TY_BOARD_INFO_BOARD_CONFIG]&0xFF) == ambd.AI_PLATFORM_USB3):
        #Print AIM Power Class on USB-C devices
        print('TY_BOARD_INFO_USB_AIM_POWER_CLASS :  %d (%s)' % (board_info[ambd.TY_BOARD_INFO_USB_AIM_POWER_CLASS], ambd.TY_AIM_POWER_CLASS_STRING(board_info[ambd.TY_BOARD_INFO_USB_AIM_POWER_CLASS])))

    print()
    print('Stream info:')
    print('TY_BOARD_INFO_PROTOCOL :             %d' % board_info[ambd.TY_BOARD_INFO_PROTOCOL])
    print('TY_BOARD_INFO_APPLICATION_TYPE :     %d' % board_info[ambd.TY_BOARD_INFO_APPLICATION_TYPE])
    print('TY_BOARD_INFO_IS_MULTI_CHANNEL :     %d' % board_info[ambd.TY_BOARD_INFO_IS_MULTI_CHANNEL])

    print()
    coupling_capabilities = ambt.PY_TY_COUPLING_CAPABILITIES.fromBitfield(board_info[ambd.TY_BOARD_INFO_COUPLING])
    irig_capabilities = ambt.PY_TY_IRIG_CAPABILITIES.fromBitfield(board_info[ambd.TY_BOARD_INFO_IRIG])

    print('  Amplitude variable : %s' % ('YES' if (board_info[ambd.TY_BOARD_INFO_CHANGE_AMPL] == True) else 'NO'))
    print('  IRIG free wheeling : %s' % ('YES' if (irig_capabilities.b_FreeWheeling == True) else 'NO'))
    print('  IRIG switch :        %s' % ('YES' if (irig_capabilities.b_IrigSwitch  == True) else 'NO'))
    print('  IRIG sinusoidal :    %s' % ('YES' if (irig_capabilities.b_Sinusoidal == True) else 'NO'))
    print('  IRIG instant set :   %s' % ('YES' if (irig_capabilities.b_InstantIrigSetOnInternalMode == True) else 'NO'))
    print('  Packed coupling :    %s' % ('YES' if (coupling_capabilities.b_Packed == True) else 'NO'))

    print()
    print('  Coupling modes :' )

    if (coupling_capabilities.b_Isolated):     print('    API_CAL_CPL_ISOLATED')
    if (coupling_capabilities.b_DigitalWrap):  print('    API_CAL_CPL_WRAP_AROUND_LOOP')
    if (coupling_capabilities.b_Transformer):  print('    API_CAL_CPL_TRANSFORM')
    if (coupling_capabilities.b_Direct):       print('    API_CAL_CPL_DIRECT')
    if (coupling_capabilities.b_Network):      print('    API_CAL_CPL_EXTERNAL')

    print()
    print('Version info:')
    versions = pymilapi.PyApiReadAllVersions(handle)

    for v in versions:
        print( v.ac_Description, ":", v.ac_FullVersion )

    print()
    print()
    print("Now execute BITE")
    bite_status = pymilapi.PyApiCmdBite(handle, ambd.API_BITE_ALL)

    result = ((bite_status[0] or bite_status[1]) and 'Failed') or 'Passed'
    print( 'Result: %s, status[%d/%d]' % (result, bite_status[0], bite_status[1]) )




