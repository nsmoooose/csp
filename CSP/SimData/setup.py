# SimDataCSP: Data Infrastructure for Simulations
# Copyright (C) 2002 Mark Rose <tm2@stm.lbl.gov>
# 
# This file is part of SimDataCSP.
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


###
# This script has many warts, but should be functional.  It will
# be cleaned up in due course....
#
# -MR


import sys

min_python_version = "2.2.0"


if len(sys.argv) == 2:
    command = sys.argv[1]
    if command == "check_version":
        old = sys.version[:5] < min_python_version
        if old:
            print >>sys.stderr, "SimData requires Python version %s or higher. (current version is %s)" % \
                        (min_python_version, sys.version[:5])
        sys.exit(old)
        
from distutils.core import setup
from distutils.core import Extension
import distutils.command.build_ext
from distutils.command.build_ext import build_ext
from distutils import sysconfig, dir_util
import os, os.path, string, re

# REMEMBER TO 'touch Version.cpp' OR REBUILD ALL
VERSION = "0.3.4"


SWIG_VERSION = None

def getSwigVersion():
  global SWIG_VERSION
  if SWIG_VERSION is None:
    (cout, cin, cerr) = os.popen3('swig -version')
    versiontext = ' '.join(cerr.readlines())
    match = re.search(r'\s(1)\.(\d)\.(\d+)\s', versiontext)
    if match is None:
      print >>sys.stderr, 'swig -version failed.  Check that SWIG is installed correctly.'
      sys.exit(1)
    SWIG_VERSION = map(int, match.groups())
  return SWIG_VERSION

def checkSwigVersion():
    major, minor, rev = getSwigVersion()
    if major < 1 or minor < 3 or rev < 16:
        print >>sys.stderr, 'SWIG version 1.3.16 or greater required; older versions are not supported.'
        sys.exit(1)
    if rev == 18 or rev == 19:
        print >>sys.stderr, 'SWIG versions 1.3.18 and 1.3.19 are not supported, please upgrade to a newer version.'
        sys.exit(1)

def isOldSwigRuntime():
    major, minor, rev = getSwigVersion()
    return major == 1 and minor == 3 and rev < 20

def copy_dir(src, dst, files, verbose=0):
    from distutils.file_util import copy_file
    from distutils.dir_util import mkpath
    from distutils.errors import DistutilsFileError, DistutilsInternalError
    from stat import ST_ATIME, ST_MTIME, ST_MODE, S_IMODE
    if not os.path.isdir(src):
        raise DistutilsFileError, \
            "cannot copy dir '%s': not a directory" % src
    mkpath(dst)
    st = os.stat(src)
    os.chmod(dst, S_IMODE(st[ST_MODE]))
    warnings = 0
    for n in files:
        src_name = os.path.join(src, n)
        dst_name = os.path.join(dst, n)
        if not os.path.isdir(src_name):
            if verbose:
                print "%s => %s" % (src_name, dst_name)
            try:
                copy_file(src_name, dst_name)
            except DistutilsFileError, e:
                print "WARNING:", e
                warnings = warnings + 1
    return warnings

