import os
import wx

from csp.tools.layout2.scripts.ui.CommandControlFactory import EventToCommandExecutionAdapter


class ProjectTreeContextMenu(wx.Menu):
    def __init__(self):
        wx.Menu.__init__(self)

        target = ProjectTree.Instance.GetSelectedFile()
        isfile = os.path.isfile(target)

        file_commands, directory_commands = ProjectTree.Instance.GetContextCommands()
        commands = file_commands if isfile else directory_commands
        for command in commands:
            item = wx.MenuItem(self, wx.NewId(), command.caption)
            self.AppendItem(item)
            self.Bind(wx.EVT_MENU, EventToCommandExecutionAdapter(command).Execute, item)


class ProjectTree(wx.TreeCtrl):
    """A tree control that provides information about all files
    within the project directory. All folders and files above
    the root directory will be hidden."""

    Instance = None

    def __init__(self, parent):
        wx.TreeCtrl.__init__(self, parent, style=wx.TR_HAS_BUTTONS | wx.TR_LINES_AT_ROOT)
        self.Bind(wx.EVT_RIGHT_DOWN, self._right_mouse_down)
        self.context_file_commands = []
        self.context_directory_commands = []

    def _right_mouse_down(self, event):
        """Internal function for handling right mouse button. We do want to provide
        a context menu where you can perform commands."""
        node = self.GetSelection()
        if node.IsOk():
            self.PopupMenu(ProjectTreeContextMenu(), event.GetPosition())

    def SetContextCommands(self, file_commands, directory_commands):
        """Sets the context menu commands that this tree control can use.
        These are right click commands that you can do in this tree control
        (to refresh, create new documents, rename or delete files)"""
        self.context_file_commands = file_commands
        self.context_directory_commands = directory_commands

    def GetContextCommands(self):
        """Returns an array of context commands that this control can use
        for right clicking a file or folder."""
        return (self.context_file_commands, self.context_directory_commands)

    def SetRootDirectory(self, directory):
        """ Sets the root directory that the tree should display. All subfolders
        and files of this directory will be displayed within the tree."""
        self.DeleteAllItems()

        # Remember the base directory so we can return
        # the selected file or folder.
        self.baseDirectory = directory

        self.nodeDictionary = {}
        # Add the base node that is the parent to all
        # nodes.
        rootNode = self.AddRoot('CSP')
        self.nodeDictionary[self.baseDirectory] = rootNode
        self.Refresh(directory)
        self.Expand(rootNode)

    def Refresh(self, directory=None):
        """Refreshes the content of the tree control"""
        if directory is None:
            directory = self.baseDirectory

        for root, dirs, files in os.walk(directory):
            parentNode = self.nodeDictionary[root]
            self.DeleteChildren(parentNode)
            dirs.sort()
            files.sort()
            for subDirectory in dirs:
                newNode = self.AppendItem(parentNode, subDirectory)
                self.nodeDictionary[os.path.join(root, subDirectory)] = newNode
            for file in files:
                if file.lower().endswith('.pyc'):
                    continue
                self.AppendItem(parentNode, file)

    def GetSelectedFile(self):
        node = self.GetSelection()
        if node.IsOk():
            items = []
            while node != self.GetRootItem():
                items.insert(0, self.GetItemText(node))
                node = self.GetItemParent(node)

            selection = self.baseDirectory
            for item in items:
                selection = os.path.join(selection, item)

            # Sometimes this will be unicode strings. We avoid this
            # by converting back to strings. The reason is that our
            # current swig binding doesn't handle the unicode
            # character set.
            return str(selection)

    def SetOpenCommand(self, command):
        self.Bind(wx.EVT_TREE_ITEM_ACTIVATED, EventToCommandExecutionAdapter(command).Execute)
