/* SPDX-FileCopyrightText: 2016-2022 AIM GmbH <info@aim-online.com> */
/* SPDX-License-Identifier: MIT OR GPL-2.0-or-later */

/*********************************************************************************/
/*! \file mil_fw.h 

          This file can be used to check if the current fw supports a specific 
          fw command.
*/
/*********************************************************************************/



#ifndef __MIL_FW_H__
#define __MIL_FW_H__





#include "mil_hw.h"


#define MIL_FW_RESERVED 0


enum MIL_FW
{
    MIL_FW_UNKNOWN,
    MIL_FW_STD_XSCALE,
    MIL_FW_STD_AT91,
    MIL_FW_STD_CORTEXA9,
    MIL_FW_STD_CORTEXR5,
    MIL_FW_STD_ARTIX7,
    MIL_FW_EBD_AT91,
    MIL_FW_EBD_CORTEXA9,
    MIL_FW_EBD_ARTIX7,
    MIL_FW_STG3838_XSCALE,
    MIL_FW_STG3838_AT91,
    MIL_FW_STG3838_CORTEXR5,
    MIL_FW_MCH_XSCALE,
    MIL_FW_MCH_AT91,
    MIL_FW_MCH_EBD_AT91
};


enum MIL_FW_CMD
{
    MIL_FW_CMD_RT_ACTIVATE_SYNC_MC,
    MIL_FW_CMD_BC_SCW_RETRY_TRANSFER_BUS_MODE,
    MIL_FW_CMD_BC_SCW_RETRY_GLOBAL_START_BUS,
    MIL_FW_CMD_BC_SSW_GLOBAL_ACTIVE_BUS,
    MIL_FW_CMD_BC_LOW_PRIORITY_LIST,
    MIL_FW_CMD_BC_MODIFY_INSTRUCTION,
    MIL_FW_CMD_RT_MC01_SUPPORT,                       /* USE: MIL_FW_CMD_RT_ACTIVATE_SYNC_MC instead */
    MIL_FW_CMD_RT_MC17_SUPPORT,                       /* USE: MIL_FW_CMD_RT_ACTIVATE_SYNC_MC instead */
    MIL_FW_CMD_BC_MC17_SUPPORT,
    MIL_FW_CMD_BC_AT91_RT_EXTENDED_ERR,
    MIL_FW_CMD_BC_ACYC_SEND_ON_TTAG,
    MIL_FW_CMD_PROT_1553A,
    MIL_FW_CMD_BC_FAST_GAPMODE,
    MIL_FW_CMD_BC_EVENT_QUEUE,
    MIL_FW_CMD_RT_EVENT_QUEUE,
    MIL_FW_CMD_BM_CAPMODE_ONLY,
    MIL_FW_CMD_BM_SEVERAL_TCBS,
    MIL_FW_CMD_BM_TRIGGER_ON_DATA_VALUE,
    MIL_FW_CMD_BM_ENH_ACTIVITY_PAGE,
    MIL_FW_CMD_BM_MSG_COUNTER,
    MIL_FW_CMD_BM_PRISEC_COUNTER,
    MIL_FW_CMD_BC_ERR_INJECTION,
    MIL_FW_CMD_RT_ERR_INJECTION,
    MIL_FW_CMD_REPLAY,
    MIL_FW_CMD_RT_SINGLE_OPERATION,
    MIL_FW_CMD_BC_TRAFFIC_OVERRIDE,
    MIL_FW_CMD_BC_INSTR_RETURN_ON_XFER_ERROR,
    MIL_FW_CMD_END_OF_TABLE
};


/*! \brief Initialize FW function lookup table. */
void mil_fw_initialize_lookup(void);

/*! \brief Check if command is supported.

    \param p_api_dev pointer to target administration structure
    \param cmd The FW command identified by the command.
    \return returns true if the command is supported false otherwise */
AiBoolean mil_fw_is_cmd_supported(TY_API_DEV * p_api_dev, enum MIL_FW_CMD cmd);




#endif /* __MIL_FW_H__ */

/* end: mil_Fw.h */
