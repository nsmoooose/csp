// Copyright (C) 2001-2002 Open Source Telecom Corporation.
//
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software 
// Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
// 
// As a special exception to the GNU General Public License, permission is 
// granted for additional uses of the text contained in its release 
// of Common C++.
// 
// The exception is that, if you link the Common C++ library with other
// files to produce an executable, this does not by itself cause the
// resulting executable to be covered by the GNU General Public License.
// Your use of that executable is in no way restricted on account of
// linking the Common C++ library code into it.
//
// This exception does not however invalidate any other reasons why
// the executable file might be covered by the GNU General Public License.
// 
// This exception applies only to the code released under the 
// name Common C++.  If you copy code from other releases into a copy of
// Common C++, as the General Public License permits, the exception does
// not apply to the code that you add in this way.  To avoid misleading
// anyone as to the status of such modified files, you must delete
// this exception notice from them.
// 
// If you write modifications of your own for Common C++, it is your choice
// whether to permit this exception to apply to your modifications.
// If you do not wish that, delete this exception notice.

#include <cc++/config.h>
#include <cc++/export.h>
#include <cc++/url.h>

#include <string>
#include <cstdio>
#include <cstdlib>
#include <fcntl.h>
#include <cerrno>
#include <iostream>

#ifdef	WIN32
#include <io.h>
#endif

#ifdef HAVE_SSTREAM
#include <sstream>
#else
#include <strstream>
#endif

#include <cctype>

#ifndef WIN32
// cause problem on Solaris
#if !defined(__sun) && !defined(__SUN__)
#include <net/if.h>
#endif
#include <sys/ioctl.h>
#endif

