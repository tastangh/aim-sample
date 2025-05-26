/* SPDX-FileCopyrightText: 2016-2024 AIM GmbH <info@aim-online.com> */
/* SPDX-License-Identifier: MIT OR GPL-2.0-or-later */

/*! \file AiHwAye.h
    Hardware description file for AYE platform */

#ifndef __AIHWAYE_H__
#define __AIHWAYE_H__

#include "Ai_cdef.h"

/* Registers dword offset */

#define AEC_REG_TT_HIGH                 0x0400     /* Read       */
#define AEC_REG_TT_LOW                  0x0401     /* Read       */
#define AEC_REG_TT_EXTENSION            0x0402     /* Read       */
#define AEC_REG_TT_STATUS               0x0403     /* Read       */
#define AEC_REG_IRIG_CORRECTION         0x0404     /* Read/Write */
#define AEC_REG_IRIG_MISC               0x0405     /* Read/Write */
#define AEC_REG_IRIG_SHIFT              0x0406     /* Read/Write */
#define AEC_REG_EXTERNAL_IRIG_RESET     0x0407     /* Read/Write */
#define AEC_REG_EXTERNAL_IRIG_PULSE     0x0408     /* Read/Write */
#define AYE_REG_TRIGGER_EDGE_CONTROL    0x0409     /* Read/Write */
#define AYE_REG_TRIGGER_DIGITAL_LOOP    0x040A     /* Read/Write */
#define AEC_REG_COUNTER                 0x040C     /* Read       */
#define AEC_REG_IRQ_HOST_GEN            0x0410     /* Write      */
#define AEC_REG_IRQ_HOST_EVENT          0x0412     /* Read/Write */
#define AEC_REG_IRQ_HOST_MASK           0x0413     /* Read/Write */
#define AEC_REG_IRQ_BIU_GEN             0x0416     /* Write      */
#define AEC_REG_IRQ_ASP_GEN             0x0417     /* Write      */
#define AEC_REG_GPIO_INPUT              0x041C     /* Read       */
#define AEC_REG_GPIO_OUTPUT             0x041D     /* Read/Write */
#define AEC_REG_GPIO_CONFIG             0x041E     /* Read/Write */
#define AEC_REG_DMA_FIFO_START          0x041F     /*      Write */
#define AEC_REG_DMA_FIFO_STATUS         0x0420     /* Read       */
#define AEC_REG_MIS_OUT                 0x0421     /* Read/Write */
#define AEC_REG_PCIE_HEADER_1           0x0424     /* Read       */
#define AEC_REG_PCIE_HEADER_2           0x0425     /* Read       */
#define AEC_REG_PCIE_HEADER_3           0x0426     /* Read       */
#define AEC_REG_PCIE_HEADER_4           0x0427     /* Read       */
#define AEC_REG_REALTIME_COUNTER        0x0428     /* Read/Write */

#define AEC_REG_IRQ_LOGLIST_STATUS      0x0480     /* Read       */
#define AEC_REG_IRQ_LOGLIST_ENABLE      0x0481     /* Read/Write */
#define AEC_REG_IRQ_LOGLIST_FIFO_READ_LO 0x0482     /* Read       */
#define AEC_REG_IRQ_LOGLIST_FIFO_READ_HI 0x0483     /* Read       */

#define AYE_REG_PXI_TRIGGER_OUT_SELECT  0x04C0     /* Read/Write */
#define AYE_REG_PXI_TRIGGER_OUT_ENABLE  0x04C1     /* Read/Write */
#define AYE_REG_PXI_TRIGGER_IN1_SELECT  0x04C2     /* Read/Write */
#define AYE_REG_PXI_TRIGGER_IN2_SELECT  0x04C3     /* Read/Write */
#define AYE_REG_PXI_IRIG_SOURCE_CONTROL 0x04C5     /* Read/Write */
#define AYE_REG_PXI_GEOGRAPHICAL_ADDR   0x04C6     /* Read       */

#define AYE_REG_PXI_GEOGRAPHICAL_ADDR_MASK 0x1FL    /* valid address bits mask*/


/*! \def AYE_REG_TRIGGER_EDGE_CONTROL_BITS
 * Valid bits per BIU
 */
