// Copyright (C) 1999-2003 Open Source Telecom Corporation.
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
 * @file socket.h
 * @short Network addresses and sockets related classes.
 **/

#ifndef	CCXX_SOCKET_H_
#define	CCXX_SOCKET_H_

#ifndef	CCXX_CONFIG_H_
#include <cc++/config.h>
#endif

#ifndef	CCXX_THREAD_H_
#include <cc++/thread.h>
#endif

#if defined(WIN32) && !defined(__CYGWIN32__)
#define	__WINSOCK__
#include <winsock2.h>
#include <io.h>
#define TIMEOUT_INF ~((timeout_t) 0)
typedef int socklen_t;
#else
#define INVALID_SOCKET	-1
typedef int SOCKET;
#endif

#include <iostream>

#ifndef	MSG_DONTWAIT
#define	MSG_DONTWAIT	0
#endif

#ifdef	CCXX_NAMESPACES
namespace ost {
#endif



/**
 * Transport Protocol Ports.
 */
typedef unsigned short tpport_t;

class CCXX_CLASS_EXPORT InetAddress;
class CCXX_CLASS_EXPORT InetHostAddress;
class CCXX_CLASS_EXPORT InetMaskAddress;
class CCXX_CLASS_EXPORT BroadcastAddress;
class CCXX_CLASS_EXPORT Socket;
class CCXX_CLASS_EXPORT UDPSocket;
class CCXX_CLASS_EXPORT UDPBroadcast;
class CCXX_CLASS_EXPORT UDPTransmit;
class CCXX_CLASS_EXPORT UDPReceive;
class CCXX_CLASS_EXPORT UDPDuplex;
class CCXX_CLASS_EXPORT TCPSocket;
class CCXX_CLASS_EXPORT TCPStream;
class CCXX_CLASS_EXPORT tcpstream;
class CCXX_CLASS_EXPORT TCPSession;
class CCXX_CLASS_EXPORT SocketPort;
class CCXX_CLASS_EXPORT SocketService;

/**
 * Classes derived from InetAddress would require an specific
 * validator to pass to the InetAddress constructor. This is a base
 * class for classes of function objects used by such derived classes.
 *
 * @author Federico Montesino <p5087@quintero.fie.us.es>
 * @short Abstract base class for derived inet addresses validators. 
 */
class InetAddrValidator 
{
public:
	/**
	 * Constructor. Does not deal with any state.
	 */
	InetAddrValidator() { };

	/**
	 * Pure virtual application operator. Apply the validation
	 * algorithm specific to derived classes.  
	 */
	inline virtual void 
	operator()(const in_addr address) const = 0;
};

/**
 * Class for the function object that validates multicast addresses.
 * Implements a specific application operator to validate multicast
 * addresses.
 *
 * @author Federico Montesino <p5087@quintero.fie.us.es>
 * @short Validating class specialized for multicast addresses.  
 */
class InetMcastAddrValidator: public InetAddrValidator
{
public:
	/**
	 * Constructor. Does not deal with any state.
	 */
	InetMcastAddrValidator(){};

	/**
	 * Application operator. Apply the validation algorithm
	 * specific to multicast addresses
	 */
	inline void 
	operator()(const in_addr address) const; 
private:
#if __BYTE_ORDER == __BIG_ENDIAN
	enum {
		MCAST_VALID_MASK = 0xF0000000,
		MCAST_VALID_VALUE = 0xE0000000
	};
#else
	enum { 
		MCAST_VALID_MASK = 0x000000F0,
		MCAST_VALID_VALUE = 0x000000E0 
	};
#endif
};

/**
 *  The network name and address objects are all derived from a common 
 * InetAddress base class. Specific classes, such as InetHostAddress,
 * InetMaskAddress, etc, are defined from InetAddress entirely so that the
 * manner a network address is being used can easily be documented and
 * understood from the code and to avoid common errors and accidental misuse 
 * of the wrong address object.  For example, a "connection" to something
 * that is declared as a "InetHostAddress" can be kept type-safe from a
 * "connection" accidently being made to something that was declared a 
 * "InetBroadcastAddress".
 * 
 * @author David Sugar <dyfet@ostel.com>
 * @short Internet Address binary data type.
 */
class InetAddress
{
private:
	// The validator given to an InetAddress object must not be a
	// transient object, but that must exist at least until the
	// last address object of its kind is deleted. This is an
	// artifact to be able to do specific checks for derived
	// classes inside constructors.
	const InetAddrValidator *validator;

protected:
	struct in_addr * ipaddr;
	size_t addr_count;
	mutable char* hostname;  // hostname for ipaddr[0]. Used by getHostname
#if defined(WIN32)
	static MutexCounter counter;
#else
	static Mutex mutex;
#endif
	/**
	 * Sets the IP address from a string representation of the
	 * numeric address, ie "127.0.0.1"
	 *
	 * @param host The string representation of the IP address
	 * @return true if successful
	 */
	bool setIPAddress(const char *host);

	/**
	 * Used to specify a host name or numeric internet address.
	 *
	 * @param host The string representation of the IP address or
	 *	a hostname, , if NULL, it will default to INADDR_ANY
	 */
	void setAddress(const char *host);

public:
	/**
	 * Create an Internet Address object with an empty (0.0.0.0)
	 * address.
	 *
	 * @param validator optional validator function object, intended for
	 *        derived classes.
	 */
	InetAddress(const InetAddrValidator *validator = NULL);

	/**
	 * Convert the system internet address data type (struct in_addr)
	 * into a Common C++ InetAddress object.
	 *
	 * @param addr struct of system used binary internet address.
	 * @param validator optional validator function object, intended for
	 *        derived classes.
	 */
	InetAddress(struct in_addr addr, const InetAddrValidator *validator = NULL);

	/**
	 * Convert a null terminated ASCII host address string
	 * (example: "127.0.0.1") or host address name (example:
	 * "www.voxilla.org") directly into a Common C++ InetAddress
	 * object.
	 *
	 * @param address null terminated C string.  
	 * @param validator optional validator function object, intended for
	 *        derived classes.
	 */
	InetAddress(const char *address, const InetAddrValidator *validator = NULL);

        /**
         * Copy constructor
         */
        InetAddress(const InetAddress &rhs);

	/**
	 * Destructor
	 */
	virtual ~InetAddress();

	/**
	 * Provide a string representation of the value (Internet Address)
	 * held in the InetAddress object.
	 * 
	 * @return string representation of InetAddress.
	 */
	const char *getHostname(void) const;

	/**
	 * May be used to verify if a given InetAddress returned
	 * by another function contains a "valid" address, or "0.0.0.0"
	 * which is often used to mark "invalid" InetAddress values.
	 *
	 * @return true if address != 0.0.0.0.
	 */
	bool isInetAddress(void) const;

	/**
	 * Provide a low level system usable struct in_addr object from
	 * the contents of InetAddress.  This is needed for services such
	 * as bind() and connect().
	 * 
	 * @return system binary coded internet address.
	 */
	struct in_addr getAddress(void) const;

	/**
	 * Provide a low level system usable struct in_addr object from
	 * the contents of InetAddress.  This is needed for services such
	 * as bind() and connect().
	 *
	 * @param i for InetAddresses with multiple addresses, returns the
	 *	address at this index.  User should call getAddressCount() 
	 *	to determine the number of address the object contains.
	 * @return system binary coded internet address.  If parameter i is
	 *	out of range, the first address is returned.
	 */
	struct in_addr getAddress(size_t i) const;

	/**
	 * Returns the number of internet addresses that an InetAddress object
	 * contains.  This usually only happens with InetHostAddress objects
	 * where multiple IP addresses are returned for a DNS lookup
	 */
	size_t getAddressCount() const { return addr_count; }

	InetAddress &operator=(const char *str);
	InetAddress &operator=(struct in_addr addr);
        InetAddress &operator=(const InetAddress &rhs);

