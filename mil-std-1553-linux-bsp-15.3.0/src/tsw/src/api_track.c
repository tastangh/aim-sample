/* SPDX-FileCopyrightText: 2008-2022 AIM GmbH <info@aim-online.com> */
/* SPDX-License-Identifier: MIT OR GPL-2.0-or-later */

#include "mil_tsw_includes.h"




NAMESPACE1553_START







TY_MIL_COM_TABLE_ENTRY axTargetSysTrackCommands[] = {
  {MIL_COM_LS_SYS_TRACK_DEF,        (TY_MIL_COM_FUNC_PTR)api_sys_track_def_cmd,          "api_sys_track_def_cmd",        sizeof(TY_MIL_COM_LS_SYS_TRACK_DEF_INPUT),           sizeof(TY_MIL_COM_LS_SYS_TRACK_DEF_OUTPUT)   },
  {MIL_COM_LS_SYS_TRACK_READ,       (TY_MIL_COM_FUNC_PTR)api_sys_track_read_cmd,         "api_sys_track_read_cmd",       sizeof(TY_MIL_COM_LS_SYS_TRACK_READ_INPUT),          0                        },
  {MIL_COM_LS_SYS_TRACK_SCAN,       (TY_MIL_COM_FUNC_PTR)api_sys_track_scan_cmd,         "api_sys_track_scan_cmd",       sizeof(TY_MIL_COM_LS_SYS_TRACK_SCAN_INPUT),          0                        },
  {MIL_COM_LS_SYS_TRACK_ALLOC,      (TY_MIL_COM_FUNC_PTR)api_sys_track_alloc_cmd,        "api_sys_track_alloc_cmd",      sizeof(TY_MIL_COM_LS_SYS_TRACK_ALLOC_INPUT),         0                        }
};



void api_ls_sys_command_table( TY_MIL_COM_TABLE_ENTRY ** table, AiUInt32 * size )
{
    *size  = sizeof(axTargetSysTrackCommands) / sizeof(TY_MIL_COM_TABLE_ENTRY);
    *table = axTargetSysTrackCommands;
}


/*****************************************************************************/
/*                                                                           */
/*   Module : API_SYS                  Submodule : api_sys_track_prealloc    */
/*                                                                           */
/*   Author : Patrick Giesel           Project   : AVM1553T/3910TX           */
/*                                                                           */
/*   Source : C                        Tools     : PC/AT; Norton Editor;     */
/*                                                 Microtec Research's       */
/*                                                 MCC68K ANSI-C Compiler    */
/*                                                 ASM68K Assembler          */
/*---------------------------------------------------------------------------*/
/*   Create : 04.03.08   Update :                                            */
/*---------------------------------------------------------------------------*/
/*    Descriptions                                                           */
/*    ------------                                                           */
/*    Inputs    : Track Identifier [uTrackId]                                */
/*                True if High speed Buffer [bHs]                            */
/*                Pre-Alloc Nb [ul_PreAllocNb]                               */
/*                List of Mux States [pul_MuxStates]                         */
/*                                                                           */
/*    Outputs   : Instruction acknowledge type [ackfl]                       */
/*                                                                           */
/*    Description :                                                          */
/*  This function is used used to                                            */
/*                                                                           */
/*****************************************************************************/
L_WORD api_sys_track_alloc_cmd( TY_API_DEV *p_api_dev, TY_MIL_COM_LS_SYS_TRACK_ALLOC_INPUT* in, TY_MIL_COM_LS_SYS_TRACK_ALLOC_OUTPUT * out )
{
    L_WORD ret_val = 0;
    
    ret_val = api_sys_track_prealloc( p_api_dev, FALSE, in->id, in->count, in->states, &out->count, out->buffer_start );

    if( ret_val == 0 )
        out->xAck.ulSize = sizeof(TY_MIL_COM_LS_SYS_TRACK_ALLOC_OUTPUT) - sizeof(AiUInt32) + out->count * sizeof(AiUInt32); 
    else
        out->xAck.ulSize = sizeof(TY_MIL_COM_ACK); 

    return ret_val;
}