#define AYE_REG_TRIGGER_EDGE_CONTROL_BITS 3L

/*! \def AYE_REG_TRIGGER_EDGE_CONTROL_MASK
 * Valid bits per BIU masked
 */
#define AYE_REG_TRIGGER_EDGE_CONTROL_MASK 0x7L

/*! \def AEC_REG_ASP_EMERGENCY
 * This register is used for signaling maintenance boot
 * requests on ANET boards 
 */
#define AEC_REG_ASP_EMERGENCY           0x0440


/* DMA channel 0 used for MilScope access */
#define AYE_REG_DMA_MS_CONTROL          0x0C00     /* Read/Write */
#define AYE_REG_DMA_MS_STATUS           0x0C01     /* Read       */
#define AYE_REG_DMA_MS_HEADER_COUNTER   0x0C02     /* Read       */
#define AYE_REG_DMA_MS_RESERVED         0x0C03     /* Read       */
#define AYE_REG_DMA_MS_SGL_CONTROL      0x0C04     /* Write      */
#define AYE_REG_DMA_MS_SGL_PCI_LOW      0x0C05     /* Write      */
#define AYE_REG_DMA_MS_SGL_PCI_HIGH     0x0C06     /* Write      */
#define AYE_REG_DMA_MS_SGL_DEVICE_LOW   0x0C07     /* Write      */

/* DMA channel 1 used for RAM (Global/Shared) access */
#define AYE_REG_DMA_MEM0_CONTROL          0x0C08     /* Read/Write */
#define AYE_REG_DMA_MEM0_STATUS           0x0C09     /* Read       */
#define AYE_REG_DMA_MEM0_HEADER_COUNTER   0x0C0A     /* Read       */
#define AYE_REG_DMA_MEM0_RESERVED         0x0C0B     /* Read       */
#define AYE_REG_DMA_MEM0_SGL_CONTROL      0x0C0C     /* Write      */
#define AYE_REG_DMA_MEM0_SGL_PCI_LOW      0x0C0D     /* Write      */
#define AYE_REG_DMA_MEM0_SGL_PCI_HIGH     0x0C0E     /* Write      */
#define AYE_REG_DMA_MEM0_SGL_DEVICE_LOW   0x0C0F     /* Write      */


/* Mil Scope controls register offsets */

/*! \def AYE_REG_MS_CONTROL
    offset of scope control register in 32bit steps
    relative to start of IO memory */
#define AYE_REG_MS_CONTROL                0x0840

/*! \def AYE_REG_MS_TRIGGER
    offset of scope trigger source, mode and channel select register 
    in 32bit steps relative to start of IO memory */
#define AYE_REG_MS_TRIGGER                0x0841


/*! \def AYE_REG_MS_TRIGGER_STROBE
    offset of scope trigger output strobe register in 32bit
    steps relative to start of IO memory */
#define AYE_REG_MS_TRIGGER_STROBE         0x842


/*! \def AYE_REG_MS_ACQUISITION
    offset of scope acquisition register in 32bit steps
    relative to start of IO memory */
#define AYE_REG_MS_ACQUISITION            0x0843

/*! \def AYE_REG_MS_COUPLING
    offset of scope coupling register in 32bit steps
    relative to start of IO memory */
#define AYE_REG_MS_COUPLING               0x0844

/*! \def AYE_REG_MS_SAMPLING_RATE
    offset of scope sampling rate register in 32bit steps
    relative to start of IO memory */
#define AYE_REG_MS_SAMPLING_RATE          0x0845

/*! \def AYE_REG_MS_TRIGGER_VALUE1
    offset of scope trigger value 1 register in 32bit steps
    relative to start of IO memory */
#define AYE_REG_MS_TRIGGER_VALUE1         0x0846

/*! \def AYE_REG_MS_TRIGGER_VALUE2
    offset of scope trigger value 2 register in 32bit steps
    relative to start of IO memory */
#define AYE_REG_MS_TRIGGER_VALUE2         0x0847

/*! \def AYE_REG_MS_TRIGGER_NO_OF_SAMPLES1
    offset of scope trigger samples register in 32bit steps
    relative to start of IO memory */
