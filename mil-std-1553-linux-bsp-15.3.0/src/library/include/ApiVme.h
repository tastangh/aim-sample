/* SPDX-FileCopyrightText: 2016-2022 AIM GmbH <info@aim-online.com> */
/* SPDX-License-Identifier: MIT */

/*! \file ApiVme.h
 *
 *  This header file contains declarations for
 *  vme related API functionality
 *
 */

#ifndef AI1553_VME_H
#define AI1553_VME_H 



/************/
/* defines  */
/************/


/**************************/
/* structs and typedefs   */
/**************************/

typedef struct ty_api_vme1553_open_input
{
    AiUInt32 A16Address;
    AiUInt32 A32Address;
    AiUInt32 A32UserAccess;
    AiUInt32 A32Address2;    /* only needed for PMC */
    AiUInt32 A32UserAccess2; /* only needed for PMC */
    AiUInt32 InitAsSlave;
}TY_VME1553_OPEN_INPUT;

typedef struct ty_api_vme1553_open_output
{
    AiInt32 ModuleId[MAX_API_MODULE];
}TY_VME1553_OPEN_OUTPUT;

typedef struct ty_api_1553_pcimap_input
{
    AiUInt32 aulPciAddress[MAX_API_MODULE];
    AiUInt32 aulPciUserAccess[MAX_API_MODULE];
    AiUInt32 InitAsSlave;
}TY_1553_PCIMAP_INPUT;

typedef struct ty_api_1553_pcimap_output
{
    AiInt32 ModuleId[MAX_API_MODULE];
}TY_1553_PCIMAP_OUTPUT;



/***********************/
/* interface functions */
/***********************/


AI_LIB_FUNC AiReturn AI_CALL_CONV AiVme1553MapModule(TY_VME_MAP_MODULE_IN *in);
AI_LIB_FUNC AiReturn AI_CALL_CONV AiVme1553UnmapModule(TY_VME_MAP_MODULE_IN *in);
AI_LIB_FUNC AiReturn AI_CALL_CONV Ai1553CheckModule(TY_VME_MAP_MODULE_IN *in);



/**********************/
/* internal functions */
/**********************/


#endif /* AI1553_VME_H */
