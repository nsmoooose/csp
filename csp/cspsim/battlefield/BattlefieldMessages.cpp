// Generated on Sun Jun 20 00:11:47 2010
// This is a generated file --- DO NOT EDIT!

#include <csp/cspsim/battlefield/BattlefieldMessages.h>
#include <csp/csplib/net/TaggedRecordRegistry.h>


int JoinRequest::m_CustomId = 64;
namespace { csp::TaggedRecordFactory<JoinRequest> __JoinRequest_factory; }

void JoinRequest::serialize(csp::TagWriter &writer) const {
	writer.beginCompound();
	if (has_user_name()) {
		writer.writeTag(TAG_user_name);
		writer.writer << m_user_name;
	}
	if (has_internal_ip_addr()) {
		writer.writeTag(TAG_internal_ip_addr);
		writer.writer << m_internal_ip_addr;
	}
	if (has_external_ip_addr()) {
		writer.writeTag(TAG_external_ip_addr);
		writer.writer << m_external_ip_addr;
	}
	if (has_local_time()) {
		writer.writeTag(TAG_local_time);
		writer.writer << m_local_time;
	}
	writer.endCompound();
}

void JoinRequest::serialize(csp::TagReader &reader) {
	reader.beginCompound();
	for (int tag = reader.nextTag(); tag != 0; tag = reader.nextTag()) {
		switch (tag) {
			case TAG_user_name: {
				reader.reader >> m_user_name;
				m_has0 |= 0x00000001;
				break;
			}
			case TAG_internal_ip_addr: {
				reader.reader >> m_internal_ip_addr;
				m_has0 |= 0x00000002;
				break;
			}
			case TAG_external_ip_addr: {
				reader.reader >> m_external_ip_addr;
				m_has0 |= 0x00000004;
				break;
			}
			case TAG_local_time: {
				reader.reader >> m_local_time;
				m_has0 |= 0x00000008;
				break;
			}
			default: break;
		}
	}
	reader.endCompound();
}

void JoinRequest::dump(std::ostream &, int) const {}
void JoinRequest::dump(csp::DumpWriter &out, const char *name) const {
	if (name) {
		out.line() << name << ": JoinRequest {";
	} else {
		out.line() << "JoinRequest {";
	}
	out.indent();
	if (has_user_name()) {
		out.line() << "user_name: " << m_user_name;
	}
	if (has_internal_ip_addr()) {
		out.line() << "internal_ip_addr: " << m_internal_ip_addr;
	}
	if (has_external_ip_addr()) {
		out.line() << "external_ip_addr: " << m_external_ip_addr;
	}
	if (has_local_time()) {
		out.line() << "local_time: " << m_local_time;
	}
	out.dedent();
	out.line() << "}";
}


int JoinResponse::m_CustomId = 65;
namespace { csp::TaggedRecordFactory<JoinResponse> __JoinResponse_factory; }

void JoinResponse::serialize(csp::TagWriter &writer) const {
	writer.beginCompound();
	if (has_success()) {
		writer.writeTag(TAG_success);
		writer.writer << m_success;
	}
	if (has_details()) {
		writer.writeTag(TAG_details);
		writer.writer << m_details;
	}
	if (has_first_id()) {
		writer.writeTag(TAG_first_id);
		writer.writer << m_first_id;
	}
	if (has_id_count()) {
		writer.writeTag(TAG_id_count);
		writer.writer << m_id_count;
	}
	writer.endCompound();
}

void JoinResponse::serialize(csp::TagReader &reader) {
	reader.beginCompound();
	for (int tag = reader.nextTag(); tag != 0; tag = reader.nextTag()) {
		switch (tag) {
			case TAG_success: {
				reader.reader >> m_success;
				m_has0 |= 0x00000001;
				break;
			}
			case TAG_details: {
				reader.reader >> m_details;
				m_has0 |= 0x00000002;
				break;
			}
			case TAG_first_id: {
				reader.reader >> m_first_id;
				m_has0 |= 0x00000004;
				break;
			}
			case TAG_id_count: {
				reader.reader >> m_id_count;
				m_has0 |= 0x00000008;
				break;
			}
			default: break;
		}
	}
	reader.endCompound();
}

