import wx

from controls.OutputPane import OutputPane

class CommandControlMediator(object):
	"""The purpose of this class is to hold a binding between a command
	and several UI controls. This class is responsible for enabling and
	disabling UI controls depending on if commands are available or not.

	Call the apropriate ObserveXxxYyy functions in this class to make
	it observe for changes in documents, controls etc.
	"""

	def __init__(self):
		self.commands = {}

	def ObserveDocumentRegistry(self, registry):
		"""Attaches this object to necesarry events that the document
		registry publishes. Whenever a document is changed, current doc
		changed etc we will update necessarry controls."""
		registry.GetCurrentDocumentChangedSignal().Connect(self.on_Update)
		registry.GetDocumentAddedSignal().Connect(self.on_DocumentAdded)

	def on_Update(self, subject):
		self.UpdateControls()

	def on_DocumentAdded(self, document):
		document.GetChangedSignal().Connect(self.on_Update)
		document.actionHistory.GetChangedSignal().Connect(self.on_Update)

	def AddControl(self, command, control):
		"""First argument is the command we wish to remember a binding
		for. Second argument is a tuple of (parent control, control id, control)"""
		if not self.commands.has_key(command):
			self.commands[command] = []
		self.commands[command].append(control)

	def UpdateControls(self):
		"""Iterates all command objects and tests to see if they are
		enabled or not. Example: if there is no documents opened the
		SaveCurrentDocumentCommand should be disabled in toolbar and
		menus."""
		for command, controls in self.commands.iteritems():
			enabled = command.Enabled()
			for parent, id, control in controls:
				if control.GetClassName() == "wxToolBarToolBase":
					# Toolbar controls must be disabled and
					# enabled using the toolbar control in order
					# to get the grayed image when disabled.
					if parent.GetToolEnabled(id) != enabled:
						parent.EnableTool(id, enabled)
				else:
					if control.IsEnabled() != enabled:
						control.Enable(enabled)


class CommandControlFactory(object):
	"""A factory that creates ui controls that is bound to command objects."""

	def __init__(self, control_mediator):
		self.control_mediator = control_mediator

	def CreateMenuItems(self, parent, commands, controlid={}):
		"""Creates a menu with all the commands specified in the commands array.
		A None value in the array means that we want to add a separator."""
		menu = wx.Menu()
		for command in commands:
			# Add a separator if the command is equal to None
			if command is None:
				menu.AppendSeparator()
				continue

			controlId = wx.NewId() if not controlid.has_key(command) else controlid[command]
			control = self.AppendInMenu(parent, menu, controlId, command)
			self.control_mediator.AddControl(command, (menu, controlId, control))
		return menu

	def AppendInMenu(self, parent, menu, controlId, command):
		menuItem = menu.Append(id = controlId, text = command.caption, help = command.tooltip, kind = wx.ITEM_NORMAL)
		imageName = command.toolbarimage
		if imageName:
			bitmap = wx.ArtProvider.GetBitmap(imageName, client = wx.ART_MENU, size = (16, 16))
			menuItem.SetBitmap(bitmap)
		parent.Bind(wx.EVT_MENU, EventToCommandExecutionAdapter(command).Execute, menuItem)
		return menuItem

	def CreateToolBarButtons(self, parent, toolbar, commands, controlid={}):
		"""Creates a toolbar with all the commands specified in the commands array.
		A None value in the array means that we want to add a separator."""
		for command in commands:
			# Add a separator if the command is equal to None
			if command is None:
				toolbar.AddSeparator()
				continue

			controlId = wx.NewId() if not controlid.has_key(command) else controlid[command]

			control = self.AppendInToolBar(parent, toolbar, controlId, command)
			self.control_mediator.AddControl(command, (toolbar, controlId, control))

		toolbar.Realize()

	def AppendInToolBar(self, parent, toolbar, controlId, command):
		imageName = command.toolbarimage
		if not imageName:
			imageName = 'generic'
		bitmap = wx.ArtProvider.GetBitmap(imageName, client = wx.ART_TOOLBAR, size = toolbar.GetToolBitmapSize())
		tool = toolbar.AddLabelTool( id = controlId, label = command.caption, bitmap = bitmap, bmpDisabled = wx.NullBitmap, kind = wx.ITEM_NORMAL, shortHelp = command.tooltip, longHelp = command.tooltip )
		parent.Bind(wx.EVT_TOOL, EventToCommandExecutionAdapter(command).Execute, tool)
		return tool

class EventToCommandExecutionAdapter(object):
	"""This class can be bound to a wx event (click on a menuitem, button, toolbar
	button etc). When the event is fired the command sent to the constructor is
	executed."""

	def __init__(self, command):
		"""Constructs this instance with a command object. This command will be
		executed when the bound event is fired."""
		if not isinstance(command, type):
			raise Exception, "The EventToCommandExecutionAdapter takes the command type as parameter. Not an instance. %s" % command.__class__.__name__
		self.command = command

	def Execute(self, event):
		"""Bind this method to a wx event. When the event is fired the command will
		be executed."""

		# If possible we will print the command execution to the output pane.
		if OutputPane.Instance is not None:
			OutputPane.Instance.AppendLine("Executing: %s" % self.command.__name__)

		# Execute the command.
		instance = self.command()
		instance.Execute()
