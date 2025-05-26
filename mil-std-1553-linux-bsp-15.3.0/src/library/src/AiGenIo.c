/* SPDX-FileCopyrightText: 2002-2022 AIM GmbH <info@aim-online.com> */
/* SPDX-License-Identifier: MIT */

/*! \file AiGenIo.c
 *
 *  API Command module of the Application Interface
 *
 */


//****************************************************
// platform specific specific includes
//****************************************************
#include "AiOs.h"

//****************************************************
// my own includes
//****************************************************
#include "Ai_cdef.h"
#if defined(_AIM_VME)  || defined(_AIM_LINUX) || defined(_AIM_VXI)
  #ifdef _API3910
    #include "Ai3910i_def.h"
    #include "Ai3910i_fnc.h"
    #include "Ai1553_Internal_def.h"
    #include "Ai3910_Internal_def.h"
  #else
    #include "Ai1553i_def.h"
    #include "Ai1553i_fnc.h"
    #include "Ai1553_Internal_def.h"
  #endif
#else
  #include "Ai3910i_def.h"
  #include "Ai3910i_fnc.h"
  #include "Ai1553_Internal_def.h"
  #include "Ai3910_Internal_def.h"
#endif

#include "AiGenIo.h"
#include "DEFS_V.H"
#include "Dll_Fnc.h"
#include "stdlib.h" /* for malloc and free */
#include "AiMapDllToTsw.h"

/* This function is used to define OS specific members of the device info
* structure.
* Parameters:  TY_DEVICE_INFO* - pointer to the device info struct to initialize
*/
void _ApiInitializeDeviceOsSpecific(TY_DEVICE_INFO* p_DeviceInfo);

//***************************************************************************
//
//   Module : API                        Submodule : ApiCmdGenIoGetBoards
//
//   Author : Thomas Jahn              Project   : API1553-DS
//
//---------------------------------------------------------------------------
//   Create : 22.07.00   Update : 22.07.00
//---------------------------------------------------------------------------
//    Descriptions
//    ------------
//    Inputs    :
//
//    Outputs   : Instruction execution status [status]
//
//    Description :
//
//***************************************************************************

