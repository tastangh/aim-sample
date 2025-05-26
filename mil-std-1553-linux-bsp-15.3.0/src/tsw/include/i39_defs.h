/* SPDX-FileCopyrightText: 2015-2024 AIM GmbH <info@aim-online.com> */
/* SPDX-License-Identifier: MIT OR GPL-2.0-or-later */

/*! \file i39_defs.h
 *
 *  Header information of the HS Driver Software. Definition of variables, arrays and structures, external references.   
 *
 */

#ifndef __I39_DEFS_H__
  #define __I39_DEFS_H__


/*************************************/
/***  ASP 3910 BIU Constants       ***/
/*************************************/


  #define API39_BIU_1           0       /*  Bus Interface Unit 1 for structure setting */
  #define API39_BIU_2           1       /*  Bus Interface Unit 2 for structure setting */
  #define API39_BIU_3           1       /*  Bus Interface Unit 3 for structure setting */
  #define API39_BIU_4           1       /*  Bus Interface Unit 4 for structure setting */

#if defined(_NUCLEUS)
  /*Only 6U boards can have 2 3910 streams */
  #define API39_MAX_STREAM        2
#else
  #define API39_MAX_STREAM        1
#endif

  #define API39_LS_BIU_1          0       /*  Bus Interface Low Speed Biu 1 for structure setting */
  #define API39_LS_BIU_2          2       /*  Bus Interface Low Speed Biu 3 for structure setting */
  #define API39_HS_BIU_1          0       /*  Bus Interface High Speed Stream 1 for structure setting */
  #define API39_HS_BIU_2          1       /*  Bus Interface High Speed Stream 2 for structure setting */

  #define API3910_HS_CMD_DEST_LL    0x00000800L  /* Indicates HS Command lower limit*/ 
  #define API3910_HS_CMD_DEST_UL    0x00004CFFL  /* Indicates HS (3910 and EFEX) Command upper limit*/ 

/*************************************/
/***  ASP 3910 Driver Command Codes ***/
/*************************************/

/* GPIO Commands */
  #define API39_GPIO_RESET              0x00000860   /*   -  ACK2   17  */
  #define API39_GPIO_BROADC_CMD         0x00000861   /*   1  ACK     -  */
  #define API39_GPIO_BROADC_WRITE       0x00000862   /*   2  ACK     -  */
  #define API39_GPIO_CMD                0x00000863   /*   2  ACK     -  */
  #define API39_GPIO_STATUS             0x00000864   /*   1  ACK2    1  */
  #define API39_GPIO_READ               0x00000865   /*   2  ACK2    1  */
  #define API39_GPIO_WRITE              0x00000866   /*   3  ACK     -  */
  #define API39_GPIO_READ_BLOCK         0x00000867   /*   3  ACK2    2+ */
  #define API39_GPIO_WRITE_BLOCK        0x00000868   /*   4+ ACK     -  */




/*************************************/
/*** General Constants             ***/
/*************************************/

/* System Status Word */
  #define API39_SCB_FID_CONFIG_BITMASK  0x00FF0000L     /* Board COnfiguration Bitmask */
  #define API39_SCB_FID_CONFIG_BITPOS   16          /* Board Configuration Bitposition */

  #define API39_SCB_FID_CONFIG_1553 1           /* Board Configuration 1553        */
  #define API39_SCB_FID_CONFIG_3910 4           /* Board Configuration 3910        */

  #define API39_BOARD_MASK      0x0000F000L
  #define API39_BOARD_TYPE_EFEX     0x7000
  #define API39_BOARD_TYPE_3910     0x3000
  #define API39_BOARD_TYPE_1553     0x1000
  #define API39_BOARD_TYPE_1553_4   0xA000

  #define API39_BOARD_MASK_2          0xF0000000L
  #define API39_BOARD_TYPE_EFEX_2     0x70000000L
  #define API39_BOARD_TYPE_3910_2     0x30000000L

  #define API39_TYPE_1553       0x0
  #define API39_TYPE_3910       0x1
  #define API39_TYPE_EFEX       0x2


/*************************************/
/*** Buffer Constants              ***/
/*************************************/

  #define MAX_API39_DEFAULT_BUF_ID      256 /* used for the default memory mapping */

  #define MAX_API39_HID         512

  #define MAX_API39_BID         256     /* Describes the queue size (limit see fw spec) rh*/

  #define API39_REP_BUF_AREA_BYTE_SIZE  0x0C0000L

  #define API39_BUF_BC_TYPE     1
  #define API39_BUF_RT_TYPE     2
  #define MAX_API39_BUF_SIZE_FCPA_ADD    4132

  #define MAX_BM_BUF_SIZE_IN_RESET  64  /* 64 * 64kByte = 4MByte Monitor Memory */
  #define MAX_BM_BUF_SIZE_IN_RESET_2MB  32  /* 32 * 64kByte = 2MByte Monitor Memory */
  #define API39_BM_BUF_SIZE_REC     16  /* 16 * 64kByte = 1MByte Monitor Memory */

