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

#include <osg/AnimationPath>
#include <osg/Node>

#include <SimData/Key.h>
#include <SimData/InterfaceRegistry.h>
#include <SimData/osg.h>
#include <SimData/Vector3.h>

#include <Bus.h>

/**
 * Base class for animation callbacks.
 *
 * This class serves as an update callback for a transformation node that
 * animates part of a model scene graph.  Update requests will modify the
 * transform only if necessary.  The callback maintains a dirty counter
 * which is used to determine if a particular node transform is stale.  To
 * test this, a separate counter is stored in the user data slot of the
 * transform and updated accordingly.  For continuous animation loops, the
 * dirty counter is not used and the transform is updated after every
 * callback.
 */
class AnimationCallback: public osg::AnimationPathCallback {
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
	AnimationCallback(const osg::AnimationPathCallback& oapc): 
		osg::AnimationPathCallback(oapc),
		m_DirtyCount(0) {
	}
	virtual ~AnimationCallback() {}
	inline void dirty() { ++m_DirtyCount; }
	bool needsUpdate(osg::Node& node);
	bool needsUpdate(osg::NodeCallback& node_callback);
	inline std::string const &getChannelName() const { return m_ChannelName; }
	inline void setChannelName(std::string const &name) { m_ChannelName = name; }
	void bind(osg::Node &node);
	void bind(osg::NodeCallback &node_callback);
	virtual void bindChannel(simdata::Ref<const DataChannelBase>)=0;
	virtual void setValue(float) {}
};


/**
 * Base class for animated 3d model components.
 *
 * Animation instances are bound to ObjectModels to provide data about
 * particular model animations.  Animation instances, like ObjectModels,
 * are shared by multiple SceneModel instances.
 */
class Animation: public simdata::Object {
private:

	simdata::Key m_ModelID;
	std::string m_ChannelName;
	float m_DefaultValue;
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
	template <class A, class CT = double, class VT = float> class Callback_A_C: public AnimationCallback {
	protected:
		simdata::Ref<const A> m_Parameters;
		simdata::Ref<const DataChannel<CT> > m_Channel;
		VT m_Value;
		virtual void updateValue() = 0;
	public:
		Callback_A_C(const A *const param):
			m_Parameters(param),
			m_Value(0.0f) {
				assert(param);
		}
		Callback_A_C(const A *const param, const osg::AnimationPathCallback& oapc):
			AnimationCallback(oapc),
			m_Parameters(param),
			m_Value(0.0f) { 
				assert(param); 
		}
		virtual void bindChannel(simdata::Ref<const DataChannelBase> channel) {
			m_Channel = channel;
		}
		virtual void setValue(float x) {
			m_Value = x;
			dirty();
		}
		virtual ~Callback_A_C(){}
	};

	template <class A> class Callback_A: public Callback_A_C<A> {
	protected:
		virtual void updateValue() {
			if (m_Channel.valid()) {
				float value = m_Channel->value();
				if (std::abs(value - m_Value) > 0.00001) {
					m_Value = value;
					dirty();
				}
			}
		}
	public:
		Callback_A(const A *const param):
			Callback_A_C<A>(param) {}
		Callback_A(const A *const param, const osg::AnimationPathCallback& oapc):
			Callback_A_C<A>(param,oapc) {}
		virtual ~Callback_A(){}
	};

	template <class A, class C>  AnimationCallback *newCallback_(osg::Node *node) const {
		AnimationCallback *callback = new C(dynamic_cast<const A* const>(this));
		init(callback);
		assert(node);
		callback->bind(*node);
		return callback;
	}

	template <class A, class C>  AnimationCallback *newCallback_(osg::NodeCallback *node_callback) const {
		AnimationCallback *callback = new C(dynamic_cast<const A* const>(this));
		init(callback);
		assert(node_callback);
		callback->bind(*node_callback);
		return callback;
	}

	void init(AnimationCallback *callback) const {
		assert(callback);
		callback->setChannelName(m_ChannelName);
		callback->setValue(m_DefaultValue);
	}

public:
	BEGIN_SIMDATA_XML_VIRTUAL_INTERFACE(Animation)
		SIMDATA_XML("model_id", Animation::m_ModelID, true)
		SIMDATA_XML("channel_name", Animation::m_ChannelName, true)
		SIMDATA_XML("lod_limit", Animation::m_LOD, false)
		SIMDATA_XML("default", Animation::m_DefaultValue, false)
		SIMDATA_XML("limit_0", Animation::m_Limit0, false)
		SIMDATA_XML("limit_1", Animation::m_Limit1, false)
		SIMDATA_XML("gain", Animation::m_Gain, false)
	END_SIMDATA_XML_INTERFACE

