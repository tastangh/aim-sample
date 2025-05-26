/* SPDX-FileCopyrightText: 2015-2022 AIM GmbH <info@aim-online.com> */
/* SPDX-License-Identifier: MIT OR GPL-2.0-or-later */

#ifndef UNIVERSE_H			
  #define UNIVERSE_H

typedef struct BoardStat
{
  UINT32 BoRegBasAddr;
  UINT8 SlaveImage[ 9 ]; 
  UINT32 userOffset[ 9 ]; 
} BOARD_STATUS;

/*Global definations*/
  #ifndef FALSE
    #define FALSE 	0
  #endif 

  #ifndef TRUE
    #define TRUE  	1
  #endif 

// #define OK  	0xff		// confilct with Tornado --> is never use


/*	#define BSWAP16	adjust16
#define BSWAP32	adjust32*/


/************************************/
/* defines System boundary			*/
/************************************/
  #define MAX_BOARDNO 		32	/* maximum number of boards */


/************************************/
/* defines for Error Level			*/
/************************************/

  #define WrnImgNo			0xFE
  #define ImgInUse			0xFD
  #define WrnSize			0xFC
  #define WrnInput			0xFB
  #define WrnBoNo			0xFA
  #define WrnRegBasAddr			0xF9
  #define IntEnErr			0xF8
  #define IntDisErr			0xF7
  #define StatIdErr			0xF6
  #define StatIdOutErr 			0xF5
  #define WrnPciInt			0xF4	
  #define IntlevErr			0xF3
  #define IntGenErr			0xF2
  #define WrnIntVec			0xF1

/************************************
defines VME Interrupt Handling
 ************************************/

  #define VME_SW_INT_GEN	0x00001000
  #define UNI_MASK_SW_IACK	0x00070000
  #define UNI_SHIFT_SW_IACK	16	/* << to get SW_IACK in VINT_MAP1 */



/************************************
PCI Interrupt Mask
 ************************************/
  #define LINT0  		0x00000001
  #define LINT1  		0x00000002
  #define LINT2  		0x00000004
  #define LINT3  		0x00000008
  #define LINT4  		0x00000010
  #define LINT5  		0x00000020
  #define LINT6  		0x00000040
  #define LINT7  		0x00000080

  #define DMA			0x00000100
  #define LERR			0x00000200
  #define VERR			0x00000400

  #define SW_INT 		0x00001000

  #define M_MBOX0		0x00010000
  #define M_MBOX1		0x00020000
  #define M_MBOX2		0x00040000
  #define M_MBOX3	 	0x00080000

  #define UNIV_SW_INT1	0x02000000			// Name Conflict with API Driver software
  #define UNIV_SW_INT2	0x04000000
  #define UNIV_SW_INT3	0x08000000
  #define UNIV_SW_INT4	0x10000000
  #define UNIV_SW_INT5	0x20000000
  #define UNIV_SW_INT6	0x40000000
  #define UNIV_SW_INT7	0x80000000

/************************************
VME Interrupt
 ************************************/
  #define VIRQ0			0x00
  #define VIRQ1			0x01
  #define VIRQ2			0x02
  #define VIRQ3			0x03
  #define VIRQ4			0x04
  #define VIRQ5			0x05
  #define VIRQ6			0x06
  #define VIRQ7			0x07



