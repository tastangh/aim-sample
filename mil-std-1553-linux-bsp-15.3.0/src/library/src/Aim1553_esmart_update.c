/* SPDX-FileCopyrightText: 2016-2022 AIM GmbH <info@aim-online.com> */
/* SPDX-License-Identifier: MIT */

/*! \file Aim1553_esmart_udpate.cpp
 *
 *  This file contains implementations necessary for updating
 *  AIM devices with a eSmart upate interface.
 *
 *  Adapted from: AiFcUpdate.cpp
 */


#include "AiOs.h"
#include "Api1553.h"
#include "esmart.h"

#ifdef _AIM_WINDOWS
  #include "AiMPW_io.h"
#endif

#ifdef _AIM_LINUX
  #include "Aim1553_io.h"
#endif

#ifdef __VXWORKS__
  #include "Aim1553_io.h"
  #include <vxWorks.h>
  #include <ioLib.h>
  #include <sys/stat.h>
#endif

#include "Dll_Fnc.h"

static void INLINE(api_esmart_setup_req_header(struct esmart_request* header, AiUInt32 request_id, AiUInt32 request_size))
{
    header->magic = ESMART_MAGIC;
    header->request_id = request_id;
    header->request_size = request_size;

}




/*! \brief Maps eSmart errors to FC API errors
 *
 * @param the eSmart status to map
 * @return the corresponding FC API error
 */
static AiInt16 INLINE(api_esmart_map_status(int esmartStatus))
{
    AiReturn ret = API_ERR;

    switch(esmartStatus)
    {
    case ESMART_OK:
        ret = 0;
        break;
    case ESMART_FILE_EOF:
        ret = 0;
        break;
    case ESMART_INVALID_SESSION:
    case ESMART_INVALID_FRAME_SIZE:
    case ESMART_INVALID_REQUEST:
    case ESMART_INVALID_TRANSFER_DIR:
        ret = API_ERR;
        break;
    case ESMART_FILE_WRITE_ERROR:
    case ESMART_FILE_READ_ERROR:
    case ESMART_FILE_NO_ACCESS:
        ret = API_ERR_INVALID_DEVICE_STATE;
        break;
    case ESMART_FILE_IN_USE:
    case ESMART_FILE_RELEASE_ERROR:
        ret = API_ERR_INVALID_DEVICE_STATE;
        break;
    case ESMART_MEMORY_ERROR:
        ret = API_ERR_MALLOC_FAILED;
        break;
    default:
        ret = API_ERR;
    }

    return ret;
}




/*! \brief Issue an eSmart request
 *
 * This function will issue a eSmart request on a given device.
 * It will perform some basic response checking e.g. magic and size
 * and return the response to caller.
 * @param boardHandle the handle to the board where request shall be issued on
 * @param request pointer to request data. Must be correctly initialized
 * @param response  pointer to buffer that will receive response data. Must be at least of size \ref expectedResponseSize
 * @param expectedResponseSize expected size of response data in bytes
 * @return returns 0 on success, an error code on failure
 */
static int api_esmart_issue_request(AiUInt32 module, struct esmart_request* request, struct esmart_response* response,
                                   size_t expectedResponseSize)
{
    AiReturn ret = 0;
    TY_DEVICE_INFO * device_info;

    if(!request || !response)
    {
        return API_ERR_PARAM1_IS_NULL;
    }

    /* Check if we have a local or remote device */
    if( GET_SERVER_ID(module) )
    {
        return API_ERR_PLATTFORM_NOT_SUPPORTED;
    }

    device_info = _ApiGetDeviceInfoPtrByModule( module );

    if( device_info == NULL )
        return( API_ERR_NO_MODULE_EXTENSION );

    ret = _ApiOsIoChannelCommand(device_info, ESMART_HOSTIO_CHANNEL, request, request->request_size, response, expectedResponseSize);

    if(ret)
    {
        DEBUGOUT(DBG_ERROR, __FUNCTION__, "PCI channel command failed");
        return ret;
    }

    if(response->magic != ESMART_MAGIC || response->response_size < sizeof(struct esmart_response ))
    {
        DEBUGOUT(DBG_ERROR, __FUNCTION__, "Invalid response header in eSmart response");
        return API_ERR;
    }

    if(response->request_id != request->request_id)
    {
        DEBUGOUT(DBG_ERROR, __FUNCTION__, "Request ID not matching in response ");
        return API_ERR;
    }

    if(response->response_size > expectedResponseSize)
    {
        DEBUGOUT(DBG_ERROR, __FUNCTION__, "Response too large");
        return API_ERR;
    }

    return 0;
}


