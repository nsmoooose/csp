# CSPEdit - Copyright (C) 2003 Mark Rose
#
# This program is free software; you can redistribute it and/or
# modify it under the terms of the GNU General Public License
# as published by the Free Software Foundation; either version 2
# of the License, or (at your option) any later version.
# 
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
# 
# You should have received a copy of the GNU General Public License
# along with this program; if not, write to the Free Software
# Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, US

from wxPython.wx import *
from wxPython.lib.filebrowsebutton import FileBrowseButton, FileBrowseButtonWithHistory,DirBrowseButton
from Config import Config
import os.path

class wxBitmapToggleButton(wxBitmapButton):
	"""A two state button with distinct bitmaps for each stat"""
	def __init__(self, *args, **kw):
		wxBitmapButton.__init__(self, *args, **kw)
		self.active = false
	def SetBitmaps(self, bm_inactive, bm_active):
		self.bm_inactive = bm_inactive
		self.bm_active = bm_active
	def GetActive(self):
		return self.active
	def ToggleActive(self):
		self.SetActive(not self.active)
	def SetActive(self, active):
		if active == self.active:
			return
		if active:
			bm = self.bm_active
		else:
			bm = self.bm_inactive
		self.SetBitmapLabel(bm)
		self.SetBitmapSelected(bm)
		self.SetBitmapFocus(bm)
		self.SetBitmapDisabled(bm)
		self.active = active


class ToolDialog(wxFrame):
	def __init__(self, core, parent, ID, title):
		wxFrame.__init__(self, parent, ID, title,
						 wxDefaultPosition, wxSize(200, 150))
		self._core = core
		self.SetClientSize((400, 300))
		self.SetSizeHints(128, 64)
		self.dialogs = {}
		self.MakeDialogs()
		self.vbox = wxBoxSizer(wxVERTICAL)
		self.SetSizer(self.vbox)
		self.Select('move')

	def Select(self, tool):
		dialog = self.dialogs.get(tool, None)
		if dialog is None: 
			return
		self.vbox.Add(dialog, 1, wxEXPAND)
		self.vbox.SetSizeHints(self)

	def MakeDialogs(self):
		self.MakeMoveDialog()
		self.MakeRotateDialog()

	def MakeMoveDialog(self):
		dialog = wxPanel(dialog, -1)
		vbox = wxBoxSizer(wxVERTICAL)
		vbox.Add(wxStaticText(abs, -1, "Not Implemented"),1,wxEXPAND)
		dialog.SetSizer(vbox)
		vbox.SetSizeHints(dialog)
		self.dialogs['move'] = dialog

	def MakeRotateDialog(self):
		dialog = wxNotebook(self, -1)
		self.dialogs['rotate'] = dialog
	
class IDGen:
	def __init__(self, id):
		self.id = id
	def __call__(self):
		id = self.id
		self.id = id + 1
		return id


