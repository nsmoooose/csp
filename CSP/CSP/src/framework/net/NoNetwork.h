// NoNetwork specific for StandardNetwork

#include "StandardNetwork.h"

//! This class is a derivation from StandardNetwork with no functionality.
/*! \class NoNetworkClass
  The only use is for FactoryNetwork to return no network.
  \author Paracelsius74
  $Author: brandonb $
  \version 0.1, $Revision: 1.1 $
  \date    12. February 2001 (last change: $Date: 2003/05/31 13:54:38 $)
*/

class NoNetworkClass: public StandardNetwork
{

private:


public:
	/**@name Init-Methods*/
	//@{
	// Standard Framework - required for every framework object
	void  SetSetup(void *setup);
	short GetDependencyCount();
	void  GetDependencyDescription(short Index, _DEPENDENCY *Dependency);
	short SetDependencyData(short Index, void* Data);
	void  GetSetup(void *setup);
	short CheckStatus();
	//@}
	/**@name Constructor, Destructor*/
	//@{
	NoNetworkClass();
	~NoNetworkClass();
	//@}
	/**@name Methods, which affect the connection*/
	//@{
	//! This method has no functionality and returns NETWORK_RET_ERROR.
	/*!
	\param _socketType [IN] Defines the protocol. Available  are TCP/IP (NETWORK_TCPIP) and UDP/IP (NETWORK_UDPIP).
	\param _ip [IN/OUT] Contains the local ip-address(host-byte-order) of this socket. 
	\param _port [IN/OUT] Contains the local port of this socket. For clients set this to 0 (Then system uses a available port).
	\retval NETWORK_RET_ERROR ever. 
	*/
	short Create(unsigned short _socketType,long &_ip, short &_port){return NETWORK_RET_ERROR;};
	//! This method has no functionality and returns NETWORK_RET_ERROR.
	/*!
	\retval NETWORK_RET_ERROR ever. 
	*/
	short Close(void){return NETWORK_RET_ERROR;};
	//! This method has no functionality and returns NETWORK_RET_ERROR.
	/*! 
	\param _maxConnectionNr [IN] Defines the maximum number of clients, the server is looking for. The range is system-dependent (sometimes between <5).
	\retval NETWORK_RET_ERROR ever. 
	*/
	short Listen(short _maxConnectionNr){return NETWORK_RET_ERROR;};
	//! This method has no functionality and returns NETWORK_RET_ERROR.
	/*! 
	\param _s [OUT] Is the new connection to an client, if NETWORK_RET_OK is returned.
	\param _mSecs [IN] Time to wait for connectiong clients packet (microseconds). 
	\retval NETWORK_RET_ERROR ever. 
	*/
	short Accept(StandardNetwork **_s, const long _mSecs){return NETWORK_RET_ERROR;};
	//! This method has no functionality and returns NETWORK_RET_ERROR.
	/*! 
	\param _ip [IN] defines the ip-address of foreign host.
	\param _port [IN] defines the port of foreign host.
	\retval NETWORK_RET_ERROR ever. 
	*/
	short Connect(long _ip, short _port){return NETWORK_RET_ERROR;};
	//! This method has no functionality and returns NETWORK_RET_ERROR.
	/*!
	\param _packet [IN] buffer, which contains data to send.
	\param _packetSize [IN] size of data buffer.
	\retval NETWORK_RET_ERROR ever. 
	*/
	short Send(const char *_packet,const short _packetSize){return NETWORK_RET_ERROR;};
	//! This method has no functionality and returns NETWORK_RET_ERROR.
	/*!
	\param _packet [IN] buffer, which contains data to send.
	\param _packetSize [IN] size of data buffer.
	\param _ip [IN] packet will send to this ip-address.
	\param _port [IN] packet will send to this port.
	\retval NETWORK_RET_ERROR ever. 
	*/
	short SendTo(char *_packet, const short _packetSize, long _ip, short _port){return NETWORK_RET_ERROR;};
	//! This method has no functionality and returns NETWORK_RET_ERROR.
	/*! 
	\param _packet [OUT] buffer, which is used to return the received data.
	\param _packetSize [IN/OUT] size of data buffer.
	\param _mSecs [IN] Time to wait for packet (microseconds). Set _mSecs = 0 to wait till a packet will received. 
	\retval NETWORK_RET_ERROR ever. 
	*/
	short Receive(char *_packet,short *_packetSize, const long _mSecs){return NETWORK_RET_ERROR;}; 
	//! This method has no functionality and returns NETWORK_RET_ERROR.
	/*! 
	\param _packet [OUT] buffer, which is used to return the received data.
	\param _packetSize [IN/OUT] size of data buffer.
	\param _ip [IN] ip-address from which packets will be received.
	\param _port [IN] port from which packets will be received.
	\param _mSecs [IN] Time to wait for packet (microseconds). Set _mSecs = 0 to wait till a packet will received. 
	\retval NETWORK_RET_ERROR ever. 
	*/
	short ReceiveFrom(char *_packet,short *_packetSize, long _ip, short _port,const long _mSecs){return NETWORK_RET_ERROR;};
	//@}
	/**@name Methods, which supplies information*/
	//@{
	//! This method has no functionality and returns NETWORK_RET_ERROR.
	/*!
	\param _ip [OUT] ip-address of a peer.
	\param _port [OUT] port of a peer.
	\retval NETWORK_RET_ERROR ever. 
	*/
	short GetPeerAddress(long &_ip,short &_port){return NETWORK_RET_ERROR;};
	//! This method has no functionality and returns NETWORK_RET_ERROR.
	/*!
	\param _ip [OUT] local ip-address of the socket.
	\param _port [OUT] local port of the socket.
	\retval NETWORK_RET_ERROR ever. 
	*/
	short GetSockAddress(long &_ip,short &_port){return NETWORK_RET_ERROR;};
	//! This method has no functionality and returns NETWORK_RET_ERROR.
	/*!
	\param _name [IN] A pointer to the null-terminated name of the host to resolve.
	\return A struct that contains host informations.
	\warning Do not free struct hostent returned from method on caller!
	*/
	// static struct hostent *GetHostByName(const char *_name){return NULL;};
	//! This method has no functionality and returns NETWORK_RET_ERROR.
	/*!
	\param _addr [IN] A struct that contains the address of the host to resolve.
	\return A struct that contains host informations.
	\warning Do not free struct hostent returned from method on caller!
	*/
	// static struct hostent *GetHostByAddress(struct sockaddr_in &_addr){return NULL;};
	//@}
};
