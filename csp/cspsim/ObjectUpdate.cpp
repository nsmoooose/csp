// Generated on Mon Jun 21 00:46:53 2010
// This is a generated file --- DO NOT EDIT!

#include <csp/cspsim/ObjectUpdate.h>
#include <csp/csplib/net/TaggedRecordRegistry.h>


int ObjectUpdate::m_CustomId = 1025;
namespace { csp::TaggedRecordFactory<ObjectUpdate> __ObjectUpdate_factory; }

void ObjectUpdate::serialize(csp::TagWriter &writer) const {
	writer.beginCompound();
	assert(has_timestamp());
	writer.writer << m_timestamp;
	assert(has_position());
	writer.writer << m_position;
	assert(has_velocity());
	writer.writer << m_velocity;
	if (has_attitude()) {
		writer.writeTag(TAG_attitude);
		writer.writer << m_attitude;
	}
	if (has_animation_flags()) {
		writer.writeTag(TAG_animation_flags);
		writer.writeLength(m_animation_flags.size());
		for (unsigned i=0; i < m_animation_flags.size(); ++i) {
			writer.writer << m_animation_flags[i];
		}
	}
	if (has_animation_values()) {
		writer.writeTag(TAG_animation_values);
		writer.writeLength(m_animation_values.size());
		for (unsigned i=0; i < m_animation_values.size(); ++i) {
			writer.writer << m_animation_values[i];
		}
	}
	writer.endCompound();
}

void ObjectUpdate::serialize(csp::TagReader &reader) {
	reader.beginCompound();
	reader.reader >> m_timestamp;
	m_has0 |= 0x00000001;
	reader.reader >> m_position;
	m_has0 |= 0x00000002;
	reader.reader >> m_velocity;
	m_has0 |= 0x00000004;
	for (int tag = reader.nextTag(); tag != 0; tag = reader.nextTag()) {
		switch (tag) {
			case TAG_attitude: {
				reader.reader >> m_attitude;
				m_has0 |= 0x00000008;
				break;
			}
			case TAG_animation_flags: {
				unsigned len = reader.readLength();
				m_animation_flags.resize(len);
				for (unsigned i=0; i < len; ++i) {
					reader.reader >> m_animation_flags[i];
				}
				m_has0 |= 0x00000010;
				break;
			}
			case TAG_animation_values: {
				unsigned len = reader.readLength();
				m_animation_values.resize(len);
				for (unsigned i=0; i < len; ++i) {
					reader.reader >> m_animation_values[i];
				}
				m_has0 |= 0x00000020;
				break;
			}
			default: break;
		}
	}
	reader.endCompound();
}

void ObjectUpdate::dump(std::ostream &, int) const {}
void ObjectUpdate::dump(csp::DumpWriter &out, const char *name) const {
	if (name) {
		out.line() << name << ": ObjectUpdate {";
	} else {
		out.line() << "ObjectUpdate {";
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
	if (has_animation_flags()) {
		out.line() << "animation_flags: [";
		out.indent();
		for (unsigned i=0; i < m_animation_flags.size(); ++i) {
			out.prefix() << i << ": ";
			out.line() << m_animation_flags[i];
		}
		out.dedent();
		out.line() << "]";
	}
	if (has_animation_values()) {
		out.line() << "animation_values: [";
		out.indent();
		for (unsigned i=0; i < m_animation_values.size(); ++i) {
			out.prefix() << i << ": ";
			out.line() << m_animation_values[i];
		}
		out.dedent();
		out.line() << "]";
	}
	out.dedent();
	out.line() << "}";
}

