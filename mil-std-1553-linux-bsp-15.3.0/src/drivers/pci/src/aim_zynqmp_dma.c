/* SPDX-FileCopyrightText: 2018-2024 AIM GmbH <info@aim-online.com> */
/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <linux/delay.h>
#include <linux/types.h>
#include "aim_zynqmp_device.h"
#include "aim_pci_mem.h"
#include "Ai_ringbuffer.h"


static int aim_zynqmp_dma_allocate_buffer(struct aim_pci_device* aimDevice, void ** buffer, dma_addr_t * buffer_address, size_t size)
{
    *buffer = dma_alloc_coherent(&aimDevice->pciDevice->dev, size, buffer_address, GFP_KERNEL);

    if (*buffer == NULL)
    {
        aim_dev_error(aimDevice, "aim_zynqmp_dma_allocate_buffer : dma_alloc_coherent failed\n");
        return -ENOMEM;
    }

    if ((*buffer_address & 0x3F) != 0)
    {
        // Test if address is 64 bytes aligned
        aim_dev_error(aimDevice, "aim_zynqmp_dma_allocate_buffer : dma_alloc_coherent returned wrong address alignment\n");
        return -EFAULT;
    }

    memset(*buffer, 0, size);

    return 0;
}


int aim_zynqmp_dma_channel_allocate(struct aim_pci_device* aimDevice, AIMPCI_HW_ZYNQMP_DMA_CHANNEL * channel)
{
    int ret;

    channel->reg = (ZYNQMP_DMA_REGISTERS*)aimDevice->dmaMemory + AIMPCI_HW_ZYNQMP_DMA_CHANNEL0_OFFSET;

    if (channel->reg->DMA_CHANNEL_STATUS.BIT.DMA_RUNNING_BIT)
    {
        /* The DMA engine is still active in case of a warm boot. */
        aim_zynqmp_dma_channel_stop(channel);
    }

    channel->aimDevice = aimDevice;

    channel->src_sgl_buffer_size  = AIMPCI_HW_ZYNQMP_DMA_MAX_SGL_ELEMENTS * sizeof(ZYNQMP_DMA_SRC_QUEUE_ENTRY);
    channel->dst_sgl_buffer_size  = AIMPCI_HW_ZYNQMP_DMA_MAX_SGL_ELEMENTS * sizeof(ZYNQMP_DMA_DST_QUEUE_ENTRY);
    channel->stat_sgl_buffer_size = AIMPCI_HW_ZYNQMP_DMA_MAX_SGL_ELEMENTS * sizeof(ZYNQMP_DMA_STAT_QUEUE_ENTRY);
    channel->stat_sgl_buffer_size = AIMPCI_HW_ZYNQMP_DMA_MAX_SGL_ELEMENTS * sizeof(ZYNQMP_DMA_STAT_QUEUE_ENTRY);

    do
    {
        // -- Allocate source SGL array ---
        ret = aim_zynqmp_dma_allocate_buffer(aimDevice, &channel->src_sgl_buffer, &channel->src_sgl_buffer_address, channel->src_sgl_buffer_size );

        if (ret)
            break;

        // -- Allocate destination SGL array ---
        ret = aim_zynqmp_dma_allocate_buffer(aimDevice, &channel->dst_sgl_buffer, &channel->dst_sgl_buffer_address, channel->dst_sgl_buffer_size );

        if (ret)
            break;

        // -- Allocate source status SGL array ---
        ret = aim_zynqmp_dma_allocate_buffer(aimDevice, &channel->stas_sgl_buffer, &channel->stas_sgl_buffer_address, channel->stat_sgl_buffer_size );

        if (ret)
            break;

        // -- Allocate destination status SGL array ---
        ret = aim_zynqmp_dma_allocate_buffer(aimDevice, &channel->stad_sgl_buffer, &channel->stad_sgl_buffer_address, channel->stat_sgl_buffer_size );

        if (ret)
            break;

    } while(0);

    /* we need all or none buffers */
    if (ret)
    {
        aim_zynqmp_dma_free(channel);
    }

    return ret;
}


