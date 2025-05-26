/* SPDX-FileCopyrightText: 2001-2022 AIM GmbH <info@aim-online.com> */
/* SPDX-License-Identifier: MIT OR GPL-2.0-or-later */

/*! \file api_versions.c
    Read versions related functions
*/


#include "mil_tsw_includes.h"


NAMESPACE1553_START


TY_MIL_COM_TABLE_ENTRY axTargetSysVersionsCommands[] = {
  {MIL_COM_READ_VERSION,    (TY_MIL_COM_FUNC_PTR)api_sys_read_version,    "api_sys_read_version",    sizeof(TY_MIL_COM_VERSION_INPUT),         sizeof(TY_MIL_COM_VERSION_OUTPUT) },
};



void api_sys_versions_command_table( TY_MIL_COM_TABLE_ENTRY ** table, AiUInt32 * size )
{
    *size  = sizeof(axTargetSysVersionsCommands) / sizeof(TY_MIL_COM_TABLE_ENTRY);
    *table = axTargetSysVersionsCommands;
}



/*****************************************************************************/
/*                                                                           */
/*   Module : API_SYS                  Submodule : API_SYS_READ_SW_VERS      */
/*                                                                           */
/*   Author : Ralf Kuderer             Project   : API1553-DS                */
/*                                                                           */
/*   Source : C                        Tools     : PC/AT; Norton Editor;     */
/*                                                 CYGNUS, GNU-C, As, and LD */
/*                                                 IDT-C 5.1 Toolkit         */
/*---------------------------------------------------------------------------*/
/*   Create : 01.05.98   Update : 25.05.00                                   */
/*---------------------------------------------------------------------------*/
/*    Descriptions                                                           */
/*    ------------                                                           */
/*    Inputs    : none                                                       */
/*                                                                           */
/*    Outputs   : Instruction acknowledge type [ackfl]                       */
/*                                                                           */
/*    Description :                                                          */
/*    This function handles the 'API_READ_SW_VERS' instruction to return the */
/*    API1553-DS ASP Driver SW version number and Firmware version number.   */
/*                                                                           */
/*****************************************************************************/

BYTE api_sys_read_sw_vers_legacy(TY_API_DEV *p_api_dev)
{

  BYTE ackfl;

  ackfl = API_ACK2;

  p_api_dev->sys_mem[ p_api_dev->biu ].fw_vers = (WORD) (BSWAP32( (*p_api_dev->cb_p[ p_api_dev->biu ]).cid) );
  p_api_dev->sys_mem[ p_api_dev->biu ].sw_vers = MIL_TSW_LEGACY_VERSION;
  
  if( ul_StreamIsHS( p_api_dev, p_api_dev->biu) || 
     (p_api_dev->ul_DeviceGroup == AI_DEVICE_AYX) )
  {
    p_api_dev->sys_mem[ p_api_dev->biu ].lca_id = (BSWAP32( (*p_api_dev->cb_p[ p_api_dev->biu ]).lca_id) & 0x0FFF0FFF);
  }
  else
  {
    p_api_dev->sys_mem[ p_api_dev->biu ].lca_id = (BSWAP32( (*p_api_dev->cb_p[ p_api_dev->biu ]).lca_id) & 0x00000FFF);
  }

  p_api_dev->sys_mem[ p_api_dev->biu ].lca_chks    = BSWAP32( (*p_api_dev->cb_p[ p_api_dev->biu ]).biu_core_freq);
  p_api_dev->sys_mem[ p_api_dev->biu ].ul_build    = MIL_TSW_LEGACY_BUILD;
  p_api_dev->sys_mem[ p_api_dev->biu ].ul_serialNr = p_api_dev->BoardSerial;
  p_api_dev->sys_mem[ p_api_dev->biu ].pci_lca_id  = 0;

  p_api_dev->b_length = 0;
  p_api_dev->w_length = 1;
  p_api_dev->lw_length = 5;
  p_api_dev->ack_w_ptr = &(p_api_dev->sys_mem[ p_api_dev->biu ].fw_vers);
  p_api_dev->ack_lw_ptr = &(p_api_dev->sys_mem[ p_api_dev->biu ].sw_vers);


  return (ackfl);

} /* end: api_sys_read_sw_vers */


