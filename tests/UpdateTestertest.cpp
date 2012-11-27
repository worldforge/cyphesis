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

#include "server/UpdateTester.h"

#include "server/CommServer.h"
#include "server/Connection.h"
#include "server/Lobby.h"

#include "common/Inheritance.h"
#include "common/log.h"

#include <cassert>

enum test_type { NOCLASS, NULLCLASS, OAKCLASS } stub_getclass_returns;

int main()
{
    CommServer comm_server;

    {
        UpdateTester ut(comm_server);
    }

    {
        UpdateTester ut(comm_server);

        ut.idle(0);
    }

    {
        UpdateTester ut(comm_server);

        stub_getclass_returns = NULLCLASS;
        ut.idle(0);
        stub_getclass_returns = NOCLASS;
    }

    {
        UpdateTester ut(comm_server);

        stub_getclass_returns = OAKCLASS;
        ut.idle(0);
        stub_getclass_returns = NOCLASS;
    }

    return 0;
}

// stubs

Idle::Idle(CommServer & svr) : m_idleManager(svr)
{
}

Idle::~Idle()
{
}

Lobby::Lobby(ServerRouting & s, const std::string & id, long intId) :
       Router(id, intId),
       m_server(s)
{
}

Lobby::~Lobby()
{
}

void Lobby::externalOperation(const Operation & op)
{
}

void Lobby::operation(const Operation & op, OpVector & res)
{
}

void Lobby::addToMessage(Atlas::Message::MapType & omap) const
{
}

void Lobby::addToEntity(const Atlas::Objects::Entity::RootEntity & ent) const
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

Inheritance * Inheritance::m_instance = NULL;

Inheritance::Inheritance() : noClass(0)
{
}

Inheritance & Inheritance::instance()
{
    if (m_instance == NULL) {
        m_instance = new Inheritance();
    }
    return *m_instance;
}

const Atlas::Objects::Root & Inheritance::getClass(const std::string & parent)
{
    switch (stub_getclass_returns) {
      case NULLCLASS:
        return *new Atlas::Objects::Root(0);;
        break;
      case OAKCLASS:
        {
        Atlas::Objects::Root oak;
        oak->setId("oak");
        oak->setParents(std::list<std::string>(1, "game_entity"));
        return *new Atlas::Objects::Root(oak);
        }
        break;
      case NOCLASS:
      default:
        return noClass;
        break;
    }
}

void log(LogLevel lvl, const std::string & msg)
{
}
