# Copyright (C) 1999-2002 Open Source Telecom Corporation.
#
# This file is free software; as a special exception the author gives
# unlimited permission to copy and/or distribute it, with or without
# modifications, as long as this notice is preserved.
#
# This program is distributed in the hope that it will be useful, but
# WITHOUT ANY WARRANTY, to the extent permitted by law; without even the
# implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

AUTOMAKE_OPTIONS = no-dependencies dist-shar dist-zip
ACLOCAL_AMFLAGS = -I m4
EXTRA_DIST = reconfig TODO commoncpp2.spec commoncpp2.spec.in \
	commoncpp2.proto.in pkginfo.in commoncpp2.lsm.in \
	COPYING.addendum INSTALL.win32 freebsd/Makefile.in freebsd/pkg* \
	Makefile.bcc acconfig.h commoncpp2.kdevprj

#	config/config.guess config/config.sub config/install-sh \
#	config/ltconfig config/ltmain.sh config/missing config/mkinstalldirs

PKGBASE = "/opt/GNU"
PKGNAME = "commoncpp2"

MAINTAINERCLEANFILES = configure aclocal.m4 libtool Makefile.in Makefile \
	config/config.guess config/config.sub config/install-sh \
	config/ltconfig config/ltmain.sh config/missing config/mkinstalldirs \
	pkginfo CommonC++.spec INSTALL

DIST_SUBDIRS = src win32 m4 doc demo template tests include config
if WIN32
SUBDIRS = include win32 template doc
else
if MSWIN32
SUBDIRS = include win32 template doc
else
SUBDIRS = include src template doc
endif
endif

macosx:
	(cd src ; make macosx)

darwin:
	(cd src ; make macosx)

kdoc:
	(cd include/cc++ ; make kdoc)
	find doc -maxdepth 1 -type f -a \( -name \*.html -o -name \*.kdoc \) | tar zcvf commoncpp2-kdoc-manual-@VERSION@.tar.gz -T -
	@rm -f doc/man3/todo.3cc

doxy:
	(cd doc ; doxygen Doxyfile)
	(cd doc ; doxygen Doxyman)
	(cd doc ; find html -type f -a ! -name \*.hh\* | tar zcvf ../commoncpp2-html-manual-@VERSION@.tar.gz -T -)
	(cd doc ; find latex -maxdepth 1 -type f | tar zcvf ../commoncpp2-latex-manual-@VERSION@.tar.gz -T -)

alldoc:	kdoc doxy

man:
	@rm -f doc/man3/todo.3cc
	(cd doc/man3 ; $(INSTALL_DATA) *.3cc $(DESTDIR)$(mandir)/man3 )

pkg:	
	$(MAKE) $(AM_MAKEFLAGS) prefix=$(PKGBASE) install
	pkgmk -b  $(PKGBASE) -f $(PACKAGE).proto
	pkgtrans -s /var/spool/pkg $(PKGNAME)-$(host_alias).pkg $(PKGNAME)
	mv /var/spool/pkg/$(PKGNAME)-*.pkg . 
	rm -rf $(PKGBASE)
	rm -rf /var/spool/pkg/$(PKGNAME)

lsm:	$(distdir).tar.gz
	sed -e s/SIZE/$(shell expr `wc -c *.tar.gz | cut -d\  -f2` / 1024)/ -e s/VERSION/$(VERSION)/ -e s/DATE/$(shell date +%Y%m%d)/ <$(top_srcdir)/commoncpp2.lsm.in >commoncpp2.lsm

ports:	#(distdir).tar.gz
	if test -f $(distdir).tar.gz ; then cp $(distdir).tar.gz /usr/ports/distfiles ; fi
	mkdir -p freebsd/files
	(cd freebsd ; rm -f *.tgz ; rm -f *.ports)
	(cd freebsd ; make makesum )
	(cd freebsd ; make clean )
	(cd freebsd ; shar Makefile distinfo pkg-descr pkg-plist pkg-comment > $(distdir).ports )
	(cd freebsd ; make package )

install-data-local:
	@if test -f /etc/ld.so.conf \
	  -a `echo $(libdir) | grep ^/usr`XXX != "XXX" ; then \
		if ! grep -q "$(libdir)" /etc/ld.so.conf; then \
			echo "------------------------------------------------------------------------------" ;\
			echo "Hmm, looks like you forgot to add $(libdir) to /etc/ld.so.conf." ;\
			echo "I'll try to fix it for you (but pay attention to libtool's output" ;\
			echo "next time, ok?)" ;\
			echo "$(libdir)" >> /etc/ld.so.conf && \
				echo "Ok, all fixed." \
				|| \
				(echo "" ;\
				echo "Well, that didn't work.  Remember to add $(libdir) to" ;\
				echo "/etc/ld.so.conf if you want to use this library.") ;\
			echo "------------------------------------------------------------------------------" ;\
			ldconfig ;\
		fi ;\
	fi


