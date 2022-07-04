// Cyphesis Online RPG Server and AI Engine
// Copyright (C) 2009 Alistair Riddoch
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

#include "server/EntityFactory.h"

#include "rules/simulation/World.h"

#include "common/ScriptKit.h"
#include "common/TypeNode.h"

#include <Atlas/Objects/Entity.h>
#include <cassert>
#include "common/debug.h"
#include "../TestPropertyManager.h"

class TestScriptFactory : public ScriptKit<LocatedEntity>
{
    protected:
        std::string m_package;
    public:
        const std::string& package() const override
        {
            return m_package;
        }

        int addScript(LocatedEntity& entity) const override
        {
            return 0;
        }

        int refreshClass() override
        {
            return 0;
        }
};

struct EntityFactorytest : public Cyphesis::TestBase
{
    EntityFactoryBase* m_ek;

    EntityFactorytest()
    {
        ADD_TEST(EntityFactorytest::test_newEntity);
        ADD_TEST(EntityFactorytest::test_updateProperties);
        ADD_TEST(EntityFactorytest::test_updateProperties_child);
        ADD_TEST(EntityFactorytest::test_classAttributes);
    }

    void setup() override
    {
        m_ek = new EntityFactory<Thing>;
        m_ek->m_type = new TypeNode("foo");
    }

    void teardown() override
    {
        delete m_ek->m_type;
        delete m_ek;
    }