        /**
         * Allows assignment from the return of functions like
         * inet_addr() or htonl() 
	 */
	InetAddress &operator=(unsigned long addr);

	inline InetAddress &operator=(unsigned int addr)
                {return *this = (unsigned long) addr; }

	inline bool operator!() const
		{return !isInetAddress();};

	/**
	 * Compare two internet addresses to see if they are equal
	 * (if they specify the physical address of the same internet host).
	 * 
	 * If there is more than one IP address in either InetAddress object,
	 * this will return true if all of the IP addresses in the smaller
	 * are in the larger in any order.
	 */
	bool operator==(const InetAddress &a) const;

	/**
	 * Compare two internet addresses to see if they are not
	 * equal (if they each refer to unique and different physical
	 * ip addresses).
	 *
	 * This is implimented in terms of operator==
	 */
	bool operator!=(const InetAddress &a) const;
};	

/**
 * Internet addresses used specifically as masking addresses (such as "
 * 255.255.255.0") are held in the InetMaskAddress derived object.  The
 * seperate class is used so that C++ type casting can automatically
 * determine when an InetAddress object is really a mask address object
 * rather than simply using the base class.  This also allows manipulative
 * operators for address masking to operate only when presented with a
 * Masked address as well as providing cleaner and safer source.
 * 
 * @author David Sugar <dyfet@ostel.com>
 * @short Internet Address Mask such as subnet masks.
 */
class InetMaskAddress : public InetAddress
{
public:
	/**
	 * Create the mask from a null terminated ASCII string such as
	 * "255.255.255.128".
	 * 
	 * @param mask null terminated ASCII mask string.
	 */
	InetMaskAddress(const char *mask);

	/**
	 * Masks are usually used to coerce host addresses into a specific
	 * router or class domain.  This can be done by taking the Inet
	 * Host Address object and "and"ing it with an address mask.  This
	 * operation can be directly expressed in C++ through the & operator.
	 * 
	 * @return a internet host address that has been masked.
	 * @param addr host address to be masked by subnet.
	 * @param mask inetnet mask address object to mask by.
	 */
	friend InetHostAddress operator&(const InetHostAddress &addr, 
					 const InetMaskAddress &mask);

        /**
         * Allows assignment from the return of functions like
         * inet_addr() or htonl()
	 */
	InetAddress &operator=(unsigned long addr) 
        { return InetAddress::operator =(addr); }
};

/**
 * This object is used to hold the actual and valid internet address of a 
 * specific host machine that will be accessed through a socket.
 * 
 * @author David Sugar <dyfet@ostel.com>
 * @short Address of a specific Internet host machine.
 */
class InetHostAddress : public InetAddress
{
public: 
	/**
	 * Create a new host address for a specific internet host.  The
	 * internet host can be specified in a null terminated ASCII
	 * string and include either the physical host address or the
	 * DNS name of a host machine.  Hence, an InetHostAddress
	 * ("www.voxilla.org") can be directly declaired in this manner.
	 *
	 * Defaults to the IP address that represents the interface matching
	 * "gethostname()".
	 *
	 * @param host dns or physical address of an Internet host.
	 */
	InetHostAddress(const char *host = NULL);

	/**
	 * Convert a system socket binary address such as may be
	 * returned through the accept() call or getsockpeer() into
	 * an internet host address object.
	 *
	 * @param addr binary address of internet host.
	 */
	InetHostAddress(struct in_addr addr);

        /**
         * Allows assignment from the return of functions like
         * inet_addr() or htonl() 
	 */
	InetAddress &operator=(unsigned long addr) 
        { return InetAddress::operator =(addr); }

	/**
	 * Mask the internet host address object with a network mask address.
	 * This is commonly used to coerce an address by subnet.
	 */
	InetHostAddress &operator&=(const InetMaskAddress &mask);

	friend class InetMaskAddress;
	friend InetHostAddress operator&(const InetHostAddress &addr, 
					 const InetMaskAddress &mask);
};

/**
 * The broadcast address object is used to store the broadcast address for
 * a specific subnet.  This is commonly used for UDP broadcast operations.
 */
class BroadcastAddress : public InetAddress
{
public:
	/**
	 * Specify the physical broadcast address to use and create a new
	 * broadcast address object based on a null terminated ASCII
	 * string.
	 * 
	 * @param net null terminated ASCII network address.
	 */
	BroadcastAddress(const char *net = "255.255.255.255");
};

/**
 * A specialization of InetAddress that provides address validation
 * for multicast addresses. Whenever its value changes the new value
 * is checked to be in the range from 224.0.0.1 through
 * 239.255.255.255. If it is not, an exception is thrown.
 *
 * @short A multicast network address.
 * @author Federico Montesino <p5087@quintero.fie.us.es>
 */
class InetMcastAddress: public InetAddress
{
public:
	/**
	 * Create an Internet Multicast Address object with an empty
	 * (0.0.0.0) address. 
	 */
	InetMcastAddress();

	/**
	 * Convert the system internet address data type (struct in_addr)
	 * into a Common C++ InetMcastAddress object.
	 * 
	 * @param address struct of system used binary internet address.
	 */
	InetMcastAddress(const struct in_addr address);

	/**
	 * Convert a null terminated ASCII multicast address string
	 * (example: "224.0.0.1") or multicast name string (example:
	 * "sap.mcast.net") directly into a Common C++
	 * InetMcastAddress object. Works like InetAddress(const
	 * char*).
	 *
	 * @param address null terminated C string. 
	 */
	InetMcastAddress(const char *address);
	
private:
	/**
	 * Check the address in <code>addr<code> is a valid multicast
	 * address. In case not, throws an exception.
	 *
	 * @param address a system network address 
	 * @return true if validation succeeded
	 */
	static const InetMcastAddrValidator validator;
};

/**
 * The Socket is used as the base for all Internet protocol services
 * under Common C++.  A socket is a system resource (or winsock descriptor)
 * that occupies a specific port address (and may be bound to a specific
 * network interface) on the local machine.  The socket may also be
 * directly connected to a specific socket on a remote internet host.
 * 
 * This base class is not directly used, but is
 * provided to offer properties common to other Common C++ socket classes,
 * including the socket exception model and the ability to set socket
 * properties such as QoS, "sockopts" properties like Dont-Route
 * and Keep-Alive, etc.
 * 
 *
 * @author David Sugar <dyfet@ostel.com>
 * @short base class of all sockets.
 */
class Socket
{
public:
	enum Error
	{
		errSuccess = 0,
		errCreateFailed,
		errCopyFailed,
		errInput,
		errInputInterrupt,
		errResourceFailure,
		errOutput,
		errOutputInterrupt,
		errNotConnected,
		errConnectRefused,
		errConnectRejected,
		errConnectTimeout,
		errConnectFailed,
		errConnectInvalid,
		errConnectBusy,
		errConnectNoRoute,
		errBindingFailed,
		errBroadcastDenied,
		errRoutingDenied,
		errKeepaliveDenied,
		errServiceDenied,
		errServiceUnavailable,
		errMulticastDisabled,
		errTimeout,
		errNoDelay,
		errExtended
	};

	typedef enum Error Error;

	enum Tos
	{
		tosLowDelay = 0,
		tosThroughput,
		tosReliability,
		tosMinCost,
		tosInvalid
	};
	typedef enum Tos Tos;

	enum Pending
	{
		pendingInput,
		pendingOutput,
		pendingError
	};
	typedef enum Pending Pending;

protected:
	enum State
	{
		INITIAL,
		AVAILABLE,
		BOUND,
		CONNECTED,
		CONNECTING,
		STREAM
	};
	typedef enum State State;

private:
	// used by exception handlers....
	mutable Error errid;
	mutable const char *errstr;

