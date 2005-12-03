// Combat Simulator Project
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
 * @file Animation.cpp
 *
 **/


#include <csp/cspsim/Animation.h>
#include <csp/cspsim/InputEventChannel.h>

#include <csp/csplib/util/Log.h>
#include <csp/csplib/util/Math.h>
#include <csp/csplib/util/osg.h>
#include <csp/csplib/data/Enum.h>
#include <csp/csplib/data/LUT.h>
#include <csp/csplib/data/Vector3.h>
#include <csp/csplib/data/ObjectInterface.h>

#include <osg/AnimationPath>
#include <osg/MatrixTransform>
#include <osg/PositionAttitudeTransform>
#include <osg/ref_ptr>
#include <osg/Switch>
#include <osg/Version>

#ifndef OSG_VERSION_MAJOR
#define OSG_OLD_CONTROL_POINT_INTERFACE
#endif

CSP_NAMESPACE

CSP_XML_BEGIN(Animation)
	CSP_DEF("model_id", m_NodeLabel, true)
	CSP_DEF("lod_limit", m_LOD, false)
CSP_XML_END


// Stupidly, this class is not exposed by osg so we have to reproduce it here.
class AnimationPathCallbackVisitor: public osg::NodeVisitor {
	osg::AnimationPath::ControlPoint _cp;
	osg::Vec3d _pivotPoint;
	bool _useInverseMatrix;
public:
	AnimationPathCallbackVisitor(const osg::AnimationPath::ControlPoint& cp, const osg::Vec3d& pivotPoint, bool useInverseMatrix): _cp(cp), _pivotPoint(pivotPoint), _useInverseMatrix(useInverseMatrix) {}
	virtual void apply(osg::MatrixTransform& mt) {
		osg::Matrix matrix;
		if (_useInverseMatrix) {
			_cp.getInverse(matrix);
		} else {
			_cp.getMatrix(matrix);
		}
		mt.setMatrix(osg::Matrix::translate(-_pivotPoint)*matrix);
	}
	virtual void apply(osg::PositionAttitudeTransform& pat) {
		if (_useInverseMatrix) {
			osg::Matrix matrix;
			_cp.getInverse(matrix);
			pat.setPosition(matrix.getTrans());
#ifdef OSG_OLD_CONTROL_POINT_INTERFACE
			pat.setAttitude(_cp._rotation.inverse());
			pat.setScale(osg::Vec3(1.0f/_cp._scale.x(),1.0f/_cp._scale.y(),1.0f/_cp._scale.z()));
#else
			pat.setAttitude(_cp.getRotation().inverse());
			pat.setScale(osg::Vec3(1.0f/_cp.getScale().x(),1.0f/_cp.getScale().y(),1.0f/_cp.getScale().z()));
#endif
			pat.setPivotPoint(_pivotPoint);
		} else {
#ifdef OSG_OLD_CONTROL_POINT_INTERFACE
			pat.setPosition(_cp._position);
			pat.setAttitude(_cp._rotation);
			pat.setScale(_cp._scale);
#else
			pat.setPosition(_cp.getPosition());
			pat.setAttitude(_cp.getRotation());
			pat.setScale(_cp.getScale());
#endif
			pat.setPivotPoint(_pivotPoint);
		}
	}
};


/** A wrapper class for double-precision floating point data channels.  Encapsulates
 *  functionality that is common to several animation subclasses.
 */
class DoubleChannel {
	DataChannel<double>::CRefT m_Channel;
	float m_Value;

public:
	DoubleChannel(): m_Value(0.000000113443f) {}  // hack to force an initial update

	/** Read and store the latest data from the channel.  Returns true if the value has
	 *  changed since the last call to update.
	 */
	bool update() {
		if (!m_Channel) return false;
		const float value = static_cast<float>(m_Channel->value());
		const bool changed = (value != m_Value);
		m_Value = value;
		return changed;
	}

	/** Bind to a named data channel.  Returns true on success, and logs errors
	 *  on failure.  If bus is null the channel is detached and the method returns
	 *  false.
	 */
	bool bind(Bus *bus, std::string const &name) {
		m_Channel = 0;
		if (bus) {
			try {
				m_Channel = bus->getChannel(name, false);
			} catch (ConversionError &) {
				CSPLOG(WARNING, OBJECT) << "Incompatible channel " << name << " for animation; expected double";
				return false;
			}
			if (!m_Channel) {
				CSPLOG(WARNING, OBJECT) << "Unable to bind channel " << name << " for animation";
			}
		}
		return m_Channel.valid();
	}

	/** Get the value of the channel as of the last call to update().
	 */
	inline float value() const { return m_Value; }
};


/** A wrapper class for enumlink data channels.  Encapsulates functionality that is
 *  common to several animation subclasses.  As a minor hack, EnumLinkChannel also
 *  supports bool channels, treating them as two-state enumerations with tokens
 *  "false" and "true" and corresponding values 0 and 1, respectively.
 */
class EnumLinkChannel {
	DataChannel<EnumLink>::CRefT m_Channel;
	DataChannel<bool>::CRefT m_BoolChannel;
	int m_Value;
	std::string m_Token;

public:
	EnumLinkChannel(): m_Value(-1) {}

	/** Read and store the latest value of the channel.  Returns true if the value
	 *  has changed since the last call to update.
	 */
	bool update() {
		int value = -2;
		std::string token;
		if (m_Channel.valid()) {
			if (!m_Channel) return false;
			value = m_Channel->value().getValue();
			if (value == m_Value) return false;
			token = m_Channel->value().getToken();
		} else {
			if (!m_BoolChannel) return false;
			value = m_BoolChannel->value() ? 1 : 0;
			if (value == m_Value) return false;
			token = m_BoolChannel->value() ? "true" : "false";
		}
		m_Value = value;
		m_Token = token;
		return true;
	}

	/** Bind to the named data channel.  Returns true on success, and logs errors
	 *  on failure.  If bus is null the channel is detached and the method returns
	 *  false;
	 */
	bool bind(Bus *bus, std::string const &name) {
		m_Channel = 0;
		if (bus) {
			try {
				m_Channel = bus->getChannel(name, false);
			} catch (ConversionError &e) {
				e.clear();
				try {
					m_BoolChannel = bus->getChannel(name, false);
				} catch (ConversionError &e) {
					e.clear();
					CSPLOG(WARNING, OBJECT) << "Incompatible channel " << name << " for animation; expected enumlink";
					return false;
				}
			}
			if (!m_Channel) {
				CSPLOG(WARNING, OBJECT) << "Unable to bind channel " << name << " for animation";
			}
		}
		return m_Channel.valid() || m_BoolChannel.valid();
	}

