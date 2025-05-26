/* SPDX-FileCopyrightText: 2018-2024 AIM GmbH <info@aim-online.com> */
/* SPDX-License-Identifier: MIT OR GPL-2.0-or-later */

/*! \file mil_hw_pci_driver.h 
    This file contains hw layer declarations for AyE without ASP, Artix7 without ASP, Ays without ASP.

    mil_hw_pci_driver.c               => Generic functions
    mil_hw_pci_driver_aye.c           => AyE related
    mil_hw_pci_driver_aye_pxi.c       => AyE PXI and Trigger related
    mil_hw_pci_driver_aye_milscope.c  => AyE Milscope related
    mil_hw_pci_driver_ays.c           => AyS related
    mil_hw_pci_driver_artix7.c        => Artix7 related
*/
/*********************************************************************************/

#ifndef __MIL_HW_PCI_DRIVER_H__
#define __MIL_HW_PCI_DRIVER_H__


//mil_hw_pci_driver_aye.c
AiUInt32 mil_hw_pci_driver_aye_novram_write(TY_API_DEV *p_api_dev, AiUInt32 ulOffset, AiUInt32 ulValue);
AiUInt32 mil_hw_pci_driver_aye_novram_read(TY_API_DEV *p_api_dev, AiUInt32 ulOffset, AiUInt32 *pulValue);
AiUInt32 mil_hw_pci_driver_aye_novram_finalize(TY_API_DEV *p_api_dev);
L_WORD mil_hw_pci_driver_aye_irig_set_external(TY_API_DEV *p_api_dev, BYTE con);
L_WORD mil_hw_pci_driver_aye_irig_set(TY_API_DEV * p_api_dev, L_WORD day, L_WORD hour, L_WORD min, L_WORD sec);
L_WORD mil_hw_pci_driver_aye_irig_get_status(TY_API_DEV *p_api_dev, AiUInt32 * irig_sync, AiUInt32 * irig_src);
L_WORD mil_hw_pci_driver_aye_irig_get_time(TY_API_DEV * p_api_dev, L_WORD * sth, L_WORD * stl);

L_WORD mil_hw_pci_driver_aye_version_fw(TY_API_DEV *p_api_dev, TY_E_VERSION_ID eId, TY_API_VER_INFO_VALUES * pxVersionValue, TY_API_VER_INFO_STRINGS * pxVersionString);
L_WORD mil_hw_pci_driver_aye_version_fpga(TY_API_DEV *p_api_dev, TY_E_VERSION_ID eId, TY_API_VER_INFO_VALUES * pxVersionValue, TY_API_VER_INFO_STRINGS * pxVersionString);
L_WORD mil_hw_pci_driver_aye_version_mlca(TY_API_DEV *p_api_dev, TY_E_VERSION_ID eId, TY_API_VER_INFO_VALUES * pxVersionValue, TY_API_VER_INFO_STRINGS * pxVersionString);
L_WORD mil_hw_pci_driver_aye_version_tcp(TY_API_DEV *p_api_dev, TY_E_VERSION_ID eId, TY_API_VER_INFO_VALUES * pxVersionValue, TY_API_VER_INFO_STRINGS * pxVersionString);
L_WORD mil_hw_pci_driver_aye_version_monitor(TY_API_DEV *p_api_dev, TY_E_VERSION_ID eId, TY_API_VER_INFO_VALUES * pxVersionValue, TY_API_VER_INFO_STRINGS * pxVersionString);

AiReturn mil_hw_pci_driver_aye_discretes_fifo_read(TY_API_DEV *p_api_dev, AiUInt32 int_count);
AiReturn mil_hw_pci_driver_aye_discretes_fifo(TY_API_DEV *p_api_dev, AiUInt32 new_config);
AiReturn mil_hw_pci_driver_aye_discretes_init(TY_API_DEV *p_api_dev, AiUInt32 new_config);
AiReturn mil_hw_pci_driver_aye_discretes_write(TY_API_DEV *p_api_dev, AiUInt32 Mask, AiUInt32 Value);
AiReturn mil_hw_pci_driver_aye_discretes_read(TY_API_DEV *p_api_dev, AiUInt32 * Value);
AiReturn mil_hw_pci_driver_aye_get_discretes(TY_API_DEV * p_api_dev, AiUInt32 * pulDiscreteCount, AiUInt32 * pulDiscreteConfig);
AiReturn mil_hw_pci_driver_aye_discretes_info_read(TY_API_DEV * p_api_dev, AiUInt32 *pul_Channel, AiUInt32 *pul_Canin, AiUInt32 *pul_Canout);
enum MIL_FW mil_hw_pci_driver_aye_get_fw_type(TY_API_DEV * p_api_dev, short ulBiu);
AiBoolean mil_hw_pci_driver_aye_has_digital_loop(TY_API_DEV* p_api_dev);
AiReturn mil_hw_pci_driver_aye_trigger_digital_loop_con(TY_API_DEV* p_api_dev, AiUInt32 con);
AiReturn mil_hw_pci_driver_aye_trigger_digital_loop_get(TY_API_DEV* p_api_dev, AiUInt32 * con);