AI_LIB_FUNC AiReturn AI_CALL_CONV ApiCmdGenIoGetBoards(AiUInt32 ul_Module,TY_API_GEN_IO_GET_BOARDS_IN *px_InBoards,
                                                       TY_API_GEN_IO_GET_BOARDS_OUT *px_OutBoards )
{
    AiInt16 status = API_ERR;
    AiUInt8 info[8];
    AiUInt8 uc_NumOfBoards;
    AiUInt8 uc_Item1,uc_Item2,uc_Item3,uc_Item4,uc_Item5,uc_Item6;
    AiUInt32 ul_MaxIpModulesPerCarrier = 0;
    AiInt16    uw_InByteSize;
    AiInt16    uw_InLWordSize;
    AiUInt32  pci_reg[4][16],ul_addr;
    AiUInt32 ul_data,i,BoardTypeCnt,uc_NumOfBoardsCnt,DevVenCnt;
    AiInt16 w_Read;
    TY_API_GEN_IO_DEV_VEN *px_DevVen;
    TY_API_GEN_IO_BOARD *px_BoardTemp;
    TY_API_GEN_IO_BOARD_TYPE *px_BoardTypes;
    static AiChar buf[2000];

    px_OutBoards->ul_Cnt=0;

    px_DevVen = px_InBoards->px_DevVenList->px_DevVen;
    px_BoardTemp = px_OutBoards->px_Board;

    for(DevVenCnt=0;DevVenCnt<px_InBoards->px_DevVenList->ul_Cnt;DevVenCnt++){

        sprintf( buf, "DevVenCnt = %X[%X], px_DevVen->ul_DeviceVendor [0x%X] px_DevVen->ul_SubSystemSubVendor [0x%X] \n",
            DevVenCnt,px_InBoards->px_DevVenList->ul_Cnt,
            px_DevVen->ul_DeviceVendor,px_DevVen->ul_SubSystemSubVendor);
            DEBUGOUT( DBG_INIT, FKT_NTIO, buf );
        px_BoardTypes = px_InBoards->px_BoardTypesList->px_BoardTypes;

        for(BoardTypeCnt=0;BoardTypeCnt<px_InBoards->px_BoardTypesList->ul_Cnt;BoardTypeCnt++){

            sprintf( buf, "BoardTypeCnt = %X, (px_BoardTypes->ul_BoardType [0x%X] px_BoardTypes->ul_Type [0x%X] \n",
                BoardTypeCnt, px_BoardTypes->ul_BoardType,px_BoardTypes->ul_Type);
                DEBUGOUT( DBG_INIT, FKT_NTIO, buf );


            switch(px_BoardTypes->ul_BoardType){
              case API_GEN_IO_APX_TYPE:
                  if( DevVenCnt== 0){
                      /* APX Board */
                      px_BoardTemp->ul_Board = API_GEN_IO_APX_TYPE;
                      px_BoardTemp->ul_Type = 1;
                      px_BoardTemp->ul_StartAddress = 0x0;
                      px_OutBoards->ul_Cnt++;
                      px_BoardTemp++;
                  }
                  break;

              case API_GEN_IO_DIG_TYPE:
                  info[0] = (AiUInt8)(px_DevVen->ul_DeviceVendor >> 24);
                  info[1] = (AiUInt8)(px_DevVen->ul_DeviceVendor >> 16);
                  info[2] = (AiUInt8)(px_DevVen->ul_DeviceVendor >> 8);
                  info[3] = (AiUInt8)(px_DevVen->ul_DeviceVendor);
                  info[4] = (AiUInt8)(px_DevVen->ul_SubSystemSubVendor >> 24);
                  info[5] = (AiUInt8)(px_DevVen->ul_SubSystemSubVendor >> 16);
                  info[6] = (AiUInt8)(px_DevVen->ul_SubSystemSubVendor >> 8);
                  info[7] = (AiUInt8)(px_DevVen->ul_SubSystemSubVendor);

                  status = ApiIo(ul_Module & ~API_STREAM_MASK, 0, TG_API_GEN_IO_GET_BOARDS, TG_ACK2,
                      &info[0], 8, w_p, 0,
                      &uc_NumOfBoards, &uw_InByteSize, w_p, &tg_os, &(pci_reg[0][0]), &uw_InLWordSize);

                  if(    (px_DevVen->ul_DeviceVendor       == TPCI100_2SLOT_DEV_VEN_ID_REG)
                      && (px_DevVen->ul_SubSystemSubVendor == TPCI100_2SLOT_SUB_VEN_ID_REG) )
                      ul_MaxIpModulesPerCarrier = 2;
                  else
                      ul_MaxIpModulesPerCarrier = 4;

                  for(uc_NumOfBoardsCnt=0;uc_NumOfBoardsCnt<uc_NumOfBoards;uc_NumOfBoardsCnt++){
                      for (i=0; i<ul_MaxIpModulesPerCarrier; i++)
                      {  /*i Slot*/
                          ul_addr = pci_reg[0][7] + 0x80 + 0 + (i*0x100); /*bar3*/
                          ApiCmdGenIoRead(ul_Module, ul_addr, 1, &ul_data, &w_Read);
                          uc_Item1 = (AiUInt8)(ul_data >>8);
                          uc_Item2 = (AiUInt8)(ul_data >>24);

                          ul_addr = pci_reg[0][7] + 0x80 + 4 + (i*0x100); /*bar3*/
                          ApiCmdGenIoRead(ul_Module, ul_addr, 1, &ul_data, &w_Read);
                          uc_Item3 = (AiUInt8)(ul_data >>8);
                          uc_Item4 = (AiUInt8)(ul_data >>24);

                          ul_addr = pci_reg[0][7] + 0x80 + 8 + (i*0x100); /*bar3*/
                          ApiCmdGenIoRead(ul_Module, ul_addr, 1, &ul_data, &w_Read);
                          uc_Item5 = (AiUInt8)(ul_data >>8);
                          uc_Item6 = (AiUInt8)(ul_data >>24);
                          if ((uc_Item1 == 'I' ) &&
                              (uc_Item2 == 'P' ) &&
                              (uc_Item3 == 'A' ) &&
                              (uc_Item4 == 'C' ) &&
                              (uc_Item5 == 0xA3) &&
                              (uc_Item6 == px_BoardTypes->ul_Type)){
                                  px_BoardTemp->ul_Board = API_GEN_IO_DIG_TYPE;
                                  px_BoardTemp->ul_Type = px_BoardTypes->ul_Type;
                                  px_BoardTemp->ul_StartAddress = pci_reg[0][7] + (i*0x100);
                                  px_OutBoards->ul_Cnt++;
                                  px_BoardTemp++;
                          }
                      }
                  }
                  break;

              case API_GEN_IO_ANA_TYPE:
                  info[0] = (AiUInt8)(px_DevVen->ul_DeviceVendor >> 24);
                  info[1] = (AiUInt8)(px_DevVen->ul_DeviceVendor >> 16);
                  info[2] = (AiUInt8)(px_DevVen->ul_DeviceVendor >> 8);
                  info[3] = (AiUInt8)(px_DevVen->ul_DeviceVendor);
                  info[4] = (AiUInt8)(px_DevVen->ul_SubSystemSubVendor >> 24);
                  info[5] = (AiUInt8)(px_DevVen->ul_SubSystemSubVendor >> 16);
                  info[6] = (AiUInt8)(px_DevVen->ul_SubSystemSubVendor >> 8);
                  info[7] = (AiUInt8)(px_DevVen->ul_SubSystemSubVendor);

                  status = ApiIo(ul_Module & ~API_STREAM_MASK, 0, TG_API_GEN_IO_GET_BOARDS, TG_ACK2,
                      &info[0], 8, w_p, 0,
                      &uc_NumOfBoards, &uw_InByteSize, w_p, &tg_os, &(pci_reg[0][0]), &uw_InLWordSize);

                  if(    (px_DevVen->ul_DeviceVendor       == TPCI100_2SLOT_DEV_VEN_ID_REG)
                      && (px_DevVen->ul_SubSystemSubVendor == TPCI100_2SLOT_SUB_VEN_ID_REG) )
                      ul_MaxIpModulesPerCarrier = 2;
                  else
                      ul_MaxIpModulesPerCarrier = 4;

                  for(uc_NumOfBoardsCnt=0;uc_NumOfBoardsCnt<uc_NumOfBoards;uc_NumOfBoardsCnt++){
                      for (i=0; i<ul_MaxIpModulesPerCarrier; i++)
                      {  /*i Slot*/
                          ul_addr = pci_reg[0][7] + 0x80 + 0 + (i*0x100); /*bar3*/
                          ApiCmdGenIoRead(ul_Module, ul_addr, 1, &ul_data, &w_Read);
                          uc_Item1 = (AiUInt8)(ul_data >>8);
                          uc_Item2 = (AiUInt8)(ul_data >>24);

                          ul_addr = pci_reg[0][7] + 0x80 + 4 + (i*0x100); /*bar3*/
                          ApiCmdGenIoRead(ul_Module, ul_addr, 1, &ul_data, &w_Read);
                          uc_Item3 = (AiUInt8)(ul_data >>8);
                          uc_Item4 = (AiUInt8)(ul_data >>24);

                          ul_addr = pci_reg[0][7] + 0x80 + 8 + (i*0x100); /*bar3*/
                          ApiCmdGenIoRead(ul_Module, ul_addr, 1, &ul_data, &w_Read);
                          uc_Item5 = (AiUInt8)(ul_data >>8);
                          uc_Item6 = (AiUInt8)(ul_data >>24);
                          if ((uc_Item1 == 'I' ) &&
                              (uc_Item2 == 'P' ) &&
                              (uc_Item3 == 'A' ) &&
                              (uc_Item4 == 'C' ) &&
                              (uc_Item5 == 0xB3) &&
                              (uc_Item6 == px_BoardTypes->ul_Type)){
                                  px_BoardTemp->ul_Board = API_GEN_IO_ANA_TYPE;
                                  px_BoardTemp->ul_Type = px_BoardTypes->ul_Type;
                                  px_BoardTemp->ul_StartAddress = pci_reg[0][7] + (i*0x100);
                                  px_OutBoards->ul_Cnt++;
                                  px_BoardTemp++;
                          }
                      }
                  }
                  break;
            }
            px_BoardTypes++;
        }
        px_DevVen++;
    }
    TRACE_BEGIN
        px_DevVen = px_InBoards->px_DevVenList->px_DevVen;
    TRACE_FCTA("ApiCmdGenIoGetBoards", status); 
    TRACE_PARA(ul_Module);
    TRACE_PARA(px_InBoards->px_DevVenList->ul_Cnt);
    for(i=0;i<px_InBoards->px_DevVenList->ul_Cnt;i++){
        TRACE_PARA(px_DevVen->ul_DeviceVendor);
        TRACE_PARA(px_DevVen->ul_SubSystemSubVendor);
        px_DevVen++;
    }
    TRACE_FCTE;
    TRACE1("    --> px_OutBoards->ul_Cnt     = 0x%x\n", px_OutBoards->ul_Cnt);
    TRACE_END

        return(status);

} /* end: ApiCmdGenIoGetBoards */

