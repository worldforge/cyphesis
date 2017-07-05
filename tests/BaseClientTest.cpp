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

#include "client/BaseClient.h"

#include <Atlas/Objects/RootOperation.h>

#include <cassert>

using Atlas::Objects::Root;
using Atlas::Objects::Operation::RootOperation;

class TestBaseClient : public BaseClient
{
  public:
    TestBaseClient() : BaseClient() { }

    virtual void idle() { }
};

int main()
{
    {
        BaseClient * bc = new TestBaseClient;

        delete bc;
    }

    {
        BaseClient * bc = new TestBaseClient;

        bc->createAccount("8e7e4452-f666-11df-8027-00269e5444b3", "84abee0c-f666-11df-8f7e-00269e5444b3");

        delete bc;
    }

    {
        BaseClient * bc = new TestBaseClient;

        bc->createSystemAccount();

        delete bc;
    }

    {
        BaseClient * bc = new TestBaseClient;

        bc->createCharacter("9e7f4004-f666-11df-a327-00269e5444b3");

        delete bc;
    }

    {
        BaseClient * bc = new TestBaseClient;

        bc->logout();

        delete bc;
    }

    {
        BaseClient * bc = new TestBaseClient;

        bc->handleNet();

        delete bc;
    }

    return 0;
}

// stubs

#include "client/CreatorClient.h"

#include "common/log.h"

#include <cstdlib>

#include "stubs/rulesets/stubBaseMind.h"

CreatorClient::CreatorClient(const std::string & id, long intId,
                             ClientConnection &c) :
               CharacterClient(id, intId, c)
{
}

ClientConnection::ClientConnection()
{
}

ClientConnection::~ClientConnection()
{
}

void ClientConnection::operation(const RootOperation & op)
{
}

int ClientConnection::wait()
{
    return 0;
}

RootOperation ClientConnection::pop()
{
    return RootOperation(nullptr);
}

CharacterClient::CharacterClient(const std::string & id, long intId,
                                 ClientConnection & c) :
                 BaseMind(id, intId), m_connection(c)
{
}

MemMap::MemMap(Script *& s) : m_checkIterator(m_entities.begin()), m_script(s)
{
}

AtlasStreamClient::AtlasStreamClient() : m_io_work(m_io_service), reply_flag(false), error_flag(false),
                                         serialNo(512), m_currentTask(0)
{
}

AtlasStreamClient::~AtlasStreamClient()
{
}

void AtlasStreamClient::objectArrived(const Root & obj)
{
}

void AtlasStreamClient::operation(const RootOperation & op)
{
}

void AtlasStreamClient::infoArrived(const RootOperation & op)
{
}

void AtlasStreamClient::appearanceArrived(const RootOperation & op)
{
}

void AtlasStreamClient::disappearanceArrived(const RootOperation & op)
{
}

void AtlasStreamClient::sightArrived(const RootOperation & op)
{
}

void AtlasStreamClient::soundArrived(const RootOperation & op)
{
}

void AtlasStreamClient::errorArrived(const RootOperation & op)
{
}

void AtlasStreamClient::loginSuccess(const Root & arg)
{
}

void AtlasStreamClient::send(const RootOperation & op)
{
}

void log(LogLevel lvl, const std::string & msg)
{
}

std::string create_session_username()
{
    return "admin_test";
}

long integerId(const std::string & id)
{
    long intId = strtol(id.c_str(), 0, 10);
    if (intId == 0 && id != "0") {
        intId = -1L;
    }

    return intId;
}



MemEntity::MemEntity(const std::string & id, long intId) :
           LocatedEntity(id, intId), m_lastSeen(0.)
{
}

MemEntity::~MemEntity()
{
}

void MemEntity::externalOperation(const Operation & op, Link &)
{
}

void MemEntity::operation(const Operation &, OpVector &)
{
}

void MemEntity::destroy()
{
}

PropertyBase * MemEntity::setAttr(const std::string & name, const Atlas::Message::Element & attr)
{
    return 0;
}

#include "stubs/rulesets/stubLocatedEntity.h"

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

Location::Location() : m_loc(0)
{
}

int Location::readFromEntity(const Atlas::Objects::Entity::RootEntity & ent)
{
    return 0;
}

void WorldTime::initTimeInfo()
{
}

DateTime::DateTime(int t)
{
}
