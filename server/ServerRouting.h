// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2000,2001 Alistair Riddoch

#ifndef SERVER_ROUTING_H
#define SERVER_ROUTING_H

#include "Routing.h"

class WorldRouter;
class Lobby;
class CommServer;

class ServerRouting : public Routing {
  private:
    CommServer & commServer;
    const string svrName;
  public:
    dict_t idDict;
  private:
    WorldRouter & world;
  public:
    Lobby & lobby;

    ServerRouting(CommServer & server, const string & name);
    ~ServerRouting();

    inline int idle();
    inline BaseEntity * addObject(BaseEntity * obj);

    WorldRouter & getWorld() { return world; }

    virtual void addToObject(Atlas::Message::Object &) const;
};

#endif // SERVER_ROUTING_H
