from build_support import Config, compareVersions


class Config_posix(Config):
	def config(self, env):
		self.CXXFLAGS = '-g -Wall -O2'
		self.CPPFLAGS = ''
		self.CPPPATH = ['#/Include', self.PYTHON_INC]
		self.SHLINKFLAGS = ' -shared -Wl,-z,lazyload'
		self.SHLINKLIBS = ['dl']
		self.CXXFILESUFFIX = '.cpp'
		self.ARCHIVE_FORMATS = None
		self.configSwig(env)
	def configSwig(self, env):
		version = env['SWIG_VERSION']
		self.SWIGFLAGS = ' -c++ -python -noexcept -IInclude -I%s' % self.PYTHON_INC
		if compareVersions(version, '1.3.20') >= 0:
			self.SWIGFLAGS = self.SWIGFLAGS + ' -runtime'
		else:
			self.SWIGFLAGS = self.SWIGFLAGS + ' -c'
			self.SHLINKLIBS.append('swigpy')


class Config_msvc(Config):
	def config(self, env):
		wine = '' #wine --debugmsg -all -- '
		self.CXX = wine + 'cl'
		self.LINK = wine + 'link'
		self.CXXFLAGS = '/nologo /DWIN32=1 /D_WIN32=1 /D_STLPORT=1 /DSIMDATA_EXPORTS=1 /GR /GX /W3'
		self.CPPFLAGS = ''
		self.CPPPATH = ['#/Include', self.PYTHON_INC]
		self.SHLINKFLAGS = ' /MACHINE:I386 /LIBPATH:%s /NODEFAULTLIB:LIBC' % self.PYTHON_LIB
		self.SHLINKLIBS = []
		self.CXXFILESUFFIX = '.cpp'
		self.ARCHIVE_FORMATS = ['.zip']
		self.configSwig(env)
	def configSwig(self, env):
		version = env['SWIG_VERSION']
		self.SWIGFLAGS = ' -c++ -python -noexcept -IInclude -I%s' % self.PYTHON_INC
		if compareVersions(version, '1.3.20') >= 0:
			self.SWIGFLAGS = self.SWIGFLAGS + ' -runtime'
		else:
			self.SWIGFLAGS = self.SWIGFLAGS + ' -c'
			self.SHLINKLIBS.append('swigpy')
		

