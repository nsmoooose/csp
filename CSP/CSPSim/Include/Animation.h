// Combat Simulator Project - CSPSim
// Copyright (C) 2002 The Combat Simulator Project
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
 * @file Animation.h
 *
 **/

#ifndef __ANIMATION_H__
#define __ANIMATION_H__

#include <Bus.h>

#include <osg/Node>

#include <SimData/Object.h>
#include <SimData/Vector3.h>
#include <SimData/Matrix3.h>
#include <SimData/Key.h>
#include <SimData/InterfaceRegistry.h>
#include <SimData/osg.h>

#include <vector>
#include <cstdlib>


/**
 * Base class for animation callbacks.
 *
 * This class serves as an update callback for
 * a transformation node that animates part of
 * a model scene graph.  Update requests will
 * modify the transform if only if necessary.
 * The callback maintains a dirty counter which
 * is used to determine if a particular node
 * transform is stale.  To test this, a separate
 * counter is stored in the user data slot of
 * the transform and updated accordingly.  For
 * continuous animation loops, the dirty counter
 * is not used and the transform is updated after
 * every callback.
 */
class AnimationCallback: public osg::NodeCallback {
protected:
	int m_DirtyCount;
	std::string m_ChannelName;
	class UpdateCount: public osg::Referenced {
		int m_UpdateCount;
	public:
		UpdateCount(): m_UpdateCount(0) {}
		inline void updateCount(int n) { m_UpdateCount = n; }
		inline int getCount() const { return m_UpdateCount; }
	};
public:
	AnimationCallback(): m_DirtyCount(0) {}
	virtual ~AnimationCallback() {}
	inline void dirty() { ++m_DirtyCount; }
	inline bool needsUpdate(osg::Node& node) {
		UpdateCount *count = dynamic_cast<UpdateCount*>(node.getUserData());
		assert(count);
		if (count->getCount() < m_DirtyCount) {
			count->updateCount(m_DirtyCount);
			return true;
		}
		return false;
	}
	bool needsUpdate(osg::NodeCallback& node_callback);
	inline std::string const &getChannelName() const { return m_ChannelName; }
	inline void setChannelName(std::string const &name) { m_ChannelName = name; }
	inline void bind(osg::Node &node) {
		// XXX
		//node.setCullCallback(this);
		node.setUpdateCallback(this);
		node.setUserData(new UpdateCount);
		dirty();
	}
	void bind(osg::NodeCallback &node_callback);
	virtual void bindChannel(simdata::Ref<const DataChannelBase>)=0;
	virtual void setDefault(float) {}
};


/**
 * class Animation - base class for animated 3d model
 * components.
 *
 * Animation instances are bound to ObjectModels to
 * provide data about particular model animations.
 * Animation instances, like ObjectModels, are shared
 * by multiple SceneModel instances.
 */
class Animation: public simdata::Object
{
protected:
	simdata::Key m_ModelID;
	std::string m_ChannelName;
	int m_LOD;
	float m_Default;
	osg::ref_ptr<AnimationCallback> m_Callback;

	virtual AnimationCallback *newCallback(osg::Node *node, AnimationCallback *callback) const {
		assert(node);
		assert(callback);
		callback->bind(*node);
		callback->setChannelName(m_ChannelName);
		return callback;
	}

	virtual AnimationCallback *newCallback(osg::NodeCallback* node_callback, AnimationCallback *callback) const {
		assert(node_callback);
		assert(callback);
		callback->bind(*node_callback);
		callback->setChannelName(m_ChannelName);
		return callback;
	}
public:
	BEGIN_SIMDATA_XML_VIRTUAL_INTERFACE(Animation)
		SIMDATA_XML("model_id", Animation::m_ModelID, true)
		SIMDATA_XML("channel_name", Animation::m_ChannelName, true)
		SIMDATA_XML("lod_limit", Animation::m_LOD, false)
		SIMDATA_XML("default", Animation::m_Default, false)
	END_SIMDATA_XML_INTERFACE

	Animation();
	virtual ~Animation();

	virtual AnimationCallback *newCallback(osg::Node *node) const =0;
	virtual AnimationCallback *newCallback(osg::NodeCallback *nodeCallback) const =0;
	inline std::string const &getChannelName() const { return m_ChannelName; }
	inline simdata::Key const &getModelID() const { return m_ModelID; }
};

/**
 * A simple, driven rotation animation in which the rotation
 * angle does not vary between updates.
 */
