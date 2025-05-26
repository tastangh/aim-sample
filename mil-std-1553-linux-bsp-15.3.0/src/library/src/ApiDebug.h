/* SPDX-FileCopyrightText: 2019-2022 AIM GmbH <info@aim-online.com> */
/* SPDX-License-Identifier: MIT */

/*! \file ApiBwts.h
 *
 * Debugging definitions
 * 
 */


#ifndef API_DEBUG_H
#define API_DEBUG_H

extern AiUInt32         Aiul_DbgLevel;
extern AiUInt32         Aiul_GlobalTestValue;

#define AI1553_DLL_NAME      "api_mil.dll"
#define FKT_NOOUT            ""

#define FKT_INTPOLLTHREAD     "_ApiPollSysDriver"
#define FKT_NTINIT            "        _ApiOsIni"
#define FKT_NTEXTINIT         "     _ApiOsExtIni"
#define FKT_NTOPEN            "       _ApiOsOpen"
#define FKT_NTCLOSE           "      _ApiOsClose"
#define FKT_NTIO              "         _ApiOsIo"
#define FKT_DLLINIT           "         _DLLInit"
#define FKT_NTSYNCWAIT        "   _ApiOsSyncWait"
#define FKT_NTSYNCREL         "_ApiOsSyncRelease"
#define FKT_32INSTINT         "_ApiOsInstIntHndl"
#define FKT_READREC           " _ApiOsReadRecDat"
#define FKT_WRITEREP          " _ApiOsWritRepDat"
#define FKT_APIIO             "            ApiIo"
#define FKT_APIOPEN           "          ApiOpen"
#define FKT_APICMDINI         "        ApiCmdIni"
#define FKT_DQUEUE_OPEN       " _ApiOsDQueueOpen"
#define FKT_DQUEUE_CLOSE      "_ApiOsDQueueClose"
#define FKT_DQUEUE_READ       " _ApiOsDQueueRead"
#define FKT_DQUEUE_WRITE      "_ApiOsDQueueWrite"
#define FKT_DQUEUE_CTRL       "_ApiOsDQueueCntrl"
#define FKT_DQUEUE_CAL        " _ApiOsDQueueCalc"
#define FKT_DQUEUE_RD_BUF     "_ApiOsDQueueReadB"
#define FKT_DQUEPOLLTHREAD    " ApiPollDataQueue"

#if defined(_AIM_WINDOWS)
/* Line break do not work under LINUX please do not touch this macros*/
#define DEBUGOUT(LEVEL,FKT,STRING)do{ ULONG _level = (LEVEL); if( (Aiul_DbgLevel &DBG_MINIMAL) == 0)_level &= ~DBG_MINIMAL; if( (Aiul_DbgLevel &_level) == _level) { AiOsSnprintf( (char*)dbgBuf, sizeof(dbgBuf), AiTrue, "%s (%s): %s", AI1553_DLL_NAME, FKT, STRING); OutputDebugString( (char*)dbgBuf); } } while(0)
#define DEBUGOUTN(LEVEL,STRING) do {ULONG _level = (LEVEL); if( (Aiul_DbgLevel &DBG_MINIMAL) == 0)_level &= ~DBG_MINIMAL; if((Aiul_DbgLevel &_level) == _level) { AiOsSnprintf( (char*)dbgBuf, sizeof(dbgBuf), AiTrue, "%s", STRING); OutputDebugString( (char*)dbgBuf); } } while(0)

#elif defined(_AIM_RTX64)
/* CR does not work under RTX64 */
#define DEBUGOUT(LEVEL,FKT,STRING)do{ ULONG _level = (LEVEL); if( (Aiul_DbgLevel &DBG_MINIMAL) == 0)_level &= ~DBG_MINIMAL; if( (Aiul_DbgLevel &_level) == _level) { RtPrintf( "%s (%s): %s", AI1553_DLL_NAME, FKT, STRING); } } while(0)
#define DEBUGOUTN(LEVEL,STRING) do {ULONG _level = (LEVEL); if( (Aiul_DbgLevel &DBG_MINIMAL) == 0)_level &= ~DBG_MINIMAL; if((Aiul_DbgLevel &_level) == _level) { RtPrintf( "%s", STRING); } } while(0)

#elif defined(_AIM_LINUX)
#define DEBUGOUT(LEVEL,FKT,STRING)do{ ULONG _level = (LEVEL); if( (Aiul_DbgLevel &DBG_MINIMAL) == 0)_level &= ~DBG_MINIMAL; if( (Aiul_DbgLevel &_level) == _level) {  AiOsSnprintf( (char*)dbgBuf, sizeof(dbgBuf), AiTrue, "%s (%s): %s", AI1553_DLL_NAME, FKT, STRING); printf("%s", (char*)dbgBuf); } } while(0)
#define DEBUGOUTN(LEVEL,STRING) do {ULONG _level = (LEVEL); if( (Aiul_DbgLevel &DBG_MINIMAL) == 0)_level &= ~DBG_MINIMAL; if((Aiul_DbgLevel &_level) == _level) {  AiOsSnprintf( (char*)dbgBuf, sizeof(dbgBuf), AiTrue, "%s", STRING); printf( (char*)dbgBuf); } } while(0)

