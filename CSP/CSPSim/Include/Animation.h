// Combat Simulator Project - CSPSim
// Copyright (C) 2002-2005 The Combat Simulator Project
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

#include <typeinfo>

#include <osg/Node>

#include <SimData/Key.h>
#include <SimData/InterfaceRegistry.h>
#include <SimData/osg.h>
#include <SimData/Vector3.h>

#include <Bus.h>

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
class Animation: public simdata::Object {
private:

	simdata::Key m_ModelID;
	std::string m_ChannelName;
	int m_LOD;
	float m_Limit0;
	float m_Limit1;
	float m_Gain;

protected:
	/**
	* Small template class to reduce & simplify writing.
	* It's the heart of the animation in overriding
	* virtual void operator()(osg::Node* node, osg::NodeVisitor* nv)
	* in derived classes.
	*/
	template <class A, class CT, class VT = float> class Callback_A_C: public AnimationCallback {
	protected:
		simdata::Ref<const A> m_Parameters;
		simdata::Ref<const DataChannel<CT> > m_Channel;
		VT m_Value;
		virtual void updateValue() = 0;
	public:
		Callback_A_C(const Animation *const param): 
			m_Parameters(dynamic_cast<const A *const>(param)),
			m_Value(0.0f) { 
				assert(param); 
		}
		virtual void bindChannel(simdata::Ref<const DataChannelBase> channel) {
			m_Channel = channel;
		}
		virtual void setDefault(float x) {
			m_Value = x;
			dirty();
		}
		virtual ~Callback_A_C(){}
	};

	template <class A> class Callback_A: public Callback_A_C<A, double> {
	protected:
		virtual void updateValue() {
			if (m_Channel.valid()) {
				float value = m_Channel->value();
				if (value != m_Value) {
					m_Value = value;
					dirty();
				}
			}
		}
	public:
		Callback_A(const Animation *const param): 
			Callback_A_C<A,double>(param) {}
		virtual ~Callback_A(){}
	};

	float m_Default;

	virtual AnimationCallback *newCallback(osg::Node *node, AnimationCallback *callback) const {
		assert(node);
		assert(callback);
		callback->bind(*node);
		callback->setChannelName(m_ChannelName);
		return callback;
	}
	template <class A, class C>  AnimationCallback *newCallback_(osg::Node *node) const {
		AnimationCallback *callback = Animation::newCallback(node, new C(dynamic_cast<const A* const>(this)));
		callback->setDefault(m_Default);
		return callback;
	}
	virtual AnimationCallback *newCallback(osg::NodeCallback* node_callback, AnimationCallback *callback) const {
		assert(node_callback);
		assert(callback);
		callback->bind(*node_callback);
		callback->setChannelName(m_ChannelName);
		return callback;
	}
	template <class A, class C>  AnimationCallback *newCallback_(osg::NodeCallback *nodeCallback) const {
		AnimationCallback *callback = Animation::newCallback(nodeCallback, new C(dynamic_cast<const A* const>(this)));
		callback->setDefault(m_Default);
		return callback;
	}
public:
	BEGIN_SIMDATA_XML_VIRTUAL_INTERFACE(Animation)
		SIMDATA_XML("model_id", Animation::m_ModelID, true)
		SIMDATA_XML("channel_name", Animation::m_ChannelName, true)
		SIMDATA_XML("lod_limit", Animation::m_LOD, false)
		SIMDATA_XML("default", Animation::m_Default, false)
		SIMDATA_XML("limit_0", Animation::m_Limit0, false)
		SIMDATA_XML("limit_1", Animation::m_Limit1, false)
		SIMDATA_XML("gain", Animation::m_Gain, false)
	END_SIMDATA_XML_INTERFACE

	Animation();
	virtual ~Animation() {}

	// typically, this method will call newCallback_
	virtual AnimationCallback *newCallback(osg::Node *node) const = 0;
	virtual AnimationCallback *newCallback(osg::NodeCallback *nodeCallback) const {
		 CSP_LOG(OBJECT, WARNING, typeid(*this).name() << ": nested callback not implemented" );
		 return 0;
	}
	
	const std::string &getChannelName() const { return m_ChannelName; }
	const simdata::Key &getModelID() const { return m_ModelID; }
	float getLimit0() const { return m_Limit0; }
	float getLimit1() const { return m_Limit1; }
	float getGain() const { return m_Gain; }
};


