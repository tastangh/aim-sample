/* SPDX-FileCopyrightText: 2016-2022 AIM GmbH <info@aim-online.com> */
/* SPDX-License-Identifier: MIT OR GPL-2.0-or-later */

/*! \file mil_fw.c
    This file defines FW specific limitations.
*/

#include "mil_tsw_includes.h"


#define STD_XSCALE_BITPOS           31
#define STD_AT91_BITPOS             30
#define STD_CORTEXA9_BITPOS         29
#define STD_CORTEXR5_BITPOS         28
#define STD_ARTIX7_BITPOS           27

#define EBD_AT91_BITPOS             26
#define EBD_CORTEXA9_BITPOS         25
#define EBD_ARTIX7_BITPOS           24

#define STG3838_XSCALE_BITPOS       23
#define STG3838_AT91_BITPOS         22
#define STG3838_CORTEXR5_BITPOS     21

#define MCH_XSCAL_BITPOS            20
#define MCH_AT91_BITPOS             19
#define MCH_EBD_AT91_BITPOS         18

#define MIL_FW_CREATE_LOOKUP_BITMASK( STD_XSCALE, STD_AT91, STD_CORTEXA9, STD_CORTEXR5, STD_ARTIX7,    EBD_AT91, EBD_CORTEXA9, EBD_ARTIX7,    STG3838_XSCALE, STG3838_AT91, STG3838_CORTEXR5,    MCH_XSCAL, MCH_AT91, MCH_EBD_AT91 ) \
                                    ( \
                                        (STD_XSCALE     << STD_XSCALE_BITPOS)     | \
                                        (STD_AT91       << STD_AT91_BITPOS)       | \
                                        (STD_CORTEXA9   << STD_CORTEXA9_BITPOS)   | \
                                        (STD_CORTEXR5   << STD_CORTEXR5_BITPOS)   | \
										(STD_ARTIX7     << STD_ARTIX7_BITPOS)     | \
                                        \
                                        (EBD_AT91       << EBD_AT91_BITPOS)       | \
                                        (EBD_CORTEXA9   << EBD_CORTEXA9_BITPOS)   | \
                                        (EBD_ARTIX7     << EBD_ARTIX7_BITPOS)     |  \
                                        \
                                        (STG3838_XSCALE   << STG3838_XSCALE_BITPOS)    | \
                                        (STG3838_AT91     << STG3838_AT91_BITPOS)      | \
                                        (STG3838_CORTEXR5 << STG3838_CORTEXR5_BITPOS)  | \
                                        \
                                        (MCH_XSCAL      << MCH_XSCAL_BITPOS)      | \
                                        (MCH_AT91       << MCH_AT91_BITPOS)       | \
                                        (MCH_EBD_AT91   << MCH_EBD_AT91_BITPOS)     \
                                    )


#define MIL_FW_GET_STD_XSCALE( x )      ( ((x>>STD_XSCALE_BITPOS)&1)==1 )
#define MIL_FW_GET_STD_AT91( x )        ( ((x>>STD_AT91_BITPOS)&1)==1 )
#define MIL_FW_GET_STD_CORTEXA9( x )    ( ((x>>STD_CORTEXA9_BITPOS)&1)==1 )
#define MIL_FW_GET_STD_CORTEXR5( x )    ( ((x>>STD_CORTEXR5_BITPOS)&1)==1 )
#define MIL_FW_GET_STD_ARTIX7( x )      ( ((x>>STD_ARTIX7_BITPOS)&1)==1 )

#define MIL_FW_GET_EBD_AT91( x )        ( ((x>>EBD_AT91_BITPOS)&1)==1 )
#define MIL_FW_GET_EBD_CORTEXA9( x )    ( ((x>>EBD_CORTEXA9_BITPOS)&1)==1 )
#define MIL_FW_GET_EBD_ARTIX7( x )      ( ((x>>EBD_ARTIX7_BITPOS)&1)==1 )

#define MIL_FW_GET_STG3838_XSCALE( x )    ( ((x>>STG3838_XSCALE_BITPOS)&1)==1 )
#define MIL_FW_GET_STG3838_AT91( x )      ( ((x>>STG3838_AT91_BITPOS)&1)==1 )
#define MIL_FW_GET_STG3838_CORTEXR5( x )  ( ((x>>STG3838_CORTEXR5_BITPOS)&1)==1 )

