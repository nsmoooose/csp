#ifndef __MESSAGE_H__
#define __MESSAGE_H__


#define COLLISION_MESSAGE 1
#define SET_ALTITUDE_MESSAGE 2
#define LOOK_FOR_ENEMIES_MESSAGE 10
#define WEAPON_READY_MESSAGE 11

#define CHECK_GAME_STATUS_MESSAGE 100
#define GENERATE_NEW_ENEMY_PLANES_MESSAGE 101


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
  float sentTime;
  float receiveTime;
  int data;
};

struct messageComparison {
   bool operator () (Message * left, Message * right) const
      { return left->receiveTime > right->receiveTime; }
};

#endif
