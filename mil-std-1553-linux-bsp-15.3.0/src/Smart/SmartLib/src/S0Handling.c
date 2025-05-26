/* SPDX-FileCopyrightText: 2001-2021 AIM GmbH <info@aim-online.com> */
/* SPDX-License-Identifier: MIT  */

/******************************************************************************
 *  Includes
******************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <Ai_cdef.h>
#include "S0Handling.h"

/******************************************************************************
 *  Code
******************************************************************************/

/****************************************************************************
   Function    : cpGenerateAIM_S0Record
-----------------------------------------------------------------------------
   Create      : 27.07.2011 (rh)    Update  : __.__.20__ ()
-----------------------------------------------------------------------------

   Inputs      : Input Pointers for strings

   Outputs     : -

   Return      : Pointer to a allocated S-0 String

   Description : Generate a AIM S-0-Record Line

   Modification: 


*****************************************************************************/
char* cpGenerateAIM_S0Record(char *cpName, char *cpVersion, char *cpInfo)
{
unsigned int uiLength, uiTrgIndex, ii;

char *cpS0Line;
char cChecksum;

   uiLength = strlen("S0000000");
   uiLength += 2* strlen(cpName);
   uiLength += 2* strlen(cpVersion);
   uiLength += 2* strlen(cpInfo);
   uiLength += 8; /* Seperator and Checksum */

   cpS0Line = (char *)malloc( uiLength);

   if (NULL != cpS0Line)
   {
      memset(cpS0Line, 0, uiLength);
      /* Generate Line */
      sprintf(cpS0Line,"S0000000");

      uiTrgIndex = strlen(cpS0Line);
   
      uiTrgIndex = uiInsertString(cpS0Line, uiTrgIndex, cpName);
      uiTrgIndex = uiInsertString(cpS0Line, uiTrgIndex, " ");
      uiTrgIndex = uiInsertString(cpS0Line, uiTrgIndex, cpVersion);
      uiTrgIndex = uiInsertString(cpS0Line, uiTrgIndex, " ");
      uiTrgIndex = uiInsertString(cpS0Line, uiTrgIndex, cpInfo);
      /* Keep Space for Chechsum */
      uiTrgIndex = uiInsertString(cpS0Line, uiTrgIndex, " ");

      /* Set Size Value */
      uiInsertCharVal(cpS0Line, 2, (unsigned char)((uiTrgIndex-4)/2));


      ii = strlen(cpS0Line);
      /* Set Checksum */
      cChecksum = cCalculateSRecordChecksum(cpS0Line);
      uiInsertCharVal(cpS0Line, ii-2, cChecksum);
   }


   return(cpS0Line);

}

/****************************************************************************
   Function    : iExtractInformationFromAIM_S0Record
-----------------------------------------------------------------------------
   Create      : 27.07.2011 (rh)    Update  : __.__.20__ ()
-----------------------------------------------------------------------------

   Inputs      : Pointer to S-0 String
                 Pointers to output Strings
                 Length information for Output Strings

   Outputs     : Output Strings

   Description : Extract all the information of an AIM S0-Record

   Modification: 


*****************************************************************************/
int iExtractInformationFromAIM_S0Record(char *cpSRecordLine,
                                         char *cpName,    unsigned int uiNameMaxLength,
                                         char *cpVersion, unsigned int uiVersionMaxLength,
                                         char *cpInfo,    unsigned int uiInfoMaxLength)


