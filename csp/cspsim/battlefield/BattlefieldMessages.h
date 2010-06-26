// Generated on Fri Jun 25 00:05:22 2010
// This is a generated file --- DO NOT EDIT!

#ifndef __CSP_CSPSIM_BATTLEFIELD_BATTLEFIELDMESSAGES_H__
#define __CSP_CSPSIM_BATTLEFIELD_BATTLEFIELDMESSAGES_H__

#include <csp/csplib/data/Path.h>
#include <csp/csplib/net/NetworkMessage.h>
#include <csp/csplib/data/Vector3.h>
#include <csp/csplib/data/Quat.h>


class JoinRequest : public csp::NetworkMessage {
	static int m_CustomId;
	enum {
		TAG_user_name = 8,
		TAG_internal_ip_addr = 9,
		TAG_external_ip_addr = 10,
		TAG_local_time = 11
	};

	csp::uint32 m_has0;
	
	std::string m_user_name;
	csp::uint32 m_internal_ip_addr;
	csp::uint32 m_external_ip_addr;
	csp::uint32 m_local_time;
public:

	inline bool has_user_name() const { return (m_has0 & 0x00000001) != 0; }
	inline bool has_internal_ip_addr() const { return (m_has0 & 0x00000002) != 0; }
	inline bool has_external_ip_addr() const { return (m_has0 & 0x00000004) != 0; }
	inline bool has_local_time() const { return (m_has0 & 0x00000008) != 0; }

	inline void clear_user_name() {
		m_has0 &= ~0x00000001;
	}

	inline void clear_internal_ip_addr() {
		m_has0 &= ~0x00000002;
	}

	inline void clear_external_ip_addr() {
		m_has0 &= ~0x00000004;
	}

	inline void clear_local_time() {
		m_has0 &= ~0x00000008;
	}

	inline std::string const & user_name() const { return m_user_name; }
	inline csp::uint32 const & internal_ip_addr() const { return m_internal_ip_addr; }
	inline csp::uint32 const & external_ip_addr() const { return m_external_ip_addr; }
	inline csp::uint32 const & local_time() const { return m_local_time; }

	void set_user_name(std::string const &value) {
		m_user_name = value;
		m_has0 |= 0x00000001;
	}

	void set_internal_ip_addr(csp::uint32 const &value) {
		m_internal_ip_addr = value;
		m_has0 |= 0x00000002;
	}

	void set_external_ip_addr(csp::uint32 const &value) {
		m_external_ip_addr = value;
		m_has0 |= 0x00000004;
	}

	void set_local_time(csp::uint32 const &value) {
		m_local_time = value;
		m_has0 |= 0x00000008;
	}

	JoinRequest() :
		m_has0(0),
		m_user_name(),
		m_internal_ip_addr(),
		m_external_ip_addr(),
		m_local_time()
	{ }

	void serialize(csp::TagWriter &writer) const;
	void serialize(csp::TagReader &reader);

	void dump(std::ostream &, int) const;
	void dump(csp::DumpWriter &out, const char *name=0) const;

	typedef csp::Ref<JoinRequest> Ref;
	virtual Id getId() const { return _getId(); }
	static const Id _Id = CSP_ULL(7026249182531438171);
	static inline Id _getId() { return _Id; }
	virtual int getCustomId() const { return _getCustomId(); }
	static inline int _getCustomId() { return m_CustomId; }
	static inline void _setCustomId(int id) { m_CustomId = id; }
	virtual int getVersion() const { return _getVersion(); }
	static inline int _getVersion() { return 0; }
	virtual std::string getName() const { return _getName(); }
	static inline std::string _getName() { return "JoinRequest"; }
};


class JoinResponse : public csp::NetworkMessage {
	static int m_CustomId;
	enum {
		TAG_success = 8,
		TAG_details = 9,
		TAG_first_id = 10,
		TAG_id_count = 11
	};

	csp::uint32 m_has0;
	
