/* SPDX-FileCopyrightText: 2009-2021 AIM GmbH <info@aim-online.com> */
/* SPDX-License-Identifier: MIT  */

/* MIL_WRAP.h */

#pragma once


/* 
 * version defines showing sind when some functionality is supported 
 */

#define FIRST_TSW_VER_REBOOT       1430
#define FIRST_TSW_VER_BOARDINFO    1880



/* The target software older than V18.80 Build 0270 does not recognize SREC marks in S0 lines 
 */
#define VERSION_LOADSREC_S0SUPPORTED  1880


/* The target software older than V20.40 Build 0374 does not support Novram Finalize 
 */
#define VERSION_NOVRAM_FINALIZE_SUPPORTED  2040





/*
 *  offset defines used, in case ApiGetBoardInfo is not yet suppored 
 */
#define NVR_SERIALNO               0x0C
#define NVR_BOARDCONFIG            0x28
#define NVR_BOARDTYPE              0x2C
#define NVR_PARTNO                 0x48

