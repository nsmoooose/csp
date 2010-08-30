from Command import Command
from csp.tools.layout2.scripts.ui.controls.DocumentNotebook import DocumentNotebook
from csp.tools.layout2.scripts.ui.controls.GraphicsWindow import GraphicsWindow

class ZoomOutCommand(Command):

    def GetCaption(self):
        return "Zoom out"

    def GetToolTipText(self):
        return "Zoom out"

    def GetToolBarImageName(self):
        return "zoom-out"

    def Execute(self):
        window = DocumentNotebook.Instance.GetCurrentPage()

        # There must be an active window. It must also inherit from
        # GraphicsWindow for us to be able to do the zooming.
        if window is None or not isinstance(window, GraphicsWindow):
            return

        window.ZoomOut(10)
