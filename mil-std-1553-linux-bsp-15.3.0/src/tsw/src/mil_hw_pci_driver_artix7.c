/* SPDX-FileCopyrightText: 2020-2024 AIM GmbH <info@aim-online.com> */
/* SPDX-License-Identifier: MIT OR GPL-2.0-or-later */

/*! \file mil_hw_pci_artix7.c
    This file contains hw layer implementations for Artix7 without ASP.

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
#include "hw/AiHwArtix7.h"

#include "mil_hw_pci_driver.h"



AiInt16 mil_hw_artix7_internal_tcp_command(TY_API_DEV *  p_api_dev,
    AiUInt32 ulCommand,
    AiUInt32 ulInputCount, AiUInt32 ulInput[],
    AiUInt32 ulOutputCount, AiUInt32 ulOutput[]);

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
AiUInt32 mil_hw_pci_driver_artix7_novram_write(TY_API_DEV *p_api_dev, AiUInt32 ulOffset, AiUInt32 ulValue)
{
    p_api_dev->IoRAMBase[ARTIX7_REG_NOVRAM_BASE_OFFSET + ulOffset/4] = ulValue;

    return 0;
}


/*! \brief Read from the given NOVRAM offset. */
/*!

\param  p_api_dev          TY_API_DEV*      The global variable container.
\param  ulOffset           AiUInt32         The NOVRAM offset
\param  pulValue           AiUInt32*        The NOVRAM value

\return L_WORD The return value is 0 on success or non zero on failure.
*/
AiUInt32 mil_hw_pci_driver_artix7_novram_read(TY_API_DEV *p_api_dev, AiUInt32 ulOffset, AiUInt32 *pulValue)
{
    *pulValue = p_api_dev->IoRAMBase[ARTIX7_REG_NOVRAM_BASE_OFFSET + ulOffset/4];

    return 0;
}


