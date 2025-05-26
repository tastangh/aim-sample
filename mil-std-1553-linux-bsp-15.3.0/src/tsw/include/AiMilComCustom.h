/* SPDX-FileCopyrightText: 2015-2022 AIM GmbH <info@aim-online.com> */
/* SPDX-License-Identifier: MIT OR GPL-2.0-or-later */

/*! \file AiMilComCustom.h
*/

#ifndef __MIL_COM_CUSTOM_H__
#define __MIL_COM_CUSTOM_H__

#include "Ai_cdef.h"
#include "Ai1553Com.h"




#define MIL_COM_CUSTOM_DTS_GEN_IO_TEST_SEQ       MIL_COM_CUSTOM_DTS_OFFSET +  0




/* -- MIL_COM_GEN_IO_TEST_SEQ -- */
typedef struct
{
    TY_MIL_COM                 xCommand;

    AiUInt32                   ul_Con;
    TY_API_GENIO_TEST_SEQ_TBL  xSeqTbl;       /* Test Sequence Table */
} TY_MIL_COM_CUSTOM_DTS_GEN_IO_TEST_SEQ_INPUT;


typedef struct
{
    TY_MIL_COM_ACK  xAck;
    AiUInt8         aucSeqStatus[4];
} TY_MIL_COM_CUSTOM_DTS_GEN_IO_TEST_SEQ_OUTPUT;







#endif /* __MIL_COM_CUSTOM_H__ */