/**
* Abstract class regrouping usual infos (in its SimData interface) 
* to make an Animation behaces like a rotation 
*/ 
class Rotation: public Animation {
	simdata::Vector3 m_Axis;
	float m_Phase;
	osg::Vec3 m_OSGAxis;
public:
	EXTEND_SIMDATA_XML_VIRTUAL_INTERFACE(Rotation, Animation)
		SIMDATA_XML("phase", Rotation::m_Phase, false)
		SIMDATA_XML("axis", Rotation::m_Axis, false)
	END_SIMDATA_XML_INTERFACE

	Rotation();
	virtual ~Rotation() {}

	const osg::Vec3 &getAxis() const { return m_OSGAxis; }
	float getPhase() const { return m_Phase; }

protected:
	virtual void postCreate() {
		m_OSGAxis = simdata::toOSG(m_Axis);
	}
};


/**
 * A simple, driven rotation animation in which the rotation
 * angle does not vary between updates.
 */
class DrivenRotation: public Rotation {
	class Callback: public Callback_A<DrivenRotation> {
	public:
		Callback(const Animation * const param): Callback_A<DrivenRotation>(param){}
		virtual void operator()(osg::Node* node, osg::NodeVisitor* nv);
		virtual ~Callback(){}
	};

public:
	SIMDATA_OBJECT(DrivenRotation, 0, 0)
	
	EXTEND_SIMDATA_XML_INTERFACE(DrivenRotation, Rotation)
	END_SIMDATA_XML_INTERFACE

	virtual ~DrivenRotation(){}

	virtual AnimationCallback *newCallback(osg::Node *node) const {
		return Animation::newCallback_<DrivenRotation,Callback>(node);
	}
	virtual AnimationCallback *newCallback(osg::NodeCallback *nodeCallback) const {
		return Animation::newCallback_<DrivenRotation,Callback>(nodeCallback);
	}
};


/**
* Tiny class encapsulating time values.
*/
class TimedAnimationProxy: public simdata::Object {
	float m_t0, m_t1, m_DeltaT, m_Rate;
public:
	SIMDATA_OBJECT(TimedAnimationProxy, 0, 0)
	
	BEGIN_SIMDATA_XML_INTERFACE(TimedAnimationProxy)
		SIMDATA_XML("t0", TimedAnimationProxy::m_t0, true)
		SIMDATA_XML("t1", TimedAnimationProxy::m_t1, true)
	END_SIMDATA_XML_INTERFACE

	TimedAnimationProxy():
		m_t0(0.0f),
		m_t1(1.0f) {
	}
	~TimedAnimationProxy(){}
	float getDelta_t0(float t) const {
		return simdata::clampTo(t, m_t0, m_t1) - m_t0;
	}
	float getRate() const {
		return m_Rate;
	}
	void setRate(float limit0, float limit1) {
		float delta_limit = limit1 - limit0;
		m_Rate = delta_limit / m_DeltaT;
	}
protected:
	virtual void postCreate() {
		m_DeltaT = m_t1 - m_t0;
	}
};


class TimedRotation: public Rotation {
	class Callback: public Callback_A<TimedRotation> {
	public:
		Callback(const Animation * const param): Callback_A<TimedRotation>(param){}
		virtual void operator()(osg::Node* node, osg::NodeVisitor* nv);
		virtual ~Callback(){}
	};

	simdata::Link<TimedAnimationProxy> m_TimedAnimationProxy;

public:
	SIMDATA_OBJECT(TimedRotation, 0, 0);
	
	EXTEND_SIMDATA_XML_INTERFACE(TimedRotation, Rotation)
		SIMDATA_XML("timed_animation_proxy", TimedRotation::m_TimedAnimationProxy, true)
	END_SIMDATA_XML_INTERFACE

	virtual ~TimedRotation(){}

	virtual AnimationCallback *newCallback(osg::Node *node) const {
		return Animation::newCallback_<TimedRotation,Callback>(node);
	}
	virtual AnimationCallback *newCallback(osg::NodeCallback *nodeCallback) const {
		return Animation::newCallback_<TimedRotation,Callback>(nodeCallback);
	}
	double getTimedAngle(double t) const {
		return getLimit0() + m_TimedAnimationProxy->getRate() * m_TimedAnimationProxy->getDelta_t0(t);
	}
	
protected:
	virtual void postCreate() {
		Rotation::postCreate();
		m_TimedAnimationProxy->setRate(getLimit0(),getLimit1());
	}
};


