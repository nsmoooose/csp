#!/usr/bin/env python
import wx

from CheckCommand import CheckCommand

class TogglePaneCommand(CheckCommand):
	"""Toggle the visibility of the pane passed at construction.
	For this command to be valid, the pane must have been added to the auiManager."""
	
	AllInstances = set()
	
	@staticmethod
	def UpdateAllPanes():
		for instance in TogglePaneCommand.AllInstances:
			instance.UpdatePane();
	
	@staticmethod
	def on_PaneClose(pane):
		for instance in TogglePaneCommand.AllInstances:
			instance.PaneClosed(pane);
	
	def __init__(self, pane):
		CheckCommand.__init__(self)
		TogglePaneCommand.AllInstances.add( self )
		self.pane = pane
		
		auiManager = wx.GetApp().auiManager
		self.checked = auiManager.GetPane(self.pane).IsShown()
	
	def GetCaption(self):
		auiManager = wx.GetApp().auiManager
		return "Show %s" % auiManager.GetPane(self.pane).caption

	def GetToolTipText(self):
		auiManager = wx.GetApp().auiManager
		return "Toggle the visibility of the %s window" % auiManager.GetPane(self.pane).caption

	def GetToolBarImageName(self):
		return "generic"

	def UpdatePane(self):
		auiManager = wx.GetApp().auiManager
		self.Check( auiManager.GetPane(self.pane).IsShown() )

	def PaneClosed(self, pane):
		if pane is self.pane:
			self.Check( False )
	
	def Execute(self):
		self.Toggle()
		auiManager = wx.GetApp().auiManager
		auiManager.GetPane(self.pane).Show( self.IsChecked() )
		auiManager.Update()
