// Copyright (C) 2001 Dr. Eckhardt + Partner GmbH
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

/**
 * @file ftp.h
 * @short File Transfer Protocol sockets and streams.
 **/

#ifndef	CCXX_FTP_H_
#define	CCXX_FTP_H_

#ifndef CCXX_CONFIG_H_
#include <cc++/config.h>
#endif

#ifndef	CCXX_SOCKET_H_
#include <cc++/socket.h>
#endif

#include <string>
#include <set>
#include <iostream>
#include <utility>

#ifdef	CCXX_NAMESPACES
namespace ost {
using std::string;
using std::set;
#endif

/**
 * This class represents a FTP-client to be used in c++-programms.
 * It is based on the CommonC++-Framework.
 *
 * @short ftp access stream class.
 * @author Dr. Eckhardt and Partner GmbH
 */
class FTPSocket
{
protected:
      TCPStream* cs;

      friend class oftpstream;
      friend class iftpstream;
      static int debug;

public:

      /**
       low level functions
       */
      void sendCommand(string cmd);
      int getResponse(string* resp = 0);

      /**
       empty constructor
       */
      FTPSocket();

      /**
       constructor; connects to a host on creation
       */
      explicit FTPSocket(InetHostAddress host, timeout_t to = 0);

      /**
       constructor; connects to a host and logs in
       */
      FTPSocket(InetHostAddress host, string user, string passwd,
          timeout_t to = 0);

      /**
       destructor; logs out and disconnects if needed
       */
      ~FTPSocket();

      /**
       enable debugging
       */
      static void setDebug(int d = 1)  { debug = d; }

      /**
       queries debugging
       */
      static int  getDebug()           { return debug; }

      /**
       set ascii-mode for datatransfer
       */
      void setAscii();

      /**
       set ebcdic-mode for datatransfer
       */
      void setEbcdic();

      /**
       set binary-mode for datatransfer
       */
      void setBinary();

      /**
       process a ftp-command
       */
      string command(string cmd);

      /**
       direntry represents a entry in a directory
       */
      class DirEntry
      {
      protected:
          string line;
          string access;
          string user;
          string group;
          string name;
          unsigned int size;

      public:
          struct Compare
          {
            bool operator()(const DirEntry& d1, const DirEntry& d2) const
            { return d1.getName() < d2.getName(); }
          };

          /**
           set members to the values found in li
           */
          bool parse(string li);

          /**
           accessor for members
           */
          string  getLine() const    { return line; }
          string  getAccess() const  { return access; }
          string  getUser() const    { return user; }
          string  getGroup() const   { return group; }
          string  getName() const    { return name; }
          unsigned int getSize() const    { return size; }

          bool isDir() const           { return access[0] == 'd'; }
          bool isLink() const          { return access[0] == 'l'; }
          bool canUserRead() const     { return access[1] == 'r'; }
          bool canUserWrite() const    { return access[2] == 'w'; }
          bool canGroupRead() const    { return access[4] == 'r'; }
          bool canGroupWrite() const   { return access[5] == 'w'; }
          bool canOtherRead() const    { return access[7] == 'r'; }
          bool canOtherWrite() const   { return access[8] == 'w'; }
      };

      /**
       set of direntries
       */
      typedef set<DirEntry, DirEntry::Compare> DirType;

      /**
       sessionmanagement

       creates a connection to host
       */
      void open(InetHostAddress host, timeout_t to = 0);

      /**
       creates a connection to host and logs in
       */
      void open(InetHostAddress host, string user, string passwd,
                timeout_t to = 0);

      /**
       logs in to a host; connection must be set up
       */
      void login(string user, string passwd);

      /**
       log out
       */
      void close(void);

      /**
       close connection
       */
      void quit(void);

      /**
       get directory
       */
      DirType       getDir(string dir = "", timeout_t to = 0);

      /**
       get current working directory on the server
       */
      string    pwd(void);

      /**
       change current working directory on the server
       */
      void           cwd(string dir);

      /**
       change current working directory
       */
      void           cdup(void);

      /**
       send local file to remote
       */
      void put(string file, string rfilename, timeout_t to = 0);

      /**
       send local file to remote; local and remote filenames are identical
       */
      void put(string file, timeout_t to = 0)
        { put(file, file, to); }

      /**
       get file from server
       */
      void get(string file, string lfilename, timeout_t to = 0);

      /**
       get file from server; local and remote filenames are identical
       */
      void get(string file, timeout_t to = 0)
        { get(file, file, to); }

      /**
       delete file from server
       */
      void remove(string file);

      /**
       rename file on server
       */
      void rename(string from, string to);

      /**
       create directory on server
       */
      void mkdir(string dir);

      /**
       remove directory on server
       */
      void rmdir(string dir);
};

/**
 * oftpstream is a outputstream to a remote file
 */
class oftpstream : public tcpstream
{
protected:
      FTPSocket& ftpref;

public:
      /**
       constructor

       create a oftpstream to host with filename rfilename
       */
      oftpstream(FTPSocket& f, string rfilename, int buffer = 512,
              timeout_t to = 0);

      /**
       destructor; ends transmission
       */
      ~oftpstream()
      { close(); }

      /**
       ends transmission
       */
      void close();
};

/**
 * iftpstream is a inputstream from a remote file
 */
class iftpstream : public tcpstream
{
protected:
      FTPSocket& ftpref;

public:
      /**
       constructor; opens file rfilename on ftpserver
       */
      iftpstream(FTPSocket& f, string rfilename, int buffer = 512,
              timeout_t to = 0);

      /**
       destructor; ends filetransfer
       */
      ~iftpstream()
      { close(); }

      /**
       ends filetransfer
       */
      void close();
};

#ifdef	COMMON_STD_EXCEPTION
/**
 * errors are submitted with this exception
 */
class FTPException : public SockException
{
public:
	FTPException(string str) : SockException(str) {};                   
};

/**
 * error codes from ftpserver throws a ftpresponse-exception
 */
class FTPResponse : public FTPException
{
protected:
      int code;
public:
      FTPResponse(int c, string r) : FTPException(r), code(c) {};

      /**
       accessor for response-code
       */
      int getCode() const  { return code; }
};
#endif

#ifdef	CCXX_NAMESPACES
};
#endif

#endif