void JoinResponse::dump(std::ostream &, int) const {}
void JoinResponse::dump(csp::DumpWriter &out, const char *name) const {
	if (name) {
		out.line() << name << ": JoinResponse {";
	} else {
		out.line() << "JoinResponse {";
	}
	out.indent();
	if (has_success()) {
		out.line() << "success: " << m_success;
	}
	if (has_details()) {
		out.line() << "details: " << m_details;
	}
	if (has_first_id()) {
		out.line() << "first_id: " << m_first_id;
	}
	if (has_id_count()) {
		out.line() << "id_count: " << m_id_count;
	}
	out.dedent();
	out.line() << "}";
}


int PlayerJoin::m_CustomId = 66;
namespace { csp::TaggedRecordFactory<PlayerJoin> __PlayerJoin_factory; }

void PlayerJoin::serialize(csp::TagWriter &writer) const {
	writer.beginCompound();
	if (has_user_name()) {
		writer.writeTag(TAG_user_name);
		writer.writer << m_user_name;
	}
	if (has_peer_id()) {
		writer.writeTag(TAG_peer_id);
		writer.writer << m_peer_id;
	}
	if (has_port()) {
		writer.writeTag(TAG_port);
		writer.writer << m_port;
	}
	if (has_ip_addr()) {
		writer.writeTag(TAG_ip_addr);
		writer.writer << m_ip_addr;
	}
	if (has_incoming_bw()) {
		writer.writeTag(TAG_incoming_bw);
		writer.writer << m_incoming_bw;
	}
	if (has_outgoing_bw()) {
		writer.writeTag(TAG_outgoing_bw);
		writer.writer << m_outgoing_bw;
	}
	writer.endCompound();
}

void PlayerJoin::serialize(csp::TagReader &reader) {
	reader.beginCompound();
	for (int tag = reader.nextTag(); tag != 0; tag = reader.nextTag()) {
		switch (tag) {
			case TAG_user_name: {
				reader.reader >> m_user_name;
				m_has0 |= 0x00000001;
				break;
			}
			case TAG_peer_id: {
				reader.reader >> m_peer_id;
				m_has0 |= 0x00000002;
				break;
			}
			case TAG_port: {
				reader.reader >> m_port;
				m_has0 |= 0x00000004;
				break;
			}
			case TAG_ip_addr: {
				reader.reader >> m_ip_addr;
				m_has0 |= 0x00000008;
				break;
			}
			case TAG_incoming_bw: {
				reader.reader >> m_incoming_bw;
				m_has0 |= 0x00000010;
				break;
			}
			case TAG_outgoing_bw: {
				reader.reader >> m_outgoing_bw;
				m_has0 |= 0x00000020;
				break;
			}
			default: break;
		}
	}
	reader.endCompound();
}

void PlayerJoin::dump(std::ostream &, int) const {}
void PlayerJoin::dump(csp::DumpWriter &out, const char *name) const {
	if (name) {
		out.line() << name << ": PlayerJoin {";
	} else {
		out.line() << "PlayerJoin {";
	}
	out.indent();
	if (has_user_name()) {
		out.line() << "user_name: " << m_user_name;
	}
	if (has_peer_id()) {
		out.line() << "peer_id: " << m_peer_id;
	}
	if (has_port()) {
		out.line() << "port: " << m_port;
	}
	if (has_ip_addr()) {
		out.line() << "ip_addr: " << m_ip_addr;
	}
	if (has_incoming_bw()) {
		out.line() << "incoming_bw: " << m_incoming_bw;
	}
	if (has_outgoing_bw()) {
		out.line() << "outgoing_bw: " << m_outgoing_bw;
	}
	out.dedent();
	out.line() << "}";
}


int PlayerQuit::m_CustomId = 67;
namespace { csp::TaggedRecordFactory<PlayerQuit> __PlayerQuit_factory; }

void PlayerQuit::serialize(csp::TagWriter &writer) const {
	writer.beginCompound();
	if (has_peer_id()) {
		writer.writeTag(TAG_peer_id);
		writer.writer << m_peer_id;
	}
	writer.endCompound();
}

