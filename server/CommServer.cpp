// Cyphesis Online RPG Server and AI Engine
// Copyright (C) 2000-2005 Alistair Riddoch
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

// $Id: CommServer.cpp,v 1.57 2006-10-26 00:48:14 alriddoch Exp $

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "CommServer.h"

#include "CommSocket.h"
#include "Idle.h"
#include "ServerRouting.h"

#include "common/log.h"
#include "common/debug.h"
#include "common/BaseWorld.h"
#include "common/compose.hpp"

#include <iostream>

extern "C" {
#ifdef HAVE_EPOLL_CREATE
    #include <sys/epoll.h>
#endif // HAVE_EPOLL_CREATE
    #include <errno.h>
}

static const bool debug_flag = false;

/// \brief Construct a new CommServer object, storing a reference to the core
/// server object.
CommServer::CommServer(ServerRouting & svr) : m_congested(false), m_server(svr)
{
#ifdef HAVE_EPOLL_CREATE
    // 64 seems like a suitable value for initial number of sockets to be
    // handled, though there is very little documentation on what would be
    // a good choice here.
    m_epollFd = epoll_create(64);
    if (m_epollFd < 0) {
        log(CRITICAL, String::compose("epoll_create: %s", strerror(errno)).c_str());
        exit_flag = true;
    }
#endif // HAVE_EPOLL_CREATE
}

CommServer::~CommServer()
{
#ifdef HAVE_EPOLL_CREATE
    close(m_epollFd);
#endif // HAVE_EPOLL_CREATE
    CommSocketSet::const_iterator Iend = m_sockets.end();
    for (CommSocketSet::const_iterator I = m_sockets.begin(); I != Iend; ++I) {
        delete *I;
    }
}

/// \brief Idle function called from the main loop.
///
/// Poll all the Idle objects that want to be polled regularly,
/// Call the core server object idle function.
/// @return true if the core server wants to be called again as soon as
/// possible.
bool CommServer::idle()
{
    // Update the time, and get the core server object to process
    // stuff.
    time_t old_seconds = m_timeVal.tv_sec;
    gettimeofday(&m_timeVal, NULL);

    bool busy = m_server.m_world.idle(m_timeVal.tv_sec, m_timeVal.tv_usec);

    // We only call the idlers if the world has returned that it is not busy,
    // and the last call to select/poll with a sleep time provided did not
    // return any traffic.
    if (!busy && !m_congested && old_seconds != m_timeVal.tv_sec) {
        IdleSet::const_iterator I = m_idlers.begin();
        IdleSet::const_iterator Iend = m_idlers.end();
        for (; I != Iend; ++I) {
            (*I)->idle(m_timeVal.tv_sec);
        }
    } else {
        // if (busy) { std::cout << "No idle because server busy" << std::endl << std::flush; }
        // if (m_congested) { std::cout << "No idle because clients busy" << std::endl << std::flush; }
    }

    return busy;
}

