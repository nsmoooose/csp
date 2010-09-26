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
 * @file FlightControlSystem.cpp
 *
 * Control nodes for defining (nearly) arbitrary flight control systems.  These
 * stock components work well for flight control systems that can be defined by
 * analog circuits.  If the control system involves complicated combinatorial
 * logic it is probably best to create a custom control node to encapsulate that
 * logic as a "black box."  Note however that nodes can have only one direct
 * output, which places restrictions on the types of black box nodes that can
 * be created.
 **/

#include <csp/cspsim/ControlNode.h>
#include <csp/cspsim/FlightControlSystem.h>
#include <csp/cspsim/System.h>
#include <csp/cspsim/DataRecorder.h>

#include <csp/csplib/data/LUT.h>
#include <csp/csplib/data/ObjectInterface.h>

#include <cstdlib>
#include <vector>


namespace csp {

/** @bug circuits can create circular references.  need to add an "unlink" function to zero all references, and call that for each node when the fcs system is destroyed (otherwise we leak memory). */

//namespace fcsnode {

CSP_XML_BEGIN(ControlNode)
	CSP_DEF("id", m_ID, true)
	CSP_DEF("clamp_lo", m_ClampLo, false)
	CSP_DEF("clamp_hi", m_ClampHi, false)
	CSP_DEF("rate_limit_dec", m_RateLimitDec, false)
	CSP_DEF("rate_limit_inc", m_RateLimitInc, false)
CSP_XML_END


/** An abstract node that defines a single input channel.
 */
class Junction1: public ControlNode {
	ControlNode::RefT m_Input;
	TypeID m_InputID;
	double m_Gain, m_Offset;
public:
	CSP_DECLARE_ABSTRACT_OBJECT(Junction1)
	Junction1(): m_Gain(1.0), m_Offset(0.0) {}
protected:
	void link(MapID &map) {
		m_Input = map[m_InputID];
		if (!m_Input) {
			CSPLOG(ERROR, APP) << "Cannot find input channel for FCS junction node " << getID();
			assert(0); /** @TODO make fatal */
		}
		assert(m_Input.valid()); }
	double getInput(Timer const &dt) { return m_Input->step(dt) * m_Gain + m_Offset; }
};

CSP_XML_BEGIN(Junction1)
	CSP_DEF("input", m_InputID, true)
	CSP_DEF("gain", m_Gain, false)
	CSP_DEF("offset", m_Offset, false)
CSP_XML_END


/** An abstract node that defines two input channels.
 */
class Junction2: public ControlNode {
	ControlNode::RefT m_InputA;
	ControlNode::RefT m_InputB;
	TypeID m_InputIDA;
	TypeID m_InputIDB;
	double m_GainA, m_GainB, m_OffsetA, m_OffsetB;
public:
	CSP_DECLARE_ABSTRACT_OBJECT(Junction2)
	Junction2(): m_GainA(1.0), m_GainB(1.0), m_OffsetA(0.0), m_OffsetB(0.0) {}
protected:
	void link(MapID &map) {
		m_InputA = map[m_InputIDA];
		m_InputB = map[m_InputIDB];
		if (!m_InputA) {
			CSPLOG(ERROR, APP) << "Cannot find input channel A for FCS Junction2 node " << getID();
			assert(0); /** @TODO make fatal */
		}
		if (!m_InputB) {
			CSPLOG(ERROR, APP) << "Cannot find input channel B for FCS Junction2 node " << getID();
			assert(0); /** @TODO make fatal */
		}
	}
	double getInputA(Timer const &dt) { return m_InputA->step(dt) * m_GainA + m_OffsetA; }
	double getInputB(Timer const &dt) { return m_InputB->step(dt) * m_GainB + m_OffsetB; }
};

CSP_XML_BEGIN(Junction2)
	CSP_DEF("input_a", m_InputIDA, true)
	CSP_DEF("gain_a", m_GainA, false)
	CSP_DEF("offset_a", m_OffsetA, false)
	CSP_DEF("input_b", m_InputIDB, true)
	CSP_DEF("gain_b", m_GainB, false)
	CSP_DEF("offset_b", m_OffsetB, false)
CSP_XML_END


/** An abstract node that defines three input channels.
 */
class Junction3: public ControlNode {
	ControlNode::RefT m_InputA;
	ControlNode::RefT m_InputB;
	ControlNode::RefT m_InputC;
	TypeID m_InputIDA;
	TypeID m_InputIDB;
	TypeID m_InputIDC;
	double m_GainA, m_GainB, m_GainC, m_OffsetA, m_OffsetB, m_OffsetC;
public:
	CSP_DECLARE_ABSTRACT_OBJECT(Junction3)
	Junction3(): m_GainA(1.0), m_GainB(1.0), m_GainC(1.0), m_OffsetA(0.0), m_OffsetB(0.0), m_OffsetC(0.0) { }
protected:
	void link(MapID &map) {
		m_InputA = map[m_InputIDA];
		m_InputB = map[m_InputIDB];
		m_InputC = map[m_InputIDC];
		if (!m_InputA) {
			CSPLOG(ERROR, APP) << "Cannot find input channel A for FCS Junction3 node " << getID();
			assert(0); /** TODO make fatal */
		}
		if (!m_InputB) {
			CSPLOG(ERROR, APP) << "Cannot find input channel B for FCS Junction3 node " << getID();
			assert(0); /** TODO make fatal */
		}
		if (!m_InputC) {
			CSPLOG(ERROR, APP) << "Cannot find input channel C for FCS Junction3 node " << getID();
			assert(0); /** TODO make fatal */
		}
	}
	double getInputA(Timer const &dt) { return m_InputA->step(dt) * m_GainA + m_OffsetA; }
	double getInputB(Timer const &dt) { return m_InputB->step(dt) * m_GainB + m_OffsetB; }
	double getInputC(Timer const &dt) { return m_InputC->step(dt) * m_GainC + m_OffsetC; }
};

CSP_XML_BEGIN(Junction3)
	CSP_DEF("input_a", m_InputIDA, true)
	CSP_DEF("gain_a", m_GainA, false)
	CSP_DEF("offset_a", m_OffsetA, false)
	CSP_DEF("input_b", m_InputIDB, true)
	CSP_DEF("gain_b", m_GainB, false)
	CSP_DEF("offset_b", m_OffsetB, false)
	CSP_DEF("input_c", m_InputIDC, true)
	CSP_DEF("gain_c", m_GainC, false)
	CSP_DEF("offset_c", m_OffsetC, false)
CSP_XML_END


/** A simple low pass filter with laplace transform H(s) = a / (s+a).
 *
 * The IIR digital filter is generated using a bilinear transform *without*
 * frequency prewarping.
 *
 * For reference:
 *
 * s = 2/T * (z-1)/(z+1)  <bilinear transform>
 *
 * H(z) = aT (z+1) / [ 2 * (z-1) + aT * (z+1) ]
 *
 *      = aT (z+1) / [ (aT+2) z + (aT-2) ]
 *
 *      = aT/2 (1+Z) / [ (aT/2+1) + (aT/2-1) Z ]
 */
class LagFilter: public Junction1 {
	double m_Chi;
	double m_Input0;
public:
	CSP_DECLARE_OBJECT(LagFilter)
	LagFilter(): m_Chi(0.0), m_Input0(0.0) { }
protected:
	void postCreate() {
		//m_Chi *= 0.1592;
	}
private:
	virtual void evaluate(Timer const &timer) {
		double input = getInput(timer);
		double output = getOutput();
		double f = timer.dt() * m_Chi;
		f = f / (2.0 + f);
		setOutput(output + f * ((input + m_Input0) - 2.0 * output), timer);
		m_Input0 = input;
	}
};

CSP_XML_BEGIN(LagFilter)
	CSP_DEF("a", m_Chi, true)
CSP_XML_END


/** Lead-lag filter:  H(s) = c * (s + a) / (s + b)
 *  NB: no frequency prewarping is applied in generating the IIR digital filter.
 */
class LeadLagFilter: public Junction1 {
	double m_A, m_B, m_C;
	double m_Input0;
public:
	CSP_DECLARE_OBJECT(LeadLagFilter)
	LeadLagFilter(): m_A(0.0), m_B(0.0), m_C(0.0), m_Input0(0.0) { }
private:
	void evaluate(Timer const &timer) {
		const double input = getInput(timer) * m_C;
		const double output = getOutput();
		const double a = m_A * timer.dt();
		const double b = m_B * timer.dt();
		setOutput(((2.0 - b) * output + (a + 2.0) * input + (a - 2.0) * m_Input0) / (2.0 + b) , timer);
		m_Input0 = input;
	}
};

CSP_XML_BEGIN(LeadLagFilter)
	CSP_DEF("a", m_A, true)
	CSP_DEF("b", m_B, true)
	CSP_DEF("c", m_C, true)
CSP_XML_END


/** A simple high-pass filter: H(s) = s / (s + a)
 */
class LeadFilter: public Junction1 {
	double m_A;
	double m_Input0;
public:
	CSP_DECLARE_OBJECT(LeadFilter)
	LeadFilter(): m_A(1.0), m_Input0(0.0) { }
private:
	void evaluate(Timer const &timer) {
		double input = getInput(timer);
		double output = getOutput();
		double g = m_A * timer.dt();
		double d = 1.0 / (2.0 + g);
		setOutput((2.0 - g) * d * output + 2.0 * d * (input - m_Input0), timer);
		m_Input0 = input;
	}
};

CSP_XML_BEGIN(LeadFilter)
	CSP_DEF("a", m_A, true)
CSP_XML_END


/**
 * A node for integrating an input channel with respect to time.  Equivalent
 * to a laplace transform H(s) = 1 / s.
 */
class Integrator: public Junction1 {
public:
	CSP_DECLARE_OBJECT(Integrator)
	Integrator() { }
private:
	void evaluate(Timer const &timer) {
		setOutput(getOutput() + timer.dt() * getInput(timer), timer);
	}
};

CSP_XML_BEGIN(Integrator)
CSP_XML_END


/** A node for multiplying (mixing) to inputs.
 */
class Multiply: public Junction2 {
public:
	CSP_DECLARE_OBJECT(Multiply)
private:
	virtual void evaluate(Timer const &timer) {
		setOutput(getInputA(timer) * getInputB(timer), timer);
	}
};

CSP_XML_BEGIN(Multiply)
CSP_XML_END


/** A node for dividing two inputs.  Caution: can result in infinities or
 *  NaN's.
 */
class Divide: public Junction2 {
public:
	CSP_DECLARE_OBJECT(Divide)
private:
	virtual void evaluate(Timer const &timer) {
		setOutput(getInputA(timer) / getInputB(timer), timer);
	}
};

CSP_XML_BEGIN(Divide)
CSP_XML_END


/** A node for adding two inputs with independent gains.
 */
class Adder: public Junction2 {
public:
	CSP_DECLARE_OBJECT(Adder)
private:
	virtual void evaluate(Timer const &timer) {
		setOutput(getInputA(timer) + getInputB(timer), timer);
	}
};

CSP_XML_BEGIN(Adder)
CSP_XML_END


/** A node for adding three inputs with independent gains.
 */
class Adder3: public Junction3 {
public:
	CSP_DECLARE_OBJECT(Adder3)
private:
	virtual void evaluate(Timer const &timer) {
		setOutput(getInputA(timer) + getInputB(timer) + getInputC(timer), timer);
	}
};

CSP_XML_BEGIN(Adder3)
CSP_XML_END


/** A single input node for appling an offset and a gain.
 */
class Scale: public Junction1 {
public:
	CSP_DECLARE_OBJECT(Scale)
private:
	virtual void evaluate(Timer const &timer) {
		setOutput(getInput(timer), timer);
	}
};

CSP_XML_BEGIN(Scale)
CSP_XML_END


/** A programmable schedule.  The output is the scheduled value of
 *  the input channel.
 *
 *  XML interface:
 *    schedule: A 1D lookup table indexed by the input channel.
 */
class Schedule1: public Junction1 {
	Table1 m_Schedule;
public:
	CSP_DECLARE_OBJECT(Schedule1)
	Schedule1() {}
private:
	virtual void evaluate(Timer const &timer) {
		setOutput(m_Schedule[static_cast<float>(getInput(timer))], timer);
	}
};

CSP_XML_BEGIN(Schedule1)
	CSP_DEF("schedule", m_Schedule, true)
CSP_XML_END


/** A two channel programmable schedule.  The output is the product of
 *  input channel A and the scheduled value of input channel B.
 *
 *  XML interface:
 *    schedule: A 1D lookup table indexed by input channel B.
 */
class Schedule2: public Junction2 {
	Table1 m_Schedule;
public:
	CSP_DECLARE_OBJECT(Schedule2)
	Schedule2() {}
private:
	virtual void evaluate(Timer const &timer) {
		setOutput(getInputA(timer) * m_Schedule[static_cast<float>(getInputB(timer))], timer);
	}
};

CSP_XML_BEGIN(Schedule2)
	CSP_DEF("schedule", m_Schedule, true)
CSP_XML_END


/** A three channel junction that passes either input channel A or
 *  input channel B depending on the value of input channel C.
 *
 *  XML interface:
 *    compare: A scalar value to which input channel C is compared.
 *      When input channel C is less than this value, input channel
 *      A is passed through to the output.  Otherwise input channel
 *      B is passed through.
 */
class Switch: public Junction3 {
	double m_Compare;
public:
	CSP_DECLARE_OBJECT(Switch)
private:
	virtual void evaluate(Timer const &timer) {
		if (getInputC(timer) < m_Compare) {
			setOutput(getInputA(timer), timer);
		} else {
			setOutput(getInputB(timer), timer);
		}
	}
};

CSP_XML_BEGIN(Switch)
	CSP_DEF("compare", m_Compare, true)
CSP_XML_END


/** A three channel junction that passes either input channel A or
 *  input channel B depending whether input channel C is true or
 *  false.
 *
 *  XML interface:
 *    input_c: The name of a boolean channel that is used to select
 *      input channel A (true) or input channel B (false).
 */
class BooleanSwitch: public Junction2 {
	DataChannel<bool>::CRefT m_InputC;
	std::string m_SwitchChannel;
public:
	CSP_DECLARE_OBJECT(BooleanSwitch)
	virtual void importChannels(Bus *bus) {
		Junction2::importChannels(bus);
		m_InputC = bus->getChannel(m_SwitchChannel);
	}
private:
	virtual void evaluate(Timer const &timer) {
		setOutput(m_InputC->value() ? getInputA(timer) : getInputB(timer), timer);
	}
};

CSP_XML_BEGIN(BooleanSwitch)
	CSP_DEF("channel", m_SwitchChannel, true)
CSP_XML_END


/** A simple control node that provides a constant output value.  The
 *  output value is specified by the XML interface.
 */
class Constant: public ControlNode {
	double m_Value;
public:
	CSP_DECLARE_OBJECT(Constant)
	Constant(): m_Value(0.0) {}
protected:
	void postCreate() {
		setInitialOutput(m_Value);
	}

private:
	virtual void evaluate(Timer const &) { }
};

CSP_XML_BEGIN(Constant)
	CSP_DEF("value", m_Value, true)
CSP_XML_END


/** An abstract control node that provides access to values on the system
 *  bus.  Concrete subclasses provide access to specific data channel types.
 *
 *  XML interface:
 *    channel: The (string) name of the system bus channel.
 */
class InputChannel: public ControlNode {
	std::string m_Name;
	double m_Scale;
public:
	CSP_DECLARE_ABSTRACT_OBJECT(InputChannel)
	InputChannel(): m_Scale(1.0) { }
	virtual void importChannels(Bus *bus) {
		DataChannelBase::CRefT channel;
		channel = bus->getChannel(m_Name);
		setChannel(channel);
	}
protected:
	virtual double getChannelValue() const = 0;
	virtual void setChannel(DataChannelBase::CRefT &channel)=0;
private:
	virtual void evaluate(Timer const &timer) {
		setOutput(getChannelValue() * m_Scale, timer);
	}
};

CSP_XML_BEGIN(InputChannel)
	CSP_DEF("channel", m_Name, true)
	CSP_DEF("scale", m_Scale, false)
CSP_XML_END


/** An input channel that provides access to the "X" component of a data
 *  channel vector.
 */
class InputVectorXChannel: public InputChannel {
	DataChannel<Vector3>::CRefT m_Channel;
public:
	CSP_DECLARE_OBJECT(InputVectorXChannel)
protected:
	virtual double getChannelValue() const { return m_Channel->value().x(); }
	virtual void setChannel(DataChannelBase::CRefT &channel) { m_Channel = channel; }
};

CSP_XML_BEGIN(InputVectorXChannel)
CSP_XML_END


/** An input channel that provides access to the "Y" component of a data
 *  channel vector.
 */
class InputVectorYChannel: public InputChannel {
	DataChannel<Vector3>::CRefT m_Channel;
public:
	CSP_DECLARE_OBJECT(InputVectorYChannel)
protected:
	virtual double getChannelValue() const { return m_Channel->value().y(); }
	virtual void setChannel(DataChannelBase::CRefT &channel) { m_Channel = channel; }
};

CSP_XML_BEGIN(InputVectorYChannel)
CSP_XML_END


/** An input channel that provides access to the "Z" component of a data
 *  channel vector.
 */
class InputVectorZChannel: public InputChannel {
	DataChannel<Vector3>::CRefT m_Channel;
public:
	CSP_DECLARE_OBJECT(InputVectorZChannel)
protected:
	virtual double getChannelValue() const { return m_Channel->value().z(); }
	virtual void setChannel(DataChannelBase::CRefT &channel) { m_Channel = channel; }
};

CSP_XML_BEGIN(InputVectorZChannel)
CSP_XML_END


/** An input channel that provides access to a scalar data channel.
 */
class InputScalarChannel: public InputChannel {
	DataChannel<double>::CRefT m_Channel;
public:
	CSP_DECLARE_OBJECT(InputScalarChannel)
protected:
	virtual double getChannelValue() const { return m_Channel->value(); }
	virtual void setChannel(DataChannelBase::CRefT &channel) { m_Channel = channel; }
};

CSP_XML_BEGIN(InputScalarChannel)
CSP_XML_END


/** A channel for writing (scalar) values to the system bus.
 *
 *  XML interface:
 *    channel: The (string) name of the output data channel.
 */
class OutputChannel: public Junction1 {
	std::string m_Name;
	DataChannel<double>::RefT m_Channel;
public:
	CSP_DECLARE_OBJECT(OutputChannel)
	OutputChannel() {}
	typedef Ref<OutputChannel> RefT;
	void connect(Bus *bus) {
		m_Channel = bus->registerLocalDataChannel(m_Name, 0.0);
	}
	void bindRecorder(DataRecorder *recorder) const {
		if (recorder) {
			CSPLOG(INFO, APP) << "Binding channel to recorder " << m_Channel->getName();
			recorder->addSource(m_Channel);
		}
	}
private:
	virtual void evaluate(Timer const &timer) {
		setOutput(getInput(timer), timer);
		m_Channel->value() = getOutput();
	}
};

CSP_XML_BEGIN(OutputChannel)
	CSP_DEF("channel", m_Name, true)
CSP_XML_END


/** A simple comparator junction that outputs the greater of the two
 *  input values.
 */
class Greater: public Junction2 {
public:
	CSP_DECLARE_OBJECT(Greater)
private:
	virtual void evaluate(Timer const &timer) {
		setOutput(std::max(getInputA(timer),getInputB(timer)), timer);
	}
};

CSP_XML_BEGIN(Greater)
CSP_XML_END


/** A simple comparator junction that outputs the lesser of the two
 *  input values.
 */
class Lesser: public Junction2 {
public:
	CSP_DECLARE_OBJECT(Lesser)
private:
	virtual void evaluate(Timer const &timer) {
		setOutput(std::min(getInputA(timer),getInputB(timer)), timer);
	}
};

CSP_XML_BEGIN(Lesser)
CSP_XML_END


/**
 * A general-purpose, programmable flight control system.  The flight control
 * rules are defined by a network of control nodes.  The network includes
 * input nodes that read values from the vehicle system bus, and output nodes
 * that write values to the bus.  The output values are generated by
 * periodically re-evaluating the control node network.
 */
class FlightControlSystem: public System {
	typedef Link<ControlNode>::vector ControlNodeList;
	typedef std::vector<ControlNode::RefT> OutputList;
	ControlNodeList m_ControlNodes;
	OutputList m_Outputs;
	ControlNode::Timer m_Timer;

public:
	CSP_DECLARE_OBJECT(FlightControlSystem)

