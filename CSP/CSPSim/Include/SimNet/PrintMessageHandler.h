#ifndef __PRINT_MESSAGE_HANDLER_H__
#define __PRINT_MESSAGE_HANDLER_H__

#include <SimNet/NetworkMessageHandler.h>

class PrintMessageHandler : public NetworkMessageHandler
{
	protected:
		int m_frequency;
		int m_count;
	public:
		PrintMessageHandler(); 
		virtual void process(NetworkMessage * message);
		virtual ~PrintMessageHandler();

		void setFrequency(int frequency) { m_frequency = frequency; }
		int getFrequency() { return m_frequency; }
};

#endif