L_WORD api_sys_track_prealloc(TY_API_DEV *p_api_dev, BYTE bHs, L_WORD ul_TrackId, L_WORD ul_PreAllocNb, WORD states[], L_WORD * out_count, L_WORD buffer_start[] )
{
    L_WORD ret_val = 0;
    L_WORD i;

    TY_API_TRACK_DEF *pTrack;
    TY_API_TRACK_DEF *pTrackList;
    TY_API_TRACK_LIST_ELEMENT x_TrackElement;

    *out_count = 0;

    if(bHs)
        pTrackList = p_api_dev->hs_track[ p_api_dev->hs_stream ];
    else
        pTrackList = p_api_dev->ls_track[ p_api_dev->biu ];

    pTrack = &pTrackList[ ul_TrackId ];

    if( 0 == pTrack->uTrackStates ) /* only if dynamic mem allocation is enabled */
    {
        /* search for all given mux states and look if they have to be allocated */
        for( i=0; i<ul_PreAllocNb; i++ )
        {
            TY_API_TRACK_LIST_ELEMENT *px_TrackElement = NULL;

            buffer_start[ i ]  = 0L;
            api_sys_track_find_list_element(p_api_dev, pTrack->px_TrackStateList, states[i], &px_TrackElement);

            if( NULL == px_TrackElement ) /* if mux state element is not found, it has to be allocated now */
            {
                x_TrackElement.puw_Dest = (WORD*)MALLOC(pTrack->uTrackSize * sizeof(WORD));

                if( x_TrackElement.puw_Dest )
                {
                    x_TrackElement.uc_Flags      = 0;
                    x_TrackElement.uw_TrackState = states[i];

                    if( API_OK != api_sys_track_add_list_element(p_api_dev, pTrack, states[i], &x_TrackElement) )
                    {
                        FREE(x_TrackElement.puw_Dest);
                        x_TrackElement.puw_Dest = NULL;
                    }
                    else
                    {
                        /* Do not return x_TrackElement.puw_Dest, as it is a kernel space address */
                        buffer_start[ i ] = 0xFFFFFFFFL;
                    }
                }
            }
            *out_count = *out_count + 1;
        }
    }

    pTrack->bPreAllocated = 1;

    return ret_val;
}


/*****************************************************************************/
/*                                                                           */
/*   Module : API_SYS                  Submodule : api_sys_track_scan        */
/*                                                                           */
/*   Author : Patrick Giesel           Project   : AVM1553T/3910TX           */
/*                                                                           */
/*   Source : C                        Tools     : PC/AT; Norton Editor;     */
/*                                                 Microtec Research's       */
/*                                                 MCC68K ANSI-C Compiler    */
/*                                                 ASM68K Assembler          */
/*---------------------------------------------------------------------------*/
/*   Create : 19.02.08   Update :                                            */
/*---------------------------------------------------------------------------*/
/*    Descriptions                                                           */
/*    ------------                                                           */
/*    Inputs    : Track Identifier [uTrackId]                                */
/*                True if High speed Buffer [bHs]                            */
/*                Track Mode (Create/Delete) [uTrackMode]                    */
/*                RT-Address [uRT]                                           */
/*                                                                           */
/*    Outputs   : Instruction acknowledge type [ackfl]                       */
/*                                                                           */
/*    Description :                                                          */
/*  This function is used used to                                            */
/*                                                                           */
/*****************************************************************************/
L_WORD api_sys_track_scan_cmd( TY_API_DEV *p_api_dev, TY_MIL_COM_LS_SYS_TRACK_SCAN_INPUT * in, TY_MIL_COM_LS_SYS_TRACK_SCAN_OUTPUT * out )
{
    L_WORD ret_val = 0;

    ret_val = api_sys_track_scan( p_api_dev, FALSE, (BYTE)in->id, in->index, in->size, &out->count, &out->more, out->data );

    if( ret_val == 0 )
    {
        out->xAck.ulSize = sizeof(TY_MIL_COM_LS_SYS_TRACK_SCAN_OUTPUT) - sizeof(AiUInt16) + out->count * sizeof(AiUInt16); 
        out->xAck.ulSwapControl = out->count << 16;
    }
    else
    {
        out->xAck.ulSize = sizeof(TY_MIL_COM_ACK); 
    }

    return ret_val;
}


L_WORD api_sys_track_scan(TY_API_DEV *p_api_dev, BYTE bHs, BYTE uc_TrackId, L_WORD ul_ChunkNb, L_WORD ul_ChunkSize, L_WORD * state_count, L_WORD * more_data, WORD states[] )
{
  L_WORD                    ul_NbOfReturnedStates;
  BYTE                      b_Searching = TRUE;
  TY_API_TRACK_LIST_ELEMENT *px_ListStart;
  L_WORD                    ul_NbOfFoundStates;
  L_WORD                    ul_StartStateNb = ul_ChunkNb*ul_ChunkSize + 1;
  L_WORD                    ul_EndStateNb   = ul_StartStateNb + ul_ChunkSize;

  TY_API_TRACK_DEF *pTrackList;
  TY_API_TRACK_DEF *pTrack;

  *state_count = 0; /* number of returned states */
  *more_data   = 0; /* more data flag            */

  /*if(uc_TrackId < API_MAX_MUX_BUFFER)  // Warning: API_MAX_MUX_BUFFER == 256 > BYTE */
  {

    if(bHs)
      pTrackList = p_api_dev->hs_track[ p_api_dev->hs_stream ];
    else
      pTrackList = p_api_dev->ls_track[ p_api_dev->biu ];

    pTrack = &pTrackList[ uc_TrackId ];
    px_ListStart = (TY_API_TRACK_LIST_ELEMENT *)pTrack->px_TrackStateList;

    ul_NbOfFoundStates  = 0;
    ul_NbOfReturnedStates = 0;
    while( b_Searching )
    {
      if( NULL == px_ListStart )
      {
        b_Searching = FALSE;
      }
      else
      {
        if( px_ListStart->uc_Flags & 1 ) /* only return state, if update flag is set */
        {
          ul_NbOfFoundStates++;

          if( (ul_NbOfFoundStates >= ul_StartStateNb) && (ul_NbOfFoundStates < ul_EndStateNb) )
          {
            /* only return the states of the chunk */
            states[ ul_NbOfReturnedStates++ ] = px_ListStart->uw_TrackState;
          }

          if( ul_NbOfFoundStates >= ul_EndStateNb )
          {
            b_Searching = FALSE;
            *more_data = 1;
          }
        }

        px_ListStart = (TY_API_TRACK_LIST_ELEMENT *)px_ListStart->px_Next;
      }
    }

    *state_count = ul_NbOfReturnedStates; /* number of returned states */
  }

  return 0;
}