	bool m_success;
	std::string m_details;
	csp::int32 m_first_id;
	csp::int32 m_id_count;
public:

	inline bool has_success() const { return (m_has0 & 0x00000001) != 0; }
	inline bool has_details() const { return (m_has0 & 0x00000002) != 0; }
	inline bool has_first_id() const { return (m_has0 & 0x00000004) != 0; }
	inline bool has_id_count() const { return (m_has0 & 0x00000008) != 0; }

	inline void clear_success() {
		m_has0 &= ~0x00000001;
	}

	inline void clear_details() {
		m_has0 &= ~0x00000002;
	}

	inline void clear_first_id() {
		m_has0 &= ~0x00000004;
	}

	inline void clear_id_count() {
		m_has0 &= ~0x00000008;
	}

	inline bool const & success() const { return m_success; }
	inline std::string const & details() const { return m_details; }
	inline csp::int32 const & first_id() const { return m_first_id; }
	inline csp::int32 const & id_count() const { return m_id_count; }

	void set_success(bool const &value) {
		m_success = value;
		m_has0 |= 0x00000001;
	}

	void set_details(std::string const &value) {
		m_details = value;
		m_has0 |= 0x00000002;
	}

	void set_first_id(csp::int32 const &value) {
		m_first_id = value;
		m_has0 |= 0x00000004;
	}

	void set_id_count(csp::int32 const &value) {
		m_id_count = value;
		m_has0 |= 0x00000008;
	}

	JoinResponse() :
		m_has0(0),
		m_success(),
		m_details(),
		m_first_id(),
		m_id_count()
	{ }

	void serialize(csp::TagWriter &writer) const;
	void serialize(csp::TagReader &reader);

	void dump(std::ostream &, int) const;
	void dump(csp::DumpWriter &out, const char *name=0) const;

	typedef csp::Ref<JoinResponse> Ref;
	virtual Id getId() const { return _getId(); }
	static const Id _Id = CSP_ULL(4789336035534859801);
	static inline Id _getId() { return _Id; }
	virtual int getCustomId() const { return _getCustomId(); }
	static inline int _getCustomId() { return m_CustomId; }
	static inline void _setCustomId(int id) { m_CustomId = id; }
	virtual int getVersion() const { return _getVersion(); }
	static inline int _getVersion() { return 0; }
	virtual std::string getName() const { return _getName(); }
	static inline std::string _getName() { return "JoinResponse"; }
};


class PlayerJoin : public csp::NetworkMessage {
	static int m_CustomId;
	enum {
		TAG_user_name = 8,
		TAG_peer_id = 9,
		TAG_port = 10,
		TAG_ip_addr = 11,
		TAG_incoming_bw = 12,
		TAG_outgoing_bw = 13
	};

	csp::uint32 m_has0;
	
	std::string m_user_name;
	csp::int16 m_peer_id;
	csp::int16 m_port;
	csp::int32 m_ip_addr;
	csp::int32 m_incoming_bw;
	csp::int32 m_outgoing_bw;
public:

	inline bool has_user_name() const { return (m_has0 & 0x00000001) != 0; }
	inline bool has_peer_id() const { return (m_has0 & 0x00000002) != 0; }
	inline bool has_port() const { return (m_has0 & 0x00000004) != 0; }
	inline bool has_ip_addr() const { return (m_has0 & 0x00000008) != 0; }
	inline bool has_incoming_bw() const { return (m_has0 & 0x00000010) != 0; }
	inline bool has_outgoing_bw() const { return (m_has0 & 0x00000020) != 0; }

	inline void clear_user_name() {
		m_has0 &= ~0x00000001;
	}

	inline void clear_peer_id() {
		m_has0 &= ~0x00000002;
	}

	inline void clear_port() {
		m_has0 &= ~0x00000004;
	}

	inline void clear_ip_addr() {
		m_has0 &= ~0x00000008;
	}

	inline void clear_incoming_bw() {
		m_has0 &= ~0x00000010;
	}

