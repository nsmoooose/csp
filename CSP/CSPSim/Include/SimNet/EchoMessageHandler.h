#ifndef __ECHO_MESSAGE_HANDLER_H__
#define __ECHO_MESSAGE_HANDLER_H__

#include <SimNet/NetworkMessageHandler.h>

class EchoMessageHandler : public NetworkMessageHandler
{
        protected:
		NetworkMessenger * m_messenger;
	public: 
		EchoMessageHandler();
		virtual void process(NetworkMessage * message);
		virtual ~EchoMessageHandler();

		
		virtual void setMessenger(NetworkMessenger * messenger)
		             { m_messenger = messenger; }
		virtual NetworkMessenger * getMessenger()
			     { return m_messenger; }
	
};

#endif

