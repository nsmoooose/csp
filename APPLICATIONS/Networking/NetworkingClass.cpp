#include "NetworkingClass.h"

NetworkingClass::NetworkingClass(char *IP, char *Port)
{

  p_bShutdown = true;
  p_sConnectionCount = 0;

  memset(p_Connect, 0, sizeof(p_Connect));

  // Setup variables
  p_MutexShared = new Mutex;

  // Setup our side of connection.
  InetAddress addr = IP;
  p_Socket = new UDPSocket(addr, (tpport_t)atoi(Port));

  // Turn off blocking, we want this asyncronus
  p_Socket->setCompletion(false);

  // Turn on receiving thread.
  start();

  // Signal we are up not shutting down, and ready to receive connections.
  p_MutexShared->enterMutex();
  p_bShutdown = false;
  p_MutexShared->leaveMutex();

}

NetworkingClass::~NetworkingClass()
{

  long Counter;

  if(p_MutexShared != 0)
  {
    p_MutexShared->enterMutex();
    p_bShutdown = true;
    p_MutexShared->leaveMutex();
  }

  // Send disconnect to connection
  if(p_sConnectionCount > 0)
  {
    for(Counter = 0; Counter<=255; Counter++)
    {
      if(p_Connect[Counter].inuse == true && p_Connect[Counter].pendingclose == false)
      {
        Disconnect(Counter, 0,0);
      }
    }
   }

  // Wait for all connections to get a disconnect ack.
  while(1)
  {
    if(p_sConnectionCount > 0)
    {

      for(Counter = 0; Counter<=255; Counter++)
      {
        if(p_Connect[Counter].inuse == true)
        {
          Recv(Counter, 0, 0, 0);
        }
      }

      sleep(10);
    }
    else
    {
      break;
    }
  }

  // Turn off receiving thread.
  terminate();

  // Delete objects.
  for(short y=0; y<=255;y++)
  {
    for(short x=0; x<=255;x++)
    {
      if(p_Connect[y].recvbuffer[x].packet != 0)
      {
        delete []p_Connect[y].recvbuffer[x].packet;
      }
      if(p_Connect[y].sendbuffer[x].packet != 0)
      {
        delete []p_Connect[y].sendbuffer[x].packet;
      }
    }
  }

  if(p_Socket != 0)
  {
    delete p_Socket;
    p_Socket = 0;
  }

  if(p_MutexShared != 0)
  {
    delete p_MutexShared;
    p_MutexShared = 0;
  }

}

// ------------------------------------
// Threaded Commands
// ------------------------------------
void NetworkingClass::initial()
{

}

