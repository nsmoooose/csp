#pragma once

#include <cc++\socket.h>
#include <cc++\thread.h>
#include <time.h>

#ifdef  CCXX_NAMESPACES
using namespace std;
using namespace ost;
#endif

// errors on functions
typedef short NET_ERROR;

const NET_ERROR NET_NO_MORE_DATA            = -3;
const NET_ERROR NET_INVALID_STATE           = -2;
const NET_ERROR NET_TOO_MUCH_DATA           = -1;
const NET_ERROR NET_NO_ERROR                =  0;

// ----------------------------------------------------------

typedef short NET_TYPE;

const NET_TYPE CONNECT                     = 0x0001;
const NET_TYPE DISCONNECT                  = 0x0002;
const NET_TYPE ACK                         = 0x0004;
const NET_TYPE REJECT                      = 0x0008;
const NET_TYPE SEQNBR                      = 0x0010;
const NET_TYPE ACKSEQNBR                   = 0x0020;
const NET_TYPE RESEND                      = 0x0040;
const NET_TYPE KEEPALIVE                   = 0x0080;
const NET_TYPE TIME                        = 0x0100;
const NET_TYPE LAG                         = 0x0200;
const NET_TYPE LAG_RECOVERED               = 0x0400;
const NET_TYPE TIMEOUT                     = 0x0800;
const NET_TYPE PACKETLOSS                  = 0x1000;
const NET_TYPE POSSDUPE                    = 0x2000;
const NET_TYPE DATA                        = 0x4000;

// ----------------------------------------------------------
// Move to Privates of NetworkingClass
const short PACKET_IDENTIFIER           = 0x5E10;
const short DATARATE_TIMEOUT						= 1000;
const short DROPPED_TIMEOUT             = 2500;
const short KEEPALIVE_TIMEOUT           = 5000;
const short KEEPALIVE_COUNTER           = 6;
const short NO_SEQ_NBR                  = -1;

// ----------------------------------------------------------

// Main Class.
class NetworkingClass : private Thread
{

private:

  // buffer struct
  struct NetBuffer
  {
    bool      processed;
    NET_TYPE  type;

    short     packetseqnbr;
    short     packetlength;
    void     *packet;
    
    short     dataoffset;
    short     datalength;

  };

  // Each variable will need to be flipflopped.
  struct NetHeader
  {
    short StartFiller;
    NET_TYPE Type;
    unsigned char SeqNbr;
    unsigned char AckSeqNbr;
    short Time;
  };

  // Internal Struct to hold information.
  struct NetConnections
  {

    // Connection
    bool              inuse;          // if we have open connection this is true
    bool              pendingclose;   // if we get a disconnect we need to still Recv
                                      //    the message so we need to pending close
                                      //    to keep the inuse open until all messages
                                      //    are read from buffer.

    InetHostAddress   addr;           //  address of connection
    tpport_t          port;           //  port of connection

    short             todo;						// what shall we do next send packet?

    // Ping Latency datarates
		unsigned long			recvdatabytes;
		unsigned long			senddatabytes;

    short             ping;
    short             latency;

    clock_t						timestamp;

		// Internal counters. Timeouts, keepalives and datarates

		short							datarate_counter;

    short             keepalive_counter;
    short             keepalive_time;

    short             dropped_seqnbr;
    short             dropped_time;

    // Buffer
		unsigned long			recvdatarate;
    unsigned char     recvcount;
    unsigned char     recvseqnbr;
    unsigned char     recvwritepointer;
    unsigned char     recvreadpointer;
    NetBuffer         recvbuffer[256];

		unsigned long			senddatarate;
    unsigned char     sendcount;
    unsigned char     sendwritepointer;
    unsigned char     sendreadpointer;
    NetBuffer         sendbuffer[256];

  };

  // Thread functions
  void            initial();
  void            run();
  void            final();

  // Class variables
  unsigned char   p_sConnectionCount;
  bool            p_bShutdown;

  UDPSocket      *p_Socket;
  Mutex          *p_MutexShared;

  NetConnections  p_Connect[256];

  short Resend(short ConnectionId, NetBuffer *Packet);
  short Reject(InetHostAddress Addr, tpport_t Port, void *Data, short Length);
  short ReportLocalMessage(short ConnectionId, NET_TYPE Type);

public:
  
  NetworkingClass(char *IP, char *Port);
  ~NetworkingClass();

  // ------------------------------

  // Operations
  short Connect(short ConnectionId, char *IP, char *Port, void*Data, short Length);
  short Send(short ConnectionId, bool Guaranteed, void *Data, short Length);
  short Recv(short ConnectionId, NET_TYPE *Type, void *Data, short *Length);
  short Accept(short ConnectionId, void *Data, short Length);
  short Reject(short ConnectionId, void *Data, short Length);
  short Disconnect(short ConnectionId, void *Data, short Length);

  // Query Performance
  short GetConnections(); // How many others we're connected to.
  bool  GetActive(short ConnectionId); // See if this connection is active.
  short GetPing(short ConnectionId); // Time to go over internet.
  short GetLatency(short ConnectionId); // Time the server takes to ack.
  short GetRecvMessages(short ConnectionId); // How many messages are waiting to be recv'd.
  short GetSendBufferCount(short ConnectionId); // How many packets are in the sendbuffer yet to be acknowledged from server.
  short GetRecvBufferCount(short ConnectionId); // How many packets are in the recvbuffer waiting to be acknowledged from us.
  unsigned long GetSendDataRate(short ConnectionId); // How many bytes per second are being sent.
  unsigned long GetRecvDataRate(short ConnectionId); // How many bytes per second are being recv.

  // FlipFlop functions, used to convert Endians.
  // Anything on the network should be in network order.
  // Use these to convert to local order. (or vise versa)
  void FlipFlop(unsigned short &number);
  void FlipFlop(short &number);
  void FlipFlop(unsigned long &number);
  void FlipFlop(long &number);
  void FlipFlop(float &number);
  void FlipFlop(double &number);

};
