/* SPDX-FileCopyrightText: 2021-2022 AIM GmbH <info@aim-online.com> */
/* SPDX-License-Identifier: MIT */


#include "AiOs.h"
#include "Api1553.h"


struct dtp_command
{
    unsigned int command;
    unsigned int payload;
};

#define DTP_COMMAND_SET_NXW       1
#define DTP_COMMAND_CLEAR_NXW     2
#define DTP_COMMAND_SET_MC16_DATA 3
#define DTP_COMMAND_SET_MC19_DATA 4
#define DTP_COMMAND_INIT          5
#define DTP_COMMAND_SET_RT_ADDR   6
#define DTP_COMMAND_DISABLE_RT    7
#define DTP_COMMAND_EXIT          8
#define DTP_COMMAND_MAX           DTP_COMMAND_EXIT
#define DTP_COMMAND_ACK           0xFFFFFFFF


AiReturn LsPtpVtpSample_SetupRT(AiUInt32 ulModHandle, AiUInt32 ulRtAddr);
AiReturn LsPtpVtpSample_DisableRT(AiUInt32 ulModHandle, AiUInt32 ulRtAddr);
AiReturn LsPtpVtpSample_StartOperation(AiUInt32 ulModHandle);
AiReturn LsPtpVtpSample_StopOperation(AiUInt32 ulModHandle);
AiReturn LsPtpVtpSample_DataDump(AiUInt32 ulModHandle);
AiReturn LsPtpVtpSample_Status(AiUInt32 ulModHandle);
AiInt16 LsPtpVtpSample_PrepareAndStartRTOperation(AiUInt32 ulModHandle, AiUInt8 ucNewRtAddr);
void LsPtpVtpSample_Help();
void LsPtpVtpSample_DTP(AiUInt32 ulModHandle);
void LsPtpVtpSample_DTPServer(AiUInt32 ulModHandle);
void LsPtpVtpSample_DTPServerCleanup(AiUInt32 ulModHandle);
AiReturn LsPtpVtpSample_DtpCommand(AiUInt32 ulModHandle, struct dtp_command cmd);


#if defined _AIM_WINDOWS 
   /* Windows operating systems */
   /* "os/windows/ls_ptp_vtp_sample_dtp_server.c" */
#elif defined _AIM_LINUX
   /* Linux operating systems */
   /* "os/linux/ls_ptp_vtp_sample_dtp_server.c" */
#else
   void LsPtpVtpSample_DTPServer(AiUInt32 ulModHandle)
   {
        
       printf("Starting DiagnosticTestProcedure server not implemented for this OS\n");

   }
#endif
