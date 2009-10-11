class Action(object):
	"""Base class for all actions that can be undone."""
	
	NotExecuted = 0
	Done = 1
	Undone = 2
	Canceled = 3
	
	def __init__(self, targetDocument):
		# The Document modified by this action
		self.targetDocument = targetDocument
		
		# Indicates the current state of this action:
		# - NotExecuted: Execute() must be called prior to any other action
		# - Done: Undo() can be called
		# - Undone: Do() can be called
		# - Canceled: this action must be removed from all ActionHistory
		self.state = self.NotExecuted
		
		# Set of all newer actions that mask this action
		self.maskers = set()
		
		# Set of all older actions that are masked by this action
		#self.maskedActions = set()
	
	def Cancel(self):
		self.state = self.Canceled
		self.targetDocument = None
		self.maskers = None
		#self.maskedActions = None
	
	def GetDescription(self):
		"""Inheriting classes should return the description to be displayed in a action history."""
		return ""

	def GetImageName(self):
		"""Inheriting classes should return the name of the image file to be displayed in a action history."""
		return ""
	
	def Execute(self):
		"""This methode should be called only once."""
		if self.state != self.NotExecuted:
			print "BUG: Action.Execute() should be called only once"
			return
		
		if not self.Do():
			return
		
		self.state = self.Done
		
		self.targetDocument.actionHistory.Add(self)
		
		allReferrers = self.targetDocument.GetAllReferrers()
		allReferrers.discard( self.targetDocument )
		for document in allReferrers:
			document.actionHistory.Add(self)
	
	def IsIrreversible(self):
		"""Inheriting classes should return True if they implement actions that are irreversibles."""
		return False
	
	def Do(self):
		"""This methode should only be called by ActionHistory.
		Inheriting classes should implement this method to do the actual job of this action.
		Return True if successful.
		Return False in case of failure to prevent the action from being added to action history."""
		return False
	
	def Undo(self):
		"""This methode should only be called by ActionHistory.
		Inheriting classes should implement this method to reverse the job of this action.
		Return True if successful.
		Return False in case of failure."""
		return False
	
	def IsMaskedByOtherThan(self, action):
		if action in self.maskers:
			return len(self.maskers) > 1
		else:
			return len(self.maskers) > 0
