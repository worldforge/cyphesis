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

#include "rules/simulation/ExternalMind.h"

#include "server/Connection.h"

#include "rules/simulation/Entity.h"

#include "rules/simulation/BaseWorld.h"

#include <Atlas/Objects/Operation.h>

#include <cassert>

class TestExternalMind : public ExternalMind
{
  public:
    TestExternalMind(Ref<Entity> & e) : ExternalMind(0, e) { }

    void test_deleteEntity(const std::string & id) {
        deleteEntity(id, false);
    }

    void test_purgeEntity(const LocatedEntity & ent) {
        purgeEntity(ent);
    }

};

int stub_baseworld_receieved_op = -1;
int stub_link_send_op = -1;
int stub_link_send_count = 0;


#include "../TestWorld.h"



int main()
{
    TestWorld::extension.messageFn = [](const Operation & op, LocatedEntity & ent){stub_baseworld_receieved_op = op->getClassNo();};
    TestWorld world{};

    {
        Ref<Entity> e(new Entity(2));

        ExternalMind em(3, e);
    }

    {
        Ref<Entity> e(new Entity(2));

        TestExternalMind em(e);

        em.test_deleteEntity("3");
    }

    // Purge with empty contains
    {
        Ref<Entity> e(new Entity(2));
        e->m_contains.reset(new LocatedEntitySet);

        TestExternalMind em(e);

        em.test_purgeEntity(*e);
    }

    // Purge with populated contains
    {
        Ref<Entity> e(new Entity(2));
        e->m_contains.reset(new LocatedEntitySet);
        e->m_contains->insert(new Entity(3));

        TestExternalMind em(e);

        em.test_purgeEntity(*e);
    }

    // Connect to nothing
    {
        Ref<Entity> e(new Entity(2));

        ExternalMind em(3, e);

        em.linkUp(0);
    }

    // Connect to something
    {
        Ref<Entity> e(new Entity(2));

        ExternalMind em(3, e);

        Connection conn(*(CommSocket*)0,
                        *(ServerRouting*)0,
                        "addr", 4);
        em.linkUp(&conn);
    }

    // Connect to something, then disconnect
    {
        Ref<Entity> e(new Entity(2));

        ExternalMind em(3, e);

        Connection conn(*(CommSocket*)0,
                        *(ServerRouting*)0,
                        "addr", 4);
        em.linkUp(&conn);
        em.linkUp(0);
    }

    // Connect to something, then check connection ID
    {
        Ref<Entity> e(new Entity(2));

        ExternalMind em(3, e);

        Connection conn(*(CommSocket*)0,
                        *(ServerRouting*)0,
                        "addr", 4);
        em.linkUp(&conn);
        const std::string & id = em.connectionId();
        assert(id == "4");
    }

    // Send a random operation
    {
        Ref<Entity> e(new Entity(2));

        TestExternalMind em(e);

        stub_baseworld_receieved_op = -1;
        OpVector res;
        em.operation(Atlas::Objects::Operation::RootOperation(), res);
        assert(stub_baseworld_receieved_op == -1);
    }

    // Send a Delete operation
    {
        Ref<Entity> e(new Entity(2));

        TestExternalMind em(e);

        stub_baseworld_receieved_op = -1;
        OpVector res;
        em.operation(Atlas::Objects::Operation::Delete(), res);
        assert(stub_baseworld_receieved_op == -1);
    }

    // Send a Delete operation to an ephemeral entity
    {
        Ref<Entity> e(new Entity(2));
        e->addFlags(entity_ephem);

        TestExternalMind em(e);

        stub_baseworld_receieved_op = -1;
        OpVector res;
        em.operation(Atlas::Objects::Operation::Delete(), res);
        assert(stub_baseworld_receieved_op == -1);
    }

    // Send a random operation to a connected mind, and make sure it's filtered out. Only Info ops are allowed.
    {
        Ref<Entity> e(new Entity(2));

        TestExternalMind em(e);

        Connection conn(*(CommSocket*)0,
                        *(ServerRouting*)0,
                        "addr", 4);
        em.linkUp(&conn);

        stub_link_send_op = -1;
        stub_link_send_count = 0;
        OpVector res;
        em.operation(Atlas::Objects::Operation::RootOperation(), res);
        assert(stub_link_send_op == -1);
        assert(stub_link_send_count == 0);
    }

    // Send an Info operation and an  to a connected mind, and make sure sent filtered out. Only Info ops are allowed.
    {
        Ref<Entity> e(new Entity(2));

        TestExternalMind em(e);

        Connection conn(*(CommSocket*)0,
                *(ServerRouting*)0,
                "addr", 4);
        em.linkUp(&conn);

        stub_link_send_op = -1;
        stub_link_send_count = 0;
        OpVector res;
        em.operation(Atlas::Objects::Operation::Info(), res);
        assert(stub_link_send_op == Atlas::Objects::Operation::INFO_NO);
        em.operation(Atlas::Objects::Operation::Sight(), res);
        assert(stub_link_send_op == Atlas::Objects::Operation::SIGHT_NO);
        assert(stub_link_send_count == 2);
    }

    // Send a Sight operation to a connected mind
    {
        Ref<Entity> e(new Entity(2));

        TestExternalMind em(e);

        Connection conn(*(CommSocket*)0,
                *(ServerRouting*)0,
                "addr", 4);
        em.linkUp(&conn);

        stub_link_send_op = -1;
        stub_link_send_count = 0;
        OpVector res;
        em.operation(Atlas::Objects::Operation::Sight(), res);
        assert(stub_link_send_op == Atlas::Objects::Operation::SIGHT_NO);
        assert(stub_link_send_count == 1);
    }


    return 0;
}

// stubs

#include "rules/Script.h"

#include "common/log.h"

using Atlas::Message::MapType;

#include "../stubs/server/stubConnection.h"
#include "../stubs/common/stubInheritance.h"
#include "../stubs/common/stubcustom.h"
#include "../stubs/common/stubTypeNode.h"

#define STUB_Entity_destroy
void Entity::destroy()
{
    destroyed.emit();
}

#define STUB_Entity_sendWorld
void Entity::sendWorld(Operation op)
{
    BaseWorld::instance().message(op, *this);
}


#include "../stubs/rules/simulation/stubEntity.h"



#include "../stubs/rules/stubLocatedEntity.h"
#include "../stubs/rules/stubScript.h"

#include "../stubs/rules/stubLocation.h"

#define STUB_Link_send
void Link::send(const Operation & op) const
{
    stub_link_send_op = op->getClassNo();
    ++stub_link_send_count;
}


#include "../stubs/common/stubLink.h"
#include "../stubs/common/stubRouter.h"
#include "../stubs/rules/simulation/stubBaseWorld.h"
#include "../stubs/common/stublog.h"