/// \brief Main program loop called repeatedly.
///
/// Call the server idle function to do its processing. If the server is
/// is currently busy, poll all the sockets as quickly as possible.
/// If the server is idle, use select() to sleep on the sockets for
/// a short period of time. If any sockets get broken or disconnected,
/// they are noted and closed down at the end of the process.
void CommServer::poll()
{
    // This is the main code loop.
    // Classic select code for checking incoming data on sockets.

    // It would be useful to let idle know if we are currently dealing with
    // traffic
    bool busy = idle();

#ifdef HAVE_EPOLL_CREATE
    static const int max_events = 16;

    static struct epoll_event events[max_events];

    int rval = ::epoll_wait(m_epollFd, events, max_events, (busy ? 0 : 100));

    if (rval <  0) {
        if (errno != EINTR) {
            log(ERROR, String::compose("epoll_wait: %1", strerror(errno)).c_str());
        }
        return;
    }

    m_congested = (rval != 0) || m_congested && busy;

    if (rval == max_events) {
        // If we see this alot, we should increase the maximum
        log(NOTICE, "epoll_wait returned the maximum number of events.");
    }

    for (int i = 0; i < rval; ++i) {
        struct epoll_event & event = events[i];
        CommSocket * cs = (CommSocket *)event.data.ptr;
        if (event.events & EPOLLHUP) {
            removeSocket(cs);
        } else {
            // FIXME If this never happens, then it can go
            if (event.events & EPOLLERR) {
                log(WARNING, "Socket error returned by epoll()");
            }
            if (event.events & EPOLLIN) {
                if (cs->eof()) {
                    removeSocket(cs);
                } else {
                    if (cs->read() != 0) {
                        // Remove it?
                        // FIXME It could be bad to do this, as dispatch()
                        // has not been called.
                        removeSocket(cs);
                    } else {
                        cs->dispatch();
                    }
                }
            }
        }
    }
#else // HAVE_EPOLL_CREATE

    fd_set sock_fds;
    int highest = 0;
    struct timeval tv;

    tv.tv_sec = 0;
    tv.tv_usec = (busy ? 0 : 100000);

    FD_ZERO(&sock_fds);

    bool pendingConnections = false;
    CommSocketSet::const_iterator Iend = m_sockets.end();
    for (CommSocketSet::const_iterator I = m_sockets.begin(); I != Iend; ++I) {
       if (!(*I)->isOpen()) {
           pendingConnections = true;
           continue;
       }
       int socket_fd = (*I)->getFd();
       FD_SET(socket_fd, &sock_fds);
       if (socket_fd > highest) {
           highest = socket_fd;
       }
    }
    highest++;
    int rval = ::select(highest, &sock_fds, NULL, NULL, &tv);

    if (rval < 0) {
        if (errno != EINTR) {
            log(ERROR, "Error caused by select() in main loop");
            logSysError(ERROR);
        }
        return;
    }

    if ((rval == 0) && !pendingConnections) {
        return;
    }
    
    // We assume Iend is still valid. m_sockets must not have been modified
    // between Iend's initialisation and here.
    CommSocketSet obsoleteConnections;
    for (CommSocketSet::const_iterator I = m_sockets.begin(); I != Iend; ++I) {
       CommSocket * socket = *I;
       if (!socket->isOpen()) {
           obsoleteConnections.insert(socket);
           continue;
       }
       if (FD_ISSET(socket->getFd(), &sock_fds)) {
           if (!socket->eof()) {
               if (socket->read() != 0) {
                   debug(std::cout << "Removing socket due to failure"
                                   << std::endl << std::flush;);
                   obsoleteConnections.insert(socket);
               }
               socket->dispatch();
           } else {
               // It is not clear why but on some implementation/circumstances
               // socket->eof() is true, and sometimes it isn't.
               // Either way, the stream is now done, and we should remove it
               obsoleteConnections.insert(socket);
           }
       }
    }
    CommSocketSet::const_iterator J = obsoleteConnections.begin();
    CommSocketSet::const_iterator Jend = obsoleteConnections.end();
    for (; J != Jend; ++J) {
        removeSocket(*J);
    }
#endif // HAVE_EPOLL_CREATE
}

/// Add a new CommSocket object to the manager.
void CommServer::addSocket(CommSocket * cs)
{
#ifdef HAVE_EPOLL_CREATE
    struct epoll_event ee;
    ee.events = EPOLLIN | EPOLLERR | EPOLLHUP;
    ee.data.u64 = 0;
    ee.data.ptr = cs;
    int ret = ::epoll_ctl(m_epollFd, EPOLL_CTL_ADD, cs->getFd(), &ee);
    if (ret != 0) {
        log(ERROR, "Error calling epoll_ctl to add socket");
        logSysError(ERROR);
    }
#endif // HAVE_EPOLL_CREATE
    m_sockets.insert(cs);
}

/// \brief Remove and delete a CommSocket from the server.
///
/// Does not take into account if the socket is
/// @param socket Pointer to the socket object to be removed.
void CommServer::removeSocket(CommSocket * cs)
{
#ifdef HAVE_EPOLL_CREATE
    struct epoll_event ee;
    // FIXME This may not be necessary
    int ret = ::epoll_ctl(m_epollFd, EPOLL_CTL_DEL, cs->getFd(), &ee);
    if (ret != 0) {
        log(ERROR, "Error calling epoll_ctl to remove socket");
        logSysError(ERROR);
    }
#endif // HAVE_EPOLL_CREATE
    m_sockets.erase(cs);
    delete cs;
}