	void setSocket(void);
	friend SOCKET dupSocket(SOCKET s,Socket::State state);

protected:
	mutable struct
	{
		bool thrown: 1;
		bool broadcast: 1;
		bool route: 1;
		bool keepalive: 1;
		bool loopback: 1;
		bool multicast: 1;
		bool completion: 1;
		bool linger: 1;
		unsigned ttl: 8;
	} flags;

	/**
	 * the actual socket descriptor, in Windows, unlike posix it 
	 * *cannot* be used as an file descriptor
	 * that way madness lies -- jfc
	 */
	SOCKET so;
	State state;

	/**
	 * This service is used to throw all socket errors which usually
	 * occur during the socket constructor.
	 *
	 * @param error defined socket error id.
	 * @param errstr string or message to pass.
	 */
	Error error(Error error, char *errstr = NULL) const;

	/**
	 * This service is used to throw application defined socket errors
	 * where the application specific error code is a string.
	 *
	 * @param errstr string or message to pass.
	 */
	inline void error(char *errstr) const
		{error(errExtended, errstr);};
	
	/**
	 * This service is used to turn the error handler on or off for
	 * "throwing" exceptions by manipulating the thrown flag.
	 *
	 * @param enable true to enable handler.
	 */
	inline void setError(bool enable)
		{flags.thrown = !enable;};

	/**
	 * Used as the default destructor for ending a socket.  This
	 * will cleanly terminate the socket connection.  It is provided
	 * for use in derived virtual destructors.
	 */
	void endSocket(void);

	/**
	 * Used as a common handler for connection failure processing.
	 *
	 * @return correct failure code to apply.
	 */
	Error connectError(void);

	/**
	 * Set the subnet broadcast flag for the socket.  This enables
	 * sending to a subnet and may require special image privileges
	 * depending on the operating system.
	 *
	 * @return 0 (errSuccess) on success, else error code.
	 * @param enable when set to true.
	 */
	Error setBroadcast(bool enable);

	/**
	 * Setting multicast binds the multicast interface used for
	 * the socket to the interface the socket itself has been
	 * implicitly bound to.  It is also used as a check flag
	 * to make sure multicast is enabled before multicast
	 * operations are used.
	 *
	 * @return 0 (errSuccess) on success, else error code.
	 * @param enable when set to true.
	 */
	Error setMulticast(bool enable);

	/**
	 * Set the multicast loopback flag for the socket.  Loopback
	 * enables a socket to hear what it is sending.
	 *
	 * @return 0 (errSuccess) on success, else error code.
	 * @param enable when set to true.
	 */
	Error setLoopback(bool enable);

	/**
	 * Set the multicast time to live for a multicast socket.
	 *
	 * @return 0 (errSuccess) on success, else error code.
	 * @param ttl time to live.
	 */
	Error setTimeToLive(unsigned char ttl);

	/**
	 * Join a multicast group.
	 *
	 * @return 0 (errSuccess) on success, else error code.
	 * @param ia address of multicast group to join.
	 */
	Error join(const InetMcastAddress &ia);

	/**
	 * Drop membership from a multicast group.
	 *
	 * @return 0 (errSuccess) on success, else error code.
	 * @param ia address of multicast group to drop.
	 */
	Error drop(const InetMcastAddress &ia);

	/**
	 * Set the socket routing to indicate if outgoing messages
	 * should bypass normal routing (set false).
	 *
	 * @return 0 on success.
	 * @param enable normal routing when set to true.
	 */
	Error setRouting(bool enable);


	/**
	 * Enable/disable delaying packets (Nagle algorithm)
	 *
	 * @return 0 on success.
	 * @param enable disable Nagle algorithm when set to true.
	 */
	Error setNoDelay(bool enable);

	/**
	 * An unconnected socket may be created directly on the local
	 * machine.  Sockets can occupy both the internet domain (AF_INET)
	 * and UNIX socket domain (AF_UNIX) under unix.  The socket type
	 * (SOCK_STREAM, SOCK_DGRAM) and protocol may also be specified.
	 * If the socket cannot be created, an exception is thrown.
	 * 
	 * @param domain socket domain to use.
	 * @param type base type and protocol family of the socket.
	 * @param protocol specific protocol to apply.
	 */
	Socket(int domain, int type, int protocol = 0);

	/**
	 * A socket object may be created from a file descriptor when that
	 * descriptor was created either through a socket() or accept()
	 * call.  This constructor is mostly for internal use.
	 * 
	 * @param fd file descriptor of an already existing socket.
	 */
	Socket(SOCKET fd);

	/**
         * A socket can also be constructed from an already existing
         * Socket object. On POSIX systems, the socket file descriptor
         * is dup()'d. On Win32, DuplicateHandle() is used.
	 * 
	 * @param source of existing socket to clone.
	 */
	Socket(const Socket &source);

	/**
	 * Process a logical input line from a socket descriptor
	 * directly.
	 *
	 * @param buf pointer to string.
	 * @param len maximum length to read.
	 * @param timeout for pending data in milliseconds.
	 * @return number of bytes actually read.
	 */
	ssize_t readLine(char *buf, size_t len, timeout_t timeout = 0);

        /**
         * Read in a block of len bytes with specific separator.  Can
         * be zero, or any other char.  If \n or \r, it's treated just
         * like a readLine().  Otherwise it looks for the separator.
         *
         * @param pointer to byte allocation.
         * @param maximum length to read.
         * @param timeout for pending data in milliseconds.
         * @param separator for a particular ASCII character
         * @return number of bytes actually read.
         */
        virtual ssize_t readData(void * buf,size_t len,char separator=0,timeout_t t=0);

        /**
         * Write a block of len bytes to socket.
         *
         * @param pointer to byte allocation.
         * @param maximum length to write.
         * @param timeout for pending data in milliseconds.
         * @return number of bytes actually read.
         */
        virtual ssize_t writeData(const void* buf,size_t len,timeout_t t=0);


public:
	/**
	 * The socket base class may be "thrown" as a result of an
	 * error, and the "catcher" may then choose to destroy the
	 * object.  By assuring the socket base class is a virtual
	 * destructor, we can assure the full object is properly
	 * terminated.
	 */
	virtual ~Socket();

	/**
	 * Sockets may also be duplicated by the assignment operator.
	 */
	Socket &operator=(const Socket &from);

	/**
	 * May be used to examine the origin of data waiting in the
	 * socket receive queue.  This can tell a TCP server where pending
	 * "connect" requests are coming from, or a UDP socket where it's
	 * next packet arrived from.
	 *
	 * @param port ptr to port number of sender.
	 * @return host address, test with "isInetAddress()".
	 */
	InetHostAddress getSender(tpport_t *port = NULL) const;

	/**
	 * Get the host address and port of the socket this socket
	 * is connected to.  If the socket is currently not in a
	 * connected state, then a host address of 0.0.0.0 is
	 * returned.
	 *
	 * @param port ptr to port number of remote socket.
	 * @return host address of remote socket.
	 */
	InetHostAddress getPeer(tpport_t *port = NULL) const;

	/**
	 * Get the local address and port number this socket is
	 * currently bound to.
	 *
	 * @param port ptr to port number on local host.
	 * @return host address of interface this socket is bound to.
	 */
	InetHostAddress getLocal(tpport_t *port = NULL) const;
	
	/**
	 * Used to specify blocking mode for the socket.  A socket
	 * can be made non-blocking by setting setCompletion(false)
	 * or set to block on all access with setCompletion(true).
	 * I do not believe this form of non-blocking socket I/O is supported
	 * in winsock, though it provides an alternate asynchronous set of
	 * socket services.
	 * 
	 * @param immediate mode specify socket I/O call blocking mode.
	 */
	void setCompletion(bool immediate);

	/**
	 * Enable lingering sockets on close.
	 *
	 * @param linger specify linger enable.
	 */
	Error setLinger(bool linger);

