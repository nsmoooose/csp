#ifndef _WindowsNetwork__H_
#define _WindowsNetwork__H_

#include <winsock2.h>
#include <stdio.h>
#include "../StandardNetwork.h"

//! System-dependent(win32) Network implementation.
/*! \class WindowsNetwork
 *  This class should be the lowest netcode level. 
 *  It is system dependent to Win32. On the top of this class (StandardNetwork) the netcode is independent from system. 
 *	TCP/IP and UDP/IP are the possible protocols:
 *	- Using TCP/IP:<br>To use network for TCP/IP it is important to distinguish between client and server. 
 *	The server uses WindowsNetwork::Create to establish a socket connected to local address and defined port.
 *	Next it uses WindowsNetwork::Listen to inform the system that it await connect requests from clients. 
 *	Now this network is only for receiving connection requests and build up new connections.  
 *	With WindowsNetwork::Accept the server is checking for this requests It must do this periodicaly.
 *	If a client want to connect, the server receives that on WindowsNetwork::Accept an build up an new connection.
 *	This new connection will used to communicate with this special client.
 *	The client uses WindowsNetwork::Create to establish a socket connected to local address. The port should set to 0.
 *	If port is set to 0 the system is selecting an free port. The next step is to connect with WindowsNetwork::Connect to the server.
 *	After establishing these connection the client can communicate with defined server.<br>
 *	- Using UDP/IP:<br>That's more simple. At first use WindowsNetwork::Create to establish a socket connected to local address.
 *	Servers must define the port (clients should use port = 0). 
 *	After that client and server could use WindowsNetwork::Connect to define a default destination. So WindowsNetwork::Send and WindowsNetwork::Receive are usable.
 *	Without doing that, only WindowsNetwork::SendTo and WindowsNetwork::ReceiveFrom to receive from or send to a special destination.
 *	Without specifying the default destination WindowsNetwork::Receive will receive from all destinations (I think so :)). That's usefull on servers.
 *
 *  \author Paracelsius74
 *  \version 0.1
 *  \date    12. February 2001
 *  \attention It is necessary to link "ws2_32.lib".
 *  \todo Check max_packet_size on UDP/IP.
 *	\todo Add on some methods time-depended execution. (WindowsNetwork::Send, WindowsNetwork::SendTo, WindowsNetwork::Connect).
 */
