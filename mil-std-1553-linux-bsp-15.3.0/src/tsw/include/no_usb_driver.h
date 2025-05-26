/* SPDX-FileCopyrightText: 2015-2022 AIM GmbH <info@aim-online.com> */
/* SPDX-License-Identifier: MIT OR GPL-2.0-or-later */

#ifndef _NO_USB_DRIVER_H
#define _NO_USB_DRIVER_H

/* Functions defined in UsbGlobalMemoryManagement.h */




#define UsbSynchronizeLWord(                         a, b, c             )
#define UsbSynchronizeSystemControlBlock(            a, b, c             )
#define UsbSynchronizeBC(                            a, b, c             )
#define UsbSynchronizeRT(                            a, b, c             )
#define UsbSynchronizeTransfers(                     a, b, c             )
#define UsbSynchronizeTransfer(                      a, b, c, d          )
#define UsbSynchronizeTransferByID(                  a, b, c, d, e       )
#define UsbSynchronizeInstructionLists(              a, b, c             )
#define UsbSynchronizeBCSystemControlBlock(          a, b, c             )
#define UsbSynchronizeMemoryArea(                    a, b, c, d          )
#define UsbSynchronizeAcyclicInstructionList(        a, b, c             )
#define UsbSynchronizeRTDescriptorArea(              a, b, c             )
#define UsbSynchronizeRTSubAddressDescriptorArea(    a, b, c             )
#define UsbSynchronizeRTBuffers(                     a, b, c             )
#define UsbSynchronizeRTBuffer(                      a, b, c             )
#define UsbSynchronizeRTSystemControlBlock(          a, b, c             )
#define UsbSynchronizeBM(                            a, b, c             )
#define UsbSynchronizeMonitorDataBuffer(             a, b, c             )
#define UsbSynchronizeMonitorActivityFilteringPage(  a, b, c             )
#define UsbSynchronizeTriggerControlBlockArea(       a, b, c             )
#define UsbSynchronizeBMSystemControlBlock(          a, b, c             )
#define UsbSynchronizeInterruptLoglist(              a, b, c             )
#define UsbSynchronizeReplay(                        a, b, c             )
#define UsbSynchronizeReplaySystemControlBlock(      a, b, c             )
#define UsbSynchronizeReplayBuffer(                  a, b, c             )
#define UsbSynchronizeActiveMemoryBlocks(            a, b, c, d          )
#define UsbSynchronizeActiveBiuMemory(               a, b, c, d, e, f, g )
#define UsbSynchronizeActiveInterruptLoglistMemory(  a, b, c, d, e, f, g )
#define UsbSynchronizeActiveMonitorActivityMemory(   a, b, c, d, e, f, g )
#define UsbSynchronizeActiveRTBufferHeaderMemory(    a, b, c, d, e, f, g )
#define UsbSynchronizeActiveRTStatusQueueMemory(     a, b, c, d, e, f, g )
#define UsbSynchronizeActiveRTEventQueueMemory(      a, b, c, d, e, f, g )
#define UsbSynchronizeActiveBCBufferHeaderMemory(    a, b, c, d, e, f, g )
#define UsbSynchronizeActiveBCStatusQueueMemory(     a, b, c, d, e, f, g )
#define UsbSynchronizeActiveBCEventQueueMemory(      a, b, c, d, e, f, g )
#define UsbSynchronizeActiveMonitorBufferMemory(     a, b, c, d, e, f, g )
#define UsbSynchronizeActiveSimulationBufferMemory(  a, b, c, d, e, f, g )
#define UsbSynchronizeBCMiscBlock(                   a, b, c             )
#define UsbSynchAndReadLWord(                        a, b                )
#define UsbTransferListAdd(                          a, b, c, d, e       )
#define UsbTransferListClear(                        a, b                )
#define UsbRTBufferListAdd(                          a, b, c, d          )
#define UsbRTBufferListClear(                        a, b                )
#define UsbSynchronizeMonitorActivityPageOfRT(       a, b, c, d          )
#define UsbSynchronizeMonitorActivityPageOfSA(       a, b, c, d, e, f    )
#define UsbSynchronizeRTDescriptor(                  a, b, c, d          )

/* Functions defined in UsbPciLcaAccess.h */

#define UsbPciLcaRegisterRead(  a, b    ) 0
#define UsbPciLcaRegisterWrite( a, b, c )


/* Functions defined in UsbTcpAccess.h */
#define UsbTcpPortRead( a, b, c )
#define UsbTcpPortWrite( a, b, c )

#endif /* _NO_USB_DRIVER_H */