	/**
	 * Set the keep-alive status of this socket and if keep-alive
	 * messages will be sent.
	 *
	 * @return 0 on success.
	 * @param enable keep alive messages.
	 */
	Error setKeepAlive(bool enable);

	/**
	 * Set packet scheduling on platforms which support ip quality
	 * of service conventions.  This effects how packets in the
	 * queue are scheduled through the interface.
	 *
	 * @return 0 on success, error code on failure.
	 * @param service type of service enumerated type.
	 */
	Error setTypeOfService(Tos service);

	/**
	 * Can test to see if this socket is "connected", and hence
	 * whether a "catch" can safely call getPeer().  Of course,
	 * an unconnected socket will return a 0.0.0.0 address from
	 * getPeer() as well.
	 *
	 * @return true when socket is connected to a peer.
	 */
	bool isConnected(void) const;

	/**
	 * Test to see if the socket is at least operating or if it
	 * is mearly initialized.  "initialized" sockets may be the
	 * result of failed constructors.
	 *
	 * @return true if not in initial state.
	 */
	bool isActive(void) const;

	/**
	 * Operator based testing to see if a socket is currently
	 * active.
	 */
	bool operator!() const;

	/**
	 * Return if broadcast has been enabled for the specified
	 * socket.
	 *
	 * @return true if broadcast socket.
	 */
	inline bool isBroadcast(void) const
		{return flags.broadcast;};

	/**
	 * Return if socket routing is enabled.
	 *
	 * @return true if routing enabled.
	 */
	inline bool isRouted(void) const
		{return flags.route;};

	/**
 	 * Often used by a "catch" to fetch the last error of a thrown
	 * socket.
	 * 
	 * @return error number of Error error.
 	 */
	inline Error getErrorNumber(void) const {return errid;}
	
	/**
 	 * Often used by a "catch" to fetch the user set error string
	 * of a thrown socket, but only if EXTENDED error codes are used.
	 * 
	 * @return string for error message.
 	 */
	inline const char *getErrorString(void) const {return errstr;}

	/**
	 * Get the status of pending operations.  This can be used to
	 * examine if input or output is waiting, or if an error has
	 * occured on the descriptor.
	 *
	 * @return true if ready, false on timeout.
	 * @param pend ready check to perform.
	 * @param timeout in milliseconds, inf. if not specified.
	 */
	virtual bool isPending(Pending pend, timeout_t timeout = TIMEOUT_INF);
};

/**
 * UDP sockets implement the TCP SOCK_DGRAM UDP protocol.  They can be
 * used to pass unverified messages between hosts, or to broadcast a
 * specific message to an entire subnet.  Please note that Streaming of
 * realtime data commonly use UDPDuplex related classes rather than
 * UDPSocket.
 * 
 * In addition to connected TCP sessions, Common C++ supports UDP sockets and
 * these also cover a range of functionality.  Like a TCPSocket, A UDPSocket
 * can be created bound to a specific network interface and/or port address,
 * though this is not required.  UDP sockets also are usually either 
 * connected or otherwise "associated" with a specific "peer" UDP socket.
 * Since UDP sockets operate through discreet packets, there are no streaming
 * operators used with UDP sockets.
 * 
 * In addition to the UDP "socket" class, there is a "UDPBroadcast" class.
 * The UDPBroadcast is a socket that is set to send messages to a subnet as a
 * whole rather than to an individual peer socket that it may be associated
 * with.
 * 
 * UDP sockets are often used for building "realtime" media  streaming
 * protocols and full duplex messaging services.  When used in this manner,
 * typically a pair of UDP sockets are used together; one socket is used to
 * send and the other to receive data with an associated pair of UDP sockets
 * on a "peer" host.  This concept is represented through the Common C++
 * UDPDuplex object, which is a pair of sockets that communicate with another
 * UDPDuplex pair.
 * 
 * 
 * @author David Sugar <dyfet@ostel.com>
 * @short Unreliable Datagram Protocol sockets.
 */
class UDPSocket : public Socket
{
private:
	inline Error setKeepAlive(bool enable)
		{return Socket::setKeepAlive(enable);};

protected:
	struct sockaddr_in peer;

public:
	/**
	 * Create an unbound UDP socket, mostly for internal use.
	 */
	UDPSocket(void);

	/**
	 * Create a UDP socket and bind it to a specific interface
	 * and port address so that other UDP sockets on remote
	 * machines (or the same host) may find and send UDP messages
	 * to it.  On failure to bind, an exception is thrown.
	 * 
	 * @param bind address to bind this socket to.
	 * @param port number to bind this socket to.
	 */
	UDPSocket(const InetAddress &bind, tpport_t port);

	/**
	 * Destroy a UDP socket as a socket.
	 */
	virtual ~UDPSocket();

	/**
	 * set the peer address to send message packets to.  This can be
	 * set before every send() call if nessisary.
	 *
	 * @param host address to send packets to.
	 * @param port number to deliver packets to.
	 */
	void setPeer(const InetHostAddress &host, tpport_t port);

        /**
         * get the interface index for a named network device
         *
         * @param ethX is device name, like "eth0" or "eth1"
         * @param InterfaceIndex is the index value returned by os
	 * @todo Win32 implementation.
         */
        Socket::Error getInterfaceIndex(const char *ethX,int& InterfaceIndex);

        /**
         * join a multicast group on a particular interface
         *
         * @param ia is the multicast address to use
         * @param InterfaceIndex is the index value returned by 
	 * getInterfaceIndex
	 * @todo Win32 implementation.
         */
        Socket::Error join(const InetMcastAddress &ia,int InterfaceIndex);


	/**
	 * Send a message packet to a peer host.
	 *
	 * @param buf pointer to packet buffer to send.
	 * @param len of packet buffer to send.
	 * @return number of bytes sent.
	 */
	inline int send(const void *buf, size_t len)
		{return ::sendto(so, (const char*)buf, len, 0, (struct sockaddr *)&peer, (socklen_t)sizeof(peer));};

	/**
	 * Receive a message from any host.
	 *
	 * @param buf pointer to packet buffer to receive.
	 * @param len of packet buffer to receive.
	 * @return number of bytes received.
	 */
	inline int receive(void *buf, size_t len)
		{return ::recv(so, (char *)buf, len, 0);};

	/**
	 * Examine address of sender of next waiting packet.  This also
	 * sets "peer" address to the sender so that the next "send"
	 * message acts as a "reply".  This additional behavior overides
	 * the standard socket getSender behavior.
	 *
	 * @param port pointer to hold port number.
	 */
	InetHostAddress getPeer(tpport_t *port = NULL) const;

	/**
	 * Examine contents of next waiting packet.
	 *
	 * @param buf pointer to packet buffer for contents.
	 * @param len of packet buffer.
	 * @return number of bytes examined.
	 */
	inline int peek(void *buf, size_t len)
		{return ::recv(so, (char *)buf, len, MSG_PEEK);};
};


/**
 * Representing a UDP socket used for subnet broadcasts, this class
 * provides an alternate binding and setPeer() capability for UDP
 * sockets.
 *
 * @author David Sugar <dyfet@ostel.com>
 * @short Unreliable Datagram for subnet broadcasts.
 */
class UDPBroadcast : public UDPSocket
{
private:
	void setPeer(const InetHostAddress &ia, tpport_t port) {};

	Error setBroadcast(bool enable)
		{return Socket::setBroadcast(enable);};

public:
	/**
	 * Create and bind a subnet broadcast socket.
	 *
	 * @param ia address to bind socket under locally.
	 * @param port to bind socket under locally.
	 */
	UDPBroadcast(const InetAddress &ia, tpport_t port);

