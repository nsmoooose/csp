import wx

from .Command import Command

class QuitCommand(Command):
	caption = "Quit"
	tooltip = "Quits this application"
	toolbarimage = "quit"

	def Execute(self):
		application = wx.GetApp()
		topWindow = application.GetTopWindow()
		if topWindow != None:
			topWindow.Close()

			# Close the configuration file in order to force saving
			application.Configuration.close()
