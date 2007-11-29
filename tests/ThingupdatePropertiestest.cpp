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

// $Id: ThingupdatePropertiestest.cpp,v 1.2 2007-11-29 01:30:05 alriddoch Exp $

#include "allOperations.h"

#include "rulesets/Thing.h"

#include <Atlas/Objects/Anonymous.h>

#include <cassert>

using Atlas::Message::MapType;
using Atlas::Message::ListType;
using Atlas::Objects::smart_dynamic_cast;
using Atlas::Objects::Entity::Anonymous;
using Atlas::Objects::Entity::RootEntity;
using Atlas::Objects::Operation::Update;

class testThing : public Thing {
  public:
    testThing(const std::string & id, long intId) : Thing(id, intId) { }
    using Thing::updateProperties;
};

int main()
{
    // FIXME This test was written when there was a hard coded name
    // property, which is no longer there. Needs to be re-written.
#if 0
    // Test to ensure that Update op triggers a Sight(Set) listing at
    // least the attributes directed in the argument of the Update op,
    // but the Update op does not cause the attributes to be modified.
    {
        static const std::string testName("bob");
        static const std::string testNewName("fred");

        assert(testName != testNewName);

        testThing e("1", 1);
        e.setName(testName);
        assert(e.getName() == testName);

        Anonymous update_arg;
        update_arg->setName(testNewName);

        Update u;
        u->setArgs1(update_arg);

        OpVector res;
       
        e.updateProperties(u, res);

        // The update operation should not have actually changed the name
        // at all
        assert(e.getName() == testName);

        // The result should be a Sight op
        assert(res.size() == 1);

        const Operation & result = res.front();

        assert(result->getClassNo() == Atlas::Objects::Operation::SIGHT_NO);

        // The result argument should be a Set op
        assert(result->getArgs().size() == 1);

        const Operation & result_inner = smart_dynamic_cast<Operation>(result->getArgs().front());

        assert(result_inner.isValid());

        assert(result_inner->getClassNo() == Atlas::Objects::Operation::SET_NO);
        assert(result_inner->getArgs().size() == 1);

        const RootEntity & set_arg = smart_dynamic_cast<RootEntity>(result_inner->getArgs().front());

        // Make sure the name on the sight set argument has been set to the
        // name of the entity, not the name in the Update op.
        assert(!set_arg->isDefaultName());
        assert(set_arg->getName() == testName);
    }
#endif // 0

    return 0;
}
