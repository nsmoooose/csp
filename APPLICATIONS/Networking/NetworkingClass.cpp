#include "NetworkingClass.h"

NetworkingClass::NetworkingClass(char *IP, char *Port)
{

  p_bListen = false;

  // Setup variables
  p_MutexShared = new Mutex;

  // Setup our side of connection.
  InetAddress addr = IP;
 
  p_Socket = new UDPSocket(addr, (tpport_t)atoi(Port));

  // Turn off blocking, we want this asyncronus
  p_Socket->setCompletion(false);

  // Turn on receiving thread.
  start();

}

NetworkingClass::~NetworkingClass()
{

  Listen(false);

  unsigned int Counter;

  // Send disconnect to connection
  for(Counter = 0; Counter < p_Connect.size(); Counter++)
  {
    Disconnect(p_Connect[Counter].connectionid, 0);
  }

  // Wait for all connections to get a disconnect ack.
  while(1)
  {
		if(p_Connect.size() != 0)
		{
			sleep(100);
		}
    else
    {
      break;
    }
  }

  // Turn off receiving thread.
  terminate();

  // Wait for thread to stop
  while(isRunning() == true)
  {
    sleep(100);
  }

  // Delete objects.
  if(p_MutexShared != 0)
  {
    delete p_MutexShared;
    p_MutexShared = 0;
  }

  if(p_Socket != 0)
  {
    delete p_Socket;
    p_Socket = 0;
  }

}

// ------------------------------------
// Threaded Commands
// ------------------------------------
void NetworkingClass::initial()
{

}

void NetworkingClass::final()
{

}

void NetworkingClass::run()
{  // This is the thread that handles the handshakes and receives.

  clock_t       begintime;
  clock_t       endtime;
  long          Counter;
  unsigned int  x;

  // -------------------------------

  // Thread can gracefully shutdown.
  setCancel(Thread::cancelDeferred);

  while(testCancel() == false)
  {

    begintime = clock();

    // Calculate next tick event.

    Counter = 1000;

    for(x = 0;x < p_Connect.size(); x++)
    {
      if(p_Connect[x].datarate_counter < Counter)
      {
        Counter = p_Connect[x].datarate_counter;
      }

      if(p_Connect[x].keepalive_time < Counter)
      {
        Counter = p_Connect[x].keepalive_time;
      }

      if(p_Connect[x].dropped_seqnbr != NO_SEQ_NBR)
      {
        if(p_Connect[x].dropped_time < Counter)
        {
          Counter = p_Connect[x].dropped_time;
        }
      }
    }

    // Wait for receiving information for next tick event.
    if(p_Socket->isPending(p_Socket->pendingInput, Counter) == true)
    { // incoming information
      LowLevelRecv();
    }

    // update time on connections.
    endtime = clock();

    p_MutexShared->enterMutex();

    for(x = 0; x < p_Connect.size();x++)
    {

			p_Connect[x].datarate_counter -= (short)(endtime - begintime);
      p_Connect[x].keepalive_time -= (short)(endtime - begintime);

      if(p_Connect[x].dropped_seqnbr != NO_SEQ_NBR)
      {
        p_Connect[x].dropped_time -= (short)(endtime - begintime);
      }

			if(p_Connect[x].datarate_counter <= 5)
			{
				p_Connect[x].datarate_counter = DATARATE_TIMEOUT;
				p_Connect[x].recvdatarate = p_Connect[x].recvdatabytes;
				p_Connect[x].senddatarate = p_Connect[x].senddatabytes;
				p_Connect[x].recvdatabytes = 0;
				p_Connect[x].senddatabytes = 0;
			}

      if(p_Connect[x].keepalive_time <= 5)
      {
        p_Connect[x].keepalive_time = KEEPALIVE_TIMEOUT;
        p_Connect[x].keepalive_counter++;

        if(p_Connect[x].keepalive_counter >= KEEPALIVE_COUNTER)
        {

          ReportLocalMessage(p_Connect[x].connectionid, TIMEOUT);

          p_Connect.erase(&p_Connect[x]);
        }
        else
        {

          // Send Keepalive
          p_Connect[x].todo |= KEEPALIVE;
					p_Connect[x].todo |= ACKSEQNBR;
					p_Connect[x].todo |= TIME;

          if(p_Connect[x].connectionid != NO_SEQ_NBR)
          {
            p_MutexShared->leaveMutex();

            InternalSend(x, false);

            p_MutexShared->enterMutex();
          }

					if(p_Connect[x].keepalive_counter == 2)
					{
						// When lag is first detected report lag.
						// Using counter == 2 because == 1 is very common.

            ReportLocalMessage(p_Connect[x].connectionid, LAG);
					}
        }
      }

      if(p_Connect[x].dropped_seqnbr != NO_SEQ_NBR)
      {
        if(p_Connect[x].dropped_time <= 5)
        {

          p_Connect[x].dropped_time = DROPPED_TIMEOUT;

          // Send Resend
          p_Connect[x].todo |= RESEND;
          p_Connect[x].todo |= ACKSEQNBR;

          p_MutexShared->leaveMutex();

          InternalSend(x, false);

          p_MutexShared->enterMutex();

          ReportLocalMessage(p_Connect[x].connectionid, PACKETLOSS);

        }
      }

    }

    p_MutexShared->leaveMutex();

    // Give CPU breathing room.
    yield();
  }

  return;

}

