/* SPDX-FileCopyrightText: 2016-2024 AIM GmbH <info@aim-online.com> */
/* SPDX-License-Identifier: GPL-2.0-or-later */

/*! \file aim_tsw_mil.c
 *
 *  This file contains definitions for handling the 1553 target software
 *
 */


#include "aim_tsw_mil.h"
#include "api_prot.h"
#include "aim_pci_mem.h"
#include "aim_timer.h"
#include "mil_tsw_version.h"
#include "mil_hw.h"
#include "aim_ays_device.h"
#include "esmart.h"
#include "hw/AiHwArtix7.h"
#include "hw/AiHwArtixUS.h"



/*! \def TARGET_COMMAND_REG_OFFSET
 *
 * Byte offset of target command registers in shared memory
 */
#define TARGET_COMMAND_REG_OFFSET 0x1F00

/*! \def TARGET_COMMAND_OFFSET
 *
 * Byte offset to target command area in shared memory
 */
#define TARGET_COMMAND_OFFSET 0x2000

/*! \def TARGET_ACKNOWLEDGE_OFFSET
 *
 * Byte offset to acknowledge area in shared memory
 */
#define TARGET_ACKNOWLEDGE_OFFSET 0x5000

/*! \def TARGET_COMMAND_SIZE
 *
 * Size of target command area in bytes
 */
#define TARGET_COMMAND_SIZE (TARGET_ACKNOWLEDGE_OFFSET - TARGET_COMMAND_OFFSET)

/*! \def TARGET_TIMEOUT_SEC
 *
 * Target command time-out in seconds
 */
#define TARGET_TIMEOUT_SEC 30

/*! \def TARGET_CMD_EXE_BIT
 *
 * Bit that indicates target is executing a command
 */
#define TARGET_CMD_EXE_BIT  0x1

/*! \def TARGET_CMD_COMPLETE_BIT
 *
 * Bit that indicates processing of a target command has completed
 */
#define TARGET_CMD_COMPLETE_BIT 0x2




#define NVR_MAGIC_A         0x00
#define NVR_SERIAL_A        0x04
#define NVR_BOARDCFG_A      0x08
#define NVR_HWVARIANT_A     0x0C
#define NVR_BOARDTYPE_A     0x10
#define NVR_BOARDSUBTYP_A   0x14  // Board Sub-Type Word location
#define NVR_BOARDAPP_TYPE_A 0x18  // Board Application Type
#define NVR_PARTNO_A        0x1C
#define NVR_PARTNO_EXT_A    0x20
#define NVR_FIRSTDATE_A     0x28   // Board Build Date
#define NVR_ACTDATE_A       0x2C
#define NVR_PL_Version      0x34  // Programmable Logic Qverall Version
#define NVR_PHY1RAM_SIZE_A  0x40
#define NVR_PHY2RAM_SIZE_A  0x44
#define NVR_PHY3RAM_SIZE_A  0x48


