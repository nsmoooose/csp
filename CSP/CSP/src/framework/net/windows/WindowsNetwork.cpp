#ifndef _WindowsNetwork__CPP_
#define _WindowsNetwork__CPP_

#include "WindowsNetwork.h"
#include <stdio.h>

// -------------------------------------------
// STANDARD FRAMEWORK
// -------------------------------------------
void  WindowsNetwork::SetSetup(void *setup)
{
}

short WindowsNetwork::GetDependencyCount()
{
  return 0;
}

void  WindowsNetwork::GetDependencyDescription(short Index, _DEPENDENCY *Dependency)
{
  memset(Dependency, 0, sizeof(_DEPENDENCY));
}

short WindowsNetwork::SetDependencyData(short ID, void* Data)
{
  return 0;
}

void  WindowsNetwork::GetSetup(void*setup)
{
}

short WindowsNetwork::CheckStatus()
{
  return 0;
}

/////////////////////////////////////////////////////////////////////////////////////////////////
WindowsNetwork::WindowsNetwork(SOCKET _initializedSocket)
{
	WORD wVersionRequested;
	WSADATA wsaData;
	int err;
 
	wVersionRequested = MAKEWORD( 2, 0 );
 
	err = WSAStartup( wVersionRequested, &wsaData );
	if ( err != 0 ) 
		printf("ERROR: Couldn't find a unable WinSock DLL\n");
	else 
	{
		if( LOBYTE( wsaData.wVersion ) != 2 || HIBYTE( wsaData.wVersion ) != 0 ) 
		{
			printf("ERROR: Couldn't find a unable WinSock DLL\n");
		    WSACleanup( );
		}
	}
#ifdef _DEBUG_
	printf("Found WinSock DLL\n");
#endif

	this->m_sock = _initializedSocket;
	if(_initializedSocket != INVALID_SOCKET)
	{
		long ip;
		short p;
		GetSockAddress(ip,p);
	}
}

/////////////////////////////////////////////////////////////////////////////////////////////////
WindowsNetwork::~WindowsNetwork(void)
{
	this->Close();
}

/////////////////////////////////////////////////////////////////////////////////////////////////
short WindowsNetwork::Create(unsigned short _socketType,long &_ip, short &_port)
{
	m_localAddress.sin_family = AF_INET;
	m_localAddress.sin_port = htons(_port);
	m_localAddress.sin_addr.S_un.S_addr = htonl(_ip);

	/// create socket and check for errors
	this->m_sock = socket(AF_INET,_socketType,0);

	if(this->m_sock == INVALID_SOCKET)
	{
		WindowsNetwork::CheckWSAError(stdout,"WindowsNetwork::Create(...)");
		return NETWORK_RET_ERROR;
	}
#ifdef _DEBUG_
	else
	{
		printf("Socket successfull created\n");
		fflush(stdout);
	}
#endif
	/// bind this socket to an address
	if( bind(this->m_sock,(struct sockaddr *)&m_localAddress,sizeof(m_localAddress)) == SOCKET_ERROR )
	{
		WindowsNetwork::CheckWSAError(stdout,"WindowsNetwork::Create(...)");
		return NETWORK_RET_ERROR;
	}

	// get the assigned address and (more important) the port
	short ret = this->GetSockAddress(_ip,_port);

	return ret;
}

/////////////////////////////////////////////////////////////////////////////////////////////////
short WindowsNetwork::Close(void)
{
	if(this->m_sock != INVALID_SOCKET)
	{
		if(closesocket(this->m_sock) == SOCKET_ERROR)
		{
			WindowsNetwork::CheckWSAError(stdout,"WindowsNetwork::Close(...)");
			return NETWORK_RET_ERROR;
		}
		this->m_sock = INVALID_SOCKET;
	}
	return NETWORK_RET_OK;
}

/////////////////////////////////////////////////////////////////////////////////////////////////
short WindowsNetwork::Listen(short _maxConnectionNr)
{
	if(listen(this->m_sock,_maxConnectionNr) == SOCKET_ERROR)
	{
		WindowsNetwork::CheckWSAError(stdout,"WindowsNetwork::Listen(...)");
		return NETWORK_RET_ERROR;
	}

	return NETWORK_RET_OK;
}

