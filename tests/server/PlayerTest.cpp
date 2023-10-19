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


#ifdef NDEBUG
#undef NDEBUG
#endif
#ifndef DEBUG
#define DEBUG
#endif

#include "../TestBase.h"
#include "../TestWorld.h"

#include "server/Player.h"

#include "server/Connection.h"
#include "server/Ruleset.h"
#include "server/ServerRouting.h"
#include "rules/simulation/ExternalMind.h"

#include "rules/simulation/Entity.h"

#include "common/CommSocket.h"
#include "common/compose.hpp"
#include "common/debug.h"
#include "../NullPropertyManager.h"

#include <Atlas/Objects/Anonymous.h>
#include <Atlas/Objects/Operation.h>
#include <Atlas/Objects/SmartPtr.h>

#include <cassert>
#include <server/EntityBuilder.h>

using Atlas::Message::Element;
using Atlas::Message::ListType;
using Atlas::Message::MapType;
using Atlas::Objects::Root;
using Atlas::Objects::Entity::Anonymous;
using Atlas::Objects::Entity::RootEntity;

using String::compose;

template <typename T>
std::ostream & operator<<(std::ostream & os,
                          const std::list<T> & sl)
{
    typename std::list<T>::const_iterator I = sl.begin();
    typename std::list<T>::const_iterator Iend = sl.end();
    os << "[";
    for (; I != Iend; ++I) {
        if (I != sl.begin()) {
            os << ", ";
        }
        os << *I;
    }
    os << "]";
    return os;
}

class Playertest : public Cyphesis::TestBase
{
  protected:
    static long m_id_counter;

    ServerRouting * m_server;
    Connection * m_connection;
    Player * m_account;
    TestWorld* m_world;

    public:
    Playertest();

    static long newId();

    void setup() override;
    void teardown() override;

    void test_getType();

};

long Playertest::m_id_counter = 0L;

Playertest::Playertest() : m_server(nullptr),
                         m_connection(nullptr),
                         m_account(nullptr)
{
    ADD_TEST(Playertest::test_getType);
}

long Playertest::newId()
{
    return ++m_id_counter;
}

void Playertest::setup()
{
    Ref<Entity> gw = new Entity(m_id_counter++);
    m_world = new TestWorld(gw);

    m_server = new ServerRouting(*m_world,
                                 *(Persistence*)nullptr,
                                 "5529d7a4-0158-4dc1-b4a5-b5f260cac635",
                                 "bad621d4-616d-4faf-b9e6-471d12b139a9",
                                 m_id_counter++,
                                 AssetsHandler({}));
    m_connection = new Connection(*(CommSocket*)0, *m_server,
                                  "8d18a4e8-f14f-4a46-997e-ada120d5438f",
                                  m_id_counter++);
    m_account = new Player(m_connection,
                          "6c9f3236-5de7-4ba4-8b7a-b0222df0af38",
                          "fa1a03a2-a745-4033-85cb-bb694e921e62",
                          m_id_counter++);
}

void Playertest::teardown()
{
    delete m_world;
    delete m_server;
    delete m_account;
    delete m_connection;
}

void Playertest::test_getType()
{
    ASSERT_TRUE(m_account != 0);

    const char * type = m_account->getType();
    ASSERT_EQUAL(std::string("player"), type);
}


int main()
{
    boost::asio::io_context io_context;
    EntityBuilder eb;
    NullPropertyManager propertyManager;
    Ruleset ruleset(eb, io_context, propertyManager);

    Playertest t;

    return t.run();
}

// stubs

#include "server/Connection.h"
#include "server/Juncture.h"
#include "server/Persistence.h"
#include "server/PossessionAuthenticator.h"

#include "common/globals.h"
#include "common/id.h"
#include "common/Inheritance.h"
#include "common/log.h"

#include <cstdlib>
#include <cstdio>

#include "../stubs/server/stubAccount.h"
#include "../stubs/server/stubConnection.h"
#include "../stubs/server/stubEntityBuilder.h"

#include "../stubs/server/stubRuleHandler.h"

#include "../stubs/server/stubEntityRuleHandler.h"
#include "../stubs/server/stubArchetypeRuleHandler.h"
#include "../stubs/server/stubOpRuleHandler.h"
#include "../stubs/server/stubPropertyRuleHandler.h"
#include "../stubs/server/stubConnectableRouter.h"
#include "../stubs/server/stubRuleset.h"
#include "../stubs/server/stubJuncture.h"
#include "../stubs/server/stubServerRouting.h"
#include "../stubs/server/stubLobby.h"
#include "../stubs/server/stubPossessionAuthenticator.h"

#include "../stubs/server/stubPersistence.h"
#include "../stubs/rules/simulation/stubThing.h"
#include "../stubs/rules/simulation/stubEntity.h"
#include "../stubs/rules/stubLocatedEntity.h"
#include "../stubs/common/stubLink.h"
#include "../stubs/common/stubTypeNode.h"
#include "../stubs/common/stubAssetsHandler.h"

#define STUB_Inheritance_getClass
const Atlas::Objects::Root& Inheritance::getClass(const std::string & parent, Visibility) const
{
    return noClass;
}


#define STUB_Inheritance_getType
const TypeNode* Inheritance::getType(const std::string & parent) const
{
    auto I = atlasObjects.find(parent);
    if (I == atlasObjects.end()) {
        return 0;
    }
    return I->second.get();
}

#define STUB_Inheritance_hasClass
bool Inheritance::hasClass(const std::string & parent)
{
    auto I = atlasObjects.find(parent);
    if (I == atlasObjects.end()) {
        return false;
    }
    return true;
}

#include "../stubs/common/stubInheritance.h"

#define STUB_Router_clientError
void Router::clientError(const Operation & op,
                         const std::string & errstring,
                         OpVector & res,
                         const std::string & to) const
{
    res.push_back(Atlas::Objects::Operation::Error());
}

#define STUB_Router_error
void Router::error(const Operation & op,
                   const std::string & errstring,
                   OpVector & res,
                   const std::string & to) const
{
    res.push_back(Atlas::Objects::Operation::Error());
}

#include "../stubs/common/stubRouter.h"
#include "../stubs/rules/simulation/stubBaseWorld.h"
#include "../stubs/rules/stubLocation.h"
#include "../stubs/common/stubProperty.h"
#include "../stubs/common/stubPropertyManager.h"

RouterId newId()
{
    long new_id = Playertest::newId();
    return {new_id};
}

#include "../stubs/common/stublog.h"

bool database_flag = false;

namespace Atlas { namespace Objects { namespace Operation {
int MONITOR_NO = -1;
} } }

#include <common/Shaker.h>

Shaker::Shaker()
{
}
std::string Shaker::generateSalt(size_t length)
{
    return "";
}