//mil_hw_pci_driver_ays.c
AiUInt32 mil_hw_pci_driver_ays_novram_write(TY_API_DEV *p_api_dev, AiUInt32 ulOffset, AiUInt32 ulValue);
AiUInt32 mil_hw_pci_driver_ays_novram_read(TY_API_DEV *p_api_dev, AiUInt32 ulOffset, AiUInt32 *pulValue);
AiUInt32 mil_hw_pci_driver_ays_novram_finalize(TY_API_DEV *p_api_dev);
L_WORD mil_hw_pci_driver_ays_irig_set_external(TY_API_DEV *p_api_dev, BYTE con);
L_WORD mil_hw_pci_driver_ays_irig_set(TY_API_DEV * p_api_dev, L_WORD day, L_WORD hour, L_WORD min, L_WORD sec);
L_WORD mil_hw_pci_driver_ays_irig_get_status(TY_API_DEV *p_api_dev, AiUInt32 * irig_sync, AiUInt32 * irig_src);
L_WORD mil_hw_pci_driver_ays_irig_get_time(TY_API_DEV * p_api_dev, L_WORD * sth, L_WORD * stl);

L_WORD mil_hw_pci_driver_ays_version_fw(TY_API_DEV *p_api_dev, TY_E_VERSION_ID eId, TY_API_VER_INFO_VALUES * pxVersionValue, TY_API_VER_INFO_STRINGS * pxVersionString);
L_WORD mil_hw_pci_driver_ays_version_fpga(TY_API_DEV *p_api_dev, TY_E_VERSION_ID eId, TY_API_VER_INFO_VALUES * pxVersionValue, TY_API_VER_INFO_STRINGS * pxVersionString);
L_WORD mil_hw_pci_driver_ays_version_mlca(TY_API_DEV *p_api_dev, TY_E_VERSION_ID eId, TY_API_VER_INFO_VALUES * pxVersionValue, TY_API_VER_INFO_STRINGS * pxVersionString);
L_WORD mil_hw_pci_driver_ays_version_tcp(TY_API_DEV *p_api_dev, TY_E_VERSION_ID eId, TY_API_VER_INFO_VALUES * pxVersionValue, TY_API_VER_INFO_STRINGS * pxVersionString);
L_WORD mil_hw_pci_driver_ays_version_os(TY_API_DEV *p_api_dev, TY_E_VERSION_ID eId, TY_API_VER_INFO_VALUES * pxVersionValue, TY_API_VER_INFO_STRINGS * pxVersionString);

AiReturn mil_hw_pci_driver_ays_discretes_init(TY_API_DEV *p_api_dev, AiUInt32 new_config);
AiReturn mil_hw_pci_driver_ays_discretes_write(TY_API_DEV *p_api_dev, AiUInt32 Mask, AiUInt32 Value);
AiReturn mil_hw_pci_driver_ays_discretes_read(TY_API_DEV *p_api_dev, AiUInt32 * Value);
AiReturn mil_hw_pci_driver_ays_get_discretes(TY_API_DEV * p_api_dev, AiUInt32 * pulDiscreteCount, AiUInt32 * pulDiscreteConfig);
AiReturn mil_hw_pci_driver_ays_discretes_info_read(TY_API_DEV * p_api_dev, AiUInt32 *pul_Channel, AiUInt32 *pul_Canin, AiUInt32 *pul_Canout);
enum MIL_FW mil_hw_pci_driver_ays_get_fw_type(TY_API_DEV * p_api_dev, short ulBiu);
AiBoolean mil_hw_pci_driver_ays_has_digital_loop(TY_API_DEV* p_api_dev);
AiReturn mil_hw_pci_driver_ays_trigger_digital_loop_con(TY_API_DEV* p_api_dev, AiUInt32 con);
AiReturn mil_hw_pci_driver_ays_trigger_digital_loop_get(TY_API_DEV* p_api_dev, AiUInt32 * con);