class DrivenRotation: public Animation
{
	class Callback: public AnimationCallback {
		simdata::Ref<DrivenRotation const> m_Parameters;
		DataChannel<double>::CRef m_Channel;
		float m_Value;
	public:
		Callback(DrivenRotation const *param): m_Parameters(param), m_Value(0.0) { assert(param); }
		virtual void bindChannel(simdata::Ref<const DataChannelBase> channel) {
			m_Channel = channel;
		}
		virtual void operator()(osg::Node* node, osg::NodeVisitor* nv);
		virtual void setDefault(float x) {
			m_Value = x;
			dirty();
		}
	};

public:

	SIMDATA_OBJECT(DrivenRotation, 0, 0);
	
	EXTEND_SIMDATA_XML_INTERFACE(DrivenRotation, Animation)
		SIMDATA_XML("limit_0", DrivenRotation::m_Limit0, false)
		SIMDATA_XML("limit_1", DrivenRotation::m_Limit1, false)
		SIMDATA_XML("gain", DrivenRotation::m_Gain, false)
		SIMDATA_XML("offset", DrivenRotation::m_Offset, false)
		SIMDATA_XML("axis", DrivenRotation::m_Axis, false)
	END_SIMDATA_XML_INTERFACE

	DrivenRotation();
	virtual ~DrivenRotation();

	virtual void postCreate();

	virtual AnimationCallback *newCallback(osg::Node *node) const {
		AnimationCallback *callback = Animation::newCallback(node, new Callback(this));
		//callback->update(m_Default);
		callback->setDefault(m_Default);
		return callback;
	}

	virtual AnimationCallback *newCallback(osg::NodeCallback *nodeCallback) const {
		AnimationCallback *callback = Animation::newCallback(nodeCallback, new Callback(this));
		//callback->update(m_Default);
		callback->setDefault(m_Default);
		return callback;
	}

	inline osg::Vec3 getAxis() const { return simdata::toOSG(m_Axis); }
	inline float getLimit0() const { return m_Limit0; }
	inline float getLimit1() const { return m_Limit1; }
	inline float getGain() const { return m_Gain; }
	inline float getOffset() const { return m_Offset; }

protected:
	simdata::Vector3 m_Axis;
	float m_Limit0;
	float m_Limit1;
	float m_Gain;
	float m_Offset;
};

/**
 * A simple, driven translation animation in which the translation
 * magnitude does not vary between updates.
 */
class DrivenMagnitudeTranslation: public Animation
{
	class Callback: public AnimationCallback {
		simdata::Ref<DrivenMagnitudeTranslation const> m_Parameters;
		DataChannel<double>::CRef m_Channel;
		float m_Value;
	public:
		Callback(DrivenMagnitudeTranslation const *param): m_Parameters(param), m_Value(0.0) { assert(param); }
		virtual void bindChannel(simdata::Ref<const DataChannelBase> channel) {
			m_Channel = channel;
		}
		virtual void operator()(osg::Node* node, osg::NodeVisitor* nv);
		virtual void setDefault(float x) {
			m_Value = x;
			dirty();
		}
	};

public:

	SIMDATA_OBJECT(DrivenMagnitudeTranslation, 0, 0);
	
	EXTEND_SIMDATA_XML_INTERFACE(DrivenMagnitudeTranslation, Animation)
		SIMDATA_XML("limit_0", DrivenMagnitudeTranslation::m_Limit0, false)
		SIMDATA_XML("limit_1", DrivenMagnitudeTranslation::m_Limit1, false)
		SIMDATA_XML("gain", DrivenMagnitudeTranslation::m_Gain, false)
		SIMDATA_XML("direction", DrivenMagnitudeTranslation::m_Direction, false)
		SIMDATA_XML("offset", DrivenMagnitudeTranslation::m_Offset, false)
	END_SIMDATA_XML_INTERFACE

	DrivenMagnitudeTranslation();
	virtual ~DrivenMagnitudeTranslation();

	virtual void postCreate();

	virtual AnimationCallback *newCallback(osg::Node *node) const {
		AnimationCallback *callback = Animation::newCallback(node, new Callback(this));
		//callback->update(m_Default);
		callback->setDefault(m_Default);
		return callback;
	}

	virtual AnimationCallback *newCallback(osg::NodeCallback *nodeCallback) const {
		AnimationCallback *callback = Animation::newCallback(nodeCallback, new Callback(this));
		//callback->update(m_Default);
		callback->setDefault(m_Default);
		return callback;
	}

