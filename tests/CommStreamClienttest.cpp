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


#ifdef NDEBUG
#undef NDEBUG
#endif
#ifndef DEBUG
#define DEBUG
#endif

#include "server/CommStreamClient_impl.h"

#include "null_stream.h"

#include <cassert>

template class CommStreamClient<null_stream>;

class TestCommStreamClient : public CommStreamClient<null_stream>
{
  public:
    TestCommStreamClient(int fd) :
          CommStreamClient<null_stream>(*(CommServer*)0, fd)
    {
    }

    TestCommStreamClient() : CommStreamClient<null_stream>(*(CommServer*)0)
    {
    }

    null_stream & test_stream()
    {
        return m_clientIos;
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
        TestCommStreamClient * cs = new TestCommStreamClient();

        delete cs;
    }

    {
        TestCommStreamClient * cs = new TestCommStreamClient(MAGIC_FD);

        int fd = cs->getFd();

        assert(fd == MAGIC_FD);
    }

    {
        TestCommStreamClient * cs = new TestCommStreamClient();
        cs->test_stream().reset_is_open_val(false);

        bool open = cs->isOpen();

        assert(!open);
    }

    {
        TestCommStreamClient * cs = new TestCommStreamClient();
        cs->test_stream().reset_is_open_val(true);

        bool open = cs->isOpen();

        assert(open);
    }

    {
        TestCommStreamClient * cs = new TestCommStreamClient();
        cs->test_stream().reset_fail_val(true);

        bool eof = cs->eof();

        assert(eof);
    }

    {
        TestCommStreamClient * cs = new TestCommStreamClient();
        cs->test_stream().reset_fail_val(false);
        cs->test_stream().reset_peek_val(-1);

        bool eof = cs->eof();

        assert(eof);
    }

    {
        TestCommStreamClient * cs = new TestCommStreamClient();
        cs->test_stream().reset_fail_val(false);
        cs->test_stream().reset_peek_val(1);

        bool eof = cs->eof();

        assert(!eof);
    }

    return 0;
}

// Stub functions

CommSocket::CommSocket(CommServer & svr) : m_commServer(svr) { }

CommSocket::~CommSocket()
{
}

int CommSocket::flush()
{
    return 0;
}

void log(LogLevel lvl, const std::string & msg)
{
}
