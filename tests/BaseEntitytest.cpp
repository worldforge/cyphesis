// Cyphesis Online RPG Server and AI Engine
// Copyright (C) 2006 Alistair Riddoch
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

// $Id: BaseEntitytest.cpp,v 1.12 2008-01-29 01:48:52 alriddoch Exp $

#error This file has been removed from the build
#ifndef DOXYGEN_SHOULD_SKIP_THIS

#include "EntityExerciser.h"
#include "allOperations.h"

#include "common/BaseEntity.h"

#include <cassert>

class BaseEntityTest : public BaseEntity {
  public:
    BaseEntityTest(const std::string & id, int iid) : BaseEntity(id, iid) { }

    virtual void operation(const Operation &, OpVector &) { }
};

int main()
{
    {
        // Test constructor
        BaseEntityTest e1("1", 1);
    }

    {
        // Test destructor
        BaseEntity * e1 = new BaseEntityTest("1", 1);

        delete e1;
    }

    {
        // Test string ID
        BaseEntityTest e1("1", 1);

        assert(e1.getId() == "1");
    }

    {
        // Test integer ID
        BaseEntityTest e1("1", 1);

        assert(e1.getIntId() == 1);
    }

    {
        Atlas::Objects::Operation::RootOperation op;
        OpVector res;
        BaseEntityTest e1("1", 1);

        assert(res.empty());
        e1.error(op, "You got an error", res);
        assert(res.size() == 1);
        Atlas::Objects::Operation::RootOperation result = res.front();
        assert(result->isDefaultTo());
        assert(result->isDefaultRefno());
    }

    {
        Atlas::Objects::Operation::RootOperation op;
        OpVector res;
        BaseEntityTest e1("1", 1);

        assert(res.empty());
        e1.error(op, "You got an error", res, "foo");
        assert(res.size() == 1);
        Atlas::Objects::Operation::RootOperation result = res.front();
        assert(!result->isDefaultTo());
        assert(result->getTo() == "foo");
        assert(result->isDefaultRefno());
    }

    {
        Atlas::Objects::Operation::RootOperation op;
        OpVector res;
        BaseEntityTest e1("1", 1);

        op->setSerialno(23);
        assert(!op->isDefaultSerialno());

        assert(res.empty());
        e1.error(op, "You got an error", res);
        assert(res.size() == 1);
        Atlas::Objects::Operation::RootOperation result = res.front();
        assert(result->isDefaultTo());
        assert(result->isDefaultRefno());
    }

    {
        Atlas::Objects::Operation::RootOperation op;
        OpVector res;
        BaseEntityTest e1("1", 1);

        op->setSerialno(23);
        assert(!op->isDefaultSerialno());

        assert(res.empty());
        e1.error(op, "You got an error", res, "foo");
        assert(res.size() == 1);
        Atlas::Objects::Operation::RootOperation result = res.front();
        assert(!result->isDefaultTo());
        assert(result->getTo() == "foo");
        assert(!result->isDefaultRefno());
    }

    {
        Atlas::Objects::Operation::RootOperation op;
        OpVector res;
        BaseEntityTest e1("1", 1);

        assert(res.empty());
        e1.clientError(op, "You got an error", res);
        assert(res.size() == 1);
        Atlas::Objects::Operation::RootOperation result = res.front();
        assert(result->isDefaultTo());
        assert(result->isDefaultRefno());
    }

    {
        Atlas::Objects::Operation::RootOperation op;
        OpVector res;
        BaseEntityTest e1("1", 1);

        assert(res.empty());
        e1.clientError(op, "You got an error", res, "foo");
        assert(res.size() == 1);
        Atlas::Objects::Operation::RootOperation result = res.front();
        assert(!result->isDefaultTo());
        assert(result->getTo() == "foo");
        assert(result->isDefaultRefno());
    }

    {
        Atlas::Objects::Operation::RootOperation op;
        OpVector res;
        BaseEntityTest e1("1", 1);

        op->setSerialno(23);
        assert(!op->isDefaultSerialno());

        assert(res.empty());
        e1.clientError(op, "You got an error", res);
        assert(res.size() == 1);
        Atlas::Objects::Operation::RootOperation result = res.front();
        assert(result->isDefaultTo());
        assert(result->isDefaultRefno());
    }

    {
        Atlas::Objects::Operation::RootOperation op;
        OpVector res;
        BaseEntityTest e1("1", 1);

        op->setSerialno(23);
        assert(!op->isDefaultSerialno());

        assert(res.empty());
        e1.clientError(op, "You got an error", res, "foo");
        assert(res.size() == 1);
        Atlas::Objects::Operation::RootOperation result = res.front();
        assert(!result->isDefaultTo());
        assert(result->getTo() == "foo");
        assert(!result->isDefaultRefno());
    }

    BaseEntityTest be("1", 1);

    EntityExerciser<BaseEntity> ee(be);

    ee.runOperations();
    ee.runConversions();

    std::set<std::string> opNames;
    ee.addAllOperations(opNames);

    ee.runOperations();

    return 0;
}