void NetworkingClass::run()
{  // This is the thread that handles the handshakes and receives.

  NetHeader PacketHead;
  char      PacketBuffer[4096];
  long      PacketLength;

  short     HeaderLength;

  long      ConnectionId;

  long      Counter;

  clock_t   begintime;
  clock_t   endtime;

  short     x;
  // -------------------------------

  // Thread can gracefully shutdown.
  setCancel(Thread::cancelDeferred);

  while(testCancel() == false)
  {

    begintime = clock();

    // Calculate next tick event.

    Counter = 1000;

    for(x = 0; x<=255; x++)
    {
      if(p_Connect[x].inuse == true && p_Connect[x].pendingclose == false)
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
    }

    // Wait for receiving information for next tick event.
    if(p_Socket->isPending(p_Socket->pendingInput, Counter) == true)
    { // incoming information
     
      // get address and port of other side.
      InetHostAddress addr;
      tpport_t        port;

      try
      {
        addr = p_Socket->getSender(&port);
      }
      catch(...)
      { // KABOOM
        goto SKIP;
      }

      // scan local connection id's for match
      ConnectionId = -1;

      p_MutexShared->enterMutex();

      for(Counter = 0; Counter <= 255; Counter++)
      {
        if(p_Connect[Counter].inuse == true && p_Connect[Counter].pendingclose == false)
        {
          if(p_Connect[Counter].addr == addr &&
             p_Connect[Counter].port == port)
          {
            ConnectionId = Counter;
            break;
          }
        }
      }

      p_MutexShared->leaveMutex();
        
      // put message into temp buffer
      PacketLength = p_Socket->receive(PacketBuffer, sizeof(PacketBuffer));
      if(PacketLength <= 0)
      {
        // So we really don't have data? /boggle.
        goto SKIP;
      }

      if(PacketLength > 4096)
      {
        // We don't support bigger than 4k buffers. Drop packet.
        // Must not be for us.
        goto SKIP;
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
        goto SKIP;
      }

      // Uncompress header into non compressed struct.
      HeaderLength = 4;

      if(PacketHead.Type & SEQNBR)
      {
        memcpy(&PacketHead.SeqNbr, PacketBuffer + HeaderLength, 2);
        //FlipFlop(PacketHead.SeqNbr);

        HeaderLength += 2;
      }

      if(PacketHead.Type & ACKSEQNBR)
      {

        memcpy(&PacketHead.AckSeqNbr, PacketBuffer + HeaderLength, 2);
        //FlipFlop(PacketHead.AckSeqNbr);

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
        goto SKIP;
      }

      if(PacketHead.Type & DATA)
      {
        if(PacketLength == HeaderLength)
        {
          // Invalid packet, says we have data but we don't.
          goto SKIP;
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
          if(ConnectionId == -1)
          {

            bool Shutdown;

            p_MutexShared->enterMutex();
            Shutdown = p_bShutdown;
            p_MutexShared->leaveMutex();

            if(Shutdown == false)
            {

              // See if there is any room left in array.
              p_MutexShared->enterMutex();

              for(Counter = 0;Counter<=255;Counter++)
              {
                if(p_Connect[Counter].inuse == false)
                {
                  break;
                }
              }

              p_MutexShared->leaveMutex();

              // Check if server is full
              if(Counter == 256)
              {
                // Send Reject packet saying server is full.
                Reject(addr, port, "Server Full", 12);
                goto SKIP;
              }
              else
              {
                // Server has room, fill out array.
                ConnectionId = Counter;

                p_Connect[ConnectionId].inuse = true;
                p_Connect[ConnectionId].addr = addr;
                p_Connect[ConnectionId].port = port;

                p_Connect[ConnectionId].todo = 0;

								p_Connect[ConnectionId].recvdatabytes = 0;
								p_Connect[ConnectionId].senddatabytes = 0;
								p_Connect[ConnectionId].datarate_counter = DATARATE_TIMEOUT;

                p_Connect[ConnectionId].keepalive_time = KEEPALIVE_TIMEOUT;
                p_Connect[ConnectionId].keepalive_counter = 0;

                p_Connect[ConnectionId].dropped_seqnbr = NO_SEQ_NBR;

                p_Connect[ConnectionId].sendreadpointer = 0;
                p_Connect[ConnectionId].sendwritepointer = 0;
                p_Connect[ConnectionId].recvreadpointer = 0;
                p_Connect[ConnectionId].recvwritepointer = 0;
                p_Connect[ConnectionId].recvcount = 0;
                p_Connect[ConnectionId].sendcount = 0;


                p_Connect[ConnectionId].latency = 0;
                p_Connect[ConnectionId].ping = 0;
                p_Connect[ConnectionId].recvreadpointer = 0;
                p_Connect[ConnectionId].senddatarate = 0;
                p_Connect[ConnectionId].recvseqnbr = 0;

								p_Connect[ConnectionId].timestamp = clock();

              }
            }
            else
            {
              // Send reject packet saying shutdown in progress
              Reject(addr, port, "Shutting Down", 15);
              goto SKIP;
            }
          }
          else
          {
            // Already connected, 
            // Send reject packet saying already connected
            Reject(addr, port, "Already Connected", 18);
            goto SKIP;
          }
        }
      }

      // If no connection id, nothing else to do.
      if(ConnectionId == -1)
      { 
        goto SKIP;
      }

      // Add random packet loss for testing
      /*
      if((rand() % 10) == 5)
      {
        cout << endl << "DROPPING";
        goto SKIP;
      }
      */

      p_MutexShared->enterMutex();

			// Signal LAG RECOVERED if we finially got a packet.
      if(p_Connect[ConnectionId].keepalive_counter >= 2)
      {
				// Using counter == 2 because == 1 is very common.
				p_MutexShared->leaveMutex();
        ReportLocalMessage(ConnectionId, LAG_RECOVERED);
				p_MutexShared->enterMutex();
			}

			// Reset Keepalives
      p_Connect[ConnectionId].keepalive_time = KEEPALIVE_TIMEOUT;
      p_Connect[ConnectionId].keepalive_counter = 0;

			// Set Timestamp
			if(PacketHead.Type & TIME)
			{
				clock_t nowtime = clock();

				p_Connect[ConnectionId].ping = (nowtime - 
						p_Connect[ConnectionId].timestamp) - PacketHead.Time;

				if(p_Connect[ConnectionId].ping < 0) p_Connect[ConnectionId].ping = 0;
			}

			p_Connect[ConnectionId].timestamp = clock();

			// Update recv bytes
			p_Connect[ConnectionId].recvdatabytes += PacketLength;

      // See if this could be a duplicate
      if(PacketHead.Type & POSSDUPE)
      {

        Counter = NO_SEQ_NBR;

        // If we aren't currently looking for a RESEND drop packet
        if(p_Connect[ConnectionId].dropped_seqnbr == NO_SEQ_NBR)
        {
          // Set the Counter to be anything != NO_SEQ_NBR to skip.
          Counter = 0; 
        }

        // If SEQNBR coming in with POSSDUPE anything other than
        // The one we are looking for, then drop it.
        if(PacketHead.SeqNbr != p_Connect[ConnectionId].recvseqnbr)
        {
          // Set the Counter to be anything != NO_SEQ_NBR to skip.
          Counter = 0; 
        }

        // If SEQNBR coming in with POSSDUPE is the one we are looking for
        // See if we already have it in our recv buffer.
        if(PacketHead.SeqNbr == p_Connect[ConnectionId].recvseqnbr)
        {
          if(p_Connect[ConnectionId].recvwritepointer < p_Connect[ConnectionId].recvreadpointer)
          {

            for(x = p_Connect[ConnectionId].recvreadpointer;x<=255;x++)
            {
              if(p_Connect[ConnectionId].recvbuffer[x].packetseqnbr == PacketHead.SeqNbr)
              {
                Counter = x;
                break;
              }
            }

            if(Counter == NO_SEQ_NBR)
            {
              for(x=0;x<p_Connect[ConnectionId].recvwritepointer;x++)
              {
                if(p_Connect[ConnectionId].recvbuffer[x].packetseqnbr == PacketHead.SeqNbr)
                {
                  Counter = x;
                  break;
                }
              }
            }
          }
          else
          {
            for(x = p_Connect[ConnectionId].recvreadpointer;x<p_Connect[ConnectionId].recvwritepointer;x++)
            {
              if(p_Connect[ConnectionId].recvbuffer[x].packetseqnbr == PacketHead.SeqNbr)
              {
                Counter = x;
                break;
              }
            }
          }       
        }

        if(Counter != NO_SEQ_NBR)
        {
					p_MutexShared->leaveMutex();
          goto SKIP;
        }

      }

      // Other side detected a dropped packet.
      if(PacketHead.Type & RESEND)
      {

        Counter = NO_SEQ_NBR;

        if(p_Connect[ConnectionId].sendwritepointer < p_Connect[ConnectionId].sendreadpointer)
        {

          for(x = p_Connect[ConnectionId].sendreadpointer;x<=255;x++)
          {
            if(p_Connect[ConnectionId].sendbuffer[x].packetseqnbr == PacketHead.AckSeqNbr + 1)
            {
              Counter = x;
              break;
            }
          }

          if(Counter == NO_SEQ_NBR)
          {
            for(x=0;x<p_Connect[ConnectionId].sendwritepointer;x++)
            {
              if(p_Connect[ConnectionId].sendbuffer[x].packetseqnbr == PacketHead.AckSeqNbr + 1)
              {
                Counter = x;
                break;
              }
            }
          }
        }
        else
        {
          for(x = p_Connect[ConnectionId].sendreadpointer;x<p_Connect[ConnectionId].sendwritepointer;x++)
          {
            if(p_Connect[ConnectionId].sendbuffer[x].packetseqnbr == PacketHead.AckSeqNbr + 1)
            {
              Counter = x;
              break;
            }
          }
        }

				if(Counter == NO_SEQ_NBR)
				{
				}
				else
				{
					NetBuffer tempBuffer;
					tempBuffer = p_Connect[ConnectionId].sendbuffer[Counter];

					p_MutexShared->leaveMutex();

					Resend(ConnectionId, &tempBuffer);

					p_MutexShared->enterMutex();

				}

        // If its a resend we don't really want to process this message.
        // We just want to send out another duplicate of the missing packet.

				p_MutexShared->leaveMutex();
        goto SKIP;
      }

      // See if wrap around recv buffer is already full.
      if(p_Connect[ConnectionId].recvcount == 256)
      {
        // Buffer full.
        // Still pondering if we should disconnect or have a wait in here.
        int a = 0;
      }

      // Place data into recv buffer.
      p_Connect[ConnectionId].recvbuffer
          [p_Connect[ConnectionId].recvwritepointer].processed = false;

      p_Connect[ConnectionId].recvbuffer
          [p_Connect[ConnectionId].recvwritepointer].packetlength = PacketLength;

      if(p_Connect[ConnectionId].recvbuffer
          [p_Connect[ConnectionId].recvwritepointer].packet != 0)
      {
        delete []p_Connect[ConnectionId].recvbuffer
          [p_Connect[ConnectionId].recvwritepointer].packet;
      }

      p_Connect[ConnectionId].recvbuffer
          [p_Connect[ConnectionId].recvwritepointer].packet = new char[PacketLength];

      memcpy(p_Connect[ConnectionId].
             recvbuffer[p_Connect[ConnectionId].recvwritepointer].packet,
             PacketBuffer, PacketLength);

      p_Connect[ConnectionId].recvbuffer
          [p_Connect[ConnectionId].recvwritepointer].dataoffset = HeaderLength;

      p_Connect[ConnectionId].recvbuffer
          [p_Connect[ConnectionId].recvwritepointer].datalength = PacketLength - HeaderLength;

      p_Connect[ConnectionId].recvbuffer
          [p_Connect[ConnectionId].recvwritepointer].type = PacketHead.Type;

      //p_MutexShared->leaveMutex();

      // Connect packet (we just want to record we got it here)
      if(PacketHead.Type & CONNECT)
      {
        // Server is ACKING or NACKING
        if(PacketHead.Type & ACK)
        {
        }
        if(PacketHead.Type & REJECT)
        {
          p_Connect[ConnectionId].pendingclose = true;
        }
      }

      // Other side disconnecting.
      if(PacketHead.Type & DISCONNECT)
      {

        // See if we are client receiving the ack from server.
        if(PacketHead.Type & ACK)
        {
          p_Connect[ConnectionId].pendingclose = true;
        }
        else
        {
          // We are the server responding with an ACK.
          //p_MutexShared->enterMutex();
          p_Connect[ConnectionId].todo |= DISCONNECT;
          p_Connect[ConnectionId].todo |= ACK;
          p_MutexShared->leaveMutex();

          Send(ConnectionId, false, 0, 0);

          // Drop connection we don't care if they actually get ACK or not.
          p_MutexShared->enterMutex();
          p_Connect[ConnectionId].pendingclose = true;
        }
      }

      // Incoming Seqnbr, timestamp it and have next packet respond.
      if(PacketHead.Type & SEQNBR)
      {

        if(p_Connect[ConnectionId].dropped_seqnbr == PacketHead.SeqNbr)
        {
          // turn off the resending requests.
          p_Connect[ConnectionId].dropped_seqnbr = NO_SEQ_NBR;
          p_Connect[ConnectionId].dropped_time = 0;
        }

        p_Connect[ConnectionId].recvbuffer
            [p_Connect[ConnectionId].recvwritepointer].packetseqnbr = PacketHead.SeqNbr;

			}
      else
      {
        p_Connect[ConnectionId].recvbuffer
            [p_Connect[ConnectionId].recvwritepointer].packetseqnbr = NO_SEQ_NBR;
      }

      // Incoming AckSeqNbr, we can clear out the send buffer and update ping
      if(PacketHead.Type & ACKSEQNBR)
      {

				Counter = NO_SEQ_NBR;

				if(p_Connect[ConnectionId].sendwritepointer < p_Connect[ConnectionId].sendreadpointer)
				{

					for(x = p_Connect[ConnectionId].sendreadpointer;x<=255;x++)
					{
						if(p_Connect[ConnectionId].sendbuffer[x].packetseqnbr == PacketHead.AckSeqNbr)
						{
							Counter = x;
							break;
						}
					}

					if(Counter == NO_SEQ_NBR)
					{
						for(x=0;x<p_Connect[ConnectionId].sendwritepointer;x++)
						{
							if(p_Connect[ConnectionId].sendbuffer[x].packetseqnbr == PacketHead.AckSeqNbr)
							{
								Counter = x;
								break;
							}
						}
					}
				}
				else
				{
					for(x = p_Connect[ConnectionId].sendreadpointer;x<p_Connect[ConnectionId].sendwritepointer;x++)
					{
						if(p_Connect[ConnectionId].sendbuffer[x].packetseqnbr == PacketHead.AckSeqNbr)
						{
							Counter = x;
							break;
						}
					}
				}

        // Update SendBuffer Counters
				if(Counter != NO_SEQ_NBR)
				{
					p_Connect[ConnectionId].sendreadpointer = Counter;
				}

        if(p_Connect[ConnectionId].sendwritepointer < p_Connect[ConnectionId].sendreadpointer)
        {
          p_Connect[ConnectionId].sendcount = (256 - p_Connect[ConnectionId].sendreadpointer) +
            p_Connect[ConnectionId].sendwritepointer;
        }
        else
        {
          p_Connect[ConnectionId].sendcount = 
            p_Connect[ConnectionId].sendwritepointer - 
            p_Connect[ConnectionId].sendreadpointer;
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
					p_Connect[ConnectionId].todo |= KEEPALIVE;
					p_Connect[ConnectionId].todo |= ACK;
					p_Connect[ConnectionId].todo |= ACKSEQNBR;
					p_MutexShared->leaveMutex();

					Send(ConnectionId, false, 0, 0);

					p_MutexShared->enterMutex();

				}
      }

      // Loop through and see if we have more seqnbr packets
      // This code should only be hit if there are out of order
      // or missing seq nbrs.

			Counter = NO_SEQ_NBR;
			bool Found = false;

      if(p_Connect[ConnectionId].recvwritepointer < p_Connect[ConnectionId].recvreadpointer)
      {

        for(x = p_Connect[ConnectionId].recvreadpointer; 
            x <= 255;
            x++)
        {
          if((p_Connect[ConnectionId].recvbuffer[x].packetseqnbr != 
              p_Connect[ConnectionId].recvseqnbr) &&
							p_Connect[ConnectionId].recvbuffer[x].packetseqnbr != NO_SEQ_NBR &&
              p_Connect[ConnectionId].recvbuffer[x].processed == false)
          {
            Counter = x;
          }

					if(p_Connect[ConnectionId].recvbuffer[x].packetseqnbr == 
              p_Connect[ConnectionId].recvseqnbr)
					{
						Counter = NO_SEQ_NBR;
						Found = true;
						break;
					}

        }

				if(Found == false)
				{
					for(x = 0; 
							x < p_Connect[ConnectionId].recvwritepointer;
							x++)
					{

						if((p_Connect[ConnectionId].recvbuffer[x].packetseqnbr != 
								p_Connect[ConnectionId].recvseqnbr) &&
								p_Connect[ConnectionId].recvbuffer[x].packetseqnbr != NO_SEQ_NBR &&
								p_Connect[ConnectionId].recvbuffer[x].processed == false)
						{
							Counter = x;
						}

						if(p_Connect[ConnectionId].recvbuffer[x].packetseqnbr == 
								p_Connect[ConnectionId].recvseqnbr)
						{
							Counter = NO_SEQ_NBR;
							break;
						}
					}
				}
      }
      else
      {

        for(x = p_Connect[ConnectionId].recvreadpointer; 
            x < p_Connect[ConnectionId].recvwritepointer;
            x++)
        {

          if((p_Connect[ConnectionId].recvbuffer[x].packetseqnbr != 
              p_Connect[ConnectionId].recvseqnbr) &&
							p_Connect[ConnectionId].recvbuffer[x].packetseqnbr != NO_SEQ_NBR &&
              p_Connect[ConnectionId].recvbuffer[x].processed == false)
          {
            Counter = x;
          }

					if(p_Connect[ConnectionId].recvbuffer[x].packetseqnbr == 
              p_Connect[ConnectionId].recvseqnbr)
					{
						Counter = NO_SEQ_NBR;
						break;
					}
        }
      }

			if(Counter != NO_SEQ_NBR)
			{
				if(p_Connect[ConnectionId].dropped_seqnbr == NO_SEQ_NBR)
				{
					p_Connect[ConnectionId].dropped_seqnbr = p_Connect[ConnectionId].recvseqnbr;
					p_Connect[ConnectionId].dropped_time = DROPPED_TIMEOUT;
				}
			}

      // Update RecvBuffer Counters
      p_Connect[ConnectionId].recvwritepointer++;

      if(p_Connect[ConnectionId].recvwritepointer < p_Connect[ConnectionId].recvreadpointer)
      {
        p_Connect[ConnectionId].recvcount = (256 - p_Connect[ConnectionId].recvreadpointer) +
          p_Connect[ConnectionId].recvwritepointer;
      }
      else
      {
        p_Connect[ConnectionId].recvcount = 
          p_Connect[ConnectionId].recvwritepointer - 
          p_Connect[ConnectionId].recvreadpointer;
      }

      p_MutexShared->leaveMutex();

    }

SKIP:

    // update time on connections.
    endtime = clock();

    p_MutexShared->enterMutex();

    for(Counter = 0; Counter<=255;Counter++)
    {
      
      if(p_Connect[Counter].inuse == true && 
         p_Connect[Counter].pendingclose == false)
      {
				p_Connect[Counter].datarate_counter -= (endtime - begintime);
        p_Connect[Counter].keepalive_time -= (endtime - begintime);

        if(p_Connect[Counter].dropped_seqnbr != NO_SEQ_NBR)
        {
          p_Connect[Counter].dropped_time -= (endtime - begintime);
        }

				if(p_Connect[Counter].datarate_counter <= 5)
				{
					p_Connect[Counter].datarate_counter = DATARATE_TIMEOUT;
					p_Connect[Counter].recvdatarate = p_Connect[Counter].recvdatabytes;
					p_Connect[Counter].senddatarate = p_Connect[Counter].senddatabytes;
					p_Connect[Counter].recvdatabytes = 0;
					p_Connect[Counter].senddatabytes = 0;
				}

        if(p_Connect[Counter].keepalive_time <= 5)
        {
          p_Connect[Counter].keepalive_time = KEEPALIVE_TIMEOUT;
          p_Connect[Counter].keepalive_counter++;
          if(p_Connect[Counter].keepalive_counter >= KEEPALIVE_COUNTER)
          {
            // Timeout
            p_Connect[Counter].pendingclose = true;

            p_MutexShared->leaveMutex();

            ReportLocalMessage(Counter, TIMEOUT);

            p_MutexShared->enterMutex();
          }
          else
          {

            // Send Keepalive
            p_Connect[Counter].todo |= KEEPALIVE;
						p_Connect[Counter].todo |= ACKSEQNBR;
						p_Connect[Counter].todo |= TIME;

            p_MutexShared->leaveMutex();

            Send(Counter, false, 0, 0);

            p_MutexShared->enterMutex();

						if(p_Connect[Counter].keepalive_counter == 2)
						{
							// When lag is first detected report lag.
							// Using counter == 2 because == 1 is very common.
							p_MutexShared->leaveMutex();
							ReportLocalMessage(Counter, LAG);
							p_MutexShared->enterMutex();
						}

          }
        }

        if(p_Connect[Counter].dropped_seqnbr != NO_SEQ_NBR)
        {
          if(p_Connect[Counter].dropped_time <= 5)
          {

            p_Connect[Counter].dropped_time = DROPPED_TIMEOUT;

            // Send Resend
            p_Connect[Counter].todo |= RESEND;
            p_Connect[Counter].todo |= ACKSEQNBR;

            p_MutexShared->leaveMutex();

            Send(Counter, false, 0, 0);
            ReportLocalMessage(Counter, PACKETLOSS);

            p_MutexShared->enterMutex();
          }
        }
      }
    }

    p_MutexShared->leaveMutex();

    // Give CPU breathing room.
    yield();
  }

  return;

}

