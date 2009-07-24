#!/usr/bin/env python

from csp.tools.layout2.layout_module import OsgModelWindow
from GraphicsWindow import GraphicsWindow

class ModelWindow(GraphicsWindow):
	def __init__(self, *args, **kwargs):
		GraphicsWindow.__init__(self, *args, **kwargs)

		# Set the graphics window implementation used. This class we are instantiating here
		# is implemented in c++ and is responsible for all renderings.
		self.SetGraphicsWindow(OsgModelWindow())

	def SetDocument(self, document):
		# Let the base class handle this document.
		GraphicsWindow.SetDocument(self, document)

		# Get the C++ implementation of the rendering engine and 
		# load the osg model.
		implementation = self.GetGraphicsWindow()
		implementation.loadModel(document.GetFileName())
