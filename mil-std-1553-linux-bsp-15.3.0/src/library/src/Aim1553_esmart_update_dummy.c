/* SPDX-FileCopyrightText: 2019-2022 AIM GmbH <info@aim-online.com> */
/* SPDX-License-Identifier: MIT */

/*! \file Aim1553_esmart_udpate_dummy.cpp
 *
 *  This file contains a dummy stub with the functions required for the esmart update
 *
 */


#include "AiOs.h"
#include "Api1553.h"
#include "esmart.h"


/*! \brief Copies a file from host to target device
 *
 *  This function can be used to copy files from the host onto the embedded device system.
 *  \param ul_BoardHandle handle to board that file will be transferred to.
 *  \param sz_FilePath the path to the file to download. The file will be stored with basename on the embedded device
 *  \return returns 0 on success, an error code on failure.
 */
AI_LIB_FUNC AiReturn AI_CALL_CONV ApiFileDownload( AiUInt32 module, const char* sz_FilePath)
{
    return API_ERR_PLATTFORM_NOT_SUPPORTED;
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
	return API_ERR_PLATTFORM_NOT_SUPPORTED;
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
	return API_ERR_PLATTFORM_NOT_SUPPORTED;
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
	return API_ERR_PLATTFORM_NOT_SUPPORTED;
}


