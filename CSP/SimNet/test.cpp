// Combat Simulator Project - CSPSim
// Copyright (C) 2004 The Combat Simulator Project
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


#include <SimNet/NetworkInterface.h>
#include <SimNet/PacketDecoder.h>
#include <SimNet/MessageQueue.h>
#include <SimNet/NetworkNode.h>
#include <SimNet/TestMessage.h>
#include <SimData/Timing.h>
#include <SimData/Thread.h>
#include <SimData/Trace.h>


using namespace simnet;

// XXX XXX XXX XXX
void hackBuildMessageIndex() {
	simdata::TaggedRecordRegistry const &registry = simdata::TaggedRecordRegistry::getInstance();
	std::vector<simdata::TaggedRecordFactoryBase *> factories = registry.getFactories();
	for (int i = 0; i < factories.size(); ++i) {
		factories[i]->setCustomId(i);
	}
}

bool read_last = false;
bool read_first = false;
double start_read = 0.0;
double end_read = 0.0;
int recvcnt = 0;


class MessageDump: public MessageHandler {
public:
	virtual void handleMessage(NetworkMessage::Ref msg) {
		if (!read_first) {
			start_read = simdata::get_realtime();
			read_first = true;
			std::cout << "read first!\n";
		}
		if (msg->getId() != TestMessage::_getId()) {
		} else {
			TestMessage::Ref tm = NetworkMessage::FastCast<TestMessage>(msg);
			if (!read_last && !tm->has_helo()) {
				read_last = true;
				end_read = simdata::get_realtime();
				std::cout << recvcnt << " message recv'd (done)\n";
			} else {
				/*
				if (tm->number() != recvcnt)  {
					std::cout << "expected " << recvcnt << ", got " << tm->number() << "\n";
					assert(0);
				}
				*/
				if (recvcnt % 100 == 0) { std::cout << recvcnt << "\n"; }
			}
		}
		++recvcnt;
		//if ((recvcnt % 1000) == 0) std::cout << recvcnt << " message recv'd\n";
		//msg->dump(std::cout);
		//std::cout << "-------\n";
	}
};


int main(int argc, char** argv) {

	simdata::Trace::install();

	if (argc != 2) {
		std::cout << "must specify local id\n";
		return 1;
	}

	hackBuildMessageIndex();

	int local_id = atoi(argv[1]);
	int bunch = 100 + local_id * 50;
	NetworkNode local("127.0.0.1", 4567 + local_id);

	std::cout << "creating interface\n";
	NetworkInterface::Ref ni = new NetworkInterface();

	ni->initialize(local);
	ni->setServerId(local_id);

	for (int id = 1; id < 4; ++id) {
		if (id == local_id) continue;
		NetworkNode node("127.0.0.1", 4567 + id);
		ni->hackPeerIndex(id, node, 256000, 256000);
	}

	std::cout << "adding handlers\n";
	simdata::Ref<MessageQueue> mq = new MessageQueue();
	simdata::Ref<PacketDecoder> pd = new PacketDecoder();
	pd->addMessageHandler(new MessageDump());
	ni->addPacketHandler(pd);
	ni->setPacketSource(mq);

	double start = simdata::get_realtime();
	double start_send;
	double end_send;

	bool send_start = false;
	bool send_first = false;
	bool send_last = false;
	bool send_exit = false;

	int send_count = 0;
	int send_done = 0;

	if (local_id == 1) {
		send_exit = true;
		std::cout << "receive only\n";
	} else {
		send_start = true;
		std::cout << "send and receive\n";
	}

	std::cout << "starting loop " << local_id << "\n";

	while (true) {

		double tt0, tt1;
		if (send_start) {
			if (!send_first) {
				send_first = true;
				std::cout << "starting to send\n";
			}
			tt0 = simdata::get_realtime();
			for (int i=0; i<bunch && !send_last; ++i) {
				simdata::Ref<TestMessage> msg = new TestMessage();
				msg->setDestination(1);
				msg->setPriority(2);
				msg->set_number(send_count);
				msg->set_helo("HELOXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX");
				mq->queueMessage(msg);
				if (send_count % 1000 == 0) std::cout << send_count << "\n";
				if (++send_count >= 100000) {
					send_last = true;
					end_send = simdata::get_realtime();
					std::cout << "send complete (" << send_count << ")\n";
					break;
				}
			}
			tt1 = simdata::get_realtime();
			if (tt1 - tt0 > 0.01) std::cout << "queueing " << ((tt1-tt0)*1000.0) << "ms!\n";

			if (send_last && !send_exit) {
				if (send_done < 10) {
					simdata::Ref<TestMessage> msg = new TestMessage();
					msg->setPriority(3);
					msg->setDestination(1);
					msg->set_number(0);
					mq->queueMessage(msg);
					send_done++;
				} else {
					send_exit = true;
				}
			}

		}

		tt0 = simdata::get_realtime();
		ni->processOutgoing(0.003);
		tt1 = simdata::get_realtime();
		if (tt1 - tt0 > 0.005) std::cout << "outgoing " << ((tt1-tt0)*1000.0) << "ms!\n";

		tt0 = simdata::get_realtime();
		ni->processIncoming(0.003);
		tt1 = simdata::get_realtime();
		//if (tt1 - tt0 > 0.005) std::cout << "incoming " << ((tt1-tt0)*1000.0) << "ms!\n";

		tt0 = simdata::get_realtime();
		simdata::thread::sleep(0.01);
		tt1 = simdata::get_realtime();
		//if (tt1 - tt0 > 0.02) std::cout << "slept for " << ((tt1-tt0)*1000.0) << "ms!\n";

		if (send_exit && read_last) break;
	}

	std::cout << "done! (" << ((end_send - start_send)*1e+6) << " us write, " << ((end_read - start_read)*1e+6) << " us read)\n";

	// a little extra loop time to allow us to confirm the 'done' messages
	for (int i = 0; i < 400; ++i) {
		ni->processOutgoing(0.003);
		ni->processIncoming(0.003);
		simdata::thread::sleep(0.01);
	}

	return 0;
}