BYTE api_sys_read_sw_vers_ex_legacy(TY_API_DEV *p_api_dev)
{


  mil_hw_get_bsp_version_ex( p_api_dev,
          &p_api_dev->lword_data_arr[  0 ], /* ulLsIoLcaPbi1  */
          &p_api_dev->lword_data_arr[  1 ], /* ulPciLca       */
          &p_api_dev->lword_data_arr[  2 ], /* ulFwBiu1Biu2   */
          &p_api_dev->lword_data_arr[  3 ], /* ulTSW          */
          &p_api_dev->lword_data_arr[  4 ], /* ulTSWBuild     */
          &p_api_dev->lword_data_arr[  5 ], /* ulSN           */
          &p_api_dev->lword_data_arr[  6 ], /* ulHsIoLcaPbi1  */
          &p_api_dev->lword_data_arr[  7 ], /* ulAslLca       */
          &p_api_dev->lword_data_arr[  8 ], /* ulLsIoLcaPbi2  */
          &p_api_dev->lword_data_arr[  9 ], /* ulHsIoLcaPbi2  */
          &p_api_dev->lword_data_arr[ 10 ], /* ulFwBiu3Biu4   */
          &p_api_dev->lword_data_arr[ 11 ]  /* ulMonitor      */ );


  p_api_dev->b_length = 0;
  p_api_dev->w_length = 0;
  p_api_dev->lw_length = 12;
  p_api_dev->ack_lw_ptr = &(p_api_dev->lword_data_arr[ 0 ]);

  return API_ACK2;
} /* end: api_sys_read_sw_vers_ex */




const short aul_MapVersionToPhysicalBiu[] =
{
    -1, /* AI_SYS_DRV_VER          = 0,*/
    -1, /* AI_ANS_VER              = 1,*/
    -1, /* AI_DLL_VER              = 2,*/
    -1, /* AI_REMOTE_DLL_VER       = 3,*/
    -1, /* AI_TARGET_VER           = 4,*/
    0,  /* AI_FIRMWARE_VER_BIU1    = 5,*/
    -1, /* AI_MAIN_LCA_VER         = 6,*/
    0,  /* AI_IO_LCA_VER_BIU1      = 7,*/
    -1, /* AI_PCI_LCA_VER          = 8,*/
    -1, /* AI_TCP_VER              = 9,*/
    -1, /* AI_BOOTSTRAP_VER        = 10,*/
    -1, /* AI_MONITOR_VER          = 11,*/
    -1, /* AI_TARGET_OS_VER        = 12,*/
    -1, /* AI_FPGA_VERSION         = 13,*/
    1,  /* AI_FIRMWARE_VER_BIU2    = 14,*/
    2,  /* AI_FIRMWARE_VER_BIU3    = 15,*/
    3,  /* AI_FIRMWARE_VER_BIU4    = 16,*/
    1,  /* AI_IO_LCA_VER_BIU2      = 17,*/
    2,  /* AI_IO_LCA_VER_BIU3      = 18,*/
    3   /* AI_IO_LCA_VER_BIU4      = 19,*/
};