//mil_hw_pci_driver_artix7.c
AiUInt32 mil_hw_pci_driver_artix7_novram_write(TY_API_DEV *p_api_dev, AiUInt32 ulOffset, AiUInt32 ulValue);
AiUInt32 mil_hw_pci_driver_artix7_novram_read(TY_API_DEV *p_api_dev, AiUInt32 ulOffset, AiUInt32 *pulValue);
AiUInt32 mil_hw_pci_driver_artix7_novram_finalize(TY_API_DEV *p_api_dev);
L_WORD mil_hw_pci_driver_artix7_irig_set_external(TY_API_DEV *p_api_dev, BYTE con);
L_WORD mil_hw_pci_driver_artix7_irig_set(TY_API_DEV * p_api_dev, L_WORD day, L_WORD hour, L_WORD min, L_WORD sec, L_WORD us);
L_WORD mil_hw_pci_driver_artix7_irig_get_status(TY_API_DEV *p_api_dev, AiUInt32 * irig_sync, AiUInt32 * irig_src);
L_WORD mil_hw_pci_driver_artix7_irig_get_time(TY_API_DEV * p_api_dev, L_WORD * sth, L_WORD * stl);
L_WORD mil_hw_pci_driver_artix7_irig_set_capabilities(TY_API_DEV* p_api_dev);

L_WORD mil_hw_pci_driver_artix7_version_fw(TY_API_DEV *p_api_dev, TY_E_VERSION_ID eId, TY_API_VER_INFO_VALUES * pxVersionValue, TY_API_VER_INFO_STRINGS * pxVersionString);
L_WORD mil_hw_pci_driver_artix7_version_tcp(TY_API_DEV *p_api_dev, TY_E_VERSION_ID eId, TY_API_VER_INFO_VALUES * pxVersionValue, TY_API_VER_INFO_STRINGS * pxVersionString);
L_WORD mil_hw_pci_driver_artix7_version_mlca(TY_API_DEV *p_api_dev, TY_E_VERSION_ID eId, TY_API_VER_INFO_VALUES * pxVersionValue, TY_API_VER_INFO_STRINGS * pxVersionString);
L_WORD mil_hw_pci_driver_artix7_version_monitor(TY_API_DEV *p_api_dev, TY_E_VERSION_ID eId, TY_API_VER_INFO_VALUES * pxVersionValue, TY_API_VER_INFO_STRINGS * pxVersionString);

L_WORD api_sys_read_version_fw_artix7(TY_API_DEV *p_api_dev, TY_E_VERSION_ID eId, TY_API_VER_INFO_VALUES * pxVersionValue, TY_API_VER_INFO_STRINGS * pxVersionString);

AiReturn mil_hw_pci_driver_artix7_discretes_init(TY_API_DEV *p_api_dev, AiUInt32 new_config);
AiReturn mil_hw_pci_driver_artix7_discretes_write(TY_API_DEV *p_api_dev, AiUInt32 Mask, AiUInt32 Value);
AiReturn mil_hw_pci_driver_artix7_discretes_read(TY_API_DEV *p_api_dev, AiUInt32 * Value);
AiReturn mil_hw_pci_driver_artix7_get_discretes(TY_API_DEV * p_api_dev, AiUInt32 * pulDiscreteCount, AiUInt32 * pulDiscreteConfig);
AiReturn mil_hw_pci_driver_artix7_discretes_info_read(TY_API_DEV * p_api_dev, AiUInt32 *pul_Channel, AiUInt32 *pul_Canin, AiUInt32 *pul_Canout);
enum MIL_FW mil_hw_pci_driver_artix7_get_fw_type(TY_API_DEV * p_api_dev, short ulBiu);
AiBoolean mil_hw_pci_driver_artix7_has_digital_loop(TY_API_DEV* p_api_dev);
AiReturn mil_hw_pci_driver_artix7_trigger_digital_loop_con(TY_API_DEV* p_api_dev, AiUInt32 con);
AiReturn mil_hw_pci_driver_artix7_trigger_digital_loop_get(TY_API_DEV* p_api_dev, AiUInt32 * con);