/*************************************/
/***  System Control Block         ***/
/*************************************/

  #define API39_SCB_MASK            0x07FFFFFFUL /* System Control Word SW Mask          */
  #define API39_SCB_COM_BITPOS      27      /* Command field position */
  #define API39_SCB_COM_BITMASK     0xF8000000UL /* Comammand field Bit mask */


/* System Control Block Register numbers */
  #define API39_SCB_SCW_REG     0       /* System Control Word          */
  #define API39_SCB_SSW_REG     1       /* System Status Word           */
  #define API39_SCB_FID_REG     2       /* Firmware Identification Register */
  #define API39_SCB_ILP_REG     3       /* Interrupt Loglist Pointer    */
  #define API39_SCB_CAR_REG     4       /* Coupling and Output Power Register */

/* Bus Controller and Remote Terminal Registers */
  #define API39_SCB_BCBP_REG        12      /* BC Transfer Descriptor Base Pointer */

  #define API39_SCB_RT_MC_REG       19      /* RT Transfer Counter          */
  #define API39_SCB_RT_EC_REG       20      /* RT Error Counter             */
  #define API39_SCB_BC_MC_REG       21      /* BC Transfer Counter          */
  #define API39_SCB_BC_EC_REG       22      /* BC Error Counter             */

  #define API39_SCB_RT_LS_AW        28      /* RT-LS Command and Action Word */
  #define API39_SCB_RT_LS_CON       29      /* RT-LS Control                */
  #define API39_SCB_BC_LS_AW        30      /* BC-LS Command and Action Word */
  #define API39_SCB_BC_LS_CON       31      /* BC-LS Control                */

/* Monitor Registers */
  #define API39_SCB_MBBA_REG        32      /* Monitor Buffer Base Address      */
  #define API39_SCB_MBFP_REG        33      /* Monitor Buffer Fill Pointer      */
  #define API39_SCB_MSW_REG         34      /* Monitor Status and Trigger Control Word */            
  #define API39_SCB_MSTP_REG        35      /* Monitor Start Trigger Pointer    */
  #define API39_SCB_MTAT_REG        36      /* Monitor Trace after Trigger Size */
  #define API39_SCB_BM_ET_REG       37      /* Monitor Error Trigger Register   */
  #define API39_SCB_MDWT_REG        38      /* Monitor Data Word Trigger Register */
  #define API39_SCB_MCAWT_REG       39      /* Monitor Command and Action Word Trigger */
  #define API39_SCB_MCAWTM_REG      40      /* Monitor Command and Action Word Trigger Mask */
  #define API39_SCB_MFHWT_REG       41      /* Monitor Frame Header Word Trigger Mask */
  #define API39_SCB_MFHTM_REG       42      /* Monitor Frame Header Trigger Mask */
  #define API39_SCB_BM_MC_REG       43      /* Monitor Transfer Counter     */
  #define API39_SCB_BM_EC_REG       44      /* Monitor Error Counter        */
  #define API39_SCB_BM_LSAW_REG     45      /* Monitor LS Command and Action Word */
  #define API39_SCB_BM_LSC_REG      46      /* Monitor LS Control           */
  #define API39_SCB_BM_RSV_REG      47      /* Monitor Reserved             */

/*    Replay Registers */
  #define API39_SCB_RCW_REG         48      /* Replay Control Word          */
  #define API39_SCB_RREW_REG        49      /* Replay RT Enable Word        */
  #define API39_SCB_RBP_REG         51      /* Replay Buffer Pointer        */
  #define API39_SCB_RBB_REG         50      /* Replay Buffer Base Pointer   */
  #define API39_SCB_REC_REG         52      /* Replay Entry Count           */
  #define API39_SCB_RAEC_REG        53      /* Replay Actual Entry Counter  */
  #define API39_SCB_RAMTO_REG       54      /* Replay Absolute Minute Offsett Register */
  #define API39_SCB_RAMIO_REG       55      /* Replay Absolute Microsecond Offset Register */


/* System Control Block Constants */
  #define API39_SCB_COM_ENA_BIU     0x01
  #define API39_SCB_COM_ENA_BC      0x02
  #define API39_SCB_COM_ENA_RT      0x03
  #define API39_SCB_COM_ENA_BM      0x04
  #define API39_SCB_COM_ENA_REP     0x05

  #define API39_SCB_COM_DIS_BIU     0x09
  #define API39_SCB_COM_DIS_BC      0x0A
  #define API39_SCB_COM_DIS_RT      0x0B
  #define API39_SCB_COM_DIS_BM      0x0C
  #define API39_SCB_COM_DIS_REP     0x0D

  #define API39_SCB_COM_RD_CAR      0x10

  #define API39_SCB_COM_GPIO_RDD    0x11    
  #define API39_SCB_COM_GPIO_WRD    0x12    
  #define API39_SCB_COM_GPIO_RDS    0x13    
  #define API39_SCB_COM_GPIO_WRC    0x14    

  #define API39_SCB_COM_HOST_CTRL_INCR 0x1A

  #define API39_SCB_COM_ENA_BITE    0x1F


