/* SPDX-FileCopyrightText: 2018-2022 AIM GmbH <info@aim-online.com> */
/* SPDX-License-Identifier: GPL-2.0-or-later */

/*! \file mil_linux_module.h 

Ubuntu 16.04.4 and above have the Kernel config option CONFIG_THIN_ARCHIVES enabled.

This compiles all build in modules as thin archives instead of plain objects.
Using thin archives as shipped objects does not work.

Changing the TSW Makefile to produce a Module instead of build in helps
to work around this issue.

For our embedded platforms we still use build in in combination with thin
archives disabled.

Use this file to add module related stuff to statisfy any compiler errors.

Please note that we do not use the module. We only use the object file as o_shipped.

******************************************************************************/
#include <linux/module.h>


MODULE_AUTHOR("AIM GmbH");
MODULE_LICENSE("GPL");

