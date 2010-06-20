// Generated on Mon Jun 21 00:46:46 2010
// This is a generated file --- DO NOT EDIT!

#include <csp/csplib/net/ClientServerMessages.h>
#include <csp/csplib/net/TaggedRecordRegistry.h>


int Reserved0::m_CustomId = 0;
namespace { csp::TaggedRecordFactory<Reserved0> __Reserved0_factory; }

void Reserved0::serialize(csp::TagWriter &writer) const {
	writer.beginCompound();
	writer.endCompound();
}

void Reserved0::serialize(csp::TagReader &reader) {
	reader.beginCompound();
	reader.endCompound();
}

void Reserved0::dump(std::ostream &, int) const {}
void Reserved0::dump(csp::DumpWriter &out, const char *name) const {
	if (name) {
		out.line() << name << ": Reserved0 {";
	} else {
		out.line() << "Reserved0 {";
	}
	out.indent();
	out.dedent();
	out.line() << "}";
}


int Reserved1::m_CustomId = 1;
namespace { csp::TaggedRecordFactory<Reserved1> __Reserved1_factory; }

void Reserved1::serialize(csp::TagWriter &writer) const {
	writer.beginCompound();
	writer.endCompound();
}

void Reserved1::serialize(csp::TagReader &reader) {
	reader.beginCompound();
	reader.endCompound();
}

void Reserved1::dump(std::ostream &, int) const {}
void Reserved1::dump(csp::DumpWriter &out, const char *name) const {
	if (name) {
		out.line() << name << ": Reserved1 {";
	} else {
		out.line() << "Reserved1 {";
	}
	out.indent();
	out.dedent();
	out.line() << "}";
}


int Acknowledge::m_CustomId = 2;
namespace { csp::TaggedRecordFactory<Acknowledge> __Acknowledge_factory; }

void Acknowledge::serialize(csp::TagWriter &writer) const {
	writer.beginCompound();
	writer.endCompound();
}

void Acknowledge::serialize(csp::TagReader &reader) {
	reader.beginCompound();
	reader.endCompound();
}

void Acknowledge::dump(std::ostream &, int) const {}
void Acknowledge::dump(csp::DumpWriter &out, const char *name) const {
	if (name) {
		out.line() << name << ": Acknowledge {";
	} else {
		out.line() << "Acknowledge {";
	}
	out.indent();
	out.dedent();
	out.line() << "}";
}


int Disconnect::m_CustomId = 3;
namespace { csp::TaggedRecordFactory<Disconnect> __Disconnect_factory; }

void Disconnect::serialize(csp::TagWriter &writer) const {
	writer.beginCompound();
	writer.endCompound();
}

void Disconnect::serialize(csp::TagReader &reader) {
	reader.beginCompound();
	reader.endCompound();
}

void Disconnect::dump(std::ostream &, int) const {}
void Disconnect::dump(csp::DumpWriter &out, const char *name) const {
	if (name) {
		out.line() << name << ": Disconnect {";
	} else {
		out.line() << "Disconnect {";
	}
	out.indent();
	out.dedent();
	out.line() << "}";
}


int Reset::m_CustomId = 4;
namespace { csp::TaggedRecordFactory<Reset> __Reset_factory; }

void Reset::serialize(csp::TagWriter &writer) const {
	writer.beginCompound();
	writer.endCompound();
}

void Reset::serialize(csp::TagReader &reader) {
	reader.beginCompound();
	reader.endCompound();
}

void Reset::dump(std::ostream &, int) const {}
void Reset::dump(csp::DumpWriter &out, const char *name) const {
	if (name) {
		out.line() << name << ": Reset {";
	} else {
		out.line() << "Reset {";
	}
	out.indent();
	out.dedent();
	out.line() << "}";
}


int ConnectionRequest::m_CustomId = 5;
namespace { csp::TaggedRecordFactory<ConnectionRequest> __ConnectionRequest_factory; }

void ConnectionRequest::serialize(csp::TagWriter &writer) const {
	writer.beginCompound();
	if (has_password()) {
		writer.writeTag(TAG_password);
		writer.writer << m_password;
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

void ConnectionRequest::serialize(csp::TagReader &reader) {
	reader.beginCompound();
	for (int tag = reader.nextTag(); tag != 0; tag = reader.nextTag()) {
		switch (tag) {
			case TAG_password: {
				reader.reader >> m_password;
				m_has0 |= 0x00000001;
				break;
			}
			case TAG_incoming_bw: {
				reader.reader >> m_incoming_bw;
				m_has0 |= 0x00000002;
				break;
			}
			case TAG_outgoing_bw: {
				reader.reader >> m_outgoing_bw;
				m_has0 |= 0x00000004;
				break;
			}
			default: break;
		}
	}
	reader.endCompound();
}

void ConnectionRequest::dump(std::ostream &, int) const {}
void ConnectionRequest::dump(csp::DumpWriter &out, const char *name) const {
	if (name) {
		out.line() << name << ": ConnectionRequest {";
	} else {
		out.line() << "ConnectionRequest {";
	}
	out.indent();
	if (has_password()) {
		out.line() << "password: " << m_password;
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


int ConnectionResponse::m_CustomId = 6;
namespace { csp::TaggedRecordFactory<ConnectionResponse> __ConnectionResponse_factory; }

void ConnectionResponse::serialize(csp::TagWriter &writer) const {
	writer.beginCompound();
	if (has_server_id()) {
		writer.writeTag(TAG_server_id);
		writer.writer << m_server_id;
	}
	if (has_response()) {
		writer.writeTag(TAG_response);
		writer.writer << m_response;
	}
	if (has_success()) {
		writer.writeTag(TAG_success);
		writer.writer << m_success;
	}
	if (has_client_id()) {
		writer.writeTag(TAG_client_id);
		writer.writer << m_client_id;
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

void ConnectionResponse::serialize(csp::TagReader &reader) {
	reader.beginCompound();
	for (int tag = reader.nextTag(); tag != 0; tag = reader.nextTag()) {
		switch (tag) {
			case TAG_server_id: {
				reader.reader >> m_server_id;
				m_has0 |= 0x00000001;
				break;
			}
			case TAG_response: {
				reader.reader >> m_response;
				m_has0 |= 0x00000002;
				break;
			}
			case TAG_success: {
				reader.reader >> m_success;
				m_has0 |= 0x00000004;
				break;
			}
			case TAG_client_id: {
				reader.reader >> m_client_id;
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

void ConnectionResponse::dump(std::ostream &, int) const {}
void ConnectionResponse::dump(csp::DumpWriter &out, const char *name) const {
	if (name) {
		out.line() << name << ": ConnectionResponse {";
	} else {
		out.line() << "ConnectionResponse {";
	}
	out.indent();
	if (has_server_id()) {
		out.line() << "server_id: " << m_server_id;
	}
	if (has_response()) {
		out.line() << "response: " << m_response;
	}
	if (has_success()) {
		out.line() << "success: " << m_success;
	}
	if (has_client_id()) {
		out.line() << "client_id: " << m_client_id;
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

