/* SPDX-FileCopyrightText: 2015-2024 AIM GmbH <info@aim-online.com> */
/* SPDX-License-Identifier: MIT OR GPL-2.0-or-later */

/*! \file api_defs.h
 *
 *  Header information of the ASP Driver Software, definition of constants and macros.
 *
 */

#ifndef __API_DEFS_H__
  #define __API_DEFS_H__

/******************************************************************************
 *  Defines
 ******************************************************************************/
  #ifndef PASS
    #define PASS   1
  #endif

  #ifndef FAIL
    #define FAIL   0
  #endif

  #ifndef FALSE
    #define FALSE  0
    #define TRUE   1
  #endif

#include "Ai_cdef.h"

/************************************/
/* define to set NAK info           */
/************************************/

#define SET_NAK_INFO(x)  *p_api_dev->lword_data_arr = x

/************************************/
/* defines Byte swapping            */
/************************************/


  #if defined (HOST_ENDIAN_LITTLE) || defined (_MIPSEL_)
    #define BSWAP32(X)     (X)
    #define BSWAP16(X)     (X)
  #elif defined (HOST_ENDIAN_BIG)
    #define BSWAP32        _BSWAP32
    #define BSWAP16        _BSWAP16
  #else
    #ifdef _AMC1553
      #error Endian order not defined HOST_ENDIAN_BIG or HOST_ENDIAN_LITTLE.
    #else
      #define BSWAP32        _BSWAP32
      #define BSWAP16        _BSWAP16
    #endif
  #endif

/***************************************/
/***  Conditional Compiler switches ***/
/***************************************/
  #define CMPL4SIM /* compile for Simulator functions */
  #define CMPL4MON /* compile for Monitor functions */
  #define CMPL4REP /* compile for Replay functions */

  #define IRIG


/*************************************/
/***  BIU Constants                ***/
/*************************************/

  #define MAX_BIU      8       /* Maximum Logical Bus Interface Units on board */
  #define MAX_PHY_BIU  4       /* Maximum of Physical Bus Interface Units on board */
  #define BIU_SYS   0       /* System related Driver Command */
  #define BIU_1     1       /* Bus Interface Unit 1 related Driver Command */
  #define BIU_2     2       /* Bus Interface Unit 2 related Driver Command */
  #define BIU_3     3       /* Bus Interface Unit 3 related Driver Command */
  #define BIU_4     4       /* Bus Interface Unit 4 related Driver Command */
  #define BIU_5     5       /* Bus Interface Unit 4 related Driver Command */
  #define BIU_6     6       /* Bus Interface Unit 4 related Driver Command */
  #define BIU_7     7       /* Bus Interface Unit 4 related Driver Command */
  #define BIU_8     8       /* Bus Interface Unit 4 related Driver Command */
  #define BIU_ASP   3       /* ASP Monitor Command selected */
  #define BIU_NONE  0xFF

  #define AIM_SH_RAM_ID	0x53484152					/* should be "SHAR" but it is reverse for Little Endian	*/

/*************************************/
/***  DBG Constants                ***/
/*************************************/
  #define DBG_FCT_CALL                      0x00000001L
  #define DBG_MUX_TRACK                     0x00000002L
  #define DBG_REC_DATA_WCNT_CHK             0x00000004L
  #define DBG_INTERRUPT_CNT                 0x00000008L
  #define DBG_DQUEUE                        0x00000010L
  #define DBG_DQUEUE_IR                     0x00000020L
  #define DBG_DQUEUE_PARAM_REC              0x00000040L
  #define DBG_REPLAY                        0x00000080L
  #define DBG_MILSCOPE                      0x00000100L
  #define DBG_TRACE_MILSCOPE                0x00000200L
  #define DBG_DMA                           0x00001000L
  #define DBG_MEMORY                        0x00002000L
  #define DBG_GENIO                         0x00004000L
  #define DBG_RESET_INTERRUPT_CNT           0x80000000L


/*************************************/
/***  Instrumentation Constants    ***/
/*************************************/
#define MAX_INSTR_LEVEL                       2
#define INSTR_MILSCOPE                        0
#define INSTR_MILSCOPE_GENERATE_SAW_TOOTH     0x00000001L
#define INSTR_MILSCOPE_NO_COPY                0x00000002L
#define INSTR_MILSCOPE_NO_SAMPLE_REORDER      0x00000004L
#define INSTR_MILSCOPE_NO_SAMPLE_OFFSET       0x00000008L
#define INSTR_MILSCOPE_PRINT                  0x00000010L
#define INSTR_MILSCOPE_BACKUP_DATA            0x00000020L
#define INSTR_MILSCOPE_BACKUP_PRINT           0x00000040L

#define INSTR_MILSCOPE_BACKUP_DQUEUE          0x00000200L
#define INSTR_MILSCOPE_BACKUP_DQUEUE_PRINT    0x00000400L

#define INSTR_SYS                             1
#define INSTR_IRQ_STACK_OVERFLOW              0x00000001L
#define INSTR_SYS_CONFIG                      0x00000002L




/*************************************/
/***  ASP Driver Command Codes     ***/
/*************************************/

  #define API_LS_CMD_DEST_LL          0x00000022L  /* Indicates LS Command lower limit*/
  #define API_LS_CMD_DEST_UL          0x000000FFL  /* Indicates HS Command upper limit*/


/* System Commands */
  #define API_SYS_GET_MEM_PARTITION   0x2B /*   5             LEGACY */
  #define API_INI                     0x30 /*   5  ACK2  118  LEGACY */
  #define API_RESET                   0x31 /*   5  ACK2   14  LEGACY */
  #define API_READ_SW_VERS            0x34 /*   4  ACK2   14  LEGACY */
  #define API_READ_SW_VERS_EX       0x4034 /*   4  ACK2   14  LEGACY */
  #define API_LOAD_SREC               0x36 /*  10+ ACK2    9  LEGACY */
  #define API_PROG_FLASH              0x37 /*  16  ACK2    5  LEGACY */

/* Data Buffer Commands */
  #define API_RAM_WRITE               0x52 /*   9+ ACK     -  */
  #define API_RAM_READ                0x53 /*   9  ACK2    4+ */
  #define API_RAM_LWORD_WR            0x54 /*  12  ACK     -  */
  #define API_RAM_LWORD_RD            0x55 /*   8  ACK2    8  */
  #define API_RAM_WORD_WR             0x56 /*  10  ACK     -  */
  #define API_RAM_WORD_RD             0x57 /*   8  ACK2    6  */
  #define API_RAM_BYTE_WR             0x58 /*   9  ACK     -  */
  #define API_RAM_BYTE_RD             0x59 /*   8  ACK2    5  */

/* 1760 DTS specific Commands */
  #define API_IP_OUTPUT_CHN_WRITE     0xE0 /*  14  ACK     -  */
  #define API_IP_INPUT_CHN_READ       0xE1 /*  10  ACK2    8  */
  #define API_IP_STASK_CTRL           0xE6 /*  18  ACK     -  */
  #define API_IP_MODULE_ADDR_INIT     0xE7 /*  12  ACK     -  */
  #define API_IP_OUTPUT_CHN_READ      0xE8 /*  10  ACK     8  */
  #define API_IP_MODULE_ADDR_READ     0xE9 /*  12  ACK2    4  */

/* Special command for AXI board*/
  #define API_DEF_AXI_TRG_LINE        0xF0 /*  22  ACK     -  */

/* Data Queue Commands */
  #define API_DATA_QUEUE_PARA_REC_DEF 0xF6
  #define API_SCOPE_SETUP             0xF7
  #define API_SCOPE_TRG_DEF           0xF8
  #define API_SCOPE_CALIBRATE         0xF9

  #define API_SYS_UTIL	              0xFF /* LEGACY */


  /* GenIo Commands */
  #define API_GEN_IO_GET_BOARDS       0x300
  #define API_GEN_IO_ADDR_INIT        0x301  /*  12  ACK     -  */
  #define API_GEN_IO_OUT_CHN_WRITE    0x302	 /*  14  ACK     -  */
  #define API_GEN_IO_IN_CHN_READ      0x303	 /*  10  ACK2    8  */
  #define API_GEN_IO_STASK_CTRL       0x304	 /*  18  ACK     -  */
  #define API_GEN_IO_READ             0x305	 /*  10  ACK2    8  */
  #define API_GEN_IO_OUT_CHN_READ     0x306

/*************************************/
/***  System Control Block         ***/
/*************************************/

/* System Control Block Register numbers */
  #define API_SCB_SCW_REG     0  /* System Control Word */
  #define API_SCB_SSW_REG     1  /* System Status Word */
  #define API_SCB_FID_REG     2  /* Firmware Identification Register */
  #define API_SCB_ILP_REG     3  /* Interrupt Loglist Pointer */
  #define API_SCB_CAR_REG     4  /* Coupling and Amplitude Register */
  #define API_SCB_RTPD1_REG   5  /* RT Protocol Definition Register 1 */
  #define API_SCB_RTPD2_REG   6  /* RT Protocol Definition Register 2 */
  #define API_SCB_MFC_REG    12  /* Major Frame Counter */
  #define API_SCB_MFT_REG    13  /* Minor Frame Time */
  #define API_SCB_HIP_REG    14  /* High Priority BC Instruction List Pointer */
  #define API_SCB_LIP_REG    15  /* Low Priority BC Instruction List Pointer */
  #define API_SCB_AIP_REG    16  /* Acyclic Event BC Instruction List Pointer */
  #define API_SCB_STL_REG    17  /* Simulator Session Time Low Word */
  #define API_SCB_STH_REG    18  /* Simulator Session Time High Word */
  #define API_SCB_RT_MC_REG  19  /* RT Transfer Counter */
  #define API_SCB_RT_EC_REG  20  /* RT Error Counter */
  #define API_SCB_BC_MC_REG  21  /* BC Transfer Counter */
  #define API_SCB_BC_EC_REG  22  /* BC Error Counter */
  #define API_SCB_MBBA_REG   32  /* Monitor Buffer Base Address */
  #define API_SCB_MBFP_REG   33  /* Monitor Buffer Fill Pointer */
  #define API_SCB_MSW_REG    34  /* Monitor Status Word */
  #define API_SCB_MFTW_REG   35  /* Monitor Function Trigger Word */
  #define API_SCB_MTIW_REG   36  /* Monitor Trigger Index Word */
  #define API_SCB_MSTP_REG   37  /* Monitor Start Trigger Pointer */
  #define API_SCB_MCC_REG    38  /* Monitor Message Capture Count */
  #define API_SCB_TAT_REG    39  /* Monitor Trace after Trigger Count */
  #define API_SCB_SWXM_REG   40  /* Monitor Status Word Exception Mask */
  #define API_SCB_MTL_REG    41  /* Monitor Session Time Low Word */
  #define API_SCB_MTH_REG    42  /* Monitor Session Time High Word */
  #define API_SCB_BM_MC_REG  43  /* Monitor Transfer Counter */
  #define API_SCB_BM_EC_REG  44  /* Monitor Error Counter */

