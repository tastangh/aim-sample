/* SPDX-FileCopyrightText: 2016-2021 AIM GmbH <info@aim-online.com> */
/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <linux/scatterlist.h>

#include "aim_pci_module.h"
#include "aim_pci_device_mil.h"
#include "aim_rw.h"
#include "aim_dma.h"

#include "aim_rw_interface.h"

int aim_pci_mil_ayx_device_start_dma_read(struct aim_pci_device* device, struct aim_dma_buffer* buffer, enum aim_memory_type memType, loff_t offset);
