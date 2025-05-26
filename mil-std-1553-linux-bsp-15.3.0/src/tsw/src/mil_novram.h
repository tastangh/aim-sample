/* SPDX-FileCopyrightText: 2021-2022 AIM GmbH <info@aim-online.com> */
/* SPDX-License-Identifier: MIT OR GPL-2.0-or-later */

/*! \file mil_novram.h 

          This file contains helper functions to deal with NOVRAM values.
*/
/*********************************************************************************/



#ifndef __MIL_NOVRAM_H__
#define __MIL_NOVRAM_H__





#include "Ai_types.h"

/* Prototypes */
AiBoolean mil_novram_interpret_subtype_amplitude_high_res(AiUInt32 ulBoardSubType);
AiBoolean mil_novram_interpret_subtype_amplitude_packed(AiUInt32 ulBoardSubType);
AiBoolean mil_novram_interpret_subtype_amplitude_variable( AiUInt32 ulBoardSubType );


/*****************************************************************************
 *                                                                           *
 *                                                                           *
 *                 NOVRAM Board Sub Type Interpretation                      *
 *                                                                           *
 *                                                                           *
 *****************************************************************************/


/*! \brief This function interprets the board sub type value and returns true if the card supports a variable amplitude */
/*!
    This function interprets the board sub type value and returns true if the card supports a variable amplitude
    
    \param  ulBoardSubType AiUInt32  The NOVRAM board sub type value
   
    \return AiBoolean True if the transceiver has a variable amplitude
*/
 AiBoolean mil_novram_interpret_subtype_amplitude_variable( AiUInt32 ulBoardSubType )
{
    AiBoolean ret;
    /* -- check for variable amplitude --- */

    switch (ulBoardSubType & 0xF)
    {
    case 0x1:                             /* HOLT transceiver HI1570PSIF, var. Amplitude    */
    case 0x2:                             /* CMAC FC1553721VSC transceiver, var. Amplitude  */
    case 0x4:                             /* CMAC FC1553721VSC transceiver var. Amplitude & MILScope V02.00 PBI (only for BIU1 available) */
    case 0x6:                             /* HOLT HI1590, var.Amplitude with Fine Tuning range/switch. */
    case 0x7:                             /* HOLT HI1590, var.Amplitude with Fine Tuning range/switch. & MILScope V03.xx PBI(only for BIU1 available)*/
    case 0x8:	                          /* HOLT HI1590, var. Amplitude common control Range (CMAC- Adapter on V02.10 PBI) */
    case 0x9:	                          /* HOLT HI1590, var. Amplitude common control Range (CMAC- Adapter & MILScope V02.10 PBI, only for BIU1 available) */
        ret = AiTrue;
        break;
    case 0x3:                             /* HOLT transceiver HI1573PCT, fixed Amplitude    */
    case 0x5:                             /* CMAC transceiver (FC1553395) fixed Amplitude   */
        ret = AiFalse;
        break;
    default:
        /* Better we are able to switch the amplitude
        even if the HW cant do it. Than the other way around
        */
        ret = AiTrue;
        break;
    }

    return ret;
}




 /*! \brief This function interprets the board sub type value and returns true if the card has a packed variable amplitude */
 /*!
 This function interprets the board sub type value and returns true if the card has a packed variable amplitude

 \param  ulBoardSubType AiUInt32  The NOVRAM board sub type value

 \return AiBoolean True if the transceiver has a packed variable amplitude
 */
AiBoolean mil_novram_interpret_subtype_amplitude_packed(AiUInt32 ulBoardSubType)
 {
    AiBoolean ret;
     /* Check Bit 0-3: Transceiver */
     /* -- check for packed variable amplitude --- */

     switch (ulBoardSubType & 0xF)
     {
     case 0x1:                             /* HOLT transceiver HI1570PSIF, var. Amplitude    */
     case 0x6:                             /* HOLT HI1590, var.Amplitude with Fine Tuning range/switch. */
     case 0x7:                             /* HOLT HI1590, var.Amplitude with Fine Tuning range/switch. & MILScope V03.xx PBI(only for BIU1 available)*/
     case 0x8:	                          /* HOLT HI1590, var. Amplitude common control Range (CMAC- Adapter on V02.10 PBI) */
     case 0x9:	                          /* HOLT HI1590, var. Amplitude common control Range (CMAC- Adapter & MILScope V02.10 PBI, only for BIU1 available) */
         ret = AiTrue;
         break;
     case 0x2:                             /* CMAC FC1553721VSC transceiver, var. Amplitude  */
     case 0x3:                             /* HOLT transceiver HI1573PCT, fixed Amplitude    */
     case 0x4:                             /* CMAC FC1553721VSC transceiver var. Amplitude & MILScope V02.00 PBI (only for BIU1 available) */
     case 0x5:                             /* CMAC transceiver (FC1553395) fixed Amplitude   */
     default:
         /* Better we are able to switch the amplitude
         even if the HW cant do it. Than the other way around
         */
         ret = AiFalse;
         break;
     }

     return ret;
 }

 /*! \brief This function interprets the board sub type value and returns true if the card has a high resolution amplitude setting */
 /*!
This function interprets the board sub type value and returns true if the card has a high resolution amplitude setting 

 \param  ulBoardSubType AiUInt32  The NOVRAM board sub type value

 \return AiBoolean True if the transceiver has a high resolution variable amplitude
 */