int aim_zynqmp_dma_channel_start(AIMPCI_HW_ZYNQMP_DMA_CHANNEL * channel)
{
    if (channel->reg->DMA_CHANNEL_STATUS.BIT.DMA_RUNNING_BIT)
        return -EAGAIN;


    channel->reg->SRC_Q_LOW.BIT.QUEUE_ENABLE   = 1;
    channel->reg->SRC_Q_LOW.BIT.QUEUE_LOCATION = AIMPCI_HW_ZYNQMP_DMA_LOCATION_PCI;
    channel->reg->SRC_Q_LOW.BIT.START_ADDR_LOW = ((channel->src_sgl_buffer_address    ) & 0xFFFFFFFFULL) >> 6; /* SGL Address must be 64byte aligned */
    channel->reg->SRC_Q_HI                     = ((channel->src_sgl_buffer_address>>32) & 0xFFFFFFFFULL);
    channel->reg->SRC_Q_SIZE                   = AIMPCI_HW_ZYNQMP_DMA_MAX_SGL_ELEMENTS;
    channel->reg->SRC_Q_LIMIT                  = 0; /* Indicates that the SW did not provide data yet */
    channel->reg->SRC_Q_NEXT                   = 0;

    channel->reg->DST_Q_LOW.BIT.QUEUE_ENABLE   = 1;
    channel->reg->DST_Q_LOW.BIT.QUEUE_LOCATION = AIMPCI_HW_ZYNQMP_DMA_LOCATION_PCI;
    channel->reg->DST_Q_LOW.BIT.START_ADDR_LOW = ((channel->dst_sgl_buffer_address    ) & 0xFFFFFFFFULL) >> 6; /* SGL Address must be 64byte aligned */
    channel->reg->DST_Q_HI                     = ((channel->dst_sgl_buffer_address>>32) & 0xFFFFFFFFULL);
    channel->reg->DST_Q_SIZE                   = AIMPCI_HW_ZYNQMP_DMA_MAX_SGL_ELEMENTS;
    channel->reg->DST_Q_LIMIT                  = 0; /* Indicates that the SW did not provide data yet */
    channel->reg->DST_Q_NEXT                   = 0;

    memset( channel->stas_sgl_buffer, 0, channel->stat_sgl_buffer_size );

    channel->reg->STAS_Q_LOW.BIT.QUEUE_ENABLE   = 1;
    channel->reg->STAS_Q_LOW.BIT.QUEUE_LOCATION = AIMPCI_HW_ZYNQMP_DMA_LOCATION_PCI;
    channel->reg->STAS_Q_LOW.BIT.START_ADDR_LOW = ((channel->stas_sgl_buffer_address    ) & 0xFFFFFFFFULL) >> 6; /* SGL Address must be 64byte aligned */
    channel->reg->STAS_Q_HI                     = ((channel->stas_sgl_buffer_address>>32) & 0xFFFFFFFFULL);
    channel->reg->STAS_Q_SIZE                   = AIMPCI_HW_ZYNQMP_DMA_MAX_SGL_ELEMENTS;
    channel->reg->STAS_Q_LIMIT                  = AIMPCI_HW_ZYNQMP_DMA_MAX_SGL_ELEMENTS-1; /* Indicates that no status is available yet */
    channel->reg->STAS_Q_NEXT                   = 0;

    memset( channel->stad_sgl_buffer, 0, channel->stat_sgl_buffer_size );

    channel->reg->STAD_Q_LOW.BIT.QUEUE_ENABLE    = 1;
    channel->reg->STAD_Q_LOW.BIT.QUEUE_LOCATION  = AIMPCI_HW_ZYNQMP_DMA_LOCATION_PCI;
    channel->reg->STAD_Q_LOW.BIT.START_ADDR_LOW = ((channel->stad_sgl_buffer_address    ) & 0xFFFFFFFFULL) >> 6; /* SGL Address must be 64byte aligned */
    channel->reg->STAD_Q_HI                     = ((channel->stad_sgl_buffer_address>>32) & 0xFFFFFFFFULL);
    channel->reg->STAD_Q_SIZE                   = AIMPCI_HW_ZYNQMP_DMA_MAX_SGL_ELEMENTS;
    channel->reg->STAD_Q_LIMIT                  = AIMPCI_HW_ZYNQMP_DMA_MAX_SGL_ELEMENTS-1; /* Indicates that no status is available yet */
    channel->reg->STAD_Q_NEXT                   = 0;



    channel->reg->DMA_CHANNEL_CTRL.BIT.DMA_ENABLE = 1; /* Enable DMA, as last step */
    return 0;
}