	FlightControlSystem(): System() {}

protected:

	/**
	 * postCreate constructs the network from the control node list defined
	 * in xml.  Each control node is identified by a unique key (a string in
	 * XML), and nodes may reference other nodes by id.  Those direct links
	 * between nodes are created here.
	 */
	void postCreate() {
		ControlNode::MapID idmap;
		ControlNodeList::iterator node;
		// build id to node map
		for (node = m_ControlNodes.begin(); node != m_ControlNodes.end(); ++node) {
			idmap[(*node)->getID()] = *node;
		}
		// link nodes together based on id
		for (node = m_ControlNodes.begin(); node != m_ControlNodes.end(); ++node) {
			(*node)->link(idmap);
		}
	}

	/**
	 * Connect output nodes to the system bus.  Any other system on the bus
	 * can read the output values.  The data channel names are defined in
	 * the xml specification of the output nodes.
	 */
	void registerChannels(Bus *bus) {
		ControlNodeList::iterator node;
		Ref<OutputChannel> output;
		for (node = m_ControlNodes.begin(); node != m_ControlNodes.end(); ++node) {
			if (output.tryAssign(*node)) {
				output->connect(bus);
				m_Outputs.push_back(*node);
			}
		}
	}

	/**
	 * Connect input nodes to the system bus.  The data channel names are
	 * defined in the xml specification of the input nodes.  Other systems
	 * on the bus must export (and update) these channels.
	 */
	void importChannels(Bus *bus) {
		ControlNodeList::iterator node;
		Ref<InputChannel> input;
		for (node = m_ControlNodes.begin(); node != m_ControlNodes.end(); ++node) {
			(*node)->importChannels(bus);
			/*if (input.tryAssign(*node)) {
				input->connect(bus);
			}*/
		}
	}

