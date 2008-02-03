// CSPLayout - Copyright 2003-2005 Mark Rose <mkrose@users.sourceforge.net>
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

#ifndef __CSP_LAYOUT_HANDLE_H__
#define __CSP_LAYOUT_HANDLE_H__

#include <osg/Vec3>
#include <osg/Vec4>

namespace osg { class Geode; }
namespace osg { class Geometry; }

namespace csp {
namespace layout {

/** Utility class for creating scene graph elements that act as handles
 *  for manipulating/visualizing layout nodes.
 */
class Handle {
public:
	/** Make an octahedron geometry of the specified color, position, and size.
	 */
	static osg::Geometry *makeDiamond(osg::Vec4 const &color, osg::Vec3 const &pos, float size);

	/** Create a handle geode of the specified color, position, and size.
	 */
	static osg::Geode *makeHandle(osg::Vec4 const &color, osg::Vec3 const &pos=osg::Vec3(0, 0, 1), float size=1.0);
};

}	// end namespace layout
}	// end namespace csp

#endif // __CSP_LAYOUT_HANDLE_H__

