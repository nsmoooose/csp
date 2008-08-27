#!/usr/bin/env python
from Command import Command

class ZoomInCommand(Command):

    def GetCaption(self):
        return "Zoom in"

    def GetToolTipText(self):
        return "Zoom in"

    def GetToolBarImageName(self):
        return "zoom-in.png"

    def Execute(self):
        pass

