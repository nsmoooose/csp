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

  void dump()
  {
	  printf("MessageHeader - MagicNumber: %u\n", m_magicNumber);
	  printf("MessageHeader - PayloadLen: %u\n", m_payloadLen);
	  printf("MessageHeader - MessageType: %u\n", m_messageType);
	  printf("MessageHeader - ipaddr: %u\n", m_ipaddr);
	  printf("MessageHeader - port: %u\n", m_port);
	  printf("MessageHeader - id: %u\n", m_id);
  }	 

  void dumpOffsets()
  {
	simdata::uint32 thisAddr = (simdata::uint32)this;
	simdata::uint32 magicNumberAddr = (simdata::uint32)&m_magicNumber;
	simdata::uint32 PayloadLenAddr = (simdata::uint32)&m_payloadLen;
	simdata::uint32 MessageTypeAddr = (simdata::uint32)&m_messageType;
	simdata::uint32 ipaddrAddr = (simdata::uint32)&m_ipaddr;
	simdata::uint32 portAddr = (simdata::uint32)&m_port;
	simdata::uint32 idAddr = (simdata::uint32)&m_id;

	printf("MessageHeader - MagicNumberOffset: %d\n", magicNumberAddr - thisAddr );
	printf("MessageHeader - PayloadLenOffset: %d\n", PayloadLenAddr - thisAddr );
	printf("MessageHeader - MessageTypeOffset: %d\n", MessageTypeAddr - thisAddr );
	printf("MessageHeader - ipOffset: %d\n", ipaddrAddr - thisAddr );
	printf("MessageHeader - portOffset: %d\n", portAddr - thisAddr );
	printf("MessageHeader - idOffset: %d\n", idAddr - thisAddr );
  }
		  
};

class NetworkMessage
{
   
    protected: 
    MessageHeader m_header;
    simdata::uint8 m_payloadBuf[];
    
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

