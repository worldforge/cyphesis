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

#include "server/CommHttpClient.h"

#include "server/CommServer.h"
#include "server/ServerRouting.h"
#include "server/HttpCache.h"

#include "common/log.h"

#include <Atlas/Codec.h>
#include <Atlas/Negotiate.h>
#include <Atlas/Objects/Encoder.h>
#include <Atlas/Objects/RootEntity.h>
#include <Atlas/Objects/RootOperation.h>
#include <Atlas/Objects/SmartPtr.h>

#include <cstdlib>
#include <cstring>

#include <cassert>

class TestCommHttpClient : public CommHttpClient
{
  public:
    TestCommHttpClient(CommServer & svr, int fd) : CommHttpClient(svr, fd)
    {
    }

    void test_openSocket()
    {
        int fds[2];
        if (socketpair(AF_UNIX, SOCK_STREAM, 0, &fds[0]) != 0) {
            exit(1);
        }

        m_clientIos.setSocket(fds[0]);

        pid_t p = fork();
        if (p == -1) {
            exit(1);
        }
        if (p == 0) {
            char buf[1];
            while(::read(fds[1], &buf[0], 1) == 1);
            exit(0);
        }
    }

    void test_set_req_complete()
    {
        m_req_complete = true;
    }

    void test_writeRequest(const char * req_text)
    {
        int fds[2];
        if (socketpair(AF_UNIX, SOCK_STREAM, 0, &fds[0]) != 0) {
            exit(1);
        }

        m_clientIos.setSocket(fds[0]);

        pid_t p = fork();
        if (p == -1) {
            exit(1);
        }
        if (p == 0) {
            ssize_t status = ::write(fds[1], &req_text[0], strlen(req_text));
            exit(status > 0 ? 0 : 1);
        } else {
            m_clientIos.peek();
        }
    }

};

int main()
{
    CommServer comm_server;

    {
        new TestCommHttpClient(comm_server, 0);
    }

    {
        CommHttpClient * cs = new TestCommHttpClient(comm_server, 0);

        delete cs;
    }

    {
        TestCommHttpClient * cs = new TestCommHttpClient(comm_server, 0);

        cs->read();

        delete cs;
    }

    {
        TestCommHttpClient * cs = new TestCommHttpClient(comm_server, 0);

        cs->test_writeRequest("GET / HTTP/1.0\r\nHost: your.host.name\r\n\r\n");

        cs->read();

        delete cs;
    }

    {
        TestCommHttpClient * cs = new TestCommHttpClient(comm_server, 0);

        cs->dispatch();

        delete cs;
    }

    {
        TestCommHttpClient * cs = new TestCommHttpClient(comm_server, 0);

        cs->test_set_req_complete();

        cs->dispatch();

        delete cs;
    }

    return 0;
}

// Stub functions

#include "CommStreamClient_stub_impl.h"

#include <skstream/skstream.h>

template class CommStreamClient<tcp_socket_stream>;

HttpCache * HttpCache::m_instance = 0;

HttpCache::HttpCache()
{
}

void HttpCache::processQuery(std::ostream & io,
                             const std::list<std::string> & headers)
{
}

CommSocket::CommSocket(CommServer & svr) : m_commServer(svr) { }

CommSocket::~CommSocket()
{
}

int CommSocket::flush()
{
    return 0;
}

void log(LogLevel, const std::string & msg)
{
}

CommServer::CommServer() : m_congested(false)
{
}

CommServer::~CommServer()
{
}

Router::Router(const std::string & id, long intId) : m_id(id),
                                                             m_intId(intId)
{
}

Router::~Router()
{
}

void Router::addToMessage(Atlas::Message::MapType & omap) const
{
}

void Router::addToEntity(const Atlas::Objects::Entity::RootEntity & ent) const
{
}

// Library stubs