/*! \brief Download data chunk to device via eSmart
 *
 *
 * @param boardHandle handle to the board the chunk will be send to
 * @param sessionID The session id of the eSmart file transmission to use
 * @param data pointer to raw data to download
 * @param size size of data to download in bytes
 * @return returns 0 on success, a error code otherwise
 */
static int api_esmart_download_chunk(AiUInt32 module, int sessionID, void* data, size_t size)
{
    struct esmart_send_chunk_req* request = NULL;
    struct esmart_send_chunk_resp response;
    int ret = 0;
    size_t dataOffset = 0;

    request = (struct esmart_send_chunk_req*) malloc(esmart_send_chunk_req_size(size));
    if(!request)
    {
        return API_ERR_MALLOC_FAILED;
    }

    api_esmart_setup_req_header(&request->header, ESMART_REQ_SEND_CHUNK, esmart_send_chunk_req_size(size));

    request->session_id = sessionID;
    request->data_size = (AiUInt32)size;

    dataOffset = offsetof(struct esmart_send_chunk_req, data);
    memcpy((AiUInt8*) request + dataOffset, data, size);

    ret = api_esmart_issue_request( module, (struct esmart_request*) request, (struct esmart_response*) &response, sizeof(response));
    
    if(ret != 0)
    {
        return ret;
    }

    return api_esmart_map_status(response.status);
}


/*! \brief Downloads a file to device via eSmart
 *
 * @param boardHandle handle to board to download file to
 * @param sessionID The session id of the eSmart file transmission to use
 * @param file file stream to download
 * @return returns 0 on success, an error code on failure
 */
static AiInt16 api_esmart_download_file(AiUInt32 module, int sessionID, FILE* file)
{
    size_t bytesRead = 0;
    AiUInt8 data_chunk[4096];
    AiReturn ret = 0;

    /* Read data chunks from file and download each of them,
     * until no more chunks are left
     */
    do
    {
        bytesRead = fread(data_chunk, 1, sizeof(data_chunk), file);
        if(bytesRead)
        {
            ret = api_esmart_download_chunk(module, sessionID, data_chunk, bytesRead);
            
            if(ret != 0)
            {
                break;
            }
        }
        else
        {
            /* Check if we are really at end of file when nothing is read */
            if(!feof(file))
            {
                ret = API_ERR;
            }
        }
    }while(bytesRead > 0);

    return ret;
}


/*! \brief Upload data chunk from device via eSmart
 *
 *
 * @param boardHandle handle to the board the chunk will be requested from
 * @param sessionID The session id of the eSmart file transmission to use
 * @param data pointer to buffer that will receive data chunk
 * @param maxSize maximum size of data to upload in bytes
 * @param bytesLoaded pointer to variable that receives actual number of bytes transferred
 * @return returns 0 on success, a error code otherwise
 */
static AiInt16 api_esmart_upload_chunk(AiUInt32 module, int sessionID, void* data, size_t maxSize, size_t* bytesLoaded)
{
    struct esmart_receive_chunk_req request;
    struct esmart_receive_chunk_resp* response = NULL;
    int ret = 0;
    size_t dataOffset = 0;

    /* Create response buffer that can hold maximum chunk size */
    response = (struct esmart_receive_chunk_resp*) malloc(esmart_receive_chunk_resp_size(maxSize));
    
    if(!response)
    {
        return API_ERR_MALLOC_FAILED;
    }

    api_esmart_setup_req_header(&request.header, ESMART_REQ_RECEIVE_CHUNK, sizeof(struct esmart_receive_chunk_req));

    request.session_id = sessionID;
    request.max_data_size = (AiUInt32)maxSize;

    ret = api_esmart_issue_request(module, (struct esmart_request*) &request, (struct esmart_response*) response,
                                   esmart_receive_chunk_resp_size(maxSize));
    if(ret != 0)
    {
        return ret;
    }

    ret = api_esmart_map_status(response->status);

    /* Copy chunk data from response to buffer */
    if(ret == 0)
    {
        dataOffset = offsetof(struct esmart_receive_chunk_resp, data);
        memcpy(data, (AiUInt8*) response + dataOffset, response->data_size);
        *bytesLoaded = response->data_size;
    }

    return ret;
}


