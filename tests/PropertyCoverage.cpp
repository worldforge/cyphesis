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

#include "PropertyCoverage.h"
#include "TestWorld.h"

#include "rulesets/Entity.h"

#include <Atlas/Objects/Anonymous.h>
#include <Atlas/Objects/SmartPtr.h>

using Atlas::Message::Element;
using Atlas::Message::MapType;
using Atlas::Message::ListType;
using Atlas::Objects::Entity::Anonymous;

PropertyCoverage::PropertyCoverage(PropertyBase * pb) :
    prop(pb),
    tlve(new Entity("0", 0)),
    wrld(new TestWorld(*tlve)),
    ent(new Entity("1", 1))
{
    ent->m_location.m_loc = tlve;
    ent->m_location.m_pos = Point3D(1,0,0);

    tlve->m_contains = new LocatedEntitySet;
    tlve->m_contains->insert(ent);

    m_testData.push_back(23);
    m_testData.push_back(23.);
    m_testData.push_back("twenty_three");
    m_testData.push_back(ListType(1, 23));
    m_testData.push_back(ListType(1, 23.));
    m_testData.push_back(ListType(1, "twenty_three"));

    MapType map;
    map["one"] = 23;
    map["two"] = 23.;
    map["three"] = "twenty_three";
    map["four"] = ListType(1, 23);
    map["five"] = ListType(1, 23.);
    map["six"] = ListType(1, "twenty_three");

    m_testData.push_back(map);
}

PropertyCoverage::~PropertyCoverage()
{
    ent->m_location.m_loc = 0;
    delete ent;
    delete tlve;
    delete prop;
}

void PropertyCoverage::basicCoverage()
{
    Element val;

    // assert(pb->flags() == 4);

    prop->get(val);

    // FIXME cover all types inc. map
    ListType::const_iterator I = m_testData.begin();
    ListType::const_iterator Iend = m_testData.end();
    for (; I != Iend; ++I) {
        prop->set(*I);
    }

    prop->get(val);

    prop->install(ent);

    I = m_testData.begin();
    for (; I != Iend; ++I) {
        prop->set(*I);
        prop->apply(ent);
    }

    MapType map;
    prop->add("test_name", map);
    Anonymous ent;
    prop->add("test_name", ent);
}

void PropertyCoverage::testDataAppend(const Element & o)
{
    m_testData.push_back(o);
}
