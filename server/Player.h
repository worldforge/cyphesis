#ifndef PLAYER_H
#define PLAYER_H

typedef int bad_type; // Remove this to get unset type reporting

#define None 0 // Remove this to deal with un-initialied vars

#include "Account.h"

class Player : public Account {
  public:
    Player(Connection * conn, const string & username, const string & passwd) :
        Account(conn, username, passwd) {
        type = "player";
    }
    virtual ~Player() { }

  protected:
    virtual oplist character_error(const Create & op, const Message::Object & ent);
};

#endif /* PLAYER_H */
