// Generated on Sun Jun 20 22:28:23 2010
// This is a generated file --- DO NOT EDIT!

#ifndef __CSP_CSPLIB_NET_CLIENTSERVERMESSAGES_H__
#define __CSP_CSPLIB_NET_CLIENTSERVERMESSAGES_H__

#include <csp/csplib/net/NetworkMessage.h>


class Reserved0 : public csp::NetworkMessage {
	static int m_CustomId;
public:

	Reserved0() { }

	void serialize(csp::TagWriter &writer) const;
	void serialize(csp::TagReader &reader);

	void dump(std::ostream &, int) const;
	void dump(csp::DumpWriter &out, const char *name=0) const;

	typedef csp::Ref<Reserved0> Ref;
	virtual Id getId() const { return _getId(); }
	static const Id _Id = CSP_ULL(593456734086862890);
	static inline Id _getId() { return _Id; }
	virtual int getCustomId() const { return _getCustomId(); }
	static inline int _getCustomId() { return m_CustomId; }
	static inline void _setCustomId(int id) { m_CustomId = id; }
	virtual int getVersion() const { return _getVersion(); }
	static inline int _getVersion() { return 0; }
	virtual std::string getName() const { return _getName(); }
	static inline std::string _getName() { return "Reserved0"; }
};


class Reserved1 : public csp::NetworkMessage {
	static int m_CustomId;
public:

	Reserved1() { }

	void serialize(csp::TagWriter &writer) const;
	void serialize(csp::TagReader &reader);

	void dump(std::ostream &, int) const;
	void dump(csp::DumpWriter &out, const char *name=0) const;

	typedef csp::Ref<Reserved1> Ref;
	virtual Id getId() const { return _getId(); }
	static const Id _Id = CSP_ULL(14597302889048327985);
	static inline Id _getId() { return _Id; }
	virtual int getCustomId() const { return _getCustomId(); }
	static inline int _getCustomId() { return m_CustomId; }
	static inline void _setCustomId(int id) { m_CustomId = id; }
	virtual int getVersion() const { return _getVersion(); }
	static inline int _getVersion() { return 0; }
	virtual std::string getName() const { return _getName(); }
	static inline std::string _getName() { return "Reserved1"; }
};


class Acknowledge : public csp::NetworkMessage {
	static int m_CustomId;
public:

	Acknowledge() { }

	void serialize(csp::TagWriter &writer) const;
	void serialize(csp::TagReader &reader);

	void dump(std::ostream &, int) const;
	void dump(csp::DumpWriter &out, const char *name=0) const;

	typedef csp::Ref<Acknowledge> Ref;
	virtual Id getId() const { return _getId(); }
	static const Id _Id = CSP_ULL(11661513110747016705);
	static inline Id _getId() { return _Id; }
	virtual int getCustomId() const { return _getCustomId(); }
	static inline int _getCustomId() { return m_CustomId; }
	static inline void _setCustomId(int id) { m_CustomId = id; }
	virtual int getVersion() const { return _getVersion(); }
	static inline int _getVersion() { return 0; }
	virtual std::string getName() const { return _getName(); }
	static inline std::string _getName() { return "Acknowledge"; }
};


class Disconnect : public csp::NetworkMessage {
	static int m_CustomId;
public:

	Disconnect() { }

	void serialize(csp::TagWriter &writer) const;
	void serialize(csp::TagReader &reader);

	void dump(std::ostream &, int) const;
	void dump(csp::DumpWriter &out, const char *name=0) const;

	typedef csp::Ref<Disconnect> Ref;
	virtual Id getId() const { return _getId(); }
	static const Id _Id = CSP_ULL(16275836784021579780);
	static inline Id _getId() { return _Id; }
	virtual int getCustomId() const { return _getCustomId(); }
	static inline int _getCustomId() { return m_CustomId; }
	static inline void _setCustomId(int id) { m_CustomId = id; }
	virtual int getVersion() const { return _getVersion(); }
	static inline int _getVersion() { return 0; }
	virtual std::string getName() const { return _getName(); }
	static inline std::string _getName() { return "Disconnect"; }
};


class Reset : public csp::NetworkMessage {
	static int m_CustomId;
public:

	Reset() { }

	void serialize(csp::TagWriter &writer) const;
	void serialize(csp::TagReader &reader);

	void dump(std::ostream &, int) const;
	void dump(csp::DumpWriter &out, const char *name=0) const;

	typedef csp::Ref<Reset> Ref;
	virtual Id getId() const { return _getId(); }
	static const Id _Id = CSP_ULL(17414570450226710756);
	static inline Id _getId() { return _Id; }
	virtual int getCustomId() const { return _getCustomId(); }
	static inline int _getCustomId() { return m_CustomId; }
	static inline void _setCustomId(int id) { m_CustomId = id; }
	virtual int getVersion() const { return _getVersion(); }
	static inline int _getVersion() { return 0; }
	virtual std::string getName() const { return _getName(); }
	static inline std::string _getName() { return "Reset"; }
};


class ConnectionRequest : public csp::NetworkMessage {
	static int m_CustomId;
	enum {
		TAG_password = 8,
		TAG_incoming_bw = 9,
		TAG_outgoing_bw = 10
	};

	csp::uint32 m_has0;
	
	std::string m_password;
	csp::int32 m_incoming_bw;
	csp::int32 m_outgoing_bw;
public:

