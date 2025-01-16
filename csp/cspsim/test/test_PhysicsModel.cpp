// Combat Simulator Project
// Copyright (C) 2007 The Combat Simulator Project
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

#include <csp/cspsim/BaseDynamics.h>
#include <csp/cspsim/KineticsChannels.h>
#include <csp/cspsim/PhysicsModel.h>
#include <csp/cspsim/SystemsModel.h>
#include <csp/csplib/util/Testing.h>

using namespace csp;
using namespace csp::bus;
using namespace csp::test;

// A simplified version of DynamicObject that register kinematic
// channels on the system bus.
class MockObject: public Referenced {
public:
	MockObject() {}

	void bind(SystemsModel *sys) {
		m_SystemsModel = sys;
		Bus *bus = sys->getBus();
		b_Position = DataChannel<Vector3>::newLocal(Kinetics::Position, Vector3::ZERO);
		b_ModelPosition = DataChannel<Vector3>::newLocal(Kinetics::ModelPosition, Vector3::ZERO);
		b_Mass = DataChannel<double>::newLocal(Kinetics::Mass, 1.0);
		b_Inertia = DataChannel<Matrix3>::newLocal(Kinetics::Inertia, Matrix3::IDENTITY);
		b_InertiaInv = DataChannel<Matrix3>::newLocal(Kinetics::InertiaInverse, Matrix3::IDENTITY);
		b_AngularVelocity = DataChannel<Vector3>::newLocal(Kinetics::AngularVelocity, Vector3::ZERO);
		b_AngularVelocityBody = DataChannel<Vector3>::newLocal(Kinetics::AngularVelocityBody, Vector3::ZERO);
		b_Velocity = DataChannel<Vector3>::newLocal(Kinetics::Velocity, Vector3::ZERO);
		b_AccelerationBody = DataChannel<Vector3>::newLocal(Kinetics::AccelerationBody, Vector3::ZERO);
		b_Attitude = DataChannel<Quat>::newLocal(Kinetics::Attitude, Quat::IDENTITY);
		b_CenterOfMassOffset = DataChannel<Vector3>::newLocal(Kinetics::CenterOfMassOffset, Vector3::ZERO);

		bus->registerChannel(b_Position.get());
		bus->registerChannel(b_ModelPosition.get());
		bus->registerChannel(b_Velocity.get());
		bus->registerChannel(b_AccelerationBody.get());
		bus->registerChannel(b_AngularVelocity.get());
		bus->registerChannel(b_AngularVelocityBody.get());
		bus->registerChannel(b_Attitude.get());
		bus->registerChannel(b_Mass.get());
		bus->registerChannel(b_Inertia.get());
		bus->registerChannel(b_InertiaInv.get());
		bus->registerChannel(b_CenterOfMassOffset.get());
	}

	Vector3 const& position() const { return b_Position->value(); }
	Vector3 const& velocity() const { return b_Velocity->value(); }
	Vector3 const& angularVelocity() const { return b_AngularVelocity->value(); }
	Vector3 const& angularVelocityBody() const { return b_AngularVelocityBody->value(); }
	Quat const& attitude() const { return b_Attitude->value(); }

	Vector3 xaxis() const { return b_Attitude->value().rotate(Vector3::XAXIS); }
	Vector3 yaxis() const { return b_Attitude->value().rotate(Vector3::YAXIS); }
	Vector3 zaxis() const { return b_Attitude->value().rotate(Vector3::ZAXIS); }

	void setPosition(Vector3 const& p) { b_Position->value() = p; }
	void setVelocity(Vector3 const& v) { b_Velocity->value() = v; }
	void setAngularVelocity(Vector3 const& w) {
		b_AngularVelocity->value() = w;
		b_AngularVelocityBody->value() = b_Attitude->value().invrotate(w);
	}
	void setAttitude(Quat const& q) { b_Attitude->value() = q; }
	void setMass(double mass) { b_Mass->value() = mass; }
	void setInertia(Matrix3 const& I) {
		b_Inertia->value().set(I);
		b_InertiaInv->value().invert(I);
	}

	void dump(std::ostream& os) const {
		os << " pos: " << position();
		os << " vel: " << velocity();
		os << " ang: " << angularVelocity();
		os << " att: " << attitude();
	}

private:
	DataChannel<Vector3>::RefT b_ModelPosition;
	DataChannel<Vector3>::RefT b_Position;  // cm position
	DataChannel<Vector3>::RefT b_Velocity;
	DataChannel<double>::RefT b_Mass;
	DataChannel<Matrix3>::RefT b_Inertia;
	DataChannel<Matrix3>::RefT b_InertiaInv;
	DataChannel<Vector3>::RefT b_AngularVelocity;
	DataChannel<Vector3>::RefT b_AngularVelocityBody;
	DataChannel<Vector3>::RefT b_AccelerationBody;
	DataChannel<Vector3>::RefT b_CenterOfMassOffset;
	DataChannel<Quat>::RefT b_Attitude;
	Ref<SystemsModel> m_SystemsModel;
};


