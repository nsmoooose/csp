#include "NetworkingClass.h"

NetworkingClass *Client;

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

    if(stricmp(inputchars, "/c") == 0)
    {
			cout << "Enter IP address or DNS name to connect to" << endl;
			cin >> inputchars;

			char ipaddress[256];
			strcpy(ipaddress, inputchars);

			cout << "Enter port" << endl;
			cin >> inputchars;

			char port[256];
			strcpy(port, inputchars);

			cout << "Now Connecting to " << ipaddress << " " << port;
      Client->Connect(0, ipaddress, port, 0, 0);
    }

    if(stricmp(inputchars, "/d") == 0)
    {
      Client->Disconnect(0, 0, 0);
    }

    if(stricmp(inputchars, "/s") == 0)
    {
      Client->Send(0, true, "Blah", 5);
    }

    if(stricmp(inputchars, "/cs") == 0)
    {
      for(unsigned char x=0;x<100;x++)
      {
        Client->Send(0, true, "Blah", 5);
      }
    }

    if(stricmp(inputchars, "/stats") == 0)
    {
      cout << endl;
      cout << "Connections  - " << Client->GetConnections() << endl;
      cout << "Ping         - " << Client->GetPing(0) << endl;
      cout << "Latency      - " << Client->GetLatency(0) << endl;
      cout << "RecvBuffer   - " << Client->GetRecvBufferCount(0) << endl;
      cout << "SendBuffer   - " << Client->GetSendBufferCount(0) << endl;
      cout << "Messages     - " << Client->GetRecvMessages(0) << endl;
      cout << "RecvDataRate - " << Client->GetRecvDataRate(0) << endl;
      cout << "SendDataRate - " << Client->GetSendDataRate(0);
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

	cout << "Networking Client v 1.0" << endl;
	cout << "You may type in commands while there is activity going on." << endl;
	cout << "Type /? for help" << endl;
	cout << "---------------------------------------------";
  
  NET_TYPE   Type;
	char tempType[256];

  void      *Data = 0;
  short      Length;
  long       Counter;
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

  Client = new NetworkingClass(ipaddress, port);

  Demo *InputThread;

  InputThread = new Demo;

  InputThread->start();

  // Main Receiving Loop
  while(InputThread->isRunning() == true)
  {
    for(Counter = 0; Counter <= 255; Counter++)
    {
      if(Client->GetActive(Counter) == true)
      {
        while(Client->Recv(Counter, &Type, Data, &Length) != NET_NO_MORE_DATA)
        {

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
							"L: " << Client->GetLatency(Counter) << " " <<
							"P: " << Client->GetPing(Counter) << " " <<
							"RBC: " << Client->GetRecvBufferCount(Counter) << " " <<
							"SBC: " << Client->GetSendBufferCount(Counter) << " " <<
							"RDR: " << Client->GetRecvDataRate(Counter) << " " <<
							"SDR: " << Client->GetSendDataRate(Counter) << ">";

        }
      }

    }

    Sleep(100);
  }

  // delete objects
  delete InputThread;
  delete Client;

  return 0;
}