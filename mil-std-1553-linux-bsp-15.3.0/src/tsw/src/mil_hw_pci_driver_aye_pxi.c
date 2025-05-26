/* SPDX-FileCopyrightText: 2018-2023 AIM GmbH <info@aim-online.com> */
/* SPDX-License-Identifier: MIT OR GPL-2.0-or-later */

/*! \file mil_hw_pci_driver_aye_pxi.c
    This file contains hw layer implementations for AyE without ASP (PXI part).

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
 *                              PXI Access                                   *
 *                                                                           *
 *                                                                           *
 *****************************************************************************/


typedef struct ty_api_pxi_mapping
{
    AiInt8  cOutputPBI2;
    AiInt8  cOutputPBI4;
    AiInt8  cInputPBI2;
    AiInt8  cInputPBI4;
    AiInt8  cInputRegPBI4;
} TY_API_PXI_MAPPING;


#define MAP_API_PXI_TRIGGER_TABLE_SIZE 21

static const TY_API_PXI_MAPPING map_api_pxi_trigger_to_hw_def[] = {
    /*        {cOutputPBI2, cOutputPBI4, cInputPBI2, cInputPBI4, cInputRegPBI4}*/
    /*PXI0 */ {         -1,          -1,         -1,         -1,             0 },
    /*PXI1 */ {         -1,          -1,         -1,         -1,             0 },
    /*PXI2 */ {         -1,          -1,         -1,         -1,             0 },
    /*PXI3 */ {         -1,          -1,         -1,         -1,             0 },
    /*PXI4 */ {         -1,          -1,         -1,         -1,             0 },
    /*PXI5 */ {         -1,          -1,         -1,         -1,             0 },
    /*PXI6 */ {         -1,          -1,         -1,         -1,             0 },
    /*PXI7 */ {         -1,          -1,         -1,         -1,             0 },
    /*BC1  */ {          4,           4,          4,          4,             1 },
    /*BC2  */ {          5,           5,          5,          5,             1 },
    /*BC3  */ {         -1,          12,         -1,          2,             2 },
    /*BC4  */ {         -1,          13,         -1,          3,             2 },
    /*RT1  */ {          2,           6,          2,          2,             1 },
    /*RT2  */ {          3,           7,          3,          3,             1 },
    /*RT3  */ {         -1,           8,         -1,          0,             2 },
    /*RT4  */ {         -1,           9,         -1,          1,             2 },
    /*BM1  */ {          0,           0,          0,          0,             1 },
    /*BM2  */ {          1,           1,          1,          1,             1 },
    /*BM3  */ {         -1,          10,         -1,          6,             1 },
    /*BM4  */ {         -1,          11,         -1,          7,             1 },
    /*BM HS*/ {         -1,          -1,         -1,         -1,             0 }
};

/*! \brief Check if PXI is supported on this device  */
/*!
    This function returns true if PXI is supported and false in any other case.
    
    \param  p_api_dev      TY_API_DEV*             The global variable container.
   
    \return                AiBoolean               True on PXI support
*/
AiBoolean mil_hw_pxi_is_supported( TY_API_DEV* p_api_dev )
{
    if( p_api_dev->ul_DeviceGroup != AI_DEVICE_AYE )
        return AiFalse;

    if( p_api_dev->HwVariant & (1<<8) )
        return AiTrue;
    else
        return AiFalse;
}


/*! \brief Disable all PXI input triggers  */
/*!
    This function disables all input triggers and resets the register to the default values.
    
    \param  p_api_dev      TY_API_DEV*             The global variable container.
   
    \return                AiReturn                Zero on success
*/
AiReturn  mil_hw_pxi_trigger_in_disable( TY_API_DEV* p_api_dev )
{
    AiUInt32 * aulIo = p_api_dev->IoRAMBase;

    if( !mil_hw_pxi_is_supported(p_api_dev) )
        return API_ERR_PLATTFORM_NOT_SUPPORTED;


    aulIo[ AYE_REG_PXI_TRIGGER_IN1_SELECT ] = BSWAP32(0x00000000); /* Clear all Bits to default settings in PXI Trigger Input Select Register 1 */
    aulIo[ AYE_REG_PXI_TRIGGER_IN2_SELECT ] = BSWAP32(0x00000000); /* Clear all Bits to default settings in PXI Trigger Input Select Register 2 */
    return 0;
}