/*****************************************************************************/
/*                                                                           */
/*   Module : API_SYS                  Submodule : api_sys_track_def         */
/*                                                                           */
/*   Author : Magnus Moehr             Project   : AVM1553T/3910TX           */
/*                                                                           */
/*   Source : C                        Tools     : PC/AT; Norton Editor;     */
/*                                                 Microtec Research's       */
/*                                                 MCC68K ANSI-C Compiler    */
/*                                                 ASM68K Assembler          */
/*---------------------------------------------------------------------------*/
/*   Create : 15.05.02   Update : 01.02.08                                   */
/*---------------------------------------------------------------------------*/
/*    Descriptions                                                           */
/*    ------------                                                           */
/*    Inputs    : Track Identifier [uTrackId]                                */
/*                True if High speed Buffer [bHs]                            */
/*                Track Mode (Create/Delete) [uTrackMode]                    */
/*                RT-Address [uRT]                                           */
/*                Sub Address / Message Identifier [uSaMid]                  */
/*                Track Start Position on Buffer [uTrackStartPos]            */
/*                Bit Position of the track parameter [uBpos]                */
/*                Bit Length of the track parameter [uBlen]                  */
/*                Number of continous Tracks in buffer [uTrackFileCount]     */
/*                Size of one track in words [uTrackSize]                    */
/*                Numer of different Multiplex Locations [uTrackCount]       */
/*                                                                           */
/*    Outputs   : Instruction acknowledge type [ackfl]                       */
/*                                                                           */
/*    Description :                                                          */
/*  This function is used used to define a Track file respectively multiplex */
/*  Buffer which is used to interpret data depending on the value of a       */
/*  Multiplex parameter.                                                     */
/*                                                                           */
/*****************************************************************************/
L_WORD api_sys_track_def_cmd( TY_API_DEV *p_api_dev, TY_MIL_COM_LS_SYS_TRACK_DEF_INPUT  * in, TY_MIL_COM_LS_SYS_TRACK_DEF_OUTPUT * out )
{
    return api_sys_track_def( p_api_dev, FALSE, (BYTE)in->track_id, (BYTE)in->bt, (WORD) in->xid_rt_sa, (BYTE)in->mode, (WORD)in->start, (BYTE)in->bit_pos, (BYTE)in->bit_len, (WORD) in->size, (WORD)in->states, (WORD)in->file_count, (int)in->offset, &out->buffer_start );
}

