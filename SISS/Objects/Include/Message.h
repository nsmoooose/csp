#ifndef __MESSAGE_H__
#define __MESSAGE_H__






class Message
{

 public:

  Message();
  virtual ~Message();
  Message(const Message &);
  Message & operator=(const Message &);

  bool operator<(const Message & rhs) const;
  bool operator>(const Message & rhs) const;
  bool operator==(const Message &rhs) const;

  void dump();

  unsigned int type;
  unsigned int senderID;
  unsigned int receiverID;
  unsigned int sentTime;
  unsigned int receiveTime;
  int data;
};

struct messageComparison {
   bool operator () (Message * left, Message * right) const
      { return left->receiveTime > right->receiveTime; }
};

#endif