	/**
	 * Set peer by subnet rather than specific host.
	 *
	 * @param subnet of peer hosts to send to.
	 * @param port number to use.
	 */
	void setPeer(const BroadcastAddress &subnet, tpport_t port);
};	

/**
 * Representing half of a two-way UDP connection, the UDP transmitter
 * can broadcast data to another selected peer host or to an entire
 * subnet.
 *
 * @author David Sugar <dyfet@ostel.com>
 * @short Unreliable Datagram Peer Associations.
 */
class UDPTransmit : protected UDPSocket
{
private:
	/**
	 * Common code for diferent flavours of Connect (host, broadcast,
	 * multicast).
	 *
	 * @param ia network address to associate with
	 * @param port port number to associate with
	 */
	Error cConnect(const InetAddress &ia, tpport_t port);

protected:
	/**
	 * Create a UDP transmitter.
	 */
	UDPTransmit();

	/**
	 * Create a UDP transmitter, bind it to a specific interface
	 * and port address so that other UDP sockets on remote
	 * machines (or the same host) may find and send UDP messages
	 * to it, and associate it with a given port on a peer host.  
         * On failure to bind, an exception is thrown.  This class is
	 * only used to build the UDP Duplex.
	 * 
	 * @param bind address to bind this socket to.
	 * @param port number to bind this socket to.
	 */
	UDPTransmit(const InetAddress &bind, tpport_t port = 5005);

	/**
	 * Associate this socket with a specified peer host.  The port
	 * number from the constructor will be used.  All UDP packets
	 * will be sent to and received from the specified host.
	 *
	 * @return 0 on success, -1 on error.
	 * @param host address to connect socket to.
	 * @param port to connect socket to.
	 */
	Error connect(const InetHostAddress &host, tpport_t port);

	/**
	 * Associate this socket with a subnet of peer hosts for
	 * subnet broadcasting.  The server must be able to assert
	 * broadcast permission for the socket.
	 *
	 * @return 0 on success, -1 on error.
	 * @param subnet subnet address to broadcast into.
	 * @param port transport port to broadcast into.
	 */
	Error connect(const BroadcastAddress &subnet, tpport_t port);

	/**
	 * Associate this socket with a multicast group.
	 *
	 * @return 0 on success, -1 on error.
	 * @param mgroup address of the multicast group to send to.
	 * @param port port number
	 */
	Error connect(const InetMcastAddress &mgroup, tpport_t port);

	/**
	 * Disassociate this socket from any host connection.  No data
	 * should be read or written until a connection is established.
	 */
	Error disconnect(void);

	/**
	 * Transmit "send" to use "connected" send rather than sendto.
	 *
	 * @return number of bytes sent.
	 * @param buf address of buffer to send.
	 * @param len of bytes to send.
	 */
	inline int send(const void *buf, int len)
		{return ::send(so, (const char *)buf, len, 0);}

	/**
	 * Stop transmitter.
	 */
	inline void endTransmitter(void)
		{Socket::endSocket();}

	/*
	 * Get transmitter socket.
	 *
	 * @return transmitter.
	 */
	inline SOCKET getTransmitter(void)
		{return so;};

	inline Error setMulticast(bool enable)
		{return Socket::setMulticast(enable);};

	inline Error setTimeToLive(unsigned char ttl)
		{return Socket::setTimeToLive(ttl);};

public:
	/**
	 * Transmit "send" to use "connected" send rather than sendto.
	 *
	 * @note Windows does not support MSG_DONTWAIT, so it is defined
	 *	 as 0 on that platform.
	 * @return number of bytes sent.
	 * @param buffer address of buffer to send.
	 * @param len of bytes to send.
	 */
	inline int transmit(const char *buffer, size_t len)
		{return ::send(so, buffer, len, MSG_DONTWAIT);}

	/**
	 * See if output queue is empty for sending more packets.
	 *
	 * @return true if output available.
	 * @param timeout in milliseconds to wait.
	 */
	inline bool isOutputReady(unsigned long timeout = 0l)
		{return Socket::isPending(Socket::pendingOutput, timeout);};


	inline Error setRouting(bool enable)
		{return Socket::setRouting(enable);};

	inline Error setTypeOfService(Tos tos)
		{return Socket::setTypeOfService(tos);};

	inline Error setBroadcast(bool enable)
		{return Socket::setBroadcast(enable);};
};

/**
 * Representing half of a two-way UDP connection, the UDP receiver
 * can receive data from another peer host or subnet.  This class is
 * used exclusivily to derive the UDPDuplex.
 *
 * @author David Sugar <dyfet@ostel.com>
 * @short Unreliable Datagram Peer Associations.
 */
class UDPReceive : protected UDPSocket
{
protected:
	/**
	 * Create a UDP receiver, bind it to a specific interface
	 * and port address so that other UDP sockets on remote
	 * machines (or the same host) may find and send UDP messages
	 * to it, and associate it with a given port on a peer host.
         * On failure to bind, an exception is thrown.
	 *
	 * @param bind address to bind this socket to.
	 * @param port number to bind this socket to.
	 */
	UDPReceive(const InetAddress &bind, tpport_t port);

	/**
	 * Associate this socket with a specified peer host.  The port
	 * number from the constructor will be used.  All UDP packets
	 * will be sent received from the specified host.
	 *
	 * @return 0 on success, -1 on error.
	 * @param host host network address to connect socket to.
	 * @param port host transport port to connect socket to.
	 */
	Error connect(const InetHostAddress &host, tpport_t port);

	/**
	 * Disassociate this socket from any host connection.  No data
	 * should be read or written until a connection is established.
	 */
	Error disconnect(void);

	/**
	 * Check for pending data.
	 *
	 * @return true if data is waiting.
	 * @param timeout in milliseconds.
	 */
	bool isPendingReceive(timeout_t timeout)
		{return Socket::isPending(Socket::pendingInput, timeout);};

	/**
	 * End receiver.
	 */
	inline void endReceiver(void)
		{Socket::endSocket();}

	inline SOCKET getReceiver(void) const
		{return so;};

	inline Error setRouting(bool enable)
		{return Socket::setRouting(enable);};

	inline Error setMulticast(bool enable)
		{return Socket::setMulticast(enable);};

	inline Error join(const InetMcastAddress &ia)
	        {return Socket::join(ia);}

	inline Error drop(const InetMcastAddress &ia)
	        {return Socket::drop(ia);}

public:
	/**
	 * Receive a data packet from the connected peer host.
	 *
	 * @return num of bytes actually received.
	 * @param buf address of data receive buffer.
	 * @param len size of data receive buffer.
	 */
	inline int receive(void *buf, size_t len)
		{return ::recv(so, (char *)buf, len, 0);};

	/**
	 * See if input queue has data packets available.
	 *
	 * @return true if data packets available.
	 * @param timeout in milliseconds. TIMEOUT_INF if not specified.
	 */
	inline bool isInputReady(timeout_t timeout = TIMEOUT_INF)
		{return Socket::isPending(Socket::pendingInput, timeout);};
};

/**
 * UDP duplex connections impliment a bi-directional point-to-point UDP
 * session between two peer hosts.  Two UDP sockets are typically used
 * on alternating port addresses to assure that sender and receiver
 * data does not collide or echo back.  A UDP Duplex is commonly used
 * for full duplex real-time streaming of UDP data between hosts.
 *
 * @author David Sugar <dyfet@ostel.com>
 * @short Unreliable Datagram Peer Associations.
 */
class UDPDuplex : public UDPTransmit, public UDPReceive
{
public:
	/**
	 * Create a UDP duplex as a pair of UDP simplex objects
         * bound to alternating and interconnected port addresses.
	 *
	 * @param bind address to bind this socket to.
	 * @param port number to bind sender.
	 */
	UDPDuplex(const InetAddress &bind, tpport_t port);

