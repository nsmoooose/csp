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

#endif
