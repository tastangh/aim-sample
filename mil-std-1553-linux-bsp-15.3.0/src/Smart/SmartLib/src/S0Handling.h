/* SPDX-FileCopyrightText: 2001-2021 AIM GmbH <info@aim-online.com> */
/* SPDX-License-Identifier: MIT  */

#ifndef _S0HANDLING_H
#define _S0HANDLING_H

/******************************************************************************
 *  Prototypes
******************************************************************************/
char* cpGenerateAIM_S0Record(char *cpName, char *cpVersion, char *cpInfo);
int iExtractInformationFromAIM_S0Record(char *cpSRecordLine,
                                         char *cpName,    unsigned int uiNameMaxLength,
                                         char *cpVersion, unsigned int uiVersionMaxLength,
                                         char *cpInfo,    unsigned int uiInfoMaxLength);
int iConvertVersionStringToBCD(char *cpVersion, unsigned int*uipMajor, unsigned int*uipMinor, unsigned int*uipBuild);
unsigned int uiInsertString(char *cpTrgString, unsigned int uiTrgIndex, char* cpString);
unsigned int uiInsertCharVal(char *cpTrgString, unsigned int uiTrgIndex, char cValue);
char cCalculateSRecordChecksum(char *cpTrgString);
char cExtractSRecordLineLength(char *cpTrgString);
unsigned int uiExtractSRecordStartAddress(char *cpSrcString, unsigned int *puiAddressLength, char *pbData);
unsigned int uiWriteSrecDataToAddress(char *cpSrcString, unsigned int uiDestSize, char* cpDestOffset);
char cExtractSRecordChecksum(char *cpTrgString);
char AsciiToInt( char cAsciiChar);
void usage(void);
void vTestExtract(char *cpSRecordLine);

#endif