/* 01.08.99 */
  #define API_SCB_RCW_REG    48  /* Replay Control Word */
  #define API_SCB_RREW_REG   49  /* Replay RT Enable Word */
  #define API_SCB_RBP_REG    51  /* Replay Buffer Pointer */
  #define API_SCB_RBB_REG    50  /* Replay Buffer Base Pointer */
  #define API_SCB_REC_REG    52  /* Replay Entry Count */
  #define API_SCB_RAEC_REG   53  /* Replay Actual Entry Counter */
  #define API_SCB_RAMTO_REG  54  /* Replay Absolute Minute Offsett Register */
  #define API_SCB_RAMIO_REG  55  /* Replay Absolute Microsecond Offset Register */

/* System Control Block Constants */
  #define API_SCB_COM_ENA_BIU    0x01
  #define API_SCB_COM_ENA_BC     0x02
  #define API_SCB_COM_ENA_RT     0x03
  #define API_SCB_COM_ENA_BM     0x04
  #define API_SCB_COM_ENA_REP    0x05
  #define API_SCB_COM_DIS_BIU    0x09
  #define API_SCB_COM_DIS_BC     0x0A
  #define API_SCB_COM_DIS_RT     0x0B
  #define API_SCB_COM_DIS_BM     0x0C
  #define API_SCB_COM_DIS_REP    0x0D
  #define API_SCB_COM_RD_CAR     0x10
  #define API_SCB_COM_RD_RTP     0x11
  #define API_SCB_COM_RD_IRIG    0x12
  #define API_SCB_COM_BC_CONT    0x13
  #define API_SCB_COM_RD_TIW     0x15
  #define API_SCB_COM_SET_SYNC   0x16
  #define API_SCB_COM_GET_SYNC   0x17
  #define API_SCB_COM_SET_SOM    0x18 /* set special operation mode */
  #define API_SCB_COM_HOST_CTRL_INCR   0x1A
  #define API_SCB_COM_ASP_INTR   0x1C
  #define API_SCB_COM_ENA_BITE   0x1F

  #define API_SCB_COM_INOUT_64US_CNT        0x00000001
  #define API_SCB_COM_INOUT_SPEC_MC         0x00000100
  #define API_SCB_COM_INOUT_RT_START_TRIG   0x00000200
  #define API_SCB_COM_INOUT_SINGLE_RT       0x00000300
  #define API_SCB_COM_INOUT_ASSERT_INT      0x00010000
  #define API_SCB_COM_INOUT_RTADDR_POS      24
  #define API_SCB_COM_INOUT_RTADDR_MASK     0x1F
  #define API_SCB_COM_INOUT_RTADDR_PARITY   0x20000000
  #define API_SCB_COM_INOUT_BC_TRAFFIC_OVERRIDE 0x00000001


  #define API_SCB_RTRY_DIS       0x0
  #define API_SCB_RTRY_ONCE      0x1
  #define API_SCB_RTRY_TWICE     0x2
  #define API_SCB_RTRY_BOTH      0x3

  #define API_SCB_MMOD_STD_CAP   0x0
  #define API_SCB_MMOD_SEL_CAP   0x1

  #define API_SCB_IMOD_DIS       0x0
  #define API_SCB_IMOD_BUF_FULL  0x1
  #define API_SCB_IMOD_TR_START  0x2
  #define API_SCB_IMOD_TR_STOP   0x3

  #define API_SCB_SMOD_DIS       0x0
  #define API_SCB_SMOD_BUF_FULL  0x1
  #define API_SCB_SMOD_TR_START  0x2
  #define API_SCB_SMOD_TR_STOP   0x3

  #define API_SCB_TBM_TRANS      0x0
  #define API_SCB_TBM_GLOB	 0x1

  #define API_SCB_GSB_PRI	 0x0
  #define API_SCB_GSB_SEC	 0x1


  #define API_SSW_OPE_BIT  0x02000000L
  #define API_SSW_SFE_BIT  0x01000000L
  #define API_SSW_MNE_BIT  0x00800000L
  #define API_SSW_BCE_BIT  0x00400000L
  #define API_SSW_RTE_BIT  0x00200000L
  #define API_SSW_RPE_BIT  0x00100000L
  #define API_SSW_SCM_BIT  0x00080000L

/***************************************/
/*** System Constants                ***/
/***************************************/
  #define INIT_ALL  0
  #define INIT_RT   1
  #define INIT_BC   2
  #define INIT_BM   3
  #define INIT_REP  4

  /* Get/Set IRIG Time */
  #define API_DONT_CHANGE_TIMETAG_SOURCE 0
  #define API_CHANGE_TIMETAG_SOURCE      1
  #define API_IRIG_ONBOARD               0
  #define API_IRIG_EXTERN                1
  #define API_IRIG_NOT_SYNC              0
  #define API_IRIG_SYNC                  1

  #define API_PXI_SET_TRG               0
  #define API_PXI_CLR_TRG               1
  #define API_PXI_SET_TTSRC_EXTERNAL    2
  #define API_PXI_SET_TTSRC_INTERNAL    3

  #define API_TRG_PXI0                  0
  #define API_TRG_PXI1                  1
  #define API_TRG_PXI2                  2
  #define API_TRG_PXI3                  3
  #define API_TRG_PXI4                  4
  #define API_TRG_PXI5                  5
  #define API_TRG_PXI6                  6
  #define API_TRG_PXI7                  7
  #define API_TRG_BC_CHN1               8
  #define API_TRG_BC_CHN2               9
  #define API_TRG_BC_CHN3               10
  #define API_TRG_BC_CHN4               11
  #define API_TRG_RT_CHN1               12
  #define API_TRG_RT_CHN2               13
  #define API_TRG_RT_CHN3               14
  #define API_TRG_RT_CHN4               15
  #define API_TRG_BM_CHN1               16
  #define API_TRG_BM_CHN2               17
  #define API_TRG_BM_CHN3               18
  #define API_TRG_BM_CHN4               19
  #define API_TRG_BM_HS                 20

/*************************************/
/*** Buffer Constants              ***/
/*************************************/
  #define MAX_API_DEFAULT_BUF_ID      2048 /* used for the default memory mapping */
  #define MAX_API_BUF_SIZE   (32*2L) /* Bytes */
  #define MAX_API_ISLP_SIZE   128 /* Entries of 2 LongWords */

  #define API_SIM_BUF_AREA_BYTE_SIZE(x) (((x >= 2) ? 0x80000L : 0x40000L))
  #define API_REP_BUF_AREA_BYTE_SIZE 0x20000L

  #define API_BUF_BC_TYPE   1
  #define API_BUF_RT_TYPE   2

  #define MAX_API_HID       512


/***************************************/
/*** Calibration Constants           ***/
/***************************************/

  #define API_CAL_PRI_BUS 1
  #define API_CAL_SEC_BUS 2
  #define API_CAL_HIGHRES 3
  #define API_CAL_PACKED  4

  #define API_CAL_SIG_ENA 1
  #define API_CAL_SIG_DIS 0

  #define API_CAL_TRANS_500K 1
  #define API_CAL_TRANS_1M   0
  #define API_CAL_CPL_ISO    0
  #define API_CAL_CPL_TRF    1
  #define API_CAL_CPL_DIR    2
  #define API_CAL_CPL_EXT    3
  #define API_CAL_CPL_WRAP   4

/***************************************/
/*** Remote Terminal Constants       ***/
/***************************************/

  #define MAX_API_RT          32   /* RT Descriptors */
  #define MAX_API_RT_MC_SA    32   /* RT,SA,Modecodes */
  #define MAX_API_RT_HID      MAX_API_HID /* Buffer Header */
  #define MAX_API_RT_SQ       512  /* Status Queue */
  #define MAX_API_RT_EQ       512  /* Event Queue */

  #define MAX_API_RT_RCV_SA   32
  #define MAX_API_RT_XMT_SA   32
  #define MAX_API_RT_RCV_MC   32
  #define MAX_API_RT_XMT_MC   32
  #define MAX_API_RT_SA       (MAX_API_RT_RCV_SA + MAX_API_RT_XMT_SA)
  #define MAX_API_RT_MC       (MAX_API_RT_RCV_MC + MAX_API_RT_XMT_MC)
  #define MAX_API_RT_SA_MC    (MAX_API_RT_SA + MAX_API_RT_MC)

  #define API_RT_SA_TYPE_RCV  0
  #define API_RT_SA_TYPE_XMT  1
  #define API_RT_MC_TYPE_RCV  2
  #define API_RT_MC_TYPE_XMT  3
  #define API_RT_SA_TYPE_ALL  4

  #define API_RT_DYTAG_ENA    1
  #define API_RT_DYTAG_DIS    0

  #define API_RT_FIFO_ENA     1
  #define API_RT_FIFO_DIS     0

/* RT Descriptor */
  #define API_RT_DISABLE                0
  #define API_RT_ENABLE                 1
  #define API_RT_MAILBOX                2
  #define API_RT_ENABLE_DUAL_REDUNDANT  3

  #define API_RT_RESP_BOTH    0
  #define API_RT_RESP_PRI     1
  #define API_RT_RESP_SEC     2

  #define API_RT_RESP_TIME_MIN  (4 * 4)         /* 0.25 usec */
  #define API_RT_RESP_TIME_DEF  (8 * 4)         /* 0.25 usec */
  #define API_RT_RESP_TIME_MAX ((64 * 4) - 1)   /* 0.25 usec */

/* RT SA Descriptor */
  #define API_RT_SA_DISABLE           0
  #define API_RT_SA_ENABLE            1
  #define API_RT_SA_ENA_INTR_ON_XFER  2
  #define API_RT_SA_ENA_INTR_ON_ERR   3

  #define API_RT_SA_RESP_MODE_STD     0
  #define API_RT_SA_RESP_MODE_FAST    1
  #define API_RT_SA_RESP_MODE_AUTO    2
  #define API_RT_SA_RESP_MODE_RES     3
  #define API_RT_SA_RESP_MODE_DEF     API_RT_SA_RESP_MODE_FAST

  #define API_RT_SA_SMOD_OR_SW   0
  #define API_RT_SA_SMOD_AND_SW  1
  #define API_RT_SA_SMOD_OR_NXW  2
  #define API_RT_SA_SMOD_AND_NXW 3

/* RT Buffer Header Descriptor */
  #define API_RT_BH_BQM_CYCLIC   0
  #define API_RT_BH_BQM_ONCE     1
  #define API_RT_BH_BQM_HOST     2
  #define API_RT_BH_BQM_AUTO     3

  #define API_RT_BH_BSM_DISCARD  0
  #define API_RT_BH_BSM_KEEP     1

  #define API_RT_BH_STS_NO_SQ    0
  #define API_RT_BH_STS_ONE_SQ   1
  #define API_RT_BH_STS_QSIZE_SQ 2

  #define API_RT_BH_EVS_NO_EQ    0
  #define API_RT_BH_EVS_ONE_EQ   1
  #define API_RT_BH_EVS_QSIZE_EQ 2

