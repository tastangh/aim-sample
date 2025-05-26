/* SPDX-FileCopyrightText: 2010-2024 AIM GmbH <info@aim-online.com> */
/* SPDX-License-Identifier: MIT */

#include "1553_sample.h"



int main( int argc, char *argv[ ] )
{
    ConfigStruct         xConfig;
    TY_API_RESET_INFO    xApiResetInfo;
    TY_API_OPEN          xApiOpen;
    TY_API_DRIVER_INFO   xDriverInfo;

    AiUInt8             ucIsFullFunction      = 1;
    AiUInt8             ucIsEmbedded          = 1;
    AiUInt8             ucIsSimulatorOrSingle = 1;
    AiUInt32            ulBoardCount           = 0;
    AiInt16             wBoardCount           = 0;
    AiUInt32            ulModHandle           = 0;
    AiReturn            ret               = 0;
    AiUInt32            ulValue               = 0;
    AiUInt8             ucResetMode           = 0;
    AiUInt8             ucResetStream = 1;

    char                szDummy[32];

    memset( &xApiResetInfo, 0, sizeof(xApiResetInfo) );
    memset( &xApiOpen,      0, sizeof(xApiOpen)      );
    memset( &xDriverInfo,   0, sizeof(xDriverInfo)   );


    /* -- Initialize the sample --- */
    /* 
     * For VxWorks VME this function will map the board.
     * This is specific to the CPU and the VxWorks BSP in use.
     * Some examples for mapping are added to Sample/src/os/vxworks/map folder. 
     * In case one of these CPUs are being used, they can be enabled with the appropriate define
     * in the sample project
     * 
     * For Xenomai this function will create a real time task and can be used to initialize the rt_printf buffer.

     * For Windows, Linux, RTX64 this function does nothing.
     */
    sample_map_board();

    printf("*******************************************************************************\n");
    printf("***          This program is licensed under the MIT license.                ***\n");
    printf("***       Copyright (c) 2010-2024 AIM GmbH <info@aim-online.com>            ***\n");
    printf("*******************************************************************************\n");


    ReadConfiguration( argc, argv, &xConfig );



    /* -- 1.) Open the API library --- */

    if( xConfig.ucUseServer )
    {
        /* Remote board access */
        printf("Connecting to server %s...\n", xConfig.acServer);
        ret = ApiConnectToServer( xConfig.acServer, &wBoardCount );
        ulBoardCount = wBoardCount;

        if( ret )
        {
            /* Open failed */
            printf("Connect to server %s failed with 0x%08X\n", xConfig.acServer, ret );
            printf("Error : %s\n", ApiGetErrorMessage(ret));
            WaitForUser();
            return 1;
        }
    }
    else
    {
        /* Local board access */
        ulBoardCount = ApiInit();
    }

    if( ulBoardCount < 1 )
    {
        /* No board found => exit */
        printf( "No module found on %s.\n", xConfig.acServer );
        WaitForUser();
        return 1;
    }

    printf( "Found %d boards on %s\n", ulBoardCount, xConfig.acServer );

    /* -- 2.) Open a stream on a board --- */

    /* This main program shows how to open a singele stream
       on one device. If multiple streams should be opened
       the programming sequence would be:
       Call ApiOpenEx for all streams to be opened and store all module handles
       Call ApiCmdReset on all the module handles.
    */
 
    /* The open parameters are set in ReadConfiguration() */
    
    xApiOpen.ul_Module = xConfig.ulDevice;
    xApiOpen.ul_Stream = xConfig.ulStream;
    AiOsSnprintf(xApiOpen.ac_SrvName, 28, AiTrue, "%s", xConfig.acServer);

    ret = ApiOpenEx( &xApiOpen, &ulModHandle );
 
    if( ret )
    {
        /* Open failed */
        printf("Open stream %d on module %d failed with 0x%08X\n", xApiOpen.ul_Stream, xApiOpen.ul_Module, ret );
        AIM_ERROR_MESSAGE(ret);

        WaitForUser();
        return 2;
    }

    PrintVersionInfo(ulModHandle);


    /* -- 3.) Check if a different process is already connected to this device  --- */

    /* this command might not be available in old drivers */
    ret = ApiGetDriverInfo( ulModHandle, &xDriverInfo );

    if( ret == API_OK )
    {
        if( xDriverInfo.ul_OpenConnections > 1 )
        {
            /* Different application is connected */
            /* Check for options                  */
            printf("Another application is already connected to this device\n");
            printf("0 : Abort \n");
            printf("1 : Skip reset and continue\n");
            printf("2 : Reset stream and continue\n");

            printf("choice [0] ? ");
            AiOsGetString( szDummy, sizeof(szDummy), "0" );
            sscanf( szDummy, "%d", &ulValue );

            switch( ulValue )
            {
                case 1 : ucResetStream = 0; break;
                case 2 : ucResetStream = 1; break;
                default:
                    ApiClose( ulModHandle );
                    WaitForUser();
                    return 3; /* Abort */
            }
        }
    }



    if (ucResetStream)
    {
        /* -- 5.) Reset the stream --- */
        /* ApiCmdReset can be used to reset the stream associated with the module handle. 
           The function does however also initialize the simulation buffer with zeros.
           Since the simulation buffer is shared between all streams calling ApiCmdReset 
           in a multi-tasking or multi-threading environment does affect the data transmitted 
           or received by all streams of the board. To avoid this use the option 
           API_RESET_WITHOUT_SIMBUF instead of API_RESET_ALL below.
        */
        if (xDriverInfo.ul_OpenConnections > 1)
            ucResetMode = API_RESET_WITHOUT_SIMBUF;
        else
            ucResetMode = API_RESET_ALL;

        ret = ApiCmdReset(ulModHandle, 0, ucResetMode, &xApiResetInfo);

        if( ret )
        {
            /* Reset failed */
            printf("Reset stream %d on module %d failed\n", xApiOpen.ul_Stream, xApiOpen.ul_Module);
            AIM_ERROR_MESSAGE(ret);
            ApiClose(ulModHandle);
            WaitForUser();
            return 5;
        }
    }

    /* First configure the hardware of the current stream */
    ret = MilUtilConfigureHardware(ulModHandle, xConfig.ulCoupling);

    if (ret)
    {
        /* configure the hardware failed */
        printf("MilUtilConfigureHardware error : %s\n", ApiGetErrorMessage(ret));
        ApiClose(ulModHandle);
        WaitForUser();
        return 5;
    }

    /* ######################################################################################*/
    /*                                                                                       */
    /* -- 6.)                      The board is now ready to be used                     --- */
    /*                                                                                       */
    /* ######################################################################################*/
 
    ucIsSimulatorOrSingle = CheckIfDeviceIsSimulatorOrSingleFunction(ulModHandle);
    ucIsEmbedded = CheckIfDeviceIsEmbedded(ulModHandle);

    if (ucIsSimulatorOrSingle)
    {
        /* Device is simulator only or single function. Not all tests are possible */
        if ((xConfig.ulTest > 1) && (xConfig.ulTest < 21))
        {
            printf("Error: only the samples 0, 1, 21 and 22 are possible on boards with limited funktionality\n");
            ApiClose(ulModHandle);
            ApiExit();
            WaitForUser();
            return 6;
        }
    }

    if (ucIsEmbedded)
    {
        /* Device is embedded. Not all tests are possible */
        if ((xConfig.ulTest == 5) || (xConfig.ulTest == 6) || (xConfig.ulTest == 10) || (xConfig.ulTest == 20))
        {
            printf("Error: the sample is not possible on embedded devices\n");
            ApiClose(ulModHandle);
            ApiExit();
            WaitForUser();
            return 6;
        }
    }

    switch( xConfig.ulTest )
    {
        /* LS Samples with internal coupling */
        case  0 : ret = TemplateSample(                 ulModHandle  ); break;
        case  1 : ret = PrintBoardVersions(             ulModHandle, &xApiResetInfo  ); break;
        case  2 : ret = LsBcRtBmSample(                 ulModHandle  ); break;
        case  3 : ret = LsAcyclicSample(                ulModHandle  ); break;
        case  4 : ret = LsInstructionListSample(        ulModHandle  ); break;
        case  5 : ret = LsRecordingReplaySample(        ulModHandle, xConfig.ulStream, xConfig.ucUseServer ); break;
        case  6 : ret = LsDynamicDataSample(            ulModHandle  ); break;
        case  7 : ret = LsBufferQueueSample(            ulModHandle  ); break;
        case  8 : ret = LsInterruptSample(              ulModHandle  ); break;
        case  9 : ret = LsFifoSample(                   ulModHandle  ); break;
        case 10 : ret = LsBcRtBmTcbSample(              ulModHandle); break;

        /* LS Samples with external coupling */
        case 20: ret = LsMilscopeSampleAPE(ulModHandle); break;
        case 21: ret = LsPtpVtpSample(ulModHandle); break;
        case 22: ret = LsBmSample(ulModHandle, xConfig.ulStream, xConfig.ucUseServer); break;
        case 23: ret = LsBcSample(ulModHandle); break;

        /* LsPtpVtpSample automatic mode to call e.g. from a CI system */
        case 29: ret = LsPtpVtpSample_Automatic(ulModHandle); break;
        default:
            printf("Sample %d not found\n", xConfig.ulTest );
            break;
    }
    
    if( ret )
    {
        /* Sample failed */
        printf("Sample %d on stream %d on module %d failed with 0x%08X\n", xConfig.ulTest, xApiOpen.ul_Stream, xApiOpen.ul_Module, ret );
        WaitForUser();
        ApiExit();
        return 6;
    }


    /* ######################################################################################*/
    /*                                                                                       */
    /*                                           done                                        */
    /*                                                                                       */
    /* ######################################################################################*/

    /* -- 7.) Close the connection to the board --- */
    ret = ApiClose( ulModHandle );

    if( ret )
    {
        /* Reset failed */
        printf("Close stream %d on module %d failed with 0x%08X\n", xApiOpen.ul_Stream, xApiOpen.ul_Module, ret );
        AIM_ERROR_MESSAGE(ret);
        ApiExit();
        WaitForUser();
        return 7;
    }

    if( xConfig.ucUseServer )
        ApiDisconnectFromServer( xConfig.acServer );

    sample_unmap_board();

    WaitForUser();

    ApiExit();

    return 0;
}



