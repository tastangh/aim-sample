/* SPDX-FileCopyrightText: 2016-2021 AIM GmbH <info@aim-online.com> */
/* SPDX-License-Identifier: MIT  */

/*
 * ANS_Protocol.c
 *
 */


#include "ANS_Protocol.h"



struct AnsProtocol g_AnsProtocol = {0, {0,0}};





void AnsProtocol_setMagic(struct AnsProtocol* protocol, AiUInt32 magic)
{
    protocol->magic = magic;
}


void AnsProtocol_setVersion(struct AnsProtocol* protocol, const struct AnsProtocolVersion* new_version)
{
    struct AnsProtocolVersion* current_version = &protocol->version;

    current_version->majorVersion = new_version->majorVersion;
    current_version->minorVersion = new_version->minorVersion;
}
