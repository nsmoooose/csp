import wx

class ImageWindow(wx.Window):
	"""This window holds a single bitmap control. It is used to display
	an image loaded."""

	def __init__(self, parent, id):
		wx.Window.__init__(self, parent, id)
		self.document = None

	def SetDocument(self, document):
		if document is None:
			return

		# Store a reference to the document for later use.
		self.document = document

		bitmap = wx.Bitmap(document.GetFileName())
		bitmapControl = wx.StaticBitmap(self, wx.ID_ANY, bitmap)
	
	def GetDocument(self):
		return self.document

