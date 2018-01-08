from .Command import Command

class PanRightCommand(Command):
    caption = "Pan right"
    tooltip = "Pan right"
    toolbarimage = "arrow-right"

    def Execute(self):
        pass

    @staticmethod
    def Enabled():
        return False