{
unsigned int uiLength;
unsigned int uiIndex2, uiIndexHelp;
char cChecksum;
unsigned int uiLineLength;
char szHeplString[80];
char cCh;

   uiLength = strlen(cpSRecordLine);

   cChecksum = cExtractSRecordChecksum(cpSRecordLine);
   uiLineLength = (unsigned int)(unsigned char)cExtractSRecordLineLength(cpSRecordLine);


   if ( (cChecksum == cCalculateSRecordChecksum(cpSRecordLine)) )
   {
      if (uiLineLength == uiLength)
      {
         uiIndex2 = strlen("S0000000");

         /* Extract Name */
         uiIndexHelp = 0;
         do
         {
            cCh = AsciiToInt(cpSRecordLine[uiIndex2++]);
            cCh <<= 4;
            cCh |= AsciiToInt(cpSRecordLine[uiIndex2++]);

            szHeplString[uiIndexHelp++] = cCh;
         }
         while ( ( cCh != ' ') &&
                 ( uiIndexHelp < uiNameMaxLength-1) &&
                 ( uiIndex2 < uiLength) );

         szHeplString[uiIndexHelp-1] = '\0';
         strcpy(cpName, szHeplString);

         /* Extract Version */
         uiIndexHelp = 0;
         do
         {
            cCh = AsciiToInt(cpSRecordLine[uiIndex2++]);
            cCh <<= 4;
            cCh |= AsciiToInt(cpSRecordLine[uiIndex2++]);

            szHeplString[uiIndexHelp++] = cCh;
         }
         while ( ( cCh != ' ') &&
                 ( uiIndexHelp < uiVersionMaxLength-1) &&
                 ( uiIndex2 < uiLength) );
         szHeplString[uiIndexHelp-1] = '\0';
         strcpy(cpVersion, szHeplString);

         /* Extract Info */
         if (uiIndex2 < uiLength-2) 
         {
            uiIndexHelp = 0;
            do
            {
               cCh = AsciiToInt(cpSRecordLine[uiIndex2++]);
               cCh <<= 4;
               cCh |= AsciiToInt(cpSRecordLine[uiIndex2++]);

               szHeplString[uiIndexHelp++] = cCh;
            }
            while ( ( uiIndex2 < uiLength-2) &&
                    ( uiIndexHelp < uiInfoMaxLength-1) );
            szHeplString[uiIndexHelp] = '\0';
            strcpy(cpInfo, szHeplString);
         }
         else
            cpInfo[0] = '\0';

      }
      else
      {
         printf("Line Length incorrect\n");
         return -1;
      }
   }
   else
   {
      printf("Checksum of S-Record Line incorrect\n");
      return -1;
   }
   return 0;
}

/****************************************************************************
   Function    : iConvertVersionStringToBCD
-----------------------------------------------------------------------------
   Create      : 27.07.2011 (rh)    Update  : __.__.20__ ()
-----------------------------------------------------------------------------

   Inputs      : Pointer to Target String
                 Major-, Minor- and Build-No

   Outputs     : String in the format "VMM.mm.build"

   Description : Convert Version Numbers to a String  

   Modification: 


*****************************************************************************/
int iConvertVersionStringToBCD(char *cpVersion, unsigned int*uipMajor, unsigned int*uipMinor, unsigned int*uipBuild)
{
unsigned int uiIndex, uiTemp;
unsigned int uiStringLen = strlen(cpVersion);

   if (cpVersion[0] == 'V')
   {
      /* Extract Major */
      uiIndex = 1;
      uiTemp = 0;
      while ((cpVersion[uiIndex] != '.') &&
             (uiIndex < uiStringLen))
      {
         uiTemp <<=4;
         uiTemp |= AsciiToInt(cpVersion[uiIndex++]);
      }
      *uipMajor = uiTemp;
      /* Extract Minor */
      uiIndex++;
      uiTemp = 0;
      while ((cpVersion[uiIndex] != '.') &&
             (uiIndex < uiStringLen))
      {
         uiTemp <<=4;
         uiTemp |= AsciiToInt(cpVersion[uiIndex++]);
      }
      *uipMinor = uiTemp;
      /* Extract Build */
      uiIndex++;
      uiTemp = 0;
      while (uiIndex < uiStringLen)
      {
         uiTemp <<=4;
         uiTemp |= AsciiToInt(cpVersion[uiIndex++]);
      }
      *uipBuild = uiTemp;
   }
   else
   {
      printf("Wrong String!\n");
         return -1;
   }

   return 0;
}

/****************************************************************************
   Function    : uiInsertString
-----------------------------------------------------------------------------
   Create      : 27.07.2011 (rh)    Update  : __.__.20__ ()
-----------------------------------------------------------------------------

   Inputs      : Pointer to Target String
                 Index 
                 Pointer to input string

   Outputs     : -

   Return      : New index

   Description : Insert a String to Target String at indexed position

   Modification: 


*****************************************************************************/
unsigned int uiInsertString(char *cpTrgString, unsigned int uiTrgIndex, char* cpString)
{
char cAsciiVal;
char acAsciiStr[3];
unsigned int uiSrcIndex, uiSrcLen;

   uiSrcLen = strlen(cpString);
   for (uiSrcIndex=0; uiSrcIndex < uiSrcLen ; uiSrcIndex++, uiTrgIndex+=2)
   {
      cAsciiVal = cpString[uiSrcIndex];
      sprintf(acAsciiStr,"%02x", (unsigned char)cAsciiVal);
      cpTrgString[uiTrgIndex] = acAsciiStr[0];
      cpTrgString[uiTrgIndex+1] = acAsciiStr[1];
   }

   return uiTrgIndex;
}