short NetworkingClass::LowLevelRecv()
{

  short     ArrayEntry;
  long      Counter;

  NetHeader PacketHead;
  char      PacketBuffer[4096];
  long      PacketLength;

  short     HeaderLength;

  unsigned int  x;

  // get address and port of other side.
  InetHostAddress addr;
  tpport_t        port;

  try
  {
    addr = p_Socket->getSender(&port);
  }
  catch(...)
  { // KABOOM
    return NET_EXCEPTION;
  }

  // scan local connection id's for match
  ArrayEntry = NO_SEQ_NBR;

  p_MutexShared->enterMutex();

  for(x = 0; x < p_Connect.size(); x++)
  {
    if(p_Connect[x].addr == addr &&
        p_Connect[x].port == port)
    {
      ArrayEntry = x;
      break;
    }
  }

  p_MutexShared->leaveMutex();
    
  // put message into temp buffer
  PacketLength = p_Socket->receive(PacketBuffer, sizeof(PacketBuffer));
  if(PacketLength <= 0)
  {
    // So we really don't have data? /boggle.
    return NET_INVALID_PACKET;
  }

  if(PacketLength > 4096)
  {
    // We don't support bigger than 4k buffers. Drop packet.
    // Must not be for us.
    return NET_INVALID_PACKET;
  }

  // HEADER
  // This will use header compression.
  memset(&PacketHead, 0, sizeof(PacketHead));

  // Pull out initial stuff and convert to correct endians
  memcpy(&PacketHead, &PacketBuffer, 4);

  FlipFlop(PacketHead.StartFiller);
  FlipFlop(PacketHead.Type);

  // verify packet seems legit
  if(PacketHead.StartFiller != PACKET_IDENTIFIER)
  {
    // drop packet. Whos sending me this stuff on this port?
    return NET_INVALID_PACKET;
  }

  // Uncompress header into non compressed struct.
  HeaderLength = 4;

  if(PacketHead.Type & SEQNBR)
  {
    memcpy(&PacketHead.SeqNbr, PacketBuffer + HeaderLength, 2);
    FlipFlop(PacketHead.SeqNbr);
    HeaderLength += 2;
  }

  if(PacketHead.Type & ACKSEQNBR)
  {
    memcpy(&PacketHead.AckSeqNbr, PacketBuffer + HeaderLength, 2);
    FlipFlop(PacketHead.AckSeqNbr);
    HeaderLength += 2;
  }

  if(PacketHead.Type & TIME)
  {
    memcpy(&PacketHead.Time, PacketBuffer + HeaderLength, 2);
    FlipFlop(PacketHead.Time);
    HeaderLength += 2;
  }

  if(PacketLength < HeaderLength)
  {
    // Invalid packet. We didn't get a complete header.
    return NET_INVALID_PACKET;
  }

  if(PacketHead.Type & DATA)
  {
    if(PacketLength == HeaderLength)
    {
      // Invalid packet, says we have data but we don't.
      return NET_INVALID_PACKET;
    }
  }

  // -----------------------------------------
  // Check Packet to see what it is....
  // -----------------------------------------
  if(PacketHead.Type & CONNECT)
  {
    if((PacketHead.Type & ACK) || (PacketHead.Type & REJECT))
    {
      // we'll get this later.
    }
    else
    {

      // We must be a server getting a connect.
      if(ArrayEntry == NO_SEQ_NBR)
      {

        bool Listen;

        p_MutexShared->enterMutex();

        Listen = p_bListen;

        p_MutexShared->leaveMutex();

        if(Listen == true)
        {

					// Fill out Array
					NetConnections tempConnect;

					memset(&tempConnect, 0, sizeof(tempConnect));

          tempConnect.addr = addr;
          tempConnect.port = port;
          tempConnect.connectionid = NO_SEQ_NBR;

					tempConnect.timestamp = clock();

					tempConnect.datarate_counter = DATARATE_TIMEOUT;
          tempConnect.keepalive_time = KEEPALIVE_TIMEOUT;
          tempConnect.dropped_seqnbr = NO_SEQ_NBR;

          p_MutexShared->enterMutex();

					p_Connect.push_back(tempConnect);
					ArrayEntry = p_Connect.size() - 1;

          p_MutexShared->leaveMutex();

        }
      }
      else
      {
        // Already connected, 
        // Send reject packet saying already connected

        string message = "Already Connected";

        LowLevelSend(addr, port, CONNECT | REJECT, 0, 0, 0, &message, 0);

        return NET_NO_ERROR;
      }
    }
  }

  // If no connection id, nothing else to do.
  if(ArrayEntry == NO_SEQ_NBR)
  { 
    return NET_NO_ERROR;
  }

  /*  // Add in intentional random packet loss for testing
  if((rand() % 100) == 50)
  {
    return NET_NO_ERROR;
  }
  */

  p_MutexShared->enterMutex();

	// Signal LAG RECOVERED if we finially got a packet.
  if(p_Connect[ArrayEntry].keepalive_counter >= 2)
  {
		// Using counter == 2 because == 1 is very common.
    ReportLocalMessage(p_Connect[ArrayEntry].connectionid, LAG_RECOVERED);
	}

	// Reset Keepalives
  p_Connect[ArrayEntry].keepalive_time = KEEPALIVE_TIMEOUT;
  p_Connect[ArrayEntry].keepalive_counter = 0;

	// Set Timestamp
	if(PacketHead.Type & TIME)
	{
		clock_t nowtime = clock();

		p_Connect[ArrayEntry].ping = (short)((nowtime - 
				                          p_Connect[ArrayEntry].timestamp) - PacketHead.Time);

		if(p_Connect[ArrayEntry].ping < 0) p_Connect[ArrayEntry].ping = 0;
	}

	p_Connect[ArrayEntry].timestamp = clock();

	// Update recv bytes
	p_Connect[ArrayEntry].recvdatabytes += PacketLength;

  // See if this could be a duplicate
  if(PacketHead.Type & POSSDUPE)
  {

    Counter = NO_SEQ_NBR;

    // If we aren't currently looking for a RESEND drop packet
    if(p_Connect[ArrayEntry].dropped_seqnbr == NO_SEQ_NBR)
    {
			p_MutexShared->leaveMutex();
      return NET_NO_ERROR;
    }

    // If SEQNBR coming in with POSSDUPE anything other than
    // The one we are looking for, then drop it.
    if(PacketHead.SeqNbr != p_Connect[ArrayEntry].dropped_seqnbr)
    {
			p_MutexShared->leaveMutex();
      return NET_NO_ERROR;
    }

    // If SEQNBR coming in with POSSDUPE is the one we are looking for
    // See if we already have it in our recv buffer.
    for(x = 0;x < p_Connect[ArrayEntry].recvbuffer.size();x++)
    {
      if(p_Connect[ArrayEntry].recvbuffer[x].header.SeqNbr == PacketHead.SeqNbr)
      {
			  p_MutexShared->leaveMutex();
        return NET_NO_ERROR;
      }
    }
  }

  // Other side detected a dropped packet.
  if(PacketHead.Type & RESEND)
  {

    Counter = NO_SEQ_NBR;

    for(x = 0;x < p_Connect[ArrayEntry].sendbuffer.size();x++)
    {
      if(p_Connect[ArrayEntry].sendbuffer[x].header.SeqNbr == PacketHead.AckSeqNbr + 1)
      {
        Counter = x;
        break;
      }
    }

		if(Counter == NO_SEQ_NBR)
		{
		}
		else
		{

      LowLevelSend(p_Connect[ArrayEntry].addr,
                   p_Connect[ArrayEntry].port,
                   p_Connect[ArrayEntry].sendbuffer[Counter].header.Type | POSSDUPE,
                   p_Connect[ArrayEntry].sendbuffer[Counter].header.SeqNbr,
                   p_Connect[ArrayEntry].sendbuffer[Counter].header.AckSeqNbr,
                   p_Connect[ArrayEntry].sendbuffer[Counter].header.Time,
                   &p_Connect[ArrayEntry].sendbuffer[Counter].data,
                   0);

      /* //add in intentional duping for testing
      if((rand() % 7) == 5)
      {
        LowLevelSend(p_Connect[ArrayEntry].addr,
                     p_Connect[ArrayEntry].port,
                     p_Connect[ArrayEntry].sendbuffer[Counter].header.Type | POSSDUPE,
                     p_Connect[ArrayEntry].sendbuffer[Counter].header.SeqNbr,
                     p_Connect[ArrayEntry].sendbuffer[Counter].header.AckSeqNbr,
                     p_Connect[ArrayEntry].sendbuffer[Counter].header.Time,
                     &p_Connect[ArrayEntry].sendbuffer[Counter].data,
                     0);

      }
      */
		}

    // If its a resend we don't really want to process this message.
    // We just want to send out another duplicate of the missing packet.

		p_MutexShared->leaveMutex();
    return NET_NO_ERROR;
  }

  // Place data into recv buffer.

  NetBuffer tempBuffer;

  tempBuffer.processed = false;
  tempBuffer.header = PacketHead;

  tempBuffer.data.append(PacketBuffer + HeaderLength, PacketLength - HeaderLength);

  p_Connect[ArrayEntry].recvbuffer.push_back(tempBuffer);

  // Connect packet (we just want to record we got it here)
  if(PacketHead.Type & CONNECT)
  {
    // Server is ACKING or NACKING
    if(PacketHead.Type & ACK)
    {
    }
    if(PacketHead.Type & REJECT)
    {

      ReportLocalMessage(p_Connect[ArrayEntry].connectionid, 
                         CONNECT | REJECT,
                         &p_Connect[ArrayEntry].recvbuffer.back().data);

      p_Connect.erase(&p_Connect[ArrayEntry]);

      p_MutexShared->leaveMutex();

      return NET_NO_ERROR;
    }
  }

  // Other side disconnecting.
  if(PacketHead.Type & DISCONNECT)
  {

    // See if we are client receiving the ack from server.
    if(PacketHead.Type & ACK)
    {

      ReportLocalMessage(p_Connect[ArrayEntry].connectionid, 
                         DISCONNECT | ACK,
                         &p_Connect[ArrayEntry].recvbuffer.back().data);

      p_Connect.erase(&p_Connect[ArrayEntry]);

      p_MutexShared->leaveMutex();

      return NET_NO_ERROR;
    }
    else
    {
      // We are the server responding with an ACK.
      p_Connect[ArrayEntry].todo |= DISCONNECT;
      p_Connect[ArrayEntry].todo |= ACK;
      p_MutexShared->leaveMutex();

      InternalSend(ArrayEntry, false);

      // Drop connection we don't care if they actually get ACK or not.
      p_MutexShared->enterMutex();

      ReportLocalMessage(p_Connect[ArrayEntry].connectionid, 
                         DISCONNECT,
                         &p_Connect[ArrayEntry].recvbuffer.back().data);

      p_Connect.erase(&p_Connect[ArrayEntry]);

      p_MutexShared->leaveMutex();

      return NET_NO_ERROR;
    }
  }

  // Incoming Seqnbr, if we are looking for a dropped packet, then we got it.
  if(PacketHead.Type & SEQNBR)
  {

    if(p_Connect[ArrayEntry].dropped_seqnbr == PacketHead.SeqNbr)
    {
      // turn off the resending requests.
      p_Connect[ArrayEntry].dropped_seqnbr = NO_SEQ_NBR;
      p_Connect[ArrayEntry].dropped_time = 0;
    }

    p_Connect[ArrayEntry].recvbuffer.back().header.SeqNbr = PacketHead.SeqNbr;
	}
  else
  {
    p_Connect[ArrayEntry].recvbuffer.back().header.SeqNbr = NO_SEQ_NBR;
  }

  // Incoming AckSeqNbr, we can clear out the send buffer
  if(PacketHead.Type & ACKSEQNBR)
  {
		for(x = 0;x<p_Connect[ArrayEntry].sendbuffer.size();x++)
		{

      if((p_Connect[ArrayEntry].sendbuffer[x].header.SeqNbr <= PacketHead.AckSeqNbr) ||
         (p_Connect[ArrayEntry].sendbuffer[x].header.SeqNbr >  p_Connect[ArrayEntry].sendseqnbr))
      {
        p_Connect[ArrayEntry].sendbuffer.erase(&p_Connect[ArrayEntry].sendbuffer[x]);
      }
		}
  }

  // Other side has no activity wants some.
  if(PacketHead.Type & KEEPALIVE)
  {

		// We are the client getting the response.
    if(PacketHead.Type & ACK)
    {
    }
		else
		{
			// We are the server responding with an ACK.

      if(p_Connect[ArrayEntry].connectionid != NO_SEQ_NBR)
      {

			  p_Connect[ArrayEntry].todo |= KEEPALIVE;
			  p_Connect[ArrayEntry].todo |= ACK;
			  p_Connect[ArrayEntry].todo |= ACKSEQNBR;

			  p_MutexShared->leaveMutex();

			  InternalSend(ArrayEntry, false);

			  p_MutexShared->enterMutex();
      }
		}
  }

  // Add this packet to our recv buffer.
  for(x = 0; 
      x < p_Connect[ArrayEntry].recvbuffer.size();
      x++)
  {

    bool Report = false;

    if(p_Connect[ArrayEntry].recvbuffer[x].processed == false)
    {
      if(p_Connect[ArrayEntry].recvbuffer[x].header.Type & SEQNBR)
      {
        if(p_Connect[ArrayEntry].recvbuffer[x].header.SeqNbr == p_Connect[ArrayEntry].recvseqnbr)
        {
          Report = true;
        }
      }
      else
      {
        Report = true;
      }
    }

    if(p_Connect[ArrayEntry].recvbuffer[x].header.Type & KEEPALIVE)
    {

      p_Connect[ArrayEntry].recvbuffer[x].processed = true;

      Report = false;
    } 

    if(Report == true)
    {

      ReportLocalMessage(p_Connect[ArrayEntry].connectionid, 
                         p_Connect[ArrayEntry].recvbuffer[x].header.Type,
                        &p_Connect[ArrayEntry].recvbuffer[x].data);

      if(p_Connect[ArrayEntry].recvbuffer[x].header.Type & SEQNBR)
      {
        p_Connect[ArrayEntry].recvseqnbr++;
      }

      p_Connect[ArrayEntry].recvbuffer[x].processed = true;
      x = 0;
    }
  }

  // Loop through and see if we have more seqnbr packets
  // This code should only be hit if there are out of order
  // or missing seq nbrs.

	Counter = NO_SEQ_NBR;

  for(x = 0; 
      x < p_Connect[ArrayEntry].recvbuffer.size();
      x++)
  {
    if(p_Connect[ArrayEntry].recvbuffer[x].processed == false)
    {
      if(p_Connect[ArrayEntry].recvbuffer[x].header.Type & SEQNBR)
      {
        if(p_Connect[ArrayEntry].recvbuffer[x].header.SeqNbr != p_Connect[ArrayEntry].recvseqnbr)
        {
          Counter = x;
        }
        else
        {
			    Counter = NO_SEQ_NBR;
			    break;
        }
      }
    }
  }

	if(Counter != NO_SEQ_NBR)
	{
		if(p_Connect[ArrayEntry].dropped_seqnbr == NO_SEQ_NBR)
		{
			p_Connect[ArrayEntry].dropped_seqnbr = p_Connect[ArrayEntry].recvseqnbr;
			p_Connect[ArrayEntry].dropped_time = DROPPED_TIMEOUT;
		}
	}

  p_MutexShared->leaveMutex();

  return NET_NO_ERROR;
}

