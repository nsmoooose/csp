
#include <cc++/slog.h>
#include <iostream>
#include <cstdio>

#ifdef	CCXX_NAMESPACES
using namespace std;
using namespace ost;
#endif


int main(int argc, char* argv[])
{
   slog("slogTest", Slog::classUser, Slog::levelInfo);
   slog.error() << "Howdy daemon and clog." << endl;
   slog.clogEnable(false);
   slog.info() << "This is only for the daemon." << endl;
   slog.clogEnable(true);
   slog.warn() << "Are you still there?" << endl;
   slog.level(Slog::levelNotice);
   slog.debug() << "I should not output, below level info" << endl;
   return 0;
}