	/** Cycle the data channel to the next value.  Only possible for shared data channels.
	 */
	void cycle() {
		if (m_Channel.valid()) {
			assert(m_Channel->isShared());
			if (m_Channel->isShared()) {
				EnumLink &e = const_cast<EnumLink&>(m_Channel->value());
				e.cycle(); // TODO push
			}
		} else {
			if (m_BoolChannel.valid()) {
				if (m_BoolChannel->isShared()) {
					bool &b = const_cast<bool&>(m_BoolChannel->value());
					b = !b;
				} else {
					m_BoolChannel->requestSet(!(m_BoolChannel->value()));
				}
			}
		}
	}

	/** Get the enumeration associated with the data channel.  Should only be called
	 *  if bind() succeeded (asserts otherwise).
	 */
	Enumeration const &getEnumeration() const {
		assert(m_Channel.valid());
		return m_Channel->value().getEnumeration();
	}

	/** Get the value of the data channel as of the last call to update().
	 */
	inline int value() const { return m_Value; }

	/** Get the token of the data channel as of the last call to update().
	 */
	inline std::string const &token() const { return m_Token; }
};


AnimationCallback::AnimationCallback() {
}

// Defining the destructor outside of the header allows some osg classes to be forward declared.
AnimationCallback::~AnimationCallback() {
}

void AnimationCallback::bind(osg::Node &node) {
	osg::NodeCallback *old = node.getUpdateCallback();
	if (old) {
		if (dynamic_cast<AnimationCallback*>(old)) {
			CSPLOG(INFO, OBJECT) << "Adding nested animation callback (node " << node.getName() << ")";
			old->addNestedCallback(this);
			return;
		}
		CSPLOG(WARNING, OBJECT) << "Overwriting update callback with animation callback (node " << node.getName() << ")";
	}
	node.setUpdateCallback(this);
}

double AnimationCallback::elapsedTime(const osg::NodeVisitor *nv) {
	const double now = nv->getFrameStamp()->getReferenceTime();
	const double dt = now - m_LastUpdateTime;
	m_LastUpdateTime = now;
	return dt;
}


////////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////////

/**
 * Abstract animation class for rotations around a defined axis.
 *
 * XML parameters:
 *   channel_name: name of a double-precision floating point channel to drive the rotation.
 *   axis: the rotation axis.
 *   gain: a scaling factor frome the control channel to radians (default=1).
 *   offset: the rotation angle offset in radians (default=0).
 *   limit_0: the "minimum" angle, in radians.  the actual interpretation varies by subclass (default=-PI).
 *   limit_1: the "maximum" angle, in radians.  the actual interpretation varies by subclass (default=PI).
 *   pre_multiply: if true, the rotation will be applied before the existing node rotation (default=false).
 *   post_multiply: if true, the rotation will be applied after the existing node rotation (default=false).
 *
 * If both pre_multiply and post_multiply are false, the rotation will replace the existing node rotation
 * (although the position offset will be preserved).  Specifying both pre and post multiply is an error.
 *
 * Rotation and its subclasses require manual definition of animation parameters that are generally
 * easier to specify as part of the 3D model (using animation paths).  Except for simple cases it may
 * be better to use animations based on AnimationPath instead.
 */
class Rotation: public Animation {
	std::string m_ChannelName;
	Vector3 m_Axis;
	double m_Gain;
	double m_Phase;
	double m_Limit0;
	double m_Limit1;
	bool m_PreMultiply;
	bool m_PostMultiply;
	osg::Vec3 m_OSGAxis;
	Table1 m_NonLinearScale;

public:
	CSP_DECLARE_ABSTRACT_OBJECT(Rotation)

	Rotation():
		m_Axis(Vector3::ZERO),
		m_Gain(1.0f),
		m_Phase(0.0f),
		m_Limit0(-180),
		m_Limit1(180),
		m_PreMultiply(false),
		m_PostMultiply(false)
	{ }

	inline const std::string &getChannelName() const { return m_ChannelName; }
	inline double getGain() const { return m_Gain; }
	inline double getPhase() const { return m_Phase; }
	inline const osg::Vec3 &getAxis() const { return m_OSGAxis; }
	inline double getLimit0() const { return m_Limit0; }
	inline double getLimit1() const { return m_Limit1; }
	inline bool getPreMultiply() const { return m_PreMultiply; }
	inline bool getPostMultiply() const { return m_PostMultiply; }

	double rescaleAngle(double value) const {
		double angle = m_NonLinearScale.isInterpolated() ? m_NonLinearScale[static_cast<float>(value)] : value * m_Gain;
		double lower = std::min(getLimit0(), getLimit1());
		double upper = std::max(getLimit0(), getLimit1());
		return toRadians(clampTo(angle + m_Phase, lower, upper));
	}

protected:
	virtual void postCreate() {
		Animation::postCreate();
		m_OSGAxis = toOSG(m_Axis);
		assert(!(m_PreMultiply && m_PostMultiply));
	}
};

CSP_XML_BEGIN(Rotation)
	CSP_DEF("channel_name", m_ChannelName, false)
	CSP_DEF("axis", m_Axis, false)
	CSP_DEF("gain", m_Gain, false)
	CSP_DEF("phase", m_Phase, false)
	CSP_DEF("limit_0", m_Limit0, false)
	CSP_DEF("limit_1", m_Limit1, false)
	CSP_DEF("pre_multiply", m_PreMultiply, false)
	CSP_DEF("post_multiply", m_PostMultiply, false)
	CSP_DEF("non_linear_scale", m_NonLinearScale, false)
CSP_XML_END


////////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////////

/** A multistate switch represented by an enumerated channel that drives
 *  a rotation node.  Prefer AnimatedSwitch when possible to avoid having
 *  to manually specify the rotation parameters.
 */
class RotarySwitch: public Animation {
	class Callback;
	std::string m_ChannelName;
	Vector3 m_Axis;
	std::vector<float> m_Angles;
	osg::Vec3 m_OSGAxis;

public:
	CSP_DECLARE_OBJECT(RotarySwitch)

	virtual AnimationCallback *newCallback(osg::Node *node) const;

	inline const std::string &getChannelName() const { return m_ChannelName; }
	inline const osg::Vec3 &getAxis() const { return m_OSGAxis; }
	inline std::vector<float> const &getAngles() const { return m_Angles; }

protected:
	virtual void postCreate() {
		Animation::postCreate();
		m_OSGAxis = toOSG(m_Axis);
	}
};

CSP_XML_BEGIN(RotarySwitch)
	CSP_DEF("channel_name", m_ChannelName, false)
	CSP_DEF("axis", m_Axis, false)
	CSP_DEF("angles", m_Angles, true)
CSP_XML_END


class RotarySwitch::Callback: public AnimationCallback {
	Ref<const RotarySwitch> m_Animation;
	EnumLinkChannel m_Channel;
	osg::MatrixTransform *m_Transform;

public:
	Callback(RotarySwitch const *rotary_switch): m_Animation(rotary_switch), m_Transform(0) { assert(rotary_switch); }

