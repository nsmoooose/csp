import wx

from .Command import Command

class DefaultLayoutCommand(Command):
    caption = "Default layout"
    tooltip = "Reset to the default windows layout"
    toolbarimage = "generic"

    def Execute(self):
        application = wx.GetApp()
        application.auiManager.LoadPerspective(application.defaultPerspective, update = True)
