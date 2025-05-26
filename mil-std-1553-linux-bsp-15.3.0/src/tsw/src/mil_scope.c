/* SPDX-FileCopyrightText: 2015-2022 AIM GmbH <info@aim-online.com> */
/* SPDX-License-Identifier: MIT OR GPL-2.0-or-later */


/*! \file mil_scope.c
 *  Hardware platform independent file which contains
 *  definitions of all scope related functions        */

#include "mil_tsw_includes.h"


TY_MIL_COM_TABLE_ENTRY axTargetScopeCommands[] = {
  {MIL_COM_SCOPE_START,      (TY_MIL_COM_FUNC_PTR)mil_scope_start,       "mil_scope_start",       sizeof(TY_MIL_COM_SCOPE_START_INPUT),      sizeof(TY_MIL_COM_ACK)                    },
  {MIL_COM_SCOPE_STATUS,     (TY_MIL_COM_FUNC_PTR)mil_scope_status,      "mil_scope_status",      sizeof(TY_MIL_COM),                        sizeof(TY_MIL_COM_SCOPE_STATUS_OUTPUT)    },
  {MIL_COM_SCOPE_STOP,       (TY_MIL_COM_FUNC_PTR)mil_scope_stop,        "mil_scope_stop",        sizeof(TY_MIL_COM),                        sizeof(TY_MIL_COM_ACK)                    },
  {MIL_COM_SCOPE_TRIGGER,    (TY_MIL_COM_FUNC_PTR)mil_scope_trigger,     "mil_scope_trigger",     sizeof(TY_MIL_COM_SCOPE_TRIGGER_INPUT),    sizeof(TY_MIL_COM_ACK)                    },
  {MIL_COM_SCOPE_TRIG_STROBE,(TY_MIL_COM_FUNC_PTR)mil_scope_trigger_strobe, "mil_scope_trigger_strobe", sizeof(TY_MIL_COM_SCOPE_TRG_STROBE_INPUT), sizeof(TY_MIL_COM_ACK)              },
  {MIL_COM_SCOPE_RESET,      (TY_MIL_COM_FUNC_PTR)mil_scope_reset,       "mil_scope_reset",        sizeof(TY_MIL_COM),                       sizeof(TY_MIL_COM_ACK)                    },
  {MIL_COM_SCOPE_OFFSET_COMP, (TY_MIL_COM_FUNC_PTR)mil_scope_offset_comp, "mil_scope_offset_comp", sizeof(TY_MIL_COM_SCOPE_OFFSET_COMP),     sizeof(TY_MIL_COM_SCOPE_OFFSET_COMP_OUT)  },
};


void api_scope_command_table( TY_MIL_COM_TABLE_ENTRY ** table, AiUInt32 * size )
{
    *size = sizeof(axTargetScopeCommands) / sizeof(TY_MIL_COM_TABLE_ENTRY);
    *table = axTargetScopeCommands;
}


/*! \brief Converts the byte stream of a scope setup target command to scope setup structure

    This function uses the byte stream delivered by API for a scope setup command,
    parses the values and stores it in the given scope setup structure 
    \param p_api_dev pointer to \ref TY_API_DEV target administration structure 
    \param p_scope_setup pointer to \ref TY_API_SCOPE_SETUP structure which receives the parsed scope setup 
    \param p_command_params pointer to byte stream of scope setup target command to parse
    \param size complete size (including header) of scope setup target command in bytes
    \return returns 0 on success, API_NAK on error */
static AiUInt32 scope_command_to_scope_setup(TY_API_DEV* p_api_dev, TY_API_SCOPE_SETUP* p_scope_setup, void* p_command_params, AiUInt16 size)
{
    BYTE* p_byte_stream = (BYTE*)p_command_params;
    static const AiUInt16 min_expected_command_size = 31; /* We expect at least 6 uint32 values plus command header of size 7 */

    if(!p_scope_setup || !p_command_params)
    {
        return API_ERR_NAK;
    }

    if(size < min_expected_command_size)
    {
        return API_ERR_WRONG_CMD_SIZE;
    }

    p_scope_setup->ul_CouplingPri = MAKE_B2LWORD(*(p_byte_stream), *(p_byte_stream + 1), *(p_byte_stream + 2), *(p_byte_stream + 3) );
    p_scope_setup->ul_CouplingSec = MAKE_B2LWORD(*(p_byte_stream + 4), *(p_byte_stream + 5), *(p_byte_stream + 6), *(p_byte_stream + 7) );
    p_scope_setup->ul_SamplingRate = MAKE_B2LWORD(*(p_byte_stream + 8), *(p_byte_stream + 9), *(p_byte_stream + 10), *(p_byte_stream + 11) );
    p_scope_setup->ul_CaptureMode = MAKE_B2LWORD(*(p_byte_stream + 12), *(p_byte_stream + 13), *(p_byte_stream + 14), *(p_byte_stream + 15) );
    p_scope_setup->ul_OperationMode = MAKE_B2LWORD(*(p_byte_stream + 16), *(p_byte_stream + 17), *(p_byte_stream + 18), *(p_byte_stream + 19) );
    p_scope_setup->ul_SingleShotBuf = MAKE_B2LWORD(*(p_byte_stream + 20), *(p_byte_stream + 21), *(p_byte_stream + 22), *(p_byte_stream + 23) );

    return 0;
}


