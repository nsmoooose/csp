#include "stdinc.h"

#include "MessageQueue.h"
#include "SimTime.h"
#include "BaseObject.h"
#include "VirtualBattlefield.h"

using namespace std;

MessageQueue * g_pMessageQueue = new MessageQueue;
extern SimTime * g_pSimTime;
extern VirtualBattlefield * g_pBattlefield;

MessageQueue::MessageQueue()
{

}

void MessageQueue::sendMsg(int type, unsigned int senderID, unsigned int receiverID, int data)
{
  Message * msg = new Message;
  msg->type = type;
  msg->senderID = senderID;
  msg->receiverID = receiverID;
  msg->sentTime = g_pSimTime->getSimTime();
  msg->receiveTime = g_pSimTime->getSimTime();
  msg->data = data;

  routeMessage( msg );

}

void MessageQueue::sendDelayedMsg(int type, unsigned int senderID, unsigned int receiverID, unsigned int delay, int data)
{
  CSP_LOG(CSP_APP, CSP_INFO, "Processing sendDelayedMsg, curTime " << g_pSimTime->getSimTime() << ", Message Type " << type);


  Message * msg = new Message;
  msg->type = type;
  msg->senderID = senderID;
  msg->receiverID = receiverID;
  msg->sentTime = g_pSimTime->getSimTime();
  msg->receiveTime = g_pSimTime->getSimTime() + delay;
  msg->data = data;

  routeMessage( msg );

}

void MessageQueue::routeMessage( Message* msg)
{
  CSP_LOG(CSP_APP, CSP_INFO, "routeMessage - Entering");

  BaseObject * object = g_pBattlefield->getObjectFromID( msg->receiverID );

  // check to make sure we found an object. We could have been 
  // given a bogus ID or the object could be gone.
  if (!object)
    {
      CSP_LOG(CSP_APP, CSP_INFO, "routeMessage - Message is for unknown object");
      return;
    }

  // if the message needs to be receive in the future store
  // it in the delayed message list.
  if (msg->receiveTime > g_pSimTime->getSimTime())
    {
      storeDelayedMsg(msg);
      return;
    }

  // send message to the stateMachine/controller of the object
  BaseController * controller = g_pBattlefield->getControllerFromID( object->getControllerID() );
  if (controller)
    controller->OnMessage(msg);
  else
    cerr << "routeMessage - failed to find controller for object " << object->getObjectID() << endl;

  cout << "routeMessage - Leaving" << endl;

}

void MessageQueue::storeDelayedMsg( Message * msg)
{
 CSP_LOG(CSP_APP, CSP_INFO, "Processing storeDelayedMsg, curTime " << g_pSimTime->getSimTime() << ", Message Type " << msg->type);

  msg->dump();
  delayedMessageList.push(msg);

}

// process messages in the delayed msg list.
void MessageQueue::processDelayedMsgList()
{
  // loop until we find a message with time greater then the current time.
  while (!delayedMessageList.empty())
    {
      float curTime = g_pSimTime->getSimTime();
      Message * message = delayedMessageList.top();
      if (message->receiveTime <= curTime)
        {
            CSP_LOG(CSP_APP, CSP_INFO, "Processing delayed message, curTime " << curTime << ", Message Type " << message->type);
    
	        delayedMessageList.pop();
	        message->dump();
	        routeMessage(message);
	    }
      else
          return;
    }
  
}

