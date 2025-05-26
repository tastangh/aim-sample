/* SPDX-FileCopyrightText: 2009-2021 AIM GmbH <info@aim-online.com> */
/* SPDX-License-Identifier: MIT  */

/*!
SmartConVxWorks429.c
Contains the SMART VxWorks entry point for the console program (function "int update()" )
After mapping a single board will be updated.

@remarks n/a

*/
/* SmartCon.cpp : Contains the SMART VxWorks entry point. */



#include "api429.h"
#include "AiVmeGeneric.h"
#include "Api429Update.h"
#include "stdio.h"
#include "string.h"  /* for memset */
#include "stdlib.h"  /* for exit */



#define __MVME5100__



#ifdef __MVME5100__
   #define VME_A16_ADDRESS    0xFBFFC000			/* This is the Board VME A16 Address. */
   #define VME_A32_ADDRESS    0x08000000			/* This is the Board VME A32 Address defined as real physical address. */
   #define VME_USER_ACCESS    0x20000000			/* This is the Address, where the PCC acesses the VME-Bus (OS physical address) */
#endif

#ifdef __MVME6100__
   #define VME_A16_ADDRESS    0x9100C000
   #define VME_A32_ADDRESS    0x08000000
   #define VME_USER_ACCESS    0x80000000
#endif











/* function prototypes */
int main( int argc, char *argv[]);
void checkA16Addr( AiUInt32 A16Addr );








int update( void )
{
#if 1
    /* normal update */
    char *argv[4] = {"SMART", "-u", "AIM/Arinc429/SRE/update.upd", "update"};
    int  argc = 4;
#endif

#if 0
    /* normal update */
    char *argv[3] = {"SMART", "-u", "AIM/Arinc429/SRE/update.upd"};
    int  argc = 3;
#endif

#if 0
    /* force update */
    char *argv[5] = {"SMART", "--force", "-u", "AIM/Arinc429/SRE/update.upd", "update"};
    int  argc = 5;
#endif

#if 0
    /* start menu */
    char *argv[1] = {"SMART"};
    int  argc = 1;
#endif

    AiUInt8            ucModuleHandle  = 0;

    TY_VME_EXAMINE_SLOT xVmePreInitin;
    TY_PCI_INFO out1, out2;
    TY_VME_MAP_MODULE_IN xVmeInitIn;

#ifdef __MVME6100__
AiUInt32 *pul_Tmp;
    pul_Tmp = (AiUInt32*)(0xa0000318);
    *pul_Tmp = 0;
    pul_Tmp = (AiUInt32*)(0xa000012c);
    *pul_Tmp = 0x90000000;
#endif  



    Api429LibInit();

    /* -- 2.) Map the board --- */
    /* in case the board is located on the local PCI bus */
    if(VME_A16_ADDRESS == 0xFFFFFFFF)
    {
      AiPciScan();
      memset(&xVmeInitIn, 0, sizeof(xVmeInitIn));
      xVmeInitIn.px_PCI_Info = AiPciGetHeader(0);
      xVmeInitIn.ul_A32Addr = VME_A32_ADDRESS;
      xVmeInitIn.ul_A32UserAccess = VME_USER_ACCESS;
      ucModuleHandle = AiVme429MapModule(&xVmeInitIn);
    }
    
    /* in case the board is located on the VME bus */
    if(VME_A16_ADDRESS != 0xFFFFFFFF)
  	{
      checkA16Addr(VME_A16_ADDRESS); /* this is optional, but may help to find the correct A16 address */


      memset(&xVmePreInitin, 0, sizeof(xVmePreInitin));
      xVmePreInitin.ul_A16Addr = VME_A16_ADDRESS;
      xVmePreInitin.ul_TempA32Addr = VME_A32_ADDRESS;
      xVmePreInitin.ul_TempA32UserAccess = VME_USER_ACCESS;
      AiVmeExamineSlot(&xVmePreInitin, &out1, &out2);

      memset(&xVmeInitIn, 0, sizeof(xVmeInitIn));
      xVmeInitIn.px_PCI_Info = &out1;			       /* update the PMC1 on an AVC-2 carrier */
      /*xVmeInitIn.px_PCI_Info = &out2;*/
      xVmeInitIn.ul_A32Addr = VME_A32_ADDRESS;
      xVmeInitIn.ul_A32UserAccess = VME_USER_ACCESS;
      ucModuleHandle = AiVme429MapModule(&xVmeInitIn);
    }


    main( argc, argv );



    AiVme429UnmapModule(&xVmeInitIn);
    
    return 0;
}



int TestApplication(void)
{
   return update();
}




void checkA16Addr( AiUInt32 A16Addr )
{
  AiUInt32 *A16Ptr = (AiUInt32 *)A16Addr;

  if( A16Addr == 0xFFFFFFFF )
  {
    /*printf("checkA16Addr:   Local PCI bus is used and no VME bus.  No check is done.\r\n");*/
    return;
  }


  printf("checking A16 Address:   %08x-->%p: %08X\r\n", A16Addr, A16Ptr, *A16Ptr);


  if( (*A16Ptr != BSWAP32(0x000010E3))  &&
      (*A16Ptr != BSWAP32(0x014810E3))  )
  {
    printf("  Nothing found at the given A16 Address. Aborting\r\n");
    exit(-1);
  }
  else
  {
    printf("  A16 Address seems to be OK\r\n");
  }
}


/* add dummies for the esmart update */
AI_LIB_FUNC AiReturn AI_CALL_CONV Api429FileDownload( AiUInt8 module, const char* sz_FilePath)
{
	return AI429_ERR_FUNCTION_NOT_IMPLEMENTED;
}
AI_LIB_FUNC AiReturn AI_CALL_CONV Api429FileUpload( AiUInt8 module, struct api429_file_upload_in* px_UploadIn)
{
	return AI429_ERR_FUNCTION_NOT_IMPLEMENTED;
}
AI_LIB_FUNC AiReturn AI_CALL_CONV Api429UpdateStatus( AiUInt8 module, struct a429_packet_update_status_out* px_UpdateStatus)
{
	return AI429_ERR_FUNCTION_NOT_IMPLEMENTED;
}
AI_LIB_FUNC AiReturn AI_CALL_CONV Api429UpdateStart( AiUInt8 module, const AiChar* updatePackageName)
{
	return AI429_ERR_FUNCTION_NOT_IMPLEMENTED;
}