#define AYE_REG_MS_TRIGGER_NO_OF_SAMPLES1 0x0848

/*! \def AYE_REG_MS_TRIGGER_NO_OF_SAMPLES2
    offset of scope trigger samples 2 register in 32bit steps
    relative to start of IO memory */
#define AYE_REG_MS_TRIGGER_NO_OF_SAMPLES2 0x0849

/*! \def AYE_REG_MS_TRIGGER_DELAY
    offset of scope trigger delay register in 32bit steps
    relative to start of IO memory */
#define AYE_REG_MS_TRIGGER_DELAY          0x084A

/*! \def AYE_REG_MS_OFFSETS_STUB
    offset of scope offset register for stub coupled mode
    in 32bit steps relative to start of IO memory */
#define AYE_REG_MS_OFFSETS_STUB           0x084B

/*! \def AYE_REG_MS_OFFSETS_EXTERN
    offset of scope offset register for extern coupled mode
    in 32bit steps relative to start of IO memory */
#define AYE_REG_MS_OFFSETS_EXTERN         0x084D

/*! \def AYE_REG_MS_NO_OF_CYCLES
    offset of scope NoOfCycles register in 32bit steps
    relative to start of IO memory */
#define AYE_REG_MS_NO_OF_CYCLES           0x084C

/*! \def AYE_REG_MS_STATUS
    offset of scope state/status register in 32bit steps
    relative to start of IO memory */
#define AYE_REG_MS_STATUS                 0x0850




/* Interrupts */
#define AEC_IRQ_GPIO_0_HIGH         0x00000001
#define AEC_IRQ_GPIO_1_HIGH         0x00000002
#define AEC_IRQ_GPIO_2_HIGH         0x00000004
#define AEC_IRQ_GPIO_3_HIGH         0x00000008
#define AEC_IRQ_GPIO_4_HIGH         0x00000010
#define AEC_IRQ_GPIO_5_HIGH         0x00000020
#define AEC_IRQ_GPIO_6_HIGH         0x00000040
#define AEC_IRQ_GPIO_7_HIGH         0x00000080

#define AEC_IRQ_GPIO_0_LOW          0x00000100
#define AEC_IRQ_GPIO_1_LOW          0x00000200
#define AEC_IRQ_GPIO_2_LOW          0x00000400
#define AEC_IRQ_GPIO_3_LOW          0x00000800
#define AEC_IRQ_GPIO_4_LOW          0x00001000
#define AEC_IRQ_GPIO_5_LOW          0x00002000
#define AEC_IRQ_GPIO_6_LOW          0x00004000
#define AEC_IRQ_GPIO_7_LOW          0x00008000

#define AEC_IRQ_BIU_1               0x00010000
#define AEC_IRQ_BIU_2               0x00020000
#define AEC_IRQ_BIU_3               0x00040000
#define AEC_IRQ_BIU_4               0x00080000

#define AEC_IRQ_HOST                0x00100000
#define AEC_IRQ_ASP                 0x00200000
#define AEC_IRQ_RTC                 0x00400000
#define AEC_IRQ_DMA_MS              0x00800000 /* 23 */
#define AEC_IRQ_DMA_MEM0            0x01000000 /* 24 */
#define AEC_IRQ_DMA_MEM1            0x02000000 /* 25 */
#define AEC_IRQ_DMA_MS_OVERFLOW     0x04000000 /* 26 */

#define AEC_IRQ_ALL                 (AEC_IRQ_BIU_1 | AEC_IRQ_BIU_2 | AEC_IRQ_BIU_3 | AEC_IRQ_BIU_4 | AEC_IRQ_DMA_MS  | AEC_IRQ_DMA_MEM0 | AEC_IRQ_DMA_MS_OVERFLOW)

