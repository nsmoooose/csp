# Copyright (C) 1999-2001 Open Source Telecom Corporation.
#
# This file is free software; as a special exception the author gives
# unlimited permission to copy and/or distribute it, with or without
# modifications, as long as this notice is preserved.
#
# This program is distributed in the hope that it will be useful, but
# WITHOUT ANY WARRANTY, to the extent permitted by law; without even the
# implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

MAINTAINERCLEANFILES = Makefile.in Makefile

EXTRA_DIST = README apesock.dsp apesock.dsw Makefile.bcc test.xml
CXXFLAGS = @CXXFLAGS@ $(THREAD_FLAGS) @WARN_FLAGS@
INCLUDES = -I$(top_srcdir)/include
LDADD = ../src/libccgnu2.la $(THREAD_LIBS) $(DYN_LOADER)
Z_LIBS = -lz

noinst_PROGRAMS = tcp tcpthread tcpservice serial cmdlineopt \
	urlfetch xmlfetch portsample netdevices slogTest crc32 shadigest
noinst_HEADERS = serialecho.h SampleSocketPort.h

urlfetch_SOURCES = urlfetch.cpp
urlfetch_LDADD = ../src/libccext2.la $(XML_LIBS) $(Z_LIBS) $(LDADD)

xmlfetch_SOURCES = xmlfetch.cpp
xmlfetch_LDADD = ../src/libccext2.la $(XML_LIBS) $(Z_LIBS) $(LDADD)

slogTest_SOURCES = slogTest.cpp

tcp_SOURCES = tcp.cpp

tcpthread_SOURCES = tcpthread.cpp

tcpservice_SOURCES = tcpservice.cpp

portsample_SOURCES = SampleSocketPort.cpp

netdevices_SOURCES = netdevices.cpp

serial_SOURCES = serialmain.cpp serialecho.cpp
serial_LDADD = ../src/libccext2.la $(XML_LIBS) $(Z_LIBS) $(LDADD)

cmdlineopt_SOURCES = cmdlineopt.cpp
cmdlineopt_LDADD = ../src/libccext2.la $(XML_LIBS) $(Z_LIBS) $(LDADD)

crc32_SOURCES = crc32.cpp
crc32_LDADD = ../src/libccext2.la $(XML_LIBS) $(Z_LIBS) $(LDADD)

shadigest_SOURCES = shadigest.cpp
shadigest_LDADD = ../src/libccext2.la $(XML_LIBS) $(Z_LIBS) $(LDADD)