void NetworkingClass::final()
{

}

// -----------------------------------------------------------------
 
short NetworkingClass::Connect(short ConnectionId, char *IP, char *Port, void *Data, short DataLength)
{

  if(GetActive(ConnectionId) == true)
  {
    return NET_INVALID_STATE;
  }

  p_MutexShared->enterMutex();

  // Set ip/port of other side so we can ack/nack a response correctly.
  p_Connect[ConnectionId].inuse = true;

  p_Connect[ConnectionId].addr = IP;
  p_Connect[ConnectionId].port = (tpport_t)atoi(Port);

  p_Connect[ConnectionId].todo |= CONNECT;

	p_Connect[ConnectionId].recvdatabytes = 0;
	p_Connect[ConnectionId].senddatabytes = 0;
	p_Connect[ConnectionId].datarate_counter = DATARATE_TIMEOUT;

  p_Connect[ConnectionId].keepalive_time = KEEPALIVE_TIMEOUT;
  p_Connect[ConnectionId].keepalive_counter = 0;

  p_Connect[ConnectionId].dropped_seqnbr = NO_SEQ_NBR;

  p_Connect[ConnectionId].sendreadpointer = 0;
  p_Connect[ConnectionId].sendwritepointer = 0;
  p_Connect[ConnectionId].recvreadpointer = 0;
  p_Connect[ConnectionId].recvwritepointer = 0;
  p_Connect[ConnectionId].recvcount = 0;
  p_Connect[ConnectionId].sendcount = 0;

  p_Connect[ConnectionId].latency = 0;
  p_Connect[ConnectionId].ping = 0;
  p_Connect[ConnectionId].recvreadpointer = 0;
  p_Connect[ConnectionId].senddatarate = 0;
  p_Connect[ConnectionId].recvseqnbr = 0;

	p_Connect[ConnectionId].timestamp = clock();
  
	p_sConnectionCount++;

  p_MutexShared->leaveMutex();

  return Send(ConnectionId, true, Data, DataLength);
}