/************************************
Universe2 Register Map Offsets
 ************************************/
  #define PCI_ID		0x0000	/* PCI Configuration Space ID Register 	*/
  #define PCI_CSR		0x0004	/* PCI Config Space Control and Status Reg*/

  #define PCI_CLASS		0x0008	/* PCI Config Class Register 					*/
  #define PCI_MISC0		0x000C	/* PCI Config Miscellaneous 0 Register 	*/
  #define PCI_BS0		0x0010	/* PCI Config Base Address Register 		*/
  #define PCI_BS1		0x0014	/* PCI Config Address Space Register    	*/
  #define PCI_MISC1		0x003C	/* PCI Config Miscellaneous 1 Register		*/

  #define LSI0_CTL		0x0100	/* PCI Slave Image 0 Control					*/
  #define LSI0_BS		0x0104	/* PCI Slave Image 0 Base Address Reg		*/
  #define LSI0_BD		0x0108	/* PCI Slave Image 0 Bound Address Reg		*/
  #define LSI0_TO		0x010C	/* PCI Slave Image 0 Translation Offset	*/

  #define LSI1_CTL		0x0114	/* PCI Slave Image 1 Control					*/
  #define LSI1_BS		0x0118	/* PCI Slave Image 1 Base Address Reg		*/
  #define LSI1_BD		0x011C	/* PCI Slave Image 1 Bound Address Reg		*/
  #define LSI1_TO		0x0120	/* PCI Slave Image 1 Translation Offset	*/

  #define LSI2_CTL		0x0128	/* PCI Slave Image 2 Control					*/
  #define LSI2_BS		0x012C	/* PCI Slave Image 2 Base Address Reg		*/
  #define LSI2_BD		0x0130	/* PCI Slave Image 2 Bound Address Reg		*/
  #define LSI2_TO		0x0134	/* PCI Slave Image 2 Translation Offset	*/

  #define LSI3_CTL		0x013C	/* PCI Slave Image 3 Control					*/
  #define LSI3_BS		0x0140	/* PCI Slave Image 3 Base Address Reg		*/
  #define LSI3_BD		0x0144	/* PCI Slave Image 3 Bound Address Reg		*/
  #define LSI3_TO		0x0148	/* PCI Slave Image 3 Translation Offset	*/

  #define SCYC_CTL		0x0170	/* Special Cycle Control Register			*/
  #define SCYC_ADDR		0x0174	/* Special Cycle PCI bus Address Register	*/
  #define SCYC_EN		0x0178	/* Special Cycle Swap/Compare Enable Reg	*/
  #define SCYC_CMP		0x017C	/* Special Cycle Compare Data Register		*/
  #define SCYC_SWP		0x0180	/* Special Cycle Swap Data Register			*/

  #define LMISC			0x0184	/* PCI Miscellaneous Register					*/
  #define SLSI			0x0188	/* Special PCI Slave Image						*/
  #define L_CMDERR		0x018C	/* PCI Command Error Log Register			*/
  #define LAERR			0x0190	/* PCI Address Error Log						*/

  #define LSI4_CTL		0x01A0	/* PCI Slave Image 4 Control					*/
  #define LSI4_BS		0x01A4	/* PCI Slave Image 4 Base Address Reg		*/
  #define LSI4_BD		0x01A8	/* PCI Slave Image 4 Bound Address Reg		*/
  #define LSI4_TO		0x01AC	/* PCI Slave Image 4 Translation Offset	*/

  #define LSI5_CTL		0x01B4	/* PCI Slave Image 5 Control					*/
  #define LSI5_BS		0x01B8	/* PCI Slave Image 5 Base Address Reg		*/
  #define LSI5_BD		0x01BC	/* PCI Slave Image 5 Bound Address Reg		*/
  #define LSI5_TO		0x01C0	/* PCI Slave Image 5 Translation Offset	*/

  #define LSI6_CTL		0x01C8	/* PCI Slave Image 6 Control					*/
  #define LSI6_BS		0x01CC	/* PCI Slave Image 6 Base Address Reg		*/
  #define LSI6_BD		0x01D0	/* PCI Slave Image 6 Bound Address Reg		*/
  #define LSI6_TO		0x01D4	/* PCI Slave Image 6 Translation Offset	*/

  #define LSI7_CTL		0x01DC	/* PCI Slave Image 7 Control					*/
  #define LSI7_BS		0x01E0	/* PCI Slave Image 7 Base Address Reg		*/
  #define LSI7_BD		0x01E4	/* PCI Slave Image 7 Bound Address Reg		*/
  #define LSI7_TO		0x01E8	/* PCI Slave Image 7 Translation Offset	*/

  #define DCTL			0x0200	/* DMA Transfer Control Register				*/
  #define DTBC			0x0204	/* DMA Transfer Byte Count Register			*/
  #define DLA			0x0208	/* DMA PCI bus Address Register				*/
  #define DVA			0x0210	/* DMA VMEbus Address Register				*/
  #define DCPP			0x0218	/* DMA Command Packet Pointer					*/
  #define DGCS			0x0220	/* DMA General Control and Status Reg		*/
  #define D_LLUE		0x0224	/* DMA Linked List Update Enable Register	*/

  #define LINT_EN		0x0300	/* PCI Interrupt Enable						*/
  #define LINT_STAT		0x0304	/* PCI Interrupt Status						*/
  #define LINT_MAP0		0x0308	/* PCI Interrupt Map 0						*/
  #define LINT_MAP1		0x030C	/* PCI Interrupt Map 1						*/

  #define VINT_EN		0x0310	/* VMEbus Interrupt Enable					*/
  #define VINT_STAT		0x0314	/* VMEbus Interrupt Status					*/
  #define VINT_MAP0		0x0318	/* VMEbus Interrupt Map 0					*/
  #define VINT_MAP1		0x031C	/* VMEbus Interrupt Map 1					*/

  #define STATID		0x0320	/* Interrupt Status/ID Out					*/

  #define V1_STATID		0x0324	/* VIRQ1 Status/ID						*/
  #define V2_STATID		0x0328	/* VIRQ2 Status/ID						*/
  #define V3_STATID		0x032C	/* VIRQ3 Status/ID						*/
  #define V4_STATID		0x0330	/* VIRQ4 Status/ID						*/
  #define V5_STATID		0x0334	/* VIRQ5 Status/ID						*/
  #define V6_STATID		0x0338	/* VIRQ6 Status/ID						*/
  #define V7_STATID		0x033C	/* VIRQ7 Status/ID						*/

  #define LINT_MAP2		0x0340	/* PCI Interrupt Map 2						*/
  #define VINT_MAP2		0x0344	/* VME Interrupt Map 1						*/

  #define MBOX0			0x0348	/* Mailbox 0 Register						*/
  #define MBOX1			0x034C	/* Mailbox 1 Register						*/
  #define MBOX2			0x0350	/* Mailbox 2 Register						*/
  #define MBOX3			0x0354	/* Mailbox 3 Register						*/

  #define SEMA0			0x0358	/*Semaphore Register 0						*/
  #define SEMA1			0x035C	/*Semaphore Register 1						*/

  #define MAST_CTL		0x0400	/* Master Control									*/
  #define MISC_CTL		0x0404	/* Miscellaneous Control 						*/
  #define MISC_STAT		0x0408	/* Miscellaneous Status							*/
  #define USER_AM		0x040C	/* User AM codes Register						*/

  #define VSI0_CTL		0x0F00	/* VMEbus Slave Image 0 Control				*/
  #define VSI0_BS		0x0F04	/* VMEbus Slave Image 0 Base Address Reg	*/
  #define VSI0_BD		0x0F08	/* VMEbus Slave Image 0 Bound Address Reg	*/
  #define VSI0_TO		0x0F0C	/* VMEbus Slave Image 0 Translation Offset*/

  #define VSI1_CTL		0x0F14	/* VMEbus Slave Image 1 Control				*/
  #define VSI1_BS		0x0F18	/* VMEbus Slave Image 1 Base Address Reg	*/
  #define VSI1_BD		0x0F1C	/* VMEbus Slave Image 1 Bound Address Reg	*/
  #define VSI1_TO		0x0F20	/* VMEbus Slave Image 1 Translation Offset*/

  #define VSI2_CTL		0x0F28	/* VMEbus Slave Image 2 Control				*/
  #define VSI2_BS		0x0F2C	/* VMEbus Slave Image 2 Base Address Reg	*/
  #define VSI2_BD		0x0F30	/* VMEbus Slave Image 2 Bound Address Reg	*/
  #define VSI2_TO		0x0F34	/* VMEbus Slave Image 2 Translation Offset*/

  #define VSI3_CTL		0x0F3C	/* VMEbus Slave Image 3 Control				*/
  #define VSI3_BS		0x0F40	/* VMEbus Slave Image 3 Base Address Reg	*/
  #define VSI3_BD		0x0F44	/* VMEbus Slave Image 3 Bound Address Reg	*/
  #define VSI3_TO		0x0F48	/* VMEbus Slave Image 3 Translation Offset*/

  #define LM_CTL		0X0F64	/* Location Monitor Control Register		*/
  #define LM_BS			0X0F68	/* Location Monitor Base adress Register	*/

  #define VRAI_CTL		0x0F70	/* VMEbus Reg Access Image Control Reg		*/
  #define VRAI_BS		0x0F74	/* VMEbus Reg Access Image Base Address	*/

  #define VCSR_CTL		0x0F80	/* VMEbus CSR Control Register				*/
  #define VCSR_TO		0x0F84	/* VMEbus CSR Translation Offset				*/
  #define V_AMERR		0x0F88	/* VMEbus AM Code Error Log					*/
  #define VAERR			0x0F8C	/* VMEbus Address Error Log					*/

  #define VSI4_CTL		0x0F90	/* VMEbus Slave Image 4 Control				*/
  #define VSI4_BS		0x0F94	/* VMEbus Slave Image 4 Base Address Reg	*/
  #define VSI4_BD		0x0F98	/* VMEbus Slave Image 4 Bound Address Reg	*/
  #define VSI4_TO		0x0F9C	/* VMEbus Slave Image 4 Translation Offset*/

  #define VSI5_CTL		0x0FA4	/* VMEbus Slave Image 5 Control				*/
  #define VSI5_BS		0x0FA8	/* VMEbus Slave Image 5 Base Address Reg	*/
  #define VSI5_BD		0x0FAC	/* VMEbus Slave Image 5 Bound Address Reg	*/
  #define VSI5_TO		0x0FB0	/* VMEbus Slave Image 5 Translation Offset*/

  #define VSI6_CTL		0x0FB8	/* VMEbus Slave Image 6 Control				*/
  #define VSI6_BS		0x0FBC	/* VMEbus Slave Image 6 Base Address Reg	*/
  #define VSI6_BD		0x0FC0	/* VMEbus Slave Image 6 Bound Address Reg	*/
  #define VSI6_TO		0x0FC4	/* VMEbus Slave Image 6 Translation Offset*/

  #define VSI7_CTL		0x0FCC	/* VMEbus Slave Image 7 Control				*/
  #define VSI7_BS		0x0FC0	/* VMEbus Slave Image 7 Base Address Reg	*/
  #define VSI7_BD		0x0FC4	/* VMEbus Slave Image 7 Bound Address Reg	*/
  #define VSI7_TO		0x0FC8	/* VMEbus Slave Image 7 Translation Offset*/


  #define VCSR_CLR		0x0FF4	/* VMEbus CSR Bit Clear Register				*/
  #define VCSR_SET		0x0FF8	/* VMEbus CSR Bit Set Register				*/
  #define VCSR_BS		0x0FFC	/* VMEbus CSR Base Address Register			*/



