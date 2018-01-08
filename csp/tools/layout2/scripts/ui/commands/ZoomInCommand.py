from .Command import Command
from csp.tools.layout2.scripts.ui.controls.DocumentNotebook import DocumentNotebook

class ZoomInCommand(Command):

    caption = "Zoom in"
    tooltip = "Zoom in"
    toolbarimage = "zoom-in"

    def Execute(self):
        window = DocumentNotebook.Instance.GetCurrentPage()

        # There must be an active window. It must also inherit from
        # GraphicsWindow for us to be able to do the zooming.
        if window is None or not hasattr(window, "ZoomIn"):
            return

        window.ZoomIn(10)

    @staticmethod
    def Enabled():
        window = DocumentNotebook.Instance.GetCurrentPage()
        return False if window is None or not hasattr(window, "ZoomIn") else True
