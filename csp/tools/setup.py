# Combat Simulator Project - Workspace Setup Utilities
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
Installs a stub module csp.py into the Python site-packages directory,
allowing csp and its submodules to be imported from the current client
workspace without modifying the default Python module path.

Also prepares the build environment for the current platform.  (Not
fully implemented yet.)

This module is used by the scons build environment.  See the top-level
README for details.
"""

import sys
import os
import os.path
import commands
import logging
from distutils import sysconfig
from distutils import file_util
from distutils import util


class BootstrapInstaller:
	BOOTSTRAP = os.path.join(os.path.dirname(__file__), 'csp.bootstrap')

	class MustForceOverwrite(Exception): pass

	def __init__(self, log=None):
		self._log = log
		self._checkModuleSource()
		self._testBootstrapModule()

	def _info(self, msg):
		if self._log: self._log.info(msg)

	def _warning(self, msg):
		if self._log: self._log.warning(msg)

	def _error(self, msg):
		if self._log: self._log.error(msg)

	def _checkModuleSource(self):
		"""Check the the bootstrap loader source file exists and extract the version number."""
		self._source_version = 0
		if os.path.exists(BootstrapInstaller.BOOTSTRAP):
			self._info('Found source module %s' % BootstrapInstaller.BOOTSTRAP)
			for line in open(BootstrapInstaller.BOOTSTRAP):
				if line.startswith('BOOTSTRAP_VERSION ='):
					self._source_version = int(line.split('=')[1].strip())
					break
			if not self._source_version:
				self._warning('BOOTSTRAP_VERSION not found in source module')
		else:
			self._warning('Bootstrap loader module %s not found' % BootstrapInstaller.BOOTSTRAP)

	def _testBootstrapModule(self):
		"""Test if the bootstrap module is installed and extract the version number."""
		self._installed_version = 0
		try:
			import csp
			self._installed_version = getattr(csp, 'BOOTSTRAP_VERSION', 1)
		except ImportError:
			pass

	def install(self, force):
		"""Install tools/csp.bootstrap as csp.py in Python site-packages."""

		if self._installed_version:
			self._info('Found installed bootstrap loader version %d' % self._installed_version)

		if not self._source_version:
			self._error('Unable to install bootstrap loader %s' % BootstrapInstaller.BOOTSTRAP)
			return 0

		site_packages = sysconfig.get_python_lib()
		target = os.path.join(site_packages, 'csp.py')
		if os.path.exists(target) and (self._installed_version >= self._source_version and not force):
			self._error('Unable to install bootstrap loader; %s already exists.' % target)
			return 0

		source = BootstrapInstaller.BOOTSTRAP
		self._info('Installing bootstrap loader version %d to %s' % (self._source_version, target))
		try:
			file_util.copy_file(source, target)
		except file_util.DistutilsFileError, e:
			self._error('Error installing bootstrap loader: %s' % e)
			self._error('Note that you may need root/admin privileges to install the loader.')
			return 0

		self._info('Byte compiling bootstrap loader')
		util.byte_compile(target)
		return 1


def SetupClientWorkspace(force=0, log=None):
	installer = BootstrapInstaller(log)
	return installer.install(force)

