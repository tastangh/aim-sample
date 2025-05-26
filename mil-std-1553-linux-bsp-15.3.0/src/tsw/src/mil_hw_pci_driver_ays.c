/* SPDX-FileCopyrightText: 2017-2024 AIM GmbH <info@aim-online.com> */
/* SPDX-License-Identifier: MIT OR GPL-2.0-or-later */

/*! \file mil_hw_pci_driver_ays.c
    This file contains hw layer implementations for Ays without ASP.

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
#include "hw/AiHwAys.h"

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
AiUInt32 mil_hw_pci_driver_ays_novram_write(TY_API_DEV *p_api_dev, AiUInt32 ulOffset, AiUInt32 ulValue)
{
    AiUInt32 ul_BytesWritten;
    AiUInt32 ul_ReVal;

    ul_ReVal = ai_tsw_os_novram_write(p_api_dev->p_Device, ulOffset, &ulValue, sizeof(AiUInt32), &ul_BytesWritten);

    return ul_ReVal;
}


/*! \brief Read from the given NOVRAM offset. */
/*!

\param  p_api_dev          TY_API_DEV*      The global variable container.
\param  ulOffset           AiUInt32         The NOVRAM offset
\param  pulValue           AiUInt32*        The NOVRAM value

\return L_WORD The return value is 0 on success or non zero on failure.
*/
AiUInt32 mil_hw_pci_driver_ays_novram_read(TY_API_DEV *p_api_dev, AiUInt32 ulOffset, AiUInt32 *pulValue)
{
    AiUInt32 ul_BytesRead;
    AiUInt32 ul_ReVal;

    if (!pulValue)
        return API_ERR_MYMON_ERROR;

    ul_ReVal = ai_tsw_os_novram_read(p_api_dev->p_Device, ulOffset, pulValue, sizeof(AiUInt32), &ul_BytesRead);

    return ul_ReVal;
}


/*! \brief Flush the NOVRAM mirror. */
/*!

\param  p_api_dev          TY_API_DEV*      The global variable container.
\return L_WORD The return value is 0 on success or non zero on failure.
*/
AiUInt32 mil_hw_pci_driver_ays_novram_finalize(TY_API_DEV *p_api_dev)
{
    AiUInt32 ul_ReVal;

    ul_ReVal = ai_tsw_os_novram_finalize(p_api_dev->p_Device);

    return ul_ReVal;
}








/*****************************************************************************
 *                                                                           *
 *                                                                           *
 *                 HW Layer public TSW IRIG access                           *
 *                                                                           *
 *                                                                           *
 *****************************************************************************/
static AiInt16 mil_hw_internal_tcp_command(TY_API_DEV *  p_api_dev,
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
       p_api_dev->IoRAMBase[AYS_REG_TCP_INPUT_0 + i] = ulInput[i];

    /* execute command */
    ulCommand = ulCommand << 24;

    p_api_dev->IoRAMBase[AYS_REG_TCP_CONTROL] = ulCommand;

    ai_tsw_os_wait_usec(100);

    /* wait until command is finished */

    for (i = 0; i<10000; i++)
    {
        ulValue = p_api_dev->IoRAMBase[AYS_REG_TCP_STATUS];

        if (((ulValue & MYMON_STATUS_MASK) == MYMON_STATUS_FREE)
            && ((ulValue & MYMON_STATUS_CMD) == ulCommand))
            /* finished */
            break;

        ai_tsw_os_wait_usec(100);
    }

    if (i == 10000)
        /* timeout occured */
        return MYMON_ERR_TIMEOUT;

    /* copy return data */

    if (ulOutputCount > 0)
       for (i = 0; i<ulOutputCount; i++)
          ulOutput[i] = p_api_dev->IoRAMBase[AYS_REG_TCP_OUTPUT_0 + i];
    
    /* -- execute command 0 to reset the status --- */
    p_api_dev->IoRAMBase[AYS_REG_TCP_CONTROL] = ulCommandZero;

    return MYMON_OK;
}



