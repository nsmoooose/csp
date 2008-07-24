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
 * @file SoundModel.h
 *
 * SoundModel manages sounds for a single DynamicObject.
 **/


#ifndef __SOUNDMODEL_H__
#define __SOUNDMODEL_H__

#include <csp/csplib/util/Ref.h>
#include <csp/csplib/data/Vector3.h>
#include <osg/ref_ptr>
#include <map>

namespace osg { class Node; }
namespace osg { class Group; }
namespace osgAL { class SoundNode; }
namespace osgAL { class SoundState; }

namespace csp {

class SceneModel;
class SoundEffect;

/** SoundModel attaches to a DynamicObject and provides a simple interface for
 *  Systems that want to add play sounds associated with the object.  Supports
 *  three types of sounds:
 *
 *    .  External sounds, which are heard at full volume from external views,
 *       but muffled in internal views.
 *
 *    .  Internal sounds, which are only heard in internal views.
 *
 *    .  Headset sounds, which are position-independent sounds that are only
 *       heard in internal views.
 */
class SoundModel: public Referenced {
	typedef osg::ref_ptr<osgAL::SoundNode> SoundNode;
	typedef osg::ref_ptr<osgAL::SoundState> Sound;
	typedef osg::ref_ptr<osg::Node> Node;

public:
	typedef enum { EXTERNAL, INTERNAL, HEADSET } Mode;

	SoundModel();

	void bind(SceneModel *scene_model);
	void unbind(SceneModel *scene_model);

	void addExternalSound(osgAL::SoundState *sound, Vector3 const &offset=Vector3::ZERO, Vector3 const &direction=Vector3::YAXIS);
	void addInternalSound(osgAL::SoundState *sound, Vector3 const &offset=Vector3::ZERO, Vector3 const &direction=Vector3::YAXIS);
	void addHeadsetSound(osgAL::SoundState *sound);
	bool removeSound(osgAL::SoundState *sound);
	bool removeSound(Ref<SoundEffect> const &effect);
	bool removeAndDeleteSound(Ref<SoundEffect> &effect);

	void setInternalView(bool internal=true);

protected:
	virtual ~SoundModel();

private:
	osg::Node *placeNode(SoundNode &sound, Vector3 const &offset, Vector3 const &direction) const;

	struct SoundData {
		Node scene_node;
		SoundNode sound_node;
		Mode mode;
	};

	typedef std::map<osgAL::SoundState*, SoundData> SoundIndex;
	SoundIndex m_SoundIndex;

	osg::ref_ptr<osg::Group> m_SoundGroup;
	bool m_InternalView;
};


} // namespace csp

#endif // __SOUNDMODEL_H__

