#!/usr/bin/env python3

# Combat Simulator Project
# Copyright (C) 2002, 2004 The Combat Simulator Project
# http://csp.sourceforge.net
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


## @author Mark Rose <mkrose@users.sourceforge.net>

"""
Input interface map compiler.  Reads an input interface map and generates
compact representation used by the simulation.  See tools/hid/README.map
for a description of the interface map language.
"""

import sys
import os.path


USE_OSG_EVENT = 0

if USE_OSG_EVENT:
	# OSG modifiers
	KMODS = {
		"LSHIFT" : 0x0001,
		"RSHIFT" : 0x0002,
		"LCTRL"  : 0x0004,
		"RCTRL"  : 0x0008,
		"LALT"   : 0x0010,
		"RALT"   : 0x0020,
		"LMETA"  : 0x0040,
		"RMETA"  : 0x0080,
		"LSUPER" : 0x0100,
		"RSUPER" : 0x0200,
		"LHYPER" : 0x0400,
		"RHYPER" : 0x0800,
		"NUM"    : 0x1000,
		"CAPS"   : 0x2000,
	}
else:
	# SDL modifiers
	KMODS = {
		"LSHIFT" : 0x0001,
		"RSHIFT" : 0x0002,
		"LCTRL"  : 0x0040,
		"RCTRL"  : 0x0080,
		"LALT"   : 0x0100,
		"RALT"   : 0x0200,
		"LMETA"  : 0x0400,
		"RMETA"  : 0x0800,
		"NUM"    : 0x1000,
		"CAPS"   : 0x2000,
		"MODE"   : 0x4000,
	}

MULTI_KMODS = {
	"SHIFT" : ("LSHIFT", "RSHIFT"),
	"CTRL"  : ("LCTRL", "RCTRL"),
	"ALT"   : ("LALT", "RALT"),
	"META"  : ("LMETA", "RMETA"),
}


BUTTON_PRESSED = 0x01

def BUTTON_FIELD_BIT(x):
	if x < 0 or x > 7:
		raise Error("invalid mouse button identifier %d (should be 0..7)." % x)
	if x == 0: return 0
	return BUTTON_PRESSED << (x-1)


class Error(Exception):
	def __init__(self, msg):
		self.msg = msg


def convertToInt(s):
	try:
		if s.startswith("0x"):
			return int(s[2:], 16)
		else:
			return int(s)
	except:
		raise Error("expected an integer or hex value, got '%s' instead." % s)


def generatePermutations(m):
	result = []
	if len(m) > 0:
		subperms = generatePermutations(m[1:])
		for item in m[0]:
			result.extend(list(map(lambda x, i=item: i|x, subperms)))
	else:
		result = [0]
	return result


class Action:
	def __init__(self, id, time, stop = 0, loop = -1, mode = -1, jmod = -1):
		self.id = id
		self.time = time
		self.stop = stop
		self.loop = loop
		self.mode = mode
		self.jmod = jmod
		
	def __repr__(self):
		s = "%0.2f:%s" % (self.time, self.id)
		if self.mode >= 0: s = s + "/MODE_%d" % self.mode
		if self.stop: s = s + "/YIELD"
		if self.jmod >= 0: s = s + "/SHIFT_%d" % self.jmod
		if self.loop >= 0: s = s + "/LOOP_%d" % self.loop
		return s


class Mapping:
	def __init__(self, device, number, mmod, kmod, jmod, type, mode, event_id, script):
		self.device = device
		self.number = number
		self.type = type
		self.mmod = mmod
		self.kmod = kmod
		self.jmod = jmod
		self.event_id = event_id
		self.mode = mode
		self.script = script

	def write(self, outf=sys.stdout):
		for action in self.script:
			script = "@%05.2f %+03d %+05d %+03d %+03d %s" % (action.time, action.mode, action.jmod, action.stop, action.loop, action.id)
			print(script, file=outf)
		code = "+"
		code = code + self.device.upper()[0]
		code = code + " %1d " % self.number
		code = code + self.type.upper()[0]
		code = code + " %04d %04d %04d %04d" % (self.kmod, self.jmod, self.mmod, self.event_id)
		code = code + " %02d" % self.mode
		print(code, file=outf)
		
	def dump(self):
		print("%s:%d" % (self.device.upper(), self.number), end=' ')
		if self.device == 'joystick':
			self.dump_joystick()
		elif self.device == 'keyboard':
			self.dump_keyboard()
		elif self.device == 'mouse':
			self.dump_mouse()

	def dump_joystick(self):
		if self.type == 'axis':
			print("AXIS(%d) =" % self.event_id, end=' ')
			print(self.script)
		else:
			if self.jmod > 0:
				print("SHIFT +", end=' ')
			print("BUTTON(%d)" % self.event_id, end=' ')
			print(self.type.upper(), end=' ')
			print("=", self.script, end=' ')
			print("in mode", self.mode)
	
	def dump_keyboard(self):
		print("KEY(%d){%d}" % (self.event_id, self.kmod), end=' ')
		print("=", self.script, end=' ')
		print("in mode", self.mode)

	def dump_mouse(self):
		if self.type == "move":
			print("MOVE (mmod=%d, kmod=%d)" % (self.mmod, self.kmod), end=' ') 
		else:
			print("BUTTON(%d)" % self.event_id, end=' ')
			print("(kmod=%d)" % self.kmod, end=' ')
		print("=", self.script, end=' ')
		print("in mode", self.mode)


