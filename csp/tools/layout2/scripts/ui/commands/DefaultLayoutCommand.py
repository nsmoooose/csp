import wx

from Command import Command
from TogglePaneCommand import TogglePaneCommand

class DefaultLayoutCommand(Command):

	def __init__(self, defaultPerspective):
		Command.__init__(self)
		self.defaultPerspective = defaultPerspective
	
	def GetCaption(self):
		return "Default layout"

	def GetToolTipText(self):
		return "Reset to the default windows layout"

	def GetToolBarImageName(self):
		return "generic"

	def Execute(self):
		auiManager = wx.GetApp().auiManager
		auiManager.LoadPerspective(self.defaultPerspective, update = True)
		TogglePaneCommand.UpdateAllPanes()
