# -*-python-*-

# SimData: Data Infrastructure for Simulations
# Copyright (C) 2002, 2003 Mark Rose <mkrose@users.sf.net>
#
# This file is part of SimData.
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
##

# Utilities for building and installing with SCons


import SCons.Action
import SCons.Builder
import SCons.Scanner
import SCons.Util
import SCons.Errors


Action = SCons.Action.Action
Builder = SCons.Builder.Builder
Scanner = SCons.Scanner.Base

import os, os.path, re, shutil, glob, sys, types

# configure tests (defined below)
configure_tests = {}



############################################################################
# python module and package installation
############################################################################

def getPythonInc():
    from distutils import sysconfig
    return sysconfig.get_python_inc()

def getPythonLib():
    from distutils import sysconfig
    return sysconfig.get_python_lib()

def installFile(target, source, env):
    """Install a source file into a target using the function specified
    as the INSTALL construction variable."""
    try:
        install = env['INSTALL']
    except KeyError:
        raise SCons.Errors.UserError('Missing INSTALL construction variable.')
    return install(target[0].path, source[0].path, env)

def byteCompile(target, source, env):
    """Byte compile a python source file."""
    target = str(target[0])
    if target.endswith(".py") and os.path.exists(target):
        import py_compile
        try:
            py_compile.compile(target)
            os.chmod(target+"c", 0644)
            return 0
        except:
            pass
    return 0

def installFileString(target, source, env):
    source = str(source[0])
    target = str(target[0])
    return 'copying %s -> %s' % (source, target)

def byteCompileString(target, source, env):
    target = str(target[0])
    if target.endswith(".py"):
        return 'byte-compiling %s to %sc' % (target, os.path.basename(target))

installFileAction = Action(installFile, installFileString)
byteCompileAction = Action(byteCompile, byteCompileString)

PythonInstallBuilder = Builder(action=[installFileAction, byteCompileAction])

def installPythonSources(env, dir, source):
	"""Install specified files in the given directory."""
	import SCons.Node
	sources = env.arg2nodes(source, env.fs.File)
	dnodes = env.arg2nodes(dir, env.fs.Dir)
	#sources = SCons.Node.arg2nodes(source, env.fs.File)
	#dnodes = SCons.Node.arg2nodes(dir, env.fs.Dir)
	tgt = []
	for dnode in dnodes:
		for src in sources:
			target = SCons.Node.FS.default_fs.File(src.name, dnode)
			tgt.append(PythonInstallBuilder(env, target, src))
	if len(tgt) == 1:
		tgt = tgt[0]
	return tgt


############################################################################
# general utilities
############################################################################

def compareVersions(a, b):
    a = map(int, a.split('.'))
    b = map(int, b.split('.'))
    for i in range(min(len(a), len(b))):
        if a[i] < b[i]: return -1
        if a[i] > b[i]: return 1
    if len(a) < len(b): return -1
    if len(a) > len(b): return 1
    return 0

def addConfigTests(conf): 
    conf.AddTests(configure_tests)

def CustomConfigure(env):
    conf = env.Configure(log_file='#/.config.log')
    addConfigTests(conf)
    return conf

def ExpandList(list, arg):
	if type(arg) == types.ListType:
		list.extend(arg)
	else:
		list.append(arg)

############################################################################
# additional builders
############################################################################

def addDoxygen(env):
    def dox(target, source, env):
        cwd = os.getcwd()
        os.chdir(os.path.dirname(str(source[0])))
        sources = map(os.path.basename, map(str, source))
        result = os.system("doxygen %s" % " ".join(sources))
        os.chdir(cwd)
        return result
    #action = 'cd $SOURCE.dir && doxygen $SOURCES.file'
    action = dox
    env.Append(BUILDERS = {'Doxygen': Builder(action=action)})

def addCopyFile(env):
    def copy(target, source, env):
        shutil.copy(str(source[0]), str(target[0]))
    def report(target, source, env):
        source = str(source[0])
        target = str(target[0])
        return 'copying %s -> %s' % (source, target)
    CopyFile = Builder(action=Action(copy, report))
    env.Append(BUILDERS = {'CopyFile': CopyFile})

