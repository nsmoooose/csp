#ifndef __REDIRECT_MESSAGE_HANDLER_H__
#define __REDIRECT_MESSAGE_HANDLER_H__

#include <SimNet/NetworkMessageHandler.h>

class RedirectMessageHandler : public NetworkMessageHandler
{
        protected:
		NetworkMessenger * m_messenger;
		std::set<NetworkNode *> m_ClientNodeList;
	public: 
		RedirectMessageHandler();
		virtual void process(NetworkMessage * message);
		virtual ~RedirectMessageHandler();

		
		virtual void setMessenger(NetworkMessenger * messenger)
		             { m_messenger = messenger; }
		virtual NetworkMessenger * getMessenger()
			     { return m_messenger; }
	
};

#endif