//***************************************************************************
//
//   Module : API                      Submodule : ApiCmdGenIoAddrInit
//
//   Author : Thomas Jahn              Project   : API1553-DS
//
//---------------------------------------------------------------------------
//   Create : 22.07.00   Update : 22.07.00
//---------------------------------------------------------------------------
//    Descriptions
//    ------------
//    Inputs    :
//
//    Outputs   : Instruction execution status [status]
//
//    Description :
//
//***************************************************************************
AI_LIB_FUNC AiReturn AI_CALL_CONV ApiCmdGenIoAddrInit(AiUInt32 ul_Module, AiUInt8 genio_type, AiUInt8 io_type, AiUInt8 chn, AiUInt8 res, AiUInt32 addr)
{
    AiInt16 status;
    AiUInt8 info[8];

    info[0] = genio_type;
    info[1] = io_type;
    info[2] = chn;
    info[3] = res;
    info[4] = (AiUInt8)(addr >> 24);
    info[5] = (AiUInt8)(addr >> 16);
    info[6] = (AiUInt8)(addr >> 8);
    info[7] = (AiUInt8)(addr);

    status = ApiIo(ul_Module & ~API_STREAM_MASK, 0, TG_API_GEN_IO_ADDR_INIT, TG_ACK,
        &info[0], 8, w_p, 0,
        b_p, &tg_os, w_p, &tg_os, lw_p, &tg_os);

    TRACE_BEGIN
        TRACE_FCTA("ApiCmdGenIoAddrInit", status); 
    TRACE_PARA(ul_Module);
    TRACE_PARA(genio_type);
    TRACE_PARA(io_type);
    TRACE_PARA(chn);
    TRACE_PARA(addr);
    TRACE_FCTE;
    TRACE_END


        return(status);

} /* end: ApiCmdGenIoAddrInit */