L_WORD api_sys_read_version_io(  TY_API_DEV *p_api_dev, TY_E_VERSION_ID eId, TY_API_VER_INFO_VALUES * pxVersionValue, TY_API_VER_INFO_STRINGS * pxVersionString )
{
    L_WORD retval       = 0;
    L_WORD version      = 0;
    int    logical_biu  = 0;
    int    physical_biu = 0;

    L_WORD ulEncVersionMajor = 0;
    L_WORD ulEncVersionMinor = 0;

    char ucDesc[AI_VERSION_STRINGL];


    physical_biu = aul_MapVersionToPhysicalBiu[eId];

    if( physical_biu < 0 )
        return API_ERR_INVALID_MODE;

    if( physical_biu >= p_api_dev->physical_bius )
        return API_ERR_INVALID_MODE;

    logical_biu = api_sys_convert_biu_physical_to_logical( p_api_dev, (BYTE)physical_biu );

    /* Firmware */
    if( logical_biu < p_api_dev->chns )
    {
        AI_TSW_OS_SPRINTF(ucDesc, AI_VERSION_STRINGL, "IO LCA%d", physical_biu+1);
        version = BSWAP32(p_api_dev->cb_p[logical_biu]->lca_id);
        retval = 0;
    }
    else
    {
        retval = API_ERR_INVALID_MODE;
    }


    if( retval == 0 )
    {

        pxVersionValue->ul_VersionType  = eId;
        pxVersionValue->ul_PatchVersion = 0;
        pxVersionValue->ul_BuildNr      = 0;

        AI_TSW_OS_SPRINTF(pxVersionString->ac_Description,AI_DESCRIPTION_STRINGL,ucDesc);


        pxVersionValue->ul_MajorVer     = BCD2BIN_B( EXTRACT_BITS(version,4,0xFF  ) ); /* Main/Core FPGA Version */
        pxVersionValue->ul_MinorVer     = BCD2BIN_B( EXTRACT_BITS(version,0,0xF   ) ); /* Main/Core FPGA Version */

        ulEncVersionMajor = BCD2BIN_B( EXTRACT_BITS(version,20,0xFF ) ); /* Encoder/Decoder FPGA Version */
        ulEncVersionMinor = BCD2BIN_B( EXTRACT_BITS(version,16,0xF  ) ); /* Encoder/Decoder FPGA Version */


        if( ulEncVersionMajor || ulEncVersionMinor )
        {
            AI_TSW_OS_SPRINTF(pxVersionString->ac_FullVersion, AI_VERSION_STRINGL,"%d.%d-%d.%d",
                                                      pxVersionValue->ul_MajorVer,
                                                      pxVersionValue->ul_MinorVer,
                                                      ulEncVersionMajor,
                                                      ulEncVersionMinor );
        }
        else
        {
            AI_TSW_OS_SPRINTF(pxVersionString->ac_FullVersion, AI_VERSION_STRINGL,"%d.%d",
                                                      pxVersionValue->ul_MajorVer,
                                                      pxVersionValue->ul_MinorVer );
        }
    }


    return retval;
}







L_WORD api_sys_read_version_fw(  TY_API_DEV *p_api_dev, TY_E_VERSION_ID eId, TY_API_VER_INFO_VALUES * pxVersionValue, TY_API_VER_INFO_STRINGS * pxVersionString )
{
    L_WORD retval       = 0;
    L_WORD build        = 0;
    L_WORD git_hash     = 0;
    L_WORD cid          = 0;
    int    logical_biu  = 0;
    int    physical_biu = 0;

    char ucDesc[AI_VERSION_STRINGL];


    physical_biu = aul_MapVersionToPhysicalBiu[eId];

    if( physical_biu < 0 )
        return API_ERR_INVALID_MODE;

    if( physical_biu >= p_api_dev->physical_bius )
        return API_ERR_INVALID_MODE;

    logical_biu = api_sys_convert_biu_physical_to_logical( p_api_dev, (BYTE)physical_biu );

    /* Firmware */
    if( logical_biu < p_api_dev->chns )
    {
        AI_TSW_OS_SPRINTF(ucDesc, AI_DESCRIPTION_STRINGL,"FW BIU%d", physical_biu+1);
        cid = BSWAP32(p_api_dev->cb_p[logical_biu]->cid);
        retval = 0;
    }
    else
    {
        retval = API_ERR_INVALID_MODE;
    }


    if( retval == 0 )
    {
        pxVersionValue->ul_VersionType  = eId;
        pxVersionValue->ul_MajorVer     = BCD2BIN_B( EXTRACT_BITS(cid,8,0xFF  ) );
        pxVersionValue->ul_MinorVer     = BCD2BIN_B( EXTRACT_BITS(cid,0,0xFF  ) );
        pxVersionValue->ul_PatchVersion = 0;
        pxVersionValue->ul_BuildNr      = 0;

        AI_TSW_OS_SPRINTF(pxVersionString->ac_Description, AI_DESCRIPTION_STRINGL,"%s", ucDesc);

#ifdef _API3910
        if ((cid & 0x00FF0000) == 0x00040000)  /* board configuration 0x04 means 3910 (according to 3910 FW Spec V01.16 and EFEX FW Spec V01.27) */
        {
            git_hash = px_api39_cb_p[0]->ul_git_hash;
            build    = px_api39_cb_p[0]->ul_build;
        }
        else
#endif
        {
            git_hash = p_api_dev->cb_p[logical_biu]->git_hash;
            build = BSWAP32(p_api_dev->cb_p[logical_biu]->fw_build);
        }


        pxVersionValue->ul_BuildNr = BCD2BIN_W(EXTRACT_BITS(build, 0, 0xFFFF));

        if (git_hash)
        {
            AI_TSW_OS_SPRINTF(pxVersionString->ac_FullVersion, AI_VERSION_STRINGL, "%d.%d.%d-g%x", pxVersionValue->ul_MajorVer, pxVersionValue->ul_MinorVer, pxVersionValue->ul_BuildNr, git_hash);
        }
        else
        {
            AI_TSW_OS_SPRINTF(pxVersionString->ac_FullVersion, AI_VERSION_STRINGL, "%d.%d.%d", pxVersionValue->ul_MajorVer, pxVersionValue->ul_MinorVer, pxVersionValue->ul_BuildNr);
        }


    }


    return retval;
}








