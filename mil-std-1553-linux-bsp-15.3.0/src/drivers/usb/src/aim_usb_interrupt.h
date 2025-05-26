/* SPDX-FileCopyrightText: 2017-2021 AIM GmbH <info@aim-online.com> */
/* SPDX-License-Identifier: GPL-2.0-or-later */

/*! aim_usb_interrupt.h
 *
 */

#ifndef AIM_USB_INTERRUPT_H_
#define AIM_USB_INTERRUPT_H_




/* Forward declaration */
struct aim_usb_int_ep;




/*! \typedef aim_usb_int_ep_completion
 *
 * Function pointers that are called on interrupt endpoint completion
 */
typedef void (*aim_usb_int_ep_completion)(struct aim_usb_int_ep*, void*, size_t);




/*! \struct aim_usb_int_ep
 *
 * This structure encapsulates handling of one specific interrupt endpoint.
 * A specific handler function can be assigned to the endpoint.
 * This one is called when the corresponding URB is completed
 */
struct aim_usb_int_ep
{
    struct aim_usb_interface* intf;     /*!< The AIM USB interface the endpoint belongs to */
    struct usb_host_endpoint* ep;       /*!< The actual interrupt endpoint of the device */
    struct urb* urb;                    /*!< The URB to use for receiving interrupts from the endpoint */
    void* urb_buffer;                   /*!< The URB transfer buffer were interrupt data will be stored */
    size_t buffer_len;                  /*!< size of URB transfer buffer in bytes */
    aim_usb_int_ep_completion completion_handler; /*!< handler function for the endpoint that is called when interrupt was received */
};


/*! \brief Initializes an interrupt endpoint
 *
 * @param int_ep the endpoint to initialize
 * @param intf the AIM USB interface the endpoint belongs to
 * @param ep the actual USB endpoint of the device to use
 * @return 0 on success, errno code on failure
 */
extern int aim_usb_int_ep_init(struct aim_usb_int_ep* int_ep, struct aim_usb_interface* intf, struct usb_host_endpoint* ep);


/*! \brief Frees an interrupt endpoint
 *
 * Must be called before destroying a specific endpoint.
 * Will release all resources allocated for the endpoint
 * @param int_ep  the interrupt endpoint to free
 */
extern void aim_usb_int_ep_free(struct aim_usb_int_ep* int_ep);


/*! \brief Start an interrupt endpoint
 *
 * Will start interrupt reception on a specific interrupt endpoint.
 * Submits an URB on the underlying device endpoint to poll for interrupts
 * @param int_ep the interrupt endpoint to start
 * @return 0 on success, errno code on failure
 */
extern int aim_usb_int_ep_start(struct aim_usb_int_ep* int_ep);


/*! \brief Stop an interrupt endpoint
 *
 * Will stop reception of interrupts on a specific interrupt endpoint.
 * Must not be called in interrupt context as the URB cancellation might sleep
 * @param int_ep the interrupt endpoint to stop
 */
extern void aim_usb_int_ep_stop(struct aim_usb_int_ep* int_ep);


/*! \brief Set completion handler for an interrupt endpoint
 *
 * @param int_ep the interrupt handler to set completion handler for
 * @param completion the completion handler to set
 */
extern void aim_usb_int_ep_set_completion(struct aim_usb_int_ep* int_ep, aim_usb_int_ep_completion completion);




#endif /* AIM_USB_INTERRUPT_H_ */