/*! \brief Flush the NOVRAM mirror. */
/*!

\param  p_api_dev          TY_API_DEV*      The global variable container.
\return L_WORD The return value is 0 on success or non zero on failure.
*/
AiUInt32 mil_hw_pci_driver_artix7_novram_finalize(TY_API_DEV *p_api_dev)
{
    AiUInt16 wRetVal    = 0;
    AiUInt32 ulOutput   = 0;

    wRetVal = mil_hw_artix7_internal_tcp_command(p_api_dev, ARTIX7_MON_CMD_NOVRAM_FLUSH, 0, 0, 1, &ulOutput);

    if ((wRetVal  == ARTIX7_MON_OK)
     && (ulOutput != ARTIX7_MON_PASS))
    {
        wRetVal = ARTIX7_MON_ERR_STATUS;
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
AiInt16 mil_hw_artix7_internal_tcp_command(TY_API_DEV *  p_api_dev,
    AiUInt32 ulCommand,
    AiUInt32 ulInputCount, AiUInt32 ulInput[],
    AiUInt32 ulOutputCount, AiUInt32 ulOutput[])
{
    AiUInt32  i = 0;
    AiUInt32  ulValue = 0;
    AiUInt32  ulCommandZero = 0;

    /* check if no other process is running */
    /* multiple processes are not supported */

    /* copy input data into registers */
    for (i = 0; i<ulInputCount; i++)
        p_api_dev->IoRAMBase[ARTIX7_REG_TCP_INPUT_0 + i] = ulInput[i];

    /* execute command */
    ulCommand = ulCommand << 24;

    p_api_dev->IoRAMBase[ARTIX7_REG_TCP_CONTROL] = ulCommand;

    ai_tsw_os_wait_usec(100);

    /* wait until command is finished */

    for (i = 0; i<10000; i++)
    {
        ulValue = p_api_dev->IoRAMBase[ARTIX7_REG_TCP_STATUS];

        if (((ulValue & ARTIX7_MON_STATUS_MASK) == ARTIX7_MON_STATUS_FREE)
            && ((ulValue & ARTIX7_MON_STATUS_CMD) == ulCommand))
            /* finished */
            break;

        ai_tsw_os_wait_usec(100);
    }

    if (i == 10000)
        /* timeout occured */
        return ARTIX7_MON_ERR_TIMEOUT;

    /* copy return data */

    if (ulOutputCount > 0)
        for (i = 0; i<ulOutputCount; i++)
            ulOutput[i] = p_api_dev->IoRAMBase[ARTIX7_REG_TCP_OUTPUT_0 + i];

    /* -- execute command 0 to reset the status --- */
    p_api_dev->IoRAMBase[ARTIX7_REG_TCP_CONTROL] = ulCommandZero;

    return ARTIX7_MON_OK;
}







/*! \brief This function sets the hw irig time source the TCP. */
/*!
    This function handles the 'API_EXEC_SYS' instruction 1

    \param  p_api_dev  TY_API_DEV*      The global variable container.
    \param  con        BYTE             System Command control
    
    \return ackfl      BYTE             Instruction acknowledge type.
*/
L_WORD mil_hw_pci_driver_artix7_irig_set_external(TY_API_DEV *p_api_dev, BYTE con)
{
    L_WORD value;

    if (con & 0x01)
    {
        value = p_api_dev->IoRAMBase[ARTIX7_REG_TIME_SOURCE_CONTROL];
        value |= ARTIX7_REG_TIME_SOURCE_CONTROL_BIT_EXTERN; /* Bit 8=1 is irig external*/
        p_api_dev->IoRAMBase[ARTIX7_REG_TIME_SOURCE_CONTROL] = value;
    }
    else
    {
        value = p_api_dev->IoRAMBase[ARTIX7_REG_TIME_SOURCE_CONTROL];
        value &= ~ARTIX7_REG_TIME_SOURCE_CONTROL_BIT_EXTERN; /* Bit 8=1 is irig external*/
        p_api_dev->IoRAMBase[ARTIX7_REG_TIME_SOURCE_CONTROL] = value;
    }

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
    \param  us         BYTE             IRIG time encode micro second

    \return ackfl      BYTE             Instruction acknowledge type.
*/
L_WORD mil_hw_pci_driver_artix7_irig_set(TY_API_DEV* p_api_dev,
    L_WORD         day,
    L_WORD         hour,
    L_WORD         min,
    L_WORD         sec,
    L_WORD         us)
{
    L_WORD   ret_val = 0;
    WORD     wRetVal = 0;
    AiUInt32 ulInput[3];
    AiUInt32 irig_status;
    /*
    Legacy FPGA irig implementation:
        Irig internal/external: Set the Irig encoder register with TCP-FW command.
                                Wait for 3 seconds.
                                The time is ready in the decoder register.

    Updated FPGA time implementation (with an external irig loop encoder->decoder for cases where the external signal shall be used):
        Irig internal: Set the board time register in us with IO register write command.
                       The time is ready to be read immediately.
                       Set the Irig encoder register with TCP-FW command in order to have a similar time in case we switch to irig extern later.
                       Switch the time to external.
                       Wait for 3 seconds.
                       The time is ready in the decoder register.

        Irig external: Set the board time register in us with IO register write command.
                       The time is ignored.
                       Set the Irig encoder register with TCP-FW command.
                       Wait for 3 seconds.
                       The time is ready in the decoder register.
                       Switch the time to internal.
                       The updated time from the previous register write is returned.
    */


    ulInput[0] = 0;           /* time                */
    ulInput[1] = 0;           /* IEEE1344 (reserved) */
    ulInput[2] = 1ul << 31;   /* update + leap Year  */

    ulInput[0] |= sec;
    ulInput[0] |= min << 6;
    ulInput[0] |= hour << 12;
    ulInput[0] |= day << 17;

    /* -- check if we can set the time incl. us by writing the irig register --- */
    irig_status = p_api_dev->IoRAMBase[ARTIX7_REG_TIME_SOURCE_CONTROL];

    if (irig_status & ARTIX7_REG_TIME_SOURCE_CONTROL_BIT_INSTANT)
    {
        /* This will only set the the on-board time register which is decoupled from the irig encoder/decoder */
        p_api_dev->IoRAMBase[ARTIX7_REG_TT_HIGH] = ulInput[0];
        p_api_dev->IoRAMBase[ARTIX7_REG_TT_LOW] = min << 26 | sec << 20 | us;
        wbflush();
    }

    /* -- set irig encoder register via TCP-FW command --- */
    wRetVal = mil_hw_artix7_internal_tcp_command(p_api_dev, ARTIX7_MON_CMD_IRIG_SET, 3, ulInput, 0, 0);

    if (wRetVal != API_OK)
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
L_WORD mil_hw_pci_driver_artix7_irig_get_status(TY_API_DEV *p_api_dev, AiUInt32 * irig_sync, AiUInt32 * irig_src)
{
    L_WORD irig_status = 0;

    irig_status = p_api_dev->IoRAMBase[ARTIX7_REG_TIME_SOURCE_CONTROL];
    *irig_src    = (irig_status & ARTIX7_REG_TIME_SOURCE_CONTROL_BIT_EXTERN);

    if (*irig_src == 0) /* intern IRIG is set */
        *irig_src  = API_IRIG_ONBOARD;
    else
    {
        *irig_src = API_IRIG_EXTERN;
    }

    irig_status = p_api_dev->IoRAMBase[ARTIX7_REG_TT_STATUS];
    if (0 == (irig_status & (1 << 27)))
        *irig_sync = API_IRIG_NOT_SYNC;

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
L_WORD mil_hw_pci_driver_artix7_irig_get_time(TY_API_DEV * p_api_dev,
                           L_WORD     * sth, 
                           L_WORD     * stl )
{
    *sth = p_api_dev->IoRAMBase[ARTIX7_REG_TT_HIGH];
    *stl = p_api_dev->IoRAMBase[ARTIX7_REG_TT_LOW];

    return 0;
}




/*! \brief Initialize the IRIG capabilies structure within p_api_dev */
/*!
    This function initializes the IRIG capabilities structure.

    \param  p_api_dev   TY_API_DEV*      The global variable container.

    \return Always 0.
*/
L_WORD mil_hw_pci_driver_artix7_irig_set_capabilities(TY_API_DEV* p_api_dev)
{
    L_WORD irig_status = 0;

    p_api_dev->x_IrigCapabilities.b.ul_Res = 0;
    p_api_dev->x_IrigCapabilities.b.b_IrigSwitch   = AiTrue;
    p_api_dev->x_IrigCapabilities.b.b_FreeWheeling = AiTrue;
    p_api_dev->x_IrigCapabilities.b.b_Sinusoidal   = AiTrue;


    irig_status = p_api_dev->IoRAMBase[ARTIX7_REG_TIME_SOURCE_CONTROL];

    /* Check if FPGA supports instant set irig time */
    if (irig_status & ARTIX7_REG_TIME_SOURCE_CONTROL_BIT_INSTANT)
        p_api_dev->x_IrigCapabilities.b.b_InstantIrigSetOnInternalMode = AiTrue;
    else
        p_api_dev->x_IrigCapabilities.b.b_InstantIrigSetOnInternalMode = AiFalse;

    /* Check if FPGA provides an external irig signal in case of internal irig mode */
    if (irig_status & ARTIX7_REG_TIME_SOURCE_CONTROL_BIT_OUT_SIGNAL)
        p_api_dev->x_IrigCapabilities.b.b_NoExternalSignalOnInternalMode = AiTrue;
    else
        p_api_dev->x_IrigCapabilities.b.b_NoExternalSignalOnInternalMode = AiFalse;


    return 0;
}




/*****************************************************************************
 *                                                                           *
 *                                                                           *
 *                 HW Layer public TSW Discretes access                      *
 *                                                                           *
 *                                                                           *
 *****************************************************************************/

/*! \brief Initialize discrete input/output */
/*!

    \param  p_api_dev          TY_API_DEV*      The global variable container.
    \param  ul_DiscreteSetup   AiUInt32         The discrete setup

    \return AiReturn The return value is 0 on success or non zero on failure.
*/
AiReturn mil_hw_pci_driver_artix7_discretes_init(TY_API_DEV *p_api_dev, AiUInt32 new_config)
{
    L_WORD current_config;
    current_config = BSWAP32(p_api_dev->IoRAMBase[ARTIX7_REG_GPIO_CONFIG]);

    current_config &= ~0xFF; /* Clear bits 0-7 */
    new_config &= 0xFF;  /* Only bits 0-7 are valid */
    current_config |= new_config;

    p_api_dev->IoRAMBase[ARTIX7_REG_GPIO_CONFIG] = BSWAP32(current_config);

    return 0;
}

/*! \brief Write discrete output values */
/*!

    \param  p_api_dev    TY_API_DEV*      The global variable container.
    \param  Mask         AiUInt32         Output mask
    \param  Value        AiUInt32         Output value

    \return AiReturn The return value is 0 on success or non zero on failure.
*/
AiReturn mil_hw_pci_driver_artix7_discretes_write(TY_API_DEV *p_api_dev, AiUInt32 Mask, AiUInt32 Value)
{
    L_WORD ul_Tmp;

    Mask  &= 0x000000ff; /* only the lowest 8 bit are valid */
    Value &= 0x000000ff; /* only the lowest 8 bit are valid */

    ul_Tmp = p_api_dev->IoRAMBase[ARTIX7_REG_GPIO_OUTPUT];
    ul_Tmp &= (Value | ~Mask); /* Set all related bits to low  */
    ul_Tmp |= (Value &  Mask); /* Set all related bits to high */
    p_api_dev->IoRAMBase[ARTIX7_REG_GPIO_OUTPUT] = ul_Tmp;

    return 0;
}


/*! \brief Read discrete input values */
/*!

    \param  p_api_dev    TY_API_DEV*      The global variable container.
    \param  Value        AiUInt32         input value

    \return AiReturn The return value is 0 on success or non zero on failure.
*/
AiReturn mil_hw_pci_driver_artix7_discretes_read(TY_API_DEV *p_api_dev, AiUInt32 * Value)
{
    *Value = p_api_dev->IoRAMBase[ARTIX7_REG_GPIO_INPUT];

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
AiReturn mil_hw_pci_driver_artix7_get_discretes(TY_API_DEV * p_api_dev, AiUInt32 * pulDiscreteCount, AiUInt32 * pulDiscreteConfig)
{
    switch (p_api_dev->BoardConfig & 0x000000FF)
    {
    case AI_PLATFORM_MINI_PCIE_ARTIX7:
        *pulDiscreteCount = 8;
        *pulDiscreteConfig = 0xC0; /* 6 IN + 2IN/OUT (0b11000000)*/
        break;
    case AI_PLATFORM_M2_B_M_ARTIX7:
        *pulDiscreteCount = 4;
        *pulDiscreteConfig = 0x03; /* 2 IN/OUT + 2IN (0b0011)*/
        break;
    default:
        *pulDiscreteCount = 0;
        *pulDiscreteConfig = 0;
        break;
    }

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
AiReturn mil_hw_pci_driver_artix7_discretes_info_read(TY_API_DEV * p_api_dev, AiUInt32 *pul_Channel, AiUInt32 *pul_Canin, AiUInt32 *pul_Canout)
{
    switch (p_api_dev->BoardConfig & 0x000000FF)
    {
    case AI_PLATFORM_MINI_PCIE_ARTIX7:
        *pul_Channel = 8;
        *pul_Canin   = 0xFF; // 0b11111111
        *pul_Canout  = 0xC0; // 0b11000000
        break;
    case AI_PLATFORM_M2_B_M_ARTIX7:
        *pul_Channel = 4;
        *pul_Canin   = 0xF; // 0b1111
        *pul_Canout  = 0x3; // 0b0011
        break;
    default:
        *pul_Channel = 0;
        *pul_Canin   = 0;
        *pul_Canout  = 0;
        break;
    }

    return 0;
}


/*! \brief Returns the type of the firmware

\param p_api_dev pointer to target administration structure

\return returns The type of the FW 
*/
enum MIL_FW mil_hw_pci_driver_artix7_get_fw_type(TY_API_DEV * p_api_dev, short ulBiu)
{
    if (API_DEVICE_MODE_IS_EMBEDDED(p_api_dev->drv_mode[ulBiu]))
        return MIL_FW_EBD_ARTIX7;   // AMEE1553
    else
        return MIL_FW_STD_ARTIX7;   // ARTIX7 Test and Simzulation;
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
L_WORD mil_hw_pci_driver_artix7_version_fw(TY_API_DEV *p_api_dev, TY_E_VERSION_ID eId, TY_API_VER_INFO_VALUES * pxVersionValue, TY_API_VER_INFO_STRINGS * pxVersionString)
{
    return api_sys_read_version_fw(p_api_dev, eId, pxVersionValue, pxVersionString);
}


/*! \brief Read a specific TCP version

\param p_api_dev        Global variable container
\param eId              The version to read
\param pxVersionValue   The numeric version representation
\param pxVersionString  The string version representation

\return returns         0 on success or API_ERR_INVALID_MODE if version not found.
*/
L_WORD mil_hw_pci_driver_artix7_version_tcp(TY_API_DEV *p_api_dev, TY_E_VERSION_ID eId, TY_API_VER_INFO_VALUES * pxVersionValue, TY_API_VER_INFO_STRINGS * pxVersionString)
{
    AiUInt16 wRetVal   = 0;
    AiUInt32 version   = 0;

    wRetVal = mil_hw_artix7_internal_tcp_command(p_api_dev, ARTIX7_MON_CMD_MYMON_VER, 0, 0, 1, &version);

    if (wRetVal  == ARTIX7_MON_OK)
        wRetVal = ARTIX7_MON_ERR_STATUS;

    pxVersionValue->ul_MajorVer = BCD2BIN_B(EXTRACT_BITS(version, 24, 0xFF));
    pxVersionValue->ul_MinorVer = BCD2BIN_B(EXTRACT_BITS(version, 16, 0xFF));
    pxVersionValue->ul_PatchVersion = 0;
    pxVersionValue->ul_BuildNr = BCD2BIN_W(EXTRACT_BITS(version, 0, 0xFFFF));

    AI_TSW_OS_SPRINTF(pxVersionString->ac_Description, AI_DESCRIPTION_STRINGL, "TCP");
    AI_TSW_OS_SPRINTF(pxVersionString->ac_FullVersion, AI_VERSION_STRINGL, "%d.%d.%d",
        pxVersionValue->ul_MajorVer,
        pxVersionValue->ul_MinorVer,
        pxVersionValue->ul_BuildNr);

    return 0;
}


/*! \brief Read a specific MLCA version

\param p_api_dev        Global variable container
\param eId              The version to read
\param pxVersionValue   The numeric version representation
\param pxVersionString  The string version representation

\return returns         0 on success or API_ERR_INVALID_MODE if version not found.
*/
L_WORD mil_hw_pci_driver_artix7_version_mlca(TY_API_DEV *p_api_dev, TY_E_VERSION_ID eId, TY_API_VER_INFO_VALUES * pxVersionValue, TY_API_VER_INFO_STRINGS * pxVersionString)
{
    L_WORD  version = 0;

    version = p_api_dev->IoRAMBase[ARTIX7_REG_PL_VERSION];

    pxVersionValue->ul_MajorVer = BCD2BIN_B(EXTRACT_BITS(version, 24, 0xFF));
    pxVersionValue->ul_MinorVer = BCD2BIN_B(EXTRACT_BITS(version, 16, 0xFF));
    pxVersionValue->ul_PatchVersion = 0;
    pxVersionValue->ul_BuildNr = BCD2BIN_W(EXTRACT_BITS(version, 0, 0xFFFF));

    AI_TSW_OS_SPRINTF(pxVersionString->ac_Description, AI_DESCRIPTION_STRINGL, "PL");
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
L_WORD mil_hw_pci_driver_artix7_version_monitor(TY_API_DEV *p_api_dev, TY_E_VERSION_ID eId, TY_API_VER_INFO_VALUES * pxVersionValue, TY_API_VER_INFO_STRINGS * pxVersionString)
{
    L_WORD ret;
    
    ret = mil_hw_pci_driver_artix7_version_tcp(p_api_dev, eId, pxVersionValue, pxVersionString);

    if (ret == 0)
        // Monitor and TCP are implemented in the same file.
        AI_TSW_OS_SPRINTF(pxVersionString->ac_Description, AI_DESCRIPTION_STRINGL, "Monitor");

    return ret;
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
AiBoolean mil_hw_pci_driver_artix7_has_digital_loop(TY_API_DEV* p_api_dev)
{
    return AiTrue;
}

/*! \brief Configure digital trigger loop */
/*!
This function configures the digital trigger loop.

\param  p_api_dev      TY_API_DEV*             The global variable container.
\param  con            AiUInt32                Configuration value.

*/
AiReturn mil_hw_pci_driver_artix7_trigger_digital_loop_con(TY_API_DEV* p_api_dev, AiUInt32 con)
{
    p_api_dev->IoRAMBase[ARTIX7_REG_TRIGGER_DIGITAL_LOOP] = con;
    return 0;
}

/*! \brief Read digital trigger loop Configuration */
/*!
This function reads the digital trigger loop configures.

\param  p_api_dev      TY_API_DEV*             The global variable container.
\param  con            AiUInt32*               Configuration value.

*/
AiReturn mil_hw_pci_driver_artix7_trigger_digital_loop_get(TY_API_DEV* p_api_dev, AiUInt32 * con)
{
    *con = p_api_dev->IoRAMBase[ARTIX7_REG_TRIGGER_DIGITAL_LOOP];
    return 0;
}


#endif /* defined(_AIM_SYSDRV) */