void PrintSampleList()
{
    printf("\n");
    printf("Samples :\n");
    
    printf("-- Samples with internal coupling ---- \n"                );
    printf(" 0  TemplateSample\n"                 );
    printf(" 1  PrintBoardVersions\n"             );
    printf(" 2  LsBcRtBmSample\n");
    printf(" 3  LsAcyclicSample\n"                );
    printf(" 4  LsInstructionListSample\n"        );
    printf(" 5  LsRecordingReplaySample\n"        );
    printf(" 6  LsDynamicDataSample\n"            );
    printf(" 7  LsBufferQueueSample\n"            );
    printf(" 8  LsInterruptSample\n"              );
    printf(" 9  LsFifoSample\n"                   );
    printf("10  LsBcRtBmTcbSample\n"              );
    printf("\n");
    printf("-- Samples with external coupling ---- \n");
    printf("20  LsMilscopeSample (local only)\n");
    printf("21  LsPtpVtpSample (RT UUT Setup)\n");
    printf("22  LsBmSample\n");
    printf("23  LsBcSample\n");
    printf("\n");
}

void PrintAvailableLocalModules()
{
    AiReturn ret;
    AiUInt32 module_count;
    AiUInt32 i;
    AiUInt32 sn;
    AiUInt32 module_handle;
    TY_API_OPEN          xApiOpen;
    TY_API_DRIVER_INFO   xDriverInfo;

    printf("List available local modules:\n");

    module_count = ApiInit();

    for (i=0; i<module_count; i++ )
    {
        xApiOpen.ul_Module = i;
        xApiOpen.ul_Stream = API_STREAM_1;
        sprintf(xApiOpen.ac_SrvName, "local");

        ret = ApiOpenEx(&xApiOpen, &module_handle);

        if (ret)
        {
            printf("Open stream %d on module %d failed with 0x%08X\n", xApiOpen.ul_Stream, xApiOpen.ul_Module, ret);
            continue;
        }

        ApiCmdSysGetBoardInfo(module_handle, TY_BOARD_INFO_SERIAL, 1, &sn, NULL);
        ApiGetDriverInfo(module_handle, &xDriverInfo);

        printf("%2d: %-16s SN %4d tasks connected to module %d\n", i, ApiGetBoardName(module_handle), sn, xDriverInfo.ul_OpenConnections - 1);

    }

    ApiExit();
}