L_WORD api_sys_track_def(TY_API_DEV *p_api_dev, BYTE bHs, BYTE uTrackId, BYTE uBt, WORD uXidRtSa, BYTE uTrackMode, WORD uTrackStartPos, BYTE uBpos, BYTE uBlen, WORD uTrackSize, WORD uTrackStates, WORD uTrackFileCount, int l_Offset, L_WORD * buffer_start)
{
  L_WORD ret_val = 0;
  L_WORD tmpl;
  L_WORD index;
  BYTE bNoMoreMem = FALSE;
  TY_API_TRACK_LIST_ELEMENT *px_TempRemove = NULL;

  /*if(uc_TrackId < API_MAX_MUX_BUFFER)  // Warning: API_MAX_MUX_BUFFER == 256 > BYTE */
  {
    TY_API_TRACK_DEF *pTrackList;
    TY_API_TRACK_DEF *pTrack;

    if(bHs)
      pTrackList = p_api_dev->hs_track[ p_api_dev->hs_stream ];
    else
      pTrackList = p_api_dev->ls_track[ p_api_dev->biu ];

    pTrack = &pTrackList[ uTrackId ];

    if(uTrackMode == API_ON)
    {
      /* Switch off an eventually existing */ 
      /* Recursive!! */
      api_sys_track_def(p_api_dev, bHs, uTrackId, uBt, uXidRtSa, API_OFF, 0, 0, 0, 0, 0, 0, 0, buffer_start);
      pTrack->bEnable = TRUE;
      pTrack->uType = uBt;
      pTrack->uType2 = 0;

      pTrack->l_Offset = l_Offset;
      pTrack->ul_LastTimeTag = 0;

      pTrack->uXfer = uXidRtSa;
      pTrack->uRT = (BYTE) ( (uXidRtSa >> 8) &0x1f);
      pTrack->uSA = (BYTE) (  uXidRtSa & 0xff); /* 12.03.2013, wg. EFA & EFEX */
      pTrack->bTX = (BYTE) ( (uXidRtSa &0x8000) != 0);

      pTrack->uStartPos = uTrackStartPos;
      pTrack->uBpos = uBpos;
      pTrack->uBlen = uBlen;
      pTrack->uTrackFileCount = uTrackFileCount;
      pTrack->uTrackSize = uTrackSize;
      pTrack->uTrackStates = uTrackStates;
      pTrack->bPreAllocated = 0;

      if( p_api_dev->ul_DbgLevel & DBG_MUX_TRACK)
      {
        PRINTF6("ID: %i, T:%i XF:%i RT:%i SA:%i TX:%i ", uTrackId, pTrack->uType, pTrack->uXfer, pTrack->uRT, pTrack->uSA, pTrack->bTX);
        PRINTF8("START:%i BP:%i BL:%i SIZE:%i CNT: %i TFC:%i HS:%i OFFSET:%d\n\r", uTrackStartPos,uBpos, uBlen,uTrackSize, uTrackStates, uTrackFileCount, bHs, l_Offset);
      }

      /* Allocate Data */ 
      if( pTrack->uTrackStates != 0 )
      {
        /* define and allocate the memory of the trackstate list for all wanted states */
        TY_API_TRACK_LIST_ELEMENT *px_Temp = NULL;
        TY_API_TRACK_LIST_ELEMENT *px_TempPrev = NULL;
        L_WORD i;
        for( i=0; i<pTrack->uTrackStates; i++ )
        {
          px_Temp = (TY_API_TRACK_LIST_ELEMENT*) MALLOC(sizeof(TY_API_TRACK_LIST_ELEMENT));
          if( px_Temp )
          {
            px_Temp->px_Next = NULL;
            px_Temp->puw_Dest = (WORD*)MALLOC(uTrackSize * sizeof(WORD));
            if(px_Temp->puw_Dest)
            {
              px_Temp->uc_Flags = 0;
              px_Temp->uw_TrackState = (WORD)i;

              if( NULL == pTrack->px_TrackStateList )
              {
                pTrack->px_TrackStateList = px_Temp;
                px_Temp->px_Prev = NULL;
              }
              else
              {
                px_Temp->px_Prev = (L_WORD *)px_TempPrev;
                px_TempPrev->px_Next = (L_WORD *)px_Temp;
              }

              px_TempPrev = px_Temp;
            }
            else
              bNoMoreMem = TRUE;
          }
          else
            bNoMoreMem = TRUE;
        }

        /* if list could not be fully generated due to no more memory, free the list elements which have been allocated
         * until now 
         */
        if( bNoMoreMem )
        {
          if(pTrack->px_TrackStateList)
          {
            BYTE uc_Abort = FALSE;

            while( !uc_Abort )
            {
              if( pTrack->px_TrackStateList->puw_Dest )
              {
                FREE(pTrack->px_TrackStateList->puw_Dest);
                pTrack->px_TrackStateList->puw_Dest = NULL;
              }
              if(NULL == pTrack->px_TrackStateList->px_Next)
                uc_Abort = TRUE;
              pTrack->px_TrackStateList = (TY_API_TRACK_LIST_ELEMENT *)pTrack->px_TrackStateList->px_Next;

              /*@AIMFIXIT: TY_API_TRACK_LIST_ELEMENT is not freed, allocated above 
              Do we need to call api_sys_track_remove_list_element here ?
              */
            }

            pTrack->px_TrackStateList = NULL;
          }

          pTrack->bEnable = FALSE;
          if (uTrackStates>0)
              /* uTrackStates==0 => Buffers are allocated later with prealloc*/
              ret_val = API_ERR_NO_SPACE_LEFT;
          if(buffer_start)
              *buffer_start = 0;
        }
      }

      if( (pTrack->uTrackStates == 0) ||
          ((pTrack->uTrackStates != 0) && (FALSE == bNoMoreMem)) )
      {
        if(pTrack->uType == API_TRACK_TYPE_MUX)
        {
          /* Track on track */
          /* Set the type 2 flag of the related track ID */   
          if(uXidRtSa < API_MAX_MUX_BUFFER)
          {

            if(pTrackList[ uXidRtSa ].bEnable)
            {
              /* Store the type 2 flag */
              pTrack->uType2 = pTrackList[ uXidRtSa ].uType;
              pTrackList[ uXidRtSa ].uRelCount++;
            }
            else
            {
              PRINTF0("Related Track not enabled");
              ret_val = API_ERR_ELEMENT_NOT_ENABLED;
            }
          }
          else
          {
            PRINTF0("Invalid related Track");
            ret_val = API_ERR_PARAMETER_OUT_OF_RANGE;
          }
        }
        else
        /* Enable Int */
        if(!bHs)
        {
          /* LS Int */               
          if(pTrack->uType == API_TRACK_TYPE_BC)
          {
            tmpl = BSWAP32( (*p_api_dev->bc_xfer_desc_p[ p_api_dev->biu ])[ pTrack->uXfer ].xf_ctrl);
            LWORD_RESET_BITS(tmpl, 20, 3);
            LWORD_SET_BITS(tmpl, API_BC_XFER_IR_EOT, 20, 3);
            (*p_api_dev->bc_xfer_desc_p[ p_api_dev->biu ])[ pTrack->uXfer ].xf_ctrl = BSWAP32(tmpl);

            if(p_api_dev->bc_status[p_api_dev->biu] == API_BUSY)
            {
                UsbSynchronizeLWord(p_api_dev, 
                    API_GLB_MEM_ADDR_REL(&(*p_api_dev->bc_xfer_desc_p[ p_api_dev->biu ])[ pTrack->uXfer ].xf_ctrl), Out);
            }
          }
          else
          if(pTrack->uType == API_TRACK_TYPE_RT)
          {
            if(pTrack->bTX)
              index = pTrack->uSA + (MAX_API_RT_MC_SA *API_RT_SA_TYPE_XMT);
            else
              index = pTrack->uSA + (MAX_API_RT_MC_SA *API_RT_SA_TYPE_RCV);

            if( (BSWAP32( (*p_api_dev->rt_sa_desc_p[ p_api_dev->biu ])[ pTrack->uRT ].sa_mc[ index ].sa_ctrl) &0xC0000000L) != 0L)
            {
              (*p_api_dev->rt_sa_desc_p[ p_api_dev->biu ])[ pTrack->uRT ].sa_mc[ index ].sa_ctrl &= BSWAP32(0x3FFFFFFFUL);
              (*p_api_dev->rt_sa_desc_p[ p_api_dev->biu ])[ pTrack->uRT ].sa_mc[ index ].sa_ctrl |= BSWAP32(0x80000000UL);

              if(p_api_dev->rt_status[p_api_dev->biu] == API_BUSY)
              {
                  UsbSynchronizeLWord(p_api_dev, 
                      API_GLB_MEM_ADDR_REL(&(*p_api_dev->rt_sa_desc_p[ p_api_dev->biu ])[ pTrack->uRT ].sa_mc[ index ].sa_ctrl), Out);
              }
            }
          }
        }
        else
        {
          /* HS Int */
          if(pTrack->uType == API_TRACK_TYPE_BC)
          {
            tmpl = BSWAP32( (*api39_bc_xfer_desc_p[ p_api_dev->hs_stream ])[ pTrack->uXfer ].ul_XferCtrl);
            LWORD_RESET_BITS(tmpl, 27, 3);
            LWORD_SET_BITS(tmpl, API39_RT_MID_IR_ACC, 27, 3);
            (*api39_bc_xfer_desc_p[ p_api_dev->hs_stream ])[ pTrack->uXfer ].ul_XferCtrl = BSWAP32(tmpl);
          }
          else
          if(pTrack->uType == API_TRACK_TYPE_RT)
          {
            if(pTrack->bTX)
              index = pTrack->uSA + (Api39_glb_mem.stream[ p_api_dev->hs_stream ].count.rt_mid_rcv *API39_RT_MID_TYPE_XMT);
            else
              index = pTrack->uSA + (Api39_glb_mem.stream[ p_api_dev->hs_stream ].count.rt_mid_rcv *API39_RT_MID_TYPE_RCV);

            if(ul_StreamIsEFEX( p_api_dev, p_api_dev->biu ))
              tmpl = (*apief_rt_mid_desc_p[ p_api_dev->hs_stream ])[ pTrack->uRT ].mid_mc[ index ].ul_MidCtrl;
            else
              tmpl = (*api39_rt_mid_desc_p[ p_api_dev->hs_stream ])[ pTrack->uRT ].mid_mc[ index ].ul_MidCtrl;
            if( (tmpl &0xC0000000L) != 0L)
            {
              LWORD_RESET_BITS(tmpl, 28, 2); /* mask Interrupt */
              LWORD_SET_BITS(tmpl, API39_RT_MID_IR_ACC, 28, 2); /* enable Interrupt */
              if(ul_StreamIsEFEX( p_api_dev, p_api_dev->biu ))
                (*apief_rt_mid_desc_p[ p_api_dev->hs_stream ])[ pTrack->uRT ].mid_mc[ index ].ul_MidCtrl = tmpl;
              else
                (*api39_rt_mid_desc_p[ p_api_dev->hs_stream ])[ pTrack->uRT ].mid_mc[ index ].ul_MidCtrl = tmpl;
            }
          }
        }
      }
      else
      {
        pTrack->bEnable = FALSE;
        PRINTF0("Track not enabled during define! -> no more memory\r\n");
      }

      if(buffer_start)
          *buffer_start = ( pTrack->px_TrackStateList ) ? 0xFFFFFFFFL : 0L;
      
      if (uTrackStates>0)
          /* uTrackStates==0 => Buffers are allocated later with prealloc*/
          if( pTrack->px_TrackStateList == 0 ) 
              ret_val = API_ERR_NO_SPACE_LEFT;
    }
    else
    {
      /* Disable Int */

      /* free the list elements which have been allocated */
      if(pTrack->px_TrackStateList)
      {
        while( pTrack->px_TrackStateList )
        {
          px_TempRemove = pTrack->px_TrackStateList;
          api_sys_track_remove_list_element( p_api_dev, pTrack, px_TempRemove );
        }
      }

      pTrack->bEnable = FALSE;
    }
  }

  return ret_val;
}