	inline void clear_outgoing_bw() {
		m_has0 &= ~0x00000020;
	}

	inline std::string const & user_name() const { return m_user_name; }
	inline csp::int16 const & peer_id() const { return m_peer_id; }
	inline csp::int16 const & port() const { return m_port; }
	inline csp::int32 const & ip_addr() const { return m_ip_addr; }
	inline csp::int32 const & incoming_bw() const { return m_incoming_bw; }
	inline csp::int32 const & outgoing_bw() const { return m_outgoing_bw; }

	void set_user_name(std::string const &value) {
		m_user_name = value;
		m_has0 |= 0x00000001;
	}

	void set_peer_id(csp::int16 const &value) {
		m_peer_id = value;
		m_has0 |= 0x00000002;
	}

	void set_port(csp::int16 const &value) {
		m_port = value;
		m_has0 |= 0x00000004;
	}

	void set_ip_addr(csp::int32 const &value) {
		m_ip_addr = value;
		m_has0 |= 0x00000008;
	}

	void set_incoming_bw(csp::int32 const &value) {
		m_incoming_bw = value;
		m_has0 |= 0x00000010;
	}

	void set_outgoing_bw(csp::int32 const &value) {
		m_outgoing_bw = value;
		m_has0 |= 0x00000020;
	}

	PlayerJoin() :
		m_has0(0),
		m_user_name(),
		m_peer_id(),
		m_port(),
		m_ip_addr(),
		m_incoming_bw(),
		m_outgoing_bw()
	{ }

	void serialize(csp::TagWriter &writer) const;
	void serialize(csp::TagReader &reader);

	void dump(std::ostream &, int) const;
	void dump(csp::DumpWriter &out, const char *name=0) const;

	typedef csp::Ref<PlayerJoin> Ref;
	virtual Id getId() const { return _getId(); }
	static const Id _Id = CSP_ULL(2232294147692029740);
	static inline Id _getId() { return _Id; }
	virtual int getCustomId() const { return _getCustomId(); }
	static inline int _getCustomId() { return m_CustomId; }
	static inline void _setCustomId(int id) { m_CustomId = id; }
	virtual int getVersion() const { return _getVersion(); }
	static inline int _getVersion() { return 0; }
	virtual std::string getName() const { return _getName(); }
	static inline std::string _getName() { return "PlayerJoin"; }
};


class PlayerQuit : public csp::NetworkMessage {
	static int m_CustomId;
	enum {
		TAG_peer_id = 8
	};

	csp::uint32 m_has0;
	
	csp::int16 m_peer_id;
public:

	inline bool has_peer_id() const { return (m_has0 & 0x00000001) != 0; }

	inline void clear_peer_id() {
		m_has0 &= ~0x00000001;
	}

	inline csp::int16 const & peer_id() const { return m_peer_id; }

	void set_peer_id(csp::int16 const &value) {
		m_peer_id = value;
		m_has0 |= 0x00000001;
	}

	PlayerQuit() :
		m_has0(0),
		m_peer_id()
	{ }

	void serialize(csp::TagWriter &writer) const;
	void serialize(csp::TagReader &reader);

	void dump(std::ostream &, int) const;
	void dump(csp::DumpWriter &out, const char *name=0) const;

	typedef csp::Ref<PlayerQuit> Ref;
	virtual Id getId() const { return _getId(); }
	static const Id _Id = CSP_ULL(16142517240213339025);
	static inline Id _getId() { return _Id; }
	virtual int getCustomId() const { return _getCustomId(); }
	static inline int _getCustomId() { return m_CustomId; }
	static inline void _setCustomId(int id) { m_CustomId = id; }
	virtual int getVersion() const { return _getVersion(); }
	static inline int _getVersion() { return 0; }
	virtual std::string getName() const { return _getName(); }
	static inline std::string _getName() { return "PlayerQuit"; }
};


