#ifndef __TYPESNETWORK_H_
#define __TYPESNETWORK_H_

//#include "Framework.h"

// Available Preprocessors for Network
// 

/*!
  \def MAX_DGRAM_SIZE
  Is now not used, but later a checking of packetsize should maybe implemented.
*/
#define MAX_DGRAM_SIZE 512

/*!
  \def NETWORK_RET_ERROR
  Is used as return value of methods which used for network specific stuff. 
  It defines that a error has occured while executing the method.
*/
#define NETWORK_RET_ERROR   -1
/*!
  \def NETWORK_RET_OK
  Is used as return value of methods which used for network specific stuff. 
  It defines that during the method execution no error has occured.
*/
#define NETWORK_RET_OK      0
/*!
  \def NETWORK_RET_TIMEOUT
  Is used as return value of methods which used for network specific stuff. 
  It defines for methods with blocking that the blocking time is expired without event.
  For example on ReceiveDatagram it means that no packet was received within the time given for.
*/
#define NETWORK_RET_TIMEOUT	1

/*!
  \def NETWORK_TCPIP
  Is used as synonym for the TCP/IP-protocol (streaming-oriented). 
*/
#define NETWORK_TCPIP		1

/*!
  \def NETWORK_UDPIP
  Is used as synonym for the UDP/IP-protocol (datagram-oriented). 
*/
#define NETWORK_UDPIP		2



