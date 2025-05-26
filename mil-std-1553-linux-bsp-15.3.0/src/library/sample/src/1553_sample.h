/* SPDX-FileCopyrightText: 2010-2024 AIM GmbH <info@aim-online.com> */
/* SPDX-License-Identifier: MIT */

#ifndef __1553_SAMPLE_H__
#define __1553_SAMPLE_H__

#include "AiOs.h"
#include "Api1553.h"

#include "mil_util.h"


#if defined(MVME5100)
  #include "map/MVME5100.h"
#elif defined(MVME6100)
  #include "map/MVME6100.h"
#elif defined(CUSTOM)
  #include "map/CUSTOM.h"
#elif defined(_AIM_XENOMAI)
  #include "os/xenomai/initialize.h"
#else
  #define sample_map_board()
  #define sample_unmap_board()
#endif


/* Datatypes used in sample.c */
typedef struct ConfigStruct
{
  AiUInt32 ulDevice;
  AiUInt32 ulStream;
  AiUInt32 ulTest;
  AiUInt32 ulCoupling;
  AiUInt8  ucUseServer;
  AiChar   acServer[28];
} ConfigStruct;



/* help functions in sample.c */
AiReturn PrintVersionInfo(AiUInt32 ulModHandle);
AiUInt8 CheckIfDeviceIsEmbedded(AiUInt32 ulModHandle);
AiUInt8 CheckIfDeviceIsSimulatorOrSingleFunction(AiUInt32 ulModHandle);
void ReadConfiguration( int argc, char *argv[ ], ConfigStruct * pxConfig  );
void WaitForUser();

/* functions implemented in function files */
AiReturn TemplateSample(AiUInt32 ulModHandle);
AiReturn PrintBoardVersions(AiUInt32 ulModHandle, TY_API_RESET_INFO * pxReset);
AiReturn LsBcRtBmSample(AiUInt32 ulModHandle);
AiReturn LsRecordingReplaySample(AiUInt32 ulModHandle, AiUInt32 ulStream, AiUInt8 ucUseAns);
AiReturn LsInstructionListSample(AiUInt32 ulModHandle);
AiReturn LsDynamicDataSample(AiUInt32 ulModHandle);
AiReturn LsAcyclicSample(AiUInt32 ulModHandle);
AiReturn LsBufferQueueSample(AiUInt32 ulModHandle);
AiReturn LsInterruptSample(AiUInt32 ulModHandle);
AiReturn LsMilscopeSampleAPE(AiUInt32 ulModHandle);
AiReturn LsFifoSample(AiUInt32 ulModHandle);
AiReturn LsSABasedWCntSample(AiUInt32 ulModHandle);
AiReturn LsBcRtBmTcbSample(AiUInt32 ulModHandle);

AiReturn LsPtpVtpSample(AiUInt32 ulModHandle);
AiReturn LsBmSample(AiUInt32 ulModHandle, AiUInt32 ulStream, AiUInt8 ucUseAns);
AiReturn LsBcSample(AiUInt32 ulModHandle);

AiReturn LsPtpVtpSample_Automatic(AiUInt32 ulModHandle);
#endif /* __1553_SAMPLE_H__ */







