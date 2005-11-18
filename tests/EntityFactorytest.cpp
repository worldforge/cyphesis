// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2005 Alistair Riddoch

#include "TestWorld.h"

#include "rulesets/World.h"

#include "server/EntityFactory.h"

#include <Atlas/Objects/Anonymous.h>

#include <cassert>

using Atlas::Message::MapType;
using Atlas::Objects::Entity::Anonymous;

int main()
{
    {
        World e("1", 1);
        TestWorld test_world(e);
        Anonymous attributes;

        EntityFactory::init(test_world);

        assert(EntityFactory::instance() != 0);

        assert(EntityFactory::instance()->newEntity("1", 1, "world", attributes) == 0);
        assert(EntityFactory::instance()->newEntity("1", 1, "thing", attributes) != 0);

        EntityFactory::del();
        assert(EntityFactory::instance() == 0);
    }

    {
        World e("1", 1);
        TestWorld test_world(e);
        Anonymous attributes;
        Atlas::Message::Element val;

        EntityFactory::init(test_world);

        assert(EntityFactory::instance() != 0);

        Entity * test_ent = EntityFactory::instance()->newEntity("1", 1, "thing", attributes);
        assert(test_ent != 0);
        assert(!test_ent->get("funky", val));
        assert(val.isNone());

        attributes->setAttr("funky", "true");

        test_ent = EntityFactory::instance()->newEntity("1", 1, "thing", attributes);
        assert(test_ent != 0);
        assert(test_ent->get("funky", val));
        assert(val.isString());
        assert(val.String() == "true");

        EntityFactory::del();
        assert(EntityFactory::instance() == 0);
    }
}
