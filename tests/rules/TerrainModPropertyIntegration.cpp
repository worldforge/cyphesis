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

#include "../TestBase.h"

#include "rules/simulation/Entity.h"
#include "rules/simulation/TerrainModProperty.h"
#include "rules/simulation/TerrainProperty.h"

#include "common/OperationRouter.h"
#include "common/PropertyFactory_impl.h"
#include "rules/simulation/BaseWorld.h"

#include "../stubs/rules/stubLocation.h"
#include "../stubs/rules/stubAtlasProperties.h"

#include <Atlas/Objects/Operation.h>
#include "rules/PhysicalProperties.h"

using Atlas::Message::MapType;
using Atlas::Objects::Operation::Delete;
using Atlas::Objects::Operation::Move;

#include "../TestWorld.h"

class TerrainModPropertyintegration : public Cyphesis::TestBase
{
  private:
    Ref<Entity> m_rootEntity;
    Ref<Entity> m_entity;
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
    m_rootEntity = new Entity(0);

    m_world.reset();
    m_world.reset(new TestWorld(m_rootEntity));

    m_entity = new Entity(1);
    m_entity->requirePropertyClassFixed<PositionProperty>().data() = Point3D(5.f, 5.f, 5.f);
    m_entity->m_parent = m_rootEntity.get();

    PropertyFactory<TerrainModProperty> terrainmod_property_factory;

    auto terrainProperty = new TerrainProperty;
    terrainProperty->install(*m_rootEntity, "terrain");
    auto tProp = m_rootEntity->setProperty("terrain", std::unique_ptr<PropertyBase>(terrainProperty));
    tProp->apply(*m_rootEntity);
    m_rootEntity->propertyApplied("terrain", *tProp);

    auto terrainModProp = terrainmod_property_factory.newProperty();
    terrainModProp->install(*m_entity, "terrainmod");
    auto prop = m_entity->setProperty("terrainmod", std::move(terrainModProp));
    prop->apply(*m_entity);
    m_entity->propertyApplied("terrainmod", *prop);
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

#include "rules/simulation/BaseWorld.h"
#include "common/id.h"
#include "common/log.h"
#include "common/PropertyManager.h"

#include "physics/Vector3D.h"

#include "rules/simulation/DomainProperty.h"
#include "../stubs/common/stubcustom.h"
#include "../stubs/rules/stubDomain.h"
#include "../stubs/rules/simulation/stubDomainProperty.h"
#include "../stubs/common/stubVariable.h"
#include "../stubs/common/stubMonitors.h"

void addToEntity(const Point3D & p, std::vector<double> & vd)
{
    vd.resize(3);
    vd[0] = p[0];
    vd[1] = p[1];
    vd[2] = p[2];
}
#include "../stubs/common/stubRouter.h"


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

#include "../stubs/common/stubLink.h"
#include "../stubs/rules/simulation/stubBaseWorld.h"
#include "../stubs/rules/stubScript.h"
#include "../stubs/rules/stubAtlasProperties.h"
#include "../stubs/common/stubPropertyManager.h"
#include "../stubs/common/stubid.h"
#include "../stubs/common/stublog.h"
#include "../stubs/rules/stubModifier.h"