//***************************************************************************
//
//   Module : API                      Submodule : ApiCmdGenIoOutChnWrite
//
//   Author : Thomas Jahn              Project   : API1553-DS
//
//---------------------------------------------------------------------------
//   Create : 22.07.00   Update : 22.07.00
//---------------------------------------------------------------------------
//    Descriptions
//    ------------
//    Inputs    :
//
//    Outputs   : Instruction execution status [status]
//
//    Description :
//
//***************************************************************************

AI_LIB_FUNC AiReturn AI_CALL_CONV ApiCmdGenIoOutChnWrite(AiUInt32 ul_Module, AiUInt8 genio_type, AiUInt8 chn, AiUInt32 res, AiUInt32 val)
{
    AiInt16 status;
    AiUInt8 info[10];

    info[0] = genio_type;
    info[1] = chn;
    info[2] = (AiUInt8)(res >> 24);
    info[3] = (AiUInt8)(res >> 16);
    info[4] = (AiUInt8)(res >> 8);
    info[5] = (AiUInt8)(res);
    info[6] = (AiUInt8)(val >> 24);
    info[7] = (AiUInt8)(val >> 16);
    info[8] = (AiUInt8)(val >> 8);
    info[9] = (AiUInt8)(val);

    status = ApiIo(ul_Module & ~API_STREAM_MASK, 0, TG_API_GEN_IO_OUT_CHN_WRITE, TG_ACK,
        &info[0], 10, w_p, 0,
        b_p, &tg_os, w_p, &tg_os, lw_p, &tg_os);

    TRACE_BEGIN
        TRACE_FCTA("ApiCmdGenIoOutChnWrite", status); 
    TRACE_PARA(ul_Module);
    TRACE_PARA(genio_type);
    TRACE_PARA(chn);
    TRACE_PARA(val);
    TRACE_FCTE;
    TRACE_END
        return(status);

} // end: ApiCmdGenIoOutChnWrite
//***************************************************************************
//
//   Module : API                      Submodule : ApiCmdGenOutInChnRead
//
//   Author : Thomas Jahn              Project   : API1553-DS
//
//---------------------------------------------------------------------------
//   Create : 22.11.10   Update : 22.11.10
//---------------------------------------------------------------------------
//    Descriptions
//    ------------
//    Inputs    :
//
//    Outputs   : Instruction execution status [status]
//
//    Description :
//
//***************************************************************************

