/* SPDX-FileCopyrightText: 2018-2024 AIM GmbH <info@aim-online.com> */
/* SPDX-License-Identifier: MIT OR GPL-2.0-or-later */

/*! \file mil_hw_pci_driver_aye.c
    This file contains hw layer implementations for AyE without ASP.

    mil_hw_pci_driver.c               => Generic functions
    mil_hw_pci_driver_aye.c           => AyE related
    mil_hw_pci_driver_aye_pxi.c       => AyE PXI and Trigger related
    mil_hw_pci_driver_aye_milscope.c  => AyE Milscope related
    mil_hw_pci_driver_ays.c           => AyS related
    mil_hw_pci_driver_artix7.c        => Artix7 related
    mil_hw_pci_driver_artixus.c       => Artix UltraScale+ related
 */


#if defined(_AIM_SYSDRV) || defined (_AIM_VME) || defined (_AIM_LVRT)
/* PCI system driver and VME driver library only */

/* TSW */
#include "mil_tsw_includes.h"
#include "hw/AiHwAye.h"

#include "mil_hw_pci_driver.h"

/*****************************************************************************
 *                                                                           *
 *                                                                           *
 *                 HW Layer public TSW NOVRAM access                         *
 *                                                                           *
 *                                                                           *
 *****************************************************************************/

/*! \brief Write the given value to the NOVRAM offset. */
/*!

\param  p_api_dev          TY_API_DEV*      The global variable container.
\param  ulOffset           AiUInt32         The NOVRAM offset
\param  ulValue            AiUInt32         The NOVRAM value

\return L_WORD The return value is 0 on success or non zero on failure.
*/
AiUInt32 mil_hw_pci_driver_aye_novram_write(TY_API_DEV *p_api_dev, AiUInt32 ulOffset, AiUInt32 ulValue)
{
    AiUInt16 wRetVal  = API_OK;

    if (!p_api_dev->pxMyMonContext)
        return API_ERR_MYMON_ERROR;

    wRetVal = MyMonCmdNovramMirrorWrite(p_api_dev->pxMyMonContext, ulOffset/4, ulValue, NULL);

    if (wRetVal != API_OK)
    {
        return API_ERR_MYMON_ERROR;
    }

    return wRetVal;
}


/*! \brief Read from the given NOVRAM offset. */
/*!

\param  p_api_dev          TY_API_DEV*      The global variable container.
\param  ulOffset           AiUInt32         The NOVRAM offset
\param  pulValue           AiUInt32*        The NOVRAM value

\return L_WORD The return value is 0 on success or non zero on failure.
*/
AiUInt32 mil_hw_pci_driver_aye_novram_read(TY_API_DEV *p_api_dev, AiUInt32 ulOffset, AiUInt32 *pulValue)
{
    AiBoolean        bRetVal  = AiFalse;
    TyMyMonContext * pContext = p_api_dev->pxMyMonContext;

    if (!pulValue)
        return API_ERR_MYMON_ERROR;

    if (!pContext)
        return API_ERR_MYMON_ERROR;

    if (!pContext->pfReadGlobalExt)
        return API_ERR_MYMON_ERROR;

    bRetVal = pContext->pfReadGlobalExt(pContext, MYMON_GREG_NOVRAM_BASE_OFFSET + ulOffset, pulValue, 1);

    if (!bRetVal)
        return API_ERR_MYMON_ERROR;
    else
        return API_OK;
}


/*! \brief Flush the NOVRAM mirror. */
/*!

\param  p_api_dev          TY_API_DEV*      The global variable container.
\return L_WORD The return value is 0 on success or non zero on failure.
*/
AiUInt32 mil_hw_pci_driver_aye_novram_finalize(TY_API_DEV *p_api_dev)
{
    AiUInt16 wRetVal  = API_OK;

    if (!p_api_dev->pxMyMonContext)
        return API_ERR_MYMON_ERROR;

    wRetVal = MyMonCmdNovramMirrorFlush(p_api_dev->pxMyMonContext, NULL);

    if (wRetVal != API_OK)
    {
        return API_ERR_MYMON_ERROR;
    }

    return wRetVal;
}