/*! \brief Converts the byte stream of a scope trigger setup target command to scope trigger structure

    This function uses the byte stream delivered by API for a scope trigger setup command,
    parses the values and stores it in the given scope trigger structure 
    \param p_api_dev pointer to \ref TY_API_DEV target administration structure 
    \param p_scope_setup pointer to \ref TY_API_SCOPE_TRG_EX structure which receives the parsed scope trigger setup 
    \param p_command_params pointer to byte stream of scope setup target command to parse
    \param size complete size (including header) of scope setup target command in bytes
    \return returns 0 on success, an appropriate error code on failure */
static AiUInt32 scope_command_to_scope_trigger(TY_API_DEV* p_api_dev, TY_API_SCOPE_TRG_EX* p_scope_trigger, void* p_command_params, AiUInt16 size)
{
    BYTE* p_byte_stream = (BYTE*)p_command_params;
    static const AiUInt16 min_expected_command_size = 35; /* We expect at least 7 uint32 values plus command header of size 7 */

    if(!p_scope_trigger || !p_command_params)
    {
        return API_ERR_NAK;
    }

    if(size < min_expected_command_size)
    {
        return API_ERR_WRONG_CMD_SIZE;
    }

    p_scope_trigger->ul_TrgMode      = MAKE_B2LWORD(*(p_byte_stream), *(p_byte_stream + 1), *(p_byte_stream + 2), *(p_byte_stream + 3) );
    p_scope_trigger->ul_TrgSrc       = MAKE_B2LWORD(*(p_byte_stream + 4), *(p_byte_stream + 5), *(p_byte_stream + 6), *(p_byte_stream + 7) );
    p_scope_trigger->ul_TrgValue     = MAKE_B2LWORD(*(p_byte_stream + 8), *(p_byte_stream + 9), *(p_byte_stream + 10), *(p_byte_stream + 11) );
    p_scope_trigger->ul_TrgNbSamples = MAKE_B2LWORD(*(p_byte_stream + 12), *(p_byte_stream + 13), *(p_byte_stream + 14), *(p_byte_stream + 15) );
    p_scope_trigger->ul_TrgFlags     = MAKE_B2LWORD(*(p_byte_stream + 16), *(p_byte_stream + 17), *(p_byte_stream + 18), *(p_byte_stream + 19) );
    p_scope_trigger->ul_TrgDelay     = MAKE_B2LWORD(*(p_byte_stream + 20), *(p_byte_stream + 21), *(p_byte_stream + 22), *(p_byte_stream + 23) );
    p_scope_trigger->ul_TrgTbt       = MAKE_B2LWORD(*(p_byte_stream + 24), *(p_byte_stream + 25), *(p_byte_stream + 26), *(p_byte_stream + 27) );

     /* 0x400 is the maximum value for scope samples, so this is the negative voltage of triggerValue 
        This value is used in API_SCOPE_MODE_GREATER_OR_LESS_THAN. Use ApiCmdScopeTriggerDefEx to set this individually */
    if(p_scope_trigger->ul_TrgMode == API_SCOPE_MODE_GREATER_OR_LESS_THAN)
    {
        p_scope_trigger->ul_TrgValue2 = 0x400 - p_scope_trigger->ul_TrgValue;
    }

    return 0;
}