class RegisterUnit : public csp::NetworkMessage {
	static int m_CustomId;
	enum {
		TAG_unit_id = 8,
		TAG_unit_class = 9,
		TAG_unit_type = 10,
		TAG_grid_x = 11,
		TAG_grid_y = 12
	};

	csp::uint32 m_has0;
	
	csp::uint32 m_unit_id;
	csp::Path m_unit_class;
	csp::uint8 m_unit_type;
	csp::uint32 m_grid_x;
	csp::uint32 m_grid_y;
public:

	inline bool has_unit_id() const { return (m_has0 & 0x00000001) != 0; }
	inline bool has_unit_class() const { return (m_has0 & 0x00000002) != 0; }
	inline bool has_unit_type() const { return (m_has0 & 0x00000004) != 0; }
	inline bool has_grid_x() const { return (m_has0 & 0x00000008) != 0; }
	inline bool has_grid_y() const { return (m_has0 & 0x00000010) != 0; }

	inline void clear_unit_id() {
		m_has0 &= ~0x00000001;
	}

	inline void clear_unit_class() {
		m_has0 &= ~0x00000002;
	}

	inline void clear_unit_type() {
		m_has0 &= ~0x00000004;
	}

	inline void clear_grid_x() {
		m_has0 &= ~0x00000008;
	}

	inline void clear_grid_y() {
		m_has0 &= ~0x00000010;
	}

	inline csp::uint32 const & unit_id() const { return m_unit_id; }
	inline csp::Path const & unit_class() const { return m_unit_class; }
	inline csp::uint8 const & unit_type() const { return m_unit_type; }
	inline csp::uint32 const & grid_x() const { return m_grid_x; }
	inline csp::uint32 const & grid_y() const { return m_grid_y; }

	void set_unit_id(csp::uint32 const &value) {
		m_unit_id = value;
		m_has0 |= 0x00000001;
	}

	void set_unit_class(csp::Path const &value) {
		m_unit_class = value;
		m_has0 |= 0x00000002;
	}

	void set_unit_type(csp::uint8 const &value) {
		m_unit_type = value;
		m_has0 |= 0x00000004;
	}

	void set_grid_x(csp::uint32 const &value) {
		m_grid_x = value;
		m_has0 |= 0x00000008;
	}

	void set_grid_y(csp::uint32 const &value) {
		m_grid_y = value;
		m_has0 |= 0x00000010;
	}

	RegisterUnit() :
		m_has0(0),
		m_unit_id(),
		m_unit_class(),
		m_unit_type(),
		m_grid_x(),
		m_grid_y()
	{ }

	void serialize(csp::TagWriter &writer) const;
	void serialize(csp::TagReader &reader);

	void dump(std::ostream &, int) const;
	void dump(csp::DumpWriter &out, const char *name=0) const;

	typedef csp::Ref<RegisterUnit> Ref;
	virtual Id getId() const { return _getId(); }
	static const Id _Id = CSP_ULL(3570889835994609180);
	static inline Id _getId() { return _Id; }
	virtual int getCustomId() const { return _getCustomId(); }
	static inline int _getCustomId() { return m_CustomId; }
	static inline void _setCustomId(int id) { m_CustomId = id; }
	virtual int getVersion() const { return _getVersion(); }
	static inline int _getVersion() { return 0; }
	virtual std::string getName() const { return _getName(); }
	static inline std::string _getName() { return "RegisterUnit"; }
};


class NotifyUnitMotion : public csp::NetworkMessage {
	static int m_CustomId;
	enum {
		TAG_unit_id = 8,
		TAG_grid_x = 9,
		TAG_grid_y = 10
	};

	csp::uint32 m_has0;
	
	csp::uint32 m_unit_id;
	csp::uint32 m_grid_x;
	csp::uint32 m_grid_y;
public:

	inline bool has_unit_id() const { return (m_has0 & 0x00000001) != 0; }
	inline bool has_grid_x() const { return (m_has0 & 0x00000002) != 0; }
	inline bool has_grid_y() const { return (m_has0 & 0x00000004) != 0; }