void ProcessSampleUtilityCommands(char * name, char * command)
{
    /* only one argument, print usage or list modules */
    if (AiOsStringCmp(command, "list"))
    {
        /* List available local modules */
        PrintAvailableLocalModules();

        WaitForUser();
        exit(0);
    }
    else if (AiOsStringCmp(command, "help"))
    {
        /* Show available sample command line arguments */
        printf("%s\n", command);
        printf("%s help\n", name);
        printf("%s list\n", name);
        printf("%s server module_nr stream_nr sample_nr\n", name);

        PrintSampleList();

        WaitForUser();
        exit(0);
    }
}

void ReadConfiguration( int argc, char *argv[ ], ConfigStruct * pxConfig  )
{
    char szDummy[32];

    if( argc == 5 )
    {
        /* use command line arguments */
        sscanf( argv[1], "%s", pxConfig->acServer  );
        sscanf( argv[2], "%d", &pxConfig->ulDevice );
        sscanf( argv[3], "%d", &pxConfig->ulStream );
        sscanf( argv[4], "%d", &pxConfig->ulTest   );
    }
    else if( argc == 2 )
    {
        ProcessSampleUtilityCommands(argv[0], argv[1]);
    }
    else
    {
        /* no arguments, ask user */
        printf("action [sample, help, list] ? ");
        AiOsGetString(szDummy, sizeof(szDummy), "sample");
        ProcessSampleUtilityCommands(argv[0], szDummy);

        printf("server [local] ? ");
        AiOsGetString( pxConfig->acServer, sizeof(pxConfig->acServer), "local");

        printf("module number [%d..%d] ? ", API_MODULE_1, API_MODULE_32 );
        AiOsGetString( szDummy, sizeof(szDummy), "0" );
        sscanf( szDummy, "%d", &pxConfig->ulDevice );

        printf("stream number [%d..%d] ? ", API_STREAM_1, API_STREAM_8  );
        AiOsGetString( szDummy, sizeof(szDummy), "1" );
        sscanf( szDummy, "%d", &pxConfig->ulStream );


        PrintSampleList();

        printf("sample number ? ");
        AiOsGetString( szDummy, sizeof(szDummy), "1" );
        sscanf( szDummy, "%d", &pxConfig->ulTest );
    }


    if( AiOsStringCmp( pxConfig->acServer, "local" ) != 0 )
        pxConfig->ucUseServer = 0;
    else
        pxConfig->ucUseServer = 1;

    if ((pxConfig->ulTest >= 20) && (pxConfig->ulTest < 30))
    {
        pxConfig->ulCoupling = API_CAL_CPL_TRANSFORM;
    }
    else
    {
        pxConfig->ulCoupling = API_CAL_CPL_ISOLATED;
    }
}