	void operator()(osg::Node* node, osg::NodeVisitor* nv) {
		if (nv->getVisitorType() == osg::NodeVisitor::UPDATE_VISITOR && m_Channel.update()) {
			if (!m_Transform) {
				m_Transform = dynamic_cast<osg::MatrixTransform*>(node);
				assert(m_Transform);
			}
			if (m_Transform && m_Channel.value() >= 0) {
				assert(static_cast<int>(m_Animation->getAngles().size()) > m_Channel.value());
				const double angle = toRadians(m_Animation->getAngles()[m_Channel.value()]);
				osg::Matrix m = osg::Matrix::rotate(angle, m_Animation->getAxis());
				m.setTrans(m_Transform->getMatrix().getTrans());
				m_Transform->setMatrix(m);
			}
		}
		traverse(node, nv);
	}

	virtual bool bindChannels(Bus *bus) { return m_Channel.bind(bus, m_Animation->getChannelName()); }

	virtual bool pick(int /*flags*/) {
		m_Channel.cycle();
		return true;
	}
};

AnimationCallback *RotarySwitch::newCallback(osg::Node *node) const {
	assert(node);
	AnimationCallback *callback = new Callback(this);
	callback->bind(*node);
	return callback;
}


////////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////////

/** A multistate switch represented by an enumerated channel that drives
 *  an osg::Switch node to show/hide model parts.  Children of the specified
 *  node must have labels that match the tokens of the enumerated channel.
 *
 *  TODO Add a mapping from model node label to enumeration token to the xml
 *  interface.
 */
class StateSwitch: public Animation {
	class Callback;
	std::string m_ChannelName;

public:
	CSP_DECLARE_OBJECT(StateSwitch)

	virtual AnimationCallback *newCallback(osg::Node *node) const;
	const std::string &getChannelName() const { return m_ChannelName; }
	virtual bool needsSwitch() const { return true; }
};

CSP_XML_BEGIN(StateSwitch)
	CSP_DEF("channel_name", m_ChannelName, true)
CSP_XML_END


class StateSwitch::Callback: public AnimationCallback {
	Ref<const StateSwitch> m_Animation;
	EnumLinkChannel m_Channel;
	std::vector<int> m_Translation;
	osg::Switch *m_Switch;

	enum { UNASSIGNED = -1, MISSING = -2 };

public:

	Callback(StateSwitch const *state_switch): m_Animation(state_switch), m_Switch(0) { assert(state_switch); }

	void operator()(osg::Node* node, osg::NodeVisitor* nv) {
		if (nv->getVisitorType() == osg::NodeVisitor::UPDATE_VISITOR && m_Channel.update()) {
			if (!m_Switch) {
				CSPLOG(INFO, OBJECT) << "StateSwitch found node";
				m_Switch = dynamic_cast<osg::Switch*>(node);
				assert(m_Switch);
			}
			if (m_Switch && m_Channel.value() >= 0) {
				int child = m_Translation[m_Channel.value()];
				if (child == UNASSIGNED) {
					for (unsigned u = 0; u < m_Switch->getNumChildren(); ++u) {
						if (m_Switch->getChild(u)->getName() == m_Channel.token()) {
							child = static_cast<int>(u);
							m_Translation[m_Channel.value()] = child;
							break;
						}
					}
					if (child == UNASSIGNED) {
						CSPLOG(WARNING, SCENE) << "Unknown child " << m_Channel.token() << " in StateSwitch " << m_Animation->getChannelName();
						m_Translation[m_Channel.value()] = MISSING;
					}
				}
				if (child >= 0) {
					m_Switch->setSingleChildOn(static_cast<unsigned>(child));
				}
			}
		}
		traverse(node, nv);
	}

	virtual bool bindChannels(Bus *bus) {
		if (m_Channel.bind(bus, m_Animation->getChannelName())) {
			Enumeration const &e = m_Channel.getEnumeration();
			m_Translation.resize(e.size(), UNASSIGNED);
			return true;
		}
		return false;
	}
};

AnimationCallback *StateSwitch::newCallback(osg::Node *node) const {
	assert(node);
	CSPLOG(INFO, OBJECT) << "Creating StateSwitch callback";
	AnimationCallback *callback = new Callback(this);
	callback->bind(*node);
	return callback;
}


////////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////////

/** A multistate switch represented by an enumerated channel that drives
 *  an animation path.  The times corresponding to each state can be specified
 *  through the xml interface.  Between states, the animation path is driven
 *  at the specified rate (default = 1.0).  If the times parameter is omitted,
 *  the states will be default to integer times (0, 1, 2, ...).
 */
class AnimatedSwitch: public Animation {
	class Callback;
public:
	CSP_DECLARE_OBJECT(AnimatedSwitch)

	virtual AnimationCallback *newCallback(osg::Node *node) const;
	inline const std::string &getChannelName() const { return m_ChannelName; }
	inline double getRate() const { return m_Rate; }
	inline std::vector<double> const &getTimes() const { return m_Times; }

	AnimatedSwitch(): m_Rate(1.0) { }

private:
	std::string m_ChannelName;
	double m_Rate;
	std::vector<double> m_Times;
};

CSP_XML_BEGIN(AnimatedSwitch)
	CSP_DEF("channel_name", m_ChannelName, true)
	CSP_DEF("times", m_Times, false)
	CSP_DEF("rate", m_Rate, false)
CSP_XML_END


class AnimatedSwitch::Callback: public AnimationCallback {
	Ref<const AnimatedSwitch> m_Animation;
	osg::ref_ptr<const osg::AnimationPathCallback> m_AnimationPathCallback;
	EnumLinkChannel m_Channel;
	double m_TimeLimit;
	double m_Target;
	double m_LastTime;
	double m_AnimationTime;
	double m_Direction;
	bool m_Pause;

protected:
	// Get the target time, which is either the enum value (as an integer) or the
	// explicit times for each value set in the constructor.
	double targetTime(int value) const {
		std::vector<double> const &times = m_Animation->getTimes();
		if (times.empty()) return value;
		if (value < 0) {
			CSPLOG(WARNING, SCENE) << "AnimatedSwitch value < 0" << m_Animation->getChannelName();
			return times[0];
		}
		if (value >= static_cast<int>(times.size())) {
			CSPLOG(WARNING, SCENE) << "AnimatedSwitch value exceeds specified times" << m_Animation->getChannelName();
			return times[times.size() - 1];
		}
		return times[value];
	}

	// When the value changes, set the target time and unpause the animation.
	bool update() {
		if (m_Channel.update()) {
			m_Target = targetTime(m_Channel.value());
			m_Direction = (m_Target > m_AnimationTime) ? 1.0 : -1.0;
			m_Pause = false;
		}
		return !m_Pause;
	}

