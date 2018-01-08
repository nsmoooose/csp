from .Command import Command

class PanDownCommand(Command):
    caption = "Pan down"
    tooltip = "Pan down"
    toolbarimage = "arrow-down"

    def Execute(self):
        pass

    @staticmethod
    def Enabled():
        return False