/****************************************************************************
   Function    : uiInsertCharVal
-----------------------------------------------------------------------------
   Create      : 27.07.2011 (rh)    Update  : __.__.20__ ()
-----------------------------------------------------------------------------

   Inputs      : Pointer to String
                 Index 
                 Character Value

   Outputs     : -

   Return      : New index

   Description : Insert a Character to Target String at indexed position

   Modification: 


*****************************************************************************/
unsigned int uiInsertCharVal(char *cpTrgString, unsigned int uiTrgIndex, char cValue)
{
char acAsciiStr[3];

   sprintf(acAsciiStr,"%02x", (unsigned char)cValue);
   cpTrgString[uiTrgIndex] = acAsciiStr[0];
   cpTrgString[uiTrgIndex+1] = acAsciiStr[1];
   return uiTrgIndex;
}


/****************************************************************************
   Function    : cCalculateSRecordChecksum
-----------------------------------------------------------------------------
   Create      : 27.07.2011 (rh)    Update  : __.__.20__ ()
-----------------------------------------------------------------------------

   Inputs      : Pointer to string

   Outputs     : -

   Return      : Calculated S-Record Chacksum

   Description : Calculate S-Record Chacksum

   Modification: 


*****************************************************************************/
char cCalculateSRecordChecksum(char *cpTrgString)
{
char cCs= 0;
char cHelp1, cHelp2;
unsigned int uiLength, uiIndex;

   uiLength = strlen(cpTrgString);
   if (cpTrgString[uiLength-1] == '\n')
      uiLength--;


   for (uiIndex = 2; uiIndex < uiLength-2; uiIndex+=2)
   {
      cHelp1 = AsciiToInt( cpTrgString[uiIndex  ] );
      cHelp2 = AsciiToInt( cpTrgString[uiIndex+1] );

      cHelp1 = (cHelp1<<4);
      cHelp1 += cHelp2;

      cCs +=cHelp1;
   }

   return (0xff - cCs);
}


/****************************************************************************
   Function    : cExtractSRecordChecksum
-----------------------------------------------------------------------------
   Create      : 27.07.2011 (rh)    Update  : __.__.20__ ()
-----------------------------------------------------------------------------

   Inputs      : Pointer to String

   Outputs     : -

   Return      : S-Record Checksum of S-Record Line

   Description : Extract S-Record from a given S-Record Line (String)

   Modification: 


*****************************************************************************/
char cExtractSRecordChecksum(char *cpTrgString)
{
unsigned int uiLength;
char cChecksum;

   uiLength = strlen(cpTrgString);
   if (cpTrgString[uiLength-1] == '\n')
      uiLength--;

   cChecksum = AsciiToInt(cpTrgString[uiLength-2]);
   cChecksum <<= 4;
   cChecksum |= AsciiToInt(cpTrgString[uiLength-1]);
   return (cChecksum);
}


/****************************************************************************
   Function    : cExtractSRecordLineLength
-----------------------------------------------------------------------------
   Create      : 27.07.2011 (rh)    Update  : __.__.20__ ()
-----------------------------------------------------------------------------

   Inputs      : Pointer to S-Record String

   Outputs     : -

   Return      : Length of S-Record Line

   Description : Extract S-Record Line Length (in Byte) of a given S-Record 
                 Line (String)

   Modification: 


*****************************************************************************/
char cExtractSRecordLineLength(char *cpTrgString)
{
unsigned int uiLength;
char cLineLength;

   uiLength = strlen(cpTrgString);
   if (3 < uiLength)
   {
      cLineLength = AsciiToInt(cpTrgString[2]);
      cLineLength <<= 4;
      cLineLength |= AsciiToInt(cpTrgString[3]);
      cLineLength*=2;   /* Correction because ASCII conversion */
      cLineLength+=4;   /* Correct size of 'S' and '0' */

   }
   else 
      cLineLength = 0;
   return (cLineLength);
}

 
/****************************************************************************
   Function    : uiExtractSRecordStartAddress
-----------------------------------------------------------------------------
   Create      : 27.07.2011 (rh)    Update  : __.__.20__ ()
-----------------------------------------------------------------------------

   Inputs      : See input parameters

   Outputs     : -

   Return      : Start address of S-Record

   Description : Extract S-Record Start Address of a given S-Record Line (String)

   Modification: 


*****************************************************************************/
unsigned int uiExtractSRecordStartAddress(char *cpSrcString, unsigned int *puiAddressLength, char *pbData)
{
unsigned int uiAddressLength, uiAddressIndex = 4/*here starts the Address*/, uiAddressMaxIndex;
unsigned int uiStartAddress = 0;

   /* determ address length */
   if ( (cpSrcString[0] == 'S') && ((cpSrcString[1] == '1')||(cpSrcString[1] == '9')) )
   { 
      *puiAddressLength = uiAddressLength = 4; 
      if (cpSrcString[1] == '1')
         *pbData = AiTrue;
      else 
         *pbData = AiFalse;
   }
   else if ( (cpSrcString[0] == 'S') && ((cpSrcString[1] == '2')||(cpSrcString[1] == '8')) )
   { 
      *puiAddressLength = uiAddressLength = 6; 
      if (cpSrcString[1] == '2')
         *pbData = AiTrue;
      else 
         *pbData = AiFalse;
   }
   else if ( (cpSrcString[0] == 'S') && ((cpSrcString[1] == '3')||(cpSrcString[1] == '7')) )
   { 
      *puiAddressLength = uiAddressLength = 8; 
      if (cpSrcString[1] == '3')
         *pbData = AiTrue;
      else 
         *pbData = AiFalse;

   }
   else
   { 
      uiAddressLength = 0; 
   }

   uiAddressMaxIndex = uiAddressIndex + uiAddressLength;
   if (0<uiAddressLength)
   {
      /* Extract Address */
      do
      {
         uiStartAddress <<= 4;
         uiStartAddress |= AsciiToInt(cpSrcString[uiAddressIndex++]);
      }
      while( uiAddressIndex < uiAddressMaxIndex);
   }
   return (uiStartAddress);
}


