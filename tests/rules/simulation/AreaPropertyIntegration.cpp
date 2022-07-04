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

#include "../../TestBase.h"

#include "rules/simulation/AreaProperty.h"
#include "rules/simulation/Entity.h"

#include "common/TypeNode.h"

using Atlas::Message::ListType;
using Atlas::Message::MapType;

// Check what happens when two instance of a type both instantiate
// this property when there is a script. The underlying instance of
// Shape needs to be copied 
class AreaPropertyintegration : public Cyphesis::TestBase
{
  private:
    TypeNode * m_char_type;
    PropertyBase * m_char_property;
    Ref<Entity>  m_char1;
    Ref<Entity>  m_char2;
  public:
    AreaPropertyintegration();

    void setup();
    void teardown();

    void test_copy();
};

AreaPropertyintegration::AreaPropertyintegration()
{
    ADD_TEST(AreaPropertyintegration::test_copy);
}

void AreaPropertyintegration::setup()
{
    m_char_type = new TypeNode("char_type");

    m_char_property = new AreaProperty;
    m_char_property->addFlags(prop_flag_class);
    m_char_property->set(
        MapType{
            std::make_pair("shape", MapType{
                std::make_pair("type", "polygon"),
                std::make_pair("points", ListType(3, ListType(2, 1.f)))
            })
        }
    );
    m_char_type->injectProperty("char_type", std::unique_ptr<PropertyBase>(m_char_property));

    m_char1 = new Entity(1);
    m_char1->setType(m_char_type);
    m_char_property->install(*m_char1, "char_prop");
    m_char_property->apply(*m_char1);
    m_char1->propertyApplied("char_prop", *m_char_property);

    m_char2 = new Entity(2);
    m_char2->setType(m_char_type);
    m_char_property->install(*m_char2, "char_prop");
    m_char_property->apply(*m_char2);
    m_char2->propertyApplied("char_prop", *m_char_property);
}

void AreaPropertyintegration::teardown()
{
    m_char1 = nullptr;
    m_char2 = nullptr;
    delete m_char_type;
}

void AreaPropertyintegration::test_copy()
{
    AreaProperty * pb =
          m_char1->modPropertyClass<AreaProperty>("char_type");

    ASSERT_NOT_EQUAL(pb, m_char_property);
}

int main()
{
    AreaPropertyintegration t;

    return t.run();
}

// stubs

#include "common/log.h"

const TerrainProperty * TerrainEffectorProperty::getTerrain(LocatedEntity& owner, LocatedEntity**)
{
    return 0;
}

// stubs

#include "rules/AtlasProperties.h"
#include "rules/Domain.h"
#include "rules/Script.h"

#include "rules/simulation/BaseWorld.h"
#include "common/log.h"
#include "common/PropertyManager.h"
#include "common/TypeNode.h"

#include "rules/simulation/DomainProperty.h"
#include "../../stubs/common/stubcustom.h"
#include "../../stubs/rules/stubDomain.h"
#include "../../stubs/rules/simulation/stubDomainProperty.h"
#include "../../stubs/common/stubVariable.h"
#include "../../stubs/common/stubMonitors.h"
#include "../../stubs/common/stubProperty.h"
#include "../../stubs/common/stubLink.h"
#include "../../stubs/common/stubRouter.h"

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

#include "../../stubs/common/stubTypeNode.h"


void addToEntity(const Point3D & p, std::vector<double> & vd)
{
}

#include "../../stubs/rules/simulation/stubBaseWorld.h"

#include "../../stubs/rules/stubScript.h"
#include "../../stubs/rules/stubLocation.h"
#include "../../stubs/rules/stubAtlasProperties.h"
#include "../../stubs/rules/stubPhysicalProperties.h"
#include "../../stubs/common/stubPropertyManager.h"
#include "../../stubs/common/stubid.h"
#include "../../stubs/common/stublog.h"
#include "../../stubs/rules/stubModifier.h"
