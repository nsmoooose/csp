#!/usr/bin/env python
from Command import Command

class ZoomOutCommand(Command):

    def GetCaption(self):
        return "Zoom out"

    def GetToolTipText(self):
        return "Zoom out"

    def GetToolBarImageName(self):
        return "zoom-out.png"

    def Execute(self):
        pass