/*! \brief Disable all PXI output triggers  */
/*!
    This function disables all output triggers and resets the register to the default values.
    
    \param  p_api_dev      TY_API_DEV*             The global variable container.
   
    \return                AiReturn                Zero on success
*/
AiReturn mil_hw_pxi_trigger_out_disable( TY_API_DEV* p_api_dev )
{
    AiUInt32 * aulIo = p_api_dev->IoRAMBase;

    if( !mil_hw_pxi_is_supported(p_api_dev) )
        return API_ERR_PLATTFORM_NOT_SUPPORTED;

    aulIo[ AYE_REG_PXI_TRIGGER_OUT_SELECT ] = BSWAP32(0x00);
    aulIo[ AYE_REG_PXI_TRIGGER_OUT_ENABLE ] = BSWAP32(0xFF);

    return 0;
}



/*! \brief Control the PXI irig source */
/*!
    This function enables, disables or clears the PXI irig signal.
    
    \param  p_api_dev      TY_API_DEV*             The global variable container.
    \param  TTAGSrc        AiUInt32                Enable PXI irig if value is 1
    \param  TTAGClrEn       AiUInt32               Clear PXI Irig if value is 1
   
    \return                AiReturn                Zero on success
*/
AiReturn  mil_hw_pxi_timetag_src_control( TY_API_DEV* p_api_dev, AiUInt32 TTAGSrc, AiUInt32 TTClrEn)
{
    /* PXI_TTAGSrc_Ctrl(UINT32 TTAGSrc, UINT32 TTClrEn) */
    AiUInt32 * aulIo  = p_api_dev->IoRAMBase;
    AiUInt32   TTCtrl = 0;

    if( !mil_hw_pxi_is_supported(p_api_dev) )
        return API_ERR_PLATTFORM_NOT_SUPPORTED;

    if (TTAGSrc==1)
        TTCtrl = TTCtrl | 0x00000001;

    if (TTClrEn==1)
        TTCtrl = TTCtrl | 0x00000002;

    aulIo[AYE_REG_PXI_IRIG_SOURCE_CONTROL] = BSWAP32(TTCtrl);

    return 0;
}





/*! \brief Connect a PBI trigger line with PXI output trigger line */
/*!
   
    \param  p_api_dev      TY_API_DEV*             The global variable container.
    \param  TrgInLine      AiUInt8                 The PBI trigger line
    \param  PXITrgLine     AiUInt8                 The PXI trigger output line
   
    \return                AiReturn                Zero on success
*/
AiReturn  mil_hw_pxi_trigger_in_select( TY_API_DEV* p_api_dev, AiUInt8 TrgInLine, AiUInt32 PXITrgLine )
{
    /* PXI_TrgIn_Sel(UINT8 PBIType, UINT8 TrgInLine, UINT32 PXITrgLine) */
    AiUInt32 * aulIo  = p_api_dev->IoRAMBase;
    AiUInt32 OEReg    = 0;
    AiInt32  LineNr   = 0;
    AiUInt32 PXITrgInSelReg1,PXITrgInSelReg2;


    if( !mil_hw_pxi_is_supported(p_api_dev) )
        return API_ERR_PLATTFORM_NOT_SUPPORTED;

    if( TrgInLine > MAP_API_PXI_TRIGGER_TABLE_SIZE )
        return API_ERR_VALUE_NOT_IN_RANGE;


    PXITrgInSelReg1 = BSWAP32(aulIo[AYE_REG_PXI_TRIGGER_IN1_SELECT]);
    PXITrgInSelReg2 = BSWAP32(aulIo[AYE_REG_PXI_TRIGGER_IN2_SELECT]);

    if( p_api_dev->uc_DeviceType !=  TYPE_APE1553_4 )
    {
        /* ------------------------------ 1553-1(DS)/-2(DS) PBI ----------------------------------- */
        LineNr = map_api_pxi_trigger_to_hw_def[TrgInLine].cInputPBI2;

        if( LineNr < 0 )
            return API_ERR_VALUE_NOT_IN_RANGE;

        PXITrgInSelReg1=(PXITrgInSelReg1 | (PXITrgLine=(PXITrgLine << (LineNr*4) ) ) );
    }
    else
    {
        /* ------------------------------ 1553-4 PBI ----------------------------------- */
        LineNr = map_api_pxi_trigger_to_hw_def[TrgInLine].cInputPBI4;

        if( LineNr < 0 )
            return API_ERR_VALUE_NOT_IN_RANGE;

        if( map_api_pxi_trigger_to_hw_def[TrgInLine].cInputRegPBI4 == 2 )
        {
            PXITrgInSelReg2=(PXITrgInSelReg2 | (PXITrgLine=(PXITrgLine << (LineNr*4) ) ) );
        }
        else
        {
            PXITrgInSelReg1=(PXITrgInSelReg1 | (PXITrgLine=(PXITrgLine << (LineNr*4) ) ) );
        }
    }

    // Determine which PXI Trigger Input Buffer must be enabled
    if(TrgInLine==API_TRG_BM_CHN1)  OEReg=(OEReg|0x00100000);
    if(TrgInLine==API_TRG_BM_CHN2)  OEReg=(OEReg|0x00200000);
    if(TrgInLine==API_TRG_RT_CHN1)  OEReg=(OEReg|0x00400000);
    if(TrgInLine==API_TRG_RT_CHN2)  OEReg=(OEReg|0x00800000);
    if(TrgInLine==API_TRG_BC_CHN1)  OEReg=(OEReg|0x01000000);
    if(TrgInLine==API_TRG_BC_CHN2)  OEReg=(OEReg|0x02000000);
    if(TrgInLine==API_TRG_BM_CHN3)  OEReg=(OEReg|0x04000000);
    if(TrgInLine==API_TRG_BM_CHN4)  OEReg=(OEReg|0x08000000);
    if(TrgInLine==API_TRG_RT_CHN3)  OEReg=(OEReg|0x10000000);
    if(TrgInLine==API_TRG_RT_CHN4)  OEReg=(OEReg|0x20000000);
    if(TrgInLine==API_TRG_BC_CHN3)  OEReg=(OEReg|0x40000000);
    if(TrgInLine==API_TRG_BC_CHN4)  OEReg=(OEReg|0x80000000);


    OEReg = OEReg & 0xFFF00000;
    PXITrgInSelReg2 = PXITrgInSelReg2 | OEReg; 

    /* Assign the PXI-Trigger Lines to the BIU-Triggers and enable the PXI Trigger Input Buffers */
    aulIo[AYE_REG_PXI_TRIGGER_IN1_SELECT] = BSWAP32(PXITrgInSelReg1);
    aulIo[AYE_REG_PXI_TRIGGER_IN2_SELECT] = BSWAP32(PXITrgInSelReg2);

    return 0;
}


