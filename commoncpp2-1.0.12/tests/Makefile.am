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

EXTRA_DIST = README dotests.sh test.sh output.txt vc6.mak
CXXFLAGS = @CXXFLAGS@ $(THREAD_FLAGS) @WARN_FLAGS@
INCLUDES = -I$(top_srcdir)/include
LDADD = ../src/libccgnu2.la $(THREAD_LIBS) $(DYN_LOADER)
Z_LIBS = -lz

noinst_PROGRAMS = bug1 bug2 thread1 thread2 digest tcpstr1 url1 unixio

bug1_SOURCES = bug1.cpp

bug2_SOURCES = bug2.cpp

unixio_SOURCES = unixio.cpp

digest_SOURCES = digest.cpp
digest_LDADD = ../src/libccext2.la $(XML_LIBS) $(Z_LIBS) $(LDADD)


thread1_SOURCES = thread1.cpp

thread2_SOURCES = thread2.cpp

tcpstr1_SOURCES = tcpstr1.cpp

url1_SOURCES = url1.cpp
url1_LDADD = ../src/libccext2.la $(XML_LIBS) $(Z_LIBS) $(LDADD)

