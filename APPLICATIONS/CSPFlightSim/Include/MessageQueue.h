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
 * @file MessageQueue.h
 *
 **/

#ifndef __MESSAGEQUEUE_H__
#define __MESSAGEQUEUE_H__

#include <queue>

#include "Message.h"

using namespace std;

/**
 * class MessageQueue
 *
 * @author unknown
 */
class MessageQueue
{

public:
    MessageQueue();

    void sendMsg(int type, unsigned int senderID, unsigned int receiverID, int data);
    void sendDelayedMsg(int type, unsigned int senderID, unsigned int receiverID, unsigned int delay, int data);
    void routeMessage( Message* msg);
    void storeDelayedMsg( Message * msg);
    void MessageQueue::processDelayedMsgList();

protected:

    priority_queue<Message *, vector<Message*>, messageComparison > delayedMessageList;

};

#endif // __MESSAGEQUEUE_H__