	void updateAnimation(osg::Node *node) {
		osg::AnimationPath::ControlPoint cp;
		const double animation_time = std::min(m_AnimationTime * m_Animation->getRate(), m_TimeLimit);
		if (m_AnimationPathCallback->getAnimationPath()->getInterpolatedControlPoint(animation_time, cp)) {
			AnimationPathCallbackVisitor apcv(cp, m_AnimationPathCallback->getPivotPoint(), m_AnimationPathCallback->getUseInverseMatrix());
			node->accept(apcv);
		}
	}

public:
	void operator()(osg::Node* node, osg::NodeVisitor* nv) {
		if (nv->getVisitorType() == osg::NodeVisitor::UPDATE_VISITOR && nv->getFrameStamp()) {
			const double now = nv->getFrameStamp()->getReferenceTime();
			if (m_LastTime >= 0.0 && update()) {
				assert(m_Channel.value() >= 0);
				m_AnimationTime += (now - m_LastTime) * m_Direction;
				if ((m_AnimationTime - m_Target) * m_Direction > 0) {
					m_AnimationTime = m_Target;
					m_Pause = true;
				}
				updateAnimation(node);
			}
			m_LastTime = now;
		}
		traverse(node, nv);
	}

	// Picking just cycles the switch for now.  When flags are added we might also
	// allow reverse cycling.
	virtual bool pick(int /*flags*/) {
		m_Channel.cycle();
		return true;
	}

	Callback(AnimatedSwitch const *animated_switch, osg::AnimationPathCallback const *path):
		m_Animation(animated_switch),
		m_AnimationPathCallback(path),
		m_Target(0.0),
		m_LastTime(-1.0),
		m_AnimationTime(0.0),
		m_Direction(1.0),
		m_Pause(true)
	{
		assert(animated_switch);
		assert(path);
		assert(m_Animation->getRate() > 0.0);
		if (m_AnimationPathCallback->getAnimationPath()->getLoopMode() != osg::AnimationPath::LOOP) {
			CSPLOG(WARNING, OBJECT) << "Overriding loop mode of animation path " << m_Animation->getChannelName();
			osg::AnimationPath *ap = const_cast<osg::AnimationPath*>(m_AnimationPathCallback->getAnimationPath());
			ap->setLoopMode(osg::AnimationPath::LOOP);
		}
		m_TimeLimit = m_AnimationPathCallback->getAnimationPath()->getPeriod() - 0.001;  // prevent wraparound
	}

	virtual bool bindChannels(Bus *bus) { return m_Channel.bind(bus, m_Animation->getChannelName()); }
};

AnimationCallback *AnimatedSwitch::newCallback(osg::Node *node) const {
	assert(node);
	osg::AnimationPathCallback *oapc = dynamic_cast<osg::AnimationPathCallback*>(node->getUpdateCallback());
	if (!oapc) {
		CSPLOG(WARNING, OBJECT) << "AnimatedSwitch node has no animation path (channel " << getChannelName() << ")";
		return 0;
	}
	AnimationCallback *callback = new Callback(this, oapc);
	callback->bind(*node);
	return callback;
}


////////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////////

/** A specialized animation for attitude displays that show pitch and roll.
 *  Rotates the specified node by the pitch angle around the Z axis and by
 *  the roll angle around the Y axis.  The transformations are inserted before
 *  the existing node transform to operate in the base coordinate system of
 *  the 3D part.
 */
class AttitudeAnimation: public Animation {
	class Callback;
public:
	CSP_DECLARE_OBJECT(AttitudeAnimation)

	virtual AnimationCallback *newCallback(osg::Node *node) const;
};

CSP_XML_BEGIN(AttitudeAnimation)
CSP_XML_END


class AttitudeAnimation::Callback: public AnimationCallback {
	Ref<const AttitudeAnimation> m_Animation;
	DoubleChannel m_Pitch;
	DoubleChannel m_Roll;
	osg::Matrix m_Base;
	osg::Vec3 m_Origin;
	osg::MatrixTransform *m_Transform;

public:
	Callback(AttitudeAnimation const *animation): m_Animation(animation), m_Transform(0) { assert(animation); }

	void operator()(osg::Node* node, osg::NodeVisitor* nv) {
		if (nv->getVisitorType() == osg::NodeVisitor::UPDATE_VISITOR) {
			const bool pitch_update = m_Pitch.update();  // force eval
			const bool roll_update = m_Roll.update();  // force eval
			if (pitch_update || roll_update) {
				if (!m_Transform) {
					m_Transform = dynamic_cast<osg::MatrixTransform*>(node);
					assert(m_Transform);
					m_Origin = m_Transform->getMatrix().getTrans();
					m_Base = m_Transform->getMatrix();
				}
				if (m_Transform) {
					osg::Matrix matrix;
					matrix.makeRotate(m_Pitch.value(), 0, 0, 1);
					matrix.postMult(osg::Matrix::rotate(m_Roll.value(), 0, 1, 0));
					matrix.postMult(m_Base);
					m_Transform->setMatrix(matrix);
				}
			}
		}
		traverse(node, nv);
	}

	virtual bool bindChannels(Bus *bus) {
		const bool bind_pitch = m_Pitch.bind(bus, "Kinetics.Pitch");  // force eval
		const bool bind_roll = m_Roll.bind(bus, "Kinetics.Roll");  // force eval
		return bind_pitch || bind_roll;  // OR or AND?
	}
};

AnimationCallback *AttitudeAnimation::newCallback(osg::Node *node) const {
	assert(node);
	Callback *callback = new Callback(this);
	callback->bind(*node);
	return callback;
}


////////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////////

/** A rotation animation in which the rotation angle is set by the data channel and
 *  does not vary between updates.
 */
class DrivenRotation: public Rotation {
	class Callback;
	double m_RateLimit;
public:
	CSP_DECLARE_OBJECT(DrivenRotation)
	DrivenRotation(): m_RateLimit(0) { }
	virtual AnimationCallback *newCallback(osg::Node *node) const;
	double rateLimit() const { return m_RateLimit; }
};

CSP_XML_BEGIN(DrivenRotation)
	CSP_DEF("rate_limit", m_RateLimit, false)
CSP_XML_END


class DrivenRotation::Callback: public AnimationCallback {
	Ref<const DrivenRotation> m_Animation;
	DoubleChannel m_Channel;
	osg::MatrixTransform *m_Transform;
	osg::Matrix m_Original;
	double m_Angle;
	double m_AngleTarget;
	double m_LastTime;
	double m_RateLimit;

public:
	Callback(DrivenRotation const *driven_rotation): m_Animation(driven_rotation), m_Transform(0) {
		assert(driven_rotation);
		m_RateLimit = (driven_rotation->rateLimit());
	}

