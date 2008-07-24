// Combat Simulator Project
// Copyright (C) 2005 The Combat Simulator Project
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
 * @file SceneConstants.h
 *
 **/

#ifndef __CSPSIM_SCENE_CONSTANTS_H__
#define __CSPSIM_SCENE_CONSTANTS_H__

#include <csp/csplib/util/Namespace.h>

namespace csp {

/** NodeVisitor traversal mask bits that are set when rendering the scene.
 *
 *  UPDATE_ONLY - set during the update traversal.
 *  CULL_ONLY - set during the cull traversal.
 *  LABELS - set only if labels are turned on.
 *  NEAR - set for traversals of the near scene graph.
 *  FAR - set for traversals of all far scene graphs.
 *
 *  For example, the NEAR mask bit is used to enable rendering of canopy
 *  reflections in internal views by setting the node mask of a canopy model
 *  with a cockpit reflection texture map to NEAR.  That node is culled in
 *  external views, and a second canopy model with node mask FAR and an
 *  environmental reflection texture map is rendered instead.
 *
 *  Note that the OSG enables a node if there is any overlap between the node
 *  mask and the traversal mask.  So for example, there is no way to set a node
 *  mask such that a node is only enabled for update traversals (UPDATE_ONLY)
 *  that occur in the near scene graph (NEAR).
 */
class SceneMasks {
public:
	typedef enum {
		UPDATE_ONLY = 0x0001,
		CULL_ONLY = 0x0002,
		NORMAL = 0x0003,
		LABELS = 0x0100,
		NEAR = 0x200,
		FAR = 0x400
	} NodeMask;
};

} // namespace csp

#endif // __CSPSIM_SCENE_CONSTANTS_H__

