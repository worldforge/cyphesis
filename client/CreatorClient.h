// This file may be redistributed and modified only under the terms of
// the GNU Lesser General Public License (See COPYING for details).
// Copyright (C) 2001 Alistair Riddoch

#ifndef CREATOR_CLIENT_H
#define CREATOR_CLIENT_H

#include "CharacterClient.h"

class Entity;

class CreatorClient : public CharacterClient {
  public:
    CreatorClient();

    Entity * make(const Atlas::Message::Object&);
};

#endif // CREATOR_CLIENT_H