/*****************************************************************************
 *                                                                           *
 *                                                                           *
 *                 HW Layer public TSW IRIG access                           *
 *                                                                           *
 *                                                                           *
 *****************************************************************************/
  



/*! \brief This function sets the hw irig time source the TCP. */
/*!
    This function handles the 'API_EXEC_SYS' instruction 1

    \param  p_api_dev  TY_API_DEV*      The global variable container.
    \param  con        BYTE             System Command control
    
    \return ackfl      BYTE             Instruction acknowledge type.
*/
L_WORD mil_hw_pci_driver_aye_irig_set_external(TY_API_DEV *p_api_dev, BYTE con)
{
    /* IRIG Time Source: 0 internal, 1 external */
    MyMonCmdIrigSwitch( p_api_dev->pxMyMonContext, con, NULL, NULL );

    return 0;
}


/*! \brief This function sets the hw irig time through the TCP. */
/*!
    This function handles the 'API_SET_IRIG_TIME' instruction to set the
    IRIG encoder of the TCP (Time Code Processor)

    \param  p_api_dev  TY_API_DEV*      The global variable container.
    \param  day_hi     BYTE             IRIG time encode day high
    \param  day_low    BYTE             IRIG time encode day low
    \param  hour       BYTE             IRIG time encode hour
    \param  min        BYTE             IRIG time encode minute
    \param  sec        BYTE             IRIG time encode second
    
    \return ackfl      BYTE             Instruction acknowledge type.
*/
L_WORD mil_hw_pci_driver_aye_irig_set( TY_API_DEV * p_api_dev, 
                        L_WORD         day, 
                        L_WORD         hour, 
                        L_WORD         min, 
                        L_WORD         sec )
{
    L_WORD   ret_val  = 0;

    ret_val = MyMonCmdIrigSet(p_api_dev->pxMyMonContext, (WORD)day, (WORD)hour, (BYTE)min, (BYTE)sec, 0);

    if (ret_val != API_OK)
    {
        ret_val = API_ERR_MYMON_ERROR;
    }

    return ret_val;
}



/*! \brief This function reads the hw irig time and source through the TCP. */
/*!
    This function handles the 'API_GET_IRIG_TIME' instruction to read the
    IRIG time decode information

    \param  p_api_dev  TY_API_DEV*      The global variable container.
    \return ackfl      BYTE             Instruction acknowledge type.
*/
L_WORD mil_hw_pci_driver_aye_irig_get_status(TY_API_DEV *p_api_dev, AiUInt32 * irig_sync, AiUInt32 * irig_src)
{
    L_WORD irig_status = 0;
    L_WORD pxi_source_ctrl = 0;

    pxi_source_ctrl = p_api_dev->IoRAMBase[AYE_REG_PXI_IRIG_SOURCE_CONTROL];
    irig_status     = p_api_dev->IoRAMBase[AEC_REG_TT_STATUS];
    *irig_src       = (irig_status >> 15) & 0x1;

    if (*irig_src == 0) /* intern IRIG is set */
        *irig_src = API_IRIG_ONBOARD;
    else
        *irig_src = API_IRIG_EXTERN;

    if (0 == (irig_status & 0x01))
        *irig_sync = API_IRIG_NOT_SYNC;


    // AMCX/AXC devices do no have the AYE_REG_PXI_IRIG_SOURCE_CONTROL register. Make sure we are on a PXI card.
    if (mil_hw_pxi_is_supported(p_api_dev))
    {
        if (1 == (pxi_source_ctrl & 0x01)) /* PXI 10MHz is active (Bit0: SEL==1)*/
        {
            /* When  SEL is set to 1 the decoder ADC interface is disabled, the IRIG DECODER
            works in Free Wheeling mode but synchronously with the 10MHz PXI clock.

            In PXI mode the 1KHz mode driving the IRIG signal is derived from the 10MHz
            PXI clock instead of the external IRIG input. Due to the disconnected ADC
            the sync bit (irig_status&0x1) is always 0 (not in sync). The FW bit can be
            used to detect if the 10MHz clock is present FW==1 or is absent FW==0.
            */
            if (0x10 == (irig_status & 0x10)) /* Bit4 = FW - "Free Wheeling"*/
                *irig_sync = API_IRIG_SYNC;
            else
                *irig_sync = API_IRIG_NOT_SYNC;
        }
    }

    return 0;
}


