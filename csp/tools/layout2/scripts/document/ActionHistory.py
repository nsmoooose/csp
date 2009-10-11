from __future__ import with_statement

from csp.base.signals import Signal

class ActionHistory(object):
	def __init__(self, documentOwner):
		# The Document that own this ActionHistory
		self.documentOwner = documentOwner
		
		# The Action stack.
		# Action can be shared by several ActionHistory
		self.actions = []
		
		# The canceled actions that need to be removed from self.actions
		self.canceledActions = set()
		
		# Signal when this ActionHistory has changed
		self.changedSignal = Signal()

	def Dispose(self):
		self.actions = None
		self.canceledActions = None
		self.changedSignal.Dispose()
		self.changedSignal = None
	
	def GetChangedSignal(self):
		return self.changedSignal
	
	def EmitChangedSignal(self):
		self.changedSignal.Emit(self)
	
	def VisibleActions(self):
		for action in self.actions:
			if action.state == action.Canceled:
				self.canceledActions.add(action)
				continue
			else:
				yield action
	
	def HasAction(self):
		with Cleaner(self):
			for action in self.VisibleActions():
				return True
			return False
	
	def CanUndo(self):
		with Cleaner(self):
			for action in self.VisibleActions():
				if action.state == action.Done:
					if action.maskers or action.IsIrreversible():
						return False
					else:
						return True
			return False
	
	def CanRedo(self):
		with Cleaner(self):
			for action in self.VisibleActions():
				return action.state == action.Undone
			return False
	
	def Add(self, action):
		with Cleaner(self):
			self.CancelLastUndoneActions()
			self.MaskPreviousActionWith(action)
			self.actions.insert(0, action)
			self.EmitChangedSignal()
	
	def Undo(self):
		with Cleaner(self):
			actionIterator = iter( self.VisibleActions() )
			try:
				# Find the newest Done action
				while True:
					action = actionIterator.next()
					if action.state == action.Done:
						break
				
				# Masked actions can't be undone
				if action.maskers:
					return
				
				# Irreversible actions can't be undone
				if action.IsIrreversible():
					return
				
				# Undo the action
				if not action.Undo():
					return
				
				action.state = action.Undone
				
				# Unmask the previous action if necessary
				previousAction = actionIterator.next()
				previousAction.maskers.discard(action)
			except StopIteration:
				pass
			
			self.EmitChangedSignal()
	
	def Redo(self):
		with Cleaner(self):
			# Find the oldest Undone action
			action = None
			actionIterator = iter( self.VisibleActions() )
			try:
				action = actionIterator.next()
				if action.state == action.Done:
					return
				
				while True:
					previousAction = actionIterator.next()
					if previousAction.state == action.Done:
						break
					
					action = previousAction
			except StopIteration:
				previousAction = None
			
			if action is None:
				return
			
			# Redo it
			if not action.Do():
				return
			
			action.state = action.Done
			
			# Mask the previous action if necessary
			if previousAction is not None:
				if previousAction.targetDocument is not action.targetDocument:
					previousAction.maskers.add(action)
			
			self.EmitChangedSignal()
	
	def CancelLastUndoneActions(self):
		with Cleaner(self):
			for action in self.VisibleActions():
				if action.state == action.Undone:
					action.Cancel()
					self.canceledActions.add(action)
				else:
					return
	
	def MaskPreviousActionWith(self, action):
		with Cleaner(self):
			for action in self.VisibleActions():
				if action.targetDocument is not action.targetDocument:
					action.maskers.add(action)
				return
	
	def Clean(self):
		if self.canceledActions:
			for action in self.canceledActions:
				self.actions.remove( action )
			
			self.canceledActions = set()


class Cleaner(object):
	def __init__(self, actionHistory):
		self.actionHistory = actionHistory
	
	def __enter__(self):
		pass
	
	def __exit__(self, exc_type, exc_value, traceback):
		self.actionHistory.Clean()
		return False
