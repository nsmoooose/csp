from DirectoryCommand import DirectoryCommand
from csp.tools.layout2.scripts.ui.controls.ProjectTree import ProjectTree

class RefreshDirectoryCommand(DirectoryCommand):
    def GetCaption(self):
        return "Refresh"

    def GetToolTipText(self):
        return "Refresh the directory"

    def GetToolBarImageName(self):
        return "generic"

    def Execute(self):
        ProjectTree.Instance.Refresh(self.GetDirectory())
