/* SPDX-FileCopyrightText: 2017-2021 AIM GmbH <info@aim-online.com> */
/* SPDX-License-Identifier: GPL-2.0-or-later */

/*! \file aim_usb_hw.c
 *
 */


#include "aim_usb_hw.h"
#include "aim_usb_ays.h"
#include "aim_usb_apu.h"




int aim_usb_hw_init(struct aim_usb_interface* aim_intf)
{
    aim_usb_intf_dbg(aim_intf, "Initializing Hardware");

    switch(aim_intf->platform)
    {
        case AI_DEVICE_AYS_ASP:
        case AI_DEVICE_ZYNQMP_ASP:
            return aim_usb_ays_hw_init(aim_intf);

        case AI_DEVICE_USB:
            return aim_usb_apu_hw_init(aim_intf);

        default:
            aim_usb_intf_err(aim_intf, "Unsupported platform");
            return -EINVAL;
    }
}


int aim_usb_hw_start(struct aim_usb_interface* aim_intf)
{
    int err;

    aim_usb_intf_dbg(aim_intf, "Starting Hardware");

    err = 0;
    do{
        if(aim_intf->interrupt_ep)
        {
            err = aim_usb_int_ep_start(aim_intf->interrupt_ep);
            if(err)
            {
                aim_usb_intf_err(aim_intf, "Failed to start interrupt endpoint");
                break;
            }
        }

        switch(aim_intf->platform)
        {
            case AI_DEVICE_USB:
                err = aim_usb_apu_hw_start(aim_intf);
                break;

            case AI_DEVICE_AYS_ASP:
            case AI_DEVICE_ZYNQMP_ASP:
                err = aim_usb_ays_hw_start(aim_intf);
                break;

            default:
                break;
        }

        if(err)
        {
            break;
        }

    }while(0);

    return err;
}


void aim_usb_hw_stop(struct aim_usb_interface* aim_intf)
{
    aim_usb_intf_dbg(aim_intf, "Stopping Hardware");

    /* Call platform specific stop function here */
    switch(aim_intf->platform)
    {
        case AI_DEVICE_USB:
            aim_usb_apu_hw_stop(aim_intf);
            break;

        default:
            break;
    }

    if(aim_intf->interrupt_ep)
    {
        aim_usb_int_ep_stop(aim_intf->interrupt_ep);
    }
}


int aim_usb_hw_read(struct aim_usb_interface* aim_intf, void* buffer, size_t len, TY_E_MEM_TYPE mem_type, loff_t offset)
{
    BUG_ON(!aim_intf || !buffer);

    if(!len)
    {
        return 0;
    }

    switch(aim_intf->platform)
    {
        case AI_DEVICE_AYS_ASP:
        case AI_DEVICE_ZYNQMP_ASP:
            return aim_usb_ays_hw_read(aim_intf, buffer, len, mem_type, offset);

        case AI_DEVICE_USB:
            return aim_usb_apu_hw_read(aim_intf, buffer, len, mem_type, offset);

        default:
            aim_usb_intf_err(aim_intf, "Platform does not support raw memory read");
            return -EOPNOTSUPP;
    }
}


int aim_usb_hw_write(struct aim_usb_interface* aim_intf, const void* buffer, size_t len, TY_E_MEM_TYPE mem_type,
                     loff_t offset)
{
    BUG_ON(!aim_intf || !buffer);

    if(!len)
    {
        return 0;
    }

    switch(aim_intf->platform)
    {
        case AI_DEVICE_AYS_ASP:
        case AI_DEVICE_ZYNQMP_ASP:
            return aim_usb_ays_hw_write(aim_intf, buffer, len, mem_type, offset);

        case AI_DEVICE_USB:
            return aim_usb_apu_hw_write(aim_intf, buffer, len, mem_type, offset);

        default:
            aim_usb_intf_err(aim_intf, "Platform does not support raw memory write");
            return -EOPNOTSUPP;
    }
}


ssize_t aim_usb_hw_com_channel_cmd(struct aim_usb_com_channel* channel, const void* command, size_t cmd_len,
                                   void* response, size_t resp_len)
{
    BUG_ON(!channel || !channel->intf || !command);

    switch(channel->intf->platform)
    {
        case AI_DEVICE_AYS_ASP:
        case AI_DEVICE_ZYNQMP_ASP:
            return aim_usb_ays_com_channel_cmd(channel, command, cmd_len, response, resp_len);
        default:
            return -EOPNOTSUPP;
    }
}


int aim_usb_hw_novram_get(struct aim_usb_interface* intf, AiUInt32 address, AiUInt32* value)
{
    BUG_ON(!intf || !value);

    switch(intf->platform)
    {
        case AI_DEVICE_USB:
            return aim_usb_apu_novram_get(intf, address, value);

        case AI_DEVICE_AYS_ASP:
        case AI_DEVICE_ZYNQMP_ASP:
            return aim_usb_ays_novram_get(intf, address, value);

        default:
            return -EOPNOTSUPP;
    }
}


int aim_usb_hw_novram_set(struct aim_usb_interface* intf, AiUInt32 address, AiUInt32 value)
{
    BUG_ON(!intf);

    switch(intf->platform)
    {
        case AI_DEVICE_USB:
            return aim_usb_apu_novram_set(intf, address, value);

        default:
            return -EOPNOTSUPP;
    }
}


int aim_usb_hw_tcp_reg_read(struct aim_usb_interface* intf, AiUInt32 address, AiUInt32* value)
{
    BUG_ON(!intf);

    switch( intf->platform )
    {
        case AI_DEVICE_USB:
            return aim_usb_apu_tcp_reg_read(intf, address, value);

        default:
            return -EOPNOTSUPP;
    }
}


int aim_usb_hw_tcp_reg_write(struct aim_usb_interface* intf, AiUInt32 address, AiUInt32 value)
{
    BUG_ON(!intf);

    switch( intf->platform )
    {
        case AI_DEVICE_USB:
            return aim_usb_apu_tcp_reg_write(intf, address, value);

        default:
            return -EOPNOTSUPP;
    }
}

