import Tkinter, CSP
import traceback, sys
import threading

class Scrolled:
	def __init__(self, barx, bary):
		self.barx = barx
		self.bary = bary
		if barx is not None:
			barx['command'] = self.__scrollXHandler
			self["xscrollcommand"] = barx.set
		if bary is not None:
			bary['command'] = self.__scrollYHandler
			self["yscrollcommand"] = bary.set
			
	def __scrollXHandler(self, *L):
		op, count = L[0], L[1]
		if op == "scroll":
			units = L[2]
			self.xview_scroll(count, units)
		else:
			self.xview_moveto(count)
			
	def __scrollYHandler(self, *L):
		op, count = L[0], L[1]
		if op == "scroll":
			units = L[2]
			self.yview_scroll(count, units)
		else:
			self.yview_moveto(count)


class SmartEntry(Tkinter.Entry, Scrolled):
	def __init__(self, barx, bary, hwindow, *args, **opts):
		Tkinter.Entry.__init__(self, *args, **opts)
		Scrolled.__init__(self, barx, bary)
		self.hwindow = hwindow
		self.contents = Tkinter.StringVar()
		self["textvariable"] = self.contents
		self.bind('<Up>', self.up)
		self.bind('<Down>', self.down)
		self.bind('<Key-Return>', self.enter)
		self.history=['']
		self.idx = 0
		self.command = ''
		self.cb = None

	def setCallback(self, cb):
		self.cb = cb

	def getCommand(self):
		return self.command

	def save(self):
		if self.idx == 0:
			self.history[-1] = self.contents.get()
			
	def enter(self, event):
		self.command = self.contents.get()
		self.idx = 0
		if len(self.command) > 0:
			self.idx = 0
			self.save()
			self.hwindow.add(">>> " + self.command)
			self.history.append('')
		self.contents.set('')
		if self.cb is not None:
			self.cb(self)

	def jump(self, idx):
		if idx >= 0 and idx < len(self.history):
			self.idx = idx
			command = self.history[-1-idx]
			self.contents.set(command)
			self.icursor(len(command))

	def up(self, event):
		self.save()
		self.jump(self.idx + 1)

	def down(self, event):
		self.jump(self.idx - 1)


class History(Tkinter.Text, Scrolled):
	def __init__(self, barx, bary, *args, **opts):
		Tkinter.Text.__init__(self, *args, **opts)
		Scrolled.__init__(self, barx, bary)

	def add(self, line):
		self['state'] = Tkinter.NORMAL
		self.insert(Tkinter.END, line+"\n")
		self['state'] = Tkinter.DISABLED
		self.see(Tkinter.END)

		
font = ("Courier", "14")


class TkThread(threading.Thread):
	def __init__(self, sim):
		threading.Thread.__init__(self)
		self.sim = sim
	def __del__(self):
		print dir(self)
		print self.__dict__
		print "~THREAD"
	def run(self):
		self.build()
		self.toplevel.deiconify()
		self.toplevel.mainloop()
		self.toplevel.withdraw()
		self.clean()
		self.sim.endConsole()
		print "THREAD COMPLETE."
	def clean(self):
		self.toplevel.destroy()
		self.root.destroy()
		del self.root
		del self.toplevel
		del self.frame
		del self.entry
		del self.history
	def build(self):
		self.root = Tkinter.Tk()
		self.root.withdraw()
		self.toplevel = Tkinter.Toplevel()
		self.toplevel.protocol("WM_DELETE_WINDOW", self.exit)
		self.frame = Tkinter.Frame(self.toplevel)
		self.frame.master.title("CSP Console")
		self.frame.pack(expand=1, fill="both")
		frame = Tkinter.Frame(self.frame)
		frame.pack(expand=1, fill="both")
		scroll = Tkinter.Scrollbar(frame, orient=Tkinter.VERTICAL)
		self.history = History(None, scroll, frame, height=20, width=80, state=Tkinter.DISABLED, takefocus=0, selectbackground="blue", selectforeground="white")
		self.history.pack(expand=1, side='left', fill="both")
		scroll.pack(side='right', expand=0, fill="y")  
		frame = Tkinter.Frame(self.frame)
		frame.pack(expand=0, fill="both")
		scroll = Tkinter.Scrollbar(frame, orient=Tkinter.HORIZONTAL)
		self.entry = SmartEntry(scroll, None, self.history, frame, width=80, bg="white")
		self.entry.bind('<Escape>', self.exit)
		self.entry.pack(side='top', fill="x")
		self.entry.setCallback(self.do)
		scroll.pack(side='bottom', fill="x") 
		self.locals = {}
	def do(self, entry):
		command = entry.getCommand()
		if command.strip() == "quit":
			self.exit()
			return
		gl = CSP.__dict__
		try:
			self.history.add(str(eval(command, gl, self.locals)))
		except:
			try:
				exec(command, gl, self.locals)
			except:
				msg = ''.join(traceback.format_exception(sys.exc_info()[0], sys.exc_info()[1], sys.exc_info()[2]))
				self.history.add(msg)
	def exit(self, *args):
		#print self.entry.contents.get()
		self.frame.quit()	

		

class Console:
	def __init__(self, sim):
		self.sim = sim

	def run(self):
		thread = TkThread(self.sim)
		thread.start()
		print "THREAD STARTED."


if __name__ == "__main__":
	Console().run()