/* Frame Mode Select */
  #define API39_SCB_FMOD_STD_STANAG     0x0
  #define API39_SCB_FMOD_PRE_STANAG     0x1
  #define API39_SCB_FMOD_DDL        0x2
  #define API39_SCB_FMOD_PRE_DDL    0x3

/* Monitor Mode Select */
  #define API39_SCB_MMOD_STD_CAP    0x0
  #define API39_SCB_MMOD_SEL_CAP    0x1

  #define API39_SCB_IMOD_DIS        0x0
  #define API39_SCB_IMOD_BUF_FULL   0x1
  #define API39_SCB_IMOD_TR_START   0x2
  #define API39_SCB_IMOD_TR_STOP    0x3

  #define API39_SCB_SMOD_DIS        0x0
  #define API39_SCB_SMOD_BUF_FULL   0x1
  #define API39_SCB_SMOD_TR_START   0x2
  #define API39_SCB_SMOD_TR_STOP    0x3


/* System Status Register */
  #define API39_SSW_OPE_BIT     0x02000000L
  #define API39_SSW_SFE_BIT     0x01000000L
  #define API39_SSW_MNE_BIT     0x00800000L
  #define API39_SSW_BCE_BIT     0x00400000L
  #define API39_SSW_RTE_BIT     0x00200000L
  #define API39_SSW_RPE_BIT     0x00100000L

  #define API39_SSW_MNE_BITPOS      23

/***************************************/
/*** Remote Terminal Constants       ***/
/***************************************/

/* LS RT Subaddress/Mode Code Descriptor */
  #define API39_RT_SUBTYP_DIS_HS    0
  #define API39_RT_SUBTYP_ENA_HS    1

/*** High Speed Subaddress Definition ***/
  #define API39_HSSA_MIN        1
  #define API39_HSSA_DEF        26
  #define API39_HSSA_MAX        30

/* HS RT Descriptor */
  #define API39_RT_DISABLE      0
  #define API39_RT_ENABLE       1
  #define API39_RT_MAILBOX      2

  #define API39_RT_HS_XMT_INI_TIME_MIN  (0  * 4)                /* 0.25 usec  */
  #define API39_RT_HS_XMT_INI_TIME_DEF  (28 * 4)                /* 0.25 usec  */
  #define API39_RT_HS_XMT_INI_TIME_MAX  ((256 * 4) - 1) /* 0.25 usec  */

  #define API39_RT_HS_NO_PRE_MIN    (  0 )              /* 4 bits    */
  #define API39_RT_HS_NO_PRE_DEF    ( 40 )              /* 4 bits    */
  #define API39_RT_HS_NO_PRE_MAX    (255 )              /* 4 bits    */

/* HS RT Buffer Header */
  #define MAX_API39_RT_HID      MAX_API39_HID /* Buffer Header */


  #define API39_RT_BH_BQM_CYCLIC    0
  #define API39_RT_BH_BQM_ONCE      1
  #define API39_RT_BH_BQM_HOST      2
  #define API39_RT_BH_BQM_RSV       3

  #define API39_RT_BH_BSM_DISCARD   0
  #define API39_RT_BH_BSM_KEEP      1

  #define API39_RT_BH_STS_NO_SQ     0
  #define API39_RT_BH_STS_ONE_SQ    1
  #define API39_RT_BH_STS_QSIZE_SQ  2

  #define API39_RT_BH_EVS_NO_EQ     0
  #define API39_RT_BH_EVS_ONE_EQ    1
  #define API39_RT_BH_EVS_QSIZE_EQ  2


/* RT MID /Mode Code Descriptor */
  #define API39_RT_MID_DISABLE      0
  #define API39_RT_MID_ENABLE       1
  #define API39_RT_MID_TYPE_MC      2


  #define API39_RT_MID_IR_NONE      0
  #define API39_RT_MID_IR_ACC       1
  #define API39_RT_MID_IR_ERR       2

  #define MAX_API39_RT_RCV_MID      128  /* 3910 */
  #define MAX_API39_RT_XMT_MID      128  /* 3910 */
  #define MAX_APIEF_RT_RCV_MID      256  /* EFEX */
  #define MAX_APIEF_RT_XMT_MID      256  /* EFEX */
  #define MAX_API39_RT_MC            32
  #define MAX_API39_RT_MID          (ul_StreamIsEFEX( p_api_dev, p_api_dev->biu )?(MAX_APIEF_RT_RCV_MID + MAX_APIEF_RT_XMT_MID):(MAX_API39_RT_RCV_MID + MAX_API39_RT_XMT_MID))
  #define MAX_API39_RT_MID_INT      (ul_StreamIsEFEX( p_api_dev, p_api_dev->biu )?(MAX_APIEF_RT_RCV_MID + MAX_APIEF_RT_XMT_MID):(MAX_API39_RT_RCV_MID + MAX_API39_RT_XMT_MID))
  #define MAX_API39_RT_MID_MC       (MAX_API39_RT_MID + MAX_API39_RT_MC)
  #define MAX_API39_RT_MID_MC_INT   (MAX_API39_RT_MID_INT + MAX_API39_RT_MC)