void PlayerQuit::serialize(csp::TagReader &reader) {
	reader.beginCompound();
	for (int tag = reader.nextTag(); tag != 0; tag = reader.nextTag()) {
		switch (tag) {
			case TAG_peer_id: {
				reader.reader >> m_peer_id;
				m_has0 |= 0x00000001;
				break;
			}
			default: break;
		}
	}
	reader.endCompound();
}

void PlayerQuit::dump(std::ostream &, int) const {}
void PlayerQuit::dump(csp::DumpWriter &out, const char *name) const {
	if (name) {
		out.line() << name << ": PlayerQuit {";
	} else {
		out.line() << "PlayerQuit {";
	}
	out.indent();
	if (has_peer_id()) {
		out.line() << "peer_id: " << m_peer_id;
	}
	out.dedent();
	out.line() << "}";
}


int RegisterUnit::m_CustomId = 68;
namespace { csp::TaggedRecordFactory<RegisterUnit> __RegisterUnit_factory; }

void RegisterUnit::serialize(csp::TagWriter &writer) const {
	writer.beginCompound();
	if (has_unit_id()) {
		writer.writeTag(TAG_unit_id);
		writer.writer << m_unit_id;
	}
	if (has_unit_class()) {
		writer.writeTag(TAG_unit_class);
		writer.writer << m_unit_class;
	}
	if (has_unit_type()) {
		writer.writeTag(TAG_unit_type);
		writer.writer << m_unit_type;
	}
	if (has_grid_x()) {
		writer.writeTag(TAG_grid_x);
		writer.writer << m_grid_x;
	}
	if (has_grid_y()) {
		writer.writeTag(TAG_grid_y);
		writer.writer << m_grid_y;
	}
	writer.endCompound();
}

void RegisterUnit::serialize(csp::TagReader &reader) {
	reader.beginCompound();
	for (int tag = reader.nextTag(); tag != 0; tag = reader.nextTag()) {
		switch (tag) {
			case TAG_unit_id: {
				reader.reader >> m_unit_id;
				m_has0 |= 0x00000001;
				break;
			}
			case TAG_unit_class: {
				reader.reader >> m_unit_class;
				m_has0 |= 0x00000002;
				break;
			}
			case TAG_unit_type: {
				reader.reader >> m_unit_type;
				m_has0 |= 0x00000004;
				break;
			}
			case TAG_grid_x: {
				reader.reader >> m_grid_x;
				m_has0 |= 0x00000008;
				break;
			}
			case TAG_grid_y: {
				reader.reader >> m_grid_y;
				m_has0 |= 0x00000010;
				break;
			}
			default: break;
		}
	}
	reader.endCompound();
}

void RegisterUnit::dump(std::ostream &, int) const {}
void RegisterUnit::dump(csp::DumpWriter &out, const char *name) const {
	if (name) {
		out.line() << name << ": RegisterUnit {";
	} else {
		out.line() << "RegisterUnit {";
	}
	out.indent();
	if (has_unit_id()) {
		out.line() << "unit_id: " << m_unit_id;
	}
	if (has_unit_class()) {
		out.line() << "unit_class: " << m_unit_class;
	}
	if (has_unit_type()) {
		out.line() << "unit_type: " << m_unit_type;
	}
	if (has_grid_x()) {
		out.line() << "grid_x: " << m_grid_x;
	}
	if (has_grid_y()) {
		out.line() << "grid_y: " << m_grid_y;
	}
	out.dedent();
	out.line() << "}";
}


int NotifyUnitMotion::m_CustomId = 69;
namespace { csp::TaggedRecordFactory<NotifyUnitMotion> __NotifyUnitMotion_factory; }

void NotifyUnitMotion::serialize(csp::TagWriter &writer) const {
	writer.beginCompound();
	if (has_unit_id()) {
		writer.writeTag(TAG_unit_id);
		writer.writer << m_unit_id;
	}
	if (has_grid_x()) {
		writer.writeTag(TAG_grid_x);
		writer.writer << m_grid_x;
	}
	if (has_grid_y()) {
		writer.writeTag(TAG_grid_y);
		writer.writer << m_grid_y;
	}
	writer.endCompound();
}