	Animation(float defaultValue=0.0f);
	virtual ~Animation() { }

	// typically, this method will call newCallback_
	virtual AnimationCallback *newCallback(osg::Node *node) const = 0;
	virtual AnimationCallback *newCallback(osg::NodeCallback*) const {
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
 * Abstract animation class for rotations.
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

	const osg::Vec3 &getAxis() const { return m_OSGAxis; }
	float getPhase() const { return m_Phase; }

protected:
	virtual void postCreate() {
		m_OSGAxis = simdata::toOSG(m_Axis);
	}
};


/**
 * A simple, driven rotation animation in which the rotation angle does not
 * vary between updates.
 */
class DrivenRotation: public Rotation {
	class Callback: public Callback_A<DrivenRotation> {
	public:
		Callback(const DrivenRotation * const param): Callback_A<DrivenRotation>(param){}
		virtual void operator()(osg::Node* node, osg::NodeVisitor* nv);
		virtual ~Callback(){}
	};

public:
	SIMDATA_OBJECT(DrivenRotation, 0, 0)
	
	EXTEND_SIMDATA_XML_INTERFACE(DrivenRotation, Rotation)
	END_SIMDATA_XML_INTERFACE

	virtual AnimationCallback *newCallback(osg::Node *node) const {
		return newCallback_<DrivenRotation,Callback>(node);
	}

	virtual AnimationCallback *newCallback(osg::NodeCallback *nodeCallback) const {
		return newCallback_<DrivenRotation,Callback>(nodeCallback);
	}
};


/**
 * Tiny class encapsulating time values.
 */
class TimedAnimationProxy: public simdata::Object {
	float m_t0;
	float m_t1;
	float m_TimeLength;
	float m_Rate;

public:
	SIMDATA_OBJECT(TimedAnimationProxy, 0, 0)
	
	BEGIN_SIMDATA_XML_INTERFACE(TimedAnimationProxy)
		SIMDATA_XML("t0", TimedAnimationProxy::m_t0, true)
		SIMDATA_XML("t1", TimedAnimationProxy::m_t1, true)
	END_SIMDATA_XML_INTERFACE

	TimedAnimationProxy():
		m_t0(0.0f),
		m_t1(0.0f),
		m_TimeLength(1.0f),
		m_Rate(0.0f) {
	}
	~TimedAnimationProxy(){}
	float getDelta_t0(float t) const { return clamp(t) - m_t0; }
	float clamp(float t) const { return simdata::clampTo(t, m_t0, m_t1); }
	float getRate() const { return m_Rate; }
	void setRate(float limit0, float limit1) {
		float delta_limit = limit1 - limit0;
		m_Rate = delta_limit / m_TimeLength;
	}
	float getTimeLength() const { return m_TimeLength; }
	float gett_0() const { return m_t0; }
	float gett_1() const { return m_t1; }

protected:
	virtual void postCreate() { m_TimeLength = m_t1 - m_t0; }
};


class TimedRotation: public Rotation {
	class Callback: public Callback_A<TimedRotation> {
	public:
		Callback(const TimedRotation * const param): Callback_A<TimedRotation>(param){}
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
		return newCallback_<TimedRotation,Callback>(node);
	}
	virtual AnimationCallback *newCallback(osg::NodeCallback *nodeCallback) const {
		return newCallback_<TimedRotation,Callback>(nodeCallback);
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
 * Abstract animation class for translations.
 */
class Translation: public Animation {
	simdata::Vector3 m_Direction;
	simdata::Vector3 m_Offset;
	osg::Vec3 m_OSGOffset;
	osg::Vec3 m_OSGDirection;

public:
	EXTEND_SIMDATA_XML_VIRTUAL_INTERFACE(Translation, Animation)
		SIMDATA_XML("direction", Translation::m_Direction, false)
		SIMDATA_XML("offset", Translation::m_Offset, false)
	END_SIMDATA_XML_INTERFACE

	Translation();

	const osg::Vec3 &getDirection() const { return m_OSGDirection; }
	const osg::Vec3 &getOffset() const { return m_OSGOffset; }

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
		Callback(const DrivenMagnitudeTranslation * const param):Callback_A<DrivenMagnitudeTranslation>(param){}
		virtual void operator()(osg::Node* node, osg::NodeVisitor* nv);
		virtual ~Callback(){}
	};

public:
	SIMDATA_OBJECT(DrivenMagnitudeTranslation, 0, 0)
	
	EXTEND_SIMDATA_XML_INTERFACE(DrivenMagnitudeTranslation, Translation)
	END_SIMDATA_XML_INTERFACE

	virtual ~DrivenMagnitudeTranslation(){}

	virtual AnimationCallback *newCallback(osg::Node *node) const {
		return newCallback_<DrivenMagnitudeTranslation,Callback>(node);
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
		return newCallback_<DrivenVectorialTranslation,Callback>(node);
	}
};


/**
 * A simple, timed translation animation in which the translation
 * magnitude is controlled by a TimedAnimationProxy.
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

	virtual AnimationCallback *newCallback(osg::Node *node) const {
		return newCallback_<TimedMagnitudeTranslation,Callback>(node);
	}

	osg::Vec3 getTimedTranslation(double t) const {
		return getDirection() * (getLimit0() + m_TimedAnimationProxy->getRate() * m_TimedAnimationProxy->getDelta_t0(t));
	}

protected:
	virtual void postCreate();
};


/**
 * An animation class that controls the visibility of the animated part.
 * Although driven by a continuous variable, the switch has only two states.
 * For non-zero values the part is visible, otherwise it is hidden using
 * the OSG node mask.
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

	DrivenSwitch(): Animation(/*defaultValue=*/1.0f) { }
	virtual ~DrivenSwitch(){}