BYTE mil_scope_cmd(TY_API_DEV* p_api_dev, AiUInt32 command_code, AiUInt16 size, void* p_command_params)
{
    BYTE ackfl;
    AiUInt32 return_val = API_ERR_NAK;
    TY_API_SCOPE_SETUP scope_setup = {0};
    TY_API_SCOPE_TRG_EX scope_trigger = {0};

    switch(command_code)
    {
    case API_SCOPE_SETUP:
        return_val = scope_command_to_scope_setup(p_api_dev, &scope_setup, p_command_params, size);
        if(return_val != 0)
        {
            SET_NAK_INFO(return_val);
            return API_NAK;
        }
        else
        {
            /* call hardware layer for actually setting up scope */
            return_val = mil_hw_scope_setup(p_api_dev, &scope_setup);
            if(return_val != 0)
            {
                SET_NAK_INFO(return_val);
                return API_NAK;
            }
        }
        ackfl = API_ACK;
        break;

    case API_SCOPE_TRG_DEF:
        return_val = scope_command_to_scope_trigger(p_api_dev, &scope_trigger, p_command_params, size);
        if(return_val != 0)
        {
            SET_NAK_INFO(return_val);
            return API_NAK;
        }
        else
        {
            /* call hardware layer for actually setting up the scope trigger */
            return_val = mil_hw_scope_trigger(p_api_dev, &scope_trigger);
            if(return_val != 0)
            {
                SET_NAK_INFO(return_val);
                return API_NAK;
            }
        }
        ackfl = API_ACK;
        break;

  
    case API_SCOPE_CALIBRATE:
        /* AyX scope calibrate only */
        return_val = mil_hw_scope_calibrate( p_api_dev, *(BYTE*)(p_command_params) );

        if(return_val != 0)
        {
            SET_NAK_INFO(return_val);
            return API_NAK;
        }
  
        ackfl = API_ACK2;
        break;

    default:
        SET_NAK_INFO(API_ERR_CMD_NOT_FOUND);
        return API_NAK;
    }

    return ackfl;
}



L_WORD mil_scope_start( TY_API_DEV *p_api_dev, TY_MIL_COM_SCOPE_START_INPUT * cmd_p, TY_MIL_COM_ACK * ack_p )
{
    L_WORD return_val = API_ERR_NAK;

    return_val = mil_hw_scope_start(p_api_dev, cmd_p->ulMode, cmd_p->ulNoOfCycles );

    return return_val;
}


L_WORD mil_scope_status(TY_API_DEV* p_api_dev, TY_MIL_COM* cmd_p, TY_MIL_COM_SCOPE_STATUS_OUTPUT* ack_p)
{
    L_WORD return_val = API_ERR;
    TY_API_SCOPE_STATUS eStatus;
 
    ack_p->xAck.ulSize = sizeof(TY_MIL_COM_SCOPE_STATUS_OUTPUT);

    return_val = mil_hw_scope_status(p_api_dev, &eStatus);
    if(return_val != 0)
    {
        return return_val;
    }

    ack_p->eScopeStatus = eStatus;

    return return_val;
}


L_WORD mil_scope_stop( TY_API_DEV* p_api_dev, TY_MIL_COM* cmd_p, TY_MIL_COM_ACK* ack_p )
{
    L_WORD return_val = API_ERR;

    return_val = mil_hw_scope_stop(p_api_dev);

    return return_val;
}


L_WORD mil_scope_trigger(TY_API_DEV* p_api_dev, TY_MIL_COM_SCOPE_TRIGGER_INPUT* cmd_p, TY_MIL_COM_ACK* ack_p)
{
    L_WORD return_val = API_ERR_NAK;

    return_val = mil_hw_scope_trigger(p_api_dev, &cmd_p->trigger_settings);

    return return_val;
}


L_WORD mil_scope_trigger_strobe(TY_API_DEV* p_api_dev, TY_MIL_COM_SCOPE_TRG_STROBE_INPUT* cmd_p, TY_MIL_COM_ACK* ack_p)
{
    L_WORD return_val = API_ERR_NAK;

    return_val = mil_hw_scope_trigger_strobe(p_api_dev, &cmd_p->xTriggerStrobeDef);

    return return_val;
}


L_WORD mil_scope_reset(TY_API_DEV* p_api_dev, TY_MIL_COM* cmd_p, TY_MIL_COM_ACK* ack_p)
{
    L_WORD return_val = API_ERR_NAK;

    return_val = mil_hw_scope_reset(p_api_dev);

    return return_val;
}


L_WORD mil_scope_offset_comp(TY_API_DEV* p_api_dev, TY_MIL_COM_SCOPE_OFFSET_COMP* cmd_p, TY_MIL_COM_SCOPE_OFFSET_COMP_OUT* ack_p)
{
    L_WORD return_val = API_ERR_NAK;
    TY_API_SCOPE_OFFSETS* p_scope_offsets = NULL;

    switch(cmd_p->ulMode)
    {
    case SCOPE_GET_OFFSETS :
        p_scope_offsets = &ack_p->scope_offsets;
        ack_p->xAck.ulSize = sizeof(TY_MIL_COM_SCOPE_OFFSET_COMP_OUT);
        return mil_hw_scope_get_offsets(p_api_dev, p_scope_offsets);

    case SCOPE_SET_OFFSETS :
        p_scope_offsets = &cmd_p->scope_offsets;
        return mil_hw_scope_set_offsets(p_api_dev, p_scope_offsets);
    
    default:
        return API_ERR_INVALID_MODE;
    }

    return return_val;
}


