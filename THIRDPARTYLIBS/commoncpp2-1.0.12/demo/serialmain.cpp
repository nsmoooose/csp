/**********************************************************************
 * C/C++ Source: main.cc
 *
 * Test harness for the serialecho class
 *
 * Created: Sat Oct 14 12:34:39 EDT 2000
 * $Log: serialmain.cpp,v $
 * Revision 1.1  2003/05/31 14:28:33  brandonb
 * no message
 *
 * Revision 1.5.2.1  2002/12/30 12:37:55  fedemp
 * -Wall -ansi -pedantic
 *
 * Revision 1.5  2002/05/08 23:18:30  dyfet
 * updated
 *
 * Revision 1.10  2002/02/17 09:26:09  freddy77
 * appling new coding style
 *
 * Revision 1.9  2001/08/01 18:27:59  dyfet
 * new sample code
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
 * @author:  Gary Lawrence Murphy <garym@teledyn.com>
 * Copyright:  2000 TeleDynamics Communications Inc (www.teledyn.com)
 * @version: $Id: serialmain.cpp,v 1.1 2003/05/31 14:28:33 brandonb Exp $
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
//
// Created 2000/10/14 10:56:35 EDT by garym@teledyn.com

#include "serialecho.h"

int main(int argc, char **argv)
{
  cout << "Serial Echo to TCP Sessions" << endl;
  SerialEcho *modem;
  try {
    modem = new SerialEcho("/dev/modem2");
  } catch (SerialEcho::xError *e) {
    cout << "Modem Error; aborting" << endl;
    ::exit(1);
  } catch (Serial *e) {
    cout << "Serial Error: " 
         << modem->getErrorString() 
         << "; aborting" 
         << endl;
    ::exit(1);
  }

  char* b = new char[modem->getBufferSize()];

  cout << "Modem code:" << modem->start() << endl;

  while (cin >> b, b[0]) {

    *modem << b << "\r" << endl;

    cout << "sent: " << b << endl;
    memset( b, 0, sizeof(b));

  }
  cout << "fin" << endl;

  delete [] b;

  return 0;
}

/**  2000 by TeleDynamics Communications Inc - teledynamics@canada.com*/

