// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2000 Alistair Riddoch

#ifndef COMM_SERVER_H
#define COMM_SERVER_H

#include <common/const.h>

extern "C" {
    #include <netinet/in.h>
    #include "protocol_instructions.h"
}

class CommClient;
class ServerRouting;

typedef std::map<int, CommClient *> client_map_t;

class CommServer {
    int serverFd;
    int serverPort;
    client_map_t clients;

    int accept();
    void idle();

    static const int metaserverPort = 8453;
    time_t metaserverTime;
    struct sockaddr_in meta_sa;
    int metaFd;
  public:
    static bool useMetaserver;
    const string identity;
    ServerRouting & server;

    CommServer(const string & ident);

    int setup(int port);
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

#endif // COMM_SERVER_H