static void aim_mil_initialize_tsw_config( struct aim_pci_mil_device* milDevice, TY_API_HW_INIT_PARAMS * pxTswInitParams )
{
    struct aim_mil_device  * pMilDevice  = NULL;

    pxTswInitParams->BoardSerial  = milDevice->aimDevice.properties.serial;
    pxTswInitParams->BoardConfig  = milDevice->aimDevice.properties.config;
    pxTswInitParams->BoardType    = milDevice->aimDevice.properties.type;
    pxTswInitParams->BoardSubType = milDevice->aimDevice.properties.sub_type;
    pxTswInitParams->BoardSubType2= 0;
    pxTswInitParams->BoardAppType = 0;
    pxTswInitParams->BoardAppType2= 0;
    pxTswInitParams->BoardPartNo  = milDevice->aimDevice.properties.part_number;
    pxTswInitParams->HwVariant    = milDevice->aimDevice.properties.hw_variant;

    pxTswInitParams->CPUClk          = 0;
    pxTswInitParams->SYSClk          = 0;

    // Monitor / MyMon
    pxTswInitParams->mon_data        = NULL;
    pxTswInitParams->pxMyMonContext  = milDevice->aimDevice.mymonData;

    pxTswInitParams->PlatformGroup = milDevice->aimDevice.deviceData->platform;

    // Initialize pointers to memory bars
    pxTswInitParams->pDevice         = pMilDevice;
    pxTswInitParams->pDeviceContext  = pMilDevice;
    pxTswInitParams->SharedRAMBase   = milDevice->aimDevice.sharedMemory;
    pxTswInitParams->GlobalRAMBase   = milDevice->aimDevice.globalMemory;
    pxTswInitParams->GlobalRAMSize   = milDevice->aimDevice.globalMemorySize;
    pxTswInitParams->SharedRAMSize   = milDevice->aimDevice.sharedMemorySize;
    pxTswInitParams->IORAMBase       = milDevice->aimDevice.ioMemory;
    pxTswInitParams->PlatformGroup   = milDevice->aimDevice.deviceData->platform;


    if( milDevice->aimDevice.deviceData->platform == AI_DEVICE_AYE )
    {
        // -- copy TSW version into MyMon version table  ---
        __u32 ulTswVersion = 0;
        __u32 appType      = 0;

        ulTswVersion |= (bin2bcd(MIL_TSW_MAJOR_VERSION) << 24) | bin2bcd(MIL_TSW_MINOR_VERSION) << 16;
        ulTswVersion |= (bin2bcd((MIL_TSW_PATCH_VERSION / 100)) << 8) + bin2bcd(MIL_TSW_PATCH_VERSION % 100);

        iowrite32(ulTswVersion, milDevice->aimDevice.globalExtMemory + MYMON_GREG_VERSION_ASP);

        // -- get application type from NOVRAM  ---
        appType = ioread32(milDevice->aimDevice.globalExtMemory + MYMON_GREG_NOVRAM_BOARDAPP_TYP);

        pxTswInitParams->BoardAppType  = appType;
    }
    
    if( milDevice->aimDevice.deviceData->platform == AI_DEVICE_AYS)
    {
    	int ret;
    	int BoardType;
        pxTswInitParams->BoardAppType = 0;
		ret = aim_gen_ays_novram_get(&milDevice->aimDevice,NVR_BOARDAPP_TYPE_A,sizeof(AiUInt32),0,&BoardType);
        pxTswInitParams->BoardAppType  = BoardType;
		pxTswInitParams->pDevice  = mil_to_aim(milDevice);
    }

    if( milDevice->aimDevice.deviceData->platform == AI_DEVICE_ARTIX7)
    {
        pxTswInitParams->BoardAppType  = aim_pci_io_read32(&milDevice->aimDevice, ARTIX7_REG_NOVRAM_BOARDAPP_TYP* sizeof(__u32));
        // pxTswInitParams->BoardSubType see top of function
        pxTswInitParams->pDevice  = mil_to_aim(milDevice);
    }

    if( milDevice->aimDevice.deviceData->platform == AI_DEVICE_ARTIXUS)
    {
        pxTswInitParams->BoardAppType  = aim_pci_io_read32(&milDevice->aimDevice, ARTIXUS_REG_NOVRAM_BOARDAPP_TYP* sizeof(__u32));
        pxTswInitParams->BoardAppType2 = aim_pci_io_read32(&milDevice->aimDevice, ARTIXUS_REG_NOVRAM_BOARDAPP_TYP2* sizeof(__u32));
        // pxTswInitParams->BoardSubType see top of function
        pxTswInitParams->BoardSubType2 = aim_pci_io_read32(&milDevice->aimDevice, ARTIXUS_REG_NOVRAM_FRONTEND_CAP2* sizeof(__u32));
        pxTswInitParams->pDevice  = mil_to_aim(milDevice);
    }

}

