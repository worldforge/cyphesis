// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2000,2001 Alistair Riddoch

#ifndef SERVER_COMM_SERVER_H
#define SERVER_COMM_SERVER_H

#include <set>

class CommSocket;
class CommIdleSocket;
class ServerRouting;

typedef std::set<CommSocket *> CommSocketSet;
typedef std::set<CommIdleSocket *> CommIdleSocketSet;

/// \brief Central class in the commications subsystem.
///
/// Stores all the objects that handle network communications. This includes
/// remote clients, local clients, listeners, database monitors and any
/// other code which requires listening on a socket.
class CommServer {
  private:
    /// Set of pointers to CommSocket objects managed by this object.
    CommSocketSet m_sockets;
    /// Set of pointer to CommIdleSocket object which need to be polled.
    CommIdleSocketSet m_idleSockets;

    bool idle();

    /// Private and unimplemented to prevent slicing.
    CommServer(const CommServer &);
    /// Private and unimplemented to prevent slicing.
    CommServer & operator=(const CommServer &);
  public:
    /// Reference to the core ServerRouting object.
    ServerRouting & server;

    explicit CommServer(ServerRouting & srv);
    ~CommServer();

    void loop();
    void removeSocket(CommSocket * client, char * msg);
    void removeSocket(CommSocket * client);

    /// Add a new CommSocket object to the manager.
    void add(CommSocket * cs) {
        m_sockets.insert(cs);
    }

    /// \brief Add a new CommIdleSocket object to the manager.
    ///
    /// There is current no mechanism for removing things from the
    /// idle set. If one is needed in future, it must be implemented.
    void addIdle(CommIdleSocket * cs) {
        m_idleSockets.insert(cs);
    }
};

#endif // SERVER_COMM_SERVER_H