short NetworkingClass::InternalSend(short ArrayEntry, bool Guaranteed)
{
  return InternalSend(ArrayEntry, Guaranteed, "");
}

short NetworkingClass::InternalSend(short ArrayEntry, bool Guaranteed, string Data)
{

  if(Data.size() > 4000)
  {
      return NET_TOO_MUCH_DATA;
  }

  // ---------------------------------------------------

  unsigned int x;

  p_MutexShared->enterMutex();

  // Set Seq Nbr
  if(Guaranteed == true)
  {
    p_Connect[ArrayEntry].todo |= SEQNBR;
    p_Connect[ArrayEntry].todo |= ACKSEQNBR;
    p_Connect[ArrayEntry].todo |= TIME;
  }

  // Set Latency time
  if(p_Connect[ArrayEntry].todo & TIME)
  {
		clock_t nowtime = clock();

		p_Connect[ArrayEntry].latency = (short)(nowtime - p_Connect[ArrayEntry].timestamp);

		if(p_Connect[ArrayEntry].latency < 0) p_Connect[ArrayEntry].latency = 0;
  }

	p_Connect[ArrayEntry].timestamp = clock();

  // Update recv buffers
  if(p_Connect[ArrayEntry].todo & ACKSEQNBR)
  {

    // Update RecvBuffer Counters.
    for(x = 0; x < p_Connect[ArrayEntry].recvbuffer.size();x++)
    {
      if(p_Connect[ArrayEntry].recvbuffer[x].processed == true)
      {
        p_Connect[ArrayEntry].recvbuffer.erase(&p_Connect[ArrayEntry].recvbuffer[x]);
      }
    }
  }

  // Save off Packet in case they need a resend.
  if(p_Connect[ArrayEntry].todo & SEQNBR)
  {

    // Place contents into buffer

    p_Connect[ArrayEntry].sendseqnbr++;

    NetBuffer tempBuffer;

    tempBuffer.header.Type = p_Connect[ArrayEntry].todo;
    tempBuffer.header.SeqNbr = p_Connect[ArrayEntry].sendseqnbr;
    tempBuffer.header.AckSeqNbr = p_Connect[ArrayEntry].recvseqnbr-1;
    tempBuffer.header.Time = p_Connect[ArrayEntry].latency;
    tempBuffer.data = Data;
      
    p_Connect[ArrayEntry].sendbuffer.push_back(tempBuffer);

  }

  // Low Level Send
  short Length;

  LowLevelSend(p_Connect[ArrayEntry].addr, p_Connect[ArrayEntry].port,
               p_Connect[ArrayEntry].todo,
               p_Connect[ArrayEntry].sendseqnbr-1, p_Connect[ArrayEntry].recvseqnbr-1,
               p_Connect[ArrayEntry].latency,
               &Data,
               &Length);

  // Update Stats
	p_Connect[ArrayEntry].senddatabytes += Length;
  p_Connect[ArrayEntry].todo = 0;

  p_MutexShared->leaveMutex();

  return NET_NO_ERROR;

}

