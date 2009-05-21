# Copyright 2004-2006 Mark Rose <mkrose@users.sourceforge.net>
#
# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 2 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program; if not, write to the Free Software
# Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

"""
CSP uses SCons (http://scons.sf.net) for build management, but customizes
and extends the standard SConscript interface.  One of the main goals is
to make it very simple to specify build structures that are commonly used
in CSP.

This package is imported by the top-level SConstruct file, and provides
the following features:

  * Simple configuration and dependency generation for external libraries.
    External libraries (or sets of libraries) are identified by name and
    can be depended on by any build rule.  Platform-specific library
    detection and configuration are handled by adding one or more
    configuration instances to the library definition.  Build options
    needed by the library are propagated to all rules that depend on the
    library.  Configuration settings (e.g., include paths) are stored
    between runs, but can be regenerated using the 'config' build target.

  * Build targets are specified using a small number of custom build rules
    (metarules) that in turn generate the actual SCons build rules.  All
    metarules are processed before generating the final build rules,
    allowing metarules to depend on other metarules that have not been
    encountered yet.  Dependencies on other metarules and on external
    libraries automatically set build options (e.g. include paths) as
    needed.

  * Metarules provide builtin support for common tasks like documentation
    generation and build target aliasing.

  * Several enhancements to basic SCons functionality, including better
    SWIG support.

Caveats:

  * Although several of these build features could be generally useful,
    some refactoring would be required to make them fully independent of
    CSP.

  * The full SCons interface, as well as arbitrary python code, can be
    used directly in the build scripts if necessary.  Ideally though,
    such functionality should be rolled into this build package.

  * The build extensions are not well documented yet.  Refer to the CSP
    SConstruct and SConscript to learn the interface by example.
"""

import os
import sys

if __name__ == '__main__':
	print 'This module provides support for the SCons build system; it is not'
	print 'intended to be run as a standalone program.'
	sys.exit(1)

# Before doing any real work, check that the csp bootstrap module is installed.
try:
	import csp
except ImportError:
	print 'The CSP build environment has not been initialized.  Please'
	print 'run "python tools/setup.py" as root or admin.'

from csp.tools.build.setup import Environment

from csp.tools.build.rules import \
	SourceGroup, Program, Generate, SharedLibrary, PythonSharedLibrary, Command, Test

from csp.tools.build.autoconf import \
	CheckSConsVersion, CheckPythonVersion, GetPythonInclude, GetPythonLibrary, GetGCCVersion

from csp.tools.build.libconf import \
	ExternalLibrary, PkgConfig, CommandConfig, UnixLibConfig, WindowsLibConfig, DevpackConfig


