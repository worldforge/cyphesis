#ifndef CONNECTION_H
#define CONNECTION_H

typedef int bad_type; // Remove this to get unset type reporting

#define None 0 // Remove this to deal with un-initialied vars

#include "Routing.h"

class CommClient;
class ServerRouting;
class Account;
class Player;

class Connection : public Routing {
  public:
    ServerRouting * server;
    CommClient * comm_client;

    Connection(CommClient * comm_client);
    virtual ~Connection() { }
    void destroy();
    void disconnect();
    void send(RootOperation * msg);

    virtual RootOperation * operation(const RootOperation & op);
    virtual RootOperation * Operation(const Login & obj);
    virtual RootOperation * Operation(const Logout & obj);
    virtual RootOperation * Operation(const Create & obj);

    Account * add_player(string &, string &);
};

#endif /* CONNECTION_H */
