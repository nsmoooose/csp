# Combat Simulator Project - CSPSim
# Copyright (C) 2002 The Combat Simulator Project
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


import traceback, sys

class Shell:
	def __init__(self):
		import ShellEnvironment
		self.locals = ShellEnvironment.__dict__
		self.globals = ShellEnvironment.__dict__

	def run(self, command):
		result = ""
		try:
			result = eval(command, self.globals, self.locals)
			if result is None:
				result = ""
			else:
				result = str(result)
		except:
			try:
				exec(command, self.globals, self.locals)
			except:
				msg = ''.join(traceback.format_exception(sys.exc_info()[0], sys.exc_info()[1], sys.exc_info()[2]))
				result = msg
		return result.strip()