/* 3910 Error Injection / Transfer Descriptor / Error Specification Word */
  #define API39_ERR_NONE        0       /* 00000 */
  #define API39_ERR_SDELIM_INV  1       /* 00001 */
  #define API39_ERR_SDELIM_MIS  2       /* 00010 */
  #define API39_ERR_EDELIM_INV  3       /* 00011 */
  #define API39_ERR_EDELIM_MIS  4       /* 00100 */
  #define API39_ERR_MANHI       5       /* 00101 */
  #define API39_ERR_MANLO       6       /* 00110 */
  #define API39_ERR_BCHI        7       /* 00111 */
  #define API39_ERR_BCLO        8       /* 01000 */
  #define API39_ERR_GAP         9       /* 01001 */
  #define API39_ERR_FCS         10      /* 01010 */
  #define API39_ERR_ALTBUS      11      /* 01011 */
  #define APIEF_ERR_WCNT        12      /* 01100 */
  #define APIEF_ERR_ILL_WCNT    13      /* 01101 */


/* RT Event Queue Entry */
  #define API39_RT_DYTAG_DIS        0
  #define API39_RT_DYTAG_ENA        1


/* generell Constants */
  #define MAX_API39_RT          32

  #define MAX_API39_RT_SQ       512 /* Status Queue */
  #define MAX_API39_RT_EQ       512 /* Event Queue  */
  #define MAX_API39_RT_DYTAG        32      /* Dynamic Data */


/* RT Memory area */
  #define MAX_API39_RT_DESC_SIZE     (32*4L) /* Bytes */

  #define MAX_API39_RT_MID_DESC_SIZE (2*4L)  /* Bytes */
  #define MAX_API39_RT_BH_DESC_SIZE  (4*4L)  /* Bytes */
  #define MAX_API39_RT_SQ_DESC_SIZE  (6*4L)  /* Bytes */
  #define MAX_API39_RT_EQ_DESC_SIZE  (4*4L)  /* Bytes */

  #define MAX_API39_RT_MID_AREA_SIZE    (MAX_API39_RT_MID_DESC_SIZE * MAX_API39_RT_MID_MC)  /* Bytes */
  #define MAX_API39_RT_MID_AREA_SIZE_INT (MAX_API39_RT_MID_DESC_SIZE * MAX_API39_RT_MID_MC_INT) /* Bytes */

  #define MAX_API39_RT_BH_AREA_SIZE(size)   (MAX_API39_RT_BH_DESC_SIZE * size)      /* Bytes */
  #define MAX_API39_RT_SQ_AREA_SIZE(size)   (MAX_API39_RT_SQ_DESC_SIZE * size)      /* Bytes */
  #define MAX_API39_RT_EQ_AREA_SIZE(size)   (MAX_API39_RT_EQ_DESC_SIZE * size)      /* Bytes */

  #define API39_BASE_ADDR_MASK      0x00FFFFFFL

  /* RT Mode Control */
  #define API_HS_RT_MODE_CONFIGURED_DYTAGS     3

/***************************************/
/*** Calibration     Constants       ***/
/***************************************/

/* CMD 0x8D0 */
  #define API39_CAL_SIG_DIS     0
  #define API39_CAL_SIG_ENA     1

  #define API39_CAL_BUS_A       1
  #define API39_CAL_BUS_B       2


/* CMD 0x8D3 */
  #define API39_CAL_CPL_ISO     0
  #define API39_CAL_CPL_DIR_EXT_FOFE    2
  #define API39_CAL_CPL_DIR_EXT_BITE    4
  #define API39_CAL_CPL_EXT_WRAP    5


  #define API39_CAL_CPL_MASK        0xFFFFF0FFUL
  #define API39_CAL_OUT_MASK_A      0xFFFFFF00UL
  #define API39_CAL_OUT_MASK_B      0xFF00FFFFUL

  #define API39_CAL_CALA_MASK       0xFFFFEFFFUL
  #define API39_CAL_CALB_MASK       0xEFFFFFFFUL
  #define API39_CAL_CALA_ENA        0x00001000UL
  #define API39_CAL_CALB_ENA        0x10000000UL

  #define API39_CAL_FOFE_BIT_MASK   0x1FFFFFFFUL 


/*************************************/
/*** Bus Controller Commands       ***/
/*************************************/

  #define MAX_API39_BUF_SIZE_4096   4096
  #define MAX_API39_BUF_SIZE_32     32
  #define MAX_API39_BUF_SIZE_BLOCK  128
  #define MAX_API39_BUF_SIZE        (MAX_API39_BUF_SIZE_4096/(MAX_API39_BUF_SIZE_BLOCK))

