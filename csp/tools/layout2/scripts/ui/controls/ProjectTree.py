#!/usr/bin/env python
import os
import wx

class ProjectTree(wx.TreeCtrl):
	"""A tree control that provides information about all files
	within the project directory. All folders and files above
	the root directory will be hidden."""

	Instance = None
	
	def __init__(self, parent):
		wx.TreeCtrl.__init__(self, parent, style=wx.TR_HIDE_ROOT|wx.TR_HAS_BUTTONS|wx.TR_LINES_AT_ROOT)
		
		self.Bind(wx.EVT_TREE_ITEM_ACTIVATED, self.on_ItemActivated)

	def SetRootDirectory(self, directory):
		""" Sets the root directory that the tree should display. All subfolders
		and files of this directory will be displayed within the tree."""
		self.DeleteAllItems()

		# Remember the base directory so we can return 
		# the selected file or folder.
		self.baseDirectory = directory

		self.nodeDictionary = {}
		# Add the base node that is the parent to all 
		# nodes.
		self.nodeDictionary[self.baseDirectory] = self.AddRoot('CSP')

		for root, dirs, files in os.walk(directory):
			parentNode = self.nodeDictionary[root]
			if '.svn' in dirs:
				dirs.remove('.svn')
			if '_svn' in dirs:
				dirs.remove('_svn')
				
			dirs.sort()
			files.sort()
			for subDirectory in dirs:
				newNode = self.AppendItem(parentNode, subDirectory)
				self.nodeDictionary[os.path.join(root, subDirectory)] = newNode
			for file in files:
				if file.lower().endswith('.pyc'):
					continue
				self.AppendItem(parentNode, file)

	def GetSelectedFile(self):
		node = self.GetSelection()
		if node.IsOk():
			items = []
			while node != self.GetRootItem():
				items.insert(0, self.GetItemText(node))
				node = self.GetItemParent(node)
			
			selection = self.baseDirectory
			for item in items:
				selection = os.path.join(selection, item)

			# Sometimes this will be unicode strings. We avoid this
			# by converting back to strings. The reason is that our
			# current swig binding doesn't handle the unicode
			# character set.
			return str(selection)
	
	def SetOpenCommand(self, command):
		self.openCommand = command

	def on_ItemActivated(self, event):
		if self.openCommand is None:
			return

		if os.path.isfile(self.GetSelectedFile()):
			command = self.openCommand()
			command.Execute()