/*! \brief This function sets the hw irig time source the TCP. */
/*!
    This function handles the 'API_EXEC_SYS' instruction 1

    \param  p_api_dev  TY_API_DEV*      The global variable container.
    \param  con        BYTE             System Command control
    
    \return ackfl      BYTE             Instruction acknowledge type.
*/
L_WORD mil_hw_pci_driver_ays_irig_set_external(TY_API_DEV *p_api_dev, BYTE con)
{
    L_WORD value;

    if (con & 0x01)
    {
        value = p_api_dev->IoRAMBase[AYS_REG_TIME_SOURCE_CONTROL];
        value |= 0x00000100L; /* Bit 8=1 is irig external*/
        p_api_dev->IoRAMBase[AYS_REG_TIME_SOURCE_CONTROL] = value;
    }
    else
    {
        value = p_api_dev->IoRAMBase[AYS_REG_TIME_SOURCE_CONTROL];
        value &= 0xFFFFFEFFL; /* Bit 8=1 is irig external*/
        p_api_dev->IoRAMBase[AYS_REG_TIME_SOURCE_CONTROL] = value;
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
    
    \return ackfl      BYTE             Instruction acknowledge type.
*/
L_WORD mil_hw_pci_driver_ays_irig_set( TY_API_DEV * p_api_dev, 
                        L_WORD         day, 
                        L_WORD         hour, 
                        L_WORD         min, 
                        L_WORD         sec )
{
    L_WORD   ret_val  = 0;
    WORD   wRetVal = 0;
    AiUInt32 ulInput[3];

     /* -- set irig time if commanded --- */
     do
     {
         ulInput[0] = 0;     /* time                */
         ulInput[1] = 0;     /* IEEE1344 (reserved) */
         ulInput[2] = 0;     /* update + leap Year  */

         if (sec  >  59){ wRetVal = MYMON_ERR_RANGE_CHECK; break; }
         if (min  >  59){ wRetVal = MYMON_ERR_RANGE_CHECK; break; }
         if (hour >  23){ wRetVal = MYMON_ERR_RANGE_CHECK; break; }
         if (day  > 366){ wRetVal = MYMON_ERR_RANGE_CHECK; break; }

         ulInput[0] |= sec;
         ulInput[0] |= min << 6;
         ulInput[0] |= hour << 12;
         ulInput[0] |= day << 17;

         ulInput[2] = 1ul << 31;

         wRetVal = mil_hw_internal_tcp_command(p_api_dev, MYMON_CMD_IRIG_SET, 3, ulInput, 0, 0);

     } while (0);


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
L_WORD mil_hw_pci_driver_ays_irig_get_status(TY_API_DEV *p_api_dev, AiUInt32 * irig_sync, AiUInt32 * irig_src)
{
    L_WORD irig_status = 0;

    irig_status = p_api_dev->IoRAMBase[AYS_REG_TIME_SOURCE_CONTROL];
    *irig_src    = (irig_status >> 8) & 0x1;

    if(*irig_src == 0) /* intern IRIG is set */
       *irig_src  = API_IRIG_ONBOARD;
    else
    {
        *irig_src = API_IRIG_EXTERN;
    }

    irig_status = p_api_dev->IoRAMBase[AYS_REG_TT_STATUS];
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
L_WORD mil_hw_pci_driver_ays_irig_get_time(TY_API_DEV * p_api_dev,
                           L_WORD     * sth, 
                           L_WORD     * stl )
{
    *sth = p_api_dev->IoRAMBase[AYS_REG_TT_HIGH];
    *stl = p_api_dev->IoRAMBase[AYS_REG_TT_LOW];

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
AiReturn mil_hw_pci_driver_ays_discretes_init(TY_API_DEV *p_api_dev, AiUInt32 new_config)
{
    L_WORD current_config;
    current_config = BSWAP32(p_api_dev->IoRAMBase[AYS_REG_GPIO_CONFIG]);

    current_config &= ~0xFF; /* Clear bits 0-7 */
    new_config &= 0xFF;  /* Only bits 0-7 are valid */
    current_config |= new_config;

    p_api_dev->IoRAMBase[AYS_REG_GPIO_CONFIG] = BSWAP32(current_config);

    return 0;
}

/*! \brief Write discrete output values */
/*!

    \param  p_api_dev    TY_API_DEV*      The global variable container.
    \param  Mask         AiUInt32         Output mask
    \param  Value        AiUInt32         Output value

    \return AiReturn The return value is 0 on success or non zero on failure.
*/
AiReturn mil_hw_pci_driver_ays_discretes_write(TY_API_DEV *p_api_dev, AiUInt32 Mask, AiUInt32 Value)
{
    L_WORD ul_Tmp;

    Mask  &= 0x000000ff; /* only the lowest 8 bit are valid */
    Value &= 0x000000ff; /* only the lowest 8 bit are valid */

    ul_Tmp = p_api_dev->IoRAMBase[AYS_REG_GPIO_OUTPUT];
    ul_Tmp &= (Value | ~Mask); /* Set all related bits to low  */
    ul_Tmp |= (Value &  Mask); /* Set all related bits to high */
    p_api_dev->IoRAMBase[AYS_REG_GPIO_OUTPUT] = ul_Tmp;

    return 0;
}


/*! \brief Read discrete input values */
/*!

    \param  p_api_dev    TY_API_DEV*      The global variable container.
    \param  Value        AiUInt32         input value

    \return AiReturn The return value is 0 on success or non zero on failure.
*/
AiReturn mil_hw_pci_driver_ays_discretes_read(TY_API_DEV *p_api_dev, L_WORD * Value)
{
    *Value = p_api_dev->IoRAMBase[AYS_REG_GPIO_INPUT];

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

    \return AiReturn The return value is 0 on success or non zero on failure.
*/
AiReturn mil_hw_pci_driver_ays_get_discretes(TY_API_DEV * p_api_dev, AiUInt32 * pulDiscreteCount, AiUInt32 * pulDiscreteConfig)
{
    switch (p_api_dev->BoardConfig & 0x000000FF)
    {
        case AI_PLATFORM_MINI_PCIE_CARD:
            *pulDiscreteCount  = 4;
            *pulDiscreteConfig = 0;
            break;
        case AI_PLATFORM_MINI_PCIE_CARD_AP:
            *pulDiscreteCount  = 6; 
            *pulDiscreteConfig = 0x3C;
            break;
        default:
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
AiReturn mil_hw_pci_driver_ays_discretes_info_read(TY_API_DEV * p_api_dev, AiUInt32 *pul_Channel, AiUInt32 *pul_Canin, AiUInt32 *pul_Canout)
{
    switch (p_api_dev->BoardConfig & 0x000000FF)
    {
    case AI_PLATFORM_MINI_PCIE_CARD:
        *pul_Channel = 4;
        *pul_Canin   = 0x3;
        *pul_Canout  = 0xC;
        break;
    case AI_PLATFORM_MINI_PCIE_CARD_AP:
        *pul_Channel = 6;
        *pul_Canin   = 0x3F;
        *pul_Canout  = 0x3C;
        break;
    default:
        break;
    }
    return 0;
}


/*! \brief Returns the type of the firmware

\param p_api_dev pointer to target administration structure

\return returns The type of the FW 
*/
enum MIL_FW mil_hw_pci_driver_ays_get_fw_type(TY_API_DEV * p_api_dev, short ulBiu)
{
    if ((p_api_dev->BoardApplicationType & 0x10) != 0)
        return MIL_FW_EBD_CORTEXA9; // AME1553-E/AP
    else
        return MIL_FW_STD_CORTEXA9; // AME1553
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
L_WORD mil_hw_pci_driver_ays_version_fw(TY_API_DEV *p_api_dev, TY_E_VERSION_ID eId, TY_API_VER_INFO_VALUES * pxVersionValue, TY_API_VER_INFO_STRINGS * pxVersionString)
{
    L_WORD retval = 0;
    TY_VER_INFO x_Version;

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
        retval = ai_tsw_os_version_read(p_api_dev->p_Device, eId, &x_Version);
        if (retval == 0)
        {
            pxVersionValue->ul_VersionType = x_Version.ul_VersionType;
            pxVersionValue->ul_MajorVer = x_Version.ul_MajorVer;
            pxVersionValue->ul_MinorVer = x_Version.ul_MinorVer;
            pxVersionValue->ul_PatchVersion = x_Version.ul_PatchVersion;
            pxVersionValue->ul_BuildNr = x_Version.ul_BuildNr;

            AI_TSW_OS_SPRINTF(pxVersionString->ac_Description, AI_DESCRIPTION_STRINGL, "%s", x_Version.ac_Description);
            AI_TSW_OS_SPRINTF(pxVersionString->ac_FullVersion, AI_VERSION_STRINGL, "%s", x_Version.ac_FullVersion);
        }
        else
        {
            retval = API_ERR_INVALID_MODE;
        }
    }
    else{
        retval = API_ERR_INVALID_MODE;
    }
    return retval;
}


/*! \brief Read a specific TCP version

\param p_api_dev        Global variable container
\param eId              The version to read
\param pxVersionValue   The numeric version representation
\param pxVersionString  The string version representation

\return returns         0 on success or API_ERR_INVALID_MODE if version not found.
*/
L_WORD mil_hw_pci_driver_ays_version_tcp(TY_API_DEV *p_api_dev, TY_E_VERSION_ID eId, TY_API_VER_INFO_VALUES * pxVersionValue, TY_API_VER_INFO_STRINGS * pxVersionString)
{
    L_WORD  version = 0;

    version = p_api_dev->IoRAMBase[AYS_REG_TCP_SW_VERSION];
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
L_WORD mil_hw_pci_driver_ays_version_mlca(TY_API_DEV *p_api_dev, TY_E_VERSION_ID eId, TY_API_VER_INFO_VALUES * pxVersionValue, TY_API_VER_INFO_STRINGS * pxVersionString)
{
    L_WORD retval = 0;
    TY_VER_INFO x_Version;


    retval = ai_tsw_os_version_read(p_api_dev->p_Device, eId, &x_Version);
    if (retval == 0)
    {
        pxVersionValue->ul_VersionType = x_Version.ul_VersionType;
        pxVersionValue->ul_MajorVer = x_Version.ul_MajorVer;
        pxVersionValue->ul_MinorVer = x_Version.ul_MinorVer;
        pxVersionValue->ul_PatchVersion = x_Version.ul_PatchVersion;
        pxVersionValue->ul_BuildNr = x_Version.ul_BuildNr;

        AI_TSW_OS_SPRINTF(pxVersionString->ac_Description, AI_DESCRIPTION_STRINGL, "%s", x_Version.ac_Description);
        AI_TSW_OS_SPRINTF(pxVersionString->ac_FullVersion, AI_VERSION_STRINGL, "%s", x_Version.ac_FullVersion);
    }
    else
    {
        retval = API_ERR_INVALID_MODE;
    }

    return retval;
}


/*! \brief Read a specific FPGA version

\param p_api_dev        Global variable container
\param eId              The version to read
\param pxVersionValue   The numeric version representation
\param pxVersionString  The string version representation

\return returns         0 on success or API_ERR_INVALID_MODE if version not found.
*/
L_WORD mil_hw_pci_driver_ays_version_fpga(TY_API_DEV *p_api_dev, TY_E_VERSION_ID eId, TY_API_VER_INFO_VALUES * pxVersionValue, TY_API_VER_INFO_STRINGS * pxVersionString)
{
    L_WORD retval = 0;
    TY_VER_INFO x_Version;


    retval = ai_tsw_os_version_read(p_api_dev->p_Device, eId, &x_Version);

    if (retval == 0)
    {
        pxVersionValue->ul_VersionType = x_Version.ul_VersionType;
        pxVersionValue->ul_MajorVer = x_Version.ul_MajorVer;
        pxVersionValue->ul_MinorVer = x_Version.ul_MinorVer;
        pxVersionValue->ul_PatchVersion = x_Version.ul_PatchVersion;
        pxVersionValue->ul_BuildNr = x_Version.ul_BuildNr;

        AI_TSW_OS_SPRINTF(pxVersionString->ac_Description, AI_DESCRIPTION_STRINGL, "%s", x_Version.ac_Description);
        AI_TSW_OS_SPRINTF(pxVersionString->ac_FullVersion, AI_VERSION_STRINGL, "%s", x_Version.ac_FullVersion);
    }
    else
    {
        retval = API_ERR_INVALID_MODE;
    }

    return retval;
}


/*! \brief Read a specific OS version

\param p_api_dev        Global variable container
\param eId              The version to read
\param pxVersionValue   The numeric version representation
\param pxVersionString  The string version representation

\return returns         0 on success or API_ERR_INVALID_MODE if version not found.
*/
L_WORD mil_hw_pci_driver_ays_version_os(TY_API_DEV *p_api_dev, TY_E_VERSION_ID eId, TY_API_VER_INFO_VALUES * pxVersionValue, TY_API_VER_INFO_STRINGS * pxVersionString)
{
    L_WORD retval = 0;
    TY_VER_INFO x_Version;

    retval = ai_tsw_os_version_read(p_api_dev->p_Device, eId, &x_Version);
    if (retval == 0)
    {
        pxVersionValue->ul_VersionType = x_Version.ul_VersionType;
        pxVersionValue->ul_MajorVer = x_Version.ul_MajorVer;
        pxVersionValue->ul_MinorVer = x_Version.ul_MinorVer;
        pxVersionValue->ul_PatchVersion = x_Version.ul_PatchVersion;
        pxVersionValue->ul_BuildNr = x_Version.ul_BuildNr;

        AI_TSW_OS_SPRINTF(pxVersionString->ac_Description, AI_DESCRIPTION_STRINGL, "%s", x_Version.ac_Description);
        AI_TSW_OS_SPRINTF(pxVersionString->ac_FullVersion, AI_VERSION_STRINGL, "%s", x_Version.ac_FullVersion);
    }
    else
    {
        retval = API_ERR_INVALID_MODE;
    }

    return retval;
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
AiBoolean mil_hw_pci_driver_ays_has_digital_loop(TY_API_DEV* p_api_dev)
{
    return AiTrue;
}

/*! \brief Configure digital trigger loop */
/*!
This function configures the digital trigger loop.

\param  p_api_dev      TY_API_DEV*             The global variable container.
\param  con            AiUInt32                Configuration value.

*/
AiReturn mil_hw_pci_driver_ays_trigger_digital_loop_con(TY_API_DEV* p_api_dev, AiUInt32 con)
{
    p_api_dev->IoRAMBase[AYS_REG_TRIGGER_DIGITAL_LOOP] = con;
    return 0;
}

/*! \brief Read digital trigger loop Configuration */
/*!
This function reads the digital trigger loop configures.

\param  p_api_dev      TY_API_DEV*             The global variable container.
\param  con            AiUInt32*               Configuration value.

*/
AiReturn mil_hw_pci_driver_ays_trigger_digital_loop_get(TY_API_DEV* p_api_dev, AiUInt32 * con)
{
    *con = p_api_dev->IoRAMBase[AYS_REG_TRIGGER_DIGITAL_LOOP];
    return 0;
}


#endif /* defined(_AIM_SYSDRV) */
