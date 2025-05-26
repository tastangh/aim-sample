/* SPDX-FileCopyrightText: 2013-2021 AIM GmbH <info@aim-online.com> */
/* SPDX-License-Identifier: GPL-2.0-or-later */

/*! \file aim_aio.h
 *
 *  This file contains declarations for
 *  protocol generic asynchronous IO functionality
 *
 */

#ifndef AIM_AIO_H_
#define AIM_AIO_H_


#include <linux/spinlock.h>
#include <linux/kref.h>
#include <linux/sched.h>
#include <linux/wait.h>
#include "aim_ioctl_interface.h"
#include "aim_list.h"




/*! \def AIO_REQ_MAX_ATTCHMNT_SIZE
 * Maximum allowed size of attachments in bytes for asynchronous IO requests
 */
#define AIO_REQ_MAX_ATTCHMNT_SIZE  128



struct aim_aio_request;




/*! \struct aim_aio_queue
 *
 * This structure represents an asynchronous IO queue
 */
struct aim_aio_queue
{
    int id;                         /*!< ID of the queue */
    struct file* f_handle;          /*!< Optional file handle assigned to queue. May be used for tracking ownership of queue */
    spinlock_t lock;                /*!< lock for synchronizing access to the queue */
    struct kref refcount;           /*!< Reference count for counting usage of the queue */
    struct list_head pending;       /*!< List holding pending IO requests */
    struct list_head completed;     /*!< List holding completed IO requests */
    wait_queue_head_t wait_queue;   /*!< wait queue for waiting for completed requests */
};


/*! \typedef aim_aio_request_start_fn
 *
 * Custom handler function for starting asynchronous IO requests.
 * May be set with \ref aim_aio_request_set_start_func and is then called during request start
 * @param The request that is started
 * @return 0 on success, negative errno code on failure
 */
typedef int (*aim_aio_request_start_fn)(struct aim_aio_request*);


/*! \typedef aim_aio_request_cancel_fn
 *
 * Custom handler function for cancelling of asynchronous IO operations
 * May be set with \ref aim_aio_request_set_cancel_func and is then called when request is cancelled
 * @param The request that is cancelled
 * @return 0 on success, negative errno code on failure
 */
typedef int (*aim_aio_request_cancel_fn)(struct aim_aio_request*);




/*! \struct aim_aio_request
 *
 *  This structure comprises all properties of an asynchronous IO request
 */
struct aim_aio_request
{
    __u64 id;                                    /*!< Unique Identifier of request */
    enum aim_dma_direction direction;            /*!< Direction of AIO operation */
    size_t bytes_transferred;                    /*!< Number of bytes transferred with the asynchronous IO operation */
    __u32 flags;                                 /*!< Flags for future use */
    enum aim_aio_op_state state;                 /*!< Current state of request */
    struct aim_pci_device* device;               /*!< Device the AIO operation will be processed on */
    struct kref ref_count;                       /*!< Reference counter for the request */
    spinlock_t lock;                             /*!< spinlock for synchronizing access to the request */
    struct aim_dma_buffer* dma_buffer;           /*!< DMA buffer to use for the AIO operation */
    __u8 attachment[AIO_REQ_MAX_ATTCHMNT_SIZE];  /*!< Buffer for protocol specific data */
    size_t attachment_size;                      /*!< Number of valid bytes in attachment */
    aim_aio_request_start_fn start;              /*!< Protocol specific start function */
    aim_aio_request_cancel_fn cancel;            /*!< Protocol specific cancel function */
    struct list_head list_anchor;                /*!< List anchor for queuing of the request */
};




/*! \brief Initializes an asynchronous IO queue
 *
 * @param queue The queue to initialize
 * @param id ID to initialize queue with
 * @param device the device the queue belongs to
 * @return 0 on success errno code on failure
 */
extern int aim_aio_queue_init(struct aim_aio_queue* queue, int id, struct aim_pci_device* device);


/*! \brief Get hold of asynchronous IO queue
 *
 * Will increase the reference count of the queue, so it can't be destroyed while in use
 * @param queue The queue to get a hold of
 */
extern void aim_aio_queue_get(struct aim_aio_queue* queue);


/*! \brief Release hold to asynchronous IO queue
 *
 * Will decrease the reference count of the queue. If this one drops to zero,
 * the queue will be destroyed
 * @param queue the queue to release the hold of
 */
extern void aim_aio_queue_put(struct aim_aio_queue* queue);


/*! \brief Assign file handle to asynchronous IO queue
 *
 * @param queue the queue to assign file handle for
 * @param handle the file handle to assign
 */
extern void aim_aio_queue_assign_fhandle(struct aim_aio_queue* queue, struct file* handle);


/*! \brief Get file handle of asynchronous IO queue
 *
 * @param queue the queue to get file handle of
 * @return the file handle of the queue. NULL if none was set
 */