std::ostream& operator<<(std::ostream& os, MockObject const& obj) { return obj.dump(os), os; }


// A dynamics function that supplies a fixed torque in body coordinates.
class FixedTorque: public BaseDynamics {
public:
	FixedTorque(Vector3 const& torque) { m_Moment = torque; }
private:
	virtual void registerChannels(Bus*) {}
	virtual void importChannels(Bus*) {}
	virtual void computeForceAndMoment(double) { }
};

// A dynamics function that supplies a fixed force in body coordinates.
class FixedForce: public BaseDynamics {
public:
	FixedForce(Vector3 const& force) { m_Force = force; }
private:
	virtual void registerChannels(Bus*) {}
	virtual void importChannels(Bus*) {}
	virtual void computeForceAndMoment(double) { }
};


// Compute the magnitude of the difference of two vectors or quaternions.
template <typename T>
double vdiff(T const &a, T const &b) {
	return (a - b).length();
}

CSP_TESTFIXTURE(PhysicsModel) {
public:
	void setup() {
		physics = new PhysicsModel;
		systems = new SystemsModel;
		object = new MockObject;
		object->bind(systems.get());
		systems->addChild(physics.get());
		systems->bindSystems();

		// turn off gravity and pseudoforces by default.  these
		// are explicitly enabled by specific tests belawe.
		physics->enableGravity(false);
		physics->enablePseudoForces(false);
	}

protected:
	CSP_TESTCASE(SkewRotation) {
		object->setAttitude(Quat(0.0, 0.0, sqrt(0.5), sqrt(0.5)));
		object->setAngularVelocity(Vector3(1, 0, 0));

		CSPLOG(DEBUG, PHYSICS) << "initial: " << *object;

		// PI seconds
		for (int i = 0; i < 1000; ++i) {
			physics->doSimStep(M_PI / 1000.0);
			if ((i % 10) == 0) {
				CSPLOG(DEBUG, PHYSICS) << i << "(" << (i * 180.0 / 1000) << "): " << *object;
			}
		}

		CSPLOG(DEBUG, PHYSICS) << "final: " << *object;

		CSP_EXPECT_EQ(0.0, vdiff(object->position(), Vector3::ZERO));
		CSP_EXPECT_EQ(0.0, vdiff(object->velocity(), Vector3::ZERO));
		CSP_EXPECT_GT(0.0001, vdiff(object->angularVelocity(), Vector3(1.0, 0.0, 0.0)));
		CSP_EXPECT_GT(0.0001, vdiff(object->xaxis(), -Vector3::YAXIS));
	}

	CSP_TESTCASE(SimpleRotation) {
		object->setVelocity(Vector3(1, 0, 0));
		object->setAngularVelocity(Vector3(0, 0, 1));

		CSPLOG(DEBUG, PHYSICS) << "initial: " << *object;

		// 10 seconds @ 100 Hz
		for (int i = 0; i < 1000; ++i) {
			physics->doSimStep(0.01);
			if ((i % 10) == 0) {
				CSPLOG(DEBUG, PHYSICS) << i << "(" << (i * 0.01) << "): " << *object;
			}
		}

		CSPLOG(DEBUG, PHYSICS) << "final: " << *object;

		CSP_EXPECT_GT(0.0001, vdiff(object->position(), Vector3(10.0, 0.0, 0.0)));
		CSP_EXPECT_GT(0.0001, vdiff(object->velocity(), Vector3(1.0, 0.0, 0.0)));
		CSP_EXPECT_GT(0.0001, vdiff(object->angularVelocity(), Vector3(0.0, 0.0, 1.0)));
		CSP_EXPECT_GT(0.0001, vdiff(object->attitude(), Quat(0, 0, -0.958924, 0.283662)));
	}

	CSP_TESTCASE(DumbellRotation) {
		object->setInertia(Matrix3(0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 1.0));
		object->setAngularVelocity(Vector3(1, 1, 0).normalized());

		CSPLOG(DEBUG, PHYSICS) << "initial: " << *object;

		// PI seconds @ 100 Hz
		for (int i = 0; i < 1000; ++i) {
			physics->doSimStep(M_PI / 1000.0);
			if ((i % 10) == 0) {
				CSPLOG(DEBUG, PHYSICS) << i << "(" << (i * 180.0 * 0.001) << "): " << *object;
			}
		}

		CSPLOG(DEBUG, PHYSICS) << "final: " << *object;

		CSP_EXPECT_GT(0.0001, vdiff(object->position(), Vector3::ZERO));
		CSP_EXPECT_GT(0.0001, vdiff(object->velocity(), Vector3::ZERO));
		CSP_EXPECT_GT(0.0001, vdiff(object->angularVelocity(), Vector3(0.0, sqrt(0.5), 0.0)));
		CSP_EXPECT_GT(0.0001, vdiff(object->attitude(), Quat(0, 0.896019, 0, 0.444016)));
	}

	CSP_TESTCASE(DumbellForcedRotation) {
		physics->addDynamics(new FixedTorque(Vector3(0.0, 0.0, 0.45)));

		object->setInertia(Matrix3(0.1, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 1.0));
		object->setAngularVelocity(Vector3(1, 1, 0).normalized());

		CSPLOG(DEBUG, PHYSICS) << "initial: " << *object;

		// PI seconds @ 100 Hz
		for (int i = 0; i < 1000; ++i) {
			physics->doSimStep(M_PI / 1000.0);
			if ((i % 10) == 0) {
				CSPLOG(DEBUG, PHYSICS) << i << "(" << (i * 180.0 * 0.001) << "): " << *object;
			}
		}

		CSPLOG(DEBUG, PHYSICS) << "final: " << *object;

		CSP_EXPECT_GT(0.0001, vdiff(object->position(), Vector3::ZERO));
		CSP_EXPECT_GT(0.0001, vdiff(object->velocity(), Vector3::ZERO));
		CSP_EXPECT_GT(0.0001, vdiff(object->angularVelocity(), Vector3(1.0, 1.0, 0.0).normalized()));
		CSP_EXPECT_GT(0.0001, vdiff(object->xaxis(), Vector3(0.0, 1.0, 0.0)));
	}

	CSP_TESTCASE(LinearAcceleration) {
		physics->addDynamics(new FixedForce(Vector3(0.0, 0.0, 1.0)));

		object->setInertia(Matrix3(0.1, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 1.0));
		object->setPosition(Vector3(1, 1, 1));
		object->setVelocity(Vector3(2, 0, 0));

		CSPLOG(DEBUG, PHYSICS) << "initial: " << *object;

		// 10 seconds @ 100 Hz
		for (int i = 0; i < 1000; ++i) {
			physics->doSimStep(10.0 / 1000.0);
			if ((i % 10) == 0) {
				CSPLOG(DEBUG, PHYSICS) << i << "(" << (i * 10.0 * 0.001) << "): " << *object;
			}
		}

		CSPLOG(DEBUG, PHYSICS) << "final: " << *object;

		CSP_EXPECT_GT(0.0001, vdiff(object->position(), Vector3(21.0, 1.0, 51)));
		CSP_EXPECT_GT(0.0001, vdiff(object->velocity(), Vector3(2, 0, 10)));
		CSP_EXPECT_GT(0.0001, vdiff(object->angularVelocity(), Vector3::ZERO));
		CSP_EXPECT_GT(0.0001, vdiff(object->xaxis(), Vector3::XAXIS));
	}

	CSP_TESTCASE(RotatingForce) {
		physics->addDynamics(new FixedForce(Vector3(-24.0, 0.0, 0.0)));

		object->setMass(3.0);
		object->setInertia(Matrix3(0.1, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 1.0));
		object->setPosition(Vector3(0.5, 0, 0));
		object->setVelocity(Vector3(0, 2, 0));
		object->setAngularVelocity(Vector3(0, 0, 4));

		CSPLOG(DEBUG, PHYSICS) << "initial: " << *object;

		// 5/32 PI seconds @ 100 Hz
		for (int i = 0; i < 1000; ++i) {
			//physics->doSimStep(5 * M_PI / 32 / 1000.0);
			physics->doSimStep(5 * M_PI / 16 / 1000.0);
			if ((i % 10) == 0) {
				CSPLOG(DEBUG, PHYSICS) << i << "(" << (i * 225.0 * 0.001) << "): " << *object;
			}
		}

		CSPLOG(DEBUG, PHYSICS) << "final: " << *object;

		CSP_EXPECT_GT(0.0001, vdiff(object->position(), 0.5 * Vector3(-1, -1, 0).normalized()));
		CSP_EXPECT_GT(0.0001, vdiff(object->velocity(), 2.0 * Vector3(1, -1, 0).normalized()));
		CSP_EXPECT_GT(0.0001, vdiff(object->angularVelocity(), Vector3(0, 0, 4)));
		CSP_EXPECT_GT(0.0001, vdiff(object->xaxis(), Vector3(-1, -1, 0).normalized()));
	}

	CSP_TESTCASE(SpinningPlate) {
		// I1 = 1, I2 = 2*I1, I3 = I1 + I2
		object->setInertia(Matrix3(1.0, 0.0, 0.0, 0.0, 2.0, 0.0, 0.0, 0.0, 3.0));
		object->setAngularVelocity(Vector3(sqrt(3.0), 0, 1));  // 30 degrees (a) from the plane, |w|=2

		// with no forces and torques, w_body_y(t) should be |w| cos(a) tanh(|w| t sin(a)).
		// a = 30 degrees, so cos(a) = sqrt(3)/2 and sin(a) = 1/2.  the expected result is
		// thus sqrt(3) * tanh(t).

		// 5 seconds @ 100 Hz.  Check w_body_y after each step.
		const double dt = 5.0 / 1000.0;
		double t = 0.0;
		for (int i = 0; i < 1000; ++i) {
			physics->doSimStep(dt); t += dt;
			CSP_ENSURE_GT(0.0001, fabs(object->angularVelocityBody().y() - sqrt(3.0) * tanh(t)));
		}
	}

	CSP_TESTCASE(Gravity) {
		physics->enableGravity(true);
		object->setAngularVelocity(Vector3(1, 2, 3));
		object->setPosition(Vector3(-10, -20, -30));
		for (int i = 0; i < 1000; ++i) physics->doSimStep(0.01);
		CSP_EXPECT_DEQ(-10, object->position().x());
		CSP_EXPECT_DEQ(-20, object->position().y());
		CSP_EXPECT_DEQ(-30 - 0.5 * 9.8 * (10*10), object->position().z());
		CSP_EXPECT_DEQ(0, object->velocity().x());
		CSP_EXPECT_DEQ(0, object->velocity().y());
		CSP_EXPECT_DEQ(-9.8 * 10, object->velocity().z());
		CSP_EXPECT_DEQ(0, vdiff(object->angularVelocity(), Vector3(1, 2, 3)));
	}

	CSP_TESTCASE(ForceFreeTop) {
		// force free motion of a symmetric top, with I3 / I12 = sqrt(7) / 2.
		object->setInertia(Matrix3(1.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.5 * sqrt(7.0)));
		// w(0) is 30 degrees from z, in the x-z plane.
		object->setAngularVelocity(Vector3(0.5, 0, 1) * sqrt(5.0));
		// w should precess about z with angular frequency omega, and |w|
		// should remain constant.
		double omega = (0.5 * sqrt(7.0) - 1.0) * sqrt(5.0);

		const double dt = 5.0 / 1000.0;
		double t = 0.0;
		for (int i = 0; i < 1000; ++i) {
			physics->doSimStep(dt); t += dt;
			Vector3 w = object->angularVelocityBody();
			double phase = atan2(w.y(), w.x());
			CSP_ENSURE_GT(0.00001, fabs(fmod(phase - omega * t, 2*PI)));
			CSP_ENSURE_GT(0.00001, fabs(w.z() - sqrt(5.0)));
		}
	}

	CSP_TESTCASE(ScratchSpin) {
		object->setInertia(Matrix3(1, 0, 0, 0, 1, 0, 0, 0, 1));
		object->setAngularVelocity(Vector3(0, 0, 1));

		// force-free spin, with decreasing moment of inertia.  angular
		// momentum should decrease as well, since PhysicsModel treats all
		// inertia changes as external (i.e., as a result of shedding or
		// gaining mass as opposed to internal reconfigurations of mass).  this
		// is appropriate, for example, to the release of external stores, but
		// not correct for mass reconfigurations such as fuel redistribution or
		// geometry changes.  the latter are effects tend to be small compared
		// to typical forces, and are thus neglected in the simulation.

		const double dt = 5.0 / 1000.0;
		double t = 0.0;
		for (int i = 0; i < 1000; ++i) {
			double Iz = 1.0 / (1.0 + t);  // Iz reduction over time.
			object->setInertia(Matrix3(1, 0, 0, 0, 1, 0, 0, 0, Iz));
			physics->doSimStep(dt); t += dt;
			CSP_ENSURE_GT(0.00001, vdiff(object->angularVelocity(), Vector3(0, 0, 1)));
		}
	}

private:
	Ref<PhysicsModel> physics;
	Ref<SystemsModel> systems;
	Ref<MockObject> object;
};

