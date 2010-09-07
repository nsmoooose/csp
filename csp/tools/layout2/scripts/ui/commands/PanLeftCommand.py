from Command import Command

class PanLeftCommand(Command):
    caption = "Pan left"
    tooltip = "Pan left"
    toolbarimage = "arrow-left"

    def Execute(self):
        pass

    @staticmethod
    def Enabled():
        return False
