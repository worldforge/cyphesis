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

#include "CommStreamClient.h"

CommStreamClient::CommStreamClient(CommServer & svr, int fd) :
                  CommSocket(svr),
                  m_clientIos(fd)
{
}

CommStreamClient::CommStreamClient(CommServer & svr) :
                  CommSocket(svr)
{
}

CommStreamClient::~CommStreamClient()
{
    m_clientIos.close();
}

int CommStreamClient::getFd() const
{
    return m_clientIos.getSocket();
}

bool CommStreamClient::isOpen() const
{
    return m_clientIos.is_open();
}

bool CommStreamClient::eof()
{
    return (m_clientIos.fail() || m_clientIos.peek() == EOF);
}