static int aim_pci_mil_init_ays(struct aim_pci_device * aimDevice)
{
	int ret = 0;
	AiUInt32 nvram_value = 0;
    /* ---- NovRAM addresses -------- */

	ret = aim_gen_ays_novram_get(aimDevice, NVR_SERIAL_A, sizeof(AiUInt32), 0,&nvram_value);
	if(ret)
		aim_dev_error(aimDevice,"Failed to read Serial Number from NOVRAM");
	else
		aimDevice->properties.serial = nvram_value;

	ret = aim_gen_ays_novram_get(aimDevice, NVR_BOARDCFG_A, sizeof(AiUInt32), 0, &nvram_value);
	if(ret)
		aim_dev_error(aimDevice,"Failed to read Board Config from NOVRAM");
	else
		aimDevice->properties.config = nvram_value;

	ret = aim_gen_ays_novram_get(aimDevice, NVR_BOARDTYPE_A, sizeof(AiUInt32), 0, &nvram_value);
	if(ret)
		aim_dev_error(aimDevice,"Failed to read Board Type from NOVRAM");
	else
		aimDevice->properties.type = nvram_value;

	ret = aim_gen_ays_novram_get(aimDevice, NVR_BOARDSUBTYP_A,  sizeof(AiUInt32), 0, &nvram_value);
	if(ret)
		aim_dev_error(aimDevice,"Failed to read BoardSubType from NOVRAM");
	else
		aimDevice->properties.sub_type = nvram_value;

	ret = aim_gen_ays_novram_get(aimDevice, NVR_HWVARIANT_A,  sizeof(AiUInt32), 0, &nvram_value);
	if(ret)
		aim_dev_error(aimDevice,"Failed to read HW Variant from NOVRAM");
	else
		aimDevice->properties.hw_variant = nvram_value;

	ret = aim_gen_ays_novram_get(aimDevice, NVR_PARTNO_A, sizeof(AiUInt32), 0, &nvram_value);
	if(ret)
		aim_dev_error(aimDevice,"Failed to read Part Number from NOVRAM");
	else
		aimDevice->properties.part_number = nvram_value;


	return ret;
}

int aim_pci_mil_init_tsw(struct aim_pci_mil_device* milDevice)
{
    int ret = 0;
    struct aim_pci_device * aimDevice;
    TY_API_HW_INIT_PARAMS xTswConfig;

    memset( &xTswConfig, 0, sizeof(xTswConfig) );

    aimDevice = mil_to_aim(milDevice);

    BUG_ON(!milDevice);

    aim_dev_debug(mil_to_aim(milDevice), "Initializing target software");

    if( aimDevice->deviceData->platform == AI_DEVICE_AYS)
    {
    	ret = aim_pci_mil_init_ays(aimDevice);

        if(ret)
            return ret;
    }

    milDevice->tswData = kzalloc(sizeof(TY_API_DEV), GFP_KERNEL);

    if(!milDevice->tswData)
    {
        aim_dev_error(mil_to_aim(milDevice), "Failed to allocate target software data");
        return -ENOMEM;
    }

    // -- TSW LIB entry ---

    aim_mil_initialize_tsw_config( milDevice, &xTswConfig );

    /* call target software initialization functions */

    aim_dev_info(aimDevice, "Initializing TSW %s", MIL_TSW_VERSION_STRING_FULL );
    aim_dev_info(aimDevice, "Build date %s", MIL_TSW_BUILD_DATE );

    mil_hw_initialize_tsw( milDevice->tswData, &xTswConfig );

    return ret;
}


int aim_exec_asp_command_mil(struct aim_pci_mil_device* device, void* command, size_t commandSize, void* ack_buffer, size_t expectedAckSize)
{
	TY_API_TARG_COM_REG* targetComRegisters = NULL;
    void* acknowledgeArea = NULL;
    struct aim_expiration_timer timer;
    __u32 status = 0;
    int ret = 0;
    AiUInt32 ackSize = 0;
    TY_MIL_COM_ACK* newAcknowledge = ack_buffer;
    unsigned char* legacyAcknowledge = ack_buffer;

    BUG_ON(!device || !device->aimDevice.sharedMemory || !command);

    targetComRegisters = device->aimDevice.sharedMemory + TARGET_COMMAND_REG_OFFSET;

    acknowledgeArea = device->aimDevice.sharedMemory + TARGET_ACKNOWLEDGE_OFFSET;

    if( commandSize > TARGET_COMMAND_SIZE )
    {
        aim_dev_error(mil_to_aim(device), "Exceeding command size: %zu\n", commandSize);
        return -EINVAL;
    }

    do
    {
        memcpy_toio(device->aimDevice.sharedMemory + TARGET_COMMAND_OFFSET, command, commandSize);

        /* Clear target communication status register flags. */
        iowrite32(0xff000000, (void __iomem*) &targetComRegisters->tg_status);

        aim_init_expiration_timer(&timer, TARGET_TIMEOUT_SEC * 1000);
        aim_start_expiration_timer(&timer);

        /* Trigger command execution and wait for completion */
        iowrite32(TARGET_CMD_EXE_BIT, (void __iomem*) &targetComRegisters->tg_control);

        while( !(status & TARGET_CMD_COMPLETE_BIT) && !aim_is_expiration_timer_expired(&timer) )
        {
            status = ioread32((void __iomem*) &targetComRegisters->tg_status);
        }

        aim_stop_expiration_timer(&timer);

        if( aim_is_expiration_timer_expired(&timer) )
        {
            aim_dev_error(mil_to_aim(device),  "Target command timeout occurred\n");
            ret = -ETIME;
            break;
        }

        memcpy_fromio(ack_buffer, acknowledgeArea, sizeof(TY_MIL_COM_ACK));

        if(newAcknowledge->ulMagic == MIL_COM_MAGIC)
        {
            /* This was a new host-to-target command */
            ackSize = newAcknowledge->ulSize;

        }
        else
        {
            switch (legacyAcknowledge[1])
            {
                case API_TG_ACK:
                    ackSize = 8;
                    break;

                case API_TG_ACK2:
                    ackSize = (legacyAcknowledge[2] << 8) + legacyAcknowledge[3];
                    break;

                case API_TG_NAK:
                    ackSize = 8;
                    break;

                default:
                    ret = -EIO;
                    break;
            }
        }

        if( ackSize > expectedAckSize )
        {
            aim_dev_error(mil_to_aim(device), "Ack buffer too small\n");
            ret = -EINVAL;
            break;
        }

        if( ackSize > sizeof(TY_MIL_COM_ACK) )
        {
            memcpy_fromio(ack_buffer + sizeof(TY_MIL_COM_ACK), acknowledgeArea + sizeof(TY_MIL_COM_ACK), ackSize - sizeof(TY_MIL_COM_ACK));
        }

    } while( 0 );

    return ret == 0 ? ackSize : ret;
}

