#!/usr/bin/env python
#
# Combat Simulator Project Workspace Setup Script
# Copyright (C) 2004 The Combat Simulator Project
# http://csp.sourceforge.net
#
# This program is free software; you can redistribute it and/or
# modify it under the terms of the GNU General Public License
# as published by the Free Software Foundation; either version 2
# of the License, or (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program; if not, write to the Free Software
# Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.


"""
CSP Workspace Setup Script

Installs a stub module CSP.py into the Python site-packages directory,
allowing CSP and its submodules to be imported from the current client
workspace without modifying the default Python module path.

Also prepares the build environment for the current platform.  (Not
fully implemented yet.)

Usage: %(prog)s [flags]
"""

import sys
import os.path
from distutils import sysconfig
from distutils import file_util
from distutils import util
from distutils import log
from base import app  # (CSP.base)

log.set_verbosity(log.INFO)


def InstallLoader(force):
	"""Install tools/CSP_bootstrap as CSP.py in Python site-packages."""
	site_packages = sysconfig.get_python_lib()
	target = os.path.join(site_packages, 'CSP.py')
	if os.path.exists(target) and not force:
		print 'Error: unable to import CSP, but CSP.py exists in site-packages.'
		print 'Use --force to overwrite.'
		sys.exit(1)
	source = os.path.join('tools', 'CSP_bootstrap')
	log.info('installing bootstrap loader')
	try:
		file_util.copy_file(source, target)
	except file_util.DistutilsFileError, e:
		print e
		sys.exit(1)
	log.info('byte compiling bootstrap loader')
	util.byte_compile(target)


def main(args):
	do_install = app.options.force
	try:
		import CSP
	except ImportError:
		do_install = 1
	if do_install:
		InstallLoader(app.options.force)
	else:
		print 'Nothing to do; try %s --help for more options.' % app.programName()


app.addOption('-f', '--force', action='store_true', default=False, help='force reinstall')
app.start()
