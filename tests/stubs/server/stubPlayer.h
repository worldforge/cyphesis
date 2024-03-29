// AUTOGENERATED file, created by the tool generate_stub.py, don't edit!
// If you want to add your own functionality, instead edit the stubPlayer_custom.h file.

#ifndef STUB_SERVER_PLAYER_H
#define STUB_SERVER_PLAYER_H

#include "server/Player.h"
#include "stubPlayer_custom.h"

#ifndef STUB_Player_Player
//#define STUB_Player_Player
   Player::Player(Connection * conn, const std::string & username, const std::string & passwd, RouterId id)
    : Account(conn, username, passwd, id)
  {
    
  }
#endif //STUB_Player_Player

#ifndef STUB_Player_Player_DTOR
//#define STUB_Player_Player_DTOR
   Player::~Player()
  {
    
  }
#endif //STUB_Player_Player_DTOR

#ifndef STUB_Player_getType
//#define STUB_Player_getType
  const char* Player::getType() const
  {
    return nullptr;
  }
#endif //STUB_Player_getType


#endif