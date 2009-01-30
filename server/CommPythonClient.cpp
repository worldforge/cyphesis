// Cyphesis Online RPG Server and AI Engine
// Copyright (C) 2009 Alistair Riddoch
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

#include "CommPythonClient.h"
#include "CommServer.h"
#include "HttpCache.h"

static const bool debug_flag = false;

CommPythonClient::CommPythonClient(CommServer & svr, int fd) :
                CommStreamClient(svr, fd)
{
    m_clientIos.setTimeout(0,1000); // FIXME?
}

CommPythonClient::~CommPythonClient()
{
}

void CommPythonClient::dispatch()
{
}

int CommPythonClient::read()
{
    m_clientIos.peek();

    std::streamsize count;

    while ((count = m_clientIos.rdbuf()->in_avail()) > 0) {

        for (int i = 0; i < count; ++i) {

            int next = m_clientIos.rdbuf()->sbumpc();
            if (next == '\n') {
                if (m_incoming.empty()) {
                    std::cout << "[NOT]" << std::endl << std::flush;
                } else {
                    std::cout << m_incoming << std::endl << std::flush;
                    m_incoming.clear();
                }
            } else if (next == '\r') {
            } else {
                m_incoming.append(1, next);
            }
        }
    }

    return 0;
}
