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

#include <Python.h>

#include "../python_testers.h"

#include "client/cyclient/Python_ClientAPI.h"

#include "pythonbase/Python_API.h"

#include <cassert>
#include <client/cyclient/CyPy_CreatorClient.h>
#include <client/cyclient/ClientConnection.h>
#include "rules/SimpleTypeStore.h"
#include <rules/simulation/Entity.h>
#include <rules/simulation/python/CyPy_Server.h>
#include <rules/python/CyPy_Atlas.h>
#include <rules/ai/python/CyPy_Ai.h>
#include <rules/python/CyPy_Common.h>
#include <rules/python/CyPy_Physics.h>
#include <rules/python/CyPy_Rules.h>
#include <common/Inheritance.h>
#include "pythonbase/PythonMalloc.h"
#include "pycxx/CXX/Objects.hxx"
#include "../NullPropertyManager.h"

namespace
{
Atlas::Objects::Factories factories;
Inheritance inheritance(factories);
}

static bool stub_make_fail = false;
static bool stub_look_fail = false;
static bool stub_lookfor_fail = false;

int main()
{
    setupPythonMalloc();
    {
        NullPropertyManager propertyManager;
        SimpleTypeStore typeStore(propertyManager);

        boost::asio::io_context io_context;

        init_python_api({&CyPy_Server::init,
                         &CyPy_Rules::init,
                         &CyPy_Atlas::init,
                         &CyPy_Physics::init,
                         &CyPy_Common::init,
                         &CyPy_Ai::init});
        extend_client_python_api();

        ClientConnection conn(io_context, factories);
        Ref<CreatorClient> client(new CreatorClient(1, "2", conn, typeStore));
        Ref<MemEntity> entity(new MemEntity(1));
        OpVector res;
        client->setOwnEntity(res, entity);

        Py::Module module("server");
        //Moves ownership to Python.
        module.setAttr("testclient", CyPy_CreatorClient::wrap(std::move(client)));

        run_python_string("import server");
        run_python_string("import ai");
        run_python_string("import rules");
        run_python_string("import atlas");
        expect_python_error("server.CreatorClient(1)", PyExc_RuntimeError);
        expect_python_error("server.CreatorClient(\"one\")", PyExc_RuntimeError);
        run_python_string("c=server.testclient");
        run_python_string("c.as_entity()");
        expect_python_error("c.make()", PyExc_IndexError);
        expect_python_error("c.make('1')", PyExc_TypeError);
        run_python_string("c.make(atlas.Entity('1'))");
        stub_make_fail = true;
        expect_python_error("c.make(atlas.Entity('1'))", PyExc_RuntimeError);
        stub_make_fail = false;
        run_python_string("c.set('1', atlas.Entity('1'))");
        expect_python_error("c.set('1', 'not an entity')", PyExc_TypeError);
        expect_python_error("c.set(1, atlas.Entity('1'))", PyExc_TypeError);
        run_python_string("c.look('1')");
        stub_look_fail = true;
        expect_python_error("c.look('1')", PyExc_RuntimeError);
        stub_look_fail = false;
        expect_python_error("c.look(1)", PyExc_TypeError);
        run_python_string("e=c.look('1')");
        run_python_string("assert type(e) == server.Thing");
        run_python_string("c.look_for(atlas.Entity('1'))");
        stub_lookfor_fail = true;
        run_python_string("c.look_for(atlas.Entity('1'))");
        stub_lookfor_fail = false;
        expect_python_error("c.look_for('1')", PyExc_TypeError);
        run_python_string("c.send(atlas.Operation('info'))");
        expect_python_error("c.send('info')", PyExc_TypeError);
        expect_python_error("c.send()", PyExc_IndexError);
        run_python_string("c.delete('1')");
        expect_python_error("c.delete(1)", PyExc_TypeError);
        expect_python_error("c.delete()", PyExc_IndexError);
        run_python_string("assert c == server.testclient");

        run_python_string("assert type(c.map) == ai.MemMap");
        expect_python_error("c.foo", PyExc_AttributeError);
        expect_python_error("c.foo_operation", PyExc_AttributeError);
        run_python_string("c.foo = 1");
        run_python_string("assert c.foo == 1");
        run_python_string("c.foo = [1,2]");
        expect_python_error("c.map = 1", PyExc_AttributeError);
    }
    shutdown_python_api();
    return 0;
}

// stubs

#include "client/cyclient/ObserverClient.h"
#include "client/cyclient/CreatorClient.h"

#include "rules/simulation/Entity.h"

#include "common/id.h"

#include <Atlas/Objects/Operation.h>
#include <Atlas/Objects/RootEntity.h>

using Atlas::Objects::Entity::RootEntity;

#define STUB_CharacterClient_look

Ref<LocatedEntity> CharacterClient::look(const std::string& id)
{
    if (stub_look_fail) {
        return nullptr;
    }
    return Ref<LocatedEntity>(new Entity(RouterId(id)));
}

#define STUB_CharacterClient_lookFor

Ref<LocatedEntity> CharacterClient::lookFor(const RootEntity& entity)
{
    if (stub_lookfor_fail) {
        return nullptr;
    }
    return Ref<LocatedEntity>(new Entity(RouterId(entity->getId())));
}

#define STUB_CreatorClient_make

Ref<LocatedEntity> CreatorClient::make(const RootEntity& entity)
{
    if (stub_make_fail) {
        return nullptr;
    }
    return Ref<LocatedEntity>(new Entity(RouterId(entity->getId())));
}

#include "../stubs/client/cyclient/stubCreatorClient.h"
#include "../stubs/client/cyclient/stubCharacterClient.h"
#include "../stubs/client/cyclient/stubObserverClient.h"
#include "../stubs/client/cyclient/stubBaseClient.h"
#include "../stubs/client/cyclient/stubClientConnection.h"