/******** AI3910_BcXferDytag  *********/
  #define API39_BC_DYTAG_DIS        0
  #define API39_BC_DYTAG_ENA        1

  #define API39_HS_DYTAG_EOL_BIT_SET    0x80000000

  #define API39_HS_DYTAG_DIRECT_MODIFY   0
  #define API39_HS_DYTAG_FUNCTION_MODIFY 1

  #define API39_HS_DYTAG_FCT_NONE   0
  #define API39_HS_DYTAG_FCT_SAW_16 1
  #define API39_HS_DYTAG_FCT_SAW_8_LOW  2
  #define API39_HS_DYTAG_FCT_SAW_8_HIGH 3

  #define MAX_API39_BC_HID      MAX_API39_HID /* Buffer Header */
  #define MAX_API39_BC_SQ       (ul_StreamIsEFEX( p_api_dev, p_api_dev->biu )?100:512)    /* Status Queue */

  #define MAX_API39_BC_EQ       512 /* Event Queue */
  #define MAX_APIEF_BC_EQ       100 /* Event Queue */

  #define MAX_API39_BC_XFER     256 /* limited to 256 due to compliance requirement of 1553 part */
/* Half of LS Transfers */
  #define MAX_API39_BC_XFER_MASK    0x3FF 

  #define MAX_API39_BC_AW       (MAX_API39_BC_XFER * 2)

/* HS BC Transfer Descriptor */
  #define API39_BC_HS_XFER      8
  #define API39_BC_XFER_TYPE_HSBCRT     API39_BC_HS_XFER
  #define API39_BC_XFER_TYPE_HSRTBC     (API39_BC_HS_XFER + 1)
  #define API39_BC_XFER_TYPE_HSBCBR     (API39_BC_HS_XFER + 2)
  #define API39_BC_XFER_TYPE_HSRTRT     (API39_BC_HS_XFER + 3)
  #define API39_BC_XFER_TYPE_HSRTBR     (API39_BC_HS_XFER + 4)

  #define API39_BC_XFER_TYPE_NONE   (API39_BC_HS_XFER + 8)

  #define API39_BC_XFER_TYPE_MASK   0x3F

/* EFEX */
  #define APIEF_BC_HS_XFER                20
  #define APIEF_BC_XFER_TYPE_XXRTRT       APIEF_BC_HS_XFER           /* EFEX to EFEX            */
  #define APIEF_BC_XFER_TYPE_X_RTBR      (APIEF_BC_HS_XFER + 1)      /* EFEX to EFEX  Broadcast */
  #define APIEF_BC_XFER_TYPE_XERTRT      (APIEF_BC_HS_XFER + 2)      /* EFEX to EFA             */
  #define APIEF_BC_XFER_TYPE_X_RTBRMIXED (APIEF_BC_HS_XFER + 3)      /* EFEX to MIX   Broadcast */
  #define APIEF_BC_XFER_TYPE_EXRTRT      (APIEF_BC_HS_XFER + 4)      /* EFA  to EFEX            */
  #define APIEF_BC_XFER_TYPE_EXRTBR      (APIEF_BC_HS_XFER + 5)      /* EFA  to EFEX  Broadcast */
  #define APIEF_BC_XFER_TYPE_EERTRT      (APIEF_BC_HS_XFER + 6)      /* EFA  to EFA             */ 
  #define APIEF_BC_XFER_TYPE_E_RTBR      (APIEF_BC_HS_XFER + 7)      /* EFA  to EFA   Broadcast */
  #define APIEF_BC_XFER_TYPE_E_RTBRMIXED (APIEF_BC_HS_XFER + 8)      /* EFA  to MIX   Broadcast */
  #define APIEF_BC_XFER_TYPE_MCRT        (APIEF_BC_HS_XFER + 9)      /* EFEX Modecode           */
  #define APIEF_BC_XFER_TYPE_MCBR        (APIEF_BC_HS_XFER + 10)     /* EFEX Modecode Broadcast */

  #define APIEF_MC_TRANS_VECTOR_WORD    0x10
  #define APIEF_MC_TRANS_STATUS_FRAME   0x02
  #define APIEF_MC_TRANS_BIT_INFO       0x13

/* HS BC Memory area */
  #define MAX_API39_BC_XFER_DESC_SIZE   (ul_StreamIsEFEX( p_api_dev, p_api_dev->biu )?16*4L:6*4L)  /* Bytes */
  #define MAX_API39_BC_XFER_DESC_SIZE_INT (ul_StreamIsEFEX( p_api_dev, p_api_dev->biu )?16*4L:6*4L)  /* Bytes */
  #define MAX_API39_BC_RESP_SIM_DESC_SIZE   (ul_StreamIsEFEX( p_api_dev, p_api_dev->biu )?8*4L:0L)  /* Bytes */
  #define MAX_API39_BC_BH_DESC_SIZE (4*4L) /* Bytes */
  #define MAX_API39_BC_SQ_DESC_SIZE (6*4L) /* Bytes */
  #define MAX_API39_BC_EQ_DESC_SIZE (4*4L) /* Bytes */
  #define MAX_API39_BC_AW_DESC_SIZE (1*4L) /* Bytes */

  #define MAX_API39_BC_BH_AREA_SIZE(size)   (MAX_API39_BC_BH_DESC_SIZE * size)  /* Bytes */
  #define MAX_API39_BC_SQ_AREA_SIZE(size)   (MAX_API39_BC_SQ_DESC_SIZE * size)  /* Bytes */
  #define MAX_API39_BC_EQ_AREA_SIZE(size)   (MAX_API39_BC_EQ_DESC_SIZE * size)  /* Bytes */

  #define MAX_API39_BC_XFER_DESC_AREA_SIZE(size) (MAX_API39_BC_XFER_DESC_SIZE * size)
  #define MAX_API39_BC_RESP_SIM_DESC_AREA_SIZE(size) (MAX_API39_BC_RESP_SIM_DESC_SIZE * size)
  #define MAX_API39_BC_AW_AREA_SIZE(size)   (MAX_API39_BC_AW_DESC_SIZE * size)

