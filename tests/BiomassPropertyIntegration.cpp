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
#include "rules/simulation/BiomassProperty.h"

#include "common/operations/Eat.h"
#include "common/OperationRouter.h"
#include "common/PropertyFactory_impl.h"

#include <Atlas/Objects/Operation.h>

using Atlas::Message::MapType;
using Atlas::Objects::Operation::Eat;

class BiomassPropertyintegration : public Cyphesis::TestBase
{
  private:
    Ref<Entity> m_world;
    Ref<Entity> m_entity;
    PropertyBase * m_property;
  public:
    BiomassPropertyintegration();

    void setup();
    void teardown();

    void test_handler();
};

BiomassPropertyintegration::BiomassPropertyintegration()
{
    ADD_TEST(BiomassPropertyintegration::test_handler);
}

void BiomassPropertyintegration::setup()
{
    m_world = new Entity("0", 0);

    m_entity = new Entity("1", 1);
    m_entity->m_location.m_parent = m_world;

    PropertyFactory<BiomassProperty> decays_property_factory;

    m_property = decays_property_factory.newProperty();
    m_property->install(m_entity.get(), "biomass");
    m_entity->setProperty("biomass", m_property);
}

void BiomassPropertyintegration::teardown()
{
    m_entity = nullptr;
}

void BiomassPropertyintegration::test_handler()
{
    Eat e;

    OpVector res;
    m_entity->operation(e, res);

    ASSERT_EQUAL(res.size(), 2u);
    const Operation & reply = res.front();
    ASSERT_EQUAL(reply->getClassNo(), Atlas::Objects::Operation::SET_NO);
    ASSERT_EQUAL(reply->getTo(), m_entity->getId());
}

int main()
{
    BiomassPropertyintegration t;

    return t.run();
}

// stubs

#include "rules/AtlasProperties.h"
#include "rules/Domain.h"
#include "rules/Script.h"

#include "rules/simulation/BaseWorld.h"
#include "common/id.h"
#include "common/PropertyManager.h"

#include "physics/Vector3D.h"

#include "rules/simulation/DomainProperty.h"
#include "stubs/common/stubcustom.h"
#include "stubs/rules/stubDomain.h"
#include "stubs/rules/simulation/stubDomainProperty.h"
#include "stubs/common/stubVariable.h"
#include "stubs/common/stubMonitors.h"
#include "stubs/common/stubPropertyManager.h"


void addToEntity(const Point3D & p, std::vector<double> & vd)
{
    vd.resize(3);
    vd[0] = p[0];
    vd[1] = p[1];
    vd[2] = p[2];
}


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

#include "stubs/rules/simulation/stubBaseWorld.h"
#include "stubs/rules/stubScript.h"
#include "stubs/rules/stubLocation.h"
#include "stubs/common/stubRouter.h"
#include "stubs/common/stubLink.h"

IdProperty::IdProperty(const std::string & data) : PropertyBase(per_ephem),
                                                   m_data(data)
{
}

int IdProperty::get(Atlas::Message::Element & e) const
{
    return 0;
}

void IdProperty::set(const Atlas::Message::Element & e)
{
}

void IdProperty::add(const std::string & key,
                     Atlas::Message::MapType & ent) const
{
}

void IdProperty::add(const std::string & key,
                     const Atlas::Objects::Entity::RootEntity & ent) const
{
}

IdProperty * IdProperty::copy() const
{
    return 0;
}

ContainsProperty::ContainsProperty(LocatedEntitySet & data) :
      PropertyBase(per_ephem), m_data(data)
{
}

int ContainsProperty::get(Atlas::Message::Element & e) const
{
    return 0;
}

void ContainsProperty::set(const Atlas::Message::Element & e)
{
}

void ContainsProperty::add(const std::string & s,
                           const Atlas::Objects::Entity::RootEntity & ent) const
{
}

ContainsProperty * ContainsProperty::copy() const
{
    return 0;
}

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