/*! \brief Uploads a file from device via eSmart
 *
 * @param boardHandle handle to board to upload file from
 * @param sessionID The session id of the eSmart file transmission to use
 * @param file file stream to store uploaded file into
 * @return returns 0 on success, an error code on failure
 */
static AiInt16 api_esmart_upload_file(AiUInt32 module, int sessionID, FILE* file)
{
    AiUInt8 data_chunk[4096];
    AiReturn ret = 0;
    size_t bytesLoaded = 0;
    size_t bytesWritten = 0;

    /* Request data chunks from file and write each of them to file stream
     * until no more chunks are left
     */
    do
    {
        ret = api_esmart_upload_chunk(module, sessionID, data_chunk, sizeof(data_chunk), &bytesLoaded);
        
        if( (ret == 0) && bytesLoaded)
        {
            bytesWritten = fwrite(data_chunk, 1, bytesLoaded, file);
            if(bytesWritten != bytesLoaded)
            {
                ret = API_ERR;
            }
        }

    }while(bytesLoaded && (ret == 0));

    return ret;
}


/*! \brief Initialize eSmart file transfer session
 *
 * @param boardHandle handle to board to initialize session on
 * @param fileName name of the file on board to transfer to/from
 * @param direction transfer direction. See \ref esmart_ftransfer_dir
 * @param sessionID pointer to variable that will receive the ID for the file session on success
 * @return returns 0 on success, an error code on failure
 */
static AiInt16 api_esmart_init_ftransfer(AiUInt32 module, const AiChar* fileName, enum esmart_ftransfer_dir direction, int* sessionID)
{
    struct esmart_init_transfer_req* request = NULL;
    struct esmart_init_transfer_resp response;
    int ret = 0;
    size_t fileNameOffset = 0;

    /* allocate request buffer that can hold the variable file name */
    request = (struct esmart_init_transfer_req*) malloc(esmart_init_transfer_req_size(strlen(fileName)));

    if(!request)
    {
        DEBUGOUT(DBG_ERROR, __FUNCTION__, "Failed to allocate response buffer");
        return API_ERR_MALLOC_FAILED;
    }

    api_esmart_setup_req_header(&request->header, ESMART_REQ_INIT_FTRANSFER, esmart_init_transfer_req_size(strlen(fileName)));

    request->direction = direction;
    request->file_name_len = (AiUInt32)strlen(fileName);

    /* Now determine offset of file name in request frame and copy it to it */
    fileNameOffset = offsetof(struct esmart_init_transfer_req, file_name);

    if(AiOsStrncpy(((char*) request) + fileNameOffset, fileName, strlen(fileName) + 1, AiFalse))
    {
        DEBUGOUT(DBG_ERROR, __FUNCTION__, "Failed to copy file name to request frame");
        return API_ERR;
    }

    ret = api_esmart_issue_request(module, (struct esmart_request*) request, (struct esmart_response*) &response, sizeof(struct esmart_init_transfer_resp));

    if(ret != 0)
    {
        return ret;
    }

    ret = api_esmart_map_status(response.status);
    
    if(ret == 0)
    {
        *sessionID = response.session_id;
    }

    return ret;
}


/*! \brief Complete eSmart file transfer session
 *
 * @param boardHandle handle to board to complete session on
 * @param sessionID file transfer session ID to complete
 * @return returns 0 on success, an error code on failure
 */
