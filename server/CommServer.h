// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2000,2001 Alistair Riddoch

#ifndef SERVER_COMM_SERVER_H
#define SERVER_COMM_SERVER_H

#include <set>

class CommSocket;
class CommIdleSocket;
class ServerRouting;

typedef std::set<CommSocket *> comm_set_t;
typedef std::set<CommIdleSocket *> commi_set_t;

class CommServer {
  private:
    comm_set_t sockets;
    commi_set_t idleSockets;

    void idle();

    // Private and unimplemented.
    CommServer(const CommServer &);
    CommServer & operator=(const CommServer &);
  public:
    ServerRouting & server;

    CommServer(ServerRouting & srv);
    ~CommServer();

    void loop();
    void removeSocket(CommSocket * client, char * msg);
    void removeSocket(CommSocket * client);

    void add(CommSocket * cs) {
        sockets.insert(cs);
    }

    // There is current no mechanism for removing things from the
    // idle set. If one is needed in future, it must be implemented.
    void addIdle(CommIdleSocket * cs) {
        idleSockets.insert(cs);
    }
};

#endif // SERVER_COMM_SERVER_H
