from Command import Command

class PanUpCommand(Command):

    def GetCaption(self):
        return "Pan up"

    def GetToolTipText(self):
        return "Pan up"

    def GetToolBarImageName(self):
        return "arrow-up"

    def Execute(self):
        pass
