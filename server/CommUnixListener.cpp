// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2000,2001 Alistair Riddoch

#include "CommUnixListener.h"

#include "CommLocalClient.h"
#include "CommServer.h"

#include "common/debug.h"
#include "common/globals.h"
#include "common/log.h"

#include <iostream>

static const bool debug_flag = false;

/// \brief Constructor unix listen socket object.
///
/// @param svr Reference to the object that manages all socket communication.
CommUnixListener::CommUnixListener(CommServer & svr) : CommSocket(svr),
                                                       m_bound(false)
{
}

CommUnixListener::~CommUnixListener()
{
    if (m_bound) {
        unlink(m_path.c_str());
    }
}


int CommUnixListener::getFd() const
{
    return m_unixListener.getSocket();
}

bool CommUnixListener::eof()
{
    return false;
}
bool CommUnixListener::isOpen() const
{
    return true;
}

int CommUnixListener::read()
{
    accept();
    return 0;
}

void CommUnixListener::dispatch()
{
}

#define UNIX_PATH_MAX 108

/// \brief Create and bind the listen socket.
int CommUnixListener::setup()
{
    m_path = var_directory + "/tmp/cyphesis.sock";

    m_unixListener.open(m_path);

    m_bound = m_unixListener.is_open();
    return m_bound ? 0 : 1;
}

/// \brief Accept a new connect to the listen socket.
bool CommUnixListener::accept()
{
    debug(std::cout << "Local accepting.." << std::endl << std::flush;);
    int asockfd = m_unixListener.accept();

    if (asockfd < 0) {
        return false;
    }
    debug(std::cout << "Local accepted" << std::endl << std::flush;);
    CommLocalClient * newcli = new CommLocalClient(m_commServer, asockfd);

    newcli->setup();

    // Add this new client to the list.
    m_commServer.add(newcli);

    return true;
}
