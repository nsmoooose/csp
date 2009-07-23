#!/usr/bin/env python
import wx
import wx.aui

from csp.tools.layout2.scripts.document.SceneDocument import SceneDocument

from ControlIdGenerator import ControlIdGenerator
from CommandControlFactory import CommandControlFactory
from controls.DocumentNotebook import DocumentNotebook
from controls.OutputPane import OutputPane
from controls.ProjectTree import ProjectTree
from controls.SceneWindow import SceneWindow

from commands.CloseCurrentDocumentCommand import CloseCurrentDocumentCommand
from commands.MoveCameraToHomeCommand import MoveCameraToHomeCommand
from commands.OpenSelectedFileCommand import OpenSelectedFileCommand
from commands.QuitCommand import QuitCommand
from commands.ReCompileDataArchiveCommand import ReCompileDataArchiveCommand
from commands.SaveAllDocumentsCommand import SaveAllDocumentsCommand
from commands.SaveCurrentDocumentCommand import SaveCurrentDocumentCommand

from commands.PanDownCommand import PanDownCommand
from commands.PanLeftCommand import PanLeftCommand
from commands.PanRightCommand import PanRightCommand
from commands.PanUpCommand import PanUpCommand
from commands.ZoomInCommand import ZoomInCommand
from commands.ZoomOutCommand import ZoomOutCommand

from commands.CreateModelXmlFileCommand import CreateModelXmlFileCommand
from commands.CreateInterfaceInformationDocumentCommand import CreateInterfaceInformationDocumentCommand