int aim_zynqmp_dma_channel_stop(AIMPCI_HW_ZYNQMP_DMA_CHANNEL * channel)
{
    if (!channel)
        return 0;

    channel->reg->DMA_CHANNEL_CTRL.BIT.DMA_ENABLE = 0;

    udelay(500);

    if (channel->reg->DMA_CHANNEL_STATUS.BIT.DMA_RUNNING_BIT)
    {
        /* DMA not stopped, force reset */
        channel->reg->DMA_CHANNEL_CTRL.BIT.DMA_RESET = 1;
        udelay(10 /*us*/); /* Reset bit must be set for at least 256 ns, */
        channel->reg->DMA_CHANNEL_CTRL.BIT.DMA_RESET = 0;
    }

    return 0;
}

int aim_zynqmp_dma_free(AIMPCI_HW_ZYNQMP_DMA_CHANNEL * channel)
{
    if (!channel)
        return 0;

    if (channel->src_sgl_buffer)
    {
        dma_free_coherent(&channel->aimDevice->pciDevice->dev, channel->src_sgl_buffer_size, channel->src_sgl_buffer, channel->src_sgl_buffer_address);

        channel->src_sgl_buffer         = NULL;
        channel->src_sgl_buffer_size    = 0;
        channel->src_sgl_buffer_address = 0;
    }

    if (channel->dst_sgl_buffer)
    {
        dma_free_coherent(&channel->aimDevice->pciDevice->dev, channel->dst_sgl_buffer_size, channel->dst_sgl_buffer, channel->dst_sgl_buffer_address);

        channel->dst_sgl_buffer         = NULL;
        channel->dst_sgl_buffer_size    = 0;
        channel->dst_sgl_buffer_address = 0;
    }

    if (channel->stas_sgl_buffer)
    {
        dma_free_coherent(&channel->aimDevice->pciDevice->dev, channel->stat_sgl_buffer_size, channel->stas_sgl_buffer, channel->stas_sgl_buffer_address);

        channel->stas_sgl_buffer         = NULL;
        channel->stas_sgl_buffer_address = 0;
    }

    if (channel->stad_sgl_buffer)
    {
        dma_free_coherent(&channel->aimDevice->pciDevice->dev, channel->stat_sgl_buffer_size, channel->stad_sgl_buffer, channel->stad_sgl_buffer_address);

        channel->stad_sgl_buffer         = NULL;
        channel->stat_sgl_buffer_size    = 0;
        channel->stad_sgl_buffer_address = 0;
    }

    return 0;
}