void NotifyUnitMotion::serialize(csp::TagReader &reader) {
	reader.beginCompound();
	for (int tag = reader.nextTag(); tag != 0; tag = reader.nextTag()) {
		switch (tag) {
			case TAG_unit_id: {
				reader.reader >> m_unit_id;
				m_has0 |= 0x00000001;
				break;
			}
			case TAG_grid_x: {
				reader.reader >> m_grid_x;
				m_has0 |= 0x00000002;
				break;
			}
			case TAG_grid_y: {
				reader.reader >> m_grid_y;
				m_has0 |= 0x00000004;
				break;
			}
			default: break;
		}
	}
	reader.endCompound();
}

void NotifyUnitMotion::dump(std::ostream &, int) const {}
void NotifyUnitMotion::dump(csp::DumpWriter &out, const char *name) const {
	if (name) {
		out.line() << name << ": NotifyUnitMotion {";
	} else {
		out.line() << "NotifyUnitMotion {";
	}
	out.indent();
	if (has_unit_id()) {
		out.line() << "unit_id: " << m_unit_id;
	}
	if (has_grid_x()) {
		out.line() << "grid_x: " << m_grid_x;
	}
	if (has_grid_y()) {
		out.line() << "grid_y: " << m_grid_y;
	}
	out.dedent();
	out.line() << "}";
}


int CommandUpdatePeer::m_CustomId = 70;
namespace { csp::TaggedRecordFactory<CommandUpdatePeer> __CommandUpdatePeer_factory; }

void CommandUpdatePeer::serialize(csp::TagWriter &writer) const {
	writer.beginCompound();
	if (has_unit_id()) {
		writer.writeTag(TAG_unit_id);
		writer.writer << m_unit_id;
	}
	if (has_peer_id()) {
		writer.writeTag(TAG_peer_id);
		writer.writer << m_peer_id;
	}
	if (has_stop()) {
		writer.writeTag(TAG_stop);
		writer.writer << m_stop;
	}
	writer.endCompound();
}

void CommandUpdatePeer::serialize(csp::TagReader &reader) {
	reader.beginCompound();
	for (int tag = reader.nextTag(); tag != 0; tag = reader.nextTag()) {
		switch (tag) {
			case TAG_unit_id: {
				reader.reader >> m_unit_id;
				m_has0 |= 0x00000001;
				break;
			}
			case TAG_peer_id: {
				reader.reader >> m_peer_id;
				m_has0 |= 0x00000002;
				break;
			}
			case TAG_stop: {
				reader.reader >> m_stop;
				m_has0 |= 0x00000004;
				break;
			}
			default: break;
		}
	}
	reader.endCompound();
}

void CommandUpdatePeer::dump(std::ostream &, int) const {}
void CommandUpdatePeer::dump(csp::DumpWriter &out, const char *name) const {
	if (name) {
		out.line() << name << ": CommandUpdatePeer {";
	} else {
		out.line() << "CommandUpdatePeer {";
	}
	out.indent();
	if (has_unit_id()) {
		out.line() << "unit_id: " << m_unit_id;
	}
	if (has_peer_id()) {
		out.line() << "peer_id: " << m_peer_id;
	}
	if (has_stop()) {
		out.line() << "stop: " << m_stop;
	}
	out.dedent();
	out.line() << "}";
}


int CommandAddUnit::m_CustomId = 71;
namespace { csp::TaggedRecordFactory<CommandAddUnit> __CommandAddUnit_factory; }

void CommandAddUnit::serialize(csp::TagWriter &writer) const {
	writer.beginCompound();
	if (has_unit_id()) {
		writer.writeTag(TAG_unit_id);
		writer.writer << m_unit_id;
	}
	if (has_unit_class()) {
		writer.writeTag(TAG_unit_class);
		writer.writer << m_unit_class;
	}
	if (has_unit_type()) {
		writer.writeTag(TAG_unit_type);
		writer.writer << m_unit_type;
	}
	if (has_owner_id()) {
		writer.writeTag(TAG_owner_id);
		writer.writer << m_owner_id;
	}
	if (has_grid_x()) {
		writer.writeTag(TAG_grid_x);
		writer.writer << m_grid_x;
	}
	if (has_grid_y()) {
		writer.writeTag(TAG_grid_y);
		writer.writer << m_grid_y;
	}
	writer.endCompound();
}

