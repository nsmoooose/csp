/**********************************************************************
 * C/C++ Source: serialecho.cc
 *
 * Defines the methods for the SerialEcho class
 *
 * Created: Sat Oct 14 12:38:24 EDT 2000
 * $Log: serialecho.cpp,v $
 * Revision 1.1  2003/05/31 14:28:33  brandonb
 * no message
 *
 * Revision 1.5.2.3  2002/12/30 12:59:30  fedemp
 * fix
 *
 * Revision 1.5.2.2  2002/12/30 12:37:55  fedemp
 * -Wall -ansi -pedantic
 *
 * Revision 1.5.2.1  2002/09/27 13:22:27  fedemp
 * fixed constructor parameters in serialecho demo
 *
 * Revision 1.5  2002/05/08 23:18:30  dyfet
 * updated
 *
 * Revision 1.14  2002/02/17 11:30:34  freddy77
 * appling new coding style
 *
 * Revision 1.13  2002/02/17 09:26:09  freddy77
 * appling new coding style
 *
 * Revision 1.12  2002/02/12 18:04:01  dyfet
 * master include under common.h now
 *
 * Revision 1.11  2001/10/29 13:09:38  dyfet
 * fixed for current release
 *
 * Revision 1.10  2001/10/12 15:41:13  freddy77
 * Removed posix semantic
 *
 * Revision 1.9  2001/08/01 18:27:59  dyfet
 * new sample code
 *
 * Revision 1.3  2001/06/22 17:27:11  dyfet
 * new std namespace streaming and stuff
 *
 * Revision 1.2  2001/02/15 15:28:22  dyfet
 * cmdline options
 *
 * Revision 1.5  2001/02/14 05:31:44  gianni
 *
 * Add CommandOption/CommandOptionParse classes and example code.  This allows
 * for easier command line parsing.  This implementation is a wrapper around
 * getopt_long.  It takes the difficulty out of adding new options and maintaing
 * usage comments.  It also "objectizes" command line options so they are more
 * independant.
 *
 * Also fixed some compile errors in demo code.
 *
 * Revision 1.4  2000/12/20 18:12:55  dyfet
 * merged code
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
 * @version: $Id: serialecho.cpp,v 1.1 2003/05/31 14:28:33 brandonb Exp $
 ********************************************************************
*/
// Copyright (C) 1999-2000 Teledynamics Communications Inc.
//  
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version.
// 
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
// 
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software 
// Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.

#include "serialecho.h"

using namespace std;

SerialEcho::SerialEcho(const char *device, 
                       int priority, int stacksize) :
  TTYSession( device, priority, stacksize ) {

  cout << "Creating SerialEcho" << endl;

  if (!(*this)) {
    throw xError();
    ::exit(1);
  } else {
    cout << "modem ready" << endl;
  }

  interactive(false);

  if (setSpeed(38400)) cout << getErrorString() << endl;
  if (setCharBits(8)) cout << getErrorString() << endl;
  if (setParity(Serial::parityNone)) cout << getErrorString() << endl;
  if (setStopBits(1)) cout << getErrorString() << endl;
  if (setFlowControl(Serial::flowHard)) cout << getErrorString() << endl;

  cout << "config done" << endl;
}

void SerialEcho::run() {
  char* s = new char[getBufferSize()];

  cout << "start monitor" << endl;

  while (s[0] != 'X') {
    while (isPending(Serial::pendingInput)) {
      cout.put( get() );
    } 
    sleep(500);
  }

  cout << "end of monitor" << endl;

  delete [] s;

  exit();
}