    void test_classAttributes()
    {
        {
            //Test with no existing value and only a default value
            Atlas::Message::Element existing = Atlas::Message::Element();

            ClassAttribute tested{"a string"};

            tested.combine(existing);
            ASSERT_EQUAL(Atlas::Message::Element("a string"), existing)

            existing = Atlas::Message::Element();
            tested.defaultValue = 1;
            tested.combine(existing);
            ASSERT_EQUAL(Atlas::Message::Element(1), existing)

            existing = Atlas::Message::Element();
            tested.defaultValue = 1.0;
            tested.combine(existing);
            ASSERT_EQUAL(Atlas::Message::Element(1.0), existing)

            existing = Atlas::Message::Element();
            tested.defaultValue = Atlas::Message::MapType{{"foo", "bar"}};
            tested.combine(existing);
            ASSERT_EQUAL(Atlas::Message::Element(Atlas::Message::MapType{{"foo", "bar"}}), existing)

            existing = Atlas::Message::Element();
            tested.defaultValue = Atlas::Message::ListType{"foo"};
            tested.combine(existing);
            ASSERT_EQUAL(Atlas::Message::Element(Atlas::Message::ListType{"foo"}), existing)

        }

        {
            //Test with an existing value and only a default value. The value should be replaced
            Atlas::Message::Element existing = "another string";

            ClassAttribute tested{"a string"};

            tested.combine(existing);
            ASSERT_EQUAL(Atlas::Message::Element("a string"), existing)

            existing = 2;
            tested.defaultValue = 1;
            tested.combine(existing);
            ASSERT_EQUAL(Atlas::Message::Element(1), existing)

            existing = 2.0;
            tested.defaultValue = 1.0;
            tested.combine(existing);
            ASSERT_EQUAL(Atlas::Message::Element(1.0), existing)

            existing = Atlas::Message::MapType{{"foz", "baz"}};
            tested.defaultValue = Atlas::Message::MapType{{"foo", "bar"}};
            tested.combine(existing);
            ASSERT_EQUAL(Atlas::Message::Element(Atlas::Message::MapType{{"foo", "bar"}}), existing)

            existing = Atlas::Message::ListType{"bar"};
            tested.defaultValue = Atlas::Message::ListType{"foo"};
            tested.combine(existing);
            ASSERT_EQUAL(Atlas::Message::Element(Atlas::Message::ListType{"foo"}), existing)

        }

        {
            //Test with incompatible values
            Atlas::Message::Element existing = 1;

            ClassAttribute tested{"a string"};

            tested.combine(existing);
            ASSERT_EQUAL(Atlas::Message::Element("a string"), existing)

            existing = "a string";
            tested.defaultValue = 1;
            tested.combine(existing);
            ASSERT_EQUAL(Atlas::Message::Element(1), existing)

            existing = "a string";
            tested.defaultValue = 1.0;
            tested.combine(existing);
            ASSERT_EQUAL(Atlas::Message::Element(1.0), existing)

            existing = "a string";
            tested.defaultValue = Atlas::Message::MapType{{"foo", "bar"}};
            tested.combine(existing);
            ASSERT_EQUAL(Atlas::Message::Element(Atlas::Message::MapType{{"foo", "bar"}}), existing)

            existing = "a string";
            tested.defaultValue = Atlas::Message::ListType{"foo"};
            tested.combine(existing);
            ASSERT_EQUAL(Atlas::Message::Element(Atlas::Message::ListType{"foo"}), existing)
        }

        {
            //Test with append
            Atlas::Message::Element existing = "a string";

            ClassAttribute tested{Atlas::Message::Element(), "another string"};

            tested.combine(existing);
            ASSERT_EQUAL(Atlas::Message::Element("a stringanother string"), existing)

            existing = 1;
            tested.append = 1;
            tested.combine(existing);
            ASSERT_EQUAL(Atlas::Message::Element(2), existing)

            existing = 1.0;
            tested.append = 1.0;
            tested.combine(existing);
            ASSERT_EQUAL(Atlas::Message::Element(2.0), existing)

            existing = Atlas::Message::MapType{{"foz", "baz"}};
            tested.append = Atlas::Message::MapType{{"foo", "bar"}};
            tested.combine(existing);
            ASSERT_EQUAL(Atlas::Message::Element(Atlas::Message::MapType{{"foz", "baz"},
                                                                         {"foo", "bar"}}), existing)

            existing = Atlas::Message::ListType{"foo"};
            tested.append = Atlas::Message::ListType{"bar"};
            tested.combine(existing);
            ASSERT_EQUAL(Atlas::Message::Element(Atlas::Message::ListType{"foo", "bar"}), existing)
        }

        {
            //Test with prepend
            Atlas::Message::Element existing = "a string";

            ClassAttribute tested{Atlas::Message::Element(), Atlas::Message::Element(), "another string"};

            tested.combine(existing);
            ASSERT_EQUAL(Atlas::Message::Element("another stringa string"), existing)

            existing = 1;
            tested.prepend = 1;
            tested.combine(existing);
            ASSERT_EQUAL(Atlas::Message::Element(2), existing)

            existing = 1.0;
            tested.prepend = 1.0;
            tested.combine(existing);
            ASSERT_EQUAL(Atlas::Message::Element(2.0), existing)

            existing = Atlas::Message::MapType{{"foz", "baz"}};
            tested.prepend = Atlas::Message::MapType{{"foo", "bar"}};
            tested.combine(existing);
            ASSERT_EQUAL(Atlas::Message::Element(Atlas::Message::MapType{{"foz", "baz"},
                                                                         {"foo", "bar"}}), existing)

            existing = Atlas::Message::ListType{"foo"};
            tested.prepend = Atlas::Message::ListType{"bar"};
            tested.combine(existing);
            ASSERT_EQUAL(Atlas::Message::Element(Atlas::Message::ListType{"bar", "foo"}), existing)
        }

        {
            //Test with subtract
            Atlas::Message::Element existing = "a string";

            ClassAttribute tested{Atlas::Message::Element(), Atlas::Message::Element(), Atlas::Message::Element(), "a string"};

            tested.combine(existing);
            //subtract of string does nothing
            ASSERT_EQUAL(Atlas::Message::Element("a string"), existing)

            existing = 2;
            tested.subtract = 1;
            tested.combine(existing);
            ASSERT_EQUAL(Atlas::Message::Element(1), existing)

            existing = 2.0;
            tested.subtract = 1.0;
            tested.combine(existing);
            ASSERT_EQUAL(Atlas::Message::Element(1.0), existing)

            existing = Atlas::Message::MapType{{"foz", "baz"},
                                               {"foo", "bar"}};
            tested.subtract = Atlas::Message::MapType{{"foo", ""}};
            tested.combine(existing);
            ASSERT_EQUAL(Atlas::Message::Element(Atlas::Message::MapType{{"foz", "baz"}}), existing)

            existing = Atlas::Message::ListType{"foo", "bar", "bar"};
            tested.subtract = Atlas::Message::ListType{"bar"};
            tested.combine(existing);
            ASSERT_EQUAL(Atlas::Message::Element(Atlas::Message::ListType{"foo"}), existing)
        }

        {
            //Test with multiple effects
            Atlas::Message::Element existing;

            ClassAttribute tested{"default", "append", "prepend", "subtract"};

            tested.combine(existing);
            //subtract of string does nothing
            ASSERT_EQUAL(Atlas::Message::Element("prependdefaultappend"), existing)

            tested.defaultValue = 1;
            tested.append = 2;
            tested.prepend = 4;
            tested.subtract = 8;
            tested.combine(existing);
            ASSERT_EQUAL(Atlas::Message::Element(-1), existing)

            tested.defaultValue = 1.0;
            tested.append = 2.0;
            tested.prepend = 4.0;
            tested.subtract = 8.0;
            tested.combine(existing);
            ASSERT_EQUAL(Atlas::Message::Element(-1.0), existing)

            tested.defaultValue = Atlas::Message::MapType{{"foo", "bar"}};
            tested.append = Atlas::Message::MapType{{"foo1", "bar1"}};
            tested.prepend = Atlas::Message::MapType{{"foo2", "bar2"}};
            tested.subtract = Atlas::Message::MapType{{"foo", "bar"}};
            tested.combine(existing);
            ASSERT_EQUAL(Atlas::Message::Element(Atlas::Message::MapType{{"foo1", "bar1"}, {"foo2", "bar2"}}), existing)

            tested.defaultValue = Atlas::Message::ListType{"foo"};
            tested.append = Atlas::Message::ListType{"bar"};
            tested.prepend = Atlas::Message::ListType{"baz"};
            tested.subtract = Atlas::Message::ListType{"foo"};
            tested.combine(existing);
            ASSERT_EQUAL(Atlas::Message::Element(Atlas::Message::ListType{"baz", "bar"}), existing)
        }
    }


