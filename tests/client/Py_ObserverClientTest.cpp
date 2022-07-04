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
#include "client/cyclient/ObserverClient.h"
#include "client/cyclient/CyPy_ObserverClient.h"

#include "pythonbase/Python_API.h"
#include "rules/SimpleTypeStore.h"
#include "../NullPropertyManager.h"

#include <cassert>
#include <rules/simulation/python/CyPy_Server.h>
#include <rules/python/CyPy_Atlas.h>
#include <common/Inheritance.h>
#include "pythonbase/PythonMalloc.h"

Atlas::Objects::Factories factories;

static bool stub_setup_fail = false;
static bool stub_createCharacter_fail = false;
static bool stub_send_wait_fail = false;
static int stub_send_wait_results = 0;
static bool stub_wait_fail = false;

int main()
{
    setupPythonMalloc();
    {
        NullPropertyManager propertyManager;
        SimpleTypeStore typeStore(propertyManager);

        Inheritance inheritance(factories);
        boost::asio::io_context io_context;

        init_python_api({&CyPy_Server::init, &CyPy_Atlas::init});
        extend_client_python_api();

        ObserverClient client(io_context, factories, typeStore);

        Py::Module module("server");
        module.setAttr("testclient", CyPy_ObserverClient::wrap(&client));

        run_python_string("import atlas");
        run_python_string("import server");
        run_python_string("import types");
        run_python_string("o=server.testclient");
        run_python_string("o.setup()");
        expect_python_error("o.setup('bob')", PyExc_IndexError);
        run_python_string("o.setup('bob', 'jim')");
        run_python_string("o.setup('bob', 'jim', 'settler')");
        stub_setup_fail = true;
        expect_python_error("o.setup('bob', 'jim', 'settler')",
                            PyExc_RuntimeError);
        stub_setup_fail = false;
        run_python_string("o.create_avatar('settler')");
        expect_python_error("o.create_avatar(1)", PyExc_TypeError);
        run_python_string("o.run()");
        expect_python_error("o.send()", PyExc_IndexError);
        expect_python_error("o.send('get')", PyExc_TypeError);
        run_python_string("o.send(atlas.Operation('get'))");
        expect_python_error("o.send_wait()", PyExc_IndexError);
        expect_python_error("o.send_wait('get')", PyExc_TypeError);
        run_python_string("o.send_wait(atlas.Operation('get'))");
        stub_send_wait_results = 1;
        run_python_string("assert type(o.send_wait(atlas.Operation('get'))) == atlas.Operation");
        stub_send_wait_results = 2;
        run_python_string("assert type(o.send_wait(atlas.Operation('get'))) == atlas.Operation");
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
    }
    shutdown_python_api();
    return 0;
}

// stubs

#include "client/cyclient/ObserverClient.h"
#include "client/cyclient/CreatorClient.h"

#include <Atlas/Objects/Operation.h>

using Atlas::Objects::Entity::RootEntity;

#include "../stubs/client/cyclient/stubCharacterClient.h"
#include "../stubs/client/cyclient/stubCreatorClient.h"

#define STUB_ObserverClient_setup
int ObserverClient::setup(const std::string & account , const std::string & password , const std::string & avatar )
{
    if (stub_setup_fail) {
        return -1;
    }
    return 0;
}
#include "../stubs/client/cyclient/stubObserverClient.h"

#define STUB_BaseClient_createCharacter
Ref<CreatorClient> BaseClient::createCharacter(const std::string & type)
{
    if (stub_createCharacter_fail) {
        return 0;
    }
    return Ref<CreatorClient>(new CreatorClient(1, "2", m_connection, m_typeStore));
}
#include "../stubs/client/cyclient/stubBaseClient.h"





#define STUB_ClientConnection_wait
int ClientConnection::wait()
{
    if (stub_wait_fail) {
        return -1;
    }
    return 0;
}

#define STUB_ClientConnection_sendAndWaitReply
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

#include "../stubs/client/cyclient/stubClientConnection.h"