/* BC Buffer Header Descriptor */
  #define API39_BC_BH_BQM_CYCLIC    0
  #define API39_BC_BH_BQM_ONCE      1
  #define API39_BC_BH_BQM_HOST      2
  #define API39_BC_BH_BQM_AUTO      3

  #define API39_BC_BH_BSM_DISCARD   0
  #define API39_BC_BH_BSM_KEEP      1

  #define API39_BC_BH_STS_NO_SQ     0
  #define API39_BC_BH_STS_ONE_SQ    1
  #define API39_BC_BH_STS_QSIZE_SQ  2

  #define API39_BC_BH_EVS_NO_EQ     0
  #define API39_BC_BH_EVS_ONE_EQ    1
  #define API39_BC_BH_EVS_QSIZE_EQ  2


  #define API39_BC_BH_CUR_BUF_INDEX_MASK 0x00FFUL
  #define API39_BC_BH_DATA_BUF_ADDR_MASK 0x03FFFFFFUL

  #define API39_BUF_FCPA_DEF        0
  #define API39_BUF_FCPA_FIELD_SIZE 3 

  /* BC Mode Control */
  #define API_HS_BC_MODE_CONFIGURED_DYTAGS     3


/*************************************/
/*** Misc Constants                ***/
/*************************************/


  #define API39_BC_MODE         1
  #define API39_RT_MODE         2
  #define API39_SF_MODE         3
  #define API39_CAL_MODE        4
  #define API39_BM_MODE         5
  #define API39_REP_MODE        6 


/* 3910 HS-Transfer Handling / LS- Transfer Descriptor / BC Buffer Header Pointer reserved */
  #define API39_DISABLE_HS_XFER     0
  #define API39_ENABLE_HS_XFER      1

  #define API39_REP_STOP        0
  #define API39_REP_BUSY        1


/* 3910 Interrupt Control / Transfer Descriptor / Transfer Descriptor Control Word */
  #define API39_BC_HS_XMT_INI_TIME_MIN  (0 * 4)         /* 0.25 usec  steps 0us */
  #define API39_BC_HS_XMT_INI_TIME_DEF  (28 * 4)        /* 0.25 usec steps 28us */
  #define API39_BC_HS_XMT_INI_TIME_MAX  ((256 * 4) - 1) /* 0.25 usec steps 255,75 us */

  #define API39_BC_HS_NO_PRE_MIN    0                   /* 4 bits    */
  #define API39_BC_HS_NO_PRE_DEF    40                  /* 4 bits    */
  #define API39_BC_HS_NO_PRE_MAX    255                 /* 4 bits    */

  #define API39_BC_XFER_RESP_CTRL_AUTO   0
  #define API39_BC_XFER_RESP_CTRL_CW1_NO 1
  #define API39_BC_XFER_RESP_CTRL_CW2_NO 2

  #define API39_BC_XFER_RESP_CTRL_RES    3

  #define API39_BC_AW1          0
  #define API39_BC_AW2          1

  #define API39_OFF             0
  #define API39_ON          1

  #define API39_SYS_RIOT_MASK       0xFFFFFC00L


/* System Dyanmic Data Words */
  #define MAX_API39_SYS_HS_SYSTAGS  255 /* not 256 because parameter of systag_con is only byte */

  #define API39_SYSTAG_DIS      0
  #define API39_SYSTAG_ENA      1

  #define API39_SYSTAG_RUN_SUSPEND       0
  #define API39_SYSTAG_RUN_RESUME        1
  #define API39_SYSTAG_RUN_DISABLED      2
  #define API39_SYSTAG_RUN_ENABLED       3
  #define API39_SYSTAG_RECALC_CHECKSUM   4

  #define API39_HS_SYSTAG_RESET_ALL      0
  #define API39_HS_SYSTAG_RESET_BC_ALL   1
  #define API39_HS_SYSTAG_RESET_RT_ALL   2
  #define API39_HS_SYSTAG_RESET_RT_ONLY  3
  #define API39_HS_SYSTAG_RESET_SA_ONLY  4
  #define API39_HS_SYSTAG_RESET_XID_ONLY 5

  #define API39_SYSTAG_NONE         0
  #define API39_SYSTAG_RAMP_INC     1
  #define API39_SYSTAG_RAMP_DEC     2
  #define API39_SYSTAG_TRIANG_INC   3
  #define API39_SYSTAG_TRIANG_DEC   4
  #define API39_SYSTAG_DATASET      5
  #define API39_SYSTAG_STATES       6
  #define API39_SYSTAG_COMP         7
  #define API39_SYSTAG_CHECKSUM     8