/////////////////////////////////////////////////////////////////////////////////////////////////
short WindowsNetwork::Accept(StandardNetwork **_s, const long _mSecs)
{

	fd_set fdSet;
	struct timeval tVal;
	struct sockaddr_in addr;
	int addrLen = sizeof(addr);
	SOCKET newSock;

	FD_ZERO(&fdSet);
	FD_SET(this->m_sock,&fdSet);

    tVal.tv_sec = 0;
    tVal.tv_usec = _mSecs;
	
	switch(select(this->m_sock+1,&fdSet,NULL,NULL,&tVal))
	{
	case 0:
		return NETWORK_RET_TIMEOUT;
	case SOCKET_ERROR:
		WindowsNetwork::CheckWSAError(stdout,"WindowsNetwork::Accept(...)");
		return NETWORK_RET_ERROR;
	default:
		newSock = accept(this->m_sock,(struct sockaddr *)&addr,&addrLen);
		if(newSock == INVALID_SOCKET)
		{
			WindowsNetwork::CheckWSAError(stdout,"WindowsNetwork::Accept(...)");
			return NETWORK_RET_ERROR;
		}
		else
		{
			*_s = new WindowsNetwork(newSock);
			return NETWORK_RET_OK;
		}
	}
}

/////////////////////////////////////////////////////////////////////////////////////////////////
short WindowsNetwork::Connect(long _ip,short _port)
{
	m_connectAddress.sin_family = AF_INET;
	m_connectAddress.sin_port = htons(_port);
	m_connectAddress.sin_addr.S_un.S_addr = htonl(_ip);

	if(connect(this->m_sock,(struct sockaddr *)&m_connectAddress,sizeof(m_connectAddress)) == SOCKET_ERROR)
	{
		m_connectAddress.sin_port = 0;
		m_connectAddress.sin_addr.S_un.S_addr = 0;
		WindowsNetwork::CheckWSAError(stdout,"WindowsNetwork::Connect(...)");
		return NETWORK_RET_ERROR;
	}

	return NETWORK_RET_OK;
}

/////////////////////////////////////////////////////////////////////////////////////////////////
short WindowsNetwork::Send(const char *_packet,const short _packetSize)
{
	if(send(this->m_sock,_packet,_packetSize,0) == SOCKET_ERROR)
	{
		WindowsNetwork::CheckWSAError(stdout,"WindowsNetwork::Send(...)");
		return NETWORK_RET_ERROR;
	}
	return NETWORK_RET_OK;
}

/////////////////////////////////////////////////////////////////////////////////////////////////
short WindowsNetwork::SendTo(char *_packet, const short _packetSize, long _ip, short _port)
{
	struct sockaddr_in addr;

	addr.sin_family = AF_INET;
	addr.sin_port = htons(_port);
	addr.sin_addr.S_un.S_addr = htonl(_ip);
	
	if(sendto(this->m_sock,_packet,_packetSize,0,(struct sockaddr *)&addr,sizeof(addr)) == SOCKET_ERROR)
	{
		WindowsNetwork::CheckWSAError(stdout,"WindowsNetwork::SendTo(...)");
		return NETWORK_RET_ERROR;
	}

	return NETWORK_RET_OK;
}

/////////////////////////////////////////////////////////////////////////////////////////////////
short WindowsNetwork::Receive(char *_packet,short *_packetSize, const long _mSecs)
{
	fd_set fdSet;
	struct timeval tVal;
	short ret = -1;

	FD_ZERO(&fdSet);
	FD_SET(this->m_sock,&fdSet);

    tVal.tv_sec = 0;
    tVal.tv_usec = _mSecs;
	
	switch(select(this->m_sock+1,&fdSet,NULL,NULL,&tVal))
	{
	case 0:
		return NETWORK_RET_TIMEOUT;
	case SOCKET_ERROR:
		WindowsNetwork::CheckWSAError(stdout,"WindowsNetwork::Receive(...)");
		return NETWORK_RET_ERROR;
	default:
		ret = recv(this->m_sock,_packet,*_packetSize,0);
		if(ret == SOCKET_ERROR)
		{
			WindowsNetwork::CheckWSAError(stdout,"WindowsNetwork::Receive(...)");
			return NETWORK_RET_ERROR;
		}
		else
		{
			*_packetSize = ret;
			if(ret == 0) return NETWORK_RET_ERROR;
			else		 return NETWORK_RET_OK;
		}
	}
}

