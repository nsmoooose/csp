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

class Config_win32(Config):
    def config(self, env):
        import sys
        from os.path import join
        self.PYTHON_LIBS = join(sys.prefix, 'libs')
        self.CPPFLAGS = '/EHsc /DWIN32=1 /DSIMDATA_EXPORTS=1 /GR /GX /W3 /MD'
        self.CPPPATH = ['#/Include', self.PYTHON_INC]
        self.SHLINKFLAGS = '/MACHINE:I386 /LIBPATH:%s /DLL' % self.PYTHON_LIBS
        self.CXXFILESUFFIX = '.cpp'
        self.ARCHIVE_FORMATS = ['.zip']
        
        