L_WORD api_sys_track_find_list_element(TY_API_DEV *p_api_dev, TY_API_TRACK_LIST_ELEMENT *px_TrackListStart, WORD uMuxId, TY_API_TRACK_LIST_ELEMENT **px_TrackElement)
{
  BYTE uc_Abort = FALSE;
  BYTE uc_Found = FALSE;
  TY_API_TRACK_LIST_ELEMENT *px_CurrentListElement;
  TY_API_TRACK_LIST_ELEMENT *px_FoundListElement = NULL;

  /*PRINTF1("api_sys_track_find_list_element: Try to find track, mux id: %i\n\r", uMuxId);*/

  px_CurrentListElement = px_TrackListStart;
  if( NULL != px_CurrentListElement )
  {
    while(!uc_Abort && !uc_Found)
    {
      if( px_CurrentListElement->uw_TrackState == uMuxId )
      {
        px_FoundListElement = px_CurrentListElement;
        uc_Found = TRUE;
        /*PRINTF2("api_sys_track_find_list_element: Found mux id: %i -> %p\n\r", uMuxId, px_FoundListElement);*/
      }
      else
      {
        if(NULL != px_CurrentListElement->px_Next)
          px_CurrentListElement = (TY_API_TRACK_LIST_ELEMENT *)px_CurrentListElement->px_Next;
        else
          uc_Abort = TRUE;
      }
    }
  }
  /*else
   * PRINTF0("api_sys_track_find_list_element: px_TrackListElement is NULL\n\r");
   */

  if( uc_Found )
    *px_TrackElement = px_FoundListElement;
  else
    *px_TrackElement = NULL;

  return 0;
}