	/**
	 * Associate the duplex with a specified peer host. Both
	 * the sender and receiver will be interconnected with
	 * the remote host.
	 *
	 * @return 0 on success, error code on error.
	 * @param host address to connect socket to.
	 * @param port number to connect socket to.
	 */
	Error connect(const InetHostAddress &host, tpport_t port);

	/**
	 * Disassociate this duplex from any host connection.  No data
	 * should be read or written until a connection is established.
	 *
	 * @return 0 on success, error code on error.
	 */
	Error disconnect(void);
};


/**
 * TCP sockets are used for stream based connected sessions between two
 * sockets.  Both error recovery and flow control operate transparently
 * for a TCP socket connection.  The TCP socket base class is primary used
 * to bind a TCP "server" for accepting TCP streams.
 * 
 * An implicit and unique TCPSocket object exists in Common C++ to represent
 * a bound TCP socket acting as a "server" for receiving connection requests.
 * This class is not part of TCPStream because such objects normally perform
 * no physical I/O (read or write operations) other than to specify a listen
 * backlog queue and perform "accept" operations for pending connections.
 * The Common C++ TCPSocket offers a Peek method to examine where the next
 * pending connection is coming from, and a Reject method to flush the next
 * request from the queue without having to create a session.
 * 
 * The TCPSocket also supports a "OnAccept" method which can be called when a
 * TCPStream related object is created from a TCPSocket.  By creating a
 * TCPStream from a TCPSocket, an accept operation automatically occurs, and
 * the TCPSocket can then still reject the client connection through the
 * return status of it's OnAccept method.
 * 
 * @author David Sugar <dyfet@tycho.com>
 * @short bound server for TCP streams and sessions.
 */
class TCPSocket : protected Socket
{
protected:
	/**
	 * A method to call in a derived TCPSocket class that is acting
	 * as a server when a connection request is being accepted.  The
	 * server can implement protocol specific rules to exclude the
	 * remote socket from being accepted by returning false.  The
	 * Peek method can also be used for this purpose.
	 * 
	 * @return true if client should be accepted.
	 * @param ia internet host address of the client.
	 * @param port number of the client.
	 */
	virtual bool onAccept(const InetHostAddress &ia, tpport_t port)
		{return true;};

	friend class TCPStream;
	friend class SocketPort;
	friend class tcpstream;

public:
	/**
	 * A TCP "server" is created as a TCP socket that is bound
	 * to a hardware address and port number on the local machine
	 * and that has a backlog queue to listen for remote connection
	 * requests.  If the server cannot be created, an exception is
	 * thrown.
	 * 
	 * @param bind local ip address or interface to use.
	 * @param port number to bind socket under.
	 * @param backlog size of connection request queue.
	 */
	TCPSocket(const InetAddress &bind, tpport_t port, int backlog = 5);
	
	/**
	 * Return address and port of next connection request.  This
	 * can be used instead of OnAccept() to pre-evaluate connection
	 * requests.
	 *
	 * @return host requesting a connection.
	 * @param port number of requestor.
	 */
	inline InetHostAddress getRequest(tpport_t *port = NULL) const
		{return Socket::getSender(port);};

	/**
	 * Used to reject the next incoming connection request.
	 */
	void reject(void);

	/**
	 * Used to get local bound address.
	 */
	inline InetHostAddress getLocal(tpport_t *port = NULL) const
		{return Socket::getLocal(port);};

	/**
	 * Used to wait for pending connection requests.
	 * @return true if data packets available.
	 * @param timeout in milliseconds. TIMEOUT_INF if not specified.
	 */
	inline bool isPendingConnection(timeout_t timeout = TIMEOUT_INF) /* not const -- jfc */
		{return Socket::isPending(Socket::pendingInput, timeout);}

	/**
	 * Use base socket handler for ending this socket.
	 */
	virtual ~TCPSocket()
		{endSocket();};
};

/*
:\projects\libraries\cplusplus\commonc++\win32\socket.h(357) : warning C4275: non dll-interface class 'streambuf' used as base for dll-interface class 'TCPStream'
        c:\program files\microsoft visual studio\vc98\include\streamb.h(69) : see declaration of 'streambuf'
c:\projects\libraries\cplusplus\commonc++\win32\socket.h(358) : warning C4275: non dll-interface class 'iostream' used as base for dll-interface class 'TCPStream'
        c:\program files\microsoft visual studio\vc98\include\iostream.h(66) : see declaration of 'iostream'
*/

#ifdef _MSC_VER
#pragma warning(disable:4275) // disable C4275 warning
#endif

/**
 * TCP streams are used to represent TCP client connections to a server
 * by TCP protocol servers for accepting client connections.  The TCP
 * stream is a C++ "stream" class, and can accept streaming of data to
 * and from other C++ objects using the << and >> operators.
 * 
 *  TCPStream itself can be formed either by connecting to a bound network
 *  address of a TCP server, or can be created when "accepting" a
 *  network connection from a TCP server.
 *
 * @author David Sugar <dyfet@ostel.com>
 * @short streamable TCP socket connection.
 */
#if defined(__KCC)
#define iostream iostream_withassign
using std::iostream;
#endif
class TCPStream : protected std::streambuf, public Socket, public std::iostream
{
private:
	inline Error setBroadcast(bool enable)
		{return Socket::setBroadcast(enable);};

	inline InetHostAddress getSender(tpport_t *port) const
		{return InetHostAddress();};

	int doallocate();

	friend TCPStream& crlf(TCPStream&);
	friend TCPStream& lfcr(TCPStream&);

protected:
	timeout_t timeout;
	int bufsize;
	char *gbuf, *pbuf;

	/**
	 * The constructor required for "tcpstream", a more C++ style
	 * version of the TCPStream class.
	 */
	TCPStream(bool throwflag = true);

	/**
	 * Used to allocate the buffer space needed for iostream
	 * operations.  This function is called by the constructor.
	 *
	 * @param size of stream buffers from constructor.
	 */
	void allocate(int size);

	/**
	 * Used to terminate the buffer space and cleanup the socket
	 * connection.  This fucntion is called by the destructor.
	 */
	void endStream(void);

	/**
	 * This streambuf method is used to load the input buffer
	 * through the established tcp socket connection.
	 *
	 * @return char from get buffer, EOF if not connected.
	 */
	int underflow();

	/**
	 * This streambuf method is used for doing unbuffered reads
	 * through the establish tcp socket connection when in interactive mode.
	 * Also this method will handle proper use of buffers if not in
	 * interative mode.
	 *
	 * @return char from tcp socket connection, EOF if not connected.
	 */
	int uflow();

	/**
	 * This streambuf method is used to write the output
	 * buffer through the established tcp connection.
	 *
	 * @param ch char to push through.
	 * @return char pushed through.
	 */
	int overflow(int ch);

	/**
	 * Create a TCP stream by connecting to a TCP socket (on
	 * a remote machine).
	 *
	 * @param host address of remote TCP server.
	 * @param port number to connect.
	 * @param size of streaming input and output buffers.
	 */
	void connect(const InetHostAddress &host, tpport_t port, int size);

	/**
	 * Used in derived classes to refer to the current object via
	 * it's iostream.  For example, to send a set of characters
	 * in a derived method, one might use *tcp() << "test".
	 *
	 * @return stream pointer of this object.
	 */
	std::iostream *tcp(void)
		{return ((std::iostream *)this);};

public:
	/**
	 * Create a TCP stream by accepting a connection from a bound
	 * TCP socket acting as a server.  This performs an "accept"
	 * call.
	 *
	 * @param server socket listening
	 * @param size of streaming input and output buffers.
	 * @param throwflag flag to throw errors.
	 * @param timeout for all operations.
	 */
	TCPStream(TCPSocket &server, int size = 512, bool throwflag = true, timeout_t timeout = 0);

