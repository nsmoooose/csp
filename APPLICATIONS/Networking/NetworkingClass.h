#pragma once

#include <cc++\socket.h>
#include <cc++\thread.h>
#include <time.h>

#include <vector>
#include <deque>
#include <algorithm>

#ifdef  CCXX_NAMESPACES
using namespace std;
using namespace ost;
#endif

// errors on functions
typedef short NET_ERROR;

const NET_ERROR NET_INVALID_PACKET          = -5;
const NET_ERROR NET_EXCEPTION               = -4;
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
const short DROPPED_TIMEOUT             = 500;
const short KEEPALIVE_TIMEOUT           = 5000;
const short KEEPALIVE_COUNTER           = 6;
const short NO_SEQ_NBR                  = -1;

// ----------------------------------------------------------

// Main Class.
class NetworkingClass : private Thread
{

private:

  // Recv struct
  struct NetRecv
  {
    short         ConnectionId;
    NET_TYPE      Type;
    string        Data;
  };

  // Each variable will need to be flipflopped.
  struct NetHeader
  {
    short StartFiller;
    NET_TYPE Type;
    short SeqNbr;
    short AckSeqNbr;
    short Time;
  };

  // Packet
  struct NetBuffer
  {
    bool          processed;
    NetHeader     header;
    string        data;
  };

  // Internal Struct to hold information.
  struct NetConnections
  {

    // Connection
    InetHostAddress   addr;           //  address of connection
    tpport_t          port;           //  port of connection

		short							connectionid;		// Connection ID for app.

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
    short             recvseqnbr;
    vector<NetBuffer> recvbuffer;

		unsigned long			senddatarate;
    short             sendseqnbr;
    vector<NetBuffer> sendbuffer;

  };

  // Class variables
  bool                    p_bListen;

  UDPSocket              *p_Socket;
  Mutex                  *p_MutexShared;

  vector<NetConnections>  p_Connect;
  deque<NetRecv>          p_Recv;

  // Thread functions
  void            initial();
  void            run();
  void            final();

  // Private functions

  short LowLevelRecv();

  short InternalSend(short ArrayEntry, bool Guaranteed);
  short InternalSend(short ArrayEntry, bool Guaranteed, string Data);

  short LowLevelSend(InetHostAddress Addr,
                     tpport_t Port,
                     short Type,
                     short SeqNbr,
                     short AckSeqNbr,
                     short Time,
                     string *Data,
                     short *Length);

	short ConnectionIdToArrayEntry(short ConnectionId);

  short ReportLocalMessage(short ConnectionId, NET_TYPE Type);
  short ReportLocalMessage(short ConnectionId, NET_TYPE Type, string *Data);

public:
  
  NetworkingClass(char *IP, char *Port);
  ~NetworkingClass();

  // ------------------------------

  // Operations
  short Connect(short ConnectionId, char *IP, char *Port, string Data);
  short Connect(short ConnectionId, char *IP, char *Port);

  short Send(short ConnectionId, bool Guaranteed, string Data);
  short Recv(short *ConnectionId, NET_TYPE *Type, string *Data);

  short Listen(bool On);

  short Accept(short ConnectionId, string Data);
  short Accept(short ConnectionId);

  short Reject(short ConnectionId, string Data);
  short Reject(short ConnectionId);

  short Disconnect(short ConnectionId, string Data);
  short Disconnect(short ConnectionId);

  // Query Performance
  short GetConnections(); // How many others we're connected to.
  short GetPing(short ConnectionId); // Time to go over internet.
  short GetLatency(short ConnectionId); // Time the server takes to ack.
  short GetSendBufferCount(short ConnectionId); // How many packets are in the sendbuffer yet to be acknowledged from server.
  short GetRecvBufferCount(short ConnectionId); // How many packets are in the recvbuffer waiting to be acknowledged from us.
  short GetSendDataRate(short ConnectionId); // How many bytes per second are being sent.
  short GetRecvDataRate(short ConnectionId); // How many bytes per second are being recv.

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