/**
* Analog of the Rotation class. This abstract class provides general
* infos about a translation transformation seen as an Animation.
*/
class Translation: public Animation {
	simdata::Vector3 m_Direction, m_Offset;
	osg::Vec3 m_OSGOffset, m_OSGDirection;

public:
	EXTEND_SIMDATA_XML_VIRTUAL_INTERFACE(Translation, Animation)
		SIMDATA_XML("direction", Translation::m_Direction, false)
		SIMDATA_XML("offset", Translation::m_Offset, false)
	END_SIMDATA_XML_INTERFACE

	Translation();
	virtual ~Translation() {}

	const osg::Vec3 &getDirection() const { return m_OSGDirection;}
	const osg::Vec3 &getOffset() const { return m_OSGOffset;}

protected:
	virtual void postCreate() {
		m_Direction.normalized();
		m_OSGDirection = simdata::toOSG(m_Direction.normalized());
		m_OSGOffset = simdata::toOSG(m_Offset);
	}
};


/**
 * A simple, driven translation animation in which the translation
 * magnitude does not vary between updates.
 */
class DrivenMagnitudeTranslation: public Translation {
	class Callback: public Callback_A<DrivenMagnitudeTranslation> {
		public:
		Callback(const Animation * const param):Callback_A<DrivenMagnitudeTranslation>(param){}
		virtual void operator()(osg::Node* node, osg::NodeVisitor* nv);
		virtual ~Callback(){}
	};

public:
	SIMDATA_OBJECT(DrivenMagnitudeTranslation, 0, 0)
	
	EXTEND_SIMDATA_XML_INTERFACE(DrivenMagnitudeTranslation, Translation)
	END_SIMDATA_XML_INTERFACE

	virtual ~DrivenMagnitudeTranslation(){}

	virtual AnimationCallback *newCallback(osg::Node *node) const {
		return Animation::newCallback_<DrivenMagnitudeTranslation,Callback>(node);
	}
};


class DrivenVectorialTranslation: public Translation {
	class Callback: public Callback_A_C<DrivenVectorialTranslation, simdata::Vector3> {
	protected:
		virtual void updateValue() {
			if (m_Channel.valid()) {
				float value = m_Parameters->getDirection() * simdata::toOSG(m_Channel->value());
				if (value != m_Value) {
					m_Value = value;
					dirty();
				}
			}
		}
	public:
		Callback(const DrivenVectorialTranslation *param):Callback_A_C<DrivenVectorialTranslation,simdata::Vector3>(param){}
		virtual ~Callback(){}

		virtual void operator()(osg::Node* node, osg::NodeVisitor* nv);
	};

public:
	SIMDATA_OBJECT(DrivenVectorialTranslation, 0, 0)
	
	EXTEND_SIMDATA_XML_INTERFACE(DrivenVectorialTranslation, Translation)
	END_SIMDATA_XML_INTERFACE

	virtual ~DrivenVectorialTranslation(){}

	virtual AnimationCallback *newCallback(osg::Node *node) const {
		return Animation::newCallback_<DrivenVectorialTranslation,Callback>(node);
	}
};


/**
 * A simple, timed translation animation in which the translation
 * magnitude does not vary between updates.
 */
class TimedMagnitudeTranslation: public Translation {
	class Callback: public Callback_A<TimedMagnitudeTranslation> {
		public:
		Callback(const TimedMagnitudeTranslation * const param):Callback_A<TimedMagnitudeTranslation>(param){}
		virtual void operator()(osg::Node* node, osg::NodeVisitor* nv);
		virtual ~Callback(){}
	};

	simdata::Link<TimedAnimationProxy> m_TimedAnimationProxy;
	
public:
	SIMDATA_OBJECT(TimedMagnitudeTranslation, 0, 0)
	
	EXTEND_SIMDATA_XML_INTERFACE(TimedMagnitudeTranslation, Translation)
		SIMDATA_XML("timed_animation_proxy", TimedMagnitudeTranslation::m_TimedAnimationProxy, true)
	END_SIMDATA_XML_INTERFACE

	virtual ~TimedMagnitudeTranslation(){}

	virtual AnimationCallback *newCallback(osg::Node *node) const {
		return Animation::newCallback_<TimedMagnitudeTranslation,Callback>(node);
	}