def make_install(win, args):
    from distutils.errors import DistutilsFileError
    lib = sysconfig.get_python_lib()
    inc = sysconfig.get_python_inc()
    modpath = os.path.join(lib, "SimData")
    incpath = os.path.join(inc, "SimData")
    localinc = os.path.normpath("Include/SimData")
    libpath = modpath
    prefix = ""
    verbose = 0
    warnings = 0
    for arg in args:
        if arg.startswith("--prefix="):
            prefix = arg[9:]
            modpath = os.path.normpath(prefix+modpath)
            incpath = os.path.normpath(prefix+incpath)
            libpath = os.path.normpath(prefix+libpath)
        elif arg=='-v' or arg=='--verbose':
            verbose = 1
    package_files = ['__init__.py', 'Debug.py', 'Parse.py', 'Compile.py']
    if win:
        package_files.extend(['cSimData.py', '_cSimData.dll', '_cSimData.lib','_cSimDatad.dll', '_cSimDatad.lib'])
        src = os.path.join("VisualStudio","cSimData.py")
        if os.path.exists(src):
            from distutils.file_util import copy_file
            try:
                copy_file(src, os.path.join("SimData","cSimData.py"))
            except DistutilsFileError, e:
                print "WARNING:", e
                warnings = warnings + 1
    else:
        package_files.extend(['cSimData.py', '_cSimData.so'])
    try:
        print "Installing SimData package to", modpath
        warnings = warnings + copy_dir("SimData", modpath, package_files, verbose)
        print "Installing SimData headers to", incpath
        warnings = warnings + copy_dir(localinc, incpath, headers, verbose)
        warnings = warnings + copy_dir(localinc, incpath, interfaces, verbose)
        if not win:
            print "Installing SimData libraries to", libpath
            warnings = warnings + copy_dir("SimData", libpath, ['_cSimData.so', 'libSimData.a'], verbose)
        print "Byte compiling the Python modules..."
        import py_compile
        for file in package_files:
            if file.endswith(".py"):
                script = os.path.join(modpath, file)
                if os.path.exists(script):
                    py_compile.compile(script)  
                    os.chmod(script+"c", 0644)
    except Exception, e:
        print e
        sys.exit(1)
    if warnings > 0:
        print "WARNING: Some errors were encountered during installation."
        if win:
            print
            print "         If you have only built the release version of SimData, you"
            print "         can safely ignore warnings related to _cSimDatad.dll  and"
            print "         _cSimDatad.lib."
    sys.exit(0)

class build_swig_ext(build_ext):
    
    options = []

    def build_extension(self, ext):
        self.ext = ext
        build_ext.build_extension(self, ext)
        
    def swig_sources (self, sources):

        """Walk the list of source files in 'sources', looking for SWIG
        interface (.i) files.  Run SWIG on all that are found, and
        return a modified 'sources' list with SWIG source files replaced
        by the generated C (or C++) files.
        """

        new_sources = []
        swig_sources = []
        swig_targets = {}

        # XXX this drops generated C/C++ files into the source tree, which
        # is fine for developers who want to distribute the generated
        # source -- but there should be an option to put SWIG output in
        # the temp dir.

        self.swig_cpp = 1

        if self.swig_cpp:
            target_ext = '.cpp'
        else:
            target_ext = '.c'

        for source in sources:
            (base, ext) = os.path.splitext(source)
            if ext == ".i":             # SWIG interface file
                new_sources.append(base + target_ext)
                swig_sources.append(source)
                swig_targets[source] = new_sources[-1]
            else:
                new_sources.append(source)

        if not swig_sources:
            return new_sources

        swig = self.find_swig()
        swig_cmd = [swig] + build_swig_ext.options
        
        if 1 or self.inplace:
            fullname = self.get_ext_fullname(self.ext.name)
            modpath = string.split(fullname, '.')
            package = string.join(modpath[0:-1], '.')
            base = modpath[-1]
            build_py = self.get_finalized_command('build_py')
            package_dir = build_py.get_package_dir(package)
        else:
            package_dir = self.build_lib

        for source in swig_sources:
            target = swig_targets[source]
            self.announce("swigging %s to %s" % (source, target))
            self.spawn(swig_cmd + ["-o", target, source])
            (base, ext) = os.path.splitext(source)
            proxy_full = base + '.py'
            proxy = os.path.basename(proxy_full)
            proxy_lib = os.path.join(self.build_lib, os.path.join(package_dir, proxy))
            print proxy_full, "=>", proxy_lib
            os.rename(proxy_full, proxy_lib)

        return new_sources

orig_build_ext = build_ext
distutils.command.build_ext.build_ext = build_swig_ext

if os.name == "posix":
    # changes the compiler from gcc to g++
    from distutils import sysconfig
    save_init_posix = sysconfig._init_posix
    def gpp_init_posix():
        print 'gpp_init_posix: changing gcc to g++'
        save_init_posix()
        g = sysconfig._config_vars
        g['CC'] = 'g++'
        g['LDSHARED'] = 'g++ -shared'
    sysconfig._init_posix = gpp_init_posix

