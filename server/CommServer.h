// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2000,2001 Alistair Riddoch

#ifndef SERVER_COMM_SERVER_H
#define SERVER_COMM_SERVER_H

#include <set>
#include <string>

class CommSocket;
class CommMetaClient;
class ServerRouting;

typedef std::set<CommSocket *> comm_set_t;

class CommServer {
  private:
    comm_set_t sockets;
    time_t metaserverTime;
    CommMetaClient & metaClient;
    bool useMetaserver;

    void idle();

  public:
    const std::string identity;
    ServerRouting & server;

    CommServer(ServerRouting & srv, const std::string & ident);
    ~CommServer();

    void setupMetaserver(const std::string &);
    void shutdown();

    void loop();
    void removeSocket(CommSocket * client, char * msg);
    void removeSocket(CommSocket * client);

    int numClients() {
        return sockets.size();
    }

    void add(CommSocket * cs) {
        sockets.insert(cs);
    }
};

#endif // SERVER_COMM_SERVER_H