static AiInt16 api_esmart_complete_ftransfer(AiUInt32 module, int sessionID)
{
    struct esmart_complete_transfer_req request;
    struct esmart_complete_transfer_resp response;
    int ret = 0;

    api_esmart_setup_req_header(&request.header, ESMART_REQ_COMPLETE_FTRANSFER, sizeof(struct esmart_complete_transfer_req));

    request.session_id = sessionID;

    ret = api_esmart_issue_request(module, (struct esmart_request*) &request,
                                            (struct esmart_response*) &response,
                                             sizeof(struct esmart_complete_transfer_resp));
    if(ret != 0)
    {
        return ret;
    }

    return api_esmart_map_status(response.status);
}


/*! \brief Abort eSmart file transfer session
 *
 *  The down-/upload of the file will be aborted when this function is called.
 * @param boardHandle handle to board to abort session on
 * @param sessionID file transfer session ID to abort
 * @return returns 0 on success, an error code on failure
 */
static AiInt16 api_esmart_abort_ftransfer(AiUInt32 module, int sessionID)
{
    struct esmart_abort_transfer_req request;
    struct esmart_abort_transfer_resp response;
    int ret = 0;

    api_esmart_setup_req_header(&request.header, ESMART_REQ_ABORT_FTRANSFER, sizeof(struct esmart_abort_transfer_req));

    request.session_id = sessionID;

    ret = api_esmart_issue_request(module, (struct esmart_request*) &request,
                                  (struct esmart_response*) &response,
                                   sizeof(struct esmart_abort_transfer_resp));
    if(ret != 0)
    {
        return ret;
    }

    return api_esmart_map_status(response.status);
}


/*! \brief Maps eSmart update procedure states to FC API states
 *
 * @param the eSmart update procedure status to map
 * @return returns the corresponding API update state value
 */
static TY_API_E_UPDATE_STATUS INLINE(api_esmart_update_status_map(enum esmart_update_status status))
{
    switch(status)
    {
    case ESMART_UPDATE_TASK_FAILURE:
        return API_UPDATE_DEV_CON_FAILURE;
    case ESMART_UPDATE_WAITING:
        return API_UPDATE_IDLE;
    case ESMART_UPDATE_STARTING:
        return API_UPDATE_START_REQUESTED;
    case ESMART_UPDATE_EXTRACTING:
    case ESMART_UPDATE_RUNNING:
        return API_UPDATE_IN_PROGRESS;
    case ESMART_UPDATE_FAILED:
        return API_UPDATE_FAILED;
    case ESMART_UPDATE_FINISHED:
        return API_UPDATE_FINISHED;
    default:
        return API_UPDATE_DEV_CON_FAILURE;
    }
}


/*! \brief Get device update status via eSmart
 *
 * @param boardHandle handle to board to get status from
 * @param status the current update procedure status of the device
 * @param componentsDone number of components that have been updated so far in current update procedure
 * @param totalComponents total number of components that will be updated in current update procedure
 * @return returns 0 on success, an AIFC_ERR code otherwise.
 */
static AiInt16 api_esmart_update_status(AiUInt32 boardHandle, TY_API_E_UPDATE_STATUS* status, AiUInt32* componentsDone, AiUInt32* totalComponents)
{
    AiReturn ret = 0;
    struct esmart_update_status_req request;
    struct esmart_update_status_resp response;

    if(!status)
    {
        return API_ERR_PARAM1_IS_NULL;
    }

    api_esmart_setup_req_header(&request.header, ESMART_REQ_UPDATE_STATUS, sizeof(request));

    ret = api_esmart_issue_request(boardHandle, (struct esmart_request*) &request, (struct esmart_response*) &response, sizeof(response));

    if(ret != 0)
    {
        return ret;
    }

    *status = api_esmart_update_status_map(response.status);

    if(componentsDone)
    {
        *componentsDone = response.components_done;
    }

    if(totalComponents)
    {
        *totalComponents = response.num_components;
    }

    return ret;
}


/*! \brief Starts update procedure on a device
 *
 * @param boardHandle handle to board to start update on
 * @param updatePackageName name of the update package to apply
 * @return returns 0 on success, an AIFC_ERR code otherwise
 */