	/** A helper functor for advancing a control node to the next time step.
	 */
	class ControlNodeUpdateOp {
		ControlNode::Timer const &_timer;
	public:
		ControlNodeUpdateOp(ControlNode::Timer const &timer): _timer(timer) {}
		void operator()(ControlNode::RefT &node) { node->step(_timer); }
	};

	/** Synchronous update callback for the system.  Advances the flight
	 *  control network by the elapsed time since the last update.
	 */
	double onUpdate(double dt) {
		m_Timer.set(dt);
		std::for_each(m_Outputs.begin(), m_Outputs.end(), ControlNodeUpdateOp(m_Timer));
		return 0.0;  // request immediate callback
	}

public:

	/** Attach a data recorder to the flight control system.  Records all
	 *  output channels.
	 */
	virtual void bindRecorder(DataRecorder *recorder) const {
		if (recorder == 0) return;
		OutputList::const_iterator iter;
		for (iter=m_Outputs.begin(); iter!=m_Outputs.end(); ++iter) {
			(*iter)->bindRecorder(recorder);
			/*
			Ref<OutputChannel const> output;
			if (output.tryAssign(*iter)) {
				output->bindRecorder(recorder);
			}
			*/
		}
	}

};

CSP_XML_BEGIN(FlightControlSystem)
	CSP_DEF("nodes", m_ControlNodes, true)
CSP_XML_END


//} // namespace fcsnode

void registerFlightControlSystemObjects() {
	{ static ControlNode::__csp_interface_proxy instance; }
	{ static Junction1::__csp_interface_proxy instance; }
	{ static Junction2::__csp_interface_proxy instance; }
	{ static Junction3::__csp_interface_proxy instance; }
	{ static LagFilter::__csp_interface_proxy instance; }
	{ static LeadLagFilter::__csp_interface_proxy instance; }
	{ static LeadFilter::__csp_interface_proxy instance; }
	{ static Integrator::__csp_interface_proxy instance; }
	{ static Multiply::__csp_interface_proxy instance; }
	{ static Divide::__csp_interface_proxy instance; }
	{ static Adder::__csp_interface_proxy instance; }
	{ static Adder3::__csp_interface_proxy instance; }
	{ static Scale::__csp_interface_proxy instance; }
	{ static Schedule1::__csp_interface_proxy instance; }
	{ static Schedule2::__csp_interface_proxy instance; }
	{ static Switch::__csp_interface_proxy instance; }
	{ static BooleanSwitch::__csp_interface_proxy instance; }
	{ static Constant::__csp_interface_proxy instance; }
	{ static InputChannel::__csp_interface_proxy instance; }
	{ static InputVectorXChannel::__csp_interface_proxy instance; }
	{ static InputVectorYChannel::__csp_interface_proxy instance; }
	{ static InputVectorZChannel::__csp_interface_proxy instance; }
	{ static InputScalarChannel::__csp_interface_proxy instance; }
	{ static OutputChannel::__csp_interface_proxy instance; }
	{ static Greater::__csp_interface_proxy instance; }
	{ static Lesser::__csp_interface_proxy instance; }
	{ static FlightControlSystem::__csp_interface_proxy instance; }
}

} // namespace csp

