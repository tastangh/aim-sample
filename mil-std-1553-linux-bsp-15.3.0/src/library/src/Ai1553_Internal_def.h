/* SPDX-FileCopyrightText: 2001-2022 AIM GmbH <info@aim-online.com> */
/* SPDX-License-Identifier: MIT */

/*! \file Ai1553_Internal_def.h
 *
 *  Internal constants and structures for the 1553 Application Interface.
 *
 */

#ifndef AI1553_INTERNAL_DEF_H
#define AI1553_INTERNAL_DEF_H


//*************************************
// Module Defines
//*************************************

#define API_HS_ACCESS    0x80000000
#define API_STREAM(x)    ((x&API_STREAM_MASK)>>API_STREAM_POS)

// the module handle is defined as
// 31..12   11..8   7..5   4..0
// res      biu     srv    module

#define TG_EXT_SET_REP_ADDR     5

///************************************
// DLL specific DEFs
///************************************

// BITE Expected Responses
#define TG_BITE_OK  0

// Driver Response Words
#define TG_XMT_ACK   0x20
#define TG_XMT_ACK2  0x22
#define TG_XMT_NAK   0x24
#define TG_XMT_NAK2  0x26

// Driver Communication Status
#define TG_API_IO_ERR  API_ERR
#define TG_ACK         2
#define TG_ACK2        3
#define TG_NAK         4
#define TG_ACK_ERR     5
#define TG_ERR_CLOSED  6

// Driver Init Error Codes (base numbers)
#define TG_API_INI_OS_ERR       100

#define TG_API_INI_OS_ERR_EXT   200
#define TG_API_INI_DRV_ERR      300


/******************************************/
/***  API1553-DS Instruction Codes      ***/
/******************************************/
/* System Commands */
#define TG_API_SYS_GET_MEM_PARTITION   0x2B /* LEGACY */
#define TG_API_INI                     0x30
#define TG_API_RESET                   0x31 /* LEGACY */
#define TG_API_BITE                    0x32
#define TG_API_READ_SW_VERS            0x34 /* LEGACY */
#define TG_API_READ_SW_VERS_EX       0x4034 /* LEGACY */
#define TG_API_LOAD_SREC               0x36 /* LEGACY */
#define TG_API_PROG_FLASH              0x37 /* LEGACY */

/* Data Buffer Commands */
#define TG_API_BUF_DEF                 0x50
#define TG_API_BUF_READ                0x51
#define TG_API_RAM_WRITE               0x52
#define TG_API_RAM_READ                0x53
#define TG_API_RAM_LWORD_WR            0x54
#define TG_API_RAM_LWORD_RD            0x55
#define TG_API_RAM_WORD_WR             0x56
#define TG_API_RAM_WORD_RD             0x57
#define TG_API_RAM_BYTE_WR             0x58
#define TG_API_RAM_BYTE_RD             0x59
#define TG_API_RAM_DATASET_WR          0x5A
#define TG_API_RAM_DATASET_RD          0x5B
#define TG_API_BUF_WRITE               0x5D 
#define TG_API_BUF_C1760_CON           0x5E 


#ifdef _CMPL4HATS
    #define TG_API_EXT_TPMC_INI             0x100
    #define TG_API_EXT_TPMC_DIN_READ        0x101
    #define TG_API_EXT_TPMC_DOUT_WRITE      0x102
    #define TG_API_EXT_TPMC_LED_CON         0x103
    #define TG_API_EXT_TPMC_DAC_WRITE       0x104
    #define TG_API_EXT_TPMC_DAC_READ        0x105
    #define TG_API_EXT_TPMC_ADC_READ        0x106
    #define TG_API_EXT_TPMC_TEST            0x107
    #define TG_API_EXT_LVDT_INI             0x108
    #define TG_API_EXT_LVDT_OUT_WRITE       0x109
    #define TG_API_EXT_LVDT_OUT_READ        0x10A
    #define TG_API_EXT_LVDT_IN_READ         0x10B
#endif


/* Data Queue Commands */
#define TG_API_PARA_DEF                0xF6
#define TG_API_SCOPE_SETUP             0xF7
#define TG_API_SCOPE_TRG_DEF           0xF8
#define TG_API_SCOPE_CALIBRATE         0xF9

/* special commands only for AXI*/
#define TG_API_SET_AXI_TRG_MUX        0xF0  

#define TG_API_UTIL                   0xFF /* LEGACY */