/* RT Memory area */
  #define MAX_API_RT_DESC_SIZE          (16*4L) /* Bytes */
  #define MAX_API_RT_SA_DESC_SIZE       (2*4L)  /* Bytes */
  #define MAX_API_RT_BH_DESC_SIZE       (4*4L)  /* Bytes */
  #define MAX_API_RT_SQ_DESC_SIZE       (4*4L)  /* Bytes */
  #define MAX_API_RT_EQ_DESC_SIZE       (4*4L)  /* Bytes */
  #define MAX_API_RT_SA_AREA_SIZE       (MAX_API_RT_SA_DESC_SIZE * MAX_API_RT_SA_MC) /* Bytes */
  #define MAX_API_RT_BH_AREA_SIZE(size) (MAX_API_RT_BH_DESC_SIZE * size)   /* Bytes */
  #define MAX_API_RT_SQ_AREA_SIZE(size) (MAX_API_RT_SQ_DESC_SIZE * size)    /* Bytes */
  #define MAX_API_RT_EQ_AREA_SIZE(size) (MAX_API_RT_EQ_DESC_SIZE * size)    /* Bytes */

  /* RT Mode Control */
  #define API_RT_MODE_CONFIGURED_DYTAGS     3
  #define API_RT_MODE_LANE_CTRL             4
  #define API_RT_MODE_LSW_HANDLING          5   /* Implemented only in special BSP V11.11 Patch 5 for Dassault */
  #define API_RT_MODE_OPERATION_CTRL        6

  #define API_RT_RSP_BOTH_BUSSES        0
  #define API_RT_RSP_PRI_BUS            1
  #define API_RT_RSP_SEC_BUS            2


/***************************************/
/*** Bus Controller Constants        ***/
/***************************************/

  #define MAX_API_BC_XFER      512   /* Transfer Descriptors */
  #define MAX_API_BC_XFER_MASK 0x7FF /* Transfer Descriptors */
  #define MAX_API_BC_FRAME     64    /* Minor Frames */
  #define MAX_API_BC_HID       MAX_API_HID /* Buffer Header */
  #define MAX_API_BC_SQ        512   /* Status Queue */
  #define MAX_API_BC_EQ        512   /* Event Queue */
  #define MAX_API_BC_SKIP_CNT  128   /* Skip Count */

  #define MAX_API_BC_XFER_IN_FRAME       128 /* Maximum Transfers in Minor Frame */
  #define MAX_API_BC_FRAME_IN_MFRAME     512 /* Maximum Minor Frames in Major Frame */
  #define MAX_API_BC_FRAME_IN_MFRAME_DEF 64
  #define MAX_API_BC_LIP_INSTR           MAX_API_BC_XFER_IN_FRAME
  #define MAX_API_BC_ACYC_INSTR          MAX_API_BC_XFER_IN_FRAME
  #define MAX_API_BC_HIP_INSTR           ((MAX_API_BC_XFER_IN_FRAME * MAX_API_BC_FRAME_IN_MFRAME_DEF)-MAX_API_BC_LIP_INSTR-MAX_API_BC_ACYC_INSTR)

  #define API_BC_STATUS_HALT  1
  #define API_BC_STATUS_BUSY  2

  #define API_BC_DYTAG_ENA    1
  #define API_BC_DYTAG_DIS    0

  #define API_BC_FIFO_ENA    1
  #define API_BC_FIFO_DIS    0

/* BC Transfer Descriptor */
  #define API_BC_XFER_TYPE_BCRT  0
  #define API_BC_XFER_TYPE_RTBC  1
  #define API_BC_XFER_TYPE_RTRT  2
  #define API_BC_XFER_TYPE_NOP   3

  #define API_BC_XFER_MIL_CHN_A  0
  #define API_BC_XFER_MIL_CHN_B  1

  #define API_BC_XFER_RET_NONE   0
  #define API_BC_XFER_RET_AUTO   1

  #define API_BC_XFER_RETRY_DIS  0
  #define API_BC_XFER_RETRY_ENA  1

  #define API_BC_XFER_TOGGLE_BUS_KEEP 0
  #define API_BC_XFER_TOGGLE_BUS_ALT  1

  #define API_BC_XFER_SWX_NONE   0
  #define API_BC_XFER_SWX_VEC    1

  #define API_BC_XFER_IR_NONE    0  /* No Interrupt */
  #define API_BC_XFER_IR_EOT     1  /* Interrupt on End of Transfer */
  #define API_BC_XFER_IR_ERR     2  /* Interrupt on Transfer Error */
  #define API_BC_XFER_IR_SWX     3  /* Interrupt on Status Word Exception */

  #define API_BC_XFER_HALT_NONE  0  /* Do not halt on exception */
  #define API_BC_XFER_HALT_ERR   1  /* Halt on Transfer Error */
  #define API_BC_XFER_HALT_SWX   2  /* Halt on Status Word Exception */
  #define API_BC_XFER_HALT_BOTH  3  /* Halt on Transfer Error or Status Word Exception */
  #define API_BC_XFER_HALT_ANY   4  /* Halt on any Interrupt */

  #define API_BC_XFER_IMG_TWT    0  /* Transfer Wait Time */
  #define API_BC_XFER_IMG_STD    1  /* Standard Gap Mode */
  #define API_BC_XFER_IMG_FAST   2  /* Fast Gap Mode */
  #define API_BC_XFER_IMG_RES    3  /* Reserved */

  #define API_BC_XFER_RESP_CTRL_AUTO   0
  #define API_BC_XFER_RESP_CTRL_NO_CW1 1
  #define API_BC_XFER_RESP_CTRL_NO_CW2 2
  #define API_BC_XFER_RESP_CTRL_RES    3

  #define API_BC_INSTR_XFER_CODE  1
  #define API_BC_INSTR_SKIP_CODE  2
  #define API_BC_INSTR_WAIT_CODE  3
  #define API_BC_INSTR_TSTR_CODE  4
  #define API_BC_INSTR_RESMF_CODE 5


  #define API_BC_XFER_RUN_IMM      1
  #define API_BC_XFER_RUN_ETR      2
  #define API_BC_XFER_RUN_DBA      3
  #define API_BC_XFER_RUN_EXT      4
  #define API_BC_XFER_RUN_SETUP    5
  #define API_BC_XFER_RUN_FAST_IMM 6
  #define API_BC_XFER_RUN_TBL      7
  #define API_BC_XFER_RUN_CONT     8
  #define API_BC_XFER_RUN_TBL_DBA  9

  #define API_BC_ACYC_RUN_IMM             0
  #define API_BC_ACYC_SEND_TTAG           1
  #define API_BC_ACYC_RUN_AT_END_OF_FRAME 2

  #define API_SEND_SRVW_ON_SA0     0
  #define API_SEND_SRVW_ON_SA31    1

  #define API_BC_XFER_RUN_MIN_FTIME 10
  #define API_BC_XFER_RUN_MAX_FTIME 0x100000

  #define API_BC_XFER_HIP  1
  #define API_BC_XFER_LIP  2
  #define API_BC_XFER_AIP  3

/* BC Instruction Opcodes */
  #define API_BC_INSTR_XFER      0x10
  #define API_BC_INSTR_XFER_EFEX 0x11
  #define API_BC_INSTR_CALL      0x20
  #define API_BC_INSTR_RET       0x21
  #define API_BC_INSTR_JMP       0x22
  #define API_BC_INSTR_SKIP      0x24
  #define API_BC_INSTR_WTRG      0x25
  #define API_BC_INSTR_STR       0x26
  #define API_BC_INSTR_DJZ       0x27
  #define API_BC_INSTR_WMFT      0x28
  #define API_BC_INSTR_HALT      0x29
  #define API_BC_INSTR_WAIT      0x2A
  #define API_BC_INSTR_MODIFY    0x2B
  #define API_BC_INSTR_CMFT      0x2C
  #define API_BC_INSTR_RMF       0x2D
  #define API_BC_INSTR_ACYC_TTAG 0x2E

/* BC Buffer Header Descriptor */
  #define API_BC_BH_BQM_CYCLIC   0
  #define API_BC_BH_BQM_ONCE     1
  #define API_BC_BH_BQM_HOST     2
  #define API_BC_BH_BQM_AUTO     3

  #define API_BC_BH_BSM_DISCARD  0
  #define API_BC_BH_BSM_KEEP     1

  #define API_BC_BH_STS_NO_SQ    0
  #define API_BC_BH_STS_ONE_SQ   1
  #define API_BC_BH_STS_QSIZE_SQ 2

  #define API_BC_BH_EVS_NO_EQ    0
  #define API_BC_BH_EVS_ONE_EQ   1
  #define API_BC_BH_EVS_QSIZE_EQ 2

/* BC Memory area */
  #define MAX_API_BC_XFER_DESC_SIZE            (8*4L) /* Bytes */
  #define MAX_API_BC_XFER_INST_SIZE            (1*4L) /* Bytes */
  #define MAX_API_BC_BH_DESC_SIZE              (4*4L) /* Bytes */
  #define MAX_API_BC_SQ_DESC_SIZE              (4*4L) /* Bytes */
  #define MAX_API_BC_EQ_DESC_SIZE              (4*4L) /* Bytes */
  #define MAX_API_BC_BH_AREA_SIZE(size)        (MAX_API_BC_BH_DESC_SIZE * size)  /* Bytes */
  #define MAX_API_BC_SQ_AREA_SIZE(size)        (MAX_API_BC_SQ_DESC_SIZE * size)   /* Bytes */
  #define MAX_API_BC_EQ_AREA_SIZE(size)        (MAX_API_BC_EQ_DESC_SIZE * size)   /* Bytes */
  #define MAX_API_BC_XFER_DESC_AREA_SIZE(size) (MAX_API_BC_XFER_DESC_SIZE * size)
  #define MAX_API_BC_LIP_INST_AREA_SIZE(size)  (MAX_API_BC_XFER_INST_SIZE * size)
  #define MAX_API_BC_ACYC_INST_AREA_SIZE(size) (MAX_API_BC_XFER_INST_SIZE * size)
  #define MAX_API_BC_HIP_INST_AREA_SIZE(size)  (MAX_API_BC_XFER_INST_SIZE * size)

  /* BC Mode Control */
  #define API_BC_MODE_INSERT_MC17_SYNC_CNT  1
  #define API_BC_MODE_STOP_ON_MC0_DBCA_FLAG 2
  #define API_BC_MODE_CONFIGURED_DYTAGS     3

/***************************************/
/*** Bus Monitor Constants           ***/
/***************************************/

  #define MAX_API_TCB            256   /* Monitor Trigger Block Descriptors */
  #define MAX_API_BM_RT_ACT_SIZE (MAX_API_RT * MAX_API_RT_SA_MC) /* Monitor Activity List Entries */

  #define MAX_API_BM_MSG_FLT_CMD 256

  #define API_BM_STATUS_HALT      1
  #define API_BM_STATUS_BUSY      2

  #define API_BM_RUN_SETUP        3
  #define API_BM_RUN_FAST         4

  #define API_BM_CMOD_ALL         0
  #define API_BM_CMOD_ONLY        1
  #define API_BM_CMOD_REC         2
  #define API_BM_CMOD_MSG_FLT_REC 3

  #define API_BM_STACK_STP        0
  #define API_BM_STACK_CTP        1
  #define API_BM_STACK_ETP        2
  #define API_BM_STACK_ABS        3

  #define API_BM_TAT_DEF          10000L
  #define API_BM_SWXM_DEF         0x000007FFUL

