/* SPDX-FileCopyrightText: 2010-2022 AIM GmbH <info@aim-online.com> */
/* SPDX-License-Identifier: MIT */

/*! \file DeviceNotification.h
 *
 * Function declarations used for PNP Device Notification
 * 
 */

#ifndef DEVICENOTIFICATION_H_
#define DEVICENOTIFICATION_H_

/* Maximum length that device prefixes used for notification can consist of */
#define DEVICE_PREFIX_LEN 16


/* struct that contains a device prefix for notification
 * can be added to a single-linked list
 */
typedef struct _DeviceNotification
{
    char s_DevicePrefix[DEVICE_PREFIX_LEN + 1];
    struct _DeviceNotification* p_Next;
} DeviceNotification;



/* Starts the thread that checks for arrival and removal
   of devices
   Parameters: DeviceNotifaction* - pointer to list of device notifications that
                                    thread shall watch for
   Return: void */
void StartMilDeviceNotificationThread(DeviceNotification* p_DeviceNotificationList);


/* Stops the thread that checks for arrival and removal
 * of devices
 */
void StopMilDeviceNotificationThread(void);


/* Checks if device notification thread is already running
 * Return: true if thread is active, false otherwise
 */
int isDeviceNotificationActive(void);


#endif /* DEVICENOTIFICATION_H_ */
