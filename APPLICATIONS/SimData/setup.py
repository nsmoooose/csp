from distutils.core import setup
from distutils.core import Extension
import distutils.command.build_ext
from distutils.command.build_ext import build_ext
import os, os.path, string

# REMEMBER TO 'touch Version.cpp' OR REBUILD ALL
VERSION = "\"0.3.1\""


class build_swig_ext(build_ext):
	
	swig_options = ''

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
		swig_cmd = [swig, "-python", "-c++"] + build_swig_ext.options.split()
		
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
	"Date",
	"Enum",
	"Exception",
	"External",
	"GeoPos",
	"HashUtility",
	"InterfaceRegistry",
	"Interpolate",
	"List",
	"LogStream",
	"Math",
	"Matrix3",
	"Object",
	"Pack",
	"Path",
	"Quaternion",
	"Random",
	"Spread",
	"TypeAdapter",
	"Vector3",
	"Version",
]

headers = [
	"BaseType.h",
	"DataArchive.h",
	"Date.h",
	"Enum.h",
	"Exception.h",
	"Export.h",
	"External.h",
	"GeoPos.h",
	"GlibCsp.h",
	"hash_map.h",
	"HashUtility.h",
	"Integer.h",
	"InterfaceRegistry.h",
	"Interpolate.h",
	"List.h",
	"Log.h",
	"LogStream.h",
	"Math.h",
	"Matrix3.h",
	"ns-simdata.h",
	"Object.h",
	"ObjectInterface.h",
	"Pack.h",
	"Path.h",
	"Quaternion.h",
	"Random.h",
	"Real.h",
	"Spread.h",
	"String.h",
	"TypeAdapter.h",
	"Types.h",
	"Vector3.h",
	"Version.h",
]

interfaces = [
	"cSimData"
]

def fullpath(path, ext, list):
	return map(lambda x: path+x+ext, list)

sources = fullpath("Source/", ".cpp", sources)
interfaces = fullpath("Source/", ".i", interfaces)
headers = fullpath("Include/SimData/", "", headers)

build_swig_ext.options = "-DUSE_NAMESPACE_SIMDATA -IInclude -noexcept"
includes = ["Include"]
defines = [("USE_NAMESPACE_SIMDATA", None), ("SIMDATA_VERSION", VERSION)]
libraries = ["swigpy", "dl"]
cflags = []

cSimData = Extension("SimData._cSimData", 
                     sources + interfaces, 
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
      url="http://csp.sourceforge.net/wiki/",
      packages=['SimData'],
	  headers = headers,
	  ext_modules = [cSimData],
	  )