/* BM Activity and Message Filter */
  #define API_BM_ACT_TAGM_LOHI  0
  #define API_BM_ACT_TAGM_LO    1
  #define API_BM_ACT_TAGM_HI    2
  #define API_BM_ACT_TAGM_NONE  3

  #define API_BM_ACT_TFCT_DIS   0
  #define API_BM_ACT_TFCT_TXCWX 1
  #define API_BM_ACT_TFCT_T0CW1 2
  #define API_BM_ACT_TFCT_T0CWX 3

/* BM Trigger Control Block */
  #define API_BM_TCB_TRG_DIS    0
  #define API_BM_TCB_TRG_ENA    1

  #define API_BM_TCB_TYPE_ERR   0
  #define API_BM_TCB_TYPE_EXT   1
  #define API_BM_TCB_TYPE_RCV   2
  #define API_BM_TCB_TYPE_DATA  3
  #define API_BM_TCB_TYPE_HS	4

  #define API_BM_TCB_SOT_DIS    0
  #define API_BM_TCB_SOT_ENA    1

  #define API_BM_TCB_TRI_DIS    0
  #define API_BM_TCB_TRI_ENA    1

  #define API_BM_TCB_INV_DIS    0
  #define API_BM_TCB_INV_ENA    1

  #define API_BM_WRITE_ALL      0
  #define API_BM_WRITE_TI0      1
  #define API_BM_WRITE_TI1      2
  #define API_BM_WRITE_AEI      3
  #define API_BM_WRITE_ETI      4

  /* BM Dyntag Monitor */
  #define MAX_API_BM_DYNTAG_MON_ID  64  /* 11.11.05 */
  #define API_BM_DYNTAG_MON_DIS      0  /* 11.11.05 */
  #define API_BM_DYNTAG_MON_ENA      1  /* 11.11.05 */

/***************************************/
/*** Data Queue Constants            ***/
/***************************************/

  #define API_DATA_QUEUE_CTRL_START       0
  #define API_DATA_QUEUE_CTRL_STOP        1
  #define API_DATA_QUEUE_CTRL_SUSPEND     2
  #define API_DATA_QUEUE_CTRL_FLUSH       3

  #define API_DATA_QUEUE_STATUS_START      0x80000000
  #define API_DATA_QUEUE_STATUS_CAP_SINGLE 0x00800000
  #define API_DATA_QUEUE_STATUS_CAP_CONT   0x00400000
  #define API_DATA_QUEUE_STATUS_CHN_OPR    0x00300000
  #define API_DATA_QUEUE_STATUS_OVERFLOW   0x00008000
  #define API_DATA_QUEUE_STATUS_ERROR      0x0000FFFF


/***************************************/
/*** 1760 Protocol Constants         ***/
/***************************************/

  #define MAX_API_C1760_BUF  64
  #define API_C1760_DIS       0
  #define API_C1760_ENA       1
  #define MAX_API_C1760_C02  16
  #define MIN_API_C1760_WPOS  1
  #define MAX_API_C1760_WPOS (MAX_API_BUF_SIZE / 2)

/***************************************/
/*** API Host Interrupt Constants    ***/
/***************************************/

  #if defined(_NUCLEUS)
    #define GT_ICAUSE_BIU1_INDEX	0
    #define GT_ICAUSE_BIU2_INDEX	1
    #define GT_ICAUSE_DMA0_INDEX	2
    #define GT_ICAUSE_CMD__INDEX	4
  #endif


/*************************************/
/***  API Shared RAM Addresses     ***/
/*************************************/
/*
 * When converting absolute addresses to relative offsets we are sure
 * that the offset is within 32bit range. The TSW does not deal well
 * with uintptr_t and uintptr_t and a simple cast gives lots of warnings.
 * */
#define TRUNCATE_64BIT_TO32BIT(x) ((L_WORD)((uintptr_t)(x))%0xFFFFFFFFL)


/* this macro is used for Interrupt routines */

  #ifdef _AMC1553
    #ifndef _NUCLEUS
      #define API_SHARED_MEM_START_ADDR_IR  (p_api_dev->shared)
    #else
      #define API_SHARED_MEM_START_ADDR_IR  (px_api_dev->SharedRAMBase)
    #endif /* _NUCLEUS */
  #else
    #define API_SHARED_MEM_START_ADDR_IR  (px_api_dev->SharedRAMBase)
  #endif

  #define API_LOCAL_MEM_START_ADDR      (p_api_dev->LocalRAMBase)
  #define API_LOCAL_MEM_START_ADDR_ABS(addr) ((L_WORD)(addr)+API_LOCAL_MEM_START_ADDR)

  #ifndef _NUCLEUS
    #ifdef _AMC1553
      #define API_SHARED_MEM_START_ADDR     (p_api_dev->shared)
    #else
      #define API_SHARED_MEM_START_ADDR     (p_api_dev->SharedRAMBase)
    #endif
  #else
    #define API_SHARED_MEM_START_ADDR     (p_api_dev->SharedRAMBase)
  #endif /* _NUCLEUS */


  #define API_SHARED_MEM_ADDR_ABS(addr) (BYTE*) ((uintptr_t) (addr) + (uintptr_t) API_SHARED_MEM_START_ADDR)

  #define API_TARG_COM_REG_ADDR (API_SHARED_MEM_START_ADDR + 0x00001f00L) /* Target Control Registers */
  #define API_TARG_COM_CMD_ADDR (API_SHARED_MEM_START_ADDR + 0x00002000L) /* Target Command area */
  #define API_TARG_COM_ACK_ADDR (API_SHARED_MEM_START_ADDR + 0x00005000L) /* Target Acknowledge area */

/*************************************/
/***  API Dataset Memory Addresses ***/
/*************************************/

/* both following macros are used for interrupt routines */
  #define API_DATASET_MEM_START_ADDR_IR     (API_SHARED_MEM_START_ADDR_IR + 0x00090000L)
  #define API_DATASET_MEM_ADDR_ABS_IR(dsid) (((L_WORD)(dsid)*MAX_API_BUF_SIZE)+API_DATASET_MEM_START_ADDR_IR)

  #define API_DATASET_MEM_START_ADDR        (API_SHARED_MEM_START_ADDR + 0x00090000L)
  #define API_DATASET_MEM_ADDR_ABS(dsid)    (((L_WORD)(dsid)*MAX_API_BUF_SIZE)+API_DATASET_MEM_START_ADDR)
/*************************************/
/***  API Recording/Replay Buffer  ***/
/*************************************/

  #define API_REC_SHARED_MEM_START_ADDR   ((ptrdiff_t) API_SHARED_MEM_START_ADDR + (ptrdiff_t) 0x00010000L)

/*************************************/
/***  API Global Memory Addresses  ***/
/*************************************/
#define API_GRAM2_OFFSET 0x02000000

/* macros used for interrupt routines */
#ifdef _AMC1553
  #define API_GLB_MEM_ADDR_ABS_IR(addr, biu) (BYTE*) ( ((uintptr_t) (addr) >= (uintptr_t) API_GLB_MEM_START_ADDR) ? (uintptr_t) (addr) : (uintptr_t) (addr) + (uintptr_t) API_GLB_MEM_START_ADDR )
#else
  #define API_GLB_MEM_ADDR_ABS_IR(addr, biu)  (BYTE*)get_global_ram_addr_abs(px_api_dev, (uintptr_t)addr, biu)
#endif

/* macros used for normal routines */
  #define API_GLB_MEM_START_ADDR        (p_api_dev->GlobalRAMBase)

  #ifdef _AMC1553
    #define API_GLB_MEM_ADDR_ABS(addr)    (BYTE*) ( ((uintptr_t)(addr) >= (uintptr_t) API_GLB_MEM_START_ADDR) ? (uintptr_t)(addr) : (uintptr_t)(addr) + (uintptr_t) API_GLB_MEM_START_ADDR )
  #else
    #define API_GLB_MEM_ADDR_ABS(addr)    (BYTE*)get_global_ram_addr_abs(p_api_dev, (uintptr_t)(addr), p_api_dev->biu)
  #endif

  #ifdef _AMC1553
    #define API_GLB_MEM_ADDR_REL(addr)        (uintptr_t)(addr) >= (uintptr_t) API_GLB_MEM_START_ADDR ? TRUNCATE_64BIT_TO32BIT((uintptr_t)(addr) - (uintptr_t) API_GLB_MEM_START_ADDR) : TRUNCATE_64BIT_TO32BIT(addr)
    #define API_GLB_MEM_ADDR_REL_IR(addr,biu) (uintptr_t)(addr) >= (uintptr_t) API_GLB_MEM_START_ADDR ? TRUNCATE_64BIT_TO32BIT((uintptr_t)(addr) - (uintptr_t) API_GLB_MEM_START_ADDR) : TRUNCATE_64BIT_TO32BIT(addr)
  #else
    #define API_GLB_MEM_ADDR_REL(addr)        (uintptr_t)get_global_ram_addr_rel(p_api_dev, (uintptr_t)(addr), p_api_dev->biu)
    #define API_GLB_MEM_ADDR_REL_IR(addr,biu) (uintptr_t)get_global_ram_addr_rel(p_api_dev, (uintptr_t)(addr), biu)
  #endif



  /* The following defines are use to clarify problems that arise with boards that have more than one PBI
   * e.g. the ACX-6U or the AVX
   *
   * The defines below are named, using the following naming convention
   * - PBIREL
   *     is an offset to the start of the memory area the PBI can access. All offsets written
   *     into the GLOBAL memory to be accessed by the firmware have to be in this format.
   *     On boards with only one PBI, this is the same as GLOBREL.
   * - GLOBREL
   *     is an offset to the start of the global memory. This is needed for all direct memory
   *     accesses made by the DLL. As such all adresses provided to the DLL have to be in this format.
   *     On boards with only one PBI, this is the same as PBIREL.
   * - TSWPTR
   *     is a pointer within the target software. Whenever the target software needs to read/write
   *     to a location within the GLOBAL memory, this format has to be chosen.
   *
   */
  /* this is the normal set of defines, that assume that p_api_dev->biu can be used */
  #define API_GLB_PBIREL_TO_TSWPTR(offset)    API_GLB_MEM_ADDR_ABS(offset)
  #define API_GLB_PBIREL_TO_GLOBREL(offset)   BSWAP32(ul_ConvertGRAMPtrToTSWPtr( p_api_dev, p_api_dev->biu, offset)) /* BSWAP is done here to undo the BSWAP made in ul_ConvertGRAMPtrToTSWPtr */
  #define API_GLB_GLOBREL_TO_PBIREL(offset)   BSWAP32(ul_ConvertTSWPtrToGRAMPtr( p_api_dev, p_api_dev->biu, offset)) /* BSWAP is done here to undo the BSWAP made in ul_ConvertTSWPtrToGRAMPtr */
  #define API_GLB_GLOBREL_TO_TSWPTR(offset)   (BYTE*)(API_GLB_MEM_START_ADDR + offset)
  #define API_GLB_TSWPTR_TO_PBIREL(ptr)       (L_WORD)API_GLB_MEM_ADDR_REL(ptr)
  #define API_GLB_TSWPTR_TO_GLOBREL(ptr)      (L_WORD)((uintptr_t)ptr - (uintptr_t)API_GLB_MEM_START_ADDR)

  /* this is a set of defines, that assume p_api_dev->biu cannot be used.
   * as it's the case within the TSW interrupt handler (hence the name) */
  #define API_GLB_PBIREL_TO_TSWPTR_BIU(offset, logicalbiu)    API_GLB_MEM_ADDR_ABS_IR(offset, logicalbiu)
  #define API_GLB_PBIREL_TO_GLOBREL_BIU(offset, logicalbiu)   BSWAP32(ul_ConvertGRAMPtrToTSWPtr( p_api_dev, logicalbiu, offset)) /* BSWAP is done here to undo the BSWAP made in ul_ConvertTSWPtrToGRAMPtr */
  #define API_GLB_GLOBREL_TO_PBIREL_BIU(offset, logicalbiu)   BSWAP32(ul_ConvertTSWPtrToGRAMPtr( p_api_dev, logicalbiu, offset)) /* BSWAP is done here to undo the BSWAP made in ul_ConvertTSWPtrToGRAMPtr */
  #define API_GLB_GLOBREL_TO_TSWPTR_BIU(offset, logicalbiu)   (BYTE*)(API_GLB_MEM_START_ADDR + offset)
  #define API_GLB_TSWPTR_TO_PBIREL_BIU(ptr, logicalbiu)       (L_WORD)API_GLB_MEM_ADDR_REL_IR(ptr, logicalbiu)
  #define API_GLB_TSWPTR_TO_GLOBREL_BIU(ptr, logicalbiu)      (L_WORD)((uintptr_t)ptr - (uintptr_t)API_GLB_MEM_START_ADDR)


