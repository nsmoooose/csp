import wx
import wx.aui

from .CommandControlFactory import CommandControlFactory
from .controls.DocumentNotebook import DocumentNotebook
from .controls.OutputPane import OutputPane
from .controls.ProjectTree import ProjectTree
from .controls.CurrentDocumentPropertiesPane import CurrentDocumentPropertiesPane
from .controls.SceneWindow import SceneWindow

from .commands import *

class MainFrame(wx.Frame):
	"""This is the top window that contains all controls used by the layout editor.
	It contains the main menu, toolbar, tree control and the rendered scene.

	It also receives the idle event wich is responsible for calling the rendering
	method of the GLCanvas control."""

	def __init__(self, *args, **kwargs):
		# First, call the base class' __init__ method to create the frame
		wx.Frame.__init__(self, *args, **kwargs)

		# Get the application object. This object is used to retreive the
		# configuration object.
		application = wx.GetApp()

		# Let the advanced user interface (aui) manage the MainFrame
		application.auiManager = wx.aui.AuiManager(self)

		# Declare a class that is responsible for creating instances of
		# menu items, toolbar buttons and keyboard shortcuts to command
		# objects.
		controlFactory = CommandControlFactory(application.controlMediator)

		fileMenuCommands = [
			OpenSelectedFileCommand,
			CloseCurrentDocumentCommand,
			SaveCurrentDocumentCommand,
			SaveAllDocumentsCommand,
			None,
			QuitCommand ]
		editMenuCommands = [
			ShowActionHistoryCommand,
			UndoCommand,
			RedoCommand ]
		viewMenuCommands = [
			MoveCameraToHomeCommand,
			None,
			ZoomInCommand,
			ZoomOutCommand,
			None,
			PanDownCommand,
			PanLeftCommand,
			PanRightCommand,
			PanUpCommand ]
		toolsMenuCommands = [
			ReCompileDataArchiveCommand,
			CreateModelXmlFileCommand,
			CreateInterfaceInformationDocumentCommand ]
		toolbarCommands = [
			OpenSelectedFileCommand,
			SaveCurrentDocumentCommand,
			CloseCurrentDocumentCommand,
			None,
			UndoCommand,
			RedoCommand,
			ShowActionHistoryCommand,
			None,
			MoveCameraToHomeCommand,
			ReCompileDataArchiveCommand,
			None,
			ZoomInCommand,
			ZoomOutCommand,
			None,
			PanDownCommand,
			PanLeftCommand,
			PanRightCommand,
			PanUpCommand ]

		# Menu items.
		menuBar = wx.MenuBar()
		self.SetMenuBar(menuBar)
		menuBar.Append(controlFactory.CreateMenuItems(self, fileMenuCommands, controlid={ QuitCommand : wx.ID_EXIT }), "File")
		menuBar.Append(controlFactory.CreateMenuItems(self, editMenuCommands), "Edit")
		menuBar.Append(controlFactory.CreateMenuItems(self, viewMenuCommands), "View")
		menuBar.Append(controlFactory.CreateMenuItems(self, toolsMenuCommands), "Tools")

		# Create the toolbar
		toolbar = wx.ToolBar(self, style=wx.TB_FLAT | wx.TB_NODIVIDER)
		toolbar.SetToolBitmapSize((32,32))
		controlFactory.CreateToolBarButtons(self, toolbar, toolbarCommands)
		application.auiManager.AddPane( toolbar,
			wx.aui.AuiPaneInfo().Name('Toolbar')
			.Caption('General toolbar').ToolbarPane().Top() )

		# Status bar control at the bottom of the screen.
		statusBar = wx.StatusBar(self)
		self.SetStatusBar(statusBar)

		# Divides the document area from the properties control.
		# To the left we have a project, properties and that type
		# of controls. To the center we have all opened documents.
		# And at the bottom we have the output panel.
		DocumentNotebook.Instance = DocumentNotebook(self)
		application.auiManager.AddPane( DocumentNotebook.Instance,
			wx.aui.AuiPaneInfo().Name('Documents').CenterPane() )

		# Display the tree control with all files in this project. Also
		# register this instance so all command object can use it to
		# access the selected file or directory.
		ProjectTree.Instance = ProjectTree(self)
		ProjectTree.Instance.SetRootDirectory(application.Configuration['LayoutApplication.DataDirectory'])
		ProjectTree.Instance.SetOpenCommand( OpenSelectedFileCommand )
		ProjectTree.Instance.SetContextCommands([None], [RefreshDirectoryCommand, CloudBoxWizardCommand])

		application.auiManager.AddPane( ProjectTree.Instance,
			wx.aui.AuiPaneInfo().Name('Project')
			.Caption('Project').Layer(1).Left().BestSize((300, 300)) )

		CurrentDocumentPropertiesPane.Instance = CurrentDocumentPropertiesPane(self)
		application.auiManager.AddPane( CurrentDocumentPropertiesPane.Instance,
			wx.aui.AuiPaneInfo().Name('CurrentDocumentPropertiesPane')
			.Caption('Document properties').Layer(1).Left().BestSize((300, 300)) )

		OutputPane.Instance = OutputPane(self)
		application.auiManager.AddPane( OutputPane.Instance,
			wx.aui.AuiPaneInfo().Name('Output')
			.Caption('Output').Bottom().BestSize((300, 300)) )

		# Create the Windows menu and toolbar.
		windowsToolbarCommands = [
			ToggleGeneralToolbarPaneCommand,
			ToggleProjectTreePaneCommand,
			TogglePropertiesPaneCommand,
			ToggleOutputPaneCommand ]

		toolbar = wx.ToolBar(self, style=wx.TB_FLAT | wx.TB_NODIVIDER)
		toolbar.SetToolBitmapSize((32,32))
		controlFactory.CreateToolBarButtons(self, toolbar, windowsToolbarCommands)
		application.auiManager.AddPane( toolbar,
			wx.aui.AuiPaneInfo().Name('Windows')
			.Caption('Windows toolbar').ToolbarPane().Top() )

		application.defaultPerspective = application.auiManager.SavePerspective()
		windowsMenuCommands = [
			DefaultLayoutCommand,
			None,
			ToggleWindowToolbarPaneCommand ] + windowsToolbarCommands
		menuBar.Append(controlFactory.CreateMenuItems(self, windowsMenuCommands), "Windows")

		# Update the layout
		currentPerspective = application.Configuration.get('LayoutApplication.auiManager.currentPerspective')
		if currentPerspective is not None:
			application.auiManager.LoadPerspective(currentPerspective, update = False)
		application.auiManager.Update()

		# Connect idle event.
		wx.EVT_IDLE(self, self.on_Idle)
		self.Bind(wx.EVT_CLOSE, self.on_Close)

		application.UpdateUI()

	def on_Idle(self, event):
		application = wx.GetApp()
		application.GetIdleSignal().Emit(event)

	def on_Close(self, event):
		application = wx.GetApp()

		# Save the current layout
		currentPerspective = application.auiManager.SavePerspective()
		application.Configuration['LayoutApplication.auiManager.currentPerspective'] = currentPerspective
		if not self.IsMaximized():
			application.Configuration['LayoutApplication.MainFrame.position'] = self.GetPosition()
			application.Configuration['LayoutApplication.MainFrame.size'] = self.GetSize()
		application.Configuration['LayoutApplication.MainFrame.IsMaximized'] = self.IsMaximized()

		# Close all opened documents
		DocumentNotebook.Instance.CloseAllPages()

		# Free the advanced user interface (aui) from managing the MainFrame
		application.auiManager.UnInit()

		# Destroy the window
		self.Destroy()