	osg::Vec3 getTimedTranslation(double t) const {
		return getDirection() * (getLimit0() + m_TimedAnimationProxy->getRate() * m_TimedAnimationProxy->getDelta_t0(t));
	}

protected:
	virtual void postCreate();
};


/**
 * A simple, driven switch animation in which the culling
 * value does not vary between updates.
 */
class DrivenSwitch: public Animation {
	class Callback: public Callback_A<DrivenSwitch> {
		public:
		Callback(const DrivenSwitch * const param):Callback_A<DrivenSwitch>(param){}
		virtual void operator()(osg::Node* node, osg::NodeVisitor* nv);
		virtual ~Callback(){}
	};

public:
	SIMDATA_OBJECT(DrivenSwitch, 0, 0)
	
	EXTEND_SIMDATA_XML_INTERFACE(DrivenSwitch, Animation)
	END_SIMDATA_XML_INTERFACE

	DrivenSwitch() {
		m_Default = 1.0f;
	}
	virtual ~DrivenSwitch(){}

	virtual AnimationCallback *newCallback(osg::Node *node) const {
		return Animation::newCallback_<DrivenSwitch,Callback>(node);
	}
};


/**
* TimedSequence controls TimedAnimation providing a basic time counter.
* It exports (registers) 2 channels which can be imported; ReferenceTime 
* denotes the time from the beginning of the animation and NormalizedTime
* the corresponding normalized value in [0,1].
*/

class TimedSequence: public simdata::Object {
	double m_TimeLength;
	DataChannel<double>::Ref b_ReferenceTime, b_NormalizedTime;
	double m_Direction, m_PreviousDirection;
	std::string m_Name;
	void setTimeValues(double t) {
		b_NormalizedTime->value() = simdata::clampTo(t / m_TimeLength, 0.0, 1.0);
		setReferenceTime();
	}

public:
	SIMDATA_OBJECT(TimedSequence, 0, 0)

	BEGIN_SIMDATA_XML_INTERFACE(TimedSequence)
		// TODO: add initial values of channels
		SIMDATA_XML("time_length", TimedSequence::m_TimeLength, true)
		SIMDATA_XML("name", TimedSequence::m_Name, true)
	END_SIMDATA_XML_INTERFACE

	TimedSequence():
		m_Direction(0.0f),
		m_PreviousDirection(0.0f) {
	}
	virtual ~TimedSequence(){}

	void setReferenceTime() {
		b_ReferenceTime->value() = simdata::clampTo(b_NormalizedTime->value() * m_TimeLength, 0.0, m_TimeLength);
	}
	double onUpdate(double dt) {
		double t = b_ReferenceTime->value();
		t += m_Direction*dt;
		t = simdata::clampTo(t,0.0,m_TimeLength);
		setTimeValues(t);
		m_PreviousDirection = m_Direction;
		return 0.016;
	}
	void setDirection(double direction) {
		m_PreviousDirection = m_Direction;
		m_Direction = direction;
	}
	void stop() {
		setDirection(0.0f);
	}
	void forward() {
		setDirection(m_PreviousDirection);
	}
	void backward() {
		setDirection(-m_PreviousDirection);
	}
	void start() {
		// don't allow to start again until the current sequence is not over
		if (b_NormalizedTime->value() < 0.0001) {
			setTimeValues(0.0);
			setDirection(1.0);
		}
	}
	void rstart() {
		// don't allow to rstart again until the current sequence is not over
		if (b_NormalizedTime->value() > 0.9999) {
			setTimeValues(m_TimeLength);
			setDirection(-1.0);
		}
	}
	void registerChannels(Bus::Ref bus) {
		bus->registerChannel(b_ReferenceTime.get());
		bus->registerChannel(b_NormalizedTime.get());
	}
	double getNormalizedTime() const {
		return b_NormalizedTime->value();
	}

protected:
	virtual void postCreate() {
		CSP_LOG(OBJECT, DEBUG, "TimedSequence: m_Name = " << m_Name);
		b_ReferenceTime = DataChannel<double>::newLocal(m_Name + ".ReferenceTime", m_TimeLength);
		b_NormalizedTime = DataChannel<double>::newLocal(m_Name + ".NormalizedTime", 1.0);
	}
};

#endif // __ANIMATION_H__