def addLinkFile(env):
    def copy(target, source, env):
        source = str(source[0])
        target = str(target[0])
        if os.name=='posix':
            os.link(source, target)
        else:
            shutil.copy(source, target)
    def report(target, source, env):
        source = str(source[0])
        target = str(target[0])
        return 'copying %s -> %s' % (source, target)
    CopyFile = Builder(action=Action(copy, report))
    env.Append(BUILDERS = {'LinkFile': CopyFile})

def addBuilders(env):
    addDoxygen(env)
    addCopyFile(env)
    addLinkFile(env)

############################################################################
# SWIG support
############################################################################

SWIG = 'swig'

def checkSwig(context, min_version, not_versions=[]):
	ok = 0
	context.Message("Checking for swig...")
	swig_in, swig_out, swig_err = os.popen3('%s -version' % SWIG, 't')
	if swig_err is not None:
		output = swig_err.readlines()
		output = " ".join(map(lambda x: x.strip(), output))
		match = re.search(r'SWIG Version (\d+\.\d+.\d+)', output)
		if match is not None:
			swig_version = match.groups()[0]
			if compareVersions(swig_version, min_version) >= 0:
				ok = 1
				for nv in not_versions:
					if compareVersions(swig_version, nv) == 0:
						ok = 0
	if ok:
		context.Result("yes (%s)" % swig_version)
		context.env['SWIG_VERSION'] = swig_version
	else:
		context.Result("no")
	return ok

configure_tests['checkSwig'] = checkSwig

  
def emitSwig(target, source, env):
	target = []
	assert(len(source)==1)
	ext = env['CXXFILESUFFIX']
	for s in source:
		wrapper = os.path.splitext(str(s))[0]+'_wrap'+ext
		target.append(wrapper)
		# XXX
		# Module name really should be based on the %module
		# directive in the interface file.
		module = os.path.splitext(str(s))[0]+'.py'
		target.append(module)
	return (target, source)

def addSwigBuild(env):
    builder = Builder(action = '$SWIG $SWIGFLAGS -o ${TARGETS[0]} $SOURCE',
                      emitter = emitSwig)
    env.Append(BUILDERS = {'Swig': builder})

def addSwigDep(env):
    def SwigScanner(node, env, path, arg=None):
        cmd = env.subst('$SWIG -MM $_CPPINCFLAGS %s' % str(node))
        stdin, stdout, stderr = os.popen3(cmd, 't')
        deps = ''.join(map(lambda x: x.strip(), stdout.readlines()))
        deps = map(lambda x: "#/"+x.strip(), deps.split('\\'))[1:]
        return deps
    scanner = Scanner(name = 'SwigScanner', function = SwigScanner, skeys = ['.i'], recursive = 0)
    env.Append(SCANNERS = scanner)

def addSwigSupport(env):
    global SWIG
    SWIG = SCons.Util.WhereIs('swig')
    env['SWIG'] = SWIG
    addSwigDep(env)
    addSwigBuild(env)


############################################################
# helper classes
############################################################

class OptionSet:
    class Empty:
        pass
    def __init__(self):
        self.set = OptionSet.Empty()
        self.add = OptionSet.Empty()
    def __getattr__(self, x):
        d = self.__dict__
        if x in d.keys(): return d[x]
        return getattr(d['set'], x)
    def methods(self):
        return self.set, self.add
    def apply(self, env):
        for key, value in self.set.__dict__.iteritems():
                env[key] = value
        for key, value in self.add.__dict__.iteritems():
                env.Append(**{key: value})

class Globals:
    class Empty:
        pass
    def __init__(self):
        self.add = Globals.Empty()
    def set(self):
        for key, value in self.set.__dict__.iteritems():
            eval("%s=%s" % (key, value), globals(), globals())



