// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2000,2001 Alistair Riddoch

#ifndef SERVER_COMM_SERVER_H
#define SERVER_COMM_SERVER_H

#include <set>
#include <string>

// extern "C" {
    // #include <netinet/in.h>
// }

class CommSocket;
class CommMetaClient;
class ServerRouting;

typedef std::set<CommSocket *> comm_set_t;

class CommServer {
  private:
    // int serverFd;
    // int serverPort;
    comm_set_t sockets;
    time_t metaserverTime;
    CommMetaClient & metaClient;
    bool useMetaserver;
    // struct sockaddr_in meta_sa;
    // int metaFd;

    // bool accept();
    void idle();

    static const int metaserverPort = 8453;

  public:
    const std::string identity;
    ServerRouting & server;

    CommServer(const std::string & ruleset, const std::string & ident);
    ~CommServer();

    bool setup(int port);
    void shutdown();

    void loop();
    void removeSocket(CommSocket * client, char * msg);
    void removeSocket(CommSocket * client);
    // void metaserverKeepalive();
    // void metaserverReply();
    // void metaserverTerminate();

    int numClients() {
        return sockets.size();
    }

    void add(CommSocket * cs) {
	sockets.insert(cs);
    }
};

#endif // SERVER_COMM_SERVER_H
