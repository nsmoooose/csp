// Combat Simulator Project - FlightSim Demo
// Copyright (C) 2004 The Combat Simulator Project
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
 * @file ObjectUpdateMessage.cpp
 * @author Scott Flicker (Wolverine)
 */

#include "Networking.h"

//ObjectUpdateMessage::ObjectUpdateMessage(simdata::uint16 id, simdata::Vector3 position, simdata::Vector3 velocity)
//{
//    // reserve space to hold 6 doubles.
//    double num;
//    short magicNumber = 0xFCCF;
//    m_BufferLen = 3*sizeof(short) + sizeof(unsigned int) + 6 * sizeof(double);
//    m_Buf = new unsigned char [ m_BufferLen ];
//    m_MessageType = 2;
    
    
//   unsigned char * ptrBuf = m_Buf;
//   memcpy((void*)ptrBuf, (void*)&magicNumber,        sizeof(short)); ptrBuf += sizeof(short);
//   memcpy((void*)ptrBuf, (void*)&m_BufferLen, sizeof(short)); ptrBuf += sizeof(short);
//   memcpy((void*)ptrBuf, (void*)&m_MessageType,   sizeof(short)); ptrBuf += sizeof(short);   
//   memcpy((void*)ptrBuf, (void*)&id, sizeof(unsigned int)); ptrBuf += sizeof(unsigned int);
//   num = position.x(); memcpy((void*)ptrBuf, (void*)&num, sizeof(double)); ptrBuf += sizeof(double);   
//   num = position.y(); memcpy((void*)ptrBuf, (void*)&num, sizeof(double)); ptrBuf += sizeof(double);   
//   num = position.z(); memcpy((void*)ptrBuf, (void*)&num, sizeof(double)); ptrBuf += sizeof(double);   
//   num = velocity.z(); memcpy((void*)ptrBuf, (void*)&num, sizeof(double)); ptrBuf += sizeof(double);
//   num = velocity.y(); memcpy((void*)ptrBuf, (void*)&num, sizeof(double)); ptrBuf += sizeof(double);
//   num = velocity.z(); memcpy((void*)ptrBuf, (void*)&num, sizeof(double));
   
    
//}

