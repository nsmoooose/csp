// CSPLayout
// Copyright 2003, 2005 Mark Rose <mkrose@users.sourceforge.net>
//
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either version 2
// of the License, or (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, US


#ifndef __CSP_LAYOUT_UNDO_H__
#define __CSP_LAYOUT_UNDO_H__

#include <osg/Referenced>
#include <string>
#include <deque>


/** Abstract base class for commands that can be undone/redone.
 */
class Command: public osg::Referenced {
public:
	virtual ~Command() { }
	virtual void undo() = 0;
	virtual void redo() = 0;
	virtual std::string label() const = 0;
};


/** CommandHistory manages undo/redo operations for a document.  It maintains
 *  two stacks of Command instances, one for applied commands that can be undone,
 *  and one for undone commands that can be redone.  When a command is undone, it
 *  is popped from the first stack and pushed onto the second stack.  The converse
 *  is true for redo events.  The second stack is cleared whenever a new action is
 *  applied.
 */
class CommandHistory {
	std::deque< osg::ref_ptr<Command> > m_Undo;
	std::deque< osg::ref_ptr<Command> > m_Redo;
public:
	/** Flush all commands (undo and redo).
	 */
	void clear() {
		m_Undo.clear();
		m_Redo.clear();
	}

	/** Get the undo label of the most recently applied command, or an empty
	 *  string if the command history is empty.
	 */
	std::string getUndoLabel() const { return m_Undo.empty() ? "" : m_Undo.back()->label(); }

	/** Get the redo label of the most recently undone command, or an empty
	 *  string if the redo stack is empty.
	 */
	std::string getRedoLabel() const { return m_Redo.empty() ? "" : m_Redo.back()->label(); }

	/** Test if the command history has entries.  If this method returns false the document
	 *  should be unmodified, modulo changes that are not tracked by this command history.
	 */
	bool hasUndo() const { return !m_Undo.empty(); }

	/** Test if any commands have been undone and can be reapplied.
	 */
	bool hasRedo() const { return !m_Redo.empty(); }

	/** Undo the most recently applied command, if any.
	 */
	void undo() {
		if (hasUndo()) {
			m_Undo.back()->undo();
			m_Redo.push_back(m_Undo.back());
			m_Undo.pop_back();
		}
	}

	/** Redo the most recently undone command, if any.  Undone commands can not be redone
	 *  if new commands have since been added to the command history.
	 */
	void redo() {
		if (hasRedo()) {
			m_Redo.back()->redo();
			m_Undo.push_back(m_Redo.back());
			m_Redo.pop_back();
		}
	}

	/** Run a command and add it to the command history.
	 */
	void runCommand(Command *command) {
		addCommand(command);
		command->redo();
	}

	/** Add a command to the command history, assuming it has already been applied to the
	 *  document.
	 */
	void addCommand(Command *command) {
		assert(command);
		m_Undo.push_back(command);
		m_Redo.clear();
	}
};

#endif //  __CSP_LAYOUT_UNDO_H__
