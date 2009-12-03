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

#include "client/ClientConnection.h"

#include <Atlas/Objects/Operation.h>
#include <Atlas/Objects/Anonymous.h>

#include <cassert>

using Atlas::Objects::Root;
using Atlas::Objects::Entity::Anonymous;
using Atlas::Objects::Operation::Error;
using Atlas::Objects::Operation::Info;
using Atlas::Objects::Operation::RootOperation;

class TestClientConnection : public ClientConnection {
  public:
    void test_operation(const RootOperation & op) {
        operation(op);
    }

    void test_objectArrived(const Root & obj) {
        objectArrived(obj);
    }

    void test_errorArrived(const RootOperation & op) {
        errorArrived(op);
    }

    void test_infoArrived(const RootOperation & op) {
        infoArrived(op);
    }

};

int main()
{
    ClientConnection * cc = new ClientConnection();
    delete cc;

    // Try all the method calls when not connected
    cc = new ClientConnection();

    cc->login("username", "password");
    cc->create("player", "username", "password");
    cc->wait();

    {
        Atlas::Objects::Operation::RootOperation op;
        cc->send(op);
    }

    cc->pop();
    cc->pending();
    
    delete cc;

    TestClientConnection * tcc = new TestClientConnection();

    {
        Atlas::Objects::Root obj;
        Atlas::Objects::Operation::RootOperation op;
        tcc->test_operation(op);
        tcc->test_errorArrived(op);
        tcc->test_infoArrived(op);
        tcc->test_objectArrived(op);

        Anonymous op_arg;
        op->setArgs1(op_arg);
        tcc->test_infoArrived(op);

        op->setFrom("1");
        tcc->test_infoArrived(op);

        op->setParents(std::list<std::string>());
        tcc->test_operation(op);
        tcc->test_objectArrived(op);

        Info i;
        tcc->test_objectArrived(i);

        Error e;
        tcc->test_objectArrived(e);

        tcc->test_objectArrived(obj);
        obj->setParents(std::list<std::string>());
        tcc->test_objectArrived(obj);

        
    }

    return 0;
}
