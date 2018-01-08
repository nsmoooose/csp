import wx

from .Command import Command

class ShowActionHistoryCommand(Command):
	"""Show the action history."""

	caption = "Action history"
	tooltip = "Show the action history"
	toolbarimage = "generic"

	def Execute(self):
		documentRegistry = wx.GetApp().GetDocumentRegistry()
		currentDocument = documentRegistry.GetCurrentDocument()
		title = "Action history of %s" % currentDocument.actionHistory.documentOwner.GetName()
		actionHistoryDialog = ActionHistoryDialog( wx.GetApp().GetTopWindow(), title = title, actionHistory = currentDocument.actionHistory )
		actionHistoryDialog.ShowModal()
		actionHistoryDialog.Destroy()

	@staticmethod
	def Enabled():
		documentRegistry = wx.GetApp().GetDocumentRegistry()
		currentDocument = documentRegistry.GetCurrentDocument()
		if currentDocument is None:
			return False
		return currentDocument.actionHistory.HasAction()

class ActionHistoryDialog(wx.Dialog):
	imageListSize = (16, 16)
	actionColumn = 0
	descriptionColumn = 1
	maskedColor = wx.Colour(127, 127, 127)

	def __init__(self, parent, actionHistory, *args, **kwargs):
		wx.Dialog.__init__(self, parent, style = wx.DEFAULT_DIALOG_STYLE | wx.RESIZE_BORDER, *args, **kwargs)

		self.actionHistory = actionHistory

		sizer = wx.BoxSizer(wx.VERTICAL)
		self.SetSizer(sizer)

		self.listView = wx.ListView(self)
		sizer.AddF( self.listView, wx.SizerFlags().Expand().Proportion(1).Border(wx.ALL, 5) )

		self.InitListView()
		self.FillListView()

		sizer.Add( self.CreateButtonSizer(wx.OK | wx.CANCEL) )

	def InitListView(self):
		width, height = self.imageListSize
		self.imageList = wx.ImageList(width, height)
		self.imageListItems = {}
		self.listView.SetImageList(self.imageList, wx.IMAGE_LIST_SMALL)

		self.listView.InsertColumn(self.actionColumn, 'Action', width = 60)
		self.listView.InsertColumn(self.descriptionColumn, 'Description', width = 300)

	def FillListView(self):
		itemIndex = 0

		for action in self.actionHistory.doneActions:
			self.listView.InsertImageStringItem( itemIndex, 'Undo', self.ImageListItem('edit-undo') )

			imageName = action.GetImageName()
			if imageName:
				self.listView.SetStringItem( itemIndex, self.descriptionColumn, action.GetDescription(), self.ImageListItem(imageName) )
			else:
				self.listView.SetStringItem( itemIndex, self.descriptionColumn, action.GetDescription() )

			if action.IsIrreversible():
				self.listView.SetItemTextColour(itemIndex, self.maskedColor)

			itemIndex += 1

		for action in reversed( self.actionHistory.undoneActions ):
			self.listView.InsertImageStringItem( itemIndex, 'Redo', self.ImageListItem('edit-redo') )

			imageName = action.GetImageName()
			if imageName:
				self.listView.SetStringItem( itemIndex, self.descriptionColumn, action.GetDescription(), self.ImageListItem(imageName) )
			else:
				self.listView.SetStringItem( itemIndex, self.descriptionColumn, action.GetDescription() )

			if action.IsIrreversible():
				self.listView.SetItemTextColour(itemIndex, self.maskedColor)

			itemIndex += 1

	def ImageListItem(self, imageListItemName):
		imageListItem = self.imageListItems.get(imageListItemName)
		if imageListItem is None:
			bitmap = wx.ArtProvider.GetBitmap( imageListItemName, size = self.imageListSize )
			imageListItem = self.imageList.Add(bitmap)
			self.imageListItems[imageListItemName] = imageListItem
		return imageListItem