L_WORD api_sys_track_add_list_element(TY_API_DEV *p_api_dev, TY_API_TRACK_DEF *px_Track, WORD uMuxId, TY_API_TRACK_LIST_ELEMENT *px_TrackElement)
{
  BYTE uc_Abort = FALSE;
  TY_API_TRACK_LIST_ELEMENT *px_CurrentListElement;
  TY_API_TRACK_LIST_ELEMENT *px_FoundListElement = NULL;
  TY_API_TRACK_LIST_ELEMENT *px_ListElementToAdd = NULL;
  TY_API_TRACK_LIST_ELEMENT *px_ListElementTemp  = NULL;
  BYTE b_AddAtFirstPosition = FALSE;

  /*PRINTF1("Try to add track, mux id: %i\n\r", uMuxId);*/

  px_CurrentListElement = px_Track->px_TrackStateList;
  if( NULL == px_CurrentListElement )
  { /* element to be added is the first one*/
    px_Track->px_TrackStateList = (TY_API_TRACK_LIST_ELEMENT *)MALLOC(sizeof(TY_API_TRACK_LIST_ELEMENT));
    /*PRINTF1("track list ptr = %p\r\n",px_Track->px_TrackStateList);*/

    if(NULL == px_Track->px_TrackStateList)
      return API_ERR_PARAM1_IS_NULL;

    px_Track->px_TrackStateList->puw_Dest      = px_TrackElement->puw_Dest;
    px_Track->px_TrackStateList->px_Next       = NULL;
    px_Track->px_TrackStateList->px_Prev       = NULL;
    px_Track->px_TrackStateList->uc_Flags      = px_TrackElement->uc_Flags;
    px_Track->px_TrackStateList->uw_TrackState = uMuxId;
  }
  else
  {
    while(!uc_Abort)
    {
      if( px_CurrentListElement->uw_TrackState < uMuxId )
      {
        if(NULL == px_CurrentListElement->px_Next)
          px_FoundListElement = px_CurrentListElement;
        else
          px_CurrentListElement = (TY_API_TRACK_LIST_ELEMENT *)px_CurrentListElement->px_Next;
      }
      else if( px_CurrentListElement->uw_TrackState > uMuxId )
      {
        if(NULL == px_CurrentListElement->px_Prev)
        {
          px_FoundListElement = px_CurrentListElement;
          b_AddAtFirstPosition = TRUE;
        }
        else
          px_FoundListElement = (TY_API_TRACK_LIST_ELEMENT *)px_CurrentListElement->px_Prev;
      }

      if( NULL != px_FoundListElement )
      {
        uc_Abort = TRUE;

        /* add element to list*/
        px_ListElementToAdd = (TY_API_TRACK_LIST_ELEMENT *)MALLOC(sizeof(TY_API_TRACK_LIST_ELEMENT));
        if(NULL == px_ListElementToAdd)
          return API_ERR_NO_SPACE_LEFT;

        px_ListElementToAdd->puw_Dest      = px_TrackElement->puw_Dest;

        if( b_AddAtFirstPosition )
        {
          px_ListElementToAdd->px_Next       = (L_WORD *)px_Track->px_TrackStateList;
          px_ListElementToAdd->px_Prev       = NULL;
          px_ListElementToAdd->uc_Flags      = px_TrackElement->uc_Flags;
          px_ListElementToAdd->uw_TrackState = uMuxId;

          px_Track->px_TrackStateList->px_Prev = (L_WORD *)px_ListElementToAdd;
          px_Track->px_TrackStateList        = (TY_API_TRACK_LIST_ELEMENT *)px_ListElementToAdd;
        }
        else
        {
          px_ListElementToAdd->puw_Dest      = px_TrackElement->puw_Dest;
          px_ListElementToAdd->px_Next       = px_FoundListElement->px_Next;
          px_ListElementToAdd->px_Prev       = (L_WORD *)px_FoundListElement;
          px_ListElementToAdd->uc_Flags      = px_TrackElement->uc_Flags;
          px_ListElementToAdd->uw_TrackState = uMuxId;

          px_ListElementTemp                 = (TY_API_TRACK_LIST_ELEMENT *)px_FoundListElement->px_Next;
          px_FoundListElement->px_Next       = (L_WORD *)px_ListElementToAdd;

          if(px_ListElementTemp != NULL)
          {
            px_ListElementTemp->px_Prev        = (L_WORD *)px_ListElementToAdd;
          }
        }
      }

      /* if element already exists, do nothing! */
      if( px_CurrentListElement->uw_TrackState == uMuxId )
        uc_Abort = TRUE;
    }
  }

  return 0;
}

