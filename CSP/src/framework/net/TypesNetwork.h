#include "standard/StandardFramework.h"

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

// list of APIS
const short _NONETWORK      = -1;
const short _NETWORK		= -2;

// if none then default to nothing.
#ifndef NETWORK_HAS_A_DEFAULT
  const short DEFAULT_NETWORK = _NONETWORK;
#endif

// common structs
struct _NETWORK_DESCRIPTION
{
  char   *Name;
  short   ID;
};
