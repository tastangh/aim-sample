/* SPDX-FileCopyrightText: 2010-2022 AIM GmbH <info@aim-online.com> */
/* SPDX-License-Identifier: MIT */

/*! \file DeviceNotification.c
 *
 * Function declarations used for PNP Device Notification
 * 
 */

#include "DeviceNotification.h"
#include <pthread.h>
#include <stdio.h>
#include <sys/inotify.h>
#include <sys/types.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>

#if defined _API1553 || defined _API429
#include "Ai_cdef.h"
#endif

#ifdef _API1553
  #include "Ai1553i_def.h"
  #include "Aim1553_io.h"
#elif defined _API429
  #include "Api429.h"
  #include "AiCPU_io.h"
#else
  #include "AiFdx_io.h"
#endif


/* Size of buffer used for reading inotify events.
 * Rather arbitrary size (10 events with expected file name size of 16 each) */
#define INOTIFY_BUF_SIZE (sizeof(struct inotify_event) + 16) * 100


/* structure that contains inotify handles that are used by
 * device notification thread */
typedef struct _InotifyHandles
{
    int i_InotifyHandler;
    int i_WatchHandler;
} InotifyHandles;


/* struct that contains the inotify handles and the
 * device notification list of the notifier thread
 */
typedef struct _NotificationThreadParams
{
    InotifyHandles* p_InotifyHandles;
    DeviceNotification* p_DeviceNotifications;
} NotificationThreadParams;


/* Posix thread handle for the device notification thread */
pthread_t MilDeviceNotificationThreadHandle = 0l;


/* Posix thread that checks for arrival and removal of PnP devices.
 * It uses the inotify API in order to do this
 * Parameters:  void* - thread argument (List of device notifications)
 * Return: Not used here
 */
static void* MilDeviceNotificationThread(void* p_Argument);


/* Cleanup function that closes inotify handles when the
 * device notification thread is canceled
 * Parameters: void* - pointer to struct that contains inotify handles to close
 */
static void MilDeviceNotificationThreadCleanup(void* p_Argument);


/* Sets up inotify watches in order to be notified of device arrival
 * and removal
 * Parameters:  InotiyHandles* - pointer to structure that will receive the
 *                               inotify handles that were set up
 * Return:  returns 0 on success
 */
static int InstallInotifyWatches(InotifyHandles* p_InotifyHandles);


/* Suspends the current thread, until an inotify event is received
 * Parameters:  InotifyHandles* - pointer to struct that contains the
 *                                inotify handle to wait for
 * Return:  returns 0 on success
 */
static int WaitForInotifyEvents(InotifyHandles* p_InotifyHandles);


/* This function reads and processes inotify events
 * Parameters:  InotifyHandles* - pointer to struct that contains
 *                                the inotify file descriptor to read events from
 *              DeviceNotification* - pointer to start of device notification list
 * Return:  returns 0 on success
 */
static int ProcessInotifyEvents(InotifyHandles* p_InotifyHandles,
                                DeviceNotification* p_DeviceNotificationList);


/* Forks a new process and calls udevadm command in order
 * to wait for all udev events to settle.
 * This is necessary to wait for udev rules to be applied before
 * device is actually opened.
 * Return: returns 0 on success.
 */
static int WaitForUdevSettle();




/* Starts the thread that checks for arrival and removal
 * of devices
 * Parameters: DeviceNotifaction* - pointer to list of device notifications that
 * thread shall watch for
 * Return: void */
void StartMilDeviceNotificationThread(DeviceNotification* p_DeviceNotificationList)
{
    int ret;

    ret = pthread_create(&MilDeviceNotificationThreadHandle, NULL, MilDeviceNotificationThread,
                         p_DeviceNotificationList);
    if (ret)
    {
        perror("Device Notification Thread");
    }
}


/* Stops the thread that checks for arrival and removal
 * of devices
 */
void StopMilDeviceNotificationThread(void)
{
    if (MilDeviceNotificationThreadHandle != 0)
    {
        pthread_cancel(MilDeviceNotificationThreadHandle);
        pthread_join(MilDeviceNotificationThreadHandle, NULL);
        MilDeviceNotificationThreadHandle = 0;
    }
}

int isDeviceNotificationActive(void)
{
    return MilDeviceNotificationThreadHandle != 0;
}


/* Posix thread that checks for arrival and removal of PnP devices.
 * It uses the inotify API in order to do this
 * Parameters:  void* - thread argument (not used here)
 * Return: Not used here
 */
