#ifndef _LinuxNetwork__CPP_
#define _LinuxNetwork__CPP_

#include "LinuxNetwork.h"
#include <unistd.h>
#include <netdb.h>
#include <errno.h>
#include <sys/time.h>
#include <stdio.h>

// -------------------------------------------
// STANDARD FRAMEWORK
// -------------------------------------------
void  LinuxNetwork::SetSetup(void *setup)
{
}

short LinuxNetwork::GetDependencyCount()
{
  return 0;
}

void  LinuxNetwork::GetDependencyDescription(short Index, _DEPENDENCY *Dependency)
{
  memset(Dependency, 0, sizeof(_DEPENDENCY));
}

short LinuxNetwork::SetDependencyData(short ID, void* Data)
{
  return 0;
}

void  LinuxNetwork::GetSetup(void*setup)
{
}

short LinuxNetwork::CheckStatus()
{
  return 0;
}

/////////////////////////////////////////////////////////////////////////////////////////////////
LinuxNetwork::LinuxNetwork(SOCKET _initializedSocket)
{
	this->m_sock = _initializedSocket;
	if(_initializedSocket != INVALID_SOCKET)
	{
		long ip;
		short p;
		GetSockAddress(ip,p);
	}
}

/////////////////////////////////////////////////////////////////////////////////////////////////
LinuxNetwork::~LinuxNetwork(void)
{
	this->Close();
}

/////////////////////////////////////////////////////////////////////////////////////////////////
short LinuxNetwork::Create(unsigned short _socketType,long &_ip, short &_port)
{
	m_localAddress.sin_family = AF_INET;
	m_localAddress.sin_port = htons(_port);
	m_localAddress.sin_addr.s_addr = htonl(_ip);

	/// create socket and check for errors
	if(_socketType == NETWORK_TCPIP)
	  this->m_sock = socket(AF_INET,SOCK_STREAM,0);
	else if(_socketType == NETWORK_UDPIP)
	  this->m_sock = socket(AF_INET,SOCK_DGRAM,0);
	else
	{
	  printf("ERROR (LinuxNetwork::Create(...)):Wrong \"_socketType\".\n");
	  return NETWORK_RET_ERROR;
	}


	if(this->m_sock == INVALID_SOCKET)
	{
		LinuxNetwork::CheckError(stdout,"LinuxNetwork::Create(...)");
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
		LinuxNetwork::CheckError(stdout,"LinuxNetwork::Create(...)");
		return NETWORK_RET_ERROR;
	}

	// get the assigned address and (more important) the port
	short ret = this->GetSockAddress(_ip,_port);

	return ret;
}


/////////////////////////////////////////////////////////////////////////////////////////////////
short LinuxNetwork::Close(void)
{
	if(this->m_sock != INVALID_SOCKET)
	{
		if(close(this->m_sock) == SOCKET_ERROR)
		{
		        LinuxNetwork::CheckError(stdout,"LinuxNetwork::Close(...)");
			return NETWORK_RET_ERROR;
		}
		this->m_sock = INVALID_SOCKET;
	}
	return NETWORK_RET_OK;
}

/////////////////////////////////////////////////////////////////////////////////////////////////
short LinuxNetwork::Listen(short _maxConnectionNr)
{
	if(listen(this->m_sock,_maxConnectionNr) == SOCKET_ERROR)
	{
       	        LinuxNetwork::CheckError(stdout,"LinuxNetwork::Listen(...)");
		return NETWORK_RET_ERROR;
	}

	return NETWORK_RET_OK;
}

/////////////////////////////////////////////////////////////////////////////////////////////////
short LinuxNetwork::Accept(StandardNetwork **_s, const long _mSecs)
{

	fd_set fdSet;
	struct timeval tVal;
	struct sockaddr_in addr;
	socklen_t addrLen = sizeof(addr);
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
       	        LinuxNetwork::CheckError(stdout,"LinuxNetwork::Accept(...)");
		return NETWORK_RET_ERROR;
	default:
		newSock = accept(this->m_sock,(struct sockaddr *)&addr,&addrLen);
		if(newSock == INVALID_SOCKET)
		{
		  LinuxNetwork::CheckError(stdout,"LinuxNetwork::Accept(...)");
		  return NETWORK_RET_ERROR;
		}
		else
		{
			*_s = new LinuxNetwork(newSock);
			return NETWORK_RET_OK;
		}
	}
}

