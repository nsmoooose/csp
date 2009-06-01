#!/usr/bin/env python
import wx

from csp.tools.layout2.scripts.ui.controls.SceneWindow import SceneWindow
from Command import Command

class MoveCameraToHomeCommand(Command):
	"""This command moves the camera to the home position for
	the current scene control. If the current wx control isn't
	a SceneWindow the command is ignored."""

	def GetCaption(self):
		return "Move camera to home"

	def GetToolTipText(self):
		return "Moves the camera to the home position"

	def GetToolBarImageName(self):
		return "go-home.png"

	def Execute(self):
		focusWindow = wx.Window.FindFocus()

		# Test to see if we have got a window with focus. If not
		# we simply return and ignore this command.
		if focusWindow == None:
			return

		# Test to see if the window is of the right type. If not
		# we simply return and ignore this command.
		if isinstance(focusWindow, SceneWindow) == False:
			return

		# All is well. Lets execute the command.
		focusWindow.MoveCameraToHome()