/*! \brief This function reads the hw irig time through the TCP. */
/*!
    This function reads the hw irig time through the TCP.

    \param  p_api_dev   TY_API_DEV*      The global variable container.
    \param  sth         L_WORD *         IRIG time encode Time Tag High
    \param  stl         L_WORD *         IRIG time encode Time Tag Low
    
    \return ackfl      BYTE             Instruction acknowledge type.
*/
L_WORD mil_hw_pci_driver_aye_irig_get_time(TY_API_DEV * p_api_dev,
                           L_WORD     * sth, 
                           L_WORD     * stl )
{
    *sth = p_api_dev->IoRAMBase[AEC_REG_TT_HIGH];
    *stl = p_api_dev->IoRAMBase[AEC_REG_TT_LOW];

    return 0;
}





/*****************************************************************************
 *                                                                           *
 *                                                                           *
 *                 HW Layer public TSW Discretes access                      *
 *                                                                           *
 *                                                                           *
 *****************************************************************************/

/*! \brief Read Discrete interrupt entries from Loglist */
/*!

\param  p_api_dev          TY_API_DEV*      The global variable container.
\param  int_count          AiUInt32         Interrupt count

\return AiReturn The return value is 0 on success or non zero on failure.
*/
AiReturn mil_hw_pci_driver_aye_discretes_fifo_read(TY_API_DEV *p_api_dev, AiUInt32 int_count)
{
    L_WORD   i, ulTmp;
    volatile L_WORD ttag_hi = 0;
    volatile L_WORD ttag_lo = 0;
    volatile L_WORD ulValue = 0;
    volatile L_WORD ulData = 0;

    /* Handle AyE GPIO interrupts for Recording */

    /* Read Loglist FIFO status */
    ulValue = p_api_dev->IoRAMBase[AEC_REG_IRQ_LOGLIST_STATUS];
    if ((ulValue & 0x80000000L) == 0) {
        if ((ulValue & 0xFFL) <= 32) {
            for (i = 0; i < ulValue; i++) {

                ulData = 0;
                ttag_lo = 0;
                ttag_hi = 0;

                /* Read FIFO Loglist entries */
                ttag_lo = p_api_dev->IoRAMBase[AEC_REG_IRQ_LOGLIST_FIFO_READ_LO];
                ttag_hi = p_api_dev->IoRAMBase[AEC_REG_IRQ_LOGLIST_FIFO_READ_HI];

                /* Interrupt cause (Bit8..15) and Status of all Discretes (Bit 0..7) */
                ulData = ttag_hi >> 16;

                /* Put Recording entry into queue */
                api_ir_enque_aye_discretes_from_fpga(p_api_dev, 1, ttag_hi & 0x00003FFFL, ttag_lo, ulData);
            }
        }
    }
    else {
        /* FIFO full indicated */
        for (i = 0; i < 32; i++) {
            /* Flush */
            ulTmp = p_api_dev->IoRAMBase[AEC_REG_IRQ_LOGLIST_FIFO_READ_LO];
            ulTmp = p_api_dev->IoRAMBase[AEC_REG_IRQ_LOGLIST_FIFO_READ_HI];
        }
    }

    return 0;
}

