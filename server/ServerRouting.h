// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2000,2001 Alistair Riddoch

#ifndef SERVER_SERVER_ROUTING_H
#define SERVER_SERVER_ROUTING_H

#include "Account.h"

#include "common/OOGThing.h"
#include "common/serialno.h"

class BaseWorld;
class Lobby;

class ServerRouting : public OOGThing {
  private:
    BaseDict objects;
    AccountDict accounts;
    const std::string svrRuleset;
    const std::string svrName;
    int numClients;
  public:
    BaseWorld & world;
    Lobby & lobby;

    ServerRouting(BaseWorld & wrld,
                  const std::string & ruleset,
                  const std::string & name);
    ~ServerRouting();

    int idle();

    void addObject(BaseEntity * obj) {
        objects[obj->getId()] = obj;
    }

    void addAccount(Account * a) {
        accounts[a->username] = a;
        addObject(a);
    }

    int getSerialNo() {
        return opSerialNo();
    }

    const BaseDict & getObjects() const {
        return objects;
    }

    BaseEntity * getObject(const std::string & id) const {
        BaseDict::const_iterator I = objects.find(id);
        if (I == objects.end()) {
            return 0;
        } else {
            return I->second;
        }
    }

    Account * getAccountByName(const std::string & username) const {
        AccountDict::const_iterator I = accounts.find(username);
        if (I == accounts.end()) {
            return 0;
        } else {
            return I->second;
        }
    }

    void incClients() { ++numClients; }
    void decClients() { --numClients; }

    BaseWorld & getWorld() { return world; }

    const std::string & getName() const { return svrName; }

    virtual void addToObject(Atlas::Message::Object::MapType &) const;
};

#endif // SERVER_SERVER_ROUTING_H
