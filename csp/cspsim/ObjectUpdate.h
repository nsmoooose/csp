// Generated on Fri Jun 25 00:05:22 2010
// This is a generated file --- DO NOT EDIT!

#ifndef __CSP_CSPSIM_OBJECTUPDATE_H__
#define __CSP_CSPSIM_OBJECTUPDATE_H__

#include <csp/csplib/net/NetworkMessage.h>
#include <csp/csplib/data/Vector3.h>
#include <csp/csplib/data/Quat.h>
#include <csp/csplib/util/MessageTypes.h>


class ObjectUpdate : public csp::NetworkMessage {
	static int m_CustomId;
	enum {
		TAG_timestamp = 8,
		TAG_position = 9,
		TAG_velocity = 10,
		TAG_attitude = 11,
		TAG_animation_flags = 12,
		TAG_animation_values = 13
	};

	csp::uint32 m_has0;
	
	csp::int32 m_timestamp;
	csp::GlobalPosition m_position;
	csp::Vector3f m_velocity;
	csp::Vector4f m_attitude;
	std::vector<csp::uint8> m_animation_flags;
	std::vector<csp::uint8> m_animation_values;
public:

	inline bool has_timestamp() const { return (m_has0 & 0x00000001) != 0; }
	inline bool has_position() const { return (m_has0 & 0x00000002) != 0; }
	inline bool has_velocity() const { return (m_has0 & 0x00000004) != 0; }
	inline bool has_attitude() const { return (m_has0 & 0x00000008) != 0; }
	inline bool has_animation_flags() const { return (m_has0 & 0x00000010) != 0; }
	inline bool has_animation_values() const { return (m_has0 & 0x00000020) != 0; }

	inline void clear_timestamp() {
		m_has0 &= ~0x00000001;
	}

	inline void clear_position() {
		m_has0 &= ~0x00000002;
	}

	inline void clear_velocity() {
		m_has0 &= ~0x00000004;
	}

	inline void clear_attitude() {
		m_has0 &= ~0x00000008;
	}

	inline void clear_animation_flags() {
		m_has0 &= ~0x00000010;
		m_animation_flags.clear();
	}

	inline void clear_animation_values() {
		m_has0 &= ~0x00000020;
		m_animation_values.clear();
	}

	inline csp::int32 const & timestamp() const { return m_timestamp; }
	inline csp::GlobalPosition const & position() const { return m_position; }
	inline csp::Vector3f const & velocity() const { return m_velocity; }
	inline csp::Vector4f const & attitude() const { return m_attitude; }
	inline std::vector<csp::uint8> const & animation_flags() const { return m_animation_flags; }
	inline std::vector<csp::uint8> const & animation_values() const { return m_animation_values; }

	void set_timestamp(csp::int32 const &value) {
		m_timestamp = value;
		m_has0 |= 0x00000001;
	}

	void set_position(csp::GlobalPosition const &value) {
		m_position = value;
		m_has0 |= 0x00000002;
	}

	void set_velocity(csp::Vector3f const &value) {
		m_velocity = value;
		m_has0 |= 0x00000004;
	}

	void set_attitude(csp::Vector4f const &value) {
		m_attitude = value;
		m_has0 |= 0x00000008;
	}

	void set_animation_flags(std::vector<csp::uint8> const &value) {
		m_animation_flags = value;
		m_has0 |= 0x00000010;
	}

	std::vector<csp::uint8> &set_animation_flags() {
		m_has0 |= 0x00000010;
		return m_animation_flags;
	}

	void set_animation_values(std::vector<csp::uint8> const &value) {
		m_animation_values = value;
		m_has0 |= 0x00000020;
	}

	std::vector<csp::uint8> &set_animation_values() {
		m_has0 |= 0x00000020;
		return m_animation_values;
	}

	ObjectUpdate() :
		m_has0(0),
		m_timestamp(),
		m_position(),
		m_velocity(),
		m_attitude(),
		m_animation_flags(),
		m_animation_values()
	{ }

	void serialize(csp::TagWriter &writer) const;
	void serialize(csp::TagReader &reader);

	void dump(std::ostream &, int) const;
	void dump(csp::DumpWriter &out, const char *name=0) const;

	typedef csp::Ref<ObjectUpdate> Ref;
	virtual Id getId() const { return _getId(); }
	static const Id _Id = CSP_ULL(18372914333876130613);
	static inline Id _getId() { return _Id; }
	virtual int getCustomId() const { return _getCustomId(); }
	static inline int _getCustomId() { return m_CustomId; }
	static inline void _setCustomId(int id) { m_CustomId = id; }
	virtual int getVersion() const { return _getVersion(); }
	static inline int _getVersion() { return 0; }
	virtual std::string getName() const { return _getName(); }
	static inline std::string _getName() { return "ObjectUpdate"; }
};

#endif
