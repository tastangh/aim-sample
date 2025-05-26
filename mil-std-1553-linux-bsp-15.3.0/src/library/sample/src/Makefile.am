# SPDX-FileCopyrightText: 2010-2024 AIM GmbH <info@aim-online.com>
# SPDX-License-Identifier: MIT


bin_PROGRAMS = sample

sample_SOURCES = ls_buffer_queue_sample.c \
                 ls_dynamic_data_sample.c \
                 ls_recording_replay_sample.c \
                 ls_instruction_list_sample.c \
                 ls_acyclic_sample.c \
                 ls_interrupt_sample.c \
                 ls_bc_rt_bm_sample.c \
                 ls_bc_rt_bm_tcb_sample.c \
                 print_versions.c \
                 template_sample.c \
                 ls_milscope_sample_ape.c \
                 ls_fifo_sample.c \
                 os/linux/ls_ptp_vtp_sample_dtp_server.c \
                 ls_ptp_vtp_sample.c \
                 ls_bm_sample.c \
                 ls_bc_sample.c \
                 mil_util.c
if HS
sample_SOURCES += hs/3910_Efex_sample.c \
                  hs/ef_bc_rt_sample.c \
                  hs/hs_buffer_queue_sample.c \
                  hs/hs_recording_replay_sample.c \
                  hs/hs_interrupt_sample.c \
                  hs/hs_bc_rt_sample.c \
                  hs/ef_efa_efex_switch_sample.c \
                  hs/ef_recording_sample.c \
                  hs/mil_hs_util.c \
                  hs/mil_ef_util.c
else
sample_SOURCES += 1553_sample.c
endif

if AIM_XENOMAI
sample_CFLAGS  = -I=@XENOMAI_INCLUDES@ -D_GNU_SOURCE -D_REENTRANT -D__XENO__
sample_LDFLAGS = -lnative -L@XENOMAI_LIBS@ -lxenomai -lpthread -lrt
sample_CFLAGS += -D_AIM_LINUX -D_AIM_XENOMAI
sample_CFLAGS += -I../include -I../../include -I@AIM_BSP_COMMON@/include -I@MIL_TSW@/include
sample_CFLAGS += -L../../src/.libs -L../../src
sample_LDADD     = -laim_mil_xenomai
else
sample_CFLAGS  = -D_AIM_LINUX
sample_CFLAGS += -I../include -I../../include -I@AIM_BSP_COMMON@/include -I@MIL_TSW@/include
sample_CFLAGS += -L../../src/.libs -L../../src
sample_LDADD     = -laim_mil
endif