	inline void clear_unit_id() {
		m_has0 &= ~0x00000001;
	}

	inline void clear_grid_x() {
		m_has0 &= ~0x00000002;
	}

	inline void clear_grid_y() {
		m_has0 &= ~0x00000004;
	}

	inline csp::uint32 const & unit_id() const { return m_unit_id; }
	inline csp::uint32 const & grid_x() const { return m_grid_x; }
	inline csp::uint32 const & grid_y() const { return m_grid_y; }

	void set_unit_id(csp::uint32 const &value) {
		m_unit_id = value;
		m_has0 |= 0x00000001;
	}

	void set_grid_x(csp::uint32 const &value) {
		m_grid_x = value;
		m_has0 |= 0x00000002;
	}

	void set_grid_y(csp::uint32 const &value) {
		m_grid_y = value;
		m_has0 |= 0x00000004;
	}

	NotifyUnitMotion() :
		m_has0(0),
		m_unit_id(),
		m_grid_x(),
		m_grid_y()
	{ }

	void serialize(csp::TagWriter &writer) const;
	void serialize(csp::TagReader &reader);

	void dump(std::ostream &, int) const;
	void dump(csp::DumpWriter &out, const char *name=0) const;

	typedef csp::Ref<NotifyUnitMotion> Ref;
	virtual Id getId() const { return _getId(); }
	static const Id _Id = CSP_ULL(6055069276926594070);
	static inline Id _getId() { return _Id; }
	virtual int getCustomId() const { return _getCustomId(); }
	static inline int _getCustomId() { return m_CustomId; }
	static inline void _setCustomId(int id) { m_CustomId = id; }
	virtual int getVersion() const { return _getVersion(); }
	static inline int _getVersion() { return 0; }
	virtual std::string getName() const { return _getName(); }
	static inline std::string _getName() { return "NotifyUnitMotion"; }
};


class CommandUpdatePeer : public csp::NetworkMessage {
	static int m_CustomId;
	enum {
		TAG_unit_id = 8,
		TAG_peer_id = 9,
		TAG_stop = 10
	};

	csp::uint32 m_has0;
	
	csp::uint32 m_unit_id;
	csp::uint16 m_peer_id;
	bool m_stop;
public:

	inline bool has_unit_id() const { return (m_has0 & 0x00000001) != 0; }
	inline bool has_peer_id() const { return (m_has0 & 0x00000002) != 0; }
	inline bool has_stop() const { return (m_has0 & 0x00000004) != 0; }

	inline void clear_unit_id() {
		m_has0 &= ~0x00000001;
	}

	inline void clear_peer_id() {
		m_has0 &= ~0x00000002;
	}

	inline void clear_stop() {
		m_has0 &= ~0x00000004;
	}

	inline csp::uint32 const & unit_id() const { return m_unit_id; }
	inline csp::uint16 const & peer_id() const { return m_peer_id; }
	inline bool const & stop() const { return m_stop; }

	void set_unit_id(csp::uint32 const &value) {
		m_unit_id = value;
		m_has0 |= 0x00000001;
	}

	void set_peer_id(csp::uint16 const &value) {
		m_peer_id = value;
		m_has0 |= 0x00000002;
	}

	void set_stop(bool const &value) {
		m_stop = value;
		m_has0 |= 0x00000004;
	}

	CommandUpdatePeer() :
		m_has0(0),
		m_unit_id(),
		m_peer_id(),
		m_stop()
	{ }

	void serialize(csp::TagWriter &writer) const;
	void serialize(csp::TagReader &reader);

	void dump(std::ostream &, int) const;
	void dump(csp::DumpWriter &out, const char *name=0) const;