class Package:

	def _addDistBuilder(self):
		def DistAction(target, source, env):
			dist = str(target[0])
			assert dist.startswith('dist')
			if dist.endswith('.tar.gz'):
				base = dist[5:-7]
				command = "tar -vc --gzip -C dist -f %s %s" % (dist, base)
			elif dist.endswith('.tar.bz2'):
				base = dist[5:-8]
				command = "tar -vc --bzip2 -C dist -f %s %s" % (dist, base)
			elif dist.endswith('.tar.Z'):
				base = dist[5:-6]
				command = "tar -vc --compress -C dist -f %s %s" % (dist, base)
			elif dist.endswith('.tar'):
				base = dist[5:-4]
				command = "tar -vc -C dist -f %s %s" % (dist, base)
			elif dist.endswith('.zip'):
				base = dist[:-4]
				try:
					import zipfile
				except ImportError:
					raise SCons.Errors.InternalError('Internal zip module (zipfile) not available')
				def visit(z, dirname, names):
					for name in names:
						path = os.path.normpath(os.path.join(dirname, name))
						if os.path.isfile(path):
							z.write(path, path[5:])
				z = zipfile.ZipFile(dist, "w", compression=zipfile.ZIP_DEFLATED)
				os.path.walk(base, visit, z)
				z.close()
				return 0
			else:
				error = "Unknown archive format extension '%s'" % os.path.splitext(dist)[1]
				raise SCons.Errors.UserError(error)
				return 0
			return Action(command)(target, "", env)
		def DistReport(target, source, env):
			print "Creating package archive %s..." % str(target[0])
		env = self.env
		try:
			bld = env['BUILDERS']['DistArchive']
		except KeyError:
			bld = Builder(action=Action(DistAction, DistReport),
			              source_factory=SCons.Node.FS.default_fs.Entry)
			env['BUILDERS']['DistArchive'] = bld

	class Info: pass

	def __init__(self, env, **kw):
		addBuilders(env)
		self.env = env
		self._addDistBuilder()
		self.info = Package.Info()
		self.info.__dict__.update(kw)
		self.source_content = []
		self.binary_content = []
		self.dists = {}

	def addManifest(self, filename, type='both'):
		if not os.path.exists(filename): return
		f = open(filename, 'rt')
		include = []
		exclude = []
		for entry in f.readlines():
			cmd = entry.strip().split()
			if len(cmd) == 0: continue
			if len(cmd) == 1:
				action, args = 'include', cmd
			else:
				action, args = cmd[0], cmd[1:]
			if action == 'include':
				for arg in args:
					include.extend(glob.glob(arg))
			elif action == 'prune':
				for arg in args:
					exclude.extend(glob.glob(arg))
			elif action == 'recursive-include':
				dir, patterns = args[0], args[1:]
				def visit(arg, dir, names):
					include, patters = arg
					for pattern in patterns:
						include.extend(glob.glob(os.path.join(dir, pattern)))
				os.path.walk(dir, visit, (include, patterns))
		self.add(include, type)
		self.remove(exclude, type)

	def add(self, content, type='both'):
		content = self.env.arg2nodes(content, self.env.fs.File)
		#content = SCons.Node.arg2nodes(content, self.env.fs.File)
		if type == 'source' or type == 'both':
			self.source_content.extend(content)
		if type == 'binary' or type == 'both':
			self.binary_content.extend(content)

	def _filter_content(self, content, list):
		return filter(lambda x, y=map(str,list): not str(x) in y, content)

	def remove(self, content, type='both'):
		content = self.env.arg2nodes(content, self.env.fs.File)
		#content = SCons.Node.arg2nodes(content, self.env.fs.File)
		if type == 'source' or type == 'both':
			self.source_content = self._filter_content(self.source_content, content)
		if type == 'binary' or type == 'both':
			self.binary_content = self._filter_content(self.binary_content, content)

	def dump(self):
		pass #print map(str, self.content)

	def distpath(self, base=''):
		return os.path.join('dist', base)

	def base(self, extra=''):
		return '%s-%s%s' % (self.info.package, self.info.version, extra)

	def sdist(self, alias='sdist'):
		dist = self.distpath(self.base('-src'))
		self._dist(self.source_content, dist , alias)

	def bdist(self, alias='bdist'):
		dist = self.distpath(self.base())
		self._dist(self.binary_content, dist, alias)

	def writeInfo(self, base_dir):
		out = open(os.path.join(base_dir, "PKG-INFO"), 'wt')
		info = self.info.__dict__
		for key, value in info.iteritems():
			out.write("%s: %s\n" % (key, value))
		out.close()

	default_format = {'posix': 'tgz', 'nt': 'zip'}
	def _getArchiveFormats(self):
		env = self.env
		formats = getattr(env, 'ARCHIVE_FORMATS', None)
		if formats is None:
			try:
				formats = self.default_format[os.name]
			except KeyError:
				error = "Don't know how to create archive on platform '%s'" % os.name
				raise SCons.Errors.UserError(error)
		return formats.split()

	def _getArchiveExtension(self, format):
		if format in ['gztar', 'tar.gz', 'gzip', 'gz', 'tgz']:
			return '.tar.gz'
		if format in ['bztar', 'tar.bz2', 'bzip2', 'bz2', 'tbz2', 'tbz']:
			return '.tar.bz2'
		if format in ['ztar', 'tar.Z', 'compress', 'Z']:
			return '.tar.Z'
		if format in ['tar']:
			return '.tar'
		if format in ['zip']:
			return '.zip'
		return None

	def _dist(self, content, target, alias):
		env = self.env
		formats = self._getArchiveFormats()
		dist = target
		src = map(str, content)
		dst = map(lambda x: os.path.join(dist, x), src)
		src = self.env.arg2nodes(src, env.fs.File)
		dst = self.env.arg2nodes(dst, env.fs.File)
		#src = SCons.Node.arg2nodes(src, env.fs.File)
		#dst = SCons.Node.arg2nodes(dst, env.fs.File)
		ret = []
		for source, target in map(lambda x, y: (x,y), src, dst):
			ret.append(env.LinkFile(target, source))
		for format in formats:
			ext = self._getArchiveExtension(format)
			if ext is None:
				error = "Don't know how to create distribution archive format '%s'" % format
				raise SCons.Errors.UserError(error)
			ret.append(env.DistArchive(dist+ext, dst))
		self.dists[alias] = ret
  		env.Alias(alias, ret)

	def installTarget(self, package_files, include_files):
		env = self.env
		info = self.info
		package_target = os.path.join(env['PACKAGE_PREFIX'], info.package)
		include_target = os.path.join(env['INCLUDE_PREFIX'], info.package)
		install_package = installPythonSources(env, package_target, package_files)
		install_headers = installPythonSources(env, include_target, include_files)
		env.Alias('install_package', install_package)
		env.Alias('install_headers', install_headers)
		env.Alias('install', install_package + install_headers)

	def debian(self):
		env = self.env
		info = self.info
		base = self.base()
		path = self.distpath('debian')
		user = os.environ.get('USER', '')
		cmd = ['rm -rf %s && mkdir -p %s' % (path, path),
		       'cd %s && tar -vxzf ../%s-src.tar.gz && mv %s-src %s' % (path, base, base, base.lower()),
	 	       'cd %s/%s && LOGNAME="%s" dpkg-buildpackage -rfakeroot -d "-i.sconsign|.pyc|debian|.cache"' % (path, base.lower(), user)]
		env.Command('debs', self.dists['sdist'], cmd)

	def setTargets(self, targets, default=1):
		self.targets = targets
		if default: self.env.Default(targets)

	def unitTests(self, command):
		self.env.Depends('unittests', self.targets)
		self.env.Alias('test', self.env.Command('unittests', '', command))

