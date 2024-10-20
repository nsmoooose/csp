import wx

from .Command import Command

class MoveCameraToHomeCommand(Command):
    """This command moves the camera to the home position for
    the current scene control. If the current wx control isn't
    a SceneWindow the command is ignored."""

    caption = "Move camera to home"
    tooltip = "Moves the camera to the home position"
    toolbarimage = "go-home"

    def Execute(self):
        focusWindow = wx.Window.FindFocus()

        # Test to see if we have got a window with focus. If not
        # we simply return and ignore this command.
        if focusWindow == None:
            return

        # Test to see if the window is of the right type. If not
        # we simply return and ignore this command.
        if hasattr(focusWindow, "MoveCameraToHome"):
            focusWindow.MoveCameraToHome()

    @staticmethod
    def Enabled():
        focusWindow = wx.Window.FindFocus()
        if focusWindow == None:
            return False
        return True if hasattr(focusWindow, "MoveCameraToHome") else False

