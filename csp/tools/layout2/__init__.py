import os
# this path hack allows the layout extension module to be loaded
# transparently from the .bin directory.  extending rather than
# replacing __path__ is necessary for py2exe imports to work.
bin = os.path.join(os.path.dirname(__file__), '.bin')
__path__.append(bin)