class VirtualDeviceDefinition:
	def __init__(self, include_path='.'):
		self.included = {}
		self.values = {}
		self.scripts = {}
		self.modes = {}
		self.maps = []
		self.definitions = {}
		self.devices = {}
		self.filestack = []
		self.pathstack = []
		self.include_path = []
		if include_path is not None:
			if isinstance(include_path, str):
				include_path = [include_path]
			self.include_path = list(include_path)
		#if include_path:
		#	self.pathstack = [include_path]
		self.line = 0
		self.file = ""
		self.bind = {}

	def write(self, file):
		bindings = list(self.bind.keys())
		bindings.sort()
		for b in bindings:
			print("=%s" % b, file=file)
		for m in self.maps:
			m.write(file)

	def read(self, fn):
		if fn == '-':
			f = sys.stdin
		else:
			f = open(fn, 'rt')
		if f is not None:
			self.file = fn
			self.parse(f)
		else:
			raise Error("unable to open file '%s'." % fn)

	def parse(self, f):
		for line in f:
			self.line = self.line + 1
			idx = line.find("#")
			if idx >= 0:
				line = line[:idx]
			line = line.strip()
			if len(line) == 0: continue;
			p = line.split()
			command = p[0]
			try:
				self.processCommand(command, p[1:])
			except Error as e:
				for file, line_number in self.filestack:
					print("In file included from %s(%d), " % (file, line_number), file=sys.stderr)
				print("%s(%d): %s" % (self.file, self.line, e.msg), file=sys.stderr)
				print(">", line, file=sys.stderr)
				sys.exit(1)

	def search(self, fn):
		if os.path.isabs(fn):
			return fn, 0
		local = (os.path.dirname(fn) == '')
		for path in self.pathstack[-1:] + self.include_path:
			trial = os.path.join(path, fn)
			if os.path.exists(trial):
				return trial, not local
		raise Error("include file not found")

	def includeCommand(self, args):
		n = len(args)
		if n == 1:
			fn = args[0]
			if fn not in self.included:
				self.included[fn] = 1
				self.filestack.append((self.file, self.line))
				fn, relative = self.search(fn)
				if relative: self.pathstack.append(os.path.dirname(fn))
				self.read(fn)
				if relative: self.pathstack.pop()
				self.file, self.line = self.filestack.pop()
		else:
			raise Error("incorrect number of parameters for 'include' statement.")
	
	def modeCommand(self, args):
		n = len(args)
		if n == 1:
			mode = args[0]
			if mode not in self.modes:
				self.modes[mode] = len(self.modes)
		else:
			raise Error("incorrect number of parameters for 'mode' statement.")

	def scriptCommand(self, args):
		n = len(args)
		if n == 1 or n == 2:
			name = args[0]
			if n == 1:
				self.scripts[name] = None
			else:
				script = args[1]
				script = self.unrollScript(script)
				if name in self.scripts:
					print("WARNING: script %s is multiply defined." % name)
				else:
					self.scripts[name] = script
		else:
			raise Error("incorrect number of parameters for 'script' statement.")

	def deviceCommand(self, args):
		n = len(args)
		if n == 3 or n == 4:
			type = args[0]
			if not type in ("joystick", "keyboard", "mouse"):
				raise Error("unrecognized device type '%s'.  Must be 'joystick', 'mouse', or 'keyboard'." % type)
			number = int(args[1])
			if number < 0 or number > 7:
				raise Error("invalid device number %d (must be 0..7)." % number)
			name = args[2]
			if n == 4:
				definitions = args[3]
				if definitions not in self.definitions:
					raise Error("undefined device definition '%s'." % definitions)
			else:
				definitions = None
			self.devices[name] = (type, number, definitions)
		else:
			raise Error("incorrect number of parameters for 'device' statement.")

	def defineCommand(self, args):
		n = len(args)
		if n == 2:
			device, name = self.splitDevice(args[0])
			id = self.unrollDefinition(args[1])
			self.setDefinition(device, name, id)
		else:
			raise Error("incorrect number of parameters for 'define' statement.")

	def mapCommand(self, args):
		n = len(args)
		if n == 3 or n == 4:
			multi, device_type, device_number, mmod, kmod, jmod, value  = self.parseEvent(args[0])
			type = args[1]
			if device_type == 'mouse':
				valid = ('press', 'release', 'move')
			elif device_type == 'joystick':
				valid = ('press', 'release', 'axis')
			else:
				valid = ('press', 'release')
			if not type in valid:
					raise Error("invalid option '%s' in map command.  must be one of %s." % (type, str(valid)))
			if type == 'move':
				mmod = mmod | BUTTON_FIELD_BIT(value)
			if n > 3:
				modes = args[2]
				script = args[3]
			else:
				modes = "*"
				script = args[2]
			if '*' in modes:
				modes = list(self.modes.keys())
				if len(modes) == 0:
					raise Error("map statement, but no modes defined.")
			else:
				modes = modes.split("|")
				for mode in modes:
					if mode not in self.modes:
						raise Error("mode '%s' used but not defined." % mode)
			script = self.unrollScript(script)
			if type in ('move', 'axis') and len(script) > 1:
				raise Error("'%s' mapping can have only one target identifier." % type)
			if len(multi) > 0:
				multi = generatePermutations(multi)
			else:
				multi = (0,)
			script = self.buildScript(script)
			for mode in modes:
				mode_id = self.modes[mode]
				for m in multi:
					map = Mapping(device_type, device_number, mmod, kmod|m, jmod, type, mode_id, value, script)
					self.maps.append(map)
		else:
			raise Error("incorrect number of parameters for 'map' statement.")

	def bindCommand(self, args):
		n = len(args)
		if n == 1:
			self.bind[args[0]] = 1
		else:
			raise Error("incorrect number of parameters for 'bind' statement.")

	def processCommand(self, command, args):
			if command == "include":
				self.includeCommand(args)
			elif command == "mode":
				self.modeCommand(args)
			elif command == "script":
				self.scriptCommand(args)
			elif command == "device":
				self.deviceCommand(args)
			elif command == "define":
				self.defineCommand(args)
			elif command == "map":
				self.mapCommand(args)
			elif command == "bind":
				self.bindCommand(args)
			else:
				raise Error("Unrecognized command '%s'" % command)

	def parseEvent(self, event):
		parts = event.split(':')
		if len(parts) != 2:
			raise Error("bad syntax in '%s', should be of the form 'device:event'." % event)
		device_name, event = parts
		device = self.devices.get(device_name, None)
		if device is None:
			raise Error("undefined device '%s'." % device_name)
		type, id, definitions = device
		if definitions:
			definitions = self.definitions.get(definitions, {})
		else:
			definitions = {}
		mmod, kmod, jmod, value = 0, 0, 0, 0
		multi = []
		mods = event.split('-')
		mods, event = mods[:-1], mods[-1]
		if type == "joystick":
			for mod in mods:
				if mod == "SHIFT":
					jmod = 1
		else:
			for mod in mods:
				if mod in KMODS:
					kmod = kmod + KMODS[mod]
				elif mod in MULTI_KMODS:
					multi.append(list(map(KMODS.get, MULTI_KMODS[mod])))
				elif type == "mouse" and mod in definitions:
					mmod = mmod | BUTTON_FIELD_BIT(definitions[mod])
				else:
					raise Error("unrecognized modifier '%s' in event '%s'." % (mod, event))
		if event in definitions:
			value = definitions[event]
		else:
			try:
				value = int(event);
			except:
				raise Error("unknown event id '%s'" % event)
		if value < 0 or value > 4095:
			raise Error("invalid event id %d (must be 0..4095)." % value)
		return multi, type, id, mmod, kmod, jmod, value

	def unrollScript(self, script):
		result = []
		actions = script.split("|")
		for action in actions:
			try:
				delay = float(action)
				if delay > 0.0:
					result.append(delay)
			except:
				if action in self.scripts:
					s = self.scripts[action]
					if s is not None:
						result.extend(s)
				else:
					result.append(action)
		return result
		
	def splitDevice(self, device):
		return device.split(':')

	def unrollDefinition(self, d):
		if ':' in d:
			device, id = d.split(':')
			type, number, defs = self.devices.get(device, ("", 0, {}))
			return defs.get(id, None)
		return int(d, 0)

	def setDefinition(self, device, name, id):
		defs = self.definitions.setdefault(device, {})
		defs[name] = id
		
	def buildScript(self, script):
		time = 0.0
		loopstart = 0
		actions = []
		for item in script:
			if isinstance(item, str):
				item = item.split('/')
				command = item[0]
				if len(item) > 1:
					opts = item[1]
				else:
					opts = ''
				stop, loop, jmod, mode = 0, -1, -1, -1
				if 'B' in opts:
					stop = 1
				if '*' in opts:
					loopstart = len(actions)
				if 'R' in opts:
					loop = loopstart
				if command in self.modes:
					mode = self.modes[command]
					command = ""
				elif command == "SHIFT":
					jmod = 1
					command = ""
				elif command == "NOSHIFT":
					jmod = 0
					command = ""
				if time >= 100.0:
					raise Error("Script delay too long (%.1f >= 100 s)" % time)
				action = Action(command, time, stop, loop, mode, jmod)
				actions.append(action)
				time = 0.0
				if loop > -1:
					break
			else:
				time = time + item
		if len(actions) == 0:
			actions.append(Action("", 0.0))  # nop
		return actions


class VirtualDevice:
	def __init__(self):
		self.mode = 0
		self.mapping = {}


class MapCompiler:
	def compile(mapfile, outfile, include_path=''):
		if isinstance(outfile, str): outfile = open(outfile, 'w')
		v = VirtualDeviceDefinition(include_path=include_path)
		v.read(mapfile)
		v.write(outfile)
	compile = staticmethod(compile)


