// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2000,2001 Alistair Riddoch

#include "CommServer.h"

#include "CommClient.h"
#include "CommMetaClient.h"
#include "ServerRouting_methods.h"
#include "protocol_instructions.h"

#include "common/log.h"
#include "common/debug.h"

#include <iostream>

#include <errno.h>

static const bool debug_flag = false;

/// \brief Construct a new CommServer object, storing a reference to the core
/// server object.
CommServer::CommServer(ServerRouting & svr) : m_server(svr)
{
}

CommServer::~CommServer()
{
    CommSocketSet::const_iterator Iend = m_sockets.end();
    for (CommSocketSet::const_iterator I = m_sockets.begin(); I != Iend; ++I) {
        delete *I;
    }
}

/// \brief Idle function called from the main loop.
///
/// Poll all the CommIdleSocket objects that want to be polled regularly,
/// the call the core server object idle function.
/// @return true if the core server wants to be called again as soon as
/// possible.
bool CommServer::idle()
{
    // Update the time, and get the core server object to process
    // stuff.
    // FIXME These idle methods are now getting called way too often
    // if the core server is busy. Cut it back a bit. Probably can avoid
    // calling them at all if we are busy.
    time_t ctime = time(NULL);
    CommIdleSocketSet::const_iterator I = m_idleSockets.begin();
    CommIdleSocketSet::const_iterator Iend = m_idleSockets.end();
    for (; I != Iend; ++I) {
        (*I)->idle(ctime);
    }
    // server.idle() is inlined, and simply calls the world idle method,
    // which is not directly accessible from here.
    return m_server.idle();
}

/// \brief Main program loop called repeatedly.
///
/// Call the server idle function to do its processing. If the server is
/// is currently busy, poll all the sockets as quickly as possible.
/// If the server is idle, use select() to sleep on the sockets for
/// a short period of time. If any sockets get broken or disconnected,
/// they are noted and closed down at the end of the process.
void CommServer::loop()
{
    // This is the main code loop.
    // Classic select code for checking incoming data on sockets.
    // It may be beneficial to re-write this code to use the poll(2) system
    // call.
    bool busy = idle();

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
            perror("select");
            log(ERROR, "Error caused by select() in main loop");
        }
        return;
    }

    if ((rval == 0) && !pendingConnections) {
        return;
    }
    
    // We assume Iend is still valid. m_sockets must not have been modified
    // between Iend's initialisation and here.
    std::set<CommSocket *> obsoleteConnections;
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
}

/// \brief Remove and delete a CommSocket from the server.
///
/// Does not take into account if the socket is
/// @param socket Pointer to the socket object to be removed.
void CommServer::removeSocket(CommSocket * socket)
{
    m_sockets.erase(socket);
    delete socket;
}