L_WORD api_sys_track_remove_list_element(TY_API_DEV *p_api_dev, TY_API_TRACK_DEF *px_Track, TY_API_TRACK_LIST_ELEMENT *px_TrackElement)
{
  TY_API_TRACK_LIST_ELEMENT *px_PrevListElement;

  if( px_TrackElement )
  {
    px_PrevListElement = (TY_API_TRACK_LIST_ELEMENT *)px_TrackElement->px_Prev;

    if( px_PrevListElement )
      px_PrevListElement->px_Next = px_TrackElement->px_Next;
    else /* we are removing the first element in the list */
    {
      px_Track->px_TrackStateList = (TY_API_TRACK_LIST_ELEMENT *)px_TrackElement->px_Next;
      if( px_Track->px_TrackStateList )
        px_Track->px_TrackStateList->px_Prev = NULL;
    }

    if( px_TrackElement->puw_Dest )
    {
      FREE( px_TrackElement->puw_Dest );
      px_TrackElement->puw_Dest = NULL;
    }

    FREE(px_TrackElement);
  }

  return 0;
}


/*****************************************************************************/
/*                                                                           */
/*   Module : API_SYS                  Submodule : api_sys_track_read        */
/*                                                                           */
/*   Author : Magnus Moehr             Project   : AVM1553T/3910TX           */
/*                                                                           */
/*   Source : C                        Tools     : PC/AT; Norton Editor;     */
/*                                                 Microtec Research's       */
/*                                                 MCC68K ANSI-C Compiler    */
/*                                                 ASM68K Assembler          */
/*---------------------------------------------------------------------------*/
/*   Create : 15.05.02   Update :                                            */
/*---------------------------------------------------------------------------*/
/*    Descriptions                                                           */
/*    ------------                                                           */
/*    Inputs    : Track Identifier [uTrackId]                                */
/*                Mux ID                                                     */
/*                                                                           */
/*    Outputs   : Instruction acknowledge type [ackfl]                       */
/*                                                                           */
/*    Description :                                                          */
/*  This function returnes the multiplexed data of the corresponding track.  */
/*                                                                           */
/*****************************************************************************/
L_WORD api_sys_track_read_cmd( TY_API_DEV *p_api_dev, TY_MIL_COM_LS_SYS_TRACK_READ_INPUT * in, TY_MIL_COM_LS_SYS_TRACK_READ_OUTPUT * out )
{
    L_WORD ret_val = 0;

    ret_val = api_sys_track_read( p_api_dev, FALSE, (BYTE)in->id, (WORD)in->index, (BYTE)in->mode, &out->valid, &out->last_time_tag, &out->count, out->data );

    if( ret_val == 0 )
    {
        out->xAck.ulSize = sizeof(TY_MIL_COM_LS_SYS_TRACK_READ_OUTPUT) - sizeof(AiUInt16) + out->count * sizeof(AiUInt16); 
        out->xAck.ulSwapControl = out->count << 16;
    }
    else
    {
        out->xAck.ulSize = sizeof(TY_MIL_COM_ACK); 
    }

    return ret_val;
}

