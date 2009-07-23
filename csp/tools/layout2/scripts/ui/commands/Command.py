#!/usr/bin/env python
import os.path
import wx

from csp.tools.layout2.scripts.ui.CommandControlFactory import EventToCommandExecutionAdapter

class Command:
	"""Base class for all command objects. Implement
	apropriate methods in your derived class to execute
	this command."""
	
	def __init__(self):
		self.menuItems = set()
		self.tools = set()
		self.enabled = True
	
	def GetKind(self):
		return wx.ITEM_NORMAL
	
	def GetControlId(self):
		return None
	
	def GetCaption(self):
		"""The name of the command to be displayed in a menu
		for example."""
		return ""

	def GetToolTipText(self):
		"""Returns a text to use when the mouse is hoovering 
		over the command button in a toolbar."""
		return ""

	def GetToolBarImageName(self):
		"""Returns the name of the image file for this command.
		The name will be used to load the file into a
		toolbar button."""
		return "generic.png"

	def Enable(self, enable = True):
		self.enabled = enable
		for menuItem in self.menuItems:
			menuItem.Enable(self.enabled)
		for tool in self.tools:
			tool.Enable(self.enabled)
		return self

	def IsEnabled(self):
		return self.enabled
	
	def AppendInMenu(self, parent, menu, controlId):
		menuItem = menu.Append( id = controlId, text = self.GetCaption(), help = self.GetToolTipText(), kind = self.GetKind() )
		menuItem.Enable(self.enabled)
		parent.Bind(wx.EVT_MENU, EventToCommandExecutionAdapter(self).Execute, menuItem)
		self.menuItems.add( menuItem )
		return menuItem

	def AppendInToolBar(self, parent, toolbar, controlId):
		bitmap = wx.BitmapFromImage(wx.Image(os.path.join( "images", self.GetToolBarImageName() )))
		tool = toolbar.AddLabelTool( id = controlId, label = self.GetCaption(), bitmap = bitmap, kind = self.GetKind(), shortHelp = self.GetToolTipText(), longHelp = self.GetToolTipText() )
		tool.Enable(self.enabled)
		parent.Bind(wx.EVT_TOOL, EventToCommandExecutionAdapter(self).Execute, tool)
		self.tools.add( tool )
		return tool
	
	def Execute(self):
		pass
