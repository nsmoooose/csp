## Process this file with automake to produce Makefile.in

AUTOMAKE_OPTIONS               = foreign

AUX_DIST               = config.guess \
                       config.sub \
                       install-sh \
                       ltconfig \
                       ltmain.sh \
                       mdate-sh \
                       missing \
                       mkinstalldirs \
                       bootstrap \
                       Include \
                       Demos

SUBDIRS                        = Source

MAINTAINERCLEANFILES   = Makefile.in aclocal.m4 configure config-h.in \
                         stamp-p.in $(AUX_DIST)

ACLOCAL                = aclocal 

dist-hook:
	for file in $(AUX_DIST) $(AUX_DIST_EXTRA); do \
	  cp -r $$file $(distdir)/$$file; \
	done
	cp -rp Demos $(distdir)
	rm -rf `find $(distdir) -type d -name CVS -print`



##libtool: @LIBTOOL_DEPS@
##	cd $(srcdir) && \
##           $(SHELL) ./config.status --recheck