short NetworkingClass::Disconnect(short ConnectionId, void *Data, short DataLength)
{

  if(GetActive(ConnectionId) == false)
  {
    return NET_INVALID_STATE;
  }

  p_MutexShared->enterMutex();

  p_Connect[ConnectionId].todo |= DISCONNECT;

  p_MutexShared->leaveMutex();

  return Send(ConnectionId, true, Data, DataLength);
}

short NetworkingClass::Send(short ConnectionId, bool Guaranteed, void *Data, short DataLength)
{

  if(GetActive(ConnectionId) == false)
  {
    return NET_INVALID_STATE;
  }

  long          Counter;
  short         x;

  // -------------------------------

  // Validate size
  if(DataLength > 4000)
  {
      return NET_TOO_MUCH_DATA;
  }

  // ------------------------------
  // Main
  // ------------------------------
  p_MutexShared->enterMutex();

  // Set Data flag if anything is passed in.
  if(DataLength > 0)
  {
    p_Connect[ConnectionId].todo |= DATA;
  }

  // Set Seq Nbr
  if(Guaranteed == true)
  {
    p_Connect[ConnectionId].todo |= SEQNBR;
    p_Connect[ConnectionId].todo |= ACKSEQNBR;
    p_Connect[ConnectionId].todo |= TIME;
  }

  // Set Latency time
  if(p_Connect[ConnectionId].todo & TIME)
  {
		clock_t nowtime = clock();

		p_Connect[ConnectionId].latency = nowtime - p_Connect[ConnectionId].timestamp;

		if(p_Connect[ConnectionId].latency < 0) p_Connect[ConnectionId].latency = 0;
  }

	p_Connect[ConnectionId].timestamp = clock();

  // Build Packet.
  char      Buffer[4096];
  long      Length = 0;
  short     tempType;

  NetHeader Head;
  memset(&Head, 0, sizeof(Head));

  Head.StartFiller = PACKET_IDENTIFIER;
  Head.Type = p_Connect[ConnectionId].todo;
  Head.SeqNbr = p_Connect[ConnectionId].sendwritepointer;
  Head.AckSeqNbr = p_Connect[ConnectionId].recvseqnbr-1;
  Head.Time = p_Connect[ConnectionId].latency;

  // Save off copy so we don't have flipflopped constants
  tempType = Head.Type;

  // Build Header
  FlipFlop(Head.StartFiller);
  memcpy(Buffer + Length, &Head.StartFiller, 2);

  Length += 2;

  FlipFlop(Head.Type);
  memcpy(Buffer + Length, &Head.Type, 2);

  Length += 2;

  if(tempType & SEQNBR)
  {
    //FlipFlop(Head.SeqNbr);
    memcpy(Buffer + Length, &Head.SeqNbr, 2);

    Length += 2;
  }

  if(tempType & ACKSEQNBR)
  {
    memcpy(Buffer + Length, &Head.AckSeqNbr, 2);

    Length += 2;
  }

  if(tempType & TIME)
  {
    FlipFlop(Head.Time);
    memcpy(Buffer + Length, &Head.Time, 2);

    Length += 2;
  }

  if(tempType & DATA)
  {
    // This data should be flipflopped already by App.
    memcpy(Buffer + Length, 
           Data,
           DataLength);

    Length += DataLength;

  }

  if(tempType & SEQNBR)
  {

    // See if wrap around sendbuffer is already full.
    if(p_Connect[ConnectionId].sendwritepointer+1 ==
        p_Connect[ConnectionId].sendreadpointer)
    {
      // Buffer full.
      // Still pondering if we should disconnect or have a wait in here.
    }

    // Timestamp and place contents into buffer
    //p_Connect[ConnectionId].sendbuffer[p_Connect[ConnectionId].sendwritepointer]
    //         .timestamp = clock();

    p_Connect[ConnectionId].sendbuffer[p_Connect[ConnectionId].sendwritepointer]
      .packetseqnbr = p_Connect[ConnectionId].sendwritepointer;

    p_Connect[ConnectionId].sendbuffer[p_Connect[ConnectionId].
                sendwritepointer].packetlength = Length;

    if(p_Connect[ConnectionId].sendbuffer[p_Connect[ConnectionId].sendwritepointer]
           .packet != 0)
    {
      delete []p_Connect[ConnectionId].sendbuffer[p_Connect[ConnectionId].
                sendwritepointer].packet;
    }

    p_Connect[ConnectionId].sendbuffer[p_Connect[ConnectionId].sendwritepointer]
           .packet = new char[Length];

    memcpy(p_Connect[ConnectionId].sendbuffer[p_Connect[ConnectionId].sendwritepointer]
           .packet, Buffer, Length);

    // Update SendBuffer Counters.

    p_Connect[ConnectionId].sendwritepointer++;

    p_Connect[ConnectionId].sendcount = 
      p_Connect[ConnectionId].sendwritepointer - p_Connect[ConnectionId].sendreadpointer;

  }

  if(tempType & ACKSEQNBR)
  {

		bool Found = false;

    // Update RecvBuffer Counters.
    if(p_Connect[ConnectionId].recvwritepointer < p_Connect[ConnectionId].recvreadpointer)
    {
      for(x = p_Connect[ConnectionId].recvreadpointer; x <= 255;x++)
      {
        if(p_Connect[ConnectionId].recvbuffer[x].processed == true)
        {
          p_Connect[ConnectionId].recvreadpointer++;
        }
        else
        {
					Found = true;
          break;
        }
      }

			if(Found == false)
			{
				for(x = 0; x < p_Connect[ConnectionId].recvwritepointer;x++)
				{
					if(p_Connect[ConnectionId].recvbuffer[x].processed == true)
					{
						p_Connect[ConnectionId].recvreadpointer++;
					}
					else
					{
						break;
					}
				}
			}

    }
    else
    {

      for(x = p_Connect[ConnectionId].recvreadpointer; x < p_Connect[ConnectionId].recvwritepointer;x++)
      {
        if(p_Connect[ConnectionId].recvbuffer[x].processed == true)
        {
          p_Connect[ConnectionId].recvreadpointer++;
        }
        else
        {
          break;
        }
      }
    }

		if(p_Connect[ConnectionId].recvwritepointer < p_Connect[ConnectionId].recvreadpointer)
    {
      p_Connect[ConnectionId].recvcount = (256 - p_Connect[ConnectionId].recvreadpointer) +
        p_Connect[ConnectionId].recvwritepointer;
    }
    else
    {
      p_Connect[ConnectionId].recvcount = p_Connect[ConnectionId].recvwritepointer - p_Connect[ConnectionId].recvreadpointer;
    }
  }

  p_Connect[ConnectionId].todo = 0;

  // Do the send.
  p_Socket->setPeer(p_Connect[ConnectionId].addr, p_Connect[ConnectionId].port);
  p_Socket->send(Buffer, Length);

	p_Connect[ConnectionId].senddatabytes += Length;

  p_MutexShared->leaveMutex();

  return NET_NO_ERROR;
}