class MainWindow(wxFrame):
	def __init__(self, core, parent, ID, title):
		wxFrame.__init__(self, parent, ID, title,
						 wxDefaultPosition, wxSize(200, 150))
		self.id = IDGen(2000)
		self._core = core
		self._toolset = {}
		self.CreateStatusBar()
		self.SetStatusText("This is the statusbar")
		self.MakeMenu()
		self.MakeWindow()
		self.MakeToolBar()
		#self.ToolDialog = ToolDialog(core, NULL, -1, "Tool Options")
		#self.ToolDialog.Show(true)
		self._tool = None
		self._edit = false
		self._tools = ['move', 'rotate']
		self.Layout()

	def MakeToolBar(self):
		idgen = IDGen(1000)
		toolbar = self.CreateToolBar(wxTB_HORIZONTAL|wxSUNKEN_BORDER|wxTB_TEXT|wxTB_3DBUTTONS)
		toolbar.SetToolBitmapSize((64,64))
		toolbar.SetMargins((8,8))
		SEPARATOR, RADIO, CONTROL, TOOL, CHECK, TOGGLE = range(6)
		tools = [
			(TOGGLE, self.OnEdit, 'edit', 'toggle edit or view mode'),
			(SEPARATOR,),
			(TOGGLE, self.OnMove, 'move', 'reposition an object'),
			(TOGGLE, self.OnRotate, 'rotate', 'rotate an object'),
			(SEPARATOR,),
			(TOOL, self.OnViewX, 'viewx', 'look along x-axis'),
			(TOOL, self.OnViewY, 'viewy', 'look along y-axis'),
			(TOOL, self.OnViewZ, 'viewz', 'look along z-axis'),
		]
		self._toolset = {}
		for tool in tools:
			id = idgen()
			type = tool[0]
			control = None
			if type == RADIO:
				handler, name, help = tool[1:]
				bm0 = wxBitmapFromImage(wxImage("%s.bmp" % name))
				bm1 = wxBitmapFromImage(wxImage("%s-on.bmp" % name))
				toolbar.AddRadioTool(id, bm0, bm1, name, help)
				if handler is not None:
					EVT_TOOL(self, id, handler)
			elif type == SEPARATOR:
				toolbar.AddSeparator()
			elif type == CONTROL:
				handler, control, name, help = tool[1:]
				control.SetToolTip(wxToolTip(help))
				toolbar.AddControl(id, control, name, help)
				self._toolset[name] = control
				if handler is not None:
					EVT_BUTTON(self, id, handler)
			elif type == TOOL:
				handler, name, help = tool[1:]
				bm0 = wxBitmapFromImage(wxImage("%s.bmp" % name))
				bm1 = wxNullBitmap
				toolbar.AddTool(id, bm0, bm1, 1, name, help)
				if handler is not None:
					EVT_TOOL(self, id, handler)
			elif type == CHECK:
				handler, name, help = tool[1:]
				bm0 = wxBitmapFromImage(wxImage("%s.bmp" % name))
				bm1 = wxBitmapFromImage(wxImage("%s-on.bmp" % name))
				toolbar.AddCheckTool(id, bm0, bm1, name, help)
				if handler is not None:
					EVT_TOOL(self, id, handler)
			elif type == TOGGLE:
				handler, name, help = tool[1:]
				bm0 = wxBitmapFromImage(wxImage("%s.bmp" % name))
				bm1 = wxBitmapFromImage(wxImage("%s-on.bmp" % name))
				control = wxBitmapToggleButton(toolbar, id, bm0)
				control.SetToolTip(wxToolTip(help))
				control.SetBitmaps(bm0, bm1)
				toolbar.AddControl(control)
				print name
				self._toolset[name] = control
				if handler is not None:
					EVT_BUTTON(self, id, handler)
		toolbar.SetClientSize((len(tools)*64,80))
		toolbar.Realize()
		self.toolbar = toolbar

	def MakeWindow(self):
		self.tree = self.MakeTree(self)
		load = wxBoxSizer(wxHORIZONTAL)
		id = self.id()
		load.Add(wxButton(self, id, "NEW"), 0, wxBOTTOM)
		EVT_BUTTON(self, id, self.OnLoad)
		directory = Config.get('ui.load.last_directory', '.')
		self.fbbh = FileBrowseButtonWithHistory(self, -1, wxPoint(20, 50), 
		                                        wxSize(450, -1), labelText="", 
												startDirectory=directory,
												fileMask="Models|*.osg;*.3ds|All files|*",
		                                        fileMode=wxOPEN)
		history = Config.get('ui.load.history', [])
		self.fbbh.SetHistory(history)
		load.Add(self.fbbh, 1, wxTOP)
		vbox = wxBoxSizer(wxVERTICAL)
		vbox.Add(self.tree,1,wxEXPAND|wxALL,10)
		vbox.Add(load,0,wxGROW|wxALIGN_BOTTOM|wxALL,10)
		self.SetSizer(vbox)
		vbox.SetSizeHints(self)

	def MakeMenu(self):
		ID_ABOUT = 101
		ID_EXIT  = 102

		menu = wxMenu()
		menu.Append(ID_ABOUT, "&About",
					"More information about this program")
		menu.AppendSeparator()
		menu.Append(ID_EXIT, "E&xit", "Terminate the program")

		menuBar = wxMenuBar()
		menuBar.Append(menu, "&File");

		EVT_MENU(self, ID_EXIT, self.OnExit)

		self.SetMenuBar(menuBar)

	def MakeTree(self, parent):
		tID = wxNewId()
		tree = wxTreeCtrl(parent, tID, wxDefaultPosition, wxDefaultSize, wxTR_HAS_BUTTONS|wxSUNKEN_BORDER|wxVSCROLL)
		tree.SetClientSize((400, 300))
		tree.SetSizeHints(128, 64)

		root = tree.AddRoot("ROOT")

		EVT_TREE_SEL_CHANGED(tree, tID, self.OnSelChanged)
		return tree

	def OnExit(self, event):
		try:
			#self.ToolBox.Destroy()
			pass
		except:
			pass
		self.Destroy()
		event.Skip()

	def OnSelChanged(self, event):
		item = event.GetItem()
		print item
		event.Skip()

	def OnLeftDown(self, event):
		pt = event.GetPosition();
		item, flags = self.tree.HitTest(pt)
		print "OnLeftDClick: %s\n" % self.tree.GetItemText(item)
		event.Skip()

	def OnViewX(self, event):
		print "view X"
		event.Skip()

	def OnViewY(self, event):
		print "view Y"
		event.Skip()

	def OnViewZ(self, event):
		print "view Z"
		event.Skip()

	def OnMove(self, event):
		self._tool = 'move'
		self.SetEdit(true)
		event.Skip()

	def OnRotate(self, event):
		self._tool = 'rotate'
		self.SetEdit(true)
		event.Skip()

	def OnEdit(self, event):
		self.SetEdit(not self._edit)
		event.Skip()
	
	def SetEdit(self, edit):
		self._edit = edit
		self._toolset['edit'].SetActive(edit)
		self.UpdateTools()

	def UpdateTools(self):
		for tool in self._tools:
			active = self._tool == tool and self._edit
			self._toolset[tool].SetActive(active)

	def OnLoad(self, event):
		fn = self.fbbh.GetValue()
		loaded = self._core.load(fn)
		if loaded:
			history = self.fbbh.GetHistory()
			if fn in history: history.remove(fn)
			history = [fn] + history[:11]
			self.fbbh.SetHistory(history)
			Config['ui.load.history'] = history
			Config['ui.load.last_directory'] = os.path.dirname(fn)
			self.UpdateTree(loaded)
		event.Skip()

	def UpdateTree(self, show=None):
		class TreeUpdateVisitor:
			def __init__(self, tree, root):
				self.tree = tree
				self.tree.DeleteAllItems()
				self.item = {}
				self.rootItem = tree.AddRoot("ROOT")
				self.item[root] = self.rootItem
			def __call__(self, node):
				parentItem = self.item[node.parent]
				item = self.tree.AppendItem(parentItem, node.getName())
				self.item[node] = item
			def getItem(self, node):
				return self.item[node]
		visitor = TreeUpdateVisitor(self.tree, self._core.getRoot())
		self._core.accept(visitor)
		if show is not None:
			self.tree.EnsureVisible(visitor.getItem(show))




class Init(wxApp):
	def __init__(self, core):
		self._core = core
		wxApp.__init__(self, 0)
	def OnInit(self):
		frame = MainWindow(self._core, NULL, -1, "CSPEdit")
		frame.Show(true)
		self.SetTopWindow(frame)
		return true



