import wx

from Command import Command

class TogglePaneCommand(Command):
	toolbarimage = "generic"
	panename = "No name set"

	def Execute(self):
		auiManager = wx.GetApp().auiManager
		pane = auiManager.GetPane(self.panename)
		pane.Show(not pane.IsShown())
		auiManager.Update()

class ToggleWindowToolbarPaneCommand(TogglePaneCommand):
	caption = "Toggle window toolbar"
	tooltip = "Toggle the visibility of the window toolbar"
	panename = "Windows"

class ToggleGeneralToolbarPaneCommand(TogglePaneCommand):
	caption = "Toggle general toolbar"
	tooltip = "Toggle the visibility of the general toolbar"
	panename = "Toolbar"

class ToggleProjectTreePaneCommand(TogglePaneCommand):
	caption = "Toggle project tree"
	tooltip = "Toggle the visibility of the project tree window"
	panename = "Project"

class TogglePropertiesPaneCommand(TogglePaneCommand):
	caption = "Toggle properties"
	tooltip = "Toggle the visibility of the properties window"
	panename = "CurrentDocumentPropertiesPane"

class ToggleOutputPaneCommand(TogglePaneCommand):
	caption = "Toggle output"
	tooltip = "Toggle the visibility of the output window"
	panename = "Output"
