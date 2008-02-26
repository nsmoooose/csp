#!/usr/bin/env python
import wx
import wx.richtext

from csp.tools.layout2.scripts.document.SceneDocument import SceneDocument

from ControlIdGenerator import ControlIdGenerator
from CommandControlFactory import CommandControlFactory
from controls.OutputWindow import OutputWindow
from controls.ProjectTree import ProjectTree
from controls.SceneWindow import SceneWindow

from commands.MoveCameraToHomeCommand import MoveCameraToHomeCommand
from commands.OpenSelectedFileCommand import OpenSelectedFileCommand
from commands.QuitCommand import QuitCommand
from commands.ReCompileDataArchiveCommand import ReCompileDataArchiveCommand

class MainFrame(wx.Frame):
	"""This is the top window that contains all controls used by the layout editor.
	It contains the main menu, toolbar, tree control and the rendered scene.
	
	It also receives the idle event wich is responsible for calling the rendering
	method of the GLCanvas control."""
	
	def __init__(self, parent, id, title):
		# First, call the base class' __init__ method to create the frame
		wx.Frame.__init__(self, parent, id, title)

		# Get the application object. This object is used to retreive the
		# configuration object.
		application = wx.GetApp()
		
		documentRegistry = application.GetDocumentRegistry()
		documentRegistry.GetDocumentAddedSignal().Connect(self.documentAdded_Signal)
		
		# Create a control id generator. This is used by all helper classes
		# to actualy define an unique id for each control (menu, toolbar button)
		# etc.
		self.controlIdGenerator = ControlIdGenerator()

		# Declare a class that is responsible for creating instances of
		# menu items, toolbar buttons and keyboard shortcuts to command
		# objects.
		controlFactory = CommandControlFactory(self.controlIdGenerator)

		fileMenuCommands = [QuitCommand]
		viewMenuCommands = [MoveCameraToHomeCommand]
		toolsMenuCommands = [ReCompileDataArchiveCommand]
		toolbarCommands = [OpenSelectedFileCommand, MoveCameraToHomeCommand, ReCompileDataArchiveCommand]

		# Menu items.
		menuBar = wx.MenuBar()
		menuBar.Append(controlFactory.GenerateMenuItems(self, fileMenuCommands), "File")
		menuBar.Append(controlFactory.GenerateMenuItems(self, viewMenuCommands), "View")
		menuBar.Append(controlFactory.GenerateMenuItems(self, toolsMenuCommands), "Tools")
		self.SetMenuBar(menuBar)

		# Create the toolbar
		toolbar = self.CreateToolBar(wx.TB_HORIZONTAL|wx.SUNKEN_BORDER|wx.TB_TEXT|wx.TB_3DBUTTONS)
		toolbar.SetToolBitmapSize((32,32))
		toolbar.SetMargins((8,8))
		controlFactory.GenerateToolBarButtons(self, toolbar, toolbarCommands)

		# Status bar control at the bottom of the screen.
		statusBar = wx.StatusBar(self)
		self.SetStatusBar(statusBar)

		# Divides the document area from the properties control.
		# To the left we have a project, properties and that type
		# of controls. To the right we have all opened documents.
		# And at the bottom we have the output panel.
		splitter1 = wx.SplitterWindow(self, wx.ID_ANY)
		propertyNotebook = wx.Notebook(splitter1, wx.ID_ANY, style=wx.NB_TOP)
		splitter2 = wx.SplitterWindow(splitter1, wx.ID_ANY)
		outputNotebook = wx.Notebook(splitter2, wx.ID_ANY, style=wx.NB_TOP)
		documentNotebook = wx.Notebook(splitter2, wx.ID_ANY, style=wx.NB_TOP)
		splitter1.SplitVertically(propertyNotebook, splitter2, 200)
		splitter2.SetSashGravity(1.0)
		splitter2.SplitHorizontally(documentNotebook, outputNotebook, -100)

		# We need to insert the default output window that is used
		# to display any kind of text from commands executed.
		outputPage = OutputWindow(outputNotebook, wx.ID_ANY)
		outputPage.ConnectToDocument(application.GetDocumentRegistry().GetByName('output'))
		outputNotebook.AddPage(outputPage, "Output")

		# Add the first document to this window.
		startPage = wx.richtext.RichTextCtrl(documentNotebook)
		startPage.LoadFile("start.txt")
		startPage.SetEditable(False)
		documentNotebook.AddPage(startPage, "Start")

		# Display the tree control with all files in this project. Also
		# register this instance so all command object can use it to 
		# access the selected file or directory.
		projectTreePage = ProjectTree(propertyNotebook)
		projectTreePage.SetRootDirectory(application.Configuration['LayoutApplication.DataDirectory'])
		projectTreePage.SetOpenCommand(OpenSelectedFileCommand)
		ProjectTree.Instance = projectTreePage

		propertyNotebook.AddPage(projectTreePage, "Project")
		
		# Store some variables for later use
		self.documentNotebook = documentNotebook
		self.scene = None

		# Connect idle event.
		wx.EVT_IDLE(self, self.on_Idle)

	def on_Idle(self, event):
		# Only render when the scene is visible on screen.
		if self.scene is not None and self.scene.IsShownOnScreen():
			self.scene.Frame()
			event.RequestMore()
			
	def documentAdded_Signal(self, document):
		# Add the scene control that is responsible for the 3D
		# scene.
		if isinstance(document, SceneDocument):
			scenePage = SceneWindow(self.documentNotebook, wx.ID_ANY, wx.DefaultPosition, wx.DefaultSize, wx.BORDER_NONE, "GLCanvas", 0, wx.NullPalette)
			scenePage.SetDocument(document)
			self.documentNotebook.AddPage(scenePage, "Scene")
			self.scene = scenePage

	def GetSceneWindow(self):
		return self.scene