#include "Message.h"

#include <stdlib.h>
#include <iostream>


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
  cout << "Message) Type: " << type
       << ", SenderID: " << senderID
       << ", receiverID: " << receiverID
       << ", sentTime: " << sentTime
       << ", receiveTime: " << receiveTime
       << endl;

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

