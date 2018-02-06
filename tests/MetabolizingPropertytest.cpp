// Copyright (C) 2015 Piotr Stępnicki
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

#include "PropertyCoverage.h"


#include "rulesets/MetabolizingProperty.h"



int main()
{
    MetabolizingProperty * ap = new MetabolizingProperty;

    PropertyChecker<MetabolizingProperty> pc(ap);

    pc.basicCoverage();

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

// Peter: this is the part I don't really understand
// why we assign those values to operations?

namespace Atlas { namespace Objects { namespace Operation {
int TICK_NO = -1;
int UPDATE_NO = -1;
} } }

#include "rulesets/StatusProperty.h"
#include "rulesets/AreaProperty.h"
#include "rulesets/BBoxProperty.h"

//#include "stubs/common/stubProperty.h"
#include "stubs/rulesets/stubStatusProperty.h"
#include "stubs/rulesets/stubAreaProperty.h"
#include "stubs/rulesets/stubBBoxProperty.h"