short NetworkingClass::LowLevelSend(InetHostAddress Addr,
                                    tpport_t Port,
                                    short Todo,
                                    short SeqNbr,
                                    short AckSeqNbr,
                                    short Time,
                                    string *Data,
                                    short *Length)
{

  char Buffer[4096];

  // Build Packet.
  bool      InternalLength = false;

  if(Length == 0)
  {
    Length = new short;
    InternalLength = true;
  }

  if(Data->size() > 0)
  {
    Todo |= DATA;
  }

  NetHeader Head;
  memset(&Head, 0, sizeof(Head));

  Head.StartFiller = PACKET_IDENTIFIER;
  Head.Type = Todo;
  Head.SeqNbr = SeqNbr;
  Head.AckSeqNbr = AckSeqNbr;
  Head.Time = Time;

  // Save off copy so we don't have flipflopped constants
  short     tempType;
  tempType = Head.Type;

  // Build Header

  *Length = 0;

  FlipFlop(Head.StartFiller);
  memcpy(Buffer + *Length, &Head.StartFiller, 2);

  *Length += 2;

  FlipFlop(Head.Type);
  memcpy(Buffer + *Length, &Head.Type, 2);

  *Length += 2;

  if(tempType & SEQNBR)
  {
    FlipFlop(Head.SeqNbr);
    memcpy(Buffer + *Length, &Head.SeqNbr, 2);

    *Length += 2;
  }

  if(tempType & ACKSEQNBR)
  {
    FlipFlop(Head.AckSeqNbr);
    memcpy(Buffer + *Length, &Head.AckSeqNbr, 2);

    *Length += 2;
  }

  if(tempType & TIME)
  {
    FlipFlop(Head.Time);
    memcpy(Buffer + *Length, &Head.Time, 2);

    *Length += 2;
  }

  if(tempType & DATA)
  {
    // This data should be flipflopped already by App.

    memcpy(Buffer + *Length, Data->data(), Data->size());
     
    *Length += Data->size();

  }

  // Do the send.
  p_Socket->setPeer(Addr, Port);
  p_Socket->send(Buffer, *Length);

  if(InternalLength == true)
  {
    delete Length;
    Length = 0;
  }

  return NET_NO_ERROR;

}