AiUInt8 CheckIfDeviceIsEmbedded(AiUInt32 ulModHandle)
{
    AiUInt32 ulBoardInfo;
    AiUInt32 ulBoardInfoCount;

    ApiCmdSysGetBoardInfo(ulModHandle, TY_BOARD_INFO_APPLICATION_TYPE, 1, &ulBoardInfo, &ulBoardInfoCount);

    if (API_DEVICE_MODE_IS_EMBEDDED(ulBoardInfo&0xFF))
        /* First Stream is embedded, asume the same for the whole board */
        return 1;

    return 0;
}


AiUInt8 CheckIfDeviceIsSimulatorOrSingleFunction(AiUInt32 ulModHandle)
{
    AiUInt32 ulBoardInfo;
    AiUInt32 ulBoardInfoCount;

    ApiCmdSysGetBoardInfo(ulModHandle, TY_BOARD_INFO_APPLICATION_TYPE, 1, &ulBoardInfo, &ulBoardInfoCount);

    if (API_DEVICE_MODE_IS_SIMULATOR(ulBoardInfo & 0xFF))
        /* First Stream is simulator, asume the same for the whole board */
        return 1;

    if (API_DEVICE_MODE_IS_SINGLEFUNCTION(ulBoardInfo & 0xFF))
        /* First Stream is single function, asume the same for the whole board */
        return 1;

    if (API_DEVICE_MODE_IS_DISABLE_BC(ulBoardInfo & 0xFF))
        /* First Stream is disable BC, asume the same for the whole board */
        return 1;

    return 0;
}


void WaitForUser()
{
    printf("\n--- press any key ---\n");
    AiOsGetChar();
}


AiReturn PrintVersionInfo(AiUInt32 ulModHandle)
{
    int             i;
    AiReturn        ret   = 0;
    int             ids_count = 3;
    AiUInt32        sn;
    TY_VER_INFO     version;
    TY_E_VERSION_ID ids[] = { AI_SYS_DRV_VER, AI_DLL_VER, AI_TARGET_VER };


    ret = ApiCmdSysGetBoardInfo( ulModHandle, TY_BOARD_INFO_SERIAL, 1, &sn, NULL );

    AIM_ERROR_MESSAGE_RETURN( ret );

    printf("Device info : \n");
    printf("\n");
    printf("Device %s serial number %04d\n", ApiGetBoardName(ulModHandle), sn );
    printf("\n");

    /* Read basic versions */
    for( i=0; i<ids_count; i++ )
    {
        memset( &version, 0, sizeof(version) );
        ret = ApiReadVersion( ulModHandle, ids[i], &version );

        if( ret != API_ERR_INVALID_MODE )
        {
            AIM_ERROR_MESSAGE_RETURN( ret );

            printf("%-30s:  %-40s\n", version.ac_Description, version.ac_FullVersion);
        }
    }


    return ret;
}
