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

#include <ControlNode.h>
#include <System.h>
#include <DataRecorder.h>

#include <SimData/LUT.h>

#include <cstdlib>
#include <vector>


// FIXME circuits can create circular references.  need to add an "unlink"
// function to zero all references, and call that for each node when the
// fcs system is destroyed (otherwise we leak memory).


// FIXME namespaces disabled to simplify simdata xml interface declarations.
// should consider reenabling them once simdata has better support for namespaces.

//namespace fcsnode {

/** An abstract node that defines a single input channel.
 */
class Junction1: public ControlNode {
	ControlNode::Ref m_Input;
	TypeID m_InputID;
	double m_Gain, m_Offset;
public:
	EXTEND_SIMDATA_XML_VIRTUAL_INTERFACE(Junction1, ControlNode)
		SIMDATA_XML("input", Junction1::m_InputID, true)
		SIMDATA_XML("gain", Junction1::m_Gain, false)
		SIMDATA_XML("offset", Junction1::m_Offset, false)
	END_SIMDATA_XML_INTERFACE
	Junction1(): m_Gain(1.0), m_Offset(0.0) {}
protected:
	void link(MapID &map) {
		m_Input = map[m_InputID];
		if (!m_Input) {
			CSP_LOG(APP, ERROR, "Cannot find input channel for FCS junction node " << getID());
			assert(0); // TODO make fatal
		}
		assert(m_Input.valid()); }
	double getInput(Timer const &dt) { return m_Input->step(dt) * m_Gain + m_Offset; }
};


/** An abstract node that defines two input channels.
 */
class Junction2: public ControlNode {
	ControlNode::Ref m_InputA;
	ControlNode::Ref m_InputB;
	TypeID m_InputIDA;
	TypeID m_InputIDB;
	double m_GainA, m_GainB, m_OffsetA, m_OffsetB;
public:
	EXTEND_SIMDATA_XML_VIRTUAL_INTERFACE(Junction2, ControlNode)
		SIMDATA_XML("input_a", Junction2::m_InputIDA, true)
		SIMDATA_XML("gain_a", Junction2::m_GainA, false)
		SIMDATA_XML("offset_a", Junction2::m_OffsetA, false)
		SIMDATA_XML("input_b", Junction2::m_InputIDB, true)
		SIMDATA_XML("gain_b", Junction2::m_GainB, false)
		SIMDATA_XML("offset_b", Junction2::m_OffsetB, false)
	END_SIMDATA_XML_INTERFACE
	Junction2(): m_GainA(1.0), m_GainB(1.0), m_OffsetA(0.0), m_OffsetB(0.0) {}
protected:
	void link(MapID &map) {
		m_InputA = map[m_InputIDA];
		m_InputB = map[m_InputIDB];
		if (!m_InputA) {
			CSP_LOG(APP, ERROR, "Cannot find input channel A for FCS Junction2 node " << getID());
			assert(0); // TODO make fatal
		}
		if (!m_InputB) {
			CSP_LOG(APP, ERROR, "Cannot find input channel B for FCS Junction2 node " << getID());
			assert(0); // TODO make fatal
		}
	}
	double getInputA(Timer const &dt) { return m_InputA->step(dt) * m_GainA + m_OffsetA; }
	double getInputB(Timer const &dt) { return m_InputB->step(dt) * m_GainB + m_OffsetB; }
};


/** An abstract node that defines three input channels.
 */