/*! \brief Setup Discrete interrupt and enable loglist */
/*!

\param  p_api_dev          TY_API_DEV*      The global variable container.
\param  ul_DiscreteSetup   AiUInt32         Setup Discrete interrupt and loglist enable

\return AiReturn The return value is 0 on success or non zero on failure.
*/
AiReturn mil_hw_pci_driver_aye_discretes_fifo(TY_API_DEV *p_api_dev, AiUInt32 new_config)
{
    L_WORD mask;

    /* enable Discrete Interrupts and related FIFO Enable bits */
    mask = BSWAP32(p_api_dev->IoRAMBase[AEC_REG_IRQ_HOST_MASK]);
    mask &= ~0xFFFF; /* Clear bits 0-15 */

    if (new_config & 0x01)
        mask |= AEC_IRQ_DISCR_0;
    if (new_config & 0x02)
        mask |= AEC_IRQ_DISCR_1;
    if (new_config & 0x04)
        mask |= AEC_IRQ_DISCR_2;
    if (new_config & 0x08)
        mask |= AEC_IRQ_DISCR_3;
    if (new_config & 0x10)
        mask |= AEC_IRQ_DISCR_4;
    if (new_config & 0x20)
        mask |= AEC_IRQ_DISCR_5;
    if (new_config & 0x40)
        mask |= AEC_IRQ_DISCR_6;
    if (new_config & 0x80)
        mask |= AEC_IRQ_DISCR_7;

    p_api_dev->IoRAMBase[AEC_REG_IRQ_HOST_MASK] = BSWAP32(mask);
    p_api_dev->IoRAMBase[AEC_REG_IRQ_LOGLIST_ENABLE] = BSWAP32(mask & 0x0000FFFFL);

    return 0;
}

/*! \brief Initialize discrete input/output */
/*!

    \param  p_api_dev          TY_API_DEV*      The global variable container.
    \param  ul_DiscreteSetup   AiUInt32         The discrete setup

    \return AiReturn The return value is 0 on success or non zero on failure.
*/
AiReturn mil_hw_pci_driver_aye_discretes_init(TY_API_DEV *p_api_dev, AiUInt32 new_config )
{
    L_WORD current_config;

    current_config = BSWAP32(p_api_dev->IoRAMBase[AEC_REG_GPIO_CONFIG]);

    current_config &= ~0xFF; /* Clear bits 0-7 */
    new_config     &= 0xFF;  /* Only bits 0-7 are valid */
    current_config |= new_config;

    p_api_dev->IoRAMBase[AEC_REG_GPIO_CONFIG] = BSWAP32(current_config);

    return 0;
}

/*! \brief Write discrete output values */
/*!

    \param  p_api_dev    TY_API_DEV*      The global variable container.
    \param  Mask         AiUInt32         Output mask
    \param  Value        AiUInt32         Output value

    \return AiReturn The return value is 0 on success or non zero on failure.
*/
AiReturn mil_hw_pci_driver_aye_discretes_write(TY_API_DEV *p_api_dev, AiUInt32 Mask, AiUInt32 Value)
{
    L_WORD ul_Tmp;

    Mask  &= 0x000000ff; /* only the lowest 8 bit are valid */
    Value &= 0x000000ff; /* only the lowest 8 bit are valid */

    ul_Tmp = BSWAP32(p_api_dev->IoRAMBase[AEC_REG_GPIO_OUTPUT]);

    Value = ~Value; /* Inverted logic */

    ul_Tmp &= (Value | ~Mask); /* Set all related bits to low  */
    ul_Tmp |= (Value &  Mask); /* Set all related bits to high */

    p_api_dev->IoRAMBase[AEC_REG_GPIO_OUTPUT] = BSWAP32(ul_Tmp);

    return 0;
}


