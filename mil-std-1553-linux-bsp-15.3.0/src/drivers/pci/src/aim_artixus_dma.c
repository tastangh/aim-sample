/* SPDX-FileCopyrightText: 2024-2024 AIM GmbH <info@aim-online.com> */
/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <linux/delay.h>
#include <linux/types.h>
#include "aim_artixus_device.h"
#include "aim_pci_mem.h"
#include "Ai_ringbuffer.h"

#include "hw/AiHwArtixUS.h"

static int aim_artixus_dma_allocate_buffer(struct aim_pci_device* aimDevice, void ** buffer, dma_addr_t * buffer_address, size_t size)
{
    *buffer = dma_alloc_coherent(&aimDevice->pciDevice->dev, size, buffer_address, GFP_KERNEL);

    if (*buffer == NULL)
    {
        aim_dev_error(aimDevice, "aim_artixus_dma_allocate_buffer : dma_alloc_coherent failed\n");
        return -ENOMEM;
    }

    if ((*buffer_address & 0x3) != 0)
    {
        // Test if address is 4 bytes aligned
        aim_dev_error(aimDevice, "aim_artixus_dma_allocate_buffer : dma_alloc_coherent returned wrong address alignment\n");
        return -EFAULT;
    }

    memset(*buffer, 0, size);

    return 0;
}


int aim_artixus_dma_channel_allocate(struct aim_pci_device* aimDevice, AIMPCI_HW_ARTIXUS_DMA_CHANNEL * channel)
{
    int ret;

    channel->aimDevice = aimDevice;
    channel->sgl_buffer_size  = AIMPCI_HW_ARTIXUS_DMA_MAX_SGL_ELEMENTS * sizeof(ARTIXUS_DMA_SGL_ENTRY);

    do
    {
        // -- Allocate source SGL array ---
        ret = aim_artixus_dma_allocate_buffer(aimDevice, &channel->sgl_buffer, &channel->sgl_buffer_address, channel->sgl_buffer_size );

        if (ret)
            break;

    } while(0);

    /* we need all or none buffers */
    if (ret)
    {
        aim_artixus_dma_free(channel);
    }

    return ret;
}


int aim_artixus_dma_free(AIMPCI_HW_ARTIXUS_DMA_CHANNEL * channel)
{
    if (!channel)
        return 0;

    if (channel->sgl_buffer)
    {
        dma_free_coherent(&channel->aimDevice->pciDevice->dev, channel->sgl_buffer_size, channel->sgl_buffer, channel->sgl_buffer_address);

        channel->sgl_buffer         = NULL;
        channel->sgl_buffer_size    = 0;
        channel->sgl_buffer_address = 0;
    }


    return 0;
}

