#!/usr/bin/env python
import wx
import os

class SelectDataDirectoryDialog(wx.Dialog):
	"""A dialog to be used to display a splash window for 
	this layout tool. You must also select a valid data
	directory for this application to work with. After 
	you press OK the main frame is loaded and you can begin
	to edit your layout of CSP."""

	def __init__(self, parent, id, title):
		wx.Dialog.__init__(self, parent, id, title, style=wx.DEFAULT_DIALOG_STYLE|wx.RESIZE_BORDER)

		horizontalSizer = wx.BoxSizer(wx.HORIZONTAL)

		bitmap = wx.Bitmap(os.path.join('images', 'splash.png'))
		bitmapControl = wx.StaticBitmap(self, wx.ID_ANY, bitmap)
		horizontalSizer.Add(bitmapControl, flag=wx.RIGHT, border=10)

		rightPanel = wx.Panel(self, wx.ID_ANY)
		horizontalSizer.Add(rightPanel, flag=wx.TOP|wx.BOTTOM|wx.RIGHT|wx.EXPAND, border=10, proportion=1)

		verticalSizer = wx.BoxSizer(wx.VERTICAL)
		cspLabel = wx.StaticText(rightPanel, wx.ID_ANY, 'Combat Simulator Project')
		verticalSizer.Add(cspLabel)

		infoLabel = wx.StaticText(rightPanel, wx.ID_ANY, 'Select the data directory of your CSP installation.')
		verticalSizer.Add(infoLabel)


		directoryLabel = wx.StaticText(rightPanel, wx.ID_ANY, 'Directory:')
		verticalSizer.Add(directoryLabel, border=5, flag=wx.TOP)

		directoryPanel = wx.Panel(rightPanel, wx.ID_ANY)
		directoryText = wx.TextCtrl(directoryPanel, wx.ID_ANY, '')
		directoryText.SetMinSize(wx.Size(300, directoryText.GetSize().GetHeight()))
		browseDirectoryButton = wx.Button(directoryPanel, 6666, '...', size=wx.Size(20,20))
		directorySizer = wx.BoxSizer(wx.HORIZONTAL)
		directorySizer.Add(directoryText, flag=wx.RIGHT, border=5)
		directorySizer.Add(browseDirectoryButton, flag=wx.ALIGN_RIGHT)
		directoryPanel.SetSizer(directorySizer)
		verticalSizer.Add(directoryPanel, flag=wx.EXPAND, proportion=1)

		verticalSizer.AddStretchSpacer()

		buttonPanel = wx.Panel(rightPanel, wx.ID_ANY)
		okButton = wx.Button(buttonPanel, wx.ID_OK, 'OK')
		cancelButton = wx.Button(buttonPanel, wx.ID_CANCEL, 'Cancel')
		buttonSizer = wx.BoxSizer(wx.HORIZONTAL)
		buttonSizer.Add(okButton, border=5, flag=wx.RIGHT)
		buttonSizer.Add(cancelButton)
		buttonPanel.SetSizer(buttonSizer)
		verticalSizer.Add(buttonPanel, border=5, flag=wx.TOP)

		rightPanel.SetSizer(verticalSizer)

		# Force the window to resize itself according to its content.
		horizontalSizer.SetSizeHints(self)

		self.SetSizer(horizontalSizer)

		# Store a reference to the text control containing
		# the selected directory.
		self.directoryText = directoryText
		application = wx.GetApp()
		self.directoryText.SetValue(application.Configuration.get('LayoutApplication.DataDirectory', '.'))
		
		# Bind events to buttons
		wx.EVT_BUTTON(self, wx.ID_OK, self.okButton_Click)
		wx.EVT_BUTTON(self, 6666, self.browseDirectoryButton_Click)

		okButton.SetFocus()
		
		self.SetWindowStyle(wx.DEFAULT_DIALOG_STYLE)

	def browseDirectoryButton_Click(self, event):
		# Use the current value in the text box. This will
		# be the default directory in the directory browser.
		defaultDataDirectory = self.directoryText.GetValue()
		
		# Show the directory browser.
		directoryBrowser = wx.DirDialog(None, 'Select the data directory for your csp application', defaultDataDirectory)
		if directoryBrowser.ShowModal() != wx.ID_OK:
			return False

		# Set the choosen directory into the text box.
		self.directoryText.SetValue(directoryBrowser.GetPath())

	def okButton_Click(self, event):
		dataDirectory = self.directoryText.GetValue().encode('utf8')
		application = wx.GetApp()
		application.Configuration['LayoutApplication.DataDirectory'] = dataDirectory
		application.Configuration['LayoutApplication.XmlPath'] = os.path.join(dataDirectory, 'xml')
		self.EndModal(wx.ID_OK)