short NetworkingClass::Recv(short ConnectionId, short *Type, void *Data, short *DataLength)
{

  if(GetActive(ConnectionId) == false)
  {
    return NET_INVALID_STATE;
  }

  long Counter = NO_SEQ_NBR;
	short x;

  // Loop through and get next message.
  p_MutexShared->enterMutex();

  if(p_Connect[ConnectionId].recvwritepointer < p_Connect[ConnectionId].recvreadpointer)
  {

    for(x = p_Connect[ConnectionId].recvreadpointer; 
        x <= 255;
        x++)
    {
      if(((p_Connect[ConnectionId].recvbuffer[x].packetseqnbr == 
          p_Connect[ConnectionId].recvseqnbr) ||
          p_Connect[ConnectionId].recvbuffer[x].packetseqnbr == NO_SEQ_NBR) && 
          p_Connect[ConnectionId].recvbuffer[x].processed == false)
      {

        if(Type != 0)
        {
          *Type = p_Connect[ConnectionId].recvbuffer[x].type;
          Data = ((char *)p_Connect[ConnectionId].recvbuffer[x].packet + 
                 p_Connect[ConnectionId].recvbuffer[x].dataoffset);
          *DataLength = p_Connect[ConnectionId].recvbuffer[x].datalength;
        }

        p_Connect[ConnectionId].recvbuffer[x].processed = true;

        if(p_Connect[ConnectionId].recvbuffer[x].packetseqnbr != NO_SEQ_NBR)
        {
          p_Connect[ConnectionId].recvseqnbr++;
        }

        Counter = x;

        break;
      }
    }

    if(Counter == NO_SEQ_NBR)
    {

      for(x = 0; 
          x < p_Connect[ConnectionId].recvwritepointer;
          x++)
      {

				if(((p_Connect[ConnectionId].recvbuffer[x].packetseqnbr == 
            p_Connect[ConnectionId].recvseqnbr) ||
            p_Connect[ConnectionId].recvbuffer[x].packetseqnbr == NO_SEQ_NBR) && 
            p_Connect[ConnectionId].recvbuffer[x].processed == false)
        {

          if(Type != 0)
          {
            *Type = p_Connect[ConnectionId].recvbuffer[x].type;
            Data = ((char *)p_Connect[ConnectionId].recvbuffer[x].packet + 
                   p_Connect[ConnectionId].recvbuffer[x].dataoffset);
            *DataLength = p_Connect[ConnectionId].recvbuffer[x].datalength;
          }

          p_Connect[ConnectionId].recvbuffer[x].processed = true;

          if(p_Connect[ConnectionId].recvbuffer[x].packetseqnbr != NO_SEQ_NBR)
          {
            p_Connect[ConnectionId].recvseqnbr++;
          }

          Counter = x;

          break;
        }
      }
    }
  }
  else
  {
    for(x = p_Connect[ConnectionId].recvreadpointer; 
        x < p_Connect[ConnectionId].recvwritepointer;
        x++)
    {
      if(((p_Connect[ConnectionId].recvbuffer[x].packetseqnbr == 
          p_Connect[ConnectionId].recvseqnbr) ||
          p_Connect[ConnectionId].recvbuffer[x].packetseqnbr == NO_SEQ_NBR) && 
          p_Connect[ConnectionId].recvbuffer[x].processed == false)
      {

				// Ignore Keepalives
				if(p_Connect[ConnectionId].recvbuffer[x].type & KEEPALIVE)
				{
					p_Connect[ConnectionId].recvbuffer[x].processed = true;
					continue;
				}

        if(Type != 0)
        {
          *Type = p_Connect[ConnectionId].recvbuffer[x].type;
          Data = ((char *)p_Connect[ConnectionId].recvbuffer[x].packet + 
                 p_Connect[ConnectionId].recvbuffer[x].dataoffset);
          *DataLength = p_Connect[ConnectionId].recvbuffer[x].datalength;
        }

        p_Connect[ConnectionId].recvbuffer[x].processed = true;

        if(p_Connect[ConnectionId].recvbuffer[x].packetseqnbr != NO_SEQ_NBR)
        {
          p_Connect[ConnectionId].recvseqnbr++;
        }

        Counter = x;

        break;
      }
    }
  }

  // Set variables

  if(Counter == NO_SEQ_NBR)
  {
    if(p_Connect[ConnectionId].pendingclose == true)
    {
      p_Connect[ConnectionId].inuse = false;
      p_Connect[ConnectionId].pendingclose = false;
      p_sConnectionCount--;
    }

		// clean this stuff up
		p_MutexShared->leaveMutex();

    return NET_NO_MORE_DATA;
  }

  p_MutexShared->leaveMutex();

  return NET_NO_ERROR;
}