int aim_artixus_dma_channel_feed(AIMPCI_HW_ARTIXUS_DMA_CHANNEL * channel, struct aim_pci_device* device, struct aim_dma_buffer* buffer, u32 ulTargetAddress, u32 ulSize )
{
    u32 i = 0;
    ARTIXUS_DMA_SGL_ENTRY * sgl_buffer = (ARTIXUS_DMA_SGL_ENTRY*)channel->sgl_buffer;
    struct scatterlist* scatterlistEntry = NULL;


    aim_dev_debug(aimDevice, "aim_artixus_dma_channel_feed : Setup DMA engine\n");

    if(buffer->scatterTable.nents > AIMPCI_HW_ARTIXUS_DMA_MAX_SGL_ELEMENTS )
    {
        aim_dev_error(channel->aimDevice, "aim_artixus_dma_channel_feed : to many SGL entries (%d)\n", buffer->scatterTable.nents);
        return -E2BIG;
    }

    if(buffer->scatterTable.nents == 0 )
    {
        aim_dev_error(channel->aimDevice, "aim_artixus_dma_channel_feed : not enough SGL entries (%d)\n", buffer->scatterTable.nents);
        return -E2BIG;
    }

    /* -- copy Linux SGL to ArtixUS SGL --- */
    for_each_sg(buffer->scatterTable.sgl, scatterlistEntry, buffer->scatterTable.nents, i)
    {
        // Clear SGL entry
        memset(&sgl_buffer[i], 0, sizeof(ARTIXUS_DMA_SGL_ENTRY));

        sgl_buffer[i].control.bit.size      = sg_dma_len(scatterlistEntry);
        sgl_buffer[i].control.bit.direction = ARTIXUS_DMA_CONFIG_DIRECTION_GLOBAL;

        if (i == (buffer->scatterTable.nents - 1))
        {
            sgl_buffer[i].next_sgl_address = 0;
            sgl_buffer[i].control.bit.last_entry = 1;
            sgl_buffer[i].control.bit.irq = 1;
        }
        else
        {
            sgl_buffer[i].next_sgl_address = channel->sgl_buffer_address + sizeof(ARTIXUS_DMA_SGL_ENTRY) * (i + 1);
            sgl_buffer[i].control.bit.last_entry = 0;
            sgl_buffer[i].control.bit.irq = 0;
        }

        sgl_buffer[i].device_address = ulTargetAddress;
        sgl_buffer[i].host_address = sg_dma_address(scatterlistEntry);

        /* -- calculate next device offset --- */
        ulTargetAddress += sg_dma_len(scatterlistEntry);
    }

    /* Copy SGL physical address to DMA channel FIFO. Low address part first then high part. */
    aim_pci_io_write32( device, ARTIXUS_DMA_C2H_CH1_SGL_START_ADDRESS*4, channel->sgl_buffer_address & 0xFFFFFFFF );
    wmb();
    aim_pci_io_write32( device, ARTIXUS_DMA_C2H_CH1_SGL_START_ADDRESS*4, (channel->sgl_buffer_address & 0xFFFFFFFF00000000ULL) >> 32);
    wmb();
    return 0;
}

int aim_artixus_start_dma_read_generic(struct aim_pci_device* aimDevice, struct aim_dma_buffer* buffer, enum aim_memory_type memType, loff_t offset)
{
    int      ret;
    u32      ulTargetAddress  = 0;
    u32      ulRegisterValue  = 0;

    AIMPCI_HW_ARTIXUS_DMA_CHANNEL * channel;

    channel = radix_tree_lookup(&aimDevice->pciDmaChannels, AIM_DMA_CHANNEL_READ_BLOCK_MEM_DATA);

    BUG_ON(channel == NULL);

    aim_dev_debug(channel->aimDevice, "aim_artixus_start_dma_read_generic : start dma read\n");

    if ( memType != AIM_MEMTYPE_GLOBAL )
    {
        aim_dev_error(aimDevice, "aim_artixus_start_dma_read_generic : invalid memory type (%d)\n", memType);

        return -EIO;
    }

    BUG_ON(aimDevice->deviceData->dmaOps == NULL);

    /* -- Abort if DMA is not supported --- */
    ulRegisterValue = aim_pci_io_read32(aimDevice, ARTIXUS_DMA_CONFIG* sizeof(__u32));

    if (((ulRegisterValue == 0xAFFECAFE)) || ((ulRegisterValue & ARTIXUS_DMA_CONFIG_C2H_MASK) < 1))
    {
        aim_dev_error(aimDevice, "aim_artixus_start_dma_read_generic : No C2H DMA channel found on card (0x%08X)\n", ulRegisterValue);
        return -EIO;
    }

    /* -- Abort if FIFO is full --- */
    ulRegisterValue = aim_pci_io_read32(aimDevice, ARTIXUS_DMA_C2H_CH1_SGL_FIFO_STATUS* sizeof(__u32));

    if ( (ulRegisterValue & ARTIXUS_DMA_SGL_FIFO_MAX_MASK) >= ARTIXUS_DMA_SGL_FIFO_MAX_ENTRIES)
    {
        aim_dev_error(aimDevice, "aim_artixus_start_dma_read_generic : Fifo full (%d)\n", ulRegisterValue);
        return -EIO;
    }

    /* -- calculate base offset for this DMA transfer --- */

    ulTargetAddress += (u32)offset;

    ret = aim_artixus_dma_channel_feed(channel, aimDevice, buffer, ulTargetAddress, buffer->length );

    if (ret)
    {
        aim_dev_error(aimDevice, "aim_artixus_start_dma_read_generic : failed to feed DMA channel\n");
        return ret;
    }

    return 0;
}


