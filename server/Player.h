// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2000,2001 Alistair Riddoch

#ifndef SERVER_PLAYER_H
#define SERVER_PLAYER_H

#include "Account.h"

class Player : public Account {
  protected:
    virtual OpVector characterError(const Create & op,
                            const Atlas::Message::Object::MapType & ent) const;
  public:
    Player(Connection * conn, const std::string & username, const std::string & passwd);
    virtual ~Player();
};

#endif // SERVER_PLAYER_H