static AiInt16 api_esmart_update_start(AiUInt32 module, const AiChar* updatePackageName)
{
    AiReturn ret = 0;
    struct esmart_update_start_req* request = NULL;
    struct esmart_update_start_resp response;
    TY_API_E_UPDATE_STATUS status = API_UPDATE_DEV_CON_FAILURE;
    size_t fileNameOffset = 0;

    if(!updatePackageName)
    {
        return API_ERR_PARAM1_IS_NULL;
    }

    /* Allocate request buffer depending on update package file name length */
    request = (struct esmart_update_start_req*) malloc(esmart_update_start_req_size(strlen(updatePackageName)));
    
    if(!request)
    {
        DEBUGOUT(DBG_ERROR, __FUNCTION__, "Failed to allocate request buffer");
        return API_ERR_MALLOC_FAILED;
    }

    api_esmart_setup_req_header(&request->header, ESMART_REQ_UPDATE_START,
                               esmart_update_start_req_size(strlen(updatePackageName)));

    request->file_name_len = (AiUInt32)strlen(updatePackageName);

    /* Now determine offset of file name in request frame and copy it to it */
    fileNameOffset = offsetof(struct esmart_update_start_req, file_name);

    if (AiOsStrncpy(((char*)request) + fileNameOffset, updatePackageName, strlen(updatePackageName) + 1, AiFalse))
    {
        DEBUGOUT(DBG_ERROR, __FUNCTION__, "Failed to copy file name to request frame");
        return API_ERR;
    }

    ret = api_esmart_issue_request(module, (struct esmart_request*) request, (struct esmart_response*) &response,
                                  sizeof(response));
    if(ret != 0)
    {
        return ret;
    }


    /* Check status if it has been triggered at least */
    status = api_esmart_update_status_map(response.status);

    if(status != API_UPDATE_IN_PROGRESS
        && status != API_UPDATE_FAILED
        && status != API_UPDATE_FINISHED)
    {
        return API_ERR;
    }

    return ret;
}



/*! \brief Copies a file from host to target device
 *
 *  This function can be used to copy files from the host onto the embedded device system.
 *  \param ul_BoardHandle handle to board that file will be transferred to.
 *  \param sz_FilePath the path to the file to download. The file will be stored with basename on the embedded device
 *  \return returns 0 on success, an error code on failure.
 */
AI_LIB_FUNC AiReturn AI_CALL_CONV ApiFileDownload( AiUInt32 module, const char* sz_FilePath)
{
    AiReturn ret = 0;
    int sessionID = -1;
    FILE* file = NULL;
    char tempFilePath[AI_OS_MAX_PATH];
    char* fileName = NULL;
    TY_DEVICE_INFO * device_info;

    
    device_info = _ApiGetDeviceInfoPtrByModule( module );

    if( device_info == NULL )
        return( API_ERR_NO_MODULE_EXTENSION );

    if(    (device_info->uc_DeviceGroup != AI_DEVICE_AYS_ASP)
        && (device_info->uc_DeviceGroup != AI_DEVICE_ZYNQMP_ASP)
        && (device_info->uc_DeviceGroup != AI_DEVICE_AYS))
        return API_ERR_PLATTFORM_NOT_SUPPORTED;

    if( GET_SERVER_ID(module) )
        return API_ERR_PLATTFORM_NOT_SUPPORTED;


    if(!sz_FilePath)
        return API_ERR_PARAM1_IS_NULL;

    file = fopen(sz_FilePath, "rb");
    if(!file)
    {
        AiOsStrncpy(tempFilePath, sz_FilePath, AI_OS_MAX_PATH, FALSE);
        DEBUGOUT(DBG_ERROR, __FUNCTION__, tempFilePath);
        return API_ERR;
    }

    /* Now extract the file name from the path.
     * The file will be stored with this name on the board */
    if(AiOsStrncpy(tempFilePath, sz_FilePath, AI_OS_MAX_PATH, FALSE))
    {
        return API_ERR_PARAMETER_OUT_OF_RANGE;
    }

    fileName = AiOsBasename(tempFilePath);
    
    if(!fileName)
    {
        return API_ERR_PARAMETER_OUT_OF_RANGE;
    }
    
    do
    {
        ret = api_esmart_init_ftransfer(module, fileName, ESMART_FTRANSFER_DOWN, &sessionID);
        
        if(ret != 0)
        {
            break;
        }

        ret = api_esmart_download_file(module, sessionID, file);
        
        if(ret != 0)
        {
            if(api_esmart_abort_ftransfer(module, sessionID) != 0)
            {
                DEBUGOUT(DBG_ERROR, __FUNCTION__, "File transfer abort session failed");
            }
        }
        else
        {
            ret = api_esmart_complete_ftransfer(module, sessionID);
        }
    }while(0);

    if(file)
    {
        fclose(file);
    }

    return ret;
}