	typedef csp::Ref<CommandUpdatePeer> Ref;
	virtual Id getId() const { return _getId(); }
	static const Id _Id = CSP_ULL(7670052571545178626);
	static inline Id _getId() { return _Id; }
	virtual int getCustomId() const { return _getCustomId(); }
	static inline int _getCustomId() { return m_CustomId; }
	static inline void _setCustomId(int id) { m_CustomId = id; }
	virtual int getVersion() const { return _getVersion(); }
	static inline int _getVersion() { return 0; }
	virtual std::string getName() const { return _getName(); }
	static inline std::string _getName() { return "CommandUpdatePeer"; }
};


class CommandAddUnit : public csp::NetworkMessage {
	static int m_CustomId;
	enum {
		TAG_unit_id = 8,
		TAG_unit_class = 9,
		TAG_unit_type = 10,
		TAG_owner_id = 11,
		TAG_grid_x = 12,
		TAG_grid_y = 13
	};

	csp::uint32 m_has0;
	
	csp::uint32 m_unit_id;
	csp::Path m_unit_class;
	csp::uint8 m_unit_type;
	csp::uint16 m_owner_id;
	csp::uint32 m_grid_x;
	csp::uint32 m_grid_y;
public:

	inline bool has_unit_id() const { return (m_has0 & 0x00000001) != 0; }
	inline bool has_unit_class() const { return (m_has0 & 0x00000002) != 0; }
	inline bool has_unit_type() const { return (m_has0 & 0x00000004) != 0; }
	inline bool has_owner_id() const { return (m_has0 & 0x00000008) != 0; }
	inline bool has_grid_x() const { return (m_has0 & 0x00000010) != 0; }
	inline bool has_grid_y() const { return (m_has0 & 0x00000020) != 0; }

	inline void clear_unit_id() {
		m_has0 &= ~0x00000001;
	}

	inline void clear_unit_class() {
		m_has0 &= ~0x00000002;
	}

	inline void clear_unit_type() {
		m_has0 &= ~0x00000004;
	}

	inline void clear_owner_id() {
		m_has0 &= ~0x00000008;
	}

	inline void clear_grid_x() {
		m_has0 &= ~0x00000010;
	}

	inline void clear_grid_y() {
		m_has0 &= ~0x00000020;
	}

	inline csp::uint32 const & unit_id() const { return m_unit_id; }
	inline csp::Path const & unit_class() const { return m_unit_class; }
	inline csp::uint8 const & unit_type() const { return m_unit_type; }
	inline csp::uint16 const & owner_id() const { return m_owner_id; }
	inline csp::uint32 const & grid_x() const { return m_grid_x; }
	inline csp::uint32 const & grid_y() const { return m_grid_y; }

	void set_unit_id(csp::uint32 const &value) {
		m_unit_id = value;
		m_has0 |= 0x00000001;
	}

	void set_unit_class(csp::Path const &value) {
		m_unit_class = value;
		m_has0 |= 0x00000002;
	}

	void set_unit_type(csp::uint8 const &value) {
		m_unit_type = value;
		m_has0 |= 0x00000004;
	}

	void set_owner_id(csp::uint16 const &value) {
		m_owner_id = value;
		m_has0 |= 0x00000008;
	}

	void set_grid_x(csp::uint32 const &value) {
		m_grid_x = value;
		m_has0 |= 0x00000010;
	}

	void set_grid_y(csp::uint32 const &value) {
		m_grid_y = value;
		m_has0 |= 0x00000020;
	}

	CommandAddUnit() :
		m_has0(0),
		m_unit_id(),
		m_unit_class(),
		m_unit_type(),
		m_owner_id(),
		m_grid_x(),
		m_grid_y()
	{ }

	void serialize(csp::TagWriter &writer) const;
	void serialize(csp::TagReader &reader);

	void dump(std::ostream &, int) const;
	void dump(csp::DumpWriter &out, const char *name=0) const;

