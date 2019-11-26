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

#include "rules/simulation/ArithmeticScript.h"
#include "rules/simulation/Entity.h"
#include "rules/simulation/StatisticsProperty.h"

#include "rules/simulation/BaseWorld.h"
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

#include "TestWorld.h"
/// Test implementation of the BaseWorld interface, which produces dummy
/// Arithmetic scripts.
class ArithmeticTestWorld : public TestWorld {
  public:
    explicit ArithmeticTestWorld() {
    }

    void addEntity(const Ref<LocatedEntity>& ent) override {
        m_eobjects[ent->getIntId()] = ent;
    }
    std::unique_ptr<ArithmeticScript> newArithmetic(const std::string &,
                                             LocatedEntity *) override {
        return std::make_unique<TestArithmeticScript>();
    }
};

// Check what happens when two instance of a type both instantiate
// this property when there is a script. The underlying instance of
// ArithmeticScript 
class StatisicsPropertyintegration : public Cyphesis::TestBase
{
  private:
    TypeNode * m_char_type;
    PropertyBase * m_char_property;
    Ref<Entity>  m_char1;
    Ref<Entity>  m_char2;
  public:
    StatisicsPropertyintegration();

    void setup();
    void teardown();

    void test_copy();
};

StatisicsPropertyintegration::StatisicsPropertyintegration()
{
    new ArithmeticTestWorld();

    ADD_TEST(StatisicsPropertyintegration::test_copy);
}

void StatisicsPropertyintegration::setup()
{
    m_char_type = new TypeNode("char_type");

    m_char_property = new StatisticsProperty;
    m_char_property->addFlags(flag_class);
    m_char_type->injectProperty("char_prop", std::unique_ptr<PropertyBase>(m_char_property));

    m_char1 = new Entity("1", 1);
    m_char1->setType(m_char_type);
    m_char_property->install(m_char1.get(), "char_prop");
    m_char_property->apply(m_char1.get());
    m_char1->propertyApplied("char_prop", *m_char_property);

    m_char2 = new Entity("2", 2);
    m_char2->setType(m_char_type);
    m_char_property->install(m_char2.get(), "char_prop");
    m_char_property->apply(m_char2.get());
    m_char2->propertyApplied("char_prop", *m_char_property);
}

void StatisicsPropertyintegration::teardown()
{
    m_char1 = nullptr;
    m_char2 = nullptr;
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

#include "stubs/common/stubProperty.h"

#include "rules/AtlasProperties.h"
#include "rules/Domain.h"
#include "rules/Script.h"

#include "common/log.h"
#include "common/PropertyManager.h"

#include "rules/simulation/DomainProperty.h"
#include "stubs/common/stubcustom.h"
#include "stubs/rules/stubDomain.h"
#include "stubs/rules/simulation/stubDomainProperty.h"
#include "stubs/common/stubVariable.h"
#include "stubs/common/stubMonitors.h"

#define STUB_TypeNode_TypeNode
TypeNode::TypeNode(std::string name) : m_name(name), m_parent(0)
{
}

#define STUB_TypeNode_TypeNode_DTOR
TypeNode::~TypeNode()
{
    m_defaults.clear();
}

#define STUB_TypeNode_injectProperty
TypeNode::PropertiesUpdate TypeNode::injectProperty(const std::string& name,
                                                    std::unique_ptr<PropertyBase> p)
{
    m_defaults[name] = std::move(p);
    return {};
}
#include "stubs/common/stubTypeNode.h"


void addToEntity(const Point3D & p, std::vector<double> & vd)
{
    vd.resize(3);
    vd[0] = p[0];
    vd[1] = p[1];
    vd[2] = p[2];
}

#include "stubs/rules/simulation/stubBaseWorld.h"

#include "stubs/rules/stubScript.h"
#include "stubs/rules/stubLocation.h"
#include "stubs/rules/stubAtlasProperties.h"
#include "stubs/common/stubPropertyManager.h"
#include "stubs/common/stubLink.h"
#include "stubs/common/stubRouter.h"

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