	void updateValue(double dt) {
		m_AngleTarget = m_Animation->rescaleAngle(m_Channel.value());
		if (m_RateLimit > 0) {
			const double delta = m_AngleTarget - m_Angle;
			if (delta < 0) {
				m_Angle -= std::min(-delta, dt * m_RateLimit);
			} else {
				m_Angle += std::min(delta, dt * m_RateLimit);
			}
		} else {
			m_Angle = m_AngleTarget;
		}
	}

	virtual void operator()(osg::Node* node, osg::NodeVisitor* nv) {
		if (nv->getVisitorType() == osg::NodeVisitor::UPDATE_VISITOR) {
			const double dt = elapsedTime(nv);
			if (m_Channel.update() || m_Angle != m_AngleTarget) {
				if (!m_Transform) {
					m_Transform = dynamic_cast<osg::MatrixTransform*>(node);
					assert(m_Transform);
					m_Original = m_Transform->getMatrix();
				}
				if (m_Transform) {
					updateValue(dt);
					const osg::Vec3 t = m_Transform->getMatrix().getTrans();
					osg::Matrix m = osg::Matrix::rotate(m_Angle, m_Animation->getAxis());
					if (node->getUpdateCallback() != this) {
						m.preMult(m_Transform->getMatrix());
					}
					if (m_Animation->getPostMultiply()) {
						m.preMult(m_Original);
					} else if (m_Animation->getPreMultiply()) {
						m.postMult(m_Original);
					}
					m.setTrans(t);
					m_Transform->setMatrix(m);
				}
			}
		}
		traverse(node, nv);
	}

	virtual bool bindChannels(Bus *bus) { return m_Channel.bind(bus, m_Animation->getChannelName()); }
};

AnimationCallback *DrivenRotation::newCallback(osg::Node *node) const {
	assert(node);
	Callback *callback = new Callback(this);
	callback->bind(*node);
	return callback;
}

////////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////////

/** Helper class for specifying and manipulating a time range.
 */
class TimedAnimationProxy: public Object {
	float m_t0;
	float m_t1;
	float m_TimeLength;
	float m_Rate;

public:
	CSP_DECLARE_OBJECT(TimedAnimationProxy)
	
	TimedAnimationProxy():
		m_t0(0.0f),
		m_t1(0.0f),
		m_TimeLength(1.0f),
		m_Rate(0.0f) {
	}
	~TimedAnimationProxy(){}
	float getDelta_t0(float t) const { return clamp(t) - m_t0; }
	float clamp(float t) const { return clampTo(t, m_t0, m_t1); }
	float getRate() const { return m_Rate; }
	void setRate(float limit0, float limit1) {
		float delta_limit = limit1 - limit0;
		m_Rate = delta_limit / m_TimeLength;
	}
	float getTimeLength() const { return m_TimeLength; }
	float gett_0() const { return m_t0; }
	float gett_1() const { return m_t1; }

protected:
	virtual void postCreate() {
		Object::postCreate();
		m_TimeLength = m_t1 - m_t0;
	}
};

CSP_XML_BEGIN(TimedAnimationProxy)
	CSP_DEF("t0", m_t0, true)
	CSP_DEF("t1", m_t1, true)
CSP_XML_END


////////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////////

/** A rotation that is driven by a data channel representing time.  The timed
 *  animation proxy and rotation limits define the mapping between the time
 *  channel and the rotation angle.  Note that limit1 can be less than limit0,
 *  which simply means that the rotation angle decreases with increasing time.
 */
class TimedRotation: public Rotation {
	class Callback;
	Link<TimedAnimationProxy> m_TimedAnimationProxy;

public:
	CSP_DECLARE_OBJECT(TimedRotation)

	double getTimedAngle(double t) const {
		return getLimit0() + m_TimedAnimationProxy->getRate() * m_TimedAnimationProxy->getDelta_t0(t);
	}

	virtual AnimationCallback *newCallback(osg::Node *node) const;

protected:
	virtual void postCreate() {
		Rotation::postCreate();
		m_TimedAnimationProxy->setRate(getLimit0(), getLimit1());
	}
};

CSP_XML_BEGIN(TimedRotation)
	CSP_DEF("timed_animation_proxy", m_TimedAnimationProxy, true)
CSP_XML_END


class TimedRotation::Callback: public AnimationCallback {
	Ref<const TimedRotation> m_Animation;
	DoubleChannel m_Channel;
	osg::MatrixTransform *m_Transform;

public:
	Callback(TimedRotation const *animation): m_Animation(animation), m_Transform(0) { assert(animation); }
	virtual void operator()(osg::Node* node, osg::NodeVisitor* nv) {
		if (nv->getVisitorType() == osg::NodeVisitor::UPDATE_VISITOR && m_Channel.update()) {
			if (!m_Transform) {
				m_Transform = dynamic_cast<osg::MatrixTransform*>(node);
				assert(m_Transform);
			}
			if (m_Transform) {
				osg::Matrix m = osg::Matrix::rotate(m_Animation->getTimedAngle(m_Channel.value()), m_Animation->getAxis());
				const osg::Vec3 translation = m_Transform->getMatrix().getTrans();
				const bool nested = this != node->getUpdateCallback();
				if (nested) m.preMult(m_Transform->getMatrix());
				m.setTrans(translation);
				m_Transform->setMatrix(m);
			}
		}
		traverse(node, nv);
	}
	virtual bool bindChannels(Bus *bus) { return m_Channel.bind(bus, m_Animation->getChannelName()); }
};

AnimationCallback *TimedRotation::newCallback(osg::Node *node) const {
	assert(node);
	Callback *callback = new Callback(this);
	callback->bind(*node);
	return callback;
}

////////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////////

/** Abstract animation class for translations.
 */
class Translation: public Animation {
	std::string m_ChannelName;
	float m_Gain;
	Vector3 m_Direction;
	Vector3 m_Offset;
	float m_Limit0;
	float m_Limit1;
	osg::Vec3 m_OSGOffset;
	osg::Vec3 m_OSGDirection;

public:
	CSP_DECLARE_ABSTRACT_OBJECT(Translation)

	Translation(): m_Gain(1.0f), m_Limit0(-1.0f), m_Limit1(1.0f) {}

	inline const std::string &getChannelName() const { return m_ChannelName; }
	inline float getGain() const { return m_Gain; }
	inline const osg::Vec3 &getDirection() const { return m_OSGDirection; }
	inline const osg::Vec3 &getOffset() const { return m_OSGOffset; }
	inline float getLimit0() const { return m_Limit0; }
	inline float getLimit1() const { return m_Limit1; }

protected:
	virtual void postCreate() {
		Animation::postCreate();
		m_OSGDirection = toOSG(m_Direction.normalized());
		m_OSGOffset = toOSG(m_Offset);
	}
};