	/**
	 * Create a TCP stream by connecting to a TCP socket (on
	 * a remote machine).
	 *
	 * @param host address of remote TCP server.
	 * @param port number to connect.
	 * @param size of streaming input and output buffers.
	 * @param throwflag flag to throw errors.
	 * @param timeout for all operations.
	 */
	TCPStream(const InetHostAddress &host, tpport_t port, int size = 512, bool throwflag = true, timeout_t timeout = 0);

	/**
	 * Set the I/O operation timeout for socket I/O operations.
	 *
	 * @param to timeout to set.
	 */
	inline void setTimeout(timeout_t to)
		{timeout = to;};

	/**
	 * A copy constructor creates a new stream buffer.
	 *
	 * @param source of copy.
	 *
	 */
	TCPStream(const TCPStream &source);

	/**
	 * Flush and empty all buffers, and then remove the allocated
	 * buffers.
	 */
	virtual ~TCPStream()
		{
		try { endStream(); }
		catch( ... ) { if ( ! std::uncaught_exception()) throw; }
		};

	/**
	 * Flushes the stream input and output buffers, writes
	 * pending output.
	 *
	 * @return 0 on success.
	 */
	int sync(void);

	/**
	 * Get the status of pending stream data.  This can be used to
	 * examine if input or output is waiting, or if an error or
	 * disconnect has occured on the stream.  If a read buffer
	 * contains data then input is ready and if write buffer
	 * contains data it is first flushed and then checked.
	 */
	bool isPending(Pending pend, timeout_t timeout = TIMEOUT_INF);

	/**
	 * Return the size of the current stream buffering used.
	 *
	 * @return size of stream buffers.
	 */
	int getBufferSize(void) const
		{return bufsize;};
};

/**
 * A more natural C++ "tcpstream" class for use by non-threaded
 * applications.  This class behaves a lot more like fstream and
 * similar classes.
 *
 * @author David Sugar <dyfet@ostel.com>
 * @short C++ "fstream" style tcpstream class.
 */
class tcpstream : public TCPStream
{
public:
	// copy constructor (fix a BUG in msvc7 compiler)
	tcpstream(const tcpstream &rhs):TCPStream(rhs) {};

	/**
	 * Construct an unopened "tcpstream" object.
	 */
	tcpstream();

	/**
	 * Construct and "open" (connect) the tcp stream to a remote
	 * socket.
	 *
	 * @param addr string address in form addr:port.
	 * @param buffer size for streaming (optional).
	 */
	tcpstream(const char *addr, int buffer = 512);

	/**
	 * Construct and "accept" (connect) the tcp stream through
	 * a server.
	 *
	 * @param tcp socket to accept from.
	 * @param buffer size for streaming (optional).
	 */
	tcpstream(TCPSocket &tcp, int buffer = 512);

	/**
	 * Open a tcp stream connection.  This will close the currently
	 * active connection first.
	 *
	 * @param addr string address in form addr:port.
	 * @param buffer size for streaming (optional)
	 */
	void open(const char *addr, int buffer = 512);

	/**
	 * Open a tcp stream connection by accepting a tcp socket.
	 *
	 * @param tcp socket to accept from.
	 * @param buffer size for streaming (optional)
	 */
	void open(TCPSocket &tcp, int buffer = 512);

	/**
	 * Close the active tcp stream connection.
	 */
	void close(void);

	/**
	 * Test to see if stream is open.
	 */
	bool operator!() const;
};		

/**
 * The TCP session is used to primarily to represent a client connection
 * that can be managed on a seperate thread.  The TCP session also supports
 * a non-blocking connection scheme which prevents blocking during the
 * constructor and moving the process of completing a connection into the 
 * thread that executes for the session.
 * 
 * @author David Sugar <dyfet@ostel.com>
 * @short Threaded streamable socket with non-blocking constructor.
 */
class TCPSession : public TCPStream, public Thread
{
private:
	TCPSession(const TCPSession &rhs); // not defined
protected:
	/**
	 * Normally called during the thread Initial() method by default,
	 * this will wait for the socket connection to complete when
	 * connecting to a remote socket.  One might wish to use
	 * setCompletion() to change the socket back to blocking I/O
	 * calls after the connection completes.  To implement the
	 * session one must create a derived class which implements
	 * run().
	 * 
	 * @return 0 if successful, -1 if timed out.
	 * @param timeout to wait for completion in milliseconds.
	 */
	int waitConnection(timeout_t timeout = TIMEOUT_INF);

	/**
	 * The initial method is used to esablish a connection when
	 * delayed completion is used.  This assures the constructor
	 * terminates without having to wait for a connection request
	 * to complete.
	 */
	CCXX_MEMBER(void) initial(void);

	/**
	 * TCPSession derived objects can be freely created with "new"
	 * and safely terminate their "run" method on their own by
	 * self-deleting when the thread terminates.
         *
         * @note Since a thread cannot delete its own context or join
         * itself, to make a TCPSession that is a self running object
         * that self-deletes, one has to detach the associated thread
         * by using detach() instead of start().
	 */
	CCXX_MEMBER(void) final(void)
		{delete this;};
public:
	/**
	 * Create a TCP socket that will be connected to a remote TCP
	 * server and that will execute under it's own thread.
	 * 
	 * @param host internet address of remote TCP server.
	 * @param port number of remote server.
	 * @param size of streaming buffer.
	 * @param pri execution priority relative to parent.
	 * @param stack allocation needed on some platforms.
	 */
	TCPSession(const InetHostAddress &host, 
		   tpport_t port, int size = 512, int pri = 0, int stack = 0);

	/**
	 * Create a TCP socket from a bound TCP server by accepting a pending
	 * connection from that server and execute a thread for the accepted
	 * connection.
	 * 
	 * @param server tcp socket to accept a connection from.
	 * @param size of streaming buffer.
	 * @param pri execution priority relative to parent.
	 * @param stack allocation needed on some platforms.
	 */
	TCPSession(TCPSocket &server, int size = 512, 
		   int pri = 0, int stack = 0);
};

extern CCXX_EXPORT(std::ostream&) operator<<(std::ostream &os, const InetAddress &ia);

inline struct in_addr getaddress(const InetAddress &ia)
	{return ia.getAddress();}

#if defined(WIN32)

/**
 *  class init_WSA used to initalise windows sockets specfifc stuff : there is
 *  an MS - specific init sequence for Winsock 2 this class attempts to 
 *  initalise Winsock 2.2 - needed for non - blocking I/O. It will fall back 
 *  on 1.2 or lower if 2.0 or higher is not available,  but < 2.0 does not 
 *  support non - blocking I/o
 *  TO DO : might be an idea to have a method that reports version of 
 *  Winsock in use or a predicate to test if non - blocking is OK -- JFC
 */

class init_WSA
{
public:
	init_WSA();
	~init_WSA();
};

#endif // WIN32

class SocketService;

/**
 * The socket port is an internal class which is attached to and then
 * serviced by a specific SocketService "object".  Derived versions of
 * this class offer specific functionality for specific protocols.  Both
 * Common C++ supporting frameworks and application objects may be derived
 * from related protocol specific base classes.
 * 
 * A special set of classes, "SocketPort" and "SocketService", exist
 * for building realtime streaming media servers on top of UDP and TCP
 * protocols.  The "SocketPort" is used to hold a connected or associated TCP
 * or UDP socket which is being "streamed" and which offers callback methods
 * that are invoked from a "SocketService" thread.  SocketService's can be
 * pooled into logical thread pools that can service a group of SocketPorts.
 * A millisecond accurate "timer" is associated with each SocketPort and can
 * be used to time synchronize SocketPort I/O operations.
 *
 * @author David Sugar <dyfet@ostel.com>
 * @short base class for realtime and thread pool serviced protocols.
 */
class SocketPort : public Socket, public TimerPort
{
private:
	SocketPort *next, *prev;
	SocketService *service;
#ifndef WIN32
	struct timeval porttimer;
#ifdef CCXX_USE_POLL
	struct pollfd	* ufd;
#endif
#else
	HANDLE event;
#endif
	bool detect_pending;
	bool detect_output;
	bool detect_disconnect;
	