/*! \brief Read discrete input values */
/*!

    \param  p_api_dev    TY_API_DEV*      The global variable container.
    \param  Value        AiUInt32         input value

    \return AiReturn The return value is 0 on success or non zero on failure.
*/
AiReturn mil_hw_pci_driver_aye_discretes_read(TY_API_DEV *p_api_dev, AiUInt32 * Value)
{
    *Value = BSWAP32(p_api_dev->IoRAMBase[AEC_REG_GPIO_INPUT]);
    *Value &= 0x000000ff;

    return 0;
}









/*****************************************************************************
 *                                                                           *
 *                                                                           *
 *                 HW Layer public TSW system information                    *
 *                                                                           *
 *                                                                           *
 *****************************************************************************/



/*! \brief Get some information about the discrete channels */
/*! 
    \param  p_api_dev               TY_API_DEV*    The global variable container.
    \param  pulDiscreteCount        AiUInt32*      Output variable for the ammount of discrete channels.
    \param  pulDiscreteConfig       AiUInt32*      Output variable for the configuration of the discrete channel. 
                                                   The bits of this field are one if this discrete can send data or zero if not.

    \return AiReturn  The return value is 0 on success or non zero on failure.
*/
AiReturn mil_hw_pci_driver_aye_get_discretes(TY_API_DEV * p_api_dev, AiUInt32 * pulDiscreteCount, AiUInt32 * pulDiscreteConfig)
{
    *pulDiscreteCount  = 8;
    *pulDiscreteConfig = 0xFF; /* all discrete channels are read/write */

    return 0;
}


/*! \brief Get some information about the discrete channels */
/*!
\param  p_api_dev               TY_API_DEV*    The global variable container.
\param  pul_Channel             AiUInt32*      Number of discrete channels
\param  pul_Canin               AiUInt32*      bit field that tells, which channels can be set to In 
\param  pul_Canout              AiUInt32*      bit field that tells, which channels can be set to Out
\return AiUInt32  The return value is 0 on success or non zero on failure.
*/
AiReturn  mil_hw_pci_driver_aye_discretes_info_read(TY_API_DEV * p_api_dev, AiUInt32 *pul_Channel, AiUInt32 *pul_Canin, AiUInt32 *pul_Canout)
{
    *pul_Channel = 8;
    *pul_Canin   = 0xFF;
    *pul_Canout  = 0xFF;

    return 0;
}


/*! \brief Returns the type of the firmware

\param p_api_dev pointer to target administration structure

\return returns The type of the FW 
*/
enum MIL_FW mil_hw_pci_driver_aye_get_fw_type(TY_API_DEV * p_api_dev, short ulBiu)
{
    AiUInt32 multi_channel;

    multi_channel = ul_StreamIsMultiChannel(p_api_dev, ulBiu);

    if (multi_channel != 0)
    {
        if ((p_api_dev->BoardApplicationType & 0x10) != 0)
            return MIL_FW_MCH_EBD_AT91;
        else
            return MIL_FW_MCH_AT91;
    }
    else
    {
        if ((p_api_dev->BoardApplicationType & 0x10) != 0)
            return MIL_FW_EBD_AT91;
        else
            return MIL_FW_STD_AT91;
    }

    return multi_channel ? MIL_FW_MCH_AT91 : MIL_FW_STD_AT91;
}

/*****************************************************************************
*                                                                           *
*                                                                           *
*                 HW Layer public TSW version information                   *
*                                                                           *
*                                                                           *
*****************************************************************************/
extern const short aul_MapVersionToPhysicalBiu[];


