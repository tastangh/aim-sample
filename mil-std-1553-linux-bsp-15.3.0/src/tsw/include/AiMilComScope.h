/* SPDX-FileCopyrightText: 2011-2022 AIM GmbH <info@aim-online.com> */
/* SPDX-License-Identifier: MIT OR GPL-2.0-or-later */

/*! \file AiMilComScope.h
*/

#ifndef __MIL_COM_SCOPE_H__
#define __MIL_COM_SCOPE_H__

#include "Ai_cdef.h"
#include "Ai1553Com.h"


#define MIL_COM_SCOPE_START        MIL_COM_SCOPE_OFFSET + 0
#define MIL_COM_SCOPE_STATUS       MIL_COM_SCOPE_OFFSET + 1
#define MIL_COM_SCOPE_STOP         MIL_COM_SCOPE_OFFSET + 2
#define MIL_COM_SCOPE_TRIGGER      MIL_COM_SCOPE_OFFSET + 3
#define MIL_COM_SCOPE_TRIG_STROBE  MIL_COM_SCOPE_OFFSET + 4
#define MIL_COM_SCOPE_RESET        MIL_COM_SCOPE_OFFSET + 5
#define MIL_COM_SCOPE_OFFSET_COMP  MIL_COM_SCOPE_OFFSET + 6



/* -- MIL_COM_SCOPE_START -- */

typedef struct
{
    TY_MIL_COM  xCommand;
    AiUInt32    ulMode;         /* 0 means single shot, 1 means continuous */
    AiUInt32    ulNoOfCycles;   /* only used in single shot mode */
} TY_MIL_COM_SCOPE_START_INPUT;


/*! Output structure for scope status command */
typedef struct MIL_COM_SCOPE_STATUS_OUTPUT
{
    TY_MIL_COM_ACK  xAck;               /*!< default acknowledge structure */
    TY_API_SCOPE_STATUS eScopeStatus;   /*!< scope status that is returned */
} TY_MIL_COM_SCOPE_STATUS_OUTPUT;


/*! Input structure for scope trigger command
    which is called by ApiCmdScopeTriggerDefEx */
typedef struct MIL_COM_SCOPE_TRIGGER_INPUT
{
    TY_MIL_COM xCommand;                    /*!< default command structure */
    TY_API_SCOPE_TRG_EX trigger_settings;   /*!< The trigger settings to adopt */
}TY_MIL_COM_SCOPE_TRIGGER_INPUT;

/*! Input structure for scope trigger strobe def command
    which is called by ApiCmdScopeTriggerStrobeDef */
typedef struct mil_com_scope_trg_stobe_input
{
    TY_MIL_COM xCommand;                         /*!< default command structure */
    TY_API_SCOPE_TRG_STROBE xTriggerStrobeDef;   /*!< The trigger settings to adopt */
}TY_MIL_COM_SCOPE_TRG_STROBE_INPUT;


/*! \def SCOPE_GET_OFFSETS
    Used as parameter for scope compensation command.
    Current scope offsets will be read back in this mode */
#define SCOPE_GET_OFFSETS   0


/*! \def SCOPE_SET_OFFSETS
    Used as parameter for scope compensation command.
    Sets new scope offsets */
#define SCOPE_SET_OFFSETS   1


/*! Input structure for scope compensation command */
typedef struct mil_com_scope_offset_comp
{
    TY_MIL_COM xCommand;                    /*!< default command structure */
    AiUInt32 ulMode;                        /*!< Specifies what shall be done, e.g. set or get values */
    TY_API_SCOPE_OFFSETS scope_offsets;     /*!< scope offset values to use when task is to set values */
}TY_MIL_COM_SCOPE_OFFSET_COMP;


/*! Output structure for scope offset compensation command */
typedef struct MIL_COM_SCOPE_OFFSET_COMP_OUT
{
    TY_MIL_COM_ACK  xAck;                 /*!< default acknowledge structure */
    TY_API_SCOPE_OFFSETS scope_offsets;   /*!< current active scope offsets  */
} TY_MIL_COM_SCOPE_OFFSET_COMP_OUT;



#endif /* __MIL_COM_SCOPE_H__ */