L_WORD api_sys_read_version_tsw(  TY_API_DEV *p_api_dev, TY_E_VERSION_ID eId, TY_API_VER_INFO_VALUES * pxVersionValue, TY_API_VER_INFO_STRINGS * pxVersionString )
{
    pxVersionValue->ul_VersionType  = AI_TARGET_VER;
    pxVersionValue->ul_MajorVer     = MIL_TSW_MAJOR_VERSION;
    pxVersionValue->ul_MinorVer     = MIL_TSW_MINOR_VERSION;
    pxVersionValue->ul_PatchVersion = MIL_TSW_PATCH_VERSION;
    pxVersionValue->ul_BuildNr      = 0;

    AI_TSW_OS_SPRINTF(pxVersionString->ac_Description, AI_DESCRIPTION_STRINGL, "TSW");
    AI_TSW_OS_SPRINTF(pxVersionString->ac_FullVersion, AI_VERSION_STRINGL, "%s", MIL_TSW_VERSION_STRING_FULL);

    return 0;
}




/*! \brief This function reads all available onboard versions */
/*!

    \param [in]  p_api_dev  The module handle.
    \param [in]  cmd_p      The version read command.
    \param [out] ack_p      The versions to be returned.

    \return                 Zero on normal termination
*/
L_WORD api_sys_read_version(  TY_API_DEV *p_api_dev, TY_MIL_COM_VERSION_INPUT * cmd_p, TY_MIL_COM_VERSION_OUTPUT * ack_p )
{
    AiUInt32 retVal = 0;

    if( cmd_p->eId == AI_TARGET_VER )
    {
    	ack_p->xVersionValues.ul_VersionType  = AI_TARGET_VER;
    	ack_p->xVersionValues.ul_MajorVer     = MIL_TSW_MAJOR_VERSION;
    	ack_p->xVersionValues.ul_MinorVer     = MIL_TSW_MINOR_VERSION;
    	ack_p->xVersionValues.ul_PatchVersion = MIL_TSW_PATCH_VERSION;
    	ack_p->xVersionValues.ul_BuildNr      = 0;

        AI_TSW_OS_SPRINTF(ack_p->xVersionStrings.ac_Description, AI_DESCRIPTION_STRINGL,"TSW");
        AI_TSW_OS_SPRINTF(ack_p->xVersionStrings.ac_FullVersion, AI_VERSION_STRINGL, "%s", MIL_TSW_VERSION_STRING_FULL);
    }
    else
    {
    	retVal = mil_hw_read_version( p_api_dev, cmd_p->eId, &ack_p->xVersionValues, &ack_p->xVersionStrings );
    }

    ack_p->xAck.ulSwapControl  = sizeof(TY_API_VER_INFO_STRINGS);

    return retVal;
}


NAMESPACE1553_END


/*** end of file ***/

