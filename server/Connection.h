// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2000 Alistair Riddoch

#ifndef CONNECTION_H
#define CONNECTION_H

#include "Routing.h"

class ServerRouting;
class Account;
class Player;
class CommClient;

class Connection : public Routing {
    CommClient & comm_client;
    ServerRouting & server;

    Account * add_player(string &, string &);
  public:

    Connection(CommClient & client);
    virtual ~Connection() { }

    void destroy();
    void send(const RootOperation * msg) const;

    virtual oplist operation(const RootOperation & op);
    virtual oplist Operation(const Login & op);
    virtual oplist Operation(const Logout & op);
    virtual oplist Operation(const Create & op);
    virtual oplist Operation(const Get & op);
};

#endif /* CONNECTION_H */
