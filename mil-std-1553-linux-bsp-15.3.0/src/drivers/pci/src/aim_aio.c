/* SPDX-FileCopyrightText: 2013-2021 AIM GmbH <info@aim-online.com> */
/* SPDX-License-Identifier: GPL-2.0-or-later */

/*! \file aim_aio.c
 *
 *  This file contains definitions for
 *  protocol generic asynchronous IO functionality
 *
 */


#include <linux/slab.h>
#include <linux/random.h>
#include "aim_aio.h"
#include "aim_debug.h"
#include "aim_dma.h"




/*! \brief Destroys an asynchronous IO queue
 *
 * @param refcount The reference counter of the queue to destroy.
 */
static void destroy_queue(struct kref* refcount)
{
    struct aim_aio_queue* queue;
    struct aim_aio_request* current_req;
    struct aim_aio_request* next_req;
    LIST_HEAD(cancel_list);
    int ret;

    BUG_ON(!refcount);

    queue = container_of(refcount, struct aim_aio_queue, refcount);

    aim_debug("Freeing AIO queue %d", queue->id);


    spin_lock_bh(&queue->lock);

    /* First remove all completed request */
    list_for_each_entry_safe(current_req, next_req, &queue->completed, list_anchor)
    {
        list_del(&current_req->list_anchor);

        aim_aio_request_put(current_req);
    }

    /* Move all pending requests from the pending list to
     * another temporary list. This allows to cancel the requests without
     * holding the queue lock
     */
    list_cut_position(&cancel_list, &queue->pending, queue->pending.prev);

    BUG_ON(!list_empty(&queue->pending));

    spin_unlock_bh(&queue->lock);

    /* Now cancel all requests in temporary list */
    list_for_each_entry_safe(current_req, next_req, &cancel_list, list_anchor)
    {
        ret = aim_aio_request_cancel(current_req);
        if(ret)
        {
            aim_dev_warning(current_req->device, "Couldn't cancel aio request %llu", current_req->id);
        }

        aim_aio_request_put(current_req);
    }


    kfree(queue);
}


/*! \brief Destroys an asynchronous IO request
 *
 * @param refcount The reference counter of the request to destroy
 */
static void destroy_request(struct kref* refcount)
{
    struct aim_aio_request* req;

    BUG_ON(!refcount);

    req = container_of(refcount, struct aim_aio_request, ref_count);

    aim_dev_debug(req->device, "Freeing AIO request %llu", req->id);

    if(req->dma_buffer)
    {
        aim_dma_buffer_free(req->dma_buffer, &req->device->pciDevice->dev,
                            req->state == AIO_OP_COMPLETED ? true : false);
        kfree(req->dma_buffer);
    }

    kfree(req);
}


/*! \brief get next pending request from asynchronous IO queue
 *
 * The function will not lock the queue, this is in the caller's responsibility.
 * It will grab a reference of the request. So the caller has to release it if he does not use it any more
 * @param queue the queue to get next pending request from
 * @return pointer to next pending request on success. NULL if there is no pending request
 */
static struct aim_aio_request __inline__ *get_next_pending(struct aim_aio_queue* queue)
{
    struct aim_aio_request* req;

    if(list_empty(&queue->pending))
    {
        return NULL;
    }

    req = list_first_entry(&queue->pending, struct aim_aio_request, list_anchor);


    aim_aio_request_get(req);

    return req;
}


int aim_aio_queue_init(struct aim_aio_queue* queue, int id, struct aim_pci_device* device)
{
    BUG_ON(!queue || !device);

    queue->id = id;
    spin_lock_init(&queue->lock);
    kref_init(&queue->refcount);
    INIT_LIST_HEAD(&queue->pending);
    INIT_LIST_HEAD(&queue->completed);
    init_waitqueue_head(&queue->wait_queue);

    return 0;
}


void aim_aio_queue_get(struct aim_aio_queue* queue)
{
    BUG_ON(!queue);

    aim_debug("Taking hold of queue %d", queue->id);

    kref_get(&queue->refcount);
}


void aim_aio_queue_put(struct aim_aio_queue* queue)
{
    BUG_ON(!queue);

    aim_debug("Releasing AIO queue %d", queue->id);

    kref_put(&queue->refcount, destroy_queue);
}


void aim_aio_queue_assign_fhandle(struct aim_aio_queue* queue, struct file* handle)
{
    BUG_ON(!queue);

    aim_debug("Assigning file handle %p to AIO queue %d", handle, queue->id);

    queue->f_handle = handle;
}


struct file* aim_aio_queue_get_fhandle(struct aim_aio_queue* queue)
{
    BUG_ON(!queue);

    return queue->f_handle;
}


int aim_aio_queue_submit(struct aim_aio_queue* queue, struct aim_aio_request* req)
{
    BUG_ON(!queue || !req);

    aim_dev_debug(req->device, "Submitting request %llu to AIO queue %d", req->id, queue->id);

    aim_aio_request_get(req);

    spin_lock_bh(&queue->lock);

    list_add_tail(&req->list_anchor, &queue->pending);

    spin_unlock_bh(&queue->lock);

    return aim_aio_request_start(req);
}


