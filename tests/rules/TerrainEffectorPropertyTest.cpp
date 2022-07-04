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


#ifdef NDEBUG
#undef NDEBUG
#endif
#ifndef DEBUG
#define DEBUG
#endif

#include "../TestBase.h"

#include "rules/simulation/Entity.h"
#include "rules/simulation/TerrainEffectorProperty.h"
#include "rules/simulation/TerrainProperty.h"

#include "../stubs/rules/simulation/stubTerrainProperty.h"

class TerrainEffectorPropertytest : public Cyphesis::TestBase
{
  private:
    TerrainEffectorProperty * m_property;
    Ref<Entity> m_entity;
    Ref<Entity> m_world;
  public:
    TerrainEffectorPropertytest();

    void setup();
    void teardown();

    void test_none_found();
    void test_not_terrain();
    void test_found();
};

TerrainEffectorPropertytest::TerrainEffectorPropertytest() : m_property(0)
{
    ADD_TEST(TerrainEffectorPropertytest::test_none_found);
    ADD_TEST(TerrainEffectorPropertytest::test_not_terrain);
    ADD_TEST(TerrainEffectorPropertytest::test_found);
}

void TerrainEffectorPropertytest::setup()
{
    m_property = new TerrainEffectorProperty;
    m_world = new Entity(0);
    m_entity = new Entity(1);
    m_entity->m_parent = m_world.get();
}

void TerrainEffectorPropertytest::teardown()
{
    delete m_property;
}

void TerrainEffectorPropertytest::test_none_found()
{
    ASSERT_FALSE(m_world->m_parent);
    ASSERT_EQUAL(m_entity->m_parent, m_world.get());

    const TerrainProperty * res = m_property->getTerrain(*m_entity);

    ASSERT_NULL(res);
}

void TerrainEffectorPropertytest::test_not_terrain()
{
    ASSERT_FALSE(m_world->m_parent);
    ASSERT_EQUAL(m_entity->m_parent, m_world.get());

    m_world->setProperty("terrain",
                         std::unique_ptr<PropertyBase>(new Property<Atlas::Message::MapType>));

    const TerrainProperty * res = m_property->getTerrain(*m_entity);

    ASSERT_NULL(res);
}

void TerrainEffectorPropertytest::test_found()
{
    ASSERT_FALSE(m_world->m_parent);
    ASSERT_EQUAL(m_entity->m_parent, m_world.get());

    m_world->setProperty("terrain",
                         std::unique_ptr<PropertyBase>(new TerrainProperty));

    const TerrainProperty * res = m_property->getTerrain(*m_entity);

    ASSERT_NOT_NULL(res);
}

int main()
{
    TerrainEffectorPropertytest t;

    return t.run();
}

// stubs


#include "../stubs/rules/simulation/stubEntity.h"

#include "../stubs/rules/stubLocatedEntity.h"
#include "../stubs/common/stubRouter.h"
#include "../stubs/rules/stubLocation.h"
#include "../stubs/common/stubProperty.h"
