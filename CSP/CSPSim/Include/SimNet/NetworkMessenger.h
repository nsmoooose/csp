#ifndef __NETWORK_MESSENGER_H__
#define __NETWORK_MESSENGER_H__

class RouteMessage;
class NetworkNode;
class NetworkMessage;

class NetworkMessenger
{
   private: 
//    MessageSocketDuplex * m_messageSocketDuplex;
    std::vector<RoutedMessage> m_messageSendArray;
    std::vector<RoutedMessage> m_messageReceiveArray;
    int m_messageSendArrayMax;
    int m_messageSendArrayCount;
    int m_messageSendArrayGrow;
    int m_messageReceiveArrayMax;
    int m_messageReceiveArrayCount;
    int m_messageReceiveArrayGrow;
    NetworkNode * m_originatorNode;
    std::list<NetworkMessage*> m_messagePool;
    std::list<NetworkMessageHandler *> m_MessageHandlerList;
      
    ost::UDPSocket * m_UDPReceiverSocket;
    ost::UDPSocket * m_UDPSenderSocket;	
   
    ost::InetAddress * m_receiverAddr;
    Port m_receiverPort;
   public:
      
    NetworkMessenger();
    NetworkMessenger(NetworkNode * orginatorNode);
//    NetworkMessenger(ost::InetAddress & addr, Port port);

    void queueMessage(NetworkNode * remoteNode, NetworkMessage * message);
    void sendQueuedMessages();
    void receiveMessages();
    void receiveMessages(int max);
    int getSendQueueCount() { return m_messageSendArrayCount; }

    NetworkNode * getOriginatorNode();
    void setOriginatorNode(NetworkNode * orginatorNode);

    NetworkMessage * allocMessageBuffer(int type, int payloadLen);
    NetworkMessage * allocMessageBuffer();
    void freeMessageBuffer(NetworkMessage * message);
    
    void registerMessageHandler(NetworkMessageHandler * handler);
    
    int sendto(NetworkMessage * message, ost::InetHostAddress * remoteAddress, Port * remotePort);   
    int sendto(NetworkMessage * message, NetworkNode * node);   
    
    int recvfrom(NetworkMessage ** message);

    int sendto(std::vector<RoutedMessage> * sendArray, int count);
    int recvfrom(std::vector<RoutedMessage> * receiveArray, int * count);

    NetworkMessage * receiveMessage();
    
    ost::InetAddress * getReceiverAddress() { return m_receiverAddr; }
    Port getReceiverPort() { return m_receiverPort; }

};

#endif

