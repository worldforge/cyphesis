// Cyphesis Online RPG Server and AI Engine
// Copyright (C) 2000,2001 Alistair Riddoch
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

// $Id$

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "CommUnixListener.h"

#include "CommClientFactory.h"
#include "CommServer.h"

#include "common/id.h"
#include "common/log.h"
#include "common/debug.h"
#include "common/system.h"
#include "common/globals.h"

#include <iostream>

#include <unistd.h>

#include <cassert>

#ifdef HAVE_SYS_UN_H

static const bool debug_flag = false;

/// \brief Constructor unix listen socket object.
///
/// @param svr Reference to the object that manages all socket communication.
CommUnixListener::CommUnixListener(CommServer & svr, CommClientKit & kit) :
                  CommStreamListener(svr, kit)
{
    m_listener = &m_unixListener;
}

CommUnixListener::~CommUnixListener()
{
    if (m_unixListener.is_open()) {
        ::unlink(m_path.c_str());
    }
}

/// \brief Create and bind the listen socket.
///
/// If an initial attempt fails, then this function attempts to unlink the
/// unix socket path, and tries again. This method is deliberatly called
/// _after_ attempting to open the TCP listen socket, so that this
/// will never be called in an instance of cyphesis already exists.
int CommUnixListener::setup(const std::string & name)
{
    m_path = name;

    m_unixListener.open(m_path);
    bool bound = m_unixListener.is_open();

    if (!bound) {
        ::unlink(m_path.c_str());

        m_unixListener.open(m_path);
        bound = m_unixListener.is_open();
    }

    return bound ? 0 : 1;
}

/// \brief Accept a new connect to the listen socket.
int CommUnixListener::accept()
{
    debug(std::cout << "Local accepting.." << std::endl << std::flush;);
    int asockfd = m_unixListener.accept();

    if (asockfd < 0) {
        return -1;
    }
    debug(std::cout << "Local accepted" << std::endl << std::flush;);

    return create(asockfd, "local");
}

#endif // HAVE_SYS_UN_H