/************************************************************************
Type Definitions for some Register Sets or Registers as Bit Fields.  

 ************************************************************************/

  #if !defined(_AIM_VME)

/* Definition of the VCSR Register as Bitfields */
typedef union
{
  UINT32 all;
  struct
  {
    UINT32 reserved: 29;
    UINT32 fail: 1;
    UINT32 sysfail: 1;
    UINT32 reset: 1;
  } bit;
}

TY_UNIVERS_VCSR;

/* Definition of a VCSR Control Structure of the UNIVERSE II chip for one memory image */
typedef struct
{
  TY_UNIVERS_VCSR vcsrClr;
  TY_UNIVERS_VCSR vcsrSet;
  UINT32 vcsrBs;
} TY_UNIVERS_VCSR_REGS;


/* Definition of a Control Structure of the UNIVERSE II chip for one memory image */
typedef struct
{
  UINT32 control;
  UINT32 base;
  UINT32 bound;
  UINT32 offest;
} TY_UNIVERS_IMAGE_CONF;


/* Definition of a Control Structure of the UNIVERSE II chip for internal Register Image */
typedef struct
{
  UINT32 control;
  UINT32 base;
} TY_UNIVERS_REG_IMAGE_CONF;

typedef struct
{
  UINT32 enable;
  UINT32 status;
  UINT32 map0;
  UINT32 map1;
  UINT32 statId; // Vector
} TY_UNIVERS_REG_VME_INT;


