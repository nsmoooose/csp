/** \file ftp.c

  Copyright (C) 2001 Dr. Eckhardt + Partner GmbH
    
  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 2 of the License, or
  (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program; if not, write to the Free Software 
  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.


  FTP-client for c++

  This class represents a FTP-client to be used in c++-programms.
  It is based on the CommonC++-Framework.
*/

#include <cc++/config.h>

#ifdef	COMMON_FTP_SUPPORT

#include <cc++/export.h>
#include <cc++/ftp.h>
#include <assert.h>
#include <ctype.h>
#include <stdio.h>
#include <stdexcept>
#include <sstream>
#include <fstream>
#include <algorithm>

#ifdef	CCXX_NAMESPACES
namespace ost {
using std::string;
using std::istringstream;
using std::ostringstream;
using std::ifstream;
using std::ofstream;
using std::pair;
using std::endl;
using std::ends;
using std::cerr;
using std::flush;
#endif

static const tpport_t PORT_FTP = 21;
static const tpport_t PORT_FTPDATA = 20;

static const int ST_START = 0;
static const int ST_REPL1 = 1;
static const int ST_REPL2 = 2;
static const int ST_REPL3 = 3;
static const int ST_SLINE = 4;
static const int ST_MLINE = 5;
static const int ST_MREPL = 6;
static const int ST_MREP1 = 7;
static const int ST_MREP2 = 8;
static const int ST_MREP3 = 9;
static const int ST_MREPE = 10;
static const int ST_OK    = 99;

static pair<string, tpport_t> parse_ip_port(string l);

int FTPSocket::debug = 0;

FTPSocket::FTPSocket() : cs(0)
{
}

FTPSocket::FTPSocket(InetHostAddress host, timeout_t to) : cs(0)
{
	open(host, to);
}

FTPSocket::FTPSocket(InetHostAddress host, string user, string passwd, timeout_t to) : cs(0)
{
  open(host, to);
  login(user, passwd);
}

FTPSocket::~FTPSocket()
{
  close();
}

void FTPSocket::sendCommand(string cmd)
{
  assert(cs != 0);

  if (debug)
    cerr << "net::ftp: send_cmd " << cmd << endl;

  (*cs) << cmd << '\r' << endl;
}

string FTPSocket::command(string cmd)
{
  string resp;
  sendCommand(cmd);
  getResponse(&resp);
  return resp;
}

int FTPSocket::getResponse(string* presp)
{
  int state = ST_START;
  bool end = false;
  char ret[3];
  string resp;

  while (!end)
  {
    int ch = (*cs).get();

    resp.append(1, (char)ch);

    switch(state)
    {
      case ST_START:
        if (isdigit(ch))
        {
          ret[0] = (char)ch;
          state = ST_REPL1;
        }
        else
          end = true;
        break;

      case ST_REPL1:
        if (isdigit(ch))
        {
          ret[1] = (char)ch;
          state = ST_REPL2;
        }
        else
          end = true;
        break;

      case ST_REPL2:
        if (isdigit(ch))
        {
          ret[2] = (char)ch;
          state = ST_REPL3;
        }
        else
          end = true;
        break;

      case ST_REPL3:
        if (ch == ' ')
          state = ST_SLINE;
        else if (ch == '-')
          state = ST_MLINE;
        else
          end = true;
        break;

      case ST_SLINE:
        if (ch == '\n')
        {
          state = ST_OK;
          end = true;
        }
        break;

      case ST_MLINE:
        if (ch == '\n')
          state = ST_MREPL;
        break;

      case ST_MREPL:
        if (ch == ret[0])
          state = ST_MREP1;
        else if (ch == '\n')
        { }
        else
          state = ST_MLINE;
        break;

      case ST_MREP1:
        if (ch == ret[1])
          state = ST_MREP2;
        else if (ch == '\n')
          state = ST_MREP1;
        else
          state = ST_MLINE;
        break;

      case ST_MREP2:
        if (ch == ret[2])
          state = ST_MREP3;
        else if (ch == '\n')
          state = ST_MREP1;
        else
          state = ST_MLINE;
        break;

      case ST_MREP3:
        if (ch == ' ')
          state = ST_MREPE;
        else if (ch == '\n')
          state = ST_MREP1;
        else
          state = ST_MLINE;
        break;

      case ST_MREPE:
        if (ch == '\n')
        {
          state = ST_OK;
          end = true;
        }
        break;

    } // switch (state)
  }

  if (debug)
    cerr << "net::ftp: resp ready - state " << state << ':' << endl
         << resp << ends;

  if (state != ST_OK)
  {
    if(getException() == Thread::throwObject)
	throw(this);
#ifdef	COMMON_STD_EXCEPTION
    else if(getException() == Thread::throwException)
    {
    	ostringstream buffer;
    	buffer << "ftp error - state " << state;
    	throw FTPException(buffer.str());
    }
#endif
    return 500;
  }

  if (presp)
    presp->assign(resp);

  int code = (ret[0] - '0') * 100
           + (ret[1] - '0') * 10
           + (ret[2] - '0');

  if(code >= 400)
  {
	if(getException() == Thread::throwObject)
		throw(this);
#ifdef	COMMON_STD_EXCEPTION
        else if(getException() == Thread::throwException)
    		throw FTPResponse(code, resp);
#endif
  }
  return code;
}

void FTPSocket::setAscii()
{
  ostringstream cmd;
  cmd << "TYPE A";
  sendCommand(cmd.str());
  getResponse();
}

void FTPSocket::setEbcdic()
{
  ostringstream cmd;
  cmd << "TYPE E";
  sendCommand(cmd.str());
  getResponse();
}

void FTPSocket::setBinary()
{
  ostringstream cmd;
  cmd << "TYPE I";
  sendCommand(cmd.str());
  getResponse();
}


void FTPSocket::open(InetHostAddress host, timeout_t to)
{
  delete cs;
  cs = 0;
  cs = new TCPStream(host, PORT_FTP, 512, false, to);
  if (cs->getErrorNumber() != 0)
  {
    ostringstream msg;
    msg << "Socket error " << cs->getErrorNumber();
    const char* errstr = cs->getErrorString();
    if (errstr)
    {
      if(getException() == Thread::throwObject)
	throw(this);
#ifdef	COMMON_STD_EXCEPTION
      else if(getException() == Thread::throwException)
      {
      	msg << '\n' << errstr;
    	throw FTPException( msg.str() );
      }
#endif
      return;
    }
  }
  getResponse();
}

void FTPSocket::open(InetHostAddress host, string user, string passwd, timeout_t to)
{
  open(host, to);
  login(user, passwd);
}

void FTPSocket::login(string user, string passwd)
{
  ostringstream U;
  U << "USER " << user;
  sendCommand(U.str());
  getResponse();

  ostringstream P;
  P << "PASS " << passwd;
  sendCommand(P.str());
  getResponse();
}

void FTPSocket::close()
{
  delete cs;
  cs = 0;
}

void FTPSocket::quit()
{
  if (*cs)
  {
    sendCommand("QUIT");
    getResponse();
  }
  close();
}

string FTPSocket::pwd(void)
{
  string ret;
  sendCommand("PWD");
  getResponse(&ret);

  // Antwort kommt in der Form '257 "/pub/blah" is current directory.'
  string::size_type s = ret.find('"');
  if ( s == string::npos )
  {
    if(getException() == Thread::throwObject)
	throw this;
#ifdef	COMMON_STD_EXCEPTION
    else if(getException() == Thread::throwException)
	    throw FTPException("invalid response from PWD");
#endif
    return ret;
  }
  ++s;

  string::size_type e = ret.find('"', s);
  if ( e == string::npos )
  {
    if(getException() == Thread::throwObject)
	throw this;
#ifdef	COMMON_STD_EXCEPTION
    else if(getException() == Thread::throwException)
	    throw FTPException("invalid response from PWD");
#endif
    return ret;
  }
  return ret.substr(s, e-s);
}

void FTPSocket::cwd(string dir)
{
  ostringstream cmd;
  cmd << "CWD " << dir;
  sendCommand(cmd.str());
  getResponse();
}

void FTPSocket::cdup()
{
  sendCommand("CDUP");
  getResponse();
}

FTPSocket::DirType FTPSocket::getDir(string dir, timeout_t to)
{
  DirType ret;
  string resp;

  sendCommand("PASV");
  getResponse(&resp);
  pair<string, tpport_t> p = parse_ip_port(resp);

  {
    TCPStream data(InetHostAddress(p.first.c_str()), p.second, 512, true, to);

    ostringstream cmd;
    cmd << "LIST";
    if (!dir.empty())
      cmd << ' ' << dir;
    sendCommand(cmd.str());
    getResponse();

    int ch;
    string line;
    while ( (ch = data.get()) != EOF )
    {
      if (ch == '\n')
      {
        DirEntry e;
        if (e.parse(line))
          ret.insert(e);
        line = "";
      }
      else
        line.append(1, (char)ch);
    }
  }
  getResponse();

  return ret;
}

void FTPSocket::put(string file, string rfilename, timeout_t to)
{
  string resp;

  sendCommand("PASV");
  getResponse(&resp);
  pair<string, tpport_t> p = parse_ip_port(resp);

  {
    TCPStream data(InetHostAddress(p.first.c_str()), p.second, 512, true, to);

    ostringstream stor;
    stor << "STOR " << rfilename;
    sendCommand(stor.str());
    getResponse();

    ifstream f( file.c_str() );

    int ch;
    while( (ch = f.get()), !f.eof() )
      data << (char)ch;
    data << flush;
  }

  getResponse();
}

void FTPSocket::get(string file, string lfilename, timeout_t to)
{
  string resp;

  sendCommand("PASV");
  getResponse(&resp);
  pair<string, tpport_t> p = parse_ip_port(resp);

  {
    TCPStream data(InetHostAddress(p.first.c_str()), p.second, to);

    ostringstream retr;
    retr << "RETR " << file;
    sendCommand(retr.str());
    getResponse();

    ofstream f( lfilename.c_str(), 
		std::ios::out | std::ios::trunc | std::ios::binary );

    int ch;
    while ( (ch = data.get()) != EOF )
      f << (char)ch;
  }
  getResponse();
}

void FTPSocket::remove(string file)
{
  ostringstream cmd;
  cmd << "DELE " << file;
  sendCommand(cmd.str());
  getResponse();
}

void FTPSocket::rename(string from, string to)
{
  ostringstream cmd;
  cmd << "RNFR " << from;
  sendCommand(cmd.str());
  getResponse();

  cmd.seekp(0);
  cmd << "RNTO " << to;
  sendCommand(cmd.str());
  getResponse();
}

void FTPSocket::mkdir(string dir)
{
  ostringstream cmd;
  cmd << "MKD " << dir;
  sendCommand(cmd.str());
  getResponse();
}

void FTPSocket::rmdir(string dir)
{
  ostringstream cmd;
  cmd << "RMD " << dir;
  sendCommand(cmd.str());
  getResponse();
}

////////////////////////////////////////////////////////////////////////
//
// stream-Funktionen
//
//
oftpstream::oftpstream(FTPSocket& f, string filename, int buffer,
        timeout_t to)
  : ftpref(f)
{
  setTimeout(to);

  string resp;
  ftpref.sendCommand("PASV");
  ftpref.getResponse(&resp);
  pair<string, tpport_t> p = parse_ip_port(resp);

  ostringstream addr;
  addr << p.first << ':' << p.second;
  open(addr.str().c_str(), buffer);

  ostringstream cmd;
  cmd << "STOR " << filename;
  ftpref.sendCommand(cmd.str());
  ftpref.getResponse();
}

void oftpstream::close()
{
  if (isConnected())
  {
    tcpstream::close();
    ftpref.getResponse();
  }
}

iftpstream::iftpstream(FTPSocket& f, string filename, int buffer,
        timeout_t to)
  : ftpref(f)
{
  setTimeout(to);

  // set up data-connection
  string resp;
  ftpref.sendCommand("PASV");
  ftpref.getResponse(&resp);
  pair<string, tpport_t> p = parse_ip_port(resp);

  ostringstream addr;
  addr << p.first << ':' << p.second;
  open(addr.str().c_str(), buffer);

  ostringstream cmd;
  cmd << "RETR " << filename;
  ftpref.sendCommand(cmd.str());
  ftpref.getResponse();
}

void iftpstream::close()
{
  if (isConnected())
  {
    tcpstream::close();
    ftpref.getResponse();
  }
}

/**
 * searches for (p,p,p,p,p,p) in a string
 */
pair<string, tpport_t> parse_ip_port(string l)
{
  istringstream str(l);

  // read until '('
  while(str.get() != '(')
    if (str.eof())
    {
#ifdef	COMMON_STD_EXCEPTION
      if(getException() == Thread::throwException)
	      throw FTPException("invalid response");
#endif
    }
  // extract 6 ints; 4 for IP and 2 for port
  char ch;
  int ip[4], p[2];
  str >> ip[0] >> ch >> ip[1] >> ch >> ip[2] >> ch >> ip[3] >> ch
      >> p[0] >> ch >> p[1] >> ch;
  if ( str.eof() || ch != ')' )
  {
#ifdef	COMMON_STD_EXCEPTION
    if(getException() == Thread::throwException)
	    throw FTPException("invalid response");
#endif
  }
  // create IP-number
  ostringstream IP;
  IP << ip[0] << '.' << ip[1] << '.' << ip[2] << '.' << ip[3];

  // create port-number
  tpport_t Port = p[0] * 256 + p[1];

  return make_pair(IP.str(), Port);
}

bool FTPSocket::DirEntry::parse(string li)
{
  string junk;

  line = li;

  // remove trailing cr's and lf's
  while (!line.empty() &&
       (line[line.size() - 1] == '\r' || line[line.size() - 1] == '\n'))
    line.erase(line.size() - 1);

  if (line.empty())
    return false;

  istringstream str(line);
  str >> access >> junk >> user >> group >> size >> junk >> junk >> junk;
#ifndef __BORLANDC__
  str.get();  // skip space
#endif /* __BORLANDC__ */

  // check for error in parsing
  if (!str)
    return false;

  // we ignore device-entries for now
  if (access[0] == 'b' || access[1] == 'c')
    return false;

  // the rest should be the filename, but may contain spaces.
  int ch;
  while( (ch = str.get()) != EOF )
    name.append(1, (char)ch);

  // in case of a link there may be a " -> " there
  if (access[0] == 'l')
  {
    string::size_type s = name.find(" ->");
    if (s != string::npos)
      name.erase(s);
  }

  return true;
}

#ifdef	CCXX_NAMESPACES
};  // namespace net
#endif

#endif
