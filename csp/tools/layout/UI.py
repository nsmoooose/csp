# CSP Theater Layout Tool
# Copyright 2003-2005 Mark Rose <mkrose@users.sourceforge.net>
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
from Config import Config
import cLayout
import os.path
import ids
import DataTree
import domtree
import sys
import traceback
import math


# TODO
#  * separate helper classes into other modules
#
#  * refactor to better separate mvc
#
#  * review object ownership and lifetime issues, esp. objects shared with c++.  at
#    present we are leaking many objects (not great but better than double deleting).


class Closure:
	def __init__(self, func, *args, **kw):
		self.func = func
		self.args = args
		self.kw = kw
	def __call__(self, *args):
		args = args + self.args
		self.func(*args, **self.kw)


class FileDialog(wxFileDialog):

	def __init__(self, key, parent, title, filters, flags):
		self.__key = key
		self.__filters = filters
		self.__flags = flags
		last_dir = Config.get('ui.load.%s.last_directory' % key, '.')
		wxFileDialog.__init__(self, parent, title, last_dir, '', filters, flags)
		filter_index = Config.get('ui.load.%s.filter_index' % key, 0)
		if filter_index < len(filters):
			self.SetFilterIndex(filter_index)

	def ShowModal(self):
		result = wxFileDialog.ShowModal(self)
		if result == wxID_OK:
			fn = self.GetPath()
			Config['ui.load.%s.last_directory' % self.__key] = os.path.dirname(fn)
			Config['ui.load.%s.filter_index' % self.__key] = self.GetFilterIndex()
		return result


class MRUMenu(wxMenu):
	def __init__(self, label, depth, separator=0):
		wxMenu.__init__(self)
		self.label = label
		self.depth = depth
		self.dirty = 1
		self.separator = separator
		self.items = Config.get('ui.%s.mru' % label, [])[:depth]

	def Items(self):
		return self.items

	def AddStatic(self, frame, id, label, hint, callback):
		self.menu.Append(id, label, hint)
		EVT_MENU(frame, id, callback)

	def SetHandler(self, frame, callback):
		self.frame = frame
		self.mru_handler = callback
		self.static_count = self.GetMenuItemCount()
		EVT_MENU_OPEN(self, self.UpdateUI)

	def AddItem(self, text):
		if text in self.items:
			self.items.remove(text)
		self.items.insert(0, text)
		self.items = self.items[:self.depth]
		Config['ui.%s.mru' % self.label] = self.items
		self.dirty = 1

	def UpdateUI(self, event):
		if self.dirty:
			for i in range(0, self.GetMenuItemCount() - self.static_count):
				self.Remove(ids.INSERT_MRU + i)
			if self.items and self.separator:
				self.AppendSeparator()
				self.separator = 0
				self.static_count += 1
			for i in range(len(self.items)):
				label = '%d %s' % (i, self.items[i])
				if (i < 10):
					label = '&%s' % label
				self.Append(ids.INSERT_MRU + i, label, "")
				EVT_MENU(self.frame, ids.INSERT_MRU + i, Closure(self.mru_handler, self.items[i]))
			self.dirty = 0
		event.Skip()


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
	def __init__(self, parent, ID, title):
		wxFrame.__init__(self, parent, ID, title, wxDefaultPosition, wxSize(200, 150))
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


# Fun with metaclasses.  Callbacks from C++ occur outside of the wxWindows event
# thread, so it isn't safe to manipulate the UI directly.  Instead we construct
# custom event classes for each callback handler and pass instances of these to
# registered event handlers (e.g. the main window).  Event passing is done via
# wxPostEvent, which takes care to safely transfer the event to the main event
# thread before dispatching it to the correct handlers.

class CallbackEventFactory(type):
	def BIND(cls, window, function):
		window.Connect(-1, -1, cls.eventType, function)

def MakeCallbackEvent(name):
	def init(self, *args):
		wxPyCommandEvent.__init__(self, self.eventType, -1),
		if len(args) == 1: args = args[0]  # sugar to make unpacking via self.args() cleaner
		self._args = args
	methods = {
		'eventType': wxNewEventType(),
		'__init__': init,
		'args': lambda self: self._args,
		'Clone': lambda self: self.__class__()
	}
	return CallbackEventFactory(name, (wxPyCommandEvent,), methods)


