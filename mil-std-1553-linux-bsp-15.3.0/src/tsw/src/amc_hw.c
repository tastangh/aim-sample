/* SPDX-FileCopyrightText: 2015-2022 AIM GmbH <info@aim-online.com> */
/* SPDX-License-Identifier: MIT OR GPL-2.0-or-later */

#include "mil_tsw_includes.h"

NAMESPACE1553_START

/* DMA not supported return NAK */
L_WORD api_dma_setup_sgl( TY_API_DEV *p_api_dev, TY_MIL_COM_SYS_DMA_SETUP_SGL_INPUT * cmd_p, TY_MIL_COM_ACK * ack_p )
{
    ack_p->ulError = API_ERR_CMD_NOT_FOUND;

    return API_NAK;
}

NAMESPACE1553_END

