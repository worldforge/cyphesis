// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2000,2001 Alistair Riddoch

#ifndef SERVER_CONNECTION_H
#define SERVER_CONNECTION_H

#include "Routing.h"

class ServerRouting;
class CommClient;
class Account;

class Connection : public Routing {
    CommClient & commClient;

    Account * addPlayer(const std::string &, const std::string &);
  public:
    ServerRouting & server;

    explicit Connection(CommClient & client);
    virtual ~Connection();

    void destroy();
    inline void send(const RootOperation * msg) const;

    virtual OpVector operation(const RootOperation & op);

    virtual OpVector LoginOperation(const Login & op);
    virtual OpVector LogoutOperation(const Logout & op);
    virtual OpVector CreateOperation(const Create & op);
    virtual OpVector GetOperation(const Get & op);
};

#endif // SERVER_CONNECTION_H