class Junction3: public ControlNode {
	ControlNode::Ref m_InputA;
	ControlNode::Ref m_InputB;
	ControlNode::Ref m_InputC;
	TypeID m_InputIDA;
	TypeID m_InputIDB;
	TypeID m_InputIDC;
	double m_GainA, m_GainB, m_GainC, m_OffsetA, m_OffsetB, m_OffsetC;
public:
	EXTEND_SIMDATA_XML_VIRTUAL_INTERFACE(Junction3, ControlNode)
		SIMDATA_XML("input_a", Junction3::m_InputIDA, true)
		SIMDATA_XML("gain_a", Junction3::m_GainA, false)
		SIMDATA_XML("offset_a", Junction3::m_OffsetA, false)
		SIMDATA_XML("input_b", Junction3::m_InputIDB, true)
		SIMDATA_XML("gain_b", Junction3::m_GainB, false)
		SIMDATA_XML("offset_b", Junction3::m_OffsetB, false)
		SIMDATA_XML("input_c", Junction3::m_InputIDC, true)
		SIMDATA_XML("gain_c", Junction3::m_GainC, false)
		SIMDATA_XML("offset_c", Junction3::m_OffsetC, false)
	END_SIMDATA_XML_INTERFACE
	Junction3(): m_GainA(1.0), m_GainB(1.0), m_GainC(1.0), m_OffsetA(0.0), m_OffsetB(0.0), m_OffsetC(0.0) { }
protected:
	void link(MapID &map) {
		m_InputA = map[m_InputIDA];
		m_InputB = map[m_InputIDB];
		m_InputC = map[m_InputIDC];
		if (!m_InputA) {
			CSP_LOG(APP, ERROR, "Cannot find input channel A for FCS Junction3 node " << getID());
			assert(0); // TODO make fatal
		}
		if (!m_InputB) {
			CSP_LOG(APP, ERROR, "Cannot find input channel B for FCS Junction3 node " << getID());
			assert(0); // TODO make fatal
		}
		if (!m_InputC) {
			CSP_LOG(APP, ERROR, "Cannot find input channel C for FCS Junction3 node " << getID());
			assert(0); // TODO make fatal
		}
	}
	double getInputA(Timer const &dt) { return m_InputA->step(dt) * m_GainA + m_OffsetA; }
	double getInputB(Timer const &dt) { return m_InputB->step(dt) * m_GainB + m_OffsetB; }
	double getInputC(Timer const &dt) { return m_InputC->step(dt) * m_GainC + m_OffsetC; }
};

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
	SIMDATA_OBJECT(LagFilter, 0, 0)
	EXTEND_SIMDATA_XML_INTERFACE(LagFilter, Junction1)
		SIMDATA_XML("a", LagFilter::m_Chi, true)
	END_SIMDATA_XML_INTERFACE
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


/** Lead-lag filter:  H(s) = c * (s + a) / (s + b)
 *  NB: no frequency prewarping is applied in generating the IIR digital filter.
 */
