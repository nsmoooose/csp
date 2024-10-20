class Action(object):
    """Base class for all actions that can be undone."""

    def __init__(self, targetDocument):
        # The Document modified by this action
        self.targetDocument = targetDocument

    def GetDescription(self):
        """Inheriting classes should return the description to be displayed in a action history."""
        return ""

    def GetImageName(self):
        """Inheriting classes should return the name of the image file to be displayed in a action history."""
        return ""

    def Execute(self):
        """This methode should be called only once."""
        if not self.Do():
            return

        self.targetDocument.actionHistory.Add(self)

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
