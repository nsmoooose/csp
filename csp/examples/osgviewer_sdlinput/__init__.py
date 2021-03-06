# Wrapper for csp/examples/osgviewer_sdlinput/.bin/osgviewer_sdlinput_module.py, which is generated by
# SWIG during the build.

import os
import sys

# this path hack allows this extension module to be loaded
# transparently from the .bin directory.  extending rather than
# replacing __path__ is necessary for py2exe imports to work.
bin = os.path.join(os.path.dirname(__file__), '.bin')
__path__.append(bin)

try:
	from osgviewer_sdlinput_module import *
except ImportError, e:
	sys.stderr.write(str(e))
	sys.stderr.write(
"""
Unable to import osgviewer_sdlinput_module.py from csp/examples/osgviewer_sdlinput/.bin.
This file and others needed by osgviewer_sdlinput are generated during the build.
Check that the target osgviewer_sdlinput has been built successfully.  See the
README file for build instructions.
""")
	sys.exit(1)