#if defined(_AIM_WINDOWS)
#define TRACE_BEGIN   if( ((Aiul_DbgLevel & DBG_TRACE) == DBG_TRACE ) || ((Aiul_DbgLevel & DBG_TRACE_PBAPRO) == DBG_TRACE_PBAPRO ))\
      { \
        stream = fopen( szLogFileName, "a"); \
        GetLocalTime(&SystemTime);
#define TRACE(x) if( stream ) fprintf(stream, "[%02i:%02i:%02i.%03i] %s", SystemTime.wHour, SystemTime.wMinute, SystemTime.wSecond, SystemTime.wMilliseconds,x)
#define TRACE1(x, par) do { \
          if( stream ) \
          { \
            sprintf(dbgBuf, x, par); \
            fprintf(stream, "[%02i:%02i:%02i.%03i] %s", SystemTime.wHour, SystemTime.wMinute, SystemTime.wSecond, SystemTime.wMilliseconds,dbgBuf); \
          } \
        } while(0)
#define TRACE2(x, par) do { \
          if( stream ) \
          { \
            sprintf(dbgBuf, x, par); \
            fprintf(stream, "%s", dbgBuf); \
          } \
        } while(0)
#define TRACE_FCTA(fct, retval) if( stream ) fprintf(stream, "[%02i:%02i:%02i.%03i] %x = %s(", SystemTime.wHour, SystemTime.wMinute, SystemTime.wSecond, SystemTime.wMilliseconds,retval, fct)
#define TRACE_PARA(par) if( stream ) fprintf(stream, "0x%x, ", par)
#define TRACE_PARE(par) if( stream ) fprintf(stream, "0x%x", par)
#define TRACE_RPARA(par) if( stream ) fprintf(stream, "%s, ", par)
#define TRACE_RPARE(par) if( stream ) fprintf(stream, "%s", par)
#define TRACE_FPARA(par) if( stream ) fprintf(stream, "%f, ", par)
#define TRACE_FPARE(par) if( stream ) fprintf(stream, "%f", par)
#define TRACE_PPARA(par) if( stream ) fprintf(stream, "0x%p, ", par)
#define TRACE_PPARE(par) if( stream ) fprintf(stream, "0x%p ",  par)
#define TRACE_FCTE if( stream ) fprintf(stream, ");\n")
#define TRACE_END if( stream ) fclose(stream); \
                }
#endif

#if defined(_AIM_RTX64)
#define TRACE_BEGIN   if( ((Aiul_DbgLevel & DBG_TRACE) == DBG_TRACE ) || ((Aiul_DbgLevel & DBG_TRACE_PBAPRO) == DBG_TRACE_PBAPRO )) {
#define TRACE(x) RtPrintf("%s",x)
#define TRACE1(x, par) do { \
						sprintf(dbgBuf, x, par); \
						RtPrintf("%s",dbgBuf); \
						} while(0)
#define TRACE2(x, par) do { \
						sprintf(dbgBuf, x, par); \
						RtPrintf("%s",dbgBuf); \
						} while(0)
#define TRACE_FCTA(fct, retval) RtPrintf("%x = %s(", retval, fct)
#define TRACE_PARA(par) RtPrintf("0x%x, ", par)
#define TRACE_PARE(par) RtPrintf("0x%x", par)
#define TRACE_RPARA(par) RtPrintf("%s, ", par)
#define TRACE_RPARE(par) RtPrintf("%s", par)
#define TRACE_FPARA(par) RtPrintf("%f, ", par)
#define TRACE_FPARE(par) RtPrintf("%f", par)
#define TRACE_PPARA(par) RtPrintf("0x%p, ", par)
#define TRACE_PPARE(par) RtPrintf("0x%p ",  par)
#define TRACE_FCTE RtPrintf(");\n")
#define TRACE_END }
#endif

#if defined(_AIM_LVRT)
#define TRACE_BEGIN   if( ((Aiul_DbgLevel & DBG_TRACE) == DBG_TRACE ) || ((Aiul_DbgLevel & DBG_TRACE_PBAPRO) == DBG_TRACE_PBAPRO ))\
      { \
      stream = fopen( "c:\\aiMil_LVRT.log", "a"); \
        GetLocalTime(&SystemTime);
#define TRACE(x) if( stream ) fprintf(stream, "[%02i:%02i:%02i.%03i] %s", SystemTime.wHour, SystemTime.wMinute, SystemTime.wSecond, SystemTime.wMilliseconds,x)
#define TRACE1(x, par) do { \
          if( stream ) \
          { \
            sprintf(AidbgBuf, x, par); \
            fprintf(stream, "[%02i:%02i:%02i.%03i] %s", SystemTime.wHour, SystemTime.wMinute, SystemTime.wSecond, SystemTime.wMilliseconds,AidbgBuf); \
          } \
        } while(0)
