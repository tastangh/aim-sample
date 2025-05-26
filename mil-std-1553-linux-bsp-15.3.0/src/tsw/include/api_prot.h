/* SPDX-FileCopyrightText: 2015-2024 AIM GmbH <info@aim-online.com> */
/* SPDX-License-Identifier: MIT OR GPL-2.0-or-later */

/*! \file api_prot.h
 *
 *  Prototype information of the 1553 Driver Software.
 *
 */

#ifndef __API_PROT_H__
  #define __API_PROT_H__

#if !defined(_AIM_VME)
#ifdef __cplusplus
extern "C" {
#endif
#endif


#define NAMESPACE1553_START 
#define NAMESPACE1553_END   
/*
* The following lines shall be uncommented, if namespaces shall be used.
* Please note, that namespaces do not work with plain C.
* Please make sure Avc_io.cpp is also adapted, if namespaces shall be used.

#define NAMESPACE1553_START namespace Mil1553{
#define NAMESPACE1553_END   }
*/

/* if namespaces are used, the 'extern "C"' statement has to be removed. */
#ifdef __cplusplus
extern "C" {
#endif

#include "AiMilCom.h"
#include "Ai_def.h"

/********* Prototypes / New Host to Target Communication **********************/

/* api_sys.c */
void api_sys_command_table( TY_MIL_COM_TABLE_ENTRY ** table, AiUInt32 * size );
void api_custom_command_table( TY_MIL_COM_TABLE_ENTRY ** table, AiUInt32 * size );






BYTE   api_sys_convert_biu_physical_to_logical( TY_API_DEV * p_api_dev, short physical_biu );
short   api_sys_convert_biu_logical_to_physical( TY_API_DEV * p_api_dev, short logical_biu );




/* dma_apx.c / api_dma.c / amc_dma.c */
L_WORD api_dma_setup_sgl( TY_API_DEV *p_api_dev, TY_MIL_COM_SYS_DMA_SETUP_SGL_INPUT * cmd_p, TY_MIL_COM_ACK * ack_p );




/*! \brief Configure transfer error injection on the fly. */
L_WORD api_bc_xfer_def_err( TY_API_DEV *p_api_dev, TY_MIL_COM_LS_XFER_ERROR_INPUT * pCom, TY_MIL_COM_ACK * pAck );

/*! \brief Get transfer descriptor values on the fly. */
L_WORD api_bc_xfer_desc_get( TY_API_DEV *p_api_dev, TY_MIL_COM_LS_XFER_DESC_GET_INPUT * pCom, TY_MIL_COM_LS_XFER_DESC_GET_OUTPUT * pAck );

/*! \brief Modify transfer descriptor values on the fly. */
L_WORD api_bc_xfer_desc_mod( TY_API_DEV *p_api_dev, TY_MIL_COM_LS_XFER_DESC_MOD_INPUT * pCom, TY_MIL_COM_ACK * pAck );

/* mil_scope.c */
L_WORD mil_scope_start( TY_API_DEV *p_api_dev, TY_MIL_COM_SCOPE_START_INPUT * cmd_p, TY_MIL_COM_ACK * ack_p );


/*! \brief Gets scope status of a board

    This function is called on MIL_COM_SCOPE_STATUS target command.
    It performs hardware independent tasks and branches off into
    platform specific hardware layer function. 
    \param p_api_dev pointer to target administration structure
    \param cmd_p pointer to target command structure
    \param ack_p pointer to target acknowledge structure
    \return returns 0 on success, an appropriate error code otherwise */
L_WORD mil_scope_status( TY_API_DEV* p_api_dev, TY_MIL_COM* cmd_p, TY_MIL_COM_SCOPE_STATUS_OUTPUT* ack_p );


/*! \brief Stops scope execution of a board

    This function is called on MIL_COM_SCOPE_STOP target command.
    It branches off into a platform specific hardware layer function
    \param p_api_dev pointer to target administration structure
    \param cmd_p pointer to target command structure. Just \ref TY_MIL_COM without parameters
    \param ack_p pointer to target acknowledge structure Just \ref TY_MIL_COM_ACK without return data 
    \return returns 0 on success, an appropriate error code otherwise */
L_WORD mil_scope_stop( TY_API_DEV* p_api_dev, TY_MIL_COM* cmd_p, TY_MIL_COM_ACK* ack_p );


/*! \brief Sets scope trigger of a board

    This function is called on MIL_COM_SCOPE_TRIGGER command. \n
    It branches off into a platform specific hardware layer function
    \param p_api_dev pointer to target administration structure
    \param cmd_p pointer to \ref TY_MIL_COM_SCOPE_TRIGGER_INPUT structure that holds command and trigger settings
    \param ack_p pointer to \ref TY_MIL_COM_ACK structure that receives ack data */
L_WORD mil_scope_trigger(TY_API_DEV* p_api_dev, TY_MIL_COM_SCOPE_TRIGGER_INPUT* cmd_p, TY_MIL_COM_ACK* ack_p);


/*! \brief Sets scope trigger strobe of a board

    This function is called on \ref MIL_COM_SCOPE_TRIG_STROBE command. \n
    It branches off into a platform specific hardware layer function
    \param p_api_dev pointer to target administration structure
    \param cmd_p pointer to \ref TY_MIL_COM_SCOPE_TRG_STROBE_INPUT structure that holds command and trigger strobe settings
    \param ack_p pointer to \ref TY_MIL_COM_ACK structure that receives ack data */
L_WORD mil_scope_trigger_strobe(TY_API_DEV* p_api_dev, TY_MIL_COM_SCOPE_TRG_STROBE_INPUT* cmd_p, TY_MIL_COM_ACK* ack_p);


/*! \brief Resets scope functionality of a board

    This function is called on \ref MIL_COM_SCOPE_RESET command. \n
    It branches off into a platform specific hardware layer function
    \param p_api_dev pointer to target administration structure
    \param cmd_p pointer to \ref MIL_COM structure that holds the command
    \param ack_p pointer to \ref MIL_COM_ACK structure that receives ack data
    \return returns 0 on success, an appropriate error code otherwise */
L_WORD mil_scope_reset(TY_API_DEV* p_api_dev, TY_MIL_COM* cmd_p, TY_MIL_COM_ACK* ack_p);


/*! \brief Sets scope offsets of a board

    This function is called on \ref MIL_COM_SCOPE_OFFSET_COMP command. \n
    It branches off into platform specific hardware layer functions for
    either setting or getting the current offset compensation.values.
    \param p_api_dev pointer to target administration structure
    \param cmd_p pointer to \ref TY_MIL_COM_SCOPE_OFFSET_COMP structure that holds the scope offsets
    \param ack_p pointer to \ref TY_MIL_COM_SCOPE_OFFSET_COMP_OUT structure that receives ack data and scope offsets that were read.
    \return returns 0 on success, an appropriate error code otherwise */
L_WORD mil_scope_offset_comp(TY_API_DEV* p_api_dev, TY_MIL_COM_SCOPE_OFFSET_COMP* cmd_p, TY_MIL_COM_SCOPE_OFFSET_COMP_OUT* ack_p);


void api_scope_command_table( TY_MIL_COM_TABLE_ENTRY ** table, AiUInt32 * size );


/********** Prototypes Old Host to Target Communication ***********************/


/* api_main.c */
L_WORD api_drv                           (TY_API_DEV *p_api_dev, L_WORD mode);
void enter_drv_calc_mem_layout_size      (TY_API_DEV *p_api_dev, TY_MEM_DEFINITION x_MemDef, L_WORD b_IncludeSimBuf, L_WORD *mem_used);
L_WORD enter_drv_check_mem_layout        (TY_API_DEV *p_api_dev, TY_MEM_DEFINITION ax_MemDef[MAX_BIU], L_WORD mem_used[2]);
void enter_drv_set_mem_layout            (TY_API_DEV *p_api_dev, L_WORD mode, L_WORD ul_MemoryBank, TY_MEM_DEFINITION ax_MemDef[MAX_BIU] );
void enter_drv_set_mem_layout_ls         (TY_API_DEV *p_api_dev,   L_WORD ul_MemoryBank, TY_MEM_DEFINITION ax_MemDef[MAX_BIU] );
void enter_drv_dump_mem_layout           (TY_API_DEV *p_api_dev);
void enter_drv                           (TY_API_DEV *p_api_dev);

/* api_opr.c */
void api_opr_ack                         (TY_API_DEV *p_api_dev, BYTE dest, BYTE ack, BYTE *ack_p);
void api_opr                             (TY_API_DEV *p_api_dev, BYTE *cmd_p, BYTE *ack_p);
BYTE api_opr_ls                          (TY_API_DEV *p_api_dev, BYTE *cmd_p, BYTE *ack_p);
BYTE api_opr_hs                          (TY_API_DEV *p_api_dev, BYTE *cmd_p, BYTE *ack_p);

/* api_ini.c */
L_WORD api_ini_cmd(                       TY_API_DEV *p_api_dev, TY_MIL_COM_SYS_INI_INPUT * in, TY_MIL_COM_SYS_INI_OUTPUT * out );
BYTE api_ini_legacy(                      TY_API_DEV *p_api_dev, BYTE mode);                /* required for up/downgrade */


/* TSW internal */
void api_ini_bc_glb_vars(                 TY_API_DEV *p_api_dev);
void api_ini_rt_glb_vars(                 TY_API_DEV *p_api_dev);
void api_ini_rep_glb_vars(                TY_API_DEV *p_api_dev);
void api_ini_bc_queue_glb_vars(           TY_API_DEV *p_api_dev);
void api_ini_ls_systag_glb_vars(          TY_API_DEV *p_api_dev);
void api_ini_bm_glb_vars(                 TY_API_DEV *p_api_dev);
void api_ini_bm_dyntagm_glb_vars(         TY_API_DEV *p_api_dev);
void api_ini_startup(                     TY_API_DEV *p_api_dev);

L_WORD api_ini(                           TY_API_DEV *p_api_dev, BYTE mode);

L_WORD ul_StreamIsMultiChannel(           TY_API_DEV *p_api_dev, WORD uw_Biu);
L_WORD ul_StreamIs3910(                   TY_API_DEV *p_api_dev, WORD uw_Biu);
L_WORD ul_StreamIsEFEX(                   TY_API_DEV *p_api_dev, WORD uw_Biu);
L_WORD ul_StreamIsHS(                     TY_API_DEV *p_api_dev, WORD uw_Biu);
L_WORD get_global_ram_addr_abs(           TY_API_DEV *p_api_dev, L_WORD addr, BYTE biu);
L_WORD get_global_ram_addr_rel(           TY_API_DEV *p_api_dev, L_WORD addr, BYTE biu);
L_WORD ul_ConvertGRAMPtrToTSWPtr(         TY_API_DEV *p_api_dev, WORD uw_Biu, L_WORD ul_Value);
L_WORD ul_ConvertTSWPtrToGRAMPtr(         TY_API_DEV *p_api_dev, WORD uw_Biu, L_WORD ul_Value);
BYTE uc_CalcNumberOfPhysicalBius(         TY_API_DEV *p_api_dev);
BYTE uc_CalcNumberOfStreams(              TY_API_DEV *p_api_dev);
BYTE uc_CalcNumberOfChannels(             TY_API_DEV *p_api_dev);
BYTE uc_GetDeviceType(                    TY_API_DEV *p_api_dev);
AiUInt32 hasElectricalInterface(          TY_API_DEV *p_api_dev, short ls_biu);
void v_InitializeDrvModes(                TY_API_DEV *p_api_dev);
void api_sys_ini_command_table(           TY_MIL_COM_TABLE_ENTRY ** table, AiUInt32 * size );

/* api_sys.c */
BYTE api_sys(                             TY_API_DEV *p_api_dev, L_WORD cmd, WORD size, BYTE *info_p);                /* required for up/downgrade */
BYTE api_sys_reset_legacy(                TY_API_DEV *p_api_dev, BYTE mode);                                        /* required for up/downgrade */
BYTE api_sys_load_srec_legacy(            TY_API_DEV *p_api_dev, BYTE mode, BYTE cnt, L_WORD offset, BYTE *data_p); /* required for up/downgrade */
BYTE api_sys_prog_flash_legacy(           TY_API_DEV *p_api_dev, L_WORD src_addr, L_WORD sect, L_WORD size);        /* required for up/downgrade */
BYTE api_sys_get_mem_partition_legacy(    TY_API_DEV *p_api_dev, BYTE mode );                                       /* required for up/downgrade */
BYTE api_sys_util_legacy(                 TY_API_DEV *p_api_dev, BYTE in_count, BYTE *data_p);                      /* required for up/downgrade */



L_WORD api_sys_ping(                      TY_API_DEV *p_api_dev, TY_MIL_COM_SYS_PING_INPUT               * in, TY_MIL_COM_SYS_PING_OUTPUT             * out );
L_WORD api_sys_malloc(                    TY_API_DEV *p_api_dev, TY_MIL_COM_SYS_MALLOC_INPUT             * in, TY_MIL_COM_SYS_MALLOC_OUTPUT           * out );
L_WORD api_sys_free(                      TY_API_DEV *p_api_dev, TY_MIL_COM_SYS_FREE_INPUT               * in, TY_MIL_COM_ACK                         * out );
L_WORD api_sys_get_board_info(            TY_API_DEV *p_api_dev, TY_MIL_COM_SYS_GET_INFO_INPUT           * in, TY_MIL_COM_SYS_GET_INFO_OUTPUT         * out );
L_WORD api_sys_get_mem_loc(               TY_API_DEV *p_api_dev, TY_MIL_COM_SYS_GET_MEM_LOCATION_INPUT   * in, TY_MIL_COM_SYS_GET_MEM_LOCATION_OUTPUT * out );
L_WORD api_sys_reset_info(                TY_API_DEV *p_api_dev, TY_MIL_COM                              * in, TY_MIL_COM_SYS_RESET_OUTPUT            * out );
L_WORD api_sys_reset_cmd(                 TY_API_DEV *p_api_dev, TY_MIL_COM_SYS_RESET_INPUT              * in, TY_MIL_COM_SYS_RESET_OUTPUT            * out );
L_WORD api_sys_irig_set(                  TY_API_DEV *p_api_dev, TY_MIL_COM_SYS_IRIG_SET_INPUT           * in, TY_MIL_COM_ACK                         * out );
L_WORD api_sys_irig_source_set(           TY_API_DEV *p_api_dev, TY_MIL_COM_SYS_IRIG_SOURCE_SET_INPUT    * in, TY_MIL_COM_ACK                         * out );
L_WORD api_sys_irig_get(                  TY_API_DEV *p_api_dev, TY_MIL_COM                              * in, TY_MIL_COM_SYS_IRIG_GET_OUTPUT         * out );
L_WORD api_sys_irig_status_get(           TY_API_DEV *p_api_dev, TY_MIL_COM                              * in, TY_MIL_COM_SYS_IRIG_STATUS_GET_OUTPUT  * out );
L_WORD api_sys_sync_counter_get(          TY_API_DEV *p_api_dev, TY_MIL_COM                              * in, TY_MIL_COM_SYS_SYNC_COUNTER_GET_OUTPUT * out );
L_WORD api_sys_sync_counter_set(          TY_API_DEV *p_api_dev, TY_MIL_COM_SYS_SYNC_COUNTER_SET_INPUT   * in, TY_MIL_COM_ACK                         * out );
L_WORD api_sys_discretes_init(            TY_API_DEV *p_api_dev, TY_MIL_COM_SYS_DISCRETES_INIT_INPUT     * in, TY_MIL_COM_ACK                         * out );
L_WORD api_sys_discretes_rec_init(        TY_API_DEV *p_api_dev, TY_MIL_COM_SYS_DISCRETES_REC_INIT_INPUT * in, TY_MIL_COM_ACK                         * out );
L_WORD api_sys_discretes_write(           TY_API_DEV *p_api_dev, TY_MIL_COM_SYS_DISCRETES_WRITE_INPUT    * in, TY_MIL_COM_ACK                         * out );
L_WORD api_sys_discretes_read(            TY_API_DEV *p_api_dev, TY_MIL_COM                              * in, TY_MIL_COM_SYS_DISCRETES_READ_OUTPUT   * out );
L_WORD api_sys_discretes_config_read(     TY_API_DEV *p_api_dev, TY_MIL_COM                              * in, TY_MIL_COM_SYS_DISCRETES_CONFIG_READ_OUTPUT   * out );
L_WORD api_sys_discretes_info_read(       TY_API_DEV *p_api_dev, TY_MIL_COM                              * in, TY_MIL_COM_SYS_DISCRETES_INFO_READ_OUTPUT   * out);
L_WORD api_sys_trigger_edge_input_set(TY_API_DEV *p_api_dev, TY_MIL_COM_WITH_VALUE                   * in, TY_MIL_COM_ACK                         * out);
L_WORD api_sys_trigger_edge_input_get(    TY_API_DEV *p_api_dev, TY_MIL_COM                              * in, TY_MIL_COM_ACK_WITH_VALUE              * out );
L_WORD api_sys_trigger_digital_loop_con(  TY_API_DEV *p_api_dev, TY_MIL_COM_WITH_VALUE                   * in, TY_MIL_COM_ACK                         * out );
L_WORD api_sys_trigger_digital_loop_get(  TY_API_DEV *p_api_dev, TY_MIL_COM                              * in, TY_MIL_COM_ACK_WITH_VALUE              * out );
L_WORD api_sys_pxi_con(                   TY_API_DEV *p_api_dev, TY_MIL_COM_SYS_PXI_CON_INPUT            * in, TY_MIL_COM_ACK                         * out );
L_WORD api_sys_pxi_geo_addr(TY_API_DEV *p_api_dev, TY_MIL_COM                                            * in, TY_MIL_COM_ACK_WITH_VALUE              * out );
L_WORD api_sys_def_resp_tout(             TY_API_DEV *p_api_dev, TY_MIL_COM_SYS_RESP_TOUT_DEF_INPUT      * in, TY_MIL_COM_ACK                         * out );
L_WORD api_sys_def_milbus_prot(           TY_API_DEV *p_api_dev, TY_MIL_COM_SYS_MILBUS_PROT_INPUT        * in, TY_MIL_COM_ACK                         * out );
L_WORD api_sys_ls_systag_def(             TY_API_DEV *p_api_dev, TY_MIL_COM_SYS_SYSTAG_DEF_INPUT         * in, TY_MIL_COM_ACK                         * out );
L_WORD api_sys_ls_systag_con(             TY_API_DEV *p_api_dev, TY_MIL_COM_SYS_SYSTAG_CON_INPUT         * in, TY_MIL_COM_ACK                         * out );
L_WORD api_sys_get_mem_part_cmd(          TY_API_DEV *p_api_dev, TY_MIL_COM_SYS_MEM_PART_GET_INPUT       * in, TY_MIL_COM_SYS_MEM_PART_GET_OUTPUT     * out );
L_WORD api_sys_set_mem_part_cmd(          TY_API_DEV *p_api_dev, TY_MIL_COM_SYS_MEM_PART_SET_INPUT       * in, TY_MIL_COM_SYS_MEM_PART_SET_OUTPUT     * out );
L_WORD api_sys_track_def_cmd(             TY_API_DEV *p_api_dev, TY_MIL_COM_LS_SYS_TRACK_DEF_INPUT       * in, TY_MIL_COM_LS_SYS_TRACK_DEF_OUTPUT     * out );
L_WORD api_sys_track_read_cmd(            TY_API_DEV *p_api_dev, TY_MIL_COM_LS_SYS_TRACK_READ_INPUT      * in, TY_MIL_COM_LS_SYS_TRACK_READ_OUTPUT    * out );
L_WORD api_sys_track_scan_cmd(            TY_API_DEV *p_api_dev, TY_MIL_COM_LS_SYS_TRACK_SCAN_INPUT      * in, TY_MIL_COM_LS_SYS_TRACK_SCAN_OUTPUT    * out );
L_WORD api_sys_track_alloc_cmd(           TY_API_DEV *p_api_dev, TY_MIL_COM_LS_SYS_TRACK_ALLOC_INPUT     * in, TY_MIL_COM_LS_SYS_TRACK_ALLOC_OUTPUT   * out );

/* TSW internal functions */
void api_ls_sys_command_table(            TY_MIL_COM_TABLE_ENTRY ** table, AiUInt32 * size );

L_WORD api_sys_reset(                     TY_API_DEV *p_api_dev, BYTE mode);
L_WORD api_sys_get_mem_partition(         TY_API_DEV *p_api_dev, L_WORD mode, TY_API_GET_MEM_INFO * mem_info );
L_WORD api_sys_set_mem_partition(         TY_API_DEV *p_api_dev, L_WORD mode, TY_MEM_DEFINITION ax_MemDef[MAX_BIU], L_WORD * status, L_WORD mem_used[2] );

void api_sys_track_init(                  TY_API_DEV *p_api_dev, BYTE bHs, BYTE bBC, BYTE bRT);
L_WORD api_sys_track_def(                 TY_API_DEV *p_api_dev, BYTE bHs, BYTE uTrackId, BYTE uBt, WORD uXidRtSa, BYTE uTrackMode, WORD uTrackStartPos, BYTE uBpos, BYTE uBlen, WORD uTrackSize, WORD uTrackCount, WORD uTrackFileCount, int l_Offset, L_WORD * buffer_start );
L_WORD api_sys_track_read(                TY_API_DEV *p_api_dev, BYTE bHs, BYTE uTrackId, WORD uMuxId, BYTE uReadMode, L_WORD * valid, L_WORD * ttag, L_WORD * count, WORD * data );
L_WORD api_sys_track_scan(                TY_API_DEV *p_api_dev, BYTE bHs, BYTE uc_TrackId, L_WORD ul_ChunkNb, L_WORD ul_ChunkSize, L_WORD * state_count, L_WORD * more_data, WORD states[] );
L_WORD api_sys_track_prealloc(            TY_API_DEV *p_api_dev, BYTE bHs, L_WORD ul_TrackId, L_WORD ul_PreAllocNb, WORD states[], L_WORD * out_count, L_WORD buffer_start[]  );
L_WORD api_sys_track_remove_list_element( TY_API_DEV *p_api_dev, TY_API_TRACK_DEF *px_Track, TY_API_TRACK_LIST_ELEMENT *ppx_TrackElement );
L_WORD api_sys_track_add_list_element(    TY_API_DEV *p_api_dev, TY_API_TRACK_DEF *px_Track, WORD uMuxId, TY_API_TRACK_LIST_ELEMENT *px_TrackElement );
L_WORD api_sys_track_find_list_element(   TY_API_DEV *p_api_dev, TY_API_TRACK_LIST_ELEMENT *px_TrackListStart, WORD uMuxId, TY_API_TRACK_LIST_ELEMENT **px_TrackElement );

/* api_versions.c */
L_WORD api_sys_read_version(              TY_API_DEV *p_api_dev, TY_MIL_COM_VERSION_INPUT         * cmd_p, TY_MIL_COM_VERSION_OUTPUT         * ack_p );
BYTE api_sys_read_sw_vers_legacy(         TY_API_DEV *p_api_dev); /* Requried for up/downgrade */
BYTE api_sys_read_sw_vers_ex_legacy(      TY_API_DEV *p_api_dev); /* Requried for up/downgrade */

/*TSW internal*/
L_WORD api_sys_read_version_fw(           TY_API_DEV *p_api_dev, TY_E_VERSION_ID eId, TY_API_VER_INFO_VALUES * pxVersionValue, TY_API_VER_INFO_STRINGS * pxVersionString );
L_WORD api_sys_read_version_tsw(          TY_API_DEV *p_api_dev, TY_E_VERSION_ID eId, TY_API_VER_INFO_VALUES * pxVersionValue, TY_API_VER_INFO_STRINGS * pxVersionString );
L_WORD api_sys_read_version_io(           TY_API_DEV *p_api_dev, TY_E_VERSION_ID eId, TY_API_VER_INFO_VALUES * pxVersionValue, TY_API_VER_INFO_STRINGS * pxVersionString );

void api_sys_versions_command_table( TY_MIL_COM_TABLE_ENTRY ** table, AiUInt32 * size );

/* api_utility.c */
L_WORD api_sys_util(                      TY_API_DEV *p_api_dev, TY_MIL_COM_SYS_UTIL_INPUT * in, TY_MIL_COM_SYS_UTIL_OUTPUT * out );

/* TSW internal */
void api_sys_util_command_table( TY_MIL_COM_TABLE_ENTRY ** table, AiUInt32 * size );

/* api_fifo.c */
L_WORD api_sys_ini_fifo(                  TY_API_DEV *p_api_dev, TY_MIL_COM_SYS_FIFO_INI_INPUT       * in, TY_MIL_COM_ACK * out );
L_WORD api_buf_fifo_write(                TY_API_DEV *p_api_dev, TY_MIL_COM_SYS_FIFO_WRITE_INPUT     * in, TY_MIL_COM_ACK * out );
L_WORD api_buf_fifo_status_read(          TY_API_DEV *p_api_dev, TY_MIL_COM_SYS_FIFO_STATUS_INPUT    * in, TY_MIL_COM_SYS_FIFO_STATUS_OUTPUT * out );
L_WORD api_rt_fifo_assign(                TY_API_DEV *p_api_dev, TY_MIL_COM_SYS_FIFO_RT_ASSIGN_INPUT * in, TY_MIL_COM_ACK * out );
L_WORD api_bc_fifo_assign(                TY_API_DEV *p_api_dev, TY_MIL_COM_SYS_FIFO_BC_ASSIGN_INPUT * in, TY_MIL_COM_ACK * out );



/* TSW internal functions */
void api_sys_fifo_command_table(          TY_MIL_COM_TABLE_ENTRY ** table, AiUInt32 * size );


/* api_exec_sys.c */
L_WORD api_sys_exec(                      TY_API_DEV *p_api_dev, TY_MIL_COM_SYS_EXEC_INPUT    * in, TY_MIL_COM_ACK * out );

/* TSW internal */
void api_sys_exec_command_table( TY_MIL_COM_TABLE_ENTRY ** table, AiUInt32 * size );

/* api_dts.c */
L_WORD api_gen_io_test_seq(               TY_API_DEV *p_api_dev, TY_MIL_COM_CUSTOM_DTS_GEN_IO_TEST_SEQ_INPUT * pxInput, TY_MIL_COM_CUSTOM_DTS_GEN_IO_TEST_SEQ_OUTPUT * pxOutput );
BYTE api_sys_dts(                         TY_API_DEV *p_api_dev, L_WORD cmd, WORD size, BYTE *info_p );

void api_custom_dts_command_table(        TY_MIL_COM_TABLE_ENTRY ** table, AiUInt32 * size );

#ifdef _CMPL4DTS
void api_sys_drv_glb_init(void);
void api_sys_drv_rec_con(L_WORD con);
BYTE api_sys_gen_io_read                 (TY_API_DEV *p_api_dev, L_WORD addr, L_WORD res );
void api_gen_io_test_seq_handle_event    (TY_API_DEV *p_api_dev, L_WORD timeIncr /* time increment since last call*/);
BYTE api_sys_ip_addr_init                (TY_API_DEV *p_api_dev, BYTE ip, BYTE type, BYTE chn, BYTE res, L_WORD addr);
BYTE api_sys_ip_out_chn_write            (TY_API_DEV *p_api_dev, BYTE ip, BYTE chn, L_WORD res, L_WORD val); 
BYTE api_sys_ip_out_chn_read             (TY_API_DEV *p_api_dev, BYTE ip, BYTE chn, L_WORD res); 
BYTE api_sys_ip_in_chn_read              (TY_API_DEV *p_api_dev, BYTE ip, BYTE chn, L_WORD res); 
BYTE api_sys_ip_stask_ctrl               (TY_API_DEV *p_api_dev, BYTE tmr, BYTE con, L_WORD rate, L_WORD res1, L_WORD res2);

void api_io_dig_out_write_ip408          (L_WORD id, L_WORD val);
void api_io_dig_out_write_ip470          (L_WORD id, L_WORD val, L_WORD offset );
L_WORD api_io_dig_out_read_ip408         (L_WORD id);
L_WORD api_io_dig_in_read_ip408          (L_WORD id);

void api_io_ana_out_write_ip850          (L_WORD id,L_WORD chn, L_WORD val);
L_WORD api_io_ana_in_read_ip850          (L_WORD id,L_WORD chn);

void api_io_dig_out_write_apx            (L_WORD id, L_WORD val);
#endif

/* api_bite.c */
L_WORD api_bite(                          TY_API_DEV *p_api_dev, TY_MIL_COM_SYS_BITE_INPUT * in, TY_MIL_COM_SYS_BITE_OUTPUT * out );


/* TSW internal */
void api_sys_bite_command_table(          TY_MIL_COM_TABLE_ENTRY ** table, AiUInt32 * size );

BYTE api_bite_board_en_test(              TY_API_DEV *p_api_dev);
BYTE api_bite_int_selftest(               TY_API_DEV *p_api_dev);
BYTE api_bite_paging_test(                TY_API_DEV *p_api_dev, L_WORD offset, BYTE mode);
void api_bite_fill_ram(                   TY_API_DEV *p_api_dev, L_WORD saddr, L_WORD eaddr, L_WORD fill, BYTE mode);
BYTE api_bite_rdtst_ram(                  TY_API_DEV *p_api_dev, L_WORD saddr, L_WORD eaddr, L_WORD fill, BYTE mode);
BYTE api_bite_dat_pat(                    TY_API_DEV *p_api_dev, L_WORD saddr, L_WORD eaddr, BYTE mode);
BYTE api_bite_walk_bit(                   TY_API_DEV *p_api_dev, L_WORD saddr, L_WORD eaddr, BYTE mode);
BYTE api_bite_addr_test(                  TY_API_DEV *p_api_dev, L_WORD saddr, L_WORD eaddr, BYTE mode);
BYTE api_bite_bus_test(                   TY_API_DEV *p_api_dev, L_WORD saddr, L_WORD eaddr, BYTE mode);
BYTE api_bite_ramtest(                    TY_API_DEV *p_api_dev, L_WORD saddr, L_WORD eaddr, BYTE mode);
BYTE api_bite_bc_br_test(                 TY_API_DEV *p_api_dev, BYTE mode, BYTE bus, BYTE con);
BYTE api_bite_bc_rt_test(                 TY_API_DEV *p_api_dev, BYTE mode, BYTE con);
BYTE api_bite_rt_rt_test(                 TY_API_DEV *p_api_dev, BYTE mode, BYTE bus, BYTE con);
BYTE api_bite_mc_test(                    TY_API_DEV *p_api_dev, BYTE mode, BYTE bus, BYTE con);
BYTE api_bite_bc_resp_tout_test(          TY_API_DEV *p_api_dev, BYTE mode, BYTE bus, BYTE con);
BYTE api_bite_bc_intr_test(               TY_API_DEV *p_api_dev);
BYTE api_bite_dac_test(                   TY_API_DEV *p_api_dev, BYTE mode, BYTE bus, BYTE con);
BYTE api_bite_minor_framing_time_test(    TY_API_DEV *p_api_dev, BYTE mode, BYTE bus, L_WORD ftime, BYTE con);
BYTE api_bite_transfer_test(              TY_API_DEV *p_api_dev);
BYTE api_bite_timing_test(                TY_API_DEV *p_api_dev);


/* api_rep.c */
L_WORD api_rep_ini(                       TY_API_DEV *p_api_dev, TY_MIL_COM_REP_INI_INPUT     * in, TY_MIL_COM_ACK * out );
L_WORD api_rep_start_cmd(                 TY_API_DEV *p_api_dev, TY_MIL_COM                   * in, TY_MIL_COM_ACK * out );
L_WORD api_rep_halt_cmd(                  TY_API_DEV *p_api_dev, TY_MIL_COM                   * in, TY_MIL_COM_ACK * out );
L_WORD api_rep_rt_con(                    TY_API_DEV *p_api_dev, TY_MIL_COM_REP_RT_CON_INPUT  * in, TY_MIL_COM_ACK * out );
L_WORD api_rep_status(                    TY_API_DEV *p_api_dev, TY_MIL_COM                   * in, TY_MIL_COM_REP_STATUS_OUTPUT * out );

/* TSW internal */
void api_rep_command_table(               TY_MIL_COM_TABLE_ENTRY ** table, AiUInt32 * size );
L_WORD api_rep_start(                     TY_API_DEV *p_api_dev );
L_WORD api_rep_halt(                      TY_API_DEV *p_api_dev );

void api_io_rep_ini(                      TY_API_DEV *p_api_dev, BYTE cet, BYTE nct, BYTE cyc, BYTE nimg, BYTE alt, BYTE rlt, BYTE rint, L_WORD min, L_WORD msec, BYTE sign, L_WORD fsize );
void api_io_rep_rt_con(                   TY_API_DEV *p_api_dev, BYTE con, BYTE mode, BYTE rt );
void api_io_rep_buf_ini(                  TY_API_DEV *p_api_dev );



/* api_buf.c */
L_WORD api_buf_def(                       TY_API_DEV *p_api_dev, TY_MIL_COM_LS_BUF_DEF_INPUT           * in, TY_MIL_COM_LS_BUF_DEF_OUTPUT          * out );
L_WORD api_buf_read(                      TY_API_DEV *p_api_dev, TY_MIL_COM_LS_BUF_READ_INPUT          * in, TY_MIL_COM_LS_BUF_READ_OUTPUT         * out );
L_WORD api_buf_write(                     TY_API_DEV *p_api_dev, TY_MIL_COM_LS_BUF_WRITE_INPUT         * in, TY_MIL_COM_ACK                        * out );
L_WORD api_buf_write_block(               TY_API_DEV* p_api_dev, TY_MIL_COM_LS_BUF_WRITE_BLOCK_INPUT   * in, TY_MIL_COM_ACK                        * out );
L_WORD api_buf_c1760_con(                 TY_API_DEV *p_api_dev, TY_MIL_COM_LS_BUF_C1760_CON_INPUT     * in, TY_MIL_COM_ACK                        * out );
L_WORD api_buf_dataset_write(             TY_API_DEV *p_api_dev, TY_MIL_COM_LS_BUF_DATASET_WRITE_INPUT * in, TY_MIL_COM_ACK                        * out );
L_WORD api_buf_dataset_read(              TY_API_DEV *p_api_dev, TY_MIL_COM_LS_BUF_DATASET_READ_INPUT  * in, TY_MIL_COM_LS_BUF_DATASET_READ_OUTPUT * out );
L_WORD api_buf_host_control(              TY_API_DEV* p_api_dev, TY_MIL_COM_LS_BUF_HOST_CONTROL_INPUT  * in, TY_MIL_COM_ACK                        * out );


BYTE api_buf_ram_write                   (TY_API_DEV *p_api_dev, L_WORD addr, BYTE len, BYTE *data_p);
BYTE api_buf_ram_read                    (TY_API_DEV *p_api_dev, L_WORD addr, BYTE len);
BYTE api_buf_ram_wr_lword                (TY_API_DEV *p_api_dev, L_WORD addr, L_WORD lval);
BYTE api_buf_ram_wr_word                 (TY_API_DEV *p_api_dev, L_WORD addr, WORD wval);
BYTE api_buf_ram_wr_byte                 (TY_API_DEV *p_api_dev, L_WORD addr, BYTE bval);
BYTE api_buf_ram_rd_lword                (TY_API_DEV *p_api_dev, L_WORD addr);
BYTE api_buf_ram_rd_word                 (TY_API_DEV *p_api_dev, L_WORD addr);
BYTE api_buf_ram_rd_byte                 (TY_API_DEV *p_api_dev, L_WORD addr);
BYTE api_buf(TY_API_DEV *p_api_dev, BYTE cmd, WORD size, BYTE *info_p);

/* TSW internal */
void api_ls_buf_command_table(            TY_MIL_COM_TABLE_ENTRY ** table, AiUInt32 * size );

/* api_bc.c */
  //#ifdef CMPL4SIM
L_WORD api_bc_ini_cmd(                   TY_API_DEV *p_api_dev, TY_MIL_COM_LS_BC_INI_INPUT * in, TY_MIL_COM_ACK * out);
L_WORD api_bc_bhd_def_cmd(               TY_API_DEV *p_api_dev, TY_MIL_COM_LS_BC_BH_DEF_INPUT * in, TY_MIL_COM_LS_BC_BH_DEF_OUTPUT * out );
L_WORD api_bc_xfer_def_cmd(              TY_API_DEV *p_api_dev, TY_MIL_COM_LS_BC_XFER_DEF_INPUT * in, TY_MIL_COM_LS_BC_XFER_DEF_OUTPUT * out );
L_WORD api_bc_xfer_ctrl(                 TY_API_DEV *p_api_dev, TY_MIL_COM_LS_BC_XFER_CTRL_INPUT * in, TY_MIL_COM_ACK * out );
L_WORD api_bc_start_cmd(                 TY_API_DEV *p_api_dev, TY_MIL_COM_LS_BC_START_INPUT * in, TY_MIL_COM_LS_BC_START_OUTPUT * out );
L_WORD api_bc_halt_cmd(                  TY_API_DEV *p_api_dev, TY_MIL_COM * in, TY_MIL_COM_ACK * out );
L_WORD api_bc_xfer_def_get(              TY_API_DEV *p_api_dev, TY_MIL_COM_LS_BC_XFER_DEF_GET_INPUT * in, TY_MIL_COM_LS_BC_XFER_DEF_GET_OUTPUT * out);
L_WORD api_bc_bhd_read(                  TY_API_DEV *p_api_dev, TY_MIL_COM_LS_BC_BH_READ_INPUT * in, TY_MIL_COM_LS_BC_BH_READ_OUTPUT * out);
L_WORD api_bc_xfer_bh_info_get(          TY_API_DEV *p_api_dev, TY_MIL_COM_LS_BC_BH_INFO_INPUT * in, TY_MIL_COM_LS_BC_BH_INFO_OUTPUT * out);
L_WORD api_bc_major_frame_get(           TY_API_DEV *p_api_dev, TY_MIL_COM * in, TY_MIL_COM_LS_BC_MAJOR_FRAME_GET_OUTPUT *out);
L_WORD api_bc_minor_frame_get(           TY_API_DEV *p_api_dev, TY_MIL_COM_LS_BC_MINOR_FRAME_GET_INPUT * in, TY_MIL_COM_LS_BC_MINOR_FRAME_GET_OUTPUT *out);
L_WORD api_bc_dytag_def(                 TY_API_DEV *p_api_dev, TY_MIL_COM_LS_BC_DYTAG_DEF_INPUT * in, TY_MIL_COM_ACK * out );
L_WORD api_bc_dytag_def_get(             TY_API_DEV *p_api_dev, TY_MIL_COM_LS_BC_DYTAG_DEF_GET_INPUT * in, TY_MIL_COM_LS_BC_DYTAG_DEF_GET_OUTPUT * out );
L_WORD api_bc_frame_def_cmd(             TY_API_DEV *p_api_dev, TY_MIL_COM_LS_BC_MINOR_FRAME_DEF_INPUT * in, TY_MIL_COM_ACK *out );
L_WORD api_bc_mframe_def_cmd(            TY_API_DEV *p_api_dev, TY_MIL_COM_LS_BC_MAJOR_FRAME_DEF_INPUT * in, TY_MIL_COM_ACK * out );
L_WORD api_bc_status_read_cmd(           TY_API_DEV *p_api_dev, TY_MIL_COM * in, TY_MIL_COM_LS_BC_STATUS_READ_OUTPUT * out );
L_WORD api_bc_xfer_read_cmd(             TY_API_DEV *p_api_dev, TY_MIL_COM_LS_BC_XFER_READ_INPUT * in, TY_MIL_COM_LS_BC_XFER_READ_OUTPUT * out );
L_WORD api_bc_srvw_req_read(             TY_API_DEV *p_api_dev, TY_MIL_COM_LS_BC_SRVW_REQ_READ_INPUT * in, TY_MIL_COM_LS_BC_SRVW_REQ_READ_OUTPUT * out );
L_WORD api_bc_srvw_req_con(              TY_API_DEV *p_api_dev, TY_MIL_COM_LS_BC_SRVW_REQ_CON_INPUT * in, TY_MIL_COM_ACK * out );
L_WORD api_bc_acyc_msg_prep(             TY_API_DEV *p_api_dev, TY_MIL_COM_LS_BC_ACYC_PREP_INPUT * in, TY_MIL_COM_ACK *out );
L_WORD api_bc_acyc_msg_send_cmd(         TY_API_DEV *p_api_dev, TY_MIL_COM_LS_BC_ACYC_SEND_INPUT * in, TY_MIL_COM_ACK *out );
L_WORD api_bc_mode_ctrl(                 TY_API_DEV *p_api_dev, TY_MIL_COM_LS_BC_MODE_CTRL_INPUT * in, TY_MIL_COM_ACK * out);
L_WORD api_bc_modify_def(                TY_API_DEV *p_api_dev, TY_MIL_COM_LS_BC_MODIFY_DEF_INPUT * in, TY_MIL_COM_LS_BC_MODIFY_DEF_OUTPUT * out);
L_WORD api_bc_traffic_override_control( TY_API_DEV *p_api_dev, TY_MIL_COM_WITH_VALUE * in, TY_MIL_COM_ACK * out);
L_WORD api_bc_traffic_override_get(     TY_API_DEV *p_api_dev, TY_MIL_COM * in, TY_MIL_COM_ACK_WITH_VALUE * out);



/* -- TSW internal --- */
void api_ls_bc_command_table( TY_MIL_COM_TABLE_ENTRY ** table, AiUInt32 * size );
BYTE api_bc_ini(                         TY_API_DEV *p_api_dev, BYTE retr, BYTE svrq, BYTE tbm, BYTE gsb);
L_WORD api_x_bc_ini(TY_API_DEV *p_api_dev, BYTE retr, BYTE svrq, BYTE tbm, BYTE gsb);
L_WORD api_bc_bhd_def(                   TY_API_DEV *p_api_dev, WORD hid, WORD bid, WORD sid, WORD eid, BYTE qsize, BYTE bqm, BYTE bsm, BYTE sts, BYTE evs, BYTE dbm);
L_WORD api_bc_xfer_def(                  TY_API_DEV *p_api_dev, WORD xid, WORD hid, BYTE xfer_type, BYTE chn, BYTE xmt_rt, BYTE rcv_rt, BYTE xmt_sa, BYTE rcv_sa, BYTE wcnt, BYTE xfer_ir, BYTE xfer_hlt, BYTE rte, BYTE sxh, BYTE rsp, BYTE res, WORD swxm, BYTE type, BYTE wpos, BYTE bpos, BYTE sync, BYTE contig, BYTE bc_bits, BYTE delay_mode, WORD gap, L_WORD *addr);
L_WORD api_bc_mode(                      TY_API_DEV *p_api_dev, BYTE con, BYTE smod, L_WORD cnt, L_WORD ftime, L_WORD saddr);
L_WORD api_bc_frame_def(                 TY_API_DEV *p_api_dev, BYTE frame_id, BYTE xfer_cnt, TY_BC_INSTRUCTION *input);
L_WORD api_bc_mframe_def(                TY_API_DEV *p_api_dev, L_WORD frame_cnt, L_WORD *frame_ids);
L_WORD api_bc_status_read(               TY_API_DEV *p_api_dev);
L_WORD api_bc_xfer_read(                 TY_API_DEV *p_api_dev, WORD xid, WORD clr_mode);
void api_bc_acyc_check_ttag(             TY_API_DEV *p_api_dev);
L_WORD api_bc_acyc_prep_and_send_transfer(TY_API_DEV *p_api_dev, MIL_COM_LS_BC_ACYC_PREP_AND_SEND_TRANSFER_INPUT * in, MIL_COM_LS_BC_ACYC_PREP_AND_SEND_TRANSFER_OUTPUT * out);



  //#endif 

/* api_rt.c */
  #ifdef CMPL4SIM
L_WORD api_rt_ini_cmd(                    TY_API_DEV *p_api_dev, TY_MIL_COM_LS_RT_INI_INPUT          * in, TY_MIL_COM_ACK * out );
L_WORD api_rt_con_cmd(                    TY_API_DEV *p_api_dev, TY_MIL_COM_LS_RT_CON_INPUT          * in, TY_MIL_COM_ACK * out );
L_WORD api_rt_ini_resp(                   TY_API_DEV *p_api_dev, TY_MIL_COM_LS_RT_INI_RESP_INPUT     * in, TY_MIL_COM_ACK * out );
L_WORD api_rt_ini_nxw(                    TY_API_DEV *p_api_dev, TY_MIL_COM_LS_RT_INI_NXW_INPUT      * in, TY_MIL_COM_ACK * out );
L_WORD api_rt_ini_lcw(                    TY_API_DEV *p_api_dev, TY_MIL_COM_LS_RT_INI_LCW_INPUT      * in, TY_MIL_COM_ACK * out );
L_WORD api_rt_ini_lsw(                    TY_API_DEV *p_api_dev, TY_MIL_COM_LS_RT_INI_LSW_INPUT      * in, TY_MIL_COM_ACK * out );
L_WORD api_rt_bhd_def(                    TY_API_DEV *p_api_dev, TY_MIL_COM_LS_RT_BH_DEF_INPUT       * in, TY_MIL_COM_LS_RT_BH_DEF_OUTPUT * out );
L_WORD api_rt_sa_con_cmd(                 TY_API_DEV *p_api_dev, TY_MIL_COM_LS_RT_SA_CON_INPUT       * in, TY_MIL_COM_ACK * out );
L_WORD api_rt_sa_err_con(                 TY_API_DEV *p_api_dev, TY_MIL_COM_LS_RT_SA_ERR_CON_INPUT   * in, TY_MIL_COM_ACK * out );
L_WORD api_rt_sa_err_get(                 TY_API_DEV *p_api_dev, TY_MIL_COM_LS_RT_SA_ERR_GET_INPUT   * in, TY_MIL_COM_LS_RT_SA_ERR_GET_OUTPUT * out );
L_WORD api_rt_dytag_def(                  TY_API_DEV *p_api_dev, TY_MIL_COM_LS_RT_DYTAG_DEF_INPUT    * in, TY_MIL_COM_ACK * out );
L_WORD api_rt_dytag_get(                  TY_API_DEV *p_api_dev, TY_MIL_COM_LS_RT_DYTAG_GET_INPUT    * in, TY_MIL_COM_LS_RT_DYTAG_GET_OUTPUT * out );
L_WORD api_rt_start(                      TY_API_DEV *p_api_dev, TY_MIL_COM                          * in, TY_MIL_COM_ACK * out );
L_WORD api_rt_halt(                       TY_API_DEV *p_api_dev, TY_MIL_COM                          * in, TY_MIL_COM_ACK * out );
L_WORD api_rt_status_read(                TY_API_DEV *p_api_dev, TY_MIL_COM                          * in, TY_MIL_COM_LS_RT_STATUS_READ_OUTPUT * out );
L_WORD api_rt_msg_read(                   TY_API_DEV *p_api_dev, TY_MIL_COM_LS_RT_MSG_READ_INPUT     * in, TY_MIL_COM_LS_RT_MSG_READ_OUTPUT * out );
L_WORD api_rt_status_read(                TY_API_DEV *p_api_dev, TY_MIL_COM                          * in, TY_MIL_COM_LS_RT_STATUS_READ_OUTPUT * out );
L_WORD api_rt_msg_read_all(               TY_API_DEV *p_api_dev, TY_MIL_COM                          * in, TY_MIL_COM_LS_RT_MSG_READ_ALL_OUTPUT * out );
L_WORD api_rt_sa_msg_read(                TY_API_DEV *p_api_dev, TY_MIL_COM_LS_RT_SA_MSG_READ_INPUT  * in, TY_MIL_COM_LS_RT_SA_MSG_READ_OUTPUT * out );
L_WORD api_rt_global_con(                 TY_API_DEV *p_api_dev, TY_MIL_COM_LS_RT_GLOBAL_CON_INPUT   * in, TY_MIL_COM_ACK * out );
L_WORD api_rt_get_sim_info(               TY_API_DEV *p_api_dev, TY_MIL_COM_LS_RT_GET_SIM_INFO_INPUT * in, TY_MIL_COM_LS_RT_GET_SIM_INFO_OUTPUT * out );
L_WORD api_rt_sa_bh_get(                  TY_API_DEV *p_api_dev, TY_MIL_COM_LS_RT_SA_BH_GET_INPUT    * in, TY_MIL_COM_LS_RT_SA_BH_GET_OUTPUT    * out );
L_WORD api_rt_bhd_read(                   TY_API_DEV *p_api_dev, TY_MIL_COM_LS_RT_BH_GET_INPUT       * in, TY_MIL_COM_LS_RT_BH_DEF_OUTPUT       * out );
L_WORD api_rt_mode_ctrl(                  TY_API_DEV *p_api_dev, TY_MIL_COM_LS_RT_MODE_CTRL_INPUT    * in, TY_MIL_COM_ACK * out);
L_WORD api_rt_dwc_set(                    TY_API_DEV *p_api_dev, TY_MIL_COM_LS_RT_DWC_SET_INPUT      * in, TY_MIL_COM_ACK * out);
L_WORD api_rt_dwc_get(                    TY_API_DEV *p_api_dev, TY_MIL_COM_LS_RT_DWC_GET_INPUT      * in, TY_MIL_COM_LS_RT_DWC_GET_OUTPUT * out);

/* -- TSW internal --- */
L_WORD api_rt_mode(                       TY_API_DEV *p_api_dev, BYTE con );
L_WORD api_rt_ini(                        TY_API_DEV *p_api_dev, BYTE rt_addr, BYTE con, BYTE bus, BYTE resp_time, WORD nxw );
L_WORD api_rt_con(                        TY_API_DEV *p_api_dev, BYTE rt_addr, BYTE con);
L_WORD api_rt_sa_con(                     TY_API_DEV *p_api_dev, BYTE rt_addr, BYTE sa_mc, WORD hid, BYTE sa_type, BYTE con, BYTE rmod, BYTE smod, WORD swm);


void api_ls_rt_command_table( TY_MIL_COM_TABLE_ENTRY ** table, AiUInt32 * size );



  #endif 

/* api_bm.c */
  #ifdef CMPL4MON
L_WORD api_bm_ini_cmd(                   TY_API_DEV *p_api_dev, TY_MIL_COM * in, TY_MIL_COM_ACK * out );
L_WORD api_bm_ini_intr_cmd(              TY_API_DEV *p_api_dev, TY_MIL_COM_LS_BM_INI_INTR_INPUT * in, TY_MIL_COM_ACK * out );
L_WORD api_bm_ini_cap_cmd(               TY_API_DEV *p_api_dev, TY_MIL_COM_LS_BM_INI_CAP_INPUT * in, TY_MIL_COM_ACK * out );
L_WORD api_bm_ini_swxm(                  TY_API_DEV *p_api_dev, TY_MIL_COM_LS_BM_INI_SWXM_INPUT * in, TY_MIL_COM_ACK * out );
L_WORD api_bm_start(                     TY_API_DEV *p_api_dev, TY_MIL_COM * in, TY_MIL_COM_ACK * out );
L_WORD api_bm_halt(                      TY_API_DEV *p_api_dev, TY_MIL_COM * in, TY_MIL_COM_ACK * out );
L_WORD api_bm_status_read(               TY_API_DEV *p_api_dev, TY_MIL_COM * in, TY_MIL_COM_LS_BM_STATUS_OUTPUT * out );
L_WORD api_bm_act_read(                  TY_API_DEV *p_api_dev, TY_MIL_COM_LS_BM_ACT_READ_INPUT * in, TY_MIL_COM_LS_BM_ACT_READ_OUTPUT * out );
L_WORD api_bm_stack_ptr_read(            TY_API_DEV *p_api_dev, TY_MIL_COM * in, TY_MIL_COM_LS_BM_STACK_PTR_READ_OUTPUT * out );
L_WORD api_bm_stack_entry_read(          TY_API_DEV *p_api_dev, TY_MIL_COM_LS_BM_STACK_ENTRY_READ_INPUT *in, TY_MIL_COM_LS_BM_STACK_ENTRY_READ_OUTPUT *out);
L_WORD api_bm_stack_entry_find(          TY_API_DEV *p_api_dev, TY_MIL_COM_LS_BM_STACK_ENTRY_FIND_INPUT * in, TY_MIL_COM_LS_BM_STACK_ENTRY_FIND_OUTPUT * out);
L_WORD api_bm_rec_status_read(           TY_API_DEV *p_api_dev, TY_MIL_COM * in, TY_MIL_COM_LS_BM_REC_STATUS_OUTPUT * out );
L_WORD api_bm_copy_rec_dat(              TY_API_DEV *p_api_dev, TY_MIL_COM_LS_BM_COPY_REC_DAT_INPUT * in, TY_MIL_COM_ACK * out );
L_WORD api_bm_ini_msg_flt(               TY_API_DEV *p_api_dev, TY_MIL_COM_LS_BM_INI_MSG_FLT_INPUT * in, TY_MIL_COM_ACK * out);
L_WORD api_bm_ini_msg_ill(               TY_API_DEV *p_api_dev, TY_MIL_COM_LS_BM_INI_MSG_ILL_INPUT * in, TY_MIL_COM_ACK * out);
L_WORD api_bm_ini_ftw(                   TY_API_DEV *p_api_dev, TY_MIL_COM_LS_BM_INI_FTW_INPUT * in, TY_MIL_COM_ACK * out);
L_WORD api_bm_ini_tiw(                   TY_API_DEV *p_api_dev, TY_MIL_COM_LS_BM_INI_TIW_INPUT * in, TY_MIL_COM_ACK * out);
L_WORD api_bm_ini_tci(                   TY_API_DEV *p_api_dev, TY_MIL_COM_LS_BM_INI_TCI_INPUT * in, TY_MIL_COM_ACK * out);
L_WORD api_bm_ini_tcb(                   TY_API_DEV *p_api_dev, TY_MIL_COM_LS_BM_INI_TCB_INPUT * in, TY_MIL_COM_ACK * out );
L_WORD api_bm_rt_act_read(               TY_API_DEV *p_api_dev, TY_MIL_COM_LS_BM_RT_ACT_READ_INPUT * in, TY_MIL_COM_LS_BM_RT_ACT_READ_OUTPUT * out );
L_WORD api_bm_rt_sa_act_read_cmd(        TY_API_DEV *p_api_dev, TY_MIL_COM_LS_BM_RT_SA_ACT_READ_INPUT * in, TY_MIL_COM_LS_BM_RT_SA_ACT_READ_OUTPUT * out );
L_WORD api_bm_ini_msg_flt_rec(           TY_API_DEV *p_api_dev, TY_MIL_COM_LS_BM_INI_MSG_FLT_REC_INPUT * in, TY_MIL_COM_ACK * out);
L_WORD api_bm_read_msg_flt_rec(          TY_API_DEV *p_api_dev, TY_MIL_COM_LS_BM_MSG_FLT_REC_INPUT * in, TY_MIL_COM_LS_BM_MSG_FLT_REC_OUTPUT * out );
L_WORD api_bm_dyntag_mon_def(            TY_API_DEV *p_api_dev, TY_MIL_COM_LS_BM_DYNTAG_DEF_INPUT * in, TY_MIL_COM_ACK * out );
L_WORD api_bm_dyntag_mon_read(           TY_API_DEV *p_api_dev, TY_MIL_COM_LS_BM_DYNTAG_MON_READ_INPUT * in, TY_MIL_COM_LS_BM_DYNTAG_MON_READ_OUTPUT * out );


void api_ls_bm_command_table( TY_MIL_COM_TABLE_ENTRY ** table, AiUInt32 * size );
L_WORD api_bm_stack_ptr_read_ir(TY_API_DEV *p_api_dev, short dest_biu, TY_MIL_COM_LS_BM_STACK_PTR_READ_OUTPUT * out);


/* -- TSW internal --- */
L_WORD api_bm_ini(                        TY_API_DEV *p_api_dev);
L_WORD api_bm_ini_intr(                   TY_API_DEV *p_api_dev, BYTE imod, BYTE smod);
L_WORD api_bm_ini_cap(                    TY_API_DEV *p_api_dev, BYTE cmod, L_WORD tat, WORD mcc, WORD fsize);
L_WORD api_bm_mode(                       TY_API_DEV *p_api_dev, BYTE con);
L_WORD api_bm_stack_stp_get(              TY_API_DEV *p_api_dev, short dest_biu, L_WORD ctp_addr, L_WORD etp_addr);
L_WORD api_bm_rt_sa_act_read(             TY_API_DEV *p_api_dev, BYTE rt, BYTE sa, BYTE sa_type);
  #endif



/* api_dqueue.c */

L_WORD api_data_queue_open(              TY_API_DEV *p_api_dev, TY_MIL_COM_LS_BM_DATAQUEUE_OPEN_INPUT * in, TY_MIL_COM_LS_BM_DATAQUEUE_OPEN_OUTPUT * out );
L_WORD api_data_queue_control(           TY_API_DEV *p_api_dev, TY_MIL_COM_LS_BM_DATAQUEUE_CONTROL_INPUT * in, TY_MIL_COM_ACK * out );
L_WORD api_data_queue_close(             TY_API_DEV *p_api_dev, TY_MIL_COM_LS_BM_DATAQUEUE_CLOSE_INPUT * in, TY_MIL_COM_ACK * out );

L_WORD ul_ApiDataQueueMemAddr            (TY_API_DEV *p_api_dev, L_WORD uc_Id, short biu);

void api_ls_bm_dataqueue_command_table( TY_MIL_COM_TABLE_ENTRY ** table, AiUInt32 * size );


/* api_cal.c */

L_WORD api_cal_sig_con(                   TY_API_DEV *p_api_dev, TY_MIL_COM_LS_SYS_CAL_CON_INPUT   * in, TY_MIL_COM_ACK * out );
L_WORD api_cal_cpl_con_cmd(               TY_API_DEV *p_api_dev, TY_MIL_COM_LS_SYS_CAL_CON_INPUT   * in, TY_MIL_COM_ACK * out );
L_WORD api_cal_xmt_con(                   TY_API_DEV *p_api_dev, TY_MIL_COM_LS_SYS_CAL_CON_INPUT   * in, TY_MIL_COM_ACK * out );
L_WORD api_cal_trans_con(                 TY_API_DEV *p_api_dev, TY_MIL_COM_LS_SYS_CAL_TRANS_INPUT * in, TY_MIL_COM_ACK * out );

/* TSW internale */
L_WORD api_cal_cpl_con(                   TY_API_DEV *p_api_dev, BYTE bus, BYTE cpl);

void api_ls_sys_cal_command_table( TY_MIL_COM_TABLE_ENTRY ** table, AiUInt32 * size );


/* api_io.c */

/* Buffer functions */
void api_io_write_lword                  (TY_API_DEV *p_api_dev, L_WORD addr, L_WORD lval);
L_WORD api_io_read_lword                 (TY_API_DEV *p_api_dev, L_WORD addr);
void api_io_write_word                   (TY_API_DEV *p_api_dev, L_WORD addr, WORD wval);
WORD api_io_read_word                    (TY_API_DEV *p_api_dev, L_WORD addr);
void api_io_write_byte                   (TY_API_DEV *p_api_dev, L_WORD addr, BYTE bval);
BYTE api_io_read_byte                    (TY_API_DEV *p_api_dev, L_WORD addr);

/* System Control Block */
void api_io_mode_con                     (TY_API_DEV *p_api_dev, BYTE mode, BYTE con);
short api_io_test_biu                    (TY_API_DEV *p_api_dev);
void api_io_cb_ini                       (TY_API_DEV *p_api_dev, short mode);
void api_io_resp_tout_def                (TY_API_DEV *p_api_dev, BYTE tout);
void api_io_milbus_prot_def              (TY_API_DEV *p_api_dev, BYTE mil_prot, BYTE mode, BYTE rt); 
void api_io_milprot_ini                  (TY_API_DEV *p_api_dev); 
void api_io_set_special_op_mode          (TY_API_DEV *p_api_dev, AiUInt32 mode);


/* Interrupt Loglist */
void api_io_islp_ini                     (TY_API_DEV *p_api_dev);

/* Simulator Buffer */
void api_io_sim_buf_ini                  (TY_API_DEV *p_api_dev);

/* Calibration functions */
void api_io_cal_ini                      (TY_API_DEV *p_api_dev);
void api_io_cal_sig_con                  (TY_API_DEV *p_api_dev, BYTE bus, BYTE con);
void api_io_cal_cpl_con                  (TY_API_DEV *p_api_dev, BYTE bus, BYTE cpl);
void api_io_cal_xmt_con                  (TY_API_DEV *p_api_dev, BYTE bus, BYTE amp);
void api_io_cal_trans_con                (TY_API_DEV *p_api_dev, BYTE con);

  #ifdef CMPL4SIM
/* Remote Terminal functions */
void api_io_rt_ir_clr                    (TY_API_DEV *p_api_dev, BYTE rt_addr);
BYTE api_io_rt_ir_get                    (TY_API_DEV *p_api_dev, BYTE rt_addr);
void api_io_rt_glb_cnt_ini               (TY_API_DEV *p_api_dev);
void api_io_rt_msg_cnt_ini               (TY_API_DEV *p_api_dev, BYTE rt_addr);
void api_io_rt_desc_ini                  (TY_API_DEV *p_api_dev, BYTE rt_addr);
void api_io_rt_bh_ini                    (TY_API_DEV *p_api_dev, WORD index);
void api_io_rt_sq_ini                    (TY_API_DEV *p_api_dev, WORD index);
void api_io_rt_eq_ini                    (TY_API_DEV *p_api_dev, WORD index);
void api_io_rt_ini                       (TY_API_DEV *p_api_dev);
void api_io_rt_con                       (TY_API_DEV *p_api_dev, BYTE rt_addr, BYTE con, BYTE bus, BYTE resp_time, WORD nxw);
void api_io_rt_ena_dis                   (TY_API_DEV *p_api_dev, BYTE rt_addr, BYTE con);
void api_io_rt_ena_dis_set_operation     (TY_API_DEV *p_api_dev, BYTE rt_addr, BYTE con, BYTE new_mode);
void api_io_rt_con_resp                  (TY_API_DEV *p_api_dev, BYTE rt_addr, BYTE resp_time);
void api_io_rt_con_nxw                   (TY_API_DEV *p_api_dev, BYTE rt_addr, WORD nxw);
void api_io_rt_con_lcw                   (TY_API_DEV *p_api_dev, BYTE rt_addr, WORD lcw);
void api_io_rt_con_lsw                   (TY_API_DEV *p_api_dev, BYTE rt_addr, WORD lsw);
void api_io_rt_bh_eq_con                 (TY_API_DEV *p_api_dev, WORD con, L_WORD rt_hid);
void api_io_rt_fw_dytag_dis              (TY_API_DEV *p_api_dev, WORD rt_hid);
void api_io_rt_systag_free               (TY_API_DEV *p_api_dev, short mode, BYTE rt, BYTE sa);	 /* 15.10.05 */
void api_io_rt_systag_add                (TY_API_DEV *p_api_dev, BYTE rt, BYTE sa, BYTE tag_ix); /* 15.10.05 */
void api_io_rt_systag_sub                (TY_API_DEV *p_api_dev, BYTE rt, BYTE sa, BYTE tag_ix); /* 15.10.05 */
void api_io_rt_sa_systag_init_dw         (TY_API_DEV *p_api_dev, BYTE rt, BYTE sa);
void api_io_rt_sa_systag_init_dataset    (TY_API_DEV *p_api_dev, BYTE rt, BYTE sa);
void api_io_rt_sa_systag_con             (TY_API_DEV *p_api_dev, BYTE tag_ix, BYTE rt_addr, BYTE sa);
WORD api_io_rt_bhd_def                   (TY_API_DEV *p_api_dev, WORD hid, WORD bid, WORD sid, WORD eid, BYTE qsize, BYTE bqm, BYTE bsm, BYTE sts, BYTE evs, BYTE res);
void api_io_rt_sa_con                    (TY_API_DEV *p_api_dev, BYTE rt_addr, BYTE sa_mc, WORD hid, BYTE sa_type, BYTE con, BYTE rmod, BYTE smod, WORD swm);
void api_io_rt_sa_err_con                (TY_API_DEV *p_api_dev, BYTE rt_addr, BYTE sa_mc, BYTE sa_type, BYTE type, BYTE wpos, BYTE bpos, BYTE sync, BYTE contig, BYTE bc_bits);
void api_io_rt_sa_fifo_con               (TY_API_DEV *p_api_dev, BYTE rt_addr, BYTE sa, BYTE fifo_i);
void api_io_rt_sa_dytag_con              (TY_API_DEV *p_api_dev, BYTE rt_addr, BYTE sa, BYTE sa_type, BYTE dytag_i);
void api_io_rt_sa_param_con              (TY_API_DEV *p_api_dev, BYTE uc_Biu, WORD rt_addr, BYTE sa, BYTE uc_BufLocation, BYTE uc_Type);
void api_io_rt_dwc_set                   (TY_API_DEV *p_api_dev, BYTE rt_addr, BYTE sa, BYTE sa_type, BYTE dwc);
BYTE api_io_rt_dwc_get                   (TY_API_DEV *p_api_dev, BYTE rt_addr, BYTE sa, BYTE sa_type);

  #endif 

  #ifdef CMPL4SIM
/* Bus Controller functions */
void api_io_bc_ir_clr                    (TY_API_DEV *p_api_dev);
BYTE api_io_bc_ir_get                    (TY_API_DEV *p_api_dev);
void api_io_bc_glb_cnt_ini               (TY_API_DEV *p_api_dev);
void api_io_bc_xfer_desc_tc_ini          (TY_API_DEV *p_api_dev, WORD xid);
void api_io_bc_xfer_desc_ini             (TY_API_DEV *p_api_dev, WORD xid);
void api_io_bc_sq_ini                    (TY_API_DEV *p_api_dev, WORD index);
void api_io_bc_ini                       (TY_API_DEV *p_api_dev, BYTE retr, BYTE svrq, BYTE tbm, BYTE gsb);
WORD api_io_bc_bhd_def                   (TY_API_DEV *p_api_dev, WORD hid, WORD bid, WORD sid, WORD eid, BYTE qsize, BYTE bqm, BYTE bsm, BYTE sts, BYTE evs, BYTE res);
void api_io_bc_xfer_err_con              (TY_API_DEV *p_api_dev, WORD xid, BYTE type, BYTE wpos, BYTE bpos, BYTE sync, BYTE contig, BYTE bc_bits);
L_WORD api_io_bc_xfer_desc_get           (TY_API_DEV *p_api_dev, L_WORD xid, L_WORD mode, L_WORD value[4] );
L_WORD api_io_bc_xfer_desc_mod           (TY_API_DEV *p_api_dev, L_WORD xid, L_WORD mode, L_WORD value[4] );
void api_io_bc_xfer_def                  (TY_API_DEV *p_api_dev, WORD xid, WORD hid, BYTE xfer_type, BYTE chn, BYTE xmt_rt, BYTE rcv_rt, BYTE xmt_sa, BYTE rcv_sa, BYTE wcnt, BYTE xfer_ir, BYTE xfer_hlt, BYTE rte, BYTE sxh, 
                                                                 BYTE rsp, BYTE res, L_WORD swxm, BYTE type, BYTE wpos, BYTE bpos, BYTE sync, BYTE contig, BYTE bc_bits, BYTE delay_mode, WORD gap);
void api_io_bc_xfer_ctrl                 (TY_API_DEV *p_api_dev, WORD xid, BYTE mode); 
L_WORD api_io_bc_modify_def              (TY_API_DEV *p_api_dev, L_WORD id, TY_API_BC_MODIFY_INSTRUCTION * modify, L_WORD * pbi_relative_offset);
void api_io_bc_xfer_dytag_con            (TY_API_DEV *p_api_dev, WORD xid, WORD dytag_i);
void api_io_bc_bh_eq_con                 (TY_API_DEV *p_api_dev, WORD con, L_WORD bc_hid);
void api_io_bc_fw_dytag_dis              (TY_API_DEV *p_api_dev, WORD bc_hid);
void api_io_bc_systag_free               (TY_API_DEV *p_api_dev, short mode, WORD xid);
void api_io_bc_systag_add                (TY_API_DEV *p_api_dev, WORD xid, BYTE tag_ix);
void api_io_bc_systag_sub                (TY_API_DEV *p_api_dev, WORD xid, BYTE tag_ix);
void api_io_bc_systag_init_dw            (TY_API_DEV *p_api_dev, WORD xid);
void api_io_bc_systag_init_dataset       (TY_API_DEV *p_api_dev, WORD xid);
void api_io_bc_systag_con                (TY_API_DEV *p_api_dev, BYTE tag_ix, WORD xid);
void api_io_dytag_def                    (TY_API_DEV *p_api_dev, BYTE bc_rt, WORD hid, WORD mode, WORD tag_fct1, WORD min1, WORD max1, WORD step1, WORD wpos1, WORD tag_fct2, WORD min2, WORD max2, WORD step2, WORD wpos2, WORD tag_fct3, WORD min3, WORD max3, WORD step3, WORD wpos3, WORD tag_fct4, WORD min4, WORD max4, WORD step4, WORD wpos4, L_WORD *w1, L_WORD *w2, L_WORD *w3, L_WORD *w4);
void api_io_rt_dytag_def                 (TY_API_DEV *p_api_dev, WORD rt_hid, WORD mode, WORD tag_fct1, WORD min1, WORD max1, WORD step1, WORD wpos1, WORD tag_fct2, WORD min2, WORD max2, WORD step2, WORD wpos2, WORD tag_fct3, WORD min3, WORD max3, WORD step3, WORD wpos3, WORD tag_fct4, WORD min4, WORD max4, WORD step4, WORD wpos4);
void api_io_bc_dytag_def                 (TY_API_DEV *p_api_dev, WORD bc_hid, WORD mode, WORD tag_fct1, WORD min1, WORD max1, WORD step1, WORD wpos1, WORD tag_fct2, WORD min2, WORD max2, WORD step2, WORD wpos2, WORD tag_fct3, WORD min3, WORD max3, WORD step3, WORD wpos3, WORD tag_fct4, WORD min4, WORD max4, WORD step4, WORD wpos4);
void api_io_ls_systag_dis                (TY_API_DEV *p_api_dev, BYTE mode, WORD par1, WORD par2);
void api_io_ls_systag_init_dw            (TY_API_DEV *p_api_dev, BYTE dytag_i, BYTE bc_rt_mode, WORD xid_rtsa, L_WORD dbp);
void api_io_ls_systag_init_dataset       (TY_API_DEV *p_api_dev, BYTE dytag_i, L_WORD dbp);
void api_io_ls_systag_entry_link         (TY_API_DEV *p_api_dev, BYTE tag_ix, BYTE rt_addr, BYTE sa);
void api_io_ls_systag_entry_init         (TY_API_DEV *p_api_dev, BYTE tag_ix);
void api_io_bc_eq_ini                    (TY_API_DEV *p_api_dev, WORD index);
void api_io_bc_xfer_fifo_con             (TY_API_DEV *p_api_dev, WORD xid, WORD fifo_i);
void api_io_ini_fifo                     (TY_API_DEV *p_api_dev, BYTE f_nbr, WORD size);
  #endif 

  #ifdef CMPL4SIM
/* API 1760 Protocol Functions */
WORD api_io_c1760_calc_c01_fmt           (TY_API_DEV *p_api_dev, WORD wdata[ ], BYTE epos);
void api_io_c1760_ini                    (TY_API_DEV *p_api_dev, BYTE biu_id, BYTE mode);
BYTE api_io_c1760_mode_used              (TY_API_DEV *p_api_dev, BYTE biu_id);
BYTE api_io_c1760_buf_get                (TY_API_DEV *p_api_dev, BYTE biu_id, WORD buf_id, BYTE *c1760_ix);
L_WORD api_io_c1760_con(                  TY_API_DEV *p_api_dev, BYTE biu_id, BYTE mode, WORD buf_id, BYTE c01, BYTE c02[ ]);
BYTE api_io_c1760_buf_set                (TY_API_DEV *p_api_dev, BYTE biu_id, BYTE c1760_ix, WORD volatile *buf_data);
  #endif 

  #ifdef CMPL4MON
/* Bus Monitor functions */
void api_io_bm_ir_clr                    (TY_API_DEV *p_api_dev);
BYTE api_io_bm_ir_get                    (TY_API_DEV *p_api_dev);
void api_io_bm_glb_cnt_ini               (TY_API_DEV *p_api_dev);
void api_io_bm_rt_act_ini_cnt            (TY_API_DEV *p_api_dev, BYTE rt);
void api_io_bm_rt_act_ini                (TY_API_DEV *p_api_dev, BYTE rt);
void api_io_bm_tcb_desc_ini              (TY_API_DEV *p_api_dev, BYTE tcb);
void api_io_bm_tcb_desc_con              (TY_API_DEV *p_api_dev, BYTE tid, BYTE tt, BYTE sot, BYTE tri, BYTE inv, BYTE tres, BYTE tset, BYTE tsp, BYTE next, BYTE eom, WORD tdw, WORD tmw, WORD tuli, WORD tlli);
void api_io_bm_buf_ini                   (TY_API_DEV *p_api_dev);
void api_io_bm_ini_msg_flt               (TY_API_DEV *p_api_dev, BYTE rt, L_WORD rcv_sa, L_WORD xmt_sa, L_WORD rcv_mc, L_WORD xmt_mc);
BYTE api_io_bm_ini_msg_flt_rec           (TY_API_DEV *p_api_dev, BYTE cnt, BYTE *mfrec_p);
BYTE api_io_bm_read_msg_flt_rec          (TY_API_DEV *p_api_dev, BYTE mode, BYTE con, L_WORD max_size, L_WORD max_msg);
void api_io_bm_ini_msg_ill               (TY_API_DEV *p_api_dev, BYTE rt, L_WORD rcv_sa, L_WORD xmt_sa, L_WORD rcv_mc, L_WORD xmt_mc);
void api_io_bm_ini_ftw                   (TY_API_DEV *p_api_dev, BYTE con, BYTE htm, BYTE htc, BYTE stm, BYTE stc);
void api_io_bm_ini_tiw                   (TY_API_DEV *p_api_dev, BYTE con, BYTE eti, BYTE aei, BYTE ti0, BYTE ti1);
void api_io_bm_rt_act_con                (TY_API_DEV *p_api_dev, BYTE rt, BYTE sa_mc, BYTE sa_type, BYTE tagm, BYTE tfct, BYTE tid);
void api_io_bm_ini                       (TY_API_DEV *p_api_dev);
void api_io_bm_imod_set                  (TY_API_DEV *p_api_dev, BYTE imod);
void api_io_bm_smod_set                  (TY_API_DEV *p_api_dev, BYTE smod);
void api_io_bm_intr_con                  (TY_API_DEV *p_api_dev, BYTE imod, BYTE smod);
void api_io_bm_cap_con                   (TY_API_DEV *p_api_dev, BYTE cmod, L_WORD tat, WORD mcc, WORD fsize);
void api_io_bm_swxm_con                  (TY_API_DEV *p_api_dev, WORD swxm);
L_WORD api_io_bm_read_mbfp               (TY_API_DEV *p_api_dev, short dest_biu);
void api_io_bm_dyntagm_entry_init        (TY_API_DEV *p_api_dev, L_WORD tag_ix);
void api_io_bm_dyntagm_entry_init_cnt    (TY_API_DEV *p_api_dev, L_WORD tag_ix);
void api_io_bm_dyntagm_lut_free          (TY_API_DEV *p_api_dev, L_WORD mode, L_WORD tag_ix);
void api_io_bm_dyntagm_add               (TY_API_DEV *p_api_dev, L_WORD rt, L_WORD sa, L_WORD tag_ix);
void api_io_bm_dyntagm_sub               (TY_API_DEV *p_api_dev, L_WORD rt, L_WORD sa, L_WORD tag_ix);
void api_io_bm_dyntagm_con               (TY_API_DEV *p_api_dev, L_WORD tag_ix, L_WORD rt, L_WORD sa, L_WORD sa_type);
  #endif 


/* api_ir.c */
  #if !defined(_AMC1553)   /* only for onboard-SW */
    #ifdef _NUCLEUS
void api_write_int_loli_gen_int          (TY_API_DEV *p_api_dev, WORD uw_IntType, L_WORD ul_w1, L_WORD ul_w2, L_WORD ul_w3, L_WORD ul_w4);
    #else 
void api_isr                             (L_WORD cpu_cause, L_WORD cpu_stat, EXFRAME_TY *exc);
void api_isr_transp                      (L_WORD cpu_cause, L_WORD cpu_stat, EXFRAME_TY *exc);
void api_timer0_isr                      (TY_API_DEV *p_api_dev, L_WORD cpu_cause, L_WORD cpu_stat, EXFRAME_TY *exc);
void api_write_int_loli_gen_int          (TY_API_DEV *p_api_dev, WORD uw_IntType, L_WORD ul_w1, L_WORD ul_w2, L_WORD ul_w3, L_WORD ul_w4);
void api_rti1_isr                        (L_WORD cpu_cause, L_WORD cpu_stat, EXFRAME_TY *exc);
void api_dma0_isr                        (L_WORD cpu_cause, L_WORD cpu_stat, EXFRAME_TY *exc);
void api_dma1_isr                        (L_WORD cpu_cause, L_WORD cpu_stat, EXFRAME_TY *exc);
void api_dma2_isr                        (L_WORD cpu_cause, L_WORD cpu_stat, EXFRAME_TY *exc);
void api_dma3_isr                        (L_WORD cpu_cause, L_WORD cpu_stat, EXFRAME_TY *exc);
    #endif 
  #else 
    #ifdef _WIN32
void api_write_int_loli_gen_int          (TY_API_DEV *p_api_dev, WORD uw_IntType, L_WORD ul_w1, L_WORD ul_w2, L_WORD ul_w3, L_WORD ul_w4);
    #elif defined(_NUCLEUS)
void api_write_int_loli_gen_int          (TY_API_DEV *p_api_dev, WORD uw_IntType, L_WORD ul_w1, L_WORD ul_w2, L_WORD ul_w3, L_WORD ul_w4);
    #endif 
    #ifndef _NUCLEUS
        BYTE aec_hw_wait_clocks( TY_API_DEV * p_api_dev, L_WORD u_sec );
    #endif
  #endif /* _AMC1553 */

void api_ir(                              TY_API_DEV *p_api_dev, short dest_biu );
void api_ir_transp(                       TY_API_DEV *p_api_dev, short dest_biu ); 
void api_ir_mode_con(                     TY_API_DEV *p_api_dev, short dest_biu, BYTE mode, BYTE con);
void api_ir_calculate_checksum(           TY_API_DEV *p_api_dev, short dest_biu, WORD systag_id, L_WORD dbp );
void api_ir_systag(                       TY_API_DEV *p_api_dev, short dest_biu, BYTE bc_rt_mode, WORD xid_rtsa, L_WORD dbp);
void api_ir_bcrt_queue(                   TY_API_DEV *p_api_dev, short dest_biu, WORD fifo_i, L_WORD dbp);
void api_ir_bc_srvw(                      TY_API_DEV *p_api_dev, short dest_biu, BYTE rt_addr, WORD vw, L_WORD xid);
void api_ir_bc(                           TY_API_DEV *p_api_dev, short dest_biu, L_WORD w1, L_WORD w2);
void api_ir_bc_skip(                      TY_API_DEV *p_api_dev, short dest_biu, L_WORD w1, L_WORD w2);
void api_ir_rt(                           TY_API_DEV *p_api_dev, short dest_biu, L_WORD w1, L_WORD w2);
void api_ir_bm_rec(                       TY_API_DEV *p_api_dev, short dest_biu, L_WORD addr);
void api_ir_bm_dyntagm(                   TY_API_DEV *p_api_dev, short dest_biu, L_WORD addr); 
void api_ir_bm(                           TY_API_DEV *p_api_dev, short dest_biu, L_WORD w1, L_WORD w2);
void api_ir_rep(                          TY_API_DEV *p_api_dev, short dest_biu, L_WORD w1, L_WORD w2);
void api_ir_tt(                           TY_API_DEV *p_api_dev, short dest_biu, L_WORD w1, L_WORD w2);
void api1553_gen_int2host(                TY_API_DEV *p_api_dev, L_WORD mask); 
void api_ir_track(                        TY_API_DEV *p_api_dev, short biu, BYTE uTrackId, TY_API_TRACK_DEF *pTrack, TY_API_TRACK_DEF *pTrackList, L_WORD dbp, L_WORD sqp);
void api_ir_data_queue(                   TY_API_DEV *p_api_dev, AiUInt8 id);
void api_ir_data_queue_generic_acq       (TY_API_DEV *p_api_dev, TY_API_DATA_QUEUE_HEADER *pDataQueue, L_WORD id, L_WORD size, L_WORD *data);
void api_ir_enque_aye_discretes_from_fpga(TY_API_DEV *p_api_dev, L_WORD id, L_WORD tt_hi, L_WORD tt_lo, L_WORD val);


/* DMA functions*/
void api_dma_SignalDmaCompleted          (TY_API_DEV *p_api_dev);
void api_dma_init                        (TY_API_DEV *p_api_dev);
BYTE api_dma_setup                       (TY_API_DEV *p_api_dev, BYTE uc_DmaNo, ty_api_dma_setup *px_DmaSetup);
BYTE api_dma_run                         (TY_API_DEV *p_api_dev, BYTE uc_DmaNo);
void api_dma_confirm_interrupt           (TY_API_DEV *p_api_dev, BYTE uc_DmaNo);
void api_dma_Host2TargetCom              (TY_API_DEV *p_api_dev);


void ClearHeap                           (void * pKeep);
void InitHeap                            (void *pStart, L_WORD uSize);
void *mmalloc                            (L_WORD uSize);
void mfree                               (void *p);

short ExecuteFirmwareCommand(TY_API_DEV *p_api_dev, short dest_biu, BYTE uc_FirmwareCmd);


void api_main_initialize_stream_type( TY_API_DEV * p_api_dev );
void api_main_setup_memory_layout(    TY_API_DEV * p_api_dev );
void api_main_init_and_reset(         TY_API_DEV * p_api_dev );
void api_main_free_allocated_memory(  TY_API_DEV * p_api_dev );

/*! \brief Hardware independent function for scope commands

    This function is the main entry point for all scope related 
    target software IO commands 
    \param p_api_dev pointer to \ref TY_API_DEV target software administration structure 
    \param command_code code of the target software IO command 
    \param size size of the command in bytes
    \param p_command_params pointer to buffer that holds the parameters of the command 
    \return returns API_ACK or API_ACK2 on success, API_NAK on failure */
BYTE mil_scope_cmd(TY_API_DEV* p_api_dev, AiUInt32 command_code, AiUInt16 size, void* p_command_params);


  #ifdef _NUCLEUS

void BIU1_CHN1_Interrupt                 (INT vector_number);
void BIU2_CHN1_Interrupt                 (INT vector_number);
void BIU1_CHN2_Interrupt                 (INT vector_number);
void BIU2_CHN2_Interrupt                 (INT vector_number);
void BIU3_CHN1_Interrupt                 (INT vector_number);
void BIU4_CHN1_Interrupt                 (INT vector_number);
void BIU3_CHN2_Interrupt                 (INT vector_number);
void BIU4_CHN2_Interrupt                 (INT vector_number);
void HOST_Interrupt                      (INT vector_number);

void API_HISR_Interrupt                  (void);
void API_ISR1_Interrupt                  (INT vector_number);
void API_ISR2_Interrupt                  (INT vector_number);
void API_ISR3_Interrupt                  (INT vector_number);
void API_ISR4_Interrupt                  (INT vector_number);
void API_ISR5_Interrupt                  (INT vector_number);
void API_ISR6_Interrupt                  (INT vector_number);
void API_ISR7_Interrupt                  (INT vector_number);
void API_ISR8_Interrupt                  (INT vector_number);
void API_MSCOPE_HISR                     (void);
void API_MSCOPE_Interrupt                (INT vector_number);

void API_RTI1_Interrupt                  (INT vector_number);

void API_DMA0_Interrupt                  (INT vector_number);
void API_DMA0_Interrupt2                  (INT vector_number);
void API_DMA0_Interrupt3                  (INT vector_number);
void API_DMA1_Interrupt                  (INT vector_number);

void *allocate_memory                    (UINT32 size);

void deallocate_memory                   (void *p);

int Mymain                               (void);

/* ini_misc.c/dma_lca.c */
void DMALoc2Loc                          (UINT32 DMAchannel, UINT32 SrcAddress, UINT32 DstAddress, UINT32 SizeInBytes);
void DMAWaitIdle                         (UINT32 DMAchannel);


  #endif 


L_WORD api_opr_sf(TY_API_DEV *p_api_dev, L_WORD cmd, BYTE rt, BYTE mode);



#ifdef _AIM_1553_SYSDRV_USB
    int isTswLibUsbDefinePresent(void);
#else
    int isTswLibUsbDefineNotPresent(void);
#endif



NAMESPACE1553_END

#ifdef __cplusplus
  }
#endif

#endif /* __API_PROT_H__ */

/* end: api_prot */