AI_LIB_FUNC AiReturn AI_CALL_CONV ApiCmdGenIoOutChnRead(AiUInt32 ul_Module, AiUInt8 genio_type, AiUInt8 chn, AiUInt32 ctl, AiUInt32 *val)
{
    AiInt16 status;
    AiUInt8 info[6];
    AiUInt32 ret_val;
    AiInt16 in_lword_size;

    in_lword_size = 0;

    info[0] = genio_type;
    info[1] = chn;
    info[2] = (AiUInt8)(ctl >> 24);
    info[3] = (AiUInt8)(ctl >> 16);
    info[4] = (AiUInt8)(ctl >> 8);
    info[5] = (AiUInt8)(ctl);

    status = ApiIo(ul_Module & ~API_STREAM_MASK, 0, TG_API_GEN_IO_OUT_CHN_READ, TG_ACK2,
        &info[0], 6, w_p, 0,
        b_p, &tg_os, w_p, &tg_os, &ret_val, &in_lword_size);

    *val = ret_val;
    TRACE_BEGIN
        TRACE_FCTA("ApiCmdGenIoOutChnRead", status); 
    TRACE_PARA(ul_Module);
    TRACE_PARA(genio_type);
    TRACE_PARA(chn);
    TRACE_PARA(ctl);
    TRACE_PARA(ret_val);
    TRACE_FCTE;
    TRACE_END

        return(status);

} // end: ApiCmdGenIoInChnRead

//***************************************************************************
//
//   Module : API                      Submodule : ApiCmdGenIoInChnRead
//
//   Author : Thomas Jahn              Project   : API1553-DS
//
//---------------------------------------------------------------------------
//   Create : 22.07.00   Update : 22.07.00
//---------------------------------------------------------------------------
//    Descriptions
//    ------------
//    Inputs    :
//
//    Outputs   : Instruction execution status [status]
//
//    Description :
//
//***************************************************************************

AI_LIB_FUNC AiReturn AI_CALL_CONV ApiCmdGenIoInChnRead(AiUInt32 ul_Module, AiUInt8 genio_type, AiUInt8 chn, AiUInt32 ctl, AiUInt32 *val)
{
    AiInt16 status;
    AiUInt8 info[6];
    AiUInt32 ret_val;
    AiInt16 in_lword_size;

    in_lword_size = 0;

    info[0] = genio_type;
    info[1] = chn;
    info[2] = (AiUInt8)(ctl >> 24);
    info[3] = (AiUInt8)(ctl >> 16);
    info[4] = (AiUInt8)(ctl >> 8);
    info[5] = (AiUInt8)(ctl);

    status = ApiIo(ul_Module & ~API_STREAM_MASK, 0, TG_API_GEN_IO_IN_CHN_READ, TG_ACK2,
        &info[0], 6, w_p, 0,
        b_p, &tg_os, w_p, &tg_os, &ret_val, &in_lword_size);

    *val = ret_val;
    TRACE_BEGIN
        TRACE_FCTA("ApiCmdGenIoInChnRead", status); 
    TRACE_PARA(ul_Module);
    TRACE_PARA(genio_type);
    TRACE_PARA(chn);
    TRACE_PARA(ctl);
    TRACE_PARA(ret_val);
    TRACE_FCTE;
    TRACE_END

        return(status);

} // end: ApiCmdGenIoInChnRead

//***************************************************************************
//
//   Module : API                      Submodule : ApiCmdGenIoSysSTaskCtrl
//
//   Author : Thomas Jahn              Project   : API1553-DS
//
//---------------------------------------------------------------------------
//   Create : 22.07.00   Update : 22.07.00
//---------------------------------------------------------------------------
//    Descriptions
//    ------------
//    Inputs    :
//
//    Outputs   : Instruction execution status [status]
//
//    Description :
//
//***************************************************************************

AI_LIB_FUNC AiReturn AI_CALL_CONV ApiCmdGenIoSysSTaskCtrl(AiUInt32 ul_Module, AiUInt8 tmr, AiUInt8 con, AiUInt32 rate, AiUInt32 rsync, AiUInt32 ustep)
{
    AiInt16 status;
    AiUInt8 info[14];

    info[0]  = tmr;
    info[1]  = con;
    info[2]  = (AiUInt8)(rate >> 24);
    info[3]  = (AiUInt8)(rate >> 16);
    info[4]  = (AiUInt8)(rate >> 8);
    info[5]  = (AiUInt8)(rate);
    info[6]  = (AiUInt8)(rsync >> 24);
    info[7]  = (AiUInt8)(rsync >> 16);
    info[8]  = (AiUInt8)(rsync >> 8);
    info[9]  = (AiUInt8)(rsync);
    info[10] = (AiUInt8)(ustep >> 24);
    info[11] = (AiUInt8)(ustep >> 16);
    info[12] = (AiUInt8)(ustep >> 8);
    info[13] = (AiUInt8)(ustep);

    status = ApiIo(ul_Module & ~API_STREAM_MASK, 0, TG_API_GEN_IO_STASK_CTRL, TG_ACK,
        &info[0], 14, w_p, 0,
        b_p, &tg_os, w_p, &tg_os, lw_p, &tg_os);

    TRACE_BEGIN
        TRACE_FCTA("ApiCmdGenIoSysSTaskCtrl", status); 
    TRACE_PARA(ul_Module);
    TRACE_PARA(tmr);
    TRACE_PARA(con);
    TRACE_PARA(rate);
    TRACE_PARA(rsync);
    TRACE_PARA(ustep);
    TRACE_FCTE;
    TRACE_END
        return(status);

} // end: ApiCmdGenIoSysSTaskCtrl


