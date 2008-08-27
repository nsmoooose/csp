#!/usr/bin/env python
from Command import Command

class PanLeftCommand(Command):
    def GetCaption(self):
        return "Pan left"

    def GetToolTipText(self):
        return "Pan left"

    def GetToolBarImageName(self):
        return "arrow-left.png"

    def Execute(self):
        pass