/*************************************/
/***  API Fifo Memory Addresses    ***/
/*************************************/

  #define API_FIFO_MEM_START_ADDR_BIU1	0x000D0000L
  #define API_FIFO_MEM_START_ADDR_BIU2  0x00150000L

/* both following macros used for Interrupt routines */
  #define API_FIFO_MEM_START_ADDR_IR     (((p_api_dev->biu == 0) ? API_FIFO_MEM_START_ADDR_BIU1 : API_FIFO_MEM_START_ADDR_BIU2))
  #define API_FIFO_MEM_ADDR_ABS_IR(addr) ((L_WORD)(addr)+API_SHARED_MEM_START_ADDR_IR)

  #define API_FIFO_MEM_START_ADDR     	(((p_api_dev->biu == 0) ? API_FIFO_MEM_START_ADDR_BIU1 : API_FIFO_MEM_START_ADDR_BIU2))

  #define API_FIFO_MEM_ADDR_ABS(addr) 	((L_WORD)(addr)+API_SHARED_MEM_START_ADDR)

/***********************************************/
/***  API Interrupt Loglist Memory Addresses ***/
/***********************************************/
  #define API_LOLI_MEM_START_ADDR	(API_SHARED_MEM_START_ADDR + 0x00008000L)


/****************************************/
/***  API Data Queue Memory Addresses ***/
/****************************************/
  #define API_DATA_QUEUE_START        	  0x1D0000L
  #define API_DATA_QUEUE_DATA_OFFSET   	  0x100L
  #define API_DATA_QUEUE_DATA_SIZE_MIN 	  0x400L
  #define API_DATA_QUEUE_SIZE_LS          0x100000L /* Max 1MB per LS Data Queue */
  #define API_DATA_QUEUE_SIZE_HS          0x400000L /* Max 4MB per HS Data Queue */
  #define API_DATA_QUEUE_SIZE_AYX_MS      0x18000L  /* Fix 96 KB */

  #define API_DATA_QUEUE_DATA_SIZE_LS          API_DATA_QUEUE_SIZE_LS - API_DATA_QUEUE_DATA_OFFSET
  #define API_DATA_QUEUE_DATA_SIZE_HS          API_DATA_QUEUE_SIZE_HS - API_DATA_QUEUE_DATA_OFFSET
  #define API_DATA_QUEUE_DATA_SIZE_AYX_MS      96 * API_DATA_QUEUE_DATA_SIZE_MIN  /* Fix 96 KB */

/*************************************/
/***  Misc Constants               ***/
/*************************************/

  #define API_OFF 0
  #define API_ON  1

  #define API_HALT 1
  #define API_BUSY 2

  #define API_BC_MODE      1
  #define API_RT_MODE      2
  #define API_BM_MODE      3
  #define API_REP_MODE     4
  #define API_SFT_MODE     5
  #define API_CAL_MODE     6
  #define API_PROT_MODE    7
  #define API_AINTR_MODE   8
  #define API_BIU_MODE     9
  #define API_RD_IRG_MODE  10
  #define API_BC_CONT_MODE 11

  #define API_SIM_AND_MON   0
  #define API_SIM_ONLY      1
  #define API_MON_ONLY      2

  #define API_BITE_OK 0

  #define API_BM_ENTRY_NFND 0
  #define API_BM_ENTRY_FND  1

  #define API_RESET_ALL                          0
  #define API_RESET_WITHOUT_MONITOR              1    /* without mon */
  #define API_RESET_WITHOUT_SIMBUF               2    /* without simbuf */
  #define API_RESET_WITHOUT_SIMBUF_MON           3    /* without simbuf and mon */
  #define API_RESET_ONLY_IF_NOT_ALREADY_RESETTED 4
  #define API_RESET_USE_COUNTER_MODE_2           0x40
  #define API_RESET_1760                         0x80

  #define API_SYS_SET_MEM_OK                    0
  #define API_SYS_SET_MEM_FAILED                1
  #define API_SYS_SET_MEM_PARAM_OUT_OF_RANGE    2

  #define API_BCRT_AREA     1
  #define API_BM_AREA       2

  #define API_BC_BUF        1
  #define API_RT_SA_BUF     2
  #define API_RT_RCV_MC_BUF 3
  #define API_RT_XMT_MC_BUF 4

  #define MAX_API_IR_EVENTS 256
  #define MAX_API_TIMER     4

  #define API_SYS_RESP_TOUT_DEF  (14 * 4) /* 0.25 usec */

/* Temporary arrays */
  #define MAX_API_BDATA_SIZE   256
  #define MAX_API_WDATA_SIZE   256
  #define MAX_API_LDATA_SIZE   2048

  #define API_LS_DYTAG_STD_MODE   0
  #define API_LS_DYTAG_EFA_MODE   1

  #define API_LS_DYTAG_FCT_DISABLE       0
  #define API_LS_DYTAG_FCT_POS_RAMP      1
  #define API_LS_DYTAG_FCT_NEG_RAMP      2
  #define API_LS_DYTAG_FCT_POS_TRIANGLE  3
  #define API_LS_DYTAG_FCT_NEG_TRIANGLE  4
  #define API_LS_DYTAG_FCT_XMT_WORD      5
  #define API_LS_DYTAG_FCT_SYNC_COUNTER  6

  #define API_LS_DYTAG_FCT_SAW_16        1
  #define API_LS_DYTAG_FCT_SAW_8_LOW     2
  #define API_LS_DYTAG_FCT_SAW_8_HIGH    3

  #define API_LS_DYTAG_DIRECT_MODIFY     0
  #define API_LS_DYTAG_FUNCTION_MODIFY   1

  #define API_LS_DYTAG_LAST_BIT_SET     0x08000000L
  #define API_LS_DYTAG_EFA_TAG_EQ_TYPE  0x40000000L

/* Error Injection */
  #define API_ERR_NONE                          0
  #define API_ERR_CSYNC                         1
  #define API_ERR_DSYNC                         2
  #define API_ERR_PARITY                        3
  #define API_ERR_MANHI                         4
  #define API_ERR_MANLO                         5
  #define API_ERR_CONTIG                        6
  #define API_ERR_WCHI                          7
  #define API_ERR_WCLO                          8
  #define API_ERR_BCHI                          9
  #define API_ERR_BCLO                          10
  #define API_ERR_ALTBUS                        11
  #define API_ERR_ZXING_NEG                     12
  #define API_ERR_ZXING_POS                     13
  #define API_ERR_HSSA                          14
  #define API_ERR_BUS_SWITCH                    15
  #define API_ERR_PHYSICAL_ERROR_SUPPRESSION    16
  #define API_ERR_WCLO_EXT                      17
  #define API_ERR_MSG_LENGTH_HI_IGNORE          18
  #define API_ERR_MSG_LENGTH_LO_IGNORE          19

/* Milbus Protocol Constants */
  #define API_RT_ALL         0
  #define API_RT_SING        1
  #define API_MILBUS_PROT_A  0
  #define API_MILBUS_PROT_B  1

/* Replay Constants*/
  #define API_REP_STOP       0
  #define API_REP_BUSY       1

/* Fifo Constants */

  #define MAX_API_QUEUE_SIZE 32
  #define MAX_API_FIFO_SIZE  128
/* System Dyanmic Data Words */
  #define MAX_API_SYS_LS_SYSTAGS  255 /* not 256 because parameter of systag_con is only byte */

  #define API_SYSTAG_DIS          0
  #define API_SYSTAG_ENA	        1

  #define API_SYSTAG_RUN_SUSPEND  0
  #define API_SYSTAG_RUN_RESUME   1
  #define API_SYSTAG_RUN_DISABLED 2
  #define API_SYSTAG_RUN_ENABLED  3
  #define API_SYSTAG_RECALC_CHECKSUM 4

  #define API_LS_SYSTAG_RESET_ALL      0
  #define API_LS_SYSTAG_RESET_BC_ALL   1
  #define API_LS_SYSTAG_RESET_RT_ALL   2
  #define API_LS_SYSTAG_RESET_RT_ONLY  3
  #define API_LS_SYSTAG_RESET_SA_ONLY  4
  #define API_LS_SYSTAG_RESET_XID_ONLY 5

  #define API_SYSTAG_NONE          0
  #define API_SYSTAG_RAMP_INC      1
  #define API_SYSTAG_RAMP_DEC 	   2
  #define API_SYSTAG_TRIANG_INC	   3
  #define API_SYSTAG_TRIANG_DEC    4
  #define API_SYSTAG_DATASET       5
  #define API_SYSTAG_STATES        6
  #define API_SYSTAG_COMP          7
  #define API_SYSTAG_CHECKSUM      8