    void test_newEntity()
    {
        auto e = m_ek->newEntity(1, Atlas::Objects::Entity::RootEntity());

        ASSERT_TRUE(e)
    }

    void test_updateProperties()
    {
        TestPropertyManager propertyManager;
        std::map<const TypeNode*, TypeNode::PropertiesUpdate> changes;
        m_ek->updateProperties(changes, propertyManager);
    }

    void test_updateProperties_child()
    {
        TestPropertyManager propertyManager{};
        EntityFactory<Thing> ekc{};
        ekc.m_type = m_ek->m_type;
        ekc.m_classAttributes.emplace("foo", ClassAttribute{"value"});

        m_ek->m_children.insert(&ekc);

        std::map<const TypeNode*, TypeNode::PropertiesUpdate> changes;
        m_ek->updateProperties(changes, propertyManager);

        assert(ekc.m_attributes.find("foo") != ekc.m_attributes.end());
    }
};


int main()
{
    EntityFactorytest t;

    return t.run();
}

// stubs

#include "../stubs/rules/simulation/stubThing.h"
#include "../stubs/rules/simulation/stubEntity.h"
#include "../stubs/rules/stubLocatedEntity.h"
#include "../stubs/common/stubRouter.h"
#include "../stubs/common/stubTypeNode.h"
#include "../stubs/common/stubProperty.h"
#include "common/Property_impl.h"
#include "../stubs/rules/stubLocation.h"
#include "../stubs/common/stublog.h"
#include "../stubs/common/stubProperty.h"
#include "../stubs/common/stubPropertyManager.h"