/*****************************************/
/*****      3910 Data Memory Map    ******/
/*****************************************/

  #define API39_CB_ADDR         (0x10000 + p_api_dev->hs_stream*API_GRAM2_OFFSET)
  #define API39_IR_LOG_ADDR     (API39_CB_ADDR + 0x400)
  #define API39_RT_DESC_ADDR    (API39_CB_ADDR + 0x800)

  #define API39_BM_ACT_ADDR     (ul_StreamIsEFEX( p_api_dev, p_api_dev->biu )?(API39_CB_ADDR + 0x5000):(API39_CB_ADDR + 0x8000))

  #define API39_LS_RT_SA_DESC_ADDR  (ul_StreamIsEFEX( p_api_dev, p_api_dev->biu )?0x48000L:0x30000L)

  #define API39_SIM_BUF_AREA_BASE_OFFSET 0x4  /* Offset for unused memory info */

  #define API39_NU_ADDR         0


  #define API39_RX_TOUT_DEF     (185  * 4)   /* 0.25 usec steps  */
  #define API39_RX_TOUT_MIN     0            /* 10 bits */
  #define API39_RX_TOUT_MAX     1023         /* 10 bits */

  #define VME_RAM_START_ADDR        API_SHARED_MEM_START_ADDR
  #define API_TARG_MEM_REG_ADDR     (API_SHARED_MEM_START_ADDR + 0x00001ee0L) /* Target Control Registers */

  #define MAX_API39_WDATA_SIZE      4132


/**************************************************************/
/***                Shared RAM HS Recording Area            ***/
/**************************************************************/
  #define API39_HS_REC_SHARED_MEM_START_ADDR    (API_SHARED_MEM_START_ADDR + 0x00BD0000L)

/**************************************************************/
/***                Shared RAM HS Replay Area               ***/
/**************************************************************/
  #define API39_HS_REP_SHARED_MEM_START_ADDR    (API_SHARED_MEM_START_ADDR + 0x00BD0000L)

/**************************************************************/
/***                Bus Monitor Constants                   ***/
/**************************************************************/

/* SW - api39_bm_hs_stack_find_entry  */
  #define API39_BM_ENTRY_BW     0
  #define API39_BM_ENTRY_ERR    1
  #define API39_BM_ENTRY_LSC    2
  #define API39_BM_LSCW_LSAW    3
  #define API39_BM_FCPA_DA      4

  #define API39_BM_LSTAG_SW_CORREL  20      /* usec */
  #define APIEF_BM_LSTAG_SW_CORREL  118     /* usec */
  #define API39_BM_LSTAG_XIT_CORREL 128 /* usec */

/* SW - api39_bm_ini_cap */
  #define API39_BM_CMOD_ALL     API_BM_CMOD_ALL
  #define API39_BM_CMOD_ONLY    API_BM_CMOD_ONLY

/* SW - api39_bm_hs_stack_read */
  #define API39_BM_STACK_OFF_P      0
  #define API39_BM_STACK_OFF_N      1

  #define API39_BM_STATUS_CTRL_DEF  0x00155000L     /* logical ored */
  #define API39_BM_BASE_ADDR_MASK   0x00FFFFFFL
  #define API39_BM_LTRG_MASK        0x00300000L

/****************************************************************/
/* Firmware 4.3.x System Control Block Monitor definition       */
/****************************************************************/

  #define API39_BM_TAT_DEF      10000L
  #define API39_BM_DATA_WORD_TRG_REG    0 /* 0x0000FFFFL */
  #define API39_BM_CW_AW_TRG_REG    0 /* 0xFFFFFFFFL */
  #define API39_BM_CW_AW_TRG_MASK_REG   0x00000000L
  #define API39_BM_FCPA_DA_TRG_REG  0 /* 0xFFFFFFFFL */
  #define API39_BM_FCPA_DA_TRG_MASK_REG 0x00000000L


/****************************************************************/
/* Firmware 4.3.3 Monitor Status and Control Register           */
/****************************************************************/
  #define API39_BM_MSW_BFF_SHIFT    24


/****************************************************************/
/* Firmware 4.1.1 System Control Register Monitor Mode Select   */
/****************************************************************/

  #define API39_SCW_MMOD_MASK       0x00400000L     /* MMOD mask */
  #define API39_SCW_MMOD_MASK_SHIFT 22


/****************************************************************/
/* Firmware 8.2.x BM Monitor Activity and Msg Filter Descriptor */
/****************************************************************/

  #define MAX_API39_BM_RT_ACT_SIZE  (MAX_API_RT * MAX_API39_RT_MID) /* Monitor Activity List Entries */

  #define API39_BM_ERR_TYPE_MASK    0x0000FFFFUL
  #define API39_BM_ACT_MSG_FLT_MCE  0x20000000UL
  #define API39_BM_ACT_MSG_FLT_TYP_MCE_MASK 0xF8000000UL