/*! \brief Read a specific FW version

\param p_api_dev        Global variable container
\param eId              The version to read
\param pxVersionValue   The numeric version representation
\param pxVersionString  The string version representation

\return returns         0 on success or API_ERR_INVALID_MODE if version not found.
*/
L_WORD mil_hw_pci_driver_aye_version_fw(  TY_API_DEV *p_api_dev, TY_E_VERSION_ID eId, TY_API_VER_INFO_VALUES * pxVersionValue, TY_API_VER_INFO_STRINGS * pxVersionString )
{
    L_WORD  version = 0;
    L_WORD git_hash;


    char ucDesc[AI_VERSION_STRINGL];
    
    short  logical_biu = 0;
    short  physical_biu = 0;

    physical_biu = aul_MapVersionToPhysicalBiu[eId];

    if (physical_biu < 0)
        return API_ERR_INVALID_MODE;

    if (physical_biu >= p_api_dev->physical_bius)
        return API_ERR_INVALID_MODE;

    logical_biu = api_sys_convert_biu_physical_to_logical(p_api_dev, physical_biu);

    /* Firmware */
    if (logical_biu < p_api_dev->chns)
    {
        TyMyMonContext * pContext = p_api_dev->pxMyMonContext;

        AI_TSW_OS_SPRINTF(ucDesc, AI_DESCRIPTION_STRINGL, "FW BIU%d", physical_biu+1);
        pContext->pfReadGlobalExt( pContext, MYMON_GREG_VERSION_BIU1 + physical_biu*4, &version, 1 );

        pxVersionValue->ul_VersionType  = eId;
        pxVersionValue->ul_MajorVer     = BCD2BIN_B( EXTRACT_BITS(version,24,0xFF   ) );
        pxVersionValue->ul_MinorVer     = BCD2BIN_B( EXTRACT_BITS(version,16,0xFF   ) );
        pxVersionValue->ul_PatchVersion = 0;
        pxVersionValue->ul_BuildNr      = BCD2BIN_W( EXTRACT_BITS(version,0,0xFFFF  ) );

        AI_TSW_OS_SPRINTF(pxVersionString->ac_Description, AI_DESCRIPTION_STRINGL, "FW BIU%d", physical_biu+1);

        git_hash = p_api_dev->cb_p[logical_biu]->git_hash;

        if(git_hash)
        {
            AI_TSW_OS_SPRINTF(pxVersionString->ac_FullVersion, AI_VERSION_STRINGL, "%d.%d.%d-g%x",
                                                    pxVersionValue->ul_MajorVer,
                                                    pxVersionValue->ul_MinorVer,
                                                    pxVersionValue->ul_BuildNr,
                                                    git_hash );
        }
        else
        {
            AI_TSW_OS_SPRINTF(pxVersionString->ac_FullVersion, AI_VERSION_STRINGL, "%d.%d.%d",
                pxVersionValue->ul_MajorVer,
                pxVersionValue->ul_MinorVer,
                pxVersionValue->ul_BuildNr );
        }


        return 0;
    }

    return API_ERR_INVALID_MODE;
}


/*! \brief Read a specific TCP version

\param p_api_dev        Global variable container
\param eId              The version to read
\param pxVersionValue   The numeric version representation
\param pxVersionString  The string version representation

\return returns         0 on success or API_ERR_INVALID_MODE if version not found.
*/
L_WORD mil_hw_pci_driver_aye_version_tcp(  TY_API_DEV *p_api_dev, TY_E_VERSION_ID eId, TY_API_VER_INFO_VALUES * pxVersionValue, TY_API_VER_INFO_STRINGS * pxVersionString )
{
    L_WORD  version = 0;
    TyMyMonContext * pContext = p_api_dev->pxMyMonContext;

    pContext->pfReadGlobalExt( pContext, MYMON_GREG_VERSION_MYMON, &version, 1 );

    pxVersionValue->ul_VersionType  = AI_TCP_VER;
    pxVersionValue->ul_MajorVer     = BCD2BIN_B( EXTRACT_BITS(version,24,0xFF   ) );
    pxVersionValue->ul_MinorVer     = BCD2BIN_B( EXTRACT_BITS(version,16,0xFF   ) );
    pxVersionValue->ul_PatchVersion = 0;
    pxVersionValue->ul_BuildNr      = BCD2BIN_W( EXTRACT_BITS(version,0,0xFFFF  ) );

    AI_TSW_OS_SPRINTF(pxVersionString->ac_Description, AI_DESCRIPTION_STRINGL, "TCP");
    AI_TSW_OS_SPRINTF(pxVersionString->ac_FullVersion, AI_VERSION_STRINGL, "%d.%d.%d",
                                                pxVersionValue->ul_MajorVer,
                                                pxVersionValue->ul_MinorVer,
                                                pxVersionValue->ul_BuildNr );
        
    return 0;
}