extern struct file* aim_aio_queue_get_fhandle(struct aim_aio_queue* queue);


/*! \brief Submits one asynchronous IO request to a specific queue
 *
 * Enqueues the request in the specified queue and starts the asynchronous I/O operation
 * @param queue the asynchronous I/O queue to submit request to
 * @param req the request to submit
 * @return 0 on success, negative errno code on failure
 */
extern int aim_aio_queue_submit(struct aim_aio_queue* queue, struct aim_aio_request* req);


/*! \brief Remove completed asynchronous IO requests from queue
 *
 * This function will remove completed aio requests from a specific aio queue.
 * It will return at once, if at least one request is complete.
 * If there is no completed request, the function will block for the specified time
 * until at least one request is complete.
 * The caller of this function becomes the new owner of the completed requests. He has
 * to release them after using them.
 * @param queue the queue to pop completed events from
 * @param requests the completed requests will be stored here
 * @param max_requests maximum number of requests to pop. Must match number of struct aim_aio_request in 'requests'
 * @param timeout_ms maximum time to wait for any request being completed in milliseconds
 * @return Number of actually completed requests on success. Negative errno code on failure
 */
extern int aim_aio_queue_pop_completed(struct aim_aio_queue* queue, struct aim_aio_request** requests, int max_requests,
                                       int timeout_ms);


/*! \brief Cancel specific asynchronous IO request in queue
 *
 * @param queue The queue to cancel request in
 * @param request_id The id of the request to cancel
 * @return 0 if request has been found and canceled. Negative errno code otherwise.
 */
extern int aim_aio_queue_cancel_request(struct aim_aio_queue* queue, __u64 request_id);


/*! \brief Completes next pending request in asynchronous IO queue
 *
 * @param queue the queue to complete next request on
 * @param success Indicates if asynchronous IO operation was successful
 * @param bytes_transferred number of bytes that were transferred for during the aio operation
 * @param attachment protocol specific data to attach to the request
 * @param attachment_size number of valid bytes in attachment buffer
 * @return 0 on success, negative errno code on failure
 */
extern int aim_aio_queue_complete_next(struct aim_aio_queue* queue, bool success, size_t bytes_transferred,
                                       void* attachment, size_t attachment_size);


/*! \brief Initializes an asynchronous I/O request
 *
 * @param req the request to initialize
 * @param device the device the AIO operation will be processed on
 * @param dir direction of operation
 * @param dma_buffer_addr address of user space DMA buffer for the operation
 * @param dma_buffer_size size of DMA buffer in bytes
 * @return 0 on success, negative errno code on failure
 */
extern int aim_aio_request_init(struct aim_aio_request* req, struct aim_pci_device* device, enum aim_dma_direction dir,
                                void __user* dma_buffer_addr, size_t dma_buffer_size);


/*! \brief Release hold of asynchronous IO request
 * Will decrease the request's reference counter
 * @param req the request to release hold of
 */
extern void aim_aio_request_put(struct aim_aio_request* req);


/*! \brief Grab hold of asynchronous IO request
 *
 * Will increase the request's reference counter so it can't be deleted any more
 * @param req the request to grab hold of
 */
extern void aim_aio_request_get(struct aim_aio_request* req);


/*! \brief Set start function for asynchronous IO request
 *
 * @param req the request to set start function for
 * @param func the start function to set
 */
extern void aim_aio_request_set_start_func(struct aim_aio_request* req, aim_aio_request_start_fn func);


/*! \brief Starts asynchronous IO operation
 *
 * @param req the request to start
 * @return 0 on success, negative errno code otherwise
 */
extern int aim_aio_request_start(struct aim_aio_request* req);


/*! \brief Cancels asynchronous IO operation
 *
 * @param req the request to cancel
 * @return 0 on success, negative errno code on failure
 */
extern int aim_aio_request_cancel(struct aim_aio_request* req);


/*! \brief Completes an asynchronous IO request
 *
 * Completes the request and wakes up any waiting processes.
 * @param req the request to complete
 * @param success indicates if asynchronous IO operation was successful
 * @param bytes_transferred number of bytes that were transferred during the asynchronous IO operation
 * @param attachment protocol specific data to attach to the request
 * @param attachment_size number of valid bytes in attachment buffer
 * @return 0 on success, negative errno code on failure.
 */
extern int aim_aio_request_complete(struct aim_aio_request* req, bool success, size_t bytes_transferred, void* attachment,
                                    size_t attachment_size);


/*! \brief Sets cancel function for asynchronous IO operation
 *
 * @param req the request to set cancel function for
 * @param func the cancel function to set
 */
extern void aim_aio_request_set_cancel_func(struct aim_aio_request* req, aim_aio_request_cancel_fn func);


#endif /* AIM_AIO_H_ */