class ViewCallback(cLayout.ViewCallback):
	def __init__(self, target):
		cLayout.ViewCallback.__init__(self)
		self._target = target

	def postEvent(self, event):
		wxPostEvent(self._target, event)

	MoveModeEvent = MakeCallbackEvent('MoveModeEvent')
	RotateModeEvent = MakeCallbackEvent('RotateModeEvent')
	UpdatePositionEvent = MakeCallbackEvent('UpdatePositionEvent')
	KeyEvent = MakeCallbackEvent('KeyEvent')

	def onMoveMode(self):
		event = ViewCallback.MoveModeEvent()
		self.postEvent(event)

	def onRotateMode(self):
		event = ViewCallback.RotateModeEvent()
		self.postEvent(event)

	def onUpdatePosition(self, mouse_x, mouse_y, pos_x, pos_y, dx, dy, angle):
		event = ViewCallback.UpdatePositionEvent(mouse_x, mouse_y, pos_x, pos_y, dx, dy, angle)
		self.postEvent(event)

	def onKey(self, key):
		event = ViewCallback.KeyEvent(key)
		self.postEvent(event)


class GraphCallback(cLayout.GraphCallback):
	def __init__(self, target):
		cLayout.GraphCallback.__init__(self)
		self._target = target

	def postEvent(self, event):
		wxPostEvent(self._target, event)

	SelectEvent = MakeCallbackEvent('SelectEvent')
	DeselectEvent = MakeCallbackEvent('DeselectEvent')
	SelectionClearEvent = MakeCallbackEvent('SelectionClearEvent')
	ActivateEvent = MakeCallbackEvent('ActivateEvent')
	RefreshEvent = MakeCallbackEvent('RefreshEvent')

	def onSelect(self, path, node):
		path = [n for n in path]  # convert to a normal python list
		self.postEvent(GraphCallback.SelectEvent(path, node))

	def onDeselect(self, path, node):
		path = [n for n in path]  # convert to a normal python list
		self.postEvent(GraphCallback.DeselectEvent(path, node))

	def onSelectionClear(self):
		self.postEvent(GraphCallback.SelectionClearEvent())

	def onActivate(self, path, matrix):
		path = [n for n in path]  # convert to a normal python list
		self.postEvent(GraphCallback.ActivateEvent(path))

	def onRefresh(self):
		self.postEvent(GraphCallback.RefreshEvent())


class TreeUpdateVisitor(cLayout.LayoutNodeVisitor):
	def __init__(self, tree):
		cLayout.LayoutNodeVisitor.__init__(self)
		self._tree = tree
		self._tree.DeleteAllItems()
		self.item = {}
		self.idx = [0, 0]

	def apply(self, node):
		name = unicode(node.getName(), 'latin1')
		parent = self.parent()
		if parent:
			parent_item = self.item[parent.id()]
			if not name:
				name = self.anonymous(node)
			item = self._tree.AppendItem(parent_item, name)  # fixme: id?
			self._tree.SetPyData(item, node)
		else:
			item = self.rootItem = self._tree.AddRoot(name)
			self._tree.SetPyData(item, node)
		self.item[node.id()] = item
		node.traverse(self)

	def getItem(self, node):
		return self.item.get(node.id(), None)

	def anonymous(self, node):
		if node.isGroup():
			self.idx[0] += 1
			return 'unknown group %d' % self.idx[0]
		self.idx[1] += 1
		return 'unknown model %d' % self.idx[1]


class SaveVisitor(cLayout.LayoutNodeVisitor):
	"""
	A visitor that synchronizes the xml hierarchy with the feature node graph at save time.
	We first undelete all xml nodes, then walk the feature node graph and apply updates to
	the corresponding xml nodes.  At the end, any xml nodes that haven't been updated are
	no longer in the feature node graph so we remove them (always call removeDeleted after
	the visitor walks the feature node graph).  The undelete step is necessary since deleted
	nodes may be restored in the feature node graph using the undo function; this is the
	(small) price we pay for lazy synchronization.
	"""

	def __init__(self, tree, node_map):
		cLayout.LayoutNodeVisitor.__init__(self)
		self._tree = tree
		self.node_map = node_map
		for node in node_map.nodes():
			node.undelete()

	def removeDeleted(self):
		for node in self.node_map.nodes():
			if not node.wasUpdated():
				node.delete()

	def apply(self, feature_node):
		ptr = feature_node #.asNode().this
		node = self.node_map.get(ptr)
		if node is not None:
			node.updateFrom(feature_node)
		else:
			# TODO handle new nodes correctly
			print 'WARNING: no xml node for feature node (%s)' % ptr
			DataTree.CustomLayoutModel.addLayoutNode(feature_node)
		feature_node.traverse(self)