#elif defined(_AIM_VME)
#define DEBUGOUT(LEVEL,FKT,STRING) do {unsigned long _level = (LEVEL); if( (Aiul_DbgLevel &DBG_MINIMAL) == 0)_level &= ~DBG_MINIMAL; if( (Aiul_DbgLevel &_level) == _level) {  AiOsSnprintf( (char*)dbgBuf, sizeof(dbgBuf), AiTrue, "%s (%s): %s", AI1553_DLL_NAME, FKT, STRING); printf(dbgBuf); } } while(0)
#define DEBUGOUTN(LEVEL,STRING) do { unsigned long _level = (LEVEL); if( (Aiul_DbgLevel &DBG_MINIMAL) == 0)_level &= ~DBG_MINIMAL; if( (Aiul_DbgLevel &_level) == _level) {  AiOsSnprintf( (char*)dbgBuf, sizeof(dbgBuf), AiTrue, "%s", STRING); printf(dbgBuf); } } while(0)

#elif defined(_AIM_LVRT) && !defined(_AIM_VXI)
/*#define DEBUGOUT(LEVEL,FKT,STRING) do {unsigned long _level = (LEVEL); if( (Aiul_DbgLevel &DBG_MINIMAL) == 0)_level &= ~DBG_MINIMAL; if( (Aiul_DbgLevel &_level) == _level) { sprintf(AidbgBuf, "%s (%s): %s", AI1553_DLL_NAME, FKT, STRING); printf(AidbgBuf); } } while(0) */
/*#define DEBUGOUTN(LEVEL,STRING) do { unsigned long _level = (LEVEL); if( (Aiul_DbgLevel &DBG_MINIMAL) == 0)_level &= ~DBG_MINIMAL; if( (Aiul_DbgLevel &_level) == _level) { sprintf(AidbgBuf, "%s", STRING); printf(AidbgBuf); } } while(0) */

#define DEBUGOUT(LEVEL,FKT,STRING) \
TRACE_BEGIN \
  TRACE1("     %s;\n", FKT); \
  TRACE1("    %s;\n", STRING); \
TRACE_END

  
/*  TRACE_FCTA("FKT", uw_RetVal); 
  TRACE_PARA(bModule);
  TRACE_PARA(mode);
  TRACE_RPARE("&ini");
  TRACE_FCTE;
  TRACE1("    --> ini.asp_bite_id      = 0x%x;\n", pini->asp_bite_id);
  TRACE1("    --> ini.asp_mon_id       = 0x%x;\n", pini->asp_mon_id);
  TRACE1("    --> ini.board_config     = 0x%x;\n", pini->board_config);
  TRACE1("    --> ini.bt[0]            = 0x%x;\n", pini->bt[0]);
  TRACE1("    --> ini.bt[1]            = 0x%x;\n", pini->bt[1]);
  TRACE1("    --> ini.bt[2]            = 0x%x;\n", pini->bt[2]);
  TRACE1("    --> ini.bt[3]            = 0x%x;\n", pini->bt[3]);
  TRACE1("    --> ini.chns             = 0x%x;\n", pini->chns);
  TRACE1("    --> ini.emod             = 0x%x;\n", pini->emod);
  TRACE1("    --> ini.flash_ram_addr   = 0x%x;\n", pini->flash_ram_addr);
  TRACE1("    --> ini.flash_ram_size   = 0x%x;\n", pini->flash_ram_size);
  TRACE1("    --> ini.glb_mem_addr     = 0x%x;\n", pini->glb_mem_addr);
  TRACE1("    --> ini.glb_mem_size     = 0x%x;\n", pini->glb_mem_size);
  TRACE1("    --> ini.irg              = 0x%x;\n", pini->irg);
  TRACE1("    --> ini.loc_dram_addr    = 0x%x;\n", pini->loc_dram_addr);
  TRACE1("    --> ini.loc_dram_size    = 0x%x;\n", pini->loc_dram_size);
  TRACE1("    --> ini.padd1            = 0x%x;\n", pini->padd1);
  TRACE1("    --> ini.padd2            = 0x%x;\n", pini->padd2);
  TRACE1("    --> ini.pbi_id_biu1      = 0x%x;\n", pini->pbi_id_biu1);
  TRACE1("    --> ini.pbi_id_biu2      = 0x%x;\n", pini->pbi_id_biu2);
  for(i=0; i<16; i++)
  {
    TRACE1("    --> ini.pci[%02d]          = ", i); TRACE2("0x%x\n", pini->pci[i]);
  }
  TRACE1("    --> ini.prot             = 0x%x;\n", pini->prot);
  TRACE1("    --> ini.res1             = 0x%x;\n", pini->res1);
  TRACE1("    --> ini.shared_dram_addr = 0x%x;\n", pini->shared_dram_addr);
  TRACE1("    --> ini.shared_dram_size = 0x%x;\n", pini->shared_dram_size);
TRACE_END
*/


#elif defined(_AIM_VXI)
  #define DEBUGOUT(LEVEL,FKT,STRING)
  #define DEBUGOUTN(LEVEL,STRING)
#endif 


#endif //API_DEBUG_H

