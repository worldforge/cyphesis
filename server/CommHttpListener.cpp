// Cyphesis Online RPG Server and AI Engine
// Copyright (C) 2008 Alistair Riddoch
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

#include "CommHttpListener.h"

#include "CommHttpClient.h"

#include "CommServer.h"

#include "common/log.h"

#include <iostream>

static const bool debug_flag = false;

/// \brief Constructor for listener socket object.
///
/// @param svr Reference to the object that manages all socket communication.
CommHttpListener::CommHttpListener(CommServer & svr) : CommTCPListener(svr)
{
}

CommHttpListener::~CommHttpListener()
{
}

int CommHttpListener::create(int asockfd, const char * address)
{
    CommHttpClient * newclient = new CommHttpClient(m_commServer, asockfd);

    // Add this new peer to the list.
    m_commServer.addSocket(newclient);

    return 0;
}