/*! \brief Copies a file from target device to host
 *
 *  This function can be used to copy files from the embedded device system to the host.
 *  \param ul_BoardHandle handle to board that file will be transferred from.
 *  \param px_UploadIn pointer to structure with the input parameters
 *  \return returns 0 on success, an error code on failure.
 */
AI_LIB_FUNC AiReturn AI_CALL_CONV ApiFileUpload( AiUInt32 module, TY_API_FILE_UPLOAD_IN* px_UploadIn)
{
    AiReturn ret = 0;
    int sessionID = 0;
    FILE* file = NULL;
    TY_DEVICE_INFO * device_info;
    char tempFilePath[AI_OS_MAX_PATH];

    
    device_info = _ApiGetDeviceInfoPtrByModule( module );

    if( device_info == NULL )
        return( API_ERR_NO_MODULE_EXTENSION );

    if ((device_info->uc_DeviceGroup != AI_DEVICE_AYS_ASP)
        && (device_info->uc_DeviceGroup != AI_DEVICE_ZYNQMP_ASP)
        && (device_info->uc_DeviceGroup != AI_DEVICE_AYS))
        return API_ERR_PLATTFORM_NOT_SUPPORTED;

    if( GET_SERVER_ID(module) )
        return API_ERR_PLATTFORM_NOT_SUPPORTED;

    if(!px_UploadIn)
        return API_ERR_PARAMETER_OUT_OF_RANGE;

    if(!px_UploadIn->pc_DestFilePath || !px_UploadIn->pc_SourceFileName)
        return API_ERR_PARAMETER_OUT_OF_RANGE;

    /* Check if destination file already exists.
     * If it does, check if flags for overwriting or appending are specified.
     * Otherwise return with an error
     */
    if( AiOsAccess( px_UploadIn->pc_DestFilePath, AI_F_OK ) == 0 )
    {
        if(px_UploadIn->ul_Flags & API_FILE_FLAG_OVERWRITE)
        {
			file = fopen(px_UploadIn->pc_DestFilePath, "wb");
        }
        else if(px_UploadIn->ul_Flags & API_FILE_FLAG_APPEND)
        {
            file = fopen(px_UploadIn->pc_DestFilePath, "ab");
        }
        else
        {
            DEBUGOUT(DBG_ERROR, __FUNCTION__, "Opend file failed");
            AiOsStrncpy(tempFilePath, px_UploadIn->pc_DestFilePath, AI_OS_MAX_PATH, FALSE);
            DEBUGOUT(DBG_ERROR, __FUNCTION__, tempFilePath);
            return API_ERR;
        }
    }
    else
    {
        file = fopen(px_UploadIn->pc_DestFilePath, "wb");
	}

    if(!file)
    {
        DEBUGOUT(DBG_ERROR, __FUNCTION__, "no file pointer");
        AiOsStrncpy(tempFilePath, px_UploadIn->pc_DestFilePath, AI_OS_MAX_PATH, FALSE);
        DEBUGOUT(DBG_ERROR, __FUNCTION__, tempFilePath);
        return API_ERR;
    }

    do
    {
        ret = api_esmart_init_ftransfer(module, px_UploadIn->pc_SourceFileName, ESMART_FTRANSFER_UP, &sessionID);
        if(ret != 0)
        {
            DEBUGOUT(DBG_ERROR, __FUNCTION__, "api_esmart_init_ftransfer fails");
            AiOsStrncpy(tempFilePath, px_UploadIn->pc_SourceFileName, AI_OS_MAX_PATH, FALSE);
            DEBUGOUT(DBG_ERROR, __FUNCTION__, tempFilePath);

            break;
        }

        ret = api_esmart_upload_file(module, sessionID, file);
        
        if(ret != 0)
        {
            if(api_esmart_abort_ftransfer(module, sessionID) != 0)
            {
                DEBUGOUT(DBG_ERROR, __FUNCTION__, "File transfer abort session failed");
            }
        }
        else
        {
            ret = api_esmart_complete_ftransfer(module, sessionID);
        }
    }while(0);

    if(file)
    {
        fclose(file);
    }

    return ret;
}





