#!/usr/bin/env python
import os.path
import wx

from csp.tools.layout2.scripts.document.OutputDocument import OutputDocument

class CommandControlFactory:
	"""A factory that creates ui controls that is bound to command objects."""
	
	def __init__(self, controlIdGenerator):
		""" Constructs the control factory with a control id generator. The
		generator is needed in order to create a unique id for each command
		in the scope of the parent control."""
		self.controlIdGenerator = controlIdGenerator

	def GenerateMenuItems(self, parent, commands):
		"""Creates a menu with all the commands specified in the commands array.
		A None value in the array means that we want to add a separator."""
		menu = wx.Menu()
		for command in commands:
			# Add a separator if the command is equal to None
			if command is None:
				menu.AppendSeparator()
				continue
			instance = command()

			controlId = self.controlIdGenerator.Generate()
			menu.Append(controlId, instance.GetCaption())

			wx.EVT_MENU(parent, controlId, EventToCommandExecutionAdapter(instance).Execute)
		return menu

	def GenerateToolBarButtons(self, parent, toolbar, commands):
		"""Creates a toolbar with all the commands specified in the commands array.
		A None value in the array means that we want to add a separator."""
		for command in commands:
			# Add a separator if the command is equal to None
			if command is None:
				toolbar.AddSeparator()
				continue

			instance = command()
			bitmap1 = wx.BitmapFromImage(wx.Image(os.path.join("images", instance.GetToolBarImageName())))
			bitmap2 = wx.BitmapFromImage(wx.Image(os.path.join("images", instance.GetToolBarImageName())))

			controlId = self.controlIdGenerator.Generate()
			toolbar.AddTool(controlId, bitmap1, bitmap2, wx.ITEM_NORMAL, instance.GetCaption(), instance.GetToolTipText())
			wx.EVT_TOOL(parent, controlId, EventToCommandExecutionAdapter(instance).Execute)
		
		toolbar.Realize()

class EventToCommandExecutionAdapter:
	"""This class can be bound to a wx event (click on a menuitem, button, toolbar
	button etc). When the event is fired the command sent to the constructor is 
	executed."""

	def __init__(self, command):
		"""Constructs this instance with a command object. This command will be
		executed when the bound event is fired."""
		self.command = command

	def Execute(self, event):
		"""Bind this method to a wx event. When the event is fired the command will
		be executed."""
		
		# If possible we will print the command execution to the output document.
		# We will also assign the output document to the command so the command can
		# be able the print messages to the console.
		application = wx.GetApp()
		if application is not None:
			documentRegistry = application.GetDocumentRegistry()
			documentName = 'Command history'

			# Get the output document. If it doesn't exist we will create it and
			# add it to the list of current documents.
			outputDocument = documentRegistry.GetByName(documentName)
			if outputDocument is None:
				outputDocument = OutputDocument(documentName)
				documentRegistry.Add(outputDocument)

			self.command.SetOutputDocument(outputDocument)
			outputDocument.WriteLine("Executing: " + self.command.__class__.__name__)
		
		# Execute the command.
		self.command.Execute()
		event.Skip()