/*! \brief Read a specific MLCA version

\param p_api_dev        Global variable container
\param eId              The version to read
\param pxVersionValue   The numeric version representation
\param pxVersionString  The string version representation

\return returns         0 on success or API_ERR_INVALID_MODE if version not found.
*/
L_WORD mil_hw_pci_driver_aye_version_mlca(TY_API_DEV *p_api_dev, TY_E_VERSION_ID eId, TY_API_VER_INFO_VALUES * pxVersionValue, TY_API_VER_INFO_STRINGS * pxVersionString)
{
    L_WORD  version = 0;
    TyMyMonContext * pContext = p_api_dev->pxMyMonContext;

    pContext->pfReadGlobalExt(pContext, MYMON_GREG_VERSION_SOC, &version, 1);

    pxVersionValue->ul_VersionType = AI_MAIN_LCA_VER;
    pxVersionValue->ul_MajorVer = BCD2BIN_B(EXTRACT_BITS(version, 24, 0xFF));
    pxVersionValue->ul_MinorVer = BCD2BIN_B(EXTRACT_BITS(version, 16, 0xFF));
    pxVersionValue->ul_PatchVersion = 0;
    pxVersionValue->ul_BuildNr = BCD2BIN_W(EXTRACT_BITS(version, 0, 0xFFFF));

    AI_TSW_OS_SPRINTF(pxVersionString->ac_Description, AI_DESCRIPTION_STRINGL, "SOC");
    AI_TSW_OS_SPRINTF(pxVersionString->ac_FullVersion, AI_VERSION_STRINGL, "%d.%d.%d",
        pxVersionValue->ul_MajorVer,
        pxVersionValue->ul_MinorVer,
        pxVersionValue->ul_BuildNr);

    return 0;
}


/*! \brief Read a specific FPGA version

\param p_api_dev        Global variable container
\param eId              The version to read
\param pxVersionValue   The numeric version representation
\param pxVersionString  The string version representation

\return returns         0 on success or API_ERR_INVALID_MODE if version not found.
*/
L_WORD mil_hw_pci_driver_aye_version_fpga(TY_API_DEV *p_api_dev, TY_E_VERSION_ID eId, TY_API_VER_INFO_VALUES * pxVersionValue, TY_API_VER_INFO_STRINGS * pxVersionString)
{
    L_WORD  version = 0;
    TyMyMonContext * pContext = p_api_dev->pxMyMonContext;

    pContext->pfReadGlobalExt(pContext, MYMON_GREG_VERSION_FPGA, &version, 1);

    pxVersionValue->ul_VersionType = AI_FPGA_VER;
    pxVersionValue->ul_MajorVer = BCD2BIN_B(EXTRACT_BITS(version, 24, 0xFF));
    pxVersionValue->ul_MinorVer = BCD2BIN_B(EXTRACT_BITS(version, 16, 0xFF));
    pxVersionValue->ul_PatchVersion = 0;
    pxVersionValue->ul_BuildNr = BCD2BIN_W(EXTRACT_BITS(version, 0, 0xFFFF));

    AI_TSW_OS_SPRINTF(pxVersionString->ac_Description, AI_DESCRIPTION_STRINGL, "FPGA");
    AI_TSW_OS_SPRINTF(pxVersionString->ac_FullVersion, AI_VERSION_STRINGL, "%d.%d.%d",
        pxVersionValue->ul_MajorVer,
        pxVersionValue->ul_MinorVer,
        pxVersionValue->ul_BuildNr);

    return 0;
}