/*! \brief Returns current device update process status
 *
 * This function returns the current state of a device update
 * procedure.
 * \param ul_BoardHandle handle to board to get update procedure status from
 * \param px_UpdateStatus pointer to structure that will receive the update process status
 * \return returns 0 on success, an error code on failure.
 */
AI_LIB_FUNC AiReturn AI_CALL_CONV ApiUpdateStatus( AiUInt32 module, TY_API_PACKET_UPDATE_STATUS_OUT* px_UpdateStatus)
{
    AiReturn ret = 0;
    TY_DEVICE_INFO * device_info;
    
    device_info = _ApiGetDeviceInfoPtrByModule( module );

    if( device_info == NULL )
        return( API_ERR_PLATTFORM_NOT_SUPPORTED );

    if (   (device_info->uc_DeviceGroup != AI_DEVICE_AYS_ASP)
        && (device_info->uc_DeviceGroup != AI_DEVICE_ZYNQMP_ASP)
        && (device_info->uc_DeviceGroup != AI_DEVICE_AYS))
        return API_ERR_PLATTFORM_NOT_SUPPORTED;

    if( GET_SERVER_ID(module) )
        return API_ERR_PLATTFORM_NOT_SUPPORTED;
    
    
    if(!px_UpdateStatus)
    {
        return API_ERR_PARAM1_IS_NULL;
    }

    return api_esmart_update_status(module, &px_UpdateStatus->e_Status,
                                    &px_UpdateStatus->ul_ComponentsDone, &px_UpdateStatus->ul_TotalComponents);
}





/*! \brief Start device update procedure
 *
 * This function starts the update procedure with a previously downloaded file.
 * \param ul_BoardHandle handle to board where the update procedure shall be started
 * \param updatePackageName name of the update package, that has been previously downloaded to device
 * \return return 0 on success, an error code otherwise
 */
AI_LIB_FUNC AiReturn AI_CALL_CONV ApiUpdateStart( AiUInt32 module, const AiChar* updatePackageName)
{
    AiReturn ret = 0;
    char tempFilePath[AI_OS_MAX_PATH];
    char* fileName = NULL;
    TY_DEVICE_INFO * device_info;
    
    device_info = _ApiGetDeviceInfoPtrByModule( module );

    if( device_info == NULL )
        return( API_ERR_NO_MODULE_EXTENSION );
        
    if ((device_info->uc_DeviceGroup != AI_DEVICE_AYS_ASP)
        && (device_info->uc_DeviceGroup != AI_DEVICE_ZYNQMP_ASP)
        && (device_info->uc_DeviceGroup != AI_DEVICE_AYS))
        return API_ERR_PLATTFORM_NOT_SUPPORTED;

    if( GET_SERVER_ID(module) )
        return API_ERR_PLATTFORM_NOT_SUPPORTED;


    if(!updatePackageName)
        return API_ERR_PARAM1_IS_NULL;

    /* We extract the file name from the path, as update packages
     * are stored in a dedicated path on the board's file system */
    if(AiOsStrncpy(tempFilePath, updatePackageName, AI_OS_MAX_PATH, FALSE))
    {
        return API_ERR;
    }

    fileName = AiOsBasename(tempFilePath);
    
    if(!fileName)
    {
        return API_ERR;
    }

    return api_esmart_update_start(module, fileName);
}