	virtual AnimationCallback *newCallback(osg::Node *node) const {
		return newCallback_<DrivenSwitch,Callback>(node);
	}
};


/**
* TimedSequence controls timed animations providing a basic time counter.
* It exports (registers) 2 channels which can be imported; ReferenceTime 
* denotes the time from the beginning of the animation and NormalizedTime
* the corresponding normalized value in [0,1].
*/
class TimedSequence: public simdata::Object {
	class UpdateReferenceTime: public DataChannelBase::Handler {
		TimedSequence &m_TimedSequence;
	public:
		UpdateReferenceTime(TimedSequence &timed_sequence):
			m_TimedSequence(timed_sequence) {
		}
		virtual ~UpdateReferenceTime(){}
		virtual void operator()() {
			m_TimedSequence.setReferenceTime();
		}
	};

	// SimData data
	simdata::Link<TimedAnimationProxy> m_TimeAnimationProxy;
	double m_InitialTime;
	std::string m_Name;

	// Shared channels
	DataChannel<double>::Ref b_ReferenceTime;
	DataChannel<double>::Ref b_NormalizedTime; // push channel

	// Internal members
	double m_Direction, m_PreviousDirection;
	UpdateReferenceTime* m_UpdateReferenceTime;
	SigC::Connection m_Connection;
	bool m_Enabled;
	
	void setTimeValues(double t) {
		b_NormalizedTime->value() =
			simdata::clampTo(m_TimeAnimationProxy->getDelta_t0(t)/m_TimeAnimationProxy->getTimeLength(),0.0f,1.0f);
		setReferenceTime();
	}

	void setReferenceTime() {
		b_ReferenceTime->value() =
			m_TimeAnimationProxy->clamp(m_TimeAnimationProxy->gett_0()+b_NormalizedTime->value()*m_TimeAnimationProxy->getTimeLength());
	}
	
	bool isEnabled() const {
		return m_Enabled;
	}
protected:
	bool isEnd() const {
		return 1.0 - b_NormalizedTime->value() < 0.0001;
	}
	bool isBegin() const {
		return b_NormalizedTime->value() < 0.0001;
	}
	void setDirection(double direction) {
		m_PreviousDirection = m_Direction;
		m_Direction = direction;
	}
	void stop() {
		if (isEnabled())
			setDirection(0.0f);
	}
	void forward() {
		setDirection(m_PreviousDirection);
	}
	void backward() {
		setDirection(-m_PreviousDirection);
	}
	void jumpToBeginning() {
		stop();
		setTimeValues(m_TimeAnimationProxy->gett_0());
	}
	void jumpToEnd() {
		stop();
		setTimeValues(m_TimeAnimationProxy->gett_1());
	}

public:
	SIMDATA_OBJECT(TimedSequence, 0, 0)

	BEGIN_SIMDATA_XML_INTERFACE(TimedSequence)
		SIMDATA_XML("timed_animation_proxy", TimedSequence::m_TimeAnimationProxy, true)
		SIMDATA_XML("initial_time", TimedSequence::m_InitialTime, false)
		SIMDATA_XML("name", TimedSequence::m_Name, false)
	END_SIMDATA_XML_INTERFACE

	TimedSequence():
		m_InitialTime(0.0),
		m_Direction(0.0f),
		m_PreviousDirection(0.0f),
		m_UpdateReferenceTime(0),
		m_Enabled(true) {
	}

	virtual ~TimedSequence(){
		if (m_UpdateReferenceTime) {
			delete m_UpdateReferenceTime;
			m_UpdateReferenceTime = 0;
		}
	}