static void* MilDeviceNotificationThread(void* p_Argument)
{
    int ret;
    InotifyHandles* p_InotifyHandles;
    DeviceNotification* p_DeviceNotificationList;
    NotificationThreadParams* p_ThreadParams = NULL;

    p_DeviceNotificationList = (DeviceNotification*) p_Argument;
    if(p_DeviceNotificationList == NULL)
    {
        fprintf(stderr, "No valid device notifications for device notification thread\n");
        pthread_exit(NULL);
    }

    /* Enable thread to be canceled */
    ret = pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, NULL);
    if (ret)
    {
        perror("Setting cancel state of device notification thread");
        pthread_exit(NULL);
    }

    ret = pthread_setcanceltype(PTHREAD_CANCEL_DEFERRED, NULL);
    if (ret)
    {
        perror("Setting cancel type of device notification thread");
        pthread_exit(NULL);
    }

    /* Allocate structure that will hold inotify handles
     * (must be allocated on heap, as it is used by cleanup function
     */
    p_InotifyHandles = (InotifyHandles*) malloc(sizeof(InotifyHandles));
    if (!p_InotifyHandles)
    {
           perror("Allocation of inotify handle structure");
           pthread_exit(NULL);
    }

    ret = InstallInotifyWatches(p_InotifyHandles);
    if(ret)
    {
        pthread_exit(NULL);
    }

    p_ThreadParams = (NotificationThreadParams*) malloc(sizeof(*p_ThreadParams));
    if(!p_ThreadParams)
    {
        fprintf(stderr, "Allocation of device notification thread parameters failed\n");
        if(p_InotifyHandles)
            free(p_InotifyHandles);
    }

    p_ThreadParams->p_InotifyHandles = p_InotifyHandles;
    p_ThreadParams->p_DeviceNotifications = p_DeviceNotificationList;

    /* Install thread cleanup function with the thread's parameters*/
    pthread_cleanup_push(MilDeviceNotificationThreadCleanup, (void*) p_ThreadParams);

    /* Wait for and process inotify events until thread is canceled */
    while(ret >= 0)
    {
        ret = WaitForInotifyEvents(p_InotifyHandles);

        if(ret > 0)
        {
            /* wait until potential udev rules have been applied to new device */
            ret = WaitForUdevSettle();
            if(ret)
            {
                /* If waiting for udev settle failed, do a sleep
                * so udev rules can probably be applied
                */
                sleep(1);
            }

            ret = ProcessInotifyEvents(p_InotifyHandles, p_DeviceNotificationList);
        }

        pthread_testcancel();
    }

    pthread_cleanup_pop(0);

    pthread_exit(NULL);
}


/* Cleanup function that closes inotify handles when the
 * device notification thread is canceled
 * Parameters: void* - pointer to struct that contains inotify handles to close
 */
static void MilDeviceNotificationThreadCleanup(void* p_Argument)
{
    int ret;
    NotificationThreadParams* p_ThreadParams;
    InotifyHandles* p_InotifyHandles;
    DeviceNotification* p_CurrentDeviceNotification;
    DeviceNotification* p_DeletableNotification;

    if (!p_Argument)
    {
        return;
    }

    p_ThreadParams = (NotificationThreadParams*) p_Argument;
    p_InotifyHandles = p_ThreadParams->p_InotifyHandles;

    ret = inotify_rm_watch(p_InotifyHandles->i_InotifyHandler, p_InotifyHandles->i_WatchHandler);
    if (ret)
    {
        perror("Device directory watch removal");
    }

    close(p_InotifyHandles->i_InotifyHandler);
    free(p_InotifyHandles);

    /* Now free all elements in the device notification list */
    p_CurrentDeviceNotification = p_ThreadParams->p_DeviceNotifications;

    while(p_CurrentDeviceNotification)
    {
        p_DeletableNotification = p_CurrentDeviceNotification;
        p_CurrentDeviceNotification = p_CurrentDeviceNotification->p_Next;

        free(p_DeletableNotification);
    }

    free(p_ThreadParams);
}



/* Sets up inotify watches in order to be notified of device arrival
 * and removal
 * Parameters:  InotiyHandles* - pointer to structure that will receive the
 *                               inotify handles that were set up
 * Return:  returns 0 on success
 */
static int InstallInotifyWatches(InotifyHandles* p_InotifyHandles)
{
    int i_InotifyHandler;
    int i_WatchHandler;

    /* initialize inotify */
    i_InotifyHandler = inotify_init();
    if (i_InotifyHandler == -1)
    {
        perror("Inotify initialization");
        return -1;
    }

    /* set watch on /dev directory for creation/deletion of files */
    i_WatchHandler = inotify_add_watch(i_InotifyHandler, "/dev", IN_CREATE | IN_DELETE);
    if (i_WatchHandler == -1)
    {
        perror("Adding watch to device directory");
        close(i_InotifyHandler);
        return -1;
    }

    p_InotifyHandles->i_InotifyHandler = i_InotifyHandler;
    p_InotifyHandles->i_WatchHandler = i_WatchHandler;

    return 0;
}


/* Suspends the current thread, until an inotify event is received
 * Parameters:  InotifyHandles* - pointer to struct that contains the
 *                                inotify handle to wait for
 * Return:  returns 0 on success
 */