/* Definition of a Controlword of the UNIVERSE II chip for one PCI memory image */
typedef union
{
  UINT32 all;
  struct
  {
    UINT32 las: 2;
    UINT32 res1_7: 4;
    UINT32 vct: 1;
    UINT32 res9_11: 3;
    UINT32 super: 1;
    UINT32 res13: 1;
    UINT32 pgm: 1;
    UINT32 res15: 1;
    UINT32 vas: 1;
    UINT32 res19_21: 3;
    UINT32 vdw: 2;
    UINT32 res24_29: 6;
    UINT32 pwen: 1;
    UINT32 en: 1;
  } bit;
}

PCI_TRG_CONT_TY;


/* Definition of a Controlword of the UNIVERSE II chip for one VME memory image */
    #define UNIV_CTL_PGM_RES		0 
    #define UNIV_CTL_PGM_DATA		1 
    #define UNIV_CTL_PGM_PROG		2 
    #define UNIV_CTL_PGM_BOTH		3 

    #define UNIV_CTL_SUPER_RES		0	
    #define UNIV_CTL_SUPER_NOPRI	1	
    #define UNIV_CTL_SUPER_SUPER	2
    #define UNIV_CTL_SUPER_BOTH		3

    #define UNIV_CTL_VAS_A16		0
    #define UNIV_CTL_VAS_A24		1
    #define UNIV_CTL_VAS_A32		2
    #define UNIV_CTL_VAS_RES1		3
    #define UNIV_CTL_VAS_RES2		4
    #define UNIV_CTL_VAS_RES3		5
    #define UNIV_CTL_VAS_USER1		6
    #define UNIV_CTL_VAS_USER2		7

    #define UNIV_CTL_LAS_PCIMEM		0
    #define UNIV_CTL_LAS_PCIIO		1
    #define UNIV_CTL_LAS_PCICONF	2
    #define UNIV_CTL_LAS_RES		3

