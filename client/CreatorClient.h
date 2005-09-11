// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2001 Alistair Riddoch

#ifndef CLIENT_CREATOR_CLIENT_H
#define CLIENT_CREATOR_CLIENT_H

#include "CharacterClient.h"

class Entity;

/// \brief Class to implement a creator entity in an admin client
class CreatorClient : public CharacterClient {
  private:
    Entity * sendLook(const Operation & op);
  public:
    CreatorClient(const std::string&, const std::string&, ClientConnection&);

    Entity * make(const Atlas::Objects::Entity::RootEntity &);
    void sendSet(const std::string &, const Atlas::Objects::Entity::RootEntity &);
    Entity * look(const std::string &);
    Entity * lookFor(const Atlas::Objects::Entity::RootEntity &);
    int runScript(const std::string & package, const std::string & function);
};

#endif // CLIENT_CREATOR_CLIENT_H
