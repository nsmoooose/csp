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


import sys

if __name__ == '__main__':
	print 'This module provides support for the SCons build system; it is not'
	print 'intended to be run as a standalone program.'
	sys.exit(1)


from csp.tools.build import scons

import SCons.Action
import SCons.Builder


# BUILD LOG AND DISPLAY MESSAGES
#
# The code here modifies the way scons displays build operations.  As of
# version 0.96.91, SCons provides a relatively simple mechanism for
# customizing build messages via environment variables.  Unfortunately
# this feature has some bugs and significant limitations.  In particular,
# it isn't possible to customize the output for certian types of commands
# (such as $ARCOM, which is a known issue that will probably be fixed in
# the next release), and there is no easy way to simplify the display output
# while still logging the full build commands.
#
# Avoiding these limitations requires low-level tweaks to the scons Action
# classes.  This is extremely brittle and prone to breakage with each new
# SCons release, although hopefully soon SCons will provide good enough
# support for build logging and message customization that these hacks will
# no longer be needed.


BUILD_LOG = open(scons.File('#/.buildlog').abspath, 'wt')


def WriteToBuildLog(message):
	"""
	Write a message to the build log.  The log should contain a verbose
	record of the actual commands that were executed.
	"""
	if message and BUILD_LOG:
		print >>BUILD_LOG, message
		BUILD_LOG.flush()


def TargetMessage(phrase, suffix=''):
	"""Simplified build message function that displays the first target."""
	def printer(target, source, env, phrase=phrase, suffix=suffix):
		if type(target) == type([]):
			target = map(str, target)
			if len(target) == 1:
				target = target[0]
			else:
				target = ', '.join(target)
		print '- %s %s%s' % (phrase, target, suffix)
	return printer


def SourceMessage(phrase, suffix='', limit=None):
	"""Simplified build message function that displays the first source."""
	def printer(target, source, env, phrase=phrase, suffix=suffix):
		if type(source) == type([]):
			source = map(str, source)
			if limit is not None and len(source) > limit:
				source = source[:limit]
			if len(source) == 1: source = source[0]
		print '- %s %s%s' % (phrase, source, suffix)
	return printer


def SourceToTargetMessage(phrase, suffix=''):
	"""Simplified build message function that displays the first source and target."""
	def printer(target, source, env, phrase=phrase, suffix=suffix):
		source = map(str, source)
		target = map(str, target)
		print '- %s %s to %s %s' % (phrase, source[0], target[0], suffix)
	return printer


def ActionCallWrapper(self, target, source, env, *args, **kw):
	"""
	A replacement for SCons.Action._ActionAction.__call___.  This function wraps
	the original __call__ implementation to provide low-level hooks for logging
	and displaying build messsages.
	
	Note that this is an extremely brittle hack than should be removed If and
	when SCons provides better native support for these customizations.

	Rationale: As of SCons 0.96.91, the introduction of memoization and lazy
	action evaluation makes it impossible to directly modify many of the
	important Action instances (since they are generated on the fly during the
	build).  The only customization hook is to override strfunction, but the
	replacement function does not have access to the Action instance and so
	cannot generate the actual build command string.
	"""
	if not scons.IsList(target):
		target = [target]
	if not scons.IsList(source):
		source = [source]

	# generate the full build message by directly calling the original strfunction
	# implementation.
	log = self.__class__.strfunction(self, target, source, env)
	WriteToBuildLog(log)

	# if the strfunction has been replaced with our ShortMessageWrapper helper we
	# pass the action and verbose log message as additional arguments.  these are
	# currently needed in order to suppress duplicate display messages for some
	# multi-action build steps (such as $ARCOM).
	if isinstance(self.strfunction, ShortMessageWrapper):
		self.strfunction(target, source, env, action=self, log=log)
	else:
		if self.strfunction is not None:
			self.strfunction(target, source, env)

	# disable build message display in the original __call__ implementation.
	# ideally we would let the original method handle build logging via
	# env['PRINT_CMD_LINE_FUNC'] and this wrapper would just provide the
	# short message display, but with the new memoization infrastructure in
	# SCons the only way to get a custom build function into the action is
	# to override strfunction.  (we could swap strfunction here, but that would
	# cause problems for parallel builds.)
	if len(args) > 2:
		args = list(args)
		args[2] = 0
	else:
		kw['show'] = 0

	# now call the original __call__ implementation.
	return self._internal_call(target, source, env, *args, **kw)


