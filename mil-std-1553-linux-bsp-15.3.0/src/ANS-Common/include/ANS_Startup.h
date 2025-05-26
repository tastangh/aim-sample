/* SPDX-FileCopyrightText: 2013-2021 AIM GmbH <info@aim-online.com> */
/* SPDX-License-Identifier: MIT  */

/*! \file ANS_Startup.h
 *
 * This header file contains common declarations for
 * starting the ANS either as a service, or as a console application
 *
 */

#ifndef ANS_SERVICE_H_
#define ANS_SERVICE_H_




/*! \brief Starts the ANS as a service
 *
 * This function is OS dependent. It has to start the
 * ANS as a system service.
 *
 * @param serviceName name of the service
 * @return  returns 0 on success, a negative error code otherwise
 */
int AnsServiceStart(const char* serviceName);


/*! \brief Starts the ANS server thread
 *
 * Protocol specific initialization of the server must
 * be performed before calling this function.
 * This function blocks until the server is stopped.
 *
 * @return returns 0 on success, a negative error code otherwise
 */
int AnsServerStart(void);




#endif /* ANS_SERVICE_H_ */
