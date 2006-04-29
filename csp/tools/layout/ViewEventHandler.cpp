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


#include "View.h"
#include "FeatureGraph.h"
#include "ViewEventHandler.h"
#include <osgDB/WriteFile>


void ViewEventHandler::saveScene() {
	osg::Node* node = _view->graph()->getRoot()->getGraph();
	if (node) {
		if (osgDB::writeNodeFile(*node,_writeNodeFileName.c_str())) {
			std::cout << "model saved to file " << _writeNodeFileName << std::endl;
		}
	}
}

bool ViewEventHandler::handle(const osgGA::GUIEventAdapter& ea,osgGA::GUIActionAdapter& aa) {
	if (ea.getEventType() != osgGA::GUIEventAdapter::KEYDOWN) return false;
	switch (ea.getKey()) {
		case 'f':
		case osgGA::GUIEventAdapter::KEY_Help:
		case 'h':
		case 'O':
		case osgGA::GUIEventAdapter::KEY_Print:
		case 's':
			return osgProducer::ViewerEventHandler::handle(ea, aa);
		case 'o':
			saveScene();
			return true;

		case osgGA::GUIEventAdapter::KEY_Left:
			_view->sendKey("LEFT");
			return true;
		case osgGA::GUIEventAdapter::KEY_Right:
			_view->sendKey("RIGHT");
			return true;
		case osgGA::GUIEventAdapter::KEY_Up:
			_view->sendKey("UP");
			return true;
		case osgGA::GUIEventAdapter::KEY_Down:
			_view->sendKey("DOWN");
			return true;
		case osgGA::GUIEventAdapter::KEY_Space:
			_view->sendKey("SPACE");
			return true;

		default:
			break;
	}
	return false;
}

void ViewEventHandler::getUsage(osg::ApplicationUsage& usage) const {
	usage.addKeyboardMouseBinding("f","Toggle fullscreen");
	usage.addKeyboardMouseBinding("h","Display help");
	usage.addKeyboardMouseBinding("o","Write scene graph to \"saved_model.osg\"");
	usage.addKeyboardMouseBinding("O PrtSrn","Write camera images to \"saved_image*.rgb\"");
	usage.addKeyboardMouseBinding("s","Toggle intrumention");
}

