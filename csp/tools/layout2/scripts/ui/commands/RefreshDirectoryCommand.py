from Command import Command
from csp.tools.layout2.scripts.ui.controls.ProjectTree import ProjectTree

class RefreshDirectoryCommand(Command):
    caption = "Refresh"
    tooltip = "Refresh the directory"
    toolbarimage = "generic"

    def Execute(self):
        ProjectTree.Instance.Refresh(ProjectTree.Instance.GetSelectedFile())
