/* SPDX-FileCopyrightText: 2017-2021 AIM GmbH <info@aim-online.com> */
/* SPDX-License-Identifier: GPL-2.0-or-later */

/******************************************************************************

Description:
This file contains function definitions used for DMA transactions specific to
MIL protocol and AyX cards
-------------------------------------------------------------------------------

 ******************************************************************************/

#include "aim_mil_ayx_dma.h"
#include "aim_tsw_mil.h"

#include "AiMilCom.h"


int aim_pci_mil_ayx_device_start_dma_read(struct aim_pci_device* device, struct aim_dma_buffer* buffer, enum aim_memory_type memType, loff_t offset)
{
    size_t cmd_size;
    TY_MIL_COM_SYS_DMA_SETUP_SGL_INPUT * tsw_cmd = NULL;
    TY_MIL_COM_ACK tsw_ack;
    struct scatterlist* p_CurrentScatterListEntry = NULL;
    int i;
    int ret;
    dma_addr_t DMAAddress;
    struct aim_pci_mil_device * mil_device = aim_dev_to_mil(device);

    cmd_size  = sizeof(TY_MIL_COM_SYS_DMA_SETUP_SGL_INPUT);
    cmd_size += sizeof(TY_MIL_COM_DMA_SETUP_SGL_INPUT_ENTRY) * buffer->scatterTable.nents;

    if(cmd_size > MAX_API_CMD_SIZE)
    {
        return -ENOMEM;
    }

    tsw_cmd = kmalloc(cmd_size, GFP_KERNEL);

    if(tsw_cmd == NULL)
    {
        return -ENOMEM;
    }

    // -- Prepare target command ---
    tsw_cmd->xCommand.ulMagic           = MIL_COM_MAGIC;
    tsw_cmd->xCommand.ulStream          = 0;
    tsw_cmd->xCommand.ulCommand         = MIL_COM_SYS_DMA_SETUP_SGL;
    tsw_cmd->xCommand.ulSize            = cmd_size;
    tsw_cmd->xCommand.ulExpectedAckSize = sizeof(TY_MIL_COM_ACK);

    tsw_cmd->ulOffset    = offset;         // Target side
    tsw_cmd->ulMemory    = memType;        // Target side
    tsw_cmd->ulCopySize  = buffer->length; // Target side
    tsw_cmd->ulDirection = 0;              /* 0=read; */

    tsw_cmd->ulSglEntries = buffer->scatterTable.nents;

    /* Set the scatter/gather entries of the command */
    for(i = 0; i < buffer->scatterTable.nents; i++)
    {
#if LINUX_VERSION_CODE < KERNEL_VERSION(2,6,25)
        p_CurrentScatterListEntry = &buffer->scatterTable.sgl[i];
#else
        if(p_CurrentScatterListEntry == NULL)
            p_CurrentScatterListEntry = buffer->scatterTable.sgl;
        else
            p_CurrentScatterListEntry = sg_next(p_CurrentScatterListEntry);
#endif

        DMAAddress = sg_dma_address(p_CurrentScatterListEntry);
        tsw_cmd->xEntry[i].ulPciAddressHigh = (DMAAddress & 0xFFFFFFFF00000000ULL) >> 32;
        tsw_cmd->xEntry[i].ulPciAddressLow  = DMAAddress & 0xFFFFFFFF;
        tsw_cmd->xEntry[i].ulLength         = sg_dma_len(p_CurrentScatterListEntry);
    }


    mutex_lock(&device->targetCmdLock);

    ret = aim_exec_asp_command_mil(mil_device, tsw_cmd, cmd_size, &tsw_ack, tsw_cmd->xCommand.ulExpectedAckSize );

    mutex_unlock(&device->targetCmdLock);

    if(ret != tsw_cmd->xCommand.ulExpectedAckSize )
    {
        aim_dev_error(device, "Execution of driver internal target command failed with %d\n", ret );
        ret = -EIO;
    }
    else if(tsw_ack.ulError != 0 )
    {
        aim_dev_error(device, "Execution of driver internal target command failed with NAK 0x%x\n\n", tsw_ack.ulError );
        ret =  -EIO;
    }
    else
    {
        ret = 0;
    }

    if(tsw_cmd != NULL)
        kfree(tsw_cmd);

    return ret;
}

