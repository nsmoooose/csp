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
