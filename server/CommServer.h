// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2000,2001 Alistair Riddoch

#ifndef SERVER_COMM_SERVER_H
#define SERVER_COMM_SERVER_H

#include <common/const.h>

extern "C" {
    #include <netinet/in.h>
}

#include "protocol_instructions.h"

class CommClient;
class ServerRouting;

typedef std::map<int, CommClient *> client_map_t;

class CommServer {
  private:
    int serverFd;
    int serverPort;
    client_map_t clients;
    time_t metaserverTime;
    struct sockaddr_in meta_sa;
    int metaFd;

    bool accept();
    void idle();

    static const int metaserverPort = 8453;

  public:
    bool useMetaserver;
    const std::string identity;
    ServerRouting & server;

    CommServer(const std::string & ruleset, const std::string & ident);
    ~CommServer();

    bool setup(int port);
    void loop();
    void removeClient(CommClient * client, char * msg);
    void removeClient(CommClient * client);
    void metaserverKeepalive();
    void metaserverReply();
    void metaserverTerminate();

    int numClients() {
        return clients.size();
    }
};

#endif // SERVER_COMM_SERVER_H