	inline osg::Vec3 getDirection() const { return simdata::toOSG(m_Direction); }
	inline float getLimit0() const { return m_Limit0; }
	inline float getLimit1() const { return m_Limit1; }
	inline float getGain() const { return m_Gain; }
	inline const osg::Vec3& getOffset() const { return m_OSGOffset;}

protected:
	simdata::Vector3 m_Direction, m_Offset;
	osg::Vec3 m_OSGOffset;
	float m_Limit0;
	float m_Limit1;
	float m_Gain;
};

class DrivenVectorialTranslation: public Animation
{
	class Callback: public AnimationCallback {
		simdata::Ref<DrivenVectorialTranslation const> m_Parameters;
		DataChannel<simdata::Vector3>::CRef m_Channel;
		float m_Value;
	public:
		Callback(DrivenVectorialTranslation const *param): m_Parameters(param), m_Value(0.0) { assert(param); }
		virtual void bindChannel(simdata::Ref<const DataChannelBase> channel) {
			m_Channel = channel;
		}
		virtual void operator()(osg::Node* node, osg::NodeVisitor* nv);
		virtual void setDefault(float x) {
			m_Value = x;
			dirty();
		}
	};

public:

	SIMDATA_OBJECT(DrivenVectorialTranslation, 0, 0)
	
	EXTEND_SIMDATA_XML_INTERFACE(DrivenVectorialTranslation, Animation)
		SIMDATA_XML("limit_0", DrivenVectorialTranslation::m_Limit0, false)
		SIMDATA_XML("limit_1", DrivenVectorialTranslation::m_Limit1, false)
		SIMDATA_XML("gain", DrivenVectorialTranslation::m_Gain, false)
		SIMDATA_XML("direction", DrivenVectorialTranslation::m_Direction, false)
		SIMDATA_XML("offset", DrivenVectorialTranslation::m_Offset, false)
	END_SIMDATA_XML_INTERFACE

	DrivenVectorialTranslation();
	virtual ~DrivenVectorialTranslation();

	virtual void postCreate();

	virtual AnimationCallback *newCallback(osg::Node *node) const {
		AnimationCallback *callback = Animation::newCallback(node, new Callback(this));
		//callback->update(m_Default);
		callback->setDefault(m_Default);
		return callback;
	}

	virtual AnimationCallback *newCallback(osg::NodeCallback *nodeCallback) const {
		AnimationCallback *callback = Animation::newCallback(nodeCallback, new Callback(this));
		//callback->update(m_Default);
		callback->setDefault(m_Default);
		return callback;
	}

	inline osg::Vec3 getDirection() const { return simdata::toOSG(m_Direction); }
	inline float getLimit0() const { return m_Limit0; }
	inline float getLimit1() const { return m_Limit1; }
	inline float getGain() const { return m_Gain; }
	inline const osg::Vec3& getOffset() const { return m_OSGOffset;}

protected:
	simdata::Vector3 m_Direction, m_Offset;
	osg::Vec3 m_OSGOffset;
	float m_Limit0;
	float m_Limit1;
	float m_Gain;
};

/**
 * A simple, driven switch animation in which the culling
 * value does not vary between updates.
 */
class DrivenSwitch: public Animation
{
	class Callback: public AnimationCallback {
		simdata::Ref<DrivenSwitch const> m_Parameters;
		DataChannel<double>::CRef m_Channel;
		double m_Value;
	public:
		Callback(DrivenSwitch const *param): m_Parameters(param), m_Value(1.0) { assert(param); }
		virtual void bindChannel(simdata::Ref<const DataChannelBase> channel) {
			m_Channel = channel;
		}
		virtual void operator()(osg::Node* node, osg::NodeVisitor* nv);
		virtual void setDefault(double x) {
			m_Value = x;
			dirty();
		}
	};

public:

	SIMDATA_OBJECT(DrivenSwitch, 0, 0);
	
	EXTEND_SIMDATA_XML_INTERFACE(DrivenSwitch, Animation)
	END_SIMDATA_XML_INTERFACE

	DrivenSwitch();
	virtual ~DrivenSwitch();

	virtual void postCreate();

	virtual AnimationCallback *newCallback(osg::Node *node) const {
		AnimationCallback *callback = Animation::newCallback(node, new Callback(this));
		callback->setDefault(1.0);
		return callback;
	}

	virtual AnimationCallback *newCallback(osg::NodeCallback *nodeCallback) const {
		AnimationCallback *callback = Animation::newCallback(nodeCallback, new Callback(this));
		//callback->update(m_Default);
		callback->setDefault(1.0);
		return callback;
	}
};

#endif // __ANIMATION_H__