short NetworkingClass::Accept(short ConnectionId, void *Data, short DataLength)
{

  if(GetActive(ConnectionId) == false)
  {
    return NET_INVALID_STATE;
  }

  p_MutexShared->enterMutex();

  p_Connect[ConnectionId].todo |= CONNECT;
  p_Connect[ConnectionId].todo |= ACK;
  p_sConnectionCount++;

  p_MutexShared->leaveMutex();

  return Send(ConnectionId, true, Data, DataLength);

}

short NetworkingClass::Reject(short ConnectionId, void *Data, short DataLength)
{

  if(GetActive(ConnectionId) == false)
  {
    return NET_INVALID_STATE;
  }

  p_MutexShared->enterMutex();

  p_Connect[ConnectionId].todo |= CONNECT;
  p_Connect[ConnectionId].todo |= REJECT;

  p_MutexShared->leaveMutex();

  return Send(ConnectionId, true, Data, DataLength);

}

short NetworkingClass::Reject(InetHostAddress addr, tpport_t port, void *Data, short DataLength)
{

  // Build Packet.
  char      Buffer[4096];
  long      Length = 0;

  NetHeader Head;
  memset(&Head, 0, sizeof(Head));

  Head.StartFiller = PACKET_IDENTIFIER;
  Head.Type = CONNECT | REJECT;

  if(DataLength > 0)
  {
    Head.Type |= DATA;
  }

  short tempType = Head.Type;

  // Build Header
  FlipFlop(Head.StartFiller);
  memcpy(Buffer + Length, &Head.StartFiller, 2);

  Length += 2;

  FlipFlop(Head.Type);
  memcpy(Buffer + Length, &Head.Type, 2);

  Length += 2;

  if(tempType & DATA)
  {
    // This data should be flipflopped already by App.
    memcpy(Buffer + Length, 
           Data,
           DataLength);

    Length += DataLength;
  }

  // Do the send.
  p_Socket->setPeer(addr, port);
  p_Socket->send(Buffer, Length);

  return NET_NO_ERROR;
}

