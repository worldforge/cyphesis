// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2000 Alistair Riddoch

#ifndef CONNECTION_H
#define CONNECTION_H

#include "Routing.h"
#include "CommClient.h"
#include "CommServer.h"

class ServerRouting;
class Account;
class Player;
class CommClient;

class Connection : public Routing {
    CommClient * comm_client;

    Account * add_player(string &, string &);
  public:
    ServerRouting * server;

    Connection(CommClient * client) : comm_client(client) {
        server=comm_client->server->server;
    }
    virtual ~Connection() { }
    void destroy();

    void disconnect() {
        destroy();
    }

    void send(const RootOperation * msg) const {
        if (comm_client != NULL) {
            comm_client->send(msg);
        }
    }

    virtual oplist operation(const RootOperation & op);
    virtual oplist Operation(const Login & op);
    virtual oplist Operation(const Logout & op);
    virtual oplist Operation(const Create & op);
    virtual oplist Operation(const Get & op);
};

#endif /* CONNECTION_H */
