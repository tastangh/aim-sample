/* SPDX-FileCopyrightText: 2016-2021 AIM GmbH <info@aim-online.com> */
/* SPDX-License-Identifier: GPL-2.0-or-later */

/*! \file aim_irq_mil.c
 *
 *  This file contains 1553 specific definitions
 *  used for interrupt handling
 *
 */


#include "aim_irq_mil.h"
#include "aim_pci_device_mil.h"
#include "api_prot.h"
#include "api_defs.h"
#include "api_defv.h"
#include "api_int_loglist.h"
#include "aim_nl_mil.h"
#include "aim_nl_interface_mil.h"

#include "aim_dma.h"


static bool aim_pci_mil_get_loglist_entry(struct aim_pci_mil_device* device, struct ty_api_intr_loglist_entry * entry)
{
    __u32 getCount = 0;
    __u32 putCount = 0;

    BUG_ON(!device || !device->interruptLoglist);

    aim_dev_debug(mil_to_aim(device), "getting interrupt loglist entry\n");

    getCount = ioread32(&device->interruptLoglist->get_cnt);
    putCount = ioread32(&device->interruptLoglist->put_cnt);

    BUG_ON(getCount > MAX_API_IR_EVENTS);
    BUG_ON(putCount > MAX_API_IR_EVENTS);

    aim_dev_debug(mil_to_aim(device), "Interrupt log list put %d, get %d\n", putCount, getCount);

    if(getCount != putCount)
    {
        entry->lla        = ioread32(&device->interruptLoglist->entry[getCount].lla);
        entry->llb        = ioread32(&device->interruptLoglist->entry[getCount].llb);
        entry->llc.ul_All = ioread32(&device->interruptLoglist->entry[getCount].llc);
        entry->lld.ul_All = ioread32(&device->interruptLoglist->entry[getCount].lld);

        API_INC_LOLI_CNT(getCount);
        iowrite32(getCount, &device->interruptLoglist->get_cnt);
        return true;
    }

    return false;
}




void aim_pci_mil_biu_irq(struct aim_pci_device* device, int biuID)
{
    struct aim_pci_mil_device* milDevice = NULL;
    struct ty_api_intr_loglist_entry loglistEntry = {0};

    BUG_ON(!device);

    milDevice = aim_dev_to_mil(device);

    spin_lock_bh(&milDevice->irq_loglist_lock);

    api_ir(milDevice->tswData, biuID);

    while(aim_pci_mil_get_loglist_entry(milDevice, &loglistEntry))
    {
        aim_dev_debug(mil_to_aim(milDevice), "Got loglist entry");


        /* Forward interrupt event to user space, using netlink multicast */
        if(aim_nl_mc_send_sba(device, AIM_MIL_NL_GRP_ID_IRQ_EVENT, AIM_MIL_NL_OP_LOGLIST_EVENT,
                              AIM_MIL_NL_A_LOGLIST_ENTRY, sizeof(loglistEntry), &loglistEntry))
        {
            aim_dev_error(device, "Failed to send interrupt loglist entry");
        }
    }

    spin_unlock_bh(&milDevice->irq_loglist_lock);
}


void aim_pci_mil_asp_irq(struct aim_pci_device* device)
{
    int dma_done = 0;
    struct aim_pci_mil_device* milDevice = NULL;
    struct ty_api_intr_loglist_entry loglistEntry = {0};

    BUG_ON(!device);

    milDevice = aim_dev_to_mil(device);

    spin_lock_bh(&milDevice->irq_loglist_lock);

    while(aim_pci_mil_get_loglist_entry(milDevice, &loglistEntry))
    {
        aim_dev_debug(mil_to_aim(milDevice), "Got loglist entry");

        if(loglistEntry.llc.b.uc_Dma)
        {
            aim_dev_debug(mil_to_aim(milDevice), "DMA Interrupt");
            dma_done = 1;
            continue;
        }

        /* Forward interrupt event to user space, using netlink multicast */
        if(aim_nl_mc_send_sba(device, AIM_MIL_NL_GRP_ID_IRQ_EVENT, AIM_MIL_NL_OP_LOGLIST_EVENT,
                              AIM_MIL_NL_A_LOGLIST_ENTRY, sizeof(loglistEntry), &loglistEntry))
        {
            aim_dev_error(device, "Failed to send interrupt loglist entry");
        }
    }

    spin_unlock_bh(&milDevice->irq_loglist_lock);

    if(dma_done)
    {
        aim_pci_device_dma_complete(device);
    }
}