/* Service Request and Vector Word handling */
  #define API_BC_SRVW_ID_NONE      0
  #define API_BC_SRVW_ID_SINGLE    1
  #define API_BC_SRVW_ID_MULDEL    2

  #define API_BC_SRVW_ID_SINGLE_RX 0
  #define API_BC_SRVW_ID_SINGLE_TX 1
  #define API_BC_SRVW_ID_MULTIP_RX 2
  #define API_BC_SRVW_ID_MULTIP_TX 3
  #define API_BC_SRVW_ID_DELETE_RX 4
  #define API_BC_SRVW_ID_DELETE_TX 5

  #define MAX_API_BC_SRVW_RT       32
  #define MAX_API_BC_SRVW_SAMID    128
  #define MAX_API_BC_SRVW_ID       8

  #define API_BC_SRVW_DIS          0x00
  #define API_BC_SRVW_ENA          0x01
  #define API_BC_SRVW_SINGLE       0x02
  #define API_BC_SRVW_MULTIPLE     0x03

  #define API_DONT_MODIFY_STATUS_BITS 0
  #define API_RESET_STATUS_BITS       1
  #define API_RESET_ERROR_BITS        2
  #define API_RESET_STAT_ERR_BITS     3
  #define API_RESET_SRVREQ_CNT        4

  /**************************************************************/
  /* USB Scatterlist defines                                    */
  /**************************************************************/

  /* size of one entry describing one data block (4 lonwords) */
  #define USB_SCATTERLIST_ENTRY_SIZE    16

  #define USB_SCATTERLIST_SIZE(count)   (USB_SCATTERLIST_ENTRY_SIZE * (count))

  /* Maximum number of entries for which space in global memory will be reserved for */
  #define MAX_USB_SCATTERLIST_ENTRIES   64


/****************************************/
/***   Target Communication           ***/
/****************************************/

/* Maximum size of memory area for Target Command Communication Array */
/* and Target Acknowledge Communication Array (in Bytes) */
  #ifndef MAX_API_CMD_SIZE
    #define MAX_API_CMD_SIZE (12*1024)
  #endif

  #ifndef MAX_API_DATA_BYTES
    #define MAX_API_DATA_BYTES (MAX_API_CMD_SIZE - 4) /* sub header bytes */
  #endif

/* Bit masks for Target Communication Control Registers */
  #define API_TG_COM_CONTROL_EXE_BIT_SET  0x00000001L
  #define API_TG_COM_STATUS_EXE_BIT_SET   0x00000001L
  #define API_TG_COM_STATUS_CPL_BIT_SET   0x00000002L
  #define API_TG_COM_STATUS_ERR_BIT_SET   0x00000004L
  #define API_TG_COM_CONTROL_EXE_BIT_RES  0xFFFFFFFEL
  #define API_TG_COM_STATUS_EXE_BIT_RES   0xFFFFFFFEL
  #define API_TG_COM_STATUS_CPL_BIT_RES   0xFFFFFFFDL
  #define API_TG_COM_STATUS_ERR_BIT_RES   0xFFFFFFFBL

/* Acknowledges */
  #define API_ACK      2
  #define API_ACK2     3
  #define API_NAK      4

/* Target Acknowledge codes */
  #define API_TG_ACK   0x20
  #define API_TG_ACK2  0x22
  #define API_TG_NAK   0x24

/* Bit masks for DMA Communication Control Registers */
  #define API_DMA_COM_CONTROL_EXE_BIT_SET  0x00000001L
  #define API_DMA_COM_STATUS_EXE_BIT_SET   0x00000001L
  #define API_DMA_COM_STATUS_CPL_BIT_SET   0x00000002L
  #define API_DMA_COM_STATUS_ERR_BIT_SET   0x00000004L
  #define API_DMA_COM_STATUS_INI_BIT_SET   0x80000000L
  #define API_DMA_COM_CONTROL_EXE_BIT_RES  0xFFFFFFFEL
  #define API_DMA_COM_STATUS_EXE_BIT_RES   0xFFFFFFFEL
  #define API_DMA_COM_STATUS_CPL_BIT_RES   0xFFFFFFFDL
  #define API_DMA_COM_STATUS_ERR_BIT_RES   0xFFFFFFFBL
  #define API_DMA_COM_STATUS_INI_BIT_RES   0x7FFFFFFFL

  #define API_DMA_DIRECTION_READ_GLOBAL  0
  #define API_DMA_DIRECTION_READ_SHARED  1
  #define API_DMA_DIRECTION_READ_LOCAL   2
  #define API_DMA_DIRECTION_WRITE_GLOBAL 3
  #define API_DMA_DIRECTION_WRITE_SHARED 4
  #define API_DMA_DIRECTION_WRITE_LOCAL  5

#ifdef _AIM_1553_SYSDRV_USB
  /* With USB devices we don't have that much global RAM,
   * and allocation of large memory blocks in system driver is not recommended
   */
  #define MAX_MEM_MUL                  1
#else
  /* Using a value of 32 here will create infinity loops
     on api reset. This is because the indexes are WORD
     but some of the areas grow to big and need L_WORD   */
  #define MAX_MEM_MUL                  16   /* This defines a maximum of Global Memory size 16 MByte */
#endif /* _AIM_1553_SYSDRV_USB */

/* Mimimum Block size to allocate
 * Each allocation in the simulator memory
 * is a multiple of this size in BYTES
 */
  #define API_3910_MALLOC_BLOCK_SIZE   0x40
  #define API_HS_SIM_MEM_MAP_SIZE      ((16*1024*1024) / (API_3910_MALLOC_BLOCK_SIZE*32)) /* amount of long words */


  #define API_MAX_MUX_BUFFER              256
  #define API_DATA_QUEUE_ID_BM_REC_BIU1   0
  #define API_DATA_QUEUE_ID_BM_REC_BIU2   1
  #define API_DATA_QUEUE_ID_BM_REC_BIU3   2
  #define API_DATA_QUEUE_ID_BM_REC_BIU4   3
  #define API_DATA_QUEUE_ID_BM_REC_BIU5   4
  #define API_DATA_QUEUE_ID_BM_REC_BIU6   5
  #define API_DATA_QUEUE_ID_BM_REC_BIU7   6
  #define API_DATA_QUEUE_ID_BM_REC_BIU8   7
  #define API_DATA_QUEUE_ID_MIL_SCOPE     8
  #define API_DATA_QUEUE_ID_GENERIC_ACQ   9  /* Generic Data Queue Support */

  #define API_MAX_DATA_QUEUE_ID           API_DATA_QUEUE_ID_GENERIC_ACQ
  #define API_MAX_SDP_ID                  1024

  #define API_SCOPE_PBI_MASK              0xF /* Board Sub Type / MilScope PBI identification */
  #define API_SCOPE_PBI_PBI1              0x1
  #define API_SCOPE_PBI_PBI2              0x4

  #define API_SCOPE_START_CONTINUOUS      1
  #define API_SCOPE_START_SINGLE          2


/*! \enum TY_API_SCOPE_OPERATION_MODE
    possible values for scope operation parameter of function ApiCmdScopeSetup */
enum TY_API_SCOPE_OPERATION_MODE
{
    API_SCOPE_OPR_CHANNEL_DISABLED = 0,     /*!< Both channels are disabled */
    API_SCOPE_OPR_CHANNEL_A = 1,            /*!< scope on primary channel with 50M samples */
    API_SCOPE_OPR_CHANNEL_B = 2,            /*!< scope on secondary channel with 50M samples */
    API_SCOPE_OPR_CHANNEL_AB = 3,           /*!< scope on both channels with 50M samples each */
    API_SCOPE_OPR_CHANNEL_A100 = 4,         /*!< scope on primary channel with 100M samples */
    API_SCOPE_OPR_CHANNEL_B100 = 5,         /*!< scope on secondary channel with 100M samples */
};


/*! \enum TY_API_SCOPE_COUPLING
    possible values for scope coupling parameter of function ApiCmdScopeSetup */
enum TY_API_SCOPE_COUPLING
{
    API_SCOPE_COUPLING_SECONDARY = 0,   /*!< channel is over-cross connected to other channel (For 100M mode) */
    API_SCOPE_COUPLING_EXTERN = 4,      /*!< channel is externally coupled with 30V range */
    API_SCOPE_COUPLING_STUB = 5,        /*!< channel is connected to MIL bus stub with 30V range*/
    API_SCOPE_COUPLING_STUB_3V = 6,     /*!< channel is connected to MIL bus stub with 3V range */
    API_SCOPE_COUPLING_EXTERN_3V = 7    /*!< channel is externally coupled with 3V range */
};



/***************************************/
/*** Track Type Constants            ***/
/***************************************/
  #define API_TRACK_TYPE_BC           1
  #define API_TRACK_TYPE_RT           2
  #define API_TRACK_TYPE_MUX          3

