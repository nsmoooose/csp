#!/usr/bin/env python
import wx
import wx.glcanvas

from csp.tools.layout2.layout_module import *

class SceneWindow(wx.glcanvas.GLCanvas):
	"""This class is making it possible to display the scene as 
	a control in the wxPython framework. It contains a OsgGraphicsWindow
	instance that is built with c++ and open scene graph. Signals are
	sent from OsgGraphicsWindow when rendering is done to inform
	wxPyhon to use the current open gl context and when to swap
	buffers to display the result."""
	
	def __init__(self, parent, id, pos, size, style, name, attribList, palette):
		wx.glcanvas.GLCanvas.__init__(self, parent, id, pos, size, style, name, attribList, palette)

		self.graphicsWindow = OsgGraphicsWindow()

		self.Bind(wx.EVT_SIZE, self.on_Size)
		self.Bind(wx.EVT_KEY_DOWN, self.on_KeyDown)
		self.Bind(wx.EVT_KEY_UP, self.on_KeyUp)
		self.Bind(wx.EVT_MOUSE_EVENTS, self.on_Mouse)

		self.graphicsWindow.connectToSetCurrent(self.on_SetCurrent)
		self.graphicsWindow.connectToSwapBuffers(self.on_SwapBuffers)

	def MoveCameraToHome(self):
		self.graphicsWindow.moveCameraToHome()

	def Frame(self):
		self.graphicsWindow.Frame()

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
		if event.ButtonDown():
			button = event.GetButton()
			self.graphicsWindow.handleMouseButtonDown(x, y, button)
		elif event.ButtonUp():
			button = event.GetButton()
			self.graphicsWindow.handleMouseButtonUp(x, y, button)
		else:
			self.graphicsWindow.handleMouseMotion(x, y)

	def on_SwapBuffers(self):
		self.SwapBuffers()

	def on_SetCurrent(self):
		self.SetCurrent()