	double onUpdate(double dt) {
		if (isEnabled()) {
			double t = b_ReferenceTime->value();
			t += m_Direction*dt;
			t = m_TimeAnimationProxy->clamp(t);
			setTimeValues(t);
		}
		return 0.016;
	}

	void start() {
		// do not allow to start again until the current sequence is not over
		if (isBegin()) {
			setTimeValues(m_TimeAnimationProxy->gett_0());
			setDirection(1.0);
		}
	}

	void rstart() {
		// do not allow to rstart again until the current sequence is not over
		if (isEnd()) {
			setTimeValues(m_TimeAnimationProxy->gett_1());
			setDirection(-1.0);
		}
	}

	void toggle() {
		if (isEnd())
			rstart();
		else if (isBegin())
			start();
	}

	void setNormalizedTime(double t) {
		if (isEnabled()) {
			t = simdata::clampTo(t, 0.0, 1.0);
			b_NormalizedTime->value() = t;
			setReferenceTime();
		}
	}

	double getReferenceTime() const {
		return b_ReferenceTime->value();
	}

	double getNormalizedTime() const {
		return b_NormalizedTime->value();
	}

	double getDirection() const {
		return m_Direction;
	}

	void disable() {
		m_Enabled = false;
		stop();
	}

	void enable() {
		if (!isEnabled()) {
			m_Enabled = true;
			setTimeValues(b_ReferenceTime->value());
		}
	}

	void setNameIfEmpty(std::string const &name) {
		if (m_Name.empty()) m_Name = name;
	}

	std::string const &getName() const {
		return m_Name;
	}

	void bindChannels(Bus::Ref bus) {
		assert(!m_Name.empty());
		if (!b_ReferenceTime.valid()) { 
			std::string name = m_Name + ".ReferenceTime";
			CSP_LOG(APP, DEBUG, "TimedSequence::bindChannels: " << name);
			b_ReferenceTime = bus->getSharedChannel(name);
		}
	}

	void registerChannels(Bus::Ref bus) {
		assert(!m_Name.empty());
		std::string name = m_Name + ".ReferenceTime";
		if (!bus->hasChannel(name)) {
			CSP_LOG(APP, DEBUG, "TimedSequence::registerChannels: " << name);
			b_ReferenceTime = bus->registerSharedDataChannel(name, m_InitialTime);
		}
		m_InitialTime = m_TimeAnimationProxy->clamp(m_InitialTime);
		double initial_normalized_time = simdata::clampTo(m_TimeAnimationProxy->getDelta_t0(m_InitialTime)/m_TimeAnimationProxy->getTimeLength(),0.0f,1.0f);
		b_NormalizedTime = DataChannel<double>::newSharedPush(m_Name + ".NormalizedTime", initial_normalized_time);
		m_UpdateReferenceTime = new UpdateReferenceTime(*this);
		m_Connection = b_NormalizedTime->connect(m_UpdateReferenceTime);
		bus->registerChannel(b_NormalizedTime.get());
	}
};


class TimedAnimationPath: public Animation {
	class Callback: public Callback_A<TimedAnimationPath> {
	public:
		Callback(const TimedAnimationPath * const param): 
		  Callback_A<TimedAnimationPath>(param){
		  }
		  Callback(const TimedAnimationPath * const param, const osg::AnimationPathCallback &oapc): 
			Callback_A<TimedAnimationPath>(param,oapc){
		  }
		virtual void operator()(osg::Node* node, osg::NodeVisitor* nv);
		virtual ~Callback(){}
	};
	double m_TimeMultiplier;
public:
	SIMDATA_OBJECT(TimedAnimationPath, 0, 0)
	
	EXTEND_SIMDATA_XML_INTERFACE(TimedAnimationPath, Animation)
		SIMDATA_XML("time_multiplier", TimedAnimationPath::m_TimeMultiplier, false)
	END_SIMDATA_XML_INTERFACE

	TimedAnimationPath():
		m_TimeMultiplier(1.0) {
	}
	virtual ~TimedAnimationPath(){}

	virtual AnimationCallback *newCallback(osg::Node *node) const {
		AnimationCallback *callback = 0;
		osg::ref_ptr<osg::AnimationPathCallback> oapc = dynamic_cast<osg::AnimationPathCallback *>(node->getUpdateCallback());
		if (oapc.valid()) {
			callback = new Callback(this, *oapc);
			if (callback) {
				init(callback);
				assert(node);
				callback->bind(*node);
				callback->setTimeMultiplier(m_TimeMultiplier);
			}
		}
		return callback;
	}
	virtual AnimationCallback *newCallback(osg::NodeCallback *nodeCallback) const {
		return Animation::newCallback_<TimedAnimationPath,Callback>(nodeCallback);
	}
};

#endif // __ANIMATION_H__

