#pragma once
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
 * @file FuelManagementSystem.h
 *
 **/

#include <csp/cspsim/System.h>
#include <csp/cspsim/SystemsModel.h>
#include <csp/csplib/data/Key.h>
#include <csp/csplib/data/Object.h>
#include <csp/csplib/data/ObjectInterface.h>
#include <csp/csplib/data/Real.h>

namespace csp {

class FuelTank;
class FuelManagementSystem;
class StoresManagementSystem;


class FuelNode: public Object {
friend class FuelManagementSystem;

public:
	typedef std::map<Key, FuelNode*> NodeMap;

public:
	CSP_DECLARE_ABSTRACT_OBJECT(FuelNode)

	FuelNode(): m_Blocked(false) { }

	Key const &key() const { return m_Key; }

	virtual void addFuel(double /*amount*/) { }

	virtual double capacity() const=0;
	virtual double quantity() const=0;
	virtual double space() const=0;
	virtual double fuelMass() const=0;

	virtual std::string name() const { return "?"; }

	virtual double refuel(double /*dt*/, double /*amount*/) { assert(0); return 0.0; }
	virtual double drawFuel(double /*dt*/, double /*amount*/) { assert(0); return 0.0; }

	virtual double limitIn(double /*dt*/) const { return 0.0; }
	virtual double limitOut(double /*dt*/) const { return 0.0; }

	virtual void update(double dt, bool pressurize) {
		updatePressure(dt, pressurize);
		redistribute(dt);
		for (unsigned i = 0; i < m_Children.size(); ++i) m_Children[i]->update(dt, pressurize);
	}

	void block() { m_Blocked = true; }
	void unblock() { m_Blocked = false; }
	bool blocked() const { return m_Blocked; }

protected:
	std::vector<FuelNode*> m_Children;
	std::vector<FuelNode*> m_Parents;

private:
	virtual void bindStores(StoresManagementSystem*) { }

	void bindChildren(NodeMap const &node_map);
	void addParent(FuelNode *node) { m_Parents.push_back(node); }

	virtual void redistribute(double /*dt*/) { }
	virtual void updatePressure(double /*dt*/, bool /*pressurize*/) { }

	Key m_Key;
	std::vector<Key> m_ChildKeys;
	bool m_Blocked;
};


class FuelTankNode: public FuelNode {
public:
	CSP_DECLARE_OBJECT(FuelTankNode)

	FuelTankNode();
	virtual ~FuelTankNode();

	virtual double capacity() const;
	virtual double quantity() const;
	virtual double space() const { return capacity() - quantity(); }
	virtual double fuelMass() const;

	double pressurizationRate() const { return m_PressurizationRate.value(); }
	double depressurizationRate() const { return m_DepressurizationRate.value(); }
	double inflowRate() const { return m_InflowRate.value(); }
	double outflowRate() const { return m_OutflowRate.value(); }

	virtual std::string name() const { return m_TankId; }

protected:
	virtual void redistribute(double dt);
	virtual void addFuel(double amount);

	void updatePressure(double dt, bool pressurize) {
		if (m_Tank.valid()) {
			double delta = pressurize ? std::min(0.2, 1.0 - m_Pressure) : std::max(-0.2, -m_Pressure);
			double rate = pressurize ? m_PressurizationRate : m_DepressurizationRate;
			m_Pressure = clampTo(m_Pressure + delta * rate * dt, 0.0, 1.0);
		} else {
			m_Pressure = 0.0;
		}
	}

	double refuel(double dt, double amount) {
		double added = std::min(amount, limitIn(dt));
		addFuel(added);
		for (unsigned i = 0; (added < amount) && (i < m_Children.size()); ++i) {
			added += m_Children[i]->refuel(dt, amount - added);
		}
		return added;
	}

	virtual double drawFuel(double dt, double amount) {
		amount = std::min(amount, limitOut(dt));
		addFuel(-amount);
		return amount;
	}

	virtual double limitIn(double dt) const {
		if (!m_Tank) return 0.0;
		double limit = m_InflowRate * dt;
		if (m_NeedsPressure) {
			limit *= clampTo(1.0 - m_Pressure, 0.0, 1.0);
		}
		return std::min(limit, space());
	}

	virtual double limitOut(double dt) const {
		if (!m_Tank || blocked()) return 0.0;
		double limit = m_OutflowRate * dt;
		if (m_NeedsPressure) {
			limit *= clampTo(m_Pressure, 0.0, 1.0);
		}
		return std::min(limit, quantity());
	}

private:
	virtual void bindStores(StoresManagementSystem* sms);

	std::string m_TankId;
	Ref<FuelTank> m_Tank;
	double m_Pressure;
	bool m_NeedsPressure;
	Real m_PressurizationRate;
	Real m_DepressurizationRate;
	Real m_InflowRate;
	Real m_OutflowRate;
};



/** System for managing fuel stored in multiple fuel tanks.  This class provides very
 *  basic infrastructure for implementing a fuel system with multiple interconnected
 *  fuel tanks and pumps.  Tanks are organized into a tree, with fuel drawn from the
 *  root of the tree.  Specialized behavior must be implemented in subclasses.  The
 *  FuelManagementSystem auto-exports itself onto the system bus.
 */
class FuelManagementSystem: public System, public sigc::trackable {
public:
	CSP_DECLARE_ABSTRACT_OBJECT(FuelManagementSystem)

	static const char *Channel;

	FuelNode* getNode(Key const &key) {
		FuelNode::NodeMap::const_iterator iter = m_NodeMap.find(key);
		return (iter == m_NodeMap.end()) ? 0 : iter->second;
	}

	virtual double onUpdate(double dt)=0;
	virtual double drawFuel(double dt, double amount)=0;
	virtual double refuel(double dt, double amount, bool /*ground*/)=0;

	virtual void getInfo(InfoList &info) const;

protected:
	virtual void postCreate();
	virtual void registerChannels(Bus *);
	virtual void importChannels(Bus *);

private:
	void initialize();
	void rebind();

	Link<FuelNode>::vector m_FuelNodes;
	FuelNode::NodeMap m_NodeMap;
};

} // namespace csp
