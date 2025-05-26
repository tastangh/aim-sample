/* SPDX-FileCopyrightText: 2001-2022 AIM GmbH <info@aim-online.com> */
/* SPDX-License-Identifier: MIT */

/*! \file HiddenFnc.h
 *
 * Functions that are not officially exported by the Library.
 * 
 */

#ifdef __cplusplus
extern "C" {
#endif

#ifndef _API3910
  #define _API3910
#endif

// Fast Replay Functions --> die Funktionen ApiWriteRepData und Api3910HsWriteRepData sollten mit diesen ersetzt werden
extern AiInt16 _API_DLL_FUNC ApiWriteRepDataF      (AiUInt32 bModule, AiUInt8 biu,  TY_API_REP_STATUS *api_rep_stat,
                                                    void *lpBuf, AiUInt32 *lBytesWritten);
#ifdef _API3910
  extern AiInt16 _API_DLL_FUNC Api3910HsWriteRepDataF(AiUInt32 bModule, 
                                                      TY_API_HS_REP_STATUS *px_ApiRepStat,
                                                      void *pl_Buf,
                                                      AiUInt32 *pul_BytesWritten);
#endif

#ifdef __cplusplus
}
#endif