//! System-independent network implementation.
/*! \class StandardNetwork
 *  This class should be the lowest(sys-independent) netcode level.
 *	TCP/IP and UDP/IP are the possible protocols:
 *	- Using TCP/IP:<br>To use network for TCP/IP it is important to distinguish between client and server. 
 *	The server uses WindowsNetwork::Create to establish a socket connected to local address and defined port.
 *	Next it uses StandardNetwork::Listen to inform the system that it await connect requests from clients. 
 *	Now this network is only for receiving connection requests and build up new connections.  
 *	With StandardNetwork::Accept the server is checking for this requests It must do this periodicaly.
 *	If a client want to connect, the server receives that on StandardNetwork::Accept an build up an new connection.
 *	This new connection will used to communicate with this special client.
 *	The client uses StandardNetwork::Create to establish a socket connected to local address. The port should set to 0.
 *	If port is set to 0 the system is selecting an free port. The next step is to connect with StandardNetwork::Connect to the server.
 *	After establishing these connection the client can communicate with defined server.<br>
 *	- Using UDP/IP:<br>That's more simple. At first use StandardNetwork::Create to establish a socket connected to local address.
 *	Servers must define the port (clients should use port = 0). 
 *	After that client and server could use StandardNetwork::Connect to define a default destination. So StandardNetwork::Send and StandardNetwork::Receive are usable.
 *	Without doing that, only StandardNetwork::SendTo and StandardNetwork::ReceiveFrom to receive from or send to a special destination.
 *	Without specifying the default destination StandardNetwork::Receive will receive from all destinations (I think so :)). That's usefull on servers.
 *
 *  \author Paracelsius74
 *  \version 0.1
 *  \date    12. February 2001
 *  \todo Check max_packet_size on UDP/IP.
 *	\todo Add on some methods time-depended execution. (StandardNetwork::Send, StandardNetwork::SendTo, StandardNetwork::Connect).
 */
  class StandardNetwork
  {

  private:
  public:	
	/**@name Constructor, Destructor*/
	//@{
	virtual ~StandardNetwork(){};
    virtual void Initialize() = 0;
	//@}
	/**@name Methods, which affect the connection*/
	//@{
	//! Build up a socket and bind them to a local address.  
	/*!
	\param _socketType [IN] Defines the protocol. Available  are TCP/IP (NETWORK_TCPIP) and UDP/IP (NETWORK_UDPIP).
	\param _ip [IN/OUT] Contains the local ip-address(host-byte-order) of this socket. 
	\param _port [IN/OUT] Contains the local port of this socket. For clients set this to 0 (Then system uses a available port).
	\retval NETWORK_RET_OK on a successfull execution.
	\retval NETWORK_RET_ERROR an error has occured. 
	*/
	virtual short Create(unsigned short _socketType,long &_ip,short &_port) = 0;
	//!Closes the connection. Frees ports an so on.
	/*!
	\retval NETWORK_RET_OK on a successfull execution.
	\retval NETWORK_RET_ERROR an error has occured. 
	*/
	virtual short Close(void) = 0;
	//! Informs the system, that the socket is from a server which is waiting for clients (only TCP/IP).
	/*! This socket will used after execution only for initial handshaking between client and server. 
		So this method is ever used together with StandardNetwork::Accept.
	\param _maxConnectionNr [IN] Defines the maximum number of clients, the server is looking for. The range is system-dependent (sometimes between <5).
	\retval NETWORK_RET_OK on a successfull execution.
	\retval NETWORK_RET_ERROR an error has occured. 
	*/
	virtual short Listen(short _maxConnectionNr) = 0;
	//! With this method the server looks for clients which want to contact the server (only TCP/IP).	
	/*! Before using this method StandardNetwork::Listen must be executed. The method is _mSecs microseconds waiting for an incoming connection request.
	If something is readable on socket it will be interpreted as a connection request, 
	so it's unpossible to use same socket for connection request and normal communication (i think so :).., please tell me if not).
	\param _s [OUT] Is the new connection to an client, if NETWORK_RET_OK is returned.
	\param _mSecs [IN] Time to wait for connectiong clients packet (microseconds). 
	\retval NETWORK_RET_OK on a successfull execution.
	\retval NETWORK_RET_ERROR an error has occured. 
	*/
	virtual short Accept(StandardNetwork **_s, const long _mSecs) = 0;
	//! Build up a connection to foreign address (TCP/IP) or define a default destination for StandardNetwork::Send and StandardNetwork::Receive (UDP/IP).
	/*! On UDP/IP: Any datagrams received from an address other than the destination address specified will be discarded. 
	If the ip-address is zero, the socket will be disconnected. Then no default destination is set and only StandardNetwork::SendTo and StandardNetwork::ReceiveFrom are usable.<br>
	On TCP/IP:An active connection is initiated to the foreign host using _ip(foreign host's ip-address) and _port(foreign host's port).
	When the socket call completes successfully, the socket is ready to send and receive data.
	\param _ip [IN] defines the ip-address of foreign host.
	\param _port [IN] defines the port of foreign host.
	\retval NETWORK_RET_OK on a successfull execution.
	\retval NETWORK_RET_ERROR an error has occured. 
	\todo extend it maybe with timer for blocking sockets to timeout method.
	*/
	virtual short Connect(long _ip, short _port) = 0;
	//! Sends data to the default destination. 
	/*!
	\param _packet [IN] buffer, which contains data to send.
	\param _packetSize [IN] size of data buffer.
	\retval NETWORK_RET_OK on a successfull execution.
	\retval NETWORK_RET_ERROR an error has occured. 
	\warning Data will not be splited into smaller packets, if _packetSize is larger than max. packet size.
	\todo eventually with timelimit
	\todo check eventually the packet size.
	*/
	virtual short Send(const char *_packet,const short _packetSize) = 0;
	//! Sends data to a special destination. 
	/*!
	\param _packet [IN] buffer, which contains data to send.
	\param _packetSize [IN] size of data buffer.
	\param _ip [IN] packet will send to this ip-address.
	\param _port [IN] packet will send to this port.
	\retval NETWORK_RET_OK on a successfull execution.
	\retval NETWORK_RET_ERROR an error has occured. 
	\warning Data will not be splited into smaller packets, if _packetSize is larger than max. packet size.
	\todo eventually with timelimit
	\todo check eventually the packet size.
	*/
	virtual short SendTo(char *_packet, const short _packetSize, long _ip, short _port) = 0;
	//! Receives packets. If default destination is set (StandardNetwork::Connect) it receives only packets from this address. 
	/*! It looks for incoming packets from default destination and receives that.
	\param _packet [OUT] buffer, which is used to return the received data.
	\param _packetSize [IN/OUT] size of data buffer.
	\param _mSecs [IN] Time to wait for packet (microseconds). Set _mSecs = 0 to wait till a packet will received. 
	\retval NETWORK_RET_OK on a successfull execution (packet was received).
	\retval NETWORK_RET_TIMEOUT no packet was received during the time limit.
	\retval NETWORK_RET_ERROR an error has occured. 
	*/
	virtual short Receive(char *_packet,short *_packetSize, const long _mSecs) = 0; 
	//! Receives packets from special address.
	/*! It looks for incoming packets and receives that.
	\param _packet [OUT] buffer, which is used to return the received data.
	\param _packetSize [IN/OUT] size of data buffer.
	\param _ip [IN] ip-address from which packets will be received.
	\param _port [IN] port from which packets will be received.
	\param _mSecs [IN] Time to wait for packet (microseconds). Set _mSecs = 0 to wait till a packet will received. 
	\retval NETWORK_RET_OK on a successfull execution (packet was received).
	\retval NETWORK_RET_TIMEOUT no packet was received during the time limit.
	\retval NETWORK_RET_ERROR an error has occured. 
	*/
	virtual short ReceiveFrom(char *_packet,short *_packetSize, long _ip, short _port, const long _mSecs) = 0;
	//@}
	/**@name Methods, which supplies information*/
	//@{
	//!Method retrieves the address of the peer connected to.
	/*!This method can be used only on a connected network. 
	For UDP/IP, only the address of a peer specified in a previous StandardNetwork::Connect call will be returned.
	Any address specified by a previous StandardNetwork::SendTo call will not be returned by this method.
	\param _ip [OUT] ip-address of a peer.
	\param _port [OUT] port of a peer.
	\retval NETWORK_RET_OK on a successfull execution.
	\retval NETWORK_RET_ERROR an error has occured. 
	*/
	virtual short GetPeerAddress(long &_ip,short &_port) = 0;
	//! This method retrieves the current address for network object.
	/*!It is used on the bound(UDP/IP) or connected(TCP/IP) network. 
	The local association is returned. 
	The getsockname function does not always return information about the host address when the network has been bound to an unspecified address. 
	\param _ip [OUT] local ip-address of the socket.
	\param _port [OUT] local port of the socket.
	\retval NETWORK_RET_OK on a successfull execution.
	\retval NETWORK_RET_ERROR an error has occured. 
	*/
	virtual short GetSockAddress(long &_ip,short &_port) = 0;
	//!Get informations about the host defined by name.
	/*!
	\param _name [IN] A pointer to the null-terminated name of the host to resolve.
	\return A struct that contains host informations.
	\warning Do not free struct hostent returned from method on caller!
	*/
	//virtual struct hostent *GetHostByName(const char *_name) = 0;
	//!Get informations about the host defined by address.
	/*!
	\param _addr [IN] A struct that contains the address of the host to resolve.
	\return A struct that contains host informations.
	\warning Do not free struct hostent returned from method on caller!
	*/
	//virtual struct hostent *GetHostByAddress(struct sockaddr_in &_addr);
	//@}
  };

#endif