void CommandAddUnit::serialize(csp::TagReader &reader) {
	reader.beginCompound();
	for (int tag = reader.nextTag(); tag != 0; tag = reader.nextTag()) {
		switch (tag) {
			case TAG_unit_id: {
				reader.reader >> m_unit_id;
				m_has0 |= 0x00000001;
				break;
			}
			case TAG_unit_class: {
				reader.reader >> m_unit_class;
				m_has0 |= 0x00000002;
				break;
			}
			case TAG_unit_type: {
				reader.reader >> m_unit_type;
				m_has0 |= 0x00000004;
				break;
			}
			case TAG_owner_id: {
				reader.reader >> m_owner_id;
				m_has0 |= 0x00000008;
				break;
			}
			case TAG_grid_x: {
				reader.reader >> m_grid_x;
				m_has0 |= 0x00000010;
				break;
			}
			case TAG_grid_y: {
				reader.reader >> m_grid_y;
				m_has0 |= 0x00000020;
				break;
			}
			default: break;
		}
	}
	reader.endCompound();
}

void CommandAddUnit::dump(std::ostream &, int) const {}
void CommandAddUnit::dump(csp::DumpWriter &out, const char *name) const {
	if (name) {
		out.line() << name << ": CommandAddUnit {";
	} else {
		out.line() << "CommandAddUnit {";
	}
	out.indent();
	if (has_unit_id()) {
		out.line() << "unit_id: " << m_unit_id;
	}
	if (has_unit_class()) {
		out.line() << "unit_class: " << m_unit_class;
	}
	if (has_unit_type()) {
		out.line() << "unit_type: " << m_unit_type;
	}
	if (has_owner_id()) {
		out.line() << "owner_id: " << m_owner_id;
	}
	if (has_grid_x()) {
		out.line() << "grid_x: " << m_grid_x;
	}
	if (has_grid_y()) {
		out.line() << "grid_y: " << m_grid_y;
	}
	out.dedent();
	out.line() << "}";
}


int CommandRemoveUnit::m_CustomId = 72;
namespace { csp::TaggedRecordFactory<CommandRemoveUnit> __CommandRemoveUnit_factory; }

void CommandRemoveUnit::serialize(csp::TagWriter &writer) const {
	writer.beginCompound();
	if (has_unit_id()) {
		writer.writeTag(TAG_unit_id);
		writer.writer << m_unit_id;
	}
	writer.endCompound();
}

void CommandRemoveUnit::serialize(csp::TagReader &reader) {
	reader.beginCompound();
	for (int tag = reader.nextTag(); tag != 0; tag = reader.nextTag()) {
		switch (tag) {
			case TAG_unit_id: {
				reader.reader >> m_unit_id;
				m_has0 |= 0x00000001;
				break;
			}
			default: break;
		}
	}
	reader.endCompound();
}

void CommandRemoveUnit::dump(std::ostream &, int) const {}
void CommandRemoveUnit::dump(csp::DumpWriter &out, const char *name) const {
	if (name) {
		out.line() << name << ": CommandRemoveUnit {";
	} else {
		out.line() << "CommandRemoveUnit {";
	}
	out.indent();
	if (has_unit_id()) {
		out.line() << "unit_id: " << m_unit_id;
	}
	out.dedent();
	out.line() << "}";
}


int IdAllocationRequest::m_CustomId = 73;
namespace { csp::TaggedRecordFactory<IdAllocationRequest> __IdAllocationRequest_factory; }

void IdAllocationRequest::serialize(csp::TagWriter &writer) const {
	writer.beginCompound();
	writer.endCompound();
}

void IdAllocationRequest::serialize(csp::TagReader &reader) {
	reader.beginCompound();
	reader.endCompound();
}

void IdAllocationRequest::dump(std::ostream &, int) const {}
void IdAllocationRequest::dump(csp::DumpWriter &out, const char *name) const {
	if (name) {
		out.line() << name << ": IdAllocationRequest {";
	} else {
		out.line() << "IdAllocationRequest {";
	}
	out.indent();
	out.dedent();
	out.line() << "}";
}


