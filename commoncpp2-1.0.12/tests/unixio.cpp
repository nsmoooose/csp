// test_cp.cpp
// Test Client/Server-Kommunikation via TCP/Unix
// unter Verwendung von ``tcpstream''/``unixtream''


#include <iostream>
#include <cc++/thread.h>
#include <cc++/socket.h>
#include <cc++/unix.h>
#include <cc++/slog.h>


#define SO_TCP
//#define SO_UNIX


static const char socketfile [] = "./test.socket";


class ServerThread : public ost::Thread
{
  protected:
    virtual void run ();
};


class ClientThread : public ost::Thread
{
  protected:
    virtual void run ();
};


void ServerThread::run ()
{
  try
  {
#ifdef SO_TCP    
    ost::InetAddress bind ("localhost");
    ost::TCPSocket bsocket (bind, 4711);
#endif
#ifdef SO_UNIX
    unlink (socketfile);
    ost::UnixSocket bsocket (socketfile);
#endif

    if (bsocket.isPendingConnection ())
    {
      try
      {
#ifdef SO_TCP
        ost::tcpstream server;
#endif
#ifdef SO_UNIX
        ost::unixstream server;
#endif
      
        server.setTimeout (timeout_t (5000L));
        server.open (bsocket);

        while (! server.fail () && ! server.eof ())
        {
          char s [256];

          server.getline (s, sizeof s);
          server.sync ();
          server << s << std::endl;
        };
        if (server.fail ())
        {
          std::cerr << "server fail" << std::endl;
        };
        if (server.eof ())
        {
          std::cerr << "server eof" << std::endl;
        }
      }
      catch (ost::Socket * socket)
      {
        std::cerr << "server error" << std::endl;
      }
    }
  }
  catch (ost::Socket * socket)
  {
    std::cerr << "socket error" << std::endl;
  }
}


void ClientThread::run ()
{
  try
  {
#ifdef SO_TCP
    ost::tcpstream client;
#endif
#ifdef SO_UNIX
    ost::unixstream client;
#endif
  
    client.setTimeout (timeout_t (5000L));
#ifdef SO_TCP
    client.open ("localhost:4711");
#endif
#ifdef SO_UNIX
    client.open (socketfile);
#endif

    while (! client.fail () && ! client.eof () 
           && 
           ! std::cin.fail () && ! std::cin.eof ())
    {
      char s [256];

      std::cin.getline (s, sizeof s);
      std::cin.sync ();
      client << s << std::endl;
      client.getline (s, sizeof s);
      std::cout << "from server: " << s << std::endl;
    };
    if (client.fail ())
    {
      std::cerr << "client fail" << std::endl;
    };
    if (client.eof ())
    {
      std::cerr << "client eof" << std::endl;
    };
    if (std::cin.fail ())
    {
      std::cerr << "cin fail" << std::endl;
    };
    if (std::cin.eof ())
    {
      std::cerr << "cin eof" << std::endl;
    }
  }
  catch (ost::Socket * socket)
  {
    std::cerr << "client error" << std::endl;
  }
}


int main ()
{
  try
  {
    ServerThread server;

    server.start ();
    std::cerr << "server started" << std::endl;
    ost::Thread::sleep (timeout_t (5000L));

    ClientThread client;
    client.start ();
    std::cerr << "client started" << std::endl;
    do
    {
      ost::Thread::sleep (timeout_t (1000L));
    }
    while (server.isRunning () && client.isRunning ());
    if (server.isRunning ())
    {
      std::cerr << "server still running" << std::endl;
    };
    if (client.isRunning ())
    {
      std::cerr << "client still running" << std::endl;
    }
  }
  catch (...)
  {
    std::cerr << "general error" << std::endl;
  };
  
  return 0;
}
