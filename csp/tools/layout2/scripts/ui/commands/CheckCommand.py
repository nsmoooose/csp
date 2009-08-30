#!/usr/bin/env python
import wx

from Command import Command

class CheckCommand(Command):
	"""Base class for all command objects that can be toggled on or off.
	Implement apropriate methods in your derived class to execute
	this command."""
	
	def __init__(self):
		Command.__init__(self)
		self.checked = False
	
	def GetKind(self):
		return wx.ITEM_CHECK
	
	def Check(self, check = True):
		self.checked = check
		for menuItem in self.menuItems:
			menuItem.Check(self.checked)
		for tool in self.tools:
			tool.GetToolBar().ToggleTool( tool.GetId(), self.checked )
		return self

	def IsChecked(self):
		return self.checked

	def Toggle(self):
		self.Check(not self.checked)
		return self
	
	def AppendInMenu(self, parent, menu, controlId):
		menuItem = Command.AppendInMenu(self, parent, menu, controlId)
		menuItem.Check(self.checked)
		return menuItem
	
	def SetMenuItemBitmap(self, menuItem):
		pass

	def AppendInToolBar(self, parent, toolbar, controlId):
		tool = Command.AppendInToolBar(self, parent, toolbar, controlId)
		toolbar.ToggleTool( controlId, self.checked )
		return tool
	
	def Execute(self):
		pass
