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
    void send(const RootOperation * msg);

    virtual oplist operation(const RootOperation & op);
    virtual oplist Operation(const Login & obj);
    virtual oplist Operation(const Logout & obj);
    virtual oplist Operation(const Create & obj);

    Account * add_player(string &, string &);
};

#endif /* CONNECTION_H */