short NetworkingClass::ReportLocalMessage(short ConnectionId, NET_TYPE Type)
{
  return ReportLocalMessage(ConnectionId, Type, 0);
}

short NetworkingClass::ReportLocalMessage(short ConnectionId, NET_TYPE Type, string *Data)
{

  NetRecv Recv;

  Recv.ConnectionId = ConnectionId;
  Recv.Type = Type;

  if(Data != 0)
  {
    Recv.Data = *Data;
  }
  else
  {
    Recv.Data = "";
  }

  p_Recv.push_back(Recv);

  return NET_NO_ERROR;
}

short NetworkingClass::ConnectionIdToArrayEntry(short ConnectionId)
{

	short         result = NO_SEQ_NBR;
	unsigned int  x;

  p_MutexShared->enterMutex();

	for(x=0;x<p_Connect.size();x++)
	{
		if(p_Connect[x].connectionid == ConnectionId)
		{
			result = (short)x;
			break;
		}
	}

  p_MutexShared->leaveMutex();

	return result;
}

// -----------------------------------------------------------------
 
short NetworkingClass::Connect(short ConnectionId, char *IP, char *Port)
{
  return Connect(ConnectionId, IP, Port, "");
}

short NetworkingClass::Connect(short ConnectionId, char *IP, char *Port, string Data)
{

	short ArrayEntry;

	ArrayEntry = ConnectionIdToArrayEntry(ConnectionId);

	if(ArrayEntry != NO_SEQ_NBR)
  {
    return NET_INVALID_STATE;
  }

  // ------------------------------------

	NetConnections tempConnect;

	memset(&tempConnect, 0, sizeof(tempConnect));

  tempConnect.addr = IP;
  tempConnect.port = (tpport_t)atoi(Port);
	tempConnect.connectionid = ConnectionId;

  tempConnect.todo |= CONNECT;

	tempConnect.datarate_counter = DATARATE_TIMEOUT;
  tempConnect.keepalive_time = KEEPALIVE_TIMEOUT;
  tempConnect.dropped_seqnbr = NO_SEQ_NBR;

	tempConnect.timestamp = clock();

  p_MutexShared->enterMutex();  

  p_Connect.push_back(tempConnect);

  p_MutexShared->leaveMutex();

  return Send(ConnectionId, true, Data);
}

