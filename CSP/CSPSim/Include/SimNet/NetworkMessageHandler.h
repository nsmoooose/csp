#ifndef __NETWORK_MESSAGE_HANDLER_H__ 
#define __NETWORK_MESSAGE_HANDLER_H__

class NetworkMessage;

class NetworkMessageHandler
{
	public:
	virtual void process(NetworkMessage * message) = 0;
};

#endif


