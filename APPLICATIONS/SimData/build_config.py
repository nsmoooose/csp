from build_support import Config


class Config_posix(Config):
	def config(self, env):
		self.CXXFLAGS = '-g -Wall -O2'
		self.CPPFLAGS = ''
		self.CPPPATH = ['#/Include', self.PYTHON_INC]
		self.SHLINKFLAGS = ' -shared -Wl,-z,lazyload'
		self.SHLINKLIBS = ['dl', 'swigpy']
		self.SWIGFLAGS = ' -c -c++ -python -noexcept -IInclude -I%s' % self.PYTHON_INC
		self.CXXFILESUFFIX = '.cpp'
		self.ARCHIVE_FORMATS = None

class Config_msvc(Config):
	def config(self, env):
		wine = '' #wine --debugmsg -all -- '
		self.CXX = wine + 'cl'
		self.LINK = wine + 'link'
		self.CXXFLAGS = '/nologo /DWIN32=1 /D_WIN32=1 /D_STLPORT=1 /DSIMDATA_EXPORTS=1 /GR /GX /W3'
		self.CPPFLAGS = ''
		self.CPPPATH = ['#/Include', self.PYTHON_INC]
		self.SHLINKFLAGS = ' /MACHINE:I386 /LIBPATH:%s /NODEFAULTLIB:LIBC' % self.PYTHON_LIB
		self.SHLINKLIBS = ['dl', 'swigpy']
		self.SWIGFLAGS = ' -c -c++ -python -noexcept -IInclude -I%s' % self.PYTHON_INC
		self.CXXFILESUFFIX = '.cpp'
		self.ARCHIVE_FORMATS = ['.zip']
		

