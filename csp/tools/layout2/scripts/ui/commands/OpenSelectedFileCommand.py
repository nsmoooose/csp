import wx
from csp.tools.layout2.scripts.ui.controls.ProjectTree import ProjectTree
from .Command import Command
from .FileCommandRegistry import FileCommandRegistry

class OpenSelectedFileCommand(Command):
    """Opens the selected file in the project tree of this
    application. When this command is executed we are trying
    to identify the type of file we are loading and then
    executing the correct FileCommand."""

    caption = "Open selected file"
    tooltip = "Open selected file in project tree"
    toolbarimage = "document-open"

    def Execute(self):
        """Loads a document from the project tree."""

        # Get the application object. This object is used to retreive the
        # configuration object and the top window for this application.
        application = wx.GetApp()

        # Get the top window for this application. The top window shall be the 
        # parent for the open file dialog.
        topWindow = application.GetTopWindow()
        if topWindow == None:
            return

        if ProjectTree.Instance is None:
            return

        fileName = ProjectTree.Instance.GetSelectedFile()
        if fileName is None:
            return

        # Now we have a file name and all apropriate data to identify
        # the file selected. This is the responsibility of the 
        # FileCommandRegistry. Get a command and execute it.
        command = FileCommandRegistry.Instance.GetCommandForFile(fileName)
        if command is not None:
            command.Execute()
        else:
            messageDialog = wx.MessageDialog(topWindow, 
                'Cannot find a FileCommand to execute on that file type.',
                'Error loading file', style = wx.OK|wx.ICON_ERROR)
            messageDialog.ShowModal()