class MainWindow(wxFrame):
	def __init__(self, view, xml_path, model_path, parent, ID, title):
		wxFrame.__init__(self, parent, ID, title, wxDefaultPosition, wxSize(500, 300))
		self.id = IDGen(20000)
		self._view = view
		self._xml_path = xml_path
		self._model_path = model_path
		self._node_map = None
		self._filename = None
		self._active_id = None
		self._toolset = {}
		self._status_bar = self.CreateStatusBar(2)
		self._status_bar.SetStatusWidths([-1, 130])
		self._status_bar.SetStatusText('', 0)

		# TODO update the statusbar via graph/view callbacks, not a timer
		self._status_timer = wxPyTimer(self.UpdateStatusBar)
		self._status_timer.Start(250)

		self.MakeMenu()
		self.MakeWindow()
		self.MakeToolBar()
		self._tool = 'move'
		self._edit = false
		self._tools = ['move', 'rotate']
		self._insert_dialog = FileDialog('insert_model', self, 'Insert Model', 'XML files (*.xml)|*.xml|3DS files (*.3ds)|*.3ds|OSG files (*.osg)|*.osg|All files|*.*', wxOPEN)
		self._open_dialog = FileDialog('open', self, 'Open Feature', 'XML files (*.xml)|*.xml|3DS files (*.3ds)|*.3ds|OSG files (*.osg)|*.osg|All files|*.*', wxOPEN)
		self._save_dialog = FileDialog('save', self, 'Save Feature', 'XML files (*.xml)|*.xml|All files|*.*', wxSAVE)
		self.SetGraphCallback()
		self.SetViewCallback()
		self.Layout()
		self.UpdateTree()
		self.UpdateTools()
		self.setFilename(None)

	def UpdateStatusBar(self, *args):
		state = ''
		if self._view.graph().isModified():
			state = 'MOD'
		self._status_bar.SetStatusText(state, 1)

	def onUpdatePosition(self, event):
		mouse_x, mouse_y, pos_x, pos_y, dx, dy, angle = event.args()
		msg = []
		if mouse_x != 0 or mouse_y != 0:
			msg.append('(%+4.1f %+4.1f)' % (mouse_x, mouse_y))
		if pos_x != 0 or pos_y != 0:
			msg.append('P(%+4.1f %+4.1f)' % (pos_x, pos_y))
		if dx != 0 or dy != 0:
			msg.append('D(%+4.1f %+4.1f)' % (dx, dy))
		if angle != 0:
			msg.append('R(%+4.1f)' % math.degrees(angle))
		msg = ', '.join(msg)
		self._status_bar.SetStatusText(msg, 0)

	def onKey(self, event):
		key = event.args()
		code = {'SPACE' : WXK_SPACE, 'LEFT' : WXK_LEFT, 'RIGHT' : WXK_RIGHT, 'UP' : WXK_UP, 'DOWN' : WXK_DOWN}.get(key)
		if code is not None:
			evt = wxKeyEvent(wxEVT_CHAR)
			evt.m_keyCode = code
			wxPostEvent(self._tree, evt)

	def SetGraphCallback(self):
		self._graph_event_callback = GraphCallback(self)
		self._graph_event_callback.thisown = 0
		self._view.graph().addCallback(self._graph_event_callback)
		GraphCallback.SelectEvent.BIND(self, self.onSelect)
		GraphCallback.DeselectEvent.BIND(self, self.onDeselect)
		GraphCallback.SelectionClearEvent.BIND(self, self.onSelectionClear)
		GraphCallback.RefreshEvent.BIND(self, self.onRefresh)
		GraphCallback.ActivateEvent.BIND(self, self.onActivate)

	def SetViewCallback(self):
		self._view_event_callback = ViewCallback(self)
		self._view_event_callback.thisown = 0
		self._view.addCallback(self._view_event_callback)
		ViewCallback.MoveModeEvent.BIND(self, self.onMoveMode)
		ViewCallback.RotateModeEvent.BIND(self, self.onRotateMode)
		ViewCallback.UpdatePositionEvent.BIND(self, self.onUpdatePosition)
		ViewCallback.KeyEvent.BIND(self, self.onKey)

	def FindChildNode(self, id, node):
		"""Search through the children of specified item in the tree control, returning
		the id of the item corresponding to LayoutNode node (or None).
		"""
		child_count = self._tree.GetChildrenCount(id, recursively=0)
		if child_count < 1: return None
		id, cookie = self._tree.GetFirstChild(id)
		node = node.asNode();  # needed for ptr comparisons
		for i in range(child_count):
			assert(id is not None and id.IsOk())
			child = self._tree.GetPyData(id)
			if child is not None:
				child = child.asNode()
				print 'FindChildNode:', child.this, node.this
				if child.this == node.this: return id
			id = self._tree.GetNextSibling(id)
		return None

	def SearchNodePath(self, path, id=None):
		"""Walk the tree control, searching for the items that correspond to the
		LayoutNode components of path.  Returns the id of the last item in path,
		or None.
		"""
		if not path: return id
		if id is None:
			id = self._tree.GetRootItem()
			path = path[1:]  # skip the root node for now (PyData isn't set correctly)
			if not path: return id
		node = path[0]
		id = self.FindChildNode(id, node)
		if id is None: return None
		return self.SearchNodePath(path[1:], id)

	def onSelect(self, event):
		path, node = event.args()
		self.DumpTree()
		active_id = self.SearchNodePath(path)
		assert(active_id is not None)
		assert(active_id.IsOk())
		print 'active group text', self._tree.GetItemText(active_id)
		node_id = self.FindChildNode(active_id, node)
		assert(node_id is not None)
		assert(node_id.IsOk())
		print 'selected item text', self._tree.GetItemText(node_id)
		self._disable_selection_handling = 1
		self._tree.EnsureVisible(node_id)
		self._tree.SelectItem(node_id, 1)  # XXX does not work for multiple selections; wxwindows bug from may 2001!? :-(
		self._disable_selection_handling = 0

	def DumpTree(self, id=None, indent=''):
		if id is None:
			self.DumpTree(self._tree.GetRootItem())
		else:
			name = unicode(self._tree.GetItemText(id)).encode("iso-8859-2", "replace")
			print '%s%s' % (indent, name)
			indent = indent + '  '
			child_count = self._tree.GetChildrenCount(id, recursively=0)
			if child_count < 1: return
			id, cookie = self._tree.GetFirstChild(id)
			for i in range(child_count):
				self.DumpTree(id, indent)
				id = self._tree.GetNextSibling(id)

	def onDeselect(self, event):
		path, node = event.args()
		active_id = self.SearchNodePath(path)
		assert(active_id is not None and active_id.IsOk())
		node_id = self.FindChildNode(active_id, node)
		assert(node_id is not None and node_id.IsOk())
		self._disable_selection_handling = 1
		self._tree.SelectItem(node_id, 0) 
		self._disable_selection_handling = 0

	def onSelectionClear(self, event):
		self._disable_selection_handling = 1
		self._tree.UnselectAll()
		self._disable_selection_handling = 0

	def onRefresh(self, event):
		self.UpdateTree()

	def onActivate(self, event):
		path = event.args()
		self.UpdateActivePath(path)

	def UpdateActivePath(self, path):
		if not isinstance(path, list):
			path = [item for item in path]  # convert to a normal python list
		print 'UpdateActivePath', [n.getName() for n in path]
		active_id = self.SearchNodePath(path)
		assert(active_id is not None and active_id.IsOk())
		if self._active_id is not None:
			self._tree.SetItemBold(self._active_id, 0)
		self._tree.SetItemBold(active_id)
		self._tree.EnsureVisible(active_id)
		if path:
			selected = path[-1].asGroup().getSelectedChildren()
			if selected:
				node = selected[0]
				print 'UpdateActivePath, select', node
				id = self.FindChildNode(active_id, node)
				if id is not None and id.IsOk():
					print 'found', id
					self._disable_selection_handling = 1
					#self._tree.EnsureVisible(id)
					self._tree.SelectItem(id, 1)  # XXX does not work for multiple selections; wxwindows bug from may 2001!? :-(
					self._disable_selection_handling = 0
					print 'selected'
		self._active_id = active_id

	def MakeToolBar(self):
		idgen = IDGen(21000)
		toolbar = self.CreateToolBar(wxTB_HORIZONTAL|wxSUNKEN_BORDER|wxTB_TEXT|wxTB_3DBUTTONS)
		toolbar.SetToolBitmapSize((64,64))
		toolbar.SetMargins((8,8))
		SEPARATOR, RADIO, CONTROL, TOOL, CHECK, TOGGLE = range(6)
		tools = [
			(TOGGLE, self.OnMove, 'move', 'reposition an object'),
			(TOGGLE, self.OnRotate, 'rotate', 'rotate an object'),
			#(SEPARATOR,),
			#(TOOL, self.OnUndo, 'undo', 'undo'),
			#(TOOL, self.OnRedo, 'redo', 'redo'),
			#(SEPARATOR,),
			#(TOOL, self.OnViewX, 'viewx', 'look along x-axis'),
			#(TOOL, self.OnViewY, 'viewy', 'look along y-axis'),
			(TOOL, self.OnViewZ, 'viewz', 'look along z-axis'),
		]
		self._toolset = {}
		for tool in tools:
			id = idgen()
			type = tool[0]
			control = None
			if type == RADIO:
				handler, name, help = tool[1:]
				bm0 = wxBitmapFromImage(wxImage(os.path.join("images", "%s.png" % name)))
				bm1 = wxBitmapFromImage(wxImage(os.path.join("images", "%s-on.png" % name)))
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
				bm0 = wxBitmapFromImage(wxImage(os.path.join("images", "%s.png" % name)))
				bm1 = wxNullBitmap
				toolbar.AddTool(id, bm0, bm1, 1, name, help)
				if handler is not None:
					EVT_TOOL(self, id, handler)
			elif type == CHECK:
				handler, name, help = tool[1:]
				bm0 = wxBitmapFromImage(wxImage(os.path.join("images", "%s.png" % name)))
				bm1 = wxBitmapFromImage(wxImage(os.path.join("images", "%s-on.png" % name)))
				toolbar.AddCheckTool(id, bm0, bm1, name, help)
				if handler is not None:
					EVT_TOOL(self, id, handler)
			elif type == TOGGLE:
				handler, name, help = tool[1:]
				bm0 = wxBitmapFromImage(wxImage(os.path.join("images", "%s.png" % name)))
				bm1 = wxBitmapFromImage(wxImage(os.path.join("images", "%s-on.png" % name)))
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
		panel = wxPanel(self, -1)
		vbox = wxBoxSizer(wxVERTICAL)
		self._tree = self.MakeTree(panel)
		vbox.Add(self._tree, 2, wxEXPAND, 10)
		panel.SetSizer(vbox)
		panel.SetAutoLayout(true)

	def MakeMenu(self):
		menuBar = wxMenuBar()

		menu = MRUMenu('file', 10, separator=1)
		menu.Append(ids.NEW, "&New", "Create a new feature group")
		menu.Append(ids.OPEN, "&Open", "Open a feature group from disk")
		menu.Append(ids.REVERT, "&Revert", "Revert to the last saved version")
		menu.Append(ids.SAVE, "&Save", "Save all modified nodes")
		menu.Append(ids.SAVE_AS, "Save &as...", "Save the specific nodes")
		menu.AppendSeparator()
		menu.Append(ids.EXIT, "E&xit", "Terminate the program")
		menu.SetHandler(self, self.OnOpenMRU)
		menuBar.Append(menu, "&File");
		self._openMenu = menu

		menu = wxMenu()
		menu.Append(ids.UNDO, "&Undo\tCTRL-Z", "Undo last change")
		menu.Append(ids.REDO, "&Redo\tCTRL-R", "Redo last change")
		menu.Append(ids.DEL, "&Delete\tx", "Delete selected node(s)")
		menuBar.Append(menu, "&Edit");
		self._editMenu = menu

		#menu = wxMenu()
		#menu.Append(ids.ABOUT, "&About", "More information about this program")
		#menuBar.Append(menu, "&Help");

		menu = MRUMenu('insert', 10, separator=1)
		#menu.Append(ids.INSERT_GROUP, "&Group", "Insert a CustomLayoutModel")
		menu.Append(ids.INSERT_MODEL, "&Model", "Insert a FeatureModel")
		menu.SetHandler(self, self.OnInsertMRU)
		menuBar.Append(menu, "&Insert");
		self._insertMenu = menu

		EVT_MENU(self, ids.EXIT, self.OnExit)
		EVT_MENU(self, ids.NEW, self.OnNew)
		EVT_MENU(self, ids.OPEN, self.OnOpen)
		EVT_MENU(self, ids.REVERT, self.OnRevert)
		EVT_MENU(self, ids.SAVE, self.OnSave)
		EVT_MENU(self, ids.SAVE_AS, self.OnSaveAs)
		EVT_MENU(self, ids.INSERT_GROUP, self.OnInsertGroup)
		EVT_MENU(self, ids.INSERT_MODEL, self.OnInsertModel)
		EVT_MENU(self, ids.UNDO, self.OnUndo)
		EVT_MENU(self, ids.REDO, self.OnRedo)
		EVT_MENU(self, ids.DEL, self.OnDelete)
		EVT_UPDATE_UI(self, ids.REVERT, self.OnUpdateRevert)
		EVT_UPDATE_UI(self, ids.SAVE, self.OnUpdateSave)
		EVT_UPDATE_UI(self, ids.UNDO, self.OnUpdateUndo)
		EVT_UPDATE_UI(self, ids.REDO, self.OnUpdateRedo)
		EVT_UPDATE_UI(self, ids.DEL, self.OnUpdateDelete)

		self.SetMenuBar(menuBar)

	def OnUpdateRedo(self, event):
		event.Enable(self._view.graph().hasRedo())
		event.SetText('Redo ' + self._view.graph().getRedoLabel())

	def OnUpdateUndo(self, event):
		event.Enable(self._view.graph().hasUndo())
		event.SetText('Undo ' + self._view.graph().getUndoLabel())

	def OnUpdateDelete(self, event):
		active_group = self._view.graph().getActiveGroup()
		event.Enable(active_group is not None and active_group.numSelectedChildren() > 0)

	def OnUpdateSave(self, event):
		event.Enable(self._view.graph().isModified())

	def OnUpdateRevert(self, event):
		event.Enable(self._view.graph().isModified())

	def OnUndo(self, *args):
		self._view.graph().undo()
	
	def OnRedo(self, *args):
		self._view.graph().redo()

	def OnDelete(self, *args):
		self._view.deleteSelectedNodes()

	def OnSave(self, *args):
		if self._filename is None:
			return self.OnSaveAs(self, *args)
		visitor = SaveVisitor(self._tree, self._node_map)
		self._view.graph().accept(visitor)
		visitor.removeDeleted()
		self._node_map.save()
		self._view.graph().clearHistory()
		return 1

	def OnNew(self, *args):
		if not self.PromptModified(): return
		self.Clear()

	def Clear(self):
		self._view.graph().clear();
		self.setFilename(None)
		self._active_id = None
		self._node_map = None

	def setFilename(self, fn):
		self._filename = fn
		if fn is None:
			self.SetTitle('CSP Layout')
		else:
			self.SetTitle(fn)

	def OnOpen(self, *args):
		if not self.PromptModified(): return
		result = self._open_dialog.ShowModal()
		if result == wxID_OK:
			fn = self._open_dialog.GetPath()
			if self.Open(fn):
				self._openMenu.AddItem(fn)

	def OnRevert(self, *args):
		if self._view.graph().isModified():
			result = wxMessageDialog(self, "Warning: all changes will be lost.  Are you sure?", "Revert", style = wxYES|wxCANCEL|wxICON_EXCLAMATION).ShowModal()
			if result == wxID_YES:
				self.Open(self._filename);

	def OnSaveAs(self, *args):
		saved = 0
		result = self._save_dialog.ShowModal()
		if result == wxID_OK:
			fn = self._save_dialog.GetPath()
			if self.FilePathToXMLPath(fn) is None:
				wxMessageDialog(self, "File '%s' is not in the XML path (%s)" % (fn, self._xml_path), "Save As", style = wxOK|wxICON_ERROR).ShowModal()
				return 0
			node_map = self.GetOrCreateNodeMap()
			node_map.graph().setPath(DataTree.DataPath(fn, self._xml_path))
			self.setFilename(fn)
			saved = self.OnSave(*args)
			if saved:
				self._openMenu.AddItem(fn)
		return saved

	def OnInsertGroup(self, *args):
		self.PromptNotImplemented("Insert group")

	def OnInsertModel(self, *args):
		result = self._insert_dialog.ShowModal()
		if result == wxID_OK:
			fn = self._insert_dialog.GetPath()
			model = self.InsertModel(fn)
			if model:
				self._insertMenu.AddItem(fn)
				self.UpdateTree(model)

	def InsertModel(self, fn):
		model = None
		if fn.endswith('.xml'): # FeatureModel
			model = self.InsertFeatureModel(str(fn))
		else:
			# 3d model
			wxMessageDialog(self, 'Inserting 3D models directly is not supported yet.', 'Insert', style = wxOK|wxICON_ERROR).ShowModal()
			#model = self.Insert3dModel(str(fn))
		return model

	def FilePathToXMLPath(self, fn):
		xml = os.path.abspath(self._xml_path)
		fn = os.path.abspath(fn)
		if fn.startswith(xml):
			fn = fn[len(xml):]
			if fn.endswith('.xml'):
				return fn[:-4].replace(os.sep, '.')
		return None

	def InsertFeatureModel(self, fn):
		"""
		Insert an xml file containing an FeatureModel definition.  Creates a new
		LayoutFeature in the active group to position the FeatureModel.
		"""
		print 'load feature model', fn
		path = self.FilePathToXMLPath(fn)
		if not path:
			wxMessageDialog(self, 'File "%s" is not a valid xml file in the xml data directory (%s)' % (fn, self._xml_path), "Load", style = wxOK|wxICON_ERROR).ShowModal()
			return None

		node_map = self.GetOrCreateNodeMap()
		parent = self._view.graph().getActiveGroup()
		parent_node = node_map.getGroup(parent)
		assert(parent_node is not None)

		# we are inserting a model into an existing feature group, so we need to create a new
		# featurelayout xml node that will hold the offset and orientation.
		try:
			feature_layout = DataTree.FeatureLayout.New(parent_node, path)
		except DataTree.RecursionError:
			wxMessageDialog(self, 'Inserting "%s" into the active group would create an infinite recursion.' % path, "Load", style = wxOK|wxICON_ERROR).ShowModal()
			return None

		# stop the 3d rendering loop while we modify the scene graph
		self._view.lock();
		feature_layout.realizeAsChild(parent, node_map)
		# restart the 3d rendering loop
		self._view.unlock();

		model = node_map.getRoot()
		self.UpdateTree(model)

		print 'InsertFeatureModel', parent, parent_node, model, node_map.get(model)
		command = cLayout.InsertDeleteCommand(parent, model, 1, "insert")
		self._view.graph().addCommand(command)
		command.thisown = 0

		return model

	def Insert3dModel(self, fn):
		model = cLayout.LayoutModel()
		model.load(fn)
		# FIXME this inserts the model into the scene but totally fails to set up the XML structures needed for saving
		self.AddNodeToActiveGroup(model)
		return model

	def AddNodeToActiveGroup(self, node):
		parent = self._view.graph().getActiveGroup()
		assert(node is not None)
		assert(parent is not None)
		command = cLayout.InsertDeleteCommand(parent, node, 1, "insert")
		self._view.graph().runCommand(command)
		command.thisown = 0
		node.thisown = 0
		self.UpdateTree(node)

	def PromptNotImplemented(self, title):
		wxMessageDialog(self, "Sorry, not implemented yet!", title, style = wxOK|wxICON_INFORMATION).ShowModal()

	def PromptModified(self):
		if self._view.graph().isModified():
			result = wxMessageDialog(self, "Save changes first?", "WARNING", style = wxYES|wxNO|wxCANCEL|wxICON_EXCLAMATION).ShowModal()
			if result == wxID_CANCEL:
				return 0
			if result == wxID_YES:
				return self.OnSave()
		return 1

	def OnOpenMRU(self, event, fn):
		if not self.PromptModified(): return
		if self.Open(fn):
			self._openMenu.AddItem(fn)

	def Open(self, fn):
		if not os.path.exists(fn):
			wxMessageDialog(self, 'File "%s" not found' % fn, 'Open', style = wxOK|wxICON_ERROR).ShowModal()
			return 0
		if self.OpenFeature(fn):
			self.setFilename(fn)
		return 1

	def OnInsertMRU(self, event, fn):
		self._insertMenu.AddItem(fn)
		model = self.InsertModel(fn)

	def MakeTree(self, parent):
		tID = wxNewId()
		tree = wxTreeCtrl(parent, tID, wxDefaultPosition, wxSize(400, 100), wxTR_HAS_BUTTONS|wxTR_MULTIPLE|wxTR_EXTENDED|wxSUNKEN_BORDER|wxVSCROLL|wxTR_HAS_BUTTONS|wxTR_TWIST_BUTTONS)
		tree.SetMinSize((400, 100))
		#tree = wxTreeCtrl(parent, tID, wxDefaultPosition, wxDefaultSize, wxTR_HAS_BUTTONS|wxSUNKEN_BORDER|wxVSCROLL)
		##root = tree.AddRoot("ROOT")
		EVT_TREE_SEL_CHANGING(tree, tID, self.OnSelChanging)
		EVT_TREE_SEL_CHANGED(tree, tID, self.OnSelChanged)
		EVT_TREE_ITEM_ACTIVATED(tree, tID, self.OnItemActivated)
		EVT_TREE_ITEM_RIGHT_CLICK(tree, tID, self.OnItemActivated)
		self._disable_selection_handling = 0
		return tree

	def OnExit(self, event):
		if self._view.graph().isModified():
			result = wxMessageDialog(self, "Quit without saving?", "Quit", style = wxOK|wxCANCEL|wxICON_EXCLAMATION).ShowModal()
			if result != wxID_OK: return
		self.Close()

	def OnItemActivated(self, event):
		id = event.GetItem()
		path = self.GetItemPath(id)
		if path:
			fnp = cLayout.vector_LayoutNodePtr()
			for node in path[:-1]: fnp.push_back(node)
			self._view.centerViewOnNode(fnp, path[-1])

	def GetItemPath(self, item):
		path = []
		while item.IsOk():
			path.append(self._tree.GetPyData(item))
			item = self._tree.GetItemParent(item)
		path.reverse()
		return path

	def OnSelChanging(self, event):
		if self._disable_selection_handling:
			return
		# the following doesn't work correctly for single selects outside
		# the active group.  wxwindows doesn't provide enough support for
		# multi-select tree controls to do any better, so for now we allow
		# all changes and cleanup afterward if the resulting selection is
		# invalid
		return

		# disallow multiple selections in different groups
		parent = self._tree.GetItemParent(event.GetItem())
		for item in self._tree.GetSelections():
			if parent != self._tree.GetItemParent(item):
				event.Veto()
				return

	def OnSelChanged(self, event):
		if self._disable_selection_handling:
			event.Skip()
			return

		# user has change the selection by clicking on the tree control

		# verify that the new selection is valid (all selected nodes in
		# the same group).  unfortunately, poor support for multi-select
		# tree controls in wxwindows prevents us from catching bad changes
		# before they happen in the OnSelChanging handler.
		invalid = 0
		parent = self._tree.GetItemParent(event.GetItem())
		for item in self._tree.GetSelections():
			if parent != self._tree.GetItemParent(item):
				invalid = 1

		# if multiple nodes in different groups are selected, clear all
		# selection before adding the new one (note that wxwindows only
		# returns one item in the event even if several were added to the
		# selection).
		# FIXME since wxtreectrl doesn't handle multiple selections gracefully
		# we always deselect first.
		if 1 or invalid:
			self._view.graph().deselectAll();

		id = event.GetItem()
		name = self._tree.GetItemText(id)
		nodelist = []
		while id:
			node = self._tree.GetPyData(id)
			nodelist.append(node)
			id = self._tree.GetItemParent(id)
		nodelist.reverse()
		print 'DEEP SELECT'
		self._view.graph().deepSelect(nodelist[1:])

		event.Skip()

	def OnLeftDown(self, event):
		pt = event.GetPosition();
		item, flags = self._tree.HitTest(pt)
		print "OnLeftDClick: %s\n" % self._tree.GetItemText(item)
		event.Skip()

	def OnViewX(self, event):
		self._view.setViewX();
		event.Skip()

	def OnViewY(self, event):
		self._view.setViewY();
		event.Skip()

	def OnViewZ(self, event):
		self._view.setViewZ();
		event.Skip()

	def OnMove(self, event):
		self._view.setMoveMode()
		event.Skip()

	def OpenFeature(self, file):
		"""
		Load a feature group or feature model from the specified file.  The existing
		graph, if any, will be discarded.  If file represents a feature model, a
		default feature group is created at the root of the new graph.
		"""
		if not os.path.abspath(file).startswith(os.path.abspath(self._xml_path)):
			wxMessageDialog(self, 'Cannot open features that are outside the XML path.  To change the '
			                      'XML path, rerun the program with the --xml option.\n\n'
			                      'The current path is "%s"' % self._xml_path,
			                'Error loading feature', style = wxOK|wxICON_ERROR).ShowModal()
			return 0

		graph = DataTree.Node.ParseFile(file, self._xml_path)

		if not graph.containsNode(DataTree.CustomLayoutModel):
			wxMessageDialog(self, "Cannot open leaf node FeatureModels; use insert instead.", "Error loading feature", style = wxOK|wxICON_ERROR).ShowModal()
			return 0

		# stop the 3d rendering loop while we load the new scene graph
		self._view.lock();
		node_map = graph.realize()
		self._node_map = node_map
		node = node_map.getRoot()
		assert(node is not None and node.isGroup())
		self._view.graph().setRoot(node)
		# restart the 3d rendering loop
		self._view.unlock();

		node.thisown = 0
		self.UpdateTree(node)
		return 1

	def OnRotate(self, event):
		self._view.setRotateMode()
		event.Skip()

	def onMoveMode(self, event):
		self._tool = 'move'
		self.UpdateTools()

	def onRotateMode(self, event):
		self._tool = 'rotate'
		self.UpdateTools()

	def UpdateTools(self):
		for tool in self._tools:
			active = self._tool == tool
			self._toolset[tool].SetActive(active)

	def UpdateTree(self, show=None):
		visitor = TreeUpdateVisitor(self._tree)
		self._view.graph().accept(visitor)
		self._active_id = None
		self.UpdateActivePath(self._view.graph().getActivePath())
		if show is not None:
			item = visitor.getItem(show)
			if item is None:
				print 'could not find newly added item in tree'
			else:
				self._tree.EnsureVisible(item)

	def GetOrCreateNodeMap(self):
		if self._node_map is None:
			root = self._view.graph().getRoot()
			assert(root is not None)
			assert(len(root.children()) == 0)
			path = DataTree.DataPath.NoPath(self._xml_path)
			graph, group = DataTree.CustomLayoutModel.New(path)
			self._node_map = DataTree.NodeMap(graph)
			self._node_map.add(group, root)
			self._node_map.addGroup(group, root)
			self._node_map.getRoot()  # flush the initial root
		return self._node_map


class Init(wxApp):
	def __init__(self, view, xml_path, model_path):
		self._view = view
		self._xml_path = xml_path
		self._model_path = model_path
		wxApp.__init__(self, 0)

	def OnInit(self):
		frame = MainWindow(self._view, self._xml_path, self._model_path, NULL, -1, "CSP Theater Layout Tool")
		frame.Show(true)
		self.SetTopWindow(frame)
		return true