/* platform dependent monitor function call: wait_clocks */

  #ifdef _AMC1553
    #if defined(_WIN32) && defined(_AIM_SYSDRV)

      #define WAIT_CLOCKS(clks)    ai_tsw_os_wait_usec(clks)

      #ifdef DBG
        #define PRINTF0(STRING)               DbgPrint(STRING)
        #define PRINTF1(STRING,VAR1)          DbgPrint(STRING,VAR1)
        #define PRINTF2(STRING,VAR1,VAR2)     DbgPrint(STRING,VAR1,VAR2)
        #define PRINTF3(STRING,VAR1,VAR2,VAR3) DbgPrint(STRING,VAR1,VAR2,VAR3)
        #define PRINTF4(STRING,VAR1,VAR2,VAR3,VAR4)           DbgPrint(STRING,VAR1,VAR2,VAR3,VAR4)
        #define PRINTF5(STRING,VAR1,VAR2,VAR3,VAR4,VAR5)      DbgPrint(STRING,VAR1,VAR2,VAR3,VAR4,VAR5)
        #define PRINTF6(STRING,VAR1,VAR2,VAR3,VAR4,VAR5,VAR6) DbgPrint(STRING,VAR1,VAR2,VAR3,VAR4,VAR5,VAR6)
        #define PRINTF7(STRING,VAR1,VAR2,VAR3,VAR4,VAR5,VAR6,VAR7) DbgPrint(STRING,VAR1,VAR2,VAR3,VAR4,VAR5,VAR6,VAR7)
        #define PRINTF8(STRING,VAR1,VAR2,VAR3,VAR4,VAR5,VAR6,VAR7,VAR8) DbgPrint(STRING,VAR1,VAR2,VAR3,VAR4,VAR5,VAR6,VAR7,VAR8)
      #else
        #define PRINTF0(STRING)
        #define PRINTF1(STRING,VAR1)
        #define PRINTF2(STRING,VAR1,VAR2)
        #define PRINTF3(STRING,VAR1,VAR2,VAR3)
        #define PRINTF4(STRING,VAR1,VAR2,VAR3,VAR4)
        #define PRINTF5(STRING,VAR1,VAR2,VAR3,VAR4,VAR5)
        #define PRINTF6(STRING,VAR1,VAR2,VAR3,VAR4,VAR5,VAR6)
        #define PRINTF7(STRING,VAR1,VAR2,VAR3,VAR4,VAR5,VAR6,VAR7)
        #define PRINTF8(STRING,VAR1,VAR2,VAR3,VAR4,VAR5,VAR6,VAR7,VAR8)
      #endif
      #define MALLOC(Size) ExAllocatePoolWithTag( NonPagedPool, Size, '1WST' )
      #define FREE(Addr)   ExFreePoolWithTag( Addr, '1WST' )
    #elif defined(_AIM_RTX64)

        typedef unsigned char UINT8;
        typedef unsigned short UINT16;
        typedef unsigned int   UINT32;
        #define WAIT_CLOCKS(clks)    ai_tsw_os_wait_usec(clks)

        #ifdef DBG
        #define PRINTF0(STRING)               RtPrintf(STRING)
        #define PRINTF1(STRING,VAR1)          RtPrintf(STRING,VAR1)
        #define PRINTF2(STRING,VAR1,VAR2)     RtPrintf(STRING,VAR1,VAR2)
        #define PRINTF3(STRING,VAR1,VAR2,VAR3) RtPrintf(STRING,VAR1,VAR2,VAR3)
        #define PRINTF4(STRING,VAR1,VAR2,VAR3,VAR4)           RtPrintf(STRING,VAR1,VAR2,VAR3,VAR4)
        #define PRINTF5(STRING,VAR1,VAR2,VAR3,VAR4,VAR5)      RtPrintf(STRING,VAR1,VAR2,VAR3,VAR4,VAR5)
        #define PRINTF6(STRING,VAR1,VAR2,VAR3,VAR4,VAR5,VAR6) RtPrintf(STRING,VAR1,VAR2,VAR3,VAR4,VAR5,VAR6)
        #define PRINTF7(STRING,VAR1,VAR2,VAR3,VAR4,VAR5,VAR6,VAR7) RtPrintf(STRING,VAR1,VAR2,VAR3,VAR4,VAR5,VAR6,VAR7)
        #define PRINTF8(STRING,VAR1,VAR2,VAR3,VAR4,VAR5,VAR6,VAR7,VAR8) RtPrintf(STRING,VAR1,VAR2,VAR3,VAR4,VAR5,VAR6,VAR7,VAR8)
        #else
        #define PRINTF0(STRING)
        #define PRINTF1(STRING,VAR1)
        #define PRINTF2(STRING,VAR1,VAR2)
        #define PRINTF3(STRING,VAR1,VAR2,VAR3)
        #define PRINTF4(STRING,VAR1,VAR2,VAR3,VAR4)
        #define PRINTF5(STRING,VAR1,VAR2,VAR3,VAR4,VAR5)
        #define PRINTF6(STRING,VAR1,VAR2,VAR3,VAR4,VAR5,VAR6)
        #define PRINTF7(STRING,VAR1,VAR2,VAR3,VAR4,VAR5,VAR6,VAR7)
        #define PRINTF8(STRING,VAR1,VAR2,VAR3,VAR4,VAR5,VAR6,VAR7,VAR8)
        #endif
        #define MALLOC(Size) ai_tsw_os_malloc(Size)
        #define FREE(Addr)   ai_tsw_os_free(Addr)

    #elif defined(_NUCLEUS)
      #define WAIT_CLOCKS(clks)    wait_clocks(clks);
      #define PRINTF0(STRING)
      #define PRINTF1(STRING,VAR1)
      #define PRINTF2(STRING,VAR1,VAR2)
      #define PRINTF3(STRING,VAR1,VAR2,VAR3)
      #define PRINTF4(STRING,VAR1,VAR2,VAR3,VAR4)
      #define PRINTF5(STRING,VAR1,VAR2,VAR3,VAR4,VAR5)
      #define PRINTF6(STRING,VAR1,VAR2,VAR3,VAR4,VAR5,VAR6)
      #define PRINTF7(STRING,VAR1,VAR2,VAR3,VAR4,VAR5,VAR6,VAR7)
      #define PRINTF8(STRING,VAR1,VAR2,VAR3,VAR4,VAR5,VAR6,VAR7,VAR8)
      #define MALLOC(Size) allocate_memory(Size)
      #define FREE(Addr) deallocate_memory(Addr)
      #define wbflush()     drain_wbuf()
    #elif defined(_LINUX)
      #include <linux/vmalloc.h>     /* vmalloc */
      #include <linux/string.h>     /* memset */
      #include <linux/version.h>
      #if LINUX_VERSION_CODE >= KERNEL_VERSION(3,4,0)
        #include <asm/barrier.h>
      #else
        #include <asm/system.h>       /* mb */
      #endif
      typedef unsigned char UINT8;
      typedef unsigned short UINT16;
      typedef unsigned int   UINT32;

      #define WAIT_CLOCKS(clks)    ai_tsw_os_wait_usec( clks );

      #ifdef _DEBUG
        #define PRINTF0(STRING)               printk( KERN_DEBUG "aim_mil" STRING )
        #define PRINTF1(STRING,VAR1)          printk( KERN_DEBUG "aim_mil" STRING,VAR1 )
        #define PRINTF2(STRING,VAR1,VAR2)     printk( KERN_DEBUG "aim_mil" STRING,VAR1,VAR2 )
        #define PRINTF3(STRING,VAR1,VAR2,VAR3) printk( KERN_DEBUG "aim_mil" STRING,VAR1,VAR2,VAR3 )
        #define PRINTF4(STRING,VAR1,VAR2,VAR3,VAR4)           printk( KERN_DEBUG "aim_mil" STRING,VAR1,VAR2,VAR3,VAR4 )
        #define PRINTF5(STRING,VAR1,VAR2,VAR3,VAR4,VAR5)      printk( KERN_DEBUG "aim_mil" STRING,VAR1,VAR2,VAR3,VAR4,VAR5 )
        #define PRINTF6(STRING,VAR1,VAR2,VAR3,VAR4,VAR5,VAR6) printk( KERN_DEBUG "aim_mil" STRING,VAR1,VAR2,VAR3,VAR4,VAR5,VAR6 )
        #define PRINTF7(STRING,VAR1,VAR2,VAR3,VAR4,VAR5,VAR6,VAR7) printk( KERN_DEBUG "aim_mil" STRING,VAR1,VAR2,VAR3,VAR4,VAR5,VAR6,VAR7 )
        #define PRINTF8(STRING,VAR1,VAR2,VAR3,VAR4,VAR5,VAR6,VAR7,VAR8) printk( KERN_DEBUG "aim_mil" STRING,VAR1,VAR2,VAR3,VAR4,VAR5,VAR6,VAR7,VAR8 )
      #else
        #define PRINTF0(STRING)
        #define PRINTF1(STRING,VAR1)
        #define PRINTF2(STRING,VAR1,VAR2)
        #define PRINTF3(STRING,VAR1,VAR2,VAR3)
        #define PRINTF4(STRING,VAR1,VAR2,VAR3,VAR4)
        #define PRINTF5(STRING,VAR1,VAR2,VAR3,VAR4,VAR5)
        #define PRINTF6(STRING,VAR1,VAR2,VAR3,VAR4,VAR5,VAR6)
        #define PRINTF7(STRING,VAR1,VAR2,VAR3,VAR4,VAR5,VAR6,VAR7)
        #define PRINTF8(STRING,VAR1,VAR2,VAR3,VAR4,VAR5,VAR6,VAR7,VAR8)
      #endif
      #define MALLOC(Size) ai_tsw_os_malloc(Size)
      #define FREE(Addr) ai_tsw_os_free(Addr)
      #undef wbflush
      #define wbflush() mb()
    #else
      #ifdef _DEBUG
        #define PRINTF0(STRING)                               printf(STRING)
        #define PRINTF1(STRING,VAR1)                          printf(STRING,VAR1)
        #define PRINTF2(STRING,VAR1,VAR2)                     printf(STRING,VAR1,VAR2)
        #define PRINTF3(STRING,VAR1,VAR2,VAR3)                printf(STRING,VAR1,VAR2,VAR3)
        #define PRINTF4(STRING,VAR1,VAR2,VAR3,VAR4)           printf(STRING,VAR1,VAR2,VAR3,VAR4)
        #define PRINTF5(STRING,VAR1,VAR2,VAR3,VAR4,VAR5)      printf(STRING,VAR1,VAR2,VAR3,VAR4,VAR5)
        #define PRINTF6(STRING,VAR1,VAR2,VAR3,VAR4,VAR5,VAR6) printf(STRING,VAR1,VAR2,VAR3,VAR4,VAR5,VAR6)
        #define PRINTF7(STRING,VAR1,VAR2,VAR3,VAR4,VAR5,VAR6,VAR7) printf(STRING,VAR1,VAR2,VAR3,VAR4,VAR5,VAR6,VAR7)
        #define PRINTF8(STRING,VAR1,VAR2,VAR3,VAR4,VAR5,VAR6,VAR7,VAR8) printf(STRING,VAR1,VAR2,VAR3,VAR4,VAR5,VAR6,VAR7,VAR8)
      #else
        #define PRINTF0(STRING)
        #define PRINTF1(STRING,VAR1)
        #define PRINTF2(STRING,VAR1,VAR2)
        #define PRINTF3(STRING,VAR1,VAR2,VAR3)
        #define PRINTF4(STRING,VAR1,VAR2,VAR3,VAR4)
        #define PRINTF5(STRING,VAR1,VAR2,VAR3,VAR4,VAR5)
        #define PRINTF6(STRING,VAR1,VAR2,VAR3,VAR4,VAR5,VAR6)
        #define PRINTF7(STRING,VAR1,VAR2,VAR3,VAR4,VAR5,VAR6,VAR7)
        #define PRINTF8(STRING,VAR1,VAR2,VAR3,VAR4,VAR5,VAR6,VAR7,VAR8)
      #endif
      #define MALLOC(Size)   malloc(Size)
      #define FREE(Addr) free(Addr)
      #define WAIT_CLOCKS(clks)    ai_tsw_os_wait_usec( clks );
      #define wbflush()
    #endif
  #else
    #if defined(_NUCLEUS)
      #define WAIT_CLOCKS(clks)    wait_clocks(clks);
      #define PRINTF0(STRING)                printf(STRING)
      #define PRINTF1(STRING,VAR1)           printf(STRING,VAR1)
      #define PRINTF2(STRING,VAR1,VAR2)      printf(STRING,VAR1,VAR2)
      #define PRINTF3(STRING,VAR1,VAR2,VAR3) printf(STRING,VAR1,VAR2,VAR3)
      #define PRINTF4(STRING,VAR1,VAR2,VAR3,VAR4)           printf(STRING,VAR1,VAR2,VAR3,VAR4)
      #define PRINTF5(STRING,VAR1,VAR2,VAR3,VAR4,VAR5)      printf(STRING,VAR1,VAR2,VAR3,VAR4,VAR5)
      #define PRINTF6(STRING,VAR1,VAR2,VAR3,VAR4,VAR5,VAR6) printf(STRING,VAR1,VAR2,VAR3,VAR4,VAR5,VAR6)
      #define PRINTF7(STRING,VAR1,VAR2,VAR3,VAR4,VAR5,VAR6,VAR7) printf(STRING,VAR1,VAR2,VAR3,VAR4,VAR5,VAR6,VAR7)
      #define PRINTF8(STRING,VAR1,VAR2,VAR3,VAR4,VAR5,VAR6,VAR7,VAR8) printf(STRING,VAR1,VAR2,VAR3,VAR4,VAR5,VAR6,VAR7,VAR8)
      #define MALLOC(Size) allocate_memory(Size)
      #define FREE(Addr) deallocate_memory(Addr)
      #define wbflush()     drain_wbuf()
    #else
      #define WAIT_CLOCKS(clks)    wait_clocks(clks);
      #define PRINTF0(STRING)                printf(STRING)
      #define PRINTF1(STRING,VAR1)           printf(STRING,VAR1)
      #define PRINTF2(STRING,VAR1,VAR2)      printf(STRING,VAR1,VAR2)
      #define PRINTF3(STRING,VAR1,VAR2,VAR3) printf(STRING,VAR1,VAR2,VAR3)
      #define PRINTF4(STRING,VAR1,VAR2,VAR3,VAR4)           printf(STRING,VAR1,VAR2,VAR3,VAR4)
      #define PRINTF5(STRING,VAR1,VAR2,VAR3,VAR4,VAR5)      printf(STRING,VAR1,VAR2,VAR3,VAR4,VAR5)
      #define PRINTF6(STRING,VAR1,VAR2,VAR3,VAR4,VAR5,VAR6) printf(STRING,VAR1,VAR2,VAR3,VAR4,VAR5,VAR6)
      #define PRINTF7(STRING,VAR1,VAR2,VAR3,VAR4,VAR5,VAR6,VAR7) printf(STRING,VAR1,VAR2,VAR3,VAR4,VAR5,VAR6,VAR7)
      #define PRINTF8(STRING,VAR1,VAR2,VAR3,VAR4,VAR5,VAR6,VAR7,VAR8) printf(STRING,VAR1,VAR2,VAR3,VAR4,VAR5,VAR6,VAR7,VAR8)
      #define MALLOC(Size)   mmalloc(Size)
      #define FREE(Addr) mfree(Addr)
    #endif
  #endif /* _AMC1553 */

  #ifndef CALL_SCHEDULER
    #define CALL_SCHEDULER() do { } while( 0 );
  #endif /* CALL_SCHEDULER */



  /* device type */
  #define TYPE_APX1553_1            22
  #define TYPE_APX1553_2            23
  #define TYPE_APX1553_4            24
  #define TYPE_APX3910              25
  #define TYPE_APX3910Xp            26

  #define TYPE_ACX1553_1            27
  #define TYPE_ACX1553_2            28
  #define TYPE_ACX1553_2_TWO_PBIS   29
  #define TYPE_ACX1553_4            30
  #define TYPE_ACX1553_4_TWO_PBIS   31
  #define TYPE_ACX1553_8            32
  #define TYPE_ACX3910              33
  #define TYPE_ACX3910Xp            34
  #define TYPE_ACX3910_2            35
  #define TYPE_ACX3910Xp_2          36

  #define TYPE_AVX1553_1            37
  #define TYPE_AVX1553_2            38
  #define TYPE_AVX1553_2_TWO_PBIS   39
  #define TYPE_AVX1553_4            40
  #define TYPE_AVX1553_4_TWO_PBIS   41
  #define TYPE_AVX1553_8            42
  #define TYPE_AVX3910              43
  #define TYPE_AVX3910Xp            44
  #define TYPE_AVX3910_2            45
  #define TYPE_AVX3910Xp_2          46

  #define TYPE_ACX1553_4_DS_TWO_PBIS     47
  #define TYPE_ACX_EFA_1_TWO_PBIS        48
  #define TYPE_ACX_EFAXp_1_TWO_PBIS      49
  #define TYPE_ACX_EFA_1_DS_TWO_PBIS     50
  #define TYPE_ACX_EFAXp_1_DS_TWO_PBIS   51
  #define TYPE_ACX_EFA_2_TWO_PBIS        52
  #define TYPE_ACX_EFAXp_2_TWO_PBIS      53
  #define TYPE_ACX_EFA_2_DS_TWO_PBIS     54
  #define TYPE_ACX_EFAXp_2_DS_TWO_PBIS   55
  #define TYPE_ACX_EFA_4_TWO_PBIS        56
  #define TYPE_ACX_EFAXp_4_TWO_PBIS      57

  #define TYPE_AVX1553_4_DS_TWO_PBIS     58
  #define TYPE_AVX_EFA_1_TWO_PBIS        59
  #define TYPE_AVX_EFAXp_1_TWO_PBIS      60
  #define TYPE_AVX_EFA_1_DS_TWO_PBIS     61
  #define TYPE_AVX_EFAXp_1_DS_TWO_PBIS   62
  #define TYPE_AVX_EFA_2_TWO_PBIS        63
  #define TYPE_AVX_EFAXp_2_TWO_PBIS      64
  #define TYPE_AVX_EFA_2_DS_TWO_PBIS     65
  #define TYPE_AVX_EFAXp_2_DS_TWO_PBIS   66
  #define TYPE_AVX_EFA_4_TWO_PBIS        67
  #define TYPE_AVX_EFAXp_4_TWO_PBIS      68

  #define TYPE_APU1553_1                 72
  #define TYPE_APU1553_2                 73

  #define TYPE_AEC1553_1                 74
  #define TYPE_AEC1553_2                 75

  #define TYPE_AXC1553_1                 76
  #define TYPE_AXC1553_2                 77
  #define TYPE_AXC1553_4                 78

  #define TYPE_AMCX1553_1                79
  #define TYPE_AMCX1553_2                80
  #define TYPE_AMCX1553_4                81

  #define TYPE_APE1553_1                 82
  #define TYPE_APE1553_2                 83
  #define TYPE_APE1553_4                 84


  #define TYPE_ANET1553_1                 85
  #define TYPE_ANET1553_2                 86

  #define TYPE_ANET3910                  87
  #define TYPE_ANET3910Xp                88

  #define TYPE_ASC1553_1                 89


  #define TYPE_APXX3910                  90
  #define TYPE_APXX3910Xp                91
  #define TYPE_APEX3910                  92
  #define TYPE_APEX3910Xp                93

  #define TYPE_AME1553_1                 94

  #define TYPE_ACE3910                   95
  #define TYPE_ACE3910Xp                 96

  #define TYPE_AXE1553_1                 97
  #define TYPE_AXE1553_2                 98
  #define TYPE_AXE1553_4                 99

  #define TYPE_AMCE1553_1                100
  #define TYPE_AMCE1553_2                101
  #define TYPE_AMCE1553_4                102

  #define TYPE_ASE1553_1                 103
  #define TYPE_ASE1553_2                 104
  #define TYPE_ASE1553_4                 105

  #define TYPE_AXC3910                   106
  #define TYPE_AXC3910Xp                 107

  #define TYPE_AMEE1553_1                108
  #define TYPE_AMEE1553_2                109

  #define TYPE_ASC1553_2                 110

  #define TYPE_AXCX1553_1                111
  #define TYPE_AXCX1553_2                112

  #define TYPE_AM2E1553_1                113
  #define TYPE_AM2E1553_2                114
  
  #define TYPE_AXCX1553_4                115


  #define API_TYPE_NONE              0
  #define API_TYPE_1553              1
  #define API_TYPE_3910              2
  
  


  /******************************************/
  /*  Defines for 1760 DTS System           */
  /******************************************/