CSP_XML_BEGIN(Translation)
	CSP_DEF("channel_name", m_ChannelName, true)
	CSP_DEF("gain", m_Gain, false)
	CSP_DEF("direction", m_Direction, false)
	CSP_DEF("offset", m_Offset, false)
	CSP_DEF("limit_0", m_Limit0, false)
	CSP_DEF("limit_1", m_Limit1, false)
CSP_XML_END


////////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////////

/** A translation animation that is driven by a double-precision floating point
 *  data channel.  The translation magnitude does not vary between updates.
 */
class DrivenMagnitudeTranslation: public Translation {
	class Callback;
public:
	CSP_DECLARE_OBJECT(DrivenMagnitudeTranslation)
	virtual AnimationCallback *newCallback(osg::Node *node) const;
};

CSP_XML_BEGIN(DrivenMagnitudeTranslation)
CSP_XML_END


class DrivenMagnitudeTranslation::Callback: public AnimationCallback {
	Ref<const DrivenMagnitudeTranslation> m_Animation;
	DoubleChannel m_Channel;
	osg::MatrixTransform *m_Transform;

public:
	Callback(DrivenMagnitudeTranslation const *animation): m_Animation(animation), m_Transform(0) { assert(animation); }

	virtual void operator()(osg::Node* node, osg::NodeVisitor* nv) {
		if (nv->getVisitorType() == osg::NodeVisitor::UPDATE_VISITOR && m_Channel.update()) {
			if (!m_Transform) {
				m_Transform = dynamic_cast<osg::MatrixTransform*>(node);
				assert(m_Transform);
			}
			if (m_Transform) {
				const osg::Vec3 translation = m_Animation->getDirection() * m_Animation->getGain() * m_Channel.value() + m_Animation->getOffset();
				osg::Matrix m = m_Transform->getMatrix();
				m.setTrans(translation);
				m_Transform->setMatrix(m);
			}
		}
		traverse(node, nv);
	}
	virtual bool bindChannels(Bus *bus) { return m_Channel.bind(bus, m_Animation->getChannelName()); }
};

AnimationCallback *DrivenMagnitudeTranslation::newCallback(osg::Node *node) const {
	assert(node);
	Callback *callback = new Callback(this);
	callback->bind(*node);
	return callback;
}

////////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////////

/** A translation animation that is driven by a vector data channel.  The actual
 *  displacement is the dot product of the data channel displacement and the
 *  translation axis, along the translation axis.  The displacement does not
 *  vary between updates.
 */
class DrivenVectorialTranslation: public Translation {
	class Callback;
public:
	CSP_DECLARE_OBJECT(DrivenVectorialTranslation)
	virtual AnimationCallback *newCallback(osg::Node *node) const;
};

CSP_XML_BEGIN(DrivenVectorialTranslation)
CSP_XML_END


class DrivenVectorialTranslation::Callback: public AnimationCallback {
	Ref<const DrivenVectorialTranslation> m_Animation;
	DataChannel<Vector3>::CRefT m_Channel;
	float m_Value;
	osg::MatrixTransform *m_Transform;

protected:
	virtual bool update() {
		if (m_Channel.valid()) {
			float value = m_Animation->getDirection() * toOSG(m_Channel->value());
			if (value != m_Value) {
				m_Value = value;
				return true;
			}
		}
		return false;
	}

public:
	Callback(const DrivenVectorialTranslation *animation): m_Animation(animation), m_Value(0), m_Transform(0) { assert(animation); }
	virtual void operator()(osg::Node* node, osg::NodeVisitor* nv) {
		if (nv->getVisitorType() == osg::NodeVisitor::UPDATE_VISITOR && update()) {
			if (!m_Transform) {
				m_Transform = dynamic_cast<osg::MatrixTransform*>(node);
				assert(m_Transform);
			}
			if (m_Transform) {
				const osg::Vec3 translation = m_Animation->getDirection() * m_Animation->getGain() * m_Value + m_Animation->getOffset();
				osg::Matrix m = m_Transform->getMatrix();
				m.setTrans(translation);
				m_Transform->setMatrix(m);
			}
		}
		traverse(node, nv);
	}
	virtual bool bindChannels(Bus *bus) {
		m_Channel = 0;
		if (bus) {
			try {
				m_Channel = bus->getChannel(m_Animation->getChannelName(), false);
			} catch (ConversionError &) {
				CSPLOG(WARNING, OBJECT) << "Incompatible channel (" << m_Animation->getChannelName() << ") type for DrivenVectorialTranslation";
			}
		}
		return m_Channel.valid();
	}
};

AnimationCallback *DrivenVectorialTranslation::newCallback(osg::Node *node) const {
	assert(node);
	Callback *callback = new Callback(this);
	callback->bind(*node);
	return callback;
}


////////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////////

/** A translation animation that is driven by a data channel representing time.
 *  The timed animation proxy and displacement limits define the mapping between
 *  the time channel and the displacement.  Note that limit1 can be less than
 *  limit0, which simply means that the displacement decreases with increasing time.
 */
class TimedMagnitudeTranslation: public Translation {
	class Callback;
	Link<TimedAnimationProxy> m_TimedAnimationProxy;
	
public:
	CSP_DECLARE_OBJECT(TimedMagnitudeTranslation)

	virtual AnimationCallback *newCallback(osg::Node *node) const;

	osg::Vec3 getTimedTranslation(double t) const {
		return getDirection() * (getLimit0() + m_TimedAnimationProxy->getRate() * m_TimedAnimationProxy->getDelta_t0(t));
	}

protected:
	virtual void postCreate() {
		Translation::postCreate();
		m_TimedAnimationProxy->setRate(getLimit0(),getLimit1());
	}
};

CSP_XML_BEGIN(TimedMagnitudeTranslation)
	CSP_DEF("timed_animation_proxy", m_TimedAnimationProxy, true)
CSP_XML_END


class TimedMagnitudeTranslation::Callback: public AnimationCallback {
	Ref<const TimedMagnitudeTranslation> m_Animation;
	DoubleChannel m_Channel;
	osg::MatrixTransform *m_Transform;

public:
	Callback(TimedMagnitudeTranslation const *animation): m_Animation(animation), m_Transform(0) { assert(animation); }

	virtual void operator()(osg::Node* node, osg::NodeVisitor* nv) {
		if (nv->getVisitorType() == osg::NodeVisitor::UPDATE_VISITOR && m_Channel.update()) {
			if (!m_Transform) {
				m_Transform = dynamic_cast<osg::MatrixTransform*>(node);
				assert(m_Transform);
			}
			if (m_Transform) {
				const osg::Vec3 translation = m_Animation->getTimedTranslation(m_Channel.value()) * m_Animation->getGain() + m_Animation->getOffset();
				osg::Matrix m = m_Transform->getMatrix();
				m.setTrans(translation);
				m_Transform->setMatrix(m);
			}
		}
		traverse(node, nv);
	}