/////////////////////////////////////////////////////////////////////////////////////////////////
short WindowsNetwork::ReceiveFrom(char *_packet,short *_packetSize, long _ip, short _port,const long _mSecs)
{
	fd_set fdSet;
	struct timeval tVal;
	short ret = -1;
	struct sockaddr_in addr;

	addr.sin_family = AF_INET;
	addr.sin_port = htons(_port);
	addr.sin_addr.S_un.S_addr = htonl(_ip);
	int len = sizeof(addr);
	

	FD_ZERO(&fdSet);
	FD_SET(this->m_sock,&fdSet);

    tVal.tv_sec = 0;
    tVal.tv_usec = _mSecs;
	
	switch(select(this->m_sock+1,&fdSet,NULL,NULL,&tVal))
	{
	case 0:
		return NETWORK_RET_TIMEOUT;
	case SOCKET_ERROR:
		WindowsNetwork::CheckWSAError(stdout,"WindowsNetwork::ReceiveFrom(...)");
		return NETWORK_RET_ERROR;
	default:
		ret = recvfrom(this->m_sock,_packet,*_packetSize,0,(struct sockaddr *)&addr,&len);
		if(ret == SOCKET_ERROR)
		{
			WindowsNetwork::CheckWSAError(stdout,"WindowsNetwork::ReceiveFrom(...)");
			return NETWORK_RET_ERROR;
		}
		else
		{
			*_packetSize = ret;
			return NETWORK_RET_OK;
		}
	}
}

/////////////////////////////////////////////////////////////////////////////////////////////////
short WindowsNetwork::GetPeerAddress(long &_ip,short &_port)
{
	struct sockaddr_in addr;
	int len = sizeof(addr);

	if(getpeername(this->m_sock,(struct sockaddr *)&addr,&len) == SOCKET_ERROR)
	{
		_ip = 0;
		_port = 0;
		WindowsNetwork::CheckWSAError(stdout,"WindowsNetwork::GetPeerAddress(...)");
		return NETWORK_RET_ERROR;
	}
	
	_ip = ntohl(addr.sin_addr.S_un.S_addr);
	_port = ntohs(addr.sin_port);	

	return NETWORK_RET_OK;
}

/////////////////////////////////////////////////////////////////////////////////////////////////
short WindowsNetwork::GetSockAddress(long &_ip,short &_port)
{
	int len = sizeof(m_localAddress);

	if(getsockname(this->m_sock,(struct sockaddr *)&m_localAddress,&len) == SOCKET_ERROR)
	{
		m_ip = 0;
		m_port = 0;
		
		_ip = 0;
		_port = 0;
		
		WindowsNetwork::CheckWSAError(stdout,"WindowsNetwork::GetSockAddress(...)");
		return NETWORK_RET_ERROR;
	}

	m_ip = ntohl(m_localAddress.sin_addr.S_un.S_addr);
	m_port = ntohs(m_localAddress.sin_port);	
	
	_ip = m_ip;
	_port = m_port;

	return NETWORK_RET_OK;
}

/////////////////////////////////////////////////////////////////////////////////////////////////
/*struct hostent *WindowsNetwork::GetHostByName(const char *_name)
{
	struct hostent *h = gethostbyname(_name);

	if(!h)
	{
		WindowsNetwork::CheckWSAError(stdout,"WindowsNetwork::GetHostByName(...)");
		return NULL;
	}
	return h;
}

/////////////////////////////////////////////////////////////////////////////////////////////////
struct hostent *WindowsNetwork::GetHostByAddress(struct sockaddr_in &_addr)
{
	struct hostent *h = gethostbyaddr((char *)&(_addr.sin_addr.S_un.S_un_b),4,0);

	if(!h)
	{
		WindowsNetwork::CheckWSAError(stdout,"WindowsNetwork::GetHostByAddress(...)");
		return NULL;
	}
	return h;
}
*/

