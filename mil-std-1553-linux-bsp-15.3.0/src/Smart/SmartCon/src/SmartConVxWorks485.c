/* SPDX-FileCopyrightText: 2009-2021 AIM GmbH <info@aim-online.com> */
/* SPDX-License-Identifier: MIT  */

/*!
SmartConVxWorks485.cpp
Contains the SMART VxWorks entry point for the console program (function "int update()" )
After mapping a single board will be updated.

@remarks n/a

*/
/* SmartCon.cpp : Contains the SMART VxWorks entry point. */



#if _MIL_1553__
#include "api1553.h"
#endif
#include "Aim485Lib.h"

#include "AiVmeGeneric.h"
#include "stdio.h"
#include "string.h"  /* for memset */
#include "stdlib.h"  /* for exit */



#define __XVB601_LOCAL__

#define API_ERR_OS_NOT_SUPPORTED                  0x018C

/*! \struct api_file_upload_in
 * This structure holds the input parameters for
 * the \ref ApiFileUpload API function
 */
typedef struct api_file_upload_in
{
    const AiChar* pc_DestFilePath;      /*!< path to destination file on host where uploaded file will be stored */
    const AiChar* pc_SourceFileName;    /*!< Name of the file on the embedded device to upload */
    AiUInt32 ul_Flags;                  /*!< File flags for the destination file e.g. \ref API_FILE_FLAG_OVERWRITE to allow overwriting */
} TY_API_FILE_UPLOAD_IN;

/*! \enum api429_e_update_status
 *
 * Enumeration of all possible update states
 * when using ApiUpdateStatus
 */
typedef enum api_e_update_status
{
    API_UPDATE_DEV_CON_FAILURE = -1, /*!< Update related connection to board is broken. No update possible any more */
    API_UPDATE_IDLE,                 /*!< Device is ready for updating */
    API_UPDATE_START_REQUESTED,      /*!< Device update has been requested but has not started yet */
    API_UPDATE_IN_PROGRESS,          /*!< Device update is currently in progress */
    API_UPDATE_FAILED,               /*!< Device update procedure failed */
    API_UPDATE_FINISHED              /*!< Device update procedure successfully finished */
} TY_API_E_UPDATE_STATUS;


/*! \struct api_packet_update_status_out
 *
 * This structure holds the output parameters for
 * the ApiUpdateStatus function
 */
typedef struct api_packet_update_status_out
{
    TY_API_E_UPDATE_STATUS e_Status;            /*!< Current update procedure status. See \ref api_e_update_status */
    AiUInt32               ul_ComponentsDone;   /*!< Number of components that have been updated so far. Only valid when
                                                        update procedure is in progress, finished or failed */
    AiUInt32               ul_TotalComponents;  /*!< Total number of components that have to be updated.
                                                        Only valid when update procedure is in progress, finished or failed */
} TY_API_PACKET_UPDATE_STATUS_OUT;



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

#ifdef __XVB601__
#define VME_A16_ADDRESS   0xFBFFC000				// CPU dependent (according to address settings of VME carrier)
   #define VME_A32_ADDRESS   0x08000000				// CPU dependent 
   #define VME_USER_ACCESS   0x20000000				// CPU dependent 
   #define AVI_VME_INTERRUPT_LEVEL   	0x06		// CPU dependent (not used at the moment)
   #define AVI_VME_INTERRUPT_VECTOR  	0xC1		// CPU dependent (not used at the moment)
#endif

#ifdef __XVB601_LOCAL__
   #define VME_A16_ADDRESS 0xFFFFFFFF              // value indicates local PCI access
   #define VME_A32_ADDRESS 0x40000000              // CPU dependent
   #define VME_USER_ACCESS 0x40000000				// CPU dependent
   #define AVI_VME_INTERRUPT_LEVEL   	0x06		// CPU dependent (not used at the moment)
   #define AVI_VME_INTERRUPT_VECTOR  	0xC1		// CPU dependent (not used at the moment)
#endif








/* function prototypes */
int main( int argc, char *argv[]);
void checkA16Addr( AiUInt32 A16Addr );








int update( void )
{
#if 1
    /* normal update */
    char *argv[4] = {"SMART", "-u", "AIM/aim485/SRE/update.upd", "update"};
    int  argc = 4;
#endif

#if 0
    /* normal update */
    char *argv[3] = {"SMART", "-u", "AIM/485/SRE/update.upd"};
    int  argc = 3;
#endif

#if 0
    /* force update */
    char *argv[5] = {"SMART", "--force", "-u", "AIM/485/SRE/update.upd", "update"};
    int  argc = 5;
#endif

#if 0
    /* start menu */
    char *argv[1] = {"SMART"};
    int  argc = 1;
#endif

    AiUInt8            ucModuleHandle  = 0;
    AiUInt32           ulNumberOfBoards  = 0;
	AiReturn           r_RetVal = 0;
    

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



    r_RetVal = Ai485_Init(&ulNumberOfBoards);
    

#if _MIL_1553__

    ApiInit();
    
    /* -- 2.) Map the board --- */
    /* in case the board is located on the local PCI bus */
    if(VME_A16_ADDRESS == 0xFFFFFFFF)
    {
      AiPciScan();
      memset(&xVmeInitIn, 0, sizeof(xVmeInitIn));
      xVmeInitIn.px_PCI_Info = AiPciGetHeader(0);
      xVmeInitIn.ul_A32Addr = VME_A32_ADDRESS;
      xVmeInitIn.ul_A32UserAccess = VME_USER_ACCESS;
      ucModuleHandle = AiVme1553MapModule(&xVmeInitIn);
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
      ucModuleHandle = AiVme1553MapModule(&xVmeInitIn);
    }
#endif

    main( argc, argv );



#if _MIL_1553__
    AiVme1553UnmapModule(&xVmeInitIn);
#endif
    
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
AI_LIB_FUNC AiReturn AI_CALL_CONV ApiFileDownload( AiUInt32 module, const char* sz_FilePath)
{
	return API_ERR_OS_NOT_SUPPORTED;
}
AI_LIB_FUNC AiReturn AI_CALL_CONV ApiFileUpload( AiUInt32 module, TY_API_FILE_UPLOAD_IN* px_UploadIn)
{
	return API_ERR_OS_NOT_SUPPORTED;
}
AI_LIB_FUNC AiReturn AI_CALL_CONV ApiUpdateStatus( AiUInt32 module, TY_API_PACKET_UPDATE_STATUS_OUT* px_UpdateStatus)
{
	return API_ERR_OS_NOT_SUPPORTED;
}
AI_LIB_FUNC AiReturn AI_CALL_CONV ApiUpdateStart( AiUInt32 module, const AiChar* updatePackageName)
{
	return API_ERR_OS_NOT_SUPPORTED;
}
