// Cyphesis Online RPG Server and AI Engine
// Copyright (C) 2007 Alistair Riddoch
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

// $Id: LocatedEntitytest.cpp,v 1.2 2008-01-14 16:10:19 alriddoch Exp $

#include "rulesets/LocatedEntity.h"

#include <cassert>

using Atlas::Message::MapType;
using Atlas::Message::ListType;

class LocatedEntityTest : public LocatedEntity {
  public:
    LocatedEntityTest(const std::string & id, int iid) : LocatedEntity(id, iid) { }

    virtual void operation(const Operation &, OpVector &) { /* REMOVE */ }
};

int main()
{
    // Test constructor
    {
        new LocatedEntityTest("1", 1);
    }

    // Test destructor
    {
        LocatedEntityTest * e = new LocatedEntityTest("1", 1);
        delete e;
    }

    // Test checkRef()
    {
        LocatedEntityTest * e = new LocatedEntityTest("1", 1);

        assert(e->checkRef() == 0);
        delete e;
    }

    // Test incRef()
    {
        LocatedEntityTest * e = new LocatedEntityTest("1", 1);

        e->incRef();
        assert(e->checkRef() == 1);
        delete e;
    }

    // Test decRef()
    {
        LocatedEntityTest * e = new LocatedEntityTest("1", 1);

        e->incRef();
        e->decRef();
        assert(e->checkRef() == 0);
        delete e;
    }

    // Test decRef()
    {
        LocatedEntityTest * e = new LocatedEntityTest("1", 1);

        e->decRef();
        // Enitity deleted - verified as not leaked
    }

    // Test setAttr()
    {
        LocatedEntityTest * e = new LocatedEntityTest("1", 1);

        e->setAttr("foo", "bar");
        e->decRef();
    }
    
    // Test setAttr()
    {
        LocatedEntityTest * e = new LocatedEntityTest("1", 1);

        e->setAttr("foo", "bar");
        e->setAttr("foo", 23);
        e->decRef();
    }

    // Test getAttr()
    {
        LocatedEntityTest * e = new LocatedEntityTest("1", 1);
        Atlas::Message::Element val;

        bool ret = e->getAttr("foo", val);
        assert(ret == false);
        assert(val.isNone());
        e->decRef();
    }

    // Test getAttr()
    {
        LocatedEntityTest * e = new LocatedEntityTest("1", 1);
        Atlas::Message::Element val;

        e->setAttr("foo", "bar");
        bool ret = e->getAttr("foo", val);
        assert(ret == true);
        assert(val.isString());
        e->decRef();
    }

    // Test hasAttr()
    {
        LocatedEntityTest * e = new LocatedEntityTest("1", 1);
        Atlas::Message::Element val;

        bool ret = e->hasAttr("foo");
        assert(ret == false);
        e->decRef();
    }

    // Test hasAttr()
    {
        LocatedEntityTest * e = new LocatedEntityTest("1", 1);
        Atlas::Message::Element val;

        e->setAttr("foo", "bar");
        bool ret = e->hasAttr("foo");
        assert(ret == true);
        e->decRef();
    }


    return 0;
}