#define AEC_IRQ_DISCR_0             (AEC_IRQ_GPIO_0_LOW | AEC_IRQ_GPIO_0_HIGH)
#define AEC_IRQ_DISCR_1             (AEC_IRQ_GPIO_1_LOW | AEC_IRQ_GPIO_1_HIGH)
#define AEC_IRQ_DISCR_2             (AEC_IRQ_GPIO_2_LOW | AEC_IRQ_GPIO_2_HIGH)
#define AEC_IRQ_DISCR_3             (AEC_IRQ_GPIO_3_LOW | AEC_IRQ_GPIO_3_HIGH)
#define AEC_IRQ_DISCR_4             (AEC_IRQ_GPIO_4_LOW | AEC_IRQ_GPIO_4_HIGH)
#define AEC_IRQ_DISCR_5             (AEC_IRQ_GPIO_5_LOW | AEC_IRQ_GPIO_5_HIGH)
#define AEC_IRQ_DISCR_6             (AEC_IRQ_GPIO_6_LOW | AEC_IRQ_GPIO_6_HIGH)
#define AEC_IRQ_DISCR_7             (AEC_IRQ_GPIO_7_LOW | AEC_IRQ_GPIO_7_HIGH)

#define AEC_IRQ_DISCR_ALL           (AEC_IRQ_DISCR_0 | AEC_IRQ_DISCR_1 | AEC_IRQ_DISCR_2 | AEC_IRQ_DISCR_3 | AEC_IRQ_DISCR_4 | AEC_IRQ_DISCR_5 | AEC_IRQ_DISCR_6 | AEC_IRQ_DISCR_7)


/*! \def AEC_IRQ_POWER
 * This interrupt event is used by the
 * power controller on ANET boards to request power down
 */
#define AEC_IRQ_POWER               (1 << 27)


/* DMA */
#define AYE_DMA_CTRL_START      0x02


/* MIL Scope */

/*! \def AYE_MS_MAX_TRIGGER_DELAY
    Maximum number of samples the trigger delay can be set
    on AYE boards */
#define AYE_MS_MAX_TRIGGER_DELAY 0xFFFFF

/*! \def AYE_MS_MAX_SAMPLE_VALUE
    Maximum digital value of a scope sample (= highest voltage)
    Half this value is zero-line, 0 is lowest voltage */
#define AYE_MS_MAX_SAMPLE_VALUE 0x3FF

/*! \def AYE_MS_MAX_PHYSICAL_PULSE
    Maximum number of samples that can be used for checking
    for a high pulse trigger */
#define AYE_MS_MAX_PHYSICAL_PULSE 0xFF

/*! \def AYE_MS_MAX_SYNC_GAP
    Maximum number of samples the minimum gap before
    command sync, when using command sync trigger, has to be */
#define AYE_MS_MAX_SYNC_GAP 0x3FF

/*! \def AYE_MAX_DISCRETES
    Maximum number of discrete channels of an AYE board that
    can be used for scope trigger */
#define AYE_MS_MAX_DISCRETES 8

/*! \def AYE_MS_SAMPLES_PER_SEC
    Number of scope samples per second in default sampling rate */
#define AYE_MS_SAMPLES_PER_SEC 50000000

/*! \def AYE_MS_SAMPLES_PER_USEC
    Number of scope samples per microsecond in default sampling rate */
#define AYE_MS_SAMPLES_PER_USEC (AYE_MS_SAMPLES_PER_SEC / 1000000)

/*! \def AYE_MS_MAX_OFFSET_COMP
    Maximum absolute offset value for scope offset  compensation \
    on AYE boards */
#define AYE_MS_MAX_OFFSET_COMP  31




/*! enumeration for all possible DMA destinations
    on AyE boards */
typedef enum _AyEDmaDestination
{
    AyEDmaDestGlobal = 1,  /*!< destination is global memory */
    AyEDmaDestHost = 3,    /*!< destination is host */
}AyEDmaDestination;


/*! enumeration for all possible DMA sources
    on AyE boards */
typedef enum _AyeDmaSource
{
    AyEDmaSourceMilScope = 0,   /*!< source is mil scope data */
    AyEDmaSourceGlobal = 1,     /*!< source is global memory */
    AyEDmaSourceShared = 2,     /*!< source is shared memory */
    AyEDmaSourceHost = 3,       /*!< source is host */
}AyEDmaSource;


/*! enumeration for interrupt settings
    in DMA entries on AyE boards */
typedef enum _AyEDmaInterrupt
{
    AyEDmaDisableInt = 0,
    AyEDmaEnableInt = 1,
}AyEDmaInterrupt;