	virtual bool bindChannels(Bus *bus) { return m_Channel.bind(bus, m_Animation->getChannelName()); }
};

AnimationCallback *TimedMagnitudeTranslation::newCallback(osg::Node *node) const {
	assert(node);
	Callback *callback = new Callback(this);
	callback->bind(*node);
	return callback;
}


////////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////////

/** An animation that binds a double-precision floating point data channel to an
 *  existing animation path.  The xml parameters define the mapping between the
 *  input value and the animation path time.
 */
class DrivenAnimationPath: public Animation {
	class Callback;
	std::string m_ChannelName;
	float m_Gain;
	float m_Offset;
	float m_Limit0;
	float m_Limit1;

public:
	CSP_DECLARE_OBJECT(DrivenAnimationPath)

	DrivenAnimationPath(): m_Gain(1.0), m_Offset(0.0), m_Limit0(-1e+10), m_Limit1(1e+10) { }

	inline const std::string &getChannelName() const { return m_ChannelName; }
	inline float getGain() const { return m_Gain; }
	inline float getOffset() const { return m_Offset; }
	inline float getLimit0() const { return m_Limit0; }
	inline float getLimit1() const { return m_Limit1; }

	virtual AnimationCallback *newCallback(osg::Node *node) const;
};

CSP_XML_BEGIN(DrivenAnimationPath)
	CSP_DEF("channel_name", m_ChannelName, true)
	CSP_DEF("gain", m_Gain, false)
	CSP_DEF("offset", m_Offset, false)
	CSP_DEF("limit_0", m_Limit0, false)
	CSP_DEF("limit_1", m_Limit1, false)
CSP_XML_END


class DrivenAnimationPath::Callback: public AnimationCallback {
	Ref<const DrivenAnimationPath> m_Animation;
	osg::ref_ptr<const osg::AnimationPathCallback> m_AnimationPathCallback;
	DoubleChannel m_Channel;

public:
	Callback(DrivenAnimationPath const *animation, osg::AnimationPathCallback const &oapc): m_Animation(animation), m_AnimationPathCallback(&oapc) { assert(animation); }

	virtual void operator()(osg::Node* node, osg::NodeVisitor* nv) {
		if (nv->getVisitorType() == osg::NodeVisitor::UPDATE_VISITOR && nv->getFrameStamp() && m_Channel.update()) {
			const double value = clampTo(m_Channel.value() * m_Animation->getGain() + m_Animation->getOffset(), m_Animation->getLimit0(), m_Animation->getLimit1());
			updateAnimation(node, value);
		}
		traverse(node, nv);
	}
	virtual bool bindChannels(Bus *bus) { return m_Channel.bind(bus, m_Animation->getChannelName()); }

private:
	void updateAnimation(osg::Node *node, double time) {
		osg::AnimationPath::ControlPoint cp;
		if (m_AnimationPathCallback->getAnimationPath()->getInterpolatedControlPoint(time, cp)) {
			AnimationPathCallbackVisitor apcv(cp, m_AnimationPathCallback->getPivotPoint(), m_AnimationPathCallback->getUseInverseMatrix());
			node->accept(apcv);
		}
	}
};

AnimationCallback *DrivenAnimationPath::newCallback(osg::Node *node) const {
	osg::AnimationPathCallback *oapc = dynamic_cast<osg::AnimationPathCallback*>(node->getUpdateCallback());
	if (!oapc) {
		CSPLOG(WARNING, OBJECT) << "DrivenAnimationPath node has no animation path (channel " << getChannelName() << ")";
		return 0;
	}
	Callback *callback = new Callback(this, *oapc);
	callback->bind(*node);
	return callback;
}


////////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////////

/** An specialized animation for driving multiple counter wheels to display a
 *  single value.  Each wheel represents a different digit of the input value,
 *  and rotation through 2 PI cycles a wheel through the values 0 to 9.  The
 *  model nodes should be labelled as "ANIM: anim_name: N" where N is an integer
 *  ranging from 0 for the lowest digit, and increasing sequentially for each
 *  successive digit (factor of 10).  A digit advances (linearly) whenever the
 *  preceding digit transitions from 9 to 0.
 */
class CounterWheel: public Animation {
	class Callback;

public:
	CSP_DECLARE_OBJECT(CounterWheel)

	CounterWheel():
		m_Axis(Vector3::ZERO),
		m_Gain(1.0f),
		m_Offset(0.0f)
	{ }

	inline const std::string &getChannelName() const { return m_ChannelName; }
	inline float getGain() const { return m_Gain; }
	inline float getOffset() const { return m_Offset; }
	inline const osg::Vec3 &getAxis() const { return m_OSGAxis; }

	virtual AnimationCallback *newCallback(osg::Node *node) const;

protected:
	virtual void postCreate() {
		Animation::postCreate();
		m_OSGAxis = toOSG(m_Axis);
	}

private:
	Vector3 m_Axis;
	float m_Gain;
	float m_Offset;
	std::string m_ChannelName;
	osg::Vec3 m_OSGAxis;

};

CSP_XML_BEGIN(CounterWheel)
	CSP_DEF("channel_name", m_ChannelName, false)
	CSP_DEF("gain", m_Gain, false)
	CSP_DEF("offset", m_Offset, false)
	CSP_DEF("axis", m_Axis, false)
CSP_XML_END


class CounterWheel::Callback: public AnimationCallback {
	Ref<const CounterWheel> m_Animation;
	DoubleChannel m_Channel;
	float m_Scale;  // power of 10
	osg::MatrixTransform *m_Transform;
	osg::Matrix m_Original;

public:
	Callback(CounterWheel const *counter_wheel, float scale): m_Animation(counter_wheel), m_Scale(scale), m_Transform(0) { assert(counter_wheel); }

	virtual void operator()(osg::Node* node, osg::NodeVisitor* nv) {
		if (nv->getVisitorType() == osg::NodeVisitor::UPDATE_VISITOR && m_Channel.update()) {
			if (!m_Transform) {
				m_Transform = dynamic_cast<osg::MatrixTransform*>(node);
				assert(m_Transform);
				m_Original = m_Transform->getMatrix();
			}
			if (m_Transform) {
				const float value = m_Animation->getGain() * m_Channel.value() + m_Animation->getOffset();
				float angle = 0.0;
				if (m_Scale > 1.0f) {
					angle = static_cast<float>((floor(value / m_Scale) + 0.1 * std::max(0.0, fmod(value * 10.0, 10.0 * m_Scale) - (10.0 * m_Scale - 10.0))) * (0.2 * PI));
				} else {
					angle = static_cast<float>(fmodf(value, 10.0f) * (0.2 * PI));
				}
				const osg::Vec3 t = m_Transform->getMatrix().getTrans();
				osg::Matrix m = osg::Matrix::rotate(angle, m_Animation->getAxis());
				m.preMult(m_Original);
				m.setTrans(t);
				m_Transform->setMatrix(m);
			}
		}
		traverse(node, nv);
	}

