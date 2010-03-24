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

#include "common/Router.h"

#include "common/log.h"

#include <Atlas/Objects/Anonymous.h>
#include <Atlas/Objects/Operation.h>
#include <Atlas/Objects/SmartPtr.h>

#include <cassert>

class TestRouter : public Router {
  public:
    TestRouter(const std::string & id, long intId) : Router(id, intId) { }

    virtual void operation(const Operation &, OpVector &) { }
};

int main()
{
    TestRouter test_obj("1", 1);

    {
        OpVector res;
        Atlas::Objects::Operation::Get op;

        test_obj.error(op, "test failure", res);
        test_obj.error(op, "test failure", res, "1");

        op->setSerialno(23);

        test_obj.error(op, "test failure", res);
        test_obj.error(op, "test failure", res, "1");

    }

    {
        OpVector res;
        Atlas::Objects::Operation::Get op;

        test_obj.clientError(op, "test failure", res);
        test_obj.clientError(op, "test failure", res, "1");

        op->setSerialno(17);

        test_obj.clientError(op, "test failure", res);
        test_obj.clientError(op, "test failure", res, "1");
    }

    {
        Atlas::Message::MapType msg;

        test_obj.addToMessage(msg);
    }

    {
        Atlas::Objects::Entity::Anonymous ent;

        test_obj.addToEntity(ent);
    }

    return 0;
}

// stubs

void log(LogLevel lvl, const std::string & msg)
{
}