#if defined(_NUCLEUS) /* only for APX */

  #define _CMPL4DTS

  /*#define DEBUG_IP*/
  #define ENA_TESTSIGNAL

  #define MAX_IP_DRV 4

  #define API_INIT_IP_TYPE 0
  #define API_APX_IP_TYPE  1
  #define API_DIG_IP_TYPE  2
  #define API_ANA_IP_TYPE  3
  #define API_RS_IP_TYPE   4

  #define MAX_IP_DOUT_CHN 32
  #define MAX_IP_DIN_CHN  32
  #define MAX_APX_DOUT_CHN 8
  #define MAX_APX_DIN_CHN  8
  #define MAX_IP_AOUT_CHN  4
  #define MAX_IP_AIN_CHN  16   /* 8 Differential / 16 normal */
  /*#define MAX_IP_RS_CHN    8 */

#endif /* defined(_NUCLEUS) */





/**********************************************************************************/
/***  defines for accessing the monitor activity recording/message filtering page */
/**********************************************************************************/


/* l-word offset defines for monitor activity recording/message filtering page entries */

#define ACT_CTRL   0     /* Monitor acvtivity and message filter control word */
#define ACT_CNT    1     /* 32bit transfer counter. Only primary, if activity page is enhanced */
#define ACT_ERR    2     /* 32bit error counter. Only primary, if activity page is enhanced    */
#define ACT_CNT2   3     /* 32bit transfer counter secondary, if activity page is enhanced */
#define ACT_ERR2   4     /* 32bit error counter secondary, if activity page is enhanced */





#define GET_BM_ACTIVITY_DESC_BY_BIU(p_api_dev, ucBiu, rt, sa, xmt)        \
    ((p_api_dev->ulUseEnhancedBmActivityPage) ? \
            (TY_API_BM_ACTIVITY*)API_GLB_MEM_ADDR_ABS(p_api_dev->glb_mem.biu[ucBiu].bm_act_addr \
                                 + (rt * 0xA00)                            \
                                 + (xmt * 0x280)                           \
                                 + (sa * 0x14)                             \
                                 ) \
    : \
            (TY_API_BM_ACTIVITY*)API_GLB_MEM_ADDR_ABS(p_api_dev->glb_mem.biu[ucBiu].bm_act_addr \
                                 + (rt * 0x600)                            \
                                 + (xmt * 0x180)                           \
                                 + (sa * 0xC)                              \
                                 ))

#define GET_BM_ACTIVITY_DESC(p_api_dev, rt, sa, xmt)  \
    GET_BM_ACTIVITY_DESC_BY_BIU(p_api_dev, p_api_dev->biu, rt, sa, xmt)


/* other defines for the BM activity and filtering page */
#define API_BM_ACT_PAGE_SIZE        0xC000
#define API_BM_ACT_ENH_PAGE_SIZE   0x14000




/* Defines from Windows System Driver api_defs.h */
#define SET_REP_ADDR 5
#define API_PCI_REC_ADDR    0x00010000L  /* Recording/Replay Array */ /* changed 31.03.2000 pg */
#define API_PCI_HS_REC_ADDR 0x00400000L /* HS Recording/Replay Array */ /* added 04.04.2000 pg */
#define MAX_REC_SIZE        0x00080000L
#define MAX_HS_REC_SIZE     0x00103000L
#define MAX_REP_SIZE        0x00080000L
#define MAX_HS_REP_SIZE     0x000C0000L

#define MEM_TYPE_USB_EEPROM 0
#define MEM_TYPE_USB_8051_RAM 1
#define API_PCI_CTRL_REG       0x1F00L    /* Target Control Register */
#define API_PCI_CMD_ADDR       0x2000L    /* Command Array */




#endif /* __API_DEFS_H__ */

/* end of file */
