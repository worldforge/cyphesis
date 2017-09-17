// Copyright (C) 2016 Piotr Stępnicki
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
#include "rulesets/MetabolizingProperty.h"

#include "common/Tick.h"
#include "common/OperationRouter.h"
#include "common/PropertyFactory_impl.h"


#include <Atlas/Objects/Operation.h>


/* Needed mocks and stubs for TestWorld to work */
#include "common/BaseWorld.h"

int timeoffset = 0 ;
long integerId(const std::string & id)
{
    long intId = strtol(id.c_str(), 0, 10);
    if (intId == 0 && id != "0") {
        intId = -1L;
    }

    return intId;
}

class TestWorld : public BaseWorld {
  public:
    explicit TestWorld(LocatedEntity & gw) : BaseWorld(gw) {
        m_eobjects[m_gameWorld.getIntId()] = &m_gameWorld;
    }

    virtual bool idle() { return false; }
    virtual LocatedEntity * addEntity(LocatedEntity * ent) { 
        m_eobjects[ent->getIntId()] = ent;
        return 0;
    }
    virtual LocatedEntity * addNewEntity(const std::string &,
                                  const Atlas::Objects::Entity::RootEntity &) {
        return 0;
    }
    void delEntity(LocatedEntity * obj) {}
    int createSpawnPoint(const Atlas::Message::MapType & data,
                         LocatedEntity *) { return 0; }
    int removeSpawnPoint(LocatedEntity *) {return 0; }
    int getSpawnList(Atlas::Message::ListType & data) { return 0; }
    LocatedEntity * spawnNewEntity(const std::string & name,
                            const std::string & type,
                            const Atlas::Objects::Entity::RootEntity & desc) {
        return addNewEntity(type, desc);
    }
    virtual int moveToSpawn(const std::string & name,
                            Location& location){return 0;}
    virtual Task * newTask(const std::string &, LocatedEntity &) { return 0; }
    virtual Task * activateTask(const std::string &, const std::string &,
                                LocatedEntity *, LocatedEntity &) { return 0; }
    virtual ArithmeticScript * newArithmetic(const std::string &, LocatedEntity *) {
        return 0;
    }
    virtual void message(const Atlas::Objects::Operation::RootOperation & op,
                         LocatedEntity & ent) { }
    virtual LocatedEntity * findByName(const std::string & name) { return 0; }
    virtual LocatedEntity * findByType(const std::string & type) { return 0; }
    virtual void addPerceptive(LocatedEntity *) { }
};


using Atlas::Message::MapType;
using Atlas::Objects::Operation::Tick;

class MetabolizingPropertyintegration : public Cyphesis::TestBase
{
  private:
    Entity * m_world;
    Entity * m_entity;
    //TestWorld b_world(*m_world);
    PropertyBase * m_property;
  public:
    MetabolizingPropertyintegration();

    void setup();
    void teardown();

    void test_handler();
};

MetabolizingPropertyintegration::MetabolizingPropertyintegration()
{
    ADD_TEST(MetabolizingPropertyintegration::test_handler);
}

void MetabolizingPropertyintegration::setup()
{
    m_world = new Entity("0", 0);

    m_entity = new Entity("1", 1);
    m_entity->m_location.m_loc = m_world;

    TestWorld *tw = new TestWorld(*m_world);
    PropertyFactory<MetabolizingProperty> metabolizing_property_factory;

    m_property = metabolizing_property_factory.newProperty();
    m_property->install(m_entity, "metabolizing");
    m_entity->setProperty("metabolizing", m_property);
}


void MetabolizingPropertyintegration::teardown()
{
    delete m_entity;
}

void MetabolizingPropertyintegration::test_handler()
{
    Tick e;

    OpVector res;
    m_entity->operation(e, res);

    ASSERT_EQUAL(res.size(), 0u);
    //const Operation & reply = res.front();
    //ASSERT_EQUAL(reply->getClassNo(), Atlas::Objects::Operation::SET_NO);
    //ASSERT_EQUAL(reply->getTo(), m_entity->getId());
}

int main()
{
    MetabolizingPropertyintegration t;

    return t.run();
}

// stubs

#include "rulesets/AtlasProperties.h"
#include "rulesets/Domain.h"
#include "rulesets/Script.h"

#include "rulesets/AtlasProperties.h"
#include "rulesets/StatusProperty.h"
#include "rulesets/AreaProperty.h"
#include "rulesets/BBoxProperty.h"


#include "common/id.h"
#include "common/PropertyManager.h"

#include "physics/Vector3D.h"

#include "rulesets/DomainProperty.h"
#include "stubs/common/stubCustom.h"
#include "stubs/rulesets/stubDomain.h"
#include "stubs/rulesets/stubDomainProperty.h"
#include "stubs/common/stubVariable.h"
#include "stubs/common/stubMonitors.h"
#include "stubs/rulesets/stubTransformsProperty.h"

#include "stubs/rulesets/stubStatusProperty.h"
#include "stubs/rulesets/stubAreaProperty.h"
#include "stubs/rulesets/stubBBoxProperty.h"

void addToEntity(const Point3D & p, std::vector<double> & vd)
{
    vd.resize(3);
    vd[0] = p[0];
    vd[1] = p[1];
    vd[2] = p[2];
}

Router::Router(const std::string & id, long intId) : m_id(id),
                                                             m_intId(intId)
{
}

Router::~Router()
{
}

void Router::addToMessage(Atlas::Message::MapType & omap) const
{
}

void Router::addToEntity(const Atlas::Objects::Entity::RootEntity & ent) const
{
}

Script::Script()
{
}

/// \brief Script destructor
Script::~Script()
{
}

bool Script::operation(const std::string & opname,
                       const Atlas::Objects::Operation::RootOperation & op,
                       OpVector & res)
{
   return false;
}

void Script::hook(const std::string & function, LocatedEntity * entity)
{
}

void Location::addToMessage(MapType & omap) const
{
}

Location::Location() : m_loc(0)
{
}

void Location::addToEntity(const Atlas::Objects::Entity::RootEntity & ent) const
{
}

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

PropertyKit::~PropertyKit()
{
}

PropertyManager * PropertyManager::m_instance = 0;


void log(LogLevel lvl, const std::string & msg)
{
}





