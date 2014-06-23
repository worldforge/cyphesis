#ifdef NDEBUG
#undef NDEBUG
#endif
#ifndef DEBUG
#define DEBUG
#endif
#include "rulesets/entityfilter/Filter.h"
#include "rulesets/EntityProperty.h"
#include "rulesets/Domain.h"
#include "rulesets/AtlasProperties.h"
#include "common/Property.h"
#include "common/BaseWorld.h"

#include "rulesets/Entity.h"
#include "common/TypeNode.h"

#include <Atlas/Objects/Anonymous.h>

#include <cassert>

using Atlas::Message::Element;
using Atlas::Message::MapType;
using Atlas::Objects::Entity::Anonymous;

//\brief a tester function for entity filter. Accepts a query and lists of entities that
// are supposed to pass or fail the test for a given query
void FilterTester(const std::string& query,
                  std::initializer_list<Entity*> entitiesToPass,
                  std::initializer_list<Entity*> entitiesToFail)
{
    EntityFilter::Filter f(query);
    for (auto iter = entitiesToPass.begin(); iter != entitiesToPass.end();
            ++iter) {
        assert(f.search(**iter));
    }
    for (auto iter = entitiesToFail.begin(); iter != entitiesToFail.end();
            ++iter) {
        assert(!f.search(**iter));
    }
}
int main()
{
    using namespace EntityFilter;
    //Set up testing environment
    Entity b1("1", 1);
    TypeNode* barrelType = new TypeNode("barrel");
    b1.setType(barrelType);
    b1.setProperty("mass", new SoftProperty(Element(30)));
    b1.setProperty("burn_speed", new SoftProperty(Element(0.3)));

    Entity b2("2", 2);
    b2.setProperty("mass", new SoftProperty(Element(20)));
    b2.setProperty("burn_speed", new SoftProperty(0.25));
    b2.setType(barrelType);

    Entity b3("3", 3);
    b3.setProperty("mass", new SoftProperty(Element(25)));
    b3.setProperty("burn_speed", new SoftProperty(Element(0.25)));
    b3.setType(barrelType);

    TypeNode* boulderType = new TypeNode("boulder");
    Entity bl1("4", 3);
    bl1.setProperty("mass", new SoftProperty(Element(25)));
    bl1.setType(boulderType);

// TESTS
    FilterTester("entity.type=barrel", { &b1 }, { &bl1 });

    // test entity.attribute case with various operators
    FilterTester("entity.burn_speed=0.3", { &b1 }, { &b2 });

    FilterTester("entity.burn_speed>0.3", { }, { &b1, &bl1 });

    FilterTester("entity.burn_speed<0.3", { &b2 }, { &b1 });

    //test query with several criteria

    FilterTester("entity.type=barrel&entity.burn_speed=0.3", { &b1 }, { &b2,
                         &bl1 });

    //test logical operators and precedence

    FilterTester("entity.type=barrel|entity.type=boulder", { &b1, &bl1 }, { });
    FilterTester("entity.type=boulder|entity.type=barrel&entity.burn_speed=0.3",
                 { &b1, &bl1 }, { });

//    Clean up
    delete barrelType;
    delete boulderType;
}

//LocatedEntity::LocatedEntity(const std::string & id, long intId) :
//               Router(id, intId),
//               m_refCount(0), m_seq(0),
//               m_script(0), m_type(0), m_flags(0), m_contains(0)
//{
//}
//
//LocatedEntity::~LocatedEntity()
//{
//}
//
//bool LocatedEntity::hasAttr(const std::string & name) const
//{
//    return false;
//}
//
//int LocatedEntity::getAttr(const std::string & name,
//                           Atlas::Message::Element & attr) const
//{
//    return -1;
//}
//
//int LocatedEntity::getAttrType(const std::string & name,
//                               Atlas::Message::Element & attr,
//                               int type) const
//{
//    return -1;
//}

//PropertyBase * LocatedEntity::setAttr(const std::string & name,
//                                      const Atlas::Message::Element & attr)
//{
//    return 0;
//}
//
//const PropertyBase * LocatedEntity::getProperty(const std::string & name) const
//{
//    return 0;
//}

//
//PropertyBase * LocatedEntity::modProperty(const std::string & name)
//{
//    return 0;
//}
//
//PropertyBase * LocatedEntity::setProperty(const std::string & name,
//                                          PropertyBase * prop)
//{
//    return 0;
//}
//
//void LocatedEntity::installDelegate(int, const std::string &)
//{
//}
//
//void LocatedEntity::destroy()
//{
//}
//Domain * Domain::m_instance = new Domain();

Domain * Domain::m_instance = new Domain();

Domain::Domain() :
        m_refCount(0)
{
}

Domain::~Domain()
{
}

float Domain::constrainHeight(LocatedEntity * parent,
                              const Point3D & pos,
                              const std::string & mode)
{
    return 0.f;
}

void Domain::tick(double t)
{
}
//Domain * LocatedEntity::getMovementDomain()
//{
//    return 0;
//}
//
//void LocatedEntity::sendWorld(const Operation & op)
//{
//}
//
//void LocatedEntity::onContainered(const LocatedEntity*)
//{
//}
//
//void LocatedEntity::onUpdated()
//{
//}
//
//void LocatedEntity::makeContainer()
//{
//    if (m_contains == 0) {
//        m_contains = new LocatedEntitySet;
//    }
//}
//
//void LocatedEntity::merge(const MapType & ent)
//{
//}
IdProperty::IdProperty(const std::string & data) :
        PropertyBase(per_ephem), m_data(data)
{
}

int IdProperty::get(Atlas::Message::Element & e) const
{
    e = m_data;
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

namespace Atlas
{
namespace Objects
{
namespace Operation
{
int ACTUATE_NO = -1;
int ATTACK_NO = -1;
int EAT_NO = -1;
int NOURISH_NO = -1;
int SETUP_NO = -1;
int TICK_NO = -1;
int UPDATE_NO = -1;
int RELAY_NO = -1;
}
}
}
Router::Router(const std::string & id, long intId) :
        m_id(id), m_intId(intId)
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
BaseWorld*BaseWorld::m_instance = 0;
BaseWorld::BaseWorld(LocatedEntity & gw) :
        m_gameWorld(gw)
{
    m_instance = this;
}

BaseWorld::~BaseWorld()
{
    m_instance = 0;
}

LocatedEntity * BaseWorld::getEntity(const std::string & id) const
{
    return 0;
}

void Location::addToMessage(MapType & omap) const
{
}

Location::Location() :
        m_loc(0)
{
}

void Location::addToEntity(const Atlas::Objects::Entity::RootEntity & ent) const
{
}

void log(LogLevel lvl, const std::string & msg)
{
}

