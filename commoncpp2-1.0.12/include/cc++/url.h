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
// The exception is that, if you link the Common C++ library with other files
// to produce an executable, this does not by itself cause the
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

/**
 * @file url.h
 * @short URL streams abstraction.
 **/

#ifndef	CCXX_URL_H_
#define	CCXX_URL_H_

#ifndef CCXX_CONFIG_H_
#include <cc++/config.h>
#endif

#ifndef CCXX_SOCKET_H_
#include <cc++/socket.h>
#endif

#ifndef CCXX_URLSTRING_H_
#include <cc++/urlstring.h>
#endif

#ifdef	CCXX_NAMESPACES
namespace ost {
#endif

class CCXX_CLASS_EXPORT URLStream;

/**
 * A URL processing version of TCPStream.
 *
 * @author David Sugar <dyfet@ostel.com>
 * @short C++ url processing stream class.
 */
class CCXX_CLASS_EXPORT URLStream : public TCPStream
{
public:
	/**
	 * Return error for url fetch
	 */ 
	typedef enum
	{
		errSuccess = 0,
		errUnreachable,
		errMissing,
		errDenied,
		errInvalid,
		errForbidden,
		errUnauthorized,
		errRelocated,
		errFailure,
		errTimeout
	} Error;

	/**
	 * Type of authentication
	 */
	typedef enum
	{
		authAnonymous = 0,
		authBasic
	} Authentication;

	/**
	 * Encoding used in transfer
	 */
	typedef enum
	{
		encodingBinary = 0,
		encodingChunked
	} Encoding;

	/**
	 * Type of fetch
	 */
	typedef	enum
	{
		methodHttpGet,
		methodHttpPut,
		methodHttpPost,
		methodFtpGet,
		methodFtpPut,
		methodFileGet,
		methodFilePut
	} Method;

	/**
	 * http protocol version
	 */
	typedef	enum
	{
		protocolHttp1_0,
		protocolHttp1_1
	} Protocol;

private:
	const char *agent, *referer, *cookie, *pragma, *user, *password;
	const char *localif;
	InetHostAddress proxyHost;
	tpport_t proxyPort;
	Method urlmethod;
	Encoding encoding;
	Protocol protocol;
	Authentication auth;
	timeout_t timeout;
	bool persistent;
	bool follow;
	unsigned chunk;

	Error getHTTPHeaders();
	URLStream(const URLStream& rhs);

protected:
	Error sendHTTPHeader(const char *url, const char **vars, int bufsize);
	int underflow(void);

	virtual int aRead(char *buffer, size_t len, timeout_t timeout)
#ifndef __MINGW32__
                {return ::read(so, buffer, len);};
#else
                {return ::recv(so, buffer, len, 0); };
#endif

	virtual int aWrite(char *buffer, size_t len, timeout_t timeout)
#ifndef __MINGW32__
		{return ::write(so, buffer, len);};
#else
		{return ::send(so, buffer, len, 0); };
#endif

	/**
	 * Derived method to receive and parse http "headers".
	 *
	 * @param header keyword.
	 * @param value header keyword value.
	 */
	virtual void httpHeader(const char *header, const char *value)
		{return;};

	/**
	 * A virtual to insert additional header info into the request.
	 *
	 * @return array of header attributes to add.
	 */
	virtual char **extraHeader(void)
		{return NULL;};

public:
	/**
	 * Construct an instance of URL stream.
	 *
	 * @param to default timeout.
	 */
	URLStream(timeout_t to = 0);

	/**
	 * Line parsing with conversion.
	 *
	 * @return URLStream object.
	 * @param buffer to store.
	 * @param len maximum buffer size.
	 */
	URLStream &getline(char *buffer, size_t len);

	/**
	 * Get URL data from a named stream of a known buffer size.
	 *
	 * @return url error code.
	 * @param url name of resource.
	 * @param buffer size of buffer.
	 */
	Error get(const char *url, int buffer = 512);

	/**
	 * Submit URL with vars passed as argument array.  This submit
	 * assumes "GET" method.  Use "post" member to perform post.
	 *
	 * @return url error code.
	 * @param url name of resource.
	 * @param vars to set.
	 * @param buffer size of buffer.
	 */
	Error submit(const char *url, const char **vars, int buffer = 512);

	/**
	 * Post URL vars with post method.
	 *
	 * @return url error code.
	 * @param url name of resource.
	 * @param vars to set.
	 * @param buffer size of buffer.
	 */
	Error post(const char *url, const char **vars, int buffer = 512);

	/**
	 * Used to fetch header information for a resource.
	 *
	 * @return url error code.
	 * @param url name of resource.
	 * @param buffer size of buffer.
	 */
	Error head(const char *url, int buffer = 512);

	/**
	 * Close the URL stream for a new connection.
	 */
	void close();

	/**
	 * Set the referer url.
	 *
	 * @param str referer string.
	 */
	void setReferer(const char *str);

	/**
	 * Set the cookie to pass.
	 *
	 * @param str cookie string.
	 */
	inline void setCookie(const char *str)
		{cookie = str;};

	/**
	 * Set user id for the url.
	 *
	 * @param str user id.
	 */
	inline void setUser(const char *str)
		{user = str;};

	/**
	 * Set password for the url.
	 *
	 * @param str password.
	 */
	inline void setPassword(const char *str)
		{password = str;};

	/**
	 * Set authentication type for the url.
	 *
	 * @param a authentication.
	 * @param str string.
	 */
	void setAuthentication(Authentication a, const char *str = NULL);

	/**
	 * Set the pragmas.
	 *
	 * @param str pragma setting.
	 */
	inline void setPragma(const char *str)
		{pragma = str;};

	/**
	 * Set the proxy server used.
	 *
	 * @param host proxy host.
	 * @param port proxy port.
	 */
	void setProxy(const char *host, tpport_t port);

	/**
	 * Set the agent.
	 *
	 * @param str agent value.
	 */
	inline void setAgent(const char *str)
		{agent = str;};

	/**
	 * Get url method (and protocol) employed.
	 *
	 * @return url method in effect.
	 */
	inline Method getMethod(void)
		{return urlmethod;};

	/**
	 * Set socket timeout characteristics for processing URL
	 * requests.  Set to 0 for no default timeouts.
	 *
	 * @param to timeout to set.
	 */
	inline void setTimeout(timeout_t to)
		{timeout = to;};

	/**
	 * Specify url following.  Set to false to disable following
	 * of relocation requests.
	 *
	 * @param enable true to enable following.
	 */
	inline void setFollow(bool enable)
		{follow = enable;};

	/**
	 * Specify http protocol level being used.
	 *
	 * @param pro protocol level.
	 */
	inline void setProtocol(Protocol pro)
		{protocol = pro;};
	/**
	 * Specify local interface to use
	 *
	 * @param intf Local interface name
	 */
	inline void setLocalInterface(const char *intf) 
	{localif=intf;} 
};

#ifdef	CCXX_NAMESPACES
};
#endif

#endif
/** EMACS **
 * Local variables:
 * mode: c++
 * c-basic-offset: 8
 * End:
 */
