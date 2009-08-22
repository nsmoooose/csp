#!/usr/bin/env python
import wx
import wx.glcanvas

from csp.tools.layout2.layout_module import *

class GraphicsWindow(wx.glcanvas.GLCanvas):
	"""This class is making it possible to display 3D content as 
	a control in the wxPython framework. It contains a OsgGraphicsWindow
	instance that is built with c++ and open scene graph. Signals are
	sent from OsgGraphicsWindow when rendering is done to inform
	wxPyhon to use the current open gl context and when to swap
	buffers to display the result."""
	
	def __init__(self, *args, **kwargs):
		wx.glcanvas.GLCanvas.__init__(self, *args, **kwargs)

		self.document = None
		self.graphicsWindow = None
	
	def Dispose(self):
		self.graphicsWindow.clearSignals()
		application = wx.GetApp()
		application.GetIdleSignal().Disconnect(self.on_Idle)
		
		documentRegistry = wx.GetApp().GetDocumentRegistry()
		documentRegistry.ReleaseDocument(self.document)
		self.document = None
	
	def SetGraphicsWindow(self, graphicsWindow):
		"""Sets the graphics implementation. The implementation is a c++ class
		with rendering capabilities."""
		self.graphicsWindow = graphicsWindow

		self.Bind(wx.EVT_SIZE, self.on_Size)
		self.Bind(wx.EVT_KEY_DOWN, self.on_KeyDown)
		self.Bind(wx.EVT_KEY_UP, self.on_KeyUp)
		self.Bind(wx.EVT_MOUSE_EVENTS, self.on_Mouse)

		self.graphicsWindow.connectToSetCurrent(self.on_SetCurrent)
		self.graphicsWindow.connectToSwapBuffers(self.on_SwapBuffers)
		
		application = wx.GetApp()
		application.GetIdleSignal().Connect(self.on_Idle)

	def GetGraphicsWindow(self):
		return self.graphicsWindow

	def GetDocument(self):
		return self.document
		
	def SetDocument(self, document):
		self.document = document


	def ZoomIn(self, distance):
		self.graphicsWindow.zoomIn(distance)

	def ZoomOut(self, distance):
		self.graphicsWindow.zoomOut(distance)

	def PanLeft(self, distance):
		self.graphicsWindow.panLeft(distance)

	def PanRight(self, distance):
		self.graphicsWindow.panRight(distance)

	def PanUp(self, distance):
		self.graphicsWindow.panUp(distance)

	def PanDown(self, distance):
		self.graphicsWindow.panDown(distance)


		
	def on_Idle(self, event):
		# Only render when the scene is visible on screen.
		if self.IsShownOnScreen():
			self.graphicsWindow.Frame()
			event.RequestMore()
	
	def on_Size(self, event):
		width, height = self.GetClientSizeTuple()

		# If the window size is set to 0 the scene graph 
		# will not be rendered correctly even if correct size
		# is set. There may be a more elegant solution to 
		# this. But this works.
		if width <= 0:
			width = 10
		if height <= 0:
			height = 10
		self.graphicsWindow.setSize(width, height)

	def on_KeyDown(self, event):
		key = event.GetUnicodeKey()
		self.graphicsWindow.handleKeyDown(key)
		event.Skip()

	def on_KeyUp(self, event):
		key = event.GetUnicodeKey()
		self.graphicsWindow.handleKeyUp(key)
		event.Skip()

	def on_Mouse(self, event):
		# Set focus to this control in order for it to 
		# retreive keyboard input. The view that has focus should
		# handle the keyboard command sent.
		self.SetFocus()

		# The mouse event (movement and clicking) should be
		# processed by the manipulator implemented in c++.
		x, y = event.GetX(), event.GetY()
		wheelRotation = event.GetWheelRotation()
		if event.ButtonDown():
			button = event.GetButton()
			self.graphicsWindow.handleMouseButtonDown(x, y, button)
		elif event.ButtonUp():
			button = event.GetButton()
			self.graphicsWindow.handleMouseButtonUp(x, y, button)
		elif wheelRotation != 0:
			self.graphicsWindow.handleMouseWheelRotation(wheelRotation)
		else:
			self.graphicsWindow.handleMouseMotion(x, y)

	def on_SwapBuffers(self):
		self.SwapBuffers()

	def on_SetCurrent(self):
		self.SetCurrent()
