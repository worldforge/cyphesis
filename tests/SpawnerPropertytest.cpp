// Cyphesis Online RPG Server and AI Engine
// Copyright (C) 2013 Erik Ogenvik
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

#include "rulesets/SpawnerProperty.h"
#include "common/Inheritance.h"

int main()
{
    SpawnerProperty * ap = new SpawnerProperty;

    PropertyChecker<SpawnerProperty> pc(ap);

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


// stubs

Inheritance& Inheritance::instance() {
    return *(Inheritance*)(nullptr);
}

const TypeNode * Inheritance::getType(const std::string & parent)
{
    return nullptr;
}

void addToEntity(const Point3D & p, std::vector<double> & vd)
{
}


namespace Atlas { namespace Objects { namespace Operation {
int TICK_NO = -1;
} } }
