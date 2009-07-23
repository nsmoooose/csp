#!/usr/bin/env python
import wx

from controls.OutputPane import OutputPane

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
			
			controlId = command.GetControlId()
			if controlId is None:
				controlId = self.controlIdGenerator.Generate()
			
			command.AppendInMenu(parent, menu, controlId)
		return menu

	def GenerateToolBarButtons(self, parent, toolbar, commands):
		"""Creates a toolbar with all the commands specified in the commands array.
		A None value in the array means that we want to add a separator."""
		for command in commands:
			# Add a separator if the command is equal to None
			if command is None:
				toolbar.AddSeparator()
				continue
			
			controlId = command.GetControlId()
			if controlId is None:
				controlId = self.controlIdGenerator.Generate()
			
			command.AppendInToolBar(parent, toolbar, controlId)
		
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
		
		# If possible we will print the command execution to the output pane.
		if OutputPane.Instance is not None:
			OutputPane.Instance.AppendLine("Executing: %s" % self.command.__class__.__name__)
		
		# Execute the command.
		self.command.Execute()
