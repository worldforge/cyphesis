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

#include "TestBase.h"

#include "rulesets/Entity.h"
#include "rulesets/TerrainEffectorProperty.h"
#include "rulesets/TerrainProperty.h"

#include "stubs/rulesets/stubTerrainProperty.h"

class TerrainEffectorPropertytest : public Cyphesis::TestBase
{
  private:
    TerrainEffectorProperty * m_property;
    Entity * m_entity;
    Entity * m_world;
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
    m_world = new Entity("0", 0);
    m_entity = new Entity("1", 1);
    m_entity->m_location.m_loc = m_world;
}

void TerrainEffectorPropertytest::teardown()
{
    delete m_property;
    delete m_entity;
    delete m_world;
}

void TerrainEffectorPropertytest::test_none_found()
{
    ASSERT_NULL(m_world->m_location.m_loc);
    ASSERT_EQUAL(m_entity->m_location.m_loc, m_world);

    const TerrainProperty * res = m_property->getTerrain(m_entity);

    ASSERT_NULL(res);
}

void TerrainEffectorPropertytest::test_not_terrain()
{
    ASSERT_NULL(m_world->m_location.m_loc);
    ASSERT_EQUAL(m_entity->m_location.m_loc, m_world);

    m_world->setProperty("terrain",
                         new Property<Atlas::Message::MapType>);

    const TerrainProperty * res = m_property->getTerrain(m_entity);

    ASSERT_NULL(res);
}

void TerrainEffectorPropertytest::test_found()
{
    ASSERT_NULL(m_world->m_location.m_loc);
    ASSERT_EQUAL(m_entity->m_location.m_loc, m_world);

    m_world->setProperty("terrain",
                         new TerrainProperty);

    const TerrainProperty * res = m_property->getTerrain(m_entity);

    ASSERT_NOT_NULL(res);
}

int main()
{
    TerrainEffectorPropertytest t;

    return t.run();
}

// stubs

//#include "Property_stub_impl.h"

Entity::Entity(const std::string & id, long intId) :
        LocatedEntity(id, intId)
{
}

Entity::~Entity()
{
}

void Entity::destroy()
{
    destroyed.emit();
}

void Entity::ActuateOperation(const Operation &, OpVector &)
{
}

void Entity::AppearanceOperation(const Operation &, OpVector &)
{
}

void Entity::AttackOperation(const Operation &, OpVector &)
{
}

void Entity::CombineOperation(const Operation &, OpVector &)
{
}

void Entity::CreateOperation(const Operation &, OpVector &)
{
}

void Entity::DeleteOperation(const Operation &, OpVector &)
{
}

void Entity::DisappearanceOperation(const Operation &, OpVector &)
{
}

void Entity::DivideOperation(const Operation &, OpVector &)
{
}

void Entity::EatOperation(const Operation &, OpVector &)
{
}

void Entity::GetOperation(const Operation &, OpVector &)
{
}

void Entity::InfoOperation(const Operation &, OpVector &)
{
}

void Entity::ImaginaryOperation(const Operation &, OpVector &)
{
}

void Entity::LookOperation(const Operation &, OpVector &)
{
}

void Entity::MoveOperation(const Operation &, OpVector &)
{
}

void Entity::NourishOperation(const Operation &, OpVector &)
{
}

void Entity::SetOperation(const Operation &, OpVector &)
{
}

void Entity::SightOperation(const Operation &, OpVector &)
{
}

void Entity::SoundOperation(const Operation &, OpVector &)
{
}

void Entity::TalkOperation(const Operation &, OpVector &)
{
}

void Entity::TickOperation(const Operation &, OpVector &)
{
}

void Entity::TouchOperation(const Operation &, OpVector &)
{
}

void Entity::UpdateOperation(const Operation &, OpVector &)
{
}

void Entity::UseOperation(const Operation &, OpVector &)
{
}

void Entity::WieldOperation(const Operation &, OpVector &)
{
}

void Entity::RelayOperation(const Operation &, OpVector &)
{
}

void Entity::externalOperation(const Operation & op, Link &)
{
}

void Entity::operation(const Operation & op, OpVector & res)
{
}

void Entity::addToMessage(Atlas::Message::MapType & omap) const
{
}

void Entity::addToEntity(const Atlas::Objects::Entity::RootEntity & ent) const
{
}

PropertyBase * Entity::setAttr(const std::string & name,
                               const Atlas::Message::Element & attr)
{
    return 0;
}

const PropertyBase * Entity::getProperty(const std::string & name) const
{
    PropertyDict::const_iterator I = m_properties.find(name);
    if (I != m_properties.end()) {
        return I->second;
    }
    return 0;
}

PropertyBase * Entity::modProperty(const std::string & name)
{
    return 0;
}

void Entity::installDelegate(int class_no, const std::string & delegate)
{
}

void Entity::removeDelegate(int class_no, const std::string & delegate)
{
}

Domain * Entity::getDomain()
{
    return 0;
}

const Domain * Entity::getDomain() const
{
    return 0;
}

void Entity::setType(const TypeNode* t)
{

}

void Entity::sendWorld(const Operation & op)
{
}

PropertyBase * Entity::setProperty(const std::string & name,
                                   PropertyBase * prop)
{
    return m_properties[name] = prop;
}

void Entity::onContainered(const LocatedEntity*)
{
}

void Entity::onUpdated()
{
}

#include "stubs/rulesets/stubLocatedEntity.h"
#include "stubs/common/stubRouter.h"
#include "stubs/modules/stubLocation.h"
#include "stubs/common/stubProperty.h"
