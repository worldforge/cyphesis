#ifndef CONNECTION_H
#define CONNECTION_H

#include "Routing.h"
#include "CommClient.h"
#include "CommServer.h"

class ServerRouting;
class Account;
class Player;

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

    void send(const RootOperation * msg) {
        if (comm_client != NULL) {
            comm_client->send(msg);
        }
    }

    virtual oplist operation(const RootOperation & op);
    virtual oplist Operation(const Login & obj);
    virtual oplist Operation(const Logout & obj);
    virtual oplist Operation(const Create & obj);
};

#endif /* CONNECTION_H */
