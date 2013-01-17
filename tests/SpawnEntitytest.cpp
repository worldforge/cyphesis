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

#ifdef NDEBUG
#undef NDEBUG
#endif
#ifndef DEBUG
#define DEBUG
#endif

#include "TestBase.h"

#include "server/SpawnEntity.h"

#include "rulesets/AreaProperty.h"

#include "physics/Vector3D.h"

#include <Atlas/Message/Element.h>

#include <wfmath/point.h>

#include <cassert>

class SpawnEntitytest : public Cyphesis::TestBase
{
  private:
    SpawnEntity * m_spawn;
  public:
    SpawnEntitytest();

    void setup();
    void teardown();

    void test_constructor();
    void test_setup_string();
    void test_setup_list();
    void test_setup_contains_string();
    void test_setup_contains_list();
};

SpawnEntitytest::SpawnEntitytest()
{
    ADD_TEST(SpawnEntitytest::test_constructor);
    ADD_TEST(SpawnEntitytest::test_setup_string);
    ADD_TEST(SpawnEntitytest::test_setup_list);
    ADD_TEST(SpawnEntitytest::test_setup_contains_string);
    ADD_TEST(SpawnEntitytest::test_setup_contains_list);
}

void SpawnEntitytest::setup()
{
    m_spawn = new SpawnEntity(0);
}

void SpawnEntitytest::teardown()
{
    delete m_spawn;
}

void SpawnEntitytest::test_constructor()
{
    Spawn * s = new SpawnEntity(0);

    delete s;
}

void SpawnEntitytest::test_setup_string()
{
    Atlas::Message::MapType spawn_data;
    spawn_data["character_types"] = "bob";
    m_spawn->setup(spawn_data);
}

void SpawnEntitytest::test_setup_list()
{
    Atlas::Message::MapType spawn_data;
    spawn_data["character_types"] = Atlas::Message::ListType(1, "bob");
    m_spawn->setup(spawn_data);
}

void SpawnEntitytest::test_setup_contains_string()
{
    Atlas::Message::MapType spawn_data;
    spawn_data["contains"] = "bob";
    m_spawn->setup(spawn_data);
}

void SpawnEntitytest::test_setup_contains_list()
{
    Atlas::Message::MapType spawn_data;
    spawn_data["contains"] = Atlas::Message::ListType(1, "bob");
    m_spawn->setup(spawn_data);
}

int main()
{
    SpawnEntitytest t;

    return t.run();
}

// Stubs

#include "Property_stub_impl.h"

Spawn::~Spawn()
{
}

void addToEntity(const Point3D & p, std::vector<double> & vd)
{
    vd.resize(3);
    vd[0] = p[0];
    vd[1] = p[1];
    vd[2] = p[2];
}

EntityRef::EntityRef(LocatedEntity* e) : m_inner(e)
{
}

EntityRef::EntityRef(const EntityRef& ref) : m_inner(ref.m_inner)
{
}

EntityRef& EntityRef::operator=(const EntityRef& ref)
{
    m_inner = ref.m_inner;

    return *this;
}

void EntityRef::onEntityDeleted()
{
}

AreaProperty::AreaProperty()
{
}

AreaProperty::~AreaProperty()
{
}

void AreaProperty::set(const Atlas::Message::Element & ent)
{
}

AreaProperty * AreaProperty::copy() const
{
    return 0;
}

void AreaProperty::apply(LocatedEntity * owner)
{
}

template class Property<Atlas::Message::MapType>;

PropertyBase::PropertyBase(unsigned int flags) : m_flags(flags)
{
}

PropertyBase::~PropertyBase()
{
}

void PropertyBase::install(LocatedEntity *)
{
}

void PropertyBase::apply(LocatedEntity *)
{
}

void PropertyBase::add(const std::string & s,
                       Atlas::Message::MapType & ent) const
{
}

void PropertyBase::add(const std::string & s,
                       const Atlas::Objects::Entity::RootEntity & ent) const
{
}

HandlerResult PropertyBase::operation(LocatedEntity *,
                                      const Operation &,
                                      OpVector &)
{
    return OPERATION_IGNORED;
}
