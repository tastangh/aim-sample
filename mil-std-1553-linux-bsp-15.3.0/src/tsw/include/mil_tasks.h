/* SPDX-FileCopyrightText: 2016-2022 AIM GmbH <info@aim-online.com> */
/* SPDX-License-Identifier: MIT OR GPL-2.0-or-later */


/*! \file mil_tasks.h
 *
 *  Anet specific implementations
 */

#ifndef MIL_TASKS_H_
#define MIL_TASKS_H_


#include "Ai_cdef.h"
#include "mil_tsw_includes.h"

struct mil_tasks_info;


/*! \def AIM_MIL_DATAQUEUE_PERIOD_MSEC
 * Time in milliseconds the data queue handler will copy data
 * from the Firmware buffers into the data queues.
 *
 * Wo do no longer need the 50ms from prev. drivers because
 * the data is also copied on each HFI as well as on BM halt.
 */
#define AIM_MIL_DATAQUEUE_PERIOD_MSEC 50

/*! \def MIL_ANET_BUSLOAD_PERIOD_MS
* Time in milliseconds in which periodic bus load
* calculation timer elapses
*/
#define AIM_MIL_BUSLOAD_PERIOD_MS 250



/*! \brief Allocate tasks info struct
 *
 * @param p_api_dev pointer to target software administration structure of the device to start timer for
 * @return returns 0 on success, non-zero value otherwise
 */
AiUInt32 mil_tasks_info_init(TY_API_DEV* p_api_dev);

/*! \brief Free tasks info struct
 *
 * @param p_api_dev pointer to target software administration structure of the device to start timer for
 * @return returns 0 on success, non-zero value otherwise
 */
AiUInt32 mil_tasks_info_free(TY_API_DEV* p_api_dev);

/*! \brief Starts periodic data queue timer of a device
 *
 * @param p_api_dev pointer to target software administration structure of the device to start timer for
 * @return returns 0 on success, non-zero value otherwise
 */
AiUInt32 mil_tasks_dataqueue_timer_start(TY_API_DEV* p_api_dev);


/*! \brief Stops periodic data queue timer of a device
 *
 * This function checks first if any of the BIU recording queues is still active.
 * If all of them are suspended, the timer will be actually stopped.
 * @param p_api_dev pointer to target software administration structure of the device to start timer for
 * @return returns 0 on success, non-zero value otherwise
 */
AiUInt32 mil_tasks_dataqueue_timer_stop(TY_API_DEV* p_api_dev);


/*! \brief Processes a specific data queue
 *
 * This function triggers the processing function of a specific data queue on a given device. \n
 * E.g. with recording queues, data from the BIU buffer is copied into the data queue. \n
 * dataqueueID parameter value -1 can be used to process all of the data queues.
 * @param p_api_dev pointer to the target administration structure of the device that owns the data queue to process
 * @param dataqueueID ID of the data queue to process. -1 if all queues shall be processed
 * @return return 0 on success, non-zero value otherwise.
 */
AiUInt32 mil_tasks_dataqueue_process(TY_API_DEV* p_api_dev, AiInt32 dataqueueID);


/*! \brief Starts bus load calculation for a stream
 *
 * This function will just check if the periodic bus load calculation timer
 * is already active. If not, it will start it.
 * @param p_api_dev pointer to target administration device structure.
 * @param stream ID of stream to start calculation for
 * @return returns 0 on success, non-zero value otherwise
 */
AiUInt32 mil_tasks_busload_calc_start(TY_API_DEV* p_api_dev, AiUInt8 stream);


/*! \brief Stops bus load calculation for a stream
 *
 * This function resets all calculated bus load values of the
 * given stream. \n
 * Then it checks if monitoring on any of the board's streams
 * is still active. \n
 * If not, the periodic bus load calculation timer can be stopped
 * @param p_api_dev pointer to target administration device structure
 * @param stream ID of stream to stop calculation for
 * @return returns 0 on success, non-zero value otherwise
 */
AiUInt32 mil_tasks_busload_calc_stop(TY_API_DEV* p_api_dev, AiUInt8 stream);



/*! \brief Starts FODDL monitoring for the HS stream 0
 *
 * This function will start the periodic FODDL timer.
 * @param p_api_dev pointer to target administration device structure.
 * @return returns 0 on success, non-zero value otherwise
 */
AiUInt32 mil_tasks_foddl_start(TY_API_DEV* p_api_dev);


/*! \brief Stops FODDL monitoring for the HS stream 0
 *
 * This function stops the periodic FODDL timer \n
 * @param p_api_dev pointer to target administration device structure
 * @return returns 0 on success, non-zero value otherwise
 */
AiUInt32 mil_tasks_foddl_stop(TY_API_DEV* p_api_dev);



#endif /* mil_tasks_H_ */

