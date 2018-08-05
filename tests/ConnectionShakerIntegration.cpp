// Cyphesis Online RPG Server and AI Engine
// Copyright (C) 2012 Anthony Pesce
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

#include "TestBase.h"

#include "server/Connection.h"
#include "server/ServerRouting.h"
#include "rulesets/ExternalMind.h"
#include "rulesets/ExternalProperty.h"

#include "rulesets/Character.h"

#include "common/CommSocket.h"
#include "common/Shaker.h"
#include "common/PropertyManager.h"
#include "common/globals.h"
#include "common/log.h"
#include "common/const.h"

#include <cstdio>

using Atlas::Message::Element;
using Atlas::Message::ListType;
using Atlas::Message::MapType;
using String::compose;

bool database_flag = false;

int salt_length = -1;

class ConnectionShakerintegration : public Cyphesis::TestBase
{
  private:
    long m_id_counter;

    ServerRouting * m_server;
    Connection * m_connection;
  public:
    ConnectionShakerintegration();
    void setup();
    void teardown();

    void testShaker();
};

ConnectionShakerintegration::ConnectionShakerintegration() : m_id_counter(0L),
                                                             m_server(0),
                                                             m_connection(0)
{
    ADD_TEST(ConnectionShakerintegration::testShaker);
}

void ConnectionShakerintegration::testShaker()
{
    ASSERT_NOT_NULL(m_connection);
    m_connection->addNewAccount("player",
                                "testuser",
                                "testpassword");

    ASSERT_EQUAL(salt_length, 16);
}

void ConnectionShakerintegration::setup()
{
    m_server = new ServerRouting(*(BaseWorld*)0,
                                 "b88aa6d3-44b4-40bd-bfa9-8db00045bdc0",
                                 "0f1fc7cb-5ab1-45c1-b0d3-ae49205ea437",
                                 compose("%1", m_id_counter), m_id_counter++,
                                 compose("%1", m_id_counter), m_id_counter++);
    m_connection = new Connection(*(CommSocket*)0,
                                  *m_server,
                                  "test_addr",
                                  compose("%1", m_id_counter), m_id_counter++);
}

void ConnectionShakerintegration::teardown()
{
    delete m_server;
    delete m_connection;
}

int main()
{
   ConnectionShakerintegration t;
   return t.run();
}

// Stubs

#include "server/Lobby.h"
#include "server/Player.h"

#include "common/Inheritance.h"
#include "stubs/common/stubLink.h"

using Atlas::Objects::Root;

bool restricted_flag;

namespace Atlas { namespace Objects { namespace Operation {
int UPDATE_NO = -1;
} } }



CommSocket::CommSocket(boost::asio::io_service & svr) : m_io_service(svr) { }

CommSocket::~CommSocket()
{
}

int CommSocket::flush()
{
    return 0;
}

#include "stubs/server/stubPlayer.h"
#include "stubs/server/stubAccount.h"


ConnectableRouter::ConnectableRouter(const std::string & id,
                                 long iid,
                                 Connection *c) :
                 Router(id, iid),
                 m_connection(c)
{
}
#include "stubs/server/stubServerRouting.h"

Lobby::Lobby(ServerRouting & s, const std::string & id, long intId) :
       Router(id, intId),
       m_server(s)
{
}

Lobby::~Lobby()
{
}

void Lobby::delAccount(Account * ac)
{
}

void Lobby::addToMessage(MapType & omap) const
{
}

void Lobby::addToEntity(const Atlas::Objects::Entity::RootEntity & ent) const
{
}

void Lobby::addAccount(Account * ac)
{
}

void Lobby::externalOperation(const Operation &, Link &)
{
}

void Lobby::operation(const Operation & op, OpVector & res)
{
}

ExternalMind::ExternalMind(LocatedEntity & e) : Router(e.getId(), e.getIntId()),
                                         m_link(0), m_entity(e)
{
}

void ExternalMind::externalOperation(const Operation & op, Link &)
{
}

void ExternalMind::operation(const Operation & op, OpVector & res)
{
}

ExternalProperty::ExternalProperty(ExternalMind * & data) : m_data(data)
{
}

int ExternalProperty::get(Atlas::Message::Element & val) const
{
    return 0;
}

void ExternalProperty::set(const Atlas::Message::Element & val)
{
}

void ExternalProperty::add(const std::string & s,
                         Atlas::Message::MapType & map) const
{
}

void ExternalProperty::add(const std::string & s,
                         const Atlas::Objects::Entity::RootEntity & ent) const
{
}

ExternalProperty * ExternalProperty::copy() const
{
    return 0;
}

const std::string & ExternalMind::connectionId()
{
    assert(m_link != 0);
    return m_link->getId();
}

void ExternalMind::linkUp(Link * c)
{
    m_link = c;
}

#include "stubs/rulesets/stubCharacter.h"
#include "stubs/rulesets/stubThing.h"
#include "stubs/rulesets/stubEntity.h"
#include "stubs/rulesets/stubLocatedEntity.h"
#include "stubs/server/stubExternalMindsManager.h"
#include "stubs/server/stubExternalMindsConnection.h"
#include "stubs/common/stubOperationsDispatcher.h"


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

void Router::error(const Operation & op,
                   const std::string & errstring,
                   OpVector & res,
                   const std::string & to) const
{
}

void Router::clientError(const Operation & op,
                         const std::string & errstring,
                         OpVector & res,
                         const std::string & to) const
{
}
#include "stubs/rulesets/stubLocation.h"
#include "stubs/common/stubProperty.h"


#ifndef STUB_Inheritance_getClass
#define STUB_Inheritance_getClass
const Atlas::Objects::Root& Inheritance::getClass(const std::string & parent)
{
    return noClass;}
#endif //STUB_Inheritance_getClass

#ifndef STUB_Inheritance_getType
#define STUB_Inheritance_getType
const TypeNode* Inheritance::getType(const std::string & parent)
{
    TypeNodeDict::const_iterator I = atlasObjects.find(parent);
    if (I == atlasObjects.end()) {
        return 0;
    }
    return I->second;}
#endif //STUB_Inheritance_getType

#include "stubs/common/stubInheritance.h"

void log(LogLevel lvl, const std::string & msg)
{
}
void hash_password(const std::string & pwd, const std::string & salt,
                   std::string & hash )
{
    salt_length=salt.length();
}
void logEvent(LogEvent lev, const std::string & msg)
{
}

long integerId(const std::string & id)
{
    long intId = strtol(id.c_str(), 0, 10);
    if (intId == 0 && id != "0") {
        intId = -1L;
    }

    return intId;
}

static long idGenerator = 0;

long newId(std::string & id)
{
    static char buf[32];
    long new_id = ++idGenerator;
    sprintf(buf, "%ld", new_id);
    id = buf;
    assert(!id.empty());
    return new_id;
}

void addToEntity(const Vector3D & v, std::vector<double> & vd)
{
    vd.resize(3);
    vd[0] = v[0];
    vd[1] = v[1];
    vd[2] = v[2];
}

int check_password(const std::string & pwd, const std::string & hash)
{
    return 0;
}
