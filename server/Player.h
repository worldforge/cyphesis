// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2000,2001 Alistair Riddoch

#ifndef PLAYER_H
#define PLAYER_H

#include "Account.h"

class Player : public Account {
  protected:
    virtual oplist characterError(const Create & op, const Atlas::Message::Object::MapType & ent) const;
  public:
    Player(Connection * conn, const string & username, const string & passwd);
    virtual ~Player();
};

#endif // PLAYER_H