int aim_aio_queue_pop_completed(struct aim_aio_queue* queue, struct aim_aio_request** requests, int max_requests,
                                int timeout_ms)
{
    int num_completed;
    struct aim_aio_request* current_req;
    struct aim_aio_request* next_req;
    int ret;

    BUG_ON(!queue || !requests);

    aim_debug("Popping %d completed requests from queue %d with timeout %d", max_requests, queue->id, timeout_ms);

    ret = wait_event_interruptible_timeout(queue->wait_queue, !list_empty(&queue->completed),
                                           msecs_to_jiffies(timeout_ms));
    if(ret > 0)
    {
        aim_debug("aio request on queue %d completed", queue->id);
    }
    else if(ret == 0)
    {
        aim_debug("aio completion wait on queue %d timed out", queue->id);
        return -ETIME;
    }
    else
    {
        aim_debug("aio completion wait on queue %d failed with %d", queue->id, ret);
        return ret;
    }

    spin_lock_bh(&queue->lock);

    /* Iterate over all completed requests */
    num_completed = 0;
    list_for_each_entry_safe(current_req, next_req, &queue->completed, list_anchor)
    {
        if(num_completed >= max_requests)
        {
            break;
        }

        requests[num_completed] = current_req;
        list_del(&current_req->list_anchor);
        num_completed++;
    }

    spin_unlock_bh(&queue->lock);

    return num_completed;
}


int aim_aio_queue_cancel_request(struct aim_aio_queue* queue, __u64 request_id)
{
    struct aim_aio_request* current_req;
    struct aim_aio_request* next_req;
    struct aim_aio_request* cancel_req = NULL;
    int ret;

    BUG_ON(!queue);

    aim_debug("Canceling aio request %llu in queue %d", request_id, queue->id);

    spin_lock_bh(&queue->lock);

    list_for_each_entry_safe(current_req, next_req, &queue->pending, list_anchor)
    {
        if(current_req->id == request_id)
        {
            cancel_req = current_req;
            aim_aio_request_get(cancel_req);
            break;
        }
    }

    spin_unlock_bh(&queue->lock);

    if(!cancel_req)
    {
        aim_error("Request %llu for cancellation not found", request_id);
        return -EINVAL;
    }

    ret = 0;
    do
    {
        ret = aim_aio_request_cancel(current_req);
        if(ret)
        {
            aim_error("Cancellation of Request %llu failed", request_id);
            break;
        }

        spin_lock_bh(&queue->lock);

        list_move_tail(&cancel_req->list_anchor, &queue->completed);
        wake_up(&queue->wait_queue);

        spin_unlock_bh(&queue->lock);
    }while(0);

    aim_aio_request_put(cancel_req);

    return ret;
}


int aim_aio_queue_complete_next(struct aim_aio_queue* queue, bool success, size_t bytes_transferred, void* attachment,
                                size_t attachment_size)
{
    int ret;
    struct aim_aio_request* req = NULL;

    BUG_ON(!queue);

    aim_debug("Completing next AIO request on queue %d", queue->id);

    spin_lock_bh(&queue->lock);

    ret = 0;
    do
    {
        req = get_next_pending(queue);
        if(!req)
        {
            aim_debug("No pending requests");
            ret = -ENODATA;
            break;
        }

        ret = aim_aio_request_complete(req, success, bytes_transferred, attachment, attachment_size);
        if(ret)
        {
            aim_dev_error(req->device, "Failed to complete request");
            break;
        }

        list_move_tail(&req->list_anchor, &queue->completed);
        wake_up(&queue->wait_queue);

    }while(0);

    spin_unlock_bh(&queue->lock);

    if(req)
    {
        aim_aio_request_put(req);
    }

    return ret;
}


