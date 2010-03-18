// Cyphesis Online RPG Server and AI Engine
// Copyright (C) 2010 Alistair Riddoch
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

#include "server/CommStreamClient.h"

#include <cassert>

class TestCommStreamClient : public CommStreamClient
{
  public:
    TestCommStreamClient(int fd) : CommStreamClient(*(CommServer*)0, fd)
    {
    }

    TestCommStreamClient() : CommStreamClient(*(CommServer*)0)
    {
    }

    virtual int read()
    {
        return 0;
    }

    virtual void dispatch()
    {
    }

};

static const int MAGIC_FD = 0xdadbeef;

int main()
{
    {
        new TestCommStreamClient();
    }

    {
        new TestCommStreamClient(0);
    }

    {
        CommStreamClient * cs = new TestCommStreamClient();

        delete cs;
    }

    {
        CommStreamClient * cs = new TestCommStreamClient(MAGIC_FD);

        int fd = cs->getFd();

        assert(fd == MAGIC_FD);
    }

    {
        CommStreamClient * cs = new TestCommStreamClient();

        bool open = cs->isOpen();

        assert(!open);
    }

    {
        CommStreamClient * cs = new TestCommStreamClient();

        bool eof = cs->eof();

        assert(eof);
    }

    return 0;
}

// Stub functions

CommSocket::CommSocket(CommServer & svr) : m_commServer(svr) { }

CommSocket::~CommSocket()
{
}

// Library stubs