/*! \brief Connect a PXI trigger line with PBI input trigger line */
/*!
    
    \param  p_api_dev      TY_API_DEV*             The global variable container.
    \param  TrgOutLine     AiUInt8                 The PBI trigger line
    \param  PXITrgLine     AiUInt8                 The PXI trigger input line
   
    \return                AiReturn                Zero on success
*/
AiReturn  mil_hw_pxi_trigger_out_select( TY_API_DEV* p_api_dev, AiUInt8 TrgOutLine, AiUInt8 PXITrgLine )
{
    /* PXI_TrgOut_Sel(UINT8 PBIType, UINT8 TrgOutLine, UINT8 PXITrgLine) */
    AiUInt32 * aulIo  = p_api_dev->IoRAMBase;
    AiUInt32 OEReg;
    AiUInt8  LineNr = 0;
    AiUInt32 PXITrgOutSelReg, PXITrgOutEnaReg;


    if( !mil_hw_pxi_is_supported(p_api_dev) )
        return API_ERR_PLATTFORM_NOT_SUPPORTED;


    if( TrgOutLine > MAP_API_PXI_TRIGGER_TABLE_SIZE )
        return API_ERR_VALUE_NOT_IN_RANGE;


    PXITrgOutSelReg = BSWAP32(aulIo[AYE_REG_PXI_TRIGGER_OUT_SELECT]);
    PXITrgOutEnaReg = BSWAP32(aulIo[AYE_REG_PXI_TRIGGER_OUT_ENABLE]);


    if( p_api_dev->uc_DeviceType ==  TYPE_APE1553_4 )
    {
        /* -------- If 1553-4 PBI -------- */
        LineNr = map_api_pxi_trigger_to_hw_def[TrgOutLine].cOutputPBI4;
    }
    else
    {
        /* -------- If 1553-2/-1 PBI -------- */
        LineNr = map_api_pxi_trigger_to_hw_def[TrgOutLine].cOutputPBI2;
    }

    //if( LineNr < 0 )
        //return API_ERR_VALUE_NOT_IN_RANGE;

    PXITrgOutSelReg=(PXITrgOutSelReg | (LineNr << (PXITrgLine*4) ));


    // Determine which PXI Trigger Input Buffer must be enabled
    OEReg=0xFF;
    if(PXITrgLine==API_TRG_PXI0)   OEReg=(OEReg & 0xFE);
    if(PXITrgLine==API_TRG_PXI1)   OEReg=(OEReg & 0xFD);
    if(PXITrgLine==API_TRG_PXI2)   OEReg=(OEReg & 0xFB);
    if(PXITrgLine==API_TRG_PXI3)   OEReg=(OEReg & 0xF7);
    if(PXITrgLine==API_TRG_PXI4)   OEReg=(OEReg & 0xEF);
    if(PXITrgLine==API_TRG_PXI5)   OEReg=(OEReg & 0xDF);
    if(PXITrgLine==API_TRG_PXI6)   OEReg=(OEReg & 0xBF);
    if(PXITrgLine==API_TRG_PXI7)   OEReg=(OEReg & 0x7F);

    OEReg = OEReg & 0x000000FF;
    PXITrgOutEnaReg = (PXITrgOutEnaReg & OEReg); 


    aulIo[AYE_REG_PXI_TRIGGER_OUT_SELECT] = BSWAP32(PXITrgOutSelReg); /* Assign the PXI-Trigger Lines to the BIU-Triggers */
    aulIo[AYE_REG_PXI_TRIGGER_OUT_ENABLE] = BSWAP32(PXITrgOutEnaReg); /* Set PXI Trigger Output Enable Register */

    return 0;
}