#define MIL_FW_GET_MCH_XSCALE( x )      ( ((x>>MCH_XSCAL_BITPOS)&1)==1 )
#define MIL_FW_GET_MCH_AT91( x )        ( ((x>>MCH_AT91_BITPOS)&1)==1 )
#define MIL_FW_GET_MCH_EBD_AT91( x )    ( ((x>>MCH_EBD_AT91_BITPOS)&1)==1 )


/*! FW command support lookup table */
/* See: LS-FW-Function_OV_V0XY.xlsx for details */
static AiUInt32 mil_fw_cmd_lookup[MIL_FW_CMD_END_OF_TABLE+1];

void mil_fw_initialize_lookup()
{
    /*mil_fw_cmd_lookup[MIL_FW_CMD_RT_ACTIVATE_SYNC_MC]           = MIL_FW_CREATE_LOOKUP_BITMASK(STD_XSCALE,
     *                                                                                           |  STD_AT91,
     *                                                                                           |  |  STD_CORTEXA9,
     *                                                                                           |  |  |  STD_CORTEXR5,
     *                                                                                           |  |  |  |  STD_ARTIX7, 
     *                                                                                           |  |  |  |  |     EBD_AT91
     *                                                                                           |  |  |  |  |     |  EBD_CORTEXA9
     *                                                                                           |  |  |  |  |     |  |  EBD_ARTIX7
     *                                                                                           |  |  |  |  |     |  |  |     STG3838_XSCALE,
     *                                                                                           |  |  |  |  |     |  |  |     |  STG3838_AT91,
     *                                                                                           |  |  |  |  |     |  |  |     |  |  STG3838_CORTEXR5,
     *                                                                                           |  |  |  |  |     |  |  |     |  |  |     MCH_XSCAL,
     *                                                                                           |  |  |  |  |     |  |  |     |  |  |     |  MCH_AT91
     *                                                                                           |  |  |  |  |     |  |  |     |  |  |     |  |  EBD_MCH_AT91
     *                                                                                           |  |  |  |  |     |  |  |     |  |  |     |  |  |  */
    mil_fw_cmd_lookup[MIL_FW_CMD_RT_ACTIVATE_SYNC_MC]             = MIL_FW_CREATE_LOOKUP_BITMASK(1, 1, 1, 1, 1,    1, 1, 1,    1, 1, 1,    0, 0, 0);
    mil_fw_cmd_lookup[MIL_FW_CMD_BC_SCW_RETRY_TRANSFER_BUS_MODE]  = MIL_FW_CREATE_LOOKUP_BITMASK(1, 1, 1, 1, 1,    0, 0, 0,    1, 1, 1,    0, 0, 0);
    mil_fw_cmd_lookup[MIL_FW_CMD_BC_SCW_RETRY_GLOBAL_START_BUS]   = MIL_FW_CREATE_LOOKUP_BITMASK(1, 1, 1, 1, 1,    0, 0, 0,    1, 1, 1,    0, 0, 0);
    mil_fw_cmd_lookup[MIL_FW_CMD_BC_SSW_GLOBAL_ACTIVE_BUS]        = MIL_FW_CREATE_LOOKUP_BITMASK(1, 1, 1, 1, 1,    1, 1, 1,    1, 1, 1,    0, 0, 0);
    mil_fw_cmd_lookup[MIL_FW_CMD_BC_LOW_PRIORITY_LIST]            = MIL_FW_CREATE_LOOKUP_BITMASK(1, 1, 1, 1, 1,    1, 1, 1,    1, 1, 1,    0, 0, 0);
    mil_fw_cmd_lookup[MIL_FW_CMD_BC_MODIFY_INSTRUCTION]           = MIL_FW_CREATE_LOOKUP_BITMASK(1, 1, 1, 1, 1,    0, 0, 0,    1, 1, 1,    0, 0, 0);
    mil_fw_cmd_lookup[MIL_FW_CMD_BC_MC17_SUPPORT]                 = MIL_FW_CREATE_LOOKUP_BITMASK(1, 1, 1, 1, 1,    1, 1, 1,    1, 1, 1,    0, 0, 0);
    mil_fw_cmd_lookup[MIL_FW_CMD_BC_AT91_RT_EXTENDED_ERR]         = MIL_FW_CREATE_LOOKUP_BITMASK(0, 1, 1, 1, 1,    1, 0, 0,    0, 0, 0,    0, 0, 0);
    mil_fw_cmd_lookup[MIL_FW_CMD_BC_ACYC_SEND_ON_TTAG]            = MIL_FW_CREATE_LOOKUP_BITMASK(0, 1, 1, 1, 1,    0, 0, 0,    0, 1, 1,    0, 1, 0);
    mil_fw_cmd_lookup[MIL_FW_CMD_PROT_1553A]                      = MIL_FW_CREATE_LOOKUP_BITMASK(1, 1, 1, 1, 1,    0, 0, 0,    1, 1, 1,    1, 1, 0);
    mil_fw_cmd_lookup[MIL_FW_CMD_BC_FAST_GAPMODE]                 = MIL_FW_CREATE_LOOKUP_BITMASK(1, 1, 0, 1, 1,    0, 0, 0,    1, 1, 1,    1, 1, 0);
    mil_fw_cmd_lookup[MIL_FW_CMD_BC_EVENT_QUEUE]                  = MIL_FW_CREATE_LOOKUP_BITMASK(1, 1, 1, 1, 1,    0, 0, 0,    1, 1, 1,    1, 1, 0);
    mil_fw_cmd_lookup[MIL_FW_CMD_RT_EVENT_QUEUE]                  = MIL_FW_CREATE_LOOKUP_BITMASK(1, 1, 1, 1, 1,    0, 0, 0,    1, 1, 1,    1, 1, 0);
    mil_fw_cmd_lookup[MIL_FW_CMD_BM_CAPMODE_ONLY]                 = MIL_FW_CREATE_LOOKUP_BITMASK(1, 1, 1, 1, 1,    0, 0, 0,    1, 1, 1,    1, 1, 0);
    mil_fw_cmd_lookup[MIL_FW_CMD_BM_SEVERAL_TCBS]                 = MIL_FW_CREATE_LOOKUP_BITMASK(1, 1, 1, 1, 1,    0, 0, 0,    1, 1, 1,    1, 1, 0);
    mil_fw_cmd_lookup[MIL_FW_CMD_BM_TRIGGER_ON_DATA_VALUE]        = MIL_FW_CREATE_LOOKUP_BITMASK(1, 1, 1, 1, 1,    0, 0, 0,    1, 1, 1,    1, 1, 0);
    mil_fw_cmd_lookup[MIL_FW_CMD_BM_ENH_ACTIVITY_PAGE]            = MIL_FW_CREATE_LOOKUP_BITMASK(1, 1, 1, 1, 1,    0, 0, 0,    1, 1, 1,    1, 1, 0);
    mil_fw_cmd_lookup[MIL_FW_CMD_BM_MSG_COUNTER]                  = MIL_FW_CREATE_LOOKUP_BITMASK(1, 1, 1, 1, 1,    0, 0, 0,    1, 1, 1,    1, 1, 0);
    mil_fw_cmd_lookup[MIL_FW_CMD_BM_PRISEC_COUNTER]               = MIL_FW_CREATE_LOOKUP_BITMASK(1, 1, 1, 1, 1,    0, 0, 0,    1, 1, 1,    1, 1, 0);
    mil_fw_cmd_lookup[MIL_FW_CMD_BC_ERR_INJECTION]                = MIL_FW_CREATE_LOOKUP_BITMASK(1, 1, 1, 1, 1,    0, 0, 0,    1, 1, 1,    1, 1, 0);
    mil_fw_cmd_lookup[MIL_FW_CMD_RT_ERR_INJECTION]                = MIL_FW_CREATE_LOOKUP_BITMASK(1, 1, 1, 1, 1,    0, 0, 0,    1, 1, 1,    1, 1, 0);
    mil_fw_cmd_lookup[MIL_FW_CMD_REPLAY]                          = MIL_FW_CREATE_LOOKUP_BITMASK(1, 1, 1, 1, 1,    0, 0, 0,    1, 1, 1,    1, 1, 0);
    mil_fw_cmd_lookup[MIL_FW_CMD_RT_SINGLE_OPERATION]             = MIL_FW_CREATE_LOOKUP_BITMASK(0, 0, 0, 0, 0,    1, 1, 1,    0, 0, 0,    0, 0, 1);
    mil_fw_cmd_lookup[MIL_FW_CMD_BC_TRAFFIC_OVERRIDE]             = MIL_FW_CREATE_LOOKUP_BITMASK(0, 1, 1, 1, 1,    0, 0, 0,    0, 0, 0,    0, 1, 0);
    mil_fw_cmd_lookup[MIL_FW_CMD_BC_INSTR_RETURN_ON_XFER_ERROR]   = MIL_FW_CREATE_LOOKUP_BITMASK(0, 1, 1, 1, 1,    1, 1, 1,    0, 1, 1,    1, 1, 1);
    mil_fw_cmd_lookup[MIL_FW_CMD_END_OF_TABLE]                    = MIL_FW_CREATE_LOOKUP_BITMASK(0, 0, 0, 0, 0,    0, 0, 0,    0, 0, 0,    0, 0, 0);
}






