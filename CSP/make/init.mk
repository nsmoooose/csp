
CLEAN=
DEPFILTER=

SWIG=swig

PYTHONINC=$(shell python -c 'from distutils import sysconfig; print sysconfig.get_python_inc()')
PYTHONLIB=$(shell python -c 'from distutils import sysconfig; print sysconfig.get_python_lib()')