//mil_hw_pci_driver_artixus.c
AiUInt32 mil_hw_pci_driver_artixus_novram_write(TY_API_DEV* p_api_dev, AiUInt32 ulOffset, AiUInt32 ulValue);
AiUInt32 mil_hw_pci_driver_artixus_novram_read(TY_API_DEV* p_api_dev, AiUInt32 ulOffset, AiUInt32* pulValue);
AiUInt32 mil_hw_pci_driver_artixus_novram_finalize(TY_API_DEV* p_api_dev);
L_WORD mil_hw_pci_driver_artixus_irig_set_external(TY_API_DEV* p_api_dev, BYTE con);
L_WORD mil_hw_pci_driver_artixus_irig_set(TY_API_DEV* p_api_dev, L_WORD day, L_WORD hour, L_WORD min, L_WORD sec, L_WORD us);
L_WORD mil_hw_pci_driver_artixus_irig_get_status(TY_API_DEV* p_api_dev, AiUInt32* irig_sync, AiUInt32* irig_src);
L_WORD mil_hw_pci_driver_artixus_irig_get_time(TY_API_DEV* p_api_dev, L_WORD* sth, L_WORD* stl);
L_WORD mil_hw_pci_driver_artixus_irig_set_capabilities(TY_API_DEV* p_api_dev);

L_WORD mil_hw_pci_driver_artixus_version_fw(TY_API_DEV* p_api_dev, TY_E_VERSION_ID eId, TY_API_VER_INFO_VALUES* pxVersionValue, TY_API_VER_INFO_STRINGS* pxVersionString);
L_WORD mil_hw_pci_driver_artixus_version_tcp(TY_API_DEV* p_api_dev, TY_E_VERSION_ID eId, TY_API_VER_INFO_VALUES* pxVersionValue, TY_API_VER_INFO_STRINGS* pxVersionString);
L_WORD mil_hw_pci_driver_artixus_version_mlca(TY_API_DEV* p_api_dev, TY_E_VERSION_ID eId, TY_API_VER_INFO_VALUES* pxVersionValue, TY_API_VER_INFO_STRINGS* pxVersionString);
L_WORD mil_hw_pci_driver_artixus_version_monitor(TY_API_DEV* p_api_dev, TY_E_VERSION_ID eId, TY_API_VER_INFO_VALUES* pxVersionValue, TY_API_VER_INFO_STRINGS* pxVersionString);

L_WORD api_sys_read_version_fw_artixus(TY_API_DEV* p_api_dev, TY_E_VERSION_ID eId, TY_API_VER_INFO_VALUES* pxVersionValue, TY_API_VER_INFO_STRINGS* pxVersionString);

AiReturn mil_hw_pci_driver_artixus_discretes_init(TY_API_DEV* p_api_dev, AiUInt32 new_config);
AiReturn mil_hw_pci_driver_artixus_discretes_write(TY_API_DEV* p_api_dev, AiUInt32 Mask, AiUInt32 Value);
AiReturn mil_hw_pci_driver_artixus_discretes_read(TY_API_DEV* p_api_dev, AiUInt32* Value);
AiReturn mil_hw_pci_driver_artixus_get_discretes(TY_API_DEV* p_api_dev, AiUInt32* pulDiscreteCount, AiUInt32* pulDiscreteConfig);
AiReturn mil_hw_pci_driver_artixus_discretes_info_read(TY_API_DEV* p_api_dev, AiUInt32* pul_Channel, AiUInt32* pul_Canin, AiUInt32* pul_Canout);
enum MIL_FW mil_hw_pci_driver_artixus_get_fw_type(TY_API_DEV* p_api_dev, short ulBiu);
AiBoolean mil_hw_pci_driver_artixus_has_digital_loop(TY_API_DEV* p_api_dev);
AiReturn mil_hw_pci_driver_artixus_trigger_digital_loop_con(TY_API_DEV* p_api_dev, AiUInt32 con);
AiReturn mil_hw_pci_driver_artixus_trigger_digital_loop_get(TY_API_DEV* p_api_dev, AiUInt32 * con);

#endif
