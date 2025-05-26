/* SPDX-FileCopyrightText: 2001-2023 AIM GmbH <info@aim-online.com> */
/* SPDX-License-Identifier: MIT */

/*! \file Aim1553_io.h
 *
 * IO layer implementation for Linux
 * 
 */

#include <stdbool.h>
#include <stdio.h>
#include <fcntl.h>
#include <pthread.h>
#include <errno.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <time.h>

#include <sys/stat.h>
#include <unistd.h>
#include <sys/types.h>
#include <stdlib.h>
#include <dirent.h>
#include <stdint.h>

#include "Ai_cdef.h"
#include "Aim1553.h"
#include "Ai1553i_def.h"
#include "ApiLsScope.h"
#include "ApiLsDQue.h"
#include "AiOs.h"

#include "Ai1553_Internal_def.h"
#ifdef _API3910
# include "Ai3910i_def.h"
#endif
#include "BWTS_API.H"
#include "DEFS_V.H"
#include "Dll_Fnc.h"
#include "Aim1553_io.h"
#include "DeviceNotification.h"

#include "aim_ioctl_interface.h"
#include "aim_rw_interface.h"

#include "hw/AiMyMon.h"
#include "hw/AiHwArtixUS.h"

#if HAVE_CONFIG_H

    #include "config.h"

#else

    /* Support Plug 'n' Play by default, if we don't have config.h */
    #define HAVE_PNP 1

#endif

#if HAVE_LIBNL3
  #include <netlink/netlink.h>
  #include <netlink/socket.h>
  #include <netlink/genl/genl.h>
  #include <netlink/genl/ctrl.h>
  #include "aim_nl_interface_mil.h"
#endif

AiUInt32 globalRamPtr;
AiUInt32 deviceCount = 0;
AiUInt addrarea;
volatile AiUInt * wordAddr;
volatile AiUInt * wordAddr2;

const char DEVICE_DIRECTORY[] = "/dev";
const char MIL_PCI_DEV_PREFIX[] = "aim_mil";
const char MIL_USB_DEV_PREFIX[] = "aim1553usb";

/* Constants ******************************************************************/
#define	SLEEP_MSEC(msec)	usleep((msec) * 1000)

/* Macros *********************************************************************/

#define NOTIFICATION_MUTEX_LOCK(p_Device) \
{   \
    if (pthread_mutex_lock(&p_Device->os_info->notification_lock))	\
    { \
        DEBUGOUT(DBG_ERROR, __FUNCTION__, "Mutex lock failed\n"); \
    } \
}


#define NOTIFICATION_MUTEX_UNLOCK(p_Device) \
{ \
    if (pthread_mutex_unlock(&p_Device->os_info->notification_lock)) \
    { \
        DEBUGOUT(DBG_ERROR, __FUNCTION__, "Mutex unlock failed!\n"); \
    } \
}


/*MP This macro locks the thread status structure for 'module'.		*/
#define THREAD_MUTEX_LOCK(p_Device)                     \
    {                               \
        if (pthread_mutex_lock(&p_Device->InterruptHandlerLock))    \
        {                           \
            fprintf(stderr, "thread mutex lock for %d FAILED!\n", p_Device->ul_ModuleNo); \
        }                           \
    }

/*MP This macro unlocks the thread status structure for 'module'.		*/
#define THREAD_MUTEX_UNLOCK(p_Device)                       \
    {                               \
        if (pthread_mutex_unlock(&p_Device->InterruptHandlerLock))  \
        {                           \
            fprintf(stderr, "thread mutex unlock for %d FAILED!\n", p_Device->ul_ModuleNo); \
        }                           \
    }
/* MP This macro displays the debug messages.					*/
/*
#define	LOG(LEVEL,FKT,STR)						\
	{								\
    	    printf("%s (%s): ", DLL_NAME, FKT);		\
	    printf STR ;				\
	}
*/
#define	LOG(LEVEL,FKT,STR)