L_WORD api_sys_track_read(TY_API_DEV *p_api_dev, BYTE bHs, BYTE uTrackId, WORD uMuxId, BYTE uReadMode, L_WORD * valid, L_WORD * ttag, L_WORD * count, WORD * data )
{
  L_WORD i;
  L_WORD ret_val = 0;


  *valid = 0;
  *ttag  = 0;
  *count = 0;


  if( p_api_dev->ul_DbgLevel & DBG_MUX_TRACK)
    PRINTF4("api_sys_track_read: HS: %i, uTrack: %i uMux: %i uMode: %i\n\r",bHs,uTrackId,uMuxId,uReadMode);

  /*if(uc_TrackId < API_MAX_MUX_BUFFER)  // Warning: API_MAX_MUX_BUFFER == 256 > BYTE */
  {
    TY_API_TRACK_DEF *pTrack = &p_api_dev->ls_track[ p_api_dev->biu][ uTrackId ];
    if(bHs)
      pTrack = &p_api_dev->hs_track[ p_api_dev->hs_stream ][ uTrackId ];

    if(pTrack->bEnable)
    {
      if((0 == pTrack->uTrackStates) || (uMuxId < pTrack->uTrackStates))
      {
        TY_API_TRACK_LIST_ELEMENT *px_TrackElement;

        api_sys_track_find_list_element(p_api_dev, pTrack->px_TrackStateList, uMuxId, &px_TrackElement);
        if( px_TrackElement != NULL )
        {
          /* Check for Data Valid */

          if(px_TrackElement->puw_Dest)
          {
            if(px_TrackElement->uc_Flags & 0x1)
            {
              /* return the data */
              for(i=0; i<pTrack->uTrackSize; i++)
              {
                data[ i ] = px_TrackElement->puw_Dest[i];
              }
              *count = pTrack->uTrackSize;
              *valid = px_TrackElement->uc_Flags & 0x1;
              *ttag  = pTrack->ul_LastTimeTag;


              if(uReadMode != 0)
              {
                /* Clear Update Flag */
                px_TrackElement->uc_Flags &= (~0x1);
              }
            }
          }
        }
      }
      else
      {
        PRINTF2("Wrong Mux ID: %i-%i\n\r", uMuxId, pTrack->uTrackStates);
        ret_val = API_ERR_PARAMETER_OUT_OF_RANGE;
      }
    }
    else
    {
      PRINTF4("Track not enabled / Memory Error: %i %i %p %p\n\r", uTrackId, pTrack->bEnable, pTrack->pDest, pTrack->pFlags);
      ret_val = API_ERR_ELEMENT_NOT_ENABLED;
    }
  }

  return ret_val;
}

/*****************************************************************************/
/*                                                                           */
/*   Module : API_SYS                  Submodule : api_sys_track_init        */
/*                                                                           */
/*   Author : Magnus Moehr             Project   : AVM1553T/3910TX           */
/*                                                                           */
/*   Source : C                        Tools     : PC/AT; Norton Editor;     */
/*                                                 Microtec Research's       */
/*                                                 MCC68K ANSI-C Compiler    */
/*                                                 ASM68K Assembler          */
/*---------------------------------------------------------------------------*/
/*   Create : 15.05.02   Update :                                            */
/*---------------------------------------------------------------------------*/
/*    Descriptions                                                           */
/*    ------------                                                           */
/*    Inputs    : Track Identifier [uTrackId]                                */
/*                Mux ID                                                     */
/*                                                                           */
/*    Outputs   : NONE                                                       */
/*                                                                           */
/*    Description :                                                          */
/*  This function Initialises the track structures                           */
/*                                                                           */
/*****************************************************************************/
void api_sys_track_init(TY_API_DEV *p_api_dev, BYTE bHS, BYTE bBC, BYTE bRT)
{
  BYTE bDisable;
  L_WORD i;

  /*printf("Track Init: %s: %s %s\n\r",bHS?"HS":"LS",bBC?"BC":"",bRT?"RT":"");*/
  TY_API_TRACK_DEF *pTrack = p_api_dev->ls_track[ p_api_dev->biu ];
  if(bHS)
    pTrack = p_api_dev->hs_track[ p_api_dev->hs_stream ];

  for(i = 0; i < API_MAX_MUX_BUFFER; i++, pTrack++)
  {
    if(pTrack->bEnable)
    {
      bDisable = FALSE;

      if(bBC)
      {
        if( (pTrack->uType == API_TRACK_TYPE_BC) || ( (pTrack->uType == API_TRACK_TYPE_MUX) && (pTrack->uType2 == API_TRACK_TYPE_BC) ) )
          bDisable = TRUE;
      }

      if(bRT)
      {
        if( (pTrack->uType == API_TRACK_TYPE_RT) || ( (pTrack->uType == API_TRACK_TYPE_MUX) && (pTrack->uType2 == API_TRACK_TYPE_RT) ) )
          bDisable = TRUE;
      }

      if(bDisable)
        api_sys_track_def(p_api_dev, bHS, (BYTE)i, 1, 0, API_OFF, 0, 0, 0, 0, 0, 0, 0, NULL);
    }
  }
}




NAMESPACE1553_END


/*** end of file ***/

