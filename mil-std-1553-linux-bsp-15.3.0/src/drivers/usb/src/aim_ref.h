/* SPDX-FileCopyrightText: 2020-2021 AIM GmbH <info@aim-online.com> */
/* SPDX-License-Identifier: GPL-2.0-or-later */

/*! \file aim_ref.h
 *
 *  This file contains the protocol independent declarations
 *  reference counting kref_xxx replacement due to the kref_read implementation 
 *  is not available on each kernel source
 * 
 */

#ifndef AIM_REF_H_
#define AIM_REF_H_

/**
 * aim_ref_init - set refcount to 1
 */
static inline void aim_ref_init(atomic_t* refs)
{
    atomic_set(refs,1);
}
/**
 * aim_ref_get - increment refcount for object.
 */
static inline void aim_ref_get(atomic_t* refs)
{
    BUG_ON(atomic_inc_return(refs)<2);
}
/**
 * aim_ref_put - decrement refcount for object and call release function if reached zero
 * Return 1 if the object was removed, otherwise return 0.
 */
static inline int aim_ref_put(atomic_t* refs,void (*release)(atomic_t* refs))
{
    BUG_ON(release == NULL);

    if (atomic_sub_and_test(1, refs)) {
        release(refs);
        return 1;
    }
    return 0;
}
/**
 * aim_ref_read - get a refcount's value
 * @r: the refcount
 *
 * Return: the refcount's value
 */
static inline unsigned int aim_ref_read(atomic_t* refs)
{
    return atomic_read(refs);
}

#endif /* AIM_REF_H_ */