static int WaitForInotifyEvents(InotifyHandles* p_InotifyHandles)
{
    int ret;
    fd_set FileDescriptorSet;
    struct timeval TimeOut;

    FD_ZERO(&FileDescriptorSet);
    FD_SET(p_InotifyHandles->i_InotifyHandler, &FileDescriptorSet);

    TimeOut.tv_sec = 1;
    TimeOut.tv_usec = 0;

    /* Wait for the inotify file descriptor to become readable */
    ret = select(p_InotifyHandles->i_InotifyHandler + 1, &FileDescriptorSet, NULL, NULL, &TimeOut);
    if(ret < 0)
    {
        perror("Device directory watch");
        return -1;
    }

    return ret;
}


/* This function reads and processes inotify events
 * Parameters:  InotifyHandles* - pointer to struct that contains
 *                                the inotify file descriptor to read events from
 *              DeviceNotification* - pointer to device notification list
 * Return:  returns 0 on success
 */
static int ProcessInotifyEvents(InotifyHandles* p_InotifyHandles,
                                DeviceNotification* p_DeviceNotificationList)
{
    int i;
    ssize_t BytesRead;
    char p_InotifyBuffer[INOTIFY_BUF_SIZE];
    struct inotify_event* p_InotifyEvent;
    size_t DevicePathLength;
    char* s_DevicePath;
    unsigned char b_DeviceFound = FALSE;
    DeviceNotification* p_CurrentDeviceNotification;

    i = 0;
    BytesRead = 0;

    /* Read events from inotify file descriptor */
    BytesRead = read(p_InotifyHandles->i_InotifyHandler, p_InotifyBuffer, INOTIFY_BUF_SIZE);
    if(BytesRead < 0)
    {
        perror("Reading inotify events");
        return -1;
    }

    /* Parse and process all events that were read */
    while (i < BytesRead)
    {
        p_InotifyEvent = (struct inotify_event*) &p_InotifyBuffer[i];
        b_DeviceFound = FALSE;
        p_CurrentDeviceNotification = p_DeviceNotificationList;

        /* check if event matches any of the devices in the notification list */
        while(p_CurrentDeviceNotification && !b_DeviceFound)
        {
            if (p_InotifyEvent->len >= strlen(p_CurrentDeviceNotification->s_DevicePrefix))
            {
                /* check if filename begins with device prefix */
                if (!strncmp(p_InotifyEvent->name, p_CurrentDeviceNotification->s_DevicePrefix,
                             strlen(p_CurrentDeviceNotification->s_DevicePrefix)))
                {
                    b_DeviceFound = TRUE;

                    /* length of device path = "/dev" + "/" + length of filename */
                    DevicePathLength = strlen(DEVICE_DIRECTORY) + strlen(p_InotifyEvent->name) + 1;

                    s_DevicePath = (char*) malloc(DevicePathLength + 1); /* device path + trailing zero */
                    if(!s_DevicePath)
                    {
                        perror("Device path allocation");
                        return -1;
                    }

                    /* construct absolute device file path */
                    snprintf(s_DevicePath, DevicePathLength + 1, "%s/%s", DEVICE_DIRECTORY,
                             p_InotifyEvent->name);

                    if (p_InotifyEvent->mask & IN_CREATE)
                    {
                        MilDeviceArrival(s_DevicePath);
                    }
                    else if (p_InotifyEvent->mask & IN_DELETE)
                    {
                        MilDeviceRemoval(s_DevicePath);
                    }

                    free(s_DevicePath);
                }
                else
                {
                    p_CurrentDeviceNotification = p_CurrentDeviceNotification->p_Next;
                }
            }
            else
            {
                p_CurrentDeviceNotification = p_CurrentDeviceNotification->p_Next;
            }
        }

        i += sizeof(struct inotify_event) + p_InotifyEvent->len;
    }

    return 0;
}


/* Forks a new process and calls udevadm command in order
 * to wait for all udev events to settle.
 * This is necessary to wait for udev rules to be applied before
 * device is actually opened.
 * Return: returns 0 on success.
 */
static int WaitForUdevSettle()
{
    pid_t ProcessID;
    int ret;
    int i_WaitStatus;

    ProcessID = fork();

    if(ProcessID == -1)
    {
        perror("Forking udevadm process failed");
        return -1;
    }

    if (ProcessID == 0)
    {
        /* child process */
        ret = execlp("udevadm", "udevadm", "settle", "--timeout=1", NULL);
        if (ret == -1)
        {
            perror("Calling udevadm failed\n");
            return ret;
        }

        return 0;
    }
    else
    {
        /* parent process, wait until child finished */
        ret = waitpid(ProcessID, &i_WaitStatus, 0);
        if (ret == -1)
        {
            perror("Waiting for udevadm failed\n");
            return -1;
        }

        if (i_WaitStatus != 0)
        {
            fprintf(stderr,"Warning. Udevadm returned with failure status %d\n",
                    i_WaitStatus);
        }
    }

    return 0;
}
