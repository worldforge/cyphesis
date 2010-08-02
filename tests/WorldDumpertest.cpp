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

// $Id$

#include "tools/WorldDumper.h"

#include <Atlas/Objects/Anonymous.h>
#include <Atlas/Objects/Operation.h>
#include <Atlas/Objects/SmartPtr.h>

#include <cassert>

class TestWorldDumper : public WorldDumper
{
  public:
    TestWorldDumper(const std::string & id) :
        WorldDumper(id) { }

    int test_getNewSerialNo() {
        return ++m_lastSerialNo;
    }

    int test_getCount() {
        return m_count;
    }
};

int main()
{
    // FIXME for portability
    const char null_dev[] = "/dev/null";

    {
        WorldDumper wd("1");
    }

    {
        WorldDumper wd("1");

        OpVector res;
        wd.setup(null_dev, res);
        assert(res.size() == 1);
    }

    // Unrecognised op
    {
        WorldDumper wd("1");

        Atlas::Objects::Operation::RootOperation op;
        OpVector res;
        wd.operation(op, res);
    }

    // Info op
    {
        WorldDumper wd("1");

        Atlas::Objects::Operation::Info op;
        OpVector res;
        wd.operation(op, res);
    }

    // Info op wrong refno
    {
        TestWorldDumper wd("1");

        Atlas::Objects::Operation::Info op;
        op->setRefno(wd.test_getNewSerialNo() + 1);
        OpVector res;
        wd.operation(op, res);
    }

    // Info op correct refno, no args
    {
        TestWorldDumper wd("1");

        Atlas::Objects::Operation::Info op;
        op->setRefno(wd.test_getNewSerialNo());
        OpVector res;
        wd.operation(op, res);
    }

    // Info op correct refno, with non-entity args
    {
        TestWorldDumper wd("1");

        Atlas::Objects::Operation::Info op;
        op->setRefno(wd.test_getNewSerialNo());

        op->setArgs1(Atlas::Objects::Root());

        OpVector res;
        wd.operation(op, res);
    }

    // Info op correct refno, with operation args
    {
        TestWorldDumper wd("1");

        Atlas::Objects::Operation::Info op;
        op->setRefno(wd.test_getNewSerialNo());

        op->setArgs1(Atlas::Objects::Operation::RootOperation());

        OpVector res;
        wd.operation(op, res);
    }

    // Info op correct refno, with entity args and no contains
    {
        TestWorldDumper wd("1");

        OpVector res;
        wd.setup(null_dev, res);

        Atlas::Objects::Operation::Info op;
        op->setRefno(wd.test_getNewSerialNo());

        op->setArgs1(Atlas::Objects::Entity::Anonymous());

        int old_count = wd.test_getCount();
        assert(old_count == 0);

        res.clear();
        wd.operation(op, res);

        assert(wd.test_getCount() == (old_count + 1));
        assert(wd.isComplete());
        assert(res.empty());
    }

    // Info op correct refno, with entity args single ID in contains
    {
        TestWorldDumper wd("1");

        OpVector res;
        wd.setup(null_dev, res);

        Atlas::Objects::Operation::Info op;
        op->setRefno(wd.test_getNewSerialNo());

        Atlas::Objects::Entity::Anonymous ent;
        ent->setContains(std::list<std::string>(1, "2"));
        op->setArgs1(ent);

        int old_count = wd.test_getCount();
        assert(old_count == 0);

        res.clear();
        wd.operation(op, res);

        assert(wd.test_getCount() == (old_count + 1));
        assert(!wd.isComplete());
        assert(res.size() == 1);
    }

    // Info op correct refno, with entity args and 2 contains
    {
        TestWorldDumper wd("1");

        OpVector res;
        wd.setup(null_dev, res);

        Atlas::Objects::Operation::Info op;
        op->setRefno(wd.test_getNewSerialNo());

        Atlas::Objects::Entity::Anonymous ent;
        std::list<std::string> contains;
        contains.push_back("2");
        contains.push_back("3");
        ent->setContains(contains);
        op->setArgs1(ent);

        int old_count = wd.test_getCount();
        assert(old_count == 0);

        res.clear();
        wd.operation(op, res);

        assert(wd.test_getCount() == (old_count + 1));
        assert(!wd.isComplete());
        assert(res.size() == 1);
    }

    // Info op correct refno, with entity args and mal-ordered contains
    {
        TestWorldDumper wd("1");

        OpVector res;
        wd.setup(null_dev, res);

        Atlas::Objects::Operation::Info op;
        op->setRefno(wd.test_getNewSerialNo());

        Atlas::Objects::Entity::Anonymous ent;
        std::list<std::string> contains;
        contains.push_back("3");
        contains.push_back("2");
        ent->setContains(contains);
        op->setArgs1(ent);

        int old_count = wd.test_getCount();
        assert(old_count == 0);

        res.clear();
        wd.operation(op, res);

        assert(wd.test_getCount() == (old_count + 1));
        assert(!wd.isComplete());
        assert(res.size() == 1);

        // CHeck structure of Get is as expected
        Atlas::Objects::Operation::RootOperation r1 = res.front();
        assert(r1->getClassNo() == Atlas::Objects::Operation::GET_NO);
        assert(r1->getArgs().size() == 1);
        const Atlas::Objects::Root & r1_arg = r1->getArgs().front();
        assert(!r1_arg->isDefaultId());
        // and check numerical sort of contains worked
        assert(r1_arg->getId() == "2");
    }

    return 0;
}

// stubs

#include "tools/MultiLineListFormatter.h"

#include <iostream>

#include <cstdlib>

long integerId(const std::string & id)
{
    long intId = strtol(id.c_str(), 0, 10);
    if (intId == 0 && id != "0") {
        intId = -1L;
    }

    return intId;
}

ClientTask::ClientTask() : m_complete(false)
{
}

ClientTask::~ClientTask()
{
}

MultiLineListFormatter::MultiLineListFormatter(std::iostream & s,
                                               Atlas::Bridge & b) :
                       Atlas::Formatter(s,b)
{
}

void MultiLineListFormatter::mapListItem(const std::string & name)
{
    Atlas::Formatter::mapListItem(name);
}

void MultiLineListFormatter::listMapItem()
{
    Atlas::Formatter::listMapItem();
}

void MultiLineListFormatter::listListItem()
{
    Atlas::Formatter::listListItem();
}

void MultiLineListFormatter::listIntItem(long l)
{
    Atlas::Formatter::listIntItem(l);
}

void MultiLineListFormatter::listFloatItem(double d)
{
    Atlas::Formatter::listFloatItem(d);
}

void MultiLineListFormatter::listStringItem(const std::string&s)
{
    Atlas::Formatter::listStringItem(s);
}

void MultiLineListFormatter::listEnd()
{
    Atlas::Formatter::listEnd();
}