#ifdef	CCXX_NAMESPACES
namespace ost {
using namespace std;
#endif

URLStream::URLStream(timeout_t to) :
TCPStream()
{
	persistent = false;
	proxyPort = 0;
	timeout = to;
	protocol = protocolHttp1_0;
	follow = true;
	encoding = encodingBinary;
	auth = authAnonymous;
	cookie = agent = pragma = referer = user = password = NULL;
	localif = NULL;
	setError(false);
}

int URLStream::underflow(void)
{
	int len, rlen;
	char *buf;

	if(bufsize == 1)
		return TCPStream::underflow();

	if(!gptr())
		return EOF;

	if(gptr() < egptr())
		return (unsigned char)*gptr();

	rlen = (gbuf + bufsize) - eback();
	if(encoding == encodingChunked)
	{
		buf = (char *)eback();
		*buf = '\n';
		while(!chunk && (*buf == '\n' || *buf == '\r'))
		{
			*buf = 0;
			len = readLine(buf, rlen, timeout);
		}
		if(len)
		{
			if(!chunk)
				chunk = strtol(buf, NULL, 16);

			if(rlen > (int)chunk)
				rlen = chunk;
		}
		else
			rlen = -1;
	}

	if(rlen > 0)
	{
		if(Socket::state == STREAM)
			rlen = aRead((char *)eback(), rlen, timeout);
		else if(timeout)
		{
			if(Socket::isPending(pendingInput, timeout))
				rlen = ::recv(so, (char *)eback(), rlen, 0);
			else
				rlen = -1;
		}
		else
			rlen = ::recv(so, (char *)eback(), rlen, 0);
	}
	if(encoding == encodingChunked && rlen > 0)
		chunk -= rlen;

	if(rlen < 1)
	{
		if(rlen < 0)
			clear(ios::failbit | rdstate());
		return EOF;
	}
	setg(eback(), eback(), eback() + rlen);
	return (unsigned char)*gptr();
}
	
void URLStream::setProxy(const char *host, tpport_t port)
{
	proxyHost = host;
	proxyPort = port;
}

URLStream::Error URLStream::submit(const char *path, const char **vars,
	       int bufsize)
{
	Error status = errInvalid;

	if(!strnicmp(path, "http:", 5))
	{
		urlmethod = methodHttpGet;
		path = strchr(path + 5, '/');
		status = sendHTTPHeader(path, vars, bufsize);
	}
	if((status == errInvalid || status == errTimeout))
	{
		if(Socket::state != AVAILABLE)
			close();
		return status;
	}
	else
	{
		status = getHTTPHeaders();
		if(status == errSuccess)
			return status;
		else if(status == errTimeout)
		{
			if(Socket::state != AVAILABLE)
				close();
		}
		return status;
	}
}

URLStream::Error URLStream::post(const char *path, const char **vars,
		int bufsize)
{
	Error status = errInvalid;

	if(!strnicmp(path, "http:", 5))
	{
		urlmethod = methodHttpPost;
		path = strchr(path + 5, '/');
		status = sendHTTPHeader(path, vars, bufsize);
	}

	if((status == errInvalid || status == errTimeout))
	{
		if(Socket::state != AVAILABLE)
			close();
		return status;
	}
	else
	{
		status = getHTTPHeaders();
		if(status == errSuccess)
			return status;
		else if(status == errTimeout)
		{
			if(Socket::state != AVAILABLE)
				close();
		}
		return status;
	}
}


URLStream::Error URLStream::head(const char *path, int bufsize)
{
	Error status = errInvalid;

	if(!strnicmp(path, "http:", 5))
	{
		urlmethod = methodHttpGet;
		path = strchr(path + 5, '/');
		status = sendHTTPHeader(path, NULL, bufsize);
	}

	if((status == errInvalid || status == errTimeout))
	{
		if(Socket::state != AVAILABLE)
			close();
		return status;
	}
	else
	{
		status = getHTTPHeaders();
		if(status == errSuccess)
			return status;
		else if(status == errTimeout)
		{
			if(Socket::state != AVAILABLE)
				close();
		}
		return status;
	}
}

URLStream &URLStream::getline(char *buffer, size_t size)
{
	size_t len;

	*buffer = 0;
	// TODO: check, we mix use of streambuf with Socket::readLine...
	iostream::getline(buffer, size);
	len = strlen(buffer);
	
	while(len)
	{
		if(buffer[len - 1] == '\r' || buffer[len - 1] == '\n')
			buffer[len - 1] = 0;
		else
			break;
		--len;
	}
	return *this;
}

URLStream::Error URLStream::get(const char *urlpath, int bufsize)
{
	const char *path = urlpath;
	Error status;

	urlmethod = methodFileGet;

	if(Socket::state != AVAILABLE)
		close();

	
	if(!strnicmp(path, "file:", 5))
	{
		urlmethod = methodFileGet;
		path += 5;
	}
	else if(!strnicmp(path, "http:", 5))
	{
		urlmethod = methodHttpGet;
		path = strchr(path + 5, '/');
	}
	switch(urlmethod)
	{
	case methodHttpGet:
		status = sendHTTPHeader(path, NULL, bufsize);
		break;
	case methodFileGet:
		if(so != INVALID_SOCKET)
			::close(so);
		so = ::open(path, O_RDWR);
		if(so == INVALID_SOCKET)
			so = ::open(path, O_RDONLY);
                // FIXME: open return the same handle type as socket call ??
		if(so == INVALID_SOCKET)
			return errInvalid;
		Socket::state = STREAM;
		allocate(bufsize);
		return errSuccess;
	case methodHttpPut:
	case methodHttpPost:
	case methodFtpGet:
	case methodFtpPut:
	case methodFilePut:
		break;
	}


	if((status == errInvalid || status == errTimeout))
	{
		if(Socket::state != AVAILABLE)
			close();
		return status;
	}
	else
	{
		status = getHTTPHeaders();
		if(status == errSuccess)
			return status;
		else if(status == errTimeout)
		{
			if(Socket::state != AVAILABLE)
				close();
		}
		return status;
	}
}

URLStream::Error URLStream::getHTTPHeaders()
{
	char buffer[512];
	size_t bufsize = sizeof(buffer);
	Error status = errSuccess;
	char *cp, *ep;
	ssize_t len = 1;
	char nc = 0;

	chunk = ((unsigned)-1) / 2;
	encoding = encodingBinary;
	while(len > 0)
	{
		len = readLine(buffer, bufsize, timeout);
		if(len < 1)
			return errTimeout;
		
		// FIXME: for multiline syntax ??
		if(buffer[0] == ' ' || buffer[0] == '\r' || buffer[0] == '\n')
			break;
		cp = strchr(buffer, ':');
		if(!cp)
			continue;
		*(cp++) = 0;
		while(*cp == ' ' || *cp == '\t')
			++cp;
		ep = strchr(cp, '\n');
		if(!ep)
			ep = &nc;
		while(*ep == '\n' || *ep == '\r' || *ep == ' ')
		{
			*ep = 0;
			if((--ep) < cp)
				break;
		}
		if(!stricmp(buffer, "Transfer-Encoding"))
		{
			if(!stricmp(cp, "chunked"))
			{
				chunk = 0;
				encoding = encodingChunked;
			}
		}	
		httpHeader(buffer, cp);
	}
	return status;
}


void URLStream::close(void)
{
	if(Socket::state == AVAILABLE)
		return;

	endStream();
	so = socket(AF_INET, SOCK_STREAM, 0);
	if(so != INVALID_SOCKET)
		Socket::state = AVAILABLE;
}

URLStream::Error URLStream::sendHTTPHeader(const char *url, const char **vars,
		int bufsize)
{
	// TODO: implement authentication
	// FIXME: insufficient buffer
	char reloc[1024];
	// "//" host ":" port == max 2 + 128 + 1 + 5 + 1(\0) = 137, rounded 140
	char host[140];
	// TODO: add support for //user:pass@host:port/ syntax
#ifdef HAVE_SSTREAM
	ostringstream str;
#else
	// FIXME: very insufficient url (and referer) are max 4096...
	char buffer[2048];
	strstream str(buffer, sizeof(buffer), ios::out);
#endif
	char *ref, *cp, *ep;
	char *hp;
	const char *uri = "/";
	int count = 0;
	int len = 0;
	tpport_t port = 80;
	const char **args = vars;
	const char *var;

retry:
#ifdef HAVE_SSTREAM
	str.str() = "";
#else
	buffer[0] = 0;
	str.seekp(0);
#endif
	strncpy(host, url, 139);
	host[139] = 0;
	hp = strchr(host, '/');
	while(*hp == '/')
		++hp;
	cp = strchr(hp, '/');
	if (cp) *cp = 0;
	ep = strrchr(hp, ':');
	if(ep)
	{
		port = atoi(ep+1);
		*ep = 0;
	}

	if(!proxyPort)
	{
		const char* ep = url;
		while(*ep == '/')
			++ep;
		ep = strchr(ep, '/');
		if(ep)
	        	uri = ep;
	}

	switch(urlmethod)
	{
	case methodHttpGet:
		str << "GET ";
		if(proxyPort)
		{
			str << "http:" << url;
			if(!cp) str << '/';
		}
		else
			str << uri;
		break;
	case methodHttpPost:
		str << "POST ";
		if(proxyPort)
		{
			str << "http:" << url;
			if(!cp) str << '/';
		}
		else
			str << uri;
		break;
	default:
		return errInvalid;
	}

	if(vars && urlmethod == methodHttpGet)
	{
		str << "?";
		while(*vars)
		{
			if(count++)
				str << "&";
			str << *vars;
			++vars;
		}
	}

	switch(protocol)
	{
	case protocolHttp1_1:
		str << " HTTP/1.1" << "\r\n";
		break;
	case protocolHttp1_0:
		str << " HTTP/1.0" << "\r\n";
		break;
	}

	str << "Host: " << hp << "\r\n";
        if(agent)
                str << "User-Agent: " << agent << "\r\n";

        if(cookie)
                str << "Cookie: " << cookie << "\r\n";

        if(pragma)
                str << "Pragma: " << pragma << "\r\n";

        if(referer)
                str << "Referer: " << referer << "\r\n";

	switch(auth)
	{
	case authBasic:
		str << "Authorization: Basic ";
		snprintf(reloc, 64, "%s:%s", user, password);
		b64Encode(reloc, reloc + 64, 128);
		str << reloc + 64 << "\r\n";
	case authAnonymous:
		break;
	}

        str << "Connection: close\r\n";
	char **add = extraHeader();
	if(add)
	{
		while(*add)
		{
			str << *(add++) << ": ";
			str << *(add++) << "\r\n";
		}
	}
	if(vars && urlmethod == methodHttpPost)
	{
		while(*args)
		{
			var = *args;
			if(count++)
				len += strlen(var) + 1;
			else
				len = strlen(var);
			++args;
		}
		count = 0;
		len += 2;
		str << "Content-Length: " << len << "\r\n";
	}

        str << "\r\n";
#ifdef HAVE_SSTREAM
	// sstream does not want ends
#else
	str << ends;
#endif

        if(Socket::state != AVAILABLE)
                        close();
#ifndef	WIN32
#ifdef	SOICGIFINDEX
	if (localif != NULL) 
	{
		struct ifreq ifr;
		sockaddr_in source;
		int alen = sizeof(source);

		memset(&ifr, 0, sizeof(ifr));
		strncpy(ifr.ifr_name, localif, 15);
		if (ioctl(so, SIOCGIFINDEX, &ifr) < 0) 
		{
			// FIXME: very bad! See below for other
			cerr << "Warning Unknown interface!" << endl;
		} else {
			if (setsockopt(so, 
				SOL_SOCKET, 
				SO_BINDTODEVICE, 
				&ifr,
				sizeof(ifr)) == -1) {
					cerr << "Ignoring Interface!" <<endl;
			} else if (getsockname(so, (struct sockaddr*)&source,(socklen_t *) &alen) == -1) {
				cerr << "getsockname error!" << endl;
			} else if (bind(so, (struct sockaddr*)&source, sizeof(source)) == -1) {
					cerr << "Bind Error!" << endl;
			} else {
					source.sin_port = 0;
			}
			

		}

	}
#endif
#endif

	if(proxyPort)
	        connect(proxyHost, proxyPort, bufsize);
	else
		connect(InetHostAddress(hp), port, bufsize);

        if(!isConnected())
                return errUnreachable;

	// FIXME: send (or write) can send less than len bytes
	// use stream funcion ??
#ifdef HAVE_SSTREAM
	::send(so, str.str().c_str(), str.str().length(), 0);
#else
	::send(so, buffer, strlen(buffer), 0);
#endif

	if(urlmethod == methodHttpPost && vars)
	{
#ifdef HAVE_SSTREAM
	        str.str() = "";
#else
		str.seekp(0);
#endif
		bool sep = false;
		while(*vars)
		{
                        if(sep)
                                ::send(so, "&", 1,0);
                        else
                                sep = true;
			var = *vars;
			::send(so, var, strlen(var),0);
			++vars;
		}
		::send(so, "\r\n", 2, 0);
	}

cont:
#ifdef HAVE_SSTREAM
	// FIXME: insufficient buffer?
	char buffer[2048];
#else
	// nothing here
#endif

        len = readLine(buffer, sizeof(buffer) - 1, timeout);
	if(len < 1)
		return errTimeout;

        if(strnicmp(buffer, "HTTP/", 5))
                return errInvalid;

        ref = strchr(buffer, ' ');
        while(*ref == ' ')
                ++ref;

        switch(atoi(ref))
        {
        default:
                return errInvalid;
	case 100:
		goto cont;
        case 200:
                return errSuccess;
        case 401:
                return errUnauthorized;
        case 403:
                return errForbidden;
        case 404:
                return errMissing;
        case 405:
                return errDenied;
        case 500:
        case 501:
        case 502:
        case 503:
        case 504:
        case 505:
                return errFailure;
        case 300:
        case 301:
        case 302:
                break;
        }
	if(!follow)
		return errRelocated;
        for(;;)
        {
                len = readLine(reloc, sizeof(reloc), timeout);
		if(len < 1)
			return errTimeout;
                if(!strnicmp(reloc, "Location: ", 10))
                        break;
        }
        if(!strnicmp(reloc + 10, "http:", 5))
        {
                url = strchr(reloc + 15, '/');
                ep = (char *)(url + strlen(url) - 1);
                while(*ep == '\r' || *ep == '\n')
                        *(ep--) = 0;
        }
        else
                url = reloc + 10;
        close();
        goto retry;
}

void URLStream::setAuthentication(Authentication a, const char *value)
{
	auth = a;
	if (auth != authAnonymous)
	{
		if(!user)
			user = "anonymous";
		if(!password)
			password = "";
	}
}

void URLStream::setReferer(const char *str)
{
	if(!str)
		return;
	referer = str;
};

#ifdef	CCXX_NAMESPACES
};
#endif

/** EMACS **
 * Local variables:
 * mode: c++
 * c-basic-offset: 8
 * End:
 */
