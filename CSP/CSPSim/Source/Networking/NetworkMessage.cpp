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

unsigned short NetworkMessage::m_magicNumber = 0xACDC;
unsigned short NetworkMessage::m_HeaderLen = 16;

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

/**
 * initialize()
 * 
 * This method initializes a NetworkMessage, Including allocating a buffer to
 * hold the message header and payload and also setting up the header.
 */
bool NetworkMessage::initialize( simdata::uint16  type, simdata::uint16 payloadLen, NetworkNode * orginatorNode )
{
   // return false if this has already been initialized.
   // the magic number also serves to determine if initialized.
   if (m_header.m_magicNumber == NetworkMessage::m_magicNumber)
      return false;
 
   m_header.m_magicNumber = NetworkMessage::m_magicNumber;
   m_header.m_payloadLen = payloadLen;
   m_header.m_messageType = type;
   m_header.m_ipaddr = orginatorNode->getAddress().getAddress().s_addr;
   m_header.m_port = orginatorNode->getPort();
   m_header.m_id = orginatorNode->getId();
   

   // write zeros to the payload section of the buffer.
   memset(m_payloadBuf, 0x00, m_header.m_payloadLen);
   
   return true;

}

/**
 * getBufferLen()
 *
 * Returns the number of bytes as a short that the message
 * buffer uses including the header and payload.
 */
unsigned short NetworkMessage::getPayloadLen()
{
    return m_header.m_payloadLen;
}

/**
 * getBufferPtr()
 * 
 * Returns the buffer pointer so functions can copy binary
 * data into the underlying buffer.
 */
//void * NetworkMessage::getBufferPtr()
//{
//  return m_Buf;
//}

/**
 * getPayloadPtr()
 * Returns the payload pointer so functions can copy and read
 * binary data from the undering payload section of the message.
 */
void * NetworkMessage::getPayloadPtr()
{
  return (void*)m_payloadBuf;
}

/**
 * getType()
 * Returns the message type which is stored in the message header.
 */
unsigned short NetworkMessage::getType()
{
   return m_header.m_messageType;
}

/**
 * isHeaderValid()
 * Determine if the header is valid by looking at the magic number.
 */
bool NetworkMessage::isHeaderValid()
{
//  unsigned short magicNumber = 0xFCCF;
//  unsigned short number;
//
//  memcpy((void*)&number , (void*)&m_Buf, sizeof(unsigned short));

  return (m_header.m_magicNumber == NetworkMessage::m_magicNumber);
  
}

/**
 * getHeaderLen()
 * returns the number of bytes as a short that the header occupies.
 */
//unsigned short NetworkMessage::getHeaderLen()
//{
// return m_HeaderLen;
//}


Port NetworkMessage::getOriginatorPort()
{
   return m_header.m_port;
}

NetworkNode * NetworkMessage::getOriginatorNode()
{
  return new NetworkNode(m_header.m_id, m_header.m_ipaddr, m_header.m_port);
}
