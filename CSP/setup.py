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
Combat Simulator Project - Workspace Setup Script

Installs a stub module CSP.py into the Python site-packages directory,
allowing CSP and its submodules to be imported from the current client
workspace without modifying the default Python module path.

Also prepares the build environment for the current platform.  (Not
fully implemented yet.)

Usage: %(prog)s [flags]
"""

import sys
import os
import os.path
import commands
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


def CheckSCons():
	status, output = commands.getstatusoutput('scons --version')
	return status == 0

def TestBootstrapModule():
	try:
		import CSP
		return 1
	except ImportError:
		return 0

def IsUnix():
	return os.name == 'posix'


def main(args):
	print 'Combat Simulator Project - Workspace Setup Script'

	do_install = app.options.force
	nothing_to_do = 1

	if not TestBootstrapModule():
		do_install = 1

	if do_install:
		InstallLoader(app.options.force)
		if not TestBootstrapModule():
			print 'ERROR: bootstrap CSP module installation failed.'
			return 1
		nothing_to_do = 0

	if IsUnix() and not CheckSCons():
		print
		print 'WARNING: SCons software construction tool (scons) not found.'
		print
		print 'SCons is required to build CSP on non-Windows platforms.  See '
		print 'http://scons.sf.net for more information and to download installable'
		print 'packages.  You may also be able to install the latest version of'
		print 'SCons for your distribution using a package tool such as apt-get'
		print '(e.g. "apt-get install scons").'
		return 1

	if IsUnix():
		print 'Setup complete.  Run "scons <target>" to build CSP, or see the README'
		print 'file for more information.'
	else:
		print 'Setup complete.  Open the project files in VisualStudio to build'
		print 'CSP, or see the README file for more information.'

	return 0


app.addOption('-f', '--force', action='store_true', default=False, help='force reinstall')
app.start()
