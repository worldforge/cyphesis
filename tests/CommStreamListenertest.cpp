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

#ifdef NDEBUG
#undef NDEBUG
#endif
#ifndef DEBUG
#define DEBUG
#endif

#include "server/CommStreamListener_impl.h"

#include "server/CommClientFactory.h"

#include <skstream/skserver.h>

#include <boost/make_shared.hpp>

#include <cassert>

using boost::shared_ptr;

class test_socket_server : public basic_socket_server
{
  public:
};

class TestCommClientKit : public CommClientKit
{
  public:
    virtual int newCommClient(CommServer &, int, const std::string &) {
        return 0;
    }
};

class TestCommStreamListener : public CommStreamListener<test_socket_server>
{
  public:
    explicit TestCommStreamListener(CommServer & svr) :
             CommStreamListener<test_socket_server>(svr, 
             boost::make_shared<TestCommClientKit>()) {
    }

    int accept() { return 0; }

    int test_create(int asockfd, const char * address) {
        return this->create(asockfd, address);
    }
};

int main()
{
    {
        TestCommStreamListener tcsl(*(CommServer*)0);
    }

    {
        TestCommStreamListener tcsl(*(CommServer*)0);

        tcsl.test_create(-1, "foo");
    }

    {
        TestCommStreamListener tcsl(*(CommServer*)0);

        tcsl.getFd();
    }

    {
        TestCommStreamListener tcsl(*(CommServer*)0);

        tcsl.eof();
    }

    {
        TestCommStreamListener tcsl(*(CommServer*)0);

        tcsl.isOpen();
    }

    {
        TestCommStreamListener tcsl(*(CommServer*)0);

        tcsl.read();
    }

    {
        TestCommStreamListener tcsl(*(CommServer*)0);

        tcsl.dispatch();
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

CommClientKit::~CommClientKit()
{
}

#if 0

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
    return (m_clientIos.fail() ||
            m_clientIos.peek() == std::iostream::traits_type::eof());
}

Idle::Idle(CommServer & svr) : m_idleManager(svr)
{
}

Idle::~Idle()
{
}

void log(LogLevel, const std::string & msg)
{
}

ServerRouting::ServerRouting(BaseWorld & wrld,
                             const std::string & ruleset,
                             const std::string & name,
                             const std::string & id, long intId,
                             const std::string & lId, long lIntId) :
        Router(id, intId),
        m_svrRuleset(ruleset), m_svrName(name),
        m_numClients(0), m_world(wrld), m_lobby(*(Lobby*)0)
{
}

ServerRouting::~ServerRouting()
{
}

void ServerRouting::addToMessage(Atlas::Message::MapType & omap) const
{
}

void ServerRouting::addToEntity(const Atlas::Objects::Entity::RootEntity & ent) const
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

#endif
