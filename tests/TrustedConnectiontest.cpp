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

#include "null_stream.h"

#include "server/TrustedConnection.h"

#include "server/Account.h"
#include "rulesets/ExternalMind.h"
#include "rulesets/ExternalProperty.h"
#include "server/Lobby.h"
#include "server/Player.h"
#include "server/ServerRouting.h"
#include "server/WorldRouter.h"
#include "server/SystemAccount.h"

#include "rulesets/Character.h"

#include "common/compose.hpp"
#include "common/Inheritance.h"
#include "common/SystemTime.h"
#include "common/log.h"
#include "common/CommSocket.h"

#include <Atlas/Objects/Anonymous.h>
#include <Atlas/Objects/Operation.h>
#include <Atlas/Objects/SmartPtr.h>

#include <cstdlib>
#include <cstdio>

#include <cassert>

using Atlas::Message::ListType;
using Atlas::Message::MapType;
using Atlas::Objects::Root;
using Atlas::Objects::Entity::RootEntity;
using Atlas::Objects::Entity::Anonymous;
using Atlas::Objects::Operation::Create;
using Atlas::Objects::Operation::Get;
using Atlas::Objects::Operation::Login;
using Atlas::Objects::Operation::Logout;
using Atlas::Objects::Operation::Move;

class TestCommSocket : public CommSocket
{
  public:
    TestCommSocket() : CommSocket(*(boost::asio::io_service*)0)
    {
    }

    virtual void disconnect()
    {
    }

    virtual int flush()
    {
        return 0;
    }

};

class TestConnection : public TrustedConnection {
  public:
    TestConnection(CommSocket & cc, ServerRouting & svr,
                   const std::string & addr, const std::string & id, long iid) :
        TrustedConnection(cc, svr, addr, id, iid) {
      
    }

    Account * test_newAccount(const std::string & type,
                              const std::string & username,
                              const std::string & passwd,
                              const std::string & id, long intId)
    {
        return newAccount(type, username, passwd, id, intId);
    }

    int test_verifyCredentials(const Account & ac,
                               const Atlas::Objects::Root & arg) const
    {
        return verifyCredentials(ac, arg);
    }

    size_t numObjects() const {
        return m_objects.size();
    }

    const RouterMap & getObjects() const {
        return m_objects;
    }

    void removeObject(Router * obj) {
        RouterMap::iterator I = m_objects.find(obj->getIntId());
        if (I != m_objects.end()) {
            m_objects.erase(I);
        }
    }
};

int main()
{
    // WorldRouter world(SystemTime());
    // Entity & e = world.m_gameWorld;

    ServerRouting server(*(BaseWorld*)0, "noruleset", "unittesting",
                         "1", 1, "2", 2);

    TestCommSocket * tcc = new TestCommSocket();
    TestConnection * tc = new TestConnection(*tcc, server, "addr", "3", 3);

    {
        Account * ac = tc->test_newAccount("_non_type_",
                                           "bob",
                                           "unit_test_hash",
                                           "1", 1);

        assert(ac != 0);
    }
    
    {
        Account * ac = tc->test_newAccount("sys",
                                           "bob",
                                           "unit_test_hash",
                                           "1", 1);

        assert(ac != 0);
    }
    
    {
        Account * ac = tc->test_newAccount("admin",
                                           "bob",
                                           "unit_test_hash",
                                           "1", 1);

        assert(ac != 0);
    }
    
    {
        Account * ac = tc->test_newAccount("player",
                                           "bob",
                                           "unit_test_hash",
                                           "1", 1);

        assert(ac != 0);
    }

    {
        Account * ac = new Player(0, "bill", "unit_test_password", "2", 2);
        Atlas::Objects::Root creds;

        int ret = tc->test_verifyCredentials(*ac, creds);

        assert(ret == 0);
    }
    

    delete tc;
}

// Stubs

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

#include "stubs/server/stubAdmin.h"
#include "stubs/server/stubPlayer.h"
#include "stubs/server/stubSystemAccount.h"
#include "stubs/server/stubAccount.h"

ConnectableRouter::ConnectableRouter(const std::string & id,
                                 long iid,
                                 Connection *c) :
                 Router(id, iid),
                 m_connection(c)
{
}

ConnectableRouter::~ConnectableRouter()
{
}

#include "stubs/server/stubConnection.h"
#include "stubs/server/stubServerRouting.h"
#include "stubs/server/stubLobby.h"



ExternalMind::ExternalMind(LocatedEntity & e) : Router(e.getId(), e.getIntId()),
                                         m_external(0), m_entity(e)
{
}

ExternalMind::~ExternalMind()
{
}

void ExternalMind::externalOperation(const Operation &, Link &)
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
    assert(m_external != 0);
    return m_external->getId();
}

void ExternalMind::linkUp(Link * c)
{
    m_external = c;
}

#include "stubs/rulesets/stubCharacter.h"
#include "stubs/rulesets/stubThing.h"
#include "stubs/rulesets/stubEntity.h"
#include "stubs/rulesets/stubLocatedEntity.h"


Link::Link(CommSocket & socket, const std::string & id, long iid) :
            Router(id, iid), m_encoder(0), m_commSocket(socket)
{
}

Link::~Link()
{
}

void Link::send(const Operation & op) const
{
}

void Link::disconnect()
{
}

#include "stubs/common/stubRouter.h"
#include "stubs/common/stubTypeNode.h"
#include "stubs/modules/stubLocation.h"
#include "common/Property_impl.h"
#include "stubs/common/stubProperty.h"
#include "stubs/common/stubBaseWorld.h"

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

const TypeNode * Inheritance::getType(const std::string & parent)
{
    TypeNodeDict::const_iterator I = atlasObjects.find(parent);
    if (I == atlasObjects.end()) {
        return 0;
    }
    return I->second;
}

const Root & Inheritance::getClass(const std::string & parent)
{
    return noClass;
}

void encrypt_password(const std::string & pwd, std::string & hash)
{
}

void log(LogLevel lvl, const std::string & msg)
{
}

void logEvent(LogEvent lev, const std::string & msg)
{
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

#include <common/Shaker.h>

Shaker::Shaker()
{
}

std::string Shaker::generateSalt(size_t length)
{
return "";
}