short NetworkingClass::Resend(short ConnectionId, NetBuffer *Packet)
{

  if(GetActive(ConnectionId) == false)
  {
    return NET_INVALID_STATE;
  }

  p_MutexShared->enterMutex();

  // Add in Poss Dupe Flag to original message
  NET_TYPE  tempType;

  memcpy(&tempType, (char *)Packet->packet + 2, sizeof(NET_TYPE));

  FlipFlop(tempType);

  tempType |= POSSDUPE;

  FlipFlop(tempType);

  memcpy((char *)Packet->packet + 2, &tempType, sizeof(NET_TYPE));

  // Now resend
  p_Socket->setPeer(p_Connect[ConnectionId].addr, p_Connect[ConnectionId].port);
  p_Socket->send(Packet->packet, Packet->packetlength);  

  // Cause a dupe for testing
  /*
  if(rand() % 5 == 2)
  {
    p_Socket->send(Packet->packet, Packet->packetlength);
  }
  */

  p_MutexShared->leaveMutex();

  return NET_NO_ERROR;
}

short NetworkingClass::ReportLocalMessage(short ConnectionId, NET_TYPE Type)
{

  p_MutexShared->enterMutex();

  // Make recv packet timeout
  p_Connect[ConnectionId].recvbuffer
      [p_Connect[ConnectionId].recvwritepointer].processed = false;

  p_Connect[ConnectionId].recvbuffer
      [p_Connect[ConnectionId].recvwritepointer].packetlength = 0;

  if(p_Connect[ConnectionId].recvbuffer
      [p_Connect[ConnectionId].recvwritepointer].packet != 0)
  {
    delete []p_Connect[ConnectionId].recvbuffer
      [p_Connect[ConnectionId].recvwritepointer].packet;

    p_Connect[ConnectionId].recvbuffer
      [p_Connect[ConnectionId].recvwritepointer].packet = 0;
  }

  p_Connect[ConnectionId].recvbuffer
      [p_Connect[ConnectionId].recvwritepointer].packetseqnbr = NO_SEQ_NBR;

  p_Connect[ConnectionId].recvbuffer
      [p_Connect[ConnectionId].recvwritepointer].dataoffset = 0;

  p_Connect[ConnectionId].recvbuffer
      [p_Connect[ConnectionId].recvwritepointer].datalength = 0;

  p_Connect[ConnectionId].recvbuffer
      [p_Connect[ConnectionId].recvwritepointer].type = Type;

  p_Connect[ConnectionId].recvwritepointer++;  

  p_MutexShared->leaveMutex();

  return NET_NO_ERROR;
}