#define	VALIDATE_PTR(p,__rc)						\
{									\
	if (NULL == (p))						\
	{								\
		fprintf(stderr, "%s:%d %s is NULL!\n", 			\
			__FILE__, __LINE__, #p);			\
		return __rc;						\
	}								\
}

/* Types **********************************************************************/


/*TD Description of module */
typedef	struct
{
    TY_DEVICE_INFO* p_DeviceInfo;
} THREAD_INFO;


/*! \struct device_info_os
 *  Linux specific properties of a device
 */
struct device_info_os
{
    pthread_mutex_t notification_lock;    /*!< lock for serializing access to notification function table */

    pthread_t notification_thread;        /*!< event notification thread identifier */

    pthread_barrier_t notification_startup_barrier; /*!< barrier for synchronizing start of event notification thread */
};

/* Prototypes *****************************************************************/

static int mg_1553_thread_attr_prepare (pthread_attr_t* p_ThreadAttributes);



/* This function searches for boards, the API has to handle.
   Boards that are found are inserted into global device list
   Return: returns number of boards, the API currently handles */
static AiUInt32 EnumerateExistingMilDevices(void);


/* This function searches for AIM PCI based boards, the API has to handle.
   Boards that are found are inserted into global device list
   Return: returns number AIM PCI boards found */
static AiUInt32 EnumerateExistingMilPCIDevices(void);


/* This function searches for AIM USB boards in the system.
   Boards that are found are inserted into global device list.
   Return: returns number of USB boards found */
static AiUInt32 EnumerateExistingMilUsbDevices(void);




void * _ApiOsScopeThread( void * arg );
void _ApiOsProcessScopeBuffer(AiUInt32 ul_ModuleHandle, TY_SCOPE_BUFFER_LIST_ELEMENT* p_ScopeBufferListElement, TY_API_SCOPE_BUFFER_FLAGS flags, AiBoolean bTriggerBuffer);
AiUInt16 _ApiOsWaitForScopeStatusIdle(AiUInt32 ulModuleHandle, AiUInt32 ulTimeOut);



#if HAVE_PNP
static void __inline__ _ApiOsStartDeviceNotification()
{
    DeviceNotification* p_DeviceNotification  = NULL;


    if(isDeviceNotificationActive() == TRUE)
    {
        /* Thread has already been created */
        return;
    }

    /* Initialize device notification thread */

    /* Allocate device notification structure for 1553 USB devices. */
    /* This will be deleted by the notification thread so we do not need
     * to store a handle here.
     * */
    p_DeviceNotification = (DeviceNotification*) malloc(sizeof(*p_DeviceNotification));

    if( p_DeviceNotification )
    {
        memset(p_DeviceNotification, 0, sizeof(*p_DeviceNotification));

        strncpy(p_DeviceNotification->s_DevicePrefix, MIL_USB_DEV_PREFIX, DEVICE_PREFIX_LEN);

        StartMilDeviceNotificationThread(p_DeviceNotification);
    }
    else
    {
        perror("Allocation of device notification list failed. Plug 'n' Play not available\n");
    }
}


static void __inline__ _ApiOsStopDeviceNotification()
{
    StopMilDeviceNotificationThread();
}

#else

static void __inline__ _ApiOsStartDeviceNotification(){}
static void __inline__ _ApiOsStopDeviceNotification(){}

#endif /* HAVE_PNP */



AiInt16 _ApiOsIni()
{
    AiUInt32            ui_NumDevices         = 0;


    _ApiOsStartDeviceNotification();

    /* Search devices */

    _ApiClearDeviceList();


    ui_NumDevices = EnumerateExistingMilDevices();

    return (AiInt16) ui_NumDevices;
}


//**************************************************************************
//
// _ApiOsOpen
//
//**************************************************************************
AiInt16 _ApiOsOpen(AiUInt32 ui_ModuleHandle)
{
    TY_DEVICE_INFO* p_Device = NULL;

    p_Device = _ApiGetDeviceInfoPtrByModule(ui_ModuleHandle);
    if(!p_Device)
    {
        return API_ERR_WRONG_MODULE;
    }

    if(p_Device->_hDrv == INVALID_HANDLE_VALUE)
    {
        p_Device->_hDrv = fopen(p_Device->DevicePath, "r+");
        if(!p_Device->_hDrv)
        {
            perror("Opening Device");
            p_Device->_hDrv = INVALID_HANDLE_VALUE;
            return API_ERR;
        }
    }

    p_Device->ulOpenCount++;

    return API_OK;
}

AiInt16 _ApiOsInitializeLocks( AiUInt32 ulModHandle )
{
    return 0;
}

AiInt16 _ApiOsFreeLocks( AiUInt32 ulModHandle )
{
    return 0;
}



//**************************************************************************
//
//  _ApiOsClose
//
//**************************************************************************
AiInt16 _ApiOsClose(AiUInt32 ui_ModuleHandle)
{
    TY_DEVICE_INFO* p_Device = NULL;

    p_Device = _ApiGetDeviceInfoPtrByModule(ui_ModuleHandle);
    if(!p_Device)
    {
        return API_ERR_WRONG_MODULE;
    }

    if (p_Device->ulOpenCount > 1)
    {
        p_Device->ulOpenCount--;
    }
    else if(1 == p_Device->ulOpenCount)
    {
        p_Device->ulOpenCount--;

        if(p_Device->_hDrv == INVALID_HANDLE_VALUE)
        {
            return API_ERR;
        }

        fclose((FILE*)p_Device->_hDrv);
        p_Device->_hDrv = INVALID_HANDLE_VALUE;
    }

    return API_OK;
}

//**************************************************************************
//
//  _ApiLinuxIo
//
//**************************************************************************
AiInt16 _ApiOsIo(AiUInt32 ui_ModuleHandle, AiUInt8* cmd, AiUInt8* ack)
{
	struct aim_target_command x_IoParameters;
    TY_DEVICE_INFO* p_Device = NULL;
    int i_DeviceFileDescriptor;
    TY_MIL_COM* p_TargetCommand = (TY_MIL_COM*) cmd;

    p_Device = _ApiGetDeviceInfoPtrByModule(ui_ModuleHandle);
    if(!p_Device)
    {
        return API_ERR_WRONG_MODULE;
    }

    if(p_Device->_hDrv == INVALID_HANDLE_VALUE)
    {
        return API_ERR;
    }

    i_DeviceFileDescriptor = fileno((FILE*) p_Device->_hDrv);
    if(i_DeviceFileDescriptor == -1)
    {
        perror("_ApiLinuxIo");
        return API_ERR;
    }

    x_IoParameters.command = cmd;
    x_IoParameters.acknowledge = ack;

    if(p_TargetCommand->ulMagic == MIL_COM_MAGIC)
    {
        x_IoParameters.cmdSize         = p_TargetCommand->ulSize;
        x_IoParameters.expectedAckSize = p_TargetCommand->ulExpectedAckSize;
    }
    else
    {
        x_IoParameters.cmdSize         = (cmd[5] << 8) + cmd[6];
        x_IoParameters.expectedAckSize = MAX_TG_CMD_SIZE;
    }

    if (ioctl(i_DeviceFileDescriptor, AIM_IOCTL_TARGET_COMMAND, &x_IoParameters))
    {
        perror("ioctl");
        return API_ERR;
    }

    return API_OK ;
}


//**************************************************************************
//
// _ApiOsReadMemData
//
//**************************************************************************
AiInt16 _ApiOsReadMemData(AiUInt32 ui_ModuleHandle, AiUInt8 memtype, AiUInt32 offset, AiUInt8 width,
                             void* data_p,  AiUInt32 size, AiUInt32* pul_BytesRead)
{
	struct aim_data_transaction_user mInfo;
    ssize_t sizeReturn;
    TY_DEVICE_INFO* p_Device = NULL;
    int i_DeviceFileDescriptor;

    p_Device = _ApiGetDeviceInfoPtrByModule(ui_ModuleHandle);
    if(!p_Device)
    {
        return API_ERR_WRONG_MODULE;
    }

    if(p_Device->_hDrv == INVALID_HANDLE_VALUE)
    {
        return API_ERR;
    }

    i_DeviceFileDescriptor = fileno((FILE*) p_Device->_hDrv);
    if(i_DeviceFileDescriptor == -1)
    {
        perror(__FUNCTION__);
        return API_ERR;
    }


    mInfo.memType      = (TY_E_MEM_TYPE)memtype;  /*!< memory type to read from / write to */
    mInfo.offset       = offset;                  /*!< offset in bytes to read from write/to */
    mInfo.size         = width;                   /*!< size of objects to read/write in bytes */
    mInfo.numObjects   = size;                    /*!< number of objects to read/write */
    mInfo.useDMA       = AiFalse;                 /*!< signifies if DMA engine of hardware shall be used for transaction */
    mInfo.dataBuffer   = (void*) data_p;          /*!< buffer in user space to read to / write from */

    if(    (memtype == API_MEMTYPE_GLOBAL)
        && (p_Device->x_Config.uc_DmaEnabled & 0x1)
        && ( p_Device->x_Config.ul_DmaMinimumSize <= (size*width)) )
    {
        mInfo.useDMA = AiTrue;
    }
    if(    (memtype == API_MEMTYPE_GLOBAL_DIRECT)
        && (p_Device->x_Config.uc_DmaEnabled & 0x1)
        && ( p_Device->x_Config.ul_DmaMinimumSize <= (size*width)) )
    {
        mInfo.useDMA = AiTrue;
    }
    if(    (memtype == API_MEMTYPE_SHARED)
        && (p_Device->x_Config.uc_DmaEnabled & 0x2)
        && ( p_Device->x_Config.ul_DmaMinimumSize <= (size*width)) )
    {
        mInfo.useDMA = AiTrue;
    }
    if(    (memtype == API_MEMTYPE_LOCAL)
        && (p_Device->x_Config.uc_DmaEnabled & 0x4)
        && ( p_Device->x_Config.ul_DmaMinimumSize <= (size*width)) )
    {
        mInfo.useDMA = AiTrue;
    }

#ifdef HOST_ENDIAN_BIG
     /*
      * Do not use DMA if width is not 1.
      * Because DMA does not swap the data.
      * */
     if( (mInfo.useDMA == AiTrue) && (width>1) )
     {
         mInfo.useDMA = AiFalse;
     }
#endif

    sizeReturn = read(i_DeviceFileDescriptor, &mInfo, sizeof(mInfo));
    if(sizeReturn < 0)
    {
        fprintf(stderr, "%s failed with status: %zu\n", __FUNCTION__, sizeReturn);
        return API_ERR;
    }

    if(NULL != pul_BytesRead)
        *pul_BytesRead = sizeReturn;

    return API_OK ;
}





//**************************************************************************
//
// _ApiOsWriteMemData
//
//**************************************************************************
AiInt16 _ApiOsWriteMemData(AiUInt32 ui_ModuleHandle, AiUInt8 memtype, AiUInt32 offset,
                               AiUInt8 width, void * data_p, AiUInt32 size, AiUInt32* pul_BytesWritten)
{
	struct aim_data_transaction_user mInfo;
    ssize_t sizeReturn;
    TY_DEVICE_INFO* p_Device = NULL;
    int i_DeviceFileDescriptor;

    p_Device = _ApiGetDeviceInfoPtrByModule(ui_ModuleHandle);
    if (!p_Device)
    {
        return API_ERR_WRONG_MODULE;
    }

    if (p_Device->_hDrv == INVALID_HANDLE_VALUE)
    {
        return API_ERR;
    }

    i_DeviceFileDescriptor = fileno((FILE*) p_Device->_hDrv);
    if (i_DeviceFileDescriptor == -1)
    {
        perror("_ApiLinuxWriteMemData");
        return API_ERR;
    }

    mInfo.memType      = (TY_E_MEM_TYPE)memtype;  /*!< memory type to read from / write to */
    mInfo.offset       = offset;                  /*!< offset in bytes to read from write/to */
    mInfo.size         = width;                   /*!< size of objects to read/write in bytes */
    mInfo.numObjects   = size;                    /*!< number of objects to read/write */
    mInfo.useDMA       = AiFalse;                 /*!< signifies if DMA engine of hardware shall be used for transaction */
    mInfo.dataBuffer   = (void*) data_p;          /*!< buffer in user space to read to / write from */

    sizeReturn = write(i_DeviceFileDescriptor, &mInfo, sizeof(mInfo));

    if(sizeReturn < 0)
    {
        fprintf(stderr, "%s failed with status: %zu\n", __FUNCTION__, sizeReturn);
        return API_ERR;
    }

    if(NULL != pul_BytesWritten)
        *pul_BytesWritten = sizeReturn;
    /*===========================================*/

    return API_OK ;
}



//**************************************************************************
//
//   Module : AIM_MIL_io.cpp           Submodule : _ApiLinuxGetSysDrvVersion
//
//   Author : Juergen Schmieder         Project   : API1553 Linux
//
//---------------------------------------------------------------------------
//   Create : 08.11.02     Update :
//---------------------------------------------------------------------------
//    Descriptions
//    ------------
//    Inputs    : none
//
//    Outputs   : System Driver AIM Mil Version Linux
//
//    Description :
//    Return System Driver Verion
//
//**************************************************************************
AiInt16 _ApiOsGetSysDrvVersion(TY_DEVICE_INFO* p_Device, TY_VER_INFO* version)
{
    int i_DeviceFileDescriptor;
    struct aim_driver_version_user newVersion;

    memset( version, 0, sizeof(TY_VER_INFO));

    if (!p_Device)
    {
        return API_ERR_WRONG_MODULE;
    }

    if (p_Device->_hDrv == INVALID_HANDLE_VALUE)
    {
        return API_ERR;
    }

    i_DeviceFileDescriptor = fileno((FILE*) p_Device->_hDrv);
    if (i_DeviceFileDescriptor == -1)
    {
        perror("_LinuxGetSysDrvVersion");
        return API_ERR;
    }

    /* Try to read new version */
    if (ioctl(i_DeviceFileDescriptor, AIM_IOCTL_GET_DRV_VERS, &newVersion) < 0 )
    {
        perror("Failed to execute version read IOCTL");
        return API_ERR;
    }

    version->ul_MajorVer     = newVersion.major;
    version->ul_MinorVer     = newVersion.minor;
    version->ul_PatchVersion = newVersion.patch;

    AiOsSnprintf(version->ac_Description, sizeof(version->ac_Description), AiTrue, "Driver");
    AiOsSnprintf(version->ac_FullVersion, sizeof(version->ac_FullVersion), AiTrue, newVersion.full_version );

    return API_OK;
}





#if HAVE_LIBNL3

static AiUInt8 ucMaxInterruptSouce = 11;
static AiUInt8 aucTranslateInterrupSourceToBiu[] = {
    0, /* BIU 1 */
    1, /* BIU 2 */
    MAX_API_STREAM, /* DMA   */
    MAX_API_STREAM, /* ASP (Target-SW) */
    MAX_API_STREAM, /* Reserved */
    2, /* BIU3 */
    3, /* BIU4 */
    4, /* BIU5 */
    5, /* BIU6 */
    6, /* BIU7 */
    7, /* BIU8 */
};



/*! \brief Get minor number of device
 *
 * This function is used to extract the minor number of a device,
 * as used by the device driver.
 *
 * @param device pointer to device to get minor number for
 * @return minor number of device on success, -1 on error
 */
static int __inline__ getMilDeviceMinor(TY_DEVICE_INFO* device)
{
    char* base = basename(device->DevicePath);
    char* prefix = NULL;
    int minor = 0;

    prefix = strstr(base, MIL_PCI_DEV_PREFIX);
    if(prefix)
    {
        minor = strtol(prefix + strlen(MIL_PCI_DEV_PREFIX), NULL, 10);
        return minor;
    }

    prefix = strstr(base, MIL_USB_DEV_PREFIX);
    if(prefix)
    {
        minor = strtol(prefix + strlen(MIL_USB_DEV_PREFIX), NULL, 10);
        return minor;
    }

    return -1;
}


static TY_INT_FUNC_PTR mg_1553_get_user_func (TY_DEVICE_INFO* p_Device, AiUInt8 biu, AiUInt8 type, AiUInt32 * pulModHandle)
{
    TY_INT_FUNC_PTR rc;

    if( (type > MAX_API_INT_TYPE) || (biu >= MAX_API_STREAM))
        goto exit_err;

    *pulModHandle = p_Device->ax_IntFuncTable[biu].ul_Module & ~0x80000000;
    rc = p_Device->ax_IntFuncTable[biu].af_IntFunc[type];

    return rc;

exit_err:
    fprintf(stderr, "*** Usage error mg_1553_get_user_func!! ****\n");
    return NULL;
}

static int mil_nl_receive_callback(struct nl_msg *msg, void *args)
{
    AiBoolean hs;
    int ret = 0;
    AiUInt8 hs_flag;
    AiUInt8 biu;
    AiUInt8 int_type;
    AiUInt32 ulModHandle;
    TY_DEVICE_INFO* device = (TY_DEVICE_INFO*) args;
    struct nlmsghdr *nlh = nlmsg_hdr(msg);
    struct nlattr *attrs[AIM_MIL_NL_A_MAX + 1];
    char dbg_msg[128];
    AiUInt32 channel_index = 0;
    TY_INT_FUNC_PTR user_handler = NULL;
    TY_API_INTR_LOGLIST_ENTRY notification_data = {0};

    TY_API_INTR_LOGLIST_ENTRY* event;
    struct genlmsghdr* genlHeader = NULL;
    static struct nla_policy aim_mil_nl_policy[AIM_MIL_NL_A_MAX + 1];

    aim_mil_nl_policy[AIM_MIL_NL_A_LOGLIST_ENTRY].type = NLA_UNSPEC;
    aim_mil_nl_policy[AIM_MIL_NL_A_LOGLIST_ENTRY].minlen = sizeof(TY_API_INTR_LOGLIST_ENTRY);

    ret = genlmsg_parse( nlh, 0, attrs, AIM_MIL_NL_A_MAX, aim_mil_nl_policy);
    if(ret)
    {
        DEBUGOUT(DBG_ERROR, __FUNCTION__, "Failed to parse netlink message");
        return NL_STOP;
    }

    event = (struct ty_api_intr_loglist_entry*) nla_data(attrs[AIM_MIL_NL_A_LOGLIST_ENTRY]);

    if( event->x_Lld.t.uc_IntSrc < ucMaxInterruptSouce )
        biu = aucTranslateInterrupSourceToBiu[event->x_Lld.t.uc_IntSrc];
    else
        biu = MAX_API_STREAM;

    int_type = (AiUInt8)((event->ul_Lla & 0xE0000000) >> 29);

    if( (int_type > MAX_API_INT_TYPE) || (biu >= MAX_API_STREAM))
    {
        DEBUGOUT(DBG_ERROR, __FUNCTION__, "Invalid event detected\n");
        return -1;
    }

    NOTIFICATION_MUTEX_LOCK(device);
    /*FA call the user interrupt handler function */
    user_handler = mg_1553_get_user_func(device, biu, int_type, &ulModHandle );

    NOTIFICATION_MUTEX_UNLOCK(device);

    if(user_handler)
    {
        hs_flag = API_INT_LS;

        if( ApiHwBiuIsHs(device->x_BoardInfo.ul_DeviceType, biu) != 0)
            hs_flag = API_INT_HS;


        notification_data.ul_Lla       = event->ul_Lla;
        notification_data.ul_Llb       = event->ul_Llb;
        notification_data.x_Llc.ul_All = event->x_Llc.ul_All;
        notification_data.x_Lld.ul_All = event->x_Lld.ul_All;

        if( device->bOpenExUsed )
            user_handler(ulModHandle, hs_flag, int_type, &notification_data);
        else
            user_handler((AiUInt8)ulModHandle, biu+1, int_type, &notification_data);
    }

    return 0;
}


static void mil_event_notification_cleanup(void* socket)
{
    if(socket)
    {
        nl_socket_free((struct nl_sock*) socket);
    }
}


void* mil_dev_event_notification_handler(void* args)
{
	struct nl_sock* event_socket = NULL;
    int rc = 0;
    TY_DEVICE_INFO* device = (TY_DEVICE_INFO*) args;
    char familyname[30];
    int irq_event_group = 0;

    /* Create a netlink socket for communication with kernel */
    event_socket = nl_socket_alloc();

    if (event_socket == NULL)
    {
        DEBUGOUT(DBG_ERROR, __FUNCTION__, "Failed to allocate netlink socket\n");
        return NULL;
    }

    pthread_cleanup_push(mil_event_notification_cleanup, event_socket);

    do
    {
        /* Disable sequence checking - not needed for multicast */
        nl_socket_disable_seq_check(event_socket);

        /* Install the callback function which gets called when a message arrive
         * on the socket */
        nl_socket_modify_cb(event_socket, NL_CB_VALID, NL_CB_CUSTOM, mil_nl_receive_callback, device);

        /* Connect the socket to the netlink controller */
        rc = genl_connect(event_socket);
        if (rc)
        {
            DEBUGOUT(DBG_ERROR, __FUNCTION__, "Failed to connect netlink socket\n");
            break;
        }

        /* Get the multicast group from the controller */
        if(strstr(device->DevicePath, MIL_USB_DEV_PREFIX))
        {
            sprintf(familyname, "%s%d",MIL_USB_DEV_PREFIX, getMilDeviceMinor(device));
        }
        else
        {
            sprintf(familyname, "%s%d",MIL_PCI_DEV_PREFIX, getMilDeviceMinor(device));
        }

        irq_event_group = genl_ctrl_resolve_grp(event_socket, familyname, AIM_MIL_NL_IRQ_EVENT_GROUP);
        if (irq_event_group < 0)
        {
            DEBUGOUT(DBG_ERROR, __FUNCTION__, "Failed to resolve irq event group \n");
            break;
        }

        /* Subscribe to group in order receive messages */
        rc = nl_socket_add_membership(event_socket, irq_event_group);

        if (rc)
        {
            DEBUGOUT(DBG_ERROR, __FUNCTION__, "Failed to resolve irq event group \n");
            break;
        }

        if(pthread_barrier_wait(&device->os_info->notification_startup_barrier) > 0)
        {
            DEBUGOUT(DBG_ERROR, __FUNCTION__, errno);
            rc = errno;
            break;
        }
    }while(0);

    if(rc)
    {
        if(event_socket)
        {
            nl_socket_free(event_socket);
        }

        return NULL;
    }

    while(true)
    {
        nl_recvmsgs_default(event_socket);
        pthread_testcancel();
    }

    pthread_cleanup_pop(1);
    return NULL;
}


AiInt16 _ApiOsInstIntHandler( AiUInt32 ul_Module, AiUInt8 uc_Biu, AiUInt8 uc_Type, TY_INT_FUNC_PTR pf_IntFunc)
{
    TY_DEVICE_INFO* device = _ApiGetDeviceInfoPtrByModule(ul_Module);
    int ret = API_OK;
    AiUInt32 ul_Stream = ((ul_Module & API_STREAM_MASK) >> API_STREAM_POS);

    if(!device)
    {
        return API_ERR_INVALID_HANDLE;
    }

    NOTIFICATION_MUTEX_LOCK(device);

    do
    {
    	device->ax_IntFuncTable[ uc_Biu ].ul_OpenExUsed         = ul_Stream;
    	device->ax_IntFuncTable[ uc_Biu ].ul_Module             = ul_Module;
    	device->ax_IntFuncTable[ uc_Biu ].af_IntFunc[ uc_Type ] = pf_IntFunc;

        if(device->os_info->notification_thread == 0)
        {
            pthread_attr_t thread_attributes = { 0 };
            mg_1553_thread_attr_prepare(&thread_attributes);

            if( pthread_barrier_init(&device->os_info->notification_startup_barrier, NULL, 2) )
            {
                DEBUGOUT(DBG_ERROR, __FUNCTION__, errno);
                ret = API_ERR;
                break;
            }

            if( pthread_create(&device->os_info->notification_thread, &thread_attributes, mil_dev_event_notification_handler, device) )
            {
                DEBUGOUT(DBG_ERROR, __FUNCTION__, errno);
                device->os_info->notification_thread = 0;
                ret = API_ERR;
                break;
            }

            if( pthread_barrier_wait(&device->os_info->notification_startup_barrier) > 0 )
            {
                DEBUGOUT(DBG_ERROR, __FUNCTION__, errno);
                device->os_info->notification_thread = 0;
                ret = API_ERR;
                break;
            }

            if( pthread_barrier_destroy(&device->os_info->notification_startup_barrier) )
            {
                DEBUGOUT(DBG_ERROR, __FUNCTION__, errno);
                device->os_info->notification_thread = 0;
                ret = API_ERR;
                break;
            }
        }
    }while(0);

    NOTIFICATION_MUTEX_UNLOCK(device);

    return ret;
}

AiInt16 _ApiOsDelIntHandler(AiUInt32 ul_Module, AiUInt8 uc_Biu, AiUInt8 uc_Type)
{
    int i;
    int j;
    TY_DEVICE_INFO* device = _ApiGetDeviceInfoPtrByModule(ul_Module);
    char msg[128];
    int ret = API_OK;
    bool handlers_left = false;
    pthread_t thread_to_be_deleted = 0;

    if(!device)
    {
        return API_ERR_INVALID_HANDLE;
    }

    NOTIFICATION_MUTEX_LOCK(device);

    device->ax_IntFuncTable[ uc_Biu ].ul_OpenExUsed         = 0;
    device->ax_IntFuncTable[ uc_Biu ].ul_Module             = 0;
    device->ax_IntFuncTable[ uc_Biu ].af_IntFunc[ uc_Type ] = NULL;

    /* Check if any handler function is left.
     * If not, stop event notification thread
     */
    for(i = 0; i < MAX_API_STREAM; i++)
    {
        for( j=0; j<MAX_API_INT_TYPE; j++ )
        {
            if(device->ax_IntFuncTable[i].af_IntFunc[j])
            {
                handlers_left = true;
                break;
            }
        }
    }

    if(!handlers_left && device->os_info->notification_thread != 0)
    {
        thread_to_be_deleted = device->os_info->notification_thread;
        device->os_info->notification_thread = 0;
    }

    NOTIFICATION_MUTEX_UNLOCK(device);

    if(thread_to_be_deleted != 0)
    {
        ret = pthread_cancel(thread_to_be_deleted);
        if(ret)
        {
            DEBUGOUT(DBG_ERROR, __FUNCTION__, strerror(ret));
            return API_ERR_IOCTL_ERROR;
        }

        ret = pthread_join(thread_to_be_deleted, NULL);
        if(ret)
        {
            snprintf(msg, sizeof(msg), "Event notification join failed: %s", strerror(ret));
            DEBUGOUT(DBG_ERROR, __FUNCTION__, strerror(ret));
            return API_ERR_IOCTL_ERROR;
        }
    }


    return ret;
}

#else

AiInt16 _ApiOsInstIntHandler( AiUInt32 ul_Module, AiUInt8 uc_Biu, AiUInt8 uc_Type, TY_INT_FUNC_PTR pf_IntFunc)
{
    return API_ERR_PLATTFORM_NOT_SUPPORTED;
}


AiInt16 _ApiOsDelIntHandler(AiUInt32 ul_Module, AiUInt8 uc_Biu, AiUInt8 uc_Type)
{
    return API_ERR_PLATTFORM_NOT_SUPPORTED;
}

#endif /* HAVE_LIBNL3 */


/*******************************************************************************
FD This function prepares the specified thread attribute for start
FD of the user interrupt thread.
*******************************************************************************/
static int mg_1553_thread_attr_prepare (pthread_attr_t* p_ThreadAttributes)
{
    int rc;

    VALIDATE_PTR(p_ThreadAttributes, !0);  /*FR !0 - error         */

    /*FA Initialize the thread attribute with defaults.     */
    if ((rc = pthread_attr_init(p_ThreadAttributes)) != 0)
    {
        return !0;
    }

    /*FA Enable the inheritance of scheduling attributes for the    */
    /*FA new thread.                        */
    if ((rc = pthread_attr_setinheritsched(p_ThreadAttributes, PTHREAD_INHERIT_SCHED)) != 0)
    {
        return !0;
    }

    return 0;               /*FR 0 - success    */
}



/* This function searches for boards, te API has to handle.
   Boards that are found are inserted into global device list
   Return: returns number of boards, the API currently handles */
static AiUInt32 EnumerateExistingMilDevices(void)
{
    AiUInt32 ui_NumDevices = 0;

    ui_NumDevices += EnumerateExistingMilPCIDevices();

    ui_NumDevices += EnumerateExistingMilUsbDevices();

    return ui_NumDevices;
}


/* This function searches for AIM PCI based boards, the API has to handle.
   Boards that are found are inserted into global device list
   Return: returns number AIM PCI boards found */
static AiUInt32 EnumerateExistingMilPCIDevices(void)
{
    AiUInt32 ui_NumPciDevices = 0;
    DIR* p_DeviceDirectory;
    struct dirent* p_CurrentDirectoryEntry;
    char* s_DevicePath;
    ssize_t DevicePathLength;
    int i_BytesCopied;
    int i_Retval;

    p_DeviceDirectory = opendir(DEVICE_DIRECTORY);
    if (!p_DeviceDirectory)
    {
        perror("Opening of /dev directory failed\n");
        return 0;
    }

    do
    {
        p_CurrentDirectoryEntry = readdir(p_DeviceDirectory);
        if (p_CurrentDirectoryEntry)
        {
            if (!strncmp(p_CurrentDirectoryEntry->d_name, MIL_PCI_DEV_PREFIX, strlen(MIL_PCI_DEV_PREFIX)))
            {
                DevicePathLength = strlen(DEVICE_DIRECTORY) + strlen(p_CurrentDirectoryEntry->d_name) + 1;
                s_DevicePath = (char*) malloc(DevicePathLength + 1);
                if (!s_DevicePath)
                {
                    perror("Allocation of device path memory failed\n");
                    return ui_NumPciDevices;
                }

                i_BytesCopied = snprintf(s_DevicePath, DevicePathLength + 1, "%s/%s", DEVICE_DIRECTORY,
                                         p_CurrentDirectoryEntry->d_name);
                if (i_BytesCopied != DevicePathLength)
                {
                    perror("Error in creating device path\n");
                    free(s_DevicePath);
                    return ui_NumPciDevices;
                }

                i_Retval = MilDeviceArrival(s_DevicePath);
                if (i_Retval)
                {
                    free(s_DevicePath);
                    return ui_NumPciDevices;
                }

                free(s_DevicePath);
                ui_NumPciDevices++;
            }
        }

    } while (p_CurrentDirectoryEntry);

    closedir(p_DeviceDirectory);

    return ui_NumPciDevices;
}


/* This function searches for AIM USB boards in the system.
   Boards that are found are inserted into global device list.
   Return: returns number of USB boards found */
static AiUInt32 EnumerateExistingMilUsbDevices(void)
{
    AiUInt32 ui_NumUsbDevices = 0;
    DIR* p_DeviceDirectory;
    struct dirent* p_CurrentDirectoryEntry;
    char* s_DevicePath;
    ssize_t DevicePathLength;
    int i_BytesCopied;
    int i_Retval;

    p_DeviceDirectory = opendir(DEVICE_DIRECTORY);
    if(!p_DeviceDirectory)
    {
        perror("Opening of /dev directory failed\n");
        return 0;
    }

    do
    {
        p_CurrentDirectoryEntry = readdir(p_DeviceDirectory);
        if(p_CurrentDirectoryEntry)
        {
            if(!strncmp(p_CurrentDirectoryEntry->d_name, MIL_USB_DEV_PREFIX, strlen(MIL_USB_DEV_PREFIX)))
            {
                DevicePathLength = strlen(DEVICE_DIRECTORY) + strlen(p_CurrentDirectoryEntry->d_name) + 1;
                s_DevicePath = (char*) malloc(DevicePathLength + 1);
                if(!s_DevicePath)
                {
                    perror("Allocation of device path memory failed\n");
                    return ui_NumUsbDevices;
                }

                i_BytesCopied = snprintf(s_DevicePath, DevicePathLength + 1, "%s/%s", DEVICE_DIRECTORY,
                                         p_CurrentDirectoryEntry->d_name);
                if(i_BytesCopied != DevicePathLength)
                {
                    perror("Error in creating device path\n");
                    free(s_DevicePath);
                    return ui_NumUsbDevices;
                }

                i_Retval = MilDeviceArrival(s_DevicePath);
                if(i_Retval)
                {
                    free(s_DevicePath);
                    return ui_NumUsbDevices;
                }

                free(s_DevicePath);
                ui_NumUsbDevices++;
            }
        }

    }while(p_CurrentDirectoryEntry);

    closedir(p_DeviceDirectory);

    return ui_NumUsbDevices;
}


/* This function handles arrival of a new AIM board. 
   It will create a new device structure in the global device list.
   Parameters: char* - path to the newly arrived device's device file
   Return: returns 0 on success. */
int MilDeviceArrival(char* s_DevicePath)
{
    TY_DEVICE_INFO_LIST* p_DeviceEntry;

    p_DeviceEntry = _ApiCreateMilDeviceListInfo(s_DevicePath);
    if(!p_DeviceEntry)
    {
        fprintf(stderr, "ERROR: Could not create device list entry for device %s\n", s_DevicePath);
        return -1;
    }

    pthread_mutex_init(&p_DeviceEntry->x_DeviceInfo.InterruptHandlerLock,      NULL);


    pthread_mutex_init( &p_DeviceEntry->x_DeviceInfo.ScopeBuffListCriticalSect, NULL);
    pthread_mutex_init( &p_DeviceEntry->x_DeviceInfo.ScopeThreadStateLock,      NULL);
    pthread_cond_init(  &p_DeviceEntry->x_DeviceInfo.hScopeThreadWakeEvent,     NULL);
    pthread_cond_init(  &p_DeviceEntry->x_DeviceInfo.hScopeThreadSuspendEvent,  NULL);

    /* Call Plug 'n' Play callback handler set by ApiSetPnpCallback */
    if(v_MilPnPCallback != (TY_PNP_CALLBACK_FUNC_PTR) NULL)
    {
        (v_MilPnPCallback)(p_DeviceEntry->x_DeviceInfo.ul_ModuleNo, 0L);
    }

    return 0;
}


/* This function handles removal of an AIM board.
   It will delete the device structure in the global device list.
   Parameters: char* - path to the removed device's device file
   Return: returns 0 on success. */
int MilDeviceRemoval(char* s_DevicePath)
{
    TY_DEVICE_INFO* p_Device = NULL;

    p_Device = _ApiGetDeviceInfoPtrByName(s_DevicePath);
    if(!p_Device)
    {
        return 0;
    }

    /* Call Plug 'n' Play callback handler set by ApiSetPnpCallback */
    if(v_MilPnPCallback != (TY_PNP_CALLBACK_FUNC_PTR) NULL)
    {
        (v_MilPnPCallback)(p_Device->ul_ModuleNo, 1L);
    }

    if(p_Device->_hDrv != INVALID_HANDLE_VALUE)
    {
        fclose((FILE*) p_Device->_hDrv);
        p_Device->_hDrv = INVALID_HANDLE_VALUE;
    }

    return _ApiDeleteMilDeviceListInfo(NULL, p_Device);

}


/* This function is used to define OS specific members of the device info
 * structure.
 * Parameters:  TY_DEVICE_INFO* - pointer to the device info struct to initialize
 */
void _ApiInitializeDeviceOsSpecific(TY_DEVICE_INFO* p_DeviceInfo)
{
}


/* This function sets OS specific values of device configuration
   e.g. DMA capability or number of interrupt requests */
void _ApiSetDeviceInfoConfigOsSpecific(TY_DEVICE_INFO* p_DeviceInfo)
{
    AiReturn ret = 0;
    AiUInt32 ulFpgaCoreVersion = 0;
    AiUInt32 ulDmaConfig       = 0;

    if( p_DeviceInfo->uc_DeviceGroup == AI_DEVICE_AYX )
    {
        p_DeviceInfo->x_Config.uc_DmaEnabled     = 3; /* GLOB, SHARED */
        p_DeviceInfo->x_Config.ul_DmaMinimumSize = 0x1000; /* 4 KB */
    }

    if(p_DeviceInfo->uc_DeviceGroup == AI_DEVICE_AYE)
    {
        if( 0 == ApiReadMemData( p_DeviceInfo->ul_ModuleNo, API_MEMTYPE_GLOBAL_EXTENSION, MYMON_GREG_VERSION_PCIE, 4, &ulFpgaCoreVersion ) )
        {
            if( ulFpgaCoreVersion > 0x01050000 )
            {
                p_DeviceInfo->x_Config.uc_DmaEnabled     = 1; /* GLOB */
                p_DeviceInfo->x_Config.ul_DmaMinimumSize = 0x1000; /* 4 KB */
            }
        }
    }

    /* Enable DMA transfers for ZynqMP boards */
    if (p_DeviceInfo->uc_DeviceGroup == AI_DEVICE_ZYNQMP_ASP)
    {
        if (IsDriverVersionGreaterOrEqual(p_DeviceInfo, 14, 3))
        {
            p_DeviceInfo->x_Config.uc_DmaEnabled     = 3; /* global=bit0, shared=bit1 */
            p_DeviceInfo->x_Config.ul_DmaMinimumSize = 4*1024; /* 4 KB */
        }
    }

    if(p_DeviceInfo->uc_DeviceGroup == AI_DEVICE_ARTIXUS)
    {
        ret = ApiReadMemData(p_DeviceInfo->ul_ModuleNo, API_MEMTYPE_IO, ARTIXUS_DMA_CONFIG*4, 4, &ulDmaConfig);

        if ((ret == API_OK) && (ulDmaConfig != 0xAFFECAFE) && ((ulDmaConfig & ARTIXUS_DMA_CONFIG_C2H_MASK) >= 1))
        {
            p_DeviceInfo->x_Config.uc_DmaEnabled = 1; /* global=bit0, shared=bit1 */
            p_DeviceInfo->x_Config.ul_DmaMinimumSize = 4 * 1024; /* 4 KB */
        }
    }
}


AiInt16 _ApiOsSetDeviceConfigDriverFlags( AiUInt32 ulModHandle, AiUInt32 ulDriverFlags )
{
    TY_DEVICE_INFO * pDevice  = _ApiGetDeviceInfoPtrByModule( ulModHandle );
    int i_DeviceFileDescriptor;
    int i_Retval = API_OK;

    /*
     * Driver flags :
     * Bit  0 : Enable interrupts if set
     * Bit  1 : Disable interrupts if set */

      if( pDevice == NULL )
          return API_ERR_NO_MODULE_EXTENSION;

      if(pDevice->_hDrv == INVALID_HANDLE_VALUE)
      {
          fprintf(stderr, "_LinuxSetDeviceConfigDriverFlags: Invalid file handle for module 0x%x\n", ulModHandle);
          return API_ERR;
      }

      i_DeviceFileDescriptor = fileno((FILE*) pDevice->_hDrv);
      if (i_DeviceFileDescriptor == -1)
      {
          perror("_LinuxSetDeviceConfigDriverFlags");
          return API_ERR;
      }

      if( (ulDriverFlags & 0x00000001) != 0 )
      {
          // enable interrupts
          i_Retval = ioctl(i_DeviceFileDescriptor, AIM_IOCTL_DEACTIVATE_IRQ, AiFalse);
      }

      if( (ulDriverFlags & 0x00000002) != 0)
      {
          i_Retval = ioctl(i_DeviceFileDescriptor, AIM_IOCTL_DEACTIVATE_IRQ, AiTrue);
      }

      if(i_Retval)
      {
          perror("_LinuxSetDeviceConfigDriverFlags");
          return API_ERR_IOCTL_ERROR;
      }

      return API_OK;
}


AI_LIB_FUNC AiReturn AI_CALL_CONV _ApiWriteUsbData( AiUInt32 ulModHandle, AiUInt8 memtype, AiUInt32 offset, AiUInt32 size, void* data, AiUInt32* p_BytesWritten)
{
    return API_ERR_PLATTFORM_NOT_SUPPORTED;
}


AI_LIB_FUNC AiReturn AI_CALL_CONV _ApiReadUsbData( AiUInt32 ulModHandle, AiUInt8 memtype, AiUInt32 offset, AiUInt32 size, void* data, AiUInt32* p_BytesRead)
{
    return API_ERR_PLATTFORM_NOT_SUPPORTED;
}


AI_LIB_FUNC AiReturn AI_CALL_CONV _ApiOsExit(void)
{
    _ApiOsStopDeviceNotification();

    _ApiClearDeviceList();

    return API_OK;
}

AI_LIB_FUNC AiReturn AI_CALL_CONV _ApiOsProvideScopeBuffers (AiUInt32 ulModuleHandle, AiUInt32 ulNumBuffers, TY_API_SCOPE_BUFFER* axScopeBuffers[], RPC_ASYNC_STATE *RPCScopeData_AsyncHandle)
{
  TY_DEVICE_INFO* p_Device = NULL;
  int i_DeviceFileDescriptor;
  int ret = 0;
  unsigned int i = 0;

  TY_MILSCOPE_BUFFER            x_MilscopeBuffer;
  TY_SCOPE_BUFFER_LIST_ELEMENT *px_currScopeBufferListElem;
  TY_SCOPE_BUFFER_LIST_ELEMENT *px_newScopeBufferListElem;

  /* -- prepare driver handle --- */

  p_Device = _ApiGetDeviceInfoPtrByModule(ulModuleHandle);

  if( p_Device == NULL )
      return API_ERR_NO_MODULE_EXTENSION;

  i_DeviceFileDescriptor = fileno((FILE*) p_Device->_hDrv);

  if(i_DeviceFileDescriptor == -1)
  {
      perror("Error: _ApiOsProvideScopeBuffers no valid device file descriptor");
      return API_ERR_FUNCTION_NOT_SUPPORTED;
  }


    /* -- Save buffer in DLL list and provide them to the driver --- */
  ret = pthread_mutex_lock( &p_Device->ScopeBuffListCriticalSect );

  if( ret != 0 )
  {
      perror("Error: _ApiOsProvideScopeBuffers failed to lock mutex" );
      return API_ERR_FAILED_TO_CREATE_MUTEX;
  }




  for( i =0; i<ulNumBuffers; i++ )
  {
      px_newScopeBufferListElem = (TY_SCOPE_BUFFER_LIST_ELEMENT*)calloc(1, sizeof(TY_SCOPE_BUFFER_LIST_ELEMENT));

      if( px_newScopeBufferListElem == NULL )
      {
          ret = API_ERR_MALLOC_FAILED;
          break;
      }

      /* Reset flags */
      axScopeBuffers[i]->ulFlags    = 0;
      axScopeBuffers[i]->ulDataSize = 0;

      px_newScopeBufferListElem->p_ApiScopeBuffer = axScopeBuffers[i];

      /* append new buffer element to end of list */
      /* in case list empty */
      if( NULL == p_Device->px_ScopeBufferLHead){
          p_Device->px_ScopeBufferLHead = px_newScopeBufferListElem;
          p_Device->px_ScopeBufferLTail = px_newScopeBufferListElem;
      } else {
          /* get end of the list */
          px_currScopeBufferListElem = p_Device->px_ScopeBufferLTail;

          /* set new end of list */
          px_currScopeBufferListElem->p_Next = px_newScopeBufferListElem;
          px_newScopeBufferListElem->p_Prev  = p_Device->px_ScopeBufferLTail;
          p_Device->px_ScopeBufferLTail      = px_newScopeBufferListElem;
      }

      /* set buffer values */
      x_MilscopeBuffer.ulSize            = (AiUInt32)axScopeBuffers[i]->ulBufferSize;
      x_MilscopeBuffer.ullVirtualAddress = (AiUInt64)axScopeBuffers[i]->pvBuffer;
      x_MilscopeBuffer.eType             = (TY_API_SCOPE_BUFFER_TYPE)axScopeBuffers[i]->eBufferType;

      ret = ioctl(i_DeviceFileDescriptor, AIM_IOCTL_SCOPE_BUFFERS_PROVIDE, &x_MilscopeBuffer );

      if(ret)
      {
          perror("failed to provide for scope buffers");
          ret = API_ERR;
          break;
      }

      p_Device->ulNumBuffersLeft++;
  }



  if( pthread_mutex_unlock( &p_Device->ScopeBuffListCriticalSect ) != 0 )
  {
      perror("Error: failed to unlock mutex");
  }

  if( ret != 0 )
  {
      perror("Error:_ApiOsProvideScopeBuffers failed");
      return ret;
  }

  /* -- Wait until all buffers are written to the FIFO or the FIFO is full --- */
  _ApiOsProvideScopeBuffersWait( ulModuleHandle );


  return API_OK;
}

AI_LIB_FUNC AiReturn AI_CALL_CONV _ApiOsProvideScopeBuffersWait(AiUInt32 ulModuleHandle )
{
  TY_DEVICE_INFO* p_Device = NULL;
  int i_DeviceFileDescriptor;
  int ret = 0;

  p_Device = _ApiGetDeviceInfoPtrByModule(ulModuleHandle);

  if( p_Device == NULL )
      return API_ERR_NO_MODULE_EXTENSION;

  i_DeviceFileDescriptor = fileno((FILE*) p_Device->_hDrv);

  if(i_DeviceFileDescriptor == -1)
  {
      perror("Error: _ApiOsProvideScopeBuffers no valid device file descriptor");
      return API_ERR_FUNCTION_NOT_SUPPORTED;
  }


  ret = ioctl(i_DeviceFileDescriptor, AIM_IOCTL_SCOPE_BUFFERS_WAIT, 0 );


  if(ret)
  {
      perror("failed to wait for scope buffers written to the hardware");
      return API_ERR;
  }

  return API_OK;
}

AI_LIB_FUNC AiReturn AI_CALL_CONV _ApiOsProvideScopeBuffersSync( AiUInt32 ulModuleHandle, TY_API_SCOPE_BUFFER_FLAGS * pFlags )
{
  TY_DEVICE_INFO* p_Device;
  int i_DeviceFileDescriptor;
  unsigned int flags;
  int ret = 0;

  p_Device = _ApiGetDeviceInfoPtrByModule(ulModuleHandle);

  if( p_Device == NULL )
      return API_ERR_NO_MODULE_EXTENSION;

  i_DeviceFileDescriptor = fileno((FILE*) p_Device->_hDrv);

  if(i_DeviceFileDescriptor == -1)
  {
      perror("Error: _ApiOsProvideScopeBuffers no valid device file descriptor");
      return API_ERR_FUNCTION_NOT_SUPPORTED;
  }


  ret = ioctl(i_DeviceFileDescriptor, AIM_IOCTL_SCOPE_BUFFERS_SYNC, &flags );

  if(ret)
  {
      perror("failed to wait for scope buffers finished");
      return API_ERR;
  }
  else
  {
      if(pFlags) *pFlags = flags;
  }


  return API_OK;
}

AI_LIB_FUNC AiReturn AI_CALL_CONV _ApiOsReleaseScopeBuffers (AiUInt32 ulModuleHandle)
{
  TY_DEVICE_INFO* p_Device = NULL;
  int i_DeviceFileDescriptor;
  int ret = 0;

  p_Device = _ApiGetDeviceInfoPtrByModule(ulModuleHandle);

  if( p_Device == NULL )
      return API_ERR_NO_MODULE_EXTENSION;

  i_DeviceFileDescriptor = fileno((FILE*) p_Device->_hDrv);

  if(i_DeviceFileDescriptor == -1)
  {
      perror("Error: _ApiOsProvideScopeBuffers no valid device file descriptor");
      return API_ERR_FUNCTION_NOT_SUPPORTED;
  }

  ret = ioctl(i_DeviceFileDescriptor, AIM_IOCTL_SCOPE_BUFFERS_RELEASE, 0 );

  if(ret)
  {
      perror("failed to release scope buffers");
      return API_ERR;
  }



  return API_OK;
}

AiInt16 _ApiOsCmdScopeSetup(AiUInt32 ul_ModuleHandle)
{
  int ret = API_OK;

  TY_DEVICE_INFO * pDev = _ApiGetDeviceInfoPtrByModule(ul_ModuleHandle);

  /* -- Lock thread state --- */
  pthread_mutex_lock( &pDev->ScopeThreadStateLock );


  do{
    if( pDev->pScopeThread == NULL ){
        /* Create scope thread */
        pDev->pScopeThread = (pthread_t*) malloc( sizeof(pthread_t) );

        if( pDev->pScopeThread == NULL ){
          ret = API_ERR_MALLOC_FAILED;
          break;
        }

        pDev->scopeThreadState = ScopeThreadSuspended;
        pDev->ulModHandle = ul_ModuleHandle;

        ret = pthread_create( pDev->pScopeThread, NULL, _ApiOsScopeThread, &pDev->ulModHandle );

        if( ret != 0 ){
            perror("Error: _ApiOsCmdScopeSetup failed to create thread");
            ret = API_ERR_CREATE_EVENT;
            break;
        }

    }
  }while(0);

  /* -- Free thread state --- */
  pthread_mutex_unlock( &pDev->ScopeThreadStateLock );

  return ret;
}

AiInt16 _ApiOsCmdScopeStart(AiUInt32 ul_ModuleHandle)
{
  int ret;
  TY_DEVICE_INFO * pDev = _ApiGetDeviceInfoPtrByModule(ul_ModuleHandle);

  if( pDev->pScopeThread == NULL ){
      perror("Error: _ApiOsCmdScopeSetup was not executed");
      return API_ERR_INVALID_DEVICE_STATE;
  }


  /* -- Lock thread state --- */
  ret = pthread_mutex_lock( &pDev->ScopeThreadStateLock );

  if( ret != 0 )
  {
      perror("Error: _ApiOsCmdScopeStart failed to lock mutex" );
      return API_ERR_FAILED_TO_CREATE_MUTEX;
  }

  /* -- signal wake up event --- */

  if( pDev->scopeThreadState == ScopeThreadSuspended ){

      pDev->scopeThreadState = ScopeThreadStarted;

      pthread_cond_signal(&pDev->hScopeThreadWakeEvent);
  }

  /* -- release thead state --- */

  ret = pthread_mutex_unlock( &pDev->ScopeThreadStateLock );

  if( ret != 0 )
  {
      perror("Error: _ApiOsCmdScopeStart failed to unlock mutex" );
      return API_ERR_FAILED_TO_CREATE_MUTEX;
  }


  return API_OK;
}


AI_LIB_FUNC AiReturn AI_CALL_CONV _ApiOsCmdScopeStatus(AiUInt32 ulModuleHandle, AiUInt32* pulNumBuffersLeft )
{
  TY_DEVICE_INFO* pDevice = _ApiGetDeviceInfoPtrByModule(ulModuleHandle);

  if( pDevice == NULL )
      return API_ERR_NO_MODULE_EXTENSION;

  if( pulNumBuffersLeft == NULL )
    return API_ERR_PARAM1_IS_NULL;

  /* -- Lock buffer list --- */
  pthread_mutex_lock( &pDevice->ScopeBuffListCriticalSect );


  *pulNumBuffersLeft = pDevice->ulNumBuffersLeft;

  /* -- Free buffer list --- */
  pthread_mutex_unlock( &pDevice->ScopeBuffListCriticalSect );


  return API_OK;
}

void * _ApiOsScopeThread( void * arg )
{
  int ret;
  unsigned int i;
  AiUInt32  ulModHandle           = 0;
  AiBoolean bExit                 = AiFalse;
  AiBoolean bSuspend              = AiFalse;
  AiBoolean bFirstPrimaryBuffer   = AiTrue;
  AiBoolean bFirstSecondaryBuffer = AiTrue;

  TY_SCOPE_BUFFER_LIST_ELEMENT *px_currScopeBufferListElem;
  TY_API_SCOPE_BUFFER_FLAGS    flags;
  AiUInt32                     ulCompleteNumBuffers = 0;
  TY_DEVICE_INFO * pDev   = NULL;

  if( arg != NULL ){
      ulModHandle = *(AiUInt32*)arg;

      pDev = _ApiGetDeviceInfoPtrByModule(ulModHandle);

      if( pDev == NULL ){
          ret = API_ERR;
          bExit = AiTrue;
      }
  }else{
      ret = API_ERR;
      bExit = AiTrue;
  }

  while( !bExit ){
      /* -- Lock thread state --- */
      pthread_mutex_lock( &pDev->ScopeThreadStateLock );

      while( pDev->scopeThreadState == ScopeThreadSuspended ){

          ret = pthread_cond_wait( &pDev->hScopeThreadWakeEvent, &pDev->ScopeThreadStateLock );

        if( ret != 0 ){
          LOG(DBG_ERROR, fncname, ("wait for wake event failed\n"))
        }
      }

      if( pDev->scopeThreadState == ScopeThreadExited ){
          bExit = AiTrue;
          pthread_mutex_unlock( &pDev->ScopeThreadStateLock );
          continue;
      }

      /* -- release mutex --- */
      pthread_mutex_unlock( &pDev->ScopeThreadStateLock );


      bSuspend              = AiFalse;
      bFirstPrimaryBuffer   = AiTrue;
      bFirstSecondaryBuffer = AiTrue;

      while( !bSuspend ){
          /* -- Lock thread state --- */
          pthread_mutex_lock( &pDev->ScopeBuffListCriticalSect );

          px_currScopeBufferListElem = pDev->px_ScopeBufferLHead;

          /* -- release mutex --- */
          pthread_mutex_unlock( &pDev->ScopeBuffListCriticalSect );

          if( px_currScopeBufferListElem != NULL ){

              /* Wait for buffer beeing completed */
              ret = _ApiOsProvideScopeBuffersSync( pDev->ul_ModuleNo, &flags );

              if( ret == API_OK ){
                  if( flags & SCOPE_BUFFER_FLAG_CANCELED ){

                      _ApiOsReleaseScopeBuffers( pDev->ul_ModuleNo );

                      /* buffers have been canceled clean up all buffers */
                      ulCompleteNumBuffers = pDev->ulNumBuffersLeft;
                  } else{
                      /* Buffer has valid data pricess only one buffer */
                      ulCompleteNumBuffers = 1;
                  }

                  for( i=0; i<ulCompleteNumBuffers; i++ ){
                      /* -- Lock thread state --- */
                      pthread_mutex_lock( &pDev->ScopeBuffListCriticalSect );


                      px_currScopeBufferListElem = pDev->px_ScopeBufferLHead;

                      if( px_currScopeBufferListElem != NULL ){
                          pDev->px_ScopeBufferLHead = px_currScopeBufferListElem->p_Next;

                          if(pDev->px_ScopeBufferLHead)
                            pDev->px_ScopeBufferLHead->p_Prev = NULL;
                          else
                            pDev->px_ScopeBufferLTail = NULL;

                          pDev->ulNumBuffersLeft--;

                      }else{
                          bSuspend = AiTrue;
                      }

                      /* -- release mutex --- */
                      pthread_mutex_unlock( &pDev->ScopeBuffListCriticalSect );

                      /* We have no access to the module hande used to open this stream
                       * the scope is however always on stream 1 so we can fake the handle
                       * here. */
                      ulModHandle = (1<<API_STREAM_POS) + pDev->ul_ModuleNo;

                      if( px_currScopeBufferListElem != NULL ){
                          if( bFirstPrimaryBuffer && px_currScopeBufferListElem->p_ApiScopeBuffer->eBufferType == SCOPE_BUFFER_PRIMARY ){
                              _ApiOsProcessScopeBuffer( ulModHandle, px_currScopeBufferListElem, flags, AiTrue );
                              bFirstPrimaryBuffer = AiFalse;
                          } else if( bFirstSecondaryBuffer && px_currScopeBufferListElem->p_ApiScopeBuffer->eBufferType == SCOPE_BUFFER_SECONDARY ){
                              _ApiOsProcessScopeBuffer( ulModHandle, px_currScopeBufferListElem, flags, AiTrue );
                              bFirstSecondaryBuffer = AiFalse;
                          }
                          else{
                              _ApiOsProcessScopeBuffer(ulModHandle, px_currScopeBufferListElem, flags, AiFalse );
                          }

                          free( px_currScopeBufferListElem );
                          px_currScopeBufferListElem = NULL;
                      }


                  }

              } else {
                  bSuspend = AiTrue;
              }
          } else  {
              bSuspend = AiTrue;
          }
      }



      pthread_mutex_lock( &pDev->ScopeThreadStateLock );

      pDev->scopeThreadState = ScopeThreadSuspended;
      /* Signal thread suspended */
      pthread_cond_broadcast( &pDev->hScopeThreadSuspendEvent );

      pthread_mutex_unlock( &pDev->ScopeThreadStateLock );
  }


  /* finally clean up thread and signal suspended */

  pthread_exit( (void*)&ret );

  return NULL;


}

AiInt16 _ApiOsCmdScopeStop(AiUInt32 ul_ModuleHandle)
{
  AiInt16 ret = API_ERR;
  TY_DEVICE_INFO* pDev = _ApiGetDeviceInfoPtrByModule(ul_ModuleHandle);
  TY_SCOPE_BUFFER_LIST_ELEMENT *px_currScopeBufferListElem;


  if( pDev == NULL )
      return API_ERR_NO_MODULE_EXTENSION;

  do{
      ret = _ApiOsReleaseScopeBuffers( ul_ModuleHandle );

       if( ret != API_OK ) {
          LOG(DBG_ERROR, fncname, ("_ApiOsReleaseScopeBuffers failed\n"))
          break;
      }

      if( pDev->pScopeThread == NULL ){
          LOG(DBG_ERROR, fncname, ("pScopeThread not initialized\n"))
          ret = API_OK;
          break;
      }


      /* -- Lock thread state --- */
      pthread_mutex_lock( &pDev->ScopeThreadStateLock );

      while( pDev->scopeThreadState == ScopeThreadStarted ){
          /* wait for thread suspended */
          pthread_cond_wait( &pDev->hScopeThreadSuspendEvent, &pDev->ScopeThreadStateLock );
      }

      /* -- Free thread state --- */
      pthread_mutex_unlock( &pDev->ScopeThreadStateLock );


  }
  while(0);


  /* -- Lock buffer list --- */
  pthread_mutex_lock( &pDev->ScopeBuffListCriticalSect );

  while( pDev->ulNumBuffersLeft > 0 ){

      if( NULL == pDev->px_ScopeBufferLHead ){
        px_currScopeBufferListElem = pDev->px_ScopeBufferLHead;
        pDev->px_ScopeBufferLHead = px_currScopeBufferListElem->p_Next;

        free(px_currScopeBufferListElem);

        pDev->ulNumBuffersLeft--;

      } else {
          pDev->ulNumBuffersLeft = 0;
      }
  }


  pDev->px_ScopeBufferLHead = NULL;
  pDev->px_ScopeBufferLTail = NULL;

  /* -- Free buffer list --- */
  pthread_mutex_unlock( &pDev->ScopeBuffListCriticalSect );


  return ret;
}


void _ApiOsProcessScopeBuffer(AiUInt32 ul_ModuleHandle, TY_SCOPE_BUFFER_LIST_ELEMENT* p_ScopeBufferListElement, TY_API_SCOPE_BUFFER_FLAGS flags, AiBoolean bTriggerBuffer)
{
    TY_API_SCOPE_BUFFER* p_ScopeBuffer = NULL;
    AiUInt32 trigger_id = 0;
    AiUInt32 status_register = 0;
    AiUInt32 trigger_package_id = 0;
    AiBoolean bBufferFull = FALSE;
    AiUInt32 sample_id_last_package = 0;



    if(    !p_ScopeBufferListElement
        || !p_ScopeBufferListElement->p_ApiScopeBuffer
        || !p_ScopeBufferListElement->p_ApiScopeBuffer->BufferHandler )
    {
        return;
    }

    p_ScopeBuffer = p_ScopeBufferListElement->p_ApiScopeBuffer;

    p_ScopeBuffer->ulFlags = flags;

    if( p_ScopeBuffer->ulFlags & SCOPE_BUFFER_FLAG_CANCELED )
    {
        p_ScopeBuffer->ulDataSize = 0;
        p_ScopeBuffer->BufferHandler(ul_ModuleHandle, p_ScopeBuffer);
    }
    else
    {
        p_ScopeBuffer->ulFlags |= SCOPE_BUFFER_FLAG_FILLED;

        if(bTriggerBuffer)
        {
            p_ScopeBuffer->ulFlags |= SCOPE_BUFFER_FLAG_TRIGGER;

            ApiReadMemData(ul_ModuleHandle, API_MEMTYPE_IO, 0x2148, 4, &trigger_id);
            ApiReadMemData(ul_ModuleHandle, API_MEMTYPE_IO, 0x2140, 4, &status_register);

            /* Check if the buffer was at least once completely filled */
            bBufferFull = (status_register >> 8) & 0x1;
            trigger_package_id = trigger_id / 3;

            if(bBufferFull)
            {
                /* complete data in buffer is valid */
                p_ScopeBuffer->ulDataSize = p_ScopeBuffer->ulBufferSize;

                /* As buffer is completely filled, trigger sample is in last sample package
                   of the buffer. Use trigger ID modulo 3 to determine in which one of the 3 samples of the package the trigger is */

                sample_id_last_package = ((p_ScopeBuffer->ulBufferSize / 4) * 3) - 3; /* ID of first sample in last package of buffer */
                p_ScopeBuffer->ulTriggerId = sample_id_last_package + trigger_id % 3;
            }
            else
            {
                /* trigger sample is in last valid sample package, so we can determine
                   the valid data size */
                p_ScopeBuffer->ulDataSize = trigger_package_id * sizeof(AiUInt32) + 4;

                /* trigger sample ID equals the one read from hardware, as buffer did not
                   wrap around so far. */
                p_ScopeBuffer->ulTriggerId = trigger_id;
            }
        }
        else
        {
            /* buffers that are no trigger buffers (not the first one of a channel),
               should always contain only valid data */
            p_ScopeBuffer->ulDataSize = p_ScopeBuffer->ulBufferSize;
        }

        p_ScopeBuffer->BufferHandler(ul_ModuleHandle, p_ScopeBuffer);
    }

    return;
}

AiInt16 _ApiOsWaitForScopeFinished(AiUInt32 ulModuleHandle, AiInt32 lMsTimeOut,
                                   TY_API_SCOPE_WAIT_STATE* p_WaitResultFlags)
{
  TY_DEVICE_INFO * pDev = _ApiGetDeviceInfoPtrByModule(ulModuleHandle);
  struct timespec timeout;
  AiInt16 ret_val = API_OK;
  int     wait_result = 0;

  if( pDev == NULL )
    return API_ERR_NO_MODULE_EXTENSION;

  if( p_WaitResultFlags == NULL )
    return API_ERR_PARAM3_IS_NULL;

  if( pDev->pScopeThread == NULL )
    return API_ERR_NO_SCOPE_EXECUTED;

  *p_WaitResultFlags = 0;

  /* -- Lock thread state --- */
  pthread_mutex_lock( &pDev->ScopeThreadStateLock );


  if( pDev->scopeThreadState == ScopeThreadStarted ){

      if( lMsTimeOut < 0 ){
        /* wait infinit for thread suspended */
        wait_result = pthread_cond_wait( &pDev->hScopeThreadSuspendEvent, &pDev->ScopeThreadStateLock );
      } else {

        ret_val = clock_gettime( CLOCK_REALTIME, &timeout );

        if( ret_val == 0 ){
          timeout.tv_sec  +=  (lMsTimeOut / 1000);
          timeout.tv_nsec += ((lMsTimeOut % 1000) * 1000000);

          if( timeout.tv_nsec >= 1000000000 ){
              timeout.tv_sec++;
              timeout.tv_nsec -= 1000000000;
          }

          /* wait with time out for thread suspended */
          wait_result = pthread_cond_timedwait( &pDev->hScopeThreadSuspendEvent, &pDev->ScopeThreadStateLock, &timeout );
        } else {
            wait_result = EINVAL;
        }
      }

      if(wait_result == 0 ){

        /* wait at most 1 second until scope status is really idle */
        ret_val = _ApiOsWaitForScopeStatusIdle( ulModuleHandle, 1000 );
        *p_WaitResultFlags |= SCOPE_WAIT_FINISHED;
        ret_val = API_OK;

      }else if(wait_result == ETIMEDOUT ){

        *p_WaitResultFlags |= SCOPE_WAIT_TIMEOUT;
        ret_val = API_OK;

      }else {
          LOG(DBG_ERROR, fncname, ("Invalid value for conditional wait\n"))
          ret_val = API_ERR;
      }

  }
  else{
      *p_WaitResultFlags |= SCOPE_WAIT_FINISHED;
  }


  pthread_mutex_unlock( &pDev->ScopeThreadStateLock );


  return ret_val;
}

AiUInt16 _ApiOsWaitForScopeStatusIdle(AiUInt32 ulModuleHandle, AiUInt32 ulMsTimeOut)
{
  clock_t start;
  unsigned long diff_time = 0;
  AiInt16 ret_val = API_OK;

  TY_API_SCOPE_STATUS scope_status = SCOPE_STATUS_STOPPED;


  start = clock();

  do{
      ret_val = ApiCmdScopeStatus( ulModuleHandle, &scope_status, NULL );

      if( ret_val != API_OK )
        break;

      if(    (scope_status == SCOPE_STATUS_STOPPED )
          || (scope_status == SCOPE_STATUS_OVERFLOW) )
          break;

      diff_time = ( clock() - start ) * 1000 / CLOCKS_PER_SEC;

  }while( diff_time < ulMsTimeOut );

  if( diff_time >= ulMsTimeOut ){
      LOG(DBG_WARN, fncname, ("Wait timeout\n"))
      ret_val = API_ERR;
  }


  return ret_val;
}



/*! \brief OS specific function to flush the system driver interrupt queue.

    \param ul_ModuleHandle module handle of the board where scope is setup
    \return returns API_OK on success, an appropriate error code on failure*/
AiInt16 _ApiOsFlushInterrupts(AiUInt32 ul_ModuleHandle)
{
	/* Not neccessary with NETLINK solution */
    return API_OK;
}


AiInt16 _ApiOsGetDriverInfo(AiUInt32 ul_ModuleHandle, TY_API_DRIVER_INFO * pxDriverInfo )
{
  TY_DEVICE_INFO* p_Device = NULL;
  int i_DeviceFileDescriptor;

  if( pxDriverInfo == NULL )
  {
      return API_OK;
  }

  p_Device = _ApiGetDeviceInfoPtrByModule(ul_ModuleHandle);
  if (!p_Device)
  {
      return API_ERR_NO_MODULE_EXTENSION;
  }

  if (p_Device->_hDrv == INVALID_HANDLE_VALUE)
  {
      return API_ERR_INVALID_HANDLE;
  }

  i_DeviceFileDescriptor = fileno((FILE*) p_Device->_hDrv);

  if (i_DeviceFileDescriptor == -1)
  {
      perror("_ApiOsGetDriverInfo");
      return API_ERR;
  }

  if (ioctl(i_DeviceFileDescriptor, AIM_IOCTL_GENERIC_BOARD_INFO, pxDriverInfo) == -1)
  {
      perror("ioctl AIM_READ_DRIVER_INFO");
      return(API_ERR_IOCTL_ERROR);
  }


  return API_OK;
}


AiReturn _ApiOsIoChannelCommand(TY_DEVICE_INFO* device, int comChannel, void* command, size_t size,
                                  void* response, size_t expectedResponseSize)
{
    int fd;
    int ret;
    struct aim_pci_com_chn_cmd channel_command;

    if(!command)
    {
        return API_ERR_PARAM3_IS_NULL;
    }

    if( device == NULL )
    {
        return( API_ERR_NO_MODULE_EXTENSION );
    }

    if(device->_hDrv == INVALID_HANDLE_VALUE)
    {
        return API_ERR_INVALID_DRIVER_HANDLE;
    }

    fd = fileno((FILE*)device->_hDrv);
    if(fd == -1)
    {
        DEBUGOUT(DBG_ERROR, __FUNCTION__, strerror(errno));
        return API_ERR_INVALID_DRIVER_HANDLE;
    }

    channel_command.id = comChannel;
    channel_command.commandParams.command = command;
    channel_command.commandParams.cmdSize = size;
    channel_command.commandParams.acknowledge = response;
    channel_command.commandParams.expectedAckSize = expectedResponseSize;

    ret = ioctl(fd, AIM_IOCTL_PCI_COM_CHN_CMD, &channel_command);
    if(ret)
    {
        DEBUGOUT(DBG_ERROR, __FUNCTION__, strerror(errno));
        return API_ERR_IOCTL_ERROR;
    }

    return API_OK;
}


AiReturn _ApiOsInitDeviceStruct( TY_DEVICE_INFO * pDevice )
{
    if(!pDevice->os_info)
    {
        pDevice->os_info = (struct device_info_os*) malloc(sizeof(struct device_info_os));

        if(!pDevice->os_info)
        {
            return API_ERR_MALLOC_FAILED;
        }

        pDevice->os_info->notification_thread = 0;
        pthread_mutex_init(&pDevice->os_info->notification_lock, NULL);
    }
    return API_OK;
}

void _ApiOsFreeDeviceStruct( TY_DEVICE_INFO* pDevice )
{
    if(pDevice->os_info)
    {
        free(pDevice->os_info);
        pDevice->os_info = NULL;
    }
}