//***************************************************************************
//
//   Module : API                      Submodule : ApiCmdGenIoRead
//
//   Author : Thomas Jahn              Project   : API1553-DS
//
//---------------------------------------------------------------------------
//   Create : 22.07.00   Update : 22.07.00
//---------------------------------------------------------------------------
//    Descriptions
//    ------------
//    Inputs    :
//
//    Outputs   : Instruction execution status [status]
//
//    Description :
//
//***************************************************************************

AI_LIB_FUNC AiReturn AI_CALL_CONV ApiCmdGenIoRead(AiUInt32 ul_Module, AiUInt64 ull_Addr, AiUInt32 ul_Size, void * vp_Data, AiInt16 *pw_Read)
{
    AiInt16 status;
    AiUInt8 info[8];

    info[0] = (AiUInt8)(ull_Addr >> 24);
    info[1] = (AiUInt8)(ull_Addr >> 16);
    info[2] = (AiUInt8)(ull_Addr >> 8);
    info[3] = (AiUInt8)(ull_Addr);
    info[4] = (AiUInt8)(ul_Size >> 24);;
    info[5] = (AiUInt8)(ul_Size >> 16);;
    info[6] = (AiUInt8)(ul_Size >> 8);;
    info[7] = (AiUInt8)(ul_Size);;

    status = ApiIo(ul_Module & ~API_STREAM_MASK, 0, TG_API_GEN_IO_READ, TG_ACK2,
        &info[0], 8, w_p, 0,
        b_p, &tg_os, w_p, &tg_os, (AiUInt32*) vp_Data, pw_Read);
    return(status);
} // end: ApiCmdGenIoRead


//***************************************************************************
//    Descriptions
//    ------------
//    Inputs    :
//
//    Outputs   :
//
//    Description :
//
//***************************************************************************



AI_LIB_FUNC AiReturn AI_CALL_CONV ApiCmdGenIoTestSeq(AiUInt32 ulModHandle, AiUInt32 ul_Con, TY_API_GENIO_TEST_SEQ_TBL *pxSeqTbl, AiUInt8 *auc_SeqStatus)
{
    AiInt16                              wRetVal  = API_OK;
    TY_MIL_COM_CUSTOM_DTS_GEN_IO_TEST_SEQ_INPUT     xInput;
    TY_MIL_COM_CUSTOM_DTS_GEN_IO_TEST_SEQ_OUTPUT    xOutput;

    // -- initialize command ---

    MIL_COM_INIT( &xInput.xCommand, API_STREAM(ulModHandle), 0, MIL_COM_CUSTOM_DTS_GEN_IO_TEST_SEQ, sizeof(TY_MIL_COM_CUSTOM_DTS_GEN_IO_TEST_SEQ_INPUT), sizeof(TY_MIL_COM_CUSTOM_DTS_GEN_IO_TEST_SEQ_OUTPUT) );

    if( pxSeqTbl != NULL)
        memcpy( &xInput.xSeqTbl, pxSeqTbl, sizeof(TY_API_GENIO_TEST_SEQ_TBL) );
    else
        memset( &xInput.xSeqTbl, 0, sizeof(TY_API_GENIO_TEST_SEQ_TBL) );

    xInput.ul_Con = ul_Con;

    wRetVal = API_CMD_STRUCT_CUSTOM_DTS_GEN_IO_TEST_SEQ( ulModHandle, &xInput, &xOutput );

    if( auc_SeqStatus != NULL )
        memcpy( auc_SeqStatus, xOutput.aucSeqStatus, sizeof(xOutput.aucSeqStatus) );

    return wRetVal;
}