short NetworkingClass::GetConnections()
{
  return p_sConnectionCount;
}

bool NetworkingClass::GetActive(short ConnectionId)
{
  return p_Connect[ConnectionId].inuse;
}

short NetworkingClass::GetPing(short ConnectionId)
{
  return p_Connect[ConnectionId].ping;
}

short NetworkingClass::GetLatency(short ConnectionId)
{
  return p_Connect[ConnectionId].latency;
}

short NetworkingClass::GetRecvMessages(short ConnectionId)
{

  long Counter = 0;
  short x;

  // Loop through and get next message.
  p_MutexShared->enterMutex();

  if(p_Connect[ConnectionId].recvwritepointer < p_Connect[ConnectionId].recvreadpointer)
  {

    for(x = p_Connect[ConnectionId].recvreadpointer; 
        x <= 255;
        x++)
    {
      if(p_Connect[ConnectionId].recvbuffer[x].processed == false)
      {
				if(p_Connect[ConnectionId].recvbuffer[x].type & KEEPALIVE)
				{
					continue;
				}
        Counter++;
      }
    }

    for(x = 0; 
        x < p_Connect[ConnectionId].recvwritepointer;
        x++)
    {
      if(p_Connect[ConnectionId].recvbuffer[x].processed == false)
      {
				if(p_Connect[ConnectionId].recvbuffer[x].type & KEEPALIVE)
				{
					continue;
				}
        Counter++;
      }
    }
  }
  else
  {
    for(x = p_Connect[ConnectionId].recvreadpointer; 
        x < p_Connect[ConnectionId].recvwritepointer;
        x++)
    {
      if(p_Connect[ConnectionId].recvbuffer[x].processed == false)
      {
				if(p_Connect[ConnectionId].recvbuffer[x].type & KEEPALIVE)
				{
					continue;
				}
        Counter++;
      }
    }
  }

  p_MutexShared->leaveMutex();

  return Counter;

}

short NetworkingClass::GetRecvBufferCount(short ConnectionId)
{
  return p_Connect[ConnectionId].recvcount;
}

short NetworkingClass::GetSendBufferCount(short ConnectionId)
{
  return p_Connect[ConnectionId].sendcount;
}

unsigned long NetworkingClass::GetRecvDataRate(short ConnectionId)
{
  return p_Connect[ConnectionId].recvdatarate;
}

unsigned long NetworkingClass::GetSendDataRate(short ConnectionId)
{
  return p_Connect[ConnectionId].senddatarate;
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