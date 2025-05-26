/* SPDX-FileCopyrightText: 2010-2022 AIM GmbH <info@aim-online.com> */
/* SPDX-License-Identifier: MIT */

/*
This sample prints the board features, versions and BITE status.
This sample reads all the versions from the boards and displays them.
The build in test(BITE) is executed and the result is displayed.
See 1553 Reference Manual function ApiCmdBite for details.
This test is executed with internal coupling.No external connection is required.
*/

#include "1553_sample.h"


AiReturn PrintBoardMemoryInfo(AiUInt32 ulModHandle);

AiReturn PrintBoardVersions(AiUInt32 ulModHandle, TY_API_RESET_INFO * pxReset)
{
    unsigned int i;
    TY_VER_INFO       * versions = NULL;
    TY_VERSION_OUT      version_result = {0};
    AiUInt32            board_info[TY_BOARD_INFO_MAX];
    AiUInt32            board_info_count;
    TY_COUPLING_CAPABILITIES coupling_capabilities;
    TY_IRIG_CAPABILITIES     irig_capabilities;

    memset( board_info, 0, sizeof(board_info ) );
    memset( &coupling_capabilities, 0, sizeof(coupling_capabilities ) );
    memset( &irig_capabilities, 0, sizeof(irig_capabilities ) );

    AiReturn             ret            = 0;
    AiUInt8             aucBiteResult[]    = { 0, 0 };


    printf("Running PrintBoardVersions \n");

    ret = ApiCmdSysGetBoardInfo(ulModHandle, 0, TY_BOARD_INFO_MAX, board_info, &board_info_count);

    AIM_ERROR_MESSAGE_RETURN( ret );

    if (board_info_count<TY_BOARD_INFO_MAX)
        printf("Warning: not all information is valid.\n");

    printf("\n");
    printf("Board info:\n");
    printf("\n");
    printf("TY_BOARD_INFO_SERIAL :               %d\n", board_info[TY_BOARD_INFO_SERIAL]);
    printf("TY_BOARD_INFO_BOARD_TYPE :           0x%04X\n", board_info[TY_BOARD_INFO_BOARD_TYPE]);
    printf("TY_BOARD_INFO_BOARD_CONFIG :         0x%04X\n", board_info[TY_BOARD_INFO_BOARD_CONFIG]);
    printf("TY_BOARD_INFO_DEVICE_TYPE :          %d\n", board_info[TY_BOARD_INFO_DEVICE_TYPE]);
    printf("TY_BOARD_INFO_CHANNEL_COUNT :        %d\n", board_info[TY_BOARD_INFO_CHANNEL_COUNT]);
    printf("TY_BOARD_INFO_DISCRETE_CNT :         %d\n", board_info[TY_BOARD_INFO_DISCRETE_CNT]);
    printf("TY_BOARD_INFO_HAS_EXTERNAL_IRIG :    %d\n", board_info[TY_BOARD_INFO_HAS_EXTERNAL_IRIG]);
    printf("TY_BOARD_INFO_HAS_EXTERNAL_TRIGGER : %d\n", board_info[TY_BOARD_INFO_HAS_EXTERNAL_TRIGGER]);

    if ((board_info[TY_BOARD_INFO_BOARD_CONFIG]&0xFF) == AI_PLATFORM_USB3)
    {
        // Print AIM Power Class on USB-C devices
        printf("TY_BOARD_INFO_USB_AIM_POWER_CLASS :  %d (%s)\n", board_info[TY_BOARD_INFO_USB_AIM_POWER_CLASS], TY_AIM_POWER_CLASS_STRING(board_info[TY_BOARD_INFO_USB_AIM_POWER_CLASS]));
    }

    printf("\n");
    printf("Stream info:\n");
    printf("\n");
    printf("TY_BOARD_INFO_PROTOCOL :             %d\n", board_info[TY_BOARD_INFO_PROTOCOL]);
    printf("TY_BOARD_INFO_APPLICATION_TYPE :     %d\n", board_info[TY_BOARD_INFO_APPLICATION_TYPE]);
    printf("TY_BOARD_INFO_IS_MULTI_CHANNEL :     %d\n", board_info[TY_BOARD_INFO_IS_MULTI_CHANNEL]);


    printf("\n");
    coupling_capabilities.ul_All = board_info[TY_BOARD_INFO_COUPLING];
    irig_capabilities.ul_All     = board_info[TY_BOARD_INFO_IRIG];

    printf("  Amplitude variable : %s\n", board_info[TY_BOARD_INFO_CHANGE_AMPL] ? "YES" : "NO");
    printf("  IRIG free wheeling : %s\n", irig_capabilities.b.b_FreeWheeling    ? "YES" : "NO");
    printf("  IRIG switch :        %s\n", irig_capabilities.b.b_IrigSwitch      ? "YES" : "NO");
    printf("  IRIG sinusoidal :    %s\n", irig_capabilities.b.b_Sinusoidal      ? "YES" : "NO");
    printf("  Packed coupling :    %s\n", coupling_capabilities.b.b_Packed      ? "YES" : "NO");

    printf("\n");
    printf("  Coupling modes:\n");

    if (coupling_capabilities.b.b_Isolated)     printf("    API_CAL_CPL_ISOLATED\n");
    if (coupling_capabilities.b.b_DigitalWrap)  printf("    API_CAL_CPL_WRAP_AROUND_LOOP\n");
    if (coupling_capabilities.b.b_Transformer)  printf("    API_CAL_CPL_TRANSFORM\n");
    if (coupling_capabilities.b.b_Direct)       printf("    API_CAL_CPL_DIRECT\n");
    if (coupling_capabilities.b.b_Network)      printf("    API_CAL_CPL_EXTERNAL\n");


    /* -- Check how many versions there may be available and allocate
     *--- an array with enough space--- */
    ret = ApiReadAllVersions(ulModHandle, 0, NULL, &version_result);

    AIM_ERROR_MESSAGE_RETURN(ret);

    versions = AiOsMalloc(sizeof(TY_VER_INFO) * version_result.ul_MaxCount);

    if(!versions)
    {
        printf( "  Failed to allocate memory for version read\n");
        return 1;
    }

    /* Now read all versions of the board */
    ret = ApiReadAllVersions( ulModHandle, version_result.ul_MaxCount, versions, &version_result);
    
	if( ret != API_OK )
	{
        AiOsFree(versions);
	    AIM_ERROR_MESSAGE_RETURN(ret);
	}

    printf("\n");
    printf("Version info : \n");

    for(i = 0; i < version_result.ul_Count; i++)
    {
        printf("  %-30s:  %-40s\n", versions[i].ac_Description, versions[i].ac_FullVersion);
    }

    printf("\n");

    AiOsFree(versions);
    


    /* -- Execute onboard selftest --- */
    printf("\nExecuting selftest ... ");

    ret = ApiCmdBite( ulModHandle, 0, API_BITE_ALL, aucBiteResult );

    AIM_ERROR_MESSAGE_RETURN( ret );


    if( aucBiteResult[0] != 0 )
        printf(" FAILED %2d/%2d \n", (AiInt32)aucBiteResult[0], (AiInt32)aucBiteResult[1] );
    else
        printf(" PASSED %2d/%2d \n", (AiInt32)aucBiteResult[0], (AiInt32)aucBiteResult[1] );

    return 0;
}





AiReturn PrintBoardMemoryInfo(AiUInt32 ulModHandle)
{
    TY_API_GET_MEM_INFO xMemInfo;

    memset( &xMemInfo, 0, sizeof(xMemInfo) );

    /* Read mem info from target */
    ApiCmdSysGetMemPartition( ulModHandle, 1, &xMemInfo );

    return 0;
}