typedef union
{
  UINT32 all;
  struct
  {
    UINT32 las: 2;
    UINT32 res5_2: 4;
    UINT32 llrmw: 1;
    UINT32 ld64en: 1;
    UINT32 res15_8: 8;
    UINT32 vas: 3;
    UINT32 res19: 1;
    UINT32 super: 2;
    UINT32 pgm: 2;
    UINT32 res28_24: 5;
    UINT32 pren: 1;
    UINT32 pwen: 1;
    UINT32 en: 1;
  } bit;
}

VME_SLAVE_CONT_TY;

/* Definition of a Controlword of the UNIVERSE II chip for one VME internal Register image */
typedef union
{
  UINT32 all;
  struct
  {
    UINT32 res15_0: 16;
    UINT32 vas: 2;
    UINT32 res19_18: 2;
    UINT32 super: 2;
    UINT32 pgm: 2;
    UINT32 res30_24: 7;
    UINT32 en: 1;
  } bit;
}

VME_IREG_CONT_TY;


  #else 
/* Here can be defined the same structures as above for Big Endian */

  #endif 


/**************************************
Base Function Prototypes for VME-Bus
handling.
 **************************************/
  #if defined(_AIM_VME)			
/* Prototypes only neaded for VME environment*/

UINT8 UnivWriteRegister      (UINT32 RegBaseAddr, UINT16 Register, UINT32 regValue);
UINT32 UnivReadRegister      (UINT32 RegBaseAddr, UINT16 Register);
void initVmeGlobals          (void);