short NetworkingClass::Disconnect(short ConnectionId)
{
  return Disconnect(ConnectionId, "");
}

short NetworkingClass::Disconnect(short ConnectionId, string Data)
{

	short ArrayEntry;

	ArrayEntry = ConnectionIdToArrayEntry(ConnectionId);

	if(ArrayEntry == NO_SEQ_NBR)
  {
    return NET_INVALID_STATE;
  }

  // ------------------------------------

  p_MutexShared->enterMutex();

  p_Connect[ArrayEntry].todo |= DISCONNECT;

  p_MutexShared->leaveMutex();

  return InternalSend(ArrayEntry, true, Data);
}

short NetworkingClass::Send(short ConnectionId, bool Guaranteed, string Data)
{

	short ArrayEntry;

	ArrayEntry = ConnectionIdToArrayEntry(ConnectionId);

	if(ArrayEntry == NO_SEQ_NBR)
  {
    return NET_INVALID_STATE;
  }

  // ------------------------------------

  InternalSend(ArrayEntry, Guaranteed, Data);

  return NET_NO_ERROR;
}

short NetworkingClass::Recv(short *ConnectionId, NET_TYPE *Type, string *Data)
{

  p_MutexShared->enterMutex();

  if(p_Recv.size() == 0)
  {
    p_MutexShared->leaveMutex();

    return NET_NO_MORE_DATA;
  }

  NetRecv tempRecv;

  tempRecv = p_Recv.front();

  *ConnectionId = tempRecv.ConnectionId;
  *Type = tempRecv.Type;
  *Data = tempRecv.Data;

  p_Recv.pop_front();

  p_MutexShared->leaveMutex();

  return NET_NO_ERROR;
}

