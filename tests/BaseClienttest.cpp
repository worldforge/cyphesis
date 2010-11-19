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

        bc->createSystemAccount();

        delete bc;
    }

    return 0;
}

// stubs

#include "client/CreatorClient.h"

#include "common/log.h"

#include <cstdlib>

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
    return RootOperation(0);
}

CharacterClient::CharacterClient(const std::string & id, long intId,
                                 ClientConnection & c) :
                 BaseMind(id, intId), m_connection(c)
{
}

MemMap::MemMap(Script *& s) : m_checkIterator(m_entities.begin()), m_script(s)
{
}

AtlasStreamClient::AtlasStreamClient() : reply_flag(false), error_flag(false),
                                         serialNo(512), m_fd(-1), m_encoder(0),
                                         m_codec(0), m_ios(0), m_currentTask(0)
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

BaseMind::BaseMind(const std::string & id, long intId) :
          MemEntity(id, intId), m_map(m_script)
{
}

BaseMind::~BaseMind()
{
}

void BaseMind::SightOperation(const Operation & op, OpVector & res)
{
}

void BaseMind::SoundOperation(const Operation & op, OpVector & res)
{
}

void BaseMind::AppearanceOperation(const Operation & op, OpVector & res)
{
}

void BaseMind::DisappearanceOperation(const Operation & op, OpVector & res)
{
}

void BaseMind::UnseenOperation(const Operation & op, OpVector & res)
{
}

void BaseMind::operation(const Operation & op, OpVector & res)
{
}

MemEntity::MemEntity(const std::string & id, long intId) :
           LocatedEntity(id, intId), m_visible(false), m_lastSeen(0.)
{
}

MemEntity::~MemEntity()
{
}

LocatedEntity::LocatedEntity(const std::string & id, long intId) :
               Router(id, intId),
               m_refCount(0), m_seq(0),
               m_script(0), m_type(0), m_contains(0)
{
}

LocatedEntity::~LocatedEntity()
{
}

bool LocatedEntity::hasAttr(const std::string & name) const
{
    return false;
}

bool LocatedEntity::getAttr(const std::string & name, Atlas::Message::Element & attr) const
{
    return false;
}

bool LocatedEntity::getAttrType(const std::string & name,
                                Atlas::Message::Element & attr,
                                int type) const
{
    return false;
}

void LocatedEntity::setAttr(const std::string & name, const Atlas::Message::Element & attr)
{
    return;
}

const PropertyBase * LocatedEntity::getProperty(const std::string & name) const
{
    return 0;
}

void LocatedEntity::onContainered()
{
}

void LocatedEntity::onUpdated()
{
}

void LocatedEntity::merge(const Atlas::Message::MapType & ent)
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

void Create_PyMind(BaseMind * mind, const std::string & package,
                                    const std::string & type)
{
}
