/* SPDX-FileCopyrightText: 2001-2022 AIM GmbH <info@aim-online.com> */
/* SPDX-License-Identifier: MIT OR GPL-2.0-or-later */

#include "mil_tsw_includes.h"

#ifdef _NUCLEUS
#ifndef NU_DEBUG
#pragma import(__use_no_semihosting_swi)
#endif
#endif

UINT8 dummyInit(void)
{
   
	// nothing implementation

    return 0;
}





UINT8 aviVmeInit(UINT32 bType, UINT32 gRAMSize)
{

	// nothing implementation

    return 0;
}

UINT8 axiVxiInit(UINT32 bType, UINT32 gRAMSize)
{

	// nothing implementation

    return 0;
}

void MonIntTest(void)
{

	// nothing implementation

}


void api_dma_InitGt(TY_API_DEV *p_api_dev) 
{

	// nothing implementation

}

UINT8 axi_sys(UINT32 cmd, UINT16 size, UINT8 *info_p)
{

   // nothing implementation

    return 0;
}


void start_timer(UINT32 timer, UINT32 rate)
{

	// nothing implementation

}

void  stop_timer(UINT32 timer)
{

	// nothing implementation

}

#ifdef TEST
void DisableINT(void) // asm-program in int_310.s
{

// nothing implementation

}

void EnableINT(void)  // asm-program in int_310.s
{

	// nothing implementation

}

void  Jump(unsigned JmpAddress)  // asm-program in int_321_rel.s
{

	// nothing implementation

}


void FlushCache(void)
{

	// nothing implementation

}
#endif

#ifndef _NUCLEUS_2_1
void CallSub(unsigned CallAddress) // asm-program in int_310.s
{

	// nothing implementation

}
#endif

#if defined(_NUCLEUS)
void v_GetPciRegTimeTag_NUCLEUS( UINT32 *pul_TTHigh, UINT32 *pul_TTLow ) //erc.c
{

	// nothing implementation

}
#endif