//////////////////////////////////////////////////////////////////////
short WindowsNetwork::CheckWSAError(FILE *_fp,char *_string)
{
	short val = WSAGetLastError();
	switch(val)
	{
		case WSAEACCES:				fprintf(_fp,"ERROR (%s):Permission denied.\n",((!_string)?"":_string));break;
		case WSAEADDRINUSE:			fprintf(_fp,"ERROR (%s):Address already in use.\n",((!_string)?"":_string));break;
		case WSAEADDRNOTAVAIL:		fprintf(_fp,"ERROR (%s):Cannot assign requested address.\n",((!_string)?"":_string));break;
		case WSAEAFNOSUPPORT:		fprintf(_fp,"ERROR (%s):Address family not supported by protocol family.\n",((!_string)?"":_string));break;
		case WSAEALREADY:			fprintf(_fp,"ERROR (%s):Operation already in progress.\n",((!_string)?"":_string));break;
		case WSAECONNABORTED:		fprintf(_fp,"ERROR (%s):Software caused connection abort.\n",((!_string)?"":_string));break;
		case WSAECONNREFUSED:		fprintf(_fp,"ERROR (%s):Connection refused.\n",((!_string)?"":_string));break;
		case WSAECONNRESET:			fprintf(_fp,"ERROR (%s):Connection reset by peer.\n",((!_string)?"":_string));break;
		case WSAEDESTADDRREQ:		fprintf(_fp,"ERROR (%s):Destination address required.\n",((!_string)?"":_string));break;
		case WSAEFAULT:				fprintf(_fp,"ERROR (%s):Bad address.\n",((!_string)?"":_string));break;
		case WSAEHOSTDOWN:			fprintf(_fp,"ERROR (%s):Host is down.\n",((!_string)?"":_string));break;
		case WSAEHOSTUNREACH:		fprintf(_fp,"ERROR (%s):No route to host.\n",((!_string)?"":_string));break;
		case WSAEINPROGRESS:		fprintf(_fp,"ERROR (%s):Operation now in progress.\n",((!_string)?"":_string));break;
		case WSAEINTR:				fprintf(_fp,"ERROR (%s):Interrupted function call.\n",((!_string)?"":_string));break;
		case WSAEINVAL:				fprintf(_fp,"ERROR (%s):Invalid argument.\n",((!_string)?"":_string));break;
		case WSAEISCONN:			fprintf(_fp,"ERROR (%s):Socket is already connected.\n",((!_string)?"":_string));break;
		case WSAEMFILE:				fprintf(_fp,"ERROR (%s):Too many open files.\n",((!_string)?"":_string));break;
		case WSAEMSGSIZE:			fprintf(_fp,"ERROR (%s):Message too long.\n",((!_string)?"":_string));break;
		case WSAENETDOWN:			fprintf(_fp,"ERROR (%s):Network is down.\n",((!_string)?"":_string));break;
		case WSAENETRESET:			fprintf(_fp,"ERROR (%s):Network dropped connection on reset.\n",((!_string)?"":_string));break;
		case WSAENETUNREACH:		fprintf(_fp,"ERROR (%s):Network is unreachable.\n",((!_string)?"":_string));break;
		case WSAENOBUFS:			fprintf(_fp,"ERROR (%s):No buffer space available.\n",((!_string)?"":_string));break;
		case WSAENOPROTOOPT:		fprintf(_fp,"ERROR (%s):Bad protocol option.\n",((!_string)?"":_string));break;
		case WSAENOTCONN:			fprintf(_fp,"ERROR (%s):Socket is not connected.\n",((!_string)?"":_string));break;
		case WSAENOTSOCK:			fprintf(_fp,"ERROR (%s):Socket operation on non-socket.\n",((!_string)?"":_string));break;
		case WSAEOPNOTSUPP:			fprintf(_fp,"ERROR (%s):Operation not supported.\n",((!_string)?"":_string));break;
		case WSAEPFNOSUPPORT:		fprintf(_fp,"ERROR (%s):Protocol family not supported.\n",((!_string)?"":_string));break;
		case WSAEPROCLIM:			fprintf(_fp,"ERROR (%s):Too many processes.\n",((!_string)?"":_string));break;
		case WSAEPROTONOSUPPORT:	fprintf(_fp,"ERROR (%s):Protocol not supported.\n",((!_string)?"":_string));break; 
		case WSAEPROTOTYPE:			fprintf(_fp,"ERROR (%s):Protocol wrong type for socket.\n",((!_string)?"":_string));break;
		case WSAESHUTDOWN:			fprintf(_fp,"ERROR (%s):Cannot send after socket shutdown.\n",((!_string)?"":_string));break;
		case WSAESOCKTNOSUPPORT:	fprintf(_fp,"ERROR (%s):Socket type not supported.\n",((!_string)?"":_string));break;
		case WSAETIMEDOUT:			fprintf(_fp,"ERROR (%s):Connection timed out.\n",((!_string)?"":_string));break;
		case WSATYPE_NOT_FOUND:		fprintf(_fp,"ERROR (%s):Class type not found.\n",((!_string)?"":_string));break;
		case WSAEWOULDBLOCK:		fprintf(_fp,"ERROR (%s):Resource temporarily unavailable.\n",((!_string)?"":_string));break;
		case WSAHOST_NOT_FOUND:		fprintf(_fp,"ERROR (%s):Host not found.\n",((!_string)?"":_string));break;
		case WSA_INVALID_HANDLE:	fprintf(_fp,"ERROR (%s):Specified event object handle is invalid.\n",((!_string)?"":_string));break;
		case WSA_INVALID_PARAMETER: fprintf(_fp,"ERROR (%s):One or more parameters are invalid.\n",((!_string)?"":_string));break;
#ifdef WSAINVALIDPROCTABLE
		case WSAINVALIDPROCTABLE:	fprintf(_fp,"ERROR (%s):Invalid procedure table from service provider.\n",((!_string)?"":_string));break;
#endif
#ifdef WSAINVALIDPROVIDER
		case WSAINVALIDPROVIDER:	fprintf(_fp,"ERROR (%s):Invalid service provider version number.\n",((!_string)?"":_string));break;
#endif
		case WSA_IO_INCOMPLETE:		fprintf(_fp,"ERROR (%s):Overlapped I/O event object not in signaled state.\n",((!_string)?"":_string));break;
		case WSA_IO_PENDING:		fprintf(_fp,"ERROR (%s):Overlapped operations will complete later.\n",((!_string)?"":_string));break;
		case WSA_NOT_ENOUGH_MEMORY: fprintf(_fp,"ERROR (%s):Insufficient memory available.\n",((!_string)?"":_string));break;
		case WSANOTINITIALISED:		fprintf(_fp,"ERROR (%s):Successful WSAStartup not yet performed.\n",((!_string)?"":_string));break;
		case WSANO_DATA:			fprintf(_fp,"ERROR (%s):Valid name, no data record of requested type.\n",((!_string)?"":_string));break;
		case WSANO_RECOVERY:		fprintf(_fp,"ERROR (%s):This is a non-recoverable error.\n",((!_string)?"":_string));break;
#ifdef WSAPROVIDERFAILEDINIT
		case WSAPROVIDERFAILEDINIT: fprintf(_fp,"ERROR (%s):Unable to initialize a service provider.\n",((!_string)?"":_string));break;
#endif
		case WSASYSCALLFAILURE:		fprintf(_fp,"ERROR (%s):System call failure.\n",((!_string)?"":_string));break;
		case WSASYSNOTREADY:		fprintf(_fp,"ERROR (%s):Network subsystem is unavailable.\n",((!_string)?"":_string));break;
		case WSATRY_AGAIN:			fprintf(_fp,"ERROR (%s):Non-authoritative host not found.\n",((!_string)?"":_string));break;
		case WSAVERNOTSUPPORTED:	fprintf(_fp,"ERROR (%s):WINSOCK.DLL version out of range.\n",((!_string)?"":_string));break;
		case WSAEDISCON:			fprintf(_fp,"ERROR (%s):Graceful shutdown in progress.\n",((!_string)?"":_string));break;
		case WSA_OPERATION_ABORTED: fprintf(_fp,"ERROR (%s):Overlapped operation aborted.\n",((!_string)?"":_string));break;
		default:					fprintf(_fp,"ERROR (%s):An unkown kind of error has occured\n",((!_string)?"":_string));break;
	}
	fflush(_fp);
	return val;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                  ///////////////////////////////////////////////////////////////////////////////////////////////
//  Protocol stuff  ///////////////////////////////////////////////////////////////////////////////////////////////
//                  ///////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void WindowsNetwork::PrintHostData(FILE *fp,struct hostent *h)
{
	fprintf(fp,"Host:\n");
	fprintf(fp,"	Hostname   :%s\n",h->h_name);
	for(int i=0;h->h_aliases[i]!=NULL;i++)		
		fprintf(fp,"	  Alias    :%s\n",h->h_aliases[i]);
	fprintf(fp,"	Addresstype:%d\n",h->h_addrtype);
	for(i=0;h->h_addr_list[i]!=NULL;i++)
		fprintf(fp,"	Address    :%s\n",inet_ntoa(*((struct in_addr *)h->h_addr_list[i])));
	fflush(fp);
}


///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//               //////////////////////////////////////////////////////////////////////////////////////////////////
//  Brain stuff  //////////////////////////////////////////////////////////////////////////////////////////////////
//               //////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*
WSAPROTOCOL_INFO *protBuffer = NULL;
ULONG bufferLen=0;
int protNr=0;

	// which protocols are exist on this system?????
	bool doAgain=true;

	while(doAgain)
	{
		doAgain=false;
		protNr=WSAEnumProtocols(NULL,(LPWSAPROTOCOL_INFO)protBuffer,&bufferLen);
		if(protNr==SOCKET_ERROR)
		{
			err=WindowsNetwork::CheckWSAError(stdout);
			if(err==WSAENOBUFS)
			{
				protBuffer = new WSAPROTOCOL_INFO[bufferLen/sizeof(WSAPROTOCOL_INFO)];
				doAgain = true;
			}
		}else
		{
			printf("Found %d different protocols on this system\n",protNr);
			for(int i=0;i<protNr;i++)
			{
				printf("%d.protocol:%s (UDP%ssupported)\n",i+1,protBuffer[i].szProtocol,
					(SOCK_DGRAM==protBuffer[i].iSocketType)?" ":" not ");
			
			}
			fflush(stdout);
		}
	}
*/
	// get local address
/*	struct hostent *hostEnt=gethostbyname("localhost");
	if(!hostEnt)
	{
		WindowsNetwork::CheckWSAError(stdout);
		return NETWORK_RET_ERROR;
	}else
		this->printHostData(stdout,hostEnt);

	long addr=htonl((193<<24) + (174<<16) + (66<<8) + 46);
	struct hostent *hostEnt2=gethostbyaddr((const char *)&addr,4,1);                
	if(!hostEnt2)
	{
		WindowsNetwork::CheckWSAError(stdout);
		return NETWORK_RET_ERROR;
	}else
		this->printHostData(stdout,hostEnt2);
	
	struct hostent *hostEnt3=gethostbyname(hostEnt->h_name);
	if(!hostEnt3)
	{
		WindowsNetwork::CheckWSAError(stdout);
		return NETWORK_RET_ERROR;
	}else
		this->printHostData(stdout,hostEnt3);
*/
// Get max packetsize	
/*	char str[256];
	int len=256;
	// get max size for packets 
	getsockopt(this->sock,SOL_SOCKET,SO_MAX_MSG_SIZE ,str,&len);  

	printf("Packetsize = %d (len = %d)\n",(UINT)str,len);
	fflush(stdout);
*/
#endif