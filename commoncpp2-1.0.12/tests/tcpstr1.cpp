#include <cc++/socket.h>
#include <iostream>
#include <cstdlib>

#ifdef	CCXX_NAMESPACES
using namespace std;
using namespace ost;
#endif

class ThreadOut: public Thread
{
public:
	ThreadOut()
	{
		start();
	}
	void run()
	{
		tcpstream tcp("127.0.0.1:9000");
		tcp << "pippo" << endl;
		tcp.close();
	}
};

int main(int argc, char *argv[])
{
	char line[200];

	InetAddress addr = "127.0.0.1";
	TCPSocket *sock = new TCPSocket(addr, 9000);
	// write some output automatically
	ThreadOut thread;
	while (1){
		if (sock->isPendingConnection()){
			tcpstream tcp(*sock);
			tcp.getline(line, 200);
			cout << line << endl;
			tcp.close();
			return 0;
		}
	}
	return 0;
}

