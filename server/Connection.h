// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2000,2001 Alistair Riddoch

#ifndef SERVER_CONNECTION_H
#define SERVER_CONNECTION_H

#include <common/OOGThing.h>

class ServerRouting;
class CommClient;
class Account;

class Connection : public OOGThing {
    BaseDict objects;
    CommClient & commClient;

    Account * addPlayer(const std::string &, const std::string &);
  public:
    ServerRouting & server;

    explicit Connection(CommClient & client);
    virtual ~Connection();

    BaseEntity * addObject(BaseEntity * obj) {
        objects[obj->getId()]=obj;
        return obj;
    }

    BaseEntity * getObject(const std::string & fid) const {
        BaseDict::const_iterator I = objects.find(fid);
        if (I == objects.end()) {
            return NULL;
        } else {
            return I->second;
        }
    }

    void destroy();
    inline void send(const RootOperation * msg) const;

    virtual OpVector operation(const RootOperation & op);

    virtual OpVector LoginOperation(const Login & op);
    virtual OpVector LogoutOperation(const Logout & op);
    virtual OpVector CreateOperation(const Create & op);
    virtual OpVector GetOperation(const Get & op);
};

#endif // SERVER_CONNECTION_H
