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

#include "rulesets/ArithmeticScript.h"
#include "rulesets/Entity.h"
#include "rulesets/StatisticsProperty.h"

#include "common/BaseWorld.h"
#include "common/TypeNode.h"

class TestArithmeticScript : public ArithmeticScript
{
  public:
    virtual ~TestArithmeticScript() { }

    virtual int attribute(const std::string & name, float & val)
    {
        return 0;
    }

    virtual void set(const std::string & name, const float & val)
    {
    }
};

/// Test implementation of the BaseWorld interface, which produces dummy
/// Arithmetic scripts.
class ArithmeticTestWorld : public BaseWorld {
  public:
    explicit ArithmeticTestWorld(LocatedEntity & gw) : BaseWorld(gw) {
    }

    virtual bool idle(const SystemTime &) { return false; }
    virtual LocatedEntity * addEntity(LocatedEntity * ent) { 
        m_eobjects[ent->getIntId()] = ent;
        return 0;
    }
    virtual LocatedEntity * addNewEntity(const std::string &,
                                         const Atlas::Objects::Entity::RootEntity &) { return 0; }
    int createSpawnPoint(const Atlas::Message::MapType & data,
                         LocatedEntity *) { return 0; }
    int getSpawnList(Atlas::Message::ListType & data) { return 0; }
    LocatedEntity * spawnNewEntity(const std::string & name,
                                   const std::string & type,
                                   const Atlas::Objects::Entity::RootEntity & desc) {
        return addNewEntity(type, desc);
    }
    virtual Task * newTask(const std::string &, LocatedEntity &) { return 0; }
    virtual Task * activateTask(const std::string &, const std::string &,
                                LocatedEntity *, LocatedEntity &) { return 0; }
    virtual ArithmeticScript * newArithmetic(const std::string &,
                                             LocatedEntity *) {
        return new TestArithmeticScript;
    }
    virtual void message(const Operation & op, LocatedEntity & ent) { }
    virtual LocatedEntity * findByName(const std::string & name) { return 0; }
    virtual LocatedEntity * findByType(const std::string & type) { return 0; }
    virtual void addPerceptive(LocatedEntity *) { }
};

// Check what happens when two instance of a type both instantiate
// this property when there is a script. The underlying instance of
// ArithmeticScript 
class StatisicsPropertyintegration : public Cyphesis::TestBase
{
  private:
    TypeNode * m_char_type;
    PropertyBase * m_char_property;
    Entity * m_char1;
    Entity * m_char2;
  public:
    StatisicsPropertyintegration();

    void setup();
    void teardown();

    void test_copy();
};

StatisicsPropertyintegration::StatisicsPropertyintegration()
{
    new ArithmeticTestWorld(*(LocatedEntity*)0);

    ADD_TEST(StatisicsPropertyintegration::test_copy);
}

void StatisicsPropertyintegration::setup()
{
    m_char_type = new TypeNode("char_type");

    m_char_property = new StatisticsProperty;
    m_char_property->setFlags(flag_class);
    m_char_type->addProperty("char_prop", m_char_property);

    m_char1 = new Entity("1", 1);
    m_char1->setType(m_char_type);
    m_char_property->install(m_char1, "char_prop");
    m_char_property->apply(m_char1);

    m_char2 = new Entity("2", 2);
    m_char2->setType(m_char_type);
    m_char_property->install(m_char2, "char_prop");
    m_char_property->apply(m_char2);
}

void StatisicsPropertyintegration::teardown()
{
    delete m_char1;
    delete m_char2;
    delete m_char_type;
}

void StatisicsPropertyintegration::test_copy()
{
    StatisticsProperty * pb =
          m_char1->modPropertyClass<StatisticsProperty>("char_type");

    ASSERT_NOT_EQUAL(pb, m_char_property);
}

int main()
{
    StatisicsPropertyintegration t;

    return t.run();
}

// stubs

#include "Property_stub_impl.h"

#include "rulesets/AtlasProperties.h"
#include "rulesets/Domain.h"
#include "rulesets/Script.h"

#include "common/log.h"
#include "common/PropertyManager.h"

namespace Atlas { namespace Objects { namespace Operation {
int ACTUATE_NO = -1;
int ATTACK_NO = -1;
int EAT_NO = -1;
int NOURISH_NO = -1;
int SETUP_NO = -1;
int TICK_NO = -1;
int UPDATE_NO = -1;
} } }

Domain * Domain::m_instance = new Domain();

Domain::Domain() : m_refCount(0)
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

BaseWorld * BaseWorld::m_instance = 0;

BaseWorld::BaseWorld(LocatedEntity & gw) : m_gameWorld(gw)
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

LocatedEntity * BaseWorld::getEntity(long id) const
{
    return 0;
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

void Location::addToMessage(Atlas::Message::MapType & omap) const
{
}

Location::Location() : m_loc(0)
{
}

void Location::addToEntity(const Atlas::Objects::Entity::RootEntity & ent) const
{
}

TypeNode::TypeNode(const std::string & name) : m_name(name), m_parent(0)
{
}

TypeNode::~TypeNode()
{
    PropertyDict::const_iterator I = m_defaults.begin();
    PropertyDict::const_iterator Iend = m_defaults.end();
    for (; I != Iend; ++I) {
        delete I->second;
    }
}

void TypeNode::addProperty(const std::string & name,
                           PropertyBase * p)
{
    m_defaults[name] = p;
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

PropertyBase::PropertyBase(unsigned int flags) : m_flags(flags)
{
}

PropertyBase::~PropertyBase()
{
}

void PropertyBase::install(LocatedEntity *, const std::string & name)
{
}

void PropertyBase::apply(LocatedEntity *)
{
}

void PropertyBase::add(const std::string & s,
                       Atlas::Message::MapType & ent) const
{
    get(ent[s]);
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

template<>
void Property<int>::set(const Atlas::Message::Element & e)
{
    if (e.isInt()) {
        this->m_data = e.asInt();
    }
}

template<>
void Property<double>::set(const Atlas::Message::Element & e)
{
    if (e.isNum()) {
        this->m_data = e.asNum();
    }
}

template<>
void Property<std::string>::set(const Atlas::Message::Element & e)
{
    if (e.isString()) {
        this->m_data = e.String();
    }
}

template class Property<int>;
template class Property<double>;
template class Property<std::string>;

SoftProperty::SoftProperty()
{
}

SoftProperty::SoftProperty(const Atlas::Message::Element & data) :
              PropertyBase(0), m_data(data)
{
}

int SoftProperty::get(Atlas::Message::Element & val) const
{
    val = m_data;
    return 0;
}

void SoftProperty::set(const Atlas::Message::Element & val)
{
}

SoftProperty * SoftProperty::copy() const
{
    return new SoftProperty(*this);
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

ArithmeticScript::~ArithmeticScript()
{
}

PropertyManager * PropertyManager::m_instance = 0;

PropertyManager::PropertyManager()
{
    assert(m_instance == 0);
    m_instance = this;
}

PropertyManager::~PropertyManager()
{
   m_instance = 0;
}

int PropertyManager::installFactory(const std::string & type_name,
                                    const Atlas::Objects::Root & type_desc,
                                    PropertyKit * factory)
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