short NetworkingClass::Listen(bool On)
{

  p_MutexShared->enterMutex();

  p_bListen = On;

  p_MutexShared->leaveMutex();

  return NET_NO_ERROR;

}

short NetworkingClass::Accept(short ConnectionId)
{
  return Accept(ConnectionId, "");
}

short NetworkingClass::Accept(short ConnectionId, string Data)
{

	short ArrayEntry;

	ArrayEntry = ConnectionIdToArrayEntry(NO_SEQ_NBR);

	if(ArrayEntry == NO_SEQ_NBR)
  {
    return NET_INVALID_STATE;
  }

  // ------------------------------------

  p_MutexShared->enterMutex();

  p_Connect[ArrayEntry].connectionid = ConnectionId;

  p_Connect[ArrayEntry].todo |= CONNECT;
  p_Connect[ArrayEntry].todo |= ACK;

  p_MutexShared->leaveMutex();

  return InternalSend(ArrayEntry, true, Data);

}

short NetworkingClass::Reject(short ConnectionId)
{
  return Reject(ConnectionId, "");
}

short NetworkingClass::Reject(short ConnectionId, string Data)
{

	short ArrayEntry;

	ArrayEntry = ConnectionIdToArrayEntry(NO_SEQ_NBR);

	if(ArrayEntry == NO_SEQ_NBR)
  {
    return NET_INVALID_STATE;
  }

  // ------------------------------------

  InetHostAddress   addr;
  tpport_t          port;

  p_MutexShared->enterMutex();

	addr = p_Connect[ArrayEntry].addr;
	port = p_Connect[ArrayEntry].port;

	p_Connect.erase(&p_Connect[ArrayEntry]);

  p_MutexShared->leaveMutex();

  return LowLevelSend(addr, port, CONNECT | REJECT, 0, 0, 0, &Data, 0);

}