/*! \brief Read a specific Monitor version

\param p_api_dev        Global variable container
\param eId              The version to read
\param pxVersionValue   The numeric version representation
\param pxVersionString  The string version representation

\return returns         0 on success or API_ERR_INVALID_MODE if version not found.
*/
L_WORD mil_hw_pci_driver_aye_version_monitor(TY_API_DEV *p_api_dev, TY_E_VERSION_ID eId, TY_API_VER_INFO_VALUES * pxVersionValue, TY_API_VER_INFO_STRINGS * pxVersionString)
{
    L_WORD  version = 0;
    TyMyMonContext * pContext = p_api_dev->pxMyMonContext;

    pContext->pfReadGlobalExt( pContext, MYMON_GREG_VERSION_MYMON, &version, 1 );

    pxVersionValue->ul_VersionType  = AI_MONITOR_VER;
    pxVersionValue->ul_MajorVer     = BCD2BIN_B( EXTRACT_BITS(version,24,0xFF   ) );
    pxVersionValue->ul_MinorVer     = BCD2BIN_B( EXTRACT_BITS(version,16,0xFF   ) );
    pxVersionValue->ul_PatchVersion = 0;
    pxVersionValue->ul_BuildNr      = BCD2BIN_W( EXTRACT_BITS(version,0,0xFFFF  ) );

    AI_TSW_OS_SPRINTF(pxVersionString->ac_Description, AI_DESCRIPTION_STRINGL, "Monitor");
    AI_TSW_OS_SPRINTF(pxVersionString->ac_FullVersion, AI_VERSION_STRINGL, "%d.%d.%d",
                                                pxVersionValue->ul_MajorVer,
                                                pxVersionValue->ul_MinorVer,
                                                pxVersionValue->ul_BuildNr );
        
    return 0;
}

/*****************************************************************************
*                                                                           *
*                                                                           *
*                      HW Layer public TSW Trigger                          *
*                                                                           *
*                                                                           *
*****************************************************************************/

/*! \brief Check if trigger digital loop is available in this FPGA */
/*!
This function returns true if digital trigger loop is supported and false in any other case.

\param  p_api_dev      TY_API_DEV*             The global variable container.

\return                AiBoolean               True on trigger support
*/
AiBoolean mil_hw_pci_driver_aye_has_digital_loop(TY_API_DEV* p_api_dev)
{
    if( (p_api_dev->BoardConfig & 0x000000FF) == AI_PLATFORM_PCIE_CARD )
        /*AEC1553 is no longer updated*/
        return AiFalse;
    else
        return AiTrue;
}

/*! \brief Configure digital trigger loop */
/*!
This function configures the digital trigger loop.

\param  p_api_dev      TY_API_DEV*             The global variable container.
\param  con            AiUInt32                Configuration value.

*/
AiReturn mil_hw_pci_driver_aye_trigger_digital_loop_con(TY_API_DEV* p_api_dev, AiUInt32 con)
{
    p_api_dev->IoRAMBase[AYE_REG_TRIGGER_DIGITAL_LOOP] = con;
    return 0;
}

/*! \brief Read digital trigger loop Configuration */
/*!
This function reads the digital trigger loop configures.

\param  p_api_dev      TY_API_DEV*             The global variable container.
\param  con            AiUInt32*               Configuration value.

*/
AiReturn mil_hw_pci_driver_aye_trigger_digital_loop_get(TY_API_DEV* p_api_dev, AiUInt32 * con)
{
    *con = p_api_dev->IoRAMBase[AYE_REG_TRIGGER_DIGITAL_LOOP];
    return 0;
}

#endif /* defined(_AIM_SYSDRV) */