sources = [
    "BaseType",
    "DataArchive",
    "DataManager",
    "Date",
    "Enum",
    "BaseException",
    "External",
    "FileUtility",
    "GeoPos",
    "HashUtility",
    "InterfaceRegistry",
    "Interpolate",
    "Key",
    "Link",
    "List",
    "LogStream",
    "LUT",
    "Math",
    "Matrix3",
    "Object",
    "Noise",
    "Path",
    "Quat",
    "Random",
    "Real",
    "TypeAdapter",
    "Vector3",
    "Version",
]

headers = [
    "Archive.h",
    "BaseType.h",
    "Composite.h",
    "Conversions.h",
    "DataArchive.h",
    "DataManager.h",
    "Date.h",
    "Enum.h",
    "ExceptionBase.h",
    "Export.h",
    "External.h",
    "FileUtility.h",
    "GeoPos.h",
    "hash_map.h",
    "HashUtility.h",
    "Integer.h",
    "InterfaceRegistry.h",
    "Interpolate.h",
    "Key.h",
    "Link.h",
    "List.h",
    "LUT.h",
    "Log.h",
    "LogStream.h",
    "Math.h",
    "Matrix3.h",
    "Noise.h",
    "Namespace.h",
    "Object.h",
    "ObjectInterface.h",
    "osg.h",
    "Path.h",
    "PTS.h",
    "Properties.h",
    "Quat.h",
    "Random.h",
    "Real.h",
    "Ref.h",
    "Singleton.h",
    "String.h",
    "Trace.h",
    "TypeAdapter.h",
    "Types.h",
    "Uniform.h",
    "Vector3.h",
    "Version.h",
]

interfaces = [
    "cSimData.i",
    "Archive.i",
    "BaseType.i",
    "Conversions.i",
    "DataArchive.i",
    "DataManager.i",
    "Date.i",
    "Enum.i",
    "ExceptionBase.i",
    "External.i",
    "filemap.i",
    "GeoPos.i",
    "HashUtility.i",
    "InterfaceRegistry.i",
    "Interpolate.i",
    "Key.i",
    "Link.i",
    "List.i",
    "Log.i",
    "LUT.i",
    "Matrix3.i",
    "Noise.i",
    "Object.i",
    "Path.i",
    "Quat.i",
    "Random.i",
    "Real.i",
    "String.i",
    "Types.i",
    "vector.i",
    "Vector3.i",
]


def fullpath(path, ext, list):
    return map(lambda x: path+x+ext, list)

main_interface = ["cSimData.i"]

sources = fullpath("Source/", ".cpp", sources)
main_interface_fullpath = fullpath("Source/", "", main_interface)
interfaces_fullpath = fullpath("Include/SimData/", "", interfaces)
headers_fullpath = fullpath("Include/SimData/", "", headers)

includes = ["Include"]
defines = [("SIMDATA_VERSION", '"%s"' % VERSION)]
libraries = ["dl"]
cflags = []

checkSwigVersion()
swigopts = ["-python", "-c++", "-noexcept"]
if isOldSwigRuntime():
    libraries.append('swigpy')
else:
    swigopts.append("-runtime")
build_swig_ext.options = swigopts + ["-IInclude"]

if len(sys.argv)>=2:
    command = sys.argv[1]
    if command.startswith("make_install"):
        make_install(command.endswith("win"), sys.argv[2:])
    if command == "python_include_path":
        print sysconfig.get_python_inc()
        sys.exit(0)
    if command == "ldopts":
        print '-shared', ' '.join(map(lambda x: '-l%s' % x, libraries))
        sys.exit(0)
    if command == "swigopts":
        print ' '.join(swigopts)
        sys.exit(0)

cSimData = Extension("SimData._cSimData", 
                     sources + main_interface_fullpath, 
                     include_dirs = includes,
                     define_macros = defines,
                     libraries = libraries, 
                     extra_compile_args = cflags,
                     )

setup(name="SimData",
      version=VERSION,
      description="Simulation Data Abstraction Library",
      author="Mark Rose",
      author_email="mrose@stm.lbl.gov",
      url="http://csp.sourceforge.net/wiki/SimData",
      license="GNU General Public License, version 2 or later.",
      packages=['SimData'],
      headers = headers_fullpath + interfaces_fullpath + main_interface_fullpath,
      ext_modules = [cSimData],
      )