	typedef csp::Ref<CommandAddUnit> Ref;
	virtual Id getId() const { return _getId(); }
	static const Id _Id = CSP_ULL(9015084212950032172);
	static inline Id _getId() { return _Id; }
	virtual int getCustomId() const { return _getCustomId(); }
	static inline int _getCustomId() { return m_CustomId; }
	static inline void _setCustomId(int id) { m_CustomId = id; }
	virtual int getVersion() const { return _getVersion(); }
	static inline int _getVersion() { return 0; }
	virtual std::string getName() const { return _getName(); }
	static inline std::string _getName() { return "CommandAddUnit"; }
};


class CommandRemoveUnit : public csp::NetworkMessage {
	static int m_CustomId;
	enum {
		TAG_unit_id = 8
	};

	csp::uint32 m_has0;
	
	csp::uint32 m_unit_id;
public:

	inline bool has_unit_id() const { return (m_has0 & 0x00000001) != 0; }

	inline void clear_unit_id() {
		m_has0 &= ~0x00000001;
	}

	inline csp::uint32 const & unit_id() const { return m_unit_id; }

	void set_unit_id(csp::uint32 const &value) {
		m_unit_id = value;
		m_has0 |= 0x00000001;
	}

	CommandRemoveUnit() :
		m_has0(0),
		m_unit_id()
	{ }

	void serialize(csp::TagWriter &writer) const;
	void serialize(csp::TagReader &reader);

	void dump(std::ostream &, int) const;
	void dump(csp::DumpWriter &out, const char *name=0) const;

	typedef csp::Ref<CommandRemoveUnit> Ref;
	virtual Id getId() const { return _getId(); }
	static const Id _Id = CSP_ULL(9884838619980171237);
	static inline Id _getId() { return _Id; }
	virtual int getCustomId() const { return _getCustomId(); }
	static inline int _getCustomId() { return m_CustomId; }
	static inline void _setCustomId(int id) { m_CustomId = id; }
	virtual int getVersion() const { return _getVersion(); }
	static inline int _getVersion() { return 0; }
	virtual std::string getName() const { return _getName(); }
	static inline std::string _getName() { return "CommandRemoveUnit"; }
};


class IdAllocationRequest : public csp::NetworkMessage {
	static int m_CustomId;
public:

	IdAllocationRequest() { }

	void serialize(csp::TagWriter &writer) const;
	void serialize(csp::TagReader &reader);

	void dump(std::ostream &, int) const;
	void dump(csp::DumpWriter &out, const char *name=0) const;

	typedef csp::Ref<IdAllocationRequest> Ref;
	virtual Id getId() const { return _getId(); }
	static const Id _Id = CSP_ULL(17676110009173052895);
	static inline Id _getId() { return _Id; }
	virtual int getCustomId() const { return _getCustomId(); }
	static inline int _getCustomId() { return m_CustomId; }
	static inline void _setCustomId(int id) { m_CustomId = id; }
	virtual int getVersion() const { return _getVersion(); }
	static inline int _getVersion() { return 0; }
	virtual std::string getName() const { return _getName(); }
	static inline std::string _getName() { return "IdAllocationRequest"; }
};


class IdAllocationResponse : public csp::NetworkMessage {
	static int m_CustomId;
	enum {
		TAG_first_id = 8,
		TAG_id_count = 9
	};

	csp::uint32 m_has0;
	
	csp::uint32 m_first_id;
	csp::uint32 m_id_count;
public:

	inline bool has_first_id() const { return (m_has0 & 0x00000001) != 0; }
	inline bool has_id_count() const { return (m_has0 & 0x00000002) != 0; }

	inline void clear_first_id() {
		m_has0 &= ~0x00000001;
	}

	inline void clear_id_count() {
		m_has0 &= ~0x00000002;
	}

	inline csp::uint32 const & first_id() const { return m_first_id; }
	inline csp::uint32 const & id_count() const { return m_id_count; }

	void set_first_id(csp::uint32 const &value) {
		m_first_id = value;
		m_has0 |= 0x00000001;
	}

	void set_id_count(csp::uint32 const &value) {
		m_id_count = value;
		m_has0 |= 0x00000002;
	}

