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
EXTRA_DIST=ccgnu2-config.in ccgnu2-config ost_check2.m4 Makefile.bcc \
	getopt.h getopt.c getopt1.c
LT_VERSION=@LT_CCXX_VERSION@
CXXFLAGS = -I../src -DCCXX_EXPORT_LIBRARY @CXXFLAGS@ $(THREAD_FLAGS) \
	@XML_FLAGS@ -I$(CCXX_DIR)
CFLAGS = @CFLAGS@ $(THREAD_FLAGS)
#INCLUDES = -I$(CCXX_DIR) @XML_FLAGS@
LDFLAGS = @LDFLAGS@ $(LT_VERSION) -release $(LT_RELEASE)

aclocaldir = $(datadir)/aclocal
aclocal_DATA = ost_check2.m4

bin_SCRIPTS = ccgnu2-config

lib_LTLIBRARIES = libccgnu2.la libccext2.la

libccgnu2_la_LIBADD = @THREAD_LIBS@

if GETOPT_LONG
optincludedir=$(includedir)/cc++2
optinclude_HEADERS = getopt.h

libccgnu2_la_SOURCES = thread.cpp mutex.cpp semaphore.cpp threadkey.cpp \
        friends.cpp event.cpp slog.cpp dir.cpp file.cpp inaddr.cpp \
        peer.cpp port.cpp socket.cpp network.cpp serial.cpp mempager.cpp \
        keydata.cpp dso.cpp exception.cpp process.cpp urlstring.cpp \
        private.h getopt.c getopt1.c
else
libccgnu2_la_SOURCES = thread.cpp mutex.cpp semaphore.cpp threadkey.cpp \
	friends.cpp event.cpp slog.cpp dir.cpp file.cpp inaddr.cpp \
	peer.cpp port.cpp socket.cpp network.cpp serial.cpp mempager.cpp \
	keydata.cpp dso.cpp exception.cpp process.cpp urlstring.cpp \
	private.h
endif

libccext2_la_LIBADD = @BASE_LIB@ @THREAD_LIBS@
libccext2_la_SOURCES = buffer.cpp fifo.cpp pipe.cpp numbers.cpp \
	cmdoptns.cpp url.cpp xml.cpp persist.cpp engine.cpp digest.cpp \
	date.cpp groups.cpp md5.cpp sha.cpp unix.cpp ftp.cpp 

# macosx additional thing

macosx:
	rm -rf .libs/*.dylib
	ld -r -o ccgnu2.lo $(libccgnu2_la_OBJECTS)
	../libtool --mode=link c++ -dynamic -dynamiclib -o libccgnu2.la \
		-rpath $(libdir) ccgnu2.lo $(LDFLAGS) -lpthread
	ld -r -o ccext2.lo $(libccext2_la_OBJECTS)
	../libtool --mode=link c++ -dynamic -dynamiclib -o libccext2.la \
		-rpath $(libdir) ccext2.lo $(LDFLAGS) -lpthread
