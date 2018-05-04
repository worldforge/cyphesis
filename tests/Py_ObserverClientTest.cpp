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

#include "python_testers.h"

#include "client/Python_ClientAPI.h"

#include "rulesets/Python_API.h"

#include <cassert>

static bool stub_setup_fail = false;
static bool stub_createCharacter_fail = false;
static bool stub_send_wait_fail = false;
static int stub_send_wait_results = 0;
static bool stub_wait_fail = false;

int main()
{
    init_python_api("230dabbb-d676-4d43-8a03-4623c02503b5");
    extend_client_python_api();

    run_python_string("import atlas");
    run_python_string("import server");
    run_python_string("import types");
    run_python_string("o=server.ObserverClient()");
    run_python_string("o.setup()");
    expect_python_error("o.setup('bob')", PyExc_TypeError);
    run_python_string("o.setup('bob', 'jim')");
    run_python_string("o.setup('bob', 'jim', 'settler')");
    stub_setup_fail = true;
    expect_python_error("o.setup('bob', 'jim', 'settler')",
                        PyExc_RuntimeError);
    stub_setup_fail = false;
    run_python_string("o.create_avatar('settler')");
    expect_python_error("o.create_avatar(1)", PyExc_TypeError);
    run_python_string("o.run()");
    expect_python_error("o.send()", PyExc_TypeError);
    expect_python_error("o.send('get')", PyExc_TypeError);
    run_python_string("o.send(atlas.Operation('get'))");
    expect_python_error("o.send_wait()", PyExc_TypeError);
    expect_python_error("o.send_wait('get')", PyExc_TypeError);
    run_python_string("o.send_wait(atlas.Operation('get'))");
    stub_send_wait_results = 1;
    run_python_string("assert type(o.send_wait(atlas.Operation('get'))) == atlas.Operation");
    stub_send_wait_results = 2;
    run_python_string("assert type(o.send_wait(atlas.Operation('get'))) == atlas.Oplist");
    run_python_string("assert len(o.send_wait(atlas.Operation('get'))) == 2");
    stub_send_wait_fail = true;
    // FIXME This really should fail
    // expect_python_error("o.send_wait(atlas.Operation('get'))",
    //                     PyExc_AssertionError);
    run_python_string("o.wait()");
    stub_wait_fail = true;
    expect_python_error("o.wait()", PyExc_RuntimeError);
    run_python_string("assert type(o.id) == str");
    run_python_string("o.character");
    run_python_string("o.server = 'foo'");
    expect_python_error("o.server = 23", PyExc_TypeError);

    run_python_string("o == server.ObserverClient()");
    

    shutdown_python_api();
    return 0;
}

// stubs

#include "client/ObserverClient.h"
#include "client/CreatorClient.h"

#include <Atlas/Objects/Operation.h>

using Atlas::Objects::Entity::RootEntity;

CharacterClient::CharacterClient(const std::string & id, long intId,
                                 ClientConnection & c) :
                 BaseMind(id, intId), m_connection(c)
{
}

LocatedEntity * CharacterClient::look(const std::string & id)
{
    return 0;
}

LocatedEntity * CharacterClient::lookFor(const RootEntity & ent)
{
    return 0;
}

void CharacterClient::send(const Operation & op)
{
}

CreatorClient::CreatorClient(const std::string & id, long intId,
                             ClientConnection &c) :
               CharacterClient(id, intId, c)
{
}

LocatedEntity * CreatorClient::make(const RootEntity & entity)
{
    return 0;
}

void CreatorClient::sendSet(const std::string & id,
                            const RootEntity & entity)
{
}

void CreatorClient::del(const std::string & id)
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
    if (stub_setup_fail) {
        return -1;
    }
    return 0;
}

int ObserverClient::teardown()
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
    if (stub_createCharacter_fail) {
        return 0;
    }
    return new CreatorClient("1", 1, *new ClientConnection);
}

ClientConnection::ClientConnection()
{
}

ClientConnection::~ClientConnection()
{
}

int ClientConnection::wait()
{
    if (stub_wait_fail) {
        return -1;
    }
    return 0;
}

int ClientConnection::sendAndWaitReply(const Operation & op, OpVector & res)
{
    if (stub_send_wait_fail) {
        return -1;
    }
    for (int i = 0; i < stub_send_wait_results; ++i) {
        res.push_back(Atlas::Objects::Operation::Info());
    }
    return 0;
}

void ClientConnection::operation(const Operation & op)
{
}
