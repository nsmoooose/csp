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
 * @file ControlNode.h
 *
 **/

#ifndef __CONTROL_NODE_H__
#define __CONTROL_NODE_H__

#include <SimData/InterfaceRegistry.h>
#include <SimData/Key.h>
#include <SimData/Math.h>
#include <map>

class Bus;
class DataRecorder;


/** Abstract base class for nodes in a flight control network.
 *  Nodes form a directed graph, progressing from input nodes
 *  that read values from the system bus to output nodes that
 *  write values to the system bus.  Intermediate nodes perform
 *  specialized functions such as filtering and limiting, and
 *  may be connected in arbitrarily complex ways (including
 *  cycles).
 *
 *  Each node is identified by a unique key, and the connections
 *  between nodes and control parameters for each node are
 *  specified through the simdata XML interface.
 *
 *  ControlNode XML interface:
 *    id: A unique key used to link nodes.
 *    clamp_lo: The minimum value of the node output.
 *    clamp_hi: The maximum value of the node output.
 *    rate_limit_dec: The maximum rate of decrease, per second.
 *    rate_limit_inc: The maximum rate of increase, per second.
 */
class ControlNode: public simdata::Object {

public:
	/** Helper class for synchronizing updates across a flight control
	 *  system network.  Each update is assigned a sequential count and
	 *  an elapsed time since the last update.  Network updates proceed
	 *  backwards from outputs to inputs, and during the update a given
	 *  node may be evaluated multiple times.  The count stored in the
	 *  timer object is used to ensure that each node is updated only
	 *  once (when it is first evaluated).
	 */
	class Timer {
		double _dt;
		int _count;
	public:
		Timer(): _dt(0.0), _count(0) { }
		double dt() const { return _dt; }
		int count() const { return _count; }
		void set(double dt_) {
			_dt = dt_;
			++_count;
		}
	};

public:
	typedef simdata::Ref<ControlNode> Ref;
	typedef simdata::Key TypeID;
	typedef std::map<TypeID, Ref> MapID;

	BEGIN_SIMDATA_XML_VIRTUAL_INTERFACE(ControlNode)
		SIMDATA_XML("id", ControlNode::m_ID, true)
		SIMDATA_XML("clamp_lo", ControlNode::m_ClampLo, false)
		SIMDATA_XML("clamp_hi", ControlNode::m_ClampHi, false)
		SIMDATA_XML("rate_limit_dec", ControlNode::m_RateLimitDec, false)
		SIMDATA_XML("rate_limit_inc", ControlNode::m_RateLimitInc, false)
	END_SIMDATA_XML_INTERFACE

	ControlNode():
		m_Output0(0.0),
		m_ClampLo(-1e+30),
		m_ClampHi(1e+30),
		m_RateLimitDec(-1e+30),
		m_RateLimitInc(1e+30),
		m_Count(0)
	{
	}

	/** An internal method used for setting up the control node network.  Allows
	 *  a node to bind to other nodes based on id references.
	 */
	virtual void link(MapID &) {}

	/** Get the identifier of this control node.
	 */
	TypeID const &getID() const { return m_ID; }

	/** Get the output value without updates.  In almost all cases you should
	 *  call step(timer) instead.
	 */
	double getOutput() const { return m_Output0; }

	/** Get the current value of the node, forcing an update if the node has not
	 *  yet been evaluated in this time step.  Each node tracks the count in the
	 *  timer so that it will evaluate itself at most once per time step.
	 */
	double step(Timer const &timer) {
		if (update(timer)) evaluate(timer);
		return getOutput();
	}

	// Standard bus/system methods that can be implemented by subclasses.
	virtual void registerChannels(Bus*) { }
	virtual void importChannels(Bus*) { }
	virtual void bindRecorder(DataRecorder*) { }

protected:
	/** A helper method allowing subclasses to set an initial output value.
	 */
	void setInitialOutput(double output) { m_Output0 = clamp(output); }

	/** Subclasses should call this method from evaluate to set the next output
	 *  value.  Handles output clamping and rate limiting.
	 */
	void setOutput(double output, Timer const &timer) {
		output = clamp(output);
		double delta = output - m_Output0;
		if (delta > m_RateLimitInc * timer.dt()) {
			assert(m_RateLimitInc > 0.0);
			output = m_Output0 + m_RateLimitInc * timer.dt();
		} else
		if (delta < m_RateLimitDec * timer.dt()) {
			assert(m_RateLimitDec < 0.0);
			output = m_Output0 + m_RateLimitDec * timer.dt();
		}
		m_Output0 = output;
	}

	/** Convenience method for clamping a value to within the specified output limits
	 *  of this control node.
	 */
	double clamp(double value) {
		return simdata::clampTo(value, m_ClampLo, m_ClampHi);
	}

private:
	/** Subclasses must implement this method to generate output values.  This method
	 *  will be called at most once per time step.  The output value should be set
	 *  using setOutput().
	 */
	virtual void evaluate(Timer const &)=0;

	/** A helper method for determining whether the node has already been updated
	 *  in this time step.
	 */
	bool update(Timer const &timer) {
		if (m_Count < timer.count()) {
			m_Count = timer.count();
			return true;
		}
		return false;
	}

	double m_Output0;
	double m_ClampLo;
	double m_ClampHi;
	double m_RateLimitDec;
	double m_RateLimitInc;
	int m_Count;
	simdata::Key m_ID;

};

#endif // __CONTROL_NODE_H__

