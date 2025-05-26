# SPDX-FileCopyrightText: 2013-2021 AIM GmbH <info@aim-online.com>
# SPDX-License-Identifier: MIT

noinst_LIBRARIES = libans-common.a
libans_common_a_SOURCES = Ai_Options.c Ai_Strncpy.c Ai_Strdup.c Ai_Cmdline.c ANS_Log.c Ai_Log.c Ai_Sem.c \
                          Ai_Logindex.c Ai_Date.c Ai_Time.c ANS_Client.c Ai_Opterror.c ANS_Thread.c \
                          Ai_Socket.c Ai_Sockll.c ANS_Server.c Ai_Socksv.c Ai_Socktl.c \
                          ANS_Multiplexer.c ANS_LinkInitFrame.c \
                          ANS_AdminWorker.c ANS_BoardWorker.c ANS_MemChunk.c ANS_CmdFrame.c \
                          ANS_Header.c ANS_Frame.c ANS_Startup.c \
                          ANS_Connection.c ANS_ServerPeer.c ANS_Board.c ANS_ClientPeer.c ANS_Protocol.c
                          
noinst_HEADERS = $(top_srcdir)/include/Ai_Log.h \
                 $(top_srcdir)/include/Ai_Logdef.h \
                 $(top_srcdir)/include/Ai_NotUsed.h \
                 $(top_srcdir)/include/Ai_Opterror.h \
                 $(top_srcdir)/include/Ai_Opthelp.h \
                 $(top_srcdir)/include/Ai_Options.h \
                 $(top_srcdir)/include/Ai_Socket.h \
                 $(top_srcdir)/include/ANS_AdminCommands.h \
                 $(top_srcdir)/include/ANS_BoardCommands.h \
                 $(top_srcdir)/include/ANS_Client.h \
                 $(top_srcdir)/include/ANS_CmdFrame.h \
                 $(top_srcdir)/include/ANS_Header.h \
                 $(top_srcdir)/include/ANS_LinkInitFrame.h \
                 $(top_srcdir)/include/ANS_Log.h \
                 $(top_srcdir)/include/ANS_MemChunk.h \
                 $(top_srcdir)/include/ANS_Server.h \
                 $(top_srcdir)/include/ANS_Thread.h \
                 $(top_srcdir)/include/ANS_Types.h \
                 $(top_srcdir)/include/ANS_Startup.h \
                 $(top_srcdir)/src/Ai_Logclt.h \
                 $(top_srcdir)/src/Ai_Logsrv.h \
                 $(top_srcdir)/src/Ai_Logindex.h \
                 $(top_srcdir)/src/Ai_Sockll.h \
                 $(top_srcdir)/src/Ai_Socksv.h \
                 $(top_srcdir)/src/Ai_Socktl.h \
                 $(top_srcdir)/src/ANS_AdminWorker.h \
                 $(top_srcdir)/src/ANS_BoardWorker.h \
                 $(top_srcdir)/src/ANS_Config.h \
                 $(top_srcdir)/src/ANS_Frame.h \
                 $(top_srcdir)/src/ANS_Multiplexer.h 

libans_common_a_CPPFLAGS = -D_AIM_LINUX -fPIC
libans_common_a_CFLAGS = -Wall -I$(top_srcdir)/src -I$(top_srcdir)/include -I@AIM_BSP_COMMON@/include

