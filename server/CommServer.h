// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2000,2001 Alistair Riddoch

#ifndef SERVER_COMM_SERVER_H
#define SERVER_COMM_SERVER_H

#include <set>

class CommSocket;
class Idle;
class ServerRouting;

typedef std::set<CommSocket *> CommSocketSet;
typedef std::set<Idle *> IdleSet;

/// \brief Central class in the commications subsystem.
///
/// Stores all the objects that handle network communications. This includes
/// remote clients, local clients, listeners, database monitors and any
/// other code which requires listening on a socket.
class CommServer {
  private:
    /// Set of pointers to CommSocket objects managed by this object.
    CommSocketSet m_sockets;
    /// Set of pointer to Idle objects which need to be polled.
    IdleSet m_idlers;

    bool idle();

    /// Private and unimplemented to prevent slicing.
    CommServer(const CommServer &);
    /// Private and unimplemented to prevent slicing.
    CommServer & operator=(const CommServer &);
  public:
    /// Reference to the core ServerRouting object.
    ServerRouting & m_server;

    explicit CommServer(ServerRouting & srv);
    ~CommServer();

    void loop();
    void addSocket(CommSocket * cs);
    void removeSocket(CommSocket * client);

    /// \brief Add a new Idle object to the manager.
    ///
    /// Idle objects are removed automatically from the
    /// destructor.
    void addIdle(Idle * cs) {
        m_idlers.insert(cs);
    }

    /// \brief Remove a Idle object from the manager.
    ///
    /// Called from the Idle destructor.
    void removeIdle(Idle * cs) {
        m_idlers.erase(cs);
    }
};

#endif // SERVER_COMM_SERVER_H