/////////////////////////////////////////////////////////////////////////////////////////////////
short LinuxNetwork::Connect(long _ip,short _port)
{
	m_connectAddress.sin_family = AF_INET;
	m_connectAddress.sin_port = htons(_port);
	m_connectAddress.sin_addr.s_addr = htonl(_ip);

	if(connect(this->m_sock,(struct sockaddr *)&m_connectAddress,sizeof(m_connectAddress)) == SOCKET_ERROR)
	{
		m_connectAddress.sin_port = 0;
		m_connectAddress.sin_addr.s_addr = 0;
		LinuxNetwork::CheckError(stdout,"LinuxNetwork::Connect(...)");
		return NETWORK_RET_ERROR;
	}

	return NETWORK_RET_OK;
}

/////////////////////////////////////////////////////////////////////////////////////////////////
short LinuxNetwork::Send(const char *_packet,const short _packetSize)
{
	if(send(this->m_sock,_packet,_packetSize,0) == SOCKET_ERROR)
	{
		LinuxNetwork::CheckError(stdout,"LinuxNetwork::Send(...)");
		return NETWORK_RET_ERROR;
	}
	return NETWORK_RET_OK;
}

/////////////////////////////////////////////////////////////////////////////////////////////////
short LinuxNetwork::SendTo(char *_packet, const short _packetSize, long _ip, short _port)
{
	struct sockaddr_in addr;

	addr.sin_family = AF_INET;
	addr.sin_port = htons(_port);
	addr.sin_addr.s_addr = htonl(_ip);
	
	if(sendto(this->m_sock,_packet,_packetSize,0,(struct sockaddr *)&addr,sizeof(addr)) == SOCKET_ERROR)
	{
		LinuxNetwork::CheckError(stdout,"LinuxNetwork::SendTo(...)");
		return NETWORK_RET_ERROR;
	}

	return NETWORK_RET_OK;
}

