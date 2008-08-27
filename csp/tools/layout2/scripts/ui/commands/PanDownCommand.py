#!/usr/bin/env python
from Command import Command

class PanDownCommand(Command):

    def GetCaption(self):
        return "Pan down"

    def GetToolTipText(self):
        return "Pan down"

    def GetToolBarImageName(self):
        return "arrow-down.png"

    def Execute(self):
        pass
