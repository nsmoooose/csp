#ifndef __MESSAGE_HEADER_H__
#define __MESSAGE_HEADER_H__

struct MessageHeader
{
	simdata::uint16  m_magicNumber;
	simdata::uint16  m_payloadLen;
	simdata::uint16  m_messageType;
	simdata::uint32  m_ipaddr;
	simdata::uint16  m_port;
	simdata::uint16  m_id;

	void dump() {
		CSP_LOG(NETWORK, DEBUG, "MessageHeader - MagicNumber: " << m_magicNumber);
		CSP_LOG(NETWORK, DEBUG, "MessageHeader - PayloadLen: " << m_payloadLen);
		CSP_LOG(NETWORK, DEBUG, "MessageHeader - MessageType: " << m_messageType);
		CSP_LOG(NETWORK, DEBUG, "MessageHeader - ipaddr: " << m_ipaddr);
		CSP_LOG(NETWORK, DEBUG, "MessageHeader - port: " << m_port);
		CSP_LOG(NETWORK, DEBUG, "MessageHeader - id: " << m_id);
	}

	void dumpOffsets() {
		simdata::uint32 thisAddr = (simdata::uint32)this;
		simdata::uint32 magicNumberAddr = (simdata::uint32)&m_magicNumber;
		simdata::uint32 PayloadLenAddr = (simdata::uint32)&m_payloadLen;
		simdata::uint32 MessageTypeAddr = (simdata::uint32)&m_messageType;
		simdata::uint32 ipaddrAddr = (simdata::uint32)&m_ipaddr;
		simdata::uint32 portAddr = (simdata::uint32)&m_port;
		simdata::uint32 idAddr = (simdata::uint32)&m_id;

		CSP_LOG(NETWORK, DEBUG, "MessageHeader - MagicNumberOffset: " << (magicNumberAddr - thisAddr));
		CSP_LOG(NETWORK, DEBUG, "MessageHeader - PayloadLenOffset: " << (PayloadLenAddr - thisAddr));
		CSP_LOG(NETWORK, DEBUG, "MessageHeader - MessageTypeOffset: " << (MessageTypeAddr - thisAddr));
		CSP_LOG(NETWORK, DEBUG, "MessageHeader - ipOffset: " << (ipaddrAddr - thisAddr));
		CSP_LOG(NETWORK, DEBUG, "MessageHeader - portOffset: " << (portAddr - thisAddr));
		CSP_LOG(NETWORK, DEBUG, "MessageHeader - idOffset: " << (idAddr - thisAddr));
	}

};

class NetworkMessage
{
	friend class silence_gcc_warnings;

	protected:
	MessageHeader m_header;
	simdata::uint8 m_payloadBuf[512];

	static unsigned short m_magicNumber;
	static unsigned short m_HeaderLen;

	private:

	NetworkMessage();
	NetworkMessage(NetworkMessage &);

	public:

	bool initialize(simdata::uint16 type, simdata::uint16 payloadLength, NetworkNode * senderNode);
	void dumpMessageHeader();

	simdata::uint16 getType();

	void * getPayloadPtr();
	simdata::uint16 getPayloadLen();

	Port getOriginatorPort();
	NetworkNode * getOriginatorNode();

	bool isHeaderValid();

};

#endif