	inline bool has_password() const { return (m_has0 & 0x00000001) != 0; }
	inline bool has_incoming_bw() const { return (m_has0 & 0x00000002) != 0; }
	inline bool has_outgoing_bw() const { return (m_has0 & 0x00000004) != 0; }

	inline void clear_password() {
		m_has0 &= ~0x00000001;
	}

	inline void clear_incoming_bw() {
		m_has0 &= ~0x00000002;
	}

	inline void clear_outgoing_bw() {
		m_has0 &= ~0x00000004;
	}

	inline std::string const & password() const { return m_password; }
	inline csp::int32 const & incoming_bw() const { return m_incoming_bw; }
	inline csp::int32 const & outgoing_bw() const { return m_outgoing_bw; }

	void set_password(std::string const &value) {
		m_password = value;
		m_has0 |= 0x00000001;
	}

	void set_incoming_bw(csp::int32 const &value) {
		m_incoming_bw = value;
		m_has0 |= 0x00000002;
	}

	void set_outgoing_bw(csp::int32 const &value) {
		m_outgoing_bw = value;
		m_has0 |= 0x00000004;
	}

	ConnectionRequest() :
		m_has0(0),
		m_password(),
		m_incoming_bw(),
		m_outgoing_bw()
	{ }

	void serialize(csp::TagWriter &writer) const;
	void serialize(csp::TagReader &reader);

	void dump(std::ostream &, int) const;
	void dump(csp::DumpWriter &out, const char *name=0) const;

	typedef csp::Ref<ConnectionRequest> Ref;
	virtual Id getId() const { return _getId(); }
	static const Id _Id = CSP_ULL(18445457390429651226);
	static inline Id _getId() { return _Id; }
	virtual int getCustomId() const { return _getCustomId(); }
	static inline int _getCustomId() { return m_CustomId; }
	static inline void _setCustomId(int id) { m_CustomId = id; }
	virtual int getVersion() const { return _getVersion(); }
	static inline int _getVersion() { return 0; }
	virtual std::string getName() const { return _getName(); }
	static inline std::string _getName() { return "ConnectionRequest"; }
};


class ConnectionResponse : public csp::NetworkMessage {
	static int m_CustomId;
	enum {
		TAG_server_id = 8,
		TAG_response = 9,
		TAG_success = 10,
		TAG_client_id = 11,
		TAG_incoming_bw = 12,
		TAG_outgoing_bw = 13
	};

	csp::uint32 m_has0;
	
	std::string m_server_id;
	std::string m_response;
	bool m_success;
	csp::int16 m_client_id;
	csp::int32 m_incoming_bw;
	csp::int32 m_outgoing_bw;
public:

	inline bool has_server_id() const { return (m_has0 & 0x00000001) != 0; }
	inline bool has_response() const { return (m_has0 & 0x00000002) != 0; }
	inline bool has_success() const { return (m_has0 & 0x00000004) != 0; }
	inline bool has_client_id() const { return (m_has0 & 0x00000008) != 0; }
	inline bool has_incoming_bw() const { return (m_has0 & 0x00000010) != 0; }
	inline bool has_outgoing_bw() const { return (m_has0 & 0x00000020) != 0; }

	inline void clear_server_id() {
		m_has0 &= ~0x00000001;
	}

	inline void clear_response() {
		m_has0 &= ~0x00000002;
	}

	inline void clear_success() {
		m_has0 &= ~0x00000004;
	}

	inline void clear_client_id() {
		m_has0 &= ~0x00000008;
	}

	inline void clear_incoming_bw() {
		m_has0 &= ~0x00000010;
	}

	inline void clear_outgoing_bw() {
		m_has0 &= ~0x00000020;
	}

	inline std::string const & server_id() const { return m_server_id; }
	inline std::string const & response() const { return m_response; }
	inline bool const & success() const { return m_success; }
	inline csp::int16 const & client_id() const { return m_client_id; }
	inline csp::int32 const & incoming_bw() const { return m_incoming_bw; }
	inline csp::int32 const & outgoing_bw() const { return m_outgoing_bw; }

	void set_server_id(std::string const &value) {
		m_server_id = value;
		m_has0 |= 0x00000001;
	}

	void set_response(std::string const &value) {
		m_response = value;
		m_has0 |= 0x00000002;
	}

	void set_success(bool const &value) {
		m_success = value;
		m_has0 |= 0x00000004;
	}

	void set_client_id(csp::int16 const &value) {
		m_client_id = value;
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

	ConnectionResponse() :
		m_has0(0),
		m_server_id(),
		m_response(),
		m_success(),
		m_client_id(),
		m_incoming_bw(),
		m_outgoing_bw()
	{ }

	void serialize(csp::TagWriter &writer) const;
	void serialize(csp::TagReader &reader);

	void dump(std::ostream &, int) const;
	void dump(csp::DumpWriter &out, const char *name=0) const;

	typedef csp::Ref<ConnectionResponse> Ref;
	virtual Id getId() const { return _getId(); }
	static const Id _Id = CSP_ULL(2024487927582816258);
	static inline Id _getId() { return _Id; }
	virtual int getCustomId() const { return _getCustomId(); }
	static inline int _getCustomId() { return m_CustomId; }
	static inline void _setCustomId(int id) { m_CustomId = id; }
	virtual int getVersion() const { return _getVersion(); }
	static inline int _getVersion() { return 0; }
	virtual std::string getName() const { return _getName(); }
	static inline std::string _getName() { return "ConnectionResponse"; }
};

#endif
