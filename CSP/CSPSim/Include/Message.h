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
 * @file Message.h
 *
 **/

#ifndef __MESSAGE_H__
#define __MESSAGE_H__


#define COLLISION_MESSAGE 1
#define SET_ALTITUDE_MESSAGE 2
#define LOOK_FOR_ENEMIES_MESSAGE 10
#define WEAPON_READY_MESSAGE 11

#define CHECK_GAME_STATUS_MESSAGE 100
#define GENERATE_NEW_ENEMY_PLANES_MESSAGE 101



/**
 * class Message
 *
 * @author unknown
 */
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


/**
 * class messageComparison
 *
 * @author unknown
 */
struct messageComparison {
   bool operator () (Message * left, Message * right) const
      { return left->receiveTime > right->receiveTime; }
};

#endif // __MESSAGE_H__