/*! enumeration for DMA stop settings
    in DMA entries on AyE boards */
typedef enum _AyEDmaStopAfterEntry
{
    AyEDmaDontStop = 0,
    AyEDmaStop = 1,
}AyEDmaStopAfterEntry;


/*! Represents the header free register in
    the LCA on AyE boards */
typedef union _AyEDmaHeaderCount
{
    AiUInt32 all;                   /*!< whole register value */
    struct
    {
#ifdef HOST_ENDIAN_BIG
        AiUInt32 reserved : 16;
        AiUInt32 count : 16;
#else
        AiUInt32 count : 16;        /*!< number of free u32 values in the DMA FIFO */
        AiUInt32 reserved : 16;     /*!< reserved */
#endif
    }part;
}AyEDmaHeaderCount;


/*! Represents the control word of a DMA entry
    on AyE boards */
typedef union _AyEDmaEntryControlWord
{
    AiUInt32 all;
    struct
    {
#ifdef HOST_ENDIAN_BIG
        AiUInt32 Size : 24;
        AiUInt32 MilScopeBlock : 1;
        AiUInt32 DmaDest : 2;
        AiUInt32 DmaSource : 2;
        AiUInt32 bUsePattern : 1;
        AiUInt32 bIrq : 1;
        AiUInt32 bStop : 1;
#else
        AiUInt32 StopDma : 1;           /*!< signifies if DMA engine shall be stopped after the entry */
        AiUInt32 Interrupt : 1;         /*!< signifies if interrupt shall be raises after the entry */
        AiUInt32 bUsePattern : 1;       /*!< use data pattern for test purposes instead of specified source */
        AiUInt32 DmaSource : 2;         /*!< source of the DMA transfer */
        AiUInt32 DmaDest : 2;           /*!< Destination of the DMA transfer */
        AiUInt32 MilScopeBlock : 1;     /*!< used for scope DMA, signifies if primary or secondary channel */
        AiUInt32 Size : 24;             /*!< size of DMA transfer in byte */
#endif
    }part;
}AyEDmaEntryControlWord;


/*! Represents one DMA entry on AyE boards */
typedef struct
{
    AyEDmaEntryControlWord ulControl;   /*!< The \ref AyEDmaEntryControlWord of the entry */
    AiUInt32  ulPciAddressLow;          /*!< lower 32Bit of PI DMA address */
    AiUInt32  ulPciAddressHigh;         /*!< higher 32bit of PCI DMA address */
    AiUInt32  ulDeviceAddress;          /*!< Address of target DMA address */
} TY_AYE_DMA_DMA_SGL_ENTRY;

typedef struct
{
    AiUInt32                 ulControl;
    AiUInt32                 ulStatus;
    AyEDmaHeaderCount        ulHeaderCounter;
    AiUInt32                 ulReserved;
    TY_AYE_DMA_DMA_SGL_ENTRY xSglEntry;
} TY_AYE_DMA_DMA_SGL_SETUP;


/*! \enum TY_AYE_MS_AQUISITION_MODE
    possible values for scope acquisition register
    on AYE boards */
enum TY_AYE_MS_AQUISITION_MODE
{
    MS_ACQ_DISABLED = 0,        /*!< Acquisition disabled */
    MS_ACQ_PRIMARY_50M = 1,     /*!< Acquisition on primary channel only with 50M samples */
    MS_ACQ_SECONDARY_50M = 2,   /*!< Acquisition on secondary channel only with 50M samples */
    MS_ACQ_PRIMARY_100M = 3,    /*!< Acquisition on primary channel only with 100M samples */
    MS_ACQ_SECONDARY_100M = 4,  /*!< Acquisition on secondary channel only with 50M samples */
    MS_ACQ_DUAL_LESS_50M = 5,   /*!< Acquisition on both channels with less than 50M samples (not used) */
    MS_ACQ_DUAL_50M = 6        /*!< Acquisition on both channels with 50M samples each */
};


/*! \enum TY_AYE_MS_TRIGGER_SELECT_PtMS
    possible values for the PtMS field of the 
    "Trigger Source, Mode and Channel select" register on AYE boards */
