// Cyphesis Online RPG Server and AI Engine
// Copyright (C) 2012 Alistair Riddoch
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

#include "rulesets/TerrainEffectorProperty.h"

class TerrainEffectorPropertytest : public Cyphesis::TestBase
{
  private:
    TerrainEffectorProperty * m_property;
  public:
    TerrainEffectorPropertytest();

    void setup();
    void teardown();

    void test_null();
};

TerrainEffectorPropertytest::TerrainEffectorPropertytest() : m_property(0)
{
    ADD_TEST(TerrainEffectorPropertytest::test_null);
}

void TerrainEffectorPropertytest::setup()
{
    m_property = new TerrainEffectorProperty;
}

void TerrainEffectorPropertytest::teardown()
{
    delete m_property;
}

void TerrainEffectorPropertytest::test_null()
{
    // const TerrainProperty * res = m_property->getTerrain(0);
}

int main()
{
    TerrainEffectorPropertytest t;

    t.run();

    return 0;
}

// stubs

#include "rulesets/TerrainProperty.h"

#include "Property_stub_impl.h"

TerrainProperty::~TerrainProperty()
{
}

int TerrainProperty::get(Atlas::Message::Element & ent) const
{
    return 0;
}

void TerrainProperty::set(const Atlas::Message::Element & ent)
{
}

PropertyBase::PropertyBase(unsigned int flags) : m_flags(flags)
{
}

PropertyBase::~PropertyBase()
{
}

void PropertyBase::install(Entity *)
{
}

void PropertyBase::apply(Entity *)
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

template class Property<Atlas::Message::MapType>;
