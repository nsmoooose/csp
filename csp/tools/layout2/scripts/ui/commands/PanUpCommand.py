from Command import Command

class PanUpCommand(Command):
    caption = "Pan up"
    tooltip = "Pan up"
    toolbarimage = "arrow-up"

    def Execute(self):
        pass

    @staticmethod
    def Enabled():
        return False