	virtual bool bindChannels(Bus *bus) { return m_Channel.bind(bus, m_Animation->getChannelName()); }
};

AnimationCallback *CounterWheel::newCallback(osg::Node *node) const {
	assert(node);
	std::string name = node->getName();
	std::string::size_type split = name.rfind(':');
	assert(split != std::string::npos);
	int n = atoi(name.c_str() + split + 1);
	assert(n >= 0 && n <= 8);
	float scale = powf(10.0f, n);
	Callback *callback = new Callback(this, scale);
	callback->bind(*node);
	return callback;
}


////////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////////

/** An animated switch with one or more active states that can be momentarily
 *  entered before returning to the default state.  For example, the ICP numeric
 *  buttons (one state) and the data control switch (4 states) in the F16 cockpit.
 */
class AnimatedMomentarySwitch: public Animation {
	class Callback;
public:
	CSP_DECLARE_OBJECT(AnimatedMomentarySwitch)

	virtual AnimationCallback *newCallback(osg::Node *node) const;
	inline const std::vector<std::string> &getEventChannelNames() const { return m_EventChannelNames; }
	inline double getCycleTime() const { return m_CycleTime; }

	AnimatedMomentarySwitch(): m_CycleTime(0.1) { }

private:
	std::vector<std::string> m_EventChannelNames;
	double m_CycleTime;
};

CSP_XML_BEGIN(AnimatedMomentarySwitch)
	CSP_DEF("event_channel_names", m_EventChannelNames, true)
	CSP_DEF("cycle_time", m_CycleTime, false)
CSP_XML_END


class AnimatedMomentarySwitch::Callback: public AnimationCallback, public sigc::trackable {
	Ref<const AnimatedMomentarySwitch> m_Animation;
	osg::ref_ptr<const osg::AnimationPathCallback> m_AnimationPathCallback;
	osg::AnimationPath::ControlPoint m_DefaultPoint;
	std::vector<osg::AnimationPath::ControlPoint> m_ControlPoints;
	std::vector<InputEventChannel::RefT> m_EventChannels;
	unsigned m_Index;
	double m_Cycle;
	double m_Scale;
	double m_LastTime;
	bool m_Pause;

	void initControlPoints() {
		assert(m_ControlPoints.empty());
		const unsigned points = m_Animation->getEventChannelNames().size();
		osg::AnimationPath::TimeControlPointMap const &pointmap = m_AnimationPathCallback->getAnimationPath()->getTimeControlPointMap();
		assert(pointmap.size() >= points + 1);
		osg::AnimationPath::TimeControlPointMap::const_iterator iter = pointmap.begin();
		m_DefaultPoint = iter->second;
		m_ControlPoints.reserve(pointmap.size() - 1);
		for (++iter; iter != pointmap.end() && m_ControlPoints.size() < points; ++iter) {
			m_ControlPoints.push_back(iter->second);
		}
	}

protected:
	void updateAnimation(osg::Node *node) {
		osg::AnimationPath::ControlPoint cp;
		cp.interpolate(m_Cycle, m_DefaultPoint, m_ControlPoints[m_Index]);
		AnimationPathCallbackVisitor apcv(cp, m_AnimationPathCallback->getPivotPoint(), m_AnimationPathCallback->getUseInverseMatrix());
		node->accept(apcv);
	}

	void onEvent(unsigned index) {
		assert(index < m_ControlPoints.size());
		if (m_Pause) {
			m_Pause = false;
			m_Index = index;
			m_Cycle = 0.0;
			m_Scale = 2.0 / m_Animation->getCycleTime();
			assert(m_Scale >= 0.0);
		}
	}

public:
	void operator()(osg::Node* node, osg::NodeVisitor* nv) {
		if (nv->getVisitorType() == osg::NodeVisitor::UPDATE_VISITOR && nv->getFrameStamp() && !m_Pause) {
			const double now = nv->getFrameStamp()->getReferenceTime();
			if (m_LastTime >= 0.0) {
				m_Cycle += m_Scale * (now - m_LastTime);
				if (m_Cycle > 1.0) {
					m_Cycle = 1.0;
					m_Scale = -std::abs(m_Scale);
				} else if (m_Cycle < 0.0) {
					m_Cycle = 0.0;
					m_Pause = true;
				}
				updateAnimation(node);
			}
			m_LastTime = now;
		}
		traverse(node, nv);
	}

	// TODO implement pick geodes
	virtual bool pick(int /*flags*/) {
		if (m_EventChannels.size() == 1 && m_EventChannels[0].valid()) {
			m_EventChannels[0]->signal();
			return true;
		}
		return false;
	}

	Callback(AnimatedMomentarySwitch const *animation, osg::AnimationPathCallback const *path):
		m_Animation(animation),
		m_AnimationPathCallback(path),
		m_Index(0),
		m_Cycle(0.0),
		m_Scale(-0.1),
		m_LastTime(-1.0),
		m_Pause(false)  // force initial reset
	{
		assert(animation);
		assert(path);
		initControlPoints();
	}

	virtual bool bindChannels(Bus *bus) {
		notify_callbacks();  // remove all callbacks (sigc::trackable)
		m_EventChannels.clear();
		if (bus) {
			const std::vector<std::string> &channel_names = m_Animation->getEventChannelNames();
			m_EventChannels.reserve(channel_names.size());
			for (unsigned i = 0; i < channel_names.size(); ++i) {
				InputEventChannel::RefT channel = bus->getSharedChannel(channel_names[i], false);
				if (channel.valid()) {
					channel->connect(sigc::bind(sigc::mem_fun(this, &Callback::onEvent), i));
				} else {
					CSPLOG(WARNING, OBJECT) << "InputEventChannel " << channel_names[i] << " not found; ignoring.";
				}
				m_EventChannels.push_back(channel);
			}
		}
		return true;
	}
};

AnimationCallback *AnimatedMomentarySwitch::newCallback(osg::Node *node) const {
	assert(node);
	osg::AnimationPathCallback *oapc = dynamic_cast<osg::AnimationPathCallback*>(node->getUpdateCallback());
	if (!oapc) {
		CSPLOG(WARNING, OBJECT) << "AnimatedMomentarySwitch node has no animation path (" << node->getName() << ")";
		return 0;
	}
	AnimationCallback *callback = new Callback(this, oapc);
	callback->bind(*node);
	return callback;
}

CSP_NAMESPACE_END

