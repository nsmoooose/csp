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
 * @file NetworkMessage.cpp:
 * @author Scott Flicker (Wolverine)
 *
 * Base class for Network Messages.
 * Currently contains for header and buffer routines
 */

#include "Networking.h"

unsigned short NetworkMessage::magicNumber = 0xFCCF;
unsigned short NetworkMessage::m_HeaderLen = 6;

/**
 * NetworkMessage()
 *
 * create an empty NetworkMessage Object.
 * to use this class the object should be first created then initialized with
 * a specific type and payloadLen. Once it has been initialized it can then be
 * loaded with message specific data by first uptaining the payload ptr then setting
 * bytes within it to the proper values. Note also once a message object has
 * been initialized it should be free to be used multiple times.
 */
NetworkMessage::NetworkMessage()
{
   m_Buf = NULL;
   m_PayloadBuf = NULL;
   m_BufferLen = 0;
   m_PayloadLen = 0;
   m_MessageType = 0;
   m_Initialized = false;
  
}


/**
 * ~NetworkMessage
 * 
 * Deletes a network message including removing the memory allocated for the buffer.
 */
NetworkMessage::~NetworkMessage()
{
  if (m_Initialized)
    delete [] m_Buf;
}

/**
 * initialize()
 * 
 * This method initializes a NetworkMessage, Including allocating a buffer to
 * hold the message header and payload and also setting up the header.
 */
bool NetworkMessage::initialize( unsigned short type, unsigned short payloadLen)
{
   // return false if this has already been initialized.
   if (m_Initialized)
      return false;
      
   m_PayloadLen = payloadLen;
   m_MessageType = type;
   
   m_BufferLen = m_HeaderLen + m_PayloadLen;
   
   m_Buf = new unsigned char[m_BufferLen];
   m_PayloadBuf = m_Buf + m_HeaderLen;

   // write the header;
   unsigned char * ptrBuf = m_Buf;
   memcpy((void*)ptrBuf    , (void*)&magicNumber, sizeof(unsigned short));
   ptrBuf += sizeof(unsigned short);
   memcpy((void*)ptrBuf    , (void*)&m_BufferLen, sizeof(unsigned short));
   ptrBuf += sizeof(unsigned short);
   memcpy((void*)ptrBuf    , (void*)&m_MessageType  , sizeof(unsigned short) );
   ptrBuf += sizeof(unsigned short);

   // write zeros to the payload section of the buffer.
   memset(m_PayloadBuf, 0x00, m_PayloadLen);
                                                                                                                              
   m_Initialized = true;
   return true;

}

/**
 * getBufferLen()
 *
 * Returns the number of bytes as a short that the message
 * buffer uses including the header and payload.
 */
unsigned short NetworkMessage::getBufferLen()
{
    return m_BufferLen;
}

/**
 * getBufferPtr()
 * 
 * Returns the buffer pointer so functions can copy binary
 * data into the underlying buffer.
 */
void * NetworkMessage::getBufferPtr()
{
  return m_Buf;
}

/**
 * getPayloadPtr()
 * Returns the payload pointer so functions can copy and read
 * binary data from the undering payload section of the message.
 */
void * NetworkMessage::getPayloadPtr()
{
  return (void*)m_PayloadBuf;
}

/**
 * getType()
 * Returns the message type which is stored in the message header.
 */
unsigned short NetworkMessage::getType()
{
   unsigned int type;
   memcpy((void*)&type , (void*)(m_Buf + 2), sizeof(unsigned short));
   return type;

}

/**
 * isHeaderValid()
 * Determine if the header is valid by looking at the magic number.
 */
bool NetworkMessage::isHeaderValid()
{
  unsigned short magicNumber = 0xFCCF;
  unsigned short number;

  memcpy((void*)&number , (void*)&m_Buf, sizeof(unsigned short));

  return (number == magicNumber);
  
}

/**
 * getHeaderLen()
 * returns the number of bytes as a short that the header occupies.
 */
unsigned short NetworkMessage::getHeaderLen()
{
  return m_HeaderLen;
}

/**
 * getPayloadLen()
 * returns the number of bytes as a short that the payload occupies.
 */
unsigned short NetworkMessage::getPayloadLen()
{
  return m_PayloadLen;
}