/*! \brief Get PXI Geographic Address  */
/*!
This function returns the PXI Geographic Address.

\param  p_api_dev      TY_API_DEV*             The global variable container.
\param  geo_addr       AiUInt32*               The PXI Geographic Address

\return                AiReturn                Zero on success
*/
AiReturn mil_hw_pxi_geo_addr(TY_API_DEV* p_api_dev, AiUInt32 *pxiGeographicalAddress)
{

    if (!mil_hw_pxi_is_supported(p_api_dev))
        return API_ERR_PLATTFORM_NOT_SUPPORTED;

    *pxiGeographicalAddress = p_api_dev->IoRAMBase[AYE_REG_PXI_GEOGRAPHICAL_ADDR] & AYE_REG_PXI_GEOGRAPHICAL_ADDR_MASK;

    return API_OK;
}

/*****************************************************************************
*                                                                           *
*                                                                           *
*                      HW Layer public TSW Trigger                          *
*                                                                           *
*                                                                           *
*****************************************************************************/

/*! \brief Check if external trigger is available on this device  */
/*!
This function returns true if external trigger signal is supported and false in any other case.

\param  p_api_dev      TY_API_DEV*             The global variable container.

\return                AiBoolean               True on trigger support
*/
AiBoolean mil_hw_trigger_has_external_input(TY_API_DEV* p_api_dev)
{
    return AiTrue;
}



/*! \brief Configure the trigger input edge for this biu */
/*!

\param  p_api_dev      TY_API_DEV*             The global variable container.
\param  logical_biu    AiUInt32                The logical biu
\param  edge_flags     AiUInt32                The trigger edges

\return                AiReturn                Zero on success
*/
AiReturn  mil_hw_trigger_edge_input_set(TY_API_DEV* p_api_dev, AiUInt32 logical_biu, AiUInt32 edge_flags)
{
    AiUInt32 all_edge_flags;

    if (p_api_dev->ul_DeviceGroup != AI_DEVICE_AYE)
        return API_ERR_PLATTFORM_NOT_SUPPORTED;

    if ((edge_flags & ~(AYE_REG_TRIGGER_EDGE_CONTROL_MASK)) != 0)
        return API_ERR_PARAMETER_OUT_OF_RANGE;

    /* Get all bius */
    all_edge_flags = p_api_dev->IoRAMBase[AYE_REG_TRIGGER_EDGE_CONTROL];
    /* clear current biu */
    all_edge_flags &= ~(AYE_REG_TRIGGER_EDGE_CONTROL_MASK<<(logical_biu*AYE_REG_TRIGGER_EDGE_CONTROL_BITS));
    /* mask new values */
    all_edge_flags |= (edge_flags<<(logical_biu*AYE_REG_TRIGGER_EDGE_CONTROL_BITS));
    /* Write back */
    p_api_dev->IoRAMBase[AYE_REG_TRIGGER_EDGE_CONTROL] = all_edge_flags;

    return API_OK;
}


/*! \brief Get the trigger input edge configuration for this biu */
/*!

\param  p_api_dev      TY_API_DEV*             The global variable container.
\param  logical_biu    AiUInt32                The logical biu
\param  edge_flags     AiUInt32*               The trigger edges output

\return                AiReturn                Zero on success
*/
AiReturn mil_hw_trigger_edge_input_get(TY_API_DEV* p_api_dev, AiUInt32 logical_biu, AiUInt32 * edge_flags)
{
    AiUInt32 all_edge_flags;

    if (p_api_dev->ul_DeviceGroup != AI_DEVICE_AYE)
        return API_ERR_PLATTFORM_NOT_SUPPORTED;

    all_edge_flags = p_api_dev->IoRAMBase[AYE_REG_TRIGGER_EDGE_CONTROL];

    *edge_flags = (all_edge_flags>>(logical_biu*AYE_REG_TRIGGER_EDGE_CONTROL_BITS)) & AYE_REG_TRIGGER_EDGE_CONTROL_MASK;

    return API_OK;
}


#endif /* defined(_AIM_SYSDRV) */
