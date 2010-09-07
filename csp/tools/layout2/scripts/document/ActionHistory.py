from csp.base.signals import Signal

class ActionHistory(object):
	def __init__(self, documentOwner):
		# The Document that own this ActionHistory
		self.documentOwner = documentOwner

		# The done actions stack (from oldest to newest).
		self.doneActions = []

		# The undone actions stack (from newest to oldest).
		self.undoneActions = []

		# Signal when this ActionHistory has changed
		self.changedSignal = Signal()

	def Dispose(self):
		self.doneActions = None
		self.undoneActions = None
		self.changedSignal.Dispose()
		self.changedSignal = None

	def GetChangedSignal(self):
		return self.changedSignal

	def EmitChangedSignal(self):
		self.changedSignal.Emit(self)

	def HasAction(self):
		return True if self.doneActions or self.undoneActions else False

	def CanUndo(self):
		if not self.doneActions:
			return False

		return False if self.doneActions[-1].IsIrreversible() else True

	def CanRedo(self):
		return True if self.undoneActions else False

	def Add(self, action):
		if action.IsIrreversible():
			self.doneActions = []
		self.undoneActions = []
		self.doneActions.append(action)
		self.EmitChangedSignal()

	def Undo(self):
		if not self.doneActions:
			return

		action = self.doneActions[-1]
		if action.IsIrreversible():
			return

		if not action.Undo():
			return

		self.undoneActions.append( self.doneActions.pop() )
		self.EmitChangedSignal()

	def Redo(self):
		if not self.undoneActions:
			return

		action = self.undoneActions[-1]

		if not action.Do():
			return

		self.doneActions.append( self.undoneActions.pop() )
		self.EmitChangedSignal()