class ShortMessageWrapper:
	def __init__(self, message):
		self.message = message

	def bind(self, env, action):
		if scons.GetVersionTuple() >= (0, 96, 91):
			self._bindNew(env, action)
		else:
			self._bindOld(env, action)

	def _bindNew(self, env, action):
		if isinstance(action, SCons.Action.LazyAction):
			action.gen_kw['strfunction'] = self
			#self._bind(env, action)
			#self.action = action
			return
		if isinstance(action, SCons.Action.CommandGeneratorAction) and hasattr(action, 'generator'):
			if isinstance(action.generator, SCons.Builder.DictCmdGenerator):
				for ext, subaction in action.generator.items():
					if subaction:
						self._bind(env, subaction)
					else:
						print 'warning: unable to attach short message wrapper to action (%s) for extension "%s"' % (self.message, ext)
				return
		if isinstance(action, SCons.Action.ListAction):
			for act in action.list:
				self._bind(env, act)
			return
		self._bind(env, action)

	def _bindOld(self, env, action):
		if isinstance(action, SCons.Action.ListAction):
			for act in action.list:
				self._bind(env, act)
			return
		self._bind(env, action)

	def _bind(self, env, action):
		strfunction = getattr(action, 'strfunction', None)
		if not isinstance(strfunction, ShortMessageWrapper):
			action.strfunction = self

	def __call__(self, target, source, env, action=None, log=''):
		# nasty hack.  staticlib and sharedlib are multistep actions, but we only
		# want to display one message.  here we suppress output for the less interesting
		# step(s).
		if not log.startswith('ranlib') and not log.startswith('SharedFlagChecker'):
			return self.message(target, source, env)


def SetShortMessage(env, builder, message):
	idx = builder.find(':')
	if idx > 0:
		extension = builder[idx+1:]
		builder = builder[:idx]
	else:
		extension = None
	builders = env['BUILDERS']
	if builders.has_key(builder):
		if extension is None:
			action = builders[builder].action
		else:
			action = builders[builder].cmdgen.get(extension, None)
			if action is None: return
			if type(action) == type(''):
				action = SCons.Action.Action(action)
				builders[builder].add_action(extension, action)
		ShortMessageWrapper(message).bind(env, action)


def InstallMessageHook():
	try:
		action_class = SCons.Action._ActionAction
	except:
		action_class = SCons.Action.ActionBase  # version 0.96.1 and earlier
	action_class._internal_call = action_class.__call__
	action_class.__call__ = ActionCallWrapper


def SetShortMessages(env):
	InstallMessageHook()
	SetShortMessage(env, 'Program', TargetMessage('Building'))
	SetShortMessage(env, 'StaticObject', SourceMessage('Compiling'))
	SetShortMessage(env, 'SharedObject', SourceMessage('Compiling', suffix=' (shared)'))
	SetShortMessage(env, 'Object', SourceMessage('Compiling'))
	SetShortMessage(env, 'StaticLibrary', TargetMessage('Linking'))
	SetShortMessage(env, 'SharedLibrary', TargetMessage('Linking'))
	SetShortMessage(env, 'Doxygen', SourceMessage('Generating documentation', limit=1))
	SetShortMessage(env, 'Swig', SourceMessage('Generating SWIG interface', limit=1))
	SetShortMessage(env, 'CXXFile:.i', SourceMessage('Generating SWIG interface', limit=1))
	SetShortMessage(env, 'CXXFile:.net', SourceMessage('Generating tagged record', limit=1))
	SetShortMessage(env, 'LinkFile', SourceToTargetMessage('Creating link'))
	SetShortMessage(env, 'CopyFile', SourceToTargetMessage('Copying'))

