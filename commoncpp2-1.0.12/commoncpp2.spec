# Copyright (c) 1999-2001 by Open Source Telecom Corporation.
# Verbatim copying and distribution of this entire file is permitted
# in any medium, provided this notice is preserved.

Summary: "commoncpp2" - A GNU package for creating portable C++ programs
Name: commoncpp2
Version: 1.0.12
Release: 9
Group: Development/Libraries
URL: http://www.gnu.org/software/commonc++/commonc++.html
Source: ftp://www.gnu.org/gnu/commonc++/commoncpp2-%{PACKAGE_VERSION}.tar.gz
Prefix: %{_prefix}
Copyright: GPL
BuildRoot: %{_tmppath}/CCXX-root
Packager: David Sugar <dyfet@ostel.com>

%package extras
Requires: commoncpp2
Group: Development/Libraries
Summary: XML support and optional class library for commoncpp2

%package devel
Requires: commoncpp2, commoncpp2-extras
Group: Development/Libraries
Summary: Headers and static link library for commoncpp2

%description
This is the second major release of GNU Common C++.  GNU Common C++ "2" is 
a GNU package which offers portable "abstraction" of system services such as 
threads, networks, and sockets.  GNU Common C++ "2" also offers individual 
frameworks generally useful to developing portable C++ applications 
including a object persistance engine, math libraries, threading, sockets, 
etc.  GNU Common C++ "2" is small, and highly portable.  GNU Common C++ 
"2" will  support most Unix operating systems as well as Win32, in addition 
to GNU/Linux.

%description extras
This package provides the GNU Common C++ extras shared library which adds
support for XML parsing, url file access, and other optional classes.

%description devel
This package provides the link libraries and documentation for building
GNU Common C++ applications.

%prep
rm -rf $RPM_BUILD_ROOT

%setup
CXXFLAGS="$RPM_OPT_FLAGS" ./configure --prefix=%{_prefix}

%build
uname -a|grep SMP && make -j 2 || make

%install
mkdir -p $RPM_BUILD_ROOT/%{_mandir}/man3
make prefix=$RPM_BUILD_ROOT/%{_prefix} \
	mandir=$RPM_BUILD_ROOT/%{_mandir} \
	infodir=$RPM_BUILD_ROOT/%{_infodir} install
make prefix=$RPM_BUILD_ROOT/%{_prefix} \
	mandir=$RPM_BUILD_ROOT/%{_mandir} man
strip $RPM_BUILD_ROOT/%{_prefix}/lib/libcc*.so.*

%files
%defattr(-,root,root,0755)
%{_prefix}/lib/libccgnu*so*

%files extras
%defattr(-,root,root,0755)
%{_prefix}/lib/libccext*so*

%files devel
%defattr(-,root,root,0755)
%doc AUTHORS COPYING COPYING.addendum NEWS README TODO ChangeLog
%{_prefix}/lib/libcc*a
%{_prefix}/include/cc++2
%{_prefix}/bin/ccgnu2-config
%{_mandir}/man3/*.3cc*
%{_infodir}/commoncpp2.info*
%{_datadir}/aclocal/ost_check2.m4

%clean
rm -rf $RPM_BUILD_ROOT

%post
/sbin/ldconfig -n ${exec_prefix}/lib

%post extras
/sbin/ldconfig -n ${exec_prefix}/lib

%post devel
/sbin/install-info %{_infodir}/commoncpp2.info %{_infodir}/dir

%postun
/sbin/ldconfig -n ${exec_prefix}/lib

%postun extras
/sbin/ldconfig -n ${exec_prefix}/lib

%postun devel
/sbin/install-info --delete %{_infodir}/commoncpp2.info %{_infodir}/dir