class LeadLagFilter: public Junction1 {
	double m_A, m_B, m_C;
	double m_Input0;
public:
	SIMDATA_OBJECT(LeadLagFilter, 0, 0)
	EXTEND_SIMDATA_XML_INTERFACE(LeadLagFilter, Junction1)
		SIMDATA_XML("a", LeadLagFilter::m_A, true)
		SIMDATA_XML("b", LeadLagFilter::m_B, true)
		SIMDATA_XML("c", LeadLagFilter::m_C, true)
	END_SIMDATA_XML_INTERFACE
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


/** A simple high-pass filter: H(s) = s / (s + a)
 */
class LeadFilter: public Junction1 {
	double m_A;
	double m_Input0;
public:
	SIMDATA_OBJECT(LeadFilter, 0, 0)
	EXTEND_SIMDATA_XML_INTERFACE(LeadFilter, Junction1)
		SIMDATA_XML("a", LeadFilter::m_A, true)
	END_SIMDATA_XML_INTERFACE
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


/* A node for integrating an input channel with respect to time.  Equivalent
 * to a laplace transform H(s) = 1 / s.
 */
class Integrator: public Junction1 {
public:
	SIMDATA_OBJECT(Integrator, 0, 0)
	EXTEND_SIMDATA_XML_INTERFACE(Integrator, Junction1)
	END_SIMDATA_XML_INTERFACE
	Integrator() { }
private:
	void evaluate(Timer const &timer) {
		setOutput(getOutput() + timer.dt() * getInput(timer), timer);
	}
};


/** A node for multiplying (mixing) to inputs.
 */
class Multiply: public Junction2 {
public:
	SIMDATA_OBJECT(Multiply, 0, 0)
	EXTEND_SIMDATA_XML_INTERFACE(Multiply, Junction2)
	END_SIMDATA_XML_INTERFACE
private:
	virtual void evaluate(Timer const &timer) {
		setOutput(getInputA(timer) * getInputB(timer), timer);
	}
};


/** A node for dividing two inputs.  Caution: can result in infinities or
 *  NaN's.
 */
class Divide: public Junction2 {
public:
	SIMDATA_OBJECT(Divide, 0, 0)
	EXTEND_SIMDATA_XML_INTERFACE(Divide, Junction2)
	END_SIMDATA_XML_INTERFACE
private:
	virtual void evaluate(Timer const &timer) {
		setOutput(getInputA(timer) / getInputB(timer), timer);
	}
};


/** A node for adding two inputs with independent gains.
 */
class Adder: public Junction2 {
public:
	SIMDATA_OBJECT(Adder, 0, 0)
	EXTEND_SIMDATA_XML_INTERFACE(Adder, Junction2)
	END_SIMDATA_XML_INTERFACE
private:
	virtual void evaluate(Timer const &timer) {
		setOutput(getInputA(timer) + getInputB(timer), timer);
	}
};


/** A node for adding three inputs with independent gains.
 */
class Adder3: public Junction3 {
public:
	SIMDATA_OBJECT(Adder3, 0, 0)
	EXTEND_SIMDATA_XML_INTERFACE(Adder3, Junction3)
	END_SIMDATA_XML_INTERFACE
private:
	virtual void evaluate(Timer const &timer) {
		setOutput(getInputA(timer) + getInputB(timer) + getInputC(timer), timer);
	}
};


/** A single input node for appling an offset and a gain.
 */
class Scale: public Junction1 {
public:
	SIMDATA_OBJECT(Scale, 0, 0)
	EXTEND_SIMDATA_XML_INTERFACE(Scale, Junction1)
	END_SIMDATA_XML_INTERFACE
private:
	virtual void evaluate(Timer const &timer) {
		setOutput(getInput(timer), timer);
	}
};


/** A programmable schedule.  The output is the scheduled value of
 *  the input channel.
 *
 *  XML interface:
 *    schedule: A 1D lookup table indexed by the input channel.
 */
class Schedule1: public Junction1 {
	simdata::Table1 m_Schedule;
public:
	SIMDATA_OBJECT(Schedule1, 0, 0)
	EXTEND_SIMDATA_XML_INTERFACE(Schedule1, Junction1)
		SIMDATA_XML("schedule", Schedule1::m_Schedule, true)
	END_SIMDATA_XML_INTERFACE
	Schedule1() {}
private:
	virtual void evaluate(Timer const &timer) {
		setOutput(m_Schedule[getInput(timer)], timer);
	}
};


/** A two channel programmable schedule.  The output is the product of
 *  input channel A and the scheduled value of input channel B.
 *
 *  XML interface:
 *    schedule: A 1D lookup table indexed by input channel B.
 */
class Schedule2: public Junction2 {
	simdata::Table1 m_Schedule;
public:
	SIMDATA_OBJECT(Schedule2, 0, 0)
	EXTEND_SIMDATA_XML_INTERFACE(Schedule2, Junction2)
		SIMDATA_XML("schedule", Schedule2::m_Schedule, true)
	END_SIMDATA_XML_INTERFACE
	Schedule2() {}
private:
	virtual void evaluate(Timer const &timer) {
		setOutput(getInputA(timer) * m_Schedule[getInputB(timer)], timer);
	}
};


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
	SIMDATA_OBJECT(Switch, 0, 0)
	EXTEND_SIMDATA_XML_INTERFACE(Switch, Junction3)
		SIMDATA_XML("compare", Switch::m_Compare, true)
	END_SIMDATA_XML_INTERFACE
private:
	virtual void evaluate(Timer const &timer) {
		if (getInputC(timer) < m_Compare) {
			setOutput(getInputA(timer), timer);
		} else {
			setOutput(getInputB(timer), timer);
		}
	}
};


/** A three channel junction that passes either input channel A or
 *  input channel B depending whether input channel C is true or
 *  false.
 *
 *  XML interface:
 *    input_c: The name of a boolean channel that is used to select
 *      input channel A (true) or input channel B (false).
 */
class BooleanSwitch: public Junction2 {
	DataChannel<bool>::CRef m_InputC;
	std::string m_SwitchChannel;
public:
	SIMDATA_OBJECT(BooleanSwitch, 0, 0)
	EXTEND_SIMDATA_XML_INTERFACE(BooleanSwitch, Junction2)
		SIMDATA_XML("channel", BooleanSwitch::m_SwitchChannel, true)
	END_SIMDATA_XML_INTERFACE
	virtual void importChannels(Bus *bus) {
		Junction2::importChannels(bus);
		m_InputC = bus->getChannel(m_SwitchChannel);
	}
private:
	virtual void evaluate(Timer const &timer) {
		setOutput(m_InputC->value() ? getInputA(timer) : getInputB(timer), timer);
	}
};


/** A simple control node that provides a constant output value.  The
 *  output value is specified by the XML interface.
 */
class Constant: public ControlNode {
	double m_Value;
public:
	SIMDATA_OBJECT(Constant, 0, 0)
	EXTEND_SIMDATA_XML_INTERFACE(Constant, ControlNode)
		SIMDATA_XML("value", Constant::m_Value, true)
	END_SIMDATA_XML_INTERFACE
	Constant(): m_Value(0.0) {}
protected:
	void postCreate() {
		setInitialOutput(m_Value);
	}

private:
	virtual void evaluate(Timer const &) { }
};


/** An abstract control node that provides access to values on the system
 *  bus.  Concrete subclasses provide access to specific data channel types.
 *
 *  XML interface:
 *    channel: The (string) name of the system bus channel.
 */
class InputChannel: public ControlNode {
	std::string m_Name;
public:
	EXTEND_SIMDATA_XML_VIRTUAL_INTERFACE(InputChannel, ControlNode)
		SIMDATA_XML("channel", InputChannel::m_Name, true)
	END_SIMDATA_XML_INTERFACE
	InputChannel() { }
	virtual void importChannels(Bus *bus) {
		DataChannelBase::CRef channel;
		channel = bus->getChannel(m_Name);
		setChannel(channel);
	}
protected:
	virtual double getChannelValue() const = 0;
	virtual void setChannel(DataChannelBase::CRef &channel)=0;
private:
	virtual void evaluate(Timer const &timer) {
		setOutput(getChannelValue(), timer);
	}
};


/** An input channel that provides access to the "X" component of a data
 *  channel vector.
 */
class InputVectorXChannel: public InputChannel {
	DataChannel<simdata::Vector3>::CRef m_Channel;
public:
	SIMDATA_OBJECT(InputVectorXChannel, 0, 0)
	EXTEND_SIMDATA_XML_INTERFACE(InputVectorXChannel, InputChannel)
	END_SIMDATA_XML_INTERFACE
protected:
	virtual double getChannelValue() const { return m_Channel->value().x(); }
	virtual void setChannel(DataChannelBase::CRef &channel) { m_Channel = channel; }
};


/** An input channel that provides access to the "Y" component of a data
 *  channel vector.
 */
class InputVectorYChannel: public InputChannel {
	DataChannel<simdata::Vector3>::CRef m_Channel;
public:
	SIMDATA_OBJECT(InputVectorYChannel, 0, 0)
	EXTEND_SIMDATA_XML_INTERFACE(InputVectorYChannel, InputChannel)
	END_SIMDATA_XML_INTERFACE
protected:
	virtual double getChannelValue() const { return m_Channel->value().y(); }
	virtual void setChannel(DataChannelBase::CRef &channel) { m_Channel = channel; }
};


/** An input channel that provides access to the "Z" component of a data
 *  channel vector.
 */
class InputVectorZChannel: public InputChannel {
	DataChannel<simdata::Vector3>::CRef m_Channel;
public:
	SIMDATA_OBJECT(InputVectorZChannel, 0, 0)
	EXTEND_SIMDATA_XML_INTERFACE(InputVectorZChannel, InputChannel)
	END_SIMDATA_XML_INTERFACE
protected:
	virtual double getChannelValue() const { return m_Channel->value().z(); }
	virtual void setChannel(DataChannelBase::CRef &channel) { m_Channel = channel; }
};


/** An input channel that provides access to a scalar data channel.
 */
class InputScalarChannel: public InputChannel {
	DataChannel<double>::CRef m_Channel;
public:
	SIMDATA_OBJECT(InputScalarChannel, 0, 0)
	EXTEND_SIMDATA_XML_INTERFACE(InputScalarChannel, InputChannel)
	END_SIMDATA_XML_INTERFACE
protected:
	virtual double getChannelValue() const { return m_Channel->value(); }
	virtual void setChannel(DataChannelBase::CRef &channel) { m_Channel = channel; }
};


/** A channel for writing (scalar) values to the system bus.
 *
 *  XML interface:
 *    channel: The (string) name of the output data channel.
 */
class OutputChannel: public Junction1 {
	std::string m_Name;
	DataChannel<double>::Ref m_Channel;
public:
	SIMDATA_OBJECT(OutputChannel, 0, 0)
	EXTEND_SIMDATA_XML_INTERFACE(OutputChannel, Junction1)
		SIMDATA_XML("channel", OutputChannel::m_Name, true)
	END_SIMDATA_XML_INTERFACE
	OutputChannel() {}
	typedef simdata::Ref<OutputChannel> Ref;
	void connect(Bus *bus) {
		m_Channel = bus->registerLocalDataChannel(m_Name, 0.0);
	}
	void bindRecorder(DataRecorder *recorder) const {
		if (recorder) {
			CSP_LOG(APP, INFO, "Binding channel to recorder " << m_Channel->getName());
			recorder->addSource(m_Channel);
		}
	}
private:
	virtual void evaluate(Timer const &timer) {
		setOutput(getInput(timer), timer);
		m_Channel->value() = getOutput();
	}
};


/** A simple comparator junction that outputs the greater of the two
 *  input values.
 */
class Greater: public Junction2 {
public:
	SIMDATA_OBJECT(Greater, 0, 0)
	EXTEND_SIMDATA_XML_INTERFACE(Greater, Junction2)
	END_SIMDATA_XML_INTERFACE
private:
	virtual void evaluate(Timer const &timer) {
		setOutput(std::max(getInputA(timer),getInputB(timer)), timer);
	}
};


/** A simple comparator junction that outputs the lesser of the two
 *  input values.
 */
class Lesser: public Junction2 {
public:
	SIMDATA_OBJECT(Lesser, 0, 0)
	EXTEND_SIMDATA_XML_INTERFACE(Lesser, Junction2)
	END_SIMDATA_XML_INTERFACE
private:
	virtual void evaluate(Timer const &timer) {
		setOutput(std::min(getInputA(timer),getInputB(timer)), timer);
	}
};


/**
 * A general-purpose, programmable flight control system.  The flight control
 * rules are defined by a network of control nodes.  The network includes
 * input nodes that read values from the vehicle system bus, and output nodes
 * that write values to the bus.  The output values are generated by
 * periodically re-evaluating the control node network.
 */
class FlightControlSystem: public System {
	typedef simdata::Link<ControlNode>::vector ControlNodeList;
	typedef std::vector<ControlNode::Ref> OutputList;
	ControlNodeList m_ControlNodes;
	OutputList m_Outputs;
	ControlNode::Timer m_Timer;

public:
	SIMDATA_OBJECT(FlightControlSystem, 0, 0)
	BEGIN_SIMDATA_XML_INTERFACE(FlightControlSystem)
		SIMDATA_XML("nodes", FlightControlSystem::m_ControlNodes, true)
	END_SIMDATA_XML_INTERFACE

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
		simdata::Ref<OutputChannel> output;
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
		simdata::Ref<InputChannel> input;
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
		void operator()(ControlNode::Ref &node) { node->step(_timer); }
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
			simdata::Ref<OutputChannel const> output;
			if (output.tryAssign(*iter)) {
				output->bindRecorder(recorder);
			}
			*/
		}
	}

};