enum TY_AYE_MS_TRIGGER_SELECT_PtMS
{
    MS_PTMS_DISABLED = 0,       /*!< No trigger mode selected */
    MS_PTMS_HIGHER = 1,         /*!< Trigger if higher than "value1" */
    MS_PTMS_LOWER = 2,          /*!< Trigger if lower than "value1" */
    MS_PTMS_HIGH_PULSE = 3,     /*!< High Pulse (higher than "value1", for No of "Samples1") */
    MS_PTMS_LOW_PULSE = 4,      /*!< Low Pulse (lower than value1, for No of "Samples1") */
    MS_PTMS_HIGHER1_LOWER2 = 5, /*!< Trigger if higher than "value1" or lower then "value2" */
    MS_PTMS_COMMAND_SYNC = 6    /*!< start with command sync */
};


/*! \enum TY_AYE_MS_PHYSICAL_TRIGGER_CHANNEL
    possible scope trigger channels in physical trigger mode */
enum TY_AYE_MS_PHYSICAL_TRIGGER_CHANNEL
{
    MS_TRIGGER_PRIMARY_CHANNEL = 1,     /*!< Physical trigger on primary channel */
    MS_TRIGGER_SECONDARY_CHANNEL = 2    /*!< Physical trigger on secondary channel */
};

/*! \This enum holds all possible scope states on AYE boards */
enum AYE_MS_STATUS
{
    AYE_MS_STATUS_RESET = 0,            /*!< scope is in reset */
    AYE_MS_STATUS_IDLE = 1,             /*!< scope is in idle mode */
    AYE_MS_STATUS_START = 2,            /*!< scope is in start mode */
    AYE_MS_STATUS_DELAY_ACQ = 3,        /*!< scope is started and delaying acquisition */
    AYE_MS_STATUS_WAIT_BLOCK_FULL = 4,  /*!< scope is waiting for first block RAM full */
    AYE_MS_STATUS_WAIT_TRIGGER = 5,     /*!< scope is waiting for trigger */
    AYE_MS_STATUS_ACQ_BLOCK1 = 6,       /*!< scope is triggered and is recording data in block RAM 1 */
    AYE_MS_STATUS_ACQ_BLOCK2 = 7        /*!< scope is triggered and is recording data in block RAM 2 */
};

/*! \enum AYE_MS_PROTOCOL_TRIGGER
    This enum holds all possible protocol trigger settings
    on AYE boards */
enum AYE_MS_PROTOCOL_TRIGGER
{
    AYE_MS_TRIGGER_BIU_BM  = 1,  /*!< Trigger on stream 1 monitor */
    AYE_MS_TRIGGER_BIU_BC  = 2,  /*!< Trigger on stream 1 BC strobe instruction */
    AYE_MS_TRIGGER_BIU2_BM = 3,  /*!< Trigger on stream 2 monitor */
    AYE_MS_TRIGGER_BIU2_BC = 4,  /*!< Trigger on stream 2 BC strobe instruction */
};


/*! This union describes a bit field for all possible settings
    of the scope control register on AYE boards */
typedef union _TY_AYE_MS_CONTROL
{
    AiUInt32 all;                           /*!< value of register as a whole */

    struct
    {
        AiUInt32 reset          : 1;    /*!< if true, the MILScope is set into the RESET state */
        AiUInt32 start          : 1;    /*!< if true, the MILScope is started. After start this bit is set to 0 by the LCA */
        AiUInt32 stop           : 1;    /*!< if true, the MILScope data acquisition is stopped */
        AiUInt32 reserved       : 1;    /*!< reserved bits */
        AiUInt32 ffbbt          : 1;    /*!< if true, the trigger is enabled after the block ram buffer is filled.
                                             This is to get valid trace before trigger data. */
        AiUInt32 reserved2      : 3;    /*!< reserved bits */
        AiUInt32 host_irq       : 8;    /*!< if true, primary channel is either connected to extern or stub, */
        AiUInt32 reserved3      : 16;   /*!< reserved bits */

    } settings; /*!< bit field for all possible settings of the register */
}TY_AYE_MS_CONTROL;



/*! This union describes a bit field for all possible settings
    of the scope trigger select register on AYE boards */
