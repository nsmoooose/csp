// Combat Simulator Project
// Copyright (C) 2004 The Combat Simulator Project
// http://csp.sourceforge.net
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
// Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.


/**
 * @file glDiagnostics.h
 *
 **/

#ifndef __GLDIAGNOSTICS_H__
#define __GLDIAGNOSTICS_H__


#include <vector>
#include <string>


/** Tool for capturing (most of) the current glState, and logging
 *  state differences.
 *
 *  Example:
 *
 *  GlStateSnapshot snapshot;
 *  // some gl calls here
 *  snapshot.logDiff();  // log any state changes
 */
class GlStateSnapshot {

	std::vector<std::string> m_StateLog;
	void log(std::string id, unsigned key, int type);
	void _logDiff(std::string const &label, GlStateSnapshot const &other);
	bool _hasChanged(GlStateSnapshot const &other);

public:

	/** Create a new snapshot of the current gl state.
	 */
	GlStateSnapshot();

	/** Recapture the current state.  This method only needs to be
	 *  called if you are reusing a GlStateSnapshot instance.  The
	 *  state is automatically captured when an instance of this
	 *  class is created.
	 */
	void capture();

	/** Log differences between this snapshot and another.
	 *  @param label An arbitrary label to identify this diff in the log.
	 *  @param other Another GlStateSnapshot to compare to, or 0 to
	 *    compare against the current gl state.
	 */
	void logDiff(std::string const &label, GlStateSnapshot const *other=0);

	/** Test for differences between this snapshot and another.
	 *  @param other Another GlStateSnapshot to compare to, or 0 to
	 *    compare against the current gl state.
	 */
	bool hasChanged(GlStateSnapshot const *other=0);
};


#endif // __GLDIAGNOSTICS_H__