//
SIMDATA_REGISTER_INTERFACE(FlightControlSystem)
//
SIMDATA_REGISTER_INTERFACE(ControlNode)
SIMDATA_REGISTER_INTERFACE(Junction1)
SIMDATA_REGISTER_INTERFACE(Junction2)
SIMDATA_REGISTER_INTERFACE(Junction3)
SIMDATA_REGISTER_INTERFACE(LeadFilter)
SIMDATA_REGISTER_INTERFACE(LagFilter)
SIMDATA_REGISTER_INTERFACE(LeadLagFilter)
SIMDATA_REGISTER_INTERFACE(Integrator)
SIMDATA_REGISTER_INTERFACE(Greater)
SIMDATA_REGISTER_INTERFACE(Lesser)
SIMDATA_REGISTER_INTERFACE(Adder)
SIMDATA_REGISTER_INTERFACE(Adder3)
SIMDATA_REGISTER_INTERFACE(InputChannel)
SIMDATA_REGISTER_INTERFACE(InputVectorXChannel)
SIMDATA_REGISTER_INTERFACE(InputVectorYChannel)
SIMDATA_REGISTER_INTERFACE(InputVectorZChannel)
SIMDATA_REGISTER_INTERFACE(InputScalarChannel)
SIMDATA_REGISTER_INTERFACE(Schedule1)
SIMDATA_REGISTER_INTERFACE(Schedule2)
SIMDATA_REGISTER_INTERFACE(Switch)
SIMDATA_REGISTER_INTERFACE(BooleanSwitch)
SIMDATA_REGISTER_INTERFACE(Scale)
SIMDATA_REGISTER_INTERFACE(Constant)
SIMDATA_REGISTER_INTERFACE(Multiply)
SIMDATA_REGISTER_INTERFACE(Divide)
SIMDATA_REGISTER_INTERFACE(OutputChannel)

//} // namespace fcsnode