UINT8 VmeInitImage           (UINT32 ImageBaseAddress, UINT32 ImageSize, UINT32 Offset, UINT16 ImageNo, UINT32 CrtlMask, UINT32 RegBaseAddr, UINT32 accessBaseAddres); // $%01
UINT8 VmeCloseImage          (UINT16 ImageNo);

UINT8 ReadSlvUINT8           (UINT8 *src_p);
UINT16 ReadSlvUINT16         (UINT16 *src_p);
UINT32 ReadSlvUINT32         (UINT32 *src_p);

UINT8 WriteSlvUINT8          (UINT8 *dst_p, UINT8 value);
UINT8 WriteSlvUINT16         (UINT16 *dst_p, UINT16 value);
UINT8 WriteSlvUINT32         (UINT32 *dst_p, UINT32 value);

UINT8 BlockSlvUINT8          (UINT8 *src_p, UINT8 *dst_p, UINT32 size);
UINT8 BlockSlvUINT16         (UINT16 *src_p, UINT16 *dst_p, UINT32 size);
UINT8 BlockSlvUINT32         (UINT32 *src_p, UINT32 *dst_p, UINT32 size);
UINT8 BlockSlvEnhanced       (UINT8 *src_p, UINT8 *dst_p, UINT32 size);


UINT8 ReadSlvImgUINT8        (UINT16 ImageNo, UINT32 ImageOffset);
UINT8 ReadBlockSlvImgUINT8   (UINT16 ImageNo, UINT32 ImageOffset, UINT32 size, UINT8 *Value_Addr);
UINT16 ReadSlvImgUINT16      (UINT16 ImageNo, UINT32 ImageOffset);
UINT8 ReadBlockSlvImgUINT16  (UINT16 ImageNo, UINT32 ImageOffset, UINT32 size, UINT16 *Value_Addr);
UINT32 ReadSlvImgUINT32      (UINT16 ImageNo, UINT32 ImageOffset);
UINT8 ReadBlockSlvImgUINT32  (UINT16 ImageNo, UINT32 ImageOffset, UINT32 size, UINT32 *Value_Addr);

UINT8 WriteSlvImgUINT8       (UINT16 ImageNo, UINT32 ImageOffset, UINT8 value);
UINT8 WriteBlockSlvImgUINT8  (UINT16 ImageNo, UINT32 ImageOffset, UINT32 size, UINT8 *value);
UINT8 WriteSlvImgUINT16      (UINT16 ImageNo, UINT32 ImageOffset, UINT16 value);
UINT8 WriteBlockSlvImgUINT16 (UINT16 ImageNo, UINT32 ImageOffset, UINT32 size, UINT16 *value);
UINT8 WriteSlvImgUINT32      (UINT16 ImageNo, UINT32 ImageOffset, UINT32 value);
UINT8 WriteBlockSlvImgUINT32 (UINT16 ImageNo, UINT32 ImageOffset, UINT32 size, UINT32 *value);


/*
Das Inerrupt handling ist noch nicht fertig implementiert so dass sich die prototypen noch
aendern koennen */

UINT8 EnPCIInt               (UINT16 ImageNo, UINT32 LInt);
UINT8 DisPCIInt              (UINT16 ImageNo, UINT32 LInt);
UINT8 PutValStatusId         (UINT16 ImageNo, UINT8 StatusId);
UINT8 GenVmeStwInt           (UINT16 ImageNo, UINT32 level, UINT32 vector);
UINT16 GetVmeIntVec          (UINT16 ImageNo);
UINT8 MapPCIInt2VmeInt       (UINT16 ImageNo, UINT32 PciInt, UINT8 VmeInt, UINT8 status);
  #endif /* prototypes */

#endif 



/*	End of univers.h */
