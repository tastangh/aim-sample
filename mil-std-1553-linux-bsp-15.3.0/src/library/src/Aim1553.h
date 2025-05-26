/* SPDX-FileCopyrightText: 2001-2022 AIM GmbH <info@aim-online.com> */
/* SPDX-License-Identifier: MIT */

/*! \file Aim1553.h
 *
 *  This file defines the interrupt log list entries
 */


#include "aim_ioctl_interface.h"         /* ioctl macros */

#define IO_MAP_MAX_DEVICES 16
#define MAX_1553_DEVICES 16
#define MAX_GLOBAL_BYTE_ACCESS 0x10000

#define HIBYTE(w)  ((BYTE)(w))
#define LOWBYTE(w) ((BYTE)((WORD)(w) >> 8))


/* the device structure */
typedef struct Iomap {
	unsigned long base;
	unsigned long size;
	char *ptr;
} Iomap;

/* Parameters used for host to target IO commands */
typedef struct IoParameters
{
   AiUInt32 ul_DeviceNumber;
   AiUInt32 ul_CmdSize;
   AiUChar*  puc_Cmd;
   AiUChar*  puc_Ack;
} IoParameters;

typedef struct Gdata {
	unsigned long device;
	unsigned long width;
	unsigned long offset;
	unsigned char data[MAX_GLOBAL_BYTE_ACCESS];
	unsigned long size;
} Gdata;


typedef struct {
    unsigned long ul_Lla;
    unsigned long ul_Llb;
    unsigned long ul_Llc;
    unsigned long ul_Lld;
}TY_API_ISR_LOGLIST_ENTRY;

typedef void  (*TY_ISR_FUNC_PTR)(unsigned char uc_Module,
                                 unsigned char uc_Biu,
                                 unsigned char uc_Type,
                                 TY_API_ISR_LOGLIST_ENTRY x_Info );



typedef struct Isrcallback {
	unsigned char device;
	unsigned char biu;
	unsigned char type;
	TY_ISR_FUNC_PTR pf_IntFunc;
} Isrcallback;


