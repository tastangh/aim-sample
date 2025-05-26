/* SPDX-FileCopyrightText: 2016-2021 AIM GmbH <info@aim-online.com> */
/* SPDX-License-Identifier: MIT  */

/*! \file ANS_Protocol.h
 *
 *  This header file contains declarations
 *  used for handling ANS protocol settings
 *
 */

#ifndef ANS_PROTOCOL_H_
#define ANS_PROTOCOL_H_


#include "Ai_types.h"




/*! \struct AnsProtocolVersion
 *
 * Definition of the ANS protocol version. \n
 * Has to be set by server implementation, as well as by client API.
 */

struct AnsProtocolVersion
{
    AiUInt32 majorVersion;      /*!< Major version of protocol. Must be incremented if any frame layout is altered */
    AiUInt32 minorVersion;      /*!< Minor version of protocol. Must be incremented if new frames are added */
};


/*! \struct AnsProtocol
 *
 * This structure comprises all settings
 * for the ANS communication protocol
 */
struct AnsProtocol
{
    AiUInt32 magic;                     /*!< Magic number used during link initialization */
    struct AnsProtocolVersion version;  /*!< Protocol version */
};


/*!< Global protocol settings that shall be used by server and client applications */
extern struct AnsProtocol g_AnsProtocol;


/*! \brief Get magic number of protocol
 *
 * @param protocol to get magic number of
 * @return the protocol magic number
 */
static AiUInt32 AI_INLINE AnsProtocol_getMagic(const struct AnsProtocol* protocol)
{
    return protocol->magic;
}


/*! \brief Set magic number for a protocol
 *
 * @param protocol to set magic for
 * @param magic the magic number to set
 */
extern void AnsProtocol_setMagic(struct AnsProtocol* protocol, AiUInt32 magic);


/*! \brief Get protocol version
 *
 * @param protocol the protocol to get version of
 * @return the protocol version
 */
static struct AnsProtocolVersion AI_INLINE AnsProtocol_getVersion(const struct AnsProtocol* protocol)
{
    return protocol->version;
}


/*! \brief Set protocol version
 *
 * @param protocol the protocol to set version of
 * @param new_version the version to set
 */
extern void AnsProtocol_setVersion(struct AnsProtocol* protocol, const struct AnsProtocolVersion* new_version);


/*! \brief Get major version of protocol
 *
 * @param protocol the protocol to get major version of
 * @return the major version of the protocol
 */
static AiUInt32 AI_INLINE AnsProtocol_getMajorVersion(const struct AnsProtocol* protocol)
{
    return protocol->version.majorVersion;
}


/*! \brief Get minor version of protocol
 *
 * @param protocol to get minor version of
 * @return the minor version of the protocol
 */
static AiUInt32 AI_INLINE AnsProtocol_getMinorVersion(const struct AnsProtocol* protocol)
{
    return protocol->version.minorVersion;
}



#endif /* ANS_PROTOCOL_H_ */