typedef union _TY_AYE_MS_TRIGGER_SELECT
{
    AiUInt32 all;                           /*!< value of register as a whole */

    struct
    {
        AiUInt32 trigger_source          : 3;    /*!< Trigger Source Selection (TSS) */
        AiUInt32 reserved1               : 1;    /*!< reserved bits */
        AiUInt32 physical_trigger        : 2;    /*!< Physical Trigger Selection (PyTS) */
        AiUInt32 reserved2               : 2;    /*!< reserved bits */
        AiUInt32 physical_trigger_mode   : 3;    /*!< Physical Trigger Mode Selection (PtMS) */
        AiUInt32 reserved3               : 1;    /*!< reserved bits */
        AiUInt32 external_trigger_source : 4;    /*!< External Trigger Source Selection (ETS) */
        AiUInt32 protocol_trigger        : 3;    /*!< Protocol TriggerSelection (PrTS) */
        AiUInt32 reserved4               : 13;   /*!< reserved bits */

    } settings; /*!< bit field for the trigger select register */
}TY_AYE_MS_TRIGGER_SELECT;




/*! This union describes a bit field for all possible settings
    of the scope coupling register on AYE boards */
typedef union _TY_AYE_MS_COUPLING
{
    AiUInt32 all;                           /*!< value of register as a whole */

    struct
    {
        AiUInt32 secondary_to_stub          : 1;    /*!< if true, secondary is connected to stub, externally when false */
        AiUInt32 secondary_3V               : 1;    /*!< if true, 3V range is used on secondary channel, 30V when false */
        AiUInt32 secondary_to_signal_source : 1;    /*!< if true, secondary channel is either connected to extern or stub, 
                                                    otherwise it's cross-connected to primary channel (For 100M mode) */
        AiUInt32 reserved                   : 13;   /*!< reserved bits */
        AiUInt32 primary_to_stub            : 1;    /*!< if true, primary is connected to stub, externally when false */
        AiUInt32 primary_3V                 : 1;    /*!< if true, 3V range is used on primary channel, 30V when false */
        AiUInt32 primary_to_signal_source   : 1;    /*!< if true, primary channel is either connected to extern or stub, 
                                                    otherwise it's cross-connected to secondary channel (For 100M mode) */       
        AiUInt32 reserved2                  : 13;   /*!< reserved bits */

    } settings; /*!< bit field for all possible settings of the register */
}TY_AYE_MS_COUPLING;


/*! This union describes a bit field for all possible settings
    of the scope state and status register on AYE boards */
typedef union _TY_AYE_MS_STATUS
{
    AiUInt32 all;                           /*!< value of register as a whole */

    struct
    {
        AiUInt32 state                      : 4;   /*!< the current state of the LCA state machine. See Milscope LCA spec for details */
        AiUInt32 reserved4                  : 4;   /*!< reserved bits */
        AiUInt32 fbrf                       : 1;   /*!< first block ram full. A value of '1' indicates a valid trace before trigger */
        AiUInt32 reserved3                  : 3;   /*!< reserved bits */
        AiUInt32 finish                     : 1;   /*!< a value of '1' means data acquisition has stopped */
        AiUInt32 reserved2                  : 3;   /*!< reserved bits */
        AiUInt32 dma_fail                   : 1;   /*!< a value of '1' means a DMA underrun */
        AiUInt32 reserved                   : 15;  /*!< reserved bits */
    } settings; /*!< bit field for all possible settings of the register */
}TY_AYE_MS_STATUS;


/*! This union describes a bit field for all possible
    settings of the scope DMA control register on AYE boards*/
typedef union AYE_MS_DMA_CONTROL
{
    AiUInt32 all;                /*!< value of register as a whole */

    struct
    {
        AiUInt32 reset    : 1;   /*!< bit to initiate scope DMA engine reset */
        AiUInt32 start    : 1;   /*!< bit to start scope DMA engine */
        AiUInt32 reserved : 30;  /*!< reserved bits */
    } settings; /*!< bit field for all possible settings of the register */

}TY_AYE_MS_DMA_CONTROL;


/*! enumeration for all possible trigger strobe settings
    on scope capable AYE boards */