short NetworkingClass::GetConnections()
{
  return p_Connect.size();
}

short NetworkingClass::GetPing(short ConnectionId)
{

	short ArrayEntry;

	ArrayEntry = ConnectionIdToArrayEntry(ConnectionId);

	if(ArrayEntry == NO_SEQ_NBR)
  {
    return NET_INVALID_STATE;
  }

  // ------------------------------------

  return p_Connect[ArrayEntry].ping;
}

short NetworkingClass::GetLatency(short ConnectionId)
{

	short ArrayEntry;

	ArrayEntry = ConnectionIdToArrayEntry(ConnectionId);

	if(ArrayEntry == NO_SEQ_NBR)
  {
    return NET_INVALID_STATE;
  }

  // ------------------------------------

  return p_Connect[ArrayEntry].latency;
}

short NetworkingClass::GetRecvBufferCount(short ConnectionId)
{

	short ArrayEntry;

	ArrayEntry = ConnectionIdToArrayEntry(ConnectionId);

	if(ArrayEntry == NO_SEQ_NBR)
  {
    return NET_INVALID_STATE;
  }

  // ------------------------------------

  return (short)p_Connect[ArrayEntry].recvbuffer.size();
}

short NetworkingClass::GetSendBufferCount(short ConnectionId)
{

	short ArrayEntry;

	ArrayEntry = ConnectionIdToArrayEntry(ConnectionId);

	if(ArrayEntry == NO_SEQ_NBR)
  {
    return NET_INVALID_STATE;
  }

  // ------------------------------------

  return (short)p_Connect[ArrayEntry].sendbuffer.size();
}

short NetworkingClass::GetRecvDataRate(short ConnectionId)
{

	short ArrayEntry;

	ArrayEntry = ConnectionIdToArrayEntry(ConnectionId);

	if(ArrayEntry == NO_SEQ_NBR)
  {
    return NET_INVALID_STATE;
  }

  // ------------------------------------

  return (short)p_Connect[ArrayEntry].recvdatarate;
}

short NetworkingClass::GetSendDataRate(short ConnectionId)
{

	short ArrayEntry;

	ArrayEntry = ConnectionIdToArrayEntry(ConnectionId);

	if(ArrayEntry == NO_SEQ_NBR)
  {
    return NET_INVALID_STATE;
  }

  // ------------------------------------

  return (short)p_Connect[ArrayEntry].senddatarate;
}


void NetworkingClass::FlipFlop(unsigned short &number) 
{
  unsigned short *usp;
  usp = &number;

  #ifdef WIN32
    char *cp;
    char c;
    cp = (char *)usp;
    c = cp[0];
    cp[0] = cp[1];
    cp[1] = c;
  #endif

  return;
}

void NetworkingClass::FlipFlop(short &number) 
{
  unsigned short us;
  us = (unsigned short)number;
  FlipFlop(us);
  number = (short)us;
  return;
}

void NetworkingClass::FlipFlop(unsigned long &number) 
{
  unsigned long *ulp;
  ulp = &number;

  #ifdef WIN32
    char *cp;
    char c;
    cp = (char *)ulp;
    c = cp[0];
    cp[0] = cp[3];
    cp[3] = c;
    c = cp[1];
    cp[1] = cp[2];
    cp[2] = c;
  #endif

  return;
}

void NetworkingClass::FlipFlop(long &number) 
{
  unsigned long ul;
  ul = (unsigned long)number;
  FlipFlop(ul);
  number = (long)ul;
  return;
}

void NetworkingClass::FlipFlop(float &number) 
{
  float *fp;
  fp = &number;

  #ifdef WIN32
    char *cp;
    char c;
    cp = (char *)fp;
    c = cp[0];
    cp[0] = cp[3];
    cp[3] = c;
    c = cp[1];
    cp[1] = cp[2];
    cp[2] = c;
  #endif

  return;
}

void NetworkingClass::FlipFlop(double &number) 
{
  double *dp;
  dp = &number;

  #ifdef WIN32

    char *cp;
    char c;
    cp = (char *)dp;
    c = cp[0];
    cp[0] = cp[7];
    cp[7] = c;
    c = cp[1];
    cp[1] = cp[6];
    cp[6] = c;
    c = cp[2];
    cp[2] = cp[5];
    cp[5] = c;
    c = cp[3];
    cp[3] = cp[4];
    cp[4] = c;

  #endif

  return;
}
