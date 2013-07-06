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

#include "rulesets/AtlasProperties.h"

#include "rulesets/Entity.h"

#include "PropertyCoverage.h"

int main()
{
    {
        std::string id;
        IdProperty * ip = new IdProperty(id);

        PropertyChecker<IdProperty> pc(ip);

        // Coverage is complete, but it wouldn't hurt to add some bad data here.

        pc.basicCoverage();
    }

    {
        NameProperty * np = new NameProperty(0);

        PropertyCoverage pc(np);

        // Coverage is complete, but it wouldn't hurt to add some bad data here.

        pc.basicCoverage();
    }

    {
        LocatedEntitySet les;
        ContainsProperty * cp = new ContainsProperty(les);

        PropertyCoverage pc(cp);

        // Now the world context has been set up, add some entities.
        les.insert(new Entity("2", 2));

        // Coverage is complete, but it wouldn't hurt to add some bad data here.

        pc.basicCoverage();
    }



    return 0;
}

#include "TestWorld.h"

void TestWorld::message(const Operation & op, LocatedEntity & ent)
{
}

LocatedEntity * TestWorld::addNewEntity(const std::string &,
                                 const Atlas::Objects::Entity::RootEntity &)
{
    return 0;
}
