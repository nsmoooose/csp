#!/usr/bin/env python
from Command import Command
from csp.tools.layout2.scripts.ui.controls.DocumentNotebook import DocumentNotebook
from csp.tools.layout2.scripts.ui.controls.GraphicsWindow import GraphicsWindow

class ZoomInCommand(Command):

    def GetCaption(self):
        return "Zoom in"

    def GetToolTipText(self):
        return "Zoom in"

    def GetToolBarImageName(self):
        return "zoom-in.png"

    def Execute(self):
        window = DocumentNotebook.Instance.GetCurrentPage()

        # There must be an active window. It must also inherit from
        # GraphicsWindow for us to be able to do the zooming.
        if window is None or not isinstance(window, GraphicsWindow):
            return

        window.ZoomIn(10)
