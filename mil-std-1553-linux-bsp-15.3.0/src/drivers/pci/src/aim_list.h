/* SPDX-FileCopyrightText: 2017-2021 AIM GmbH <info@aim-online.com> */
/* SPDX-License-Identifier: GPL-2.0-or-later */

/*! \file aim_list.h
 *
 *  Header file that encapsulates the Linux kernel
 *  list implementation to make it usable with different kernel versions
 */

#ifndef AIM_LIST_H_
#define AIM_LIST_H_


#include <linux/version.h>
#include <linux/list.h>


#if LINUX_VERSION_CODE < KERNEL_VERSION(2,6,26)

static inline int list_is_singular(const struct list_head *head)
{
    return !list_empty(head) && (head->next == head->prev);
}

#endif



#if LINUX_VERSION_CODE < KERNEL_VERSION(2,6,27)


static inline void __list_cut_position(struct list_head *list,
        struct list_head *head, struct list_head *entry)
{
    struct list_head *new_first = entry->next;
    list->next = head->next;
    list->next->prev = list;
    list->prev = entry;
    entry->next = list;
    head->next = new_first;
    new_first->prev = head;
}


static inline void list_cut_position(struct list_head *list,
        struct list_head *head, struct list_head *entry)
{
    if (list_empty(head))
        return;
    if (list_is_singular(head) &&
        (head->next != entry && head != entry))
        return;
    if (entry == head)
        INIT_LIST_HEAD(list);
    else
        __list_cut_position(list, head, entry);
}

#endif




#endif /* AIM_LIST_H_ */
