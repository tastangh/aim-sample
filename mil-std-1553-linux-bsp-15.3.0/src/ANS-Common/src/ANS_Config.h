/* SPDX-FileCopyrightText: 2013-2021 AIM GmbH <info@aim-online.com> */
/* SPDX-License-Identifier: MIT  */

/*! \file ANS_Config.h
 * \brief ANS Configuration
 *
 * This header file provides the default ANS configuration settings.
 *
 *
 ******************************************************************************/
#ifndef ANS_CONFIG_H
#define ANS_CONFIG_H

/* Includes *******************************************************************/

/* Constants ******************************************************************/



#define ANS_CONFIG_LINK_INIT_READ_TIMEOUT_MS (20000ul)
                                    /*!< Link initialization frame read */
                                    /*!< time-out in [ms]               */

#define ANS_CONFIG_MAX_CLIENTS     (16)
                                    /*!< Max. number of ANS clients     */
                                    /*!< supported by the system        */

#define ANS_CONFIG_MAX_CONNECTIONS_PER_CLIENT  (16)
                                    /*!< Max. number of concurrent      */
                                    /*!< connections per ANS client     */
     
/* Macros *********************************************************************/
 
/* Type definitions ***********************************************************/

/* Prototypes *****************************************************************/

#endif  /* ANS_CONFIG_H */
