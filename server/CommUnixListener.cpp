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

bool CommUnixListener::read()
{
    accept();
    return false;
}

void CommUnixListener::dispatch()
{
}

#define UNIX_PATH_MAX 108

bool CommUnixListener::setup()
{
    m_path = var_directory + "/tmp/cyphesis.sock";

    m_unixListener.open(m_path);

    m_bound = m_unixListener.is_open();
    return m_bound;
}

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
