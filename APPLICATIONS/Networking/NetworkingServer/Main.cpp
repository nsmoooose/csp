#include "NetworkingClass.h"

NetworkingClass *Server;

class Demo : public Thread
{

private:

  void            initial();
  void            run();
  void            final();

public:

};

void Demo::initial()
{
}

void Demo::run()
{

  char inputchars[256];

  // Thread can gracefully shutdown.
  setCancel(Thread::cancelDeferred);

  while(testCancel() == false)
  {
    cout << endl << "Command>";
    cin >> inputchars;
   
    if(stricmp(inputchars, "/q") == 0)
    {
      break;
    }

    if(stricmp(inputchars, "/d") == 0)
    {
      Server->Disconnect(0, 0, 0);
    }

    if(stricmp(inputchars, "/s") == 0)
    {
      Server->Send(0, true, "Blah", 5);
    }

    if(stricmp(inputchars, "/cs") == 0)
    {
      for(unsigned char x=0;x<100;x++)
      {
        Server->Send(0, true, "Blah", 5);
      }
    }

    if(stricmp(inputchars, "/stats") == 0)
    {
      cout << endl;
      cout << "Connections  - " << Server->GetConnections() << endl;
      cout << "Ping         - " << Server->GetPing(0) << endl;
      cout << "Latency      - " << Server->GetLatency(0) << endl;
      cout << "RecvBuffer   - " << Server->GetRecvBufferCount(0) << endl;
      cout << "SendBuffer   - " << Server->GetSendBufferCount(0) << endl;
      cout << "Messages     - " << Server->GetRecvMessages(0) << endl;
      cout << "RecvDataRate - " << Server->GetRecvDataRate(0) << endl;
      cout << "SendDataRate - " << Server->GetSendDataRate(0);
    }

		if(stricmp(inputchars, "/?") == 0)
		{
			cout << endl;
			cout << "/q - quit" << endl;
			cout << "/c - connect" << endl;
			cout << "/d - disconnect" << endl;
			cout << "/s - send a dummy packet" << endl;
			cout << "/cs - sends 100 dummy packets" << endl;
			cout << "/stats - display stats";
		}

  }
}

void Demo::final()
{
}

int main(void)
{

	cout << "Networking Server v 1.0" << endl;
	cout << "You may type in commands while there is activity going on." << endl;
	cout << "Type /? for help" << endl;
	cout << "---------------------------------------------";

  NET_TYPE   Type;
  void      *Data = 0;
  short      Length;
  long       Counter;
	char tempType[256];
  char inputchars[256];

	cout << endl << "Enter IP address or DNS name to listen on" << endl;
	cin >> inputchars;

	char ipaddress[256];
	strcpy(ipaddress, inputchars);

	cout << "Enter port" << endl;
	cin >> inputchars;

	char port[256];
	strcpy(port, inputchars);

	cout << "Now Listening on " << ipaddress << " " << port;

  Server = new NetworkingClass(ipaddress, port);

  Demo *InputThread;

  InputThread = new Demo;

  InputThread->start();

  // Main Receiving Loop
  while(InputThread->isRunning() == true)
  {
    for(Counter = 0; Counter < 255; Counter++)
    {

      if(Server->GetActive(Counter) == true)
      {
        while(Server->Recv(Counter, &Type, Data, &Length) != NET_NO_MORE_DATA)
        {
          if(Type & CONNECT)
          {
            Server->Accept(Counter, 0, 0);
          }

					memset(tempType, 0, sizeof(tempType));

					if(Type & CONNECT)
					{
					 strcat(tempType, " CONNECT");
					}

					if(Type & DISCONNECT)
					{
						strcat(tempType, " DISCONNECT");
					}

					if(Type & LAG)
					{
						strcat(tempType, " LAG");
					}

					if(Type & LAG_RECOVERED)
					{
						strcat(tempType, " LAG RECOVERED");
					}

					if(Type & PACKETLOSS)
					{
						strcat(tempType, " PACKETLOSS");
					}

					if(Type & DATA)
					{
						strcat(tempType, " DATA");
					}

					if(Type & TIMEOUT)
					{
						strcat(tempType, " TIMEOUT");
					}

					cout << endl << "<PACKET RECEIVED> Conn: " << Counter << " "
						              "Type: " << Type << tempType << endl <<
												  "                  Stats " <<
							"L: " << Server->GetLatency(Counter) << " " <<
							"P: " << Server->GetPing(Counter) << " " <<
							"RBC: " << Server->GetRecvBufferCount(Counter) << " " <<
							"SBC: " << Server->GetSendBufferCount(Counter) << " " <<
							"RDR: " << Server->GetRecvDataRate(Counter) << " " <<
							"SDR: " << Server->GetSendDataRate(Counter) << ">";

        }
      }
    }

    Sleep(100);
  }

  // delete objects
  delete InputThread;
  delete Server;

  return 0;
}