// Cyphesis Online RPG Server and AI Engine
// Copyright (C) 2000-2004 Alistair Riddoch
//
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version.
// 
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
// GNU General Public License for more details.
// 
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software Foundation,
// Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA


#ifndef SERVER_COMM_SERVER_H
#define SERVER_COMM_SERVER_H

#include <set>

class CommSocket;
class Idle;
class ServerRouting;
class SystemTime;

typedef std::set<CommSocket *> CommSocketSet;
typedef std::set<Idle *> IdleSet;

/// \brief Central class in the commications subsystem.
///
/// Stores all the objects that handle network communications. This includes
/// remote clients, local clients, listeners, database monitors and any
/// other code which requires listening on a socket.
/// \ingroup ServerSockets
class CommServer {
  private:
    /// Set of pointers to CommSocket objects managed by this object.
    CommSocketSet m_sockets;
    /// Set of pointer to Idle objects which need to be polled.
    IdleSet m_idlers;
    /// File descriptor used as handle for Linux epoll.
    int m_epollFd;
    /// Flag indicating whether we had network traffic last tick
    bool m_congested;
    /// Seconds when we last called idlers
    int m_tick;

    CommServer(const CommServer &) = delete;
    CommServer & operator=(const CommServer &) = delete;
  public:
    CommServer();
    ~CommServer();

    int setup();
    void poll(bool);
    bool idle(const SystemTime &, bool);
    int addSocket(CommSocket * cs);
    void removeSocket(CommSocket * client);

    /// \brief Accessor for the current time in seconds.
    long time() const {
        return m_tick;
    }

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
