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

// $Id$

#ifdef NDEBUG
#undef NDEBUG
#endif
#ifndef DEBUG
#define DEBUG
#endif

#include <Python.h>

#include "python_testers.h"

#include "client/Python_ClientAPI.h"

#include "rulesets/Python_API.h"

#include <cassert>

static bool stub_make_fail = false;
static bool stub_look_fail = false;
static bool stub_lookfor_fail = false;

int main()
{
    init_python_api();
    extend_client_python_api();

    run_python_string("import server");
    run_python_string("import atlas");
    fail_python_string("server.CreatorClient(1)");
    fail_python_string("server.CreatorClient(\"one\")");
    run_python_string("c=server.CreatorClient(\"1\")");
    run_python_string("c.as_entity()");
    fail_python_string("c.make()");
    fail_python_string("c.make('1')");
    run_python_string("c.make(atlas.Entity('1'))");
    stub_make_fail = true;
    fail_python_string("c.make(atlas.Entity('1'))");
    stub_make_fail = false;
    run_python_string("c.set('1', atlas.Entity('1'))");
    fail_python_string("c.set('1', 'not an entity')");
    fail_python_string("c.set(1, atlas.Entity('1'))");
    run_python_string("c.look('1')");
    stub_look_fail = true;
    fail_python_string("c.look('1')");
    stub_look_fail = false;
    fail_python_string("c.look(1)");
    run_python_string("e=c.look('1')");
    run_python_string("assert type(e) == server.LocatedEntity");
    run_python_string("c.look_for(atlas.Entity('1'))");
    stub_lookfor_fail = true;
    run_python_string("c.look_for(atlas.Entity('1'))");
    stub_lookfor_fail = false;
    fail_python_string("c.look_for('1')");
    run_python_string("c.send(atlas.Operation('info'))");
    fail_python_string("c.send('info')");
    fail_python_string("c.send()");
    run_python_string("c.delete('1')");
    fail_python_string("c.delete(1)");
    fail_python_string("c.delete()");
    run_python_string("c == server.CreatorClient(\"2\")");

    run_python_string("assert type(c.map) == server.Map");
    run_python_string("assert type(c.location) == atlas.Location");
    run_python_string("assert type(c.time) == server.WorldTime");
    fail_python_string("c.foo");
    fail_python_string("c.foo_operation");
    run_python_string("c.foo = 1");
    run_python_string("assert c.foo == 1");
    fail_python_string("c.foo = [1,2]");
    fail_python_string("c.map = 1");

    shutdown_python_api();
    return 0;
}

// stubs

#include "client/ObserverClient.h"
#include "client/CreatorClient.h"

#include "rulesets/Entity.h"

#include "common/id.h"

#include <Atlas/Objects/Operation.h>
#include <Atlas/Objects/RootEntity.h>

using Atlas::Objects::Entity::RootEntity;

LocatedEntity * CharacterClient::look(const std::string & id)
{
    if (stub_look_fail) {
        return 0;
    }
    return new Entity(id, integerId(id));
}

LocatedEntity * CharacterClient::lookFor(const RootEntity & entity)
{
    if (stub_lookfor_fail) {
        return 0;
    }
    return new Entity(entity->getId(), integerId(entity->getId()));
}

LocatedEntity * CreatorClient::make(const RootEntity & entity)
{
    if (stub_make_fail) {
        return 0;
    }
    return new Entity(entity->getId(), integerId(entity->getId()));
}

CreatorClient::CreatorClient(const std::string & id, long intId,
                             ClientConnection &c) :
               CharacterClient(id, intId, c)
{
}

void CreatorClient::sendSet(const std::string & id,
                            const RootEntity & entity)
{
}

void CreatorClient::del(const std::string & id)
{
}

CharacterClient::CharacterClient(const std::string & id, long intId,
                                 ClientConnection & c) :
                 BaseMind(id, intId), m_connection(c)
{
}

void CharacterClient::send(const Operation & op)
{
}

ObserverClient::ObserverClient()
{
}

ObserverClient::~ObserverClient()
{
}

int ObserverClient::setup(const std::string & account,
                          const std::string & password,
                          const std::string & avatar)
{
    return 0;
}

void ObserverClient::idle()
{
}

BaseClient::BaseClient() : m_character(0)
{
}

BaseClient::~BaseClient()
{
}

Atlas::Objects::Root BaseClient::createSystemAccount()
{
    return Atlas::Objects::Operation::Info();
}

Atlas::Objects::Root BaseClient::createAccount(const std::string & name,
                                               const std::string & password)
{
    return Atlas::Objects::Operation::Info();
}

void BaseClient::send(const Operation & op)
{
}

CreatorClient * BaseClient::createCharacter(const std::string & type)
{
    return 0;
}

ClientConnection::ClientConnection()
{
}

ClientConnection::~ClientConnection()
{
}

int ClientConnection::wait()
{
    return 0;
}

int ClientConnection::sendAndWaitReply(const Operation & op, OpVector & res)
{
    return 0;
}

void ClientConnection::operation(const Operation & op)
{
}