/****************************************************************************
   Function    : uiWriteSrecDataToAddress
-----------------------------------------------------------------------------
   Create      : 27.07.2011 (rh)    Update  : __.__.20__ ()
-----------------------------------------------------------------------------

   Inputs      : See input parameters

   Outputs     : -

   Return      : Bytes Written

   Description : Write a S-Record Lines Data to the Destination

   Modification: 


*****************************************************************************/
unsigned int uiWriteSrecDataToAddress(char *cpSrcString, unsigned int uiDestSize, char* cpDestOffset)
{
unsigned int uiBytesWritten = 0;
unsigned int uiLineAddress;
unsigned int uiAddressLength;
unsigned int uiLineIndex = 4/*here starts the Address*/;
char *cpDestPtr;
char cCh = 0;
char bData;
unsigned int uiLength = cExtractSRecordLineLength(cpSrcString);
char cChecksum = cExtractSRecordChecksum(cpSrcString);



   if (cChecksum == cCalculateSRecordChecksum(cpSrcString) )
   {
      uiLineAddress = uiExtractSRecordStartAddress(cpSrcString, &uiAddressLength, &bData);
      if (AiTrue == bData)
      {
         /* Generate Target Address */
         cpDestPtr = cpDestOffset + uiLineAddress;
         uiLineIndex += uiAddressLength;

         /* Extract Byte fpr Byte and write to Target */
         do
         {
            cCh <<= 4;
            cCh |= AsciiToInt(cpSrcString[uiLineIndex++]);
            cCh <<= 4;
            cCh |= AsciiToInt(cpSrcString[uiLineIndex++]);

            if (cpDestPtr < (cpDestOffset+uiDestSize) )
            {
               *cpDestPtr++ = cCh;
               cCh = 0;
               uiBytesWritten++;
            }
            else
            {
               printf("Data out of Range!\n");
            }
         }
         while ( uiLineIndex <= (uiLength-4));
      }
      else
      {
         printf("Line is not a Data Record!\n");
         uiBytesWritten = 0;
      }
   }
   else
   {
      printf("Checksum Error!\n");
      uiBytesWritten = 0;
   }
   return uiBytesWritten;
}



/****************************************************************************
   Function    : AsciiToInt 
-----------------------------------------------------------------------------
   Create      : 27.07.2011 (rh)    Update  : __.__.20__ ()
-----------------------------------------------------------------------------

   Inputs      : ASCII character

   Outputs     : -

   Return      : Byte Value

   Description : Converts an ASCII character to a Byte value.

   Modification: 


*****************************************************************************/
char AsciiToInt( char cAsciiChar)
{
char cXhar;

   switch (cAsciiChar)
   {
      case 0x30: cXhar = 0; break;
      case 0x31: cXhar = 1; break;
      case 0x32: cXhar = 2; break;
      case 0x33: cXhar = 3; break;
      case 0x34: cXhar = 4; break;
      case 0x35: cXhar = 5; break;
      case 0x36: cXhar = 6; break;
      case 0x37: cXhar = 7; break;
      case 0x38: cXhar = 8; break;
      case 0x39: cXhar = 9; break;

      case 'a': cXhar = 10;
      case 'A': cXhar = 10; break;
      case 'b': cXhar = 11;
      case 'B': cXhar = 11; break;
      case 'c': cXhar = 12;
      case 'C': cXhar = 12; break;
      case 'd': cXhar = 13;
      case 'D': cXhar = 13; break;
      case 'e': cXhar = 14;
      case 'E': cXhar = 14; break;
      case 'f': cXhar = 15;
      case 'F': cXhar = 15; break;
      default: cXhar = 0; break;
   }
   return cXhar;
}


