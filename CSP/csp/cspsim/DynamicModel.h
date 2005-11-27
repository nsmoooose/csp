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
 * @file DynamicModel.h
 *
 * Provides an interface for vehicle subsystems to interact with the 3D world.
 **/

#ifndef __CSPSIM_DYNAMICMODEL_H__
#define __CSPSIM_DYNAMICMODEL_H__

#include <sigc++/signal.h>
#include <csp/csplib/util/Ref.h>
#include <ObjectModel.h>  // TODO forward declare scenemodel and move signalCreateSceneModel to a cpp file


namespace osg { class Group; }

/** Interface provided by DynamicObjects to allow System classes to interact
 *  with the scene.
 */
class DynamicModel: public simdata::Referenced {
public:
	// const DynamicModel can add handlers but not send signals.
	void addInternalViewSignalHandler(sigc::slot<void, bool> const &slot) const { m_InternalViewSignal.connect(slot); }
	void addCreateSceneModelHandler(sigc::slot<void, osg::Group*> const &slot) const { m_CreateSceneModelSignal.connect(slot); }
	void addDeleteSceneModelHandler(sigc::slot<void> const &slot) const { m_DeleteSceneModelSignal.connect(slot); }

	void signalCreateSceneModel(SceneModel *model) {
		assert(model);
		m_CreateSceneModelSignal.emit(model->getDynamicGroup());
	}
	void signalDeleteSceneModel() { m_DeleteSceneModelSignal.emit(); }
	void signalInternalView(bool internal_view) { m_InternalViewSignal.emit(internal_view); }

private:
	mutable sigc::signal<void, osg::Group*> m_CreateSceneModelSignal;
	mutable sigc::signal<void> m_DeleteSceneModelSignal;
	mutable sigc::signal<void, bool> m_InternalViewSignal;
};


#endif // __CSPSIM_DYNAMICMODEL_H__

