// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2000,2001 Alistair Riddoch

#ifndef SERVER_SERVER_ROUTING_H
#define SERVER_SERVER_ROUTING_H

#include <common/OOGThing.h>

#include <common/serialno.h>

class WorldRouter;
class Lobby;
class CommServer;

class ServerRouting : public OOGThing {
  private:
    BaseDict objects;
    CommServer & commServer;
    const std::string svrRuleset;
    const std::string svrName;
    WorldRouter & world;
  public:
    Lobby & lobby;

    ServerRouting(CommServer & server, const std::string & ruleset, const std::string & name);
    ~ServerRouting();

    inline int idle();
    inline BaseEntity * addObject(BaseEntity * obj);

    inline int getSerialNo() {
        return opSerialNo();
    }

    const BaseDict & getObjects() const {
        return objects;
    }

    BaseEntity * getObject(const std::string & fid) const {
        BaseDict::const_iterator I = objects.find(fid);
        if (I == objects.end()) {
            return NULL;
        } else {
            return I->second;
        }
    }

    WorldRouter & getWorld() { return world; }

    virtual void addToObject(Atlas::Message::Object::MapType &) const;
};

#endif // SERVER_SERVER_ROUTING_H
