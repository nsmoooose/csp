// Combat Simulator Project - FlightSim Demo
// Copyright (C) 2002 The Combat Simulator Project
// http://csp.sourceforge.net
// 
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either version 2
// of the License, or (at your option) any later version.
// 
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
// 
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.


/**
 * @file Message.cpp
 *
 **/


#include "Message.h"
#include "Log.h"


Message::Message()
{

}

Message::~Message()
{

}

Message::Message(const Message & message)
{
  type           = message.type;
  senderID       = message.senderID;
  receiverID     = message.receiverID;
  sentTime       = message.sentTime;
  receiveTime    = message.receiveTime;
  data           = message.data;
}

Message & Message::operator=(const Message & message)
{
  type           = message.type;
  senderID       = message.senderID;
  receiverID     = message.receiverID;
  sentTime       = message.sentTime;
  receiveTime    = message.receiveTime;
  data           = message.data;
  return *this;
}

void Message::dump()
{
  CSP_LOG(APP, INFO, "Message) Type: " << type
       << ", SenderID: " << senderID
       << ", receiverID: " << receiverID
       << ", sentTime: " << sentTime
       << ", receiveTime: " << receiveTime );;

}

bool Message::operator<(const Message & rhs) const
{
  if ( receiveTime < rhs.receiveTime)
    return true;
  return false;
   
}

bool Message::operator>(const Message &rhs) const
{
  if (receiveTime > rhs.receiveTime)
    return true;
  return false;
}

bool Message::operator==(const Message &rhs) const
{
  return (receiveTime == rhs.receiveTime);
}