int aim_aio_request_init(struct aim_aio_request* req, struct aim_pci_device* device, enum aim_dma_direction dir,
                         void __user* dma_buffer_addr, size_t dma_buffer_size)
{
    int ret;

    BUG_ON(!req || !device);

    /* Generate random ID for the request */
    get_random_bytes(&req->id, sizeof(req->id));

    req->bytes_transferred = 0;
    req->device = device;
    req->direction = dir;
    spin_lock_init(&req->lock);
    kref_init(&req->ref_count);
    req->state = AIO_OP_NOT_PROCESSED;
    req->start = NULL;
    INIT_LIST_HEAD(&req->list_anchor);

    if(dma_buffer_size == 0)
    {
        req->dma_buffer = NULL;
        return 0;
    }

    req->dma_buffer = kzalloc(sizeof(struct aim_dma_buffer), GFP_KERNEL);
    if(!req->dma_buffer)
    {
        aim_dev_error(req->device, "Failed to allocate DMA buffer");
        return -ENOMEM;
    }

    /* Now setup a DMA buffer for the operation */
    ret = 0;
    do
    {
        ret = aim_dma_buffer_init(req->dma_buffer, dma_buffer_addr, dma_buffer_size);
        if(ret)
        {
            aim_dev_error(device, "Failed to initialize DMA buffer");
            break;
        }

        ret = aim_dma_buffer_lock_pages(req->dma_buffer);
        if(ret)
        {
            aim_dev_error(device, "Locking of pages failed %d\n", ret);
            break;
        }

        ret = aim_dma_buffer_create_sg(req->dma_buffer);
        if(ret)
        {
            aim_dev_error(device, "Creation of scatter/gather table failed\n");
            break;
        }

        ret = aim_dma_buffer_map(req->dma_buffer, &device->pciDevice->dev);
        if(ret)
        {
            aim_dev_error(device, "Mapping of scatter/gather table failed\n");
            break;
        }
    }while(0);

    if(ret)
    {
        aim_dma_buffer_free(req->dma_buffer, &device->pciDevice->dev, false);
        kfree(req->dma_buffer);
        req->dma_buffer = NULL;
    }

    return ret;
}


void aim_aio_request_put(struct aim_aio_request* req)
{
    BUG_ON(!req);

    kref_put(&req->ref_count, destroy_request);
}


void aim_aio_request_get(struct aim_aio_request* req)
{
    BUG_ON(!req);

    kref_get(&req->ref_count);
}


void aim_aio_request_set_start_func(struct aim_aio_request* req, aim_aio_request_start_fn func)
{
    BUG_ON(!req);

    req->start = func;
}


int aim_aio_request_start(struct aim_aio_request* req)
{
    aim_aio_request_start_fn start_func;

    BUG_ON(!req);

    aim_dev_debug(req->device, "Starting AIO request %llu", req->id);

    spin_lock_bh(&req->lock);

    if(req->state != AIO_OP_NOT_PROCESSED)
    {
        aim_dev_error(req->device, "AIO request %llu already processsed", req->id);
        spin_unlock_bh(&req->lock);
        return -EALREADY;
    }

    req->state = AIO_OP_PENDING;

    /* store start function so it may not be cleared after releasing the request lock */
    start_func = req->start;

    spin_unlock_bh(&req->lock);

    /* Call start function without holding the request lock */
    if(start_func)
    {
        return start_func(req);
    }

    return 0;
}


int aim_aio_request_cancel(struct aim_aio_request* req)
{
    int ret;
    aim_aio_request_cancel_fn cancel_func = NULL;

    BUG_ON(!req);

    aim_dev_debug(req->device, "Cancelling request %llu", req->id);

    spin_lock_bh(&req->lock);

    ret = 0;
    do
    {
        switch(req->state)
        {
            case AIO_OP_CANCELLED:
                ret = -EALREADY;
                break;

            case AIO_OP_COMPLETED:
            case AIO_OP_FAILED:
                ret = -EBUSY;
                break;

            default:
                req->state = AIO_OP_CANCELLED;
                cancel_func = req->cancel; /* Store cancel func as it might be removed when releasing request spinlock */
                ret = 0;
                break;
        }
    }while(0);

    spin_unlock_bh(&req->lock);

    if(ret == 0)
    {
        if(cancel_func)
        {
            ret = cancel_func(req);
        }
    }

    return ret;
}


int aim_aio_request_complete(struct aim_aio_request* req, bool success, size_t bytes_transferred, void* attachment,
                             size_t attachment_size)
{
    int ret;

    BUG_ON(!req);

    aim_dev_debug(req->device, "Completing request %llu (%zu bytes were transferred)", req->id, bytes_transferred);

    if(attachment_size > AIO_REQ_MAX_ATTCHMNT_SIZE)
    {
        aim_dev_error(req->device, "Attachment size of %zu too large for AIO request completion", attachment_size);
        return -EINVAL;
    }

    if(attachment_size > 0 && !attachment)
    {
        aim_dev_error(req->device, "No valid attachment buffer for aio completion");
        return -EFAULT;
    }

    spin_lock_bh(&req->lock);

    ret = 0;
    do
    {
        switch(req->state)
        {
            case AIO_OP_COMPLETED:
            case AIO_OP_FAILED:
                ret = -EALREADY;
                break;

            case AIO_OP_CANCELLED:
                ret = -EBUSY;
                break;

            default:
                req->state = success ? AIO_OP_COMPLETED : AIO_OP_FAILED;
                req->bytes_transferred = bytes_transferred;
                req->attachment_size = attachment_size;
                memcpy(req->attachment, attachment, attachment_size);
                ret = 0;
                break;
        }
    }while(0);

    spin_unlock_bh(&req->lock);

    return ret;
}


void aim_aio_request_set_cancel_func(struct aim_aio_request* req, aim_aio_request_cancel_fn func)
{
    BUG_ON(!req);

    req->cancel = func;
}