int aim_zynqmp_dma_channel_feed(AIMPCI_HW_ZYNQMP_DMA_CHANNEL * channel, struct aim_pci_device* device, struct aim_dma_buffer* buffer, u64 ullTargetAddress, u32 ulSize )
{
    u32 i = 0;
    ZYNQMP_DMA_SRC_QUEUE_ENTRY * src_sgl = (ZYNQMP_DMA_SRC_QUEUE_ENTRY *)channel->src_sgl_buffer;
    ZYNQMP_DMA_DST_QUEUE_ENTRY * dst_sgl = (ZYNQMP_DMA_DST_QUEUE_ENTRY *)channel->dst_sgl_buffer;
    struct scatterlist* scatterlistEntry = NULL;


    aim_dev_debug(aimDevice, "aim_zynqmp_dma_channel_feed : Setup DMA engine\n");

    if(buffer->scatterTable.nents > AIMPCI_HW_ZYNQMP_DMA_MAX_SGL_ELEMENTS )
    {
        aim_dev_error(channel->aimDevice, "aim_zynqmp_dma_channel_feed : to many SGL entries (%d)\n", buffer->scatterTable.nents);
        return -E2BIG;
    }

    /* The ZynqMP DMA engine supports adding elements to the SGL during runtime of the DMA.
       For simplicity we wo however stop and restart the DMA engine before we feed the new SGL 
       elements into the HW.

       This is why we start at index 0.
    */

    /* -- copy Linux SGL to ZynqMP SGL --- */
    for_each_sg(buffer->scatterTable.sgl, scatterlistEntry, buffer->scatterTable.nents, i)
    {
        dst_sgl[i].address                                     = sg_dma_address(scatterlistEntry);
        dst_sgl[i].control_byte_count.bit.byte_count           = sg_dma_len(scatterlistEntry);
        dst_sgl[i].control_byte_count.bit.location             = AIMPCI_HW_ZYNQMP_DMA_LOCATION_PCI;
        dst_sgl[i].control_byte_count.bit.dma_write_attributes = AIMPCI_HW_ZYNQMP_DMA_PCIE_ATTRIBUTE;
    }

    /* -- Prepare device SGL (Single entry) --- */
    src_sgl[0].address                                    = ullTargetAddress;
    src_sgl[0].control_byte_count.bit.byte_count          = ulSize;
    src_sgl[0].control_byte_count.bit.location            = AIMPCI_HW_ZYNQMP_DMA_LOCATION_AXI;
    src_sgl[0].control_byte_count.bit.eop                 = 1;
    src_sgl[0].control_byte_count.bit.interrupt           = 1;
    src_sgl[0].control_byte_count.bit.dma_read_attributes = AIMPCI_HW_ZYNQMP_DMA_AXI_ATTRIBUTE;


    /* Signal available SGL elements to DMA */
    channel->reg->DST_Q_LIMIT = buffer->scatterTable.nents;
    channel->reg->SRC_Q_LIMIT = 1;

    return 0;
}

int aim_zynqmp_start_dma_read_generic(struct aim_pci_device* aimDevice, struct aim_dma_buffer* buffer, enum aim_memory_type memType, loff_t offset)
{
    int      ret;
    u64      ullTargetAddress         = 0;

    AIMPCI_HW_ZYNQMP_DMA_CHANNEL * channel;

    channel = radix_tree_lookup(&aimDevice->pciDmaChannels, AIM_DMA_CHANNEL_READ_BLOCK_MEM_DATA);

    BUG_ON(channel == NULL);

    aim_dev_debug(channel->aimDevice, "aim_zynqmp_start_dma_read_generic : start dma read\n");

    if (   (memType != AIM_MEMTYPE_GLOBAL)
        && (memType != AIM_MEMTYPE_SHARED) )
    {
        aim_dev_error(aimDevice, "aim_zynqmp_start_dma_read_generic : invalid memory type (%d)\n", memType);

        return -EIO;
    }

    BUG_ON(aimDevice->deviceData->dmaOps == NULL);
    BUG_ON(aimDevice->deviceData->dmaOps->get_dma_device_offset == NULL);

    ret = aimDevice->deviceData->dmaOps->get_dma_device_offset(aimDevice, memType, &ullTargetAddress);

    if (ret)
    {
        aim_dev_error(aimDevice, "aim_zynqmp_start_dma_read_generic : could not obtain offset\n");
        return ret;
    }

    ret = aim_zynqmp_dma_channel_stop(channel);

    if (ret)
    {
        aim_dev_error(aimDevice, "aim_zynqmp_start_dma_read_generic : invalid memory type (%d)\n", memType);
        return ret;
    }

    ret = aim_zynqmp_dma_channel_start(channel);

    if (ret)
    {
        aim_dev_error(channel->aimDevice, "aim_zynqmp_start_dma_read_generic : failed to start DMA channel\n");
        return ret;
    }

    /* -- calculate base offset for this DMA transfer --- */

    ullTargetAddress += offset;

    ret = aim_zynqmp_dma_channel_feed(channel, aimDevice, buffer, ullTargetAddress, buffer->length );

    if (ret)
    {
        aim_dev_error(aimDevice, "aim_zynqmp_start_dma_read_generic : failed to feed DMA channel\n");
        return ret;
    }

    return 0;
}


