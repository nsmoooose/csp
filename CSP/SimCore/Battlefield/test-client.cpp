#include <SimCore/Battlefield/LocalBattlefield.h>
#include <SimNet/ClientServer.h>
#include <SimNet/NetLog.h>
#include <SimData/Timing.h>
#include <SimData/Trace.h>
#include <SimData/InterfaceRegistry.h>
#include <SimData/DataManager.h>
#include <SimData/DataArchive.h>
#include <SimData/Quat.h>

simdata::Vector3 MIRROR_OFFSET(30.0, 0, 0);

class Unit: public SimObject {
	static TestUnitState::Ref mirror_msg;
public:
	SIMDATA_OBJECT(Unit, 0, 0)
	BEGIN_SIMDATA_XML_INTERFACE(Unit)
	END_SIMDATA_XML_INTERFACE

	Unit(): SimObject(SimObject::TYPE_AIR_UNIT), m_Position(-30000, -10000, 2000.0), m_Velocity(10, 0, 0) {
	}
	virtual simdata::Vector3 getGlobalPosition() const {
		return m_Position;
	}
	simdata::Vector3 m_Position;
	simdata::Vector3 m_Velocity;
	virtual double onUpdate(double dt) {
		if (mirror_msg.valid() && isLocal()) {
			m_Position = mirror_msg->position();
			m_Velocity = mirror_msg->velocity();
		}
		//m_Position = m_Position + m_Velocity * dt;
		//m_Velocity = m_Velocity + ((m_Velocity ^ simdata::Vector3::ZAXIS) * dt * 0.02);
		if (MIRROR_OFFSET.x() > 0) {
			MIRROR_OFFSET += simdata::Vector3(5*dt, 0, 0);
		} else {
			MIRROR_OFFSET -= simdata::Vector3(5*dt, 0, 0);
		}
		return 0.0;
	}
	virtual simdata::Ref<simnet::NetworkMessage> getState(simcore::TimeStamp current_time, simdata::SimTime /*interval*/, int /*detail*/) const {
		if (mirror_msg.valid()) return mirror_msg;
		TestUnitState::Ref msg = new TestUnitState;
		msg->set_timestamp(current_time);
		msg->set_position(m_Position);
		msg->set_velocity(m_Velocity);
		msg->set_attitude(simdata::Quat::IDENTITY);
		msg->set_test_object(true);
		return msg;
	}
	virtual void setState(simdata::Ref<simnet::NetworkMessage> const &msg, simcore::TimeStamp /*now*/) {
		//if (!simnet::NetworkMessage::FastCast<TestUnitState>(msg)->has_test_object()) {
		if (msg->getSource() == 100) {
			mirror_msg = msg;
			m_Position = mirror_msg->position();
			m_Velocity = mirror_msg->velocity();
			mirror_msg->set_position(mirror_msg->position() + MIRROR_OFFSET);
		}
		/*
		TestUnitState::Ref state = msg;
		m_Position = state->position();
		m_Velocity = state->velocity();
		CSP_LOG(APP, INFO, "state updated: " << m_Position << ", " << m_Velocity);
		*/
	}
	virtual simdata::Path getObjectPath() const {
		return simdata::Path("sim:vehicles.aircraft.m2k");
	}
};

TestUnitState::Ref Unit::mirror_msg;
SIMDATA_REGISTER_INTERFACE(Unit);

void makeDataArchive() {
	simdata::Ref<Unit> unit = new Unit;
	simdata::DataArchive dar("test.dar", false /*read*/);
	dar.addObject(*unit, "sim:vehicles.aircraft.m2k");
	dar.finalize();
}


int main(int argc, char **argv) {
	int port = 4000;
	if (argc == 2) {
		port = atoi(argv[1]);
	}
	if (port & 1) MIRROR_OFFSET = -MIRROR_OFFSET;
	std::cout << "mirror offset " << MIRROR_OFFSET << "\n";
	simdata::Trace::install();
	simdata::log().setLogPriority(2);
	simnet::netlog().setLogPriority(3);
	csplog().setLogPriority(2);

	simdata::log().setLogPriority(6);
	simnet::netlog().setLogPriority(6);
	csplog().setLogPriority(5);
	csplog().setLogCategory(CSP_BATTLEFIELD);

	CSP_LOG(APP, INFO, "recreating data archive");
	makeDataArchive();

	CSP_LOG(APP, INFO, "opening data archive");
	simdata::Ref<simdata::DataManager> data_manager = new simdata::DataManager();
	data_manager->addArchive(new simdata::DataArchive("test.dar", true /*read*/));

	CSP_LOG(APP, INFO, "creating battlefield");
	//simdata::Ref<LocalBattlefield> bf = new LocalBattlefield(0);
	simdata::Ref<LocalBattlefield> bf = new LocalBattlefield(data_manager);

	CSP_LOG(APP, INFO, "creating network client");
	simdata::Ref<simnet::Client> client = new simnet::Client(simnet::NetworkNode("127.0.0.1", port), 30000, 30000);

	CSP_LOG(APP, INFO, "connecting to server");
	if (client->connectToServer(simnet::NetworkNode("127.0.0.1", 3160), 5.0)) {
		bf->setNetworkClient(client);
		bf->connectToServer("onsight");
		simdata::Timer timer;
		timer.start();
		bool added = false;
		while (1) {
			client->processAndWait(0.01, 0.01, 0.1);
			bf->update(timer.elapsed());
			timer.reset();
			if (!added && bf->isConnectionActive()) {
				added = true;
				simdata::Ref<Unit> unit = data_manager->getObject("sim:vehicles.aircraft.m2k");
				assert(unit.valid());
				bf->__test__addLocalHumanUnit(unit);
			}
		}
	}
	return 0;
}
