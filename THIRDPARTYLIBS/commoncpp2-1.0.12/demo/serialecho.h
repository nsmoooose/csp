/** ********************************************************************
 * C/C++ Source: serialecho.h
 *
 * Class definitions for the SerialEcho and related classes.
 * This package requires the Serial, TTYSession and Thread classes
 * from the FSF Common C++ library (v 1.2.4 cplusplus.sourceforge.net)
 * 
 * SerialEcho is a monitor on the serial port which runs in its own
 * thread and is responsible for detecting and echoing any serial
 * input.  The class is based on the ttysession class so it can be
 * used as any fstream-like class
 *
 * Created: Sat Oct 14 12:35:46 EDT 2000
 * $Log: serialecho.h,v $
 * Revision 1.1  2003/05/31 14:28:33  brandonb
 * no message
 *
 * Revision 1.5.2.1  2002/09/27 13:22:27  fedemp
 * fixed constructor parameters in serialecho demo
 *
 * Revision 1.5  2002/05/08 23:18:30  dyfet
 * updated
 *
 * Revision 1.15  2002/02/17 09:26:09  freddy77
 * appling new coding style
 *
 * Revision 1.14  2002/02/16 14:04:33  freddy77
 * fix include
 *
 * Revision 1.13  2002/02/12 18:04:01  dyfet
 * master include under common.h now
 *
 * Revision 1.12  2002/01/10 21:22:32  freddy77
 * include header as all other program
 *
 * Revision 1.11  2001/12/05 10:50:01  freddy77
 * fixed some defines for C++ specification
 *
 * Revision 1.10  2001/10/30 13:31:39  dyfet
 * demo apps fixed and ost_check.m4 duplicative issue resolved
 *
 * Revision 1.9  2001/10/29 13:09:38  dyfet
 * fixed for current release
 *
 * Revision 1.8  2001/10/12 15:41:13  freddy77
 * Removed posix semantic
 *
 * Revision 1.7  2001/08/01 18:27:59  dyfet
 * new sample code
 *
 * Revision 1.1  2000/10/15 17:54:54  dyfet
 * serial demo
 *
 * Revision 1.3  2000/10/14 21:08:57  garym
 * First working version of the modem echo
 *
 *
 * @author:  Gary Lawrence Murphy <garym@canada.com>
 * Copyright:  2000 TeleDynamics Communications Inc (www.teledyn.com)
 * @version: $Id: serialecho.h,v 1.1 2003/05/31 14:28:33 brandonb Exp $
 ********************************************************************
*/

#ifndef SERIALECHO_H
#define SERIALECHO_H

#include <cc++/common.h>

#ifdef	CCXX_NAMESPACES
using namespace std;
using namespace ost;
#endif

class SerialEcho : public TTYSession {
 public:

  SerialEcho(const char *device,  
             int priority = 0, int stacksize = 0);

  // Exception classes
  class xError{}; // nebulous inexplicable error
  class xLocked{}; // port is there but we are locked out
  class xOverrun{}; // too much data, too little time

 protected:

  void run();
};

#endif