/////////////////////////////////////////////////////////////////////////////////////////////////
short LinuxNetwork::Receive(char *_packet,short *_packetSize, const long _mSecs)
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
		LinuxNetwork::CheckError(stdout,"LinuxNetwork::Receive(...)");
		return NETWORK_RET_ERROR;
	default:
		ret = recv(this->m_sock,_packet,*_packetSize,0);
		if(ret == SOCKET_ERROR)
		{
		  LinuxNetwork::CheckError(stdout,"LinuxNetwork::Receive(...)");
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
short LinuxNetwork::ReceiveFrom(char *_packet,short *_packetSize, long _ip, short _port,const long _mSecs)
{
	fd_set fdSet;
	struct timeval tVal;
	short ret = -1;
	struct sockaddr_in addr;

	addr.sin_family = AF_INET;
	addr.sin_port = htons(_port);
	addr.sin_addr.s_addr = htonl(_ip);
	socklen_t len = sizeof(addr);
	

	FD_ZERO(&fdSet);
	FD_SET(this->m_sock,&fdSet);

    tVal.tv_sec = 0;
    tVal.tv_usec = _mSecs;
	
	switch(select(this->m_sock+1,&fdSet,NULL,NULL,&tVal))
	{
	case 0:
		return NETWORK_RET_TIMEOUT;
	case SOCKET_ERROR:
	       LinuxNetwork::CheckError(stdout,"LinuxNetwork::ReceiveFrom(...)");
		return NETWORK_RET_ERROR;
	default:
		ret = recvfrom(this->m_sock,_packet,*_packetSize,0,(struct sockaddr *)&addr,&len);
		if(ret == SOCKET_ERROR)
		{
		        LinuxNetwork::CheckError(stdout,"LinuxNetwork::ReceiveFrom(...)");	
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
short LinuxNetwork::GetPeerAddress(long &_ip,short &_port)
{
	struct sockaddr_in addr;
	socklen_t len = sizeof(addr);

	if(getpeername(this->m_sock,(struct sockaddr *)&addr,&len) == SOCKET_ERROR)
	{
		_ip = 0;
		_port = 0;
       	        LinuxNetwork::CheckError(stdout,"LinuxNetwork::GetPeerAddress(...)");	
		return NETWORK_RET_ERROR;
	}
	
	_ip = ntohl(addr.sin_addr.s_addr);
	_port = ntohs(addr.sin_port);	

	return NETWORK_RET_OK;
}

/////////////////////////////////////////////////////////////////////////////////////////////////
short LinuxNetwork::GetSockAddress(long &_ip,short &_port)
{
	socklen_t len = sizeof(m_localAddress);

	if(getsockname(this->m_sock,(struct sockaddr *)&m_localAddress,&len) == SOCKET_ERROR)
	{
		m_ip = 0;
		m_port = 0;
		
		_ip = 0;
		_port = 0;

       	        LinuxNetwork::CheckError(stdout,"LinuxNetwork::GetSockAddress(...)");	
		return NETWORK_RET_ERROR;
	}

	m_ip = ntohl(m_localAddress.sin_addr.s_addr);
	m_port = ntohs(m_localAddress.sin_port);	
	
	_ip = m_ip;
	_port = m_port;

	return NETWORK_RET_OK;
}

/*
/////////////////////////////////////////////////////////////////////////////////////////////////
struct hostent *LinuxNetwork::GetHostByName(const char *_name)
{
	struct hostent *h = gethostbyname(_name);

	if(!h)
	{
       	        LinuxNetwork::CheckError(stdout,"LinuxNetwork::GetHostByName(...)");	
		return NULL;
	}
	return h;
}

/////////////////////////////////////////////////////////////////////////////////////////////////
struct hostent *LinuxNetwork::GetHostByAddress(struct sockaddr_in &_addr)
{
	struct hostent *h = gethostbyaddr((char *)&(_addr.sin_addr.S_un.S_un_b),4,0);

	if(!h)
	{
       	        LinuxNetwork::CheckError(stdout,"LinuxNetwork::GetHostByAddress(...)");	
		return NULL;
		}
	return h;
}
*/

//////////////////////////////////////////////////////////////////////
short LinuxNetwork::CheckError(FILE *_fp,char *_string)
{
  fprintf(_fp,"ERROR (%s):%s\n",_string,strerror(errno));
  fflush(_fp);
  
  return errno;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                  ///////////////////////////////////////////////////////////////////////////////////////////////
//  Protocol stuff  ///////////////////////////////////////////////////////////////////////////////////////////////
//                  ///////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void LinuxNetwork::PrintHostData(FILE *fp,struct hostent *h)
{
	fprintf(fp,"Host:\n");
	fprintf(fp,"	Hostname   :%s\n",h->h_name);
	for(int i=0;h->h_aliases[i]!=NULL;i++)		
		fprintf(fp,"	  Alias    :%s\n",h->h_aliases[i]);
	fprintf(fp,"	Addresstype:%d\n",h->h_addrtype);
	for(int i=0;h->h_addr_list[i]!=NULL;i++)
	  //	fprintf(fp,"	Address    :%s\n",inet_ntoa(*((struct in_addr *)h->h_addr_list[i])));
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
			err=LinuxNetwork::CheckWSAError(stdout);
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
		LinuxNetwork::CheckWSAError(stdout);
		return NETWORK_RET_ERROR;
	}else
		this->printHostData(stdout,hostEnt);

	long addr=htonl((193<<24) + (174<<16) + (66<<8) + 46);
	struct hostent *hostEnt2=gethostbyaddr((const char *)&addr,4,1);                
	if(!hostEnt2)
	{
		LinuxNetwork::CheckWSAError(stdout);
		return NETWORK_RET_ERROR;
	}else
		this->printHostData(stdout,hostEnt2);
	
	struct hostent *hostEnt3=gethostbyname(hostEnt->h_name);
	if(!hostEnt3)
	{
		LinuxNetwork::CheckWSAError(stdout);
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