from commands.TogglePaneCommand import TogglePaneCommand
from commands.DefaultLayoutCommand import DefaultLayoutCommand

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
		
		documentRegistry = application.GetDocumentRegistry()
		
		# Create a control id generator. This is used by all helper classes
		# to actualy define an unique id for each control (menu, toolbar button)
		# etc.
		self.controlIdGenerator = ControlIdGenerator()

		# Declare a class that is responsible for creating instances of
		# menu items, toolbar buttons and keyboard shortcuts to command
		# objects.
		controlFactory = CommandControlFactory(self.controlIdGenerator)

		fileMenuCommands = [OpenSelectedFileCommand(), CloseCurrentDocumentCommand(), SaveCurrentDocumentCommand(), SaveAllDocumentsCommand(), None, QuitCommand()]
		viewMenuCommands = [MoveCameraToHomeCommand(), None, ZoomInCommand(), ZoomOutCommand(), None, PanDownCommand(), PanLeftCommand(), PanRightCommand(), PanUpCommand()]
		toolsMenuCommands = [ReCompileDataArchiveCommand(), CreateModelXmlFileCommand(), CreateInterfaceInformationDocumentCommand()]
		toolbarCommands = [OpenSelectedFileCommand(), SaveCurrentDocumentCommand(), CloseCurrentDocumentCommand(), None, MoveCameraToHomeCommand(), ReCompileDataArchiveCommand(), None, ZoomInCommand(), ZoomOutCommand(), None, PanDownCommand(), PanLeftCommand(), PanRightCommand(), PanUpCommand()]

		# Menu items.
		menuBar = wx.MenuBar()
		self.SetMenuBar(menuBar)
		menuBar.Append(controlFactory.GenerateMenuItems(self, fileMenuCommands), "File")
		menuBar.Append(controlFactory.GenerateMenuItems(self, viewMenuCommands), "View")
		menuBar.Append(controlFactory.GenerateMenuItems(self, toolsMenuCommands), "Tools")

		# Create the toolbar
		toolbar = wx.ToolBar(self, style=wx.TB_FLAT | wx.TB_NODIVIDER)
		toolbar.SetToolBitmapSize((32,32))
		controlFactory.GenerateToolBarButtons(self, toolbar, toolbarCommands)
		application.auiManager.AddPane( toolbar, wx.aui.AuiPaneInfo().Name('Toolbar').Caption('General toolbar').ToolbarPane().Top() )

		# Status bar control at the bottom of the screen.
		statusBar = wx.StatusBar(self)
		self.SetStatusBar(statusBar)

		# Divides the document area from the properties control.
		# To the left we have a project, properties and that type
		# of controls. To the center we have all opened documents.
		# And at the bottom we have the output panel.
		DocumentNotebook.Instance = DocumentNotebook(self)
		application.auiManager.AddPane( DocumentNotebook.Instance, wx.aui.AuiPaneInfo().Name('Documents').CenterPane() )

		# Display the tree control with all files in this project. Also
		# register this instance so all command object can use it to 
		# access the selected file or directory.
		ProjectTree.Instance = ProjectTree(self)
		ProjectTree.Instance.SetRootDirectory(application.Configuration['LayoutApplication.DataDirectory'])
		ProjectTree.Instance.SetOpenCommand( OpenSelectedFileCommand() )

		application.auiManager.AddPane( ProjectTree.Instance, wx.aui.AuiPaneInfo().Name('Project').Caption('Project').Left().BestSize((300, 300)) )
		
		OutputPane.Instance = OutputPane(self)
		application.auiManager.AddPane( OutputPane.Instance, wx.aui.AuiPaneInfo().Name('Output').Caption('Output').Bottom().BestSize((300, 300)) )
		
		# Create the Windows menu and toolbar.
		windowsToolbarCommands = [TogglePaneCommand(toolbar), TogglePaneCommand(ProjectTree.Instance), TogglePaneCommand(OutputPane.Instance)]
		
		toolbar = wx.ToolBar(self, style=wx.TB_FLAT | wx.TB_NODIVIDER)
		toolbar.SetToolBitmapSize((32,32))
		controlFactory.GenerateToolBarButtons(self, toolbar, windowsToolbarCommands)
		application.auiManager.AddPane( toolbar, wx.aui.AuiPaneInfo().Name('Windows').Caption('Windows toolbar').ToolbarPane().Top() )
		
		defaultPerspective = application.auiManager.SavePerspective()
		windowsMenuCommands = [DefaultLayoutCommand(defaultPerspective), None, TogglePaneCommand(toolbar)] + windowsToolbarCommands
		menuBar.Append(controlFactory.GenerateMenuItems(self, windowsMenuCommands), "Windows")
		
		# Update the layout
		currentPerspective = application.Configuration.get('LayoutApplication.auiManager.currentPerspective')
		if currentPerspective is not None:
			application.auiManager.LoadPerspective(currentPerspective, update = False)
			TogglePaneCommand.UpdateAllPanes()
		application.auiManager.Update()
		
		# Connect idle event.
		wx.EVT_IDLE(self, self.on_Idle)
		self.Bind(wx.aui.EVT_AUI_PANE_CLOSE, self.on_PaneClose)
		self.Bind(wx.EVT_CLOSE, self.on_Close)

	def on_Idle(self, event):
		application = wx.GetApp()
		application.GetIdleSignal().Emit(event)

	def on_PaneClose(self, event):
		TogglePaneCommand.on_PaneClose(event.GetPane().window)

	def on_Close(self, event):
		application = wx.GetApp()
		
		# Save the current layout
		currentPerspective = application.auiManager.SavePerspective()
		application.Configuration['LayoutApplication.auiManager.currentPerspective'] = currentPerspective
		
		documentRegistry = application.GetDocumentRegistry()
		
		# Clone the document list and iterate over each document
		# to close it. This triggers document closed signal which
		# forces all signals to be disconnected from their ui
		# widgets.
		documents = documentRegistry.GetDocuments()[:]
		for document in documents:
			documentRegistry.Close(document)
		
		# Free the advanced user interface (aui) from managing the MainFrame
		application.auiManager.UnInit()
		
		# Destroy the window
		self.Destroy()