	IdAllocationResponse() :
		m_has0(0),
		m_first_id(),
		m_id_count()
	{ }

	void serialize(csp::TagWriter &writer) const;
	void serialize(csp::TagReader &reader);

	void dump(std::ostream &, int) const;
	void dump(csp::DumpWriter &out, const char *name=0) const;

	typedef csp::Ref<IdAllocationResponse> Ref;
	virtual Id getId() const { return _getId(); }
	static const Id _Id = CSP_ULL(1160804850243922449);
	static inline Id _getId() { return _Id; }
	virtual int getCustomId() const { return _getCustomId(); }
	static inline int _getCustomId() { return m_CustomId; }
	static inline void _setCustomId(int id) { m_CustomId = id; }
	virtual int getVersion() const { return _getVersion(); }
	static inline int _getVersion() { return 0; }
	virtual std::string getName() const { return _getName(); }
	static inline std::string _getName() { return "IdAllocationResponse"; }
};


class TestUnitState : public csp::NetworkMessage {
	static int m_CustomId;
	enum {
		TAG_timestamp = 8,
		TAG_position = 9,
		TAG_velocity = 10,
		TAG_attitude = 11,
		TAG_test_object = 12
	};

	csp::uint32 m_has0;
	
	csp::uint32 m_timestamp;
	csp::Vector3 m_position;
	csp::Vector3 m_velocity;
	csp::Quat m_attitude;
	bool m_test_object;
public:

	inline bool has_timestamp() const { return (m_has0 & 0x00000001) != 0; }
	inline bool has_position() const { return (m_has0 & 0x00000002) != 0; }
	inline bool has_velocity() const { return (m_has0 & 0x00000004) != 0; }
	inline bool has_attitude() const { return (m_has0 & 0x00000008) != 0; }
	inline bool has_test_object() const { return (m_has0 & 0x00000010) != 0; }

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

	inline void clear_test_object() {
		m_has0 &= ~0x00000010;
	}

	inline csp::uint32 const & timestamp() const { return m_timestamp; }
	inline csp::Vector3 const & position() const { return m_position; }
	inline csp::Vector3 const & velocity() const { return m_velocity; }
	inline csp::Quat const & attitude() const { return m_attitude; }
	inline bool const & test_object() const { return m_test_object; }

	void set_timestamp(csp::uint32 const &value) {
		m_timestamp = value;
		m_has0 |= 0x00000001;
	}

	void set_position(csp::Vector3 const &value) {
		m_position = value;
		m_has0 |= 0x00000002;
	}

	void set_velocity(csp::Vector3 const &value) {
		m_velocity = value;
		m_has0 |= 0x00000004;
	}

	void set_attitude(csp::Quat const &value) {
		m_attitude = value;
		m_has0 |= 0x00000008;
	}

	void set_test_object(bool const &value) {
		m_test_object = value;
		m_has0 |= 0x00000010;
	}

	TestUnitState() :
		m_has0(0),
		m_timestamp(),
		m_position(),
		m_velocity(),
		m_attitude(),
		m_test_object()
	{ }

	void serialize(csp::TagWriter &writer) const;
	void serialize(csp::TagReader &reader);

	void dump(std::ostream &, int) const;
	void dump(csp::DumpWriter &out, const char *name=0) const;

	typedef csp::Ref<TestUnitState> Ref;
	virtual Id getId() const { return _getId(); }
	static const Id _Id = CSP_ULL(7438168793173873140);
	static inline Id _getId() { return _Id; }
	virtual int getCustomId() const { return _getCustomId(); }
	static inline int _getCustomId() { return m_CustomId; }
	static inline void _setCustomId(int id) { m_CustomId = id; }
	virtual int getVersion() const { return _getVersion(); }
	static inline int _getVersion() { return 0; }
	virtual std::string getName() const { return _getName(); }
	static inline std::string _getName() { return "TestUnitState"; }
};

#endif
