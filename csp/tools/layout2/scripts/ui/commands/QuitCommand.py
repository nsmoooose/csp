import wx

from Command import Command

class QuitCommand(Command):

	def GetControlId(self):
		return wx.ID_EXIT

	def GetCaption(self):
		return "Quit"

	def GetToolTipText(self):
		return "Quits this application"

	def GetToolBarImageName(self):
		return "quit"

	def Execute(self):
		application = wx.GetApp()
		topWindow = application.GetTopWindow()
		if topWindow != None:
			topWindow.Close()
			
			# Close the configuration file in order to force saving
			application.Configuration.close()