class WindowsNetwork:public StandardNetwork{
public:
	/**@name Init-Methods*/
	//@{
    void  SetSetup(void *setup);
    short GetDependencyCount();
    void  GetDependencyDescription(short Index, _DEPENDENCY *Dependency);
    short SetDependencyData(short Index, void* Data);
    void  GetSetup(void*setup);
    short CheckStatus();
	//@}
	/**@name Constructor, Destructor*/
	//@{
	//! A constructor.
    /*!
      It initializes the pointer.
	  \param _initializedSocket [IN] Will be needed from Accept. The normal usage is without using this parameter.
    */
	WindowsNetwork(SOCKET _initializedSocket = INVALID_SOCKET);
    //! A destructor.
    /*!
      It destroys the socket. It calls WindowsNetwork::Close() for that.
    */
	~WindowsNetwork(void);
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
	short Create(unsigned short _socketType,long &_ip, short &_port);// = SOCK_DGRAM
	//!Closes the connection. Frees ports an so on.
	/*!
	\retval NETWORK_RET_OK on a successfull execution.
	\retval NETWORK_RET_ERROR an error has occured. 
	*/
	short Close(void);
	//! Informs the system, that the socket is from a server which is waiting for clients (only TCP/IP).
	/*! This socket will used after execution only for initial handshaking between client and server. 
		So this method is ever used together with WindowsNetwork::Accept.
	\param _maxConnectionNr [IN] Defines the maximum number of clients, the server is looking for. The range is system-dependent (mostly between 1..5)
	\retval NETWORK_RET_OK on a successfull execution.
	\retval NETWORK_RET_ERROR an error has occured. 
	*/
	short Listen(short _maxConnectionNr);
	//! With this method the server looks for clients which want to contact the server (only TCP/IP).	
	/*! Before using this method WindowsNetwork_Listen must be executed. The method is _mSecs microseconds waiting for an incoming connection request.
	If something is readable on socket it will be interpreted as a connection request, 
	so it's unpossible to use same socket for connection request and normal communication (i think so :).., tell if not).
	\param _s [OUT] Is the new connection to an client, if NETWORK_RET_OK is returned.
	\param _mSecs [IN] Time to wait for connectiong clients packet (microseconds).
	\retval NETWORK_RET_OK on a successfull execution.
	\retval NETWORK_RET_ERROR an error has occured. 
	*/
	short Accept(StandardNetwork **_s, const long _mSecs);
	//! Build up a connection to foreign address (TCP/IP) or define a default destination for StandardNetwork::Send and WindowsNetwork::Receive (UDP/IP).
	/*! On UDP/IP: Any datagrams received from an address other than the destination address specified will be discarded. 
	If the ip-address is zero, the socket will be disconnected. Then no default destination is set and only WindowsNetwork::SendTo and WindowsNetwork::ReceiveFrom are usable.<br>
	On TCP/IP:An active connection is initiated to the foreign host using _ip(foreign host's ip-address) and _port(foreign host's port).
	When the socket call completes successfully, the socket is ready to send and receive data.
	\param _ip [IN] defines the ip-address of foreign host.
	\param _port [IN] defines the port of foreign host.
	\retval NETWORK_RET_OK on a successfull execution.
	\retval NETWORK_RET_ERROR an error has occured. 
	\todo extend it maybe with timer for blocking sockets to timeout method.
	*/
	short Connect(long _ip, short _port);
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
	short Send(const char *_packet,const short _packetSize);
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
	short SendTo(char *_packet, const short _packetSize, long _ip, short _port);
	//! Receives packets. If default destination is set (WindowsNetwork::Connect) it receives only packets from this address. 
	/*! It looks for incoming packets from default destination and receives that.
	\param _packet [OUT] buffer, which is used to return the received data.
	\param _packetSize [IN/OUT] size of data buffer.
	\param _mSecs [IN] Time to wait for packet (microseconds). Set _mSecs = 0 to wait till a packet will received. 
	\retval NETWORK_RET_OK on a successfull execution (packet was received).
	\retval NETWORK_RET_TIMEOUT no packet was received during the time limit.
	\retval NETWORK_RET_ERROR an error has occured. 
	*/
	short Receive(char *_packet,short *_packetSize, const long _mSecs); 
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
	short ReceiveFrom(char *_packet,short *_packetSize, long _ip, short _port,const long _mSecs);
	//@}
	/**@name Methods, which supplies information*/
	//@{
	//!Method retrieves the address of the peer connected to.
	/*!This method can be used only on a connected network. 
	For UDP/IP, only the address of a peer specified in a previous WindowsNetwork::Connect call will be returned.
	Any address specified by a previous WindowsNetwork::SendTo call will not be returned by this method.
	\param _ip [OUT] ip-address of a peer.
	\param _port [OUT] port of a peer.
	\retval NETWORK_RET_OK on a successfull execution.
	\retval NETWORK_RET_ERROR an error has occured. 
	*/
	short GetPeerAddress(long &_ip,short &_port);
	//! This method retrieves the current address for network object.
	/*!It is used on the bound(UDP/IP) or connected(TCP/IP) network. 
	The local association is returned. 
	The getsockname function does not always return information about the host address when the network has been bound to an unspecified address. 
	\param _ip [OUT] local ip-address of the socket.
	\param _port [OUT] local port of the socket.
	\retval NETWORK_RET_OK on a successfull execution.
	\retval NETWORK_RET_ERROR an error has occured. 
	*/
	short GetSockAddress(long &_ip,short &_port);
	//!Get informations about the host defined by name.
	/*!
	\param _name [IN] A pointer to the null-terminated name of the host to resolve.
	\return A struct that contains host informations.
	\warning Not usable now. Do not free struct hostent returned from method on caller!
	\todo must be overworked.
	*/
	//struct hostent *GetHostByName(const char *_name);
	//!Get informations about the host defined by address.
	/*!
	\param _addr [IN] A struct that contains the address of the host to resolve.
	\return A struct that contains host informations.
	\warning Not usable now. Do not free struct hostent returned from method on caller!
	\todo must be overworked.
	*/
	//struct hostent *GetHostByAddress(struct sockaddr_in &_addr);
	//@}
protected:
	//! If on WSA socket method occured an error this method will check the kind of error and writes a small message into the file.
	/*!
	\param _fp [IN] filepointer for output.
	\param _string [IN] pointer to string (list of char terminated by 0) with infos like current method name. 
	\return The error number will be returned.
	*/
	static short CheckWSAError(FILE *_fp,char *_string = NULL);
	//! Writes the host data into a file (readable output).
	/*!
	\param fp [IN] filepointer for output.
	\param h [IN] the srtuct with host data. 
	*/
	static void PrintHostData(FILE *fp,struct hostent *h);
	//! Desciptor to socket.
	SOCKET				m_sock;
	// IP address in host-byte-order
	long				m_ip;
	// port-number in host-byte-order
	short				m_port;	
	// local address all in network-byte-order	
	struct sockaddr_in	m_localAddress;
	// local address all in network-byte-order	
	struct sockaddr_in	m_connectAddress;
};

#endif