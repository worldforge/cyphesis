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

#include "../TestBase.h"

#include "server/Connection.h"
#include "server/ServerRouting.h"
#include "rules/simulation/ExternalMind.h"
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
                                 *(Persistence*)nullptr,
                                 "b88aa6d3-44b4-40bd-bfa9-8db00045bdc0",
                                 "0f1fc7cb-5ab1-45c1-b0d3-ae49205ea437",
                                 m_id_counter++,
                                 AssetsHandler({}));
    m_connection = new Connection(*(CommSocket*)0,
                                  *m_server,
                                  "test_addr",
                                  m_id_counter++);
}

void ConnectionShakerintegration::teardown()
{
    delete m_connection;
    delete m_server;
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
#include "../stubs/common/stubLink.h"
#include "../stubs/common/stubAssetsHandler.h"

using Atlas::Objects::Root;

bool restricted_flag;

namespace Atlas { namespace Objects { namespace Operation {
int UPDATE_NO = -1;
} } }



int CommSocket::flush()
{
    return 0;
}

#include "../stubs/server/stubPlayer.h"
#include "../stubs/server/stubAccount.h"
#include "../stubs/server/stubServerRouting.h"
#include "../stubs/server/stubLobby.h"
#include "../stubs/server/stubPersistence.h"

#define STUB_ExternalMind_connectionId
const std::string & ExternalMind::connectionId()
{
    assert(m_link != 0);
    return m_link->getId();
}

#define STUB_ExternalMind_linkUp
void ExternalMind::linkUp(Link * c)
{
    m_link = c;
}

#include "../stubs/rules/simulation/stubExternalMind.h"
#include "../stubs/rules/simulation/stubThing.h"
#include "../stubs/rules/simulation/stubEntity.h"
#include "../stubs/rules/stubLocatedEntity.h"
#include "../stubs/server/stubExternalMindsManager.h"
#include "../stubs/server/stubExternalMindsConnection.h"
#include "../stubs/common/stubOperationsDispatcher.h"
#include "../stubs/common/stubTypeNode.h"

#include "../stubs/common/stubRouter.h"
#include "../stubs/rules/stubLocation.h"
#include "../stubs/common/stubProperty.h"


#ifndef STUB_Inheritance_getClass
#define STUB_Inheritance_getClass
const Atlas::Objects::Root& Inheritance::getClass(const std::string & parent, Visibility) const
{
    return noClass;
}
#endif //STUB_Inheritance_getClass

#ifndef STUB_Inheritance_getType
#define STUB_Inheritance_getType
const TypeNode* Inheritance::getType(const std::string & parent) const
{
    auto I = atlasObjects.find(parent);
    if (I == atlasObjects.end()) {
        return 0;
    }
    return I->second.get();
}
#endif //STUB_Inheritance_getType

#include "../stubs/common/stubInheritance.h"
#include "../stubs/common/stublog.h"

void hash_password(const std::string & pwd, const std::string & salt,
                   std::string & hash )
{
    salt_length=salt.length();
}

#include "../stubs/common/stubid.h"

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
