#!/usr/bin/env python
from Command import Command

class PanRightCommand(Command):

    def GetCaption(self):
        return "Pan right"

    def GetToolTipText(self):
        return "Pan right"

    def GetToolBarImageName(self):
        return "arrow-right.png"

    def Execute(self):
        pass