def Prefix(dir, names):
    if type(names) == type(''):
        names = names.split()
    return map(lambda x: os.path.normpath(os.path.join(dir, x)), names)


def SelectBuildDir(env, build_dir, platform=None):
    if not platform:
        platform = env['PLATFORM']
    target_dir = os.path.join(build_dir, platform)
    if not os.path.exists(target_dir):
        os.makedirs(target_dir, 0755)
    print "Building in '%s'" % target_dir
    return target_dir


def ConfigPlatform(env):
    import build_config
    platform = env['PLATFORM']
    try:
        config = eval('build_config.Config_%s' % platform)
    except:
        print "No configuration for platform '%s', aborting" % platform
        sys.exit(0)
    config(env)

def ConfigPython(env):
    env['PYTHON_INC'] = getPythonInc()
    env['PYTHON_LIB'] = getPythonLib()

class Config:
	def __init__(self, env):
		self.__dict__['env'] = env.Dictionary()
		self.config(env)
		self.configSwig(env)
	def __setattr__(self, attr, value):
		dict = self.__dict__['env']
		dict[attr] = value
	def __getattr__(self, attr):
		dict = self.__dict__['env']
		return dict.get(attr,None)
	def configSwig(self, env):
		version = env['SWIG_VERSION']
		self.SWIGFLAGS = ' -c++ -python -noexcept -IInclude -I%s' % self.PYTHON_INC
		if compareVersions(version, '1.3.20') >= 0:
			self.SWIGFLAGS = self.SWIGFLAGS + ' -runtime'
		else:
			self.SWIGFLAGS = self.SWIGFLAGS + ' -c'
			self.SHLINKLIBS.append('swigpy')