#define TRACE2(x, par) do { \
          if( stream ) \
          { \
            sprintf(AidbgBuf, x, par); \
            fprintf(stream, "%s", AidbgBuf); \
          } \
        } while(0)
#define TRACE_FCTA(fct, retval) if( stream ) fprintf(stream, "[%02i:%02i:%02i.%03i] %x = %s(", SystemTime.wHour, SystemTime.wMinute, SystemTime.wSecond, SystemTime.wMilliseconds,retval, fct)
#define TRACE_PARA(par) if( stream ) fprintf(stream, "0x%x, ", par)
#define TRACE_PARE(par) if( stream ) fprintf(stream, "0x%x", par)
#define TRACE_RPARA(par) if( stream ) fprintf(stream, "%s, ", par)
#define TRACE_RPARE(par) if( stream ) fprintf(stream, "%s", par)
#define TRACE_FPARA(par) if( stream ) fprintf(stream, "%f, ", par)
#define TRACE_FPARE(par) if( stream ) fprintf(stream, "%f", par)
#define TRACE_PPARA(par) if( stream ) fprintf(stream, "0x%p, ", par)
#define TRACE_PPARE(par) if( stream ) fprintf(stream, "0x%p ",  par)
#define TRACE_FCTE if( stream ) fprintf(stream, ");\n")
#define TRACE_END if( stream ) fclose(stream); \
                }
#endif

#if defined _AIM_VME
#define TRACE_BEGIN   if( ((Aiul_DbgLevel & DBG_TRACE) == DBG_TRACE ) || ((Aiul_DbgLevel & DBG_TRACE_PBAPRO) == DBG_TRACE_PBAPRO )) {
#define TRACE(x) printf("%s",x)
#define TRACE1(x, par) do { \
                        sprintf(dbgBuf, x, par); \
                        printf("%s",dbgBuf); \
                      } while(0)
#define TRACE2(x, par) do { \
                        sprintf(dbgBuf, x, par); \
                        printf("%s",dbgBuf); \
                      } while(0)
#define TRACE_FCTA(fct, retval) printf("%x = %s(", retval, fct)
#define TRACE_PARA(par) printf("0x%x, ", par)
#define TRACE_PARE(par) printf("0x%x", par)
#define TRACE_RPARA(par) printf("%s, ", par)
#define TRACE_RPARE(par) printf("%s", par)
#define TRACE_FPARA(par) printf("%f, ", par)
#define TRACE_FPARE(par) printf("%f", par)
#define TRACE_PPARA(par) printf("0x%p, ", par)
#define TRACE_PPARE(par) printf("0x%p ",  par)
#define TRACE_FCTE printf(");\n")
#define TRACE_END }
#endif

#ifdef _AIM_LINUX
#define TRACE_BEGIN   if( ((Aiul_DbgLevel & DBG_TRACE) == DBG_TRACE ) || \
                          ((Aiul_DbgLevel & DBG_TRACE_PBAPRO) == DBG_TRACE_PBAPRO )) \
                      { \
                          struct timeval SystemTime; \
                          struct tm* p_CurrentDateTime; \
                          stream = fopen( "aim_mil.log", "a"); \
                          gettimeofday(&SystemTime,NULL); \
                          p_CurrentDateTime = localtime((const time_t*) &SystemTime.tv_sec);

#define TRACE(x) if( stream ) fprintf(stream, "[%02i:%02i:%02i.%03li] %s", p_CurrentDateTime->tm_hour, p_CurrentDateTime->tm_min, p_CurrentDateTime->tm_sec, SystemTime.tv_usec / 1000, x)

#define TRACE1(x, par) do { \
          if( stream ) \
          { \
            sprintf(dbgBuf, x, par); \
            fprintf(stream, "[%02i:%02i:%02i.%03li] %s", p_CurrentDateTime->tm_hour, p_CurrentDateTime->tm_min, p_CurrentDateTime->tm_sec, SystemTime.tv_usec / 1000, dbgBuf); \
          } \
        } while(0)

#define TRACE2(x, par) do { \
          if( stream ) \
          { \
            sprintf(dbgBuf, x, par); \
            fprintf(stream, "%s", dbgBuf); \
          } \
        } while(0)

#define TRACE_FCTA(fct, retval) if( stream ) fprintf(stream, "[%02i:%02i:%02i.%03li] %x = %s(",p_CurrentDateTime->tm_hour, p_CurrentDateTime->tm_min, p_CurrentDateTime->tm_sec, SystemTime.tv_usec / 1000,retval, fct)