typedef enum AYE_MS_TRIGGER_STROBE_SETTINGS
{
    AYE_MS_TRIGGER_STROBE_DISCRETE_1 = 1,   /*!< On trigger, issue strobe on discrete channel 1 */
    AYE_MS_TRIGGER_STROBE_DISCRETE_2 = 2,   /*!< On trigger, issue strobe on discrete channel 2 */
    AYE_MS_TRIGGER_STROBE_DISCRETE_3 = 3,   /*!< On trigger, issue strobe on discrete channel 3 */
    AYE_MS_TRIGGER_STROBE_DISCRETE_4 = 4,   /*!< On trigger, issue strobe on discrete channel 4 */
    AYE_MS_TRIGGER_STROBE_DISCRETE_5 = 5,   /*!< On trigger, issue strobe on discrete channel 5 */
    AYE_MS_TRIGGER_STROBE_DISCRETE_6 = 6,   /*!< On trigger, issue strobe on discrete channel 6 */
    AYE_MS_TRIGGER_STROBE_DISCRETE_7 = 7,   /*!< On trigger, issue strobe on discrete channel 7 */
    AYE_MS_TRIGGER_STROBE_DISCRETE_8 = 8,   /*!< On trigger, issue strobe on discrete channel 8 */
    AYE_MS_TRIGGER_STROBE_BM_1       = 9,   /*!< On trigger, issue strobe on BIU 1 monitor */
    AYE_MS_TRIGGER_STROBE_BC_1       = 10,  /*!< On trigger, issue strobe on BIU 1 bus controller */
    AYE_MS_TRIGGER_STROBE_BM_2       = 11,  /*!< On trigger, issue strobe on BIU 2 monitor */
    AYE_MS_TRIGGER_STROBE_BC_2       = 12,  /*!< On trigger, issue strobe on BIU 2 bus controller */
}TY_AYE_MS_TRIGGER_STROBE_SETTINGS;


/*! This union describes a bit field for the scope trigger output
    register on AYE boards */
typedef union AYE_MS_TRIGGER_STROBE
{
    AiUInt32 all;                /*!< value of register as a whole */

    struct
    {
        TY_AYE_MS_TRIGGER_STROBE_SETTINGS strobe   : 4;   /*!< trigger strobe settings */
        AiUInt32 reserved : 28;                           /*!< reserved bits */
    } settings; /*!< bit field for all possible settings of the register */

}TY_AYE_MS_TRIGGER_STROBE;


/*! This union describes the scope offset register on AYE boards */
typedef union AYE_MS_OFFSETS
{
    AiUInt32 all;                /*!< value of register as a whole */

    struct
    {
        AiUInt32 abs_primary_val_30V            : 5;    /*!< absolute offset value on primary channel in 30V measurement range */
        AiUInt32 reserved1                      : 2;    /*!< reserved */
        AiUInt32 primary_negative_offset_30V    : 1;    /*!< sign bit for primary channel in 30V measurement range. If 1, offset is negative */
        AiUInt32 abs_primary_val_3V             : 5;    /*!< absolute offset value on primary channel in 3V measurement range */
        AiUInt32 reserved2                      : 2;    /*!< reserved */
        AiUInt32 primary_negative_offset_3V     : 1;    /*!< sign bit for primary channel in 30V measurement range. If 1, offset is negative */
        AiUInt32 abs_secondary_val_30V          : 5;    /*!< absolute offset value on secondary channel in 30V measurement range */
        AiUInt32 reserved3                      : 2;    /*!< reserved */
        AiUInt32 secondary_negative_offset_30V  : 1;    /*!< sign bit for secondary channel in 30V measurement range. If 1, offset is negative */
        AiUInt32 abs_secondary_val_3V           : 5;    /*!< absolute offset value on secondary channel in 3V measurement range */
        AiUInt32 reserved4                      : 2;    /*!< reserved */
        AiUInt32 secondary_negative_offset_3V   : 1;    /*!< sign bit for secondary channel in 3V measurement range. If 1, offset is negative */
    }settings; /*!< bit field for all possible settings of the register */
}TY_AYE_MS_OFFSETS;







#endif /* __AIHWAYE_H__ */


