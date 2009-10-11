import wx

from ActionHistoryCommand import ActionHistoryCommand

class ShowActionHistoryCommand(ActionHistoryCommand):
	"""Show the action history."""
	
	def __init__(self):
		ActionHistoryCommand.__init__(self)
	
	def GetCaption(self):
		return "Action history"
	
	def GetToolTipText(self):
		return "Show the action history"
	
	def GetToolBarImageName(self):
		return "generic"
	
	def on_ActionHistoryChanged(self, actionHistory):
		if actionHistory.HasAction():
			self.Enable(True)
		else:
			self.Enable(False)
	
	def Execute(self):
		actionHistoryDialog = ActionHistoryDialog( wx.GetApp().GetTopWindow(), title = self.GetCaption(), actionHistory = self.actionHistory )
		if actionHistoryDialog.ShowModal() == wx.ID_OK:
			pass
		actionHistoryDialog.Destroy()

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
		nextAction = None
		masked = False
		
		for action in self.actionHistory.VisibleActions():
			if action.state == action.Done:
				self.listView.InsertImageStringItem( itemIndex, 'Undo', self.ImageListItem('edit-undo') )
			else:
				self.listView.InsertImageStringItem( itemIndex, 'Redo', self.ImageListItem('edit-redo') )
			
			imageName = action.GetImageName()
			if imageName:
				self.listView.SetStringItem( itemIndex, self.descriptionColumn, action.GetDescription(), self.ImageListItem(imageName) )
			else:
				self.listView.SetStringItem( itemIndex, self.descriptionColumn, action.GetDescription() )
			
			if masked:
				self.listView.SetItemTextColour(itemIndex, self.maskedColor)
			else:
				if action.IsIrreversible() or action.IsMaskedByOtherThan(nextAction):
					masked = True
					self.listView.SetItemTextColour(itemIndex, self.maskedColor)
				
				nextAction = action
			
			itemIndex += 1
	
	def ImageListItem(self, imageListItemName):
		imageListItem = self.imageListItems.get(imageListItemName)
		if imageListItem is None:
			bitmap = wx.ArtProvider.GetBitmap( imageListItemName, size = self.imageListSize )
			imageListItem = self.imageList.Add(bitmap)
			self.imageListItems[imageListItemName] = imageListItem
		return imageListItem
