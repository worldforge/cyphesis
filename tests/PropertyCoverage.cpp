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

#include "PropertyCoverage.h"
#include "TestWorld.h"

#include "rulesets/Entity.h"
#include "rulesets/Character.h"

#include "common/log.h"

#include <Atlas/Objects/Anonymous.h>
#include <Atlas/Objects/SmartPtr.h>
#include <Atlas/Objects/RootOperation.h>

using Atlas::Message::Element;
using Atlas::Message::MapType;
using Atlas::Message::ListType;
using Atlas::Objects::Entity::Anonymous;

PropertyCoverage::PropertyCoverage(PropertyBase * pb) :
    m_prop(pb),
    m_tlve(new Entity("0", 0)),
    m_wrld(new TestWorld(*m_tlve)),
    m_ent(new Entity("1", 1))
{
    m_ent->m_location.m_loc = m_tlve;
    m_ent->m_location.m_pos = Point3D(1,0,0);

    m_tlve->m_contains = new LocatedEntitySet;
    m_tlve->m_contains->insert(m_ent);

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
    m_ent->m_location.m_loc = 0;
    delete m_ent;
    delete m_tlve;
    delete m_prop;
    delete m_wrld;
}

void PropertyCoverage::interfaceCoverage()
{
}

void PropertyCoverage::basicCoverage()
{
    Element val;

    // assert(pb->flags() == 4);

    m_prop->get(val);

    // FIXME cover all types inc. map
    ListType::const_iterator I = m_testData.begin();
    ListType::const_iterator Iend = m_testData.end();
    for (; I != Iend; ++I) {
        m_prop->set(*I);
    }

    m_prop->get(val);

    m_prop->install(m_ent, "test_prop");

    I = m_testData.begin();
    for (; I != Iend; ++I) {
        m_prop->set(*I);
        m_prop->apply(m_ent);
        m_ent->propertyApplied("test_prop", *m_prop);
    }

    MapType map;
    m_prop->add("test_name", map);
    Anonymous ent;
    m_prop->add("test_name", ent);

    interfaceCoverage();
}

Character * PropertyCoverage::createCharacterEntity()
{
    m_ent->m_location.m_loc = 0;
    delete m_ent;
    m_tlve->m_contains->clear();

    Character * chr = new Character("2", 2);
    m_ent = chr;
    m_ent->m_location.m_loc = m_tlve;
    m_ent->m_location.m_pos = Point3D(1,0,0);
    m_tlve->m_contains->insert(m_ent);

    return chr;
}

void PropertyCoverage::testDataAppend(const Element & o)
{
    m_testData.push_back(o);
}

// stubs

#include "stubs/rulesets/stubCharacter.h"
#include "stubs/rulesets/stubThing.h"


#define STUB_Entity_destroy
void Entity::destroy()
{
    destroyed.emit();
}

#define STUB_Entity_getProperty
const PropertyBase * Entity::getProperty(const std::string & name) const
{
    PropertyDict::const_iterator I = m_properties.find(name);
    if (I != m_properties.end()) {
        return I->second;
    }
    return 0;
}

#include "stubs/rulesets/stubEntity.h"



#include "stubs/rulesets/stubLocatedEntity.h"
#include "stubs/common/stubRouter.h"
#include "stubs/common/stubBaseWorld.h"
#include "stubs/rulesets/stubLocation.h"

void log(LogLevel lvl, const std::string & msg)
{
}