#define TARGET_MIL_COM_CHANNEL_ID      1


int aim_exec_asp_command_mil_irq(struct aim_pci_mil_device* device, void* command, size_t commandSize, void* response, size_t expectedResponseSize)
{
    int ret;
    struct aim_pci_com_channel* channel = NULL;
    AiUInt32 ackSize = 0;
    TY_MIL_COM_ACK* newAcknowledge = response;
    unsigned char* legacyAcknowledge = response;


    BUG_ON(!device || !command);


    channel = radix_tree_lookup(&mil_to_aim(device)->pciComChannels, TARGET_MIL_COM_CHANNEL_ID);

    if(!channel)
    {
        aim_dev_error(mil_to_aim(device), "Communication channel with ID %u was not found", TARGET_MIL_COM_CHANNEL_ID);
        return -EINVAL;
    }


    aim_dev_debug(mil_to_aim(device), "Target command with IRQ\n");

    do
    {
        ret = aim_pci_com_channel_issue_cmd(channel, command, commandSize, response, expectedResponseSize);

        if(ret)
            break;

        if(newAcknowledge->ulMagic == MIL_COM_MAGIC)
        {
            /* This was a new host-to-target command */
            ackSize = newAcknowledge->ulSize;

        }
        else
        {
            switch (legacyAcknowledge[1])
            {
                case API_TG_ACK:
                    ackSize = 8;
                    break;

                case API_TG_ACK2:
                    ackSize = (legacyAcknowledge[2] << 8) + legacyAcknowledge[3];
                    break;

                case API_TG_NAK:
                    ackSize = 8;
                    break;

                default:
                    ret = -EIO;
                    break;
            }
        }

        if( ackSize > expectedResponseSize )
        {
            aim_dev_error(mil_to_aim(device), "Ack buffer too small\n");
            ret = -EINVAL;
            break;
        }
    } while( 0 );

    return ret == 0 ? ackSize : ret;
}

#define AIM_HW_ZYNQMP_MIL_GLOBAL_OFFSET 0x3E000000LL
#define AIM_HW_ZYNQMP_MIL_GLOBAL_SHARED 0x3F100000LL

int aim_tsw_mil_get_dma_memory_device_offset(struct aim_pci_device* aimDevice, enum aim_memory_type memType, u64 * offset)
{
    if ( aimDevice->deviceData->platform !=  AI_DEVICE_ZYNQMP_ASP)
        return -ENODEV;

    if ((memType != AIM_MEMTYPE_GLOBAL) && (memType != AIM_MEMTYPE_SHARED))
       return -EINVAL;

    if (memType == AIM_MEMTYPE_GLOBAL)
        *offset = AIM_HW_ZYNQMP_MIL_GLOBAL_OFFSET;
    else
        *offset = AIM_HW_ZYNQMP_MIL_GLOBAL_SHARED;

    return 0;
}
