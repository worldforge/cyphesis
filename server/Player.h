// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2000 Alistair Riddoch

#ifndef PLAYER_H
#define PLAYER_H

#include "Account.h"

class Player : public Account {
  public:
    Player(Connection * conn, const string & username, const string & passwd) :
        Account(conn, username, passwd) {
        type = "player";
    }
    virtual ~Player() { }

  protected:
    virtual oplist character_error(const Create & op, const Atlas::Message::Object & ent) const;
};

#endif /* PLAYER_H */