/****************************************************/
/* Firmware 8.4 BM Monitor Buffer Entry Definitions */
/****************************************************/

  #define API39_BM_TXINI_BUFTAGWORD_OFFSET 2
  #define API39_BM_HS_TTAG_HIGH_OFFSET     3
  #define API39_BM_HS_TTAG_LOW_OFFSET      4
  #define API39_BM_LS_CW_AW_OFFSET         5
  #define API39_BM_LS_CONW_OFFSET          6
  #define API39_BM_LS_CW_AW2_OFFSET        7
  #define API39_BM_LS_CONW2_OFFSET         8

  #define API39_BM_FCPA_DA_OFFSET          14
  #define API39_BM_WC_ENTRYSIZE_OFFSET     15
  #define API39_BM_DATA_OFFSET             16

  #define API39_BM_MSG_HEADER_LSIZE        15
  #define API39_BM_DATA_OFFSET_ADDR      0x40

  #define API39_BM_LS_TTAG_HIGH_MASK    0x000FFFFFL
  #define API39_BM_LS_TTAG_LOW_MASK     0x03FFFFFFL
  #define API39_BM_HS_TTAG_HIGH_MASK    0x000FFFFFL
  #define API39_BM_HS_TTAG_LOW_MASK     0x03FFFFFFL

  #define APIEF_BM_BUFTAGWORD_OFFSET       2
  #define APIEF_BM_CCMC_FCPA_DA_OFFSET     5
  #define APIEF_BM_LS_CW_AW_OFFSET         11
  #define APIEF_BM_LS_CW_AW2_OFFSET        12
  #define APIEF_BM_WC_ENTRYSIZE_OFFSET     15
/*** Mask ***/
  #define API39_BM_ENTRY_CNT_MASK   0x0000FFFFL

/* Maximum Monitor Size */
  #define MAX_API39_BM_BUF_SIZE     4096
  #define MAX_API39_BM_FRAME_SIZE   (9 + (MAX_API39_BM_BUF_SIZE + 1)/2 + 32/2)


  #define API39_RT_MID_TYPE_RCV     0
  #define API39_RT_MID_TYPE_XMT     1
  #define API39_RT_MID_TYPE_ALL     4
  #define APIEF_RT_MID_TYPE_MC      5

/* BM Dyntag Monitor */
  #define MAX_API39_BM_HS_DYNTAG_MON_ID  64  /* 11.11.05 */
  #define API39_BM_HS_DYNTAG_MON_DIS      0  /* 11.11.05 */
  #define API39_BM_HS_DYNTAG_MON_ENA      1  /* 11.11.05 */

  #define SW_END(dw)  (((dw & 0x0000FFFF) << 16) | ((dw & 0xFFFF0000) >> 16))

/*************************************/
/*** GPIO Constants                ***/
/*************************************/

  #define API39_GPIO_MAX_DEVICES    8

/* some status specificaitons */
  #define API39_GPIO_STATUS_DRDY    0x80    /* 100Rxxxxb  Device ready        */
  #define API39_GPIO_STATUS_FAIL    0x40    /* 010Rxxxxb  Selftest failed     */
  #define API39_GPIO_STATUS_ICMD    0x20    /* 001Rxxxxb  Command in progress */

  #define API39_GPIO_STATUS_DEVTYP_MASK 0x0F    /* 00001111b  Device type Mask */

/* some device type specificaitons */
  #define API39_GPIO_DEVTYPE_RESERVED 0x00    
  #define API39_GPIO_DEVTYPE_ERROR    0xF0    
  #define API39_GPIO_DT_VOX3910CTX  (API39_GPIO_DEVTYPE_RESERVED + 1)

/* some informations about the FOFES */
  #define API39_GPIO_FOFE_ENHANCED_AIM  0   
  #define API39_GPIO_FOFE_GENERIC       6   

/* some command specificaitons */
  #define API39_GPIO_CMD_RESET      0x01    /* 00000001b */
  #define API39_GPIO_CMD_BITE       0x02    /* 00000010b */
  #define API39_GPIO_CMD_ALL_ON     0x03    /* 00000011b */
  #define API39_GPIO_CMD_ALL_OFF    0x04    /* 00000100b */
  #define API39_GPIO_CMD_SYS1_START 0x05    /* 00000101b */
  #define API39_GPIO_CMD_SYS1_END   0x07    /* 00000111b */
  #define API39_GPIO_CMD_SDEV       0x08    /* 00001dddb */
  #define API39_GPIO_CMD_SYS2_START 0x10    /* 00010000b */
  #define API39_GPIO_CMD_SYS2_END   0x3F    /* 00111111b */
  #define API39_GPIO_CMD_SDREG      0x40    /* 01aaaaaab */
  #define API39_GPIO_CMD_DEV_START  0x80    /* 10000000b */
  #define API39_GPIO_CMD_DEV_END    0xFF    /* 11111111b */

  #define API39_GPIO_CMD_DEV_END    0xFF    /* 11111111b */


  #define API39_GPIO_CMD_SDEV_MASK  0x07    /* 00000111b */
  #define API39_GPIO_CMD_SDREG_MASK 0x3F    /* 00111111b */

#endif /* __I39_DEFS_H__ */

/* end: i39_defs.h */