#define TRACE_PARA(par)  if( stream ) fprintf(stream, "0x%x, ", par)
#define TRACE_PARE(par)  if( stream ) fprintf(stream, "0x%x",   par)
#define TRACE_RPARA(par) if( stream ) fprintf(stream, "%s, ",   par)
#define TRACE_RPARE(par) if( stream ) fprintf(stream, "%s",     par)
#define TRACE_FPARA(par) if( stream ) fprintf(stream, "%f, ",   par)
#define TRACE_FPARE(par) if( stream ) fprintf(stream, "%f",     par)
#define TRACE_PPARA(par) if( stream ) fprintf(stream, "0x%p, ", par)
#define TRACE_PPARE(par) if( stream ) fprintf(stream, "0x%p ",  par)
#define TRACE_FCTE if( stream ) fprintf(stream, ");\n")

#define TRACE_END do { \
            if( stream ) \
            { \
                fclose(stream); \
                stream = NULL; \
            } \
        } while(0); \
    }

#endif

#if !defined(NO_PROTOTYPE_INCLUDE)

//****************************************************
// functions for VME-PC only
//****************************************************
AI_LIB_FUNC AiReturn AI_CALL_CONV _ApiWinNTMapVmeBoard(AiUInt32 bModule, AiUInt8 uc_BoardNr, AiUInt8 uc_MapType, AiUInt8 uc_DataAccess, AiUInt32 ul_Offset, AiUInt32 ul_Size);
AI_LIB_FUNC AiReturn AI_CALL_CONV _ApiWinNTReadVmeData(AiUInt32 bModule, AiUInt8 uc_BoardNr, AiUInt32 ul_Offset, AiUInt8 uc_Width,
                                                          void* pv_Data, AiUInt32 ul_Size );
AI_LIB_FUNC AiReturn AI_CALL_CONV _ApiWinNTWriteVmeData(AiUInt32 bModule, AiUInt8 uc_BoardNr, AiUInt32 ul_Offset, AiUInt8 uc_Width,
                                                          void* pv_Data, AiUInt32 ul_Size );

#endif //!defined(NO_PROTOTYPE_INCLUDE) 




#if defined(_AIM_VME) || defined(_AIM_LINUX) || defined(_AIM_RTX64)
typedef void * PRPC_ASYNC_STATE;
#define RPC_ASYNC_STATE void*
#endif

AI_LIB_FUNC AiReturn AI_CALL_CONV _ApiIoCmdScopeStop(AiUInt32 ul_ModuleHandle);
AI_LIB_FUNC AiReturn AI_CALL_CONV _ApiOsCmdScopeStatus(AiUInt32 ulModuleHandle, AiUInt32* pulNumBuffersLeft);

// Parameter RPC_ASYNC_STATE *RPCScopeData_AsyncHandle used only on ANS Server with ulNumBuffers=1.
// On client set this parameter to NULL
AI_LIB_FUNC AiReturn AI_CALL_CONV _ApiOsProvideScopeBuffers(AiUInt32 ulModuleHandle, AiUInt32 ulNumBuffers, TY_API_SCOPE_BUFFER* axScopeBuffers[], RPC_ASYNC_STATE *RPCScopeData_AsyncHandle);
AI_LIB_FUNC AiReturn AI_CALL_CONV _ApiIoCmdScopeStatus(AiUInt32 ulModuleHandle, TY_API_SCOPE_STATUS* p_ScopeStatus, AiUInt32* pulNumBuffersLeft);
AI_LIB_FUNC AiReturn AI_CALL_CONV _ApiIoCmdScopeSetup(AiUInt32 ul_ModuleHandle, TY_API_SCOPE_SETUP *px_ScopeSetup);
AI_LIB_FUNC AiReturn AI_CALL_CONV _ApiIoCmdScopeStart(AiUInt32 ul_ModuleHandle);

AI_LIB_FUNC AiReturn AI_CALL_CONV _ApiOsProvideScopeBuffersSync(AiUInt32 ulModuleHandle, TY_API_SCOPE_BUFFER_FLAGS * pFlags);
AI_LIB_FUNC AiReturn AI_CALL_CONV _ApiOsProvideScopeBuffersWait(AiUInt32 ulModuleHandle);



typedef struct ty_ScopeBufferDataOut
{
    AiUInt32            *pulDataSize;
    AiUInt32            *pulTriggerId;
    AiUInt32            *pulFlags;
    PRPC_ASYNC_STATE    RPCScopeData_AsyncHandle;
    AiUInt8             *puc_ScopeBuffer;      // pointer to buffer provided by RPC function
} TY_SCOPEBUFFERDATAOUT; 


#endif



