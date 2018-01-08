import wx
import wx.glcanvas

from csp.tools.layout2.layout_module import *
from .GraphicsWindow import GraphicsWindow
from .DocumentNotebookPage import DocumentNotebookPage

class SceneWindow(GraphicsWindow, DocumentNotebookPage):
	"""This class is making it possible to display the scene as 
	a control in the wxPython framework. It contains a OsgGraphicsWindow
	instance that is built with c++ and open scene graph. Signals are
	sent from OsgGraphicsWindow when rendering is done to inform
	wxPyhon to use the current open gl context and when to swap
	buffers to display the result."""
	
	def __init__(self, *args, **kwargs):
		GraphicsWindow.__init__(self, *args, **kwargs)

		# Set the c++ specific implementation that is responsible for rendering 
		# of the scene.
		self.SetGraphicsWindow(OsgSceneWindow())

	def SetDocument(self, document):
		GraphicsWindow.SetDocument(self, document)
		self.GetGraphicsWindow().graph().setRoot(document.GetRootNode())

	def MoveCameraToHome(self):
		self.GetGraphicsWindow().moveCameraToHome()