	friend class SocketService;

protected:
	/** 
	 * Construct an accepted TCP socket connection from a specific
	 * bound TCP server.  This is meant to derive advanced application
	 * specific TCP servers that can be thread pooled.
	 *
	 * @param svc pool thread object.
	 * @param tcp socket object to accept.
	 */
	SocketPort(SocketService *svc, TCPSocket &tcp);

	/**
	 * Construct a bound UDP socket for use in deriving realtime
	 * UDP streaming protocols handled by thread pool objects.
	 *
	 * @param svc pool thread object.
	 * @param ia address of interface to bind.
	 * @param port number to bind to.
	 */
	SocketPort(SocketService *svc, const InetAddress &ia, tpport_t port);

	/**
	 * A non-blocking constructor for outbound tcp connections.
	 * To detect when the connection is established, overload
	 * SocketPort::output().  SocketPort::output() get's called by
	 * the SocketService when the connection is ready,
	 * SocketPort::disconnect() when the connect failed.  at the
	 * moment you should set the socket state to "CONNECTED" when
	 * SocketPort::output() get's called for the first time.
	 *
	 * @param svc pool thread object.
	 * @param ih addess to connect to.
	 * @param port number to connect to.
	 **/
	SocketPort(SocketService *svc, const InetHostAddress &ih, tpport_t port);

	/** 
	 * Attach yourself to the service pool thread object. The later version.
	 *
	 * @param svc pool thread object 
	 */	 
	 void attach( SocketService* svc );


	/**
	 * Disconnect the socket from the service thread pool and
	 * the remote connection.
	 */	
	virtual ~SocketPort();

	/**
	 * Used to indicate if the service thread should monitor pending
	 * data for us.
	 */
	void setDetectPending( bool );
	
	/**
	 * Get the current state of the DetectPending flag.
	 */
	bool getDetectPending( void ) const
		{ return detect_pending; }
	
	/**
	 * Used to indicate if output ready monitoring should be performed
	 * by the service thread.
	 */
	void setDetectOutput( bool );
	
	/**
	 * Get the current state of the DetectOutput flag.
	 */
	bool getDetectOutput( void ) const
		{ return detect_output; }

	/**
	 * Called by the service thread pool when the objects timer
	 * has expired.  Used for timed events.
	 */
	virtual void expired(void)
		{return;};

	/**
	 * Called by the service thread pool when input data is pending
	 * for this socket.
	 */
	virtual void pending(void)
		{return;};

	/**
	 * Called by the service thread pool when output data is pending
	 * for this socket.
	 */
	virtual void output(void)
		{return;};

	/**
	 * Called by the service thread pool when a disconnect has
	 * occured.
	 */
	virtual void disconnect(void)
		{return;};

	/**
	 * Connect a Socket Port to a known peer host.  This is normally
	 * used with the UDP constructor.  This is also performed as a
	 * non-blocking operation under Posix systems to prevent delays
	 * in a callback handler.
	 *
	 * @return 0 if successful.
	 * @param ia address of remote host or subnet.
	 * @param port number of remote peer(s).
	 */
	Error connect(const InetAddress &ia, tpport_t port);

	/**
	 * Transmit "send" data to a connected peer host.  This is not
	 * public by default since an overriding protocol is likely to
	 * be used in a derived class.
	 *
	 * @return number of bytes sent.
	 * @param buf address of buffer to send.
	 * @param len of bytes to send.
	 */
	inline int send(const void *buf, int len)
		{return ::send(so, (const char *)buf, len, 0);};

	/**
	 * Receive a message from any host.  This is used in derived
	 * classes to build protocols.
	 *
	 * @param buf pointer to packet buffer to receive.
	 * @param len of packet buffer to receive.
	 * @return number of bytes received.
	 */
	inline int receive(void *buf, size_t len)
		{return ::recv(so, (char *)buf, len, 0);};

	/**
	 * Examine the content of the next packet.  This can be used
	 * to build "smart" line buffering for derived TCP classes.
	 *
	 * @param buf pointer to packet buffer to examine.
	 * @param len of packet buffer to examine.
	 * @return number of bytes actually available.
	 */
	inline int peek(void *buf, size_t len)
		{return ::recv(so, (char *)buf, len, MSG_PEEK);};

public:
	/**
	 * Derived setTimer to notify the service thread pool of change
	 * in expected timeout.  This allows SocketService to 
	 * reschedule all timers.  Otherwise same as TimerPort.
	 *
	 * @param timeout in milliseconds.
	 */
	void setTimer(timeout_t timeout = 0);

	/**
	 * Derived incTimer to notify the service thread pool of a
	 * change in expected timeout.  This allows SocketService to
	 * reschedule all timers.  Otherwise same as TimerPort.
	 *
	 * @param timeout in milliseconds.
	 */
	void incTimer(timeout_t timeout);
};

/**
 * The SocketService is a thread pool object that is meant to service
 * attached socket ports.  Multiple pool objects may be created and
 * multiple socket ports may be attached to the same thread of execution.
 * This allows one to balance threads and sockets they service rather than
 * either using a single thread for all connections or a seperate thread
 * for each connection.  Features can be added through supported virtual
 * methods.
 *
 * @author David Sugar <dyfet@ostel.com>
 * @short Thread pool service object for socket ports.
 */
class SocketService : public Thread, private Mutex
{
private:
#ifndef WIN32
	fd_set connect;
	int iosync[2];
	int hiwater;
#else
	// private syncronization class
	class Sync;
	Sync* sync;
#endif
	int count;
	SocketPort *first, *last;

	/**
	 * Attach a new socket port to this service thread.
	 *
	 * @param port of SocketPort derived object to attach.
	 */
	void attach(SocketPort *port);
	/**
	 * Detach a socket port from this service thread.
	 *
	 * @param port of SocketPort derived object to remove.
	 */
	void detach(SocketPort *port);
	
	/**
	 * The service thread itself.
	 */
	void run(void);

	friend class SocketPort;

protected:
	/**
	 * Handles all requests other than "termination".
	 *
	 * @param buf request id as posted from update().
	 */
	virtual void onUpdate(unsigned char buf)
		{return;};

	/**
	 * Called once each time the service thread is rescheduled.
	 * This is called after the mutex is locked and can be used to
	 * slip in additional processing.
	 */
	virtual void onEvent(void)
		{return;};

	/**
	 * Called for each port that is being processed in response to
	 * an event.  This can be used to add additional notification
	 * options during callback in combination with update().
	 *
	 * @param port SocketPort who's callback events are being evaluated.
	 */
	virtual void onCallback(SocketPort *port)
		{return;};

public:
	/**
	 * Notify service thread that a port has been added or
	 * removed, or a timer changed, so that a new schedule
	 * can be computed for expiring attached ports.  A "0"
	 * is used to terminate the service thread, and additional
	 * values can be specified which will be "caught" in the
	 * onUpdate() handler.
	 *
	 * @param flag update flag value.
	 */
	void update(unsigned char flag = 0xff);

	/**
	 * Create a service thread for attaching socket ports.  The
	 * thread begins execution with the first attached socket.
	 *
	 * @param pri of this thread to run under.
	 */
	SocketService(int pri = 0);

	/**
	 * Terminate the thread pool and eliminate any attached
	 * socket ports.
	 */
	virtual ~SocketService();

	/**
	 * Get current reference count.  This can be used when selecting
	 * the least used service handler from a pool.
	 *
	 * @return count of active ports.
	 */
	inline int getCount(void) const
		{return count;};
};

#ifdef	COMMON_STD_EXCEPTION
class SockException : public IOException
{
public:
	SockException(std::string str) : IOException(str) {};
};
#endif

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
