// CSPLayout
// Copyright 2005 Mark Rose <mkrose@users.sourceforge.net>
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

#ifndef __CSP_LAYOUT_VIEWEVENTHANDLER_H__
#define __CSP_LAYOUT_VIEWEVENTHANDLER_H__

#include <osgProducer/ViewerEventHandler>
#include <osgDB/WriteFile>

class View;

/** A viewer event handler that implements some of the global keyboard
 *  commands for the 3d interface.  Overrides/extends some of the default
 *  handlers provided by osgProducer::ViewerEventHandler.
 */
class ViewEventHandler: public osgProducer::ViewerEventHandler {
	View *_view;

public:
	ViewEventHandler(View *view, osgProducer::OsgCameraGroup* cg): osgProducer::ViewerEventHandler(cg), _view(view) { }

	/** Handle keyboard events, forwarding some keypresses through the view callback
	 *  dispatcher to allow other Python to have a shot at them.  An alternative
	 *  would be to wrap ViewEventHandler with SWIG using directors, and subclass
	 *  it in Python (which would also require GUIEventAdapter to be wrapped).
	 */
	virtual bool handle(const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& aa);

	/** Set help strings for some of the key bindings to be displayed in the 3D view
	 *  when the 'h' key is pressed.
	 */
	virtual void getUsage(osg::ApplicationUsage& usage) const;

	/** Save the scene to a file in .osg format.  Includes only the FeatureGraph,
	 *  omitting viewer aids such as the dynamic grid (which is the primary reason
	 *  for overriding the default implementation in the base class).
	 */
	void saveScene();
};

#endif // __CSP_LAYOUT_VIEWEVENTHANDLER_H__