AiBoolean mil_novram_interpret_subtype_amplitude_high_res(AiUInt32 ulBoardSubType)
 {
    AiBoolean ret;
     /* Check Bit 0-3: Transceiver */
     /* -- check for high resolution variable amplitude --- */

    switch (ulBoardSubType & 0xF)
    {
    case 0x1:                             /* HOLT transceiver HI1570PSIF, var. Amplitude    */
    case 0x2:                             /* CMAC FC1553721VSC transceiver, var. Amplitude  */
    case 0x3:                             /* HOLT transceiver HI1573PCT, fixed Amplitude    */
    case 0x4:                             /* CMAC FC1553721VSC transceiver var. Amplitude & MILScope V02.00 PBI (only for BIU1 available) */
    case 0x5:                             /* CMAC transceiver (FC1553395) fixed Amplitude   */
    case 0x8:	                          /* HOLT HI1590, var. Amplitude common control Range (CMAC- Adapter on V02.10 PBI) */
    case 0x9:	                          /* HOLT HI1590, var. Amplitude common control Range (CMAC- Adapter & MILScope V02.10 PBI, only for BIU1 available) */
        ret = AiFalse;
        break;
    case 0x6:                             /* HOLT HI1590, var.Amplitude with Fine Tuning range/switch. */
    case 0x7:                             /* HOLT HI1590, var.Amplitude with Fine Tuning range/switch. & MILScope V03.xx PBI(only for BIU1 available)*/
        ret = AiTrue;
        break;
    default:
        ret = AiFalse;
        break;
    }

     return ret;
 }


/*****************************************************************************
 *                                                                           *
 *                                                                           *
 *             NOVRAM Frontend Capabilities Interpretation                   *
 *                                                                           *
 *                                                                           *
 *****************************************************************************/
#define MIL_NOVRAM_FRONTEND_CAPABILITIES_VA(x)       (((x) & (1<<0)) != 0)  /* Amplitude Variable   */
#define MIL_NOVRAM_FRONTEND_CAPABILITIES_PA(x)       (((x) & (1<<1)) != 0)  /* Amplitude Packed     */
#define MIL_NOVRAM_FRONTEND_CAPABILITIES_HR(x)       (((x) & (1<<2)) != 0)  /* Amplitude Variable High Resolution Range */

#define MIL_NOVRAM_FRONTEND_CAPABILITIES_MS(x)      (((x) & (1<<4)) != 0)  /* Milscope             */

#define MIL_NOVRAM_FRONTEND_CAPABILITIES_ISO(x)   (((x) & (1<<8)) != 0)  /* Coupling Isolated    */
#define MIL_NOVRAM_FRONTEND_CAPABILITIES_XFM(x)   (((x) & (1<<9)) != 0)  /* Coupling Transformer */
#define MIL_NOVRAM_FRONTEND_CAPABILITIES_DIR(x)   (((x) & (1<<10)) != 0) /* Coupling Direct      */
#define MIL_NOVRAM_FRONTEND_CAPABILITIES_NET(x)   (((x) & (1<<11)) != 0) /* Coupling Network     */
#define MIL_NOVRAM_FRONTEND_CAPABILITIES_PC(x)    (((x) & (1<<12)) != 0) /* Coupling Packed      */



/*****************************************************************************
 *                                                                           *
 *                                                                           *
 *                NOVRAM Hardware Variant Interpretation                     *
 *                                                                           *
 *                                                                           *
 *****************************************************************************/
#define MIL_NOVRAM_HW_VARIANT_IRIG_EXTERNALLY_ABSENT(x)        (((x) & (1<< 4)) != 0)
#define MIL_NOVRAM_HW_VARIANT_DISCRETES_EXTERNALLY_ABSENT(x)   (((x) & (9<<10)) != 0)
#define MIL_NOVRAM_HW_VARIANT_TRIGGER_EXTERNALLY_ABSENT(x)     (((x) & (1<<10)) != 0)



/*****************************************************************************
 *                                                                           *
 *                                                                           *
 *           NOVRAM Application Type Bitmask Interpretation                  *
 *                                                                           *
 *                                                                           *
 *****************************************************************************/
#define MIL_NOVRAM_APPLICATION_TYPE_BITMASK_SIM(x)           (((x) & (1<<0)) != 0)  /* Simulator Only       */
#define MIL_NOVRAM_APPLICATION_TYPE_BITMASK_SF(x)            (((x) & (1<<1)) != 0)  /* Single Function      */


#endif /* __MIL_NOVRAM_H__ */

/* end: mil_novram.h */