int IdAllocationResponse::m_CustomId = 74;
namespace { csp::TaggedRecordFactory<IdAllocationResponse> __IdAllocationResponse_factory; }

void IdAllocationResponse::serialize(csp::TagWriter &writer) const {
	writer.beginCompound();
	if (has_first_id()) {
		writer.writeTag(TAG_first_id);
		writer.writer << m_first_id;
	}
	if (has_id_count()) {
		writer.writeTag(TAG_id_count);
		writer.writer << m_id_count;
	}
	writer.endCompound();
}

void IdAllocationResponse::serialize(csp::TagReader &reader) {
	reader.beginCompound();
	for (int tag = reader.nextTag(); tag != 0; tag = reader.nextTag()) {
		switch (tag) {
			case TAG_first_id: {
				reader.reader >> m_first_id;
				m_has0 |= 0x00000001;
				break;
			}
			case TAG_id_count: {
				reader.reader >> m_id_count;
				m_has0 |= 0x00000002;
				break;
			}
			default: break;
		}
	}
	reader.endCompound();
}

void IdAllocationResponse::dump(std::ostream &, int) const {}
void IdAllocationResponse::dump(csp::DumpWriter &out, const char *name) const {
	if (name) {
		out.line() << name << ": IdAllocationResponse {";
	} else {
		out.line() << "IdAllocationResponse {";
	}
	out.indent();
	if (has_first_id()) {
		out.line() << "first_id: " << m_first_id;
	}
	if (has_id_count()) {
		out.line() << "id_count: " << m_id_count;
	}
	out.dedent();
	out.line() << "}";
}


int TestUnitState::m_CustomId = 75;
namespace { csp::TaggedRecordFactory<TestUnitState> __TestUnitState_factory; }

void TestUnitState::serialize(csp::TagWriter &writer) const {
	writer.beginCompound();
	if (has_timestamp()) {
		writer.writeTag(TAG_timestamp);
		writer.writer << m_timestamp;
	}
	if (has_position()) {
		writer.writeTag(TAG_position);
		writer.writer << m_position;
	}
	if (has_velocity()) {
		writer.writeTag(TAG_velocity);
		writer.writer << m_velocity;
	}
	if (has_attitude()) {
		writer.writeTag(TAG_attitude);
		writer.writer << m_attitude;
	}
	if (has_test_object()) {
		writer.writeTag(TAG_test_object);
		writer.writer << m_test_object;
	}
	writer.endCompound();
}

void TestUnitState::serialize(csp::TagReader &reader) {
	reader.beginCompound();
	for (int tag = reader.nextTag(); tag != 0; tag = reader.nextTag()) {
		switch (tag) {
			case TAG_timestamp: {
				reader.reader >> m_timestamp;
				m_has0 |= 0x00000001;
				break;
			}
			case TAG_position: {
				reader.reader >> m_position;
				m_has0 |= 0x00000002;
				break;
			}
			case TAG_velocity: {
				reader.reader >> m_velocity;
				m_has0 |= 0x00000004;
				break;
			}
			case TAG_attitude: {
				reader.reader >> m_attitude;
				m_has0 |= 0x00000008;
				break;
			}
			case TAG_test_object: {
				reader.reader >> m_test_object;
				m_has0 |= 0x00000010;
				break;
			}
			default: break;
		}
	}
	reader.endCompound();
}

void TestUnitState::dump(std::ostream &, int) const {}
void TestUnitState::dump(csp::DumpWriter &out, const char *name) const {
	if (name) {
		out.line() << name << ": TestUnitState {";
	} else {
		out.line() << "TestUnitState {";
	}
	out.indent();
	if (has_timestamp()) {
		out.line() << "timestamp: " << m_timestamp;
	}
	if (has_position()) {
		out.line() << "position: " << m_position;
	}
	if (has_velocity()) {
		out.line() << "velocity: " << m_velocity;
	}
	if (has_attitude()) {
		out.line() << "attitude: " << m_attitude;
	}
	if (has_test_object()) {
		out.line() << "test_object: " << m_test_object;
	}
	out.dedent();
	out.line() << "}";
}

