// Cyphesis Online RPG Server and AI Engine
// Copyright (C) 2013 Alistair Riddoch
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

#include "TestBase.h"

#include "rules/simulation/Entity.h"
#include "rules/TerrainModProperty.h"
#include "rules/TerrainProperty.h"

#include "common/OperationRouter.h"
#include "common/PropertyFactory_impl.h"
#include "rules/BaseWorld.h"

#include "stubs/rulesets/stubLocation.h"

#include <Atlas/Objects/Operation.h>

using Atlas::Message::MapType;
using Atlas::Objects::Operation::Delete;
using Atlas::Objects::Operation::Move;

#include "TestWorld.h"

class TerrainModPropertyintegration : public Cyphesis::TestBase
{
  private:
    Ref<Entity> m_rootEntity;
    Ref<Entity> m_entity;
    PropertyBase * m_property;
    PropertyBase * m_terrainProperty;
    std::unique_ptr<TestWorld> m_world;
  public:
    TerrainModPropertyintegration();

    void setup();
    void teardown();

    void test_move_handler();
    void test_delete_handler();
};

TerrainModPropertyintegration::TerrainModPropertyintegration()
{
    ADD_TEST(TerrainModPropertyintegration::test_move_handler);
    ADD_TEST(TerrainModPropertyintegration::test_delete_handler);
}

void TerrainModPropertyintegration::setup()
{
    m_rootEntity = new Entity("0", 0);

    m_world.reset();
    m_world.reset(new TestWorld(m_rootEntity));

    m_entity = new Entity("1", 1);
    m_entity->m_location.m_pos = Point3D(5.f, 5.f, 5.f);
    m_entity->m_location.m_parent = m_rootEntity;
    ASSERT_TRUE(m_entity->m_location.isValid());

    PropertyFactory<TerrainModProperty> terrainmod_property_factory;

    m_terrainProperty = new TerrainProperty;
    m_terrainProperty->install(m_rootEntity.get(), "terrain");
    m_rootEntity->setProperty("terrain", m_terrainProperty);
    m_terrainProperty->apply(m_rootEntity.get());
    m_rootEntity->propertyApplied("terrain", *m_terrainProperty);

    m_property = terrainmod_property_factory.newProperty();
    m_property->install(m_entity.get(), "terrainmod");
    m_entity->setProperty("terrainmod", m_property);
    m_property->apply(m_entity.get());
    m_entity->propertyApplied("terrainmod", *m_property);
}

void TerrainModPropertyintegration::teardown()
{

}

void TerrainModPropertyintegration::test_move_handler()
{
    Move m;
    // FIXME Move needs some args, and also probably requires the position to
    // to be set up on the entity first

    OpVector res;
    m_entity->operation(m, res);

    // FIXME Check what gives
}

void TerrainModPropertyintegration::test_delete_handler()
{
    Delete d;

    OpVector res;
    m_entity->operation(d, res);

    // FIXME Check what gives
}

int main()
{
    TerrainModPropertyintegration t;

    return t.run();
}

// stubs

#include "rules/AtlasProperties.h"
#include "rules/Domain.h"
#include "rules/Script.h"

#include "rules/BaseWorld.h"
#include "common/id.h"
#include "common/log.h"
#include "common/PropertyManager.h"

#include "physics/Vector3D.h"

#include "rules/DomainProperty.h"
#include "stubs/common/stubcustom.h"
#include "stubs/rulesets/stubDomain.h"
#include "stubs/rulesets/stubDomainProperty.h"
#include "stubs/common/stubVariable.h"
#include "stubs/common/stubMonitors.h"

void addToEntity(const Point3D & p, std::vector<double> & vd)
{
    vd.resize(3);
    vd[0] = p[0];
    vd[1] = p[1];
    vd[2] = p[2];
}
#include "stubs/common/stubRouter.h"


#ifndef STUB_BaseWorld_getEntity
#define STUB_BaseWorld_getEntity
Ref<LocatedEntity> BaseWorld::getEntity(const std::string & id) const
{
    return getEntity(integerId(id));
}

Ref<LocatedEntity> BaseWorld::getEntity(long id) const
{
    auto I = m_eobjects.find(id);
    if (I != m_eobjects.end()) {
        assert(I->second);
        return I->second;
    } else {
        return nullptr;
    }
}
#endif //STUB_BaseWorld_getEntity

#include "stubs/common/stubLink.h"
#include "stubs/rulesets/stubBaseWorld.h"
#include "stubs/rulesets/stubScript.h"
#include "stubs/rulesets/stubAtlasProperties.h"
#include "stubs/common/stubPropertyManager.h"

long integerId(const std::string & id)
{
    long intId = strtol(id.c_str(), 0, 10);
    if (intId == 0 && id != "0") {
        intId = -1L;
    }

    return intId;
}

void log(LogLevel lvl, const std::string & msg)
{
}