/*! \brief Check if command is supported.

\param p_api_dev pointer to target administration structure
\param cmd The FW command identified by the command.
\return returns 1 if the command is supported 0 if not and -1 on error */
AiBoolean mil_fw_is_cmd_supported(TY_API_DEV * p_api_dev, enum MIL_FW_CMD cmd)
{
    AiBoolean   supported = AiFalse;
    enum MIL_FW fw = mil_hw_get_fw_type(p_api_dev, p_api_dev->biu);

    switch (fw)
    {
    case MIL_FW_STD_XSCALE:       supported = MIL_FW_GET_STD_XSCALE(mil_fw_cmd_lookup[cmd]); break;
    case MIL_FW_STD_AT91:         supported = MIL_FW_GET_STD_AT91(mil_fw_cmd_lookup[cmd]); break;
    case MIL_FW_STD_CORTEXA9:     supported = MIL_FW_GET_STD_CORTEXA9(mil_fw_cmd_lookup[cmd]); break;
    case MIL_FW_STD_CORTEXR5:     supported = MIL_FW_GET_STD_CORTEXR5(mil_fw_cmd_lookup[cmd]); break;
	case MIL_FW_STD_ARTIX7:       supported = MIL_FW_GET_STD_ARTIX7(mil_fw_cmd_lookup[cmd]); break;

    case MIL_FW_EBD_AT91:         supported = MIL_FW_GET_EBD_AT91(mil_fw_cmd_lookup[cmd]); break;
    case MIL_FW_EBD_CORTEXA9:     supported = MIL_FW_GET_EBD_CORTEXA9(mil_fw_cmd_lookup[cmd]); break;
    case MIL_FW_EBD_ARTIX7:       supported = MIL_FW_GET_EBD_ARTIX7(mil_fw_cmd_lookup[cmd]); break;

    case MIL_FW_STG3838_XSCALE:   supported = MIL_FW_GET_STG3838_XSCALE(mil_fw_cmd_lookup[cmd]); break;
    case MIL_FW_STG3838_AT91:     supported = MIL_FW_GET_STG3838_AT91(mil_fw_cmd_lookup[cmd]); break;
    case MIL_FW_STG3838_CORTEXR5: supported = MIL_FW_GET_STG3838_CORTEXR5(mil_fw_cmd_lookup[cmd]); break;
    
    case MIL_FW_MCH_XSCALE:       supported = MIL_FW_GET_MCH_XSCALE(mil_fw_cmd_lookup[cmd]); break;
    case MIL_FW_MCH_AT91:         supported = MIL_FW_GET_MCH_AT91(mil_fw_cmd_lookup[cmd]); break;
    case MIL_FW_MCH_EBD_AT91:     supported = MIL_FW_GET_MCH_EBD_AT91(mil_fw_cmd_lookup[cmd]); break;
    default: break;
    }

    return supported;